/***************************************************************************//**
 *   @file   pico/pico_timer.h
 *   @brief  Header file for the pico timer driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PICO_TIMER_H_
#define _PICO_TIMER_H_

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_timer.h"

/******************************************************************************/
/************************* Variables Declarations *****************************/
/******************************************************************************/

extern struct no_os_timer_desc *pico_alarm_desc[];

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_timer_desc
 * @brief pico platform specific timer descriptor
 */
struct pico_timer_desc {
	/** Period at which an alarm will be triggered (measured in microseconds) */
	uint64_t period;
};

/**
 * @brief pico specific timer platform ops structure
 */
extern const struct no_os_timer_platform_ops pico_timer_ops;

#endif // _PICO_TIMER_H_
