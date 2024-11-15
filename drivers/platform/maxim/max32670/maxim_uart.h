/***************************************************************************//**
 *   @file   maxim_uart.h
 *   @brief  Header file of UART driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MAXIM_UART_H_
#define _MAXIM_UART_H_

#include "uart.h"
#include "no_os_irq.h"
#include "max32670.h"
#include "no_os_uart.h"

/**
 * @brief UART flow control
 */
enum max_uart_flow_ctrl {
	MAX_UART_FLOW_DIS,
	MAX_UART_FLOW_EN
};

/**
 * @brief Aditional UART config parameters
 */
struct max_uart_init_param {
	enum max_uart_flow_ctrl flow;
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
