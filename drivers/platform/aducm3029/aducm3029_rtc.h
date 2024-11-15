/***************************************************************************//**
 *   @file   aducm3029_rtc.h
 *   @brief  Header file of RTC driver for ADuCM302x
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
