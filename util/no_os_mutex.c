/*******************************************************************************
 *   @file   util/no_os_mutex.c
 *   @brief  Implementation of no-OS mutex funtionality.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "no_os_mutex.h"

/**
 * @brief Initialize mutex.
 * @param ptr - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_init(void **mutex) {}

/**
 * @brief Lock mutex.
 * @param ptr - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_lock(void *mutex) {}

/**
 * @brief Unlock mutex.
 * @param ptr - Pointer toward the mutex.
 * @return None.
 */
__attribute((weak)) inline void no_os_mutex_unlock(void *mutex) {}

/**
 * @brief Remove mutex.
 * @param ptr - Pointer toward the mutex.
 * @return None.
 */
__attribute__((weak)) inline void no_os_mutex_remove(void *mutex) {}

