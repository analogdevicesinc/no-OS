/**
 * \file
 * \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_hal.c
 *
 * ADI common lib Version: $ADI_COMMON_LIB_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_COMMON_HAL_WRAPPER_H_
#define _ADI_COMMON_HAL_WRAPPER_H_

/* include standard types and definitions */
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_common_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

/**
* \brief Used to sleep for a given number of microSeconds.
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
* \param time_us The number of micro seconds to sleep
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_wrapper_Wait_us(adi_common_Device_t *commonDev, uint32_t time_us);

#endif

#ifdef __cplusplus
}
#endif

#endif
