/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_log.h
* \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_log.c
*
* ADI common lib Version: 0.0.2.1
*/

#ifndef _ADI_COMMON_LOG_H_
#define _ADI_COMMON_LOG_H_

#include "adi_common_types.h"
#include "adi_common_macros.h"
#include "adi_common_user.h"
#include "adi_common_log_types.h"
#include "adi_common_error_types.h"
#include "adi_common_types.h"

/**
* \brief Macro to Log API function entry
* 
* This macro will call adi_common_LogWrite function with the __func__ preprocessor.
* 
* However, if the device is in time-critical mode the logging is ignored.
* 
* \param[in] commonDev  pointer to adi_common_Device_t
* \param[in] logLevel   Logging Level
*/

#define ADI_FUNCTION_ENTRY_LOG(commonDev, logLevel)                              \
    if (!ADI_COMMON_DEVICE_STATE_IS_TC(*(commonDev)))                            \
    {                                                                            \
            adi_common_LogWrite((commonDev),                                     \
                                (adi_hal_LogLevel_e) (logLevel),                 \
                                "%s(...)",                                       \
                                __func__);                                       \
    }


/**
* \brief Macro to Log Message with Variadic Arguments
* 
* However, if the device is in time-critical mode the logging is ignored.
* 
* \param[in] commonDev  pointer to adi_common_Device_t
* \param[in] logLevel   Log Level
* \param[in] message    Message to be Logged
* \param[in] ...        variable argument(s) passed to adi_common_Logwrite
*/
#define ADI_VARIABLE_LOG(commonDev, logLevel, message, ...)                      \
    if (!ADI_COMMON_DEVICE_STATE_IS_TC(*(commonDev)))                            \
    {                                                                            \
        adi_common_LogWrite((commonDev),                                         \
                            (adi_hal_LogLevel_e) (logLevel),                     \
                            message,                                             \
                            ##__VA_ARGS__);                                      \
    }

/**
* \brief Macro to log error structure
*
* This macro will call adi_common_LogWrite function with the required string for logging the error.
*
* \param[in] commonDev  pointer to adi_common_Device_t
* \param[in] err        pointer to the error structure
*/
#define  ADI_ERROR_LOG(commonDev, err)      adi_common_LogWrite(commonDev,                                                              \
                                                                ADI_HAL_LOG_ERR,                                                        \
                                                                "Device Information: Type: %" PRIX32 ", ID: %" PRIX32 ", Name: %s\n"    \
                                                                ADI_COMMON_LOG_ERR_INDENT1 "Recovery Action: 0x%" PRIX64 "\n"           \
                                                                ADI_COMMON_LOG_ERR_INDENT1 "Event Information:\n"                       \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Error Code: 0x%" PRIX64"\n"                 \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Error Source: 0x%" PRIX32 "\n"              \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Error Message: %s\n"                        \
                                                                ADI_COMMON_LOG_ERR_INDENT1 "File Information:\n"                        \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Line: %" PRIu32"\n"                         \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Function: %s\n"                             \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "File: %" PRIu32"\n"                         \
                                                                ADI_COMMON_LOG_ERR_INDENT1 "Variable Information:\n"                    \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Variable Name: %s\n"                        \
                                                                ADI_COMMON_LOG_ERR_INDENT2 "Variable Data: 0x%" PRIX64 "\n",            \
                                                                err.errDataInfo.errDeviceInfo.type,                                     \
                                                                err.errDataInfo.errDeviceInfo.id,                                       \
                                                                err.errDataInfo.errDeviceInfo.name,                                     \
                                                                err.stackTrace->action,                                                 \
                                                                err.stackTrace->errInfo.errCode,                                        \
                                                                err.stackTrace->errInfo.errSrc,                                         \
                                                                err.stackTrace->errInfo.errMsg,                                         \
                                                                err.stackTrace->fileInfo.line,                                          \
                                                                err.stackTrace->fileInfo.function,                                      \
                                                                err.stackTrace->fileInfo.file,                                          \
                                                                err.stackTrace->varInfo.varName,                                        \
                                                                err.stackTrace->varInfo.varData);
/**
* \brief Function to Set the Logging Level
* 
* Used to set the log level mask of what types of messages to log 
* 
* \param[in]    commonDev   Pointer to adi_common_Device_t
* \param[in]    logMask     Logging Mask
*
*/
ADI_API void adi_common_LogLevelSet(const adi_common_Device_t* const    commonDev,
                                    const uint32_t                      logMask);

/**
* \brief Function to Get the Logging Level
* 
* Used to read the log level mask of what types of messages are able to be logged.
*
* \param[in]    commonDev   Pointer to adi_common_Device_t
* \param[out]    logMask     Logging Mask
*
*/
ADI_API void adi_common_LogLevelGet(const adi_common_Device_t* const    commonDev,
                                    uint32_t* const                     logMask);

/**
* \brief    Function to Get Logging Status
* 
* \param[in]    commonDev   Pointer to adi_common_Device_t
* \param[out]   logStatus   Pointer to Logging Status Information
*
*/
ADI_API adi_common_ErrAction_e adi_common_LogStatusGet( const adi_common_Device_t* const    commonDev,
                                                        adi_hal_LogStatusGet_t* const       logStatus);

/**
* \brief    Function to Configure Logging to Console
* 
* \param[in]    commonDev       Pointer to adi_common_Device_t
* \param[out]   logConsoleFlag  Flag Status to be Set
*
*/
ADI_API void adi_common_LogConsoleSet(const adi_common_Device_t* const commonDev, adi_hal_LogConsole_e logConsoleFlag);

/**
* \brief Function to write to log with a selected comment
* 
* \param[in]    commonDev       Pointer to adi_common_Device_t
* \param[in]    logLevel        Logging Level
* \param[in]    comment         Message Pointer
* \param[in]    ...             variable(s) argument passed to adi_common_Logwrite
*
*/
ADI_API void adi_common_LogWrite(   const adi_common_Device_t* const    commonDev,
                                    const adi_hal_LogLevel_e            logLevel,
                                    const char*                         comment,
                                    ...);

/**
* \brief Function to open the log file associated to the log
* 
* Used to open the log file that is stored in the platform
* 
* \param[in]    commonDev   pointer to adi_common_Device_t
* \param[in]    fileName    file name to be open, if null default name in the devHalInfo structure will be used
*
*/
ADI_API void adi_common_LogFileOpen(    const adi_common_Device_t* const    commonDev,
                                        const char* const                   fileName);

/**
* \brief Function to close the log file associated to the log in the platform
*
* \param[in] commonDev   pointer to adi_common_Device_t
*
*/
ADI_API void adi_common_LogFileClose(const adi_common_Device_t* const commonDev);

/*
 * \brief   Service to Log an Error
 *
 * \param[in] commonDev         pointer to adi_common_Device_t
 * \param[in] actionRequired    RecoveryAction to be taken
 * \param[in] lineNum           line number of where the error was detected
 * \param[in] funcName          function name of the function where the error was detected
 * \param[in] fileName          file name of the file where the error was detected
 * \param[in] detErr            Detected Error Code
 * \param[in] errCodeMsg        Error Message
 * \param[in] varName           Variable name to be captured
 * \param[in] varData           Variable Data will be captured (Limitations Apply)
 * \param[in] errMsg            User Readable Error Message
 */
ADI_API adi_common_ErrAction_e adi_common_ErrLog(   const adi_common_Device_t* const    commonDev,
                                                    const uint32_t                      errSource,
                                                    const adi_common_ErrAction_e        action,
                                                    const int64_t                       errCode,
                                                    const uint32_t                      lineNum,
                                                    const char*                         funcName,
                                                    const uint32_t                      fileName,
                                                    const char*                         varName,
                                                    const int64_t                       varData,
                                                    const char*                         errMsg);

#endif /* _ADI_COMMON_LOG_H_ */
