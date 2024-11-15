/***************************************************************************//**
 *   @file   aducm3029_timer.h
 *   @brief  Header file of TIMER driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ADUCM3029_TIMER_H
#define ADUCM3029_TIMER_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_timer.h"
#include <drivers/tmr/adi_tmr.h>

/******************************************************************************/
/************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum avail_freqs
 * @brief  It lists the available source frequencies for timer instance 1
 */
enum avail_freqs {
	PCLK_DIV1,
	PCLK_DIV16,
	PCLK_DIV64,
	PCLK_DIV256,
	HFOSC_DIV1,
	HFOSC_DIV16,
	HFOSC_DIV64,
	HFOSC_DIV256,
	LFOSC_DIV1,
	LFOSC_DIV16,
	LFOSC_DIV64,
	LFOSC_DIV256,
	LFXTAL_DIV1,
	LFXTAL_DIV16,
	LFXTAL_DIV64,
	LFXTAL_DIV256,
};

/**
 * @struct aducm_timer_desc
 * @brief  It stores instance values specific for the ADuCM302x implementation
 */
struct aducm_timer_desc {
	/** Used to compare with the driver internal count */
	uint64_t	old_time;
	/** 1 if the instance is counting, 0 otherwise */
	bool		started;
	/** Timer configuration for adum3029 */
	ADI_TMR_CONFIG	tmr_conf;
};

/**
 * @struct aducm_timer_desc
 * @brief  It stores instance values specific for the ADuCM302x implementation
 */
struct aducm_timer_init_param {
	/** Selected frequency for timer clock source */
	enum avail_freqs source_freq;
};

/**
 * @brief aducm3029 specific timer platform ops structure
 */
extern const struct no_os_timer_platform_ops aducm_timer_ops;

#endif /* ADUCM3029_TIMER_H */
