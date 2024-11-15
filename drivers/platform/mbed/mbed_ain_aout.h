/***************************************************************************//*
* @file    mbed_ain_aout.h
* @brief   Header containing extra types required for
*          analog in/output functionality
******************************************************************************
* Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
