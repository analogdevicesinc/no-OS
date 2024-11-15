/***************************************************************************//**
 *   @file   pico/pico_delay.c
 *   @brief  Implementation of pico delay functions.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "pico/time.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 * @return None.
 */
void no_os_udelay(uint32_t usecs)
{
	sleep_us(usecs);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	sleep_ms(msecs);
}
