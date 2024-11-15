/***************************************************************************//**
 *   @file   mbed_delay.cpp
 *   @brief  Implementation of Mbed specific delay functionality
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus 

#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Generate microseconds delay.
 * @param usecs[in] - Delay in microseconds.
 * @return None.
 */
void no_os_udelay(uint32_t usecs)
{
	/* wait_ns is more time efficient function compared to wait_us
	 * Note: For higher values of delay (more than few msec), it is better to use
	 * no_os_mdelay() function as no error/limit checking is done in this function */
	wait_ns(usecs * 1000);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs[in] - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	if (msecs)
		thread_sleep_for(msecs);
}

#ifdef __cplusplus  // Closing extern c
}
#endif //  _cplusplus
