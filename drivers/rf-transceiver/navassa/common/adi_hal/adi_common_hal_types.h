/**
* \file
* \brief Contains ADI Transceiver common hal types.
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_HAL_TYPES_H_
#define _ADI_COMMON_HAL_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 3 Bytes per SPI transaction * 341 transactions = ~1024 byte buffer size */
/* Minimum HAL_SPIWRITEARRAY_BUFFERSIZE = 18 */
//#define HAL_SPIWRITEARRAY_BUFFERSIZE 1024
#define HAL_SPIWRITEARRAY_BUFFERSIZE 256

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


#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_HAL_TYPES_H_ */
