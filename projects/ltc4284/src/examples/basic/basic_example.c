/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for LTC4284 project
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
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

#include <stdlib.h>
#include "common_data.h"
#include "ltc4284.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Basic example main execution
 * @return 0 in case of success, negative error code otherwise
 */
int example_main()
{
	struct ltc4284_dev *ltc4284_dev;
	uint16_t vin_mv, iin_ma, vout_mv;
	uint32_t power_mw;
	uint64_t energy_mj;
	int16_t temp_mc;
	uint8_t status, faults;
	int ret;

	pr_info("LTC4284 Basic Example\n");
	pr_info("=====================\n\n");

	ret = ltc4284_init(&ltc4284_dev, &ltc4284_ip);
	if (ret) {
		pr_err("LTC4284 initialization failed: %d\n", ret);
		return ret;
	}

	pr_info("LTC4284 initialized successfully\n");
	pr_info("I2C Address: 0x%02X\n", ltc4284_dev->i2c_addr);
	pr_info("RSENSE: %u mohm\n", ltc4284_dev->rsense_mohm);
	pr_info("VSENSE: %u mV\n\n", ltc4284_dev->vsense_mv);

	ret = ltc4284_read_status(ltc4284_dev, &status);
	if (ret) {
		pr_err("Failed to read status: %d\n", ret);
		goto cleanup;
	}

	pr_info("Initial Status: 0x%02X\n", status);

	ret = ltc4284_get_fault(ltc4284_dev, &faults);
	if (ret == 0 && faults != 0) {
		pr_info("Initial faults detected: 0x%02X\n", faults);
		pr_info("Clearing initial faults...\n");
		ltc4284_clear_faults(ltc4284_dev);
	}

	pr_info("Enabling FET drivers...\n");
	ret = ltc4284_enable_fet(ltc4284_dev, true);
	if (ret) {
		pr_err("Failed to enable FET: %d\n", ret);
		goto cleanup;
	}

	/* Wait for FET to turn on */
	no_os_mdelay(100);

	pr_info("\nContinuous Monitoring (Ctrl+C to stop):\n");
	pr_info("------------------------------------------\n");

	while (1) {
		ret = ltc4284_read_vin(ltc4284_dev, &vin_mv);
		if (ret) {
			pr_err("Failed to read VIN: %d\n", ret);
			goto cleanup;
		}

		ret = ltc4284_read_vout(ltc4284_dev, &vout_mv);
		if (ret) {
			pr_err("Failed to read VOUT: %d\n", ret);
			goto cleanup;
		}

		ret = ltc4284_read_iin(ltc4284_dev, &iin_ma);
		if (ret) {
			pr_err("Failed to read IIN: %d\n", ret);
			goto cleanup;
		}

		ret = ltc4284_read_power(ltc4284_dev, &power_mw);
		if (ret) {
			pr_err("Failed to read power: %d\n", ret);
			goto cleanup;
		}

		ret = ltc4284_read_energy(ltc4284_dev, &energy_mj);
		if (ret) {
			pr_err("Failed to read energy: %d\n", ret);
			goto cleanup;
		}

		ret = ltc4284_read_temperature(ltc4284_dev, &temp_mc);
		if (ret) {
			pr_err("Failed to read temperature: %d\n", ret);
			goto cleanup;
		}

		pr_info("VIN: %5u mV | IIN: %5u mA | VOUT: %5u mV | ",
			vin_mv, iin_ma, vout_mv);
		pr_info("Power: %6u mW | Energy: %10llu mJ | ",
			power_mw, (unsigned long long)energy_mj);
		pr_info("Temp: %d.%03d C\n",
			temp_mc / 1000, abs(temp_mc % 1000));

		ret = ltc4284_get_fault(ltc4284_dev, &faults);
		if (ret == 0 && faults != 0) {
			pr_err("\n*** FAULT DETECTED: 0x%02X ***\n", faults);
			if (faults & LTC4284_FAULT_OV)
				pr_err("  - Overvoltage fault\n");
			if (faults & LTC4284_FAULT_UV)
				pr_err("  - Undervoltage fault\n");
			if (faults & LTC4284_FAULT_OC)
				pr_err("  - Overcurrent fault\n");
			if (faults & LTC4284_FAULT_POWER_BAD)
				pr_err("  - Power bad fault\n");
			if (faults & LTC4284_FAULT_FET)
				pr_err("  - FET fault\n");
			if (faults & LTC4284_FAULT_OT)
				pr_err("  - Over-temperature fault\n");

			pr_info("Clearing faults...\n\n");
			ltc4284_clear_faults(ltc4284_dev);
		}

		/* Update rate: 1 Hz */
		no_os_mdelay(1000);
	}

cleanup:
	ltc4284_enable_fet(ltc4284_dev, false);
	ltc4284_remove(ltc4284_dev);

	if (ret)
		pr_err("Example failed: %d\n", ret);
	else
		pr_info("Example completed successfully\n");

	return ret;
}
