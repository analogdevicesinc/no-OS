/**
* Copyright 2020 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adi_ad9528_error_types.h
 *
 * \brief Device Specific Error Types
 *
 */
 
#ifndef _ADI_AD9528_ERROR_TYPES_H_
#define _ADI_AD9528_ERROR_TYPES_H_

#include "adi_common_error.h"

/*
*  \brief   AD9528 File Abstractions
*/
typedef enum
{
    ADI_AD9528_FILE_PUBLIC_ERROR     = 0x0U,
    ADI_AD9528_FILE_PUBLIC_CORE,
    ADI_AD9528_FILE_PUBLIC_HAL,
    ADI_AD9528_FILE_PUBLIC_UTILITIES
} adi_ad9528_File_e;


typedef enum
{
    ADI_AD9528_ERR_PLL_LOCK_FAILED = 0U,
    ADI_AD9528_ERR_UNKNOWN
} adi_ad9528_ErrCode_e;


/*
*  \brief   AD9528 Error Source Mapping
*/
typedef enum
{
    ADI_AD9528_ERRSRC_NONE      = ADI_COMMON_ERRSRC_NONE_IDX,           /* No Error/Source; Common Error Source Mapping */
    ADI_AD9528_ERRSRC_API       = ADI_COMMON_ERRSRC_API_IDX,            /* API;             Common Error Source Mapping */
    ADI_AD9528_ERRSRC_HAL       = ADI_COMMON_ERRSRC_HAL_IDX,            /* HAL;             Common Error Source Mapping */
    ADI_AD9528_ERRSRC_DEVICEHAL = ADI_COMMON_ERRSRC_DEVICEHAL_IDX,      /* Device HAL;      Common Error Source Mapping */
    ADI_AD9528_ERRSRC_CLK,
    ADI_AD9528_ERRSRC_UNKNOWN
} adi_ad9528_ErrSource_e;

/* Error Table for API to relate AD9528 Error Codes to Strings/Recovery Actions */
typedef adi_common_ErrTableRow_t ad9528_ClkErr_t;

#endif  /* _ADI_AD9528_ERROR_TYPES_H_ */
