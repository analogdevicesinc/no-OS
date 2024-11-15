/***************************************************************************//**
 *   @file     mbed_platform_support.cpp
 *   @brief    Support functions and declarations for Mbed platform
 *   @details  This is a platform specific file that supports functionality
 *             required from application specific files.
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/************************ Includes Files **************************************/
/******************************************************************************/
#include <mbed.h>

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

#include "mbed_platform_support.h"

// Configure and instantiate UnbufferedSerial object to access the stdin.
// The default mbed baud rate is 9600, unless is it overriden in the
// mbed_app.json file, or by creating another UnbufferedSerial object using
// the same pins.
static UnbufferedSerial port(USBTX, USBRX);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
  * @brief  getchar, but does not block if nothing waiting to be read
  * @param  None
  * @return character if available, NULL otherwise
  */
char getchar_noblock(void)
{
	char rx_char = '\0';

	// Return the character read from the serial port
	if (port.readable() > 0) {
		port.read(&rx_char, 1);
	}

	return rx_char;
}

#ifdef __cplusplus // Closing extern c
}
#endif
