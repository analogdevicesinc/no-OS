/***************************************************************************//**
 *   @file   lattice_delay.c
 *   @brief  Implementation of delay Functions for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_delay.h"
#include "no_os_util.h"
#include "hal.h"
#include "sys_platform.h"

#ifndef CLOCK_OS
	#error Please define CLOCK_OS in sys_platform.h!
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
	uint64_t new_tick;
	uint64_t old_tick;

#ifndef CLOCK_OS
	#error Error, Please define CLOCK_OS in sys_platform.h!
#else
	#if CLOCK_OS < 1000000
		#error Error, CLOCK_OS must be greater than or equal to 1000000!
	#endif
	uint64_t u_ticks = ((CLOCK_OS/1000000) * usecs);
#endif

	old_tick = CLINT_MTIME_H;
	old_tick = CLINT_MTIME_L | (old_tick << 32);

	do {
		new_tick = CLINT_MTIME_H;
		new_tick = CLINT_MTIME_L | (new_tick << 32);

	} while ((new_tick - old_tick) <= u_ticks);
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	no_os_udelay(msecs * 1000);
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t = {0, 0};

#ifndef CLOCK_OS
	#error Please define CLOCK_OS in sys_platform.h!
#else
	uint64_t cycle_times;
	uint32_t rem;

	cycle_times = CLINT_MTIME_H;
	cycle_times = CLINT_MTIME_L | (cycle_times << 32);

	t.s = no_os_div_u64_rem(cycle_times, CLOCK_OS, &rem);
	t.us = no_os_div_u64((uint64_t)rem * 1000000, CLOCK_OS);
#endif

	return t;
}
