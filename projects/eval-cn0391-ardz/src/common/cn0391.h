/***************************************************************************//**
 *   @file   cn0391.h
 *   @brief  Header file of CN0391 driver.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef __CN0391_H__
#define __CN0391_H__

#include "ad7124.h"
#include "no_os_thermocouple.h"
#include "no_os_rtd.h"

#define CN0391_ADC_RESOLUTION		24
#define CN0391_ADC_HALF_RESOLUTION	(1 << (CN0391_ADC_RESOLUTION - 1))
#define CN0391_GAIN			32
#define CN0391_RTD_GAIN			1
#define CN0391_THERMO_RES		1600
#define CN0391_ADC_REF_VOLTAGE		2.5

#define CN0391_NUM_IIO_CHANNELS		4
#define CN0391_ADC_CHANNELS_PER_IIO_CH	3

#define CN0391_CH0_ID	0
#define CN0391_CH1_ID	1
#define CN0391_CH2_ID	2
#define CN0391_CH3_ID	3

/* ADC channel triplet for one thermocouple IIO channel */
struct cn0391_adc_ch_map {
	uint8_t tc;		/* thermocouple ADC channel */
	uint8_t r5;		/* reference resistor R5 ADC channel */
	uint8_t rtd;		/* CJC RTD ADC channel */
	uint8_t iout_ain;	/* AIN pin index for IOUT0 excitation (IOUT_CH0 field) */
};

struct cn0391_cache {
	double hot_junction_temp;
	double cold_junction_temp;
	double tc_voltage;
	double rtd_resistance;
};

struct cn0391_dev {
	struct ad7124_dev *ad7124_dev;
	/* Cached last measurement per IIO channel; -1 = no valid cache */
	int8_t cache_ch;
	struct cn0391_cache cache;
};

struct cn0391_init_param {
	struct ad7124_init_param *ad7124_init;
};

int cn0391_init(struct cn0391_dev **dev,
		struct cn0391_init_param *init_param);
int cn0391_remove(struct cn0391_dev *dev);
int cn0391_read_temperature(struct cn0391_dev *dev, uint8_t ch_idx,
			    double *hot_junction_temp,
			    double *cold_junction_temp,
			    double *thermocouple_voltage,
			    double *rtd_resistance);

#endif /* __CN0391_H__ */
