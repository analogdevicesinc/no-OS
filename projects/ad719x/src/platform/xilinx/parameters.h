/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to XILINX platform used by eval-adxl367z
 *           project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define BUF_LENGTH 				32U
#define SPI_DEVICE_ID				XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define SPI_OPS 				&xil_spi_ops
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_OFFSET				54U

#define RDY_PIN					GPIO_OFFSET + 32U
#define SYNC_PIN				GPIO_OFFSET + 33U

extern struct xil_spi_init_param spi_extra;
extern struct xil_gpio_init_param gpio_extra;

#endif //__PARAMETERS_H__
