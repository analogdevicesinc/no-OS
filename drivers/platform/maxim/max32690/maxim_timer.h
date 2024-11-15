/***************************************************************************//**
 *   @file   maxim_timer.h
 *   @brief  Header file for Maxim timer platform driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_TIMER_H_
#define MAXIM_TIMER_H_

#include "tmr.h"
#include "stdint.h"
#include "no_os_timer.h"

#define SOURCE_CLOCK_FREQ       PeripheralClock

/**
 * @brief Maxim specific timer platform ops.
 */
extern const struct no_os_timer_platform_ops max_timer_ops;

#endif //MAXIM_TIMER_H_
