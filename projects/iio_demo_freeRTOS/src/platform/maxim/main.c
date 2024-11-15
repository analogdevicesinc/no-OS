/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function on maxim platform used by iio_demo_freeRTOS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

/**
 * @brief IIOD example task
 * @return Returns the error code of iio_example_main function
*/
int iiodTask()
{
	return iio_example_main();
}

/**
 * @brief LED blinking task for multithreading example
 * @return Returns error code
*/
int blinkingTask()
{
	int ret = 0;
	struct no_os_gpio_desc *led_pin;
	ret = no_os_gpio_get(&led_pin, &gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_output(led_pin, NO_OS_GPIO_LOW);
	if (ret)
		goto error_pin;

	bool led_on = true;
	while (1) {
		if (!led_on) {
			ret = no_os_gpio_set_value(led_pin, NO_OS_GPIO_HIGH);
			if (ret)
				goto error_pin;
			led_on=true;
			no_os_mdelay(100);
		}
		if (led_on) {
			ret = no_os_gpio_set_value(led_pin, NO_OS_GPIO_LOW);
			if (ret)
				goto error_pin;
			led_on=false;
			no_os_mdelay(100);
		}
	}

error_pin:
	no_os_gpio_remove(led_pin);
	return ret;
}

/**
 * @brief Creating iio example with blinking led task
 * @return  Returns error code
 *
*/
int create_tasks(void)
{
	int ret;
	TaskHandle_t iio_app_handle = NULL;
	TaskHandle_t led_task_handle = NULL;

	ret = xTaskCreate(iiodTask, (const char *)"iiodTask", configIIO_APP_STACK_SIZE,
			  NULL, tskIDLE_PRIORITY + 1, &iio_app_handle);
	if (ret != pdPASS) {
		printf("xTaskCreate() failed to create iiodTask task.\n");
		goto error_iio_demo_task;
	}
	ret = xTaskCreate(blinkingTask, (const char *)"blinkingTask",
			  configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &led_task_handle);
	if (ret != pdPASS) {
		printf("xTaskCreate() failed to create blinkingTask task.\n");
		goto error_blinking_task;
	}

	vTaskStartScheduler();

error_blinking_task:
	if (led_task_handle != NULL)
		vTaskDelete(led_task_handle);
error_iio_demo_task:
	if (iio_app_handle != NULL)
		vTaskDelete(iio_app_handle);

	return -1;
}

/**
 * @brief Main function of iio_demo_freeRTOS project
 * @return int - Returns error code
 */
int main()
{
	int ret = -1;

#if TARGET_NUM == 32660
#error TARGET MAX32660 not supported
#endif

#ifdef IIO_EXAMPLE
	ret = create_tasks();
#endif

#if (IIO_EXAMPLE== 0)
#error At least one example has to be selected using y value in Makefile.
#endif
	return ret;
}
