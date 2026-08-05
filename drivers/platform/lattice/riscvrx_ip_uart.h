/***************************************************************************//**
 *   @file   riscvrx_ip_uart.h
 *   @brief  Header file of Lattice external UART IP driver for RISC-V RX
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _LATTICE_IP_UART_H_
#define _LATTICE_IP_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"
#include "no_os_irq.h"
#include "uart.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct latt_ip_uart_desc
 * @brief Lattice platform specific UART descriptor
 */
struct latt_ip_uart_desc {
	/** UART instance */
	struct uart_instance *uart_instance;
	/** Controller that handles UART interrupts */
	struct no_os_irq_ctrl_desc *nvic;
	/** RX complete callback */
	struct no_os_callback_desc rx_callback;
	/** UART interrupt priority */
	uint32_t irq_priority;
};

/**
 * @brief Additional UART config parameters
 */
struct latt_ip_uart_init_param {
	/** Base address of the UART core */
	uint32_t *base_addr;
	/** System clock of the UART core */
	uint32_t sys_clk;
	/** UART interrupt priority */
	uint32_t irq_priority;
};

/**
 * @brief Lattice specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops latt_ip_uart_ops;

#endif
