/***************************************************************************//**
 *   @file   eit.c
 *   @brief  Basic EIT (UART console) example for cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include <stdio.h>
#include "common_data.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "ad5940.h"
#include "app.h"

struct no_os_uart_desc *uart;
struct no_os_i2c_desc *i2c;

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int example_main()
{
	int ret;
	struct no_os_irq_ctrl_desc *nvic;
	struct no_os_irq_ctrl_desc *gic;

	ret = no_os_uart_init(&uart, &cn0565_uart_ip);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart);

	printf("Hello!\n");

	ret = no_os_i2c_init(&i2c, &cn0565_i2c_ip);
	if (ret)
		goto error;

	/* interrupt controller */
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = IRQ_OPS,
	};
	ret = no_os_irq_ctrl_init(&nvic, &nvic_ip);
	if (ret < 0)
		return ret;

	/* gpio interrupt controller */
	struct no_os_irq_init_param gic_ip = {
		.irq_ctrl_id = GPIO_IRQ_CTRL_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};
	ret = no_os_irq_ctrl_init(&gic, &gic_ip);
	if (ret < 0)
		return ret;

	/* callback */
	struct no_os_callback_desc int_cb = {
		.callback = ad5940_int_callback,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	ret = no_os_irq_register_callback(gic, INT_IRQn, &int_cb);
	if (ret < 0)
		return ret;

	ret = no_os_irq_trigger_level_set(gic, INT_IRQn, NO_OS_IRQ_EDGE_FALLING);
	if (ret < 0)
		return ret;

	ret = no_os_irq_enable(gic, INT_IRQn);
	if (ret < 0)
		return ret;

	ret = app_main(i2c, &cn0565_ad5940_ip);
	if (ret < 0)
		goto error;

	printf("Bye!\n");
	return 0;
error:
	printf("Bye! (%d)\n", ret);
	return ret;
}
