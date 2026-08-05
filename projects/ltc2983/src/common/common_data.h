/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc2983 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ltc2983.h"
#ifdef IIO_SUPPORT
#include "iio_ltc2983.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_spi_init_param ltc2983_spi_ip;
extern const struct no_os_gpio_init_param ltc2983_gpio_rstn;
extern struct ltc2983_init_param ltc2983_ip;

#if defined(ADT7604_BASIC_EXAMPLE) || defined(ADT7604_IIO_EXAMPLE)

/** EVAL-ADT7604-AZ Board Sensors */
extern struct ltc2983_rsense
	ltc2983_rsense_2;      /* CH2:  100Ω rsense (R4) for copper traces */
extern struct ltc2983_rsense
	ltc2983_rsense_12;     /* CH12: 1kΩ  rsense (R6) for leak + PT100 */
extern struct ltc2983_rtd
	ltc2983_copper_4;         /* CH4:  sub-ohm copper trace */
extern struct ltc2983_rtd
	ltc2983_copper_6;         /* CH6:  sub-ohm copper trace */
extern struct ltc2983_rtd
	ltc2983_copper_8;         /* CH8:  sub-ohm copper trace */
extern struct ltc2983_custom_sensor
	ltc2983_leak_custom; /* shared leak detector table */
extern struct ltc2983_thermistor ltc2983_leak_14;   /* CH14: leak detector */
extern struct ltc2983_thermistor ltc2983_leak_16;   /* CH16: leak detector */
extern struct ltc2983_rtd ltc2983_rtd_18;           /* CH18: PT100 2-wire */

#else

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

#endif /* ADT7604_BASIC_EXAMPLE || ADT7604_IIO_EXAMPLE */

#endif /* __COMMON_DATA_H__ */
