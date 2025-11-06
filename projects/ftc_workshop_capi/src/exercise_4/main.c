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
#include "adxl355_capi.h"
#include "max20303_capi.h"
#include "capi_i2c_bitbang.h"
#include "ssd_1306_capi.h"

#include "platform.h"

#if defined(CONFIG_CORDIO)
#include "cordio_init.h"
#endif

#define STEP_WINDOW_SIZE 12

uint32_t total_step_count = 0;
uint32_t battery_percentage = 0;

/* Global CAPI handles */
static struct capi_spi_controller_handle *spi_controller;
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

	// if (max_peak_val < threshold + sensitivity / 2){
	// 	printf("Not step: max_peak_val (%d) < %d\n", max_peak_val, threshold + sensitivity / 2);
	// 	return 0;
	// }

	// if (min_peak_val > threshold - sensitivity / 2){
	// 	printf("Not step: min_peak_val (%d) > %d\n", min_peak_val, threshold - sensitivity / 2);
	// 	return 0;
	// }

	if (max_peak_val < min_peak_val){
		// printf("Not Step: max = %d < min = %d\n", max_peak_val, min_peak_val);
		return 0;
	}

	if (max_peak_val - min_peak_val > sensitivity){
		threshold = max_peak_val - min_peak_val;
		// printf("Step: max = %d min = %d\n", max_peak_val, min_peak_val);
		return 1;
	}

	// printf("Not Step: max = %d min = %d\n", max_peak_val, min_peak_val);

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

// 48x24 battery icon
static const uint8_t battery_empty_icon[] = {
    // First 8 rows (top third of battery)
    0x00,0x00,0x00,0x00,0xF8,0xF8,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xF8,0xF8,0x00,0x00,0x00,0x00,
    // Middle 8 rows (middle third with battery tip)
    0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
    // Last 8 rows (bottom third of battery)
    0x00,0x00,0x00,0x00,0x1F,0x1F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1F,0x1F,0x00,0x00,0x00,0x00
};

static uint8_t battery_icon[144]; // 48x24 pixels = 144 bytes

extern int32_t ssd_1306_capi_print_icon(struct display_dev *device, const uint8_t *icon_buffer,
		uint8_t width, uint8_t height, uint8_t row, uint8_t col);

// Function to display battery level (0-100)
static int display_battery_level(struct display_dev *display, uint8_t percentage) {
	memcpy(battery_icon, battery_empty_icon, sizeof(battery_icon));

	// Calculate how many columns to fill
	int fill_columns = 0;
	if (percentage >= 100) {
		fill_columns = 35; // Maximum fillable columns (excluding borders)
	} else if (percentage > 0) {
		fill_columns = (percentage * 35) / 100;
	}

	// Fill the battery from left to right in all three sections
	for (int i = 1; i < fill_columns; i++) {
		// Fill in top section (0xFF for solid fill)
		battery_icon[6 + i] |= 0xF0;  // 1111 0000 - fills the height except border pixels
		// Fill in middle section (if not in the tip area)
		if (i < 35) {  // Don't fill where the tip is
		battery_icon[48 + 6 + i] |= 0xFF;  // 1111 1111 - fills entire height
		}
		// Fill in bottom section (0xFF for solid fill)
		battery_icon[96 + 6 + i] |= 0x0F;  // 0000 1111 - fills the height except border pixels
	}

	return ssd_1306_capi_print_icon(display, battery_icon, 48, 24, 4, 0);
}

static int init_display(void)
{
	struct capi_gpio_pin sda_pin;
	struct capi_gpio_pin scl_pin;
	struct capi_i2c_bitbang_extra bitbang_extra;
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

	/* Create no_os_i2c_desc wrapper for display driver compatibility */
	// oled_i2c_desc = no_os_calloc(1, sizeof(*oled_i2c_desc));
	// if (!oled_i2c_desc)
	// 	return -ENOMEM;

	// oled_i2c_desc->slave_address = 0x3C;
	// /* Note: The display driver needs to be updated to use CAPI directly,
	//  * but for now we create a wrapper */

	oled_display_extra.i2c_desc = &ssd_1306_capi_i2c_dev;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display: %d\n", ret);
		no_os_free(oled_i2c_desc);
		return ret;
	}

	display_clear(oled_display);
	display_on(oled_display);

	display_print_string(oled_display, "Steps: ", 0, 0);
	display_print_string(oled_display, "Battery: ", 2, 0);
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
	uint32_t dummy_battery = 0;
	uint8_t fifo_entries;
	uint32_t new_step = 0;
	uint32_t battery_uv;
	struct max20303_capi_desc *max20303;
	char steps_display_buffer[4] = {0};
	char battery_display_buffer[6] = {0};
	static struct adxl355_capi_frac_repr x_accel[16];
	static struct adxl355_capi_frac_repr y_accel[16];
	static struct adxl355_capi_frac_repr z_accel[16];
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

	struct adxl355_capi_init_param adxl355_param = {
		.spi_controller = spi_controller,
		.comm_param = adxl355_spi_config,
		.dev_type = ID_ADXL355_CAPI,
		.spi_dev = &adxl355_spi_dev,
	};
	struct adxl355_capi_dev *adxl355;
	int ret;

	ret = capi_spi_init(&spi_controller, &adxl355_spi_config);
	if (ret)
		return ret;

	adxl355_param.spi_controller = spi_controller;
	adxl355_spi_dev.controller = spi_controller;

	// taskENTER_CRITICAL();
	init_display();
	// taskEXIT_CRITICAL();

	/* 
	 * Configure the Cordio BLE stack. no-OS doesn't offer an API for controlling
	 * BLE devices, thus we need to use the Cordio functions directly into the
	 * application layer.
	 */
	cordio_init();

	/* 
	 * A disadvantage of dynamically allocating the memory used for the device
	 * handles is that we cannot know (at compile time) the location in memory
	 * where the descriptor will be stored. Thus, we need to assign that value
	 * in device parameters after the peripheral controller is initialized.
	 */
	adxl355_param.spi_controller = spi_controller;

	ret = adxl355_capi_init(&adxl355, adxl355_param);
	if (ret)
		return ret;

	ret = adxl355_capi_soft_reset(adxl355);
	if (ret)
		return ret;

	ret = adxl355_capi_set_odr_lpf(adxl355, ADXL355_CAPI_ODR_62_5HZ);
	if (ret)
		return ret;

	ret = adxl355_capi_set_op_mode(adxl355, ADXL355_CAPI_MEAS_TEMP_ON_DRDY_OFF);
	if (ret)
		return ret;

	struct capi_i2c_config i2c_config = {
		.identifier = 1,
		.initiator = 1,
		.clk_freq_hz = 400000,
		.ops = &maxim_capi_i2c_ops,
	};

	ret = capi_i2c_init(&i2c_controller, &i2c_config);
	if (ret)
		return ret;

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

	ret = max20303_init(&max20303, &max20303_param);
	if (ret)
		printf("max20303_init() error %d\n", ret);

	max20303_set_hibernate(max20303, false);

	printf("Starting BLE\n");
	printf("Cordio initialization completed - GATT services should be available\n");

	printf("GATT services available:\n");
	printf("- Core GATT services (GAP/GATT)\n");
	printf("- Battery Service - read battery level\n");
	printf("- Running speed and cadence service - read the number of steps\n");

	/* For FreeRTOS, WSF handles dispatching via its own tasks created in WsfOsInit() */
	/* This task can now just handle periodic operations or exit */
	while (1) {
		ret = adxl355_capi_get_fifo_data(adxl355, &fifo_entries, x_accel, y_accel, z_accel);
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
			max20303_read_vcell(max20303, &battery_uv);
			printf("Battery voltage: %d mV\n", battery_uv / 1000);

			ret = max20303_read_soc(max20303, &battery_percentage);
			printf("Battery percentage: %hu (%d)\n", battery_percentage, ret);

			/* Update the battery percentage variable sent over BLE */
			AppHwBattTest(battery_percentage);
			i = 0;

			sprintf(steps_display_buffer, "%d\0", total_step_count % 1000);
			sprintf(battery_display_buffer, "%d \0", battery_percentage);

			dummy_battery++;
			ret = display_battery_level(oled_display, 10 * (dummy_battery % 10));

			/* 
			 * TODO: Display the number of steps and the battery percentage on the display.
			 * The values are stored in the steps_display_buffer and the battery_display_buffer
			 * arrays.
			 * 
			 * Hint: display_print_string() can be used.
			 */

			/*
			display_print_string(oled_display, steps_display_buffer, 0, 6);
			display_print_string(oled_display, battery_display_buffer, 2, 8);
			*/

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
