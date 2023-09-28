/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_hal_types.h
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv9025_hal.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef ADRV9025_HAL_TYPES_H_
#define ADRV9025_HAL_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include <stddef.h>

#include "adi_adrv9025.h"
#include "adi_common_hal.h"

#ifdef __cplusplus
extern
"C"
 {
#endif

#define SPI_ADDR_SIZE 16
#define SPI_DATA_SIZE 8
#define SPI_MASK_SIZE 8
#define HW_RMW_DATA_BYTES 12


/**
 *  \brief Enum of possible HAL layer error codes.
 */
typedef enum adrv9025_Hal_Err
{
    ADRV9025HAL_OK = 0,        /*!< HAL function successful. No error Detected */
    ADRV9025HAL_SPI_FAIL,      /*!< HAL SPI operation failure. SPI controller Down */
    ADRV9025HAL_GPIO_FAIL,     /*!< HAL GPIO function Failure */
    ADRV9025HAL_TIMER_FAIL,    /*!< HAL Timer function Failure */
    ADRV9025HAL_WAIT_TIMEOUT,  /*!< HAL function Timeout */
    ADRV9025HAL_LOG_FAIL,
    ADRV9025HAL_LOG_LEVEL_FAIL,
    ADRV9025HAL_HAL_MODE_FAIL,
    ADRV9025HAL_GEN_SW,        /*!< HAL function failed due to general invalid  HAL data*/
    ADRV9025HAL_WARNING,       /*!< HAL function warning that non critical error was detected*/
    ADRV9025HAL_BUFFER_OVERFLOW
} adrv9025_Hal_Err_e;


#ifdef __cplusplus
}
#endif

#endif /* ADRV9025_HAL_TYPES_H_ */
