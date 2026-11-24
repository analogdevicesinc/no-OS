/**
 * \file
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper functions implemented in adi_adrv910x_hal.c
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_SPI_TYPES_H_
#define _ADI_ADRV910X_SPI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KERNEL__
#include <stdint.h>
#include <stdbool.h>
#else
#include <linux/types.h>
#endif

#include "adi_adrv910x.h"
#include "adi_common_hal.h"

#define SPI_ADDR_SIZE 16
#define SPI_DATA_SIZE 8
#define SPI_MASK_SIZE 8
#define HW_RMW_DATA_BYTES 12
#define SPI_MASTER_TOTAL_BYTES_MAX 30

/**
 * \brief Enum of possible HAL layer error codes.
 */
typedef enum adrv910x_Hal_Err
{
    ADRV910XHAL_OK = 0,                 /*!< HAL function successful. No error Detected */
    ADRV910XHAL_SPI_FAIL,               /*!< HAL SPI operation failure. SPI controller Down */
    ADRV910XHAL_GPIO_FAIL,              /*!< HAL GPIO function Failure */
    ADRV910XHAL_TIMER_FAIL,             /*!< HAL Timer function Failure */
    ADRV910XHAL_WAIT_TIMEOUT,           /*!< HAL function Timeout */
    ADRV910XHAL_LOG_FAIL,
    ADRV910XHAL_LOG_LEVEL_FAIL,
    ADRV910XHAL_HAL_MODE_FAIL,
    ADRV910XHAL_GEN_SW,                 /*!< HAL function failed due to general invalid HAL data */
    ADRV910XHAL_WARNING,                /*!< HAL function warning that non critical error was detected */
    ADRV910XHAL_BUFFER_OVERFLOW
} adrv910x_Hal_Err_e;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_SPI_TYPES_H_ */
