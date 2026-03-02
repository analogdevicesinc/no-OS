/***************************************************************************//**
 * @file iio_example.c
 * @brief IIO example source file for eval-adl8113 project.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
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

#include <stdio.h>

#include "iio_adl8113.h"
#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "iio_app.h"

/**
 * @brief Set LED state based on current mode.
 * @param[in] led_red   - Red LED GPIO descriptor.
 * @param[in] led_green - Green LED GPIO descriptor.
 * @param[in] led_blue  - Blue LED GPIO descriptor.
 * @param[in] mode      - Current ADL8113 mode.
 */
static void set_led_state(struct no_os_gpio_desc *led_red,
			  struct no_os_gpio_desc *led_green,
			  struct no_os_gpio_desc *led_blue,
			  enum adl8113_mode mode)
{
	switch (mode) {
	case ADL8113_INTERNAL_AMPLIFIER:
		/* All LEDs on for internal amplifier (active-low LEDs). */
		no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
		no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
		no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);
		break;

	case ADL8113_INTERNAL_BYPASS:
		/* Green LED on for internal bypass. */
		no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(led_green, NO_OS_GPIO_LOW);
		no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);
		break;

	case ADL8113_EXTERNAL_BYPASS_A:
		/* Blue LED on for external bypass A. */
		no_os_gpio_set_value(led_red, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(led_blue, NO_OS_GPIO_LOW);
		break;

	case ADL8113_EXTERNAL_BYPASS_B:
		/* Red LED on for external bypass B. */
		no_os_gpio_set_value(led_red, NO_OS_GPIO_LOW);
		no_os_gpio_set_value(led_green, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(led_blue, NO_OS_GPIO_HIGH);
		break;

	default:
		break;
	}
}

/**
 * @brief Optional hook called after a successful path/mode change via IIO.
 * @param[in] mode - The newly selected ADL8113 mode.
 *
 * Overriding the weak hook here keeps LED updates decoupled from the IIO driver.
 */
void adl8113_iio_on_mode_change(enum adl8113_mode mode)
{
	/* These are created in example_main(). Guard against NULL. */
	extern struct no_os_gpio_desc *g_led_red;
	extern struct no_os_gpio_desc *g_led_green;
	extern struct no_os_gpio_desc *g_led_blue;

	if (g_led_red && g_led_green && g_led_blue)
		set_led_state(g_led_red, g_led_green, g_led_blue, mode);
}

/* LED descriptors used by the mode-change hook above. */
struct no_os_gpio_desc *g_led_red;
struct no_os_gpio_desc *g_led_green;
struct no_os_gpio_desc *g_led_blue;

/**
 * @brief Main function for IIO example.
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main(void)
{
	int ret;
	struct adl8113_iio_dev *adl8113_iio_desc;
	struct adl8113_iio_init_param adl8113_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	enum adl8113_mode mode;

	printf("ADL8113 IIO Example\n");

	ret = no_os_gpio_get(&g_led_red, &led_red_ip);
	if (ret) {
		printf("Error initializing red LED\n");
		return ret;
	}

	ret = no_os_gpio_get(&g_led_green, &led_green_ip);
	if (ret) {
		printf("Error initializing green LED\n");
		goto error_led_green;
	}

	ret = no_os_gpio_get(&g_led_blue, &led_blue_ip);
	if (ret) {
		printf("Error initializing blue LED\n");
		goto error_led_blue;
	}

	/* Set LEDs as outputs, initially OFF (HIGH = OFF for active low LEDs) */
	ret = no_os_gpio_direction_output(g_led_red, NO_OS_GPIO_HIGH);
	if (ret) {
		printf("Error setting red LED direction\n");
		goto error_iio;
	}
	ret = no_os_gpio_direction_output(g_led_green, NO_OS_GPIO_HIGH);
	if (ret) {
		printf("Error setting green LED direction\n");
		goto error_iio;
	}
	ret = no_os_gpio_direction_output(g_led_blue, NO_OS_GPIO_HIGH);
	if (ret) {
		printf("Error setting blue LED direction\n");
		goto error_iio;
	}

	adl8113_iio_ip.adl8113_init_param = &adl8113_param;
	adl8113_iio_ip.has_external_bypass_a = true;
	adl8113_iio_ip.external_bypass_a_gain_db = 1;

	adl8113_iio_ip.has_external_bypass_b = true;
	adl8113_iio_ip.external_bypass_b_gain_db = 2;

	ret = adl8113_iio_init(&adl8113_iio_desc, &adl8113_iio_ip);
	if (ret) {
		printf("Error initializing ADL8113 IIO device\n");
		goto error_iio;
	}

	/* Set initial LED state based on current mode */
	ret = adl8113_get_mode(adl8113_iio_desc->adl8113_dev, &mode);
	if (!ret)
		set_led_state(g_led_red, g_led_green, g_led_blue, mode);

	printf("ADL8113 IIO device initialized\n");

	/* Setup IIO app */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adl8113",
			.dev = adl8113_iio_desc,
			.dev_descriptor = adl8113_iio_desc->iio_dev,
			.read_buff = NULL,
			.write_buff = NULL,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adl8113_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		printf("Error initializing IIO app\n");
		goto error_app;
	}

	printf("IIO server started.\nConnect with libiio.\n");

	ret = iio_app_run(app);

	iio_app_remove(app);

error_app:
	adl8113_iio_remove(adl8113_iio_desc);
error_iio:
	no_os_gpio_remove(g_led_blue);
error_led_blue:
	no_os_gpio_remove(g_led_green);
error_led_green:
	no_os_gpio_remove(g_led_red);

	if (ret)
		printf("Error!\n");

	return ret;
}
