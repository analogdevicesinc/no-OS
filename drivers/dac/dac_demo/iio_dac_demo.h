/***************************************************************************//**
 *   @file   iio_dac_demo.h
 *   @brief  Header file of DAC Demo iio.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_DEMO_DAC
#define IIO_DEMO_DAC

#include <stdlib.h>
#include "iio_types.h"
#include "dac_demo.h"

extern struct iio_device dac_demo_iio_descriptor;
extern struct iio_trigger dac_iio_sw_trig_desc;
extern struct iio_trigger dac_iio_timer_trig_desc;

#endif /* IIO_DEMO_DAC */
