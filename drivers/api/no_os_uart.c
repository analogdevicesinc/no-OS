/***************************************************************************//**
 *   @file   no_os_uart.c
 *   @brief  Implementation of the UART Interface
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include "no_os_uart.h"
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_mutex.h"
#include "no_os_util.h"

/**
 * @brief - UART mutex
*/
static void *uart_mutex_table[UART_MAX_NUMBER + 1];

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_init(struct no_os_uart_desc **desc,
			struct no_os_uart_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops
	    || param->device_id >= NO_OS_ARRAY_SIZE(uart_mutex_table))
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	no_os_mutex_init(&(uart_mutex_table[param->device_id]));
	(*desc)-> mutex = uart_mutex_table[param->device_id];

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_remove(struct no_os_uart_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	no_os_mutex_remove(desc->mutex);
	uart_mutex_table[desc->device_id] = NULL;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Check if errors occurred on UART.
 * @param desc - The UART descriptor.
 * @return number of errors in case of success, error code otherwise.
 */
uint32_t no_os_uart_get_errors(struct no_os_uart_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->get_errors)
		return -ENOSYS;

	return desc->platform_ops->get_errors(desc);
}

/**
 * @brief Read data from UART.
 * @param desc - The UART descriptor.
 * @param data - The buffer with the received data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_read(struct no_os_uart_desc *desc,
			uint8_t *data,
			uint32_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->read)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);
	ret = desc->platform_ops->read(desc, data, bytes_number);
	no_os_mutex_unlock(desc->mutex);

	return ret;
}

/**
 * @brief Write to UART.
 * @param desc - The UART descriptor.
 * @param data - The buffer with the transmitted data.
 * @param bytes_number - Number of bytes to write.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_write(struct no_os_uart_desc *desc,
			 const uint8_t *data,
			 uint32_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->write)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);
	ret = desc->platform_ops->write(desc, data, bytes_number);
	no_os_mutex_unlock(desc->mutex);

	return ret;
}

/**
 * @brief Read data from UART non-blocking.
 * @param desc - The UART descriptor.
 * @param data - The buffer with the received data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_read_nonblocking(struct no_os_uart_desc *desc,
				    uint8_t *data,
				    uint32_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->read_nonblocking)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);
	ret = desc->platform_ops->read_nonblocking(desc, data, bytes_number);
	no_os_mutex_unlock(desc->mutex);

	return ret;
}

/**
 * @brief Write to UART non-blocking.
 * @param desc - The UART descriptor.
 * @param data - The buffer with the transmitted data.
 * @param bytes_number - Number of bytes to write.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_write_nonblocking(struct no_os_uart_desc *desc,
				     const uint8_t *data,
				     uint32_t bytes_number)
{
	int32_t ret;

	if (!desc || !desc->platform_ops || !data)
		return -EINVAL;

	if (!desc->platform_ops->write_nonblocking)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);
	ret = desc->platform_ops->write_nonblocking(desc, data, bytes_number);
	no_os_mutex_unlock(desc->mutex);

	return ret;
}


void __attribute__((weak)) no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	/* This can optionally be implemented under drivers/platform.
	 * It does nothing if unimplemented. */
}
