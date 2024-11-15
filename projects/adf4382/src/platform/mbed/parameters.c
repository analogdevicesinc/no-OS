/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by ADF4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "parameters.h"

struct mbed_uart_init_param adf4382_uart_extra_ip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN
};

extern struct mbed_spi_init_param adf4382_spi_extra = {
	.spi_miso_pin = SDP_SPI_MISO,
	.spi_mosi_pin = SDP_SPI_MOSI,
	.spi_clk_pin = SDP_SPI_SCK,
	.use_sw_csb = false
};
