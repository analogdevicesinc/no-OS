/***************************************************************************//**
 *   @file   iio_pulsar_adc.h
 *   @brief  Header file for PULSAR_ADC IIO interface
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef _PULSAR_ADC_IIO_H_
#define _PULSAR_ADC_IIO_H_

#include <stdint.h>

#include "iio.h"
#include "iio_types.h"
#include "pulsar_adc.h"

/**
 * @struct pulsar_adc_iio_dev
 * @brief pulsar_adc IIO device structure
 */
struct pulsar_adc_iio_dev {
	/** pulsar_adc driver handler */
	struct pulsar_adc_dev *pulsar_adc_dev;
	/** Generic IIO device handler */
	struct iio_device *iio_dev;
	/* Voltage reference */
	uint32_t ref_voltage_mv;
	/* scan type */
	struct scan_type scan_type;
};

/**
 * @struct pulsar_adc_iio_init_param
 * @brief pulsar_adc IIO initialization structure
 */
struct pulsar_adc_iio_init_param {
	struct pulsar_adc_init_param *init_param;
	/* Voltage reference */
	uint32_t ref_voltage_mv;
};

/** Allocate memory for pulsar_adc IIO handler. */
int pulsar_adc_iio_init(struct pulsar_adc_iio_dev **dev,
			struct pulsar_adc_iio_init_param *iio_init_param);
/** Free memory allocated by iio_pulsar_adc_init(). */
int pulsar_adc_iio_remove(struct pulsar_adc_iio_dev *dev);
#endif /* __PULSAR_ADC_IIO_H__ */
