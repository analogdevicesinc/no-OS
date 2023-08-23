/***************************************************************************//**
 *   @file   no_os_uart.c
 *   @brief  Implementation of the UART Interface
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
