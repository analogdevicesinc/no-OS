/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE9078 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <stdio.h>
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "ade9078.h"
#include "platform.h"

int main(void)
{
	uint8_t ret;

	// parameters initialization structure
	struct ade9078_init_param ade9078_ip;
	// device structure
	struct ade9078_dev *ade9078_dev;

	// uart descriptor
	struct no_os_uart_desc *uart_desc;
	// psm0 descriptor
	struct no_os_gpio_desc *psm0_desc;
	// psm1 descriptor
	struct no_os_gpio_desc *psm1_desc;
	// reset descriptor
	struct no_os_gpio_desc *reset_desc;
	// gpio descriptor
	struct no_os_gpio_desc *gpio_desc;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		goto remove_uart;

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_led;

	ret = no_os_gpio_get_optional(&psm0_desc,
				      &gpio_psm0_ip);
	if (ret)
		goto remove_led;


	ret = no_os_gpio_get_optional(&psm1_desc,
				      &gpio_psm1_ip);
	if (ret)
		goto remove_psm0;

	if (psm1_desc)
		ret = no_os_gpio_direction_output(psm1_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_psm1;

	if (psm0_desc)
		ret = no_os_gpio_direction_output(psm0_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_psm1;

	ret = no_os_gpio_get_optional(&reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_psm1;

	if (reset_desc)
		ret = no_os_gpio_direction_output(reset_desc,
						  NO_OS_GPIO_HIGH);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	ade9078_ip.spi_init = &ade9078_spi_ip;
	/* Initialize PSM0 */
	ade9078_ip.psm0_desc = psm0_desc;
	/* Initialize PSM1 */
	ade9078_ip.psm1_desc = psm1_desc;
	/* Initialize Reset */
	ade9078_ip.reset_desc = reset_desc;

	/* Initialize power mode */
	ade9078_ip.power_mode = NORMAL_MODE;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE9078 SPI example \n");

	uint8_t val;

	/* Init ade9078 struct */
	ade9078_dev = (struct ade9078_dev *)no_os_calloc(1, sizeof(*ade9078_dev));
	if (!ade9078_dev)
		return -ENOMEM;

	/* Initialize the device with the values stored
	in the initialization structure */
	ret = ade9078_init(&ade9078_dev, ade9078_ip);
	if (ret)
		goto free_dev;
	/* setup the ade9078 device */
	ret = ade9078_setup(ade9078_dev);
	if (ret)
		goto free_dev;

	no_os_mdelay(RESET_TIME);

	while (1) {
		// time delay between readings
		no_os_mdelay(READ_INTERVAL);
		/* read and print the ade9078 rms measured values for PHASE A */
		ret = ade9078_read_data_ph(ade9078_dev, ADE9078_PHASE_A);
		if (ret)
			goto free_dev;

		pr_info("Vrms: %d mV\n", ade9078_dev->vrms_val);
		pr_info("Irms: %d mA\n", ade9078_dev->irms_val);

		/* toggle the LED on the MCU board */
		ret = interface_toggle_led(gpio_desc);
		if (ret)
			goto free_dev;
	}

free_dev:
	no_os_free(ade9078_dev);
remove_reset:
	no_os_gpio_remove(reset_desc);
remove_psm1:
	no_os_gpio_remove(psm1_desc);
remove_psm0:
	no_os_gpio_remove(psm0_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
