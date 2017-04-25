/*
 * Copyright (c) 2017 The Linux Foundation. All rights reserved.
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
 * DOC: contains scan structure definations
 */

#ifndef _WLAN_SCAN_STRUCTS_H_
#define _WLAN_SCAN_STRUCTS_H_
#include <wlan_cmn.h>
#include <qdf_time.h>
#include <qdf_list.h>
#include <qdf_atomic.h>
#include <wlan_cmn_ieee80211.h>

typedef uint16_t wlan_scan_requester;
typedef uint32_t wlan_scan_id;

#define WLAN_SCAN_MAX_NUM_SSID          10
#define WLAN_SCAN_MAX_NUM_BSSID         10
#define WLAN_SCAN_MAX_NUM_CHANNELS      40

#define SCM_PCL_ADVANTAGE       30
#define SCM_PCL_RSSI_THRESHOLD -75

#define SCM_BSS_CAP_VALUE_NONE  0/* not much value */
#define SCM_BSS_CAP_VALUE_HT    1
#define SCM_BSS_CAP_VALUE_VHT   2
#define SCM_BSS_CAP_VALUE_HE    3
#define SCM_BSS_CAP_VALUE_WMM   1
#define SCM_BSS_CAP_VALUE_UAPSD 1
#define SCM_BSS_CAP_VALUE_5GHZ  2

#define SCM_CANCEL_SCAN_WAIT_TIME 50
#define SCM_CANCEL_SCAN_WAIT_ITERATION 100

#define BURST_SCAN_MAX_NUM_OFFCHANNELS 3
#define P2P_SCAN_MAX_BURST_DURATION 180
/* Increase dwell time for P2P search in ms */
#define P2P_SEARCH_DWELL_TIME_INC 20

/* forward declaration */
struct wlan_objmgr_vdev;
struct wlan_objmgr_pdev;
struct wlan_objmgr_psoc;

/**
 * struct channel_info - BSS channel information
 * @chan_idx: current operating channel index
 * @cfreq0: channel frequency index0
 * @cfreq1: channel frequency index1
 * @priv: channel private information
 */
struct channel_info {
	uint8_t chan_idx;
	uint8_t cfreq0;
	uint8_t cfreq1;
	void *priv;
};

/**
 * struct element_info - defines length of a memory block and memory block
 * @len: length of memory block
 * @ptr: memory block pointer
 */
struct element_info {
	uint32_t len;
	uint8_t *ptr;
};

/**
 * struct ie_list - pointers to various IEs
 * @tim:        pointer to tim ie
 * @country:    pointer to country ie
 * @ssid:       pointer to ssid ie
 * @rates:      pointer to supported rates ie
 * @xrates:     pointer to extended supported rate ie
 * @csa:        pointer to csa ie
 * @xcsa:       pointer to extended csa ie
 * @wpa:        pointer to wpa ie
 * @wcn:        pointer to wcn ie
 * @rsn:        pointer to rsn ie
 * @wps:        pointer to wps ie
 * @wmeinfo:    pointer to wmeinfo ie
 * @wmeparam:   pointer to wmeparam ie
 * @quiet:      pointer to quiet ie
 * @htcap:      pointer to htcap ie
 * @htinfo:     pointer to htinfo ie
 * @athcaps:    pointer to athcaps ie
 * @athextcaps: pointer to extended athcaps ie
 * @sfa:        pointer to sfa ie
 * @vendor:     pointer to vendor ie
 * @qbssload:   pointer to qbssload ie
 * @wapi:       pointer to wapi ie
 * @p2p:        pointer to p2p ie
 * @alt_wcn:    pointer to alternate wcn ie
 * @extcaps:    pointer to extended caps ie
 * @ibssdfs:    pointer to ibssdfs ie
 * @sonadv:     pointer to wifi son ie
 * @vhtcap:     pointer to vhtcap ie
 * @vhtop:      pointer to vhtop ie
 * @opmode:     pointer to opmode ie
 * @cswrp:      pointer to channel switch announcement wrapper ie
 * @widebw:     pointer to wide band channel switch sub ie
 * @txpwrenvlp: pointer to tx power envelop sub ie
 * @srp: pointer to spatial reuse parameter sub extended ie
 */
struct ie_list {
	uint8_t *tim;
	uint8_t *country;
	uint8_t *ssid;
	uint8_t *rates;
	uint8_t *xrates;
	uint8_t *csa;
	uint8_t *xcsa;
	uint8_t *wpa;
	uint8_t *wcn;
	uint8_t *rsn;
	uint8_t *wps;
	uint8_t *wmeinfo;
	uint8_t *wmeparam;
	uint8_t *quiet;
	uint8_t *htcap;
	uint8_t *htinfo;
	uint8_t *athcaps;
	uint8_t *athextcaps;
	uint8_t *sfa;
	uint8_t *vendor;
	uint8_t *qbssload;
	uint8_t *wapi;
	uint8_t *p2p;
	uint8_t *alt_wcn;
	uint8_t *extcaps;
	uint8_t *ibssdfs;
	uint8_t *sonadv;
	uint8_t *vhtcap;
	uint8_t *vhtop;
	uint8_t *opmode;
	uint8_t *cswrp;
	uint8_t *widebw;
	uint8_t *txpwrenvlp;
	uint8_t *bwnss_map;
	uint8_t *secchanoff;
	uint8_t *mdie;
	uint8_t *hecap;
	uint8_t *heop;
	uint8_t *srp;
};

/**
 * struct mlme_info - mlme specific info
 * temporarily maintained in scan cache for backward compatibility.
 * must be removed as part of umac convergence.
 * @bad_ap_time: time when this ap was marked bad
 * @status: status
 * @rank: rank
 * @utility: utility
 * @assoc_state: association state
 * @chanload: channel load
 */
struct mlme_info {
	qdf_time_t bad_ap_time;
	uint32_t status;
	uint32_t rank;
	uint32_t utility;
	uint32_t assoc_state;
	uint32_t chanload;
};

/**
 * struct bss_info - information required to uniquely define a bss
 * @chan: bss operating primary channel index
 * @ssid: ssid of bss
 * @bssid: bssid of bss
 */
struct bss_info {
	uint8_t chan;
	struct wlan_ssid ssid;
	struct qdf_mac_addr bssid;
};

/**
 * struct scan_cache_node - Scan cache entry node
 * @node: node pointers
 * @ref_cnt: ref count if in use
 * @entry: scan entry pointer
 */
struct scan_cache_node {
	qdf_list_node_t node;
	qdf_atomic_t ref_cnt;
	struct scan_cache_entry *entry;
};

struct security_info {
	enum wlan_enc_type uc_enc;
	enum wlan_enc_type mc_enc;
	enum wlan_auth_type auth_type;
};

/**
 * struct scan_cache_entry: structure containing scan entry
 * @frm_subtype: updated from beacon/probe
 * @bssid: bssid
 * @mac_addr: mac address
 * @ssid: ssid
 * @seq_num: sequence number
 * @phy_mode: Phy mode of the AP
 * @avg_rssi: Average RSSI fof the AP
 * @rssi_raw: The rssi of the last beacon/probe received
 * @bcn_int: Beacon interval of the AP
 * @cap_info: Capability of the AP
 * @tsf_info: TSF info
 * @erp: erp info
 * @dtim_period: dtime period
 * @is_p2p_ssid: is P2P entry
 * @scan_entry_time: boottime in microsec when last beacon/probe is received
 * @rssi_timestamp: boottime in microsec when RSSI was updated
 * @hidden_ssid_timestamp: boottime in microsec when hidden
 *                         ssid was received
 * @channel: channel info on which AP is present
 * @channel_mismatch: if channel received in metadata
 *                    doesnot match the one in beacon
 * @tsf_delta: TSF delta
 * @prefer_value: Preffer value calulated for the AP
 * @cap_value: Capability value calculated for the AP
 * @neg_sec_info: negotiated security info
 * @rrm_parent_tsf: RRM parent tsf
 * @mlme_info: Mlme info, this will be updated by MLME for the scan entry
 * @alt_wcn_ie: alternate WCN IE
 * @ie_list: IE list pointers
 * @raw_frame: contain raw frame and the length of the raw frame
 */
struct scan_cache_entry {
	uint8_t frm_subtype;
	struct qdf_mac_addr bssid;
	struct qdf_mac_addr mac_addr;
	struct wlan_ssid ssid;
	uint16_t seq_num;
	enum wlan_phymode phy_mode;
	int32_t avg_rssi;
	int8_t rssi_raw;
	uint16_t bcn_int;
	union wlan_capability cap_info;
	union {
		uint8_t data[8];
		uint64_t tsf;
	} tsf_info;
	uint8_t erp;
	uint8_t dtim_period;
	bool is_p2p;
	qdf_time_t scan_entry_time;
	qdf_time_t rssi_timestamp;
	qdf_time_t hidden_ssid_timestamp;
	struct channel_info channel;
	bool channel_mismatch;
	struct mlme_info mlme_info;
	uint32_t tsf_delta;
	uint32_t prefer_value;
	uint32_t cap_val;
	struct security_info neg_sec_info;
	uint32_t rrm_parent_tsf;
	struct element_info alt_wcn_ie;
	struct ie_list ie_list;
	struct element_info raw_frame;
};

#define MAX_FAVORED_BSSID 16
#define MAX_AVOID_LIST_BSSID 16
#define MAX_ALLOWED_SSID_LIST 4

/**
 * struct roam_filter_params - Structure holding roaming parameters
 * @num_bssid_avoid_list:       The number of BSSID's that we should
 *                              avoid connecting to. It is like a
 *                              blacklist of BSSID's.
 *                              also for roaming apart from the connected one's
 * @num_bssid_favored:          Number of BSSID's which have a preference over
 *                              others
 * @raise_rssi_thresh_5g:       The RSSI threshold below which the
 *                              raise_factor_5g (boost factor) should be
 *                              applied.
 * @drop_rssi_thresh_5g:        The RSSI threshold beyond which the
 *                              drop_factor_5g (penalty factor) should be
 *                              applied
 * @raise_factor_5g:            Boost factor
 * @drop_factor_5g:             Penalty factor
 * @max_raise_rssi_5g:          Maximum amount of Boost that can added
 * @max_drop_rssi_5g:           Maximum amount of penalty that can be subtracted
 * @is_5g_pref_enabled:         5GHz BSSID preference feature enable/disable.
 * @bssid_avoid_list:           Blacklist SSID's
 * @bssid_favored:              Favorable BSSID's
 * @bssid_favored_factor:       RSSI to be added to this BSSID to prefer it
 *
 * This structure holds all the key parameters related to
 * initial connection and also roaming connections.
 */
struct roam_filter_params {
	uint32_t num_bssid_avoid_list;
	uint32_t num_bssid_favored;
	int raise_rssi_thresh_5g;
	int drop_rssi_thresh_5g;
	uint32_t raise_factor_5g;
	uint32_t drop_factor_5g;
	int max_raise_rssi_5g;
	int max_drop_rssi_5g;
	uint32_t is_5g_pref_enabled;
	/* Variable params list */
	struct qdf_mac_addr bssid_avoid_list[MAX_AVOID_LIST_BSSID];
	struct qdf_mac_addr bssid_favored[MAX_FAVORED_BSSID];
	uint8_t bssid_favored_factor[MAX_FAVORED_BSSID];
};

#define WLAN_SCAN_FILTER_NUM_SSID 5
#define WLAN_SCAN_FILTER_NUM_BSSID 5

/**
 * @age_threshold: If set return entry which are newer than the age_threshold
 * @p2p_results: If only p2p entries is required
 * @rrm_measurement_filter: For measurement reports.if set, only SSID, BSSID
 *                          and channel is considered for filtering.
 * @num_of_bssid: number of bssid passed
 * @num_of_ssid: number of ssid
 * @num_of_channels: number of  channels
 * @num_of_auth: number of auth types
 * @num_of_enc_type: number of unicast enc type
 * @num_of_mc_enc_type: number of multicast enc type
 * @pmf_cap: Pmf capability
 * @num_of_pcl_channels: number of pcl channels
 * @bss_type: bss type BSS/IBSS etc
 * @dot11_mode: operating modes 0 mean any
 *              11a , 11g, 11n , 11ac , 11b etc
 * @band: to get specific band 2.4G, 5G or 4.9 G
 * @rssi_threshold: AP having RSSI greater than
 *                  rssi threasholed (ignored if set 0)
 * @only_wmm_ap: If only Qos AP is needed
 * @ignore_auth_enc_type: Ignore enc type if
 *                        this is set (For WPS/OSEN connection)
 * @mobility_domain: Mobility domain for 11r
 * @country[3]: Ap with specific country code
 * @bssid_list: bssid list
 * @ssid_list: ssid list
 * @channel_list: channel list
 * @auth_type: auth type list
 * @enc_type: unicast enc type list
 * @mc_enc_type: multicast cast enc type list
 * @pcl_channel_list: PCL channel list
 */
struct scan_filter {
	uint32_t age_threshold;
	uint32_t p2p_results;
	uint32_t rrm_measurement_filter;
	uint32_t num_of_bssid;
	uint32_t num_of_ssid;
	uint32_t num_of_channels;
	uint32_t num_of_auth;
	uint32_t num_of_enc_type;
	uint32_t num_of_mc_enc_type;
	enum wlan_pmf_cap pmf_cap;
	uint32_t num_of_pcl_channels;
	enum wlan_bss_type bss_type;
	enum wlan_phymode dot11_mode;
	enum wlan_band band;
	uint32_t rssi_threshold;
	uint32_t only_wmm_ap;
	uint32_t ignore_auth_enc_type;
	uint32_t mobility_domain;
	/* Variable params list */
	uint8_t country[3];
	struct qdf_mac_addr bssid_list[WLAN_SCAN_FILTER_NUM_BSSID];
	struct wlan_ssid ssid_list[WLAN_SCAN_FILTER_NUM_SSID];
	uint8_t channel_list[QDF_MAX_NUM_CHAN];
	enum wlan_auth_type auth_type[WLAN_NUM_OF_SUPPORT_AUTH_TYPE];
	enum wlan_enc_type enc_type[WLAN_NUM_OF_ENCRYPT_TYPE];
	enum wlan_enc_type mc_enc_type[WLAN_NUM_OF_ENCRYPT_TYPE];
	uint8_t pcl_channel_list[QDF_MAX_NUM_CHAN];
};


/**
 * enum scan_priority - scan priority definitions
 * @SCAN_PRIORITY_VERY_LOW: very low priority
 * @SCAN_PRIORITY_LOW: low scan priority
 * @SCAN_PRIORITY_MEDIUM: medium priority
 * @SCAN_PRIORITY_HIGH: high priority
 * @SCAN_PRIORITY_VERY_HIGH: very high priority
 * @SCAN_PRIORITY_COUNT: number of priorities supported
 */
enum scan_priority {
	SCAN_PRIORITY_VERY_LOW,
	SCAN_PRIORITY_LOW,
	SCAN_PRIORITY_MEDIUM,
	SCAN_PRIORITY_HIGH,
	SCAN_PRIORITY_VERY_HIGH,
	SCAN_PRIORITY_COUNT,
};


/**
 * enum scan_type - type of scan
 * @SCAN_TYPE_BACKGROUND: background scan
 * @SCAN_TYPE_FOREGROUND: foregrounc scan
 * @SCAN_TYPE_SPECTRAL: spectral scan
 * @SCAN_TYPE_REPEATER_BACKGROUND: background scan in repeater
 * @SCAN_TYPE_REPEATER_EXT_BACKGROUND: background scan in extended repeater
 * @SCAN_TYPE_RADIO_MEASUREMENTS: redio measurement
 * @SCAN_TYPE_COUNT: number of scan types supported
 */
enum scan_type {
	SCAN_TYPE_BACKGROUND,
	SCAN_TYPE_FOREGROUND,
	SCAN_TYPE_SPECTRAL,
	SCAN_TYPE_REPEATER_BACKGROUND,
	SCAN_TYPE_REPEATER_EXT_BACKGROUND,
	SCAN_TYPE_RADIO_MEASUREMENTS,
	SCAN_TYPE_COUNT,
};

/**
 * struct scan_extra_params_legacy
 * extra parameters required for legacy DA scan module
 * @scan_type: type of scan
 * @min_dwell_active: min active dwell time
 * @min_dwell_passive: min passive dwell time
 * @init_rest_time: init rest time for enhanced independent repeater
 */
struct scan_extra_params_legacy {
	enum scan_type scan_type;
	uint32_t min_dwell_active;
	uint32_t min_dwell_passive;
	uint32_t init_rest_time;
};

/**
 * enum scan_dwelltime_adaptive_mode: dwelltime_mode
 * @SCAN_DWELL_MODE_DEFAULT: Use firmware default mode
 * @SCAN_DWELL_MODE_CONSERVATIVE: Conservative adaptive mode
 * @SCAN_DWELL_MODE_MODERATE: Moderate adaptive mode
 * @SCAN_DWELL_MODE_AGGRESSIVE: Aggressive adaptive mode
 * @SCAN_DWELL_MODE_STATIC: static adaptive mode
 */
enum scan_dwelltime_adaptive_mode {
	SCAN_DWELL_MODE_DEFAULT = 0,
	SCAN_DWELL_MODE_CONSERVATIVE = 1,
	SCAN_DWELL_MODE_MODERATE = 2,
	SCAN_DWELL_MODE_AGGRESSIVE = 3,
	SCAN_DWELL_MODE_STATIC = 4
};

/**
 * struct scan_req_params - start scan request parameter
 * @scan_id: scan id
 * @scan_req_id: scan requester id
 * @vdev_id: vdev id where scan was originated
 * @pdev_id: pdev id of parent pdev
 * @scan_priority: scan priority
 * @scan_ev_started: notify scan started event
 * @scan_ev_completed: notify scan completed event
 * @scan_ev_bss_chan: notify bss chan event
 * @scan_ev_foreign_chan: notify foreign chan event
 * @scan_ev_dequeued: notify scan request dequed event
 * @scan_ev_preempted: notify scan preempted event
 * @scan_ev_start_failed: notify scan start failed event
 * @scan_ev_restarted: notify scan restarted event
 * @scan_ev_foreign_chn_exit: notify foreign chan exit event
 * @scan_ev_invalid: notify invalid scan request event
 * @scan_ev_gpio_timeout: notify gpio timeout event
 * @scan_ev_suspended: notify scan suspend event
 * @scan_ev_resumed: notify scan resumed event
 * @scan_events: variable to read and set scan_ev_* flags in one shot
 *               can be used to dump all scan_ev_* flags for debug
 * @dwell_time_active: active dwell time
 * @dwell_time_passive: passive dwell time
 * @min_rest_time: min rest time
 * @max_rest_time: max rest time
 * @repeat_probe_time: repeat probe time
 * @probe_spacing_time: probe spacing time
 * @idle_time: idle time
 * @max_scan_time: max scan time
 * @probe_delay: probe delay
 * @scan_f_passive: passively scan all channels including active channels
 * @scan_f_bcast_probe: add wild card ssid prbreq even if ssid_list is specified
 * @scan_f_cck_rates: add cck rates to rates/xrates ie in prb req
 * @scan_f_ofdm_rates: add ofdm rates to rates/xrates ie in prb req
 * @scan_f_chan_stat_evnt: enable indication of chan load and noise floor
 * @scan_f_filter_prb_req: filter Probe request frames
 * @scan_f_bypass_dfs_chn: when set, do not scan DFS channels
 * @scan_f_continue_on_err:continue scan even if few certain erros have occurred
 * @scan_f_offchan_mgmt_tx: allow mgmt transmission during off channel scan
 * @scan_f_offchan_data_tx: allow data transmission during off channel scan
 * @scan_f_promisc_mode: scan with promiscuous mode
 * @scan_f_capture_phy_err: enable capture ppdu with phy errrors
 * @scan_f_strict_passive_pch: do passive scan on passive channels
 * @scan_f_half_rate: enable HALF (10MHz) rate support
 * @scan_f_quarter_rate: set Quarter (5MHz) rate support
 * @scan_f_force_active_dfs_chn: allow to send probe req on DFS channel
 * @scan_f_add_tpc_ie_in_probe: add TPC ie in probe req frame
 * @scan_f_add_ds_ie_in_probe: add DS ie in probe req frame
 * @scan_f_add_spoofed_mac_in_probe: use random mac address for TA in probe
 * @scan_f_add_rand_seq_in_probe: use random sequence number in probe
 * @scan_f_en_ie_whitelist_in_probe: enable ie whitelist in probe
 * @scan_f_forced: force scan even in presence of data traffic
 * @scan_f_2ghz: scan 2.4 GHz channels
 * @scan_f_5ghz: scan 5 GHz channels
 * @scan_f_80mhz: scan in 80 MHz channel width mode
 * @scan_flags: variable to read and set scan_f_* flags in one shot
 *              can be used to dump all scan_f_* flags for debug
 * @burst_duration: burst duration
 * @num_chan: no of channel
 * @num_bssid: no of bssid
 * @num_ssids: no of ssid
 * @n_probes: no of probe
 * @chan_list: channel list
 * @ssid: ssid list
 * @bssid_list: Lisst of bssid to scan
 * @extraie: list of optional/vendor specific ie's to be added in probe requests
 * @htcap: htcap ie
 * @vhtcap: vhtcap ie
 */

struct scan_req_params {
	uint32_t scan_id;
	uint32_t scan_req_id;
	uint32_t vdev_id;
	uint32_t pdev_id;
	enum scan_priority scan_priority;
	union {
		struct {
			uint32_t scan_ev_started:1,
				 scan_ev_completed:1,
				 scan_ev_bss_chan:1,
				 scan_ev_foreign_chan:1,
				 scan_ev_dequeued:1,
				 scan_ev_preempted:1,
				 scan_ev_start_failed:1,
				 scan_ev_restarted:1,
				 scan_ev_foreign_chn_exit:1,
				 scan_ev_invalid:1,
				 scan_ev_gpio_timeout:1,
				 scan_ev_suspended:1,
				 scan_ev_resumed:1;
		};
		uint32_t scan_events;
	};
	uint32_t dwell_time_active;
	uint32_t dwell_time_passive;
	uint32_t min_rest_time;
	uint32_t max_rest_time;
	uint32_t repeat_probe_time;
	uint32_t probe_spacing_time;
	uint32_t idle_time;
	uint32_t max_scan_time;
	uint32_t probe_delay;
	union {
		struct {
			uint32_t scan_f_passive:1,
				 scan_f_bcast_probe:1,
				 scan_f_cck_rates:1,
				 scan_f_ofdm_rates:1,
				 scan_f_chan_stat_evnt:1,
				 scan_f_filter_prb_req:1,
				 scan_f_bypass_dfs_chn:1,
				 scan_f_continue_on_err:1,
				 scan_f_offchan_mgmt_tx:1,
				 scan_f_offchan_data_tx:1,
				 scan_f_promisc_mode:1,
				 scan_f_capture_phy_err:1,
				 scan_f_strict_passive_pch:1,
				 scan_f_half_rate:1,
				 scan_f_quarter_rate:1,
				 scan_f_force_active_dfs_chn:1,
				 scan_f_add_tpc_ie_in_probe:1,
				 scan_f_add_ds_ie_in_probe:1,
				 scan_f_add_spoofed_mac_in_probe:1,
				 scan_f_add_rand_seq_in_probe:1,
				 scan_f_en_ie_whitelist_in_probe:1,
				 scan_f_forced:1,
				 scan_f_2ghz:1,
				 scan_f_5ghz:1,
				 scan_f_80mhz:1;
		};
		uint32_t scan_flags;
	};
	enum scan_dwelltime_adaptive_mode adaptive_dwell_time_mode;
	uint32_t burst_duration;
	uint32_t num_chan;
	uint32_t num_bssid;
	uint32_t num_ssids;
	uint32_t n_probes;
	uint32_t chan_list[WLAN_SCAN_MAX_NUM_CHANNELS];
	struct wlan_ssid ssid[WLAN_SCAN_MAX_NUM_SSID];
	struct qdf_mac_addr bssid_list[WLAN_SCAN_MAX_NUM_BSSID];
	struct element_info extraie;
	struct element_info htcap;
	struct element_info vhtcap;
};

/**
 * struct scan_start_request - scan request config
 * @vdev: vdev
 * @legacy_params: extra parameters required for legacy DA arch
 * @scan_req: common scan start request parameters
 */
struct scan_start_request {
	struct wlan_objmgr_vdev *vdev;
	struct scan_extra_params_legacy legacy_params;
	struct scan_req_params scan_req;
};

/**
 * enum scan_cancel_type - type specifiers for cancel scan request
 * @WLAN_SCAN_CANCEL_SINGLE: cancel particular scan specified by scan_id
 * @WLAN_SCAN_CANCEL_VAP_ALL: cancel all scans running on a particular vdevid
 * WLAN_SCAN_CANCEL_PDEV_ALL: cancel all scans running on parent pdev of vdevid
 */
enum scan_cancel_req_type {
	WLAN_SCAN_CANCEL_SINGLE = 1,
	WLAN_SCAN_CANCEL_VDEV_ALL,
	WLAN_SCAN_CANCEL_PDEV_ALL,
};

/**
 * struct scan_cancel_param - stop scan cmd parameter
 * @requester: scan requester
 * @scan_id: scan id
 * @req_type: scan request type
 * @vdev_id: vdev id
 * @pdev_id: pdev id of parent pdev
 */
struct scan_cancel_param {
	uint32_t requester;
	uint32_t scan_id;
	enum scan_cancel_req_type req_type;
	uint32_t vdev_id;
	uint32_t pdev_id;
};

/**
 * struct scan_cancel_request - stop scan cmd
 * @vdev: vdev object
 * @cancel_req: stop scan cmd parameter
 */
struct scan_cancel_request {
	/* Extra parameters consumed by scan module or serialization */
	struct wlan_objmgr_vdev *vdev;
	/* Actual scan cancel request parameters */
	struct scan_cancel_param cancel_req;
};

/**
 * struct mlme_update_info - meta information required to
 * update mlme info in scan entry
 * @vdev: vdev object
 * @bss: bss identifier
 * @mlme_info: mlme info to update
 */
struct mlme_update_info {
	struct wlan_objmgr_vdev *vdev;
	struct bss_info bss;
	struct mlme_info mlme_info;
};

/**
 * enum scan_event_type - scan event types
 * @SCAN_EVENT_TYPE_STARTED: scan started
 * @SCAN_EVENT_TYPE_COMPLETED: scan completed
 * @SCAN_EVENT_TYPE_BSS_CHANNEL: HW came back to home channel
 * @SCAN_EVENT_TYPE_FOREIGN_CHANNEL: HW moved to foreign channel
 * @SCAN_EVENT_TYPE_DEQUEUED: scan request dequeued
 * @SCAN_EVENT_TYPE_PREEMPTED: scan got preempted
 * @SCAN_EVENT_TYPE_START_FAILED: couldn't start scan
 * @SCAN_EVENT_TYPE_RESTARTED: scan restarted
 * @SCAN_EVENT_TYPE_FOREIGN_CHANNEL_EXIT: HW exited foreign channel
 * @SCAN_EVENT_TYPE_SUSPENDED: scan got suspended
 * @SCAN_EVENT_TYPE_RESUMED: scan resumed
 * @SCAN_EVENT_TYPE_NLO_COMPLETE: NLO completed
 * @SCAN_EVENT_TYPE_NLO_MATCH: NLO match event
 * @SCAN_EVENT_TYPE_INVALID: invalid request
 * @SCAN_EVENT_TYPE_GPIO_TIMEOUT: gpio timeout
 * @SCAN_EVENT_TYPE_RADIO_MEASUREMENT_START: radio measurement start
 * @SCAN_EVENT_TYPE_RADIO_MEASUREMENT_END: radio measurement end
 * @SCAN_EVENT_TYPE_BSSID_MATCH: bssid match found
 * @SCAN_EVENT_TYPE_FOREIGN_CHANNEL_GET_NF: foreign channel noise floor
 * @SCAN_EVENT_TYPE_MAX: marker for invalid event
 */
enum scan_event_type {
	SCAN_EVENT_TYPE_STARTED,
	SCAN_EVENT_TYPE_COMPLETED,
	SCAN_EVENT_TYPE_BSS_CHANNEL,
	SCAN_EVENT_TYPE_FOREIGN_CHANNEL,
	SCAN_EVENT_TYPE_DEQUEUED,
	SCAN_EVENT_TYPE_PREEMPTED,
	SCAN_EVENT_TYPE_START_FAILED,
	SCAN_EVENT_TYPE_RESTARTED,
	SCAN_EVENT_TYPE_FOREIGN_CHANNEL_EXIT,
	SCAN_EVENT_TYPE_SUSPENDED,
	SCAN_EVENT_TYPE_RESUMED,
	SCAN_EVENT_TYPE_NLO_COMPLETE,
	SCAN_EVENT_TYPE_NLO_MATCH,
	SCAN_EVENT_TYPE_INVALID,
	SCAN_EVENT_TYPE_GPIO_TIMEOUT,
	SCAN_EVENT_TYPE_RADIO_MEASUREMENT_START,
	SCAN_EVENT_TYPE_RADIO_MEASUREMENT_END,
	SCAN_EVENT_TYPE_BSSID_MATCH,
	SCAN_EVENT_TYPE_FOREIGN_CHANNEL_GET_NF,
	SCAN_EVENT_TYPE_MAX,
};

/**
 * enum scan_completion_reason - scan completion reason
 * @SCAN_REASON_NONE: un specified reason
 * @SCAN_REASON_COMPLETED: scan successfully completed
 * @SCAN_REASON_CANCELLED: scan got cancelled
 * @SCAN_REASON_PREEMPTED: scan got preempted
 * @SCAN_REASON_TIMEDOUT: couldnt complete within specified time
 * @SCAN_REASON_INTERNAL_FAILURE: cancelled because of some failure
 * @SCAN_REASON_SUSPENDED: scan suspended
 * @SCAN_REASON_RUN_FAILED: run failed
 * @SCAN_REASON_TERMINATION_FUNCTION: termination function
 * @SCAN_REASON_MAX_OFFCHAN_RETRIES: max retries exceeded thresold
 * @SCAN_REASON_MAX: invalid completion reason marker
 */
enum scan_completion_reason {
	SCAN_REASON_NONE,
	SCAN_REASON_COMPLETED,
	SCAN_REASON_CANCELLED,
	SCAN_REASON_PREEMPTED,
	SCAN_REASON_TIMEDOUT,
	SCAN_REASON_INTERNAL_FAILURE,
	SCAN_REASON_SUSPENDED,
	SCAN_REASON_RUN_FAILED,
	SCAN_REASON_TERMINATION_FUNCTION,
	SCAN_REASON_MAX_OFFCHAN_RETRIES,
	SCAN_REASON_MAX,
};


/**
 * struct scan_event - scan event definition
 * @vdev_id: vdev where scan was run
 * @type: type of scan event
 * @reason: completion reason
 * @chan_freq: channel centre frequency
 * @requester: requester id
 * @scan_id: scan id
 */
struct scan_event {
	uint32_t vdev_id;
	enum scan_event_type type;
	enum scan_completion_reason reason;
	uint32_t chan_freq;
	uint32_t requester;
	uint32_t scan_id;
};

/**
 * struct scan_event_info - scan event information
 * @vdev: vdev object
 * @event: scan event
 */
struct scan_event_info {
	struct wlan_objmgr_vdev *vdev;
	struct scan_event event;
};

/**
 * enum scm_scan_status - scan status
 * @SCAN_NOT_IN_PROGRESS: Neither active nor pending scan in progress
 * @SCAN_IS_ACTIVE: scan request is present only in active list
 * @SCAN_IS_PENDING: scan request is present only in pending list
 * @SCAN_IS_ACTIVE_AND_PENDING: scan request is present in active
 *                               and pending both lists
 */
enum scm_scan_status {
	SCAN_NOT_IN_PROGRESS = 0, /* Must be 0 */
	SCAN_IS_ACTIVE,
	SCAN_IS_PENDING,
	SCAN_IS_ACTIVE_AND_PENDING,
};

/**
 * scan_event_handler() - function prototype of scan event handlers
 * @vdev: vdev object
 * @event: scan event
 * @arg: argument
 *
 * PROTO TYPE, scan event handler call back function prototype
 *
 * @Return: void
 */
typedef void (*scan_event_handler) (struct wlan_objmgr_vdev *vdev,
	struct scan_event *event, void *arg);

/**
 * enum scan_cb_type - update beacon cb type
 * @SCAN_CB_TYPE_INFORM_BCN: Calback to indicate beacon to OS
 * @SCAN_CB_TYPE_UPDATE_BCN: Calback to indicate beacon
 *                    to MLME and update MLME info
 *
 */
enum scan_cb_type {
	SCAN_CB_TYPE_INFORM_BCN,
	SCAN_CB_TYPE_UPDATE_BCN,
};

/* Set PNO */
#define SCAN_PNO_MAX_PLAN_REQUEST   2
#define SCAN_PNO_MAX_NETW_CHANNELS_EX  60
#define SCAN_PNO_MAX_SUPP_NETWORKS  16
#define SCAN_PNO_DEF_SLOW_SCAN_MULTIPLIER 6
#define SCAN_PNO_DEF_SCAN_TIMER_REPEAT 20
#define SCAN_PNO_MATCH_WAKE_LOCK_TIMEOUT         (5 * 1000)     /* in msec */
#ifdef CONFIG_SLUB_DEBUG_ON
#define SCAN_PNO_SCAN_COMPLETE_WAKE_LOCK_TIMEOUT (2 * 1000)     /* in msec */
#else
#define SCAN_PNO_SCAN_COMPLETE_WAKE_LOCK_TIMEOUT (1 * 1000)     /* in msec */
#endif /* CONFIG_SLUB_DEBUG_ON */

#define SCAN_PNO_CHANNEL_PREDICTION 0
#define SCAN_TOP_K_NUM_OF_CHANNELS 3
#define SCAN_STATIONARY_THRESHOLD 10
#define SCAN_CHANNEL_PREDICTION_FULL_SCAN_MS 60000
#define SCAN_ADAPTIVE_PNOSCAN_DWELL_MODE 0

/**
 * enum ssid_bc_type - SSID broadcast type
 * @SSID_BC_TYPE_UNKNOWN: Broadcast unknown
 * @SSID_BC_TYPE_NORMAL: Broadcast normal
 * @SSID_BC_TYPE_HIDDEN: Broadcast hidden
 */
enum ssid_bc_type {
	SSID_BC_TYPE_UNKNOWN = 0,
	SSID_BC_TYPE_NORMAL = 1,
	SSID_BC_TYPE_HIDDEN = 2,
};

/**
 * struct pno_nw_type - pno nw type
 * @ssid: ssid
 * @authentication: authentication type
 * @encryption: encryption type
 * @bcastNetwType: broadcast nw type
 * @ucChannelCount: uc channel count
 * @aChannels: pno channel
 * @rssiThreshold: rssi threshold
 */
struct pno_nw_type {
	struct wlan_ssid ssid;
	uint32_t authentication;
	uint32_t encryption;
	uint32_t bc_new_type;
	uint8_t channel_cnt;
	uint32_t channels[SCAN_PNO_MAX_NETW_CHANNELS_EX];
	int32_t rssi_thresh;
};

/**
 * struct pno_scan_req_params - PNO Scan request structure
 * @networks_cnt: Number of networks
 * @vdev_id: vdev id
 * @fast_scan_period: Fast Scan period
 * @slow_scan_period: Slow scan period
 * @delay_start_time: delay in seconds to use before starting the first scan
 * @fast_scan_max_cycles: Fast scan max cycles
 * @pno_channel_prediction: PNO channel prediction feature status
 * @uint32_t active_dwell_time: active dwell time
 * @uint32_t passive_dwell_time: passive dwell time
 * @top_k_num_of_channels: top K number of channels are used for tanimoto
 * distance calculation.
 * @stationary_thresh: threshold value to determine that the STA is stationary.
 * @adaptive_dwell_mode: adaptive dwelltime mode for pno scan
 * @channel_prediction_full_scan: periodic timer upon which a full scan needs
 * to be triggered.
 * @networks_list: Preferred network list
 */
struct pno_scan_req_params {
	uint32_t networks_cnt;
	uint32_t vdev_id;
	uint32_t fast_scan_period;
	uint32_t slow_scan_period;
	uint32_t delay_start_time;
	uint32_t fast_scan_max_cycles;
	uint32_t active_dwell_time;
	uint32_t passive_dwell_time;
	uint32_t pno_channel_prediction;
	uint32_t top_k_num_of_channels;
	uint32_t stationary_thresh;
	enum scan_dwelltime_adaptive_mode adaptive_dwell_mode;
	uint32_t channel_prediction_full_scan;
	struct pno_nw_type networks_list[SCAN_PNO_MAX_SUPP_NETWORKS];
};

/**
 * struct pno_user_cfg - user configuration required for PNO
 * @channel_prediction: config PNO channel prediction feature status
 * @top_k_num_of_channels: def top K number of channels are used for tanimoto
 * distance calculation.
 * @stationary_thresh: def threshold val to determine that STA is stationary.
 * @pnoscan_adaptive_dwell_mode: def adaptive dwelltime mode for pno scan
 * @channel_prediction_full_scan: def periodic timer upon which full scan needs
 * to be triggered.
 */
struct pno_user_cfg {
	bool channel_prediction;
	uint8_t top_k_num_of_channels;
	uint8_t stationary_thresh;
	enum scan_dwelltime_adaptive_mode adaptive_dwell_mode;
	uint32_t channel_prediction_full_scan;
};

/**
 * struct scan_user_cfg - user configuration required for for scan
 * @active_dwell: default active dwell time
 * @passive_dwell:default passive dwell time
 * @conc_active_dwell: default concurrent active dwell time
 * @conc_passive_dwell: default concurrent passive dwell time
 * @conc_max_rest_time: default concurrent max rest time
 * @conc_min_rest_time: default concurrent min rest time
 * @conc_idle_time: default concurrent idle time
 * @scan_cache_aging_time: default scan cache aging time
 * @is_snr_monitoring_enabled: whether snr monitoring enabled or not
 * @prefer_5ghz: Prefer 5ghz AP over 2.4Ghz AP
 * @select_5gh_margin: Prefer connecting to 5G AP even if
 *    its RSSI is lower by select_5gh_margin dbm than 2.4G AP.
 *    applicable if prefer_5ghz is set.
 * @scan_bucket_threshold: first scan bucket
 * threshold to the mentioned value and all the AP's which
 * have RSSI under this threshold will fall under this
 * bucket
 * @rssi_cat_gap: set rssi category gap
 * @scan_dwell_time_mode: Adaptive dweltime mode
 * @pno_cfg: Pno related config params
 */
struct scan_user_cfg {
	uint32_t active_dwell;
	uint32_t passive_dwell;
	uint32_t conc_active_dwell;
	uint32_t conc_passive_dwell;
	uint32_t conc_max_rest_time;
	uint32_t conc_min_rest_time;
	uint32_t conc_idle_time;
	uint32_t scan_cache_aging_time;
	bool is_snr_monitoring_enabled;
	uint32_t prefer_5ghz;
	uint32_t select_5ghz_margin;
	int32_t scan_bucket_threshold;
	uint32_t rssi_cat_gap;
	enum scan_dwelltime_adaptive_mode scan_dwell_time_mode;
	struct pno_user_cfg pno_cfg;
};

/**
 * update_beacon_cb() - cb to inform/update beacon
 * @psoc: psoc pointer
 * @scan_params:  scan entry to inform/update
 *
 * @Return: void
 */
typedef void (*update_beacon_cb) (struct wlan_objmgr_pdev *pdev,
	struct scan_cache_entry *scan_entry);

/**
 * scan_iterator_func() - function prototype of scan iterator function
 * @scan_entry: scan entry object
 * @arg: extra argument
 *
 * PROTO TYPE, scan iterator function prototype
 *
 * @Return: QDF_STATUS
 */
typedef QDF_STATUS (*scan_iterator_func) (void *arg,
	struct scan_cache_entry *scan_entry);
#endif
