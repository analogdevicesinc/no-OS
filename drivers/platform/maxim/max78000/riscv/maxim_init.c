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

/**
 * @brief Initialize CPU1 services; system clocks are configured by CPU0.
 * @return 0 in case of success, negative error code otherwise.
 */
__weak int no_os_init(void)
{
	return maxim_time_init();
}

/**
 * @brief SDK board initialization hook, called by the CPU1 startup code.
 * @return 0 in case of success, negative error code otherwise.
 */
int Board_Init(void)
{
	return no_os_init();
}
