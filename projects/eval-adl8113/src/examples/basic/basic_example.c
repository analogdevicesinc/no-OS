/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for eval-adl8113 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
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

#include <stdbool.h>
#include "adl8113.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/**
 * @brief Handle button press with debounce and wait for release.
 * @param button - GPIO descriptor for the button.
 * @return true if button was pressed, false otherwise.
 */
static bool handle_button_press(struct no_os_gpio_desc *button)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(button, &val);
	if (ret)
		return false;

	if (val == 0) {
		/* Wait for debounce */
		no_os_mdelay(50);
		ret = no_os_gpio_get_value(button, &val);
		if (ret)
			return false;

		if (val == 0) {
			pr_info("Button pressed.\n");
			/* Wait for button release */
			while (val == 0) {
				ret = no_os_gpio_get_value(button, &val);
				if (ret)
					return false;
				no_os_mdelay(10);
			}
			no_os_mdelay(50);
			return true;
		}
	}
	return false;
}

/**
 * @brief Set LED state based on current mode.
 * @param led_red - Red LED GPIO descriptor.
 * @param led_green - Green LED GPIO descriptor.
 * @param led_blue - Blue LED GPIO descriptor.
 * @param mode - Current ADL8113 mode.
 * @return 0 in case of success, negative error code otherwise.
 */
static int set_led_state(struct no_os_gpio_desc *led_red,
			 struct no_os_gpio_desc *led_green,
			 struct no_os_gpio_desc *led_blue,
			 enum adl8113_mode mode)
{
	int ret;

	switch (mode) {
	case ADL8113_INTERNAL_AMPLIFIER:
		/* All LEDs on for internal amplifier */
		ret = no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		break;
	case ADL8113_INTERNAL_BYPASS:
		/* Green LED on for internal bypass */
		ret = no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		break;
	case ADL8113_EXTERNAL_BYPASS_A:
		/* Blue LED on for external bypass A */
		ret = no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		break;
	case ADL8113_EXTERNAL_BYPASS_B:
		/* Red LED on for external bypass B */
		ret = no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		break;
	default:
		break;
	}

	return 0;
}

/**
 * @brief Main function for basic example.
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main(void)
{
	int ret;
	struct adl8113_dev *device;
	struct no_os_gpio_desc *led_red;
	struct no_os_gpio_desc *led_green;
	struct no_os_gpio_desc *led_blue;
	struct no_os_gpio_desc *button1;
	struct no_os_gpio_desc *button2;
	struct no_os_uart_desc *uart_desc;
	enum adl8113_mode mode;
	bool running = true;

	/* Initialize UART */
	ret = no_os_uart_init(&uart_desc, &adl8113_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	/* Initialize LEDs */
	ret = no_os_gpio_get(&led_red, &led_red_ip);
	if (ret)
		goto error_uart;

	ret = no_os_gpio_get(&led_green, &led_green_ip);
	if (ret)
		goto error_led_green;

	ret = no_os_gpio_get(&led_blue, &led_blue_ip);
	if (ret)
		goto error_led_blue;

	/* Set LEDs as outputs, initially OFF (HIGH = OFF for active low LEDs) */
	ret = no_os_gpio_direction_output(led_red, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_button1;
	ret = no_os_gpio_direction_output(led_green, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_button1;
	ret = no_os_gpio_direction_output(led_blue, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_button1;

	/* Initialize buttons */
	ret = no_os_gpio_get(&button1, &button1_ip);
	if (ret)
		goto error_button1;

	ret = no_os_gpio_direction_input(button1);
	if (ret)
		goto error_button2;

	ret = no_os_gpio_get(&button2, &button2_ip);
	if (ret)
		goto error_button2;

	ret = no_os_gpio_direction_input(button2);
	if (ret)
		goto error_device;

	/* Initialize ADL8113 device */
	ret = adl8113_init(&device, &adl8113_param);
	if (ret)
		goto error_device;

	/* Get initial mode */
	ret = adl8113_get_mode(device, &mode);
	if (ret)
		goto error;

	/* Main loop: cycle through modes with SW2, exit with SW3 */
	while (running) {
		ret = set_led_state(led_red, led_green, led_blue, mode);
		if (ret)
			goto error;

		switch (mode) {
		case ADL8113_INTERNAL_AMPLIFIER:
			pr_info("Internal amplifier mode. SW2=cycle, SW3=exit.\n");
			if (handle_button_press(button2)) {
				running = false;
			} else if (handle_button_press(button1)) {
				mode = ADL8113_INTERNAL_BYPASS;
				ret = adl8113_set_mode(device, mode);
				if (ret)
					goto error;
			}
			break;

		case ADL8113_INTERNAL_BYPASS:
			pr_info("Internal bypass mode. SW2=cycle, SW3=exit.\n");
			if (handle_button_press(button2)) {
				running = false;
			} else if (handle_button_press(button1)) {
				mode = ADL8113_EXTERNAL_BYPASS_A;
				ret = adl8113_set_mode(device, mode);
				if (ret)
					goto error;
			}
			break;

		case ADL8113_EXTERNAL_BYPASS_A:
			pr_info("External bypass A mode. SW2=cycle, SW3=exit.\n");
			if (handle_button_press(button2)) {
				running = false;
			} else if (handle_button_press(button1)) {
				mode = ADL8113_EXTERNAL_BYPASS_B;
				ret = adl8113_set_mode(device, mode);
				if (ret)
					goto error;
			}
			break;

		case ADL8113_EXTERNAL_BYPASS_B:
			pr_info("External bypass B mode. SW2=cycle, SW3=exit.\n");
			if (handle_button_press(button2)) {
				running = false;
			} else if (handle_button_press(button1)) {
				mode = ADL8113_INTERNAL_AMPLIFIER;
				ret = adl8113_set_mode(device, mode);
				if (ret)
					goto error;
			}
			break;
		default:
			break;
		}
		no_os_mdelay(50);
	}

	pr_info("Exiting example.\n");

	/* Turn off all LEDs */
	no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
	no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
	no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);

	adl8113_remove(device);
	no_os_gpio_remove(button2);
	no_os_gpio_remove(button1);
	no_os_gpio_remove(led_blue);
	no_os_gpio_remove(led_green);
	no_os_gpio_remove(led_red);
	no_os_uart_remove(uart_desc);
	return 0;

error:
	pr_info("Error!\n");
	adl8113_remove(device);
error_device:
	no_os_gpio_remove(button2);
error_button2:
	no_os_gpio_remove(button1);
error_button1:
	no_os_gpio_remove(led_blue);
error_led_blue:
	no_os_gpio_remove(led_green);
error_led_green:
	no_os_gpio_remove(led_red);
error_uart:
	no_os_uart_remove(uart_desc);
	return ret;
}
