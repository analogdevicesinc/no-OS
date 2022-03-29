/***************************************************************************//*
* @file    mbed_ain_aout.h
* @brief   Header containing extra types required for
*          analog in/output functionality
******************************************************************************
* Copyright (c) 2021-22 Analog Devices, Inc.
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

#ifndef MBED_AIN_AOUT_H
#define MBED_AIN_AOUT_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "stdio.h"
#include "stdint.h"

/*****************************************************************************/
/********************** Macros and Constants Definition **********************/
/*****************************************************************************/

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/
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

/******************************************************************************/
/*****************************Function Declarations****************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_AIN_AOUT_H */
