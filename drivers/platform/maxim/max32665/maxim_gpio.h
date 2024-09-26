/***************************************************************************//**
 *   @file   maxim_gpio.h
 *   @brief  Header file for maxim gpio specifics.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#ifndef MAXIM_GPIO_H_
#define MAXIM_GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "max32665.h"
#include "gpio.h"

#define N_PINS	MXC_CFG_GPIO_PINS_PORT
#define N_PORTS	MXC_CFG_GPIO_INSTANCES

/**
 * @brief maxim platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops max_gpio_ops;
/**
 * @brief maxim platform specific gpio irq platform ops structure
 */
extern const struct no_os_irq_platform_ops max_gpio_irq_ops;

struct max_gpio_init_param {
	/** GPIO's voltage level */
	mxc_gpio_vssel_t vssel;
};

#endif
