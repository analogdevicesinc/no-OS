/***************************************************************************//**
 *   @file   cn0391.h
 *   @brief  Header file of CN0391 board support.
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

/* AD7124 setup indices */
#define CN0391_SETUP_TC_IDX		0	/* Thermocouple: Gain=32, bipolar, internal ref */
#define CN0391_SETUP_RTD_IDX		1	/* R5 + CJC RTD: Gain=1, bipolar, internal ref */

/* AD7124-8 internal oscillator frequency at full power mode (Hz) */
#define CN0391_ADC_CLK_HZ		614400ULL
/* Filter FS register value: sets output data rate = fCLK / (32 × FS) */
#define CN0391_FILTER_FS		384U

/*
 * Digital filter type selection.
 * Override at build time with -DCN0391_FILTER_TYPE=<value>:
 *   0 — Sinc4: best noise rejection, 4-period settling (default)
 *   2 — Sinc3: 25% faster settling, slightly lower noise rejection
 *
 * AD7124_FILT_REG_FILTER(CN0391_FILTER_TYPE) encodes this into the
 * filter register bits [23:21].
 *
 * NOTE: in single conversion mode (used by cn0391_read_temperature) the
 * AD7124 always runs the full 4-stage CIC filter for settling regardless
 * of this setting. Sinc3 vs Sinc4 therefore has no effect on acquisition
 * speed here; it only affects noise shaping of the output word. The only
 * effective knob for reducing conversion time in this mode is CN0391_FILTER_FS.
 */
#ifndef CN0391_FILTER_TYPE
#define CN0391_FILTER_TYPE		0	/* Sinc4 */
#endif

#if (CN0391_FILTER_TYPE == 0)
#define CN0391_FILTER_ORDER		4U	/* Sinc4: 4-period settling */
#elif (CN0391_FILTER_TYPE == 2)
#define CN0391_FILTER_ORDER		3U	/* Sinc3: 3-period settling */
#else
#error "CN0391_FILTER_TYPE must be 0 (Sinc4) or 2 (Sinc3)"
#endif

/*
 * Filter register value combining type, FS, and no post-filter.
 * Used to initialise all 8 AD7124 filter setup registers in common_data.c.
 */
#define CN0391_FILTER_REG_VAL \
	(AD7124_FILT_REG_FILTER(CN0391_FILTER_TYPE) | AD7124_FILT_REG_FS(CN0391_FILTER_FS))

/*
 * Worst-case time for one single conversion (µs).
 * SincN requires (filter_order + 1) conversion periods before producing a
 * valid output after a channel switch:
 *   t_conv = (filter_order + 1) × FS × 32 / fCLK
 */
#define CN0391_CONV_TIME_US \
	(((CN0391_FILTER_ORDER + 1U) * CN0391_FILTER_FS * 32ULL * 1000000ULL) \
	 / CN0391_ADC_CLK_HZ)

/*
 * Minimum number of SPI STATUS register polls to cover one worst-case
 * conversion, with a 2× safety margin.
 * Each poll transfers 2 bytes (1 command + 1 STATUS data byte):
 *   poll_time_us = 2 × 8 / spi_hz × 1e6
 *   poll_cnt = conv_time_us × 2 / poll_time_us
 *            = conv_time_us × spi_hz / (8 × 1e6)
 */
#define CN0391_SPI_RDY_POLL_CNT(spi_hz) \
	((CN0391_CONV_TIME_US * 2ULL * (spi_hz)) / (16ULL * 1000000ULL))

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

/**
 * @brief Initialize CN0391 device: set up AD7124 and configure IOUT.
 * @param dev - Pointer to pointer to CN0391 device descriptor.
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int cn0391_init(struct cn0391_dev **dev,
		struct cn0391_init_param *init_param);

/**
 * @brief Remove CN0391 device and free resources.
 * @param dev - CN0391 device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int cn0391_remove(struct cn0391_dev *dev);

/**
 * @brief Perform a full temperature measurement cycle.
 *        Reads all 3 ADC channels (thermocouple, reference resistor, CJC RTD),
 *        computes cold junction temperature, cold junction compensation voltage,
 *        and hot junction temperature.
 * @param dev - CN0391 device descriptor.
 * @param hot_junction_temp - Pointer to store hot junction temperature (°C).
 * @param cold_junction_temp - Pointer to store cold junction temperature (°C).
 * @param thermocouple_voltage - Pointer to store thermocouple voltage (mV).
 * @param rtd_resistance - Pointer to store measured RTD resistance (Ohms).
 * @return 0 on success, negative error code otherwise.
 */
int cn0391_read_temperature(struct cn0391_dev *dev, uint8_t ch_idx,
			    double *hot_junction_temp,
			    double *cold_junction_temp,
			    double *thermocouple_voltage,
			    double *rtd_resistance);
				
#endif /* __CN0391_H__ */
