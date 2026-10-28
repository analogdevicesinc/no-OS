/***************************************************************************//**
 *   @file   maxim_init.c
 *   @brief  MAX78000 RISC-V (CPU1) initialization.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_init.h"
#include "max78000.h"
#include "maxim_time.h"

/* PeripheralClock is derived from the system clock, independently of CPU1's
 * ISO clock. CPU0 may put its SDK clock variable in a different SRAM region.
 */
void SystemCoreClockUpdate(void)
{
	static const uint32_t frequencies[8] = {
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_ISO] = ISO_FREQ,
		[1] = HIRC_FREQ,
		[2] = HIRC_FREQ,
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_INRO] = INRO_FREQ,
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_IPO] = IPO_FREQ,
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_IBRO] = IBRO_FREQ,
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_ERTCO] = ERTCO_FREQ,
		[MXC_V_GCR_CLKCTRL_SYSCLK_SEL_EXTCLK] = EXTCLK_FREQ,
	};
	uint32_t control = MXC_GCR->clkctrl;
	uint32_t source = (control & MXC_F_GCR_CLKCTRL_SYSCLK_SEL) >>
			  MXC_F_GCR_CLKCTRL_SYSCLK_SEL_POS;
	uint32_t divider = (control & MXC_F_GCR_CLKCTRL_SYSCLK_DIV) >>
			   MXC_F_GCR_CLKCTRL_SYSCLK_DIV_POS;

	SystemCoreClock = frequencies[source] >> divider;
}

/** @brief Initialize CPU1 services; system clocks are configured by CPU0. */
__weak int no_os_init(void)
{
	SystemCoreClockUpdate();
	return maxim_time_init();
}

int Board_Init(void)
{
	return no_os_init();
}
