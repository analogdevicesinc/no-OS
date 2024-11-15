/***************************************************************************//**
 *   @file   generic/generic_uart.c
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			uint32_t bytes_number)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);

	return 0;
}

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			 uint32_t bytes_number)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);

	return 0;
}

/**
 * @brief Submit reading buffer to the UART driver.
 *
 * Buffer is used until bytes_number bytes are read.
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer where data will be read
 * @param bytes_number:	Number of bytes to be read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_read_nonblocking(struct no_os_uart_desc *desc, uint8_t *data,
				    uint32_t bytes_number)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);

	return 0;
}

/**
 * @brief Submit writting buffer to the UART driver.
 *
 * Data from the buffer is sent over the UART, the function returns imediatly.
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer where data will be written
 * @param bytes_number:	Number of bytes to be written.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_write_nonblocking(struct no_os_uart_desc *desc,
				     const uint8_t *data,
				     uint32_t bytes_number)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);

	return 0;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_init(struct no_os_uart_desc **desc,
			struct no_os_uart_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_remove(struct no_os_uart_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Get number of UART errors.
 * @param desc - The UART descriptor.
 * @return number of errors.
 */
uint32_t no_os_uart_get_errors(struct no_os_uart_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}
