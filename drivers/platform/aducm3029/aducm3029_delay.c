/***************************************************************************//**
 *   @file   aducm3029_delay.c
 *   @brief  Implementation of DELAY functions for ADuCM302x.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_delay.h"
#include "no_os_timer.h"
#include "no_os_error.h"
#include "aducm3029_timer.h"

/******************************************************************************/
/****************************** Global Variables*******************************/
/******************************************************************************/

/** Used for counting microseconds */
static struct no_os_timer_desc *us_timer;

/**
 *  Used to keep the hardware timer enabled to avoid delay given by its
 * initialization
 */
static struct no_os_timer_desc *dummy_timer;

/** Used for counting milliseconds */
static struct no_os_timer_desc *ms_timer;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
