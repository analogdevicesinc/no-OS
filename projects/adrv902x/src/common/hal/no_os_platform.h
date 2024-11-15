/***************************************************************************//**
 *   @file   no_os_platform.h
 *   @brief  adrv902x Hardware Abstraction Layer for no-OS.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef NO_OS_PLATFORM_H_
#define NO_OS_PLATFORM_H_

#define CONFIG_CF_AXI_ADC

struct adrv9025_hal_cfg {
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *gpio_reset_n;
	int32_t logLevel;         /*!< valid 0 - 0xFF */
};

#endif
