/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for ADAR300X: SPI and power validation.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "basic_example.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"
#include "no_os_util.h"

/*
 * Exercises direct beamstate control: put every beam in direct mode, take the
 * update command over SPI, write a distinct delay and attenuation to each
 * element, read them back, then strobe an update so the values are applied.
 */
static int adar300x_direct_control_demo(struct adar300x_dev *dev)
{
	uint8_t beam, element, delay, atten, rb;
	enum adar300x_beam_mode mode;
	int ret;

	ret = adar300x_set_update_source_spi(dev, true);
	if (ret)
		return ret;

	for (beam = 0; beam < dev->chip_info->num_beams; beam++) {
		ret = adar300x_set_beam_mode(dev, beam,
					     ADAR300X_BEAM_MODE_DIRECT);
		if (ret)
			return ret;

		ret = adar300x_get_beam_mode(dev, beam, &mode);
		if (ret)
			return ret;

		if (mode != ADAR300X_BEAM_MODE_DIRECT) {
			pr_err("beam %u mode: read %d\n", beam, mode);
			return -EIO;
		}
	}

	for (beam = 0; beam < dev->chip_info->num_beams; beam++) {
		for (element = 0; element < dev->chip_info->num_elements;
		     element++) {
			delay = (beam * ADAR300X_ELEMENTS_PER_BEAM + element) &
				ADAR300X_RAW_MAX;
			atten = ADAR300X_RAW_MAX - delay;

			ret = adar300x_set_element(dev,
						   ADAR300X_BEAMSTATE_DIRECT,
						   beam, element,
						   ADAR300X_DELAY, delay);
			if (ret)
				return ret;

			ret = adar300x_set_element(dev,
						   ADAR300X_BEAMSTATE_DIRECT,
						   beam, element,
						   ADAR300X_ATTENUATION, atten);
			if (ret)
				return ret;

			ret = adar300x_get_element(dev,
						   ADAR300X_BEAMSTATE_DIRECT,
						   beam, element,
						   ADAR300X_DELAY, &rb);
			if (ret)
				return ret;

			if (rb != delay) {
				pr_err("beam %u el %u delay: wrote 0x%02X read 0x%02X\n",
				       beam, element, delay, rb);
				return -EIO;
			}

			ret = adar300x_get_element(dev,
						   ADAR300X_BEAMSTATE_DIRECT,
						   beam, element,
						   ADAR300X_ATTENUATION, &rb);
			if (ret)
				return ret;

			if (rb != atten) {
				pr_err("beam %u el %u atten: wrote 0x%02X read 0x%02X\n",
				       beam, element, atten, rb);
				return -EIO;
			}
		}
	}

	pr_info("DIRECT beamstate write/readback: OK\n");

	ret = adar300x_update(dev, NO_OS_GENMASK(dev->chip_info->num_beams - 1,
			      0));
	if (ret)
		return ret;

	pr_info("UPDATE strobed on all %u beams\n", dev->chip_info->num_beams);

	return 0;
}

int basic_example_main(void)
{
	struct adar300x_dev *dev;
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adar300x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("ADAR300X basic example\n");

	ret = adar300x_init(&dev, &adar300x_ip);
	if (ret) {
		pr_err("adar300x_init failed: %d\n", ret);
		goto error_uart;
	}

	ret = adar300x_direct_control_demo(dev);
	if (ret)
		goto error_dev;

error_dev:
	adar300x_remove(dev);

error_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
