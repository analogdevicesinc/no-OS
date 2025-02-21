/*******************************************************************************
 *   @file   xilinx/xilinx_gpio.h
 *   @brief  Header containing extra types used in the gpio driver
 *   @author scuciurean (sergiu.cuciurean@analog.com)
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

#ifndef XILINX_GPIO_H_
#define XILINX_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

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
