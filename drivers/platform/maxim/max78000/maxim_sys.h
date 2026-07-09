/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_sys.h
 *   @brief  MAX78000 system-level helpers shared across cores/examples.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#ifndef MAXIM_SYS_H_
#define MAXIM_SYS_H_

#include "mxc_sys.h"

/**
 * @brief Read-modify-write replacement for the SDK's MXC_SYS_Reset_Periph().
 *
 * The Maxim SDK's MXC_SYS_Reset_Periph() (sys_ai85.c) resets a peripheral by
 * writing GCR->rst0/rst1 (or LPGCR->rst) as a plain single-bit assignment:
 *   MXC_GCR->rst1 = (1 << bit);
 * That store clears every other bit in the register at the same time -- for
 * rst1 that includes RST1_SMPHR (bit 16) and RST1_SIMO (bit 25). Resetting
 * SMPHR destroys the hardware semaphore some designs use as a cross-core lock,
 * and glitching SIMO can disturb the supply rail (observed as SPI corruption
 * and dropped JTAG). This wrapper sets only the requested bit and spins until
 * the hardware clears it.
 *
 * Enable by adding "-Wl,--wrap=MXC_SYS_Reset_Periph" to the link flags; the
 * linker then redirects every call site here. The SDK's original definition
 * remains as __real_MXC_SYS_Reset_Periph but is never called.
 *
 * @param reset Peripheral reset selector (mxc_sys_reset_t).
 */
void __wrap_MXC_SYS_Reset_Periph(mxc_sys_reset_t reset);

#endif /* MAXIM_SYS_H_ */
