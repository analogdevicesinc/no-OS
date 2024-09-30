/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function on stm32 platform used by max25603 freeRTOS project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/**
 * @brief Basic example task
 * @return Returns the error code of basic_example_main function
*/
void basic_task()
{
	basic_example_main();
}


/**
 * @brief LED blinking task for multithreading example
 * @return Returns error code
*/
void blinking_task()
{
	int ret = 0;
	struct no_os_gpio_desc *led_pin;
	ret = no_os_gpio_get(&led_pin, &gpio_ip);
	if (ret)
		goto blinking_exit;

	ret = no_os_gpio_direction_output(led_pin, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_pin;

	no_os_mdelay(3000);

error_pin:
	no_os_gpio_remove(led_pin);
blinking_exit:
	led_pin = NULL;
}


/**
 * @brief Creating basic example with blinking led task
 * @return  Returns error code
 *
*/
int create_tasks(void) /* Take a look and work it. */
{
	int ret;
	TaskHandle_t basic_app_handle = NULL;
	TaskHandle_t led_task_handle = NULL;

	ret = xTaskCreate(basic_task, (const char *)"basic_task", configAPP_STACK_SIZE,
			  NULL, tskIDLE_PRIORITY + 1, &basic_app_handle);
	if (ret != pdPASS) {
		printf("xTaskCreate() failed to create basicTask task.\n");
		goto error_iio_demo_task;
	}
	ret = xTaskCreate(blinking_task, (const char *)"blinking_task",
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
	if (basic_app_handle != NULL)
		vTaskDelete(basic_app_handle);

	return -1;
}

/**
 * @brief Main function of max25603 FreeRTOS project
 * @return int - Returns error code
 */
int main()
{
	int ret = -1;

	ret = create_tasks();

	return ret;
}
