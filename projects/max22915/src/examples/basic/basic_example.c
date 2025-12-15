/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for MAX22915 project
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
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

#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "common_data.h"
#include "max22915.h"

struct max22915_faults {
	uint8_t thermal_overload;
	uint8_t current_limit;
	uint8_t open_wire_off;
	uint8_t open_wire_on;
	uint8_t short_to_vdd;
	bool any_fault_present;
};

/***************************************************************************//**
 * @brief Read fault status from MAX22915 registers
 * @param dev - MAX22915 device descriptor
 * @param faults - Pointer to structure to store fault information
 * @return 0 on success, negative error code on failure
 *******************************************************************************/
static int max22915_read_faults(struct max22915_desc *dev,
				struct max22915_faults *faults)
{
	uint8_t reg_val;
	int ret;

	if (!dev || !faults)
		return -EINVAL;

	ret = max22915_reg_read(dev, MAX22915_REG_OVL_FLT, &reg_val);
	if (ret)
		return ret;
	faults->thermal_overload = reg_val;

	ret = max22915_reg_read(dev, MAX22915_REG_CL_FLT, &reg_val);
	if (ret)
		return ret;
	faults->current_limit = reg_val;

	ret = max22915_reg_read(dev, MAX22915_REG_OWOFF_FLT, &reg_val);
	if (ret)
		return ret;
	faults->open_wire_off = reg_val;

	ret = max22915_reg_read(dev, MAX22915_REG_OWON_FLT, &reg_val);
	if (ret)
		return ret;
	faults->open_wire_on = reg_val;

	ret = max22915_reg_read(dev, MAX22915_REG_SHTVDD_FLT, &reg_val);
	if (ret)
		return ret;
	faults->short_to_vdd = reg_val;

	faults->any_fault_present = (faults->thermal_overload || faults->current_limit
				     ||
				     faults->open_wire_off || faults->open_wire_on ||
				     faults->short_to_vdd);

	return 0;
}

/***************************************************************************//**
 * @brief Run diagnostics and update FLED/SLED states accordingly
 * @param dev - MAX22915 device descriptor
 * @return 0 in case of success, negative error code otherwise
 *******************************************************************************/
static int max22915_run_diagnostics(struct max22915_desc *dev)
{
	struct max22915_faults faults = {0};
	uint8_t config1_reg;
	bool auto_fled_mode;
	bool auto_sled_mode;
	uint8_t fled_bits = 0;
	uint8_t sled_bits = 0;
	bool channel_has_fault;
	int ret;
	int ch;

	if (!dev)
		return -EINVAL;

	ret = max22915_read_faults(dev, &faults);
	if (ret)
		return ret;

	/* Report any faults detected */
	if (faults.any_fault_present) {
		pr_info("Faults detected - ");
		if (faults.thermal_overload)
			pr_info("Thermal:0x%02X ", faults.thermal_overload);
		if (faults.current_limit)
			pr_info("Current:0x%02X ", faults.current_limit);
		if (faults.open_wire_off)
			pr_info("OpenOff:0x%02X ", faults.open_wire_off);
		if (faults.open_wire_on)
			pr_info("OpenOn:0x%02X ", faults.open_wire_on);
		if (faults.short_to_vdd)
			pr_info("ShortVDD:0x%02X ", faults.short_to_vdd);
		pr_info("\r\n");
	}

	ret = max22915_reg_read(dev, MAX22915_REG_CONFIG1, &config1_reg);
	if (ret) {
		pr_err("Failed to read CONFIG1 register\r\n");
		return ret;
	}

	auto_fled_mode = (config1_reg & MAX22915_FLED_SET_MASK) == 0;
	auto_sled_mode = (config1_reg & MAX22915_SLED_SET_MASK) == 0;

	if (!auto_fled_mode || !auto_sled_mode) {
		fled_bits = 0;
		sled_bits = 0;

		for (ch = 0; ch < MAX22915_MAX_CHANNELS; ch++) {
			channel_has_fault = ((faults.thermal_overload & MAX22915_CH_MASK(ch)) ||
					     (faults.current_limit & MAX22915_CH_MASK(ch)) ||
					     (faults.open_wire_off & MAX22915_CH_MASK(ch)) ||
					     (faults.open_wire_on & MAX22915_CH_MASK(ch)) ||
					     (faults.short_to_vdd & MAX22915_CH_MASK(ch)));

			if (channel_has_fault)
				fled_bits |= MAX22915_CH_MASK(ch);

			if ((!channel_has_fault) && (dev->sw_state[ch] == SWITCH_CLOSE))
				sled_bits |= MAX22915_CH_MASK(ch);
		}

		if (!auto_fled_mode) {
			ret = max22915_reg_write(dev, MAX22915_REG_SET_FLED, fled_bits);
			if (ret) {
				pr_err("Failed to write FLED register\r\n");
				return ret;
			}
		}

		if (!auto_sled_mode) {
			ret = max22915_reg_write(dev, MAX22915_REG_SET_SLED, sled_bits);
			if (ret) {
				pr_err("Failed to write SLED register\r\n");
				return ret;
			}
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief Basic example main execution.
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct max22915_desc *dev = NULL;
	struct no_os_uart_desc *uart_desc = NULL;

	ret = no_os_uart_init(&uart_desc, &max22915_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	pr_info("max22915 Basic Example with Diagnostics\r\n");

	ret = max22915_init(&dev, &max22915_ip);
	if (ret) {
		pr_info("max22915 initialization failed\r\n");
		goto error;
	}

	pr_info("Device initialized successfully\r\n");
	pr_info("Starting continuous diagnostics..\r\n");

	while (1) {
		ret = max22915_run_diagnostics(dev);
		if (ret) {
			pr_err("Diagnostics failed\r\n");
			goto error;
		}

		pr_info("Diagnostics cycle complete\r\n");

		no_os_mdelay(1000);
	}

error:
	if (dev)
		max22915_remove(dev);
	if (uart_desc)
		no_os_uart_remove(uart_desc);
	return ret;
}
