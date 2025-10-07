#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "adxl355.h"

#include "platform/maxim/maxim_platform.h"

#if defined(CONFIG_CORDIO)
#include "cordio_init.h"
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     UART0_IRQn

#define STEP_WINDOW_SIZE 12

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

/**
 * @brief First thread function
 * @param pvParameters - Thread parameters
 */
void bt_task(void *pvParameters)
{
	uint8_t fifo_entries;
	uint32_t total_step_count = 0;
	uint32_t new_step = 0;
	static struct adxl355_frac_repr x_accel[16];
	static struct adxl355_frac_repr y_accel[16];
	static struct adxl355_frac_repr z_accel[16];
	static int32_t accel_sum[16];
	struct no_os_spi_init_param adxl355_comm_param = {
		.device_id = 0,
		.max_speed_hz = 1000000,
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

	cordio_init();
	printf("Starting BLE in Peripheral mode\n");
	printf("Cordio initialization completed - GATT services should be available\n");

	printf("GATT services available:\n");
	printf("- Core GATT services (GAP/GATT)\n");
	printf("- Battery Service (0x180F) - read battery level\n");
	printf("- Device Information Service (0x180A)\n");
	printf("- Wireless Profile Service - read/write custom data\n");
	printf("You can connect with your phone and read dummy data!\n");

	printf("accel\n");

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
			printf("%d\n", accel_sum[i]);
		}

		ret = step_count(accel_sum, fifo_entries);

		if (ret){
			total_step_count += ret;
			// printf("Step count: %d\n", total_step_count);
		}
		// printf("Entries %d %d\n", accel_sum[0], total_step_count);

		// printf("Accel X: %lld.%03d m/s^2\n", x_accel.integer, abs(x_accel.fractional));
		// printf("Accel Y: %lld.%03d m/s^2\n", y_accel.integer, abs(y_accel.fractional));
		// printf("Accel Z: %lld.%03d m/s^2\n", z_accel.integer, abs(z_accel.fractional));

		/* Small delay to yield to other FreeRTOS tasks */
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/**
 * @brief Second thread function
 * @param pvParameters - Thread parameters
 */
void thread2_task(void *pvParameters)
{
	while (1) {
		printf("Tick 2\n");
		no_os_mdelay(1000);
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

	// ret = xTaskCreate(thread2_task, "Thread2", configMINIMAL_STACK_SIZE,
	// 		  NULL, tskIDLE_PRIORITY + 2, &thread2_handle);
	// if (ret != pdPASS) {
	// 	goto error_thread2;
	// }

	vTaskStartScheduler();

error_thread2:
	if (thread2_handle != NULL)
		vTaskDelete(thread2_handle);
error_thread1:
	if (thread1_handle != NULL)
		vTaskDelete(thread1_handle);

	return -1;
}

int main()
{
	int ret;
	// struct no_os_uart_desc *uart;

	// ret = no_os_uart_init(&uart, &uart_init_param);
	// if (ret)
	// 	return ret;

	// no_os_uart_stdio(uart);

	// printf("FTC Workshop - FreeRTOS Demo\n");

	ret = create_tasks();

	printf("create_tasks() - %d\n", ret);

	// no_os_uart_remove(uart);
	return ret;
}
