/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_sys.c
 *   @brief  MAX78000 system-level helpers shared across cores/examples.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

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
