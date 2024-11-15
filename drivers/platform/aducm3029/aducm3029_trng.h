/***************************************************************************//**
 *   @file   aducm3029_trng.h
 *   @brief  ADuCM302x specific header for TRNG driver
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ADUCM3029_TRNG_H
#define ADUCM3029_TRNG_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <drivers/rng/adi_rng.h>
#include "no_os_trng.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* LenReload: 0 - 4095 */
#define NO_OS_TRNG_CNT_VAL		4095
/* Prescaler: 0 - 10 */
#define NO_OS_TRNG_PRESCALER		2
/* Aducm device ID */
#define NO_OS_ADUCM_TRNG_DEVICE_ID	0

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Stucture holding the TRNG descriptor. */
struct aducm3029_trng_desc {
	/*
	 * Memory used by the DFP
	 * At least ADI_RNG_MEMORY_SIZE bytes of 4 bytes aligned memory are
	 * needed by the DFP driver. The formula is to align memory only.
	 */
	uint32_t	dev_mem[(ADI_RNG_MEMORY_SIZE + 3) / 4];
	/* DFP Hanler */
	ADI_RNG_HANDLE	dev;
};

/**
 * @brief ADuCM3029 specific TRNG platform ops structure
 */
extern const struct no_os_trng_platform_ops aducm_trng_ops;

#endif // ADUCM3029_TRNG_H
