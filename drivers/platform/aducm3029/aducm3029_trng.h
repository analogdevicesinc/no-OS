/***************************************************************************//**
 *   @file   aducm3029_trng.h
 *   @brief  ADuCM302x specific header for TRNG driver
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifndef ADUCM3029_TRNG_H
#define ADUCM3029_TRNG_H

#include <drivers/rng/adi_rng.h>
#include "no_os_trng.h"

/* LenReload: 0 - 4095 */
#define NO_OS_TRNG_CNT_VAL		4095
/* Prescaler: 0 - 10 */
#define NO_OS_TRNG_PRESCALER		2
/* Aducm device ID */
#define NO_OS_ADUCM_TRNG_DEVICE_ID	0

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
