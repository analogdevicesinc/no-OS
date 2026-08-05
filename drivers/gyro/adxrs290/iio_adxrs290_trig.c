/***************************************************************************//**
 *   @file   iio_adxrs290_trig.c
 *   @brief  Implementation of adxrs290 iio trigger.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio.h"
#include "iio_trigger.h"

struct iio_trigger adxrs290_iio_trig_desc = {
	.is_synchronous = true,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable
};
