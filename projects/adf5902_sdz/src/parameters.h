/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_ADF5902_CS				0

#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID

#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				    XPAR_XUARTPS_1_INTR

#define UART_BAUDRATE	            115200

#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#define GPIO_OFFSET					32 + 54
#define GPIO_CE						GPIO_OFFSET + 1
#define GPIO_TX_DATA				GPIO_OFFSET + 2

#endif /* _PARAMETERS_H_ */
