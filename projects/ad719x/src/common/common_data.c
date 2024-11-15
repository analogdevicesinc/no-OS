/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad719x examples.
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
	.mode = NO_OS_SPI_MODE_3,
	.chip_select = 0U,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = &spi_extra
};

const struct no_os_gpio_init_param miso_ip = {
	.number = RDY_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_extra
};

const struct no_os_gpio_init_param sync_ip = {
	.number = SYNC_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_extra
};

struct ad719x_init_param ad7190_dev_ip = {
	.spi_init = &spi_ip,
	.gpio_miso = &miso_ip,
	.sync_pin = NULL,
	.current_polarity = 1,
	.current_gain = AD719X_ADC_GAIN_64,
	.operating_mode = AD719X_MODE_SINGLE,
	.data_rate_code = 480U,
	.clock_source = AD719X_INT_CLK_4_92_MHZ,
	.input_mode = 0,
	.buffer = 0,
	.bpdsw_mode = 0,
	.chip_id = AD7190
};
