/***************************************************************************//**
 *   @file   xilinx_gpio_irq.h
 *   @brief  Implementation of Xilinx GPIO IRQ Generic Driver.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_GPIO_IRQ_H_
#define XILINX_GPIO_IRQ_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stdbool.h"
#include "no_os_list.h"
#include "xgpiops.h"
#include "no_os_irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct xil_callback_desc
 * @brief Structure holding callback parameters for each pin
 */
struct xil_callback_desc {
	int32_t pin_nb;
	struct no_os_callback_desc callback;
	bool triggered;
	bool enabled;
};

/**
 * @struct xil_gpio_irq_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific GPIO IRQ parameters.
 */
struct xil_gpio_irq_init_param {
	struct no_os_irq_ctrl_desc *parent_desc;
	int32_t gpio_device_id;
};

/**
 * @struct xil_gpio_irq_desc
 * @brief Xilinx platform specific GPIO IRQ descriptor
 */
struct xil_gpio_irq_desc {
	struct no_os_irq_ctrl_desc *parent_desc;
	XGpioPs my_Gpio;
	struct no_os_list_desc *callback_list;
	struct no_os_iterator *it;
};

/**
 * @brief Xilinx specific GPIO IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops xil_gpio_irq_ops;

#endif
