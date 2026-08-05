/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl355-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

#ifdef GPIO_DRDY_PORT_NUM
	struct no_os_gpio_desc *adxl355_gpio_desc;

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adxl355_gpio_desc, &adxl355_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adxl355_gpio_desc);
	if (ret)
		return ret;
#endif
#ifdef NVIC_GPIO_IRQ
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};
	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		return ret;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		return ret;
#endif

	return example_main();
}

