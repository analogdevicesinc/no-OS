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
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "capi/capi_spi.h"
#include "capi/capi_i2c.h"
#include "capi/capi_gpio.h"
#include "adxl355.h"
#include "i2c_bitbang.h"
#include "ssd_1306.h"

#include "platform.h"

#define STEP_WINDOW_SIZE 12

uint32_t total_step_count = 0;

/* Global CAPI handles */
static struct capi_spi_controller_handle *spi_controller;
static struct capi_i2c_controller_handle *i2c_controller;
static struct capi_i2c_controller_handle *bitbang_i2c;
static struct capi_gpio_port_handle *gpio_port0;

static struct capi_i2c_device ssd_1306_i2c_dev;

static struct capi_gpio_port_config gpio_port0_config = {
	.identifier = 0,
	.num_pins = 32,
	.ops = &maxim_capi_gpio_ops,
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

static void apply_lpf(int32_t *buffer, uint32_t len)
{
	for (int i = len - 1; i >= 3; i--){
		buffer[i] = buffer[i] + buffer[i - 1] + buffer[i - 2] + buffer[i - 3];
		buffer[i] /= 4;
	}
}

static int process_buffers(int32_t *max_buff, int32_t *min_buff)
{
	static uint32_t threshold = 0;
	int32_t max_peak_val = max_buff[0];
	uint32_t max_peak_idx = 0;
	int32_t min_peak_val = min_buff[0];
	uint32_t min_peak_idx = 0;
	int32_t sensitivity = 150;

	apply_lpf(max_buff, STEP_WINDOW_SIZE);
	apply_lpf(min_buff, STEP_WINDOW_SIZE);

	for (int i = 1; i < STEP_WINDOW_SIZE; i++){
		if (max_buff[i] > max_peak_val && max_buff[i] > threshold + sensitivity / 2){
			max_peak_val = max_buff[i];
			max_peak_idx = i;
		}
	}

	for (int i = 1; i < STEP_WINDOW_SIZE; i++){
		if (min_buff[i] < min_peak_val && min_buff[i] < threshold - sensitivity / 2){
			min_peak_val = min_buff[i];
			min_peak_idx = i;
		}
	}

	if (max_peak_val < min_peak_val){
		return 0;
	}

	if (max_peak_val - min_peak_val > sensitivity){
		threshold = max_peak_val - min_peak_val;
		return 1;
	}

	return 0;
}

static int step_count(int32_t *accel_data, uint32_t len)
{
	static int32_t accel_data_window[2 * STEP_WINDOW_SIZE];
	static uint8_t buffer_write_index = 0;
	int32_t *max_peak_buffer;
	int32_t *min_peak_buffer;
	static uint8_t w_index;
	uint8_t current_steps;
	uint8_t max_len;
	int ret = 0;

	while (len){
		max_len = no_os_min(2 * STEP_WINDOW_SIZE - buffer_write_index, len);
		memcpy(accel_data_window + buffer_write_index, accel_data, max_len * sizeof(*accel_data));
		buffer_write_index += max_len;
		len -= max_len;

		if (buffer_write_index < 2 * STEP_WINDOW_SIZE)
			return ret;

		buffer_write_index = 0;
		max_peak_buffer = accel_data_window;
		min_peak_buffer = &accel_data_window[STEP_WINDOW_SIZE];

		current_steps = process_buffers(max_peak_buffer, min_peak_buffer);
		ret += current_steps;

		/* No step detected. Look for a peak in the previous min buffer */
		if (!current_steps){
			memcpy(accel_data_window, min_peak_buffer, STEP_WINDOW_SIZE * sizeof(*accel_data));
			buffer_write_index = STEP_WINDOW_SIZE;
		}
	}

	return ret;
}

static int init_display(void)
{
	struct capi_gpio_pin sda_pin;
	struct capi_gpio_pin scl_pin;
	struct i2c_bitbang_extra bitbang_extra;
	struct capi_i2c_config bitbang_config;
	struct no_os_i2c_desc *oled_i2c_desc;
	int ret;

	/* Setup GPIO pins for I2C bitbang */
	sda_pin.port_handle = gpio_port0;
	sda_pin.number = 30;
	sda_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

	scl_pin.port_handle = gpio_port0;
	scl_pin.number = 31;
	scl_pin.flags = CAPI_GPIO_ACTIVE_HIGH;

	bitbang_config.ops = &i2c_bitbang_ops;
	bitbang_config.identifier = 0;
	bitbang_config.clk_freq_hz = CAPI_I2C_SPEED_FAST;
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
	bitbang_extra.pull_type = I2C_BITBANG_PULL_EXTERNAL;
	bitbang_extra.timeout_us = 100000;

	ret = capi_i2c_init(&bitbang_i2c, &bitbang_config);
	if (ret) {
		printf("Failed to initialize I2C bitbang: %d\n", ret);
		return ret;
	}

	ssd_1306_i2c_dev.address = 0x3C;
	ssd_1306_i2c_dev.speed = CAPI_I2C_SPEED_FAST;
	ssd_1306_i2c_dev.controller = bitbang_i2c;

	oled_display_extra.i2c_desc = &ssd_1306_i2c_dev;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display: %d\n", ret);
		no_os_free(oled_i2c_desc);
		return ret;
	}

	display_clear(oled_display);
	display_on(oled_display);

	display_print_string(oled_display, "Steps: ", 0, 0);
	display_print_string(oled_display, "0", 0, 6);

	return 0;
}

/**
 * @brief First thread function
 * @param pvParameters - Thread parameters
 */
void bt_task(void *pvParameters)
{
	struct no_os_time t1, t2;

	uint32_t i = 0;
	uint8_t fifo_entries;
	uint32_t new_step = 0;
	char steps_display_buffer[4] = {0};
	static struct adxl355_frac_repr x_accel[16];
	static struct adxl355_frac_repr y_accel[16];
	static struct adxl355_frac_repr z_accel[16];
	static int32_t accel_sum[16];
	struct capi_spi_config adxl355_spi_config = {
		.identifier = 0,
		.clk_freq_hz = 1000000,
		.ops = &maxim_capi_spi_ops,
	};

	struct capi_spi_device adxl355_spi_dev = {
		.controller = spi_controller,
		.native_cs = 0,
		.mode = CAPI_SPI_MODE_0,
		.max_speed_hz = 1000000,
	};

	struct adxl355_init_param adxl355_param = {
		.spi_controller = spi_controller,
		.comm_param = adxl355_spi_config,
		.dev_type = ID_ADXL355,
		.spi_dev = &adxl355_spi_dev,
	};
	struct adxl355_dev *adxl355;
	int ret;

	ret = capi_spi_init(&spi_controller, &adxl355_spi_config);
	if (ret)
		return;

	adxl355_param.spi_controller = spi_controller;
	adxl355_spi_dev.controller = spi_controller;

	init_display();

	/* 
	 * A disadvantage of dynamically allocating the memory used for the device
	 * handles is that we cannot know (at compile time) the location in memory
	 * where the descriptor will be stored. Thus, we need to assign that value
	 * in device parameters after the peripheral controller is initialized.
	 */
	adxl355_param.spi_controller = spi_controller;

	ret = adxl355_init(&adxl355, adxl355_param);
	if (ret)
		return;

	ret = adxl355_soft_reset(adxl355);
	if (ret)
		return;

	ret = adxl355_set_odr_lpf(adxl355, ADXL355_ODR_62_5HZ);
	if (ret)
		return;

	ret = adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret)
		return;

	/* For FreeRTOS, WSF handles dispatching via its own tasks created in WsfOsInit() */
	/* This task can now just handle periodic operations or exit */
	while (1) {
		ret = adxl355_get_fifo_data(adxl355, &fifo_entries, x_accel, y_accel, z_accel);
		if (ret){
			printf("Warning: adxl355_get_raw_xyz() = %d\n", ret);
			vTaskDelay(pdMS_TO_TICKS(50));
		}

		fifo_entries /= 3;
		for (int i = 0; i < fifo_entries; i++){
			accel_sum[i] = x_accel[i].integer * 1000 + x_accel[i].fractional / 1000000
			 	       + y_accel[i].integer * 1000 + y_accel[i].fractional / 1000000
				       + z_accel[i].integer * 1000 + z_accel[i].fractional / 1000000;

			accel_sum[i] /= 10;
		}

		ret = step_count(accel_sum, fifo_entries);
		if (ret)
			total_step_count += ret;

		if (i >= 20){
			i = 0;

			sprintf(steps_display_buffer, "%d\0", total_step_count % 1000);
			display_print_string(oled_display, steps_display_buffer, 0, 6);

			if (ret)
				printf("Display write error: %d\n", ret);
		}

		i++;
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/**
 * @brief Create FreeRTOS tasks
 * @return Returns error code
 */
int create_tasks(void)
{
	int ret;
	TaskHandle_t thread1_handle = NULL;
	TaskHandle_t thread2_handle = NULL;

	ret = xTaskCreate(bt_task, "BT task", 2048,
			NULL, tskIDLE_PRIORITY + 1, &thread1_handle);
	if (ret != pdPASS) {
		goto error_thread1;
	}

	vTaskStartScheduler();

error_thread1:
	if (thread1_handle != NULL)
		vTaskDelete(thread1_handle);

	return -1;
}

int main()
{
	/* Note: Platform-specific CAPI controller initialization
	 * (spi_controller, i2c_controller, gpio_port0) should be done
	 * in platform-specific init code before calling create_tasks() */

	return create_tasks();
}
