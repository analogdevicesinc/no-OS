/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max78000 examples.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#include "common_data.h"

/* Force linker to include max_uart_ops by taking its address */
volatile const void *_force_uart_ops_link = (const void *)&max_uart_ops;

struct no_os_uart_init_param uart_ip = {
	.device_id             = UART_DEVICE_ID,
	.asynchronous_rx       = false,
	.baud_rate             = UART_BAUDRATE,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = &max_uart_ops,
};
