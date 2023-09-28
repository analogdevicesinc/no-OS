/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_error_types.h
* \brief Contains common error data types for API Error messaging
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_ERROR_TYPES_H_
#define _ADI_COMMON_ERROR_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
*  \brief ADI common error structure
*/
typedef struct adi_common_ErrStruct
{
    int32_t     errSource;    /*!< Current source of error returned */
    int32_t     errCode;      /*!< Current error code returned */
    uint32_t    errLine;      /*!< Line of the source code where the error was returned */
    const char* errFunc;      /*!< Function name where the error occurred */
    const char* errFile;      /*!< File name where the error occurred */
    const char* varName;      /*!< Variable name which has the error */
    const char* errormessage; /*!< Error message to describe the error */
    int32_t     lastAction;   /*!< Previous action detected */
    int32_t     newAction;    /*!< Current action detected */
    uint8_t     logEnable;    /*!< Log errors enable flag */
} adi_common_ErrStruct_t;

/**
*  \brief Unique Common error sources for Common layer.
*
**  Common error sources will be given the following range: 0x0000 - 0x0FFF
*   Specific error sources for different devices/boards will be given the following range:
*   Devices 0xd000 - 0xdFFF where d is the device number
*
* TODO: replace by unique error source
* Shall this become common layer:
* -HAL
* -COMMON
* -DEVICE
* -BF
* -API
*/
typedef enum adi_common_ErrSources
{
    ADI_COMMON_ERRSRC_API,      /*!< Error detected in API */
    ADI_COMMON_ERRSRC_ADI_HAL,  /*!< ADI HAL Error Src: Error codes defined by adi_common_hal_Err_e */
    ADI_COMMON_ERRSRC_DEVICEBF, /*!< ADI BitField Error Src: Error codes defined by adrv9025_BF_Err_t */
    ADI_COMMON_ERRSRC_DEVICEHAL /*!< ADI Device HAL Error Src: Error codes defined in device error extension */
} adi_common_ErrSources_e;

/**
* \brief Unique Error source for common layer
*        this needs to be extended by the device specific source code files
*
* Common error sources will be given the following range: 0x0000 - 0x0FFF
* Specific error sources for different devices/boards will be given the following range:
* Devices 0xd000 - 0xdFFF where d is the device number
*/
#define ADI_COMMON_SRC_ERROR 0x0001 /*!< Error detected in adi_common_error.c */
#define ADI_COMMON_SRC_LOG   0x0002 /*!< Error detected in adi_common_log.c */
#define ADI_COMMON_SRC_HAL   0x0003 /*!< Error detected in adi_common_hal.c */

/**
* \brief Unique Common error codes for API.
*        Each error condition in the library shall have its own value
*        to ease debug of errors.
*
* Common error codes will be given the following range: 0x0000 - 0x0FFF
* Specific error codes for different devices/boards will be given the following range:
* Devices 0xd000 - 0xdFFF where d is the device number
*/
#define ADI_COMMON_ERR_OK              0x0000 /*!< No error detected */
#define ADI_COMMON_ERR_INV_PARAM       0x0001 /*!< Invalid parameter detected in function */
#define ADI_COMMON_ERR_NULL_PARAM      0x0002 /*!< Null parameter detected in function */
#define ADI_COMMON_ERR_API_FAIL        0x0003 /*!< Error detected in API function */
#define ADI_COMMON_ERR_SPI_FAIL        0x0004 /*!< SPI interface error detected */
#define ADI_COMMON_ERR_CPU_EXCEPTION   0x0008 /*!< CPU exception detected */

/**
*  \brief list of Common Recovery Actions used as return values from APIs.
*         More information about the action is given in the error structure at run time
*         Actions are divided into:
*         Errors are negative, they indicate that an action shall be taken in order to continue the normal operation of the device
*         Warnings are positive, they done break the normal operation of the device, they indicate that at some stage an action should be taken
*/
#define  ADI_COMMON_ACT_WARN_CHECK_PARAM    (3)  /*!< API OK - Parameter exceeds the range of values allowed */
#define  ADI_COMMON_ACT_WARN_RERUN_FEATURE  (2)  /*!< API OK - Rerun feature */
#define  ADI_COMMON_ACT_WARN_RESET_LOG      (1)  /*!< API OK - LOG Not working */
#define  ADI_COMMON_ACT_NO_ACTION           (0)  /*!< API OK - NO ACTION REQUIRED */
#define  ADI_COMMON_ACT_ERR_CHECK_TIMER     (-1) /*!< API OK - timer not working */
#define  ADI_COMMON_ACT_ERR_CHECK_PARAM     (-2) /*!< API OK - INVALID PARAM */
#define  ADI_COMMON_ACT_ERR_RESET_INTERFACE (-3) /*!< API NG - Interface Not Working */
#define  ADI_COMMON_ACT_ERR_RESET_FEATURE   (-4) /*!< API NG - Reset feature */
#define  ADI_COMMON_ACT_ERR_RESET_MODULE    (-5) /*!< API NG - Module Not working */
#define  ADI_COMMON_ACT_ERR_RESET_FULL      (-6) /*!< API NG - FULL RESET REQUIRED */

#ifdef __cplusplus
}
#endif

#endif /* _ADI_COMMON_ERROR_TYPES_H_ */
