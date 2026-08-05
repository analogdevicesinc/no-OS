/***************************************************************************//**
 *   @file   adg1736.h
 *   @brief  Header file of ADG1736/ADG2736 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef ADG1736_H_
#define ADG1736_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

enum adg1736_type {
	ADG736,
	ADG1736,
	ADG2736,
};

enum adg1736_switch {
	ADG1736_SW1,
	ADG1736_SW2,
};

enum adg1736_state {
	ADG1736_CONNECT_A,
	ADG1736_CONNECT_B,
};

struct adg1736_dev {
	enum adg1736_type type;
	struct no_os_gpio_desc *gpio_in1;
	struct no_os_gpio_desc *gpio_in2;
	struct no_os_gpio_desc *gpio_en;
};

struct adg1736_init_param {
	enum adg1736_type type;
	struct no_os_gpio_init_param gpio_in1;
	struct no_os_gpio_init_param gpio_in2;
	struct no_os_gpio_init_param *gpio_en;
};

/** Set the state of a specific switch. */
int adg1736_set_switch_state(struct adg1736_dev *dev,
			     enum adg1736_switch sw,
			     enum adg1736_state state);

/** Get the current state of a specific switch. */
int adg1736_get_switch_state(struct adg1736_dev *dev,
			     enum adg1736_switch sw,
			     enum adg1736_state *state);

/** Initialize the ADG1736 device. */
int adg1736_init(struct adg1736_dev **device,
		 struct adg1736_init_param *init_param);

/** Free resources allocated by adg1736_init(). */
int adg1736_remove(struct adg1736_dev *dev);

/** Enable the mux (ADG1736/ADG2736 only, requires EN pin). */
int adg1736_enable(struct adg1736_dev *dev);

/** Disable the mux (ADG1736/ADG2736 only, requires EN pin). */
int adg1736_disable(struct adg1736_dev *dev);

#endif // ADG1736_H_
