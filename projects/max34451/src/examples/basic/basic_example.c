/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for MAX34451 project.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "max34451.h"

struct max34451_init_param max34451_ip = {
	.i2c_init = &max34451_i2c_ip,
	.id = ID_MAX34451,
};

int example_main()
{
	struct max34451_dev *dev = NULL;
	struct max34451_status status;
	struct no_os_uart_desc *uart_desc = NULL;
	int ret = 0, vals[4];
	uint8_t chan, byte_val, serial[8];
	uint16_t word_val;
	uint8_t operation_mode;
	int margin_low, margin_high, i;
	uint32_t bit_val;

	ret = no_os_uart_init(&uart_desc, &max34451_uart_ip);
	if (ret) {
		pr_info("UART initialization failed\n");
		goto exit;
	}

	no_os_uart_stdio(uart_desc);

	pr_info("INITIALIZING\n");
	ret = max34451_init(&dev, &max34451_ip);
	if (ret) {
		pr_info("Device initialization failed\n");
		goto exit;
	} else {
		pr_info("Device initialized successfully\n");
	}

	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x80);
	if (ret)
		pr_info("Failed to enable write protection\n");
	else
		pr_info("Write protection enabled (0x80h)\n");


	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x00);
	if (ret)
		pr_info("Failed to disable write protection\n");
	else
		pr_info("Write protection disabled (0x00h)\n");

	ret = max34451_read_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT,
				 &byte_val);
	if (ret)
		pr_info("Failed to read WRITE_PROTECT\n");
	else
		pr_info("WRITE_PROTECT: 0x%x\n", byte_val);

	ret = max34451_read_byte(dev, MAX34451_CHAN_ALL, MAX34451_VOUT_MODE, &byte_val);
	if (ret)
		pr_info("Failed to read VOUT_MODE\n");
	else
		pr_info("VOUT_MODE: %x\n", byte_val);

	pr_info("\n=== Testing Page Operations ===\n");
	for (chan = MAX34451_CHAN_0; chan <= MAX34451_CHAN_3; chan++) {
		ret = max34451_set_page(dev, chan);
		if (ret)
			pr_info("Failed to set page %d\n", chan);
		else
			pr_info("Successfully set page to channel %d\n", chan);
	}

	pr_info("\n=== Testing Status Read Operations ===\n");
	for (chan = MAX34451_CHAN_0; chan <= MAX34451_CHAN_1; chan++) {
		ret = max34451_read_status(dev, chan, MAX34451_STATUS_WORD_TYPE, &status);
		if (ret)
			pr_info("Failed to read STATUS_WORD for channel %d\n", chan);
		else
			pr_info("Channel %d STATUS_WORD: 0x%04x\n", chan, status.word);

		ret = max34451_read_status(dev, chan, MAX34451_STATUS_VOUT_TYPE, &status);
		if (ret)
			pr_info("Failed to read STATUS_VOUT for channel %d\n", chan);
		else
			pr_info("Channel %d STATUS_VOUT: 0x%02x\n", chan, status.vout);

		ret = max34451_read_status(dev, chan, MAX34451_STATUS_IOUT_TYPE, &status);
		if (ret)
			pr_info("Failed to read STATUS_IOUT for channel %d\n", chan);
		else
			pr_info("Channel %d STATUS_IOUT: 0x%02x\n", chan, status.iout);
	}

	pr_info("\n=== Testing Value Read Operations ===\n");
	/* Would not work properly with ADPM with page-dependent commands */
	for (chan = MAX34451_CHAN_0; chan <= MAX34451_CHAN_1; chan++) {
		ret = max34451_read_value(dev, chan, MAX34451_VAL_VOUT, &vals[0]);
		if (ret)
			pr_info("Failed to read VOUT for channel %d\n", chan);
		else
			pr_info("Channel %d VOUT: %d mV\n", chan, vals[0]);

		ret = max34451_read_value(dev, chan, MAX34451_VAL_IOUT, &vals[1]);
		if (ret)
			pr_info("Failed to read IOUT for channel %d\n", chan);
		else
			pr_info("Channel %d IOUT: %d mA\n", chan, vals[1]);

		ret = max34451_read_value(dev, chan, MAX34451_VAL_VOUT_PEAK, &vals[2]);
		if (ret)
			pr_info("Failed to read VOUT_PEAK for channel %d\n", chan);
		else
			pr_info("Channel %d VOUT_PEAK: %d mV\n", chan, vals[2]);
	}

	pr_info("\n=== Testing Word Read Operation ===\n");
	word_val = 0;
	ret = max34451_read_word(dev, MAX34451_CHAN_2, MAX34451_READ_VOUT, &word_val);
	if (ret)
		pr_info("Failed to read VOUT word from channel 2\n");
	else
		pr_info("Channel 2 READ_VOUT (raw): 0x%04x\n", word_val);

	pr_info("\n=== Testing Clear Faults ===\n");
	ret = max34451_clear_faults(dev);
	if (ret)
		pr_info("Failed to clear faults\n");
	else
		pr_info("Faults cleared successfully\n");

	pr_info("\n=== Testing Margin Settings ===\n");
	ret = max34451_vout_margin(dev, MAX34451_CHAN_2, 2800, 3200);
	if (ret)
		pr_info("Failed to set VOUT margins\n");
	else
		pr_info("Set Channel 2 Margins: LOW=2800mV, HIGH=3200mV\n");

	ret = max34451_read_word_data(dev, MAX34451_CHAN_2, MAX34451_VOUT_MARGIN_LOW,
				      &margin_low);
	if (ret)
		pr_info("Failed to read VOUT_MARGIN_LOW\n");
	else
		pr_info("Channel 2 VOUT_MARGIN_LOW: %d mV\n", margin_low);

	ret = max34451_read_word_data(dev, MAX34451_CHAN_2, MAX34451_VOUT_MARGIN_HIGH,
				      &margin_high);
	if (ret)
		pr_info("Failed to read VOUT_MARGIN_HIGH\n");
	else
		pr_info("Channel 2 VOUT_MARGIN_HIGH: %d mV\n", margin_high);

	ret = max34451_vout_margin(dev, MAX34451_CHAN_2, 2900, 3300);
	if (ret)
		pr_info("Failed to change VOUT Margin\n");
	else
		pr_info("Set Channel 2 Margins: LOW=2900mV, HIGH=3300mV\n");

	ret = max34451_read_word_data(dev, MAX34451_CHAN_2, MAX34451_VOUT_MARGIN_LOW,
				      &margin_low);
	if (ret)
		pr_info("Failed to read VOUT_MARGIN_LOW\n");
	else
		pr_info("Channel 2 VOUT_MARGIN_LOW: %d mV\n", margin_low);

	ret = max34451_read_word_data(dev, MAX34451_CHAN_2, MAX34451_VOUT_MARGIN_HIGH,
				      &margin_high);
	if (ret)
		pr_info("Failed to read VOUT_MARGIN_HIGH\n");
	else
		pr_info("Channel 2 VOUT_MARGIN_HIGH: %d mV\n", margin_high);

	pr_info("\n=== Continuous Monitoring Loop (5 iterations) ===\n");
	if (dev->id == ID_MAX34451 || dev->id == ID_MAX34451NA6
	    || dev->id == ID_MAX34455) {
		for (i = 0; i < 5; i++) {
			pr_info("\n--- Iteration %d ---\n", i + 1);
			for (chan = MAX34451_CHAN_0; chan <= MAX34451_CHAN_1; chan++) {
				ret = max34451_read_value(dev, chan, MAX34451_VAL_VOUT, &vals[0]);
				if (ret) {
					pr_info("Failed to read VOUT for channel %d\n", chan);
					continue;
				}
				ret = max34451_read_value(dev, chan, MAX34451_VAL_IOUT, &vals[1]);
				if (ret) {
					pr_info("Failed to read IOUT for channel %d\n", chan);
					continue;
				}

				pr_info("CH%d: VOUT=%dmV IOUT=%dmA\n",
					chan, vals[0], vals[1]);
				no_os_mdelay(500);
			}
		}
	}
	if (dev->id == ID_ADPM12160 || dev->id == ID_ADPM12200
	    || dev->id == ID_ADPM12250) {
		for (i = 0; i < 5; i++) {
			ret = max34451_read_value(dev, ADPM12XXX_CHAN_2, MAX34451_VAL_VOUT, &vals[0]);
			if (ret) {
				pr_info("Failed to read VOUT for channel %d\n", ADPM12XXX_CHAN_2);
				continue;
			}
			ret = max34451_read_value(dev, ADPM12XXX_CHAN_4, MAX34451_VAL_IOUT, &vals[1]);
			if (ret) {
				pr_info("Failed to read IOUT for channel %d\n", ADPM12XXX_CHAN_4);
				continue;
			}

			pr_info("VOUT=%dmV IOUT=%dmA\n",
				vals[0], vals[1]);
			no_os_mdelay(500);
		}
	}
	pr_info("\n=== PMBus Driver Test Complete ===\n");
exit:
	if (dev)
		max34451_remove(dev);

	if (uart_desc)
		no_os_uart_remove(uart_desc);

	return ret;
}
