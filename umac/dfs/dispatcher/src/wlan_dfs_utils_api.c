/*
 * Copyright (c) 2016-2018 The Linux Foundation. All rights reserved.
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

/**
 * DOC: This file has the DFS dispatcher API implementation which is exposed
 * to outside of DFS component.
 */
#include "wlan_dfs_utils_api.h"
#include "wlan_dfs_mlme_api.h"
#include "../../core/src/dfs.h"
#include "../../core/src/dfs_zero_cac.h"
#include "../../core/src/dfs_etsi_precac.h"
#include <wlan_reg_services_api.h>
#include "../../core/src/dfs_random_chan_sel.h"
#ifdef QCA_DFS_USE_POLICY_MANAGER
#include "wlan_policy_mgr_api.h"
#endif
#ifdef QCA_DFS_NOL_PLATFORM_DRV_SUPPORT
#include <pld_common.h>
#endif
#include <qdf_module.h>

struct dfs_nol_info {
	uint16_t num_chans;
	struct dfsreq_nolelem dfs_nol[DFS_MAX_NOL_CHANNEL];
};

QDF_STATUS utils_dfs_reset(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_reset(dfs);
	dfs_nol_update(dfs);
	dfs_reset_precaclists(dfs);
	dfs_reset_etsiprecaclists(dfs);

	return QDF_STATUS_SUCCESS;
}

bool utils_dfs_freq_is_in_nol(struct wlan_objmgr_pdev *pdev, uint32_t freq)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return false;

	return dfs_freq_is_in_nol(dfs, freq);
}

QDF_STATUS utils_dfs_cac_valid_reset(struct wlan_objmgr_pdev *pdev,
		uint8_t prevchan_ieee,
		uint32_t prevchan_flags)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_cac_valid_reset(dfs, prevchan_ieee, prevchan_flags);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_cac_valid_reset);

QDF_STATUS utils_dfs_reset_precaclists(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_reset_precaclists(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_reset_precaclists);

#ifdef QCA_SUPPORT_ETSI_PRECAC_DFS
QDF_STATUS utils_dfs_reset_etsi_precaclists(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_reset_etsiprecaclists(dfs);

	return QDF_STATUS_SUCCESS;
}

qdf_export_symbol(utils_dfs_reset_etsi_precaclists);
#endif

QDF_STATUS utils_dfs_cancel_precac_timer(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_cancel_precac_timer(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_cancel_precac_timer);

QDF_STATUS utils_dfs_start_precac_timer(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS, "NULL dfs");
		return  QDF_STATUS_E_FAILURE;
	}
	dfs_start_precac_timer(dfs, dfs->dfs_precac_secondary_freq);
	return QDF_STATUS_SUCCESS;
}

#ifdef WLAN_DFS_PRECAC_AUTO_CHAN_SUPPORT
QDF_STATUS utils_dfs_precac_decide_pref_chan(struct wlan_objmgr_pdev *pdev,
					     uint8_t *ch_ieee)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS, "NULL dfs");
		return  QDF_STATUS_E_FAILURE;
	}
	dfs_decide_precac_preferred_chan(dfs, ch_ieee);

	return QDF_STATUS_SUCCESS;
}
#endif

QDF_STATUS utils_dfs_cancel_cac_timer(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_cancel_cac_timer(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_cancel_cac_timer);

QDF_STATUS utils_dfs_start_cac_timer(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_start_cac_timer(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_start_cac_timer);

QDF_STATUS utils_dfs_cac_stop(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_cac_stop(dfs);
	return  QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_cac_stop);

void utils_dfs_clear_cac_started_chan(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return;

	dfs_clear_cac_started_chan(dfs);
}

/** utils_fill_dfs_chan_info() - Fill the dfs channel structure with wlan
 * channel.
 * @chan: Pointer to DFS channel structure.
 * @wlan_chan: Pointer to WLAN Channel structure.
 *
 * Return: void
 */
static void utils_fill_dfs_chan_info(struct dfs_channel *chan,
				     struct wlan_channel *wlan_chan)
{
	chan->dfs_ch_freq = wlan_chan->ch_freq;
	chan->dfs_ch_flags = wlan_chan->ch_flags;
	chan->dfs_ch_flagext = wlan_chan->ch_flagext;
	chan->dfs_ch_ieee = wlan_chan->ch_ieee;
	chan->dfs_ch_vhtop_ch_freq_seg1 = wlan_chan->ch_freq_seg1;
	chan->dfs_ch_vhtop_ch_freq_seg2 = wlan_chan->ch_freq_seg2;
}

bool utils_dfs_is_precac_done(struct wlan_objmgr_pdev *pdev,
			      struct wlan_channel *wlan_chan)
{
	struct wlan_dfs *dfs;
	struct dfs_channel chan;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return false;
	utils_fill_dfs_chan_info(&chan, wlan_chan);
	return dfs_is_precac_done(dfs, &chan);
}

bool utils_dfs_check_for_cac_start(struct wlan_objmgr_pdev *pdev,
				   bool *continue_current_cac)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return false;

	return dfs_check_for_cac_start(dfs, continue_current_cac);
}

QDF_STATUS utils_dfs_stacac_stop(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_stacac_stop(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_stacac_stop);

QDF_STATUS utils_dfs_get_usenol(struct wlan_objmgr_pdev *pdev, uint16_t *usenol)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	*usenol = dfs_get_use_nol(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_get_usenol);

QDF_STATUS utils_dfs_radar_disable(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_radar_disable(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_radar_disable);

QDF_STATUS utils_dfs_set_update_nol_flag(struct wlan_objmgr_pdev *pdev,
		bool val)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_set_update_nol_flag(dfs, val);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_set_update_nol_flag);

QDF_STATUS utils_dfs_get_update_nol_flag(struct wlan_objmgr_pdev *pdev,
		bool *nol_flag)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	*nol_flag = dfs_get_update_nol_flag(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_get_update_nol_flag);

QDF_STATUS utils_dfs_get_dfs_use_nol(struct wlan_objmgr_pdev *pdev,
		int *dfs_use_nol)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	*dfs_use_nol = dfs_get_use_nol(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_get_dfs_use_nol);

QDF_STATUS utils_dfs_get_nol_timeout(struct wlan_objmgr_pdev *pdev,
		int *dfs_nol_timeout)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	*dfs_nol_timeout = dfs_get_nol_timeout(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_get_nol_timeout);

QDF_STATUS utils_dfs_nol_addchan(struct wlan_objmgr_pdev *pdev,
		uint16_t freq,
		uint32_t dfs_nol_timeout)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	DFS_NOL_ADD_CHAN_LOCKED(dfs, freq, dfs_nol_timeout);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_nol_addchan);

QDF_STATUS utils_dfs_nol_update(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_nol_update(dfs);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_nol_update);

QDF_STATUS utils_dfs_second_segment_radar_disable(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_second_segment_radar_disable(dfs);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS utils_dfs_bw_reduce(struct wlan_objmgr_pdev *pdev, bool bw_reduce)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs->dfs_bw_reduced = bw_reduce;

	return QDF_STATUS_SUCCESS;
}

qdf_export_symbol(utils_dfs_bw_reduce);

QDF_STATUS utils_dfs_is_bw_reduce(struct wlan_objmgr_pdev *pdev,
				  bool *bw_reduce)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	*bw_reduce = dfs->dfs_bw_reduced;

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS utils_dfs_fetch_nol_ie_info(struct wlan_objmgr_pdev *pdev,
				       uint8_t *nol_ie_bandwidth,
				       uint16_t *nol_ie_startfreq,
				       uint8_t *nol_ie_bitmap)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_fetch_nol_ie_info(dfs, nol_ie_bandwidth, nol_ie_startfreq,
			      nol_ie_bitmap);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS utils_dfs_set_rcsa_flags(struct wlan_objmgr_pdev *pdev,
				    bool is_rcsa_ie_sent,
				    bool is_nol_ie_sent)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_set_rcsa_flags(dfs, is_rcsa_ie_sent, is_nol_ie_sent);

	return QDF_STATUS_SUCCESS;
}

QDF_STATUS utils_dfs_get_rcsa_flags(struct wlan_objmgr_pdev *pdev,
				    bool *is_rcsa_ie_sent,
				    bool *is_nol_ie_sent)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;
	dfs_get_rcsa_flags(dfs, is_rcsa_ie_sent, is_nol_ie_sent);

	return QDF_STATUS_SUCCESS;
}

bool utils_dfs_process_nol_ie_bitmap(struct wlan_objmgr_pdev *pdev,
				     uint8_t nol_ie_bandwidth,
				     uint16_t nol_ie_startfreq,
				     uint8_t nol_ie_bitmap)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  false;
	return dfs_process_nol_ie_bitmap(dfs, nol_ie_bandwidth,
					 nol_ie_startfreq,
					 nol_ie_bitmap);
}

QDF_STATUS utils_dfs_set_cac_timer_running(struct wlan_objmgr_pdev *pdev,
		int val)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs->dfs_cac_timer_running = val;

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_set_cac_timer_running);

QDF_STATUS utils_dfs_get_nol_chfreq_and_chwidth(struct wlan_objmgr_pdev *pdev,
		void *nollist,
		uint32_t *nol_chfreq,
		uint32_t *nol_chwidth,
		int index)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_get_nol_chfreq_and_chwidth(nollist, nol_chfreq, nol_chwidth, index);

	return QDF_STATUS_SUCCESS;
}
qdf_export_symbol(utils_dfs_get_nol_chfreq_and_chwidth);

QDF_STATUS utils_dfs_update_cur_chan_flags(struct wlan_objmgr_pdev *pdev,
		uint64_t flags,
		uint16_t flagext)
{
	struct wlan_dfs *dfs;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return  QDF_STATUS_E_FAILURE;

	dfs_update_cur_chan_flags(dfs, flags, flagext);

	return QDF_STATUS_SUCCESS;
}

static void utils_dfs_get_max_phy_mode(struct wlan_objmgr_pdev *pdev,
		uint32_t *phy_mode)
{
	return;
}

static void utils_dfs_get_max_sup_width(struct wlan_objmgr_pdev *pdev,
		uint8_t *ch_width)
{
	return;
}

#ifndef QCA_DFS_USE_POLICY_MANAGER
void utils_dfs_get_nol_history_chan_list(struct wlan_objmgr_pdev *pdev,
					 void *clist, uint32_t *num_chan)
{
	int i, j = 0;
	struct regulatory_channel *cur_chan_list;
	struct wlan_dfs *dfs;
	struct dfs_channel *chan_list = (struct dfs_channel *)clist;

	*num_chan = 0;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return;

	cur_chan_list = qdf_mem_malloc(NUM_CHANNELS * sizeof(*cur_chan_list));
	if (!cur_chan_list)
		return;

	if (wlan_reg_get_current_chan_list(
			pdev, cur_chan_list) != QDF_STATUS_SUCCESS) {
		dfs_alert(dfs, WLAN_DEBUG_DFS_ALWAYS,
			  "failed to get cur_chan list");
		qdf_mem_free(cur_chan_list);
		return;
	}

	for (i = 0; i < NUM_CHANNELS; i++) {
		if (cur_chan_list[i].nol_history) {
			chan_list[j].dfs_ch_freq = cur_chan_list[i].center_freq;
			j++;
		}
	}

	*num_chan = j;
	qdf_mem_free(cur_chan_list);
}

void utils_dfs_get_chan_list(struct wlan_objmgr_pdev *pdev,
			     void *clist, uint32_t *num_chan)
{
	int i = 0, j = 0;
	enum channel_state state;
	struct regulatory_channel *cur_chan_list;
	struct wlan_dfs *dfs;
	struct dfs_channel *chan_list = (struct dfs_channel *)clist;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs)
		return;

	cur_chan_list = qdf_mem_malloc(NUM_CHANNELS *
			sizeof(struct regulatory_channel));
	if (!cur_chan_list) {
		dfs_alert(dfs, WLAN_DEBUG_DFS_ALWAYS, "fail to alloc");
		*num_chan = 0;
		return;
	}

	if (wlan_reg_get_current_chan_list(
			pdev, cur_chan_list) != QDF_STATUS_SUCCESS) {
		*num_chan = 0;
		dfs_alert(dfs, WLAN_DEBUG_DFS_ALWAYS,
				"failed to get curr channel list");
		return;
	}

	for (i = 0; i < NUM_CHANNELS; i++) {
		state = cur_chan_list[i].state;
		if (state == CHANNEL_STATE_DFS ||
				state == CHANNEL_STATE_ENABLE) {
			chan_list[j].dfs_ch_ieee = cur_chan_list[i].chan_num;
			chan_list[j].dfs_ch_freq = cur_chan_list[i].center_freq;
			if (state == CHANNEL_STATE_DFS)
				chan_list[j].dfs_ch_flagext =
					WLAN_CHAN_DFS;

			if (cur_chan_list[i].nol_history)
				chan_list[j].dfs_ch_flagext |=
					WLAN_CHAN_HISTORY_RADAR;
			j++;
		}
	}
	*num_chan = j;
	qdf_mem_free(cur_chan_list);

	return;
}

/**
 * utils_dfs_get_channel_list() - Get channel list from regdb component, based
 * on current channel list.
 * @pdev: Pointer to pdev structure.
 * @chan: Pointer to channel list.
 * @num_chan: number of channels.
 *
 * Get regdb channel list based on dfs current channel.
 * Ex: When  AP is operating in 5GHz channel, filter 2.4GHz and 4.9GHZ channels
 * so that the random channel function does not select either 2.4GHz or 4.9GHz
 * channel.
 */
static void utils_dfs_get_channel_list(struct wlan_objmgr_pdev *pdev,
				       struct dfs_channel *chan_list,
				       uint32_t *num_chan)
{
	struct dfs_channel *tmp_chan_list = NULL;
	struct wlan_dfs *dfs;
	bool is_curchan_5g;
	bool is_curchan_24g;
	bool is_curchan_49g;
	uint32_t chan_num;
	uint32_t center_freq;
	uint16_t flagext;
	int i, j = 0;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS, "null dfs");
		return;
	}

	tmp_chan_list = qdf_mem_malloc(*num_chan * sizeof(*tmp_chan_list));
	if (!tmp_chan_list) {
		dfs_alert(dfs, WLAN_DEBUG_DFS_ALWAYS, "mem alloc failed");
		return;
	}

	utils_dfs_get_chan_list(pdev, (void *)tmp_chan_list, num_chan);

	chan_num = dfs->dfs_curchan->dfs_ch_ieee;
	center_freq = dfs->dfs_curchan->dfs_ch_freq;
	is_curchan_5g = WLAN_REG_IS_5GHZ_CH(chan_num);
	is_curchan_24g = WLAN_REG_IS_24GHZ_CH(chan_num);
	is_curchan_49g = WLAN_REG_IS_49GHZ_FREQ(center_freq);

	for (i = 0; i < *num_chan; i++) {
		chan_num = tmp_chan_list[i].dfs_ch_ieee;
		center_freq = tmp_chan_list[i].dfs_ch_freq;
		flagext = tmp_chan_list[i].dfs_ch_flagext;

		if (!dfs_mlme_check_allowed_prim_chanlist(pdev, chan_num))
			continue;

		if ((is_curchan_5g) && WLAN_REG_IS_5GHZ_CH(chan_num)) {
			chan_list[j].dfs_ch_ieee = chan_num;
			chan_list[j].dfs_ch_freq = center_freq;
			chan_list[j].dfs_ch_flagext = flagext;
			j++;
		} else if ((is_curchan_24g) &&
				WLAN_REG_IS_24GHZ_CH(chan_num)) {
			chan_list[j].dfs_ch_ieee = chan_num;
			chan_list[j].dfs_ch_freq = center_freq;
			j++;
		} else if ((is_curchan_49g) &&
				WLAN_REG_IS_49GHZ_FREQ(center_freq)) {
			chan_list[j].dfs_ch_ieee = chan_num;
			chan_list[j].dfs_ch_freq = center_freq;
			j++;
		}
	}

	*num_chan = j;

	qdf_mem_free(tmp_chan_list);
}

#else

void utils_dfs_get_nol_history_chan_list(struct wlan_objmgr_pdev *pdev,
					 void *clist, uint32_t *num_chan)
{
	utils_dfs_get_chan_list(pdev, clist, num_chan);
}

void utils_dfs_get_chan_list(struct wlan_objmgr_pdev *pdev,
			     void *clist, uint32_t *num_chan)
{
	uint8_t pcl_ch[QDF_MAX_NUM_CHAN] = {0};
	uint8_t weight_list[QDF_MAX_NUM_CHAN] = {0};
	uint32_t len;
	uint32_t weight_len;
	int i;
	struct wlan_objmgr_psoc *psoc;
	uint32_t conn_count = 0;
	struct dfs_channel *chan_list = (struct dfs_channel *)clist;

	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		*num_chan = 0;
		dfs_err(NULL, WLAN_DEBUG_DFS_ALWAYS,  "null psoc");
		return;
	}

	len = QDF_ARRAY_SIZE(pcl_ch);
	weight_len = QDF_ARRAY_SIZE(weight_list);
	conn_count = policy_mgr_mode_specific_connection_count(
			psoc, PM_SAP_MODE, NULL);
	if (0 == conn_count)
		policy_mgr_get_pcl(psoc, PM_SAP_MODE, pcl_ch,
				&len, weight_list, weight_len);
	else
		policy_mgr_get_pcl_for_existing_conn(psoc, PM_SAP_MODE, pcl_ch,
				&len, weight_list, weight_len, true);

	if (*num_chan < len) {
		dfs_err(NULL, WLAN_DEBUG_DFS_ALWAYS,
				"Invalid len src=%d, dst=%d",
				*num_chan, len);
		*num_chan = 0;
		return;
	}

	for (i = 0; i < len; i++) {
		chan_list[i].dfs_ch_ieee  = pcl_ch[i];
		chan_list[i].dfs_ch_freq  =
			wlan_reg_chan_to_freq(pdev, pcl_ch[i]);
	}
	*num_chan = i;
	dfs_info(NULL, WLAN_DEBUG_DFS_ALWAYS, "num channels %d", i);
}

static void utils_dfs_get_channel_list(struct wlan_objmgr_pdev *pdev,
				       struct dfs_channel *chan_list,
				       uint32_t *num_chan)
{
	utils_dfs_get_chan_list(pdev, (void *)chan_list, num_chan);
}
#endif

QDF_STATUS utils_dfs_get_random_channel(
	struct wlan_objmgr_pdev *pdev,
	uint16_t flags,
	struct ch_params *ch_params,
	uint32_t *hw_mode,
	uint8_t *target_chan,
	struct dfs_acs_info *acs_info)
{
	uint32_t dfs_reg;
	uint32_t num_chan = NUM_CHANNELS;
	struct wlan_dfs *dfs = NULL;
	struct wlan_objmgr_psoc *psoc;
	struct dfs_channel *chan_list = NULL;
	struct dfs_channel cur_chan;
	QDF_STATUS status = QDF_STATUS_E_FAILURE;

	*target_chan = 0;
	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null psoc");
		goto random_chan_error;
	}

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		goto random_chan_error;
	}

	wlan_reg_get_dfs_region(pdev, &dfs_reg);
	chan_list = qdf_mem_malloc(num_chan * sizeof(*chan_list));
	if (!chan_list) {
		dfs_alert(dfs, WLAN_DEBUG_DFS_ALWAYS, "mem alloc failed");
		goto random_chan_error;
	}

	utils_dfs_get_channel_list(pdev, chan_list, &num_chan);
	if (!num_chan) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "zero channels");
		goto random_chan_error;
	}

	cur_chan.dfs_ch_vhtop_ch_freq_seg1 = ch_params->center_freq_seg0;
	cur_chan.dfs_ch_vhtop_ch_freq_seg2 = ch_params->center_freq_seg1;

	if (!ch_params->ch_width)
		utils_dfs_get_max_sup_width(pdev,
				(uint8_t *)&ch_params->ch_width);

	*target_chan = dfs_prepare_random_channel(dfs, chan_list,
		num_chan, flags, (uint8_t *)&ch_params->ch_width,
		&cur_chan, (uint8_t)dfs_reg, acs_info);

	ch_params->center_freq_seg0 = cur_chan.dfs_ch_vhtop_ch_freq_seg1;
	ch_params->center_freq_seg1 = cur_chan.dfs_ch_vhtop_ch_freq_seg2;
	dfs_info(dfs, WLAN_DEBUG_DFS_RANDOM_CHAN,
			"input width=%d", ch_params->ch_width);

	if (*target_chan) {
		wlan_reg_set_channel_params(pdev,
				*target_chan, 0, ch_params);
		utils_dfs_get_max_phy_mode(pdev, hw_mode);
		status = QDF_STATUS_SUCCESS;
	}

	dfs_info(dfs, WLAN_DEBUG_DFS_RANDOM_CHAN,
			"ch=%d, seg0=%d, seg1=%d, width=%d",
			*target_chan, ch_params->center_freq_seg0,
			ch_params->center_freq_seg1, ch_params->ch_width);

random_chan_error:
	qdf_mem_free(chan_list);

	return status;
}
qdf_export_symbol(utils_dfs_get_random_channel);

QDF_STATUS utils_dfs_bw_reduced_channel(
	struct wlan_objmgr_pdev *pdev,
	struct ch_params *ch_params,
	uint32_t *hw_mode,
	uint8_t *target_chan)
{
	struct wlan_dfs *dfs = NULL;
	struct wlan_objmgr_psoc *psoc;
	enum channel_state ch_state;
	QDF_STATUS status = QDF_STATUS_E_FAILURE;

	*target_chan = 0;
	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null psoc");
		return status;
	}

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		return status;
	}

	ch_state = reg_get_channel_state(pdev, dfs->dfs_curchan->dfs_ch_ieee);

	if (ch_state == CHANNEL_STATE_DFS ||
	    ch_state == CHANNEL_STATE_ENABLE) {
		ch_params->center_freq_seg0 =
			dfs->dfs_curchan->dfs_ch_vhtop_ch_freq_seg1;
		ch_params->center_freq_seg1 =
			dfs->dfs_curchan->dfs_ch_vhtop_ch_freq_seg2;
		wlan_reg_set_channel_params(pdev,
					    dfs->dfs_curchan->dfs_ch_ieee,
					    0, ch_params);

		*target_chan = dfs->dfs_curchan->dfs_ch_ieee;
		utils_dfs_get_max_phy_mode(pdev, hw_mode);

		return QDF_STATUS_SUCCESS;
	}

	return status;
}

qdf_export_symbol(utils_dfs_bw_reduced_channel);

#ifdef QCA_DFS_NOL_PLATFORM_DRV_SUPPORT
void utils_dfs_init_nol(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs;
	struct wlan_objmgr_psoc *psoc;
	qdf_device_t qdf_dev;
	struct dfs_nol_info *dfs_nolinfo;
	int len;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	psoc = wlan_pdev_get_psoc(pdev);
	if (!dfs || !psoc) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,
				"dfs %pK, psoc %pK", dfs, psoc);
		return;
	}

	qdf_dev = psoc->soc_objmgr.qdf_dev;
	if (!qdf_dev->dev) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null device");
		return;
	}

	dfs_nolinfo = qdf_mem_malloc(sizeof(*dfs_nolinfo));
	if (!dfs_nolinfo) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "dfs_nolinfo alloc fail");
		return;
	}

	qdf_mem_zero(dfs_nolinfo, sizeof(*dfs_nolinfo));
	len = pld_wlan_get_dfs_nol(qdf_dev->dev, (void *)dfs_nolinfo,
				   (uint16_t)sizeof(*dfs_nolinfo));
	if (len > 0) {
		dfs_set_nol(dfs, dfs_nolinfo->dfs_nol, dfs_nolinfo->num_chans);
		dfs_info(dfs, WLAN_DEBUG_DFS_ALWAYS, "nol channels in pld");
		DFS_PRINT_NOL_LOCKED(dfs);
	} else {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "no nol in pld");
	}
	qdf_mem_free(dfs_nolinfo);
}
#endif
qdf_export_symbol(utils_dfs_init_nol);

#ifndef QCA_DFS_NOL_PLATFORM_DRV_SUPPORT
void utils_dfs_save_nol(struct wlan_objmgr_pdev *pdev)
{
}
#else
void utils_dfs_save_nol(struct wlan_objmgr_pdev *pdev)
{
	struct dfs_nol_info *dfs_nolinfo;
	struct wlan_dfs *dfs = NULL;
	struct wlan_objmgr_psoc *psoc;
	qdf_device_t qdf_dev;
	int num_chans = 0;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		return;
	}

	psoc = wlan_pdev_get_psoc(pdev);
	if (!psoc) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null psoc");
		return;
	}

	qdf_dev = psoc->soc_objmgr.qdf_dev;
	if (!qdf_dev->dev) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null device");
		return;
	}

	dfs_nolinfo = qdf_mem_malloc(sizeof(*dfs_nolinfo));
	if (!dfs_nolinfo) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "dfs_nolinfo alloc fail");
		return;
	}

	qdf_mem_zero(dfs_nolinfo, sizeof(*dfs_nolinfo));
	DFS_GET_NOL_LOCKED(dfs, dfs_nolinfo->dfs_nol, &num_chans);
	if (num_chans > 0) {

		if (num_chans > DFS_MAX_NOL_CHANNEL)
			dfs_nolinfo->num_chans = DFS_MAX_NOL_CHANNEL;
		else
			dfs_nolinfo->num_chans = num_chans;

		pld_wlan_set_dfs_nol(qdf_dev->dev, (void *)dfs_nolinfo,
				     (uint16_t)sizeof(*dfs_nolinfo));
	}
	qdf_mem_free(dfs_nolinfo);
}
#endif
qdf_export_symbol(utils_dfs_save_nol);

void utils_dfs_print_nol_channels(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs = NULL;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		return;
	}

	DFS_PRINT_NOL_LOCKED(dfs);
}
qdf_export_symbol(utils_dfs_print_nol_channels);

void utils_dfs_clear_nol_channels(struct wlan_objmgr_pdev *pdev)
{
	struct wlan_dfs *dfs = NULL;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		return;
	}

	/* First print list */
	DFS_PRINT_NOL_LOCKED(dfs);

	/* clear local cache first */
	dfs_nol_timer_cleanup(dfs);
	dfs_nol_update(dfs);

	/*
	 * update platform driver nol list with local cache which is zero,
	 * cleared in above step, so this will clear list in platform driver.
	 */
	utils_dfs_save_nol(pdev);
}
qdf_export_symbol(utils_dfs_clear_nol_channels);

void utils_dfs_reg_update_nol_ch(struct wlan_objmgr_pdev *pdev,
		uint8_t *ch_list,
		uint8_t num_ch,
		bool nol_ch)
{
	/* TODO : Need locking?*/
	wlan_reg_update_nol_ch(pdev, ch_list, num_ch, nol_ch);
}
qdf_export_symbol(utils_dfs_reg_update_nol_ch);

void utils_dfs_reg_update_nol_history_ch(struct wlan_objmgr_pdev *pdev,
					 uint8_t *ch_list,
					 uint8_t num_ch,
					 bool nol_history_ch)
{
	wlan_reg_update_nol_history_ch(pdev, ch_list, num_ch, nol_history_ch);
}

uint8_t utils_dfs_freq_to_chan(uint32_t freq)
{
	uint8_t chan;

	if (freq == 0)
		return 0;

	if (freq > DFS_24_GHZ_BASE_FREQ && freq < DFS_CHAN_14_FREQ)
		chan = ((freq - DFS_24_GHZ_BASE_FREQ) / DFS_CHAN_SPACING_5MHZ);
	else if (freq == DFS_CHAN_14_FREQ)
		chan = DFS_24_GHZ_CHANNEL_14;
	else if ((freq > DFS_24_GHZ_BASE_FREQ) && (freq < DFS_5_GHZ_BASE_FREQ))
		chan = (((freq - DFS_CHAN_15_FREQ) / DFS_CHAN_SPACING_20MHZ) +
			DFS_24_GHZ_CHANNEL_15);
	else
		chan = (freq - DFS_5_GHZ_BASE_FREQ) / DFS_CHAN_SPACING_5MHZ;

	return chan;
}
qdf_export_symbol(utils_dfs_freq_to_chan);

uint32_t utils_dfs_chan_to_freq(uint8_t chan)
{
	if (chan == 0)
		return 0;

	if (chan < DFS_24_GHZ_CHANNEL_14)
		return DFS_24_GHZ_BASE_FREQ + (chan * DFS_CHAN_SPACING_5MHZ);
	else if (chan == DFS_24_GHZ_CHANNEL_14)
		return DFS_CHAN_14_FREQ;
	else if (chan < DFS_24_GHZ_CHANNEL_27)
		return DFS_CHAN_15_FREQ + ((chan - DFS_24_GHZ_CHANNEL_15) *
				DFS_CHAN_SPACING_20MHZ);
	else if (chan == DFS_5_GHZ_CHANNEL_170)
		return DFS_CHAN_170_FREQ;
	else
		return DFS_5_GHZ_BASE_FREQ + (chan * DFS_CHAN_SPACING_5MHZ);
}
qdf_export_symbol(utils_dfs_chan_to_freq);

#ifdef QCA_MCL_DFS_SUPPORT
QDF_STATUS utils_dfs_mark_leaking_ch(struct wlan_objmgr_pdev *pdev,
	enum phy_ch_width ch_width,
	uint8_t temp_ch_lst_sz,
	uint8_t *temp_ch_lst)
{
	struct wlan_dfs *dfs = NULL;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "null dfs");
		return  QDF_STATUS_E_FAILURE;
	}

	return dfs_mark_leaking_ch(dfs, ch_width, temp_ch_lst_sz, temp_ch_lst);
}
qdf_export_symbol(utils_dfs_mark_leaking_ch);
#endif

int utils_get_dfsdomain(struct wlan_objmgr_pdev *pdev)
{
	enum dfs_reg dfsdomain;

	wlan_reg_get_dfs_region(pdev, &dfsdomain);

	return dfsdomain;
}

uint16_t utils_dfs_get_cur_rd(struct wlan_objmgr_pdev *pdev)
{
	struct cur_regdmn_info cur_regdmn;

	wlan_reg_get_curr_regdomain(pdev, &cur_regdmn);

	return cur_regdmn.regdmn_pair_id;
}

#if defined(WLAN_DFS_PARTIAL_OFFLOAD) && defined(HOST_DFS_SPOOF_TEST)
QDF_STATUS utils_dfs_is_spoof_check_failed(struct wlan_objmgr_pdev *pdev,
					   bool *is_spoof_check_failed)
{
	struct wlan_dfs *dfs;

	if (!tgt_dfs_is_pdev_5ghz(pdev))
		return QDF_STATUS_SUCCESS;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "dfs is null");
		return  QDF_STATUS_E_FAILURE;
	}

	*is_spoof_check_failed = dfs->dfs_spoof_check_failed;

	return QDF_STATUS_SUCCESS;
}

qdf_export_symbol(utils_dfs_is_spoof_check_failed);
#endif

int dfs_get_num_chans(void)
{
	return NUM_CHANNELS;
}

#if defined(WLAN_DFS_FULL_OFFLOAD) && defined(QCA_DFS_NOL_OFFLOAD)
QDF_STATUS utils_dfs_get_disable_radar_marking(struct wlan_objmgr_pdev *pdev,
					       bool *disable_radar_marking)
{
	struct wlan_dfs *dfs;

	if (!tgt_dfs_is_pdev_5ghz(pdev))
		return QDF_STATUS_SUCCESS;

	dfs = global_dfs_to_mlme.pdev_get_comp_private_obj(pdev);
	if (!dfs) {
		dfs_err(dfs, WLAN_DEBUG_DFS_ALWAYS,  "dfs is null");
		return  QDF_STATUS_E_FAILURE;
	}

	*disable_radar_marking = dfs_get_disable_radar_marking(dfs);

	return QDF_STATUS_SUCCESS;
}

qdf_export_symbol(utils_dfs_get_disable_radar_marking);
#endif
