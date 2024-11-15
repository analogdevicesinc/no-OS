/***************************************************************************//**
 *   @file   aducm3029_gpio_irq.h
 *   @brief  Header file of GPIO IRQ interface for ADuCM3029 platform.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GPIO_IRQ_EXTRA_H_
#define GPIO_IRQ_EXTRA_H

#include <drivers/xint/adi_xint.h>

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
/**
 * @enum irq_ctrl_id
 * @brief Interrupt controllers ID
 */
enum irq_ctrl_id {
	ADUCM_XINT_SOFT_CTRL,
	ADUCM_GPIO_A_GROUP_SOFT_CTRL,
	ADUCM_GPIO_B_GROUP_SOFT_CTRL
};

/**
 * @struct aducm_gpio_irq_ctrl_desc
 * @brief Stores specific platform parameters
 */
struct aducm_gpio_irq_ctrl_desc {
	/** Memory needed by the ADI IRQ driver */
	uint8_t irq_memory[ADI_XINT_MEMORY_SIZE];
	/** List of user callbacks */
	struct no_os_list_desc *actions;
};

/**
 * @brief maxim platform specific gpio irq platform ops structure
 */
extern const struct no_os_irq_platform_ops aducm_gpio_irq_ops;

#endif
