/*******************************************************************************
 *   @file   no_os_mutex.h
 *   @brief  Header file of mutex implementation.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_MUTEX_H_
#define _NO_OS_MUTEX_H_

/**
* @brief Function for no-os mutex initialization and thread safety.
* This function is implemented based on different platforms/OS libraries
* that NO-OS supports. These mutex functions are used for thread safety
* of peripherals. Since these functions don't return error values it is
* the developers responsibility to implement the safety checks in case
* new mutex implementation is being added, like the following:
*
* if ((*mutex) == NULL)
* {
*      //code to initialize the mutex
* }
*
* Also these check are responsible not to allocate different mutexes
* for the same peripheral descriptor.
*/
void no_os_mutex_init(void **mutex);

/**
 * @brief Function for locking mutex
*/
void no_os_mutex_lock(void *mutex);

/**
 * @brief Function for unlocking mutex
*/
void no_os_mutex_unlock(void *mutex);

/**
 * @brief Function for removing the initialized mutex.
 * This function is responsible to remove the allocated mutex. This function is
 * also used by the peripherals mutex thread safety feature and in case
 * new mutex implementation is going to be added, it is the developers
 * responsibility to add extra check inside the function while de-allocating the memory.
 *
 * if (mutex != NULL)
 * {
 *      //code to de-allocate mutex
 * }
*/
void no_os_mutex_remove(void *mutex);

#endif // _NO_OS_MUTEX_H_
