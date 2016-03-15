/*
 * Copyright (c) 2016 The Linux Foundation. All rights reserved.
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

#include "hif.h"
#include "hif_main.h"
#include "multibus.h"
#include "snoc_api.h"
#include "dummy.h"

/**
 * hif_initialize_pci_ops() - initialize the pci ops
 * @bus_ops: hif_bus_ops table pointer to initialize
 *
 * Return: QDF_STATUS_SUCCESS
 */
QDF_STATUS hif_initialize_snoc_ops(struct hif_bus_ops *bus_ops)
{
	bus_ops->hif_bus_open = &hif_snoc_open;
	bus_ops->hif_bus_close = &hif_snoc_close;
	bus_ops->hif_bus_prevent_linkdown = &hif_dummy_bus_prevent_linkdown;
	bus_ops->hif_reset_soc = &hif_dummy_reset_soc;
	bus_ops->hif_bus_suspend = &hif_dummy_bus_suspend;
	bus_ops->hif_bus_resume = &hif_dummy_bus_resume;
	bus_ops->hif_target_sleep_state_adjust =
		&hif_dummy_target_sleep_state_adjust;

	bus_ops->hif_disable_isr = &hif_snoc_disable_isr;
	bus_ops->hif_nointrs = &hif_snoc_nointrs;
	bus_ops->hif_enable_bus = &hif_snoc_enable_bus;
	bus_ops->hif_disable_bus = &hif_snoc_disable_bus;
	bus_ops->hif_bus_configure = &hif_snoc_bus_configure;
	bus_ops->hif_irq_disable = &hif_snoc_irq_disable;
	bus_ops->hif_irq_enable = &hif_snoc_irq_enable;

	return QDF_STATUS_SUCCESS;
}
