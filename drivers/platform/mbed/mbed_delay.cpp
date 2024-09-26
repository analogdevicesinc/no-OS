/***************************************************************************//**
 *   @file   mbed_delay.cpp
 *   @brief  Implementation of Mbed specific delay functionality
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
