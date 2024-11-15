/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adxl313z examples.
 *   @author GMois (george.mois@analog.com)
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
struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &xuip,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param sip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_HZ,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.extra = &xsip,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
};

struct adxl313_init_param adxl313_user_init = {
	.dev_type = ID_ADXL313,
	.comm_type = ADXL313_SPI_COMM,
};
