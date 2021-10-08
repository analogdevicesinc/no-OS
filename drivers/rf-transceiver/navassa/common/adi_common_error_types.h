/**
* \file
* \brief Contains common error data types for API Error messaging
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_ERROR_TYPES_H_
#define _ADI_COMMON_ERROR_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

#define ADI_ERROR_MSG_MAX_LEN 314

/**
*  \brief ADI common error structure
*/
typedef struct adi_common_ErrStruct
{
    int32_t errSource;         /*!< Current source of error returned */
    int32_t errCode;           /*!< Current error code returned */
    uint32_t errLine;          /*!< Line of the source code where the error was returned */
    const char *errFunc;       /*!< Function name where the error occurred */
    const char *errFile;       /*!< File name where the error occurred */
    const char *varName;       /*!< Variable name which has the error */
                               /*!< Error message to describe the error */
    char errormessage[ADI_ERROR_MSG_MAX_LEN];
    int32_t lastAction;        /*!< Previous action detected */
    int32_t newAction;         /*!< Current action detected */
    uint8_t logEnable;         /*!< Log errors enable flag */
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
    ADI_COMMON_ERRSRC_API,                    /*!< Error detected in API */
    ADI_COMMON_ERRSRC_ADI_HAL,                /*!< ADI HAL Error Src: Error codes defined by adi_common_hal_Err_e */
    ADI_COMMON_ERRSRC_DEVICEBF,               /*!< ADI BitField Error Src: Error codes defined by adrv9001_BF_Err_t */
    ADI_COMMON_ERRSRC_DEVICEHAL               /*!< ADI Device HAL Error Src: Error codes defined in device error extension */
} adi_common_ErrSources_e;

/**
* \brief Unique Error source for common layer
* this needs to be extended by the device specific source code files
*
* Common error sources will be given the following range: 0x0000 - 0x0FFF
*   Specific error sources for different devices/boards will be given the following range:
*   Devices 0xd000 - 0xdFFF where d is the device number
*/
#define ADI_COMMON_SRC_ERROR    0x0001    /*!< Error detected in adi_common_error.c */
#define ADI_COMMON_SRC_LOG      0x0002    /*!< Error detected in adi_common_log.c */
#define ADI_COMMON_SRC_HAL      0x0003    /*!< Error detected in adi_common_hal.c */

#define ADI_COMMON_SRC_EXTENSION_1  0x1000  /*!< Error source extension point */
#define ADI_COMMON_SRC_EXTENSION_2  0x1100  /*!< Error source extension point */
#define ADI_COMMON_SRC_EXTENSION_3  0x1200  /*!< Error source extension point */
#define ADI_COMMON_SRC_EXTENSION_4  0x1300  /*!< Error source extension point */
#define ADI_COMMON_SRC_EXTENSION_5  0x1400  /*!< Error source extension point */

/**
* \brief Unique Common error codes for API.
*        Each error condition in the library shall have its own value
*        to ease debug of errors.
*
*        Common error codes will be given the following range: 0x0000 - 0x0FFF
*        Specific error codes for different devices/boards will be given the following range:
*        Devices 0xd000 - 0xdFFF where d is the device number
*/
#define ADI_COMMON_ERR_OK               0x0000      /*!< No error detected */
#define ADI_COMMON_ERR_INV_PARAM        0x0001      /*!< Invalid parameter detected in function */
#define ADI_COMMON_ERR_NULL_PARAM       0x0002      /*!< Null parameter detected in function */
#define ADI_COMMON_ERR_API_FAIL         0x0003      /*!< Error detected in API function */
#define ADI_COMMON_ERR_SPI_FAIL         0x0004      /*!< SPI interface error detected */
#define ADI_COMMON_ERR_MEM_ALLOC_FAIL   0x0005      /*!< Memory allocation failed */

#define ADI_COMMON_ERR_EXTENSION_1      0x1000      /*!< Error code extension point */
#define ADI_COMMON_ERR_EXTENSION_2      0x2000      /*!< Error code extension point */
#define ADI_COMMON_ERR_EXTENSION_3      0x3000      /*!< Error code extension point */
#define ADI_COMMON_ERR_EXTENSION_4      0x4000      /*!< Error code extension point */
#define ADI_COMMON_ERR_EXTENSION_5      0x5000      /*!< Error code extension point */


/**
*  \brief list of Common Recovery Actions used as return values from APIs.
*  More information about the action is given in the error structure at run time
*  Actions are divided into:
*  Errors are negative, they indicate that an action shall be taken in order to continue the normal operation of the device
*  Warnings are positive, they don't break the normal operation of the device, they indicate that at some stage an action should be taken
*/
#define ADI_COMMON_ACT_WARN_CHECK_PARAM             (3)         /*!< API OK - Parameter exceeds the range of values allowed */
#define ADI_COMMON_ACT_WARN_RERUN_FEATURE           (2)         /*!< API OK - Rerun feature */
#define ADI_COMMON_ACT_WARN_RESET_LOG               (1)         /*!< API OK - LOG Not working */
#define ADI_COMMON_ACT_NO_ACTION                    (0)         /*!< API OK - NO ACTION REQUIRED */
#define ADI_COMMON_ACT_ERR_CHECK_TIMER              (-1)        /*!< API OK - timer not working */
#define ADI_COMMON_ACT_ERR_CHECK_PARAM              (-2)        /*!< API OK - INVALID PARAM */
#define ADI_COMMON_ACT_ERR_RESET_INTERFACE          (-3)        /*!< API NG - Interface Not Working */
#define ADI_COMMON_ACT_ERR_RESET_FEATURE            (-4)        /*!< API NG - Reset feature */
#define ADI_COMMON_ACT_ERR_RESET_MODULE             (-5)        /*!< API NG - Module Not working */
#define ADI_COMMON_ACT_ERR_RESET_FULL               (-6)        /*!< API NG - FULL RESET REQUIRED */
#define ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED      (-7)        /*!< API NG - API not implemented */

#define ADI_COMMON_ACT_WARN_EXTENSION_1             (100)       /*!< Recovery action warning extension point */
#define ADI_COMMON_ACT_WARN_EXTENSION_2             (200)       /*!< Recovery action warning extension point */
#define ADI_COMMON_ACT_WARN_EXTENSION_3             (300)       /*!< Recovery action warning extension point */
#define ADI_COMMON_ACT_WARN_EXTENSION_4             (400)       /*!< Recovery action warning extension point */
#define ADI_COMMON_ACT_WARN_EXTENSION_5             (500)       /*!< Recovery action warning extension point */

#define ADI_COMMON_ACT_ERR_EXTENSION_1              (-100)      /*!< Recovery action error extension point */
#define ADI_COMMON_ACT_ERR_EXTENSION_2              (-200)      /*!< Recovery action error extension point */
#define ADI_COMMON_ACT_ERR_EXTENSION_3              (-300)      /*!< Recovery action error extension point */
#define ADI_COMMON_ACT_ERR_EXTENSION_4              (-400)      /*!< Recovery action error extension point */
#define ADI_COMMON_ACT_ERR_EXTENSION_5              (-500)      /*!< Recovery action error extension point */

#endif

#ifdef __cplusplus
}
#endif

#endif /* _ADI_COMMON_ERROR_TYPES_H_ */

