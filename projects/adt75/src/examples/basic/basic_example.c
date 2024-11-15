/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example header for ad74413r project
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

#include "adt75.h"

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
	int ret;
	int32_t val;
	struct adt75_desc *adt75_desc;

	ret = adt75_init(&adt75_desc, &adt75_ip);
	if (ret)
		return ret;

	while (1) {
		/* Clear the screen. */
		printf("%c",27);
		printf("%c",'[');
		printf("%c",'2');
		printf("%c",'J');

		ret = adt75_get_single_temp(adt75_desc, &val);
		if (ret)
			goto error;

		pr_info("Temperature: %.03f C\n", (double)val / 1000);
		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	adt75_remove(adt75_desc);

	return ret;
}
