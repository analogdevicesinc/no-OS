/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_common_log.h
* \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_log.c
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_COMMON_LOG_H_
#define _ADI_COMMON_LOG_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_common_log_types.h"
#include "adi_common_macros.h"
#include "adi_common_error.h"
#include "adi_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* *******************************
* ADI devices logging macros
* *******************************
*/
#if ADI_COMMON_VERBOSE > 0
/**
* \brief Macro to log API function entry
*
* This macro will call adrv9025_LogWrite function with the __FUNCTION__ preprocessor
* It will report any error discovered.
*
* \param commonDev pointer to adi_common_Device_t
* \param logLevel of type adi_common_LogLevel_e
*/
#define ADI_FUNCTION_ENTRY_LOG(commonDev, logLevel) \
    adrv9025_LogWrite(commonDev, logLevel, "%s(...)", __FUNCTION__)

/**
* \brief Macro to log API function entry
*
* This macro will call adrv9025_LogWrite function with the __FUNCTION__ preprocessor
*
* \param commonDev pointer to adi_common_Device_t
* \param logLevel of type adi_common_LogLevel_e
* \param message const char pointer that represents the message to be logged
* \param ... variable argument passed to adi_common_Logwrite
*/
#define ADI_FUNCTION_ENTRY_VARIABLE_LOG(commonDev, logLevel, message, ...) \
    adrv9025_LogWrite(commonDev, logLevel, message, __FUNCTION__, ##__VA_ARGS__)

/**
* \brief Macro to log API function entry
*
* This macro will call adrv9025_LogWrite function with the __FUNCTION__ preprocessor
*
* \param commonDev pointer to adi_common_Device_t
* \param message const char pointer that represents the message to be logged
*/
#define ADI_MESSAGE_LOG(commonDev, message) \
    adrv9025_LogWrite(commonDev, ADI_COMMON_LOG_MSG, message)

#else /*  ADI_COMMON_VERBOSE < 0 or not defined */
#define ADI_FUNCTION_ENTRY_LOG(commonDev, logLevel) \
    adrv9025_LogWrite(commonDev, logLevel, "%s(...)", __FUNCTION__)
#define ADI_FUNCTION_ENTRY_VARIABLE_LOG(commonDev, logLevel, message, ...) \
    adrv9025_LogWrite(commonDev, logLevel, message, __FUNCTION__, ##__VA_ARGS__)
#define ADI_MESSAGE_LOG(commonDev, message) \
    adrv9025_LogWrite(commonDev, ADI_COMMON_LOG_MSG, message)
#endif

/**
* \brief Macro to log error structure
*
* This macro will call adrv9025_LogWrite function with the required string for logging the error.
*
* \param commonDev pointer to adi_common_Device_t
* \param err pointer to the error structure
*/
#define  ADI_ERROR_LOG(commonDev, err) \
{\
    adrv9025_LogWrite(commonDev,\
                        ADI_COMMON_LOG_ERR,\
                        "Error number % d (0x%08x), Recovery action % d.In file % s, in function % s, in line % d, variable name % s.Error message % s.\n",\
                        err.errCode,\
		                err.errCode,\
                        err.newAction,\
                        err.errFile,\
                        err.errFunc,\
                        err.errLine,\
                        err.varName,\
                        err.errormessage); \
}

/*
* *******************************
* ADI devices common logging functions
* *******************************
*/

/**
* \brief Function to set the log level
*
* Used to set the log level mask of what types of messages to log
*
* \param commonDev pointer to adi_common_Device_t
* \param halLogLevel of type int32_t that follows adi_common_LogLevel_e
*                    sets the application log level to which all log statement levels will be commpared to.
*/
void adi_common_LogLevelSet(adi_common_Device_t* commonDev,
                            int32_t              halLogLevel);

/**
* \brief Function to get the log level
*
* Used to read the log level mask of what types of messages are able to be logged.
*
* \param commonDev pointer to adi_common_Device_t
* \param halLogLevel Pointer of type int32_t that follows adi_common_LogLevel_e
*
*/
void adi_common_LogLevelGet(adi_common_Device_t* commonDev,
                            int32_t*             halLogLevel);

/**
* \brief Function to write to log with a selected comment
*
* \param commonDev pointer to adi_common_Device_t
* \param logLevel of type adi_common_LogLevel_e
* \param comment const char pointer that represents the message to be logged
* \param ... variable argument passed to adi_common_Logwrite
*/
void adrv9025_LogWrite(adi_common_Device_t*  commonDev,
                         adi_common_LogLevel_e logLevel,
                         const char*           comment,
                         ...);

/**
* \brief Function to open the log file associated to the log
*
* Used to open the log file that is stored in the platform
*
* \param commonDev pointer to adi_common_Device_t
* \param fileName file name to be open, if null default name in the devHalInfo structure will be used
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_LogFileOpen(adi_common_Device_t* commonDev,
                               char*                fileName);

/**
* \brief Function to close the log file associated to the log
*
* Used to close the log file that is stored in the platform
*
* \param commonDev pointer to adi_common_Device_t
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_common_LogFileClose(adi_common_Device_t* commonDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ADI_COMMON_LOG_H_ */
