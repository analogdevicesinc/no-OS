/***************************************************************************//**
 *   @file   linux_timer.h
 *   @brief  Header file for Linux timer platform driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LINUX_TIMER_H_
#define LINUX_TIMER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_timer.h"

/**
 * @brief Linux specific timer platform ops.
 */
extern const struct no_os_timer_platform_ops linux_timer_ops;

#endif //LINUX_TIMER_H_

