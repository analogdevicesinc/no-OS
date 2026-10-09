/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the Maxim ADP20086 examples.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "no_os_irq.h"

int main()
{
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};
	int ret;

	/*
	 * The maxim GPIO IRQ controller does not touch the NVIC, so the GPIO
	 * port interrupt has to be enabled there separately before the INTB
	 * callback can fire.
	 */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(nvic_desc, NVIC_GPIO_IRQ, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;

	return example_main();
}