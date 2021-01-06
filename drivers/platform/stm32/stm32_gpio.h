/***************************************************************************//**
 *   @file   stm32/stm32_gpio.h
 *   @brief  Header file for stm32 gpio specifics.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef GPIO_EXTRA_H_
#define GPIO_EXTRA_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"

/**
 * @struct stm32_gpio_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 */
typedef struct stm32_gpio_init_param {
	/** Port */
	GPIO_TypeDef *port;
	/** Output mode */
	uint32_t mode;
	/** Pull-up, Pull-down mode */
	uint32_t pull;
	/** Speed grade */
	uint32_t speed;
} stm32_gpio_init_param;

/**
 * @struct stm32_gpio_desc
 * @brief stm32 platform specific gpio descriptor
 */
typedef struct stm32_gpio_desc {
	/** Port */
	GPIO_TypeDef *port;
	/** Output mode */
	uint32_t mode;
	/** Pull-up, Pull-down mode */
	uint32_t pull;
	/** Speed grade */
	uint32_t speed;
} stm32_gpio_desc;

/**
 * @brief stm32 platform specific gpio platform ops structure
 */
extern const struct gpio_platform_ops stm32_gpio_platform_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Obtain the gpio decriptor. */
int32_t stm32_gpio_get(struct gpio_desc **desc,
		       const struct gpio_init_param *param);

/* Obtain optional gpio descriptor. */
int32_t stm32_gpio_get_optional(struct gpio_desc **desc,
				const struct gpio_init_param *param);

/* Free the resources allocated by gpio_get() */
int32_t stm32_gpio_remove(struct gpio_desc *desc);

/* Enable the input direction of the specified gpio. */
int32_t stm32_gpio_direction_input(struct gpio_desc *desc);

/* Enable the output direction of the specified gpio. */
int32_t stm32_gpio_direction_output(struct gpio_desc *desc,
				    uint8_t value);

/* Get the direction of the specified gpio. */
int32_t stm32_gpio_get_direction(struct gpio_desc *desc,
				 uint8_t *direction);

/* Set the value of the specified gpio. */
int32_t stm32_gpio_set_value(struct gpio_desc *desc,
			     uint8_t value);

/* Get the value of the specified gpio. */
int32_t stm32_gpio_get_value(struct gpio_desc *desc,
			     uint8_t *value);

#endif
