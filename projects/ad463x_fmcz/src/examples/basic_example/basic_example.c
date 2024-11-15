/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of IIO example for ad463x_fmcz project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "no_os_print_log.h"

#define BITS_PER_SAMPLE 32
#define REAL_BITS 24
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
	struct ad463x_dev *dev;
	uint32_t *buf = ADC_DDR_BASEADDR;
	int32_t ret, i;
	int32_t tmp;

	ret = ad463x_init(&dev, &ad463x_init_param);
	if (ret)
		return ret;

#if ADAQ4224_DEV
	/* Apply a gain of 1/3 */
#define AD4224_INITIAL_GAIN_IDX 0

	ret = ad463x_fill_scale_tbl(dev);
	if (ret)
		goto out;

	ret = ad463x_set_pgia_gain(dev, AD4224_INITIAL_GAIN_IDX);
	if (ret)
		goto out;

#endif
	ret = ad463x_exit_reg_cfg_mode(dev);
	if (ret)
		goto out;

	ret = ad463x_read_data(dev, buf, SAMPLES_PER_CHANNEL);
	if (ret)
		goto out;

	for (i = 0; i < SAMPLES_PER_CHANNEL; i+=2) {
		tmp = no_os_sign_extend32(buf[i], REAL_BITS - 1);
		pr_info("ADC sample ch1: %lu : %ld \r\n", i, tmp);
	}

	for (i = 1; i < SAMPLES_PER_CHANNEL; i+=2) {
		tmp = no_os_sign_extend32(buf[i], REAL_BITS - 1);
		pr_info("ADC sample ch2: %lu : %ld \r\n", i, tmp);
	}

out:
	return ad463x_remove(dev);
}
