/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of BASIC example
 *   @author Axel Haslam (ahaslam@baylibre.com)
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
#include "ad796x.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief BASIC example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function basic_app_run and will not return.
*******************************************************************************/
int basic_example_main()
{
	uint32_t *buf = (uintptr_t)ADC_DDR_BASEADDR;
	int i, ret;
	struct ad796x_dev *adc_dev;

	pr_info("init\n");

	ret = ad796x_init(&adc_dev, &ad796x_init_param);
	if (ret)
		return ret;

	pr_info("Capture start.\n");
	ret = ad796x_read_data(adc_dev, buf, SAMPLES_PER_CHANNEL);
	if (ret) {
		pr_err("read data error %d\n", ret);
		ad796x_remove(adc_dev);
		return ret;
	}

	for (i = 0; i < SAMPLES_PER_CHANNEL; i++, buf++)
		printf("CH1: %ld\n", *buf);

	pr_info("\n Capture done.\n");

	return ad796x_remove(adc_dev);
}
