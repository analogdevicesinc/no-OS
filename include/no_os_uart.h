/***************************************************************************//**
 *   @file   no_os_uart.h
 *   @brief  Header file of UART interface.
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
#ifndef _NO_OS_UART_H_
#define _NO_OS_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_lf256fifo.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_MAX_NUMBER 10

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum no_os_uart_size
 * @brief UART character size (number of data bits) options.
 */
enum no_os_uart_size {
	/** 5 data bits */
	NO_OS_UART_CS_5,
	/** 6 data bits */
	NO_OS_UART_CS_6,
	/** 7 data bits */
	NO_OS_UART_CS_7,
	/** 8 data bits */
	NO_OS_UART_CS_8,
	/** 9 data bits */
	NO_OS_UART_CS_9,
};

/**
 * @enum no_os_uart_parity
 * @brief UART parity options.
 */
enum no_os_uart_parity {
	/** no parity */
	NO_OS_UART_PAR_NO,
	/** mark parity */
	NO_OS_UART_PAR_MARK,
	/** space parity */
	NO_OS_UART_PAR_SPACE,
	/** odd parity */
	NO_OS_UART_PAR_ODD,
	/** even parity */
	NO_OS_UART_PAR_EVEN
};

/**
 * @enum no_os_uart_stop
 * @brief UART number of stop bits options.
 */
enum no_os_uart_stop {
	/** one stop bit */
	NO_OS_UART_STOP_1_BIT,
	/** two stop bits */
	NO_OS_UART_STOP_2_BIT
};

/**
 * @struct no_os_uart_platform_ops
 * @brief Structure holding UART function pointers that point to the platform
 * specific function
 */
struct no_os_uart_platform_ops ;

/**
 * @struct no_os_uart_init_param
 * @brief Structure holding the parameters for UART initialization
 */
struct no_os_uart_init_param {
	/** UART Device ID */
	uint8_t	device_id;
	/** UART Interrupt ID */
	uint32_t irq_id;
	/** If set, the reception is interrupt driven. */
	bool asynchronous_rx;
	/** UART Baud Rate */
	uint32_t        baud_rate;
	/** UART number of data bits */
	enum no_os_uart_size		size;
	/** UART parity */
	enum no_os_uart_parity	parity;
	/** UART number of stop bits */
	enum no_os_uart_stop		stop;
	const struct no_os_uart_platform_ops *platform_ops;
	/** UART extra parameters (device specific) */
	void		*extra;
};

/**
 * @struct no_os_uart_desc
 * @brief Stucture holding the UART descriptor.
 */
struct no_os_uart_desc {
	/** UART mutex*/
	void            *mutex;
	/** UART Device ID */
	uint8_t 	device_id;
	/** UART Interrupt ID */
	uint32_t	irq_id;
	/** Software FIFO. */
	struct lf256fifo *rx_fifo;
	/** UART Baud Rate */
	uint32_t 	baud_rate;
	const struct no_os_uart_platform_ops *platform_ops;
	/** UART extra parameters (device specific) */
	void 		*extra;
};

/**
 * @struct no_os_uart_platform_ops
 * @brief Structure holding UART function pointers that point to the platform
 * specific function
 */
struct no_os_uart_platform_ops {
	/** UART initialization function pointer */
	int32_t (*init)(struct no_os_uart_desc **, struct no_os_uart_init_param *);
	/** UART read function pointer */
	int32_t (*read)(struct no_os_uart_desc *, uint8_t *, uint32_t);
	/** UART write function pointer */
	int32_t (*write)(struct no_os_uart_desc *, const uint8_t *, uint32_t);
	/** UART read non-blocking function pointer */
	int32_t (*read_nonblocking)(struct no_os_uart_desc *, uint8_t *, uint32_t);
	/** UART wrote non-blocking function pointer */
	int32_t (*write_nonblocking)(struct no_os_uart_desc *, const uint8_t *,
				     uint32_t);
	/** UART remove function pointer */
	int32_t (*remove)(struct no_os_uart_desc *);
	/** UART get errors function pointer */
	uint32_t (*get_errors)(struct no_os_uart_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read data from UART. Blocking function */
int32_t no_os_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			uint32_t bytes_number);

/* Write data to UART. Blocking function */
int32_t no_os_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			 uint32_t bytes_number);

/* Read data from UART. Non blocking function */
int32_t no_os_uart_read_nonblocking(struct no_os_uart_desc *desc, uint8_t *data,
				    uint32_t bytes_number);

/* Write data to UART. Non blocking function*/
int32_t no_os_uart_write_nonblocking(struct no_os_uart_desc *desc,
				     const uint8_t *data,
				     uint32_t bytes_number);

/* Initialize the UART communication peripheral. */
int32_t no_os_uart_init(struct no_os_uart_desc **desc,
			struct no_os_uart_init_param *param);

/* Free the resources allocated by no_os_uart_init(). */
int32_t no_os_uart_remove(struct no_os_uart_desc *desc);

/* Check if UART errors occurred. */
uint32_t no_os_uart_get_errors(struct no_os_uart_desc *desc);

/* Make stdio to use this UART. */
void no_os_uart_stdio(struct no_os_uart_desc *desc);

#endif // _NO_OS_UART_H_
