/***************************************************************************//**
 *   @file   adg1712.h
 *   @brief  Header file of ADG1712/ADG2712 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef ADG1712_H_
#define ADG1712_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

enum adg1712_chip_id {
	ID_ADG1712,
	ID_ADG2712,
};

enum adg1712_switch {
	ADG1712_SW1,
	ADG1712_SW2,
	ADG1712_SW3,
	ADG1712_SW4,
};

struct adg1712_dev {
	enum adg1712_chip_id chip_id;
	struct no_os_gpio_desc *gpio_in1;
	struct no_os_gpio_desc *gpio_in2;
	struct no_os_gpio_desc *gpio_in3;
	struct no_os_gpio_desc *gpio_in4;
};

struct adg1712_init_param {
	enum adg1712_chip_id chip_id;
	struct no_os_gpio_init_param gpio_in1;
	struct no_os_gpio_init_param gpio_in2;
	struct no_os_gpio_init_param gpio_in3;
	struct no_os_gpio_init_param gpio_in4;
};

/** Set the state of a specific switch. */
int adg1712_set_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool enable);

/** Get the current state of a specific switch. */
int adg1712_get_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool *enable);

/** Initialize the ADG1712 device. */
int adg1712_init(struct adg1712_dev **device,
		 struct adg1712_init_param *init_param);

/** Free resources allocated by adg1712_init(). */
int adg1712_remove(struct adg1712_dev *dev);

#endif // ADG1712_H_
