/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of ad5460 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef CHANNEL_OUTPUT_EXAMPLE
#include "channel_output_example.h"
#endif

#ifdef VOLTAGE_OUTPUT_EXAMPLE
#include "voltage_output_example.h"
#endif

#ifdef CURRENT_OUTPUT_EXAMPLE
#include "current_output_example.h"
#endif

/***************************************************************************//**
 * @brief Main function for Mbed platform.
 *
 * @return ret - Result of the enabled examples.
*******************************************************************************/

int main()
{
	int ret;
	ad5460_ip.spi_ip = ad5460_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
	no_os_uart_stdio(uart);
	ret = basic_example_main();
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
#endif

#ifdef CHANNEL_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = channel_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#ifdef VOLTAGE_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = voltage_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#ifdef CURRENT_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = current_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#if (BASIC_EXAMPLE+CHANNEL_OUTPUT_EXAMPLE+VOLTAGE_OUTPUT_EXAMPLE+CURRENT_OUTPUT_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and re-build the project.
#endif
	return 0;
}
