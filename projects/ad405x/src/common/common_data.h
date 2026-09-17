/***************************************************************************//**
 *   @file   ad405x/src/common/common_data.h
 *   @brief  Defines common data to be used by ad405x examples.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad405x.h"
#ifdef IIO_SUPPORT
#include "iio_ad405x.h"
#endif

#ifdef AD405X_SPI
#define AD405X_COMM AD405X_SPI_COMM
#define AD405X_GPIO_CNV &gpio_cnv_param
#endif
#ifdef AD405X_I3C
#define AD405X_COMM AD405X_I3C_COMM
#define AD405X_GPIO_CNV NULL
#endif

extern struct no_os_uart_init_param ad405x_uart_ip;
#ifdef AD405X_I3C
extern struct no_os_i3c_bus_init_param i3c1_ip;
extern struct no_os_i3c_init_param ad405x_i3c_ip;
#endif
#ifdef AD405X_SPI
extern struct no_os_spi_init_param ad405x_spi_ip;
#endif
extern struct ad405x_init_param ad405x_ip;
extern struct stm32_gpio_init_param gpio_init;
#ifdef AD405X_SPI
extern struct no_os_gpio_init_param gpio_cnv_param;
#endif
extern struct no_os_gpio_init_param gpio_gpio0_param;
extern struct no_os_gpio_init_param gpio_gpio1_param;

#endif /* __COMMON_DATA_H__ */
