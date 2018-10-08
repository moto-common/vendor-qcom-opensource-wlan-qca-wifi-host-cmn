/*
 * Copyright (c) 2017-2018 The Linux Foundation. All rights reserved.
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

#ifndef _SPECTRAL_DA_API_I_H_
#define _SPECTRAL_DA_API_I_H_

#include "spectral_defs_i.h"

/**
 * spectral_ctx_init_da() - Internal function to initialize spectral context
 * with direct attach specific functions
 * @sc : spectral context
 *
 * Internal function to initialize spectral context with direct attach
 * specific functions
 *
 * Return : None
 */
#if DA_SUPPORT
void spectral_ctx_init_da(struct spectral_context *sc);
#else
#define spectral_ctx_init_da(sc) /**/
#endif

#endif /* _SPECTRAL_DA_API_I_H_ */
