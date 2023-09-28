/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_common_hal.h
 * \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_hal.c
 *
 * ADI common lib Version: 0.0.1.1
 */

 /**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADI_COMMON_HAL_H_
#define _ADI_COMMON_HAL_H_

/* include standard types and definitions */
#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_common_log.h"



/*========================================
 * Prototypes
 *=======================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * ADI Device Hardware Control Functions
*===========================================================================*/

/**
* \brief Used to initialise the HAL hardware.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_HwOpen(adi_common_Device_t *commonDev);

/**
* \brief Used to close the HAL hardware.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_HwClose(adi_common_Device_t *commonDev);

/**
* \brief Used to reset the HAL hardware.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
* \param pinLevel Pin level to be set 1 will held the reset line high, 0 will held the reset line low
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_HwReset(adi_common_Device_t *commonDev, uint8_t pinLevel);

/**
* \brief Used to sleep for a given number of microSeconds.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
* \param time_us The number of micro seconds to sleep
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_hal_Wait_us(adi_common_Device_t *commonDev, uint32_t time_us);

/**
* \brief Used to sleep for a given number of milliSeconds.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
* \param time_ms The number of milli seconds to sleep.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_Wait_ms(adi_common_Device_t *commonDev, uint32_t time_ms);

/**
* \brief A helper function used to check that hardware is available to read from or write to.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_hal_HwVerify(adi_common_Device_t *commonDev);

#ifdef __cplusplus
}
#endif
#endif
