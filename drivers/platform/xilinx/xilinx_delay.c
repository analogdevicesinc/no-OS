/***************************************************************************//**
 *   @file   xilinx/xilinx_delay.c
 *   @brief  Implementation of Xilinx Delay Functions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_delay.h"
#include <sleep.h>
#ifdef _XPARAMETERS_PS_H_
#include "no_os_util.h"
#include "xtime_l.h"
#endif

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
#ifdef _XPARAMETERS_PS_H_
	usleep(usecs);
#else
	usleep_MB(usecs);
#endif
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
#ifdef _XPARAMETERS_PS_H_
	usleep(msecs * 1000);
#else
	usleep_MB(msecs * 1000);
#endif
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t = {0, 0};
#ifdef _XPARAMETERS_PS_H_
	unsigned long long Xtime_Global;
	uint32_t rem;

	XTime_GetTime(&Xtime_Global);
	t.s = no_os_div_u64_rem(Xtime_Global, COUNTS_PER_SECOND, &rem);
	t.us = no_os_div_u64((uint64_t)rem * 1000000, COUNTS_PER_SECOND);
#endif

	return t;
}
