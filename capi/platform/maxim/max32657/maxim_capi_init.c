/*******************************************************************************
 *   @file   maxim_capi_init.c
 *   @brief  Pre-main platform initialization for MAX32657 CAPI builds
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "mxc_sys.h"
#include "mxc_delay.h"

/**
 * @brief Initialization function called by MSDK
 */
int Board_Init(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	/* Forces the SysTick enable state to be saved. */
	return MXC_Delay(1);
}
