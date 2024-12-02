/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE7913 example.
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
#include "ade7913.h"
#include "common_data.h"
#include "interrupt.h"
#include "platform.h"

// nvic descriptor
struct no_os_irq_ctrl_desc *ade7913_nvic_desc;

int main(void)
{
	int ret;
	uint8_t i = 0;
	// data read
	uint8_t reg_val;
	uint16_t cnt;
	// structure for the rms values
	struct rms_adc_values rms_values;
	/* memory allocated for the rms values
	depends on number of devices */
	int32_t *i_rms;
	int32_t *v1_rms;
	int32_t *v2_rms;
	int32_t *i_rms_adc;
	int32_t *v1_rms_adc;
	int32_t *v2_rms_adc;

	// parameters initialization structure
	struct ade7913_init_param ade7913_ip;
	// device structure
	struct ade7913_dev *ade7913_dev;
	// uart descriptor
	struct no_os_uart_desc *uart_desc;
	// gpio descriptor
	struct no_os_gpio_desc *gpio_desc;

	/* Initialize NVIC */
	ret = init_nvic(&ade7913_nvic_desc);
	if (ret)
		goto error;
	/* Initialize UART */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto remove_irq;
	/* Initialize LED GPIO */
	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		goto remove_uart;
	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_led;

	// Initial value for number of devices
	ade7913_ip.no_devs = 3;

	/* Initialize SPI */
	switch (ade7913_ip.no_devs) {
	case 3:
		ade7913_ip.spi_init0 = &ade7913_spi_ip0;
		ade7913_ip.spi_init1 = &ade7913_spi_ip1;
		ade7913_ip.spi_init2 = &ade7913_spi_ip2;
		break;
	case 2:
		ade7913_ip.spi_init0 = &ade7913_spi_ip0;
		ade7913_ip.spi_init1 = &ade7913_spi_ip1;
		break;
	default:
		ade7913_ip.spi_init0 = &ade7913_spi_ip0;
		break;
	}

	// allocate memory depending on the number of devices
	i_rms = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					sizeof(i_rms));
	if (!i_rms) {
		ret = -ENOMEM;
		goto remove_led;
	}
	v1_rms = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					 sizeof(v1_rms));
	if (!v1_rms) {
		ret = -ENOMEM;
		goto remove_led;
	}
	v2_rms = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					 sizeof(v2_rms));
	if (!v2_rms) {
		ret = -ENOMEM;
		goto remove_led;
	}

	rms_values.i_rms = i_rms;
	rms_values.v1_rms = v1_rms;
	rms_values.v2_rms = v2_rms;

	i_rms_adc = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					    sizeof(i_rms_adc));
	if (!i_rms_adc) {
		ret = -ENOMEM;
		goto remove_led;
	}
	v1_rms_adc = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					     sizeof(v1_rms_adc));
	if (!v1_rms_adc) {
		ret = -ENOMEM;
		goto remove_led;
	}
	v2_rms_adc = (int32_t *)no_os_calloc(ade7913_ip.no_devs,
					     sizeof(v2_rms_adc));
	if (!v2_rms_adc) {
		ret = -ENOMEM;
		goto remove_led;
	}

	rms_values.i_rms_adc = i_rms_adc;
	rms_values.v1_rms_adc = v1_rms_adc;
	rms_values.v2_rms_adc = v2_rms_adc;

	// Use UART for stdio
	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADE7913 SPI example \n");

	/* Init ade7913 struct */
	ade7913_dev = (struct ade7913_dev *)
		      no_os_calloc(1, sizeof(*ade7913_dev));
	if (!ade7913_dev) {
		ret = -ENOMEM;
		goto remove_led;
	}

	// Delay for ADE power up
	no_os_mdelay(1000);

	// Disable the burst mode
	ade7913_ip.burst_mode = DISABLE;

	/* initialize the device with the values stored
	in the initialization structure */
	ret = ade7913_init(&ade7913_dev, ade7913_ip);
	if (ret)
		goto free_dev;

	/* Version product of devices */
	for (i = 0; i < ade7913_dev->no_devs; i++)
		pr_info("Device %d version %d \n", i, ade7913_dev->ver_product[i]);
	pr_info("\n");

	/* Setup the devices */
	/* device 1 delivers clock to the other devices if no_devs > 1 */
	switch (ade7913_dev->no_devs) {
	case 3:
		ade7913_dev->spi_desc = ade7913_dev->spi_desc0;
		ret = ade7913_set_clkout_en(ade7913_dev, ENABLE);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc1;
		ret = ade7913_set_clkout_en(ade7913_dev, DISABLE);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc2;
		ret = ade7913_set_clkout_en(ade7913_dev, DISABLE);
		if (ret)
			goto free_dev;
		ret = ade7913_write_broadcast(ade7913_dev,
					      ADE7913_REG_SYNC_SNAP, ENABLE);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc0;
		ret = ade7913_write(ade7913_dev, ADE7913_REG_EMI_CTRL, 0x55);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc1;
		ret = ade7913_write(ade7913_dev, ADE7913_REG_EMI_CTRL, 0xAA);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc2;
		ret = ade7913_write(ade7913_dev, ADE7913_REG_EMI_CTRL, 0x55);
		if (ret)
			goto free_dev;
		break;
	case 2:
		ade7913_dev->spi_desc = ade7913_dev->spi_desc0;
		ret = ade7913_set_clkout_en(ade7913_dev, ENABLE);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc1;
		ret = ade7913_set_clkout_en(ade7913_dev, DISABLE);
		if (ret)
			goto free_dev;
		ret = ade7913_write_broadcast(ade7913_dev,
					      ADE7913_REG_SYNC_SNAP, ENABLE);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc0;
		ret = ade7913_write(ade7913_dev, ADE7913_REG_EMI_CTRL, 0x55);
		if (ret)
			goto free_dev;
		ade7913_dev->spi_desc = ade7913_dev->spi_desc1;
		ret = ade7913_write(ade7913_dev, ADE7913_REG_EMI_CTRL, 0xAA);
		if (ret)
			goto free_dev;
		break;
	default:
		ade7913_dev->spi_desc = ade7913_dev->spi_desc0;
		ret = ade7913_set_clkout_en(ade7913_dev, ENABLE);
		if (ret)
			goto free_dev;
		break;
	}

	/* Initialize data ready interrupt */
	ret = inter_init(ade7913_dev);
	if (ret)
		goto free_dev;

	/* Enable burst mode read*/
	ade7913_dev->burst_mode = ENABLE;

	// main loop
	while (1) {
		if (get_drdy_flag_state() >= 1) {
			// Reset data ready flag
			reset_drdy_low_flag_state();

			/* READ the data and place it in device structure */
			ret = ade7913_read_waveforms(ade7913_dev, ADE7913_REG_IWV, &reg_val);
			if (ret)
				goto free_dev;
			cnt++;
			// Print data
			if (cnt > 600) {
				pr_info("\n");
				pr_info("Device 1 values: \n");
				pr_info("v1_adc = %d \n", ade7913_dev->v1_wav_m[0]);
				pr_info("v2_adc =  %d \n", ade7913_dev->v2_wav_m[0]);
				pr_info("i_adc = %d \n", ade7913_dev->i_wav_m[0]);
				pr_info("v1_rms = %d mV\n", rms_values.v1_rms[0]);
				pr_info("v2_rms = %d mV\n", rms_values.v2_rms[0]);
				pr_info("i_rms = %d mA\n", rms_values.i_rms[0]);
				pr_info("\n");

				pr_info("Device 2 values: \n");
				pr_info("v1_adc = %d \n", ade7913_dev->v1_wav_m[1]);
				pr_info("v2_adc = %d \n", ade7913_dev->v2_wav_m[1]);
				pr_info("i_adc = %d \n", ade7913_dev->i_wav_m[1]);
				pr_info("v1_rms = %d mV\n", rms_values.v1_rms[1]);
				pr_info("v2_rms = %d mV\n", rms_values.v2_rms[1]);
				pr_info("i_rms = %d mA\n", rms_values.i_rms[1]);
				pr_info("\n");

				pr_info("Device 3 values: \n");
				pr_info("v1_adc = %d \n", ade7913_dev->v1_wav_m[2]);
				pr_info("v2_adc = %d \n", ade7913_dev->v2_wav_m[2]);
				pr_info("i_adc = %d \n", ade7913_dev->i_wav_m[2]);
				pr_info("v1_rms = %d mV\n", rms_values.v1_rms[2]);
				pr_info("v2_rms = %d mV\n", rms_values.v2_rms[2]);
				pr_info("i_rms = %d mA\n", rms_values.i_rms[2]);
				pr_info("___________________ \n");

				ret = interface_toggle_led(gpio_desc);
				if (ret)
					goto free_dev;

				cnt = 0;
			}
			// enable interrupt
			ret = no_os_irq_enable(ade7913_dev->irq_ctrl, GPIO_RDY_PIN);
			if (ret)
				goto free_dev;
		}
		/* Compute rms values*/
		ret = rms_adc_values_read(ade7913_dev, &rms_values);
		if (ret)
			goto free_dev;
		ret = rms_adc_values_read_2(ade7913_dev, &rms_values);
		if (ret)
			goto free_dev;
		ret = rms_adc_values_read_3(ade7913_dev, &rms_values);
		if (ret)
			goto free_dev;
	}

free_dev:
	no_os_free(ade7913_dev);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);
remove_irq:
	no_os_irq_ctrl_remove(ade7913_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;
}
