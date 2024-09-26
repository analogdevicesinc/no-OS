/***************************************************************************//**
 *   @file   xilinx_gpio_irq.h
 *   @brief  Implementation of Xilinx GPIO IRQ Generic Driver.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
