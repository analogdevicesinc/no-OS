/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function on maxim platform used by iio_demo_freeRTOS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
			led_on = true;
			no_os_mdelay(100);
		}
		if (led_on) {
			ret = no_os_gpio_set_value(led_pin, NO_OS_GPIO_LOW);
			if (ret)
				goto error_pin;
			led_on = false;
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
