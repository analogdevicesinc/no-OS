/***************************************************************************//**
 *   @file   ad796x.c
 *   @brief  Implementation of AD796X Driver.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "ad796x.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD796X_BYTES_PER_SAMPLE		4

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* gpio values for the respective mode {en0, en1, en2, en3} */
static uint8_t ad796x_modes[][AD796X_NUM_GPIOS] = {
	[AD796X_MODE1_EXT_REF_5P0] =		{1, 0, 0, 1},
	[AD796X_MODE2_INT_REF_4P0] =		{1, 0, 0, 1},
	[AD796X_MODE3_EXT_REF_4P0] =		{0, 1, 0, 1},
	[AD796X_MODE4_SNOOZE] =			{1, 1, 0, 1},
	[AD796X_MODE5_TEST] =			{0, 0, 1, 0},
	[AD796X_MODE6_INVALID] =		{0, 0, 1, 1},
	[AD796X_MODE7_EXT_REF_5P0_9MHZ] =	{1, 0, 1, 1},
	[AD796X_MODE8_INT_REF_4P0_9MHZ] =	{1, 0, 1, 1},
	[AD796X_MODE9_EXT_REF_4P0_9MHZ] =	{0, 1, 1, 1},
	[AD796X_MODE10_SNOOZE2] =		{1, 1, 1, 1},
};

/**
 * @brief Read data samples from adc
 * @param dev - The device structure.
 * @param buf - The buffer to fill.
 * @param samples - number of samples to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad796x_read_data(struct ad796x_dev *dev,
			 uint32_t *buf,
			 uint16_t samples)
{
	int ret;
	struct 	axi_dma_transfer read_transfer = {
		.size = samples * AD796X_BYTES_PER_SAMPLE,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)buf,
	};

	ret = axi_dmac_transfer_start(dev->axi_dmac, &read_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start() failed!\n");
		return ret;
	}

	return axi_dmac_transfer_wait_completion(dev->axi_dmac, 3000);
}

/**
 * @brief Remove the device gpio resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad796x_gpio_remove(struct ad796x_dev *dev)
{
	int ret, i;

	if (!dev)
		return -EINVAL;

	for (i = 0; i < AD796X_NUM_GPIOS; i++) {
		ret = no_os_gpio_remove(dev->gpio_en[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Initialize the device gpios.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad796x_gpio_init(struct ad796x_dev *dev,
			    struct ad796x_init_param *init_param)
{
	int ret, i;

	if (!dev || !init_param)
		return -EINVAL;

	for (i = 0; i < AD796X_NUM_GPIOS; i++) {
		ret = no_os_gpio_get_optional(&dev->gpio_en[i], init_param->gpio_ip[i]);
		if (ret)
			goto error;

		if (!dev->gpio_en[i])
			continue;

		ret = no_os_gpio_direction_output(dev->gpio_en[i],
						  ad796x_modes[init_param->mode][i]);
		if (ret)
			goto error;
	}

	return 0;
error:
	ad796x_gpio_remove(dev);
	return ret;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad796x_init(struct ad796x_dev **device,
		struct ad796x_init_param *init_param)
{
	struct ad796x_dev *dev;
	int ret;

	dev = (struct ad796x_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = ad796x_gpio_init(dev, init_param);
	if (ret)
		goto error;

	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_ip);
	if (ret) {
		pr_err("error: axi_clkgen_init error: %d\n", ret);
		goto error;
	}

	ret = axi_clkgen_set_rate(dev->clkgen, 125000000);
	if (ret) {
		pr_err("error: axi_clkgen_set_rate error: %d\n", ret);
		goto error;
	}

	ret = no_os_pwm_init(&dev->axi_pwm_0, init_param->axi_pwm_0_ip);
	if (ret) {
		pr_err("error: no_os_pwm_init error: %d\n", ret);
		goto error;
	}

	ret = no_os_pwm_init(&dev->axi_pwm_1, init_param->axi_pwm_1_ip);
	if (ret) {
		pr_err("error: no_os_pwm_init error: %d\n", ret);
		goto error;
	}

	ret = axi_adc_init(&dev->ad796x_core, init_param->ad796x_core_ip);
	if (ret) {
		pr_err("axi_adc_init error: %d\n", ret);
		goto error;
	}

	ret = axi_dmac_init(&dev->axi_dmac, init_param->dmac_ip);
	if (ret) {
		pr_err("axi_dmac_init error: %d\n", ret);
		goto error;
	}

	*device = dev;

	return 0;

error:
	ad796x_remove(dev);
	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad796x_remove(struct ad796x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = axi_adc_remove(dev->ad796x_core);
	if (ret)
		return ret;

	ret = no_os_pwm_remove(dev->axi_pwm_1);
	if (ret)
		return ret;

	ret = no_os_pwm_remove(dev->axi_pwm_0);
	if (ret)
		return ret;

	ret = axi_clkgen_remove(dev->clkgen);
	if (ret)
		return ret;

	ret = ad796x_gpio_remove(dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}
