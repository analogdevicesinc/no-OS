/***************************************************************************//**
 *   @file   aducm3029_uart.h
 *   @brief  Header file of UART driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef ADUCM3029_UART_H_
#define ADUCM3029_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <drivers/uart/adi_uart.h>
#include <stdbool.h>
#include <stdint.h>
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_uart.h"

/** Maximum number of bytes that can be transmitted on UART in one transfer */
#define NO_OS_UART_MAX_BYTES	1024u

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum UART_ERROR
 * @brief Values returned by \ref no_os_uart_get_errors. Multiple values can be set
 */
enum UART_ERROR {
	/** No errors */
	NO_ERR				= 0x00,
	/**
	 * Bytes number for read/write was set 0 or greater than
	 * \ref NO_OS_UART_MAX_BYTES
	 */
	BAD_INPUT_PARAMETERS		= 0x01,
	/** New call was made before a callback call ended */
	WAITING_FOR_CALLBACK		= 0x02,
	/** Rx framing error */
	HW_ERR_FRAMING                  = 0x10,
	/** Rx parity error */
	HW_ERR_PARITY                   = 0x20,
	/** Receive overrun */
	HW_ERR_OVERRUN                  = 0x40,
	/** Break condition */
	BREAK_INTERRUPT                 = 0x80,
	/** Rx DMA channel bus fault detected */
	HW_ERR_RX_CHAN_DMA_BUS_FAULT     = 0x100,
	/** Tx DMA channel bus fault detected */
	HW_ERR_TX_CHAN_DMA_BUS_FAULT     = 0x200,
	/** Rx DMA channel invalid descriptor detected. */
	HW_ERR_RX_CHAN_DMA_INVALID_DESCR = 0x400,
	/** Tx DMA channel invalid descriptor detected. */
	HW_ERR_TX_CHAN_DMA_INVALID_DESCR = 0x800,
	/** Rx DMA channel unknown error detected. */
	HW_ERR_RX_CHAN_DMA_UNKNOWN_ERROR = 0x1000,
	/** Tx DMA channel unknown error detected. */
	HW_ERR_TX_CHAN_DMA_UNKNOWN_ERROR = 0x2000,
};

/**
 * @struct no_os_baud_desc
 * @brief Structure of an element from \ref baud_rates_26MHz.
 * The baud rate is calculated with the formula:
 * Baudrate = (NO_OS_CLK_FREQ / (div_m + div_n/2048) * pow(2,osr+2) * div_c)).
 */
struct no_os_baud_desc {
	/** Calculated baud rate from the following parameters */
	uint32_t	baud_rate;
	/** From 0 to 2047 */
	uint16_t	div_n;
	/** From 1 to 3 */
	uint8_t		div_m;
	/** From 1 to 65535 */
	uint16_t	div_c;
	/** From 0 to 3 */
	uint8_t		osr;
};

/**
 * @struct no_os_op_desc
 * @brief It stores the state of a operation
 */
struct no_os_op_desc {
	/** Is set when an write nonblocking operation is executing */
	bool		is_nonblocking;
	/** Current buffer*/
	uint8_t		*buff;
	/** Number of bytes pending to process */
	uint32_t	pending;
};

/**
 * @struct no_os_aducm_uart_desc
 * @brief Stores specific parameter needed by the UART driver for internal
 * operations
 */
struct no_os_aducm_uart_desc {
	/** Handle needed by low level functions */
	ADI_UART_HANDLE	uart_handler;
	/** Stores the error occurred */
	enum UART_ERROR	errors;
	/** Set if callback is enabled */
	bool		callback_enabled;
	/**
	 * Buffer needed by the ADI UART driver to operate.
	 * This buffer allocated and aligned at runtime to 32 bits
	 */
	uint8_t		*adi_uart_buffer;
	/**
	 * Stores the offset used to align adi_uart_buffer.
	 * Needed to deallocate \ref adi_uart_buffer
	 */
	uint32_t	adi_uart_buffer_offset;
	/** Status of a write operation */
	struct no_os_op_desc	write_desc;
	/** Status of a read operation */
	struct no_os_op_desc	read_desc;
	/** Interrupt controller descriptor */
	struct no_os_irq_ctrl_desc *nvic;
	/** RX complete callback */
	struct no_os_callback_desc rx_callback;
};

/**
* @brief aducm3029 platform specific UART platform ops structure
*/
extern const struct no_os_uart_platform_ops aducm_uart_ops;

#endif /* ADUCM3029_UART_H_ */
