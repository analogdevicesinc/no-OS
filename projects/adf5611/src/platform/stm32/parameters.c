/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ADF5611 project.
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param adf5611_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param adf5611_spi_extra = {
	.chip_select_port = SPI_CS_PORT,
};

struct stm32_i2c_init_param adf5611_i2c_extra_param = {
	.i2c_timing = I2C_TIMING,
};
