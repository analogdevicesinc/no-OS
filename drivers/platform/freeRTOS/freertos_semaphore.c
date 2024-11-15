/*******************************************************************************
 *   @file   freeRTOS/freertos_semaphore.c
 *   @brief  Implementation of no-OS semaphore funtionality.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <FreeRTOS.h>
#include "no_os_semaphore.h"
#include "semphr.h"

/**
 * @brief Initialize semaphore.
 * semaphore - Pointer toward the semaphore.
 * @return None.
 */
__attribute__((weak)) inline void no_os_semaphore_init(void **semaphore)
{
	if (*semaphore == NULL) {
		SemaphoreHandle_t* semaphoreTmp = (SemaphoreHandle_t *)no_os_calloc(1,
						  sizeof(SemaphoreHandle_t));

		*semaphore = xSemaphoreCreate();
		xSemaphoreGive(*semaphore);
	}
}

/**
 * @brief Take token from semaphore.
 * semaphore - Pointer toward the semaphore.
 * @return None.
 */
__attribute__((weak)) inline void no_os_semaphore_take(void *semaphore)
{
	if (semaphore != NULL)
		xSemaphoreTake((SemaphoreHandle_t)semaphore, portMAX_DELAY);
}

/**
 * @brief Give token to semaphore
 * semaphore - Pointer toward the semaphore.
 * @return None.
 */
__attribute((weak)) inline void no_os_semaphore_give(void *semaphore)
{
	if (semaphore != NULL)
		xSemaphoreGive((SemaphoreHandle_t)semaphore);
}

/**
 * @brief Remove semaphore.
 * semaphore - Pointer toward the semaphore.
 * @return None.
 */
__attribute__((weak)) inline void no_os_semaphore_remove(void *semaphore)
{
	if (semaphore != NULL) {
		vSemaphoreDelete((SemaphoreHandle_t)semaphore);
	}
}
