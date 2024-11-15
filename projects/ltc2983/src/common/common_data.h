/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc2983 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "ltc2983.h"
#ifdef IIO_SUPPORT
#include "iio_ltc2983.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_spi_init_param ltc2983_spi_ip;
extern const struct no_os_gpio_init_param ltc2983_gpio_rstn;
extern struct ltc2983_init_param ltc2983_ip;

/** DC2214A Sensors */
extern struct ltc2983_rsense ltc2983_rsense_2;
extern struct ltc2983_thermistor ltc2983_thermistor_8;
extern struct ltc2983_thermistor ltc2983_thermistor_10;

/** DC2213A Sensors */
extern struct ltc2983_rsense ltc2983_rsense_3;
extern struct ltc2983_rtd ltc2983_rtd_6;
extern struct ltc2983_rtd ltc2983_rtd_11;

/** custom data */
extern struct ltc2983_custom_sensor ltc2983_thermistor_12_custom;
extern struct ltc2983_custom_sensor ltc2983_thermocouple_20_custom;

#endif /* __COMMON_DATA_H__ */
