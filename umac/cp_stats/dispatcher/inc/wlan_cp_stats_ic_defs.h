/*
 * Copyright (c) 2018 The Linux Foundation. All rights reserved.
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
 * DOC: wlan_cp_stats_ic_defs.h
 *
 * This header file maintain structure definitions for cp stats specific to ic
 */

#ifndef __WLAN_CP_STATS_IC_DEFS_H__
#define __WLAN_CP_STATS_IC_DEFS_H__

#ifdef QCA_SUPPORT_CP_STATS

/**
 * struct pdev_80211_stats - control plane stats at pdev
 *
 * the same statistics were earlier maintained with a reference to
 * ieee80211_mac_stats in vap structure, now the same structure will be
 * used as interface structure with user space application
 * make sure to align this structure with ieee80211_mac_stats
 *
 * @cs_tx_hw_retries:
 * @cs_tx_hw_failures:
 */
struct pdev_80211_stats {
	uint64_t cs_tx_hw_retries;
	uint64_t cs_tx_hw_failures;
};

/**
 * struct pdev_ic_cp_stats - control plane stats specific to WIN at pdev
 * @stats: 80211 stats
 */
struct pdev_ic_cp_stats {
	struct pdev_80211_stats stats;
};

/**
 * struct vdev_80211_stats - control plane 80211 stats at vdev
 *
 * the same statistics were earlier maintained with a reference to
 * ieee80211_mac_stats in vap structure, now the same structure will be
 * used as interface structure with user space application
 * make sure to align this structure with ieee80211_mac_stats
 *
 * @cs_tx_bcn_swba: tx beacon
 * @cs_tx_offchan_mgmt: tx offchan mgmt
 * @cs_tx_offchan_data: tx offchan data
 * @cs_tx_offchan_fail: tx offchan fail
 * @cs_rx_wrongbss: rx from wrong bssid
 * @cs_rx_wrongdir: rx wrong direction
 * @cs_rx_not_assoc: rx discard cuz sta !assoc
 * @cs_rx_no_privacy: rx wep but privacy off
 * @cs_rx_mgt_discard: rx mgmt frames discard
 * @cs_rx_ctl: rx control frames discard
 * @cs_rx_rs_too_big: rx rate set truncated
 * @cs_rx_elem_missing: rx required element missing
 * @cs_rx_elem_too_big: rx elem too big
 * @cs_rx_chan_err: rx chan err
 * @cs_rx_node_alloc: rx frame dropped
 * @cs_rx_ssid_mismatch: rx ssid mismatch
 * @cs_rx_auth_unsupported: rx auth unsupported algo
 * @cs_rx_auth_fail: rx auth fail
 * @cs_rx_auth_countermeasures: rx auth discard cuz counter measures
 * @cs_rx_assoc_bss: rx assoc from wrong bss
 * @cs_rx_assoc_notauth: rx assoc w/o auth
 * @cs_rx_assoc_cap_mismatch: rx assoc w/ cap mismatch
 * @cs_rx_assoc_norate: rx assoc w/ no rate match
 * @cs_rx_assoc_wpaie_err: rx assoc w/ WPA err
 * @cs_rx_action: rx action frames
 * @cs_rx_auth_err: rx auth errors
 * @cs_tx_nodefkey: tx nodefkey cuz no defkey
 * @cs_tx_noheadroom: tx failed no headroom space
 * @cs_rx_nocipherctx: rx no cipher context key
 * @cs_rx_acl: rx acl
 * @cs_rx_nowds: rx 4-addr packets with no wds enabled
 * @cs_tx_nonode: tx tx failed for lack of buf
 * @cs_tx_unknown_mgt: tx unkonwn mgmt
 * @cs_tx_cipher_err: tx cipher error
 * @cs_node_timeout: node timeout
 * @cs_crypto_nomem: no memory for crypto ctx
 * @cs_crypto_tkip: tkip crypto done in s/w
 * @cs_crypto_tkipenmic: tkip en-MIC done in s/w
 * @cs_crypto_tkipcm: crypto tkip counter measures
 * @cs_crypto_ccmp: crypto ccmp done in s/w
 * @cs_crypto_wep: crypto wep done in s/w
 * @cs_crypto_setkey_cipher: crypto set key cipher
 * @cs_crypto_setkey_nokey: crypto set key no key index
 * @cs_crypto_delkey: crypto driver key delete failed
 * @cs_crypto_cipher_err: crypto cipher err
 * @cs_crypto_attach_fail: crypto attach fail
 * @cs_crypto_swfallback: crypto sw fallback
 * @cs_crypto_keyfail: crypto key fail
 * @cs_ibss_capmismatch: ibss cap mismatch
 * @cs_ps_unassoc: ps unassoc
 * @cs_ps_aid_err: ps aid err
 * @cs_padding: padding
 * @cs_invalid_macaddr_nodealloc_failcnt: invalid mac node alloc failures
 * @cs_tx_bcn_succ_cnt:tx beacon success
 * @cs_tx_bcn_outage_cnt: tx beacon outage
 * @total_num_offchan_tx_mgmt: total number of offchan TX mgmt  frames
 * @total_num_offchan_tx_data: total number of offchan TX data frames
 * @num_offchan_tx_failed: number of offchan TX frames failed
 * @sta_xceed_rlim: no of connections refused after radio limit
 * @sta_xceed_vlim: no of connections refused after vap limit
 * @mlme_auth_attempt: no of 802.11 MLME Auth Attempt
 * @mlme_auth_success: no of 802.11 MLME Auth Success
 * @authorize_attempt: no of Authorization Attempt
 * @authorize_success: no of Authorization successful
 */
struct vdev_80211_stats {
	uint64_t cs_rx_wrongbss;
	uint64_t cs_rx_wrongdir;
	uint64_t cs_rx_mcast_echo;
	uint64_t cs_rx_not_assoc;
	uint64_t cs_rx_noprivacy;
	uint64_t cs_rx_mgmt_discard;
	uint64_t cs_rx_ctl;
	uint64_t cs_rx_rs_too_big;
	uint64_t cs_rx_elem_missing;
	uint64_t cs_rx_elem_too_big;
	uint64_t cs_rx_chan_err;
	uint64_t cs_rx_node_alloc;
	uint64_t cs_rx_ssid_mismatch;
	uint64_t cs_rx_auth_unsupported;
	uint64_t cs_rx_auth_fail;
	uint64_t cs_rx_auth_countermeasures;
	uint64_t cs_rx_assoc_bss;
	uint64_t cs_rx_assoc_notauth;
	uint64_t cs_rx_assoc_cap_mismatch;
	uint64_t cs_rx_assoc_norate;
	uint64_t cs_rx_assoc_wpaie_err;
	uint64_t cs_rx_action;
	uint64_t cs_rx_auth_err;
	uint64_t cs_tx_nodefkey;
	uint64_t cs_tx_noheadroom;
	uint64_t cs_rx_acl;
	uint64_t cs_rx_nowds;
	uint64_t cs_tx_nobuf;
	uint64_t cs_tx_nonode;
	uint64_t cs_tx_cipher_err;
	uint64_t cs_tx_not_ok;
	uint64_t cs_tx_bcn_swba;
	uint64_t cs_node_timeout;
	uint64_t cs_crypto_nomem;
	uint64_t cs_crypto_tkip;
	uint64_t cs_crypto_tkipenmic;
	uint64_t cs_crypto_tkipcm;
	uint64_t cs_crypto_ccmp;
	uint64_t cs_crypto_wep;
	uint64_t cs_crypto_setkey_cipher;
	uint64_t cs_crypto_setkey_nokey;
	uint64_t cs_crypto_delkey;
	uint64_t cs_crypto_cipher_err;
	uint64_t cs_crypto_attach_fail;
	uint64_t cs_crypto_swfallback;
	uint64_t cs_crypto_keyfail;
	uint64_t cs_ibss_capmismatch;
	uint64_t cs_ps_unassoc;
	uint64_t cs_ps_aid_err;
	uint64_t cs_padding;
	uint64_t cs_tx_offchan_mgmt;
	uint64_t cs_tx_offchan_data;
	uint64_t cs_tx_offchan_fail;
	uint64_t cs_invalid_macaddr_nodealloc_fail;
	uint64_t cs_tx_bcn_success;
	uint64_t cs_tx_bcn_outage;
	uint64_t cs_sta_xceed_rlim;
	uint64_t cs_sta_xceed_vlim;
	uint64_t cs_mlme_auth_attempt;
	uint64_t cs_mlme_auth_success;
	uint64_t cs_authorize_attempt;
	uint64_t cs_authorize_success;
};

/**
 * struct vdev_80211_mac_stats - control plane 80211 mac stats at vdev
 *
 * the same statistics were earlier maintained with a reference to
 * ieee80211_mac_stats in vap structure, now the same structure will be
 * used as interface structure with user space application
 * make sure to align this structure with ieee80211_mac_stats
 *
 * @cs_rx_badkeyid: rx bad keyid
 * @cs_rx_decryptok: rx decrypt success
 * @cs_rx_wepfail: rx wep failures
 * @cs_rx_tkipreplay: rx tkip replays
 * @cs_rx_tkipformat: rx tkip format
 * @cs_rx_tkipicv: rx tkip icv
 * @cs_rx_ccmpreplay: rx ccmp replay
 * @cs_rx_ccmpformat: rx ccmp format
 * @cs_rx_ccmpmic: rx ccmp mic failures
 * @cs_rx_wpireplay: rx wpi replay
 * @cs_rx_wpimic: rx wpi mic failures
 * @cs_rx_countermeasure: rx counter measures count
 * @cs_retries: rx retries
 * @cs_tx_mgmt: tx mgmt
 * @cs_rx_mgmt: rx mgmt
 */
struct vdev_80211_mac_stats {
	uint64_t cs_rx_badkeyid;
	uint64_t cs_rx_decryptok;
	uint64_t cs_rx_wepfail;
	uint64_t cs_rx_tkipreplay;
	uint64_t cs_rx_tkipformat;
	uint64_t cs_rx_tkipicv;
	uint64_t cs_rx_ccmpreplay;
	uint64_t cs_rx_ccmpformat;
	uint64_t cs_rx_ccmpmic;
	uint64_t cs_rx_wpireplay;
	uint64_t cs_rx_wpimic;
	uint64_t cs_rx_countermeasure;
	uint64_t cs_retries;
	uint64_t cs_tx_mgmt;
	uint64_t cs_rx_mgmt;
};

/**
 * struct vdev_ic_cp_stats - control plane stats specific to WIN at vdev
 * @stats: 80211 stats
 * @ucast_stats: unicast stats
 * @mcast_stats: multicast or broadcast stats
 */
struct vdev_ic_cp_stats {
	struct vdev_80211_stats stats;
	struct vdev_80211_mac_stats ucast_stats;
	struct vdev_80211_mac_stats mcast_stats;
};

/**
 * struct peer_ic_cp_stats - control plane stats specific to WIN at peer
 * the same statistics were earlier maintained with a reference to
 * ieee80211_nodestats in ni structure, now the same structure will be
 * as interface structure with user space application
 * make sure to align this structure with ieee80211_nodestats always
 *
 *  @cs_rx_mgmt_rssi: rx mgmt rssi
 *  @cs_rx_mgmt: rx mgmt
 *  @cs_rx_noprivacy: rx no privacy
 *  @cs_rx_wepfail: rx wep failures
 *  @cs_rx_ccmpmic: rx ccmp mic failures
 *  @cs_rx_wpimic: rx wpi mic failures
 *  @cs_rx_tkipicv: rx tkip icv
 *  @cs_tx_mgmt: tx mgmt
 *  @cs_is_tx_not_ok: tx failures
 *  @cs_ps_discard: ps discard
 *  @cs_rx_mgmt_rate: rx mgmt rate
 *  @cs_tx_bytes_rate: tx rate
 *  @cs_tx_data_rate: tx data rate
 *  @cs_rx_bytes_rate: rx rate
 *  @cs_rx_data_rate: rx data rate
 *  @cs_tx_bytes_success_last: tx success count in last 1 sec
 *  @cs_tx_data_success_last: tx data success count in last 1 sec
 *  @cs_rx_bytes_last: rx rate
 *  @cs_rx_data_last: rx data rate
 *  @cs_psq_drops: psq drops
 *  @cs_tx_dropblock: tx dropblock
 *  @cs_tx_assoc: tx assoc success
 *  @cs_tx_assoc_fail: tx assoc failure
 */
struct peer_ic_cp_stats {
	int8_t   cs_rx_mgmt_rssi;
	uint32_t cs_rx_mgmt;
	uint32_t cs_rx_noprivacy;
	uint32_t cs_rx_wepfail;
	uint32_t cs_rx_ccmpmic;
	uint32_t cs_rx_wpimic;
	uint32_t cs_rx_tkipicv;
	uint32_t cs_tx_mgmt;
	uint32_t cs_is_tx_not_ok;
	uint32_t cs_ps_discard;
	uint32_t cs_rx_mgmt_rate;
#ifdef WLAN_ATH_SUPPORT_EXT_STAT
	uint32_t cs_tx_bytes_rate;
	uint32_t cs_tx_data_rate;
	uint32_t cs_rx_bytes_rate;
	uint32_t cs_rx_data_rate;
	uint32_t cs_tx_bytes_success_last;
	uint32_t cs_tx_data_success_last;
	uint32_t cs_rx_bytes_last;
	uint32_t cs_rx_data_last;
#endif
	uint32_t cs_psq_drops;
#ifdef ATH_SUPPORT_IQUE
	uint32_t cs_tx_dropblock;
#endif
	uint32_t cs_tx_assoc;
	uint32_t cs_tx_assoc_fail;
};

#endif /* QCA_SUPPORT_CP_STATS */
#endif /* __WLAN_CP_STATS_IC_DEFS_H__ */
