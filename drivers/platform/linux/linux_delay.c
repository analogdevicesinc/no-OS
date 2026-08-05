/***************************************************************************//**
 *   @file   linux/linux_delay.c
 *   @brief  Implementation of Linux platform Delay Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdint.h>
#include <unistd.h>

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */
void no_os_udelay(uint32_t usecs)
{
	usleep(usecs);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}
