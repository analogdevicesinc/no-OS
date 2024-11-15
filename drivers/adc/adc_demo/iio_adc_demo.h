/***************************************************************************//**
 *   @file   iio_adc_demo.h
 *   @brief  Header file of ADC Demo iio.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_DEMO_ADC
#define IIO_DEMO_ADC

#include <stdlib.h>
#include "iio_types.h"
#include "adc_demo.h"

extern struct iio_device adc_demo_iio_descriptor;
extern struct iio_trigger adc_iio_sw_trig_desc;
extern struct iio_trigger adc_iio_timer_trig_desc;

#endif /* IIO_DEMO_ADC */
