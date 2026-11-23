/**
* \file
* \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_log.c
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_LOG_H_
#define _ADI_COMMON_LOG_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_common_types.h"
#include "adi_common_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_TRACE
#define ADI_LOG_TRACE(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define ADI_LOG_TRACE(commonDev, fmt, ...) {}
#endif


#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_DEBUG
#define ADI_LOG_DEBUG(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_DEBUG, fmt, ##__VA_ARGS__)

/**
* \brief Macro to log API function entry
*
* This macro will call adi_common_LogWrite function with the __FUNCTION__ preprocessor
* It will report any error discovered.
*
* \param commonDev pointer to adi_common_Device_t
*/
#define ADI_FUNCTION_ENTRY_LOG(commonDev) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_DEBUG, "%s(...)", __FUNCTION__)

/**
* \brief Macro to log API function entry
*
* This macro will call adi_common_LogWrite function with the __FUNCTION__ preprocessor
*
* \param commonDev pointer to adi_common_Device_t
* \param message const char pointer that represents the message to be logged
* \param ... variable argument passed to adi_common_Logwrite
*/
#define ADI_FUNCTION_ENTRY_VARIABLE_LOG(commonDev, message, ...) \
    adi_common_LogWrite(commonDev, ADI_LOGLEVEL_DEBUG, message, __FUNCTION__, ##__VA_ARGS__)
#else
#define ADI_LOG_DEBUG(commonDev, fmt, ...) {}
#define ADI_FUNCTION_ENTRY_LOG(commonDev) {}
#define ADI_FUNCTION_ENTRY_VARIABLE_LOG(commonDev, message, ...) {}
#endif
    
#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_INFO
#define ADI_LOG_INFO(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define ADI_LOG_INFO(commonDev, fmt, ...) {}
#endif
    
#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_WARN
#define ADI_LOG_WARN(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define ADI_LOG_WARN(commonDev, fmt, ...) {}
#endif
    
#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_ERROR
#define ADI_LOG_ERROR(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define ADI_LOG_ERROR(commonDev, fmt, ...) {}
#endif
    
#if ADI_COMPILED_LOGLEVEL <= ADI_LOGLEVEL_FATAL
#define ADI_LOG_FATAL(commonDev, fmt, ...) \
    adi_common_LogWrite((adi_common_Device_t *)(commonDev), ADI_LOGLEVEL_FATAL, fmt, ##__VA_ARGS__)
#else
#define ADI_LOG_FATAL(commonDev, fmt, ...) {}
#endif

/**
* \brief Macro to log error structure
*
* This macro will call adi_common_LogWrite function with the required string for logging the error.
*
* \param commonDev pointer to adi_common_Device_t
* \param err pointer to the error structure
*/
#define  ADI_ERROR_LOG(commonDev, err) \
{\
    ADI_LOG_ERROR(commonDev, \
                  "Error number % d (0x%08x), Recovery action % d.In file % s, in function % s, in line % d, variable name % s.Error message % s.\n", \
                  err.errCode,\
                  err.errCode,\
                  err.newAction,\
                  err.errFile,\
                  err.errFunc,\
                  err.errLine,\
                  err.varName,\
                  err.errormessage); \
}

#ifndef CLIENT_IGNORE

/**
* \brief Function to write to log with a selected comment
*
* \param commonDev pointer to adi_common_Device_t
* \param logLevel   Level at which to log the message
* \param comment const char pointer that represents the message to be logged
* \param ... variable argument passed to adi_common_Logwrite
*/
void adi_common_LogWrite(adi_common_Device_t *commonDev, uint32_t logLevel, const char *comment, ...);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ADI_COMMON_LOG_H_ */