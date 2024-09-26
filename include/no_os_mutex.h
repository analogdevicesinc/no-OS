/*******************************************************************************
 *   @file   no_os_mutex.h
 *   @brief  Header file of mutex implementation.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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
