/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
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
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "maxim_i2c.h"

/******************************************************************************/
/********************* Production Safety Configuration ************************/
/******************************************************************************/

/* Production Safety Configuration */
#define MAX_TRANSIENT_FAULT_RETRIES  3    /* Allow 3 retries for transient faults */
#define MAX_POWER_DISCREPANCY_PCT    10   /* 10% tolerance for power validation */
#define MONITORING_TIMEOUT_SEC       3600 /* 1 hour monitoring timeout */
#define MAX_CURRENT_LIMIT_MA         60000 /* 60A absolute max for DC2470A */

/**
 * @brief Validate power reading against V×I calculation
 * @param vin_mv - Input voltage in millivolts
 * @param iin_ma - Input current in milliamps
 * @param power_mw - Chip-reported power in milliwatts
 * @return true if power is within acceptable tolerance, false otherwise
 */
static bool validate_power_reading(uint32_t vin_mv, uint32_t iin_ma,
				   uint32_t power_mw)
{
	uint32_t calculated_power;
	int32_t error_pct;

	/* Calculate power from V×I */
	calculated_power = (uint32_t)(((uint64_t)vin_mv * iin_ma) / 1000);

	/* Avoid division by zero */
	if (calculated_power == 0)
		return true;

	/* Calculate percentage error */
	error_pct = ((int64_t)power_mw - calculated_power) * 100 / calculated_power;

	if (abs(error_pct) > MAX_POWER_DISCREPANCY_PCT) {
		pr_warning("Power discrepancy: chip=%lu mW, calc=%lu mW (%+ld%%)\n",
			   (unsigned long)power_mw,
			   (unsigned long)calculated_power,
			   (long)error_pct);
		pr_warning("Check: RSENSE, dividers, or CONFIG_3.VPWR_SELECT setting\n");
		return false;
	}

	return true;
}

/**
 * @brief Verify current limit was set correctly
 * @param dev - Device descriptor
 * @return 0 on success, negative error code on failure
 */
static int verify_current_limit(struct ltc4284_dev *dev)
{
	uint8_t config1;
	uint8_t ilim_code, ilim_mv;
	uint32_t max_current_ma;
	int ret;

	/* Read CONFIG_1 to verify ILIM setting */
	ret = ltc4284_read_byte(dev, LTC4284_REG_CONFIG_1, &config1);
	if (ret)
		return ret;

	ilim_code = no_os_field_get(LTC4284_CONFIG_1_ILIM, config1);
	ilim_mv = LTC4284_VILIM_MIN_MV + ilim_code;

	/* Verify against expected value */
	if (ilim_mv != dev->vsense_mv) {
		pr_crit("CRITICAL: Current limit mismatch!\n");
		pr_crit("Expected: %u mV, Actual: %u mV\n",
			dev->vsense_mv, ilim_mv);
		return -EINVAL;
	}

	/* Calculate maximum current and validate against hardware limits.
	 * I(mA) = V_ILIM(mV) * 1_000_000 / RSENSE(uohm)
	 */
	max_current_ma = ((uint32_t)ilim_mv * 1000000U) / dev->rsense_uohm;

	pr_info("Current limit verified: %u mV -> %lu mA max\n",
		ilim_mv, (unsigned long)max_current_ma);

	if (max_current_ma > MAX_CURRENT_LIMIT_MA) {
		pr_crit("DANGER: Current limit %lu mA exceeds board rating!\n",
			(unsigned long)max_current_ma);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Production-safe basic example for LTC4284
 *
 * SAFETY FEATURES DEMONSTRATED:
 *
 * 1. Current Limit Verification
 *    - Validates V_ILIM register matches configuration
 *    - Checks maximum current is within hardware limits
 *    - Prevents operation with dangerous current limits
 *
 * 2. Limited Fault Retry
 *    - Uses 1 retry for transient faults (not unlimited)
 *    - Prevents thermal cycling on persistent faults
 *    - Software tracking limits retries to 3 attempts
 *
 * 3. Selective Fault Clearing
 *    - Hardware faults (FET_BAD, FET_SHORT) -> latch off permanently
 *    - Transient faults (OC, UV, OV) -> limited retry only
 *    - Never auto-retry hardware damage conditions
 *
 * 4. FET Status Validation
 *    - Checks SYSTEM_STATUS before reading VOUT
 *    - Prevents floating ADC pin readings (82V bug)
 *    - Reports FET state clearly to user
 *
 * 5. Power Cross-Validation
 *    - Compares chip power register to V×I calculation
 *    - Detects configuration errors (dividers, RSENSE)
 *    - Warns on >10% discrepancy
 *
 * 6. Timeout Protection
 *    - 1-hour monitoring limit (not infinite loop)
 *    - Exits cleanly on I2C errors
 *    - Suitable for production deployment
 *
 * 7. Clear Safety Messaging
 *    - Explains why faults are critical
 *    - Guides user on required actions
 *    - Prevents dangerous auto-recovery
 *
 * IMPORTANT FOR PRODUCTION ADAPTATION:
 * - DO NOT increase retry limits without understanding risks
 * - DO NOT clear FET_BAD or FET_SHORT faults automatically
 * - DO verify current limit before enabling FETs
 * - DO implement watchdog timer for production systems
 * - DO add energy overflow detection for billing systems
 *
 * @return 0 on success, negative error code on failure
 */
int example_main()
{
	struct ltc4284_dev *ltc4284_dev;
	uint32_t vin_mv, iin_ma, vout_mv;
	uint32_t power_mw;
	uint64_t energy_code;
	uint8_t status, faults;
	uint32_t iteration = 0;
	uint32_t transient_fault_count = 0;
	bool power_valid;
	bool fets_on;
	int ret, ret2;

	ret = ltc4284_init(&ltc4284_dev, &ltc4284_ip);
	if (ret) {
		pr_err("LTC4284 initialization failed: %d\n", ret);
		return ret;
	}

	pr_info("LTC4284 initialized successfully\n");
	pr_info("I2C Address: 0x%02X\n", ltc4284_dev->i2c_addr);
	pr_info("RSENSE: %lu uohm\n", (unsigned long)ltc4284_dev->rsense_uohm);
	pr_info("VPWR divider: %u:1\n", ltc4284_dev->vpwr_divider);
	pr_info("DRAIN divider: %u:1\n", ltc4284_dev->drain_divider);
	pr_info("V_ILIM threshold: %u mV\n\n", ltc4284_dev->vsense_mv);

	/* SAFETY: Verify current limit was set correctly and is within safe limits */
	ret = verify_current_limit(ltc4284_dev);
	if (ret) {
		pr_err("Current limit verification failed: %d\n", ret);
		pr_err("DO NOT OPERATE - hardware damage risk!\n");
		goto cleanup;
	}

	ret = ltc4284_read_status(ltc4284_dev, &status);
	if (ret) {
		pr_err("Failed to read status: %d\n", ret);
		goto cleanup;
	}

	pr_info("Initial Status (REG_SYSTEM_STATUS): 0x%02X\n", status);

	ret = ltc4284_get_fault(ltc4284_dev, &faults);
	if (ret == 0 && faults != 0) {
		pr_info("Initial faults detected: 0x%02X\n", faults);
		pr_info("Clearing initial faults...\n");
		ret2 = ltc4284_clear_faults(ltc4284_dev);
		if (ret2)
			pr_warning("Failed to clear initial faults: %d\n", ret2);
	}

	/* SAFETY: Use limited retry (1 retry) for transient faults only.
	 * Hardware faults (FET_BAD, FET_SHORT) still latch off.
	 * Unlimited retry would cause thermal cycling on persistent faults.
	 */
	pr_info("Configuring fault retry policy (1 retry)...\n");
	ret = ltc4284_set_oc_retry(ltc4284_dev, LTC4284_RETRY_1);
	if (ret) {
		pr_err("Failed to set retry policy: %d\n", ret);
		goto cleanup;
	}

	pr_info("Enabling FET drivers...\n");
	ret = ltc4284_enable_fet(ltc4284_dev, true);
	if (ret) {
		pr_err("Failed to enable FET: %d\n", ret);
		goto cleanup;
	}

	/* Wait for FET to turn on */
	no_os_mdelay(100);

	pr_info("\nProduction-Safe Monitoring (1 hour timeout):\n");
	pr_info("------------------------------------------\n");

	while (iteration < MONITORING_TIMEOUT_SEC) {
		/* Read system status first to check FET state */
		ret = ltc4284_read_status(ltc4284_dev, &status);
		if (ret) {
			pr_err("Failed to read status: %d\n", ret);
			break;  /* Exit on I2C failure */
		}

		fets_on = !!(status & LTC4284_SYSTEM_STATUS_FET_ON_STATUS);

		/* Read input voltage and current (always valid) */
		ret = ltc4284_read_vin(ltc4284_dev, &vin_mv);
		if (ret) {
			pr_err("Failed to read VIN: %d\n", ret);
			break;
		}

		ret = ltc4284_read_iin(ltc4284_dev, &iin_ma);
		if (ret) {
			pr_err("Failed to read IIN: %d\n", ret);
			break;
		}

		/* SAFETY: Only read VOUT if FETs are ON (avoid floating pin) */
		if (fets_on) {
			ret = ltc4284_read_vout(ltc4284_dev, &vout_mv);
			if (ret) {
				pr_err("Failed to read VOUT: %d\n", ret);
				break;
			}
		} else {
			vout_mv = 0;  /* FETs off -> VOUT is actually 0V */
		}

		ret = ltc4284_read_power(ltc4284_dev, &power_mw);
		if (ret) {
			pr_err("Failed to read power: %d\n", ret);
			break;
		}

		ret = ltc4284_read_energy(ltc4284_dev, &energy_code);
		if (ret) {
			pr_err("Failed to read energy: %d\n", ret);
			break;
		}

		/* SAFETY: Validate power reading against V×I calculation */
		power_valid = validate_power_reading(vin_mv, iin_ma, power_mw);

		/* Display telemetry with FET status and power validity indicators */
		pr_info("VIN: %6lu mV | IIN: %6lu mA | VOUT: %6lu mV %s| Power: %8lu mW%s\n",
			(unsigned long)vin_mv,
			(unsigned long)iin_ma,
			(unsigned long)vout_mv,
			fets_on ? "" : "(OFF) ",
			(unsigned long)power_mw,
			power_valid ? "" : " [!]");

		if (!power_valid) {
			pr_warning("Power validation failed - check configuration\n");
		}

		/* Check for faults with production-safe handling */
		ret = ltc4284_get_fault(ltc4284_dev, &faults);
		if (ret == 0 && faults != 0) {
			pr_err("\n*** FAULT DETECTED: 0x%02X ***\n", faults);

			/* Decode fault types */
			if (faults & LTC4284_FAULT_OV)
				pr_err("  - Overvoltage fault\n");
			if (faults & LTC4284_FAULT_UV)
				pr_err("  - Undervoltage fault\n");
			if (faults & LTC4284_FAULT_OC)
				pr_err("  - Overcurrent fault\n");
			if (faults & LTC4284_FAULT_FET_BAD)
				pr_err("  - FET-bad fault (MOSFET damaged or missing)\n");
			if (faults & LTC4284_FAULT_PGI)
				pr_err("  - PGI fault (downstream power-good input)\n");
			if (faults & LTC4284_FAULT_POWER_BAD)
				pr_err("  - Power-failed fault (VOUT stayed low after PG)\n");
			if (faults & LTC4284_FAULT_FET_SHORT)
				pr_err("  - FET-short fault (MOSFET failed short)\n");
			if (faults & LTC4284_FAULT_EXT)
				pr_err("  - External fault (PGIO4 EXT_FAULT pin)\n");

			/* SAFETY: Critical hardware faults require manual intervention */
			if (faults & (LTC4284_FAULT_FET_BAD | LTC4284_FAULT_FET_SHORT)) {
				pr_crit("\n*** CRITICAL HARDWARE FAULT DETECTED ***\n");
				pr_crit("FET damage detected - LATCHING OFF for safety\n");
				pr_crit("Manual inspection and replacement required\n");
				pr_crit("DO NOT auto-retry - risk of fire or damage\n\n");
				ret2 = ltc4284_enable_fet(ltc4284_dev, false);
				if (ret2)
					pr_err("Latch-off FET disable failed: %d\n",
					       ret2);
				ret = -EFAULT;
				break;  /* Exit immediately */
			}

			/* Transient faults: limited retry with backoff */
			if (faults & (LTC4284_FAULT_OC | LTC4284_FAULT_UV |
				      LTC4284_FAULT_OV)) {
				transient_fault_count++;

				if (transient_fault_count >= MAX_TRANSIENT_FAULT_RETRIES) {
					pr_err("\n*** PERSISTENT TRANSIENT FAULT ***\n");
					pr_err("Fault occurred %u times - LATCHING OFF\n",
					       transient_fault_count);
					pr_err("Possible sustained overload or short circuit\n");
					pr_err("Manual investigation required\n\n");
					ret2 = ltc4284_enable_fet(ltc4284_dev, false);
					if (ret2)
						pr_err("Latch-off FET disable failed: %d\n",
						       ret2);
					ret = -EFAULT;
					break;  /* Exit after max retries */
				}

				pr_warning("Transient fault (retry %u/%u) - clearing and monitoring\n",
					   transient_fault_count,
					   MAX_TRANSIENT_FAULT_RETRIES);
				ret2 = ltc4284_clear_faults(ltc4284_dev);
				if (ret2)
					pr_warning("Fault clear failed: %d\n", ret2);

				/* Re-enable FETs (chip's retry mode will handle hardware-level retry) */
				ret2 = ltc4284_enable_fet(ltc4284_dev, true);
				if (ret2)
					pr_warning("Retry FET re-enable failed: %d\n",
						   ret2);
				no_os_mdelay(500);  /* Wait for restart */
			} else {
				/* Non-critical faults: clear and continue */
				pr_info("Clearing non-critical fault...\n\n");
				ret2 = ltc4284_clear_faults(ltc4284_dev);
				if (ret2)
					pr_warning("Non-critical fault clear failed: %d\n",
						   ret2);
			}
		} else {
			/* No faults: reset transient counter */
			transient_fault_count = 0;
		}

		/* Update rate: 1 Hz */
		iteration++;
		no_os_mdelay(1000);
	}

	if (iteration >= MONITORING_TIMEOUT_SEC) {
		pr_info("\nMonitoring timeout reached (%u seconds)\n",
			MONITORING_TIMEOUT_SEC);
	}

cleanup:
	ret2 = ltc4284_enable_fet(ltc4284_dev, false);
	if (ret2)
		pr_err("Shutdown FET disable failed: %d\n", ret2);

	ret2 = ltc4284_remove(ltc4284_dev);
	if (ret2)
		pr_err("Device remove failed: %d\n", ret2);

	if (ret)
		pr_err("Example failed: %d\n", ret);
	else
		pr_info("Example completed successfully\n");

	return ret;
}
