/***************************************************************************//**
 *   @file   ad4110/src/app/parameters.h
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
#define BUF_LENGTH 	32U
#define SPI_DEVICE_ID 	0U
#define GPIO_OFFSET  	54U

#define NREADY_PIN   	GPIO_OFFSET + 32U
#define GPIO_IRQ_ID 	52U

#endif
