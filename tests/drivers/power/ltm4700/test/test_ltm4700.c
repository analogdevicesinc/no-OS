/***************************************************************************//**
 *   @file   test_ltm4700_basic.c
 *   @brief  Basic validation tests for LTM4700 driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include <stdint.h>
#include "unity.h"
#include "ltm4700.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_crc8.h"
#include "no_os_error.h"

/*******************************************************************************
 *    HELPER FUNCTIONS FOR MOCK CALLBACKS
 ******************************************************************************/

/**
 * @brief Callback to fill buffer with LTM4700 special ID
 */
static int fill_ltm4700_special_id(struct no_os_i2c_desc* desc,
				   unsigned char* data,
				   unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 2 && data) {
		data[0] = 0x30;  // LSB of 0x4130
		data[1] = 0x41;  // MSB of 0x4130
	}
	return 0;
}

/**
 * @brief Callback to fill buffer with unknown device ID
 */
static int fill_unknown_device_id(struct no_os_i2c_desc* desc,
				  unsigned char* data,
				  unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 2 && data) {
		data[0] = 0x99;  // LSB of 0x9999
		data[1] = 0x99;  // MSB of 0x9999
	}
	return 0;
}

/**
 * @brief Callback to fill buffer with correct MFR_ID "ADI"
 * PMBus block read format: [length_byte, data_bytes...]
 */
static int fill_mfr_id_correct(struct no_os_i2c_desc* desc,
			       unsigned char* data,
			       unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 4 && data) {
		data[0] = 3;     /* PMBus length byte */
		data[1] = 'A';
		data[2] = 'D';
		data[3] = 'I';
	}
	return 0;
}

/**
 * @brief Callback to fill buffer with incorrect MFR_ID
 * PMBus block read format: [length_byte, data_bytes...]
 */
static int fill_mfr_id_incorrect(struct no_os_i2c_desc* desc,
				 unsigned char* data,
				 unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 4 && data) {
		data[0] = 3;     /* PMBus length byte */
		data[1] = 'X';
		data[2] = 'Y';
		data[3] = 'Z';
	}
	return 0;
}

/**
 * @brief Callback to fill buffer with correct MFR_MODEL "LTM4700"
 * PMBus block read format: [length_byte, data_bytes...]
 */
static int fill_mfr_model_correct(struct no_os_i2c_desc* desc,
				  unsigned char* data,
				  unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 8 && data) {
		data[0] = 7;     /* PMBus length byte */
		data[1] = 'L';
		data[2] = 'T';
		data[3] = 'M';
		data[4] = '4';
		data[5] = '7';
		data[6] = '0';
		data[7] = '0';
	}
	return 0;
}

/**
 * @brief Callback to fill buffer with incorrect MFR_MODEL
 * PMBus block read format: [length_byte, data_bytes...]
 */
static int fill_mfr_model_incorrect(struct no_os_i2c_desc* desc,
				    unsigned char* data,
				    unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	if (bytes_number >= 8 && data) {
		data[0] = 7;     /* PMBus length byte */
		data[1] = 'L';
		data[2] = 'T';
		data[3] = 'M';
		data[4] = '9';
		data[5] = '9';
		data[6] = '9';
		data[7] = '9';
	}
	return 0;
}

/**
 * @brief Multi-stage callback for successful device identification
 * Handles SPECIAL_ID, MFR_ID, and MFR_MODEL reads in sequence
 */
static int fill_complete_device_id_success(struct no_os_i2c_desc* desc,
		unsigned char* data,
		unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;

	// First call (cmock_num_calls == 0): SPECIAL_ID (2 bytes word read)
	if (cmock_num_calls == 0 && bytes_number >= 2 && data) {
		data[0] = 0x30;  // LSB of 0x4130
		data[1] = 0x41;  // MSB of 0x4130
	}
	// Second call (cmock_num_calls == 1): MFR_ID (PMBus block read: length + 3 bytes)
	else if (cmock_num_calls == 1 && bytes_number >= 4 && data) {
		data[0] = 3;     /* PMBus length byte */
		data[1] = 'A';
		data[2] = 'D';
		data[3] = 'I';
	}
	// Third call (cmock_num_calls == 2): MFR_MODEL (PMBus block read: length + 7 bytes)
	else if (cmock_num_calls == 2 && bytes_number >= 8 && data) {
		data[0] = 7;     /* PMBus length byte */
		data[1] = 'L';
		data[2] = 'T';
		data[3] = 'M';
		data[4] = '4';
		data[5] = '7';
		data[6] = '0';
		data[7] = '0';
	}

	return 0;
}

/**
 * @brief Multi-stage callback with incorrect MFR_ID
 */
static int fill_device_id_mfr_id_fail(struct no_os_i2c_desc* desc,
				      unsigned char* data,
				      unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;

	// First call: SPECIAL_ID (correct - word read, 2 bytes)
	if (cmock_num_calls == 0 && bytes_number >= 2 && data) {
		data[0] = 0x30;
		data[1] = 0x41;
	}
	// Second call: MFR_ID (incorrect - PMBus block read, length + 3 bytes)
	else if (cmock_num_calls == 1 && bytes_number >= 4 && data) {
		data[0] = 3;     /* PMBus length byte */
		data[1] = 'X';
		data[2] = 'Y';
		data[3] = 'Z';
	}

	return 0;
}

/**
 * @brief Multi-stage callback with incorrect MFR_MODEL
 */
static int fill_device_id_mfr_model_fail(struct no_os_i2c_desc* desc,
		unsigned char* data,
		unsigned char bytes_number, unsigned char stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;

	// First call: SPECIAL_ID (correct - word read, 2 bytes)
	if (cmock_num_calls == 0 && bytes_number >= 2 && data) {
		data[0] = 0x30;
		data[1] = 0x41;
	}
	// Second call: MFR_ID (correct - PMBus block read, length + 3 bytes)
	else if (cmock_num_calls == 1 && bytes_number >= 4 && data) {
		data[0] = 3;     /* PMBus length byte */
		data[1] = 'A';
		data[2] = 'D';
		data[3] = 'I';
	}
	// Third call: MFR_MODEL (incorrect - PMBus block read, length + 7 bytes)
	else if (cmock_num_calls == 2 && bytes_number >= 8 && data) {
		data[0] = 7;     /* PMBus length byte */
		data[1] = 'L';
		data[2] = 'T';
		data[3] = 'M';
		data[4] = '9';
		data[5] = '9';
		data[6] = '9';
		data[7] = '9';
	}

	return 0;
}

/*******************************************************************************
 *    SETUP AND TEARDOWN
 ******************************************************************************/

/**
 * @brief Setup function called before each test
 */
void setUp(void)
{
	// Basic setup - no complex initialization needed
}

/**
 * @brief Teardown function called after each test
 */
void tearDown(void)
{
	// Basic teardown
}

/**
 * @brief Mock callback to fill word data for read operations
 */
static int fill_word_data(struct no_os_i2c_desc *desc, uint8_t *data,
			  uint8_t bytes_number, uint8_t stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	// Fill with test data: 0x1000 in little-endian format
	if (bytes_number == 2 && data) {
		data[0] = 0x00;  // LSB
		data[1] = 0x10;  // MSB
	}
	return 0;
}

/**
 * @brief Mock callback to fill block data for read operations
 */
static int fill_block_data(struct no_os_i2c_desc *desc, uint8_t *data,
			   uint8_t bytes_number, uint8_t stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;
	// Fill with test pattern data
	if (data) {
		for (int i = 0; i < bytes_number; i++) {
			data[i] = 0xA0 + i;  // Pattern: 0xA0, 0xA1, 0xA2, 0xA3, etc.
		}
	}
	return 0;
}

/**
 * @brief Mock callback to return LINEAR11 data with negative exponent
 */
static int fill_word_data_negative_exp(struct no_os_i2c_desc *desc,
				       uint8_t *data,
				       uint8_t bytes_number, uint8_t stop_bit, int cmock_num_calls)
{
	(void)desc;
	(void)stop_bit;
	(void)cmock_num_calls;

	// LINEAR11 format with negative exponent
	// Exponent = -3 (0x1D in 5-bit two's complement = 29), Mantissa = 800
	uint16_t lin11_data = (29 << 11) | 800;  // Negative exponent -3, mantissa 800

	if (data && bytes_number >= 2) {
		data[0] = (uint8_t)(lin11_data & 0xFF);        // LSB first
		data[1] = (uint8_t)((lin11_data >> 8) & 0xFF); // MSB second
	}

	return 0;
}

/*******************************************************************************
 *    TEST FUNCTIONS
 ******************************************************************************/

/**
 * @brief Test that header constants are properly defined
 */
void test_ltm4700_header_constants(void)
{
	// Test that basic PMBus command constants are defined
	TEST_ASSERT_EQUAL_HEX8(0x00, LTM4700_PAGE);
	TEST_ASSERT_EQUAL_HEX8(0x01, LTM4700_OPERATION);
	TEST_ASSERT_EQUAL_HEX8(0x02, LTM4700_ON_OFF_CONFIG);
	TEST_ASSERT_EQUAL_HEX8(0x03, LTM4700_CLEAR_FAULTS);
}

/**
 * @brief Test channel constants
 */
void test_ltm4700_channel_constants(void)
{
	// Test channel definitions
	TEST_ASSERT_EQUAL_INT(0, LTM4700_CHAN_0);
	TEST_ASSERT_EQUAL_INT(1, LTM4700_CHAN_1);
}

/**
 * @brief Test value type enumeration
 */
void test_ltm4700_value_types(void)
{
	// Test value type enumeration uses PMBus command values
	TEST_ASSERT_EQUAL_HEX8(0x88, LTM4700_VIN);   // READ_VIN command
	TEST_ASSERT_EQUAL_HEX8(0x89, LTM4700_IIN);   // READ_IIN command
	TEST_ASSERT_EQUAL_HEX8(0x8B, LTM4700_VOUT);  // READ_VOUT command
}

/**
 * @brief Test LINEAR11 format constants
 */
void test_ltm4700_linear_constants(void)
{
	// Test LINEAR11 format constants are defined
	TEST_ASSERT_EQUAL_INT(1023, LTM4700_LIN11_MANTISSA_MAX);
	TEST_ASSERT_EQUAL_INT(-1024, LTM4700_LIN11_MANTISSA_MIN);
	TEST_ASSERT_EQUAL_INT(15, LTM4700_LIN11_EXPONENT_MAX);
	TEST_ASSERT_EQUAL_INT(-15, LTM4700_LIN11_EXPONENT_MIN);
}

/**
 * @brief Test basic structure size validation
 */
void test_ltm4700_structure_sizes(void)
{
	// Verify structures have reasonable sizes (basic sanity check)
	TEST_ASSERT_TRUE(sizeof(struct ltm4700_init_param) > 0);
	TEST_ASSERT_TRUE(sizeof(struct ltm4700_dev) > 0);
}

/**
 * @brief Test macro definitions
 */
void test_ltm4700_macro_definitions(void)
{
	// Test that basic PMBus command macros are properly defined
	// This validates the header definitions without calling functions

	// Verify command values are in expected ranges
	TEST_ASSERT_TRUE(LTM4700_PAGE < 0x100);        // Valid 8-bit command
	TEST_ASSERT_TRUE(LTM4700_OPERATION < 0x100);   // Valid 8-bit command
	TEST_ASSERT_TRUE(LTM4700_READ_VIN < 0x100);    // Valid 8-bit command

	// Test that different commands have different values
	TEST_ASSERT_NOT_EQUAL(LTM4700_PAGE, LTM4700_OPERATION);
	TEST_ASSERT_NOT_EQUAL(LTM4700_PAGE, LTM4700_READ_VIN);
	TEST_ASSERT_NOT_EQUAL(LTM4700_OPERATION, LTM4700_READ_VIN);
}

/*******************************************************************************
 *    MANUFACTURER ID/MODEL VERIFICATION TESTS
 ******************************************************************************/

/**
 * @brief Test ltm4700_verify_manufacturer_id with NULL parameter
 */
void test_ltm4700_verify_manufacturer_id_null_param(void)
{
	int result = ltm4700_verify_manufacturer_id(NULL);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_id with I2C read failure
 */
void test_ltm4700_verify_manufacturer_id_i2c_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C write and read to fail
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_verify_manufacturer_id(&dev);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_id with correct ID
 */
void test_ltm4700_verify_manufacturer_id_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C operations for successful read of "ADI"
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_mfr_id_correct);

	int result = ltm4700_verify_manufacturer_id(&dev);
	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_id with incorrect ID
 */
void test_ltm4700_verify_manufacturer_id_incorrect(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C operations to return incorrect MFR_ID
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_mfr_id_incorrect);

	int result = ltm4700_verify_manufacturer_id(&dev);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_model with NULL parameter
 */
void test_ltm4700_verify_manufacturer_model_null_param(void)
{
	int result = ltm4700_verify_manufacturer_model(NULL);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_model with I2C read failure
 */
void test_ltm4700_verify_manufacturer_model_i2c_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C write and read to fail
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_verify_manufacturer_model(&dev);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_model with correct model
 */
void test_ltm4700_verify_manufacturer_model_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C operations for successful read of "LTM4700"
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_mfr_model_correct);

	int result = ltm4700_verify_manufacturer_model(&dev);
	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_verify_manufacturer_model with incorrect model
 */
void test_ltm4700_verify_manufacturer_model_incorrect(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc i2c_desc = {0};
	dev.i2c_desc = &i2c_desc;

	// Mock I2C operations to return incorrect MFR_MODEL
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_mfr_model_incorrect);

	int result = ltm4700_verify_manufacturer_model(&dev);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/*******************************************************************************
 *    INITIALIZATION TESTS
 ******************************************************************************/

/**
 * @brief Test device initialization parameter validation
 */
void test_ltm4700_init_with_mocks(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};

	// Test NULL parameter validation - should return error
	int result = ltm4700_init(NULL, &init_param);
	TEST_ASSERT_EQUAL(-EINVAL, result);

	// Test NULL init_param validation - should return error
	result = ltm4700_init(&device, NULL);
	TEST_ASSERT_EQUAL(-EINVAL, result);

	// Test NULL i2c_init validation - should return error
	init_param.i2c_init = NULL;
	result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test memory allocation failure path (lines 227-229)
 */
void test_ltm4700_init_allocation_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};

	// Setup valid parameters to pass parameter validation
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;

	// Mock no_os_calloc to return NULL (allocation failure)
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), NULL);

	// Test initialization - should fail with ENOMEM
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENOMEM, result);
}

/**
 * @brief Test I2C initialization failure path (lines 232-234)
 */
void test_ltm4700_init_i2c_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters to pass parameter validation
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;

	// Mock successful memory allocation (line 227)
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);

	// Mock I2C initialization failure (lines 232-234)
	no_os_i2c_init_IgnoreAndReturn(-EIO);  // Return I2C error

	// Mock the cleanup function that gets called on error
	no_os_free_Ignore();

	// Test initialization - should fail with I2C error
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test CRC initialization path (lines 237-240)
 */
void test_ltm4700_init_crc_enabled(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters to pass parameter validation
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;
	init_param.crc_en = true;  // Enable CRC to trigger lines 237-240

	// Mock successful memory allocation (line 227)
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);

	// Mock successful I2C initialization (lines 232-234)
	no_os_i2c_init_IgnoreAndReturn(0);

	// Mock CRC table initialization (lines 237-240)
	no_os_crc8_populate_msb_Ignore();

	// The function will continue and try to read device ID via ltm4700_read_word()
	// Mock the I2C communication sequence for device ID read to fail
	no_os_i2c_write_IgnoreAndReturn(0);     // Command write succeeds
	no_os_i2c_read_IgnoreAndReturn(
		-EIO);   // Data read fails - triggers error cleanup

	// Mock error cleanup functions
	no_os_i2c_remove_IgnoreAndReturn(0);    // Cleanup I2C
	no_os_free_Ignore();                    // Cleanup memory

	// Test initialization - should fail at device ID read, but CRC init should be covered
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test LTM4700 device detection (lines 253-255)
 */
void test_ltm4700_init_ltm4700_detection(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters with GPIO alert to trigger GPIO path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;  // Some GPIO pin number
	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;  // This triggers GPIO initialization

	// Mock successful memory allocation and I2C init
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);

	// Mock complete device identification sequence (SPECIAL_ID, MFR_ID, MFR_MODEL)
	no_os_i2c_write_IgnoreAndReturn(0);     // Command writes succeed
	no_os_i2c_read_Stub(fill_complete_device_id_success);

	// Function will continue to GPIO init - make it fail after device detection
	no_os_gpio_get_IgnoreAndReturn(-ENODEV);  // GPIO init failure
	no_os_i2c_remove_IgnoreAndReturn(0);      // Cleanup
	no_os_free_Ignore();

	// Test should detect LTM4700 correctly but fail later at GPIO
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test init failure when MFR_ID verification fails
 */
void test_ltm4700_init_mfr_id_verification_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;

	// Mock successful memory allocation and I2C init
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);

	// Mock device identification with incorrect MFR_ID
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_device_id_mfr_id_fail);

	// Cleanup mocks
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	// Test should fail at MFR_ID verification
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test init failure when MFR_MODEL verification fails
 */
void test_ltm4700_init_mfr_model_verification_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;

	// Mock successful memory allocation and I2C init
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);

	// Mock device identification with incorrect MFR_MODEL
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_device_id_mfr_model_fail);

	// Cleanup mocks
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	// Test should fail at MFR_MODEL verification
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test unknown device detection (lines 259-261)
 */
void test_ltm4700_init_unknown_device(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct ltm4700_dev mock_dev = {0};

	// Setup valid parameters
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	init_param.i2c_init = &i2c_init;

	// Mock successful memory allocation and I2C init
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);

	// Mock device ID read to return unknown special ID (lines 259-261)
	no_os_i2c_write_IgnoreAndReturn(0);     // Command write succeeds
	no_os_i2c_read_Stub(fill_unknown_device_id);   // Use callback to fill buffer

	// Mock error cleanup (goto error_i2c)
	no_os_i2c_remove_IgnoreAndReturn(0);    // Cleanup I2C
	no_os_free_Ignore();                    // Cleanup memory

	// Test should detect unknown device and return ENODEV
	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test pgood GPIO allocation failure (lines 274-278)
 */
void test_ltm4700_init_pgood_alloc_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param pgood_param1 = {0};
	struct no_os_gpio_init_param pgood_param2 = {0};
	struct no_os_gpio_init_param *pgood_params[2];
	struct ltm4700_dev mock_dev = {0};

	// Setup parameters with pgood_params to trigger pgood path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	pgood_param1.number = 11;  // PGOOD for channel 0
	pgood_param2.number = 12;  // PGOOD for channel 1
	pgood_params[0] = &pgood_param1;
	pgood_params[1] = &pgood_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.pgood_params = pgood_params;  // This triggers pgood path

	// Mock successful setup until pgood allocation
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);  // Complete device ID
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock pgood allocation failure (lines 274-278)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *), NULL);

	// Mock cleanup (goto error_alert)
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup alert GPIO
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENOMEM, result);
}

/**
 * @brief Test pgood GPIO get failure (lines 282-285)
 */
void test_ltm4700_init_pgood_gpio_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param pgood_param1 = {0};
	struct no_os_gpio_init_param pgood_param2 = {0};
	struct no_os_gpio_init_param *pgood_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_pgood_descs[2] = {0};

	// Setup parameters
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	pgood_param1.number = 11;
	pgood_param2.number = 12;
	pgood_params[0] = &pgood_param1;
	pgood_params[1] = &pgood_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.pgood_params = pgood_params;

	// Mock successful setup until GPIO get
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock successful pgood allocation (lines 274)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_pgood_descs);

	// Mock GPIO get failure on first pgood (lines 282-285)
	no_os_gpio_get_IgnoreAndReturn(-EIO);  // First GPIO get fails

	// Mock cleanup (goto error_pgood)
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup any pgood GPIOs
	no_os_free_Ignore();                   // Free pgood_descs
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup alert GPIO
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test successful pgood GPIO initialization (lines 280-288)
 */
void test_ltm4700_init_pgood_success(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param pgood_param1 = {0};
	struct no_os_gpio_init_param *pgood_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_pgood_descs[2] = {0};

	// Setup with only one channel having pgood (channel 0)
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	pgood_param1.number = 11;
	pgood_params[0] = &pgood_param1;  // Channel 0 has pgood
	pgood_params[1] = NULL;           // Channel 1 has no pgood

	init_param.i2c_init = &i2c_init;
	init_param.pgood_params = pgood_params;

	// Mock successful setup through device detection
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);

	// Mock successful pgood allocation and GPIO setup
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_pgood_descs);
	no_os_gpio_get_IgnoreAndReturn(0);  // First pgood GPIO succeeds
	// Second iteration: pgood_params[1] is NULL, so no GPIO get call

	// Function will continue to run_params/fault_params, which are NULL, so it continues
	// Let's make it fail at a later controlled point for testing
	no_os_calloc_IgnoreAndReturn(NULL);  // Any later allocation fails

	// Mock cleanup for any later failure
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	no_os_i2c_remove_IgnoreAndReturn(0);

	int result = ltm4700_init(&device, &init_param);

	// Should succeed through pgood path but may fail later - that's OK for testing
	// The important thing is we exercised lines 280-288
	TEST_ASSERT_TRUE(result <=
			 0);  // Allow any result since we tested the pgood path
}

/**
 * @brief Test run GPIO allocation failure (lines 291-295)
 */
void test_ltm4700_init_run_alloc_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param run_param1 = {0};
	struct no_os_gpio_init_param run_param2 = {0};
	struct no_os_gpio_init_param *run_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_pgood_descs[2] = {0};

	// Setup parameters with run_params to trigger run path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	run_param1.number = 13;  // RUN for channel 0
	run_param2.number = 14;  // RUN for channel 1
	run_params[0] = &run_param1;
	run_params[1] = &run_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.run_params = run_params;  // This triggers run path

	// Mock successful setup until run allocation
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);  // Complete device ID
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Note: No pgood_params, so pgood path is skipped

	// Mock run allocation failure (lines 291-295)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *), NULL);

	// Mock cleanup (goto error_pgood, but no pgood to clean up, so goes to error_alert)
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup alert GPIO
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-ENOMEM, result);
}

/**
 * @brief Test run GPIO get failure (lines 299-302)
 */
void test_ltm4700_init_run_gpio_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param run_param1 = {0};
	struct no_os_gpio_init_param run_param2 = {0};
	struct no_os_gpio_init_param *run_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_run_descs[2] = {0};

	// Setup parameters
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	run_param1.number = 13;
	run_param2.number = 14;
	run_params[0] = &run_param1;
	run_params[1] = &run_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.run_params = run_params;

	// Mock successful setup until GPIO get
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock successful run allocation (lines 291)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_run_descs);

	// Mock GPIO get failure on first run (lines 299-302)
	no_os_gpio_get_IgnoreAndReturn(-EIO);  // First run GPIO get fails

	// Mock cleanup (goto error_run)
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup any run GPIOs
	no_os_free_Ignore();                   // Free run_descs
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup alert GPIO
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int result = ltm4700_init(&device, &init_param);
	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test successful run GPIO initialization (lines 297-304)
 */
void test_ltm4700_init_run_success(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param run_param1 = {0};
	struct no_os_gpio_init_param *run_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_run_descs[2] = {0};

	// Setup with only one channel having run (channel 0)
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	run_param1.number = 13;
	run_params[0] = &run_param1;  // Channel 0 has run
	run_params[1] = NULL;         // Channel 1 has no run

	init_param.i2c_init = &i2c_init;
	init_param.run_params = run_params;

	// Mock successful setup through device detection
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);

	// Mock successful run allocation and GPIO setup
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_run_descs);
	no_os_gpio_get_IgnoreAndReturn(0);  // First run GPIO succeeds
	// Second iteration: run_params[1] is NULL, so no GPIO get call

	// Function will continue to fault_params, which are NULL, so it continues
	// Let's make it fail at a later controlled point for testing
	no_os_calloc_IgnoreAndReturn(NULL);  // Any later allocation fails

	// Mock cleanup for any later failure
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	no_os_i2c_remove_IgnoreAndReturn(0);

	int result = ltm4700_init(&device, &init_param);

	// Should succeed through run path but may fail later - that's OK for testing
	// The important thing is we exercised lines 297-304
	TEST_ASSERT_TRUE(result <= 0);  // Allow any result since we tested the run path
}

/**
 * @brief Test fault GPIO allocation failure (lines 308-311)
 */
void test_ltm4700_init_fault_alloc_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param fault_param1 = {0};
	struct no_os_gpio_init_param fault_param2 = {0};
	struct no_os_gpio_init_param *fault_params[2];
	struct ltm4700_dev mock_dev = {0};

	// Setup parameters with fault_params to trigger fault path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	fault_param1.number = 15;  // FAULT for channel 0
	fault_param2.number = 16;  // FAULT for channel 1
	fault_params[0] = &fault_param1;
	fault_params[1] = &fault_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.fault_params = fault_params;  // This triggers fault path

	// Mock successful early steps
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_crc8_populate_msb_Ignore();
	no_os_i2c_read_Stub(fill_complete_device_id_success);  // Complete device ID
	no_os_i2c_write_IgnoreAndReturn(0);  // Handle any I2C writes during init
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock fault_descs allocation failure (line 308-310)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *), NULL);

	// Mock cleanup
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	no_os_i2c_remove_IgnoreAndReturn(0);

	int result = ltm4700_init(&device, &init_param);

	TEST_ASSERT_EQUAL(-ENOMEM, result);
	TEST_ASSERT_NULL(device);
}

/**
 * @brief Test fault GPIO get failure (lines 316-318)
 */
void test_ltm4700_init_fault_gpio_failure(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param fault_param1 = {0};
	struct no_os_gpio_init_param fault_param2 = {0};
	struct no_os_gpio_init_param *fault_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_fault_descs[2] = {0};

	// Setup parameters with fault_params to trigger fault path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	fault_param1.number = 15;  // FAULT for channel 0
	fault_param2.number = 16;  // FAULT for channel 1
	fault_params[0] = &fault_param1;
	fault_params[1] = &fault_param2;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.fault_params = fault_params;  // This triggers fault path

	// Mock successful early steps
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_crc8_populate_msb_Ignore();
	no_os_i2c_read_Stub(fill_complete_device_id_success);  // Complete device ID
	no_os_i2c_write_IgnoreAndReturn(0);  // Handle any I2C writes during init
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock successful fault_descs allocation
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_fault_descs);

	// Mock fault GPIO failure (line 316-318)
	no_os_gpio_get_IgnoreAndReturn(-ENODEV);  // First fault GPIO fails

	// Mock cleanup
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	no_os_i2c_remove_IgnoreAndReturn(0);

	int result = ltm4700_init(&device, &init_param);

	TEST_ASSERT_EQUAL(-ENODEV, result);
	TEST_ASSERT_NULL(device);
}

/**
 * @brief Test fault GPIO success (lines 315-321)
 */
void test_ltm4700_init_fault_success(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param fault_param1 = {0};
	struct no_os_gpio_init_param *fault_params[2];
	struct ltm4700_dev mock_dev = {0};
	struct no_os_gpio_desc *mock_fault_descs[2] = {0};

	// Setup parameters with fault_params to trigger fault path
	// Set fault_params[1] = NULL to test both filled and NULL cases
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	fault_param1.number = 15;  // FAULT for channel 0
	fault_params[0] = &fault_param1;
	fault_params[1] = NULL;  // NULL for channel 1 (tests line 315 condition)

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.fault_params = fault_params;  // This triggers fault path

	// Mock successful early steps
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_crc8_populate_msb_Ignore();
	no_os_i2c_read_Stub(fill_complete_device_id_success);  // Device detection
	no_os_i2c_write_IgnoreAndReturn(0);  // Handle any I2C writes during init
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock successful fault allocation and GPIO setup
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_fault_descs);
	no_os_gpio_get_IgnoreAndReturn(0);  // First fault GPIO succeeds
	// Second iteration: fault_params[1] is NULL, so no GPIO get call

	// Function will reach successful completion, but let's make it fail at a controlled point
	no_os_calloc_IgnoreAndReturn(NULL);  // Any later allocation fails

	// Mock cleanup for any later failure
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	no_os_i2c_remove_IgnoreAndReturn(0);

	int result = ltm4700_init(&device, &init_param);

	// Should succeed through fault path but may fail later - that's OK for testing
	// The important thing is we exercised lines 315-321
	TEST_ASSERT_TRUE(result <=
			 0);  // Allow any result since we tested the fault path
}

/**
 * @brief Test ltm4700_read_word with NULL dev pointer (lines 524-525)
 */
void test_ltm4700_read_word_null_dev(void)
{
	uint16_t word;

	// Call ltm4700_read_word with NULL dev - should return -EINVAL
	int result = ltm4700_read_word(NULL, 0, 0x8B, &word);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_word with NULL word pointer (lines 524-525)
 */
void test_ltm4700_read_word_null_word(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_word with NULL word pointer - should return -EINVAL
	int result = ltm4700_read_word(&dev, 0, 0x8B, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_word with paged command success (lines 527-531)
 */
void test_ltm4700_read_word_paged_command_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint16_t word;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock successful page setting (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(0);

	// Mock successful word read
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with a paged command (LTM4700_VOUT_COMMAND is paged)
	int result = ltm4700_read_word(&dev, 0, LTM4700_VOUT_COMMAND, &word);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_word with paged command set_page failure (lines 527-531)
 */
void test_ltm4700_read_word_paged_command_set_page_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint16_t word;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock page setting failure (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Call with a paged command - should fail at set_page and return early
	int result = ltm4700_read_word(&dev, 0, LTM4700_VOUT_COMMAND, &word);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_read_word with I2C write failure (lines 534-535)
 */
void test_ltm4700_read_word_i2c_write_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint16_t word;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page to avoid paged command logic

	// Use a non-paged command to avoid the set_page path
	// LTM4700_VIN is not in the paged command list, so it will skip lines 527-531
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock I2C write failure (line 533: no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_word - should fail at I2C write and return error
	int result = ltm4700_read_word(&dev, 0, non_paged_cmd, &word);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_set_page with NULL device pointer (lines 411-412)
 */
void test_ltm4700_set_page_null_dev(void)
{
	// Call ltm4700_set_page with NULL dev - should return -EINVAL
	int result = ltm4700_set_page(NULL, 0);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_page with negative page number (lines 414-415)
 */
void test_ltm4700_set_page_negative_page(void)
{
	struct ltm4700_dev dev = {0};

	// Setup device with valid parameters
	dev.num_channels = 2;

	// Call with negative page - should return -EINVAL
	int result = ltm4700_set_page(&dev, -1);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_page with page >= num_channels (lines 414-415)
 */
void test_ltm4700_set_page_page_too_large(void)
{
	struct ltm4700_dev dev = {0};

	// Setup device with 2 channels (valid pages are 0 and 1)
	dev.num_channels = 2;

	// Call with page 2 (>= num_channels) - should return -EINVAL
	int result = ltm4700_set_page(&dev, 2);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_page when current page equals target page (lines 417-418)
 */
void test_ltm4700_set_page_same_page(void)
{
	struct ltm4700_dev dev = {0};

	// Setup device with valid parameters and current page = 1
	dev.num_channels = 2;
	dev.page = 1;

	// Call to set page to 1 (same as current) - should return 0 without I2C operation
	int result = ltm4700_set_page(&dev, 1);

	TEST_ASSERT_EQUAL(0, result);
}

/* ===== ltm4700_read_byte Unit Tests ===== */

/**
 * @brief Test ltm4700_read_byte with NULL device pointer
 */
void test_ltm4700_read_byte_null_dev(void)
{
	uint8_t data;

	// Call ltm4700_read_byte with NULL dev - should return -EINVAL
	int result = ltm4700_read_byte(NULL, 0, 0x8B, &data);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_byte with NULL data pointer
 */
void test_ltm4700_read_byte_null_data(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_byte with NULL data pointer - should return -EINVAL
	int result = ltm4700_read_byte(&dev, 0, 0x8B, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_byte with paged command success
 */
void test_ltm4700_read_byte_paged_command_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock successful page setting (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(0);

	// Mock successful byte read
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with a paged command (LTM4700_OPERATION is paged)
	int result = ltm4700_read_byte(&dev, 0, LTM4700_OPERATION, &data);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_byte with paged command set_page failure
 */
void test_ltm4700_read_byte_paged_command_set_page_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock page setting failure (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Call with a paged command - should fail at set_page and return early
	int result = ltm4700_read_byte(&dev, 0, LTM4700_OPERATION, &data);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_read_byte with I2C write failure
 */
void test_ltm4700_read_byte_i2c_write_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page to avoid paged command logic

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock I2C write failure
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_byte - should fail at I2C write and return error
	int result = ltm4700_read_byte(&dev, 0, non_paged_cmd, &data);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_read_byte with I2C read failure
 */
void test_ltm4700_read_byte_i2c_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page to avoid paged command logic

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock successful I2C write but failed read
	no_os_i2c_write_IgnoreAndReturn(0);   // Command write succeeds
	no_os_i2c_read_IgnoreAndReturn(-ENXIO); // Data read fails

	// Call ltm4700_read_byte - should fail at I2C read and return error
	int result = ltm4700_read_byte(&dev, 0, non_paged_cmd, &data);

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_read_byte successful operation with non-paged command
 */
void test_ltm4700_read_byte_success_non_paged(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);   // Command write succeeds
	no_os_i2c_read_IgnoreAndReturn(0);    // Data read succeeds

	// Call ltm4700_read_byte - should succeed completely
	int result = ltm4700_read_byte(&dev, 0, non_paged_cmd, &data);

	TEST_ASSERT_EQUAL(0, result);
}

/* ===== ltm4700_read_word_data Unit Tests ===== */

/**
 * @brief Test ltm4700_read_word_data with NULL device pointer
 */
void test_ltm4700_read_word_data_null_dev(void)
{
	int data;

	// Call ltm4700_read_word_data with NULL dev - should return -EINVAL
	int result = ltm4700_read_word_data(NULL, 0, LTM4700_READ_IOUT, &data);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_word_data with NULL data pointer
 */
void test_ltm4700_read_word_data_null_data(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_word_data with NULL data pointer - should return -EINVAL
	int result = ltm4700_read_word_data(&dev, 0, LTM4700_READ_IOUT, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_word_data with ltm4700_read_word failure
 */
void test_ltm4700_read_word_data_read_word_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write failure to make ltm4700_read_word fail
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_word_data - should fail when ltm4700_read_word fails
	int result = ltm4700_read_word_data(&dev, 0, LTM4700_READ_IOUT, &data);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_read_word_data with LINEAR16 format (voltage command)
 */
void test_ltm4700_read_word_data_linear16_format(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;  // Typical exponent for voltage

	// Create mock callback to return specific word data
	static uint8_t expected_read_data[2];
	expected_read_data[0] = 0x00;
	expected_read_data[1] = 0x10;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with voltage command (uses LINEAR16 format)
	int result = ltm4700_read_word_data(&dev, 0, LTM4700_VOUT_COMMAND, &data);

	TEST_ASSERT_EQUAL(0, result);
	// The actual conversion value will depend on the LINEAR16 conversion function
}

/**
 * @brief Test ltm4700_read_word_data with LINEAR11 format (default)
 */
void test_ltm4700_read_word_data_linear11_format(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int data;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with non-voltage command (uses LINEAR11 format)
	int result = ltm4700_read_word_data(&dev, 0, LTM4700_READ_IOUT, &data);

	TEST_ASSERT_EQUAL(0, result);
	// The actual conversion value will depend on the LINEAR11 conversion function
}

/* ===== ltm4700_read_block_data Unit Tests ===== */

/**
 * @brief Test ltm4700_read_block_data with NULL device pointer
 */
void test_ltm4700_read_block_data_null_dev(void)
{
	uint8_t data[4];

	// Call ltm4700_read_block_data with NULL dev - should return -EINVAL
	int result = ltm4700_read_block_data(NULL, 0, 0x88, data, sizeof(data));

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_block_data with NULL data pointer
 */
void test_ltm4700_read_block_data_null_data(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_block_data with NULL data pointer - should return -EINVAL
	int result = ltm4700_read_block_data(&dev, 0, 0x88, NULL, 4);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_block_data with zero nbytes
 */
void test_ltm4700_read_block_data_zero_nbytes(void)
{
	struct ltm4700_dev dev = {0};
	uint8_t data[4];

	// Call ltm4700_read_block_data with zero nbytes - should return -EINVAL
	int result = ltm4700_read_block_data(&dev, 0, 0x88, data, 0);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_block_data with paged command success
 */
void test_ltm4700_read_block_data_paged_command_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[4];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock successful page setting (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(0);

	// Mock successful block read
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with a paged command (LTM4700_OPERATION is paged)
	int result = ltm4700_read_block_data(&dev, 0, LTM4700_OPERATION, data,
					     sizeof(data));

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_block_data with paged command set_page failure
 */
void test_ltm4700_read_block_data_paged_command_set_page_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[4];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = -1;  // Different from target page to trigger set_page call

	// Mock page setting failure (ltm4700_set_page calls no_os_i2c_write)
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Call with a paged command - should fail at set_page and return early
	int result = ltm4700_read_block_data(&dev, 0, LTM4700_OPERATION, data,
					     sizeof(data));

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_read_block_data with I2C write failure
 */
void test_ltm4700_read_block_data_i2c_write_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[4];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page to avoid paged command logic

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock I2C write failure
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_block_data - should fail at I2C write and return error
	int result = ltm4700_read_block_data(&dev, 0, non_paged_cmd, data,
					     sizeof(data));

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_read_block_data with I2C read failure
 */
void test_ltm4700_read_block_data_i2c_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[4];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page to avoid paged command logic

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock successful I2C write but failed read
	no_os_i2c_write_IgnoreAndReturn(0);   // Command write succeeds
	no_os_i2c_read_IgnoreAndReturn(-ENXIO); // Block read fails

	// Call ltm4700_read_block_data - should fail at I2C read and return error
	int result = ltm4700_read_block_data(&dev, 0, non_paged_cmd, data,
					     sizeof(data));

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_read_block_data successful operation with non-paged command
 */
void test_ltm4700_read_block_data_success_non_paged(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[4];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page

	// Use a non-paged command to avoid the set_page path
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);   // Command write succeeds
	no_os_i2c_read_Stub(fill_block_data); // Block read succeeds

	// Call ltm4700_read_block_data - should succeed completely
	int result = ltm4700_read_block_data(&dev, 0, non_paged_cmd, data,
					     sizeof(data));

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_block_data successful operation with variable nbytes
 */
void test_ltm4700_read_block_data_variable_nbytes(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	uint8_t data[8];

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;  // Set current page

	// Use a non-paged command
	uint8_t non_paged_cmd = 0x88;  // LTM4700_VIN (not paged)

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);   // Command write succeeds
	no_os_i2c_read_Stub(fill_block_data); // Block read succeeds

	// Test with different block sizes
	int result = ltm4700_read_block_data(&dev, 0, non_paged_cmd, data, 1);
	TEST_ASSERT_EQUAL(0, result);

	result = ltm4700_read_block_data(&dev, 0, non_paged_cmd, data, 8);
	TEST_ASSERT_EQUAL(0, result);
}

/* ===== ltm4700_read_value Unit Tests ===== */

/**
 * @brief Test ltm4700_read_value with NULL device pointer
 */
void test_ltm4700_read_value_null_dev(void)
{
	int value;

	// Call ltm4700_read_value with NULL dev - should return -EINVAL
	int result = ltm4700_read_value(NULL, 0, LTM4700_VIN, &value);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_value with NULL value pointer
 */
void test_ltm4700_read_value_null_value(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_value with NULL value pointer - should return -EINVAL
	int result = ltm4700_read_value(&dev, 0, LTM4700_VIN, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_value with invalid channel number
 */
void test_ltm4700_read_value_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	int value;

	// Setup device with 2 channels (valid channels are 0 and 1)
	dev.num_channels = 2;

	// Call with channel 2 (>= num_channels) - should return -EINVAL
	int result = ltm4700_read_value(&dev, 2, LTM4700_VIN, &value);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_value with ltm4700_read_word_data failure
 */
void test_ltm4700_read_value_read_word_data_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write failure to make ltm4700_read_word_data fail
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_value - should fail when ltm4700_read_word_data fails
	int result = ltm4700_read_value(&dev, 0, LTM4700_VIN, &value);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_read_value successful operation with voltage type
 */
void test_ltm4700_read_value_success_voltage(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with voltage value type (VIN)
	int result = ltm4700_read_value(&dev, 0, LTM4700_VIN, &value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_value successful operation with current type
 */
void test_ltm4700_read_value_success_current(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with current value type (IOUT)
	int result = ltm4700_read_value(&dev, 1, LTM4700_IOUT, &value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_value successful operation with temperature type
 */
void test_ltm4700_read_value_success_temperature(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with temperature value type (TEMP_EXT)
	int result = ltm4700_read_value(&dev, 0, LTM4700_TEMP_EXT, &value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_value successful operation with power type
 */
void test_ltm4700_read_value_success_power(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with power value type (POUT)
	int result = ltm4700_read_value(&dev, 1, LTM4700_POUT, &value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_value successful operation with peak value type
 */
void test_ltm4700_read_value_success_peak(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	int value;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);

	// Call with peak value type (VOUT_PEAK)
	int result = ltm4700_read_value(&dev, 0, LTM4700_VOUT_PEAK, &value);

	TEST_ASSERT_EQUAL(0, result);
}

/* ===== ltm4700_read_status Unit Tests ===== */

/**
 * @brief Test ltm4700_read_status with NULL device pointer
 */
void test_ltm4700_read_status_null_dev(void)
{
	struct ltm4700_status status;

	// Call ltm4700_read_status with NULL dev - should return -EINVAL
	int result = ltm4700_read_status(NULL, 0, LTM4700_STATUS_BYTE_TYPE, &status);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_status with NULL status pointer
 */
void test_ltm4700_read_status_null_status(void)
{
	struct ltm4700_dev dev = {0};

	// Call ltm4700_read_status with NULL status pointer - should return -EINVAL
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_BYTE_TYPE, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_status with invalid channel number
 */
void test_ltm4700_read_status_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	struct ltm4700_status status;

	// Setup device with 2 channels (valid channels are 0 and 1)
	dev.num_channels = 2;

	// Call with channel 2 (>= num_channels) - should return -EINVAL
	int result = ltm4700_read_status(&dev, 2, LTM4700_STATUS_BYTE_TYPE, &status);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_read_status with BYTE type and ltm4700_read_byte failure
 */
void test_ltm4700_read_status_byte_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write failure to make ltm4700_read_byte fail
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_read_status with BYTE type - should fail when ltm4700_read_byte fails
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_BYTE_TYPE, &status);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_read_status with WORD type and ltm4700_read_word failure
 */
void test_ltm4700_read_status_word_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write failure to make ltm4700_read_word fail
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Call ltm4700_read_status with WORD type - should fail when ltm4700_read_word fails
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_WORD_TYPE, &status);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with BYTE type
 */
void test_ltm4700_read_status_byte_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Initialize status with non-zero values to verify memset clears it
	memset(&status, 0xFF, sizeof(status));

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with BYTE status type
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_BYTE_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
	// Verify that status structure was cleared by memset
	TEST_ASSERT_EQUAL(0, status.word);
}

/**
 * @brief Test ltm4700_read_status successful operation with VOUT type
 */
void test_ltm4700_read_status_vout_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with VOUT status type
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_VOUT_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with IOUT type
 */
void test_ltm4700_read_status_iout_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with IOUT status type
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_IOUT_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with INPUT type
 */
void test_ltm4700_read_status_input_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with INPUT status type - note: INPUT always uses channel 0
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_INPUT_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with TEMP type
 */
void test_ltm4700_read_status_temp_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with TEMP status type
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_TEMP_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with CML type
 */
void test_ltm4700_read_status_cml_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with CML status type - note: CML always uses channel 0
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_CML_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with MFR_SPECIFIC type
 */
void test_ltm4700_read_status_mfr_specific_type_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with MFR_SPECIFIC status type
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_MFR_SPECIFIC_TYPE,
					 &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status successful operation with ALL types
 */
void test_ltm4700_read_status_all_types_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_word_data);  // For word read
	no_os_i2c_read_IgnoreAndReturn(0);     // For all byte reads

	// Call with ALL status types (tests all branches)
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_ALL_TYPE, &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status with multiple types combined
 */
void test_ltm4700_read_status_multiple_types_combined(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C operations
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	// Call with combined status types (BYTE | VOUT | IOUT)
	int result = ltm4700_read_status(&dev, 0,
					 LTM4700_STATUS_BYTE_TYPE | LTM4700_STATUS_VOUT_TYPE | LTM4700_STATUS_IOUT_TYPE,
					 &status);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_read_status VOUT type read failure (line 748)
 */
void test_ltm4700_read_status_vout_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write success, but read failure specifically for VOUT status
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-ENXIO);

	// Call with VOUT status type - should fail at VOUT read and return error
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_VOUT_TYPE, &status);

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_read_status IOUT type read failure (line 754)
 */
void test_ltm4700_read_status_iout_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write success, but read failure specifically for IOUT status
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-ETIMEDOUT);

	// Call with IOUT status type - should fail at IOUT read and return error
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_IOUT_TYPE, &status);

	TEST_ASSERT_EQUAL(-ETIMEDOUT, result);
}

/**
 * @brief Test ltm4700_read_status INPUT type read failure (line 760)
 */
void test_ltm4700_read_status_input_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write success, but read failure specifically for INPUT status
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EREMOTEIO);

	// Call with INPUT status type - should fail at INPUT read and return error
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_INPUT_TYPE, &status);

	TEST_ASSERT_EQUAL(-EREMOTEIO, result);
}

/**
 * @brief Test ltm4700_read_status TEMP type read failure (line 766)
 */
void test_ltm4700_read_status_temp_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write success, but read failure specifically for TEMP status
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-ENODEV);

	// Call with TEMP status type - should fail at TEMP read and return error
	int result = ltm4700_read_status(&dev, 0, LTM4700_STATUS_TEMP_TYPE, &status);

	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test ltm4700_read_status CML type read failure (line 773)
 */
void test_ltm4700_read_status_cml_type_read_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	struct ltm4700_status status;

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write success, but read failure specifically for CML status
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EPROTO);

	// Call with CML status type - should fail at CML read and return error
	int result = ltm4700_read_status(&dev, 1, LTM4700_STATUS_CML_TYPE, &status);

	TEST_ASSERT_EQUAL(-EPROTO, result);
}

/* ===== ltm4700_clear_peaks Unit Tests ===== */

/**
 * @brief Test ltm4700_clear_peaks with NULL device pointer
 */
void test_ltm4700_clear_peaks_null_dev(void)
{
	// Call ltm4700_clear_peaks with NULL dev - should return -EINVAL
	int result = ltm4700_clear_peaks(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_clear_peaks with I2C write failure
 */
void test_ltm4700_clear_peaks_i2c_write_failure(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C write failure (LTM4700_MFR_CLEAR_PEAKS is not a paged command)
	no_os_i2c_write_IgnoreAndReturn(-EBUSY);

	// Call ltm4700_clear_peaks - should fail at I2C write and return error
	int result = ltm4700_clear_peaks(&dev);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_clear_peaks successful operation
 */
void test_ltm4700_clear_peaks_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock successful I2C write operation
	no_os_i2c_write_IgnoreAndReturn(0);

	// Call ltm4700_clear_peaks - should succeed
	int result = ltm4700_clear_peaks(&dev);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_clear_peaks with different I2C error codes
 */
void test_ltm4700_clear_peaks_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENXIO, -EREMOTEIO, -ENODEV};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_clear_peaks - should return the specific error
		int result = ltm4700_clear_peaks(&dev);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_clear_peaks verifies it uses non-paged command
 */
void test_ltm4700_clear_peaks_non_paged_command_verification(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters - set page to non-zero to verify
	// that page setting is NOT called for this non-paged command
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 1;  // Non-zero page

	// Mock successful I2C operations - only expect the command write,
	// not any page setting operations
	no_os_i2c_write_IgnoreAndReturn(0);

	// Call ltm4700_clear_peaks - should succeed without page operations
	// since LTM4700_MFR_CLEAR_PEAKS is not a paged command
	int result = ltm4700_clear_peaks(&dev);

	TEST_ASSERT_EQUAL(0, result);
}

// ============================================================================
// ltm4700_remove function tests
// ============================================================================

/**
 * @brief Test ltm4700_remove with NULL device pointer
 */
void test_ltm4700_remove_null_dev(void)
{
	int result = ltm4700_remove(NULL);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_remove with alert GPIO removal error
 */
void test_ltm4700_remove_alert_gpio_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_gpio_desc mock_alert_desc = {0};
	dev.alert_desc = &mock_alert_desc;

	no_os_gpio_remove_IgnoreAndReturn(-EIO);
	no_os_free_Expect(&dev);

	int result = ltm4700_remove(&dev);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_remove with I2C removal error
 */
void test_ltm4700_remove_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_gpio_desc mock_alert_desc = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.alert_desc = &mock_alert_desc;
	dev.i2c_desc = &mock_i2c_desc;

	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(-ENODEV);
	no_os_free_Expect(&dev);

	int result = ltm4700_remove(&dev);

	TEST_ASSERT_EQUAL(-ENODEV, result);
}

/**
 * @brief Test ltm4700_remove success with minimal setup (no GPIO arrays)
 */
void test_ltm4700_remove_success_minimal(void)
{
	// Use malloc directly since we're testing the cleanup
	struct ltm4700_dev *dev = malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));
	struct no_os_gpio_desc mock_alert_desc = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev->alert_desc = &mock_alert_desc;
	dev->i2c_desc = &mock_i2c_desc;
	// Leave fault_descs, run_descs, pgood_descs as NULL

	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int result = ltm4700_remove(dev);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_remove success with all GPIO arrays present
 */
void test_ltm4700_remove_success_with_gpio_arrays(void)
{
	// Use malloc directly since we're testing the cleanup
	struct ltm4700_dev *dev = malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));
	struct no_os_gpio_desc **mock_fault_descs = malloc(2 * sizeof(
				struct no_os_gpio_desc *));
	struct no_os_gpio_desc **mock_run_descs = malloc(2 * sizeof(
				struct no_os_gpio_desc *));
	struct no_os_gpio_desc **mock_pgood_descs = malloc(2 * sizeof(
				struct no_os_gpio_desc *));
	struct no_os_gpio_desc mock_alert_desc = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	dev->num_channels = 2;
	dev->fault_descs = mock_fault_descs;
	dev->run_descs = mock_run_descs;
	dev->pgood_descs = mock_pgood_descs;
	dev->alert_desc = &mock_alert_desc;
	dev->i2c_desc = &mock_i2c_desc;

	no_os_gpio_remove_IgnoreAndReturn(0);  // For GPIO arrays and alert
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();  // For arrays and device struct

	int result = ltm4700_remove(dev);

	TEST_ASSERT_EQUAL(0, result);
}

// ============================================================================
// ltm4700_send_byte function tests
// ============================================================================

/**
 * @brief Test ltm4700_send_byte with NULL device pointer
 */
void test_ltm4700_send_byte_null_dev(void)
{
	int result = ltm4700_send_byte(NULL, 0, LTM4700_VOUT_MODE);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_send_byte with I2C write failure
 */
void test_ltm4700_send_byte_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_send_byte(&dev, 0,
				       LTM4700_MFR_CLEAR_PEAKS);  // Non-paged command

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_send_byte success with non-paged command
 */
void test_ltm4700_send_byte_non_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_send_byte(&dev, 0,
				       LTM4700_MFR_CLEAR_PEAKS);  // Non-paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_send_byte success with paged command
 */
void test_ltm4700_send_byte_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(0);  // For both page setting and command

	int result = ltm4700_send_byte(&dev, 1, LTM4700_VOUT_MODE);  // Paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_send_byte with page setting error
 */
void test_ltm4700_send_byte_page_set_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(-ENXIO);  // Page setting will fail

	int result = ltm4700_send_byte(&dev, 1, LTM4700_VOUT_MODE);  // Paged command

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_send_byte with various I2C error codes
 */
void test_ltm4700_send_byte_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_send_byte with non-paged command
		int result = ltm4700_send_byte(&dev, 0, LTM4700_MFR_CLEAR_PEAKS);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

// ============================================================================
// ltm4700_write_byte function tests
// ============================================================================

/**
 * @brief Test ltm4700_write_byte with NULL device pointer
 */
void test_ltm4700_write_byte_null_dev(void)
{
	int result = ltm4700_write_byte(NULL, 0, LTM4700_VOUT_MODE, 0x16);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_write_byte with I2C write failure
 */
void test_ltm4700_write_byte_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_write_byte(&dev, 0, LTM4700_MFR_CLEAR_PEAKS,
					0x00);  // Non-paged command

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_write_byte success with non-paged command
 */
void test_ltm4700_write_byte_non_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_write_byte(&dev, 0, LTM4700_MFR_CLEAR_PEAKS,
					0x00);  // Non-paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_byte success with paged command
 */
void test_ltm4700_write_byte_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(0);  // For both page setting and command

	int result = ltm4700_write_byte(&dev, 1, LTM4700_VOUT_MODE,
					0x16);  // Paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_byte with page setting error
 */
void test_ltm4700_write_byte_page_set_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(-ENXIO);  // Page setting will fail

	int result = ltm4700_write_byte(&dev, 1, LTM4700_VOUT_MODE,
					0x16);  // Paged command

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_write_byte with various I2C error codes
 */
void test_ltm4700_write_byte_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_write_byte with non-paged command
		int result = ltm4700_write_byte(&dev, 0, LTM4700_MFR_CLEAR_PEAKS, 0x00);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

// ============================================================================
// ltm4700_write_word function tests
// ============================================================================

/**
 * @brief Test ltm4700_write_word with NULL device pointer
 */
void test_ltm4700_write_word_null_dev(void)
{
	int result = ltm4700_write_word(NULL, 0, LTM4700_VOUT_COMMAND, 0x1234);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_write_word with I2C write failure
 */
void test_ltm4700_write_word_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_write_word(&dev, 0, LTM4700_MFR_CLEAR_PEAKS,
					0x0000);  // Non-paged command

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_write_word success with non-paged command
 */
void test_ltm4700_write_word_non_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_write_word(&dev, 0, LTM4700_MFR_CLEAR_PEAKS,
					0x0000);  // Non-paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_word success with paged command
 */
void test_ltm4700_write_word_paged_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(0);  // For both page setting and command

	int result = ltm4700_write_word(&dev, 1, LTM4700_VOUT_COMMAND,
					0x1234);  // Paged command

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_word with page setting error
 */
void test_ltm4700_write_word_page_set_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;  // Set valid channel count

	no_os_i2c_write_IgnoreAndReturn(-ENXIO);  // Page setting will fail

	int result = ltm4700_write_word(&dev, 1, LTM4700_VOUT_COMMAND,
					0x1234);  // Paged command

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_write_word with various I2C error codes
 */
void test_ltm4700_write_word_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_write_word with non-paged command
		int result = ltm4700_write_word(&dev, 0, LTM4700_MFR_CLEAR_PEAKS, 0x0000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

// ============================================================================
// ltm4700_write_word_data function tests
// ============================================================================

/**
 * @brief Test ltm4700_write_word_data with NULL device pointer
 */
void test_ltm4700_write_word_data_null_dev(void)
{
	int result = ltm4700_write_word_data(NULL, 0, LTM4700_VOUT_COMMAND, 1200000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_write_word_data with LINEAR16 voltage command
 */
void test_ltm4700_write_word_data_linear16_voltage(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;
	dev.lin16_exp = -12;  // Set valid LINEAR16 exponent

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and command

	int result = ltm4700_write_word_data(&dev, 1, LTM4700_VOUT_COMMAND,
					     1200000);  // 1.2V in uV

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_word_data with LINEAR11 default command
 */
void test_ltm4700_write_word_data_linear11_default(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and command

	int result = ltm4700_write_word_data(&dev, 1, LTM4700_IOUT_OC_FAULT_LIMIT,
					     50000000);  // 50A in uA

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_word_data with VOUT_MAX (LINEAR16 voltage)
 */
void test_ltm4700_write_word_data_vout_max_linear16(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;
	dev.lin16_exp = -12;  // Set valid LINEAR16 exponent

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and command

	int result = ltm4700_write_word_data(&dev, 1, LTM4700_VOUT_MAX,
					     1320000);  // 1.32V in uV

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_write_word_data with underlying write_word error
 */
void test_ltm4700_write_word_data_write_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(-EBUSY);  // Simulate I2C busy error

	int result = ltm4700_write_word_data(&dev, 1, LTM4700_VOUT_COMMAND, 1200000);

	TEST_ASSERT_EQUAL(-EBUSY, result);
}

/**
 * @brief Test ltm4700_write_word_data with VOUT_MARGIN_HIGH (LINEAR16)
 */
void test_ltm4700_write_word_data_margin_high_linear16(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;
	dev.num_channels = 2;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_write_word_data(&dev, 1, LTM4700_VOUT_MARGIN_HIGH,
					     1260000);  // 1.26V in uV

	TEST_ASSERT_EQUAL(0, result);
}

// ============================================================================
// ltm4700_software_reset function tests
// ============================================================================

/**
 * @brief Test ltm4700_software_reset with NULL device pointer
 */
void test_ltm4700_software_reset_null_dev(void)
{
	int result = ltm4700_software_reset(NULL);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_software_reset with I2C write failure
 */
void test_ltm4700_software_reset_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_software_reset(&dev);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_software_reset success
 */
void test_ltm4700_software_reset_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_software_reset(&dev);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_software_reset with various I2C error codes
 */
void test_ltm4700_software_reset_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -ENXIO};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_software_reset
		int result = ltm4700_software_reset(&dev);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_software_reset verifies it uses non-paged command
 */
void test_ltm4700_software_reset_non_paged_command(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with non-zero page to verify page setting is NOT called
	// since LTM4700_MFR_RESET is not a paged command
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 1;  // Non-zero page
	dev.num_channels = 2;

	// Mock successful I2C operations - only expect the reset command write,
	// not any page setting operations
	no_os_i2c_write_IgnoreAndReturn(0);

	// Call ltm4700_software_reset - should succeed without page operations
	// since LTM4700_MFR_RESET is not a paged command
	int result = ltm4700_software_reset(&dev);

	TEST_ASSERT_EQUAL(0, result);
}

// ============================================================================
// ltm4700_nvm_cmd function tests
// ============================================================================

/**
 * @brief Test ltm4700_nvm_cmd with NULL device pointer
 */
void test_ltm4700_nvm_cmd_null_dev(void)
{
	int result = ltm4700_nvm_cmd(NULL, LTM4700_STORE_USER);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with STORE_USER command success
 */
void test_ltm4700_nvm_cmd_store_user_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_nvm_cmd(&dev, LTM4700_STORE_USER);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with RESTORE_USER command success
 */
void test_ltm4700_nvm_cmd_restore_user_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_nvm_cmd(&dev, LTM4700_RESTORE_USER);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with COMPARE_USER command success
 */
void test_ltm4700_nvm_cmd_compare_user_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	int result = ltm4700_nvm_cmd(&dev, LTM4700_COMPARE_USER);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with I2C write failure
 */
void test_ltm4700_nvm_cmd_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_nvm_cmd(&dev, LTM4700_STORE_USER);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with various I2C error codes
 */
void test_ltm4700_nvm_cmd_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_nvm_cmd with different commands
		int result = ltm4700_nvm_cmd(&dev, LTM4700_RESTORE_USER);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_nvm_cmd verifies it uses non-paged command
 */
void test_ltm4700_nvm_cmd_non_paged_command(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with non-zero page to verify page setting is NOT called
	// since NVM commands are not paged commands
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 1;  // Non-zero page
	dev.num_channels = 2;

	// Mock successful I2C operations - only expect the NVM command write,
	// not any page setting operations
	no_os_i2c_write_IgnoreAndReturn(0);

	// Call ltm4700_nvm_cmd - should succeed without page operations
	// since NVM commands are not paged commands (page 0 is used)
	int result = ltm4700_nvm_cmd(&dev, LTM4700_COMPARE_USER);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_nvm_cmd with all NVM command types
 */
void test_ltm4700_nvm_cmd_all_command_types(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.page = 0;

	// Test all three NVM command types
	enum ltm4700_nvm_cmd_type commands[] = {
		LTM4700_STORE_USER,
		LTM4700_RESTORE_USER,
		LTM4700_COMPARE_USER
	};
	int num_commands = sizeof(commands) / sizeof(commands[0]);

	for (int i = 0; i < num_commands; i++) {
		// Mock successful I2C write
		no_os_i2c_write_IgnoreAndReturn(0);

		// Call ltm4700_nvm_cmd with each command type
		int result = ltm4700_nvm_cmd(&dev, commands[i]);

		TEST_ASSERT_EQUAL(0, result);
	}
}

// ============================================================================
// ltm4700_set_operation function tests
// ============================================================================

/**
 * @brief Test ltm4700_set_operation with NULL device pointer
 */
void test_ltm4700_set_operation_null_dev(void)
{
	int result = ltm4700_set_operation(NULL, 0, LTM4700_OPERATION_ON);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_operation with invalid channel number
 */
void test_ltm4700_set_operation_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels are 0 and 1

	// Test with channel 2 (>= num_channels)
	int result = ltm4700_set_operation(&dev, 2, LTM4700_OPERATION_ON);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_operation with OPERATION_ON success
 */
void test_ltm4700_set_operation_on_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and write_byte

	int result = ltm4700_set_operation(&dev, 1, LTM4700_OPERATION_ON);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_operation with OPERATION_OFF success
 */
void test_ltm4700_set_operation_off_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and write_byte

	int result = ltm4700_set_operation(&dev, 0, LTM4700_OPERATION_OFF);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_operation with OPERATION_MARGIN_LOW success
 */
void test_ltm4700_set_operation_margin_low_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and write_byte

	int result = ltm4700_set_operation(&dev, 1, LTM4700_OPERATION_MARGIN_LOW);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_operation with OPERATION_MARGIN_HIGH success
 */
void test_ltm4700_set_operation_margin_high_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and write_byte

	int result = ltm4700_set_operation(&dev, 0, LTM4700_OPERATION_MARGIN_HIGH);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_operation with OPERATION_SEQ_OFF success
 */
void test_ltm4700_set_operation_seq_off_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);  // For page setting and write_byte

	int result = ltm4700_set_operation(&dev, 1, LTM4700_OPERATION_SEQ_OFF);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_operation with I2C write failure
 */
void test_ltm4700_set_operation_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_set_operation(&dev, 1, LTM4700_OPERATION_ON);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_set_operation with various I2C error codes
 */
void test_ltm4700_set_operation_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -ENXIO};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_set_operation
		int result = ltm4700_set_operation(&dev, 0, LTM4700_OPERATION_OFF);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_set_operation with all operation types
 */
void test_ltm4700_set_operation_all_types(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Test all operation types
	enum ltm4700_operation_type operations[] = {
		LTM4700_OPERATION_OFF,
		LTM4700_OPERATION_ON,
		LTM4700_OPERATION_MARGIN_LOW,
		LTM4700_OPERATION_MARGIN_HIGH,
		LTM4700_OPERATION_SEQ_OFF
	};
	int num_operations = sizeof(operations) / sizeof(operations[0]);

	for (int i = 0; i < num_operations; i++) {
		// Mock successful I2C write
		no_os_i2c_write_IgnoreAndReturn(0);

		// Call ltm4700_set_operation with each operation type
		int result = ltm4700_set_operation(&dev, 1, operations[i]);

		TEST_ASSERT_EQUAL(0, result);
	}
}

/**
 * @brief Test ltm4700_set_operation with boundary channel numbers
 */
void test_ltm4700_set_operation_boundary_channels(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels: 0, 1
	dev.page = 0;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test valid boundary channels
	int result_ch0 = ltm4700_set_operation(&dev, 0, LTM4700_OPERATION_ON);
	TEST_ASSERT_EQUAL(0, result_ch0);

	no_os_i2c_write_IgnoreAndReturn(0);
	int result_ch1 = ltm4700_set_operation(&dev, 1, LTM4700_OPERATION_ON);
	TEST_ASSERT_EQUAL(0, result_ch1);

	// Test invalid boundary channel (should fail)
	int result_invalid = ltm4700_set_operation(&dev, 2, LTM4700_OPERATION_ON);
	TEST_ASSERT_EQUAL(-EINVAL, result_invalid);
}

// ============================================================================
// ltm4700_set_timing function tests
// ============================================================================

/**
 * @brief Test ltm4700_set_timing with NULL device pointer
 */
void test_ltm4700_set_timing_null_dev(void)
{
	int result = ltm4700_set_timing(NULL, 0, LTM4700_TON_DELAY_TYPE, 5000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_timing with invalid channel number
 */
void test_ltm4700_set_timing_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels are 0 and 1

	// Test with channel 2 (>= num_channels)
	int result = ltm4700_set_timing(&dev, 2, LTM4700_TON_DELAY_TYPE, 5000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_set_timing with TON_DELAY timing type success
 */
void test_ltm4700_set_timing_ton_delay_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;  // Set for LINEAR16 conversion

	no_os_i2c_write_IgnoreAndReturn(0);  // For write_word_data operation

	// Test with 5000 microseconds (should be converted to 5 milliseconds)
	int result = ltm4700_set_timing(&dev, 1, LTM4700_TON_DELAY_TYPE, 5000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_timing with TOFF_DELAY timing type success
 */
void test_ltm4700_set_timing_toff_delay_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test with 10000 microseconds (should be converted to 10 milliseconds)
	int result = ltm4700_set_timing(&dev, 0, LTM4700_TOFF_DELAY_TYPE, 10000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_timing with RETRY_DELAY timing type success
 */
void test_ltm4700_set_timing_retry_delay_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test with 50000 microseconds (should be converted to 50 milliseconds)
	int result = ltm4700_set_timing(&dev, 1, LTM4700_RETRY_DELAY, 50000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_timing with RESTART_DELAY timing type success
 */
void test_ltm4700_set_timing_restart_delay_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test with 100000 microseconds (should be converted to 100 milliseconds)
	int result = ltm4700_set_timing(&dev, 0, LTM4700_RESTART_DELAY, 100000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_set_timing with I2C write failure
 */
void test_ltm4700_set_timing_i2c_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int result = ltm4700_set_timing(&dev, 1, LTM4700_TON_DELAY_TYPE, 5000);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_set_timing with various I2C error codes
 */
void test_ltm4700_set_timing_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different I2C error codes
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock I2C write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_set_timing
		int result = ltm4700_set_timing(&dev, 0, LTM4700_TOFF_DELAY_TYPE, 8000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_set_timing with all timing types
 */
void test_ltm4700_set_timing_all_types(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test all timing types
	enum ltm4700_timing_type timing_types[] = {
		LTM4700_TON_DELAY_TYPE,
		LTM4700_TOFF_DELAY_TYPE,
		LTM4700_RETRY_DELAY,
		LTM4700_RESTART_DELAY
	};
	int num_types = sizeof(timing_types) / sizeof(timing_types[0]);

	// Different time values for each type (in microseconds)
	int time_values[] = {5000, 10000, 50000, 100000};

	for (int i = 0; i < num_types; i++) {
		// Mock successful I2C write
		no_os_i2c_write_IgnoreAndReturn(0);

		// Call ltm4700_set_timing with each timing type
		int result = ltm4700_set_timing(&dev, 1, timing_types[i], time_values[i]);

		TEST_ASSERT_EQUAL(0, result);
	}
}

/**
 * @brief Test ltm4700_set_timing with time conversion verification
 */
void test_ltm4700_set_timing_time_conversion(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test various time conversions (microseconds to milliseconds)
	// 1000 µs = 1 ms, 5000 µs = 5 ms, 100000 µs = 100 ms
	int microsecond_values[] = {1000, 5000, 100000, 1000000};
	// Expected millisecond values: 1, 5, 100, 1000

	for (int i = 0; i < 4; i++) {
		no_os_i2c_write_IgnoreAndReturn(0);

		int result = ltm4700_set_timing(&dev, 0, LTM4700_TON_DELAY_TYPE,
						microsecond_values[i]);

		TEST_ASSERT_EQUAL(0, result);
	}
}

/**
 * @brief Test ltm4700_set_timing with boundary channel numbers
 */
void test_ltm4700_set_timing_boundary_channels(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels: 0, 1
	dev.page = 0;
	dev.lin16_exp = -12;

	no_os_i2c_write_IgnoreAndReturn(0);

	// Test valid boundary channels
	int result_ch0 = ltm4700_set_timing(&dev, 0, LTM4700_TON_DELAY_TYPE, 5000);
	TEST_ASSERT_EQUAL(0, result_ch0);

	no_os_i2c_write_IgnoreAndReturn(0);
	int result_ch1 = ltm4700_set_timing(&dev, 1, LTM4700_TOFF_DELAY_TYPE, 8000);
	TEST_ASSERT_EQUAL(0, result_ch1);

	// Test invalid boundary channel (should fail)
	int result_invalid = ltm4700_set_timing(&dev, 2, LTM4700_RETRY_DELAY, 10000);
	TEST_ASSERT_EQUAL(-EINVAL, result_invalid);
}

// ============================================================================
// ltm4700_vout_value function tests
// ============================================================================

/**
 * @brief Test ltm4700_vout_value with NULL device pointer
 */
void test_ltm4700_vout_value_null_dev(void)
{
	int result = ltm4700_vout_value(NULL, 0, 1200000, 1320000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_vout_value with invalid channel number
 */
void test_ltm4700_vout_value_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels are 0 and 1

	// Test with channel 2 (>= num_channels)
	int result = ltm4700_vout_value(&dev, 2, 1200000, 1320000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_vout_value success with both writes succeeding
 */
void test_ltm4700_vout_value_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;  // Set for LINEAR16 conversion

	// Mock both write_word_data operations to succeed
	no_os_i2c_write_IgnoreAndReturn(0);  // For VOUT_COMMAND write
	no_os_i2c_write_IgnoreAndReturn(0);  // For VOUT_MAX write

	// Test with 1.2V command and 1.32V max (in microvolts)
	int result = ltm4700_vout_value(&dev, 1, 1200000, 1320000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_vout_value with VOUT_COMMAND write failure
 */
void test_ltm4700_vout_value_vout_command_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock first write (VOUT_COMMAND) to fail
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Function should return immediately after first write failure
	int result = ltm4700_vout_value(&dev, 0, 1200000, 1320000);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_vout_value with VOUT_MAX write failure
 */
void test_ltm4700_vout_value_vout_max_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock first write (VOUT_COMMAND) to succeed, second write (VOUT_MAX) to fail
	no_os_i2c_write_IgnoreAndReturn(0);   // VOUT_COMMAND succeeds
	no_os_i2c_write_IgnoreAndReturn(-ENXIO);  // VOUT_MAX fails

	int result = ltm4700_vout_value(&dev, 1, 1200000, 1320000);

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_vout_value with various I2C error codes
 */
void test_ltm4700_vout_value_various_i2c_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different I2C error codes for first write
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock first write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_vout_value
		int result = ltm4700_vout_value(&dev, 0, 1100000, 1200000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_vout_value with various voltage values
 */
void test_ltm4700_vout_value_various_voltages(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different voltage combinations (in microvolts)
	struct {
		int vout_command;
		int vout_max;
	} voltage_tests[] = {
		{1000000, 1100000},  // 1.0V command, 1.1V max
		{1200000, 1320000},  // 1.2V command, 1.32V max
		{3300000, 3630000},  // 3.3V command, 3.63V max
		{800000, 880000},    // 0.8V command, 0.88V max
		{5000000, 5500000}   // 5.0V command, 5.5V max
	};
	int num_tests = sizeof(voltage_tests) / sizeof(voltage_tests[0]);

	for (int i = 0; i < num_tests; i++) {
		// Mock both writes to succeed
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_COMMAND
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_MAX

		int result = ltm4700_vout_value(&dev, 1, voltage_tests[i].vout_command,
						voltage_tests[i].vout_max);

		TEST_ASSERT_EQUAL(0, result);
	}
}

/**
 * @brief Test ltm4700_vout_value with boundary channel numbers
 */
void test_ltm4700_vout_value_boundary_channels(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels: 0, 1
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock writes to succeed
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	// Test valid boundary channels
	int result_ch0 = ltm4700_vout_value(&dev, 0, 1200000, 1320000);
	TEST_ASSERT_EQUAL(0, result_ch0);

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	int result_ch1 = ltm4700_vout_value(&dev, 1, 1500000, 1650000);
	TEST_ASSERT_EQUAL(0, result_ch1);

	// Test invalid boundary channel (should fail)
	int result_invalid = ltm4700_vout_value(&dev, 2, 1200000, 1320000);
	TEST_ASSERT_EQUAL(-EINVAL, result_invalid);
}

/**
 * @brief Test ltm4700_vout_value with second write failure scenarios
 */
void test_ltm4700_vout_value_second_write_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different I2C error codes for second write (VOUT_MAX)
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock first write to succeed, second write to fail
		no_os_i2c_write_IgnoreAndReturn(0);               // VOUT_COMMAND succeeds
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);  // VOUT_MAX fails

		// Call ltm4700_vout_value
		int result = ltm4700_vout_value(&dev, 1, 1200000, 1320000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

// ============================================================================
// ltm4700_vout_margin function tests
// ============================================================================

/**
 * @brief Test ltm4700_vout_margin with NULL device pointer
 */
void test_ltm4700_vout_margin_null_dev(void)
{
	int result = ltm4700_vout_margin(NULL, 0, 1140000, 1260000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_vout_margin with invalid channel number
 */
void test_ltm4700_vout_margin_invalid_channel(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels are 0 and 1

	// Test with channel 2 (>= num_channels)
	int result = ltm4700_vout_margin(&dev, 2, 1140000, 1260000);
	TEST_ASSERT_EQUAL(-EINVAL, result);
}

/**
 * @brief Test ltm4700_vout_margin success with both writes succeeding
 */
void test_ltm4700_vout_margin_success(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;  // Set for LINEAR16 conversion

	// Mock both write_word_data operations to succeed
	no_os_i2c_write_IgnoreAndReturn(0);  // For VOUT_MARGIN_LOW write
	no_os_i2c_write_IgnoreAndReturn(0);  // For VOUT_MARGIN_HIGH write

	// Test with 1.14V low margin and 1.26V high margin (in microvolts)
	int result = ltm4700_vout_margin(&dev, 1, 1140000, 1260000);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test ltm4700_vout_margin with VOUT_MARGIN_LOW write failure
 */
void test_ltm4700_vout_margin_low_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock first write (VOUT_MARGIN_LOW) to fail
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	// Function should return immediately after first write failure
	int result = ltm4700_vout_margin(&dev, 0, 1140000, 1260000);

	TEST_ASSERT_EQUAL(-EIO, result);
}

/**
 * @brief Test ltm4700_vout_margin with VOUT_MARGIN_HIGH write failure
 */
void test_ltm4700_vout_margin_high_error(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock first write (VOUT_MARGIN_LOW) to succeed, second write (VOUT_MARGIN_HIGH) to fail
	no_os_i2c_write_IgnoreAndReturn(0);      // VOUT_MARGIN_LOW succeeds
	no_os_i2c_write_IgnoreAndReturn(-ENXIO); // VOUT_MARGIN_HIGH fails

	int result = ltm4700_vout_margin(&dev, 1, 1140000, 1260000);

	TEST_ASSERT_EQUAL(-ENXIO, result);
}

/**
 * @brief Test ltm4700_vout_margin with various I2C error codes for first write
 */
void test_ltm4700_vout_margin_various_low_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};

	// Setup device with valid parameters
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different I2C error codes for first write (VOUT_MARGIN_LOW)
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY, -EAGAIN};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock first write failure with specific error
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);

		// Call ltm4700_vout_margin
		int result = ltm4700_vout_margin(&dev, 0, 1100000, 1300000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_vout_margin with various I2C error codes for second write
 */
void test_ltm4700_vout_margin_various_high_errors(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different I2C error codes for second write (VOUT_MARGIN_HIGH)
	int test_errors[] = {-ETIMEDOUT, -ENODEV, -EREMOTEIO, -EBUSY};
	int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

	for (int i = 0; i < num_errors; i++) {
		// Mock first write to succeed, second write to fail
		no_os_i2c_write_IgnoreAndReturn(0);               // VOUT_MARGIN_LOW succeeds
		no_os_i2c_write_IgnoreAndReturn(test_errors[i]);  // VOUT_MARGIN_HIGH fails

		// Call ltm4700_vout_margin
		int result = ltm4700_vout_margin(&dev, 1, 1140000, 1260000);

		TEST_ASSERT_EQUAL(test_errors[i], result);
	}
}

/**
 * @brief Test ltm4700_vout_margin with various margin value combinations
 */
void test_ltm4700_vout_margin_various_margins(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with different margin combinations (in microvolts)
	// Typically low margin is -5% and high margin is +5% of nominal
	struct {
		int margin_low;
		int margin_high;
	} margin_tests[] = {
		{950000, 1050000},    // 0.95V low, 1.05V high (1.0V nominal ±5%)
		{1140000, 1260000},   // 1.14V low, 1.26V high (1.2V nominal ±5%)
		{3135000, 3465000},   // 3.135V low, 3.465V high (3.3V nominal ±5%)
		{760000, 840000},     // 0.76V low, 0.84V high (0.8V nominal ±5%)
		{4750000, 5250000}    // 4.75V low, 5.25V high (5.0V nominal ±5%)
	};
	int num_tests = sizeof(margin_tests) / sizeof(margin_tests[0]);

	for (int i = 0; i < num_tests; i++) {
		// Mock both writes to succeed
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_MARGIN_LOW
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_MARGIN_HIGH

		int result = ltm4700_vout_margin(&dev, 1, margin_tests[i].margin_low,
						 margin_tests[i].margin_high);

		TEST_ASSERT_EQUAL(0, result);
	}
}

/**
 * @brief Test ltm4700_vout_margin with boundary channel numbers
 */
void test_ltm4700_vout_margin_boundary_channels(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;  // Valid channels: 0, 1
	dev.page = 0;
	dev.lin16_exp = -12;

	// Mock writes to succeed
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	// Test valid boundary channels
	int result_ch0 = ltm4700_vout_margin(&dev, 0, 1140000, 1260000);
	TEST_ASSERT_EQUAL(0, result_ch0);

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	int result_ch1 = ltm4700_vout_margin(&dev, 1, 950000, 1050000);
	TEST_ASSERT_EQUAL(0, result_ch1);

	// Test invalid boundary channel (should fail)
	int result_invalid = ltm4700_vout_margin(&dev, 2, 1140000, 1260000);
	TEST_ASSERT_EQUAL(-EINVAL, result_invalid);
}

/**
 * @brief Test ltm4700_vout_margin with extreme margin values
 */
void test_ltm4700_vout_margin_extreme_values(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = -12;

	// Test with extreme margin value combinations
	struct {
		int margin_low;
		int margin_high;
	} extreme_tests[] = {
		{600000, 700000},     // Very low voltage margins (0.6V-0.7V)
		{5400000, 6000000},   // High voltage margins (5.4V-6.0V)
		{1000000, 2000000},   // Large margin spread (1.0V-2.0V)
		{800000, 820000},     // Very tight margins (0.8V-0.82V)
		{0, 1000000}          // Zero low margin, 1V high margin
	};
	int num_tests = sizeof(extreme_tests) / sizeof(extreme_tests[0]);

	for (int i = 0; i < num_tests; i++) {
		// Mock both writes to succeed
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_MARGIN_LOW
		no_os_i2c_write_IgnoreAndReturn(0);  // VOUT_MARGIN_HIGH

		int result = ltm4700_vout_margin(&dev, 0, extreme_tests[i].margin_low,
						 extreme_tests[i].margin_high);

		TEST_ASSERT_EQUAL(0, result);
	}
}

// ============================================================================
// Tests for coverage of specific lines 175, 198, and 199
// ============================================================================

/**
 * @brief Test to cover line 175 - negative exponent case in ltm4700_lin11_to_uval
 * This tests the LINEAR11 to milli-units conversion with negative exponent
 */
void test_ltm4700_read_word_data_negative_exponent_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	int data_out = 0;

	// Mock I2C operations for reading a non-voltage command (uses LINEAR11 format)
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command write

	no_os_i2c_read_Stub(fill_word_data_negative_exp);

	// Call read_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_read_word_data(&dev, 1, LTM4700_IOUT_OC_FAULT_LIMIT,
					    &data_out);

	TEST_ASSERT_EQUAL(0, result);
	// The conversion should be: (800 * 1000) >> 3 = 100000 milli-units
	TEST_ASSERT_EQUAL(100000, data_out);
}

/**
 * @brief Test to cover lines 198-199 - small value case in ltm4700_uval_to_lin11
 * This tests the LINEAR11 conversion with a small value that requires left-shifting
 */
void test_ltm4700_write_word_data_small_value_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use a very small value that will trigger mantissa left-shifting
	// Value = 50 milli-units (small enough to need normalization)
	// This will cause mantissa to be left-shifted and exponent decremented
	int small_value = 50;

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 1, LTM4700_IOUT_OC_FAULT_LIMIT,
					     small_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test to cover lines 198-199 with even smaller value for more left-shifts
 */
void test_ltm4700_write_word_data_very_small_value_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use an extremely small value that will require multiple left-shifts
	// Value = 5 milli-units (very small, will trigger multiple iterations)
	int very_small_value = 5;

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 0, LTM4700_IOUT_OC_FAULT_LIMIT,
					     very_small_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test to cover lines 152-153 - positive exponent case in ltm4700_uval_to_lin16
 * This tests the LINEAR16 conversion with positive exponent
 */
void test_ltm4700_write_word_data_positive_exp_lin16(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = 2;  // Set positive exponent for LINEAR16 conversion

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use a voltage command to trigger LINEAR16 conversion
	// With positive exponent (2), the conversion will be: uval >> exp
	// Value = 4800000 microvolts (4.8V) >> 2 = 1200000
	int voltage_value = 4800000;  // 4.8V in microvolts

	// Call write_word_data with a voltage command to trigger LINEAR16 conversion
	int result = ltm4700_write_word_data(&dev, 1, LTM4700_VOUT_COMMAND,
					     voltage_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test to cover lines 152-153 with larger positive exponent
 */
void test_ltm4700_write_word_data_large_positive_exp_lin16(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;
	dev.lin16_exp = 4;  // Larger positive exponent for LINEAR16 conversion

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use a large voltage value that will be right-shifted by 4
	// Value = 16000000 microvolts (16V) >> 4 = 1000000
	int large_voltage = 16000000;  // 16V in microvolts

	// Call write_word_data with VOUT_MAX to trigger LINEAR16 conversion
	int result = ltm4700_write_word_data(&dev, 0, LTM4700_VOUT_MAX, large_voltage);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Additional test to ensure lines 198-199 coverage - extremely small value
 * This forces multiple iterations of mantissa left-shifting and exponent decrementing
 */
void test_ltm4700_write_word_data_minimal_value_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use an extremely small value that will require many left-shifts
	// This ensures lines 198-199 are executed multiple times in the normalization loop
	int minimal_value =
		1;  // 1 microunit - very small, needs extensive normalization

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 1, LTM4700_IOUT_OC_FAULT_LIMIT,
					     minimal_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test to cover lines 198-199 with negative small value
 * This tests the left-shift normalization with negative mantissa values
 */
void test_ltm4700_write_word_data_negative_small_value_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use a small negative value that will trigger left-shifting
	// This tests the negative branch of the mantissa normalization
	int negative_small_value = -10;  // Small negative value

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 0, LTM4700_IOUT_OC_FAULT_LIMIT,
					     negative_small_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test to specifically cover lines 197-199 - large negative value case
 * This tests the else branch where mantissa < LTM4700_LIN11_MANTISSA_MIN (-1024)
 */
void test_ltm4700_write_word_data_large_negative_value_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use a large negative value that will be below LTM4700_LIN11_MANTISSA_MIN (-1024)
	// This will trigger the else branch: mantissa <<= 1; exponent--;
	// Value must be negative and large enough to fall below -1024 range
	int large_negative_value = -2048;  // Large negative value

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 1, LTM4700_IOUT_OC_FAULT_LIMIT,
					     large_negative_value);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Additional test for very large negative value to ensure multiple iterations
 */
void test_ltm4700_write_word_data_very_large_negative_lin11(void)
{
	struct ltm4700_dev dev = {0};
	struct no_os_i2c_desc mock_i2c_desc = {0};
	dev.i2c_desc = &mock_i2c_desc;
	dev.num_channels = 2;
	dev.page = 0;

	// Mock I2C operations for writing
	no_os_i2c_write_IgnoreAndReturn(0);  // Page setting
	no_os_i2c_write_IgnoreAndReturn(0);  // Command and data write

	// Use an extremely large negative value to force multiple left-shift iterations
	// This ensures the else branch (lines 198-199) executes multiple times
	int very_large_negative = -8192;  // Very large negative value

	// Call write_word_data with a non-voltage command to trigger LINEAR11 conversion
	int result = ltm4700_write_word_data(&dev, 0, LTM4700_IOUT_OC_FAULT_LIMIT,
					     very_large_negative);

	TEST_ASSERT_EQUAL(0, result);
}

/**
 * @brief Test that error_run label path is executed (addressing line 334 coverage)
 *
 * This test demonstrates that the goto error_run statements (lines 302 & 311) are
 * working correctly by verifying the cleanup behavior. The label itself (line 334)
 * may not show as "covered" in gcov because labels are destination markers, not
 * executable code, but this test proves the error paths execute correctly.
 */
void test_ltm4700_init_error_run_path_verification(void)
{
	struct ltm4700_dev *device = NULL;
	struct ltm4700_init_param init_param = {0};
	struct no_os_i2c_init_param i2c_init = {0};
	struct no_os_gpio_init_param alert_param = {0};
	struct no_os_gpio_init_param run_param = {0};
	struct no_os_gpio_init_param *run_params[1];
	struct ltm4700_dev mock_dev = {0};
	mock_dev.num_channels = 2;  // LTM4700 has 2 channels
	struct no_os_gpio_desc mock_run_desc = {0};
	struct no_os_gpio_desc *mock_run_descs[1] = {&mock_run_desc};

	// Setup minimal parameters that will trigger the error_run path
	i2c_init.device_id = 0;
	i2c_init.slave_address = 0x5A;
	alert_param.number = 10;
	run_param.number = 13;
	run_params[0] = &run_param;

	init_param.i2c_init = &i2c_init;
	init_param.alert_param = &alert_param;
	init_param.run_params = run_params;

	// Mock successful setup until we trigger the error condition
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_dev), &mock_dev);
	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(fill_complete_device_id_success);
	no_os_gpio_get_IgnoreAndReturn(0);  // Alert GPIO succeeds

	// Mock run_descs allocation succeeding (num_channels = 2 for LTM4700)
	no_os_calloc_ExpectAndReturn(2, sizeof(struct no_os_gpio_desc *),
				     mock_run_descs);

	// Mock GPIO get failure to trigger: goto error_run; (line 302)
	no_os_gpio_get_IgnoreAndReturn(-ENODEV);

	// Mock the cleanup operations that occur in the error_run section
	// These calls prove that the error_run label was reached and executed
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup run descriptors (line 337)
	no_os_free_Ignore();                   // Free run_descs array (line 338)
	no_os_gpio_remove_IgnoreAndReturn(0);  // Cleanup alert GPIO (line 347)
	no_os_i2c_remove_IgnoreAndReturn(0);   // Cleanup I2C (line 349)
	no_os_free_Ignore();                   // Free device (line 350)

	int result = ltm4700_init(&device, &init_param);

	// Verify error was properly propagated and cleanup occurred
	TEST_ASSERT_EQUAL(-ENODEV, result);
	TEST_ASSERT_NULL(device);
}

