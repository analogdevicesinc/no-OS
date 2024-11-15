/***************************************************************************//**
 *   @file   freeRTOS/freertos_delay.c
 *   @brief  Implementation of freertos delay functions.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "no_os_delay.h"
#include "task.h"

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 * @return None.
 */

void no_os_udelay(uint32_t usecs)
{

	vTaskDelay((((double)usecs / 1000000) * configTICK_RATE_HZ));
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	vTaskDelay((((double)msecs / 1000) * configTICK_RATE_HZ));
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	unsigned long long Xtime_Global;
	double fractional_part = 0;
	struct no_os_time t;

	unsigned long long _system_ticks  = (unsigned long long)xTaskGetTickCount();

	Xtime_Global = HAL_GetTick();
	t.s = Xtime_Global / 1000; // milliseconds

	fractional_part = (float)Xtime_Global / 1000 - Xtime_Global / 1000;
	t.us = fractional_part * 1000;
	t.us *= 1000;

	return t;
}
