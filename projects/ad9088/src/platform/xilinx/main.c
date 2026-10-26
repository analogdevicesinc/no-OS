/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of ad9088 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef DMA_EXAMPLE
#include "dma_example.h"
#endif

/**
 * @brief Main function execution for xilinx platform.
 * @return ret - Result of the enabled examples execution.
*/
int main(void)
{
	int ret = -EINVAL;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &platform_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef DMA_EXAMPLE
	ret = dma_example_main();
#endif

	return ret;
}
