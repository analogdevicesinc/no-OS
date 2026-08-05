/***************************************************************************//**
 *   @file   iio_adxl355_trig.c
 *   @brief  Implementation of adxl355 iio trigger.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_trigger.h"
#include "iio.h"


struct iio_trigger adxl355_iio_trig_desc = {
	.is_synchronous = false,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable
};
