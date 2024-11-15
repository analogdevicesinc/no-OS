/***************************************************************************//**
 *   @file   chibios/chibios_delay.c
 *   @brief  Implementation of chibios delay functions.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "no_os_delay.h"
#include "hal.h"

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	chThdSleepMilliseconds(msecs);
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	unsigned long long Xtime_Global;
	double fractional_part = 0;
	struct no_os_time t;

	Xtime_Global = chVTGetSystemTimeX();
	t.s = Xtime_Global / 1000; // milliseconds

	fractional_part = (double)Xtime_Global / 1000 - Xtime_Global / 1000;
	t.us = fractional_part * 1000;
	t.us *= 1000;

	return t;
}
