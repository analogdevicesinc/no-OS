/***************************************************************************//**
 *   @file   freeRTOS/freertos_delay.c
 *   @brief  Implementation of freertos delay functions.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "no_os_delay.h"
#include "task.h"

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */

void no_os_udelay(uint32_t usecs)
{
	vTaskDelay((uint64_t)configTICK_RATE_HZ * usecs / 1000000);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
	vTaskDelay(configTICK_RATE_HZ * msecs / 1000);
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t;

	TickType_t _system_ticks  = xTaskGetTickCount();

	t.s = _system_ticks / configTICK_RATE_HZ;
	t.us = 1000000ULL * (_system_ticks % configTICK_RATE_HZ) / configTICK_RATE_HZ;

	return t;
}
