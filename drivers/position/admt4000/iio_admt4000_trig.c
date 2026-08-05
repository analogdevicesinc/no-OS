/***************************************************************************//**
 *   @file   iio_admt4000_trig.c
 *   @brief  Implementation of admt4000 iio trigger.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include "iio_trigger.h"
#include "iio.h"


struct iio_trigger admt4000_iio_trig_desc = {
	.is_synchronous = false,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable
};
