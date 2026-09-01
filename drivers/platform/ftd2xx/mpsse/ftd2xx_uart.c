/*******************************************************************************
 *   @file   ftd2xx_uart.c
 *   @brief  Implementation of UART driver for ftd2xx platform.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "ftd2xx_uart.h"

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int ftd2xx_uart_init(struct no_os_uart_desc **desc,
			    const struct no_os_uart_init_param *param)
{
	*desc = no_os_calloc(1, sizeof(struct no_os_uart_desc));
	if (!desc)
		return -ENOMEM;

	return 0;
};

/**
 * @brief Free the resources allocated by stm32_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
static int ftd2xx_uart_remove(struct no_os_uart_desc *desc)
{
	free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
static int ftd2xx_uart_write(struct no_os_uart_desc *desc,
			     const uint8_t *data,
			     uint32_t bytes_number)
{
	uint32_t count = 0;

	while (count < bytes_number) {
		printf("%c", data[count]);
	}

	return 0;
};

/**
 * @brief ftd2xx specific UART platform ops structure
 */
const struct no_os_uart_platform_ops ftd2xx_uart_ops = {
	.init = &ftd2xx_uart_init,
	.write = &ftd2xx_uart_write,
	.remove = &ftd2xx_uart_remove
};
