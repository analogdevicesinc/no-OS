/**
 * \file
 * \brief Contains Common API error handling function prototypes and macros 
 * that will be used by the API and will call functions in adi_common_error.c
 * 
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
 */

/**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_COMMON_ERROR_H_
#define _ADI_COMMON_ERROR_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stdio.h>
#endif

#include "adi_common_error_types.h"
#include "adi_common_types.h"
#include "adi_common_macros.h"

/**
* \brief Macro to check if device pointer is a valid pointer
* if null pointer detected return ADI_COMMON_ACT_ERR_CHECK_PARAM action
* 
* \param ptr Pointer to be checked
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_NULL_DEVICE_PTR_RETURN(ptr)\
if(ptr == NULL)\
{ \
     return ADI_COMMON_ACT_ERR_CHECK_PARAM; \
}

/**
* \brief Macro to check if a pointer is a valid pointer
* This macro upon detection of a null pointer will report the error and return ADI_COMMON_ACT_ERR_CHECK_PARAM action
* 
* \param commonDev Pointer to the common device structure
* \param ptr Pointer to be checked
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_NULL_PTR_RETURN(commonDev, ptr)\
{ \
    if(ptr == NULL) \
    { \
         ADI_ERROR_REPORT((adi_common_Device_t *)(commonDev), ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_NULL_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, ptr, "NULL Pointer passed"); \
         return ADI_COMMON_ACT_ERR_CHECK_PARAM; \
    } \
}

/**
* \brief Macro to check if return from AI function is needed
*
* \param a Action to be checked 
* 
* \retval Action passed
*/
#define ADI_ERROR_RETURN(a) if(a < ADI_COMMON_ACT_NO_ACTION) { return a; }

/**
* \brief Macro to close a file and return if an error is detected
*
* \param 1 an Action to be checked
* \param 2 the file to close
* 
* \retval Action passed
*/
#define ADI_ERROR_CLOSE_RETURN(action, fileToClose) if(action < ADI_COMMON_ACT_NO_ACTION) {(void)fclose(fileToClose); return action;}

#ifdef ADI_COMMON_VERBOSE
/**
* \brief Macro to perform error reporting
* This macro will call adi_common_ErrorReport and extract the necessary information from the API function using the preprocessors:
* __FILE__ will represent the full path of the current API function file where the error happend,
* __FUNCTION__ will expand to the name of the function in the API where the error was produced,
* __LINE__ theis preprocessor macro expands to the line number inside the API function where the error happened.
* 
* \param commonDev Pointer to the common device structure 
* \param errorSource Error source of error code of type enum adi_common_ErrSources_e
* \param error Detected error
* \param action Action to recover from
* \param variable variable that had the error; can be NULL to indicate the error does not depend on a variable only
* \param customError error message to be written to the struct
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_ERROR_REPORT(commonDev, errorSource, error, action, variable, customError) \
    adi_common_ErrorReport((adi_common_Device_t *)(commonDev), (adi_common_ErrSources_e)errorSource, (int32_t)error, (int32_t)action, __FILE__, __FUNCTION__, __LINE__, #variable, customError)
#else 
#define ADI_ERROR_REPORT(commonDev, errorSource, errorCode, action, variable, customError) \
do \
{ \
    ((adi_common_Device_t *)(commonDev))->error.errSource = errorSource; \
    ((adi_common_Device_t *)(commonDev))->error.errCode = errorCode; \
    ((adi_common_Device_t *)(commonDev))->error.errFile = __FILE__; \
    ((adi_common_Device_t *)(commonDev))->error.errFunc = __FUNCTION__; \
    ((adi_common_Device_t *)(commonDev))->error.errLine = __LINE__; \
    ((adi_common_Device_t *)(commonDev))->error.varName = #variable; \
    ((adi_common_Device_t *)(commonDev))->error.lastAction = (errorCode == 0) ? ADI_COMMON_ACT_NO_ACTION : ((adi_common_Device_t *)(commonDev))->error.newAction; \
    ((adi_common_Device_t *)(commonDev))->error.newAction = (errorCode == 0) ? ADI_COMMON_ACT_NO_ACTION : action; \
} while (0) ;
#endif /* ADI_COMMON_VERBOSE */

/**
 * \brief Macro to check a value is between a minimum and maximum (inclusive).
 *
 * This macro will create boilerplate code to:
 *		1) Test if value is less than minimum or greater than maximum.
 *		2) If so, report error as invalid parameter with appropriate error message.
 *		3) Then perform error return with the current error state.
 *		
 * \param devicePtr function pointer to the device handle
 * \param value the value to be range-checked
 * \param minimum the minimum allowable value in the range
 * \param maximum the maximum allowable value in the range
 **/
#ifdef ADI_COMMON_VERBOSE
#define ADI_RANGE_CHECK_X(devicePtr, value, minimum, maximum, formatSpecifier) \
if ((value < minimum) || (value > maximum)) \
{ \
    snprintf(devicePtr->common.error.errormessage, \
        sizeof(devicePtr->common.error.errormessage), \
        "Invalid parameter value. %s was " #formatSpecifier ", but must be between " #formatSpecifier " and " #formatSpecifier ", inclusive.", \
        #value, \
        value, \
        minimum, \
        maximum); \
    \
    ADI_ERROR_REPORT(&devicePtr->common, \
        ADI_COMMON_ERRSRC_API, \
        ADI_COMMON_ERR_INV_PARAM, \
        ADI_COMMON_ACT_ERR_CHECK_PARAM, \
        devicePtr, \
        devicePtr->common.error.errormessage); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
}

/* Legacy - no format specifier defaults to %d */
#define ADI_RANGE_CHECK(devicePtr, value, minimum, maximum) ADI_RANGE_CHECK_X(devicePtr, value, minimum, maximum, "%d")
#else
#define ADI_RANGE_CHECK(devicePtr, value, minimum, maximum) \
if ((value < minimum) || (value > maximum)) \
{ \
    return ADI_COMMON_ACT_ERR_CHECK_PARAM; \
}

#define ADI_RANGE_CHECK_X(devicePtr, value, minimum, maximum, formatSpecifier) \
if ((value < minimum) || (value > maximum)) \
{ \
    return ADI_COMMON_ACT_ERR_CHECK_PARAM; \
}
#endif

/**
* \brief Macro to perform succinct error handling
*
* This macro will create boilerplate code to:
*     1) Create a local scope return value
*     2) Invoke a function, typically a bitfield set or get
*     3) Call ADI_ERROR_REPORT macro with the device's common error message
*     4) Call ADI_ERROR_RETURN macro
* 
* \param fcnPtr function pointer to the Get or Set function to be invoked
* \param devicePtr function pointer to the device handle
* \param __VA_ARGS__ variadic macro to handle the remaining parameters passed to fcnPtr
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_EXPECT(fcnPtr, devicePtr, ...) \
{\
    int32_t _recoveryAction = ADI_COMMON_ACT_NO_ACTION; \
    _recoveryAction = fcnPtr(devicePtr, ##__VA_ARGS__); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, devicePtr->common.error.errCode, _recoveryAction, NULL, devicePtr->common.error.errormessage); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
}

/**
* \brief Macro to perform succinct error handling with a custom error message
*
* This macro will create boilerplate code to:
*     1) Create a local scope return value
*     2) Invoke a function, typically a bitfield set or get
*     3) Call ADI_ERROR_REPORT macro with custom message
*     4) Call ADI_ERROR_RETURN macro
* 
* \param errMsg error message to be logged
* \param fcnPtr function pointer to the Get or Set function to be invoked
* \param devicePtr function pointer to the device handle
* \param __VA_ARGS__ variadic macro to handle the remaining parameters passed to fcnPtr
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_MSG_EXPECT(errMsg, fcnPtr, devicePtr, ...) \
{\
    int32_t _recoveryAction = ADI_COMMON_ACT_NO_ACTION; \
    _recoveryAction = fcnPtr(devicePtr, ##__VA_ARGS__); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, devicePtr->common.error.errCode, _recoveryAction, NULL, errMsg); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
}

/**
* \brief Macro to perform succinct error handling used during function entry
*
* This macro will create boilerplate code to:
*     1) Test for device null pointer
*     2) Log entry into this function
*     
* \param devicePtr function pointer to the device handle
* \param logLevel common log enum to represent the level of logging
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_ENTRY_EXPECT(devicePtr) \
{ \
    ADI_NULL_DEVICE_PTR_RETURN(devicePtr); \
    ADI_FUNCTION_ENTRY_LOG(&devicePtr->common); \
}

/**
* \brief Macro to perform succinct error handling used during function entry
*
* This macro will create boilerplate code to:
*     1) Test for device null pointer
*     2) Log entry into this function
*     3) Test for null pointer which should be a struct pointer
*     
* \param devicePtr function pointer to the device handle
* \param logLevel common log enum to represent the level of logging
* \param ptr pointer to be validated
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_ENTRY_PTR_EXPECT(devicePtr, ptr) \
{ \
    ADI_ENTRY_EXPECT(devicePtr); \
    ADI_NULL_PTR_RETURN(&devicePtr->common, ptr); \
}

/**
* \brief Macro to perform succinct error handling used during function entry with an array
*
* This macro will create boilerplate code to:
*     1) Test for device null pointer
*     2) Log entry into this function
*     3) Test for null pointer which should be an array
*     4) Test that the array size is greater than zero
*     
* \param devicePtr function pointer to the device handle
* \param ptr pointer to be validated
* \param arraySize size of the ptr array
* \param logLevel common log enum to represent the level of logging
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_ENTRY_PTR_ARRAY_EXPECT(devicePtr, ptr, arraySize) \
{\
    ADI_ENTRY_PTR_EXPECT(devicePtr, ptr); \
    if(arraySize == 0) \
    { \
        ADI_ERROR_REPORT(&devicePtr->common, \
            ADI_COMMON_ERRSRC_API, \
            ADI_COMMON_ERR_INV_PARAM, \
            ADI_COMMON_ACT_ERR_CHECK_PARAM, \
            arraySize, \
            "Invalid arraySize"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
    } \
}

/**
* \brief Macro to return an error code from an API function.
* 
* This macro returns from an API function with the current
* error state.
*     
* \param devicePtr pointer to device handle; can be any device type
* 
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_API_RETURN(devicePtr) \
{ \
    return (((adi_common_Device_t *)devicePtr)->error.newAction); \
}

#ifdef __cplusplus
extern "C" {
#endif

/*
* *******************************
* ADI Common error functions
* *******************************
*/

#ifndef CLIENT_IGNORE

/**
 * \brief Common API error reporting facility
 *  
 * Error handling to assign actions:
 * If actionToRecover is ADI_COMMON_ACT_NO_ACTION then nothing gets logged or reported, if the previous new action in the error structure
 * is different to ADI_COMMON_ACT_NO_ACTION the error struct will be cleared and then the previous action will be stored in the lastAction 
 * member.
 * 
 * If not ADI_COMMON_ACT_NO_ACTION then check latest action previously logged in the error structure and verify if the new found action 
 * is of a higher priority level than the last error in the structure, 
 * If it is then we need to demote the error.newAction and log the previous action again
 * demoting is done by assign it to error.lastAction.
 * If action is a warning then it gets logged and assigned to the error structure the same way as the error action, if an error 
 * action occurs after a warning the warning will be demoted and could be accessed through  error.lastAction
 *
 * \param commonDev pointer to adi_common_Device_t
 * \param errSrc Error source of error code of type enum adi_common_ErrSources_e
 * \param detErr Detected error
 * \param actionToRecover Action to recover from
 * \param fileName file name of the file where the error was detected
 * \param funcName function name of the function where the error was detected
 * \param lineNum line number of where the error was detected
 * \param varName variable name that had the error, this can be NULL and it will mean that there is an error not depending on a variable only
 * \param customError error message to be written to the struct
 */
void adi_common_ErrorReport(adi_common_Device_t *commonDev,
                            adi_common_ErrSources_e errSrc,
                            int32_t detErr, 
                            int32_t actionToRecover,
                            const char* fileName,
                            const char* funcName,
                            uint32_t lineNum,
                            const char* varName,
                            const char* customError);


/**
 * \brief Function to clear existing error
 * 
 * \param commonDev pointer to adi_common_Device_t
 * 
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_common_ErrorClear(adi_common_Device_t *commonDev);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _ADI_COMMON_ERROR_H_ */
