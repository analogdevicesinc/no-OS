/***************************************************************************//**
 *   @file   maxim_uart.h
 *   @brief  Header file of UART driver.
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

#ifndef MAXIM_UART_H_
#define MAXIM_UART_H_

#include "uart.h"
#include "no_os_irq.h"
#include "max32662.h"
#include "no_os_uart.h"
#include "gpio.h"

/**
 * @brief UART flow control
 */
enum max_uart_flow_ctrl {
	MAX_UART_FLOW_DIS,
	UART_FLOW_EN
};

/**
 * @brief UART pin mapping select
 */
enum max_uart_map {
	UART_MAP_A,
	UART_MAP_B,
	UART_MAP_C,
};

/**
 * @brief Aditional UART config parameters
 */
struct max_uart_init_param {
	enum max_uart_flow_ctrl flow;
	enum max_uart_map map;
	mxc_gpio_vssel_t vssel;
};

/**
 * @brief Platform specific UART state
 */
struct max_uart_desc {
	/** Controller that handles UART interrupts */
	struct no_os_irq_ctrl_desc *nvic;
};

/**
 * @brief Maxim specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops max_uart_ops;

#endif
