/***************************************************************************//**
 *   @file   stm32/stm32_gpio_irq.h
 *   @brief  Header file for stm32 gpio irq specifics.
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
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
#ifndef STM32_GPIO_IRQ_H
#define STM32_GPIO_IRQ_H

#include "no_os_irq.h"
#include "stm32_hal.h"

/**
 * @struct stm32_gpio_irq_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific GPIO IRQ parameters.
 */
struct stm32_gpio_irq_init_param {
	uint8_t port_nb;
};

/**
 * @struct stm32_gpio_irq_desc
 * @brief stm32 platform specific GPIO IRQ descriptor
 */
struct stm32_gpio_irq_desc {
	/** EXTI line instance */
	EXTI_HandleTypeDef hexti;
	/** Port number */
	uint8_t port_nb;
};

/**
 * @brief stm32 platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops stm32_gpio_irq_ops;

#endif
