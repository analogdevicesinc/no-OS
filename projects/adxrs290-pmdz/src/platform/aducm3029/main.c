/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-adxrs290-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#include "iio_timer_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	adxrs290_ip.spi_init = adxrs290_spi_ip;

	ret = platform_init();
	if (ret)
		goto error;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	ret = iio_trigger_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
	ret = iio_timer_trigger_example_main();
	if (ret)
		goto error;
#endif

#if (IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE + IIO_TIMER_TRIGGER_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return 0;
}

