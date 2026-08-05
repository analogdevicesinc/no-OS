/***************************************************************************//**
 *   @file   mcs_gpio.h
 *   @brief  Header file of MCS_GPIO Driver.
 *   @author GGMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef MCS_GPIO_H_
#define MCS_GPIO_H_

#include <stdbool.h>
#include <stdint.h>

struct mcs_gpio_dev {
	struct no_os_gpio_desc *gpio_req;
	struct jesd204_dev *jdev;
};

struct mcs_gpio_init_param {
	struct no_os_gpio_init_param *gpio_req;
};

/* Initialize the device. */
int32_t mcs_gpio_init(struct mcs_gpio_dev **device,
		      const struct mcs_gpio_init_param *init_param);
/* Remove the device. */
int32_t mcs_gpio_remove(struct mcs_gpio_dev *device);

#endif // MCS_GPIO_H_
