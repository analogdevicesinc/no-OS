/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by adt7420-pmdz project.
 *   @author CMinajigi (chandrakant.minajigi@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
struct mbed_uart_init_param xuip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN
};

struct mbed_i2c_init_param adt7420_i2c_extra = {
	.i2c_sda_pin = I2C_SDA,
	.i2c_scl_pin = I2C_SCL
};
