/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of IIO example for basic_demo project.
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
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "pulsar_adc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute print the sample data.
*******************************************************************************/
int basic_example_main()
{
	struct pulsar_adc_dev *dev;
	uint32_t *data = ADC_DDR_BASEADDR;
	int32_t ret, i;
	uint16_t resolution;
	char sign;

	ret = pulsar_adc_init(&dev, &pulsar_adc_init_param);
	if (ret)
		return ret;

	sign = dev->dev_info->sign;
	resolution = dev->dev_info->resolution;

	ret = pulsar_adc_read_data(dev, data, SAMPLES_PER_CHANNEL);
	if (ret) {
		pr_info("Error: pulsar_adc_read_data: %ld\n", ret);
		pulsar_adc_remove(dev);
		return ret;
	}

	for(i = 0, data = ADC_DDR_BASEADDR; i < SAMPLES_PER_CHANNEL; i++, data++) {
		if (sign == 's')
			printf("ADC: %ld\n\r", no_os_sign_extend32(*data, resolution - 1));
		else
			printf("ADC: %ld\n\r", *data);
	}

	return pulsar_adc_remove(dev);
}
