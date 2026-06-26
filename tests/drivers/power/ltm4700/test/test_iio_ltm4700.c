/***************************************************************************//**
 *   @file   test_iio_ltm4700.c
 *   @brief  Unit tests for LTM4700 IIO Driver (iio_ltm4700.c)
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
 *    TEST FILE ORGANIZATION
 ******************************************************************************/
/*
 * This test file is organized into the following main sections:
 *
 * 1. INCLUDED FILES & DEFINITIONS
 * 2. TEST SUPPORT VARIABLES & MOCK FUNCTIONS
 * 3. SETUP/TEARDOWN FUNCTIONS
 * 4. INIT/REMOVE TESTS
 * 5. DEBUG REGISTER READ TESTS (ltm4700_iio_reg_read)
 *    - Word register reads (standard registers)
 *    - Block data reads (MFR_ID, MFR_MODEL)
 *    - Byte register reads (default case)
 * 6. DEBUG REGISTER WRITE TESTS (ltm4700_iio_reg_write)
 *    - Send byte commands (CLEAR_FAULTS, etc.)
 *    - Word register writes
 *    - Byte register writes (default case)
 * 7. TELEMETRY CHANNEL READ TESTS (ltm4700_iio_read_attr)
 *    - Success Path Tests (VIN, IIN, VOUT, IOUT, TEMP_EXT, TEMP_IC, FREQ, POUT, PIN)
 *    - Configuration reads (VOUT_COMMAND, VOUT_MAX, MARGIN_HIGH, MARGIN_LOW)
 *    - Operation mode reads
 *    - Device ID reads
 *    - Driver Failure Tests
 * 8. CONFIGURATION CHANNEL WRITE TESTS (ltm4700_iio_write_attr)
 *    - Success Path Tests (VOUT_COMMAND, VOUT_MAX, MARGIN_HIGH, MARGIN_LOW, OPERATION)
 *    - Clear peaks command
 *    - Error Tests (invalid format, invalid channel, driver failures)
 * 9. AVAILABLE ATTRIBUTES TESTS (ltm4700_iio_read_avail)
 * 10. ERROR/EDGE CASE TESTS
 */

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "unity.h"
#include "iio_ltm4700.h"
#include "mock_ltm4700.h"
#include "mock_no_os_alloc.h"
#include "ltm4700.h"
#include "iio_types.h"
#include "no_os_util.h"
#include <string.h>
#include <errno.h>

/*******************************************************************************
 *    PRIVATE VARIABLES
 ******************************************************************************/

static struct ltm4700_iio_desc *test_iio_desc;
static struct ltm4700_iio_desc_init_param test_iio_init_param;
static struct ltm4700_init_param test_ltm4700_init_param;
static struct ltm4700_dev test_ltm4700_dev;

/* Mock memory allocation buffer */
static char mock_memory[512] = {0};

/* Test data storage */
static uint16_t mock_word_value;
static uint8_t mock_byte_value;
static int mock_int_value;

/* External declarations for static functions (requires TEST defined in build) */
extern int ltm4700_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
extern int ltm4700_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);
extern int ltm4700_iio_read_attr(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
extern int ltm4700_iio_write_attr(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
extern int ltm4700_iio_read_avail(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

/*******************************************************************************
 *    MOCK CALLBACK FUNCTIONS
 ******************************************************************************/

/* Mock callback for no_os_calloc - returns pre-allocated buffer */
static void *mock_calloc_callback(size_t nitems, size_t size, int num_calls)
{
	memset(mock_memory, 0, sizeof(mock_memory));
	return (void *)mock_memory;
}

/* Mock callback for ltm4700_init - sets device pointer */
static int mock_ltm4700_init_callback(struct ltm4700_dev **device,
				      struct ltm4700_init_param *init_param,
				      int num_calls)
{
	if (device) {
		*device = &test_ltm4700_dev;
	}
	return 0;
}

/* Mock callback for read_block_data - fills buffer with test data */
static int mock_read_block_data_callback(struct ltm4700_dev *dev,
		uint8_t page,
		uint8_t reg,
		uint8_t *data,
		size_t nbytes,
		int num_calls)
{
	if (reg == LTM4700_MFR_ID) {
		/* Return "ADI" + padding */
		data[0] = 'A';
		data[1] = 'D';
		data[2] = 'I';
		data[3] = 0;
		return 0;
	} else if (reg == LTM4700_MFR_MODEL) {
		/* Return "LTM4700" */
		data[0] = 'L';
		data[1] = 'T';
		data[2] = 'M';
		data[3] = '4';
		return 0;
	}
	return -EIO;
}

/* Mock callback for read_word - returns test value */
static int mock_read_word_callback(struct ltm4700_dev *dev, uint8_t page,
				   uint8_t reg, uint16_t *word,
				   int num_calls)
{
	if (word) {
		*word = mock_word_value;
	}
	return 0;
}

/* Mock callback for read_byte - returns test value */
static int mock_read_byte_callback(struct ltm4700_dev *dev, uint8_t page,
				   uint8_t reg, uint8_t *data,
				   int num_calls)
{
	if (data) {
		*data = mock_byte_value;
	}
	return 0;
}

/* Mock callback for read_value - returns test value */
static int mock_read_value_callback(struct ltm4700_dev *dev, uint8_t channel,
				    enum ltm4700_value_type value_type,
				    int *val, int num_calls)
{
	if (val) {
		*val = mock_int_value;
	}
	return 0;
}

/* Mock callback for read_word_data - returns test value */
static int mock_read_word_data_callback(struct ltm4700_dev *dev,
					uint8_t channel, uint8_t reg,
					int *data, int num_calls)
{
	if (data) {
		*data = mock_int_value;
	}
	return 0;
}

/*******************************************************************************
 *    SETUP/TEARDOWN FUNCTIONS
 ******************************************************************************/

void setUp(void)
{
	/* Reset test structures */
	memset(&test_iio_desc, 0, sizeof(test_iio_desc));
	memset(&test_iio_init_param, 0, sizeof(test_iio_init_param));
	memset(&test_ltm4700_init_param, 0, sizeof(test_ltm4700_init_param));
	memset(&test_ltm4700_dev, 0, sizeof(test_ltm4700_dev));
	memset(mock_memory, 0, sizeof(mock_memory));

	/* Reset mock values */
	mock_word_value = 0;
	mock_byte_value = 0;
	mock_int_value = 0;
}

void tearDown(void)
{
	/* Cleanup after each test */
}

/*******************************************************************************
 *    INIT/REMOVE TESTS
 ******************************************************************************/

/**
 * @brief Test ltm4700_iio_init with valid parameters
 *
 * Verifies:
 * - Memory allocation succeeds
 * - ltm4700_init is called
 * - Descriptor is properly initialized
 */
void test_ltm4700_iio_init_success(void)
{
	struct ltm4700_iio_desc *iio_desc = NULL;
	int ret;

	/* Setup init parameters */
	test_iio_init_param.ltm4700_init_param = &test_ltm4700_init_param;

	/* Mock expectations */
	no_os_calloc_Stub(mock_calloc_callback);
	ltm4700_init_Stub(mock_ltm4700_init_callback);

	/* Execute */
	ret = ltm4700_iio_init(&iio_desc, &test_iio_init_param);

	/* Verify */
	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_NOT_NULL(iio_desc);
	TEST_ASSERT_NOT_NULL(iio_desc->iio_dev);
	TEST_ASSERT_EQUAL_PTR(&test_ltm4700_dev, iio_desc->ltm4700_dev);
}

/**
 * @brief Test ltm4700_iio_init with NULL iio_desc parameter
 */
void test_ltm4700_iio_init_null_iio_desc(void)
{
	int ret;

	test_iio_init_param.ltm4700_init_param = &test_ltm4700_init_param;

	ret = ltm4700_iio_init(NULL, &test_iio_init_param);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test ltm4700_iio_init with NULL init_param
 */
void test_ltm4700_iio_init_null_init_param(void)
{
	struct ltm4700_iio_desc *iio_desc = NULL;
	int ret;

	ret = ltm4700_iio_init(&iio_desc, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test ltm4700_iio_init with NULL ltm4700_init_param
 */
void test_ltm4700_iio_init_null_ltm4700_init_param(void)
{
	struct ltm4700_iio_desc *iio_desc = NULL;
	int ret;

	test_iio_init_param.ltm4700_init_param = NULL;

	ret = ltm4700_iio_init(&iio_desc, &test_iio_init_param);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test ltm4700_iio_init with memory allocation failure
 */
void test_ltm4700_iio_init_calloc_failure(void)
{
	struct ltm4700_iio_desc *iio_desc = NULL;
	int ret;

	test_iio_init_param.ltm4700_init_param = &test_ltm4700_init_param;

	/* Mock calloc failure */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltm4700_iio_desc), NULL);

	ret = ltm4700_iio_init(&iio_desc, &test_iio_init_param);

	TEST_ASSERT_EQUAL(-ENOMEM, ret);
}

/**
 * @brief Test ltm4700_iio_init with ltm4700_init failure
 */
void test_ltm4700_iio_init_driver_init_failure(void)
{
	struct ltm4700_iio_desc *iio_desc = NULL;
	int ret;

	test_iio_init_param.ltm4700_init_param = &test_ltm4700_init_param;

	/* Mock calloc success but driver init failure */
	no_os_calloc_Stub(mock_calloc_callback);
	ltm4700_init_IgnoreAndReturn(-EIO);
	no_os_free_Ignore();

	ret = ltm4700_iio_init(&iio_desc, &test_iio_init_param);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test ltm4700_iio_remove with valid descriptor
 */
void test_ltm4700_iio_remove_success(void)
{
	struct ltm4700_iio_desc iio_desc;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ltm4700_remove_ExpectAndReturn(&test_ltm4700_dev, 0);
	no_os_free_Ignore();

	ret = ltm4700_iio_remove(&iio_desc);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test ltm4700_iio_remove with NULL descriptor
 */
void test_ltm4700_iio_remove_null_desc(void)
{
	int ret;

	ret = ltm4700_iio_remove(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test ltm4700_iio_remove with ltm4700_remove failure
 */
void test_ltm4700_iio_remove_driver_failure(void)
{
	struct ltm4700_iio_desc iio_desc;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ltm4700_remove_ExpectAndReturn(&test_ltm4700_dev, -EIO);

	ret = ltm4700_iio_remove(&iio_desc);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/*******************************************************************************
 *    DEBUG REGISTER READ TESTS
 ******************************************************************************/

/**
 * @brief Test ltm4700_iio_reg_read for word registers
 *
 * Tests reading standard PMBus word registers (16-bit)
 */
void test_ltm4700_iio_reg_read_word_register(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint32_t readval = 0;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;
	mock_word_value = 0x1234;

	/* Mock read_word to return test value */
	ltm4700_read_word_Stub(mock_read_word_callback);

	ret = ltm4700_iio_reg_read(&iio_desc, LTM4700_VOUT_COMMAND, &readval);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL_UINT32(0x1234, readval);
}

/**
 * @brief Test ltm4700_iio_reg_read for MFR_ID (block data)
 */
void test_ltm4700_iio_reg_read_mfr_id(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint32_t readval = 0;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock block data read for "ADI" */
	ltm4700_read_block_data_Stub(mock_read_block_data_callback);

	ret = ltm4700_iio_reg_read(&iio_desc, LTM4700_MFR_ID, &readval);

	TEST_ASSERT_EQUAL(0, ret);
	/* Verify "ADI" is encoded properly (big-endian) */
	TEST_ASSERT_EQUAL_UINT32(0x41444900, readval);  /* 'A' 'D' 'I' 0x00 */
}

/**
 * @brief Test ltm4700_iio_reg_read for MFR_MODEL (block data)
 */
void test_ltm4700_iio_reg_read_mfr_model(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint32_t readval = 0;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock block data read for "LTM4" (first 4 bytes) */
	ltm4700_read_block_data_Stub(mock_read_block_data_callback);

	ret = ltm4700_iio_reg_read(&iio_desc, LTM4700_MFR_MODEL, &readval);

	TEST_ASSERT_EQUAL(0, ret);
	/* Verify "LTM4" is encoded properly (big-endian) */
	TEST_ASSERT_EQUAL_UINT32(0x4C544D34, readval);  /* 'L' 'T' 'M' '4' */
}

/**
 * @brief Test ltm4700_iio_reg_read for block data with driver failure
 */
void test_ltm4700_iio_reg_read_block_data_failure(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint32_t readval = 0;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock block data read failure */
	ltm4700_read_block_data_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_reg_read(&iio_desc, LTM4700_MFR_ID, &readval);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test ltm4700_iio_reg_read for byte registers (default case)
 */
void test_ltm4700_iio_reg_read_byte_register(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint32_t readval = 0;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;
	mock_byte_value = 0xAB;

	/* Mock read_byte to return test value */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_reg_read(&iio_desc, LTM4700_OPERATION, &readval);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL_UINT32(0xAB, readval);
}

/*******************************************************************************
 *    DEBUG REGISTER WRITE TESTS
 ******************************************************************************/

/**
 * @brief Test ltm4700_iio_reg_write for send byte commands
 */
void test_ltm4700_iio_reg_write_send_byte_command(void)
{
	struct ltm4700_iio_desc iio_desc;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock send_byte */
	ltm4700_send_byte_IgnoreAndReturn(0);

	ret = ltm4700_iio_reg_write(&iio_desc, LTM4700_CLEAR_FAULTS, 0);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test ltm4700_iio_reg_write for word registers
 */
void test_ltm4700_iio_reg_write_word_register(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint16_t test_value = 0x1234;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock write_word */
	ltm4700_write_word_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_VOUT_COMMAND, test_value, 0);

	ret = ltm4700_iio_reg_write(&iio_desc, LTM4700_VOUT_COMMAND, test_value);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test ltm4700_iio_reg_write for byte registers (default case)
 */
void test_ltm4700_iio_reg_write_byte_register(void)
{
	struct ltm4700_iio_desc iio_desc;
	uint8_t test_value = 0x80;
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	test_ltm4700_dev.page = 0;

	/* Mock write_byte */
	ltm4700_write_byte_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_OPERATION, test_value, 0);

	ret = ltm4700_iio_reg_write(&iio_desc, LTM4700_OPERATION, test_value);

	TEST_ASSERT_EQUAL(0, ret);
}

/*******************************************************************************
 *    TELEMETRY CHANNEL READ TESTS
 ******************************************************************************/

/**
 * @brief Test reading VIN attribute
 */
void test_ltm4700_iio_read_attr_vin(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 12345;  /* 12.345V in mV */

	/* Mock read_value to return 12.345V (12345 mV) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VIN);

	TEST_ASSERT_EQUAL(6, ret);  /* "12.345" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("12.345", buf);
}

/**
 * @brief Test reading IIN attribute
 */
void test_ltm4700_iio_read_attr_iin(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 5678;  /* 5.678A in mA */

	/* Mock read_value to return 5.678A (5678 mA) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_IIN);

	TEST_ASSERT_EQUAL(5, ret);  /* "5.678" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("5.678", buf);
}

/**
 * @brief Test reading VOUT attribute for channel 0
 */
void test_ltm4700_iio_read_attr_vout_ch0(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 3300;  /* 3.300V in mV */

	/* Mock read_value to return 3.300V (3300 mV) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT);

	TEST_ASSERT_EQUAL(5, ret);  /* "3.300" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("3.300", buf);
}

/**
 * @brief Test reading IOUT attribute for channel 1
 */
void test_ltm4700_iio_read_attr_iout_ch1(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 1};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 10500;  /* 10.500A in mA */

	/* Mock read_value to return 10.500A (10500 mA) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_IOUT);

	TEST_ASSERT_EQUAL(6, ret);  /* "10.500" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("10.500", buf);
}

/**
 * @brief Test reading TEMP_EXT attribute
 */
void test_ltm4700_iio_read_attr_temp_ext(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 45123;  /* 45.123°C in m°C */

	/* Mock read_value to return 45.123°C (45123 m°C) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_TEMP_EXT);

	TEST_ASSERT_EQUAL(6, ret);  /* "45.123" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("45.123", buf);
}

/**
 * @brief Test reading TEMP_IC attribute
 */
void test_ltm4700_iio_read_attr_temp_ic(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 50000;  /* 50.000°C in m°C */

	/* Mock read_value to return 50.000°C (50000 m°C) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_TEMP_IC);

	TEST_ASSERT_EQUAL(6, ret);  /* "50.000" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("50.000", buf);
}

/**
 * @brief Test reading FREQ attribute
 */
void test_ltm4700_iio_read_attr_freq(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 500000;  /* 500000 Hz */

	/* Mock read_value to return 500000 Hz */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_FREQ);

	TEST_ASSERT_EQUAL(6, ret);  /* "500000" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("500000", buf);
}

/**
 * @brief Test reading POUT attribute
 */
void test_ltm4700_iio_read_attr_pout(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 25500;  /* 25.500W in mW */

	/* Mock read_value to return 25.500W (25500 mW) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_POUT);

	TEST_ASSERT_EQUAL(6, ret);  /* "25.500" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("25.500", buf);
}

/**
 * @brief Test reading PIN attribute
 */
void test_ltm4700_iio_read_attr_pin(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 30250;  /* 30.250W in mW */

	/* Mock read_value to return 30.250W (30250 mW) */
	ltm4700_read_value_Stub(mock_read_value_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_PIN);

	TEST_ASSERT_EQUAL(6, ret);  /* "30.250" is 6 characters */
	TEST_ASSERT_EQUAL_STRING("30.250", buf);
}

/**
 * @brief Test reading VOUT_COMMAND attribute
 */
void test_ltm4700_iio_read_attr_vout_command(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 3300;  /* 3.300V in mV */

	/* Mock read_word_data to return 3.300V (3300 mV) */
	ltm4700_read_word_data_Stub(mock_read_word_data_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT_COMMAND);

	TEST_ASSERT_EQUAL(5, ret);  /* "3.300" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("3.300", buf);
}

/**
 * @brief Test reading VOUT_MAX attribute
 */
void test_ltm4700_iio_read_attr_vout_max(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 3600;  /* 3.600V in mV */

	/* Mock read_word_data to return 3.600V (3600 mV) */
	ltm4700_read_word_data_Stub(mock_read_word_data_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT_MAX);

	TEST_ASSERT_EQUAL(5, ret);  /* "3.600" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("3.600", buf);
}

/**
 * @brief Test reading VOUT_MARGIN_HIGH attribute
 */
void test_ltm4700_iio_read_attr_vout_margin_high(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 3465;  /* 3.465V in mV */

	/* Mock read_word_data to return 3.465V (3465 mV) */
	ltm4700_read_word_data_Stub(mock_read_word_data_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT_MARGIN_HIGH);

	TEST_ASSERT_EQUAL(5, ret);  /* "3.465" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("3.465", buf);
}

/**
 * @brief Test reading VOUT_MARGIN_LOW attribute
 */
void test_ltm4700_iio_read_attr_vout_margin_low(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_int_value = 3135;  /* 3.135V in mV */

	/* Mock read_word_data to return 3.135V (3135 mV) */
	ltm4700_read_word_data_Stub(mock_read_word_data_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT_MARGIN_LOW);

	TEST_ASSERT_EQUAL(5, ret);  /* "3.135" is 5 characters */
	TEST_ASSERT_EQUAL_STRING("3.135", buf);
}

/**
 * @brief Test reading OPERATION attribute (OFF state)
 */
void test_ltm4700_iio_read_attr_operation_off(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_byte_value = LTM4700_OPERATION_OFF;

	/* Mock read_byte to return OFF state */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(3, ret);  /* "off" is 3 characters */
	TEST_ASSERT_EQUAL_STRING("off", buf);
}

/**
 * @brief Test reading OPERATION attribute (ON state)
 */
void test_ltm4700_iio_read_attr_operation_on(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_byte_value = LTM4700_OPERATION_ON;

	/* Mock read_byte to return ON state */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(2, ret);  /* "on" is 2 characters */
	TEST_ASSERT_EQUAL_STRING("on", buf);
}

/**
 * @brief Test reading OPERATION attribute (MARGIN_HIGH state)
 */
void test_ltm4700_iio_read_attr_operation_margin_high(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_byte_value = LTM4700_OPERATION_MARGIN_HIGH;

	/* Mock read_byte to return MARGIN_HIGH state */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(11, ret);  /* "margin_high" is 11 characters */
	TEST_ASSERT_EQUAL_STRING("margin_high", buf);
}

/**
 * @brief Test reading OPERATION attribute (MARGIN_LOW state)
 */
void test_ltm4700_iio_read_attr_operation_margin_low(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_byte_value = LTM4700_OPERATION_MARGIN_LOW;

	/* Mock read_byte to return MARGIN_LOW state */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(10, ret);  /* "margin_low" is 10 characters */
	TEST_ASSERT_EQUAL_STRING("margin_low", buf);
}

/**
 * @brief Test reading OPERATION attribute (unknown state)
 */
void test_ltm4700_iio_read_attr_operation_unknown(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_byte_value = 0xFF;  /* Invalid operation value */

	/* Mock read_byte to return unknown state */
	ltm4700_read_byte_Stub(mock_read_byte_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(7, ret);  /* "unknown" is 7 characters */
	TEST_ASSERT_EQUAL_STRING("unknown", buf);
}

/**
 * @brief Test reading DEVICE_ID attribute (LTM4700 device)
 */
void test_ltm4700_iio_read_attr_device_id_ltm4700(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;
	mock_word_value = LTM4700_SPECIAL_ID_VALUE;

	/* Mock read_word to return LTM4700 special ID */
	ltm4700_read_word_Stub(mock_read_word_callback);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_DEVICE_ID);

	TEST_ASSERT_EQUAL(7, ret);  /* "LTM4700" is 7 characters */
	TEST_ASSERT_EQUAL_STRING("LTM4700", buf);
}

/**
 * @brief Test reading attribute with driver failure (VIN)
 */
void test_ltm4700_iio_read_attr_driver_failure_vin(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_value failure */
	ltm4700_read_value_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VIN);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with driver failure (TEMP_EXT)
 */
void test_ltm4700_iio_read_attr_driver_failure_temp_ext(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_value failure */
	ltm4700_read_value_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_TEMP_EXT);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with driver failure (FREQ)
 */
void test_ltm4700_iio_read_attr_driver_failure_freq(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_value failure */
	ltm4700_read_value_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_FREQ);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with driver failure (VOUT_COMMAND)
 */
void test_ltm4700_iio_read_attr_driver_failure_vout_command(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_word_data failure */
	ltm4700_read_word_data_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_VOUT_COMMAND);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with driver failure (OPERATION)
 */
void test_ltm4700_iio_read_attr_driver_failure_operation(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_byte failure */
	ltm4700_read_byte_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with driver failure (DEVICE_ID)
 */
void test_ltm4700_iio_read_attr_driver_failure_device_id(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Mock read_word failure */
	ltm4700_read_word_IgnoreAndReturn(-EIO);

	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel,
				    LTM4700_IIO_DEVICE_ID);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test reading attribute with invalid priv parameter
 */
void test_ltm4700_iio_read_attr_invalid_priv(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[64] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Use invalid priv value */
	ret = ltm4700_iio_read_attr(&iio_desc, buf, sizeof(buf), &channel, 9999);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/*******************************************************************************
 *    CONFIGURATION CHANNEL WRITE TESTS
 ******************************************************************************/

/**
 * @brief Test writing VOUT_COMMAND attribute (decimal format)
 */
void test_ltm4700_iio_write_attr_vout_command_decimal(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "3.300";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* "3.300" correctly parsed as 3300 mV */
	ltm4700_write_word_data_ExpectAndReturn(&test_ltm4700_dev, 0,
						LTM4700_VOUT_COMMAND, 3300, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_COMMAND);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing VOUT_COMMAND attribute (integer format)
 */
void test_ltm4700_iio_write_attr_vout_command_integer(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "5";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect write_word_data with 5000 mV */
	ltm4700_write_word_data_ExpectAndReturn(&test_ltm4700_dev, 0,
						LTM4700_VOUT_COMMAND, 5000, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_COMMAND);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing VOUT_MAX attribute
 */
void test_ltm4700_iio_write_attr_vout_max(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "3.600";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* "3.600" correctly parsed as 3600 mV */
	ltm4700_write_word_data_ExpectAndReturn(&test_ltm4700_dev, 0,
						LTM4700_VOUT_MAX, 3600, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_MAX);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing VOUT_MARGIN_HIGH attribute
 */
void test_ltm4700_iio_write_attr_vout_margin_high(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "3.465";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* "3.465" correctly parsed as 3465 mV */
	ltm4700_write_word_data_ExpectAndReturn(&test_ltm4700_dev, 0,
						LTM4700_VOUT_MARGIN_HIGH, 3465, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_MARGIN_HIGH);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing VOUT_MARGIN_LOW attribute
 */
void test_ltm4700_iio_write_attr_vout_margin_low(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "3.135";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* "3.135" correctly parsed as 3135 mV */
	ltm4700_write_word_data_ExpectAndReturn(&test_ltm4700_dev, 0,
						LTM4700_VOUT_MARGIN_LOW, 3135, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_MARGIN_LOW);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing VOUT_COMMAND attribute with invalid format
 */
void test_ltm4700_iio_write_attr_vout_command_invalid_format(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "invalid";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_VOUT_COMMAND);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test writing OPERATION attribute (off)
 */
void test_ltm4700_iio_write_attr_operation_off(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "off";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect write_byte with OFF value */
	ltm4700_write_byte_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_OPERATION,
					   LTM4700_OPERATION_OFF, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing OPERATION attribute (on)
 */
void test_ltm4700_iio_write_attr_operation_on(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "on";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect write_byte with ON value */
	ltm4700_write_byte_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_OPERATION,
					   LTM4700_OPERATION_ON, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing OPERATION attribute (margin_high)
 */
void test_ltm4700_iio_write_attr_operation_margin_high(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "margin_high";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect write_byte with MARGIN_HIGH value */
	ltm4700_write_byte_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_OPERATION,
					   LTM4700_OPERATION_MARGIN_HIGH, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing OPERATION attribute (margin_low)
 */
void test_ltm4700_iio_write_attr_operation_margin_low(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "margin_low";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect write_byte with MARGIN_LOW value */
	ltm4700_write_byte_ExpectAndReturn(&test_ltm4700_dev, 0,
					   LTM4700_OPERATION,
					   LTM4700_OPERATION_MARGIN_LOW, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing OPERATION attribute with invalid value
 */
void test_ltm4700_iio_write_attr_operation_invalid(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {.ch_num = 0};
	char buf[] = "invalid_operation";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test writing CLEAR_PEAKS attribute
 */
void test_ltm4700_iio_write_attr_clear_peaks(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[] = "1";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Expect clear_peaks to be called */
	ltm4700_clear_peaks_ExpectAndReturn(&test_ltm4700_dev, 0);

	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_CLEAR_PEAKS);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test writing attribute with invalid priv parameter
 */
void test_ltm4700_iio_write_attr_invalid_priv(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[] = "test";
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	/* Use invalid priv value */
	ret = ltm4700_iio_write_attr(&iio_desc, buf, sizeof(buf), &channel, 9999);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/*******************************************************************************
 *    AVAILABLE ATTRIBUTES TESTS
 ******************************************************************************/

/**
 * @brief Test reading operation available attribute
 */
void test_ltm4700_iio_read_avail_operation(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[128] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ret = ltm4700_iio_read_avail(&iio_desc, buf, sizeof(buf), &channel,
				     LTM4700_IIO_OPERATION);

	TEST_ASSERT_GREATER_THAN(0, ret);
	TEST_ASSERT_TRUE(strstr(buf, "off") != NULL);
	TEST_ASSERT_TRUE(strstr(buf, "on") != NULL);
	TEST_ASSERT_TRUE(strstr(buf, "margin_high") != NULL);
	TEST_ASSERT_TRUE(strstr(buf, "margin_low") != NULL);
}

/**
 * @brief Test reading available attribute with invalid priv
 */
void test_ltm4700_iio_read_avail_invalid_priv(void)
{
	struct ltm4700_iio_desc iio_desc;
	struct iio_ch_info channel = {0};
	char buf[128] = {0};
	int ret;

	iio_desc.ltm4700_dev = &test_ltm4700_dev;

	ret = ltm4700_iio_read_avail(&iio_desc, buf, sizeof(buf), &channel, 9999);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}
