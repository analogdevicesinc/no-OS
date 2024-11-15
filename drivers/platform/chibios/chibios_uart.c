/***************************************************************************//**
 *   @file   chibios/chibios_uart.c
 *   @brief  Source file of UART driver for chibios
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_alloc.h"
#include "chibios_uart.h"

#if (HAL_USE_UART==TRUE)

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc  - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t chibios_uart_init(struct no_os_uart_desc **desc,
				 struct no_os_uart_init_param *param)
{
	struct chibios_uart_desc *chibios_uart;
	struct no_os_uart_desc *descriptor;
	int ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	descriptor = (struct no_os_uart_desc *) no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;
	chibios_uart = (struct chibios_uart_desc *) no_os_calloc(1,
			sizeof(*chibios_uart));

	if (!chibios_uart) {
		ret = -ENOMEM;
		goto error_2;
	}

	chibios_uart->huart = ((struct chibios_uart_init_param *)(param->extra))->huart;
	chibios_uart->sdconfig = ((struct chibios_uart_init_param *)(
					  param->extra))->sdconfig;

	ret = sdStart(chibios_uart->huart, chibios_uart->sdconfig);

	if (ret) {
		ret = -ENOMEM;
		goto error_1;
	}

	descriptor->extra = chibios_uart;
	*desc = descriptor;

	return 0;

error_1:
	no_os_free(chibios_uart);
error_2:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by chibios_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t chibios_uart_remove(struct no_os_uart_desc *desc)
{
	struct chibios_uart_desc *chibios_uart;

	if (!desc)
		return -EINVAL;

	chibios_uart = desc->extra;
	sdStop(chibios_uart->huart);

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t chibios_uart_write(struct no_os_uart_desc *desc,
				  const uint8_t *data,
				  uint32_t bytes_number)
{
	struct chibios_uart_desc *chibios_uart;
	uint32_t bytes_sent = 0;
	int ret = 0;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	chibios_uart = desc->extra;

	while (bytes_sent < bytes_number) {
		ret = streamPut((BaseSequentialStream *)(chibios_uart->huart),
				*(data + bytes_sent));
		if (ret != STM_OK)
			return -EIO;
		bytes_sent++;
	}

	return bytes_sent;
}

/**
 * @brief Read data from UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, error code otherwise.
 */
static int32_t chibios_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
				 uint32_t bytes_number)
{
	struct chibios_uart_desc *chibios_uart;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	chibios_uart = desc->extra;

	return	streamRead((BaseSequentialStream *)(chibios_uart->huart), data,
			   bytes_number);
}

/**
 * @brief chibios platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops chibios_uart_ops = {
	.init = &chibios_uart_init,
	.read = &chibios_uart_read,
	.write = &chibios_uart_write,
	.remove = &chibios_uart_remove
};

#endif
