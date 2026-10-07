/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   basic_example.c
 *   @brief  Basic UART example for the max78000 project.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#include <stdio.h>
#include "common_data.h"
#include "basic_example.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main(void)
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	while (1) {
		printf("Hello world!\n\r");
		no_os_mdelay(1000);
	}

	no_os_uart_remove(uart_desc);

	return 0;
}
