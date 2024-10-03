/***************************************************************************//**
 *   @file   altera/altera_gpio.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef ALTERA_GPIO_H_
#define ALTERA_GPIO_H_

#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Altera platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops altera_gpio_ops;

/**
 * @enum gpio_type
 * @brief Altera platform architecture types
 */
enum gpio_type {
	/** Nios II architecture */
	NIOS_II_GPIO
};

/**
 * @struct altera_gpio_init_param
 * @brief Structure holding the initialization parameters for Altera platform
 * specific GPIO parameters.
 */
struct altera_gpio_init_param {
	/** Altera architecture type */
	enum gpio_type	type;
	/** Device ID */
	uint32_t	device_id;
	/** GPIO base address */
	uint32_t	base_address;
};

/**
 * @struct altera_gpio_desc
 * @brief Altera platform specific GPIO descriptor
 */
struct altera_gpio_desc {
	/** Altera architecture type */
	enum gpio_type	type;
	/** Device ID */
	uint32_t		device_id;
	/** GPIO base address */
	uint32_t base_address;
};

#endif /* ALTERA_GPIO_H_ */
