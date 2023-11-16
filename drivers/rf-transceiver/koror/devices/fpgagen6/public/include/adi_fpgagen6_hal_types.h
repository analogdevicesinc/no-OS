/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_hal_types.h
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv904x_hal.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_HAL_TYPES_H_
#define _ADI_FPGAGEN6_HAL_TYPES_H_

#define ADI_FPGAGEN6_CONVERT_WORD_TO_BYTE_ADDRESS(wordOffset) (wordOffset) * 4

#ifndef BBIC_RAMARRAYSIZE
#define BBIC_RAMARRAYSIZE (1024*4)     /*!< 4K words to allow up to 16KB */
#endif // !BBIC_RAMARRAYSIZE

#endif
