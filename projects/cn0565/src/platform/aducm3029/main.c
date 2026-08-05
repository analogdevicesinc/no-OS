/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for ADuCM3029 platform of cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_gpio.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for ADuCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	struct no_os_gpio_desc *gp0;

	ret = platform_init();
	if (ret)
		return ret;

	/* aducm3029 hack: initialize gpio before irq, otherwise it doesn't work */
	ret = no_os_gpio_get(&gp0, &cn0565_ad5940_ip.gp0_gpio_init);
	if (ret)
		return ret;

	return example_main();
}
