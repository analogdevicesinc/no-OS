/***************************************************************************//*
* @file    mbed_ain_aout.h
* @brief   Header containing extra types required for
*          analog in/output functionality
******************************************************************************
* Copyright (c) 2021-22 Analog Devices, Inc.
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

#ifndef MBED_AIN_AOUT_H
#define MBED_AIN_AOUT_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"
#include "stdint.h"

/**
 * @struct mbed_analog_init_param
 * @brief Structure holding the Analog input init parameters for mbed platform.
 */
struct mbed_ain_init_param {
	/* Analog input pin number */
	int32_t	number;
};

/**
 * @struct mbed_analog_in_desc
 * @brief Analog input specific descriptor for the mbed platform.
 */
struct mbed_ain_desc {
	/* Analog input pin number */
	int32_t	number;
	/* Analog Input instance (mbed::AnalogIn) */
	void *ain_obj;
};

/**
* @struct mbed_analog_out_desc
* @brief Structure holding the Analog output init parameters for mbed platform.
*/
struct mbed_aout_init_param {
	/* Analog output pin number */
	int32_t	number;
};

/**
 * @struct mbed_analog_out_desc
 * @brief Analog output pin specific descriptor for the mbed platform.
 */
struct mbed_aout_desc {
	/* Analog output pin number */
	int32_t	number;
	/* Analog Output instance (mbed::AnalogOut) */
	void *aout_obj;
};

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_AIN_AOUT_H */
