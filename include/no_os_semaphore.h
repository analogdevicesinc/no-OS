/*******************************************************************************
 *   @file   no_os_semaphore.h
 *   @brief  Header file of semaphore implementation.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_SEMAPHORE_H_
#define _NO_OS_SEMAPHORE_H_

/* Initialize semaphore */
void no_os_semaphore_init(void **semaphore);

/* Take token from semaphore */
void no_os_semaphore_take(void *semaphore);

/* Give token to semaphore */
void no_os_semaphore_give(void *semaphore);

/* Remove semaphore */
void no_os_semaphore_remove(void *semaphore);

#endif // _NO_OS_SEMAPHORE_H_
