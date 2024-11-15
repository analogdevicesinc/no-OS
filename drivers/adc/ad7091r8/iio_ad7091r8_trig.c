/***************************************************************************//**
 *   @file   iio_ad7091r8_trig.c
 *   @brief  Implementation of iio_ad7091r8_trig.c
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "iio_ad7091r8.h"

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
#ifndef LINUX_PLATFORM
struct iio_trigger ad7091r8_iio_timer_trig_desc = {
	.is_synchronous = true,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable,
};
#endif
