/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for MAX34451 project.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
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
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "max34451.h"

struct max34451_init_param max34451_ip = {
	.i2c_init = &max34451_i2c_ip,
	.id = ID_ADPM12250
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

	ret = max34451_read_byte(dev, MAX34451_CHAN_ALL, MAX34451_MFR_ID, &byte_val);
	if (ret)
		pr_info("Failed to read manufacturer ID\n");
	else
		pr_info("Manufacturer ID: %x\n", byte_val);

	ret = max34451_read_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_REVISION,
				 &word_val);
	if (ret)
		pr_info("Failed to read device revision\n");
	else
		pr_info("Device Revision: %x\n", word_val);

	/* Works for MAX34451/34455 but not for ADPM due to command availability */
	ret = max34451_read_word(dev, MAX34451_CHAN_ALL, MAX34451_MFR_FW_SERIAL,
				 &word_val);
	if (ret)
		pr_info("Failed to read firmware revision\n");
	else
		pr_info("Firmware Revision: %x\n", word_val);

	ret = max34451_read_byte(dev, MAX34451_CHAN_ALL, MAX34451_PMBUS_REVISION,
				 &byte_val);
	if (ret)
		pr_info("Failed to read PMBus revision\n");
	else
		pr_info("PMBus Revision: %x\n", byte_val);

	ret = max34451_read_bit(dev, MAX34451_CHAN_ALL, MAX34451_CAPABILITY,
				MAX34451_CAPABILITY_PMBUS_SPEED, &bit_val);
	if (ret)
		pr_info("Failed to read PMBus capability\n");
	else
		pr_info("PMBus Capability (PMBus Speed): %lx\n", bit_val);

	ret = max34451_read_bit(dev, MAX34451_CHAN_ALL, MAX34451_CAPABILITY,
				MAX34451_CAPABILITY_PEC, &bit_val);
	if (ret)
		pr_info("Failed to read PMBus capability\n");
	else
		pr_info("PMBus Capability (PEC Support): %lx\n", bit_val);

	ret = max34451_update_bit(dev, MAX34451_CHAN_ALL, MAX34451_MFR_MODE,
				  NO_OS_BIT(13), 0);
	if (ret)
		pr_info("Failed to update PMBus capability\n");
	else
		pr_info("PMBus Capability (Alert Support) updated\n");

	ret = max34451_read_bit(dev, MAX34451_CHAN_ALL, MAX34451_CAPABILITY,
				MAX34451_CAPABILITY_ALERT, &bit_val);
	if (ret)
		pr_info("Failed to read PMBus capability\n");
	else
		pr_info("PMBus Capability (Alert Support): %lx\n", bit_val);

	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x80);
	if (ret)
		pr_info("Failed to enable write protection\n");
	else
		pr_info("Write protection enabled (0x80h)\n");

	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x40);
	if (ret)
		pr_info("Failed to enable write protection\n");
	else
		pr_info("Write protection enabled (0x40h)\n");

	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x20);
	if (ret)
		pr_info("Failed to enable write protection\n");
	else
		pr_info("Write protection enabled (0x20h)\n");

	ret = max34451_write_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT, 0x00);
	if (ret)
		pr_info("Failed to disable write protection\n");
	else
		pr_info("Write protection disabled (0x00h)\n");

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

	ret = max34451_read_byte(dev, MAX34451_CHAN_ALL, MAX34451_WRITE_PROTECT,
				 &byte_val);
	if (ret)
		pr_info("Failed to read WRITE_PROTECT\n");
	else
		pr_info("WRITE_PROTECT: 0x%x\n", byte_val);

	ret = max34451_read_block_data(dev, MAX34451_CHAN_ALL, MAX34451_MFR_SERIAL,
				       serial, 8);
	if (ret) {
		pr_info("Failed to read MFR_SERIAL\n");
	} else {
		pr_info("MFR_SERIAL: %02x%02x%02x%02x%02x%02x%02x%02x\n", serial[0],
			serial[1], serial[2], serial[3], serial[4], serial[5], serial[6], serial[7]);
	}

	ret = max34451_read_bit(dev, MAX34451_CHAN_ALL, MAX34451_STATUS_MFR_SPECIFIC,
				NO_OS_BIT(7), &bit_val);
	if (ret)
		pr_info("Failed to read LOCK bit\n");
	else
		pr_info("STATUS_MFR_SPECIFIC LOCK bit: %lu\n", bit_val);

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

	pr_info("\n=== Testing Operation Mode Settings ===\n");
	/* Meant to fail on ADPM devices */
	ret = max34451_set_operation(dev, MAX34451_CHAN_0,
				     MAX34451_OPERATION_ON_MARGIN_OFF);
	if (ret)
		pr_info("Failed to set operation mode\n");
	else
		pr_info("Set channel 0 to MARGIN_OFF mode\n");

	ret = max34451_read_byte(dev, MAX34451_CHAN_0, MAX34451_OPERATION,
				 &operation_mode);
	if (ret)
		pr_info("Failed to read operation mode\n");
	else
		pr_info("Channel 0 OPERATION mode: 0x%02x\n", operation_mode);

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

	pr_info("\n=== Testing Timing Settings ===\n");
	ret = max34451_set_timing(dev, MAX34451_CHAN_0, MAX34451_TON_DELAY_TYPE, 100);
	if (ret)
		pr_info("Failed to set TON_DELAY\n");
	else
		pr_info("Set channel 0 TON_DELAY to 100ms\n");

	ret = max34451_set_timing(dev, MAX34451_CHAN_0, MAX34451_TOFF_DELAY_TYPE, 50);
	if (ret)
		pr_info("Failed to set TOFF_DELAY\n");
	else
		pr_info("Set channel 0 TOFF_DELAY to 50ms\n");

	pr_info("\n=== Testing Bit Operations ===\n");
	ret = max34451_read_bit(dev, MAX34451_CHAN_ALL, MAX34451_STATUS_MFR_SPECIFIC,
				NO_OS_BIT(7), &bit_val);
	if (ret)
		pr_info("Failed to read LOCK bit\n");
	else
		pr_info("STATUS_MFR_SPECIFIC LOCK bit: %lu\n", bit_val);

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
