/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adis1657x project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;

	adis1657x_ip.spi_init = &adis1657x_spi_ip;

#ifdef GPIO_DRDY_PORT_NUM

	struct no_os_gpio_desc *adis_gpio_desc;

	ret = no_os_gpio_get_optional(&adis_gpio_desc, &adis_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adis_gpio_desc);
	if (ret)
		goto remove_gpio;
#endif

#ifdef NVIC_GPIO_IRQ
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto remove_irq_ctrl;
#endif

	return example_main();

remove_irq_ctrl:
	no_os_irq_ctrl_remove(nvic_desc);
remove_gpio:
	no_os_gpio_remove(adis_gpio_desc);

	return ret;
}

