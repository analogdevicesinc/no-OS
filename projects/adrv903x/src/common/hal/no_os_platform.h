/***************************************************************************//**
 *   @file   no_os_platform.h
 *   @brief  ADRV903X Hardware Abstraction Layer for no-OS.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef NO_OS_PLATFORM_H_
#define NO_OS_PLATFORM_H_

#include "no_os_gpio.h"
#include "no_os_spi.h"

/**
 * @struct adrv903x_hal_cfg
 * @brief  HAL configuration for no-OS ADRV903X platform.
 */
struct adrv903x_hal_cfg {
	struct no_os_spi_desc	*spi;
	struct no_os_gpio_desc	*gpio_reset_n;
	int32_t			logLevel;	/*!< valid 0 - 0xFF */
};

/**
 * @brief File I/O abstraction for profile/firmware loading.
 *
 * Required by no_os_platform.c for HAL file I/O stubs. Will be populated
 * with embedded binary data in a later step when firmware is embedded.
 */
typedef struct no_os_hal_fileio {
	char *data;
	char *ptr, *start, *end;
} CUSTOM_FILE;

#endif /* NO_OS_PLATFORM_H_ */
