/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_log_types.h
* \brief Contains ADI common log types.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_LOG_TYPES_H_
#define _ADI_COMMON_LOG_TYPES_H_

#include"adi_common_error_types.h"
#include"adi_common_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* *******************************
* ADI devices common logging enums
* *******************************
*/

/**
*  \brief An enumerated type in bit mask format to list the log message
*         categories or groups.
*/
typedef enum adi_common_LogLevel
{
    ADI_COMMON_LOG_NONE     = 0x0,  /*!< Common Log enum to represent all types of log messages not selected */
    ADI_COMMON_LOG_MSG      = 0x1,  /*!< Common Log enum to represent a log message type */
    ADI_COMMON_LOG_WARN     = 0x2,  /*!< Common Log enum to represent a warning message type */
    ADI_COMMON_LOG_ERR      = 0x4,  /*!< Common Log enum to represent a error message type */
    ADI_COMMON_LOG_API      = 0x8,  /*!< Common Log enum to represent an API function entry for logging purposes */
    ADI_COMMON_LOG_API_PRIV = 0x10, /*!< Common Log enum to represent an Private API function entry for logging purposes */
    ADI_COMMON_LOG_BF       = 0x20, /*!< Common Log enum to represent a BF function entry for logging purposes */
    ADI_COMMON_LOG_HAL      = 0x40, /*!< Common Log enum to represent a ADI HAL function entry for logging purposes */
    ADI_COMMON_LOG_SPI      = 0x80, /*!< Common Log enum to represent a spi transaction type */
    ADI_COMMON_LOG_ALL      = 0xFF  /*!< Common Log enum to represent all types of log messages selected */
} adi_common_LogLevel_e;

#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_LOG_TYPES_H_ */
