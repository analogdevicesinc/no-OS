/***************************************************************************//**
 *   @file   aducm3029_rtc.h
 *   @brief  Header file of RTC driver for ADuCM302x
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
