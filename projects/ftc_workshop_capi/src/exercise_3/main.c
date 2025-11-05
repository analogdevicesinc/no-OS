/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#include "capi/capi_i2c.h"
#include "capi/capi_gpio.h"
#include "capi/capi_uart.h"
#include "max20303_capi.h"
#include "capi_i2c_bitbang.h"
#include "ssd_1306_capi.h"
#include "display.h"

#include "platform.h"

/* Global CAPI handles */
static struct capi_i2c_controller_handle *i2c_controller;
static struct capi_i2c_bitbang_handle *bitbang_i2c;
static struct capi_gpio_port_handle *gpio_port0;

static struct capi_i2c_device ssd_1306_capi_i2c_dev;

static struct capi_gpio_port_config gpio_port0_config = {
	.identifier = 0,
	.num_pins = 32,
	.ops = &maxim_capi_gpio_ops,
};

/* SSD1306 OLED Display Setup using CAPI I2C bitbang */
static ssd_1306_capi_extra oled_display_extra = {
	.i2c_desc = &ssd_1306_capi_i2c_dev,
	.comm_type = SSD1306_I2C,
};

static struct display_init_param oled_display_ini_param = {
	.cols_nb = 128,
	.rows_nb = 64,
	.controller_ops = &ssd1306_ops,
	.extra = &oled_display_extra,
};

static struct display_dev *oled_display;

/**
 * @brief Initialize the display
 * @return 0 on success, negative error code otherwise
 */
static int init_display(void)
{
	struct capi_gpio_pin sda_pin;
	struct capi_gpio_pin scl_pin;
	struct capi_i2c_bitbang_extra bitbang_extra;
	struct capi_i2c_config bitbang_config;
	int ret;

	/* Setup GPIO pins for I2C bitbang */
	sda_pin.port_handle = gpio_port0;
	sda_pin.number = 30;
	sda_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

	scl_pin.port_handle = gpio_port0;
	scl_pin.number = 31;
	scl_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

	bitbang_config.ops = &capi_i2c_bitbang_ops;
	bitbang_config.identifier = 0;
	bitbang_config.clk_freq_hz = 400000;
	bitbang_config.initiator = true;
	bitbang_config.address = 0;
	bitbang_config.device = NULL;
	bitbang_config.dma_handle = NULL;
	bitbang_config.extra = &bitbang_extra;

	ret = capi_gpio_port_init(&gpio_port0, &gpio_port0_config);
	if (ret)
		return ret;

	sda_pin.port_handle = gpio_port0;
	scl_pin.port_handle = gpio_port0;

	/* Configure I2C bitbang */
	bitbang_extra.sda_pin = sda_pin;
	bitbang_extra.scl_pin = scl_pin;
	bitbang_extra.pull_type = CAPI_I2C_BITBANG_PULL_EXTERNAL;
	bitbang_extra.timeout_us = 100000;

	ret = capi_i2c_init((struct capi_i2c_controller_handle **)&bitbang_i2c, &bitbang_config);
	if (ret) {
		printf("Failed to initialize I2C bitbang: %d\n", ret);
		return ret;
	}

	ssd_1306_capi_i2c_dev.address = 0x3C;
	ssd_1306_capi_i2c_dev.speed = 400000;
	ssd_1306_capi_i2c_dev.controller = bitbang_i2c;

	oled_display_extra.i2c_desc = &ssd_1306_capi_i2c_dev;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display: %d\n", ret);
		return ret;
	}

	display_clear(oled_display);
	display_on(oled_display);

	return 0;
}

int main()
{
	struct capi_uart_handle *uart = CAPI_UART_HANDLE;
	struct max20303_capi_desc *max20303;
	uint32_t battery_uv;
	uint32_t battery_mv;
	char voltage_str[16];
	int ret;

	/* Initialize UART for printf output */
	struct capi_uart_config uart_config = {
		.identifier = 0,
		.clk_freq_hz = 115200,
		.ops = &maxim_capi_uart_ops,
	};

	no_os_init();

	ret = capi_uart_init(&uart, &uart_config);
	if (ret) {
		return ret;
	}

	capi_uart_stdio(uart);
	printf("MAX20303 Battery Monitor Exercise 3\n");

	/* Initialize display */
	ret = init_display();
	if (ret) {
		printf("Display initialization failed: %d\n", ret);
		return ret;
	}

	printf("Display initialized\n");

	/* Initialize I2C controller for MAX20303 */
	struct capi_i2c_config i2c_config = {
		.identifier = 1,
		.initiator = 1,
		.clk_freq_hz = 400000,
		.ops = &maxim_capi_i2c_ops,
	};

	ret = capi_i2c_init(&i2c_controller, &i2c_config);
	if (ret) {
		printf("I2C initialization failed: %d\n", ret);
		return ret;
	}

	/* Configure MAX20303 I2C devices */
	struct capi_i2c_device max20303_i2c_dev = {
		.controller = i2c_controller,
		.address = MAX20303_I2C_ADDR,
		.b10addr = false,
		.speed = CAPI_I2C_SPEED_FAST,
	};

	struct capi_i2c_device max20303_fg_i2c_dev = {
		.controller = i2c_controller,
		.address = MAX20303_FG_I2C_ADDR,
		.b10addr = false,
		.speed = CAPI_I2C_SPEED_FAST,
	};

	struct max20303_capi_init_param max20303_param = {
		.i2c_controller = i2c_controller,
		.i2c_device = &max20303_i2c_dev,
		.fg_i2c_device = &max20303_fg_i2c_dev,
	};

	/* Initialize MAX20303 */
	ret = max20303_init(&max20303, &max20303_param);
	if (ret) {
		printf("MAX20303 initialization failed: %d\n", ret);
		return ret;
	}

	/* Disable hibernate mode */
	ret = max20303_set_hibernate(max20303, false);
	if (ret) {
		printf("Failed to disable hibernate: %d\n", ret);
	}

	printf("MAX20303 initialized\n");

	/* Display initial text */
	display_print_string(oled_display, "Battery Voltage:", 0, 0);
	display_print_string(oled_display, "mV", 2, 10);

	/* Main loop - read and display battery voltage */
	while (1) {
		/* Read battery voltage */
		ret = max20303_read_vcell(max20303, &battery_uv);
		if (ret) {
			printf("Error reading battery voltage: %d\n", ret);
			display_print_string(oled_display, "ERROR", 2, 0);
		} else {
			/* Convert to millivolts */
			battery_mv = battery_uv / 1000;

			/* Print to UART */
			printf("Battery voltage: %u mV\n", battery_mv);

			/* Format voltage string for display */
			snprintf(voltage_str, sizeof(voltage_str), "%u    ", battery_mv);

			/* Display voltage on OLED */
			display_print_string(oled_display, voltage_str, 2, 0);
		}

		/* Update every 1 second */
		no_os_mdelay(1000);
	}

	/* Cleanup (never reached in infinite loop) */
	max20303_remove(max20303);
	display_remove(oled_display);

	return 0;
}
