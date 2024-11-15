/***************************************************************************//**
 *   @file   iio_adis_trig.c
 *   @brief  Implementation of adis iio trigger.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_trigger.h"
#include "iio.h"


/******************************************************************************/
/************************* Variable Definitions *******************************/
/******************************************************************************/

struct iio_trigger adis_iio_trig_desc = {
	.is_synchronous = true,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable
};
