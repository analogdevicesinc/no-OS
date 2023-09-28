/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_types.h
* \brief Contains ADI common types.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_TYPES_H_
#define _ADI_COMMON_TYPES_H_

#include "adi_common_error_types.h"
#include "adi_common_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  \brief ADI common device structure
*/
typedef struct adi_common_Device
{
    void                     *devHalInfo; /*!< ADI_HAL Hardware layer settings pointer specific to this ADRV9025 instance */
    adi_common_ErrStruct_t   error;
    adi_common_Cache_t       cacheInfo;   /*!< TODO: add description */
    /* function pointer for datapacking or device specific hal */
} adi_common_Device_t;

typedef uint32_t adi_iter_t;

#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_TYPES_H_ */
