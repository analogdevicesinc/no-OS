/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxrs290-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
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

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	/* To be moved in the example when all platforms support GPIO IRQ controller */
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto error;

	ret = iio_trigger_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#error IIO_TIMER_TRIGGER_EXAMPLE is not supported on maxim platform.
#endif

#if (IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return 0;
}

