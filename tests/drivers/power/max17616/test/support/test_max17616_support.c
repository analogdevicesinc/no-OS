/***************************************************************************//**
 *   @file   test_max17616_support.c
 *   @brief  Support functions for MAX17616 testing
 *   @author Carlos Jones (carlosjr.jones@analog.com)
 *******************************************************************************
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
 ******************************************************************************/
#include "unity.h"
#include "max17616.h"
#include "test_max17616_support.h"
#include <string.h>

/**
 * @brief Setup test environment for MAX17616 driver tests
 */
void max17616_test_setup(void)
{
	/* Initialize test environment */
	/* No specific setup needed for basic driver tests */
}

/**
 * @brief Teardown test environment for MAX17616 driver tests
 */
void max17616_test_teardown(void)
{
	/* Clean up test environment */
	/* No specific cleanup needed for basic driver tests */
}

/**
 * @brief Setup mock device for testing
 * @param device - Device structure to initialize
 * @param i2c_desc - I2C descriptor to use
 * @param chip_id - Chip ID to set
 */
void max17616_test_device_setup(struct max17616_dev *device,
				struct no_os_i2c_desc *i2c_desc,
				enum max17616_chip_id chip_id)
{
	if (device && i2c_desc) {
		memset(device, 0, sizeof(*device));
		device->i2c_desc = i2c_desc;
		device->chip_id = chip_id;
		device->chip_info = max17616_test_get_chip_info();
	}
}

/**
 * @brief Validate telemetry structure
 * @param telemetry - Telemetry structure to validate
 * @return true if valid, false otherwise
 */
bool max17616_test_validate_telemetry(const struct max17616_telemetry
				      *telemetry)
{
	if (!telemetry) {
		return false;
	}

	/* Check if at least one value is valid */
	if (telemetry->valid_mask == 0) {
		return false;
	}

	/* Check reasonable ranges for valid values */
	if (telemetry->valid_mask & 0x01) { /* VIN */
		if (telemetry->vin < 0 || telemetry->vin > 100000) { /* 0-100V */
			return false;
		}
	}

	if (telemetry->valid_mask & 0x02) { /* VOUT */
		if (telemetry->vout < 0 || telemetry->vout > 50000) { /* 0-50V */
			return false;
		}
	}

	if (telemetry->valid_mask & 0x08) { /* IOUT */
		if (telemetry->iout < 0 || telemetry->iout > 100000) { /* 0-100A */
			return false;
		}
	}

	return true;
}

/**
 * @brief Create test chip info structure
 * @return Pointer to test chip info
 */
const struct max17616_chip_info *max17616_test_get_chip_info(void)
{
	static const struct max17616_chip_specific_info
		specific_info[ID_MAX17616_CHIP_COUNT] = {
		[ID_MAX17616] = {
			.ic_dev_id = "MAX17616",
			.ic_dev_id_size = 8,
		},
		[ID_MAX17616A] = {
			.ic_dev_id = "MAX17616A",
			.ic_dev_id_size = 9,
		}
	};

	static const struct max17616_chip_info chip_info = {
		.mfr_id = "MAXIM",
		.mfr_id_size = 5,
		.mfr_rev = "01",
		.mfr_rev_size = 2,
		.pmbus_rev = 0x33,
		.specific_info = {
			[ID_MAX17616] = {
				.ic_dev_id = "MAX17616",
				.ic_dev_id_size = 8,
			},
			[ID_MAX17616A] = {
				.ic_dev_id = "MAX17616A",
				.ic_dev_id_size = 9,
			}
		}
	};

	return &chip_info;
}

/**
 * @brief Get test manufacturer ID string
 * @return Pointer to test manufacturer ID
 */
const char *max17616_test_get_mfr_id(void)
{
	return "MAXIM";
}

/**
 * @brief Get test device ID string
 * @param chip_id - Chip ID to get device string for
 * @return Pointer to test device ID string
 */
const char *max17616_test_get_device_id(enum max17616_chip_id chip_id)
{
	switch (chip_id) {
	case ID_MAX17616:
		return "MAX17616";
	case ID_MAX17616A:
		return "MAX17616A";
	default:
		return "UNKNOWN";
	}
}

/**
 * @brief Initialize a test MAX17616 device structure with default values
 * @param dev - Device structure to initialize
 * @param i2c_desc - I2C descriptor to use
 */
void max17616_test_init_device(struct max17616_dev *dev,
			       struct no_os_i2c_desc *i2c_desc)
{
	memset(dev, 0, sizeof(*dev));
	dev->i2c_desc = i2c_desc;
	dev->chip_id = ID_MAX17616;
}

/**
 * @brief Initialize a test MAX17616 init parameter structure with default values
 * @param init_param - Init parameter structure to initialize
 * @param i2c_init - I2C init parameter to use
 */
void max17616_test_init_param(struct max17616_init_param *init_param,
			      struct no_os_i2c_init_param *i2c_init)
{
	memset(init_param, 0, sizeof(*init_param));
	init_param->i2c_init = i2c_init;
	init_param->chip_id = ID_MAX17616;
}

/**
 * @brief Initialize a test I2C descriptor with default values
 * @param i2c_desc - I2C descriptor to initialize
 */
void max17616_test_init_i2c_desc(struct no_os_i2c_desc *i2c_desc)
{
	memset(i2c_desc, 0, sizeof(*i2c_desc));
	i2c_desc->device_id = 0;
	i2c_desc->slave_address = 0x36;
	i2c_desc->max_speed_hz = 400000;
}

/**
 * @brief Initialize a test I2C init parameter with default values
 * @param i2c_init - I2C init parameter to initialize
 */
void max17616_test_init_i2c_init(struct no_os_i2c_init_param *i2c_init)
{
	memset(i2c_init, 0, sizeof(*i2c_init));
	i2c_init->device_id = 0;
	i2c_init->slave_address = 0x36;
	i2c_init->max_speed_hz = 400000;
}

/**
 * @brief Create expected manufacturer ID data for testing
 * @param data - Buffer to fill with expected data
 * @param size - Size of the buffer (should be at least 7 bytes)
 */
void max17616_test_create_mfr_id_data(uint8_t *data, size_t size)
{
	if (size >= 7) {
		data[0] = 6;  // Length byte
		data[1] = 'M';
		data[2] = 'A';
		data[3] = 'X';
		data[4] = 'I';
		data[5] = 'M';
		data[6] = 0;  // Null terminator
	}
}

/**
 * @brief Create expected manufacturer revision data for testing
 * @param data - Buffer to fill with expected data
 * @param size - Size of the buffer (should be at least 3 bytes)
 */
void max17616_test_create_mfr_rev_data(uint8_t *data, size_t size)
{
	if (size >= 3) {
		data[0] = 2;  // Length byte
		data[1] = '0';
		data[2] = '1';
	}
}

/**
 * @brief Create expected device ID data for testing
 * @param data - Buffer to fill with expected data
 * @param size - Size of the buffer (should be at least 11 bytes)
 * @param chip_id - Chip ID to create data for
 */
void max17616_test_create_dev_id_data(uint8_t *data, size_t size,
				      enum max17616_chip_id chip_id)
{
	if (size >= 11) {
		data[0] = 10;  // Length byte
		if (chip_id == ID_MAX17616) {
			memcpy(&data[1], "MAX17616", 8);
			data[9] = 0;
			data[10] = 0;
		} else if (chip_id == ID_MAX17616A) {
			memcpy(&data[1], "MAX17616A", 9);
			data[10] = 0;
		}
	}
}

/**
 * @brief Create expected PMBus revision data for testing
 * @param data - Buffer to fill with expected data
 * @param size - Size of the buffer (should be at least 2 bytes)
 */
void max17616_test_create_pmbus_rev_data(uint8_t *data, size_t size)
{
	if (size >= 2) {
		data[0] = 0x33;  // PMBus revision 1.3
		data[1] = 0x00;
	}
}

/*******************************************************************************
 *    PMBus Test Support Functions
 ******************************************************************************/

/**
 * @brief PMBus test setup function
 */
void max17616_pmbus_test_setup(void)
{
	/* Initialize test environment for PMBus testing */
	/* No specific setup needed */
}

/**
 * @brief PMBus test teardown function
 */
void max17616_pmbus_test_teardown(void)
{
	/* Clean up test environment */
	/* No specific cleanup needed */
}

/**
 * @brief PMBus device setup function
 */
void max17616_pmbus_test_device_setup(void)
{
	/* Setup device for testing */
	/* No specific setup needed */
}

/**
 * @brief PMBus device initialization function
 */
void max17616_pmbus_test_init_device(void)
{
	/* Initialize device for testing */
	/* No specific initialization needed */
}

/**
 * @brief Verify chip info for MAX17616
 */
int max17616_verify_chip_info_test(struct max17616_dev *dev)
{
	/* Return success for chip verification */
	(void)dev; /* Unused parameter */
	return 0;
}

/*******************************************************************************
 *    Math Support Functions
 ******************************************************************************/

/**
 * @brief Integer power function for tests
 * @param base - Base value
 * @param exp - Exponent value
 * @return base raised to the power of exp
 */
int test_int_pow(int base, int exp)
{
	int result = 1;

	if (exp < 0) {
		/* For negative exponents, return 0 for integer math */
		return 0;
	}

	while (exp > 0) {
		if (exp & 1) {
			result *= base;
		}
		base *= base;
		exp >>= 1;
	}

	return result;
}

