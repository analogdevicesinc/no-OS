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

/* Per-channel current limit applied to every output in this demo. */
#define ADP20086_EXAMPLE_ILIM		ADP20086_ILIM_416MA
/* Input protection window: trip below 4 V or above 16 V. */
#define ADP20086_EXAMPLE_UVIN_UV	4000000u
#define ADP20086_EXAMPLE_OVIN_UV	16000000u
/* Per-channel output undervoltage threshold. */
#define ADP20086_EXAMPLE_UVOUT_UV	6000000u
/* Per-channel open-load current threshold. */
#define ADP20086_EXAMPLE_IOPEN_UA	100000u
/* Number of telemetry samples to print before shutting down. */
#define ADP20086_EXAMPLE_SAMPLES	5
/* Delay between telemetry samples, in milliseconds. */
#define ADP20086_EXAMPLE_PERIOD_MS	1000

/**
 * @brief Report a fault signalled on INTB.
 *
 * Invoked from adp20086_process_interrupt() in thread context, so printing
 * and further bus access are both safe here.
 */
static void adp20086_fault_callback(struct adp20086_dev *dev,
				    const struct adp20086_device_status *dev_status,
				    const struct adp20086_channel_status *ch_status,
				    void *ctx)
{
	uint8_t faults;
	int ch;

	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		faults = ch_status->overcurrent | ch_status->overvoltage |
			 ch_status->undervoltage | ch_status->thermal_shutdown |
			 ch_status->open_load;

		if (!adp20086_channel_asserted(faults, ch))
			continue;

		printf("  !! INTB CH%d:%s%s%s%s%s\n", ch + 1,
		       adp20086_channel_asserted(ch_status->overcurrent, ch) ? " OC" : "",
		       adp20086_channel_asserted(ch_status->overvoltage, ch) ? " OV" : "",
		       adp20086_channel_asserted(ch_status->undervoltage, ch) ? " UV" : "",
		       adp20086_channel_asserted(ch_status->thermal_shutdown,
						 ch) ? " TSD" : "",
		       adp20086_channel_asserted(ch_status->open_load, ch) ? " OPEN" : "");
	}

	if (dev_status->ovin || dev_status->uvin)
		printf("  !! INTB input fault:%s%s\n",
		       dev_status->ovin ? " OVIN" : "",
		       dev_status->uvin ? " UVIN" : "");
}

int example_main()
{
	int ret;
	uint8_t part_id, revision;
	bool pece;
	uint32_t vin_uv, vdd_uv, vout_uv, iout_ua;
	struct adp20086_channel_status ch_status;
	struct adp20086_device_status dev_status;

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
		goto remove_uart;

	ret = adp20086_set_int_callback(adp20086_dev, adp20086_fault_callback,
					NULL);
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

	ret = adp20086_configure_clr(adp20086_dev, true);
	if (ret)
		goto remove_dev;

	/* Configure the current limit for all four channels. */
	ret = adp20086_set_ilim(adp20086_dev, ADP20086_ALL_CHANNELS,
				ADP20086_EXAMPLE_ILIM);
	if (ret)
		goto remove_dev;

	/* Configure the input over/undervoltage protection window. */
	ret = adp20086_set_ovin(adp20086_dev, ADP20086_EXAMPLE_OVIN_UV);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_uvin(adp20086_dev, ADP20086_EXAMPLE_UVIN_UV);
	if (ret)
		goto remove_dev;

	/* Configure per-channel output UV and open-load thresholds. */
	ret = adp20086_set_uvout(adp20086_dev, ADP20086_ALL_CHANNELS,
				 ADP20086_EXAMPLE_UVOUT_UV);
	if (ret)
		goto remove_dev;

	/* Use a controlled 1 ms current ramp at both turn-on and turn-off. */
	ret = adp20086_set_soft_start(adp20086_dev, ADP20086_ALL_CHANNELS,
				      ADP20086_SSU_1MS);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_soft_shutdown(adp20086_dev, ADP20086_ALL_CHANNELS,
					 ADP20086_SSD_1MS);
	if (ret)
		goto remove_dev;



#ifdef ADP20086_EVKIT_LOAD

	ret = adp20086_set_iopen(adp20086_dev, ADP20086_ALL_CHANNELS,
				 ADP20086_EXAMPLE_IOPEN_UA);
	if (ret)
		goto remove_dev;

	/* Enable open-load protection, then bring up all four outputs. */
	ret = adp20086_enable_open_protection(adp20086_dev,
					      ADP20086_ALL_CHANNELS);
	if (ret)
		goto remove_dev;

#endif

	ret = adp20086_enable_channel(adp20086_dev, ADP20086_ALL_CHANNELS);
	if (ret)
		goto remove_dev;

	ret = adp20086_set_gpio_en(adp20086_dev, true);
	if (ret)
		goto remove_dev;

	ret = adp20086_enable_intb(adp20086_dev);
	if (ret)
		goto remove_dev;

	printf("All channels enabled. Reading telemetry...\n\n");
	no_os_mdelay(ADP20086_EXAMPLE_PERIOD_MS);

	/* Periodically sample the on-chip ADC and fault status. */
	for (int i = 0; i < ADP20086_EXAMPLE_SAMPLES; i++) {
		ret = adp20086_read_vin_uv(adp20086_dev, &vin_uv);
		if (ret)
			goto remove_dev;

		ret = adp20086_read_vdd_uv(adp20086_dev, &vdd_uv);
		if (ret)
			goto remove_dev;

		printf("Sample %d: VIN = %u mV, VDD = %u mV\n",
		       i + 1, vin_uv / 1000, vdd_uv / 1000);

		if (ret)
			goto remove_dev;

		for (int ch = ADP20086_CHANNEL1;
		     ch <= ADP20086_CHANNEL4; ch++) {
			ret = adp20086_read_vout_uv(adp20086_dev, ch, &vout_uv);
			if (ret)
				goto remove_dev;

			ret = adp20086_read_iout_ua(adp20086_dev, ch, &iout_ua);
			if (ret)
				goto remove_dev;

			printf("  CH%d: VOUT = %u mV, IOUT = %u mA\n",
			       ch + 1, vout_uv / 1000, iout_ua / 1000);
		}

		ret = adp20086_process_interrupt(adp20086_dev);
		if (ret)
			goto remove_dev;

		no_os_mdelay(ADP20086_EXAMPLE_PERIOD_MS);
	}

	/* Controlled shutdown of all outputs before exiting. */
	ret = adp20086_reset_masks(adp20086_dev);
	if (ret)
		goto remove_dev;
	ret = adp20086_disable_channel(adp20086_dev, ADP20086_ALL_CHANNELS);
	if (ret)
		goto remove_dev;
	ret = adp20086_set_gpio_en(adp20086_dev, false);
	if (ret)
		goto remove_dev;
	ret = adp20086_disable_intb(adp20086_dev);
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
	no_os_irq_ctrl_remove(adp20086_gpio_irq_desc);
remove_uart:
	printf("Error: ADP20086 Exiting Example...\n");
	no_os_mdelay(ADP20086_EXAMPLE_PERIOD_MS);
	no_os_uart_remove(uart_desc);
	return ret;
}
