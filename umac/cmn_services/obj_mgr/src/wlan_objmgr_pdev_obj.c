/*
 * Copyright (c) 2016 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
 /**
  * DOC: Public APIs to perform operations on Global objects
  */
#include <wlan_objmgr_cmn.h>
#include <wlan_objmgr_global_obj.h>
#include <wlan_objmgr_global_obj_i.h>
#include <wlan_objmgr_psoc_obj.h>
#include <wlan_objmgr_pdev_obj.h>
#include <wlan_objmgr_vdev_obj.h>
#include <wlan_objmgr_peer_obj.h>
#include <wlan_objmgr_psoc_obj_i.h>
#include <wlan_objmgr_pdev_obj_i.h>
#include <qdf_mem.h>


/**
 ** APIs to Create/Delete Global object APIs
 */
static QDF_STATUS wlan_objmgr_pdev_object_status(
		struct wlan_objmgr_pdev *pdev)
{
	uint8_t id;
	QDF_STATUS status = QDF_STATUS_SUCCESS;

	wlan_pdev_obj_lock(pdev);
	/* Iterate through all components to derive the object status */
	for (id = 0; id < WLAN_UMAC_MAX_COMPONENTS; id++) {
		/* If component disabled, Ignore */
		if (pdev->obj_status[id] == QDF_STATUS_COMP_DISABLED) {
			continue;
		/* If component operates in Async, status is Partially created,
			break */
		} else if (pdev->obj_status[id] == QDF_STATUS_COMP_ASYNC) {
			if (pdev->pdev_comp_priv_obj[id] == NULL) {
				status = QDF_STATUS_COMP_ASYNC;
				break;
			}
		/* If component failed to allocate its object, treat it as
			failure, complete object need to be cleaned up */
		} else if ((pdev->obj_status[id] == QDF_STATUS_E_NOMEM) ||
			(pdev->obj_status[id] == QDF_STATUS_E_FAILURE)) {
			status = QDF_STATUS_E_FAILURE;
			break;
		}
	}
	wlan_pdev_obj_unlock(pdev);
	return status;
}

struct wlan_objmgr_pdev *wlan_objmgr_pdev_obj_create(
			struct wlan_objmgr_psoc *psoc, void *osdev_priv)
{
	struct wlan_objmgr_pdev *pdev;
	uint8_t id;
	wlan_objmgr_pdev_create_handler handler;
	wlan_objmgr_pdev_status_handler s_handler;
	void *arg;
	QDF_STATUS obj_status;

	if (psoc == NULL) {
		qdf_print("%s:psoc is NULL\n", __func__);
		return NULL;
	}
	/* Allocate PDEV object's memory */
	pdev = qdf_mem_malloc(sizeof(*pdev));
	if (pdev == NULL) {
		qdf_print("%s:pdev alloc failed\n", __func__);
		return NULL;
	}
	/* Attach PDEV with PSOC */
	if (wlan_objmgr_psoc_pdev_attach(psoc, pdev)
				!= QDF_STATUS_SUCCESS) {
		qdf_print("%s:pdev psoc attach failed\n", __func__);
		qdf_mem_free(pdev);
		return NULL;
	}
	/* Save PSOC object pointer in PDEV */
	wlan_pdev_set_psoc(pdev, psoc);
	/* Initialize PDEV spinlock */
	qdf_spinlock_create(&pdev->pdev_lock);
	/* Initialize PDEV's VDEV list, assign default values */
	qdf_list_create(&pdev->pdev_objmgr.wlan_vdev_list,
			WLAN_UMAC_PDEV_MAX_VDEVS);
	pdev->pdev_objmgr.wlan_vdev_count = 0;
	pdev->pdev_objmgr.max_vdev_count = WLAN_UMAC_PDEV_MAX_VDEVS;
	/* Save HDD/OSIF pointer */
	pdev->pdev_nif.pdev_ospriv = osdev_priv;

	/* Invoke registered create handlers */
	for (id = 0; id < WLAN_UMAC_MAX_COMPONENTS; id++) {
		handler = g_umac_glb_obj->pdev_create_handler[id];
		arg = g_umac_glb_obj->pdev_create_handler_arg[id];
		if (handler != NULL)
			pdev->obj_status[id] = handler(pdev, arg);
		else
			pdev->obj_status[id] = QDF_STATUS_COMP_DISABLED;
	}
	/* Derive object status */
	obj_status = wlan_objmgr_pdev_object_status(pdev);

	if (obj_status == QDF_STATUS_SUCCESS) {
		/* Object status is SUCCESS, Object is created */
		pdev->obj_state = WLAN_OBJ_STATE_CREATED;
		/* Invoke component registered status handlers */
		for (id = 0; id < WLAN_UMAC_MAX_COMPONENTS; id++) {
			s_handler = g_umac_glb_obj->pdev_status_handler[id];
			arg = g_umac_glb_obj->pdev_status_handler_arg[id];
			if (s_handler != NULL) {
				s_handler(pdev, arg,
					  QDF_STATUS_SUCCESS);
			}
		}
	/* Few components operates in Asynchrous communction, Object state
	partially created */
	} else if (obj_status == QDF_STATUS_COMP_ASYNC) {
		pdev->obj_state = WLAN_OBJ_STATE_PARTIALLY_CREATED;
	/* Component object failed to be created, clean up the object */
	} else if (obj_status == QDF_STATUS_E_FAILURE) {
		/* Clean up the psoc */
		qdf_print("%s: PDEV component objects allocation failed\n",
			  __func__);
		wlan_objmgr_pdev_obj_delete(pdev);
		return NULL;
	}
	return pdev;
}
EXPORT_SYMBOL(wlan_objmgr_pdev_obj_create);

QDF_STATUS wlan_objmgr_pdev_obj_delete(struct wlan_objmgr_pdev *pdev)
{
	uint8_t id;
	wlan_objmgr_pdev_delete_handler handler;
	QDF_STATUS obj_status;
	void *arg;

	if (pdev == NULL) {
		qdf_print("%s:pdev is NULL\n", __func__);
		return QDF_STATUS_E_FAILURE;
	}

	/* Invoke registered delete handlers */
	for (id = 0; id < WLAN_UMAC_MAX_COMPONENTS; id++) {
		handler = g_umac_glb_obj->pdev_delete_handler[id];
		arg = g_umac_glb_obj->pdev_delete_handler_arg[id];
		if (handler != NULL)
			pdev->obj_status[id] = handler(pdev, arg);
		else
			pdev->obj_status[id] = QDF_STATUS_COMP_DISABLED;
	}
	/* Derive object status */
	obj_status = wlan_objmgr_pdev_object_status(pdev);

	if (obj_status == QDF_STATUS_E_FAILURE) {
		qdf_print("%s: PDEV component objects delete failed\n",
			  __func__);
		/* Ideally should not happen */
		/* This leads to memleak ??? how to handle */
		QDF_BUG(0);
		return QDF_STATUS_E_FAILURE;
	}
	/* Deletion is in progress */
	if (obj_status == QDF_STATUS_COMP_ASYNC) {
		pdev->obj_state = WLAN_OBJ_STATE_PARTIALLY_DELETED;
		return QDF_STATUS_COMP_ASYNC;
	} else {
		/* Detach PDEV from PSOC PDEV's list */
		if (wlan_objmgr_psoc_pdev_detach(pdev->pdev_objmgr.wlan_psoc,
						 pdev) ==
						 QDF_STATUS_E_FAILURE) {
			qdf_print("%s: PSOC PDEV detach failed\n",
				  __func__);
			return QDF_STATUS_E_FAILURE;
		}
		/* de-init lock */
		qdf_spinlock_destroy(&pdev->pdev_lock);
		/* Free the memory */
		qdf_mem_free(pdev);
	}
	return QDF_STATUS_SUCCESS;
}
EXPORT_SYMBOL(wlan_objmgr_pdev_obj_delete);

/**
 ** APIs to attach/detach component objects
 */
QDF_STATUS wlan_objmgr_pdev_component_obj_attach(
		struct wlan_objmgr_pdev *pdev,
		enum wlan_umac_comp_id id,
		void *comp_priv_obj,
		QDF_STATUS status)
{
	uint8_t i;
	wlan_objmgr_pdev_status_handler s_hlr;
	void *a;
	QDF_STATUS obj_status;

	/* component id is invalid */
	if (id >= WLAN_UMAC_MAX_COMPONENTS) {
		qdf_print("%s: component-id %d is not supported\n",
			  __func__, id);
		return QDF_STATUS_MAXCOMP_FAIL;
	}
	wlan_pdev_obj_lock(pdev);
	/* If there is a valid entry, return failure */
	if (pdev->pdev_comp_priv_obj[id] != NULL) {
		qdf_print("%s: component-%d already have valid pointer\n",
			  __func__, id);
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	/* Save component's pointer and status */
	pdev->pdev_comp_priv_obj[id] = comp_priv_obj;
	pdev->obj_status[id] = status;
	wlan_pdev_obj_unlock(pdev);

	if (pdev->obj_state != WLAN_OBJ_STATE_PARTIALLY_CREATED)
		return QDF_STATUS_SUCCESS;
	/* If PDEV object status is partially created means, this API is
	invoked with differnt context, this block should be executed for async
	components only */
	/* Derive status */
	obj_status = wlan_objmgr_pdev_object_status(pdev);
	/* STATUS_SUCCESS means, object is CREATED */
	if (obj_status == QDF_STATUS_SUCCESS)
		pdev->obj_state = WLAN_OBJ_STATE_CREATED;
	/* update state as CREATION failed, caller has to delete the
	PDEV object */
	else if (obj_status == QDF_STATUS_E_FAILURE)
		pdev->obj_state = WLAN_OBJ_STATE_CREATION_FAILED;
	/* Notify components about the CREATION success/failure */
	if ((obj_status == QDF_STATUS_SUCCESS) ||
	    (obj_status == QDF_STATUS_E_FAILURE)) {
		/* nofity object status */
		for (i = 0; i < WLAN_UMAC_MAX_COMPONENTS; i++) {
			s_hlr = g_umac_glb_obj->pdev_status_handler[i];
			a = g_umac_glb_obj->pdev_status_handler_arg[i];
			if (s_hlr != NULL)
				s_hlr(pdev, a, obj_status);
		}
	}
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS wlan_objmgr_pdev_component_obj_detach(
		struct wlan_objmgr_pdev *pdev,
		enum wlan_umac_comp_id id,
		void *comp_priv_obj)
{
	QDF_STATUS obj_status;

	/* component id is invalid */
	if (id >= WLAN_UMAC_MAX_COMPONENTS)
		return QDF_STATUS_MAXCOMP_FAIL;

	wlan_pdev_obj_lock(pdev);
	/* If there is a invalid entry, return failure */
	if (pdev->pdev_comp_priv_obj[id] != comp_priv_obj) {
		pdev->obj_status[id] = QDF_STATUS_E_FAILURE;
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	/* Reset pointers to NULL, update the status*/
	pdev->pdev_comp_priv_obj[id] = NULL;
	pdev->obj_status[id] = QDF_STATUS_SUCCESS;
	wlan_pdev_obj_unlock(pdev);

	/* If PDEV object status is partially deleted means, this API is
	invoked with differnt context, this block should be executed for async
	components only */
	if ((pdev->obj_state == WLAN_OBJ_STATE_PARTIALLY_DELETED) ||
	    (pdev->obj_state == WLAN_OBJ_STATE_COMP_DEL_PROGRESS)) {
		/* Derive object status */
		obj_status = wlan_objmgr_pdev_object_status(pdev);
		if (obj_status == QDF_STATUS_SUCCESS) {
			/*Update the status as Deleted, if full object
				deletion is in progress */
			if (pdev->obj_state ==
				WLAN_OBJ_STATE_PARTIALLY_DELETED)
				pdev->obj_state = WLAN_OBJ_STATE_DELETED;
			/* Move to creation state, since this component
			deletion alone requested */
			if (pdev->obj_state ==
				WLAN_OBJ_STATE_COMP_DEL_PROGRESS)
				pdev->obj_state = WLAN_OBJ_STATE_CREATED;
		/* Object status is failure */
		} else if (obj_status == QDF_STATUS_E_FAILURE) {
			/*Update the status as Deletion failed, if full object
				deletion is in progress */
			if (pdev->obj_state ==
					WLAN_OBJ_STATE_PARTIALLY_DELETED)
				pdev->obj_state =
					WLAN_OBJ_STATE_DELETION_FAILED;
			/* Move to creation state, since this component
			deletion alone requested (do not block other
			components)*/
			if (pdev->obj_state ==
					WLAN_OBJ_STATE_COMP_DEL_PROGRESS)
				pdev->obj_state = WLAN_OBJ_STATE_CREATED;
		}

		/* Delete pdev object */
		if ((obj_status == QDF_STATUS_SUCCESS) &&
		    (pdev->obj_state == WLAN_OBJ_STATE_DELETED)) {
			/* Detach pdev object from psoc */
			if (wlan_objmgr_psoc_pdev_detach(
				pdev->pdev_objmgr.wlan_psoc, pdev) ==
						QDF_STATUS_E_FAILURE)
				return QDF_STATUS_E_FAILURE;
			/* Destroy spinlock */
			qdf_spinlock_destroy(&pdev->pdev_lock);
			/* Free PDEV memory */
			qdf_mem_free(pdev);
		}
	}
	return QDF_STATUS_SUCCESS;
}

/**
 ** APIs to operations on pdev objects
 */
static void wlan_objmgr_pdev_vdev_iterate_peers(struct wlan_objmgr_pdev *pdev,
				struct wlan_objmgr_vdev *vdev,
				wlan_objmgr_pdev_op_handler handler,
				void *arg, uint8_t lock_free_op)
{
	qdf_list_t *peer_list = NULL;
	struct wlan_objmgr_peer *peer = NULL;
	struct wlan_objmgr_peer *peer_next = NULL;

	/* Iterating through vdev's peer list, so lock is
		needed */
	if (!lock_free_op)
		wlan_vdev_obj_lock(vdev);
	/* Get peer list of the vdev */
	peer_list = &vdev->vdev_objmgr.wlan_peer_list;
	if (peer_list != NULL) {
		peer = wlan_vdev_peer_list_peek_head(peer_list);
		while (peer != NULL) {
			/* Increment ref count, to hold the
				peer pointer */
			wlan_objmgr_peer_ref_peer(peer);
			/* Get next peer pointer */
			peer_next = wlan_peer_get_next_peer_of_vdev(peer_list,
								    peer);
			/* Invoke the handler */
			handler(pdev, (void *)peer, arg);
			/* Decrement ref count, this can lead
				to peer deletion also */
			wlan_objmgr_peer_unref_peer(peer);
			peer = peer_next;
		}
	}
	if (!lock_free_op)
		wlan_vdev_obj_unlock(vdev);
}

QDF_STATUS wlan_objmgr_pdev_iterate_obj_list(
		struct wlan_objmgr_pdev *pdev,
		enum wlan_objmgr_obj_type obj_type,
		wlan_objmgr_pdev_op_handler handler,
		void *arg, uint8_t lock_free_op)
{
	struct wlan_objmgr_pdev_objmgr *objmgr = &pdev->pdev_objmgr;
	qdf_list_t *vdev_list = NULL;
	struct wlan_objmgr_vdev *vdev = NULL;
	struct wlan_objmgr_vdev *vdev_next = NULL;

	/* If caller requests for lock free opeation, do not acquire
		handler will handle the synchronization*/
	if (!lock_free_op)
		wlan_pdev_obj_lock(pdev);
	/* VDEV list */
	vdev_list = &objmgr->wlan_vdev_list;
	switch (obj_type) {
	case WLAN_VDEV_OP:
		/* Iterate through all VDEV object, and invoke handler for each
			VDEV object */
		vdev = wlan_pdev_vdev_list_peek_head(vdev_list);
		while (vdev != NULL) {
				/* TODO increment ref count */
			/* Get next vdev (handler can be invoked for
				vdev deletion also */
			vdev_next = wlan_vdev_get_next_vdev_of_pdev(vdev_list,
						vdev);
			handler(pdev, (void *)vdev, arg);
			vdev = vdev_next;
				/* TODO decrement ref count */
		}
		break;
	case WLAN_PEER_OP:
		vdev = wlan_pdev_vdev_list_peek_head(vdev_list);
		while (vdev != NULL) {
			/* TODO increment ref count */
			wlan_objmgr_pdev_vdev_iterate_peers(pdev, vdev, handler,
							    arg, lock_free_op);
			/* Get Next VDEV */
			vdev_next = wlan_vdev_get_next_vdev_of_pdev(vdev_list,
						vdev);
			/* TODO decrement ref count */
		}
		break;
	default:
		break;
	}
	if (!lock_free_op)
		wlan_pdev_obj_unlock(pdev);

	return QDF_STATUS_SUCCESS;
}
EXPORT_SYMBOL(wlan_objmgr_pdev_iterate_obj_list);

QDF_STATUS wlan_objmgr_trigger_pdev_comp_priv_object_creation(
		struct wlan_objmgr_pdev *pdev,
		enum wlan_umac_comp_id id)
{
	wlan_objmgr_pdev_create_handler handler;
	void *arg;
	QDF_STATUS obj_status = QDF_STATUS_SUCCESS;

	/* Component id is invalid */
	if (id >= WLAN_UMAC_MAX_COMPONENTS)
		return QDF_STATUS_MAXCOMP_FAIL;

	wlan_pdev_obj_lock(pdev);
	/* If component object is already created, delete old
		component object, then invoke creation */
	if (pdev->pdev_comp_priv_obj[id] != NULL) {
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	wlan_pdev_obj_unlock(pdev);

	/* Invoke registered create handlers */
	handler = g_umac_glb_obj->pdev_create_handler[id];
	arg = g_umac_glb_obj->pdev_create_handler_arg[id];
	if (handler != NULL)
		pdev->obj_status[id] = handler(pdev, arg);
	else
		return QDF_STATUS_E_FAILURE;
	/* If object status is created, then only handle this object status */
	if (pdev->obj_state == WLAN_OBJ_STATE_CREATED) {
		/* Derive object status */
		obj_status = wlan_objmgr_pdev_object_status(pdev);
		/* Move PDEV object state to Partially created state */
		if (obj_status == QDF_STATUS_COMP_ASYNC) {
			/*TODO atomic */
			pdev->obj_state = WLAN_OBJ_STATE_PARTIALLY_CREATED;
		}
	}
	return obj_status;
}

QDF_STATUS wlan_objmgr_trigger_pdev_comp_priv_object_deletion(
		struct wlan_objmgr_pdev *pdev,
		enum wlan_umac_comp_id id)
{
	wlan_objmgr_pdev_delete_handler handler;
	void *arg;
	QDF_STATUS obj_status = QDF_STATUS_SUCCESS;

	/* component id is invalid */
	if (id >= WLAN_UMAC_MAX_COMPONENTS)
		return QDF_STATUS_MAXCOMP_FAIL;

	wlan_pdev_obj_lock(pdev);
	/* Component object was never created, invalid operation */
	if (pdev->pdev_comp_priv_obj[id] == NULL) {
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	wlan_pdev_obj_unlock(pdev);

	/* Invoke registered create handlers */
	handler = g_umac_glb_obj->pdev_delete_handler[id];
	arg = g_umac_glb_obj->pdev_delete_handler_arg[id];
	if (handler != NULL)
		pdev->obj_status[id] = handler(pdev, arg);
	else
		return QDF_STATUS_E_FAILURE;

	/* If object status is created, then only handle this object status */
	if (pdev->obj_state == WLAN_OBJ_STATE_CREATED) {
		obj_status = wlan_objmgr_pdev_object_status(pdev);
		/* move object state to DEL progress */
		if (obj_status == QDF_STATUS_COMP_ASYNC)
			pdev->obj_state = WLAN_OBJ_STATE_COMP_DEL_PROGRESS;
	}
	return obj_status;
}

static void wlan_obj_pdev_vdevlist_add_tail(qdf_list_t *obj_list,
				struct wlan_objmgr_vdev *obj)
{
	qdf_list_insert_back(obj_list, &obj->vdev_node);
}

static QDF_STATUS wlan_obj_pdev_vdevlist_remove_vdev(
				qdf_list_t *obj_list,
				struct wlan_objmgr_vdev *vdev)
{
	qdf_list_node_t *vdev_node = NULL;

	if (vdev == NULL)
		return QDF_STATUS_E_FAILURE;
	/* get vdev list node element */
	vdev_node = &vdev->vdev_node;
	/* list is empty, return failure */
	if (qdf_list_remove_node(obj_list, vdev_node) != QDF_STATUS_SUCCESS)
		return QDF_STATUS_E_FAILURE;

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS wlan_objmgr_pdev_vdev_attach(struct wlan_objmgr_pdev *pdev,
					struct wlan_objmgr_vdev *vdev)
{
	struct wlan_objmgr_pdev_objmgr *objmgr = &pdev->pdev_objmgr;

	wlan_pdev_obj_lock(pdev);
	/* If Max vdev count exceeds, return failure */
	if (objmgr->wlan_vdev_count > objmgr->max_vdev_count) {
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	/* Add vdev to pdev's vdev list */
	wlan_obj_pdev_vdevlist_add_tail(&objmgr->wlan_vdev_list, vdev);
	/* Increment vdev count of pdev */
	objmgr->wlan_vdev_count++;
	wlan_pdev_obj_unlock(pdev);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS wlan_objmgr_pdev_vdev_detach(struct wlan_objmgr_pdev *pdev,
				struct wlan_objmgr_vdev *vdev)
{
	struct wlan_objmgr_pdev_objmgr *objmgr = &pdev->pdev_objmgr;

	wlan_pdev_obj_lock(pdev);
	/* if vdev count is 0, return failure */
	if (objmgr->wlan_vdev_count == 0) {
		wlan_pdev_obj_unlock(pdev);
		return QDF_STATUS_E_FAILURE;
	}
	/* remove vdev from pdev's vdev list */
	wlan_obj_pdev_vdevlist_remove_vdev(&objmgr->wlan_vdev_list, vdev);
	/* decrement vdev count */
	objmgr->wlan_vdev_count--;

	wlan_pdev_obj_unlock(pdev);
	return QDF_STATUS_SUCCESS;
}

struct wlan_objmgr_vdev *wlan_objmgr_find_vdev_by_id_from_pdev(
			struct wlan_objmgr_pdev *pdev, uint8_t vdev_id)
{
	struct wlan_objmgr_vdev *vdev;
	struct wlan_objmgr_vdev *vdev_next;
	struct wlan_objmgr_pdev_objmgr *objmgr;
	qdf_list_t *vdev_list;

	wlan_pdev_obj_lock(pdev);

	objmgr = &pdev->pdev_objmgr;
	vdev_list = &objmgr->wlan_vdev_list;
	/* Get first vdev */
	vdev = wlan_pdev_vdev_list_peek_head(vdev_list);
	/* Iterate through pdev's vdev list, till vdev id matches with
	entry of vdev list */
	while (vdev != NULL) {
		if (wlan_vdev_get_id(vdev) == vdev_id) {
			wlan_pdev_obj_unlock(pdev);
			return vdev;
		}
		/* get next vdev */
		vdev_next = wlan_vdev_get_next_vdev_of_pdev(vdev_list, vdev);
		vdev = vdev_next;
	}
	wlan_pdev_obj_unlock(pdev);
	return NULL;
}
EXPORT_SYMBOL(wlan_objmgr_find_vdev_by_id_from_pdev);

struct wlan_objmgr_vdev *wlan_objmgr_find_vdev_by_macaddr_from_pdev(
		struct wlan_objmgr_pdev *pdev, uint8_t *macaddr)
{
	struct wlan_objmgr_vdev *vdev;
	struct wlan_objmgr_vdev *vdev_next;
	struct wlan_objmgr_pdev_objmgr *objmgr;
	qdf_list_t *vdev_list;

	wlan_pdev_obj_lock(pdev);
	objmgr = &pdev->pdev_objmgr;
	vdev_list = &objmgr->wlan_vdev_list;
	/* Get first vdev */
	vdev = wlan_pdev_vdev_list_peek_head(vdev_list);
	/* Iterate through pdev's vdev list, till vdev macaddr matches with
	entry of vdev list */
	while (vdev != NULL) {
		if (WLAN_ADDR_EQ(wlan_vdev_mlme_get_macaddr(vdev), macaddr)
					== QDF_STATUS_SUCCESS) {
			wlan_pdev_obj_unlock(pdev);
			return vdev;
		}
		/* get next vdev */
		vdev_next = wlan_vdev_get_next_vdev_of_pdev(vdev_list, vdev);
		vdev = vdev_next;
	}
	wlan_pdev_obj_unlock(pdev);
	return NULL;
}
