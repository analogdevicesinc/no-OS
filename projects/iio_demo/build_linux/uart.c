/***************************************************************************//**
 *   @file   generic/uart.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "error.h"
#include "uart.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Submit reading buffer to the UART driver.
 *
 * Buffer is used until bytes_number bytes are read.
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer where data will be read
 * @param bytes_number:	Number of bytes to be read.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_read_nonblocking(struct uart_desc *desc, uint8_t *data,
			      uint32_t bytes_number)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Submit writting buffer to the UART driver.
 *
 * Data from the buffer is sent over the UART, the function returns imediatly.
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer where data will be written
 * @param bytes_number:	Number of bytes to be written.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_write_nonblocking(struct uart_desc *desc, const uint8_t *data,
			       uint32_t bytes_number)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (data) {
		// Unused variable - fix compiler warning
	}

	if (bytes_number) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (param) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by uart_init().
 * @param desc - The UART descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_remove(struct uart_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Get number of UART errors.
 * @param desc - The UART descriptor.
 * @return number of errors.
 */
uint32_t uart_get_errors(struct uart_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}
