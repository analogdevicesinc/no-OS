/***************************************************************************//*
 * @file   mbed_timer.h
 * @brief  Header containing extra types for Timer interface
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
