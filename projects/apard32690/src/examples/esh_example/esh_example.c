/***************************************************************************//**
 *   @file   esh_example.c
 *   @brief  Implementation for the esh example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include "common_data.h"
#include "no_os_esh.h"

#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

void prompt_task(void *data)
{
	prompt();
}

int thread_setup()
{
	TaskHandle_t esh_prompt_handle = NULL;
	int ret;

	ret = xTaskCreate(prompt_task, "esh_prompt", 1024, NULL, 1, &esh_prompt_handle);
	if (ret != pdPASS) {
		printf("Failed to create prompt task\n");
		return ret;
	}

	vTaskStartScheduler();

	return -1;
}

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int esh_example_main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	return thread_setup();
}
