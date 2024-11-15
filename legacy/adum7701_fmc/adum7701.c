/***************************************************************************//**
 *   @file   adum7701.c
 *   @brief  Implementation of adum7701 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "platform_drivers.h"
#include "adum7701.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adum7701_init(struct adum7701_dev **device,
		      struct adum7701_init_param init_param)
{
	struct adum7701_dev *dev;
	int32_t ret;

	dev = (struct adum7701_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* GPIO */
	ret = gpio_get(&dev->dec_ratio, init_param.dec_ratio);
	ret |= gpio_get(&dev->filter_reset, init_param.filter_reset);

	/* Set the decimation ratio */
	ret |= gpio_set_value(dev->dec_ratio, GPIO_HIGH);

	/* Perform a reset */
	ret |= gpio_set_value(dev->filter_reset, GPIO_HIGH);
	ret |= gpio_set_value(dev->filter_reset, GPIO_LOW);

	*device = dev;

	if (!ret)
		printf("adum7701 successfully initialized\n");
	mdelay(1000);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adum7701_init().
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adum7701_remove(struct adum7701_dev *dev)
{
	int32_t ret;

	ret = gpio_remove(dev->dec_ratio);
	ret |= gpio_remove(dev->filter_reset);

	free(dev);

	return ret;
}
