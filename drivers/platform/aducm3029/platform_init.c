/***************************************************************************//**
 *   @file   platform_init.c
 *   @brief  ADuCM3029 platform initialization source.
 *   @author Andrei Drimbarean (Andrei.Drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#include "no_os_error.h"

/**
 * @brief Initialize the power controller and set the core and peripherals clock
 *        divider.
 * @return 0 in case of success or error code otherwise.
 */
int32_t platform_init(void)
{
	int32_t ret;

	ret = adi_pwr_Init();
	if(ret != 0)
		return -1;
	ret = adi_pwr_SetClockDivider(ADI_CLOCK_HCLK,1);
	if(ret != 0)
		return -1;
	ret = adi_pwr_SetClockDivider(ADI_CLOCK_PCLK,1);
	if(ret != 0)
		return ret;

	adi_initComponents();

	return 0;
}
