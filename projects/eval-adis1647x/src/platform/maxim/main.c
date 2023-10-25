/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adis1647x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;

	adis1647x_ip.spi_init = &adis1647x_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adis1647x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	struct no_os_gpio_desc *adis_gpio_desc;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adis_gpio_desc, &adis_gpio_drdy_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(adis_gpio_desc);
	if (ret)
		goto remove_gpio;

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (ret)
		goto remove_gpio;

	ret = no_os_irq_enable(nvic_desc, NVIC_GPIO_IRQ);
	if (ret)
		goto remove_irq_ctrl;

	ret = iio_trigger_example_main();

remove_irq_ctrl:
	no_os_irq_ctrl_remove(nvic_desc);
remove_gpio:
	no_os_gpio_remove(adis_gpio_desc);
#endif

#if (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and re-build the project.
#endif

	return ret;
}

