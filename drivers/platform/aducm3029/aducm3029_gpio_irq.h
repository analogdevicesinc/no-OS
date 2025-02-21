/***************************************************************************//**
 *   @file   aducm3029_gpio_irq.h
 *   @brief  Header file of GPIO IRQ interface for ADuCM3029 platform.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef GPIO_IRQ_EXTRA_H_
#define GPIO_IRQ_EXTRA_H

#include <drivers/xint/adi_xint.h>

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
