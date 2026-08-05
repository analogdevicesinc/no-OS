/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines the common data used in the examples eval-adf4377 project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "parameters.h"

struct no_os_spi_init_param adf4377_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA
};

struct no_os_gpio_init_param gpio_ce_param = {
	.number = GPIO_CE,
	.port = GPIO_CE_PORT,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param gpio_enclk1_param = {
	.number = GPIO_ENCLK1,
	.port = GPIO_ENCLK1_PORT,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param gpio_enclk2_param = {
	.number = GPIO_ENCLK2,
	.port = GPIO_ENCLK2_PORT,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_uart_init_param adf4377_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct adf4377_init_param adf4377_ip = {
	.dev_id = ADF4377,
	.spi_init = &adf4377_spi_ip,
	.spi4wire = true,
	.gpio_ce_param = &gpio_ce_param,
	.gpio_enclk1_param = &gpio_enclk1_param,
	.gpio_enclk2_param = &gpio_enclk2_param,
	.clkin_freq = 125000000,
	.ref_doubler_en = 1,
	.f_clk = 10000000000,
	.ref_div_factor = 1,
	.muxout_select = ADF4377_MUXOUT_HIGH_Z,
	.cp_i = ADF4377_CP_10MA1,
};
