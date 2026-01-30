/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adi_fpgagen6_hal_types.h
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv904x_hal.c
 *
 * FPGA API Version: 2.15.0.4
 */

#ifndef _ADI_FPGAGEN6_HAL_TYPES_H_
#define _ADI_FPGAGEN6_HAL_TYPES_H_

#define ADI_FPGAGEN6_CONVERT_WORD_TO_BYTE_ADDRESS(wordOffset) (wordOffset) * 4

#ifndef BBIC_RAMARRAYSIZE
#define BBIC_RAMARRAYSIZE (1024*4)     /*!< 4K words to allow up to 16KB */
#endif // !BBIC_RAMARRAYSIZE

#endif
