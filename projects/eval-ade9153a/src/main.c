/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE9153A example.
 *   @author Radu Etz (radu.etz@analog.com)
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
#include "ade9153a.h"
#include "common_data.h"

struct no_os_irq_ctrl_desc *ade9153a_nvic_desc;
struct no_os_irq_ctrl_desc *ade9153a_gpio_irq_desc;

int main(void)
{
	uint8_t i = 0;
	uint8_t ret;

	// parameters initialization structure
	struct ade9153a_init_param ade9153a_ip;
	// device structure
	struct ade9153a_dev *ade9153a_dev;

	// uart descriptor
	struct no_os_uart_desc *uart_desc;
	// gpio descriptor
	struct no_os_gpio_desc *gpio_desc;
	// reset descriptor
	struct no_os_gpio_desc *reset_desc;
	// ss descriptor
	struct no_os_gpio_desc *ss_desc;
	// sck descriptor
	struct no_os_gpio_desc *sck_desc;
	// spi descriptor
	struct no_os_spi_desc *spi_desc;

	/* Initialize NVIC IRQ controller in order to be able to enable
	GPIO IRQ interrupt */
	struct no_os_irq_init_param ade9153a_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade9153a_nvic_desc,
				  &ade9153a_nvic_ip);
	if (ret)
		goto error;

	ret = no_os_irq_set_priority(ade9153a_nvic_desc,
				     NVIC_GPIO_IRQ, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_ctrl_init(&ade9153a_gpio_irq_desc,
				  &ade9153a_gpio_irq_ip);
	if (ret)
		goto remove_irq;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto remove_irq;

	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		goto remove_uart;

	ret = no_os_gpio_get_optional(&ss_desc,
				      &gpio_ss_ip);
	if (ret)
		goto remove_led;

	ret = no_os_gpio_get_optional(&sck_desc,
				      &gpio_sck_ip);
	if (ret)
		goto remove_ss;

	ret = no_os_gpio_get_optional(&reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_sck;

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	ade9153a_ip.spi_init = &ade9153a_spi_ip;
	// ADE9153a dev DATA_RDY init params
	ade9153a_ip.gpio_rdy = &ade9153a_gpio_rdy_ip;

	// ADE9153a dev RESET init params
	ade9153a_ip.gpio_reset = &gpio_reset_ip;

	// ADE9153a dev SPI init params
	ade9153a_ip.gpio_ss = &gpio_ss_ip;
	ade9153a_ip.gpio_sck = &gpio_sck_ip;
	ade9153a_ip.spi_en = SPI_EN;
	// IRQ init params
	ade9153a_ip.irq_ctrl = ade9153a_gpio_irq_desc;
	// initialization for EV_ADE9153a_SHIELDZ
	ade9153a_ip.ai_swap = ENABLE;
	// PGA gain set to 16
	ade9153a_ip.ai_pga_gain = ADE9153A_AI_GAIN_16;
	// hpf corner frequency
	ade9153a_ip.hpf_crn = ADE9153A_HPF_CORNER_0_625_HZ;
	// frequency select
	ade9153a_ip.freq = F_50_HZ;
	// Vlevel value assuming 1/2 full scale
	ade9153a_ip.vlevel = ADE9153A_VLEVEL;
	// V_div rsmall value
	ade9153a_ip.vlevel = ADE9153A_RSMALL;
	// Energy accumulate set to 4000
	ade9153a_ip.no_samples = ADE9153A_NO_SAMPLES;
	// AIGAIN for IAP-IAN swap
	ade9153a_ip.ai_gain = ADE9153A_AIGAIN;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE9153A SPI example \n");

	/* Init ade9153a struct */
	ade9153a_dev = (struct ade9153a_dev *)no_os_calloc(1, sizeof(*ade9153a_dev));
	if (!ade9153a_dev)
		return -ENOMEM;

	/* initialize the device with the values stored
	in the initialization structure */
	ret = ade9153a_init(&ade9153a_dev, ade9153a_ip);
	if (ret)
		goto free_dev;

	// burst disable
	ade9153a_dev->burst_en = 0;

	no_os_mdelay(RESET_TIME);

	ret = ade9153a_setup(ade9153a_dev, ade9153a_ip);
	if (ret)
		goto free_dev;

	/* read the autocalibration values and update the
	registers */
	ret = autocalibration_read_vals(ade9153a_dev);
	if (ret)
		goto free_dev;

	/* set the temperature sample time */
	ret = ade9153a_temp_time(ade9153a_dev,
				 ADE9153A_TEMP_TIME_SAMPLES_1024);
	if (ret)
		goto free_dev;

	while (1) {
		// time delay between readings
		no_os_mdelay(READ_INTERVAL);
		/* read and print the ade9153a measured values */
		ret = read_measurements(ade9153a_dev);
		if (ret)
			goto free_dev;
		ret = interface_toggle_led(gpio_desc);
		if (ret)
			goto free_dev;
	}

free_dev:
	no_os_free(ade9153a_dev);
remove_reset:
	no_os_gpio_remove(reset_desc);
remove_sck:
	no_os_gpio_remove(sck_desc);
remove_ss:
	no_os_gpio_remove(ss_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(ade9153a_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;

}
