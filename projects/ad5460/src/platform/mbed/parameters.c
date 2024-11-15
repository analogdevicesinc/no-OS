/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by ad5460 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct mbed_uart_init_param ad5460_uart_extra_ip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN
};

struct mbed_i2c_init_param ad5460_i2c_extra = {
	.i2c_sda_pin = I2C_SDA,
	.i2c_scl_pin = I2C_SCL
};

extern struct mbed_spi_init_param ad5460_spi_extra = {
	.spi_miso_pin = ARDUINO_UNO_D12,
	.spi_mosi_pin = ARDUINO_UNO_D11,
	.spi_clk_pin = ARDUINO_UNO_D13,
	.use_sw_csb = false
};


