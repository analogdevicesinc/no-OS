#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_config.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "adxl355.h"
#include "max20303.h"
#include "ssd_1306.h"

#include "platform/maxim/maxim_platform.h"

#if defined(CONFIG_CORDIO)
#include "cordio_init.h"
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     UART0_IRQn

#define STEP_WINDOW_SIZE 12

uint32_t total_step_count = 0;
uint32_t battery_percentage = 0;

struct max_gpio_init_param max_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct i2c_bitbang_init_param bitbang_init = {
	.sda_init = {
		.port = 0,
		.number = 30,
		.platform_ops = &max_gpio_ops,
		.extra = &max_gpio_extra_ip,
	},
	.scl_init = {
		.port = 0,
		.number = 31,
		.platform_ops = &max_gpio_ops,
		.extra = &max_gpio_extra_ip,
	},
	.pull_type = I2C_BITBANG_PULL_EXTERNAL,
	.timeout_us = 100000
};

struct no_os_i2c_init_param oled_display_i2c_init_param = {
	.device_id = 0,
	.max_speed_hz = 400000,
	.slave_address = 0x3C,
	.platform_ops = &i2c_bitbang_ops,
	.extra = &bitbang_init,
 };

ssd_1306_extra oled_display_extra = {
	.comm_type = SSD1306_I2C,
	.i2c_ip = &oled_display_i2c_init_param,
};

struct display_init_param oled_display_ini_param = {
	.cols_nb = 128,
	.rows_nb = 64,
	.controller_ops = &ssd1306_ops,
	.extra = &oled_display_extra,
};

static struct no_os_i2c_init_param max20303_comm_param = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = 0x36,
	.platform_ops = &max_i2c_ops,
	.extra = &(struct max_i2c_init_param){
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	},
};

static struct display_dev *oled_display;

static struct max20303_init_param max20303_param = {
	.comm_param = &max20303_comm_param
};

static int max20303_test()
{
	struct max20303_desc *max20303;
	int ret;

	ret = max20303_init(&max20303, &max20303_param);
	if (ret)
		return ret;

	return 0;
}

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

extern int32_t ssd_1306_print_icon(struct display_dev *device, const uint8_t *icon_buffer,
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

	return ssd_1306_print_icon(display, battery_icon, 48, 24, 0, 26);
}

static void init_display()
{
	struct no_os_i2c_desc *oled_display_i2c_desc;
	int ret;

	ret = no_os_i2c_init(&oled_display_i2c_desc, &oled_display_i2c_init_param);
	if (ret) {
		printf("Failed to initialize I2C.\n\r");
		return ret;
	}

	oled_display_extra.i2c_desc = oled_display_i2c_desc;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display.\n\r");
		return ret;
	}

	// display_clear(oled_display);
	display_on(oled_display);
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
	struct max20303_desc *max20303;
	static struct adxl355_frac_repr x_accel[16];
	static struct adxl355_frac_repr y_accel[16];
	static struct adxl355_frac_repr z_accel[16];
	static int32_t accel_sum[16];
	struct no_os_spi_init_param adxl355_comm_param = {
		.device_id = 0,
		.max_speed_hz = 4000000,
		.chip_select = 0,
		.platform_ops = &max_spi_ops,
		.extra = &(struct max_spi_init_param){
			.num_slaves = 1,
			.vssel = 1,
		},
	};

	struct adxl355_init_param adxl355_param = {
		.comm_init.spi_init = adxl355_comm_param,
		.comm_type = ADXL355_SPI_COMM,
		.dev_type = ID_ADXL355
	};
	struct adxl355_dev *adxl355;
	int ret;

	cordio_init();

	ret = adxl355_init(&adxl355, adxl355_param);
	if (ret)
		return ret;

	ret = adxl355_soft_reset(adxl355);
	if (ret)
		return ret;

	ret = adxl355_set_odr_lpf(adxl355, ADXL355_ODR_62_5HZ);
	if (ret)
		return ret;

	ret = adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret)
		return ret;

	ret = max20303_init(&max20303, &max20303_param);
	if (ret)
		printf("max20303_init() error %d\n", ret);

	max20303_set_hibernate(max20303, false);

	taskENTER_CRITICAL();
	init_display();
	taskEXIT_CRITICAL();

	printf("Starting BLE in Peripheral mode\n");
	printf("Cordio initialization completed - GATT services should be available\n");

	printf("GATT services available:\n");
	printf("- Core GATT services (GAP/GATT)\n");
	printf("- Battery Service - read battery level\n");
	printf("- Running speed and cadence service - read battery level\n");

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

		if (i == 20){
			max20303_read_vcell(max20303, &battery_uv);
			printf("Battery voltage: %d mV\n", battery_uv / 1000);

			ret = max20303_read_soc(max20303, &battery_percentage);
			printf("Battery percentage: %hu (%d)\n", battery_percentage, ret);

			/* Update the battery percentage variable sent over BLE */
			AppHwBattTest(battery_percentage);
			i = 0;

			dummy_battery++;
			taskENTER_CRITICAL();
			ret = display_battery_level(oled_display, 10 * (dummy_battery % 10));
			taskEXIT_CRITICAL();

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

	ret = xTaskCreate(bt_task, "BT task", configMINIMAL_STACK_SIZE,
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
	return create_tasks();
}
