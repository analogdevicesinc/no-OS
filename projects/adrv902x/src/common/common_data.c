/***************************************************************************//**
 *   @file   adrv902x/src/common/common_data.c
 *   @brief  Defines common data to be used by adrv902x examples.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "parameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Initialize the SPI structure */
const struct no_os_spi_init_param ad9528_spi_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 10000000u,
	.chip_select = AD9528_CS,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &xil_spi_ops,
	.extra = &spi_extra
};

/* Initialize the clkchip GPIO structure */
const struct no_os_gpio_init_param clkchip_gpio_init_param = {
	.number = AD9528_RESET_B,
	.platform_ops = &xil_gpio_ops,
	.extra = &xil_gpio_param
};

