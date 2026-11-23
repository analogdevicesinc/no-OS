/**
 * \file
 * \brief Hardware abstraction layer (HAL) for interacting with the FPGA9001
 *
 * Copyright 2020 Analog Devices Inc.
 * Released under the FPGA9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */
#ifndef _ADI_COMMON_HAL_H_
#define _ADI_COMMON_HAL_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdio.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

/**
 * \addtogroup common_hal "ADI Common HAL"
 * The Common HAL consists of function pointers which must be implemented by the user, such that the device API(s) can
 * call said functions to interact with the chip without having to worry about hardware details.
 * 
 * @{
 */

/**
 * \brief Perform a blocking wait for the specified length of time
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] time_us       The length of time to wait, denoted in microseconds (us)
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by API functions.
 */
extern int32_t(*adi_common_hal_Wait_us)(void *devHalCfg, uint32_t time_us);

/**
 * \brief Write a message to the log with the specified level
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] formatStr     Log message string optionally containing format specifiers which will be replaced by the
 * values specified in the additional arguments list argp
 * \param[in] argp          Variable argument list containing values with which to replace format specifiers in formatStr
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by API functions.
 */
extern int32_t(*adi_common_hal_LogWrite)(void *devHalCfg, uint32_t logLevel, const char *formatStr, va_list argp);

/** \addtogroup adi_hal_log_levels "Logging Levels"
 * Log levels increase in severity numerically. When a given level is used, events logged with a log level at least as
 * severe (greater than or equal to) as the set level should be published. Less severe events can be ignored.
 * @{
 */

#ifndef ADI_COMPILED_LOGLEVEL
/**
 * Log level events to be compiled into the device driver. Less severe logging calls will not be included, reducing
 * code size
 */
#define ADI_COMPILED_LOGLEVEL ADI_LOGLEVEL_WARN
#endif // !ADI_COMPILED_LOGLEVEL


/**
 * Log everything in exhaustive detail. Used only for development
 */
#define ADI_LOGLEVEL_TRACE (0)

/**
 * Log diagnostic information
 */
#define ADI_LOGLEVEL_DEBUG (1)

/**
 * Log state changes in the application
 */
#define ADI_LOGLEVEL_INFO (2)

/**
 * Log bad, but recoverable events
 */
#define ADI_LOGLEVEL_WARN (3)

/**
 * Log events that cannot be recovered from
 */
#define ADI_LOGLEVEL_ERROR (4)

/**
 * Log events that are likely to be fatal
 */
#define ADI_LOGLEVEL_FATAL (5)

/**
 * Disable all logging
 */
#define ADI_LOGLEVEL_NONE (255)

/** @} */

/** @} */

#endif

#ifdef __cplusplus
}
#endif

#endif /* _ADI_COMMON_HAL_H_ */
