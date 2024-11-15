/***************************************************************************//**
 *   @file   ad5758-sdz/src/app/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_DEVICE_ID           	XPAR_PS7_SPI_0_DEVICE_ID
#define AD5758_SPI_CS           	0
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET  			86U
#define GPIO_DAC_FAULT_N		GPIO_OFFSET + 2
#define GPIO_DAC_RESET_N		GPIO_OFFSET + 1
#define GPIO_DAC_LDAC_N			GPIO_OFFSET

#endif
