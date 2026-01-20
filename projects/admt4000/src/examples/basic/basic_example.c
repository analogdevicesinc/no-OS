/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example implementation for ADMT4000.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "basic_example.h"
#include "common_data.h"
#include "admt4000.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Basic example main function.
 *
 * This example demonstrates basic ADMT4000 functionality by continuously
 * reading sensor values and displaying them on the console.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int example_main()
{
	struct admt4000_dev *admt4000_desc;
	struct no_os_gpio_desc *gpio_v_en_desc;
	int ret, i;

	/* Raw sensor values */
	uint8_t raw_turns;
	uint16_t raw_angle[2];
	uint16_t raw_temp;
	int16_t raw_sine, raw_cosine;
	uint16_t raw_radius;
	uint16_t fault_reg;
	bool gpio_states[6];

	/* Converted values */
	int16_t quarter_turns;
	float angle_deg;
	float temp_celsius;
	float radius_mvv;

	ret = no_os_gpio_get(&gpio_v_en_desc, &gpio_v_en_ip);
	if (ret)
		return ret;

	/* Power on the ADMT4000 and allow communications*/
	ret = no_os_gpio_direction_output(gpio_v_en_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_gpio_ven;

	no_os_mdelay(ADMT4000_STARTUP_TIME_MS);
	/* Initialize ADMT4000 device */
	pr_info("Initializing ADMT4000...\r\n");
	admt4000_ip.gpio_fault_ip = gpio_fault_ip;
	ret = admt4000_init(&admt4000_desc, &admt4000_ip);
	if (ret) {
		pr_info("Failed to initialize ADMT4000: %d\r\n", ret);
		goto err_gpio_ven;
	}

	pr_info("ADMT4000 Basic Example Started\r\n");
	pr_info("Reading sensor values every 500ms...\r\n");

	/* Main loop - continuously read and display sensor values */
	while (1) {
		pr_info("-------------------------------------------\r\n");

		/* Read turns and angle efficiently with single SPI transaction */
		ret = admt4000_get_raw_turns_and_angle(admt4000_desc,
						       &raw_turns, raw_angle);
		if (ret) {
			pr_info("Error reading turns/angle: %d\r\n", ret);
			goto exit_example;
		}

		/* Convert quarter turns to signed value */
		ret = admt4000_quarter_turns_cnt(raw_turns, &quarter_turns);
		if (ret) {
			pr_info("Error converting quarter turns: %d\r\n", ret);
			goto exit_example;
		}

		/* Extract and convert angle to degrees */
		/* ABSANGLE register contains 10-bit absolute angle in bits [9:0] */
		/* ANGLE register contains 12-bit incremental angle */
		/* For this example, we'll use the 12-bit incremental angle */
		uint16_t angle_12bit = raw_angle[1];

		/* Convert Angle */
		ret = admt4000_angle_raw_to_scaled(angle_12bit, &angle_deg);
		if (ret) {
			pr_err("Failed to convert angle to degrees\r\n");
			continue;
		}
		pr_info("turns: %.2f (raw: 0x%02X)\r\n", quarter_turns / 4.0, raw_turns);
		pr_info("angle: %.4f degrees (raw: 0x%04X)\r\n", angle_deg, angle_12bit);

		ret = admt4000_get_temp(admt4000_desc, &raw_temp);
		if (ret) {
			pr_info("Error reading raw temperature: %d\r\n", ret);
			goto exit_example;
		}

		/* Convert temperature */
		ret = admt4000_temp_raw_to_scaled(raw_temp, &temp_celsius);
		if (ret) {
			pr_info("Error reading converted temperature: %d\r\n", ret);
			goto exit_example;
		}

		pr_info("temperature: %.2f degree Celsius (raw: 0x%04X)\r\n",
			temp_celsius, raw_temp);

		/* Read sine and cosine */
		ret = admt4000_get_sin(admt4000_desc, &raw_sine, NULL);
		if (ret) {
			pr_info("Error reading sine: %d\r\n", ret);
			goto exit_example;
		}

		ret = admt4000_get_cos(admt4000_desc, &raw_cosine, NULL);
		if (ret) {
			pr_info("Error reading cosine: %d\r\n", ret);
			goto exit_example;
		}

		pr_info("sine: %d (raw: 0x%04X)\r\n", raw_sine, (uint16_t)raw_sine);
		pr_info("cosine: %d (raw: 0x%04X)\r\n", raw_cosine, (uint16_t)raw_cosine);

		ret = admt4000_get_radius(admt4000_desc, &raw_radius, NULL);
		if (ret) {
			pr_info("Error reading raw radius: %d\r\n", ret);
			goto exit_example;
		}

		/* Convert radius */
		ret = admt4000_radius_raw_to_scaled(raw_radius, &radius_mvv);
		if (ret) {
			pr_info("Error reading converted radius: %d\r\n", ret);
			goto exit_example;
		}

		pr_info("radius: %.3f mV/V (raw: 0x%04X)\r\n",
			radius_mvv, raw_radius);

		/* Read GPIO states */
		pr_info("gpio states [");
		for (i = 0; i < 6; i++) {
			ret = admt4000_get_gpio(admt4000_desc, i, &gpio_states[i]);
			if (ret) {
				pr_info("Error reading GPIO%d: %d\r\n", i, ret);
				goto exit_example;
			}
			pr_info("%d", gpio_states[i] ? 1 : 0);
			if (i < 5) pr_info(", ");
		}
		pr_info("]\r\n");

		/* Read fault register using API function */
		ret = admt4000_get_faults(admt4000_desc, &fault_reg);
		if (ret) {
			pr_info("Error reading fault register: %d\r\n", ret);
			goto exit_example;
		}

		pr_info("fault: 0x%04X\r\n", fault_reg);

		pr_info("-------------------------------------------\r\n");

		/* Delay 500ms before next reading */
		no_os_mdelay(500);
	}

exit_example:
	/* Clean up resources */
	if (admt4000_desc)
		admt4000_remove(admt4000_desc);

err_gpio_ven:
	if (gpio_v_en_desc)
		no_os_gpio_remove(gpio_v_en_desc);

	return ret;
}
