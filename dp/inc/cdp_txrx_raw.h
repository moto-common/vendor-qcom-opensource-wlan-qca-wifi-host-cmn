/*
 * Copyright (c) 2016-2017 The Linux Foundation. All rights reserved.
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
/**
 * @file cdp_txrx_raw.h
 * @brief Define the host data path raw mode API functions
 * called by the host control SW and the OS interface module
 */
#ifndef _CDP_TXRX_RAW_H_
#define _CDP_TXRX_RAW_H_

#include "cdp_txrx_handle.h"
#include "cdp_txrx_ops.h"
/* TODO: adf need to be replaced with qdf */
static inline int cdp_get_nwifi_mode(ol_txrx_soc_handle soc,
	struct cdp_vdev *vdev)
{
	if (soc->ops->raw_ops->txrx_get_nwifi_mode)
		return soc->ops->raw_ops->txrx_get_nwifi_mode(vdev);
	return 0;
}

/**
 * @brief finds the ast entry for the packet
 * @details: Finds the ast entry i.e 4th address for the packet based on the
 *               details in the netbuf.
 *
 * @param vdev - the data virtual device object
 * @param pnbuf - pointer to nbuf
 * @param raw_ast - pointer to fill ast information
 *
 * @return - 0 on success, -1 on error, 1 if more nbufs need to be consumed.
 */

static inline void
cdp_rawsim_get_astentry (ol_txrx_soc_handle soc, struct cdp_vdev *vdev,
			qdf_nbuf_t *pnbuf, struct cdp_raw_ast *raw_ast)
{

	if (!soc || !soc->ops || !soc->ops->raw_ops)
		return;

	if (soc->ops->raw_ops->rsim_get_astentry)
		soc->ops->raw_ops->rsim_get_astentry(vdev, pnbuf, raw_ast);

	return;
}

#endif
