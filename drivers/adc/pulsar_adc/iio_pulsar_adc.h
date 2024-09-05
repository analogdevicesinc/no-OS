/***************************************************************************//**
 *   @file   iio_pulsar_adc.h
 *   @brief  Header file for PULSAR_ADC IIO interface
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _PULSAR_ADC_IIO_H_
#define _PULSAR_ADC_IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

#include "iio.h"
#include "iio_types.h"
#include "pulsar_adc.h"

/******************************************************************************/
/********************** Public/Extern Declarations ****************************/
/******************************************************************************/
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
