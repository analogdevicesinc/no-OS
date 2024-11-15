/***************************************************************************//**
 *   @file   no_os_platform.h
 *   @brief  adrv9002 Hardware Abstraction Layer for no-OS.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef NO_OS_PLATFORM_H_
#define NO_OS_PLATFORM_H_

struct adrv9002_hal_cfg {
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *gpio_reset_n;
	struct no_os_gpio_desc *gpio_ssi_sync;
};

#endif
