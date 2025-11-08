/***************************************************************************//**
 *   @file   maxim_gpio.h
 *   @brief  Header file of the GPIO driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef MAXIM_CAPI_GPIO_H_
#define MAXIM_CAPI_GPIO_H_

#include "capi_gpio.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private handle structure for MAX32655 GPIO CAPI implementation
 */
struct maxim_capi_gpio_handle {
	struct capi_gpio_port_handle capi_handle;  /**< Must be first field */
	mxc_gpio_regs_t *gpio_regs;               /**< Maxim GPIO port registers */
	uint32_t port_id;                         /**< GPIO port ID */
	uint8_t num_pins;                         /**< Number of pins in port */
	uint32_t *pin_flags;                      /**< Pin flags array */
};

#define CAPI_GPIO_HANDLE &(struct capi_gpio_port_handle){	\
		.priv = &(struct maxim_capi_gpio_handle){}	\
	}

/**
 * @brief MAX32655 GPIO CAPI operations structure
 *
 * This structure contains function pointers for all GPIO CAPI operations
 * specific to the MAX32655 platform.
 */
extern const struct capi_gpio_ops maxim_gpio_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_GPIO_H_ */