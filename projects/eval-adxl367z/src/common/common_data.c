/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adxl367z examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
const struct no_os_spi_init_param spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 100000,
	.mode = NO_OS_SPI_MODE_0,
	.chip_select = 0U,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = &spi_extra
};

struct adxl367_init_param init_param = {
	.spi_init = spi_ip,
	.comm_type = ADXL367_SPI_COMM
};
