/***************************************************************************//**
 *   @file   no_os_delay.h
 *   @brief  Header file of Delay functions
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_DELAY_H_
#define _NO_OS_DELAY_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/**
 * @struct no_os_time
 * @brief Structure holding time data (seconds, microseconds).
 */
struct no_os_time {
	unsigned int s, us;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Generate microseconds delay. */
void no_os_udelay(uint32_t usecs);

/* Generate miliseconds delay. */
void no_os_mdelay(uint32_t msecs);

/* Get current time */
struct no_os_time no_os_get_time(void);

#endif // _NO_OS_DELAY_H_
