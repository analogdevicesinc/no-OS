/***************************************************************************//**
 *   @file   maxim_trng.h
 *   @brief  MAX32655 specific header for TRNG driver
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_TRNG_H
#define MAXIM_TRNG_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_trng.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Max32655 specific TRNG platform ops structure
 */
extern const struct no_os_trng_platform_ops max_trng_ops;

#endif // MAXIM_TRNG_H
