/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl367z project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef TRIPLE_TAP_EXAMPLE
#include "triple_tap_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

#ifdef TRIPLE_TAP_EXAMPLE
	/* NVIC Interrupt Controller specific for Maxim platform. */
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (ret)
		goto uart_remove;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto irq_ctrl_remove;

	no_os_uart_stdio(uart_desc);

	ret = triple_tap_example_main();

uart_remove:
	no_os_uart_remove(uart_desc);
irq_ctrl_remove:
	no_os_irq_ctrl_remove(nvic_desc);
#endif

#ifdef DUMMY_EXAMPLE
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = dummy_example_main();

	no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#if (BASIC_EXAMPLE + IIO_EXAMPLE + TRIPLE_TAP_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable ony one example and rebuild thhe project.
#endif

	return ret;
}
