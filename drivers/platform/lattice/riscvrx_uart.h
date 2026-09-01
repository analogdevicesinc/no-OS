/***************************************************************************//**
 *   @file   riscvrx_uart.h
 *   @brief  Header file of UART driver for Lattice RISC-V RX built in UART
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _LATTICE_UART_H_
#define _LATTICE_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"
#include "no_os_irq.h"
#include "local_uart.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct latt_uart_desc
 * @brief Lattice platform specific UART descriptor
 */
struct latt_uart_desc {
	/** UART instance */
	struct local_uart_instance *uart_instance;
};

/**
 * @brief Additional UART config parameters
 */
struct latt_uart_init_param {
	/** Base address of the UART core */
	uint32_t *base_addr;
	/** System clock of the UART core */
	uint32_t sys_clk;
};

/**
 * @brief Lattice specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops latt_uart_ops;

#endif
