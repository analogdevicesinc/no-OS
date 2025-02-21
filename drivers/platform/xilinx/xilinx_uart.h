/*******************************************************************************
 *   @file   xilinx/xilinx_uart.h
 *   @brief  Header containing types used in the uart driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef XILINX_UART_H_
#define XILINX_UART_H_


#define UART_BUFF_LENGTH 256

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
