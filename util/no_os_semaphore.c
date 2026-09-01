/*******************************************************************************
 *   @file   util/no_os_semaphore.c
 *   @brief  Implementation of no-OS semaphore funtionality.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_semaphore.h"
#include "no_os_util.h"

/**
 * @brief Initialize semaphore.
 * @param ptr - Pointer toward the semaphore.
 * @return None.
 */
__no_os_weak__((weak)) void no_os_semaphore_init(void **semaphore) {}

/**
 * @brief Take token from semaphore.
 * @param ptr - Pointer toward the semaphore.
 * @return None.
 */
__no_os_weak__((weak)) void no_os_semaphore_take(void *semaphore) {}

/**
 * @brief Give token to semaphore
 * @param ptr - Pointer toward the semaphore.
 * @return None.
 */
__no_os_weak__((weak)) void no_os_semaphore_give(void *semaphore) {}

/**
 * @brief Remove semaphore.
 * @param ptr - Pointer toward the semaphore.
 * @return None.
 */
__no_os_weak__((weak)) void no_os_semaphore_remove(void *semaphore) {}

