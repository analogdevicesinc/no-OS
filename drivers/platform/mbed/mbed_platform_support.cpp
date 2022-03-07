/***************************************************************************//**
 *   @file     mbed_platform_support.cpp
 *   @brief    Support functions and declarations for Mbed platform
 *   @details  This is a platform specific file that supports functionality
 *             required from application specific files.
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
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
