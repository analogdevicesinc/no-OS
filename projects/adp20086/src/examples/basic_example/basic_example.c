/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file of the adp20086 basic example project.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "adp20086.h"

/* Input protection window: trip below 4 V or above 16 V. */
#define ADP20086_EXAMPLE_UVIN_UV	4000000u
#define ADP20086_EXAMPLE_OVIN_UV	16000000u
/* Per-channel output undervoltage threshold. */
#define ADP20086_EXAMPLE_UVOUT_UV	4800000u
/* Delay between telemetry samples, in milliseconds. */
#define ADP20086_EXAMPLE_PERIOD_MS	1000
/* Console polling granularity while waiting out a sample period, in ms. */
#define ADP20086_EXAMPLE_POLL_MS	50

/**
 * @brief Report a fault signalled on INTB.
 *
 * Invoked from adp20086_process_interrupt() in thread context, so printing
 * and further bus access are both safe here.
 */
static void adp20086_fault_callback(struct adp20086_dev *dev,
				    const struct adp20086_status *status,
				    void *ctx)
{
	uint8_t fault;
	int ch;

	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		fault = status->overcurrent[ch] | status->ovout[ch] |
			status->uvout[ch] | status->thermal_shutdown[ch] |
			status->open_load[ch];

		if (!fault)
			continue;

		printf("  !! INTB CH%d:%s%s%s%s%s\n", ch + 1,
		       status->overcurrent[ch] ? " OC" : "",
		       status->ovout[ch] ? " OV" : "",
		       status->uvout[ch] ? " UV" : "",
		       status->thermal_shutdown[ch] ? " TSD" : "",
		       status->open_load[ch] ? " OPEN" : "");
	}

	if (status->ovin || status->uvin)
		printf("  !! INTB:%s%s\n",
		       status->ovin ? " OVIN" : "",
		       status->uvin ? " UVIN" : "");
}

/**
 * @brief Wait out a sample period while watching the console for a newline.
 *
 * Relies on the asynchronous RX FIFO, so no_os_uart_read() returns -EAGAIN
 * instead of blocking when nothing has been typed.
 *
 * @return true if the user pressed Enter, false if the delay elapsed first.
 */
static bool adp20086_stop_requested(struct no_os_uart_desc *uart, uint32_t ms)
{
	uint32_t step;
	uint8_t c;

	while (ms) {
		while (no_os_uart_read(uart, &c, 1) > 0) {
			if (c == '\r' || c == '\n')
				return true;
		}

		step = no_os_min(ms, (uint32_t)ADP20086_EXAMPLE_POLL_MS);
		no_os_mdelay(step);
		ms -= step;
	}

	return false;
}

int example_main()
{
	int ret;
	uint8_t part_id, revision;
	bool pece;
	uint32_t vin_uv, vdd_uv, vout_uv, iout_ua;
	struct adp20086_status status;
	struct adp20086_masks mask = adp20086_get_default_mask();
	struct adp20086_enabled_channels channels_en;

	struct adp20086_dev *adp20086_dev;
	struct no_os_uart_desc *uart_desc;
	struct no_os_irq_ctrl_desc *adp20086_gpio_irq_desc;

	ret = no_os_uart_init(&uart_desc, &adp20086_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	printf("ADP20086 Basic Example...\n\n");

	ret = no_os_irq_ctrl_init(&adp20086_gpio_irq_desc,
				  &adp20086_gpio_irq_ip);
	if (ret)
		goto remove_uart;

	adp20086_ip.irq_ctrl = adp20086_gpio_irq_desc;

	ret = adp20086_init(&adp20086_dev, &adp20086_ip);
	if (ret)
		goto remove_irq;

	ret = adp20086_set_interrupt_callback(adp20086_dev, adp20086_fault_callback,
					      NULL);
	if (ret)
		goto remove_dev;

	ret = adp20086_clear_status(adp20086_dev);
	if (ret)
		goto remove_dev;

	/* Report the identity captured during init. */
	ret = adp20086_get_part_id(adp20086_dev, &part_id);
	if (ret)
		goto remove_dev;

	ret = adp20086_get_revision(adp20086_dev, &revision);
	if (ret)
		goto remove_dev;

	ret = adp20086_get_pece(adp20086_dev, &pece);
	if (ret)
		goto remove_dev;

	printf("Detected ADP20086: part ID %u, revision %u, PEC %s\n\n",
	       part_id, revision, pece ? "enabled" : "disabled");

	/* Configure the input over/undervoltage protection window. */
	ret = adp20086_set_ovin(adp20086_dev, ADP20086_EXAMPLE_OVIN_UV);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_uvin(adp20086_dev, ADP20086_EXAMPLE_UVIN_UV);
	if (ret)
		goto remove_dev;

	for (int ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		/* Configure per-channel output UV and open-load thresholds. */
		ret = adp20086_set_uvout(adp20086_dev, ch, ADP20086_EXAMPLE_UVOUT_UV);
		if (ret)
			goto remove_dev;
	}

	ret = adp20086_configure_interrupt_masks(adp20086_dev, mask);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_all_channels(adp20086_dev, true);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_gpio_en(adp20086_dev, true);
	if (ret)
		goto remove_dev;

	ret = adp20086_arm_interrupt(adp20086_dev);
	if (ret)
		goto remove_dev;

	ret = adp20086_get_enabled_channels(adp20086_dev, &channels_en);
	if (ret)
		goto remove_dev;

	printf("Channels Enabled:\n");
	printf("OUT1: %s, OUT2: %s, OUT3: %s, OUT4: %s\n",
	       channels_en.out1 ? "Y" : "N",
	       channels_en.out2 ? "Y" : "N",
	       channels_en.out3 ? "Y" : "N",
	       channels_en.out4 ? "Y" : "N");
	printf("Reading telemetry...\n");
	printf("Press Enter to stop.\n\n");

	no_os_mdelay(ADP20086_EXAMPLE_PERIOD_MS);

	/* Sample the on-chip ADC and fault status until the user stops us. */
	for (unsigned int i = 1; ; i++) {
		ret = adp20086_read_vin(adp20086_dev, &vin_uv);
		if (ret)
			goto remove_dev;

		ret = adp20086_read_vdd(adp20086_dev, &vdd_uv);
		if (ret)
			goto remove_dev;

		printf("Sample %u: VIN = %u mV, VDD = %u mV\n",
		       i, vin_uv / 1000, vdd_uv / 1000);

		for (int ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
			ret = adp20086_read_vout(adp20086_dev, ch, &vout_uv);
			if (ret)
				goto remove_dev;

			ret = adp20086_read_iout(adp20086_dev, ch, &iout_ua);
			if (ret)
				goto remove_dev;

			printf("  CH%d: VOUT = %u mV, IOUT = %u mA\n",
			       ch + 1, vout_uv / 1000, iout_ua / 1000);
		}

		ret = adp20086_process_interrupt(adp20086_dev);
		if (ret)
			goto remove_dev;

		if (adp20086_stop_requested(uart_desc,
					    ADP20086_EXAMPLE_PERIOD_MS)) {
			printf("\nStop requested. Shutting down...\n");
			break;
		}
	}

	/* Controlled shutdown of all outputs before exiting. */
	ret = adp20086_reset_masks(adp20086_dev);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_all_channels(adp20086_dev, false);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_gpio_en(adp20086_dev, false);
	if (ret)
		goto remove_dev;

	ret = adp20086_disarm_interrupt(adp20086_dev);
	if (ret)
		goto remove_dev;

	printf("ADP20086 Basic Example done.\n");

	no_os_mdelay(100);

	adp20086_remove(adp20086_dev);
	no_os_irq_ctrl_remove(adp20086_gpio_irq_desc);
	no_os_uart_remove(uart_desc);

	return 0;

remove_dev:
	adp20086_remove(adp20086_dev);
remove_irq:
	no_os_irq_ctrl_remove(adp20086_gpio_irq_desc);
remove_uart:
	printf("Error: ADP20086 Exiting Example...\n");
	no_os_mdelay(ADP20086_EXAMPLE_PERIOD_MS);
	no_os_uart_remove(uart_desc);
	return ret;
}
