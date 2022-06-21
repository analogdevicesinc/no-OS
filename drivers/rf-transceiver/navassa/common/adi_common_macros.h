/**
* \file
* \brief Contains ADI Transceiver general purpose macros.
* 
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_MACROS_H_
#define _ADI_COMMON_MACROS_H_

#ifdef __KERNEL__
#include <linux/math64.h>
#else
#define DIV_ROUND_UP(x, y)		  (((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y)	  (((x) + (y) / 2) / (y))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* yes, we do need the extra parentheses, in case there is a typecast before multiplication */
#define KILO_TO_BASE_UNIT(intVal_k) ((intVal_k) * 1000) 
#define MEGA_TO_BASE_UNIT(intVal_M) ((intVal_M) * 1000 * 1000)
#define GIGA_TO_BASE_UNIT(intVal_G) ((intVal_M) * 1000 * 1000 * 1000)

#ifdef ADI_VALIDATE_PARAMS
#define ADI_PERFORM_VALIDATION(rangeCheckFcn, devicePtr, ...) \
    do { \
        ADI_NULL_DEVICE_PTR_RETURN(devicePtr); \
        ADI_FUNCTION_ENTRY_LOG(&devicePtr->common); \
        ADI_EXPECT(rangeCheckFcn, devicePtr, ##__VA_ARGS__); \
    } while (0)
#else
#define ADI_PERFORM_VALIDATION(rangeCheckFcn, devicePtr, ...)
#endif // ADI_VALIDATE_PARAMS

#define ADI_ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
    
#ifdef DEBUG
#define ADI_SHOULD_NOT_EXECUTE(devicePtr) \
    do { \
        ADI_ERROR_REPORT(&devicePtr->common, \
                         ADI_COMMON_ERRSRC_API, \
                         ADI_COMMON_ERR_API_FAIL, \
                         ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED, \
                         NULL, \
                         "If ADI_VALIDATE_PARAMS is 1, this line should never be executed. Fix validation logic"); \
       ADI_API_RETURN(devicePtr); \
    } while (0)
#else
#define ADI_SHOULD_NOT_EXECUTE(devicePtr) \
    do { \
        ADI_ERROR_REPORT(&devicePtr->common, \
                         ADI_COMMON_ERRSRC_API, \
                         ADI_COMMON_ERR_INV_PARAM, \
                         ADI_COMMON_ACT_ERR_CHECK_PARAM, \
                         NULL, \
                         "Unknown error occurred. Try enabling ADI_VALIDATE_PARAMS for more info"); \
       ADI_API_RETURN(devicePtr); \
    } while (0)
#endif // DEBUG


#ifdef __cplusplus
}
#endif
#endif  /* _ADI_COMMON_MACROS_H_ */
