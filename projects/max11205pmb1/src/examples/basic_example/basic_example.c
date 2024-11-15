/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of basic example for max11205pmb1 project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example.h"
#include "max11205.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main()
{
	int ret;
	struct max11205_dev *max11205_desc;
	struct no_os_irq_ctrl_desc *max11205_gpio_irq_desc;
	int16_t adc_data_raw;
	int32_t adc_data_mv;
	bool new_data_avail;

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&max11205_gpio_irq_desc, &max11205_gpio_irq_ip);
	if (ret)
		return ret;

	/* Initialize device */
	max11205_ip.irq_ctrl = max11205_gpio_irq_desc;
	ret = max11205_init(&max11205_desc, max11205_ip);
	if (ret)
		return ret;

	/* Continuously read data */
	while (1) {
		ret = max11205_get_data_raw(max11205_desc, &new_data_avail, &adc_data_raw);
		if (ret)
			return ret;
		/* Print data only if new data is available */
		if (new_data_avail) {
			pr_info("ADC raw data %d:\n", adc_data_raw);

			ret  = max11205_get_data_mv(max11205_desc, adc_data_raw, &adc_data_mv);
			if (ret)
				return ret;

			pr_info("ADC converted data %d [mV]:\n", adc_data_mv);
		}
	}

	return 0;
}
