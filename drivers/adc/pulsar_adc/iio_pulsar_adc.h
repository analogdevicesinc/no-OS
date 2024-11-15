/***************************************************************************//**
 *   @file   iio_pulsar_adc.h
 *   @brief  Header file for PULSAR_ADC IIO interface
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
