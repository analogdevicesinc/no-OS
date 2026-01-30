/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/


/**
* \file common/adi_logging.h
*/

#ifndef _ADS10_LOGGING_H_
#define _ADS10_LOGGING_H_

#include "adi_platform.h"

/**
* \brief Opens a logFile. If the file is already open it will be closed and reopened.
*
* This function opens the file for writing and saves the resulting file 
* descriptor to the devHalCfg structure.
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param filename The user provided name of the file to open.
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogFileOpen(void* const devHalCfg, const char* const filename);

/**
* \brief Flushes the logFile buffer to the currently open log file.
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogFileFlush(void* const devHalCfg);

/**
* \brief Service to Close a Log File
*
* \param devHalCfg Pointer to device instance specific platform settings
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogFileClose(void* const devHalCfg);

/**
* \brief Sets the log level, allowing the end user to select the granularity of
*        what events get logged.
*
* \param devHalCfg Pointer to device instance specific platform settings
* \param logMask A mask of valid log levels to allow to be written to the log file.
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogLevelSet(void* const devHalCfg, const uint32_t logMask);

/**
 * \brief Gets the currently set log level: the mask of different types of log
 *         events that are currently enabled to be logged.
 *
 * \param devHalCfg Pointer to device instance specific platform settings
 * \param logMask Returns the current log level mask.
 *
 * \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
 *
 */
ADI_API adi_hal_Err_e adi_LogLevelGet(void* const devHalCfg, uint32_t* const logMask);

/**
* \brief Service to get Logging Status
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logStatus  Pointer to Structure that will be populated with Log Status
*
* \note Error Count & Flags are reset after each Status Call
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*/
ADI_API adi_hal_Err_e adi_LogStatusGet(void* const devHalCfg, adi_hal_LogStatusGet_t* const logStatus);

/**
 * \brief Service to Set to Logging to Console Flag
 *
 * \param devHalCfg         Pointer to device instance specific platform settings
 * \param logConsoleFlag    Log to Console Flag Setting
 *
 * \retval adi_hal_LogErr_e - ADI_LOG_ERR_OK if successful
 *
 */
ADI_API adi_hal_Err_e adi_LogConsoleSet(void* const devHalCfg, const adi_hal_LogConsole_e logConsoleFlag);

/**
* \brief Writes a message to the currently open logFile specified in the 
*        adi_hal_LogCfg_t of the devHalCfg structure passed
* 
* Uses the vfprintf functionality to allow the user to supply the format and
* the number of arguments that will be logged.
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logLevel   the log level to be written into
* \param comment    the string to include in the line added to the log.
* \param argp       variable argument list to be printed
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogWrite( void* const                 devHalCfg,
                                    const adi_hal_LogLevel_e    logLevel,
                                    const uint8_t               indent,
                                    const char* const           comment,
                                    va_list                     argp);

/**
* \brief Writes a message to the currently open logFile specified in the 
*        adi_hal_LogCfg_t of the devHalCfg structure passed
* 
*   Uses the vfprintf functionality to allow the user to supply the format and
*   the number of arguments that will be logged.
*
* \param devHalCfg  Pointer to device instance specific platform settings
* \param logLevel   log level to be written into
* \param comment    the string to include in the line added to the log.
* \param ...        variable arguments to be printed
*
* \retval adi_hal_Err_e - ADI_HAL_ERR_OK if successful
*
*/
ADI_API adi_hal_Err_e adi_LogWrite1(void* const                 devHalCfg,
                                    const adi_hal_LogLevel_e    logLevel,
                                    const char* const           comment,
                                    ...);

#endif /* _adi_LOGGING_H_ */
