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
#include "no_os_init.h"

#include "capi/capi_i2c.h"
#include "capi/capi_gpio.h"
#include "capi/capi_uart.h"
#include "max20303.h"
#include "i2c_bitbang.h"
#include "ssd_1306.h"
#include "display.h"

#include "platform.h"

/* Global CAPI handles */
static struct capi_i2c_controller_handle *i2c_controller;
static struct capi_i2c_controller_handle *bitbang_i2c;
static struct capi_gpio_port_handle *gpio_port0;

static struct capi_i2c_device ssd_1306_i2c_dev;

static struct capi_gpio_port_config gpio_port0_config = {
	.identifier = 0,
	.num_pins = 32,
	.ops = &maxim_gpio_ops,
};

/* SSD1306 OLED Display Setup using CAPI I2C bitbang */
static ssd_1306_extra oled_display_extra = {
	.i2c_desc = &ssd_1306_i2c_dev,
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
	struct i2c_bitbang_extra bitbang_extra;
	struct capi_i2c_config bitbang_config;
	int ret;

	/*
	 * Our I2C driver doesn't work with an I2C controller of the MCU. Instead, it is using 2 GPIOs
	 * in order to bitbang the I2C signals (SCL and SDA). However, from the application code point
	 * of view, this is an implementation detail that should only be taken into consideration during
	 * the configuration phase (defining capi_i2c_config and calling capi_i2c_init()). The same CAPI
	 * interface (capi_i2c.h) will be used for transactions using the bitbang driver and another driver
	 * for a hardware I2C controller.
	 *
	 * bitbang_config is a standard capi_i2c_config struct. The same config structure will be
	 * used for any type of I2C controller.
	 *
	 * Drivers handle particular settings by allowing the user to set the "extra" field of the config param.
	 * The extra field if a void pointer ("void *"), thus it doesn't impose any restriction on the parameter's
	 * type. This type should be mentioned in the driver's documentation, but if there isn't one available,
	 * you can check the header file of the driver for an "_extra" struct (capi_i2c_bitbang.h -> capi_i2c_bitbang_extra)
	 *
	 * TODO: As part of this exercise, you don't have to write any new code. This function configures the
	 * SSD1306 display using the ssd_1306 driver (which further uses our i2c bitbang driver). The structure
	 * of the initialization parameters is laid out below. The parameter assignment code is commented. You'll
	 * have to look over the sequence and understand what is being done.
	 */

	/*
	 * bitbang_config structure hierarchy:
	 *
	 * struct capi_i2c_config bitbang_config
	 * ├── identifier       (uint64_t)   - I2C controller ID
	 * ├── clk_freq_hz      (uint32_t)   - Clock frequency in Hz
	 * ├── initiator        (bool)       - Master/Initiator mode flag
	 * ├── address          (uint8_t)    - Device address (when acting as target)
	 * ├── device           (void*)      - Device descriptor (for external controllers)
	 * ├── dma_handle       (void*)      - Optional DMA handle
	 * ├── ops              (struct*)    - Platform-specific I2C operations
	 * └── extra            (void*)      - Points to capi_i2c_bitbang_extra
	 *     │
	 *     └── struct capi_i2c_bitbang_extra
	 *         ├── sda_pin          - SDA GPIO pin configuration
	 *         │   └── struct capi_gpio_pin
	 *         │       ├── port_handle  (struct capi_gpio_port_handle*)
	 *         │       ├── number       (int)     - Pin number
	 *         │       └── flags        (uint32_t) - Pin flags (ACTIVE_HIGH/LOW)
	 *         │
	 *         ├── scl_pin          - SCL GPIO pin configuration
	 *         │   └── struct capi_gpio_pin
	 *         │       ├── port_handle
	 *         │       ├── number
	 *         │       └── flags
	 *         │
	 *         ├── pull_type        (enum) - Pull-up configuration
	 *         │                             (EXTERNAL/INTERNAL/NONE)
	 *         └── timeout_us       (uint32_t) - Clock stretching timeout
	 */

	/*
	 * The SCL and SDA pins are P0.30 and P0.31. As they are on the same port, we can use
	 * the gpio_port0 port handle for both of them.
	 *
	 * The GPIO usage sequence is the following:
	 *
	 * capi_gpio_port_init(&gpio_port0, &gpio_port0_config);
	 *
	 * // At this point, you should make sure that sda_pin->port_handle = gpio_port0.
	 * // In our example, the following fuctions will be called by the ssd_1306 driver,
	 * // however we have to pass valid pin handles (capi_gpio_pin).
	 * capi_gpio_pin_set_direction(sda_pin, CAPI_GPIO_OUTPUT);
	 * capi_gpio_pin_set_value(sda_pin, CAPI_GPIO_HIGH);
	 */

	/* // Uncomment this
	sda_pin.port_handle = gpio_port0;
	sda_pin.number = 30;
	sda_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

	scl_pin.port_handle = gpio_port0;
	scl_pin.number = 31;
	scl_pin.flags = CAPI_GPIO_ACTIVE_HIGH;
	*/

	/* Standard capi_i2c_config struct, using the i2c_bitbang_ops and bitbang_extra*/
	/* // Uncomment this
	bitbang_config.ops = &i2c_bitbang_ops;
	bitbang_config.identifier = 0;
	bitbang_config.clk_freq_hz = CAPI_I2C_SPEED_FAST;
	bitbang_config.initiator = true;
	bitbang_config.address = 0;
	bitbang_config.device = NULL;
	bitbang_config.dma_handle = NULL;
	bitbang_config.extra = &bitbang_extra;
	*/

	ret = capi_gpio_port_init(&gpio_port0, &gpio_port0_config);
	if (ret)
		return ret;

	/*
	 * Because gpio_port0 is dynamically allocated, the value of the variable changes after
	 * the capi_gpio_port_init() call. We'll need to assign the new value.
	 */

	 /* // Uncomment this
	sda_pin.port_handle = gpio_port0;
	scl_pin.port_handle = gpio_port0;
	*/

	/* Configure I2C bitbang_extra struct */

	/* // Uncomment this
	bitbang_extra.sda_pin = sda_pin;
	bitbang_extra.scl_pin = scl_pin;
	bitbang_extra.pull_type = I2C_BITBANG_PULL_EXTERNAL;
	bitbang_extra.timeout_us = 100000;
	*/

	ret = capi_i2c_init(&bitbang_i2c, &bitbang_config);
	if (ret) {
		printf("Failed to initialize I2C bitbang: %d\n", ret);
		return ret;
	}

	/*
	 * Initialize the capi_i2c_device struct specific to the for the SSD1306 display.
	 * As the bitbang_i2c struct is dynamically allocated, we have to reassign the value
	 * in the ssd_1306_i2c_dev.controller field.
	 */
	ssd_1306_i2c_dev.address = 0x3C;
	ssd_1306_i2c_dev.speed = CAPI_I2C_SPEED_FAST;
	ssd_1306_i2c_dev.controller = bitbang_i2c;

	oled_display_extra.i2c_desc = &ssd_1306_i2c_dev;
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
	struct max20303_desc *max20303;
	uint32_t battery_uv;
	uint32_t battery_mv;
	char voltage_str[16];
	int ret;

	/* Initialize UART for printf output */
	struct capi_uart_config uart_config = {
		.identifier = 0,
		.clk_freq_hz = 115200,
		.ops = &maxim_uart_ops,
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
		.ops = &maxim_i2c_ops,
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

	struct max20303_init_param max20303_param = {
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
