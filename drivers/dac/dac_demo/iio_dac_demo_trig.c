/***************************************************************************//**
 *   @file   iio_dac_demo_trig.c
 *   @brief  Implementation of iio_dac_demo_trig.c
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "iio.h"
#include "iio_trigger.h"
#include "iio_dac_demo.h"

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct iio_attribute trig_attr[] = {
	{
		.name = "trigger_now",
		.store = iio_sw_trig_handler
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_trigger dac_iio_sw_trig_desc = {
	.is_synchronous = true,
	.attributes = trig_attr,
};

#ifndef LINUX_PLATFORM
struct iio_trigger dac_iio_timer_trig_desc = {
	.is_synchronous = true,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable,
};
#endif
