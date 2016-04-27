/*
 * Copyright (c) 2015-2016 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
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

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

#include "targcfg.h"
#include "qdf_lock.h"
#include "qdf_status.h"
#include "qdf_status.h"
#include <qdf_atomic.h>         /* qdf_atomic_read */
#include <targaddrs.h>
#include <bmi_msg.h>
#include "hif_io32.h"
#include <hif.h>
#include <htc_services.h>
#include "regtable.h"
#define ATH_MODULE_NAME hif
#include <a_debug.h>
#include "hif_main.h"
#include "hif_hw_version.h"
#include "ce_api.h"
#include "ce_tasklet.h"
#include "qdf_trace.h"
#include "qdf_status.h"
#ifdef CONFIG_CNSS
#include <net/cnss.h>
#endif
#include "epping_main.h"
#include "hif_debug.h"
#include "mp_dev.h"
#include "platform_icnss.h"

#define AGC_DUMP         1
#define CHANINFO_DUMP    2
#define BB_WATCHDOG_DUMP 3
#ifdef CONFIG_ATH_PCIE_ACCESS_DEBUG
#define PCIE_ACCESS_DUMP 4
#endif

void hif_dump(struct hif_opaque_softc *hif_ctx, uint8_t cmd_id, bool start)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);
	switch (cmd_id) {
	case AGC_DUMP:
		if (start)
			priv_start_agc(scn);
		else
			priv_dump_agc(scn);
		break;

	case CHANINFO_DUMP:
		if (start)
			priv_start_cap_chaninfo(scn);
		else
			priv_dump_chaninfo(scn);
		break;

	case BB_WATCHDOG_DUMP:
		priv_dump_bbwatchdog(scn);
		break;

#ifdef CONFIG_ATH_PCIE_ACCESS_DEBUG
	case PCIE_ACCESS_DUMP:
		hif_target_dump_access_log();
		break;
#endif
	default:
		HIF_ERROR("%s: Invalid htc dump command", __func__);
		break;
	}
}

/**
 * hif_shut_down_device() - hif_shut_down_device
 *
 * SThis fucntion shuts down the device
 *
 * @scn: hif_opaque_softc
 *
 * Return: void
 */
void hif_shut_down_device(struct hif_opaque_softc *scn)
{
	hif_stop(scn);
}

/**
 * hif_get_target_id(): hif_get_target_id
 *
 * Return the virtual memory base address to the caller
 *
 * @scn: hif_softc
 *
 * Return: A_target_id_t
 */
A_target_id_t hif_get_target_id(struct hif_softc *scn)
{
	return scn->mem;
}

static inline void hif_fw_event_handler(struct HIF_CE_state *hif_state)
{
	struct hif_msg_callbacks *msg_callbacks =
		&hif_state->msg_callbacks_current;

	if (!msg_callbacks->fwEventHandler)
		return;

	msg_callbacks->fwEventHandler(msg_callbacks->Context,
			QDF_STATUS_E_FAILURE);
}

/**
 * hif_fw_interrupt_handler(): FW interrupt handler
 *
 * This function is the FW interrupt handlder
 *
 * @irq: irq number
 * @arg: the user pointer
 *
 * Return: bool
 */
#ifndef QCA_WIFI_3_0
irqreturn_t hif_fw_interrupt_handler(int irq, void *arg)
{
	struct hif_softc *scn = arg;
	struct HIF_CE_state *hif_state = HIF_GET_CE_STATE(scn);
	uint32_t fw_indicator_address, fw_indicator;

	if (Q_TARGET_ACCESS_BEGIN(scn) < 0)
		return ATH_ISR_NOSCHED;

	fw_indicator_address = hif_state->fw_indicator_address;
	/* For sudden unplug this will return ~0 */
	fw_indicator = A_TARGET_READ(scn, fw_indicator_address);

	if ((fw_indicator != ~0) && (fw_indicator & FW_IND_EVENT_PENDING)) {
		/* ACK: clear Target-side pending event */
		A_TARGET_WRITE(scn, fw_indicator_address,
			       fw_indicator & ~FW_IND_EVENT_PENDING);
		if (Q_TARGET_ACCESS_END(scn) < 0)
			return ATH_ISR_SCHED;

		if (hif_state->started) {
			hif_fw_event_handler(hif_state);
		} else {
			/*
			 * Probable Target failure before we're prepared
			 * to handle it.  Generally unexpected.
			 */
			AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
				("%s: Early firmware event indicated\n",
				 __func__));
		}
	} else {
		if (Q_TARGET_ACCESS_END(scn) < 0)
			return ATH_ISR_SCHED;
	}

	return ATH_ISR_SCHED;
}
#else
irqreturn_t hif_fw_interrupt_handler(int irq, void *arg)
{
	return ATH_ISR_SCHED;
}
#endif /* #ifdef QCA_WIFI_3_0 */

/**
 * hif_get_targetdef(): hif_get_targetdef
 * @scn: scn
 *
 * Return: void *
 */
void *hif_get_targetdef(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	return scn->targetdef;
}

/**
 * hif_vote_link_down(): unvote for link up
 *
 * Call hif_vote_link_down to release a previous request made using
 * hif_vote_link_up. A hif_vote_link_down call should only be made
 * after a corresponding hif_vote_link_up, otherwise you could be
 * negating a vote from another source. When no votes are present
 * hif will not guarantee the linkstate after hif_bus_suspend.
 *
 * SYNCHRONIZE WITH hif_vote_link_up by only calling in MC thread
 * and initialization deinitialization sequencences.
 *
 * Return: n/a
 */
void hif_vote_link_down(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);
	QDF_BUG(scn);

	scn->linkstate_vote--;
	if (scn->linkstate_vote == 0)
		hif_bus_prevent_linkdown(scn, false);
}

/**
 * hif_vote_link_up(): vote to prevent bus from suspending
 *
 * Makes hif guarantee that fw can message the host normally
 * durring suspend.
 *
 * SYNCHRONIZE WITH hif_vote_link_up by only calling in MC thread
 * and initialization deinitialization sequencences.
 *
 * Return: n/a
 */
void hif_vote_link_up(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);
	QDF_BUG(scn);

	scn->linkstate_vote++;
	if (scn->linkstate_vote == 1)
		hif_bus_prevent_linkdown(scn, true);
}

/**
 * hif_can_suspend_link(): query if hif is permitted to suspend the link
 *
 * Hif will ensure that the link won't be suspended if the upperlayers
 * don't want it to.
 *
 * SYNCHRONIZATION: MC thread is stopped before bus suspend thus
 * we don't need extra locking to ensure votes dont change while
 * we are in the process of suspending or resuming.
 *
 * Return: false if hif will guarantee link up durring suspend.
 */
bool hif_can_suspend_link(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);
	QDF_BUG(scn);

	return scn->linkstate_vote == 0;
}

/**
 * hif_hia_item_address(): hif_hia_item_address
 * @target_type: target_type
 * @item_offset: item_offset
 *
 * Return: n/a
 */
uint32_t hif_hia_item_address(uint32_t target_type, uint32_t item_offset)
{
	switch (target_type) {
	case TARGET_TYPE_AR6002:
		return AR6002_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6003:
		return AR6003_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6004:
		return AR6004_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6006:
		return AR6006_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR9888:
		return AR9888_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_AR6320:
	case TARGET_TYPE_AR6320V2:
		return AR6320_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_QCA6180:
		return QCA6180_HOST_INTEREST_ADDRESS + item_offset;
	case TARGET_TYPE_ADRASTEA:
		/* ADRASTEA doesn't have a host interest address */
		ASSERT(0);
		return 0;
	default:
		ASSERT(0);
		return 0;
	}
}

/**
 * hif_max_num_receives_reached() - check max receive is reached
 * @scn: HIF Context
 * @count: unsigned int.
 *
 * Output check status as bool
 *
 * Return: bool
 */
bool hif_max_num_receives_reached(struct hif_softc *scn, unsigned int count)
{
	if (WLAN_IS_EPPING_ENABLED(hif_get_conparam(scn)))
		return count > 120;
	else
		return count > MAX_NUM_OF_RECEIVES;
}

/**
 * init_buffer_count() - initial buffer count
 * @maxSize: qdf_size_t
 *
 * routine to modify the initial buffer count to be allocated on an os
 * platform basis. Platform owner will need to modify this as needed
 *
 * Return: qdf_size_t
 */
qdf_size_t init_buffer_count(qdf_size_t maxSize)
{
	return maxSize;
}

/**
 * hif_save_htc_htt_config_endpoint() - save htt_tx_endpoint
 * @hif_ctx: hif context
 * @htc_htt_tx_endpoint: htt_tx_endpoint
 *
 * Return: void
 */
void hif_save_htc_htt_config_endpoint(struct hif_opaque_softc *hif_ctx,
							int htc_htt_tx_endpoint)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn) {
		HIF_ERROR("%s: error: scn or scn->hif_sc is NULL!",
		       __func__);
		return;
	}

	scn->htc_htt_tx_endpoint = htc_htt_tx_endpoint;
}

/**
 * hif_get_hw_name(): get a human readable name for the hardware
 * @info: Target Info
 *
 * Return: human readable name for the underlying wifi hardware.
 */
static const char *hif_get_hw_name(struct hif_target_info *info)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(qwlan_hw_list); i++) {
		if (info->target_version == qwlan_hw_list[i].id &&
		    info->target_revision == qwlan_hw_list[i].subid) {
			return qwlan_hw_list[i].name;
		}
	}

	return "Unknown Device";
}

/**
 * hif_get_hw_info(): hif_get_hw_info
 * @scn: scn
 * @version: version
 * @revision: revision
 *
 * Return: n/a
 */
void hif_get_hw_info(struct hif_opaque_softc *scn, u32 *version, u32 *revision,
			const char **target_name)
{
	struct hif_target_info *info = hif_get_target_info_handle(scn);
	*version = info->target_version;
	*revision = info->target_revision;
	*target_name = hif_get_hw_name(info);
}

/**
 * hif_open(): hif_open
 * @qdf_ctx: QDF Context
 * @mode: Driver Mode
 * @bus_type: Bus Type
 * @cbk: CDS Callbacks
 *
 * API to open HIF Context
 *
 * Return: HIF Opaque Pointer
 */
struct hif_opaque_softc *hif_open(qdf_device_t qdf_ctx, uint32_t mode,
				  enum qdf_bus_type bus_type,
				  struct hif_driver_state_callbacks *cbk)
{
	struct hif_softc *scn;
	QDF_STATUS status = QDF_STATUS_SUCCESS;
	int bus_context_size = hif_bus_get_context_size(bus_type);

	if (bus_context_size == 0) {
		HIF_ERROR("%s: context size 0 not allowed", __func__);
		return NULL;
	}

	scn = (struct hif_softc *)qdf_mem_malloc(bus_context_size);
	if (!scn) {
		HIF_ERROR("%s: cannot alloc memory for HIF context of size:%d",
						__func__, bus_context_size);
		return GET_HIF_OPAQUE_HDL(scn);
	}

	qdf_mem_zero(scn, bus_context_size);

	scn->qdf_dev = qdf_ctx;
	scn->hif_con_param = mode;
	qdf_atomic_init(&scn->active_tasklet_cnt);
	qdf_atomic_init(&scn->link_suspended);
	qdf_atomic_init(&scn->tasklet_from_intr);
	qdf_mem_copy(&scn->callbacks, cbk, sizeof(struct hif_driver_state_callbacks));

	status = hif_bus_open(scn, bus_type);
	if (status != QDF_STATUS_SUCCESS) {
		HIF_ERROR("%s: hif_bus_open error = %d, bus_type = %d",
				  __func__, status, bus_type);
		qdf_mem_free(scn);
		scn = NULL;
	}

	return GET_HIF_OPAQUE_HDL(scn);
}

/**
 * hif_close(): hif_close
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
void hif_close(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (scn == NULL) {
		HIF_ERROR("%s: hif_opaque_softc is NULL", __func__);
		return;
	}

	if (scn->athdiag_procfs_inited) {
		athdiag_procfs_remove();
		scn->athdiag_procfs_inited = false;
	}

	hif_bus_close(scn);
	qdf_mem_free(scn);
}

/**
 * hif_enable(): hif_enable
 * @hif_ctx: hif_ctx
 * @dev: dev
 * @bdev: bus dev
 * @bid: bus ID
 * @bus_type: bus type
 * @type: enable type
 *
 * Return: QDF_STATUS
 */
QDF_STATUS hif_enable(struct hif_opaque_softc *hif_ctx, struct device *dev,
					  void *bdev, const hif_bus_id *bid,
					  enum qdf_bus_type bus_type,
					  enum hif_enable_type type)
{
	QDF_STATUS status;
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (scn == NULL) {
		HIF_ERROR("%s: hif_ctx = NULL", __func__);
		return QDF_STATUS_E_NULL_VALUE;
	}

	status = hif_enable_bus(scn, dev, bdev, bid, type);
	if (status != QDF_STATUS_SUCCESS) {
		HIF_ERROR("%s: hif_enable_bus error = %d",
				  __func__, status);
		return status;
	}

	if (ADRASTEA_BU)
		hif_vote_link_up(hif_ctx);

	if (hif_bus_configure(scn)) {
		HIF_ERROR("%s: Target probe failed.", __func__);
		hif_disable_bus(scn);
		status = QDF_STATUS_E_FAILURE;
		return status;
	}

	/*
	 * Flag to avoid potential unallocated memory access from MSI
	 * interrupt handler which could get scheduled as soon as MSI
	 * is enabled, i.e to take care of the race due to the order
	 * in where MSI is enabled before the memory, that will be
	 * in interrupt handlers, is allocated.
	 */

	scn->hif_init_done = true;

	HIF_TRACE("%s: X OK", __func__);

	return QDF_STATUS_SUCCESS;
}

/**
 * hif_wlan_disable(): call the platform driver to disable wlan
 * @scn: HIF Context
 *
 * This function passes the con_mode to platform driver to disable
 * wlan.
 *
 * Return: void
 */
void hif_wlan_disable(struct hif_softc *scn)
{
	enum icnss_driver_mode mode;
	uint32_t con_mode = hif_get_conparam(scn);

	if (QDF_GLOBAL_FTM_MODE == con_mode)
		mode = ICNSS_FTM;
	else if (WLAN_IS_EPPING_ENABLED(con_mode))
		mode = ICNSS_EPPING;
	else
		mode = ICNSS_MISSION;

	icnss_wlan_disable(mode);
}

void hif_disable(struct hif_opaque_softc *hif_ctx, enum hif_disable_type type)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!scn)
		return;

	hif_nointrs(scn);
	if (scn->hif_init_done == false)
		hif_shut_down_device(hif_ctx);
	else
		hif_stop(hif_ctx);

	if (ADRASTEA_BU)
		hif_vote_link_down(hif_ctx);

	hif_disable_bus(scn);

	hif_wlan_disable(scn);

	scn->notice_send = false;

	HIF_INFO("%s: X", __func__);
}


/**
 * hif_crash_shutdown_dump_bus_register() - dump bus registers
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
#if defined(TARGET_RAMDUMP_AFTER_KERNEL_PANIC) \
&& defined(DEBUG)

static void hif_crash_shutdown_dump_bus_register(void *hif_ctx)
{
	struct hif_opaque_softc *scn = hif_ctx;

	if (hif_check_soc_status(scn))
		return;

	if (hif_dump_registers(scn))
		HIF_ERROR("Failed to dump bus registers!");
}

/**
 * hif_crash_shutdown(): hif_crash_shutdown
 *
 * This function is called by the platform driver to dump CE registers
 *
 * @hif_ctx: hif_ctx
 *
 * Return: n/a
 */
void hif_crash_shutdown(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	if (!hif_ctx)
		return;

	if (scn->bus_type == QDF_BUS_TYPE_SNOC) {
		HIF_INFO_MED("%s: RAM dump disabled for bustype %d",
				__func__, scn->bus_type);
		return;
	}

	if (OL_TRGET_STATUS_RESET == scn->target_status) {
		HIF_INFO_MED("%s: Target is already asserted, ignore!",
			    __func__);
		return;
	}

	if (hif_is_load_or_unload_in_progress(scn)) {
		HIF_ERROR("%s: Load/unload is in progress, ignore!", __func__);
		return;
	}

	hif_crash_shutdown_dump_bus_register(hif_ctx);

	if (ol_copy_ramdump(hif_ctx))
		goto out;

	HIF_INFO_MED("%s: RAM dump collecting completed!", __func__);

out:
	return;
}
#else
void hif_crash_shutdown(struct hif_opaque_softc *hif_ctx)
{
	HIF_INFO_MED("%s: Collecting target RAM dump disabled",
		__func__);
	return;
}
#endif /* TARGET_RAMDUMP_AFTER_KERNEL_PANIC */

#ifdef QCA_WIFI_3_0
/**
 * hif_check_fw_reg(): hif_check_fw_reg
 * @scn: scn
 * @state:
 *
 * Return: int
 */
int hif_check_fw_reg(struct hif_opaque_softc *scn)
{
	return 0;
}
#endif

#ifdef IPA_OFFLOAD
/**
 * hif_read_phy_mem_base(): hif_read_phy_mem_base
 * @scn: scn
 * @phy_mem_base: physical mem base
 *
 * Return: n/a
 */
void hif_read_phy_mem_base(struct hif_softc *scn, qdf_dma_addr_t *phy_mem_base)
{
	*phy_mem_base = scn->mem_pa;
}
#endif /* IPA_OFFLOAD */

/**
 * hif_get_device_type(): hif_get_device_type
 * @device_id: device_id
 * @revision_id: revision_id
 * @hif_type: returned hif_type
 * @target_type: returned target_type
 *
 * Return: int
 */
int hif_get_device_type(uint32_t device_id,
			uint32_t revision_id,
			uint32_t *hif_type, uint32_t *target_type)
{
	int ret = 0;

	switch (device_id) {
#ifdef QCA_WIFI_3_0_ADRASTEA
	case ADRASTEA_DEVICE_ID:
	case ADRASTEA_DEVICE_ID_P2_E12:

		*hif_type = HIF_TYPE_ADRASTEA;
		*target_type = TARGET_TYPE_ADRASTEA;
		break;
#else
	case QCA6180_DEVICE_ID:
		*hif_type = HIF_TYPE_QCA6180;
		*target_type = TARGET_TYPE_QCA6180;
		break;
#endif

	case AR9888_DEVICE_ID:
		*hif_type = HIF_TYPE_AR9888;
		*target_type = TARGET_TYPE_AR9888;
		break;

	case AR6320_DEVICE_ID:
		switch (revision_id) {
		case AR6320_FW_1_1:
		case AR6320_FW_1_3:
			*hif_type = HIF_TYPE_AR6320;
			*target_type = TARGET_TYPE_AR6320;
			break;

		case AR6320_FW_2_0:
		case AR6320_FW_3_0:
		case AR6320_FW_3_2:
			*hif_type = HIF_TYPE_AR6320V2;
			*target_type = TARGET_TYPE_AR6320V2;
			break;

		default:
			HIF_ERROR("%s: error - dev_id = 0x%x, rev_id = 0x%x",
				   __func__, device_id, revision_id);
			ret = -ENODEV;
			goto end;
		}
		break;

	default:
		HIF_ERROR("%s: Unsupported device ID!", __func__);
		ret = -ENODEV;
		break;
	}
end:
	return ret;
}

/**
 * hif_needs_bmi() - return true if the soc needs bmi through the driver
 * @hif_ctx: hif context
 *
 * Return: true if the soc needs driver bmi otherwise false
 */
bool hif_needs_bmi(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *hif_sc = HIF_GET_SOFTC(hif_ctx);
	return hif_sc->bus_type != QDF_BUS_TYPE_SNOC;
}

/**
 * hif_get_bus_type() - return the bus type
 *
 * Return: enum qdf_bus_type
 */
enum qdf_bus_type hif_get_bus_type(struct hif_opaque_softc *hif_hdl)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_hdl);
	return scn->bus_type;
}

/**
 * Target info and ini parameters are global to the driver
 * Hence these structures are exposed to all the modules in
 * the driver and they don't need to maintains multiple copies
 * of the same info, instead get the handle from hif and
 * modify them in hif
 */

/**
 * hif_get_ini_handle() - API to get hif_config_param handle
 * @hif_ctx: HIF Context
 *
 * Return: pointer to hif_config_info
 */
struct hif_config_info *hif_get_ini_handle(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *sc = HIF_GET_SOFTC(hif_ctx);

	return &sc->hif_config;
}

/**
 * hif_get_target_info_handle() - API to get hif_target_info handle
 * @hif_ctx: HIF context
 *
 * Return: Pointer to hif_target_info
 */
struct hif_target_info *hif_get_target_info_handle(
					struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *sc = HIF_GET_SOFTC(hif_ctx);

	return &sc->target_info;

}

#if defined(FEATURE_LRO)
/**
 * hif_lro_flush_cb_register - API to register for LRO Flush Callback
 * @scn: HIF Context
 * @handler: Function pointer to be called by HIF
 * @data: Private data to be used by the module registering to HIF
 *
 * Return: void
 */
void hif_lro_flush_cb_register(struct hif_opaque_softc *scn,
			       void (handler)(void *), void *data)
{
	ce_lro_flush_cb_register(scn, handler, data);
}

/**
 * hif_lro_flush_cb_deregister - API to deregister for LRO Flush Callbacks
 * @scn: HIF Context
 *
 * Return: void
 */
void hif_lro_flush_cb_deregister(struct hif_opaque_softc *scn)
{
	ce_lro_flush_cb_deregister(scn);
}
#endif

/**
 * hif_get_target_status - API to get target status
 * @hif_ctx: HIF Context
 *
 * Return: enum ol_target_status
 */
ol_target_status hif_get_target_status(struct hif_opaque_softc *hif_ctx)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	return scn->target_status;
}

/**
 * hif_set_target_status() - API to set target status
 * @hif_ctx: HIF Context
 * @status: Target Status
 *
 * Return: void
 */
void hif_set_target_status(struct hif_opaque_softc *hif_ctx,
			   ol_target_status status)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	scn->target_status = status;
}

/**
 * hif_init_ini_config() - API to initialize HIF configuration parameters
 * @hif_ctx: HIF Context
 * @cfg: HIF Configuration
 *
 * Return: void
 */
void hif_init_ini_config(struct hif_opaque_softc *hif_ctx,
			 struct hif_config_info *cfg)
{
	struct hif_softc *scn = HIF_GET_SOFTC(hif_ctx);

	qdf_mem_copy(&scn->hif_config, cfg, sizeof(struct hif_config_info));
}

/**
 * hif_get_conparam() - API to get driver mode in HIF
 * @scn: HIF Context
 *
 * Return: driver mode of operation
 */
uint32_t hif_get_conparam(struct hif_softc *scn)
{
	if (!scn)
		return 0;

	return scn->hif_con_param;
}

/**
 * hif_get_callbacks_handle() - API to get callbacks Handle
 * @scn: HIF Context
 *
 * Return: pointer to HIF Callbacks
 */
struct hif_driver_state_callbacks *hif_get_callbacks_handle(struct hif_softc *scn)
{
	return &scn->callbacks;
}

/**
 * hif_is_driver_unloading() - API to query upper layers if driver is unloading
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_driver_unloading(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_driver_unloading)
		return cbk->is_driver_unloading(cbk->context);

	return false;
}

/**
 * hif_is_load_or_unload_in_progress() - API to query upper layers if
 * load/unload in progress
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_load_or_unload_in_progress(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_load_unload_in_progress)
		return cbk->is_load_unload_in_progress(cbk->context);

	return false;
}

/**
 * hif_is_recovery_in_progress() - API to query upper layers if recovery in
 * progress
 * @scn: HIF Context
 *
 * Return: True/False
 */
bool hif_is_recovery_in_progress(struct hif_softc *scn)
{
	struct hif_driver_state_callbacks *cbk = hif_get_callbacks_handle(scn);

	if (cbk && cbk->is_recovery_in_progress)
		return cbk->is_recovery_in_progress(cbk->context);

	return false;
}
