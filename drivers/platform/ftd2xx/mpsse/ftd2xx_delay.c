/*******************************************************************************
 *   @file   ftd2xx_delay.c
 *   @brief  Implementation of delay driver for ftd2xx platform.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdint.h>
#include <unistd.h>

void no_os_udelay(uint32_t usecs)
{
	usleep(usecs);
}

void no_os_mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}
