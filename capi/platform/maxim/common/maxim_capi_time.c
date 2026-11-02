/*******************************************************************************
 *   @file   maxim_capi_time.c
 *   @brief  MAX32650/MAX32690 CAPI time functions, on top of the no-OS delay
 *	     driver which owns SysTick
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include "capi_time.h"
#include "mxc_delay.h"
#include "no_os_delay.h"

void capi_wait_us_impl(uint32_t us)
{
	MXC_Delay(MXC_DELAY_USEC(us));
}

void capi_wait_ms_impl(uint32_t ms)
{
	MXC_Delay(MXC_DELAY_MSEC(ms));
}

int capi_uptime_impl(uint64_t *us)
{
	struct no_os_time t;

	if (!us)
		return -EINVAL;

	t = no_os_get_time();
	*us = (uint64_t)t.s * 1000000ULL + t.us;

	return 0;
}
