/* SPDX-License-Identifier: BSD-3-Clause */

#include "no_os_init.h"
#include "max78000.h"
#include "maxim_time.h"

/** @brief Initialize CPU1 services; system clocks are configured by CPU0. */
__weak int no_os_init(void)
{
	return maxim_time_init();
}

int Board_Init(void)
{
	return no_os_init();
}
