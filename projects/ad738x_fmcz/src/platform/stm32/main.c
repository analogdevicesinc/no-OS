/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-ad738x project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#elif defined(IIO_EXAMPLE)
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
#define AD738X_ADC_REF_VOLTAGE 3300

int main()
{
	int ret;
	struct no_os_uart_desc *uart;

	stm32_init();
	ret = no_os_uart_init(&uart, &ad738x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#elif defined(IIO_EXAMPLE)

	ret = iio_example_main();
#else
#error At least one example has to be selected using y value in Makefile.
#endif
}
