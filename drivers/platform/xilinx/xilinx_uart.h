/*******************************************************************************
 *   @file   xilinx/xilinx_uart.h
 *   @brief  Header containing types used in the uart driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_UART_H_
#define XILINX_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_BUFF_LENGTH 256

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum xil_uart_type
 * @brief Xilinx platform architecture sections
 */
enum xil_uart_type {
	/** Programmable Logic */
	UART_PL,
	/** Processing System */
	UART_PS
};

/**
 * @struct xil_uart_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific UART parameters.
 */
struct xil_uart_init_param {
	/** Xilinx architecture */
	enum xil_uart_type	type;
	/** Interrupt Request ID */
	uint32_t			irq_id;
	/** Interrupt Request Descriptor */
	struct no_os_irq_ctrl_desc *irq_desc;
};

/**
 * @struct xil_uart_desc
 * @brief Xilinx platform specific UART descriptor
 */
struct xil_uart_desc {
	/** Xilinx architecture */
	enum xil_uart_type	type;
	/** Interrupt Request ID */
	uint32_t			irq_id;
	/** Interrupt Request Descriptor */
	struct no_os_irq_ctrl_desc *irq_desc;
	/** FIFO */
	struct no_os_fifo_element	*fifo;
	/** FIFO read offset */
	uint32_t 			fifo_read_offset;
	/** UART Buffer */
	char 				buff[UART_BUFF_LENGTH];
	/** Number of bytes received */
	uint32_t 			bytes_received;
	/** Total number of errors */
	uint32_t 			total_error_count;
	/** UART Instance */
	void				*instance;
};

/**
 * @brief Xilinx platform specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops xil_uart_ops;

#endif
