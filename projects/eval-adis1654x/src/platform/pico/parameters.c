/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of pico platform data used by eval-adis1654x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

struct pico_spi_init_param adis1654x_spi_extra_ip  = {
	.spi_tx_pin = SPI0_TX_GP19,
	.spi_rx_pin = SPI0_RX_GP16,
	.spi_sck_pin = SPI0_SCK_GP18,
	.spi_cs_pin = SPI0_CS_GP17
};

struct pico_uart_init_param adis1654x_uart_extra_ip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN,
};
