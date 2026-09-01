/***************************************************************************//**
 *   @file   riscvrx_gpio.h
 *   @brief  Header containing extra types used in the Lattice GPIO IP driver
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef LATTICE_GPIO_H_
#define LATTICE_GPIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct lattice_gpio_init_param
 * @brief Structure holding the initialization parameters for Lattice platform
 * specific GPIO parameters.
 */
struct lattice_gpio_init_param {
	/** GPIO controller base address */
	uint32_t		base_address;
	/** Number of GPIO lines */
	uint32_t		lines_num;
	/** GPIO direction configuration (bitfield: 1=output, 0=input) */
	uint32_t		gpio_dirs;
};

/**
 * @struct lattice_gpio_desc
 * @brief Lattice platform specific GPIO descriptor
 */
struct lattice_gpio_desc {
	/** Lattice BSP GPIO Instance pointer */
	void			*instance;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief Lattice platform specific GPIO platform ops structure
 */
extern const struct no_os_gpio_platform_ops lattice_gpio_ops;

#endif /* LATTICE_GPIO_H_ */
