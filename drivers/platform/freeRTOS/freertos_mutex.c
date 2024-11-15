/*******************************************************************************
 *   @file   freeRTOS/freertos_mutex.c
 *   @brief  Implementation of no-OS mutex funtionality.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <FreeRTOS.h>
#include "no_os_mutex.h"
#include "semphr.h"
#include "queue.h"

/**
 * @brief Initialize mutex.
 * mutex - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_init(void **mutex)
{
	if (*mutex == NULL) {
		SemaphoreHandle_t* mutexTmp = (SemaphoreHandle_t *)no_os_calloc(1,
					      sizeof(SemaphoreHandle_t));

		*mutex = xSemaphoreCreateBinary();
		xSemaphoreGive(*mutex);
	}
}

/**
 * @brief Lock mutex.
 * mutex - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_lock(void *mutex)
{
	if (mutex != NULL)
		xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY);
}
/**
 * @brief Unlock mutex.
 * mutex - Pointer toward the mutex.
 * @return None.
 */
__attribute((weak)) inline void no_os_mutex_unlock(void *mutex)
{
	if (mutex != NULL)
		xSemaphoreGive((SemaphoreHandle_t)mutex);
}

/**
 * @brief Remove mutex.
 * mutex - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_remove(void *mutex)
{
	if (mutex != NULL) {
		vSemaphoreDelete((SemaphoreHandle_t)mutex);
	}
}

