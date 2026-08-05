/***************************************************************************//**
 *   @file   adg2404.h
 *   @brief  Header file of ADG2404 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef ADG2404_H_
#define ADG2404_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

enum adg2404_channel {
	ADG2404_CH_OFF = 0,
	ADG2404_S1,
	ADG2404_S2,
	ADG2404_S3,
	ADG2404_S4,
};


struct adg2404_dev {
	struct no_os_gpio_desc	*gpio_a0;
	struct no_os_gpio_desc	*gpio_a1;
	struct no_os_gpio_desc	*gpio_en;
};

struct adg2404_init_param {
	struct no_os_gpio_init_param	gpio_a0;
	struct no_os_gpio_init_param	gpio_a1;
	struct no_os_gpio_init_param	gpio_en;
};

/* Select the multiplexer channel. */
int adg2404_select_channel(struct adg2404_dev *dev,
			   enum adg2404_channel channel);
/* Enable/disable the multiplexer. */
int adg2404_enable(struct adg2404_dev *dev, bool enable);
/* Initialize the device. */
int adg2404_init(struct adg2404_dev **device,
		 struct adg2404_init_param *init_param);
/* Free the resources allocated by adg2404_init(). */
int adg2404_remove(struct adg2404_dev *dev);

#endif // ADG2404_H_
