/***************************************************************************//**
 *   @file   adi_platform.h
 *   @brief  adrv9002 HAL functions interface.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __ADI_PLATFORM_H__
#define __ADI_PLATFORM_H__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

/**
 * BBIC Logging functions
 */
extern int32_t(*adi_hal_LogWrite)(void *devHalCfg, uint32_t logLevel,
				  const char *comment, va_list argp);

/**
 * BBIC Timer functions
 */
extern int32_t(*adi_hal_Wait_us)(void *devHalCfg, uint32_t time_us);

#endif /* CLIENT_IGNORE */

#ifdef __cplusplus
}
#endif
#endif /* __ADI_PLATFORM_H__ */


