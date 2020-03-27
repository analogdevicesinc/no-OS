/***************************************************************************//**
 *   @file   uart.h
 *   @brief  Header file of UART interface.
 *   @author Cristian Pop (cristian.pop@analog.com)
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
#ifndef UART_H_
#define UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct uart_init_param
 * @brief Structure holding the parameters for UART initialization
 */
struct uart_init_param {
	/** UART Device ID */
	uint8_t	device_id;
	/** UART Baud Rate */
	uint32_t 	baud_rate;
	/** UART extra parameters (device specific) */
	void 		*extra;
};

/**
 * @struct uart_desc
 * @brief Stucture holding the UART descriptor.
 */
struct uart_desc {
	/** UART Device ID */
	uint8_t 	device_id;
	/** UART Baud Rate */
	uint32_t 	baud_rate;
	/** Callback to be called when an operation is done (optional) */
	void		(*callback)(void *callback_ctx, uint32_t event,
				    void *extra);
	/** Parameter to be passed to the callback as app_param */
	void		*callback_ctx;
	/** UART extra parameters (device specific) */
	void 		*extra;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read data from UART. Blocking function */
int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number);

/* Write data to UART. Blocking function */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number);

/* Read data from UART. Non blocking function */
int32_t uart_read_nonblocking(struct uart_desc *desc, uint8_t *data,
			      uint32_t bytes_number);

/* Write data to UART. Non blocking function*/
int32_t uart_write_nonblocking(struct uart_desc *desc, const uint8_t *data,
			       uint32_t bytes_number);

/* Initialize the UART communication peripheral. */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param);

/* Free the resources allocated by uart_init(). */
int32_t uart_remove(struct uart_desc *desc);

/* Check if UART errors occurred. */
uint32_t uart_get_errors(struct uart_desc *desc);

#endif /* UART_H_ */
