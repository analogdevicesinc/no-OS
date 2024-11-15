/***************************************************************************//**
 *   @file   adi_platform.h
 *   @brief  adrv9002 HAL types.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __ADI_PLATFORM_TYPES_H__
#define __ADI_PLATFORM_TYPES_H__

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

#endif /* __ADI_PLATFORM_TYPES_H__*/


