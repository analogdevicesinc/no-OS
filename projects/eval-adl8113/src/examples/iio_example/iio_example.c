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

#include "iio_adl8113.h"
#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "iio_app.h"

/**
 * @brief Callback context for LED updates.
 */
struct iio_example_cb_ctx {
	struct no_os_gpio_desc *led_red;
	struct no_os_gpio_desc *led_green;
	struct no_os_gpio_desc *led_blue;
	struct adl8113_iio_dev *iio_dev;
	enum adl8113_mode last_mode;
};

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
 * @brief Post-step callback to update LEDs when mode changes.
 * @param arg - Callback context.
 * @return 0 in case of success, negative error code otherwise.
 */
static int iio_post_step_cb(void *arg)
{
	struct iio_example_cb_ctx *ctx = arg;
	enum adl8113_mode mode;
	int ret;

	ret = adl8113_get_mode(ctx->iio_dev->adl8113_dev, &mode);
	if (ret)
		return ret;

	if (mode != ctx->last_mode) {
		ret = set_led_state(ctx->led_red, ctx->led_green,
				    ctx->led_blue, mode);
		if (ret)
			return ret;
		ctx->last_mode = mode;
	}

	return 0;
}

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
	struct no_os_gpio_desc *led_red;
	struct no_os_gpio_desc *led_green;
	struct no_os_gpio_desc *led_blue;
	struct iio_example_cb_ctx cb_ctx;
	enum adl8113_mode mode;

	/* Initialize LEDs */
	ret = no_os_gpio_get(&led_red, &led_red_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&led_green, &led_green_ip);
	if (ret)
		goto error_led_green;

	ret = no_os_gpio_get(&led_blue, &led_blue_ip);
	if (ret)
		goto error_led_blue;

	/* Set LEDs as outputs, initially OFF (HIGH = OFF for active low LEDs) */
	ret = no_os_gpio_direction_output(led_red, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_iio;
	ret = no_os_gpio_direction_output(led_green, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_iio;
	ret = no_os_gpio_direction_output(led_blue, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_iio;

	/* Initialize ADL8113 IIO device */
	adl8113_iio_ip.adl8113_init_param = &adl8113_param;
	adl8113_iio_ip.has_external_bypass_a = true;
	adl8113_iio_ip.external_bypass_a_gain_db = 1;
	adl8113_iio_ip.has_external_bypass_b = true;
	adl8113_iio_ip.external_bypass_b_gain_db = 2;

	ret = adl8113_iio_init(&adl8113_iio_desc, &adl8113_iio_ip);
	if (ret) {
		pr_info("Error initializing ADL8113 IIO device\n");
		goto error_iio;
	}

	/* Set initial LED state based on current mode */
	ret = adl8113_get_mode(adl8113_iio_desc->adl8113_dev, &mode);
	if (ret)
		goto error_app;
	ret = set_led_state(led_red, led_green, led_blue, mode);
	if (ret)
		goto error_app;

	/* Setup callback context */
	cb_ctx.led_red = led_red;
	cb_ctx.led_green = led_green;
	cb_ctx.led_blue = led_blue;
	cb_ctx.iio_dev = adl8113_iio_desc;
	cb_ctx.last_mode = mode;

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
	app_init_param.post_step_callback = iio_post_step_cb;
	app_init_param.arg = &cb_ctx;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("Error initializing IIO app\n");
		goto error_app;
	}

	ret = iio_app_run(app);

	iio_app_remove(app);

error_app:
	adl8113_iio_remove(adl8113_iio_desc);
error_iio:
	no_os_gpio_remove(led_blue);
error_led_blue:
	no_os_gpio_remove(led_green);
error_led_green:
	no_os_gpio_remove(led_red);

	return ret;
}
