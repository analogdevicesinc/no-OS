/***************************************************************************//**
 *   @file   no_os_aout.h
 *   @author PMallick (Pratyush.Mallick@analog.com)
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NO_OS_AOUT_H
#define NO_OS_AOUT_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct aout_init_param
 * @brief Structure holding the parameters for analog output initialization
 */
struct no_os_aout_init_param {
	/* Min output range of DAC in volts */
	float aout_min_v;
	/* Max output range of DAC in volts */
	float aout_max_v;
	/* Analog output reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/**
 * @struct aout_desc
 * @brief Structure holding analog output descriptor
 */
struct no_os_aout_desc {
	/* Min output value of DAC in volts */
	float aout_min_v;
	/* Max output value of DAC in volts */
	float aout_max_v;
	/* Analog output reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Write analog output voltage */
int32_t no_os_aout_set_voltage(struct no_os_aout_desc *desc, float value);

/* Initialize the analog output peripheral */
int32_t no_so_aout_init(struct no_os_aout_desc **desc,
			const struct no_os_aout_init_param *param);

/* Free the resources allocated by no_os_aout_init() */
int32_t no_os_aout_remove(struct no_os_aout_desc *desc);

#endif	// end of NO_OS_AOUT_H
