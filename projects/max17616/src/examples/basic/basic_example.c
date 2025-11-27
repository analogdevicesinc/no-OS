/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for max17616 project.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "max17616.h"
#include <stdlib.h>

/**
 * @brief Function pointer type for fault processing
 * @param fault_group - Fault group identifier
 * @param bit - Bit position
 * @return Pointer to fault description string or NULL if not found
 */
typedef const char *(*fault_processor_t)(uint16_t fault_group, uint8_t bit);

/**
 * @brief Process fault bits using a function pointer
 * @param fault_value - 8-bit fault register value
 * @param fault_group - Fault group identifier
 * @param fault_name - Name of the fault type for display
 * @param processor - Function pointer to process individual faults
 */
static void process_fault_bits(uint8_t fault_value, uint16_t fault_group,
			       const char *fault_name,
			       fault_processor_t processor)
{
	const char *fault_desc;

	if (!fault_value)
		return;

	pr_info("%s Faults: 0x%02X\n\r", fault_name, fault_value);
	for (int bit = 0; bit < 8; bit++) {
		if (fault_value & (1 << bit)) {
			fault_desc = processor(fault_group, bit);
			if (fault_desc)
				pr_info("  [%d] %s\n\r", bit, fault_desc);
		}
	}
}

/**
 * @brief Display telemetry data in a formatted way
 * @param telemetry - Telemetry structure to display
 */
static void display_telemetry(struct max17616_telemetry *telemetry)
{
	pr_info("=== MAX17616 Telemetry ===\n\r");

	if (telemetry->valid_mask & NO_OS_BIT(0))
		pr_info("VIN:         %.3f V\n\r", telemetry->vin);

	if (telemetry->valid_mask & NO_OS_BIT(1))
		pr_info("VOUT:        %.3f V\n\r", telemetry->vout);

	if (telemetry->valid_mask & NO_OS_BIT(3))
		pr_info("IOUT:        %.3f A\n\r", telemetry->iout);

	if (telemetry->valid_mask & NO_OS_BIT(4))
		pr_info("Temperature: %.1f °C\n\r", telemetry->temp1);

	if (telemetry->valid_mask & NO_OS_BIT(5))
		pr_info("Power:       %.3f W\n\r", telemetry->pout);

	pr_info("\n\r");
}

/**
 * @brief Display device-specific settings
 * @param dev - MAX17616 device structure
 */
static void display_device_settings(struct max17616_dev *dev)
{
	enum max17616_current_limit_mode clmode;
	enum max17616_istart_ratio istart_ratio;
	enum max17616_overcurrent_timeout tstoc;
	enum max17616_overcurrent_limit istlim;
	bool operation_enabled;
	int ret;

	pr_info("=== Device Settings ===\n\r");

	ret = max17616_get_current_limit_mode(dev, &clmode);
	if (ret == 0) {
		const char *mode_str;
		switch (clmode) {
		case MAX17616_CLMODE_LATCH_OFF:
			mode_str = "Latch-off";
			break;
		case MAX17616_CLMODE_CONTINUOUS:
			mode_str = "Continuous";
			break;
		case MAX17616_CLMODE_AUTO_RETRY:
			mode_str = "Auto-retry";
			break;
		default:
			mode_str = "Unknown";
		}
		pr_info("Current Limit Mode: %s (0x%02X)\n\r", mode_str, (uint8_t)clmode);
	}

	ret = max17616_get_istart_ratio(dev, &istart_ratio);
	if (ret == 0) {
		const char *ratio_str;
		switch (istart_ratio) {
		case MAX17616_ISTART_FULL:
			ratio_str = "Full (I_limit)";
			break;
		case MAX17616_ISTART_HALF:
			ratio_str = "Half (I_limit/2)";
			break;
		case MAX17616_ISTART_QUARTER:
			ratio_str = "Quarter (I_limit/4)";
			break;
		case MAX17616_ISTART_EIGHTH:
			ratio_str = "Eighth (I_limit/8)";
			break;
		case MAX17616_ISTART_SIXTEENTH:
			ratio_str = "Sixteenth (I_limit/16)";
			break;
		default:
			ratio_str = "Unknown";
		}
		pr_info("Current Start Ratio: %s (0x%02X)\n\r", ratio_str,
			(uint8_t)istart_ratio);
	}

	ret = max17616_get_overcurrent_timeout(dev, &tstoc);
	if (ret == 0) {
		const char *timeout_str;
		switch (tstoc) {
		case MAX17616_TIMEOUT_400US:
			timeout_str = "400 microseconds";
			break;
		case MAX17616_TIMEOUT_1MS:
			timeout_str = "1 millisecond";
			break;
		case MAX17616_TIMEOUT_4MS:
			timeout_str = "4 milliseconds";
			break;
		case MAX17616_TIMEOUT_24MS:
			timeout_str = "24 milliseconds";
			break;
		default:
			timeout_str = "Unknown";
		}
		pr_info("Overcurrent Timeout: %s (0x%02X)\n\r", timeout_str, (uint8_t)tstoc);
	}

	ret = max17616_get_overcurrent_limit(dev, &istlim);
	if (ret == 0) {
		const char *limit_str;
		switch (istlim) {
		case MAX17616_OC_LIMIT_1_25:
			limit_str = "1.25:1 ratio";
			break;
		case MAX17616_OC_LIMIT_1_50:
			limit_str = "1.50:1 ratio";
			break;
		case MAX17616_OC_LIMIT_1_75:
			limit_str = "1.75:1 ratio";
			break;
		case MAX17616_OC_LIMIT_2_00:
			limit_str = "2.00:1 ratio";
			break;
		default:
			limit_str = "Unknown";
		}
		pr_info("Overcurrent Limit: %s (0x%02X)\n\r", limit_str, (uint8_t)istlim);
	}

	/* Display VOUT UV fault limit configuration */
	enum max17616_nominal_voltage voltage;
	enum max17616_pgood_threshold threshold;
	ret = max17616_get_vout_uv_fault_limit_config(dev, &voltage, &threshold);
	if (ret == 0) {
		const char *voltage_str, *threshold_str;

		switch (voltage) {
		case MAX17616_NOMINAL_5V:
			voltage_str = "5V";
			break;
		case MAX17616_NOMINAL_9V:
			voltage_str = "9V";
			break;
		case MAX17616_NOMINAL_12V:
			voltage_str = "12V";
			break;
		case MAX17616_NOMINAL_24V:
			voltage_str = "24V";
			break;
		case MAX17616_NOMINAL_36V:
			voltage_str = "36V";
			break;
		case MAX17616_NOMINAL_48V:
			voltage_str = "48V";
			break;
		case MAX17616_NOMINAL_60V:
			voltage_str = "60V";
			break;
		case MAX17616_NOMINAL_72V:
			voltage_str = "72V";
			break;
		default:
			voltage_str = "Unknown";
		}

		switch (threshold) {
		case MAX17616_PGOOD_MINUS_10_PERCENT:
			threshold_str = "-10%";
			break;
		case MAX17616_PGOOD_MINUS_20_PERCENT:
			threshold_str = "-20%";
			break;
		case MAX17616_PGOOD_MINUS_30_PERCENT:
			threshold_str = "-30%";
			break;
		default:
			threshold_str = "Unknown";
		}

		pr_info("VOUT UV Fault Limit: %s nominal, %s PGOOD threshold (0x%02X)\n\r",
			voltage_str, threshold_str, ((uint8_t)voltage << 2) | (uint8_t)threshold);
	}

	ret = max17616_get_operation_state(dev, &operation_enabled);
	if (ret == 0)
		pr_info("Operation State: %s\n\r",
			operation_enabled ? "ENABLED" : "DISABLED");

	pr_info("\n\r");
}

/**
 * @brief Display fault status in detail
 * @param dev - MAX17616 device structure
 */
static void display_fault_status(struct max17616_dev *dev)
{
	struct max17616_status status;
	int ret;

	pr_info("=== MAX17616 Status ===\n\r");
	ret = max17616_read_status(dev, &status);
	if (ret) {
		pr_err("Failed to read fault status: %d\n\r", ret);
		pr_info("\n\r");
		return;
	}

	if (status.word == 0) {
		pr_info("No faults detected.\n\r");
		pr_info("\n\r");
		return;
	}

	pr_info("=== FAULT STATUS ===\n\r");
	pr_info("STATUS_WORD: 0x%04X\n\r", status.word);

	/* Check individual fault registers */
	process_fault_bits(status.cml, MAX17616_FAULT_GRP_CML,
			   "CML", max17616_get_fault_description);
	process_fault_bits(status.temperature, MAX17616_FAULT_GRP_TEMPERATURE,
			   "Temperature", max17616_get_fault_description);
	process_fault_bits(status.input, MAX17616_FAULT_GRP_INPUT,
			   "Input", max17616_get_fault_description);
	process_fault_bits(status.iout, MAX17616_FAULT_GRP_IOUT_POUT,
			   "Output Current", max17616_get_fault_description);
	process_fault_bits(status.vout, MAX17616_FAULT_GRP_VOUT,
			   "Output Voltage", max17616_get_fault_description);
	process_fault_bits(status.mfr_specific, MAX17616_FAULT_GRP_MFR_SPECIFIC,
			   "Manufacturer", max17616_get_fault_description);

	pr_info("\n\r");
}

int example_main(void)
{
	struct max17616_dev *max17616_dev;
	struct no_os_uart_desc *uart_desc;
	struct max17616_telemetry telemetry;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	pr_info("\e[2J\e[H");
	pr_info("MAX17616 basic example.\n\r");

	ret = max17616_init(&max17616_dev, &max17616_ip);
	if (ret) {
		pr_err("Failed to initialize MAX17616: %d\n\r", ret);
		goto exit;
	}

	/* Display device settings once */
	display_device_settings(max17616_dev);

	while (1) {
		/* Read telemetry */
		ret = max17616_read_telemetry_all(max17616_dev, &telemetry);
		if (ret)
			pr_err("Failed to read telemetry: %d\n\r", ret);
		else
			display_telemetry(&telemetry);

		/* Check for faults */
		display_fault_status(max17616_dev);

		pr_info("=== Manufacturer Features ===\n\r");

		/* Read current limit mode */
		enum max17616_current_limit_mode clmode;
		ret = max17616_get_current_limit_mode(max17616_dev, &clmode);
		if (ret == 0) {
			const char *mode_str;
			switch (clmode) {
			case MAX17616_CLMODE_LATCH_OFF:
				mode_str = "Latch-off";
				break;
			case MAX17616_CLMODE_CONTINUOUS:
				mode_str = "Continuous";
				break;
			case MAX17616_CLMODE_AUTO_RETRY:
				mode_str = "Auto-retry";
				break;
			default:
				mode_str = "Unknown";
			}
			pr_info("Current Limit Mode: %s (0x%02X)\n\r", mode_str, (uint8_t)clmode);
		}

		/* Example of setting current limit mode */
		/* ret = max17616_set_current_limit_mode(max17616_dev, MAX17616_CLMODE_AUTO_RETRY); */

		/* Example of setting current start ratio (hardware current limit / 2) */
		/* ret = max17616_set_istart_ratio(max17616_dev, MAX17616_ISTART_HALF); */

		/* Example of setting overcurrent timeout */
		/* ret = max17616_set_overcurrent_timeout(max17616_dev, MAX17616_TIMEOUT_4MS); */

		/* Example of setting overcurrent limit ratio */
		/* ret = max17616_set_overcurrent_limit(max17616_dev, MAX17616_OC_LIMIT_1_50); */

		pr_info("\n\r");

		no_os_mdelay(2000);
	}

exit:
	if (ret)
		pr_err("Error %d\n", ret);
	if (max17616_dev)
		max17616_remove(max17616_dev);

	return ret;
}
