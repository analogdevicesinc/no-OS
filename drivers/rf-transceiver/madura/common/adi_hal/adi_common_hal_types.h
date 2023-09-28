/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_hal_types.h
* \brief Contains ADI Transceiver common hal types.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_HAL_TYPES_H_
#define _ADI_COMMON_HAL_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_common_user.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
*  \brief Enum of possible Errors Detected by HAL layer to be communicated
*         to ADI APIs.
*/
typedef enum adi_common_hal_Err
{
    ADI_COMMON_HAL_OK = 0, /*!< HAL function successful. No error Detected */
    ADI_COMMON_HAL_SPI_FAIL, /*!< HAL SPI operation failure. SPI controller Down */
    ADI_COMMON_HAL_GPIO_FAIL, /*!< HAL GPIO function Failure */
    ADI_COMMON_HAL_TIMER_FAIL, /*!< HAL Timer function Failure */
    ADI_COMMON_HAL_LOGGING_FAIL, /*!< HAL Logging function Failure */
    ADI_COMMON_HAL_LOGGGING_LEVEL_FAIL, /*!< HAL Logging level Failure */
    ADI_COMMON_HAL_WAIT_TIMEOUT, /*!< HAL function Timeout */
    ADI_COMMON_HAL_GEN_SW, /*!< HAL function failed due to general invalid  HAL data*/
    ADI_COMMON_HAL_FUNCTION_NOT_IMP, /*!< HAL function error, not implemented in HAL layer */
    ADI_COMMON_HAL_LIBRARY_NOT_AVAILABLE, /*!< HAL function error, HAL layer library not found or available in this build */
    ADI_COMMON_HAL_WARNING, /*!< HAL function warning that non critical error was detected*/
    ADI_COMMON_HAL_FPGA_FAIL /*!< FPGA hal function failure */
} adi_common_hal_Err_e;

/**
* \brief Data structure to hold cache device settings
*/
typedef struct adi_common_Cache
{
    uint16_t wrCacheIdx;            /*!<used to store the tx spi caching index  */
    uint16_t rdCacheIdx;            /*!<used to store the rx spi caching index  */
    uint16_t spiWrBufferCnt;        /*!<tracks the spi buffer space needed to execute the cached data. */
    uint16_t spiRdBufferCnt;        /*!<tracks the spi buffer space needed to execute the cached data.*/
    uint8_t HW_RMW_Enabled;         /*!<used to indicate if the Hardware Read Modify Write is enabled in the stream processor */
    uint8_t wrCacheState;           /*!<used to store the tx spi caching state, one of Off, BitField, Global, merging */
    uint8_t rdCacheState;           /*!<used to store the rx spi caching state, one of Off, BitField, Global */
    uint8_t mergeDistance;          /*!<determines how far back in the cache to search for register address to merge */
    uint8_t wrFlushEn;              /*!<internal cache use only. temporarily disables write cache flushing */
    uint8_t wrOnly;                 /*!<supports broad cast mode. */
    uint32_t wrCache[ADI_COMMON_HAL_SPIARRAYSIZE / 3]; /*!<used to store the tx spi caching one element per byte */
    uint32_t rdCache[ADI_COMMON_HAL_SPIARRAYSIZE / 3]; /*!<used to store the rx spi caching one element per byte */
} adi_common_Cache_t;

#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_HAL_TYPES_H_ */
