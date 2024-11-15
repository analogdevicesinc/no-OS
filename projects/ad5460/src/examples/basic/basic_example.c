/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for eval-ad5460 project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example.h"
#include "common_data.h"
#include "ad5460.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int basic_example_main()
{
	struct ad5460_desc *ad5460_desc;
	int ret;

	ret = ad5460_init(&ad5460_desc, &ad5460_ip);
	if (ret)
		goto error;

	pr_info("ad5460 successfully initialized!\r\n");

	ret = ad5460_gpio_set(ad5460_desc, AD5460_CH_C, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_ad5460;

	pr_info("ad5460 GPOC set to HIGH\r\n");

	return 0;

error_ad5460:
	ad5460_remove(ad5460_desc);
error:
	pr_info("AD5460 Error!\r\n");
	return ret;
}
