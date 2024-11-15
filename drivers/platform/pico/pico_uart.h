/***************************************************************************//**
 *   @file   pico/pico_uart.h
 *   @brief  Header file of UART driver for pico
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PICO_UART_H_
#define _PICO_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"
#include "no_os_irq.h"
#include "hardware/uart.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_uart_desc
 * @brief pico platform specific UART descriptor
 */
struct pico_uart_desc {
	/** UART instance */
	uart_inst_t *uart_instance;
	/** Controller that handles UART interrupts */
	struct no_os_irq_ctrl_desc *nvic;
	/** RX complete callback */
	struct no_os_callback_desc rx_callback;
};

/**
 * @brief Available GP config for UART TX
 */
enum uart_tx_gp {
	/* UART0 available configurations */
	UART0_TX_GP0 = 0,
	UART0_TX_GP12 = 12,
	UART0_TX_GP16 = 16,
	/* UART1 available configurations */
	UART1_TX_GP4 = 4,
	UART1_TX_GP8 = 8,
};

/**
 * @brief Available GP config for UART RX
 */
enum uart_rx_gp {
	/* UART0 available configurations */
	UART0_RX_GP1 = 1,
	UART0_RX_GP13 = 13,
	UART0_RX_GP17 = 17,
	/* UART1 available configurations */
	UART1_RX_GP5 = 5,
	UART1_RX_GP9 = 9,
};

/**
 * @brief Additional UART config parameters
 */
struct pico_uart_init_param {
	/** UART TX pin configuration */
	enum uart_tx_gp uart_tx_pin;
	/** UART RX pin configuration */
	enum uart_rx_gp uart_rx_pin;
};

/**
 * @brief pico specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops pico_uart_ops;

#endif
