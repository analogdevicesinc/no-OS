/**
 * \file
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv9001_hal.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef ADRV9001_SPI_TYPES_H_
#define ADRV9001_SPI_TYPES_H_

#ifndef __KERNEL__
#include <stdint.h>
#include <stddef.h>
#endif

#include "adi_adrv9001.h"
#include "adi_common_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_ADDR_SIZE 16
#define SPI_DATA_SIZE 8
#define SPI_MASK_SIZE 8
#define HW_RMW_DATA_BYTES 12

/**
 *  \brief Enum of possible HAL layer error codes.
 */
typedef enum adrv9001_Hal_Err
{
    ADRV9001HAL_OK = 0,        /*!< HAL function successful. No error Detected */
    ADRV9001HAL_SPI_FAIL,      /*!< HAL SPI operation failure. SPI controller Down */
    ADRV9001HAL_GPIO_FAIL,     /*!< HAL GPIO function Failure */
    ADRV9001HAL_TIMER_FAIL,    /*!< HAL Timer function Failure */
    ADRV9001HAL_WAIT_TIMEOUT,  /*!< HAL function Timeout */
    ADRV9001HAL_LOG_FAIL,
    ADRV9001HAL_LOG_LEVEL_FAIL,
    ADRV9001HAL_HAL_MODE_FAIL,
    ADRV9001HAL_GEN_SW,        /*!< HAL function failed due to general invalid  HAL data*/
    ADRV9001HAL_WARNING,       /*!< HAL function warning that non critical error was detected*/
    ADRV9001HAL_BUFFER_OVERFLOW
} adrv9001_Hal_Err_e;


#ifdef __cplusplus
}
#endif

#endif /* ADRV9001_HAL_TYPES_H_ */
