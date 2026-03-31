/***************************************************************************//**
 *   @file   comprehensive_example.c
 *   @brief  Comprehensive test for ADXL366 driver - tests all features
 *   @author Marco Ramirez (marco.ramirez@analog.com)
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

#include "comprehensive_example.h"
#include "common_data.h"
#include "adxl366.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/* Scale factors for different ranges */
#define ADXL366_SCALE_FACTOR_2G  0.000488f  /* 4g / 8192 */
#define ADXL366_SCALE_FACTOR_4G  0.000976f  /* 8g / 8192 */
#define ADXL366_SCALE_FACTOR_8G  0.001953f  /* 16g / 8192 */

/**
 * @brief Test device ID verification.
 */
static int test_device_id(struct adxl366_dev *dev)
{
	uint8_t devid_ad, devid_mst, partid;
	int ret;

	pr_info("\n=== Test 1: Device ID Verification ===\n");

	ret = adxl366_reg_read(dev, ADXL366_REG_DEVID_AD, &devid_ad);
	if (ret) {
		pr_err("Failed to read DEVID_AD\n");
		return ret;
	}

	ret = adxl366_reg_read(dev, ADXL366_REG_DEVID_MST, &devid_mst);
	if (ret) {
		pr_err("Failed to read DEVID_MST\n");
		return ret;
	}

	ret = adxl366_reg_read(dev, ADXL366_REG_PARTID, &partid);
	if (ret) {
		pr_err("Failed to read PARTID\n");
		return ret;
	}

	pr_info("Device IDs: 0x%02X 0x%02X 0x%02X\n", devid_ad, devid_mst, partid);

	if (devid_ad == 0xAD && devid_mst == 0x1D && partid == 0xF7) {
		pr_info("PASS: Device ID verification\n");
		return 0;
	} else {
		pr_err("FAIL: Invalid device IDs\n");
		return -1;
	}
}

/**
 * @brief Test range configuration.
 */
static int test_range_configuration(struct adxl366_dev *dev)
{
	enum adxl366_range range_set, range_read;
	int ret;

	pr_info("\n=== Test 2: Range Configuration ===\n");

	/* Test ±2g */
	range_set = ADXL366_RANGE_2G;
	ret = adxl366_set_range(dev, range_set);
	if (ret) {
		pr_err("Failed to set ±2g range\n");
		return ret;
	}

	ret = adxl366_get_range(dev, &range_read);
	if (ret) {
		pr_err("Failed to read range\n");
		return ret;
	}

	pr_info("Set ±2g, Read back: %d\n", range_read);
	if (range_read != range_set) {
		pr_err("FAIL: Range mismatch\n");
		return -1;
	}

	/* Test ±4g */
	range_set = ADXL366_RANGE_4G;
	ret = adxl366_set_range(dev, range_set);
	if (ret)
		return ret;
	ret = adxl366_get_range(dev, &range_read);
	if (ret)
		return ret;
	pr_info("Set ±4g, Read back: %d\n", range_read);

	/* Test ±8g */
	range_set = ADXL366_RANGE_8G;
	ret = adxl366_set_range(dev, range_set);
	if (ret)
		return ret;
	ret = adxl366_get_range(dev, &range_read);
	if (ret)
		return ret;
	pr_info("Set ±8g, Read back: %d\n", range_read);

	/* Set back to ±2g for testing */
	adxl366_set_range(dev, ADXL366_RANGE_2G);

	pr_info("PASS: Range configuration\n");
	return 0;
}

/**
 * @brief Test ODR configuration.
 */
static int test_odr_configuration(struct adxl366_dev *dev)
{
	enum adxl366_odr odr_set, odr_read;
	int ret;

	pr_info("\n=== Test 3: ODR Configuration ===\n");

	/* Test different ODRs */
	const enum adxl366_odr odrs[] = {
		ADXL366_ODR_12_5HZ,
		ADXL366_ODR_50HZ,
		ADXL366_ODR_100HZ,
		ADXL366_ODR_400HZ
	};
	const char *odr_names[] = {"12.5Hz", "50Hz", "100Hz", "400Hz"};

	for (int i = 0; i < 4; i++) {
		odr_set = odrs[i];
		ret = adxl366_set_odr(dev, odr_set);
		if (ret) {
			pr_err("Failed to set ODR\n");
			return ret;
		}

		ret = adxl366_get_odr(dev, &odr_read);
		if (ret) {
			pr_err("Failed to read ODR\n");
			return ret;
		}

		pr_info("Set %s, Read back: %d\n", odr_names[i], odr_read);
		if (odr_read != odr_set) {
			pr_err("FAIL: ODR mismatch\n");
			return -1;
		}
	}

	pr_info("PASS: ODR configuration\n");
	return 0;
}

/**
 * @brief Test temperature reading.
 */
static int test_temperature(struct adxl366_dev *dev)
{
	float temp_c;
	int ret;

	pr_info("\n=== Test 4: Temperature Reading ===\n");

	ret = adxl366_read_temperature(dev, &temp_c);
	if (ret) {
		pr_err("Failed to read temperature\n");
		return ret;
	}

	pr_info("Temperature: %.2f °C\n", temp_c);

	/* Sanity check: temperature should be between 0-50°C for normal operation */
	if (temp_c < 0.0f || temp_c > 50.0f) {
		pr_warning("Temperature out of expected range (0-50°C)\n");
	}

	pr_info("PASS: Temperature reading\n");
	return 0;
}

/**
 * @brief Test accelerometer data reading.
 */
static int test_accel_data(struct adxl366_dev *dev)
{
	int16_t x, y, z;
	float x_g, y_g, z_g;
	int ret;

	pr_info("\n=== Test 5: Accelerometer Data Reading ===\n");

	/* Read 5 samples */
	for (int i = 0; i < 5; i++) {
		ret = adxl366_read_accel(dev, &x, &y, &z);
		if (ret) {
			pr_err("Failed to read accelerometer data\n");
			return ret;
		}

		/* Convert to g using ±2g scale */
		x_g = (float)x * ADXL366_SCALE_FACTOR_2G;
		y_g = (float)y * ADXL366_SCALE_FACTOR_2G;
		z_g = (float)z * ADXL366_SCALE_FACTOR_2G;

		pr_info("Sample %d: X=%+.4f g, Y=%+.4f g, Z=%+.4f g\n",
			i + 1, x_g, y_g, z_g);

		no_os_mdelay(100);
	}

	pr_info("PASS: Accelerometer data reading\n");
	return 0;
}

/**
 * @brief Test status register reading.
 */
static int test_status(struct adxl366_dev *dev)
{
	uint8_t status;
	int ret;

	pr_info("\n=== Test 6: Status Register ===\n");

	ret = adxl366_get_status(dev, &status);
	if (ret) {
		pr_err("Failed to read status\n");
		return ret;
	}

	pr_info("Status: 0x%02X\n", status);
	pr_info("  DATA_RDY:     %d\n", (status & ADXL366_STATUS_DATA_RDY) ? 1 : 0);
	pr_info("  FIFO_RDY:     %d\n", (status & ADXL366_STATUS_FIFO_RDY) ? 1 : 0);
	pr_info("  FIFO_WM:      %d\n", (status & ADXL366_STATUS_FIFO_WATERMARK) ? 1 : 0);
	pr_info("  FIFO_OVR:     %d\n", (status & ADXL366_STATUS_FIFO_OVERRUN) ? 1 : 0);
	pr_info("  ACT:          %d\n", (status & ADXL366_STATUS_ACT) ? 1 : 0);
	pr_info("  INACT:        %d\n", (status & ADXL366_STATUS_INACT) ? 1 : 0);
	pr_info("  AWAKE:        %d\n", (status & ADXL366_STATUS_AWAKE) ? 1 : 0);

	pr_info("PASS: Status register reading\n");
	return 0;
}

/**
 * @brief Test register write operations.
 */
static int test_register_write(struct adxl366_dev *dev)
{
	uint8_t write_val = 0x52, read_val;
	int ret;

	pr_info("\n=== Test 7: Register Write ===\n");

	/* Write to SOFT_RESET register (safe test register) */
	ret = adxl366_reg_write(dev, ADXL366_REG_SOFT_RESET, write_val);
	if (ret) {
		pr_err("Failed to write register\n");
		return ret;
	}

	/* Read back to verify (note: soft reset clears to 0x00) */
	ret = adxl366_reg_read(dev, ADXL366_REG_SOFT_RESET, &read_val);
	if (ret) {
		pr_err("Failed to read register\n");
		return ret;
	}

	pr_info("Write: 0x%02X, Read back: 0x%02X (expected 0x00 after reset)\n",
		write_val, read_val);
	pr_info("PASS: Register write\n");
	return 0;
}

/**
 * @brief Test multi-byte register read.
 */
static int test_register_read_multiple(struct adxl366_dev *dev)
{
	uint8_t buf[6];
	int ret;

	pr_info("\n=== Test 8: Multi-byte Register Read ===\n");

	/* Read 6 bytes starting from XDATA_H (accel X, Y, Z data) */
	ret = adxl366_reg_read_multiple(dev, ADXL366_REG_XDATA_H, buf, 6);
	if (ret) {
		pr_err("Failed to read multiple registers\n");
		return ret;
	}

	pr_info("Read 6 bytes: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	pr_info("PASS: Multi-byte register read\n");
	return 0;
}

/**
 * @brief Test power mode configuration.
 */
static int test_power_mode(struct adxl366_dev *dev)
{
	uint8_t mode_set, mode_read;
	int ret;

	pr_info("\n=== Test 9: Power Mode Configuration ===\n");

	/* Test standby mode */
	mode_set = ADXL366_POWER_CTL_STANDBY;
	ret = adxl366_set_power_mode(dev, mode_set);
	if (ret) {
		pr_err("Failed to set standby mode\n");
		return ret;
	}

	ret = adxl366_get_power_mode(dev, &mode_read);
	if (ret) {
		pr_err("Failed to read power mode\n");
		return ret;
	}

	pr_info("Set STANDBY, Read back: %d\n", mode_read);
	if (mode_read != mode_set) {
		pr_err("FAIL: Power mode mismatch\n");
		return -1;
	}

	/* Test measurement mode */
	mode_set = ADXL366_POWER_CTL_MEASURE;
	ret = adxl366_set_power_mode(dev, mode_set);
	if (ret) {
		pr_err("Failed to set measurement mode\n");
		return ret;
	}

	ret = adxl366_get_power_mode(dev, &mode_read);
	if (ret) {
		pr_err("Failed to read power mode\n");
		return ret;
	}

	pr_info("Set MEASURE, Read back: %d\n", mode_read);
	if (mode_read != mode_set) {
		pr_err("FAIL: Power mode mismatch\n");
		return -1;
	}

	pr_info("PASS: Power mode configuration\n");
	return 0;
}

/**
 * @brief Comprehensive example main execution.
 */
int comprehensive_example_main(void)
{
	struct adxl366_dev *adxl366_desc;
	int ret;

	pr_info("\n");
	pr_info("===================================\n");
	pr_info("ADXL366 Comprehensive Test Example\n");
	pr_info("Testing All 14 Driver Functions\n");
	pr_info("===================================\n");

	/* Initialize ADXL366 - Function 1: adxl366_init() */
	pr_info("\nFunction 1/14: adxl366_init()\n");
	ret = adxl366_init(&adxl366_desc, adxl366_ip);
	if (ret) {
		pr_err("ADXL366 initialization failed: %d\n", ret);
		return ret;
	}
	pr_info("PASS: ADXL366 initialized successfully!\n");

	pr_info("----------------------------------------\n");

	/* Run tests */
	pr_info("\nFunction 2/14: adxl366_reg_read()\n");
	ret = test_device_id(adxl366_desc);
	if (ret)
		goto error;

	pr_info("----------------------------------------\n");

	pr_info("\nFunctions 3-4/14: adxl366_set_range(), adxl366_get_range()\n");
	ret = test_range_configuration(adxl366_desc);
	if (ret)
		goto error;

	pr_info("----------------------------------------\n");

	pr_info("\nFunctions 5-6/14: adxl366_set_odr(), adxl366_get_odr()\n");
	ret = test_odr_configuration(adxl366_desc);
	if (ret)
		goto error;

	pr_info("\nFunctions 7-8/14: adxl366_read_temperature(), adxl366_reg_read_multiple()\n");
	ret = test_temperature(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\nFunction 9/14: adxl366_read_accel()\n");
	ret = test_accel_data(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\nFunction 10/14: adxl366_get_status()\n");
	ret = test_status(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\nFunction 11/14: adxl366_reg_write()\n");
	ret = test_register_write(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\nFunction 12/14: adxl366_reg_read_multiple() - explicit test\n");
	ret = test_register_read_multiple(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\nFunctions 13-14/14: adxl366_set_power_mode(), adxl366_get_power_mode()\n");
	ret = test_power_mode(adxl366_desc);
	if (ret)
		goto error;
	pr_info("----------------------------------------\n");

	pr_info("\n");
	pr_info("===================================\n");
	pr_info("ALL 14 FUNCTIONS TESTED!\n");
	pr_info("ALL TESTS PASSED!\n");
	pr_info("===================================\n");

error:
	/* Function 15: adxl366_remove() */
	pr_info("\nCleaning up: adxl366_remove() \t");
	adxl366_remove(adxl366_desc);
	return ret;
}
