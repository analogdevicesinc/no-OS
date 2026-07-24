/*******************************************************************************
 *   @file   maxim_sys.c
 *   @brief  MAX78000 system-level helpers shared across cores/examples.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "maxim_sys.h"
#include "gcr_regs.h"
#include "lpgcr_regs.h"

/*
 * See maxim_sys.h for the rationale. The reset selector encodes the target
 * register by range: 0..31 -> GCR->rst0, 32..63 -> GCR->rst1, 64+ -> LPGCR->rst.
 */
void __wrap_MXC_SYS_Reset_Periph(mxc_sys_reset_t reset)
{
	uint32_t bit;

	if (reset > 63) {
		bit = reset - 64;
		MXC_LPGCR->rst |= (1u << bit);
		while (MXC_LPGCR->rst & (1u << bit)) {}
	} else if (reset > 31) {
		bit = reset - 32;
		MXC_GCR->rst1 |= (1u << bit);
		while (MXC_GCR->rst1 & (1u << bit)) {}
	} else {
		bit = (uint32_t)reset;
		MXC_GCR->rst0 |= (1u << bit);
		while (MXC_GCR->rst0 & (1u << bit)) {}
	}
}
