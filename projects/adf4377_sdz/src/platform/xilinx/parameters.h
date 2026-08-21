/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include <xparameters.h>

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS				0
#define SPI_EXTRA 			&xil_spi_extra
#define GPIO_EXTRA			&xil_gpio_init_param

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID

#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR

#define UART_BAUDRATE	            	115200

#define GPIO_OFFSET			32 + 54
#define GPIO_MUXOUT                 	GPIO_OFFSET
#define GPIO_LKDET                  	GPIO_OFFSET + 1
#define GPIO_ENCLK2                 	GPIO_OFFSET + 2
#define GPIO_CE				GPIO_OFFSET + 3
#define GPIO_ENCLK1                 	GPIO_OFFSET + 4

/* Port is unused on the Xilinx platform (kept for common_data.c portability). */
#define GPIO_CE_PORT			0
#define GPIO_ENCLK1_PORT		0
#define GPIO_ENCLK2_PORT		0

extern struct xil_spi_init_param xil_spi_extra;
extern struct xil_gpio_init_param xil_gpio_init_param;

#endif /* _PARAMETERS_H_ */
