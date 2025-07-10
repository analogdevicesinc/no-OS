/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADHV4710 example.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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
#include "maxim_spi.h"
#include "adhv4710.h"
#include "common_data.h"
#include "platform.h"

/* Toggle user LED */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

int main(void)
{
	uint8_t ret;
	/* counter for display values interval */
	uint32_t cnt;
	/* value read from the adhv registers */
	uint8_t value;
	/* used to set the alarm levels */
	uint8_t level;

	/* parameters initialization structure */
	struct adhv4710_init_param adhv4710_ip;
	/* device structure */
	struct adhv4710_dev *adhv4710_dev;

	/* uart descriptor */
	struct no_os_uart_desc *uart_desc;
	/* gpio descriptor */
	struct no_os_gpio_desc *gpio_desc;
	/* reset descriptor */
	struct no_os_gpio_desc *reset_desc;
	/* spi descriptor */
	struct no_os_spi_desc *spi_desc;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get_optional(&gpio_desc,
				      &gpio_led1_ip);
	if (ret)
		goto remove_uart;

	ret = no_os_gpio_get_optional(&reset_desc,
				      &gpio_reset_ip);
	if (ret)
		goto remove_led;

	if (reset_desc) {
		ret = no_os_gpio_direction_output(reset_desc, NO_OS_GPIO_HIGH);
		if (ret)
			goto remove_reset;
	}

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
						  NO_OS_GPIO_LOW);
	if (ret)
		goto remove_reset;

	/* Initialize SPI */
	adhv4710_ip.spi_init = &adhv4710_spi_ip;

	/* Init the reset */
	adhv4710_ip.gpio_reset = reset_desc;

	no_os_uart_stdio(uart_desc);

	pr_info("\n");
	pr_info("\n");
	pr_info("ADHV4710 SPI example \n");

	/* Init adhv4710 struct */
	adhv4710_dev = (struct adhv4710_dev *)no_os_calloc(1, sizeof(*adhv4710_dev));
	if (!adhv4710_dev)
		return -ENOMEM;

	/* Hard reset the ADE */
	adhv4710_dev->gpio_reset = adhv4710_ip.gpio_reset;
	ret = adhv4710_hw_reset(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* initialize the device with the values stored
	in the initialization structure */
	ret = adhv4710_init(&adhv4710_dev, adhv4710_ip);
	if (ret)
		goto free_dev;

	ret = adhv4710_clear_shutdown_latch(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* Enable all alarm latches */
	ret = adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_13,
				   ADHV4710_ALL_ALARM_LATCH_MSK,
				   ADHV4710_ALL_ALARM_LATCH_MSK);
	if (ret)
		goto free_dev;

	/* Nominal current */
	ret = adhv4710_set_quiescent_current(adhv4710_dev, 0x00, ADHV4710_INCREASE_I);
	if (ret)
		goto free_dev;

	/* set level of overcurrent source protection */
	level = (uint8_t) NO_OS_DIV_ROUND_CLOSEST((float)CURRENT_LIMIT_SOURCE,
			(float)ADHV4710_CURRENT_RESOLUTION);
	ret = adhv4710_set_source_overcurrent_level(adhv4710_dev, level);
	if (ret)
		goto free_dev;
	/* enable source overcurrent protection */
	ret = adhv4710_enable_source_overcurrent_protection(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* set level of overcurrent sink protection */
	level = (uint8_t) NO_OS_DIV_ROUND_CLOSEST((float)CURRENT_LIMIT_SINK,
			(float)ADHV4710_CURRENT_RESOLUTION);
	ret = adhv4710_set_sink_overcurrent_level(adhv4710_dev, level);
	if (ret)
		goto free_dev;
	/* enable sink overcurrent protection */
	ret = adhv4710_enable_sink_overcurrent_protection(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* Set level of overvoltage (positive) protection */
	level = (uint8_t) NO_OS_DIV_ROUND_CLOSEST((float)VOLTAGE_POSITIVE_LIMIT,
			(float)ADHV4710_VOLTAGE_RESOLUTION);
	ret = adhv4710_set_overvoltage_pos_level(adhv4710_dev, level);
	if (ret)
		goto free_dev;
	/* Enable overvoltage (positive) protection */
	ret = adhv4710_enable_overvoltage_pos_protection(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* Set level of overvoltage (negative) protection */
	level = (uint8_t) NO_OS_DIV_ROUND_CLOSEST((float)VOLTAGE_NEGATIVE_LIMIT,
			(float)ADHV4710_VOLTAGE_RESOLUTION);
	ret = adhv4710_set_overvoltage_neg_level(adhv4710_dev, level);
	if (ret)
		goto free_dev;
	/* Enable overvoltage (negative) protection */
	ret = adhv4710_enable_overvoltage_neg_protection(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* Set level of overtemperature protection */
	level = (uint8_t) NO_OS_DIV_ROUND_CLOSEST((float)TEMPERATURE_LIMIT +
			TEMP_OFFSET, (float)ADHV4710_TEMP_RESOLUTION);
	ret = adhv4710_set_overtemperature_level(adhv4710_dev, level);
	if (ret)
		goto free_dev;
	/* Enable overtemperature protection */
	ret = adhv4710_enable_overtemperature_protection(adhv4710_dev);
	if (ret)
		goto free_dev;

	/* reset the dispay counter */
	cnt = 0;

	while (1) {

		cnt++;

		if (cnt == READ_INTERVAL) {
			/* Check for alarms */
			adhv4710_read(adhv4710_dev, ADHV4710_REG_CTRL_REG_14, &value);
			if (value) {
				/* check if the amplifier is shutdown */
				if (value & ADHV4710_SHUTDOWN_FLAG_MSK)
					pr_info("Amplifier is shutdown\n");
				/* check which protections triggered */
				if (value & ADHV4710_OC_SRC_ALARM_MSK) {
					pr_info("Source Overcurrent Alarm\n");
					/* clear flag because is latched*/
					adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_14,
							     ADHV4710_OC_SRC_ALARM_MSK, 0);
				}
				if (value & ADHV4710_OC_SNK_ALARM_MSK) {
					pr_info("Sink Overcurrent Alarm\n");
					/* clear flag because is latched*/
					adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_14,
							     ADHV4710_OC_SNK_ALARM_MSK, 0);
				}
				if (value & ADHV4710_OV_POS_ALARM_MSK) {
					pr_info("Overvoltage Positive Alarm\n");
					/* clear flag because is latched*/
					adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_14,
							     ADHV4710_OV_POS_ALARM_MSK, 0);
				}
				if (value & ADHV4710_OV_NEG_ALARM_MSK) {
					pr_info("Overvoltage Negative Alarm\n");
					/* clear flag because is latched*/
					adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_14,
							     ADHV4710_OV_NEG_ALARM_MSK, 0);
				}
				if (value & ADHV4710_OT_ALARM_MSK) {
					pr_info("Overtemperature Alarm\n");
					/* clear flag because is latched*/
					adhv4710_update_bits(adhv4710_dev, ADHV4710_REG_CTRL_REG_14,
							     ADHV4710_OT_ALARM_MSK, 0);
				}
				if (value & ADHV4710_ALL_ALARM_LATCH_MSK) {
					/* re-enable amplifier from shutdown */
					ret = adhv4710_clear_shutdown_latch(adhv4710_dev);
					if (ret)
						goto free_dev;
				}
			}
			ret = interface_toggle_led(gpio_desc);
			if (ret)
				goto free_dev;
			cnt = 0;
		}
	}

free_dev:
	no_os_free(adhv4710_dev);
remove_reset:
	no_os_gpio_remove(reset_desc);
remove_led:
	no_os_gpio_remove(gpio_desc);
remove_uart:
	no_os_uart_remove(uart_desc);

error:
	pr_err("ERROR\n");
	return ret;

}

/**
 * @brief Toggle Led
 * @param gpio_led_desc - led descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	return no_os_gpio_set_value(gpio_led_desc, !val);
}
