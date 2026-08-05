/***************************************************************************//**
 *   @file   adl8113.h
 *   @brief  Header file for ADL8113 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef ADL8113_H_
#define ADL8113_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

/**
 * @enum adl8113_mode
 * @brief ADL8113 Operation Mode
 */
enum adl8113_mode {
	ADL8113_INTERNAL_AMPLIFIER,
	ADL8113_INTERNAL_BYPASS,
	ADL8113_EXTERNAL_BYPASS_A,
	ADL8113_EXTERNAL_BYPASS_B
};

/**
 * @struct adl8113_dev
 * @brief ADL8113 Device Descriptor.
 */
struct adl8113_dev {
	struct no_os_gpio_desc	*gpio_va;
	struct no_os_gpio_desc	*gpio_vb;
	enum adl8113_mode	current_mode;
};

/**
 * @struct adl8113_init_param
 * @brief ADL8113 Initialization Parameters structure.
 */
struct adl8113_init_param {
	struct no_os_gpio_init_param	gpio_va;
	struct no_os_gpio_init_param	gpio_vb;
	enum adl8113_mode		initial_mode;
};

/* Set operation mode (LNA or Bypass). */
int adl8113_set_mode(struct adl8113_dev *dev, enum adl8113_mode mode);

/* Get current operation mode. */
int adl8113_get_mode(struct adl8113_dev *dev, enum adl8113_mode *mode);

/* Initialize the device. */
int adl8113_init(struct adl8113_dev **device,
		 struct adl8113_init_param *init_param);

/* Free the resources allocated by adl8113_init(). */
int adl8113_remove(struct adl8113_dev *dev);

#endif /* ADL8113_H_ */
