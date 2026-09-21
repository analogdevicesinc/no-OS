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

/*
 * Exercises the amplifier bias and enable registers. The data sheet states the
 * amplifiers power up enabled with a bias of 3, so the operational state is
 * checked against that before anything is written.
 */
static int adar300x_amplifier_demo(struct adar300x_dev *dev)
{
	uint8_t i, bias;
	bool enable;
	int ret;

	for (i = 0; i < dev->chip_info->num_beams; i++) {
		ret = adar300x_get_beam_amp(dev, ADAR300X_BEAM_AMP_OPERATIONAL,
					    i, &bias, &enable);
		if (ret)
			return ret;

		if (bias != 3 || !enable) {
			pr_err("beam %u operational amp: bias %u en %u, expected bias 3 en 1\n",
			       i, bias, enable);
			return -EIO;
		}
	}

	pr_info("Amplifier power-up defaults: OK (enabled, bias 3)\n");

	for (i = 0; i < dev->chip_info->num_beams; i++) {
		ret = adar300x_set_beam_amp(dev, ADAR300X_BEAM_AMP_SLEEP, i,
					    i & ADAR300X_AMP_BIAS_MAX, false);
		if (ret)
			return ret;

		ret = adar300x_get_beam_amp(dev, ADAR300X_BEAM_AMP_SLEEP, i,
					    &bias, &enable);
		if (ret)
			return ret;

		if (bias != (i & ADAR300X_AMP_BIAS_MAX) || enable) {
			pr_err("beam %u sleep amp: read bias %u en %u\n", i,
			       bias, enable);
			return -EIO;
		}
	}

	for (i = 0; i < dev->chip_info->num_elements; i++) {
		ret = adar300x_set_element_amp(dev,
					       ADAR300X_ELEMENT_AMP_SLEEP, i,
					       ADAR300X_AMP_BIAS_MAX - i, true);
		if (ret)
			return ret;

		ret = adar300x_get_element_amp(dev,
					       ADAR300X_ELEMENT_AMP_SLEEP, i,
					       &bias, &enable);
		if (ret)
			return ret;

		if (bias != ADAR300X_AMP_BIAS_MAX - i || !enable) {
			pr_err("element %u sleep amp: read bias %u en %u\n", i,
			       bias, enable);
			return -EIO;
		}
	}

	pr_info("Amplifier bias/enable write/readback: OK\n");

	return 0;
}

/*
 * Reads the temperature sensor twice and converts to millidegrees. A stuck or
 * unclocked ADC shows up as an out of range value, so the reading is bounds
 * checked against a plausible ambient rather than just printed.
 */
static int adar300x_adc_demo(struct adar300x_dev *dev)
{
	uint8_t code, code2, anlg0, anlg1;
	int mdegc, ret;

	ret = adar300x_adc_read(dev, ADAR300X_ADC_TEMPERATURE, &code);
	if (ret)
		return ret;

	ret = adar300x_adc_read(dev, ADAR300X_ADC_TEMPERATURE, &code2);
	if (ret)
		return ret;

	if (code > code2 + 2 || code2 > code + 2) {
		pr_err("temperature unstable: code %u then %u\n", code, code2);
		return -EIO;
	}

	mdegc = ADAR300X_TEMP_NOMINAL_MDEGC +
		((int)code - ADAR300X_TEMP_NOMINAL_CODE) *
		ADAR300X_TEMP_SLOPE_NUM / ADAR300X_TEMP_SLOPE_DEN;

	if (mdegc < 0 || mdegc > 60000) {
		pr_err("temperature %d mdegC outside plausible ambient\n",
		       mdegc);
		return -EIO;
	}

	pr_info("ADC temperature: %d.%03d degC (code %u)\n", mdegc / 1000,
		mdegc % 1000, code);

	/*
	 * The analog inputs are reported but not checked: they are eval board
	 * pins that need not be driven, so any code is legitimate.
	 */
	ret = adar300x_adc_read(dev, ADAR300X_ADC_ANALOG0, &anlg0);
	if (ret)
		return ret;

	ret = adar300x_adc_read(dev, ADAR300X_ADC_ANALOG1, &anlg1);
	if (ret)
		return ret;

	pr_info("ADC ANLG0 code %u, ANLG1 code %u\n", anlg0, anlg1);

	return 0;
}

static int adar300x_ram_demo(struct adar300x_dev *dev)
{
	const uint8_t pattern_a[ADAR300X_UNPACKED_BEAMSTATE_LEN] = {
		1, 62, 3, 60, 5, 58, 7, 56
	};
	const uint8_t pattern_b[ADAR300X_UNPACKED_BEAMSTATE_LEN] = {
		63, 0, 21, 42, 63, 0, 21, 42
	};
	uint8_t readback[ADAR300X_UNPACKED_BEAMSTATE_LEN];
	int ret, i;

	ret = adar300x_set_ram_beamstate(dev, 0, 0, pattern_a);
	if (ret)
		return ret;

	/* A different beam and the last state, so paging and bounds both move */
	ret = adar300x_set_ram_beamstate(dev, 3,
					 ADAR300X_RAM_STATES_PER_BEAM - 1,
					 pattern_b);
	if (ret)
		return ret;

	ret = adar300x_get_ram_beamstate(dev, 0, 0, readback);
	if (ret)
		return ret;

	for (i = 0; i < ADAR300X_UNPACKED_BEAMSTATE_LEN; i++) {
		if (readback[i] != pattern_a[i]) {
			pr_err("beam 0 state 0 value %d: wrote %u read %u\n", i,
			       pattern_a[i], readback[i]);
			return -EIO;
		}
	}

	ret = adar300x_get_ram_beamstate(dev, 3,
					 ADAR300X_RAM_STATES_PER_BEAM - 1,
					 readback);
	if (ret)
		return ret;

	for (i = 0; i < ADAR300X_UNPACKED_BEAMSTATE_LEN; i++) {
		if (readback[i] != pattern_b[i]) {
			pr_err("beam 3 state 63 value %d: wrote %u read %u\n", i,
			       pattern_b[i], readback[i]);
			return -EIO;
		}
	}

	pr_info("RAM beamstate write/readback: OK\n");

	return 0;
}

/*
 * The FIFO cannot be read back, so the write pointer stands in for the data:
 * each committed beamstate must advance it by one.
 */
static int adar300x_fifo_demo(struct adar300x_dev *dev)
{
	const uint8_t values[ADAR300X_UNPACKED_BEAMSTATE_LEN] = {
		9, 18, 27, 36, 45, 54, 63, 0
	};
	uint8_t before, after, read_ptr;
	const uint8_t loads = 3;
	int ret, i;

	ret = adar300x_get_fifo_pointers(dev, 0, &before, &read_ptr);
	if (ret)
		return ret;

	for (i = 0; i < loads; i++) {
		ret = adar300x_load_fifo_beamstate(dev, 0, values);
		if (ret)
			return ret;
	}

	ret = adar300x_get_fifo_pointers(dev, 0, &after, &read_ptr);
	if (ret)
		return ret;

	pr_info("FIFO beam 0 write pointer %u to %u after %u loads, read %u\n",
		before, after, loads, read_ptr);

	if (after != before + loads) {
		pr_err("write pointer %u after %u loads, expected %u\n", after,
		       loads, before + loads);
		return -EIO;
	}

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

	ret = adar300x_amplifier_demo(dev);
	if (ret)
		goto error_dev;

	ret = adar300x_adc_demo(dev);
	if (ret)
		goto error_dev;

	ret = adar300x_ram_demo(dev);
	if (ret)
		goto error_dev;

	ret = adar300x_fifo_demo(dev);
	if (ret)
		goto error_dev;

error_dev:
	adar300x_remove(dev);

error_uart:
	no_os_uart_remove(uart_desc);

	return ret;
}
