/***************************************************************************//**
 *   @file   altera/altera_delay.c
 *   @brief  Implementation of Altera Delay Functions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_delay.h"
#if defined(CONFIG_ALTERA_PLATFORM_NIOSV)
#include <sys/alt_alarm.h>
#else
#include <unistd.h>
#endif

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */
void no_os_udelay(uint32_t usecs)
{
#if defined(CONFIG_ALTERA_PLATFORM_NIOSV)
	alt_busy_sleep(usecs);
#else
	usleep(usecs);
#endif
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
#if defined(CONFIG_ALTERA_PLATFORM_NIOSV)
	alt_busy_sleep(msecs * 1000U);
#else
	usleep(msecs * 1000U);
#endif
}
