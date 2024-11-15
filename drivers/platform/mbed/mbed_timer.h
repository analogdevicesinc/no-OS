/***************************************************************************//*
 * @file   mbed_timer.h
 * @brief  Header containing extra types for Timer interface
******************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_TIMER_H_
#define MBED_TIMER_H_

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include "stdint.h"

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/**
* @struct mbed_timer_desc
* @brief Structure holding the mbed specific Timer parameters
*/
struct mbed_timer_desc {
	void *timer;		// Object to the mbed Timer class
};

/**
* @brief Mbed platform specific timer platform ops structure
*/
extern const struct no_os_timer_platform_ops mbed_timer_ops;

/******************************************************************************/
/*****************************Function Declarations****************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /*MBED_TIMER_H_ */
