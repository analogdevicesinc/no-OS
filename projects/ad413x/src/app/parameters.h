/***************************************************************************//**
 *   @file   ad413x/src/app/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <xparameters.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID 	0U
#define GPIO_OFFSET  	54U

#define RDY_PIN   	GPIO_OFFSET + 32U
#define GPIO_IRQ_ID 	52U

#define UART_DEVICE_ID	XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE	115200

#endif
