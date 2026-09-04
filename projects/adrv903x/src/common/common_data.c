/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADRV903X examples.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

/* SPI init params for AD9528 clock synthesizer */
const struct no_os_spi_init_param ad9528_spi_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = AD9528_SPI_SPEED_HZ,
	.chip_select = AD9528_CS,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &xil_spi_ops,
	.extra = &spi_extra
};

/* GPIO init params for clock chip (AD9528) reset */
struct no_os_gpio_init_param clkchip_gpio_init_param = {
	.number = AD9528_RESET_B,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};
