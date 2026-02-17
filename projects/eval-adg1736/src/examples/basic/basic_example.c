/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for eval-adg1736 project.
 *   @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "adg1736.h"

int example_main(void)
{
	struct adg1736_dev *dev;
	struct no_os_uart_desc *uart;
	struct no_os_gpio_desc *led_green, *led_red;
	struct no_os_gpio_desc *gpio_d, *gpio_s;
	enum adg1736_state target_state;
	enum adg1736_switch test_sw;
	uint8_t s_val;
	int pass;
	int ret;

	ret = no_os_uart_init(&uart, &adg1736_uart_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(uart);

	pr_info("\r\n=== ADG1736 Switch Test ===\r\n");
	pr_info("Testing SW%d, %s side\r\n", TEST_SWITCH + 1,
		TEST_SIDE == 0 ? "A" : "B");
	pr_info("Pins: IN=P1.6, EN=P1.7, D=P1.8, S=P1.9\r\n\r\n");

	/* Initialize LEDs */
	ret = no_os_gpio_get(&led_green, &led_green_ip);
	if (ret)
		goto err_uart;
	ret = no_os_gpio_direction_output(led_green, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_led_green;

	ret = no_os_gpio_get(&led_red, &led_red_ip);
	if (ret)
		goto err_led_green;
	ret = no_os_gpio_direction_output(led_red, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_led_red;

	/* Initialize test GPIOs */
	ret = no_os_gpio_get(&gpio_d, &gpio_d_ip);
	if (ret)
		goto err_led_red;
	ret = no_os_gpio_direction_output(gpio_d, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_d;

	ret = no_os_gpio_get(&gpio_s, &gpio_s_ip);
	if (ret)
		goto err_d;
	ret = no_os_gpio_direction_input(gpio_s);
	if (ret)
		goto err_s;

	/* Initialize ADG1736 driver */
	ret = adg1736_init(&dev, &adg1736_ip);
	if (ret)
		goto err_s;

	/* Determine target state and switch based on TEST_SIDE and TEST_SWITCH */
	target_state = (TEST_SIDE == 0) ? ADG1736_CONNECT_A : ADG1736_CONNECT_B;
	test_sw = (TEST_SWITCH == 0) ? ADG1736_SW1 : ADG1736_SW2;

	pr_info("Running continuous test...\r\n\r\n");

	while (1) {
		/* Test 1: Set to target position, S should see D (HIGH) */
		ret = adg1736_set_switch_state(dev, test_sw, target_state);
		if (ret)
			break;
		no_os_mdelay(10);
		ret = no_os_gpio_get_value(gpio_s, &s_val);
		if (ret)
			break;
		pr_info("IN->%c: S=%d (expect 1) %s\r\n",
			TEST_SIDE == 0 ? 'A' : 'B',
			s_val, s_val == 1 ? "[PASS]" : "[FAIL]");
		pass = (s_val == 1);

		no_os_mdelay(1000);

		/* Test 2: Set to opposite position, S should be disconnected */
		ret = adg1736_set_switch_state(dev, test_sw,
					       target_state == ADG1736_CONNECT_A ?
					       ADG1736_CONNECT_B : ADG1736_CONNECT_A);
		if (ret)
			break;
		no_os_mdelay(10);
		ret = no_os_gpio_get_value(gpio_s, &s_val);
		if (ret)
			break;
		pr_info("IN->%c: S=%d (expect 0) %s\r\n",
			TEST_SIDE == 0 ? 'B' : 'A',
			s_val, s_val == 0 ? "[PASS]" : "[FAIL]");
		pass = pass && (s_val == 0);

		/* Update LEDs */
		if (pass) {
			no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
			no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
			pr_info("=== PASS ===\r\n\r\n");
		} else {
			no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
			pr_info("=== FAIL ===\r\n\r\n");
		}

		no_os_mdelay(2000);
	}

	adg1736_remove(dev);
err_s:
	no_os_gpio_remove(gpio_s);
err_d:
	no_os_gpio_remove(gpio_d);
err_led_red:
	no_os_gpio_remove(led_red);
err_led_green:
	no_os_gpio_remove(led_green);
err_uart:
	no_os_uart_remove(uart);
	return ret;
}
