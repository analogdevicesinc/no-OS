/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_user.h
*
* User modifiable defines
*
* \brief Contains ADI Transceiver common hal types.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_USER_H_
#define _ADI_COMMON_USER_H_

/* Includes that user can modify with their own libraries as long as the function prototypes matches */
#include "adi_platform_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADI_COMMON_HAL_SPIARRAYSIZE
    #define ADI_COMMON_HAL_SPIARRAYSIZE 256
#endif

#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_USER_H_ */
