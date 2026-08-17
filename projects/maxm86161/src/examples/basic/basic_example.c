/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for the MAXM86161 driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
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

#include <stdio.h>
#include "common_data.h"
#include "maxm86161.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* Number of distinct FIFO tag values (5-bit tag field). */
#define MAXM86161_NUM_TAGS 32

/* Human-readable name for a FIFO sample tag. */
static const char *maxm86161_tag_name(uint8_t tag)
{
	switch (tag) {
	case MAXM86161_TAG_PPG1_LEDC1:
		return "PPG1_LEDC1";
	case MAXM86161_TAG_PPG1_LEDC2:
		return "PPG1_LEDC2";
	case MAXM86161_TAG_PPG1_LEDC3:
		return "PPG1_LEDC3";
	case MAXM86161_TAG_PPG1_LEDC4:
		return "PPG1_LEDC4";
	case MAXM86161_TAG_PPG1_LEDC5:
		return "PPG1_LEDC5";
	case MAXM86161_TAG_PPG1_LEDC6:
		return "PPG1_LEDC6";
	case MAXM86161_TAG_PROX1:
		return "PROX1";
	case MAXM86161_TAG_SUB_DAC_UPDATE:
		return "SUB_DAC_UPDATE";
	case MAXM86161_TAG_INVALID:
		return "INVALID";
	case MAXM86161_TAG_TIME_STAMP:
		return "TIME_STAMP";
	default:
		return "UNKNOWN";
	}
}

int example_main(void)
{
	struct maxm86161_dev *dev;
	int ret;
	uint8_t fifo_count;
	uint8_t fifo_buf[MAXM86161_FIFO_DEPTH * MAXM86161_FIFO_DATA_SIZE];
	uint8_t tag;
	uint32_t data;
	uint8_t i;
	int32_t temperature;

	pr_info("MAXM86161 Basic Example\n");

	ret = maxm86161_init(&dev, &maxm86161_ip);
	if (ret) {
		pr_err("MAXM86161 init failed: %d\n", ret);
		return ret;
	}

	pr_info("MAXM86161 initialized. Part ID: 0x%02X Rev: 0x%02X\n",
		dev->part_id, dev->rev_id);

	/* Configure PPG: 100sps, 117.3us integration, 16uA ADC range */
	ret = maxm86161_set_integration_time(dev, MAXM86161_TINT_117_3_US);
	if (ret) {
		pr_err("Failed to set integration time: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_adc_range(dev, MAXM86161_ADC_RGE_16UA);
	if (ret) {
		pr_err("Failed to set ADC range: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_sample_averaging(dev, MAXM86161_SMP_AVE_1);
	if (ret) {
		pr_err("Failed to set Sample Averaging: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_sample_rate(dev, 0);
	if (ret) {
		pr_err("Failed to set Sample Rate: %d\n", ret);
		goto cleanup;
	}

	/* Configure LED settling time */
	ret = maxm86161_set_led_settling(dev, MAXM86161_LED_SETLNG_12_US);
	if (ret) {
		pr_err("Failed to set LED Settling time: %d\n", ret);
		goto cleanup;
	}

	/* Configure PD bias for typical application */
	ret = maxm86161_set_pd_bias(dev, MAXM86161_PD_BIAS_0_65PF);
	if (ret) {
		pr_err("Failed to set PD Bias: %d\n", ret);
		goto cleanup;
	}

	/* Configure LEDs: 124mA range, full-scale drive current, sequence slot */
	ret = maxm86161_set_led_cfg(dev, MAXM86161_LED_NUM_GREEN,
				    MAXM86161_LED_RGE_124MA, 2, 0xFF);
	if (ret) {
		pr_err("Failed to configure GREEN LED: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_led_cfg(dev, MAXM86161_LED_NUM_RED,
				    MAXM86161_LED_RGE_124MA, 3, 0xFF);
	if (ret) {
		pr_err("Failed to configure RED LED: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_led_cfg(dev, MAXM86161_LED_NUM_IR,
				    MAXM86161_LED_RGE_124MA, 6, 0xFF);
	if (ret) {
		pr_err("Failed to configure IR LED: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_led_cfg(dev, MAXM86161_LED_SRC_PILOT_GREEN,
				    MAXM86161_LED_RGE_124MA, 1, 0xFF);
	if (ret) {
		pr_err("Failed to configure IR LED: %d\n", ret);
		goto cleanup;
	}

	/* Remaining sequence slots: pilot green, direct ambient, unused */

	ret = maxm86161_set_led_sequence(dev, 4, MAXM86161_LED_SRC_DIRECT_AMB);
	if (ret) {
		pr_err("Failed to set LED sequence for LED %d: %d\n", 4, ret);
		goto cleanup;
	}

	ret = maxm86161_set_led_sequence(dev, 5, MAXM86161_LED_SRC_NONE);
	if (ret) {
		pr_err("Failed to set LED sequence for LED %d: %d\n", 5, ret);
		goto cleanup;
	}

	/* Configure FIFO */
	ret = maxm86161_set_fifo_watermark(dev, 15);
	if (ret) {
		pr_err("Failed to set FIFO watermark: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_fifo_rollover(dev, true);
	if (ret) {
		pr_err("Failed to set FIFO Rollover setting: %d\n", ret);
		goto cleanup;
	}

	/* Enable low power mode for <= 256sps */
	ret = maxm86161_set_low_power_mode(dev, true);
	if (ret) {
		pr_err("Failed to set Low Power mode: %d\n", ret);
		goto cleanup;
	}

	/* Clear interrupts and start sampling (exit shutdown) */
	ret = maxm86161_clear_interrupt_status(dev);
	if (ret) {
		pr_err("Failed to clear statuses: %d\n", ret);
		goto cleanup;
	}

	ret = maxm86161_set_shutdown(dev, false);
	if (ret) {
		pr_err("Failed to set to shutdown mode: %d\n", ret);
		goto cleanup;
	}

	pr_info("Sampling started. Reading FIFO data...\n");

	while (1) {
		no_os_mdelay(500);

		ret = maxm86161_read_die_temperature(dev, &temperature);
		if (ret) {
			pr_err("Failed to read die temp: %d\n", ret);
			goto cleanup;
		}

		/* temperature is in micro-degrees Celsius (1 degC = 1000000). */
		pr_info("Die temperature: %.2f C\n", temperature / 1000000.0f);

		ret = maxm86161_get_fifo_count(dev, &fifo_count);
		if (ret) {
			pr_info("Error reading FIFO count: %d\n", ret);
			break;
		}

		if (fifo_count == 0) {
			pr_info("FIFO count is 0\n");
			continue;
		}

		pr_info("FIFO count: %d\n", fifo_count);

		ret = maxm86161_read_fifo_data(dev, fifo_buf, fifo_count);
		if (ret) {
			pr_info("Error reading FIFO data: %d\n", ret);
			break;
		}

		/* Tally which tags are present in this batch. */
		uint16_t tag_count[MAXM86161_NUM_TAGS] = {0};
		uint8_t j;

		for (j = 0; j < fifo_count; j++) {
			ret = maxm86161_decode_fifo_sample(
				      &fifo_buf[j * MAXM86161_FIFO_DATA_SIZE],
				      &tag, &data);
			if (ret) {
				pr_info("Error decoding FIFO data: %d\n", ret);
				continue;
			}
			tag_count[tag]++;
		}

		/* Print every sample on one tab-separated line per tag ID. */
		for (tag = 0; tag < MAXM86161_NUM_TAGS; tag++) {
			if (tag_count[tag] == 0)
				continue;

			printf("  Tag 0x%02X (%s):", tag,
			       maxm86161_tag_name(tag));

			for (j = 0; j < fifo_count; j++) {
				uint8_t sample_tag;

				ret = maxm86161_decode_fifo_sample(
					      &fifo_buf[j * MAXM86161_FIFO_DATA_SIZE],
					      &sample_tag, &data);
				if (ret || sample_tag != tag)
					continue;
				printf("\t%u", data);
			}
			printf("\n");
		}
	}

cleanup:
	pr_info("Shutting down MAXM86161.\n");
	maxm86161_remove(dev);

	return ret;
}
