/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to XILINX platform used by eval-adxl367z
 *           project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "xilinx_spi.h"
#include <xparameters.h>

#define SPI_DEVICE_ID 	0U
#define SPI_OPS 	&xil_spi_ops

#ifdef IIO_SUPPORT
#include "xilinx_uart.h"

#define UART_IRQ_ID     XPAR_XUARTPS_1_INTR
#define UART_DEVICE_ID      XPAR_XUARTPS_0_DEVICE_ID
#define UART_BAUDRATE  115200
#define UART_EXTRA      &uart_extra_ip
#define UART_OPS        &xil_uart_ops

extern struct xil_uart_init_param uart_extra_ip;
#endif

extern struct xil_spi_init_param spi_extra;

#endif
