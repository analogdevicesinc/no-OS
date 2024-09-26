/***************************************************************************//**
 *   @file   stm32/stm32_gpio.h
 *   @brief  Header file for stm32 gpio specifics.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef STM32_GPIO_H_
#define STM32_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"

/**
 * @struct stm32_gpio_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 */
struct stm32_gpio_init_param {
	/** Output mode */
	uint32_t mode;
	/** Speed grade */
	uint32_t speed;
	/** Alternate functionality */
	uint32_t alternate;
};

/**
 * @struct stm32_gpio_desc
 * @brief stm32 platform specific gpio descriptor
 */
struct stm32_gpio_desc {
	/** Port */
	GPIO_TypeDef *port;
	/** GPIO configuration */
	GPIO_InitTypeDef gpio_config;
};

/**
 * @brief stm32 platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops stm32_gpio_ops;

#endif
