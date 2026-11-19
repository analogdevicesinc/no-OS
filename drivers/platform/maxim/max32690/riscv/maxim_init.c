/***************************************************************************//**
 *   @file   maxim_init.c
 *   @brief  MAX32690 RISC-V (CPU1) platform initialization.
 *   @author Christopher de Guzman (christopher.deguzman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_init.h"
#include "max32690.h"
#include "maxim_time.h"

/**
 * @brief Initialize CPU1 services; system clocks are configured by CPU0.
 *
 * The SDK SystemCoreClockUpdate() derives the clock from GCR.clkctrl, so it is
 * valid on CPU1 even though CPU0 selected the clock source.
 */
__weak int no_os_init(void)
{
	SystemCoreClockUpdate();
	return maxim_time_init();
}

int Board_Init(void)
{
	return no_os_init();
}
