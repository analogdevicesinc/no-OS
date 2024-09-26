/***************************************************************************//**
 *   @file   aducm3029_rtc.h
 *   @brief  Header file of RTC driver for ADuCM302x
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
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

#ifndef ADUCM3029_RTC_H_
#define ADUCM3029_RTC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <drivers/rtc/adi_rtc.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * enum aducm_rtc_freq_opt
 * @brief RTC nominal frequency options for ADuCM3029
 */
enum aducm_rtc_freq_opt {
	AUDCM_32768HZ,
	AUDCM_16384HZ,
	AUDCM_8192HZ,
	AUDCM_4096HZ,
	AUDCM_2048HZ,
	AUDCM_1024HZ,
	AUDCM_512HZ,
	AUDCM_256HZ,
	AUDCM_128HZ,
	AUDCM_64HZ,
	AUDCM_32HZ,
	AUDCM_16HZ,
	AUDCM_8HZ,
	AUDCM_4HZ,
	AUDCM_2HZ,
	AUDCM_1HZ
};

/**
 * @struct aducm_rtc_desc
 * @brief Structure holding ADuCM3029 specific RTC descriptor.
 */
struct aducm_rtc_desc {
	/** Pointer to the DFP driver handler. */
	void *instance;
	/** Pointer to the memory used by the DFP driver. */
	void *memory;
};

#endif /* ADUCM3029_RTC_H_ */
