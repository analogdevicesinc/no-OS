/*******************************************************************************
 *   @file   maxim_init.c
 *   @brief  Implementation of maxim initialization functions.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_init.h"
#include "mxc_sys.h"
#include "mxc_delay.h"

/* ************************************************************************** */
__weak int no_os_init(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	/* This has o be performed so the en state of SysTick is saved. */
	return MXC_Delay(1);
}

int Board_Init(void)
{
	return no_os_init();
}
