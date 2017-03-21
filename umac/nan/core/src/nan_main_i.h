/*
 * Copyright (c) 2012-2017 The Linux Foundation. All rights reserved.
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
 * DOC: contains declaration of common utility APIs and private structs to be
 * used in NAN modules
 */

#ifndef _WLAN_NAN_MAIN_I_H_
#define _WLAN_NAN_MAIN_I_H_

#include "qdf_types.h"
#include "qdf_status.h"
#include "nan_public_structs.h"
#include "wlan_objmgr_cmn.h"

struct wlan_objmgr_vdev;
struct wlan_objmgr_psoc;
struct scheduler_msg;

#define nan_log(level, args...) \
	QDF_TRACE(QDF_MODULE_ID_NAN, level, ## args)
#define nan_logfl(level, format, args...) \
	nan_log(level, FL(format), ## args)

#define nan_alert(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_FATAL, format, ## args)
#define nan_err(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_ERROR, format, ## args)
#define nan_warn(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_WARN, format, ## args)
#define nan_notice(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_INFO, format, ## args)
#define nan_info(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_INFO_HIGH, format, ## args)
#define nan_debug(format, args...) \
	nan_logfl(QDF_TRACE_LEVEL_DEBUG, format, ## args)

#ifndef MAX_PEERS
#define MAX_PEERS 32
#endif

/**
 * struct nan_psoc_priv_obj - nan private psoc obj
 * @lock: lock to be acquired before reading or writing to object
 * @cb_obj: struct contaning callback pointers
 */
struct nan_psoc_priv_obj {
	qdf_spinlock_t lock;
	struct nan_callbacks cb_obj;
};

/**
 * struct nan_vdev_priv_obj - nan private vdev obj
 * @lock: lock to be acquired before reading or writing to object
 * @state: Current state of NDP
 * @active_ndp_sessions: active ndp sessions per adapter
 * @active_ndp_peers: number of active ndp peers
 * @ndp_create_transaction_id: transaction id for create req
 * @ndp_delete_transaction_id: transaction id for delete req
 * @ndi_delete_rsp_reason: reason code for ndi_delete rsp
 * @ndi_delete_rsp_status: status for ndi_delete rsp
 */
struct nan_vdev_priv_obj {
	qdf_spinlock_t lock;
	enum nan_datapath_state state;
	/* idx in following array should follow conn_info.peerMacAddress */
	uint32_t active_ndp_sessions[MAX_PEERS];
	uint32_t active_ndp_peers;
	uint16_t ndp_create_transaction_id;
	uint16_t ndp_delete_transaction_id;
	uint32_t ndi_delete_rsp_reason;
	uint32_t ndi_delete_rsp_status;
};

#endif
