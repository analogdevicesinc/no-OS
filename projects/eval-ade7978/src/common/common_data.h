/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE7978 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "ade7978.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/* Hardware dependent definitions */

/* Current sesing using a shunt */
/* Value of shunt in mohms */
#define ADE7978_SHUNT_RES              	1

/* Assuming a voltage divider with Rlow 1k and Rup 990k */
#define ADE7978_UP_RES                	990000
#define ADE7978_DOWN_RES		1000
#define ADE7978_VOLTAGE_TR_FCN		((ADE7978_DOWN_RES + ADE7978_UP_RES) / ADE7978_DOWN_RES)

/**
 * @struct measurements
 * @brief measurements structure.
 */
struct measurements {
	/* I rms value */
	float				i_rms;
	/* V1 rms value */
	float				v1_rms;
	/* V2 rms value */
	float				v2_rms;
	/* Temperature °C value */
	float               		temperature_c;
	/* I ADC rms value */
	int32_t				i_rms_adc;
	/* V1 ADC rms value */
	int32_t				v1_rms_adc;
	/* V2 ADC rms value */
	int32_t				v2_rms_adc;
	/* Temperature ADC value */
	int32_t             		temperature;
};

/* Saves the current and voltage values of device 1 in rms_adc structure */
int rms_adc_values_read(struct ade7978_dev *dev, struct measurements *value,
			enum ade7978_phase phase);

#endif /* __COMMON_DATA_H__ */
