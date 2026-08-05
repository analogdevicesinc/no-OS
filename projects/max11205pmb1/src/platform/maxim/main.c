/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max11205pmb1 project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	max11205_ip.spi_init = max11205_spi_ip;

	struct no_os_irq_ctrl_desc *max11205_nvic_desc;
	struct no_os_irq_init_param max11205_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize NVIC IRQ controller in order to be able to enable GPIO IRQ interrupt */
	ret = no_os_irq_ctrl_init(&max11205_nvic_desc, &max11205_nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(max11205_nvic_desc, NVIC_GPIO_IRQ, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(max11205_nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;

	return example_main();
}
