/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_common_error.h
 * \brief Contains Common API error handling function prototypes and macros
 * that will be used by the API and will call functions in adi_common_error.c
 *
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
* ADI common lib Version: 0.0.1.1
 */

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADI_COMMON_ERROR_H_
#define _ADI_COMMON_ERROR_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdlib.h>
#endif
#include "adi_common_error_types.h"
#include "adi_common_types.h"
#include "adi_common_macros.h"

/*
* *******************************
* ADI Common error macros
* *******************************
*/

/**
* \brief Macro to check if device pointer is a valid pointer
*        if null pointer detected return ADI_COMMON_ACT_ERR_CHECK_PARAM action
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
*        This macro upon detection of a null pointer will report the error and
*        return ADI_COMMON_ACT_ERR_CHECK_PARAM action
*
* \param devHalInfo pointer to the hal layer to use logging
* \param errorPtr Pointer to the error data structure
* \param ptr Pointer to be checked
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_NULL_PTR_RETURN(commonDev, ptr)\
{ \
    if(ptr == NULL) \
    { \
         ADI_ERROR_REPORT(commonDev, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_NULL_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, ptr, "NULL Pointer passed"); \
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
* \param1 an Action to be checked
* \param2 the file to close
*
* \retval Action passed
*/
#define ADI_ERROR_CLOSE_RETURN(action, fileToClose) if(action < ADI_COMMON_ACT_NO_ACTION) {(void)fclose(fileToClose); return action;}

#if ADI_COMMON_VERBOSE > 0
#if ADI_COMMON_VARIABLE_USAGE > 0
/*
* Macro to print variable name as a string, used in Error reporting facility
*/
#ifndef GET_VARIABLE_NAME
#define GET_VARIABLE_NAME(Variable) (#Variable)
#endif
#else
#ifndef GET_VARIABLE_NAME
            #define GET_VARIABLE_NAME(Variable) ("")
#endif
#endif /* ADI_COMMON_VARIABLE_USAGE */

/**
* \brief Macro to perform error reporting
*        This macro will call adrv9025_ErrorReport and extract the
*        necessary information from the API function using the preprocessors:
* __FILE__ will represent the full path of the current API function file where the error happened,
* __FUNCTION__ will expand to the name of the function in the API where the error was produced,
* __LINE__ theis preprocessor macro expands to the line number inside the API function where the error happened.
*
*
* \param devHalInfo pointer to the hal layer to use logging
* \param errorPtr Pointer to the error data structure
* \param errorSource Error source of error code of type enum adi_common_ErrSources_e
* \param error Detected error
* \param action Action to recover from
* \param fileName file name of the file where the error was detected
* \param funcName function name of the function where the error was detected
* \param lineNum line number of where the error was detected
* \param variable variable name that had the error,GET_VARIABLE_NAME(variable) will print the variable name that needs to be checked, this can be NULL and it will mean that there is an error not depending on a variable only
* \param customError error message to be written to the struct
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM
*/
#define ADI_ERROR_REPORT(commonDev, errorSource, error, action, variable, customError) \
    adrv9025_ErrorReport(commonDev, (adi_common_ErrSources_e)errorSource, (int32_t)error, (int32_t)action, __FILE__, __FUNCTION__, __LINE__, GET_VARIABLE_NAME(variable), customError)

#else
#define ADI_ERROR_REPORT(commonDev, errorSource, error, action, variable, customError) \
    adrv9025_ErrorReport(commonDev, (adi_common_ErrSources_e)errorSource, (int32_t)error, (int32_t)action, "", "", __LINE__, "", "")
#endif /* ADI_COMMON_VERBOSE */

#ifdef __cplusplus
extern "C" {
#endif

/*
* *******************************
* ADI Common error functions
* *******************************
*/

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
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
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
void adrv9025_ErrorReport(adi_common_Device_t*    commonDev,
                            adi_common_ErrSources_e errSrc,
                            int32_t                 detErr,
                            int32_t                 actionToRecover,
                            const char*             fileName,
                            const char*             funcName,
                            uint32_t                lineNum,
                            const char*             varName,
                            const char*             customError);

/**
 * \brief Function to clear existing error
 *
 * \param commonDev pointer to adi_common_Device_t
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adrv9025_ErrorClear(adi_common_Device_t* commonDev);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_COMMON_ERROR_H_ */
