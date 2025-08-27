#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "platform/maxim/maxim_platform.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     UART0_IRQn

struct max_uart_init_param uart_extra_ip = {
	.flow = MXC_UART_FLOW_DIS,
};

struct no_os_uart_init_param uart_init_param = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &uart_extra_ip,
};

/**
 * @brief First thread function
 * @param pvParameters - Thread parameters
 */
void thread1_task(void *pvParameters)
{
	while (1) {
		printf("Tick 1\n");
		no_os_mdelay(1000);
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

	ret = xTaskCreate(thread1_task, "Thread1", configMINIMAL_STACK_SIZE,
			  NULL, tskIDLE_PRIORITY + 1, &thread1_handle);
	if (ret != pdPASS) {
		goto error_thread1;
	}

	ret = xTaskCreate(thread2_task, "Thread2", configMINIMAL_STACK_SIZE,
			  NULL, tskIDLE_PRIORITY + 1, &thread2_handle);
	if (ret != pdPASS) {
		goto error_thread2;
	}

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
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uart_init_param);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	printf("FTC Workshop - FreeRTOS Demo\n");

	ret = create_tasks();

	printf("create_tasks() - %d\n", ret);

	no_os_uart_remove(uart);
	return ret;
}