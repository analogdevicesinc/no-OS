/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of the basic example for RTC.
 *   @author Francis Roi Manabat (francisroi.manabat@analog.com)
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
#include "no_os_print_log.h"
#include "no_os_irq.h"
#include "no_os_rtc.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"
#include "maxim_uart_stdio.h"
#include "no_os_util.h"
#include "basic_example.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_irq.h"
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/

static int32_t irq_flag;
int32_t timer_cnt = 0;
static int32_t button_pressed;
const struct no_os_uart_desc *uart_desc;
const struct no_os_gpio_desc *led;
const struct no_os_rtc_desc *rtc_desc;
const struct no_os_irq_ctrl_desc *rtc_irq_desc;
const struct no_os_gpio_desc *button;

void RTC_CB(void)
{
	/* Set the flag to indicate that the RTC interrupt has occurred	*/
	irq_flag = 1;
}
int basic_example_main()
{
	/* Initialize UART */
	int ret = 0;
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		return ret;
	}

	/* Set UART as standard input/output */
	no_os_uart_stdio(uart_desc);

	pr_info("UART initialized successfully.\n");

	/* Initialize the RTC */
	ret = no_os_rtc_init(&rtc_desc, &rtc_ip);
	if (ret) {
		pr_info("RTC initialization failed.\n");
		return ret;
	}

	/* Initialize the GPIO for LED */
	ret = no_os_gpio_get(&led, &led_ip);
	if (ret) {
		pr_info("LED GPIO initialization failed.\n");
		return ret;
	}
	/* Initialize the GPIO for button */
	ret = no_os_gpio_get(&button, &button_ip);
	if (ret) {
		pr_info("Button GPIO initialization failed.\n");
		return ret;
	}

	/* Set the LED to output mode */
	ret = no_os_gpio_direction_output(led, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_info("LED GPIO direction set failed.\n");
		return ret;
	}

	/* Set the button to input mode */
	ret = no_os_gpio_direction_input(button);
	if (ret) {
		pr_info("Button GPIO direction set failed.\n");
		return ret;
	}

	/* Initialize the RTC IRQ */
	ret = no_os_irq_ctrl_init(&rtc_irq_desc, &rtc_irq_ip);
	if (ret) {
		pr_info("RTC IRQ controller initialization failed.\n");
		return ret;
	}

	/* */
	ret = no_os_irq_enable(rtc_irq_desc, RTC_IRQ_ID);
	if (ret) {
		pr_info("RTC IRQ enable failed.\n");
		return ret;
	}

	/* Register the RTC callback function */
	struct no_os_callback_desc rtc_callback = {
		.callback = RTC_CB,
		.ctx = NULL,
		.event = NO_OS_EVT_RTC,
		.peripheral = NO_OS_RTC_IRQ,
		.handle = NULL,
	};

	/* Set Time of Day Alarm at 10 Seconds */
	ret = no_os_rtc_set_irq_time(rtc_desc, 10);

	ret = no_os_irq_register_callback(rtc_irq_desc, RTC_IRQ_ID, &rtc_callback);
	if (ret) {
		pr_info("RTC IRQ callback registration failed.\n");
		return ret;
	}
	/* Start RTC Counter */
	ret = no_os_rtc_start(rtc_desc);
	if (ret) {
		pr_info("RTC start failed.\n");
		return ret;
	}

	pr_info("RTC started successfully.\n");

	irq_flag = 0;
	while (1) {
		if (irq_flag) {
			no_os_rtc_set_cnt(rtc_desc, 0);
			no_os_gpio_set_value(led, NO_OS_GPIO_LOW);
			no_os_mdelay(5000);
			irq_flag = 0;
			if (ret) {
				pr_info("RTC IRQ callback registration failed.\n");
				return ret;
			}
		}

		else {
			no_os_gpio_set_value(led, NO_OS_GPIO_HIGH);
		}
		/* Check if the button is pressed */
		ret = no_os_gpio_get_value(button, &button_pressed);
		if (ret) {
			pr_info("Button GPIO get value failed.\n");
			return ret;
		}

		if (button_pressed == NO_OS_GPIO_LOW) {
			pr_info("Button pressed!\n");
			ret = no_os_rtc_get_cnt(rtc_desc, &timer_cnt);
			if (ret) {
				pr_info("RTC get count failed.\n");
				return ret;
			}
			pr_info("Current RTC count: %d\n", timer_cnt);

			/* Debounce Delay */
			no_os_mdelay(1000);
		}

	}

	pr_info("Basic example started.\n");


	return 0;
}
