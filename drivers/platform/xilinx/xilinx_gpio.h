/*******************************************************************************
 *   @file   xilinx/xilinx_gpio.h
 *   @brief  Header containing extra types used in the gpio driver
 *   @author scuciurean (sergiu.cuciurean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_GPIO_H_
#define XILINX_GPIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum xil_gpio_type
 * @brief Xilinx platform architecture sections
 */
enum xil_gpio_type {
	/** Programmable Logic */
	GPIO_PL,
	/** Processing System */
	GPIO_PS
};

/**
 * @struct xil_gpio_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific GPIO parameters.
 */
struct xil_gpio_init_param {
	/** Xilinx architecture */
	enum xil_gpio_type	type;
	/** Device ID */
	uint32_t		device_id;
};

/**
 * @struct xil_gpio_desc
 * @brief Xilinx platform specific GPIO descriptor
 */
struct xil_gpio_desc {
	/** Xilinx architecture */
	enum xil_gpio_type	type;
	/** Xilinx GPIO configuration */
	void			*config;
	/** Xilinx GPIO Instance */
	void			*instance;
};

/**
 * @brief Xilinx platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops xil_gpio_ops;

#endif
