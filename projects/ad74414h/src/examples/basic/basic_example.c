/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for eval-ad74414h project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ad74416h_desc *ad74414h_desc;
	int ret;

	ret = ad74416h_init(&ad74414h_desc, &ad74414h_ip);
	if (ret)
		goto error;

	pr_info("ad74414h successfully initialized!\r\n");

	ret = ad74416h_gpio_set(ad74414h_desc, AD74416H_CH_C, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_ad74414h;

	pr_info("ad74414h GPO2 set to HIGH\r\n");

	return 0;

error_ad74414h:
	ad74416h_remove(ad74414h_desc);
error:
	pr_info("Error!\r\n");
	return ret;
}
