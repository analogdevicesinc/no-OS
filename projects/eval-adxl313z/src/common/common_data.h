/*******************************************************************************
 *   @file   eval-adxl313z/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxl313z examples.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "adxl313.h"
#ifdef IIO_SUPPORT
#include "iio_adxl313.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param uip;

extern struct no_os_spi_init_param sip;
extern struct adxl313_init_param adxl313_user_init;

#define SPI_MAX_HZ			4000000

#endif /* __COMMON_DATA_H__ */
