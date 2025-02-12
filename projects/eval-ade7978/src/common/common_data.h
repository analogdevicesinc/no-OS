/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE7978 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
