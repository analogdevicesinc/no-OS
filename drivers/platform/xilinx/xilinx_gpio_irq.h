/***************************************************************************//**
 *   @file   xilinx_gpio_irq.h
 *   @brief  Implementation of Xilinx GPIO IRQ Generic Driver.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
