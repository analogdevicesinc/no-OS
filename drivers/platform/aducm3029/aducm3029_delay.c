/***************************************************************************//**
 *   @file   aducm3029_delay.c
 *   @brief  Implementation of DELAY functions for ADuCM302x.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
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

#include "no_os_delay.h"
#include "no_os_timer.h"
#include "no_os_error.h"
#include "aducm3029_timer.h"

/** Used for counting microseconds */
static struct no_os_timer_desc *us_timer;

/**
 *  Used to keep the hardware timer enabled to avoid delay given by its
 * initialization
 */
static struct no_os_timer_desc *dummy_timer;

/** Used for counting milliseconds */
static struct no_os_timer_desc *ms_timer;

/**
 * @brief Initialize the timer descriptor
 * @param timer - Value where the instance of the new initialized timer is
 * stored
 * @param is_us - If 0 the timer counts milliseconds, otherwise it counts
 * microseconds
 * @return 1 on success, 0 otherwise
 */
static uint32_t initialize_timer(struct no_os_timer_desc **timer,
				 uint32_t is_us)
{
	struct no_os_timer_init_param param;

	param.id = 0;
	param.freq_hz = is_us ? 1000000u : 1000u;
	param.ticks_count = 0;
	param.platform_ops = &aducm_timer_ops;

	if (is_us) {
		if (0 != no_os_timer_init(&dummy_timer, &param))
			return 0;
		no_os_timer_start(dummy_timer);
	}

	if (0 != no_os_timer_init(timer, &param))
		return 0;
	return 1;
}

/**
 * @brief Starts the specified timer instance and waits until it have the
 * desired value.
 * @param timer - Descriptor of the timer instance.
 * @param value - Value the timer have to count to.
 */
static void start_and_wait(struct no_os_timer_desc *timer, uint32_t value)
{
	uint32_t count;

	no_os_timer_counter_set(timer, 0);
	no_os_timer_start(timer);
	do {
		no_os_timer_counter_get(timer, &count);
	} while (count < value);
	no_os_timer_stop(timer);
}

/**
 * @brief Wait until usecs microseconds passed.
 * @param usecs - Number of microseconds to wait
 */
void no_os_udelay(uint32_t usecs)
{
	if (!us_timer)
		if (!initialize_timer(&us_timer, 1))
			return ;
	start_and_wait(us_timer, usecs);
}

/**
 * @brief Wait until msecs milliseconds passed.
 * @param msecs - Number of milliseconds to wait
 */
void no_os_mdelay(uint32_t msecs)
{
	if (!ms_timer)
		if (!initialize_timer(&ms_timer, 0))
			return ;
	start_and_wait(ms_timer, msecs);
}
