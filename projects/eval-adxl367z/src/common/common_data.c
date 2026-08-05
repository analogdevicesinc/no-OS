/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adxl367z examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_uart.h"

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

#ifdef IIO_SUPPORT
struct no_os_uart_init_param adxl367_uart_ip = {
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
#endif
