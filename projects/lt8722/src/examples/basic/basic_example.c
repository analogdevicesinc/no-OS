/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for lt8722 project.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "basic_example.h"
#include "common_data.h"
#include "lt8722.h"

int basic_example_main()
{
	int ret;
	double voltage;
	struct lt8722_dev *lt8722_dev;

	pr_info("\nBasic example");

	ret = lt8722_init(&lt8722_dev, &lt8722_ip);
	if (ret)
		goto exit;

	// /* Set output voltage */
	ret = lt8722_set_output_voltage(lt8722_dev, 2.5);
	if (ret)
		goto exit;

	ret = lt8722_get_output_voltage(lt8722_dev, &voltage);
	if (ret)
		goto exit;

	return 0;

exit:
	if (ret)
		pr_err("Error!\n");

	lt8722_remove(lt8722_dev);

	return ret;
}
