/***************************************************************************//**
 *   @file   test_ltc4284.c
 *   @brief  Unit tests for LTC4284 driver
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

#include "unity.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ltc4284.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_delay.h"

/* Test device descriptors */
static struct ltc4284_dev test_dev_storage;
static struct ltc4284_dev *test_dev;
static struct no_os_i2c_desc test_i2c_desc;
static struct no_os_gpio_desc test_gpio_desc;

/* Mock register storage for simulating hardware */
static uint8_t mock_reg_value;
static uint8_t mock_reg_data[8];

/* Test I2C init parameters */
static struct no_os_i2c_init_param test_i2c_init_param = {
	.device_id = 0,
	.max_speed_hz = 400000,
	.slave_address = LTC4284_I2C_ADDR_2,
};

/* Test initialization parameters */
static struct ltc4284_init_param test_init_param = {
	.i2c_init = &test_i2c_init_param,
	.i2c_addr = LTC4284_I2C_ADDR_2,
	.rsense_mohm = 500,
	.vsense_mv = 25,
	.alert_gpio = NULL
};

/**
 * @brief Custom stub for I2C read to return mock data
 */
static int stub_i2c_read(struct no_os_i2c_desc *desc, uint8_t *data,
			 uint8_t bytes_number, uint8_t stop_bit, int num_calls)
{
	if (bytes_number <= sizeof(mock_reg_data)) {
		memcpy(data, mock_reg_data, bytes_number);
	}
	return 0;
}

/**
 * @brief Setup function called before each test
 */
void setUp(void)
{
	test_dev = NULL;
	memset(&test_dev_storage, 0, sizeof(test_dev_storage));
	memset(mock_reg_data, 0, sizeof(mock_reg_data));
	test_dev_storage.i2c_desc = &test_i2c_desc;
	test_dev_storage.i2c_addr = LTC4284_I2C_ADDR_2;
	test_dev_storage.rsense_mohm = 500;
	test_dev_storage.vsense_mv = 25;
}

/**
 * @brief Teardown function called after each test
 */
void tearDown(void)
{
	test_dev = NULL;
}

/******************************************************************************/
/************************** Initialization Tests ******************************/
/******************************************************************************/

/**
 * @brief Test successful initialization
 */
void test_ltc4284_init_success(void)
{
	int ret;

	/* Mock memory allocation */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), &test_dev_storage);

	/* Mock I2C initialization */
	no_os_i2c_init_IgnoreAndReturn(0);

	/* Mock initial register read for verification */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	ret = ltc4284_init(&test_dev, &test_init_param);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_NOT_NULL(test_dev);
}

/**
 * @brief Test initialization with NULL device pointer
 */
void test_ltc4284_init_null_device(void)
{
	int ret;

	ret = ltc4284_init(NULL, &test_init_param);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test initialization with NULL init parameters
 */
void test_ltc4284_init_null_params(void)
{
	int ret;

	ret = ltc4284_init(&test_dev, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test initialization with invalid I2C address
 */
void test_ltc4284_init_invalid_i2c_addr(void)
{
	int ret;
	struct ltc4284_init_param invalid_param = test_init_param;
	invalid_param.i2c_addr = 0xFF;

	ret = ltc4284_init(&test_dev, &invalid_param);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test initialization with invalid VSENSE
 */
void test_ltc4284_init_invalid_vsense(void)
{
	int ret;
	struct ltc4284_init_param invalid_param = test_init_param;
	invalid_param.vsense_mv = 35;  /* Invalid value */

	ret = ltc4284_init(&test_dev, &invalid_param);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test initialization with I2C failure
 */
void test_ltc4284_init_i2c_fail(void)
{
	int ret;

	/* Mock memory allocation */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), &test_dev_storage);

	/* Mock I2C init failure */
	no_os_i2c_init_IgnoreAndReturn(-EIO);

	/* Mock cleanup */
	no_os_free_Expect(&test_dev_storage);

	ret = ltc4284_init(&test_dev, &test_init_param);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test initialization with memory allocation failure
 */
void test_ltc4284_init_alloc_fail(void)
{
	int ret;

	/* Mock calloc failure */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), NULL);

	ret = ltc4284_init(&test_dev, &test_init_param);

	TEST_ASSERT_EQUAL(-ENOMEM, ret);
}

/**
 * @brief Test driver removal success
 */
void test_ltc4284_remove_success(void)
{
	int ret;

	/* Mock FET disable (calls update_bits which does read-modify-write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);  /* For update_bits read */

	/* Mock I2C removal */
	no_os_i2c_remove_ExpectAndReturn(&test_i2c_desc, 0);

	/* Mock memory free */
	no_os_free_Expect(&test_dev_storage);

	ret = ltc4284_remove(&test_dev_storage);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test driver removal with NULL device
 */
void test_ltc4284_remove_null(void)
{
	int ret;

	ret = ltc4284_remove(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/******************************************************************************/
/*********************** Register Access Tests ********************************/
/******************************************************************************/

/**
 * @brief Test read byte success
 */
void test_ltc4284_read_byte_success(void)
{
	int ret;
	uint8_t value = 0;

	/* Setup mock data */
	mock_reg_data[0] = 0x42;

	/* Mock I2C operations */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_byte(&test_dev_storage, LTC4284_REG_SYSTEM_STATUS, &value);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL(0x42, value);
}

/**
 * @brief Test read byte with NULL device
 */
void test_ltc4284_read_byte_null_dev(void)
{
	int ret;
	uint8_t value;

	ret = ltc4284_read_byte(NULL, LTC4284_REG_SYSTEM_STATUS, &value);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read byte with NULL value pointer
 */
void test_ltc4284_read_byte_null_val(void)
{
	int ret;

	ret = ltc4284_read_byte(&test_dev_storage, LTC4284_REG_SYSTEM_STATUS, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test write byte success
 */
void test_ltc4284_write_byte_success(void)
{
	int ret;
	uint8_t value = 0x12;

	/* Mock I2C write */
	no_os_i2c_write_IgnoreAndReturn(0);

	ret = ltc4284_write_byte(&test_dev_storage, LTC4284_REG_CONTROL_1, value);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test write byte with NULL device
 */
void test_ltc4284_write_byte_null(void)
{
	int ret;

	ret = ltc4284_write_byte(NULL, LTC4284_REG_CONTROL_1, 0x12);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read word success
 */
void test_ltc4284_read_word_success(void)
{
	int ret;
	uint16_t value = 0;

	/* Setup mock data (big-endian) */
	mock_reg_data[0] = 0x12;
	mock_reg_data[1] = 0x34;

	/* Mock I2C operations */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_word(&test_dev_storage, LTC4284_REG_SENSE, &value);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL(0x1234, value);
}

/**
 * @brief Test read word with NULL device
 */
void test_ltc4284_read_word_null_dev(void)
{
	int ret;
	uint16_t value;

	ret = ltc4284_read_word(NULL, LTC4284_REG_SENSE, &value);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test write word success
 */
void test_ltc4284_write_word_success(void)
{
	int ret;
	uint16_t value = 0x1234;

	/* Mock I2C write */
	no_os_i2c_write_IgnoreAndReturn(0);

	ret = ltc4284_write_word(&test_dev_storage, LTC4284_REG_SENSE, value);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test update bits success
 */
void test_ltc4284_update_bits_success(void)
{
	int ret;
	uint8_t mask = 0x0F;
	uint8_t new_bits = 0x05;

	/* Setup current register value */
	mock_reg_data[0] = 0xF0;

	/* Mock read current value */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	/* Mock write updated value (0xF5) */
	no_os_i2c_write_IgnoreAndReturn(0);

	ret = ltc4284_update_bits(&test_dev_storage, LTC4284_REG_CONTROL_1, mask,
				  new_bits);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test update bits with NULL device
 */
void test_ltc4284_update_bits_null(void)
{
	int ret;

	ret = ltc4284_update_bits(NULL, LTC4284_REG_CONTROL_1, 0x0F, 0x05);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/******************************************************************************/
/*********************** Monitoring Function Tests ****************************/
/******************************************************************************/

/**
 * @brief Test read VIN success
 */
void test_ltc4284_read_vin_success(void)
{
	int ret;
	uint16_t vin_mv = 0;

	/* Setup mock ADC data */
	mock_reg_data[0] = 0x0C;
	mock_reg_data[1] = 0x00;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_vin(&test_dev_storage, &vin_mv);

	TEST_ASSERT_EQUAL(0, ret);
	/* Value depends on LTC4284_VPWR_LSB_UV scaling */
}

/**
 * @brief Test read VIN with NULL device
 */
void test_ltc4284_read_vin_null_dev(void)
{
	int ret;
	uint16_t vin_mv;

	ret = ltc4284_read_vin(NULL, &vin_mv);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read VIN with NULL output
 */
void test_ltc4284_read_vin_null_output(void)
{
	int ret;

	ret = ltc4284_read_vin(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read IIN success
 */
void test_ltc4284_read_iin_success(void)
{
	int ret;
	uint16_t iin_ma = 0;

	/* Setup mock ADC data */
	mock_reg_data[0] = 0x08;
	mock_reg_data[1] = 0x00;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_iin(&test_dev_storage, &iin_ma);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test read IIN with NULL device
 */
void test_ltc4284_read_iin_null_dev(void)
{
	int ret;
	uint16_t iin_ma;

	ret = ltc4284_read_iin(NULL, &iin_ma);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read IIN with zero RSENSE (division by zero check)
 */
void test_ltc4284_read_iin_zero_rsense(void)
{
	int ret;
	uint16_t iin_ma = 0;

	/* Set RSENSE to 0 */
	test_dev_storage.rsense_mohm = 0;

	/* Setup mock ADC data */
	mock_reg_data[0] = 0x08;
	mock_reg_data[1] = 0x00;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_iin(&test_dev_storage, &iin_ma);

	/* Driver should catch division by zero */
	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read VOUT success
 */
void test_ltc4284_read_vout_success(void)
{
	int ret;
	uint16_t vout_mv = 0;

	/* Setup mock data */
	mock_reg_data[0] = 0x0A;
	mock_reg_data[1] = 0x00;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_vout(&test_dev_storage, &vout_mv);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test read VOUT with NULL device
 */
void test_ltc4284_read_vout_null_dev(void)
{
	int ret;
	uint16_t vout_mv;

	ret = ltc4284_read_vout(NULL, &vout_mv);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read power success
 */
void test_ltc4284_read_power_success(void)
{
	int ret;
	uint32_t power_mw = 0;

	/* Setup mock data */
	mock_reg_data[0] = 0x10;
	mock_reg_data[1] = 0x00;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_power(&test_dev_storage, &power_mw);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test read power with NULL device
 */
void test_ltc4284_read_power_null_dev(void)
{
	int ret;
	uint32_t power_mw;

	ret = ltc4284_read_power(NULL, &power_mw);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read energy success
 */
void test_ltc4284_read_energy_success(void)
{
	int ret;
	uint64_t energy_mj = 0;

	/* Setup mock 24-bit data */
	mock_reg_data[0] = 0x01;
	mock_reg_data[1] = 0x02;
	mock_reg_data[2] = 0x03;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_energy(&test_dev_storage, &energy_mj);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test read energy with NULL device
 */
void test_ltc4284_read_energy_null_dev(void)
{
	int ret;
	uint64_t energy_mj;

	ret = ltc4284_read_energy(NULL, &energy_mj);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read temperature success
 */
void test_ltc4284_read_temperature_success(void)
{
	int ret;
	int16_t temp_mc = 0;

	/* Setup mock data */
	mock_reg_data[0] = 0x06;
	mock_reg_data[1] = 0x40;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_temperature(&test_dev_storage, &temp_mc);

	TEST_ASSERT_EQUAL(0, ret);
	/* Placeholder implementation returns 25000 mC */
	TEST_ASSERT_EQUAL(25000, temp_mc);
}

/**
 * @brief Test read temperature with NULL device
 */
void test_ltc4284_read_temperature_null_dev(void)
{
	int ret;
	int16_t temp_mc;

	ret = ltc4284_read_temperature(NULL, &temp_mc);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/******************************************************************************/
/************************** Status and Fault Tests ****************************/
/******************************************************************************/

/**
 * @brief Test read status success
 */
void test_ltc4284_read_status_success(void)
{
	int ret;
	uint8_t status = 0;

	/* Setup mock status */
	mock_reg_data[0] = 0x80;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_read_status(&test_dev_storage, &status);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL(0x80, status);
}

/**
 * @brief Test read status with NULL device
 */
void test_ltc4284_read_status_null_dev(void)
{
	int ret;
	uint8_t status;

	ret = ltc4284_read_status(NULL, &status);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test get fault success
 */
void test_ltc4284_get_fault_success(void)
{
	int ret;
	uint8_t faults = 0;

	/* Setup mock faults */
	mock_reg_data[0] = 0x42;

	/* Mock register read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_get_fault(&test_dev_storage, &faults);

	TEST_ASSERT_EQUAL(0, ret);
	TEST_ASSERT_EQUAL(0x42, faults);
}

/**
 * @brief Test get fault with NULL device
 */
void test_ltc4284_get_fault_null_dev(void)
{
	int ret;
	uint8_t faults;

	ret = ltc4284_get_fault(NULL, &faults);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test clear faults success
 */
void test_ltc4284_clear_faults_success(void)
{
	int ret;

	/* Mock fault clear write */
	no_os_i2c_write_IgnoreAndReturn(0);

	ret = ltc4284_clear_faults(&test_dev_storage);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test clear faults with NULL device
 */
void test_ltc4284_clear_faults_null(void)
{
	int ret;

	ret = ltc4284_clear_faults(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test clear faults with I2C error
 */
void test_ltc4284_clear_faults_i2c_error(void)
{
	int ret;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_clear_faults(&test_dev_storage);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/******************************************************************************/
/************************** Control Function Tests ****************************/
/******************************************************************************/

/**
 * @brief Test enable FET success
 */
void test_ltc4284_enable_fet_success(void)
{
	int ret;

	/* Setup mock current control value */
	mock_reg_data[0] = 0x00;

	/* Mock update_bits operations (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_enable_fet(&test_dev_storage, true);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test enable FET with NULL device
 */
void test_ltc4284_enable_fet_null(void)
{
	int ret;

	ret = ltc4284_enable_fet(NULL, true);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test set current limit success (valid code 0-15)
 */
void test_ltc4284_set_current_limit_success(void)
{
	int ret;

	/* Setup mock register value */
	mock_reg_data[0] = 0x00;

	/* Mock update_bits operations (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	/* Use valid ilim_code (0-15) */
	ret = ltc4284_set_current_limit(&test_dev_storage, 10);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test set current limit with NULL device
 */
void test_ltc4284_set_current_limit_null(void)
{
	int ret;

	ret = ltc4284_set_current_limit(NULL, 10);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test set current limit with invalid code (> 15)
 */
void test_ltc4284_set_current_limit_invalid(void)
{
	int ret;

	/* Try to set code > 15 (4-bit value) */
	ret = ltc4284_set_current_limit(&test_dev_storage, 20);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/******************************************************************************/
/************************** EEPROM Function Tests *****************************/
/******************************************************************************/

/**
 * @brief Test store config success
 */
void test_ltc4284_store_config_success(void)
{
	int ret;

	/* Setup mock: EEPROM not busy */
	mock_reg_data[0] = 0x00;  /* EEPROM_BUSY bit not set */

	/* Mock write snapshot command */
	no_os_i2c_write_IgnoreAndReturn(0);

	/* Mock status read (no busy bit) */
	no_os_i2c_read_Stub(stub_i2c_read);

	ret = ltc4284_store_config(&test_dev_storage);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test store config with NULL device
 */
void test_ltc4284_store_config_null(void)
{
	int ret;

	ret = ltc4284_store_config(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test restore config success
 */
void test_ltc4284_restore_config_success(void)
{
	int ret;

	/* Mock reboot command write */
	no_os_i2c_write_IgnoreAndReturn(0);

	ret = ltc4284_restore_config(&test_dev_storage);

	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test restore config with NULL device
 */
void test_ltc4284_restore_config_null(void)
{
	int ret;

	ret = ltc4284_restore_config(NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/******************************************************************************/
/****************** Additional Error Path Coverage Tests *********************/
/******************************************************************************/

/**
 * @brief Test read_byte with I2C write failure
 */
void test_ltc4284_read_byte_i2c_write_fail(void)
{
	int ret;
	uint8_t value;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_byte(&test_dev_storage, LTC4284_REG_SYSTEM_STATUS, &value);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_byte with I2C read failure
 */
void test_ltc4284_read_byte_i2c_read_fail(void)
{
	int ret;
	uint8_t value;

	/* Mock I2C write success, read failure */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_byte(&test_dev_storage, LTC4284_REG_SYSTEM_STATUS, &value);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_word with I2C write failure
 */
void test_ltc4284_read_word_i2c_write_fail(void)
{
	int ret;
	uint16_t value;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_word(&test_dev_storage, LTC4284_REG_SENSE, &value);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_word with I2C read failure
 */
void test_ltc4284_read_word_i2c_read_fail(void)
{
	int ret;
	uint16_t value;

	/* Mock I2C write success, read failure */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_word(&test_dev_storage, LTC4284_REG_SENSE, &value);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_word with NULL value pointer
 */
void test_ltc4284_read_word_null_val(void)
{
	int ret;

	ret = ltc4284_read_word(&test_dev_storage, LTC4284_REG_SENSE, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test write_byte with I2C failure
 */
void test_ltc4284_write_byte_i2c_fail(void)
{
	int ret;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_write_byte(&test_dev_storage, LTC4284_REG_CONTROL_1, 0x12);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test write_word with NULL device
 */
void test_ltc4284_write_word_null(void)
{
	int ret;

	ret = ltc4284_write_word(NULL, LTC4284_REG_SENSE, 0x1234);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test write_word with I2C failure
 */
void test_ltc4284_write_word_i2c_fail(void)
{
	int ret;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_write_word(&test_dev_storage, LTC4284_REG_SENSE, 0x1234);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test update_bits with read failure
 */
void test_ltc4284_update_bits_read_fail(void)
{
	int ret;

	/* Mock I2C write success, read failure */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_update_bits(&test_dev_storage, LTC4284_REG_CONTROL_1, 0x0F, 0x05);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test update_bits with write failure
 */
void test_ltc4284_update_bits_write_fail(void)
{
	int ret;

	/* Setup mock current value */
	mock_reg_data[0] = 0xF0;

	/* Mock read success */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(stub_i2c_read);

	/* Mock write failure (second write call) */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_update_bits(&test_dev_storage, LTC4284_REG_CONTROL_1, 0x0F, 0x05);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test init with GPIO get failure
 */
void test_ltc4284_init_gpio_get_fail(void)
{
	int ret;
	struct no_os_gpio_init_param gpio_init = {0};
	struct ltc4284_init_param param_with_gpio = test_init_param;
	param_with_gpio.alert_gpio = &gpio_init;

	/* Mock memory allocation */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), &test_dev_storage);

	/* Mock I2C init success */
	no_os_i2c_init_IgnoreAndReturn(0);

	/* Mock GPIO get failure */
	no_os_gpio_get_optional_IgnoreAndReturn(-EIO);

	/* Mock cleanup */
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Expect(&test_dev_storage);

	ret = ltc4284_init(&test_dev, &param_with_gpio);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test init with GPIO get success (optional GPIO)
 * This tests the path where GPIO is provided but get_optional returns NULL
 */
void test_ltc4284_init_gpio_optional_null(void)
{
	int ret;
	struct no_os_gpio_init_param gpio_init = {0};
	struct ltc4284_init_param param_with_gpio = test_init_param;
	param_with_gpio.alert_gpio = &gpio_init;

	/* Mock memory allocation */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), &test_dev_storage);

	/* Mock I2C init success */
	no_os_i2c_init_IgnoreAndReturn(0);

	/* Mock GPIO get returning NULL (optional GPIO not available) */
	no_os_gpio_get_optional_IgnoreAndReturn(0);
	no_os_gpio_get_optional_ReturnThruPtr_desc(NULL);

	/* Mock status read */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	ret = ltc4284_init(&test_dev, &param_with_gpio);

	/* Should succeed even without GPIO */
	TEST_ASSERT_EQUAL(0, ret);
}

/**
 * @brief Test init with status read failure
 */
void test_ltc4284_init_status_read_fail(void)
{
	int ret;

	/* Mock memory allocation */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc4284_dev), &test_dev_storage);

	/* Mock I2C init success */
	no_os_i2c_init_IgnoreAndReturn(0);

	/* Mock status read failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	/* Mock cleanup */
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Expect(&test_dev_storage);

	ret = ltc4284_init(&test_dev, &test_init_param);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test remove with GPIO allocated
 */
void test_ltc4284_remove_with_gpio(void)
{
	int ret;

	/* Add GPIO to device */
	test_dev_storage.alert_gpio = &test_gpio_desc;

	/* Mock FET disable */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	/* Mock GPIO removal */
	no_os_gpio_remove_ExpectAndReturn(&test_gpio_desc, 0);

	/* Mock I2C removal */
	no_os_i2c_remove_ExpectAndReturn(&test_i2c_desc, 0);

	/* Mock memory free */
	no_os_free_Expect(&test_dev_storage);

	ret = ltc4284_remove(&test_dev_storage);

	TEST_ASSERT_EQUAL(0, ret);

	/* Clean up for next test */
	test_dev_storage.alert_gpio = NULL;
}

/**
 * @brief Test remove with FET disable failure
 */
void test_ltc4284_remove_fet_disable_fail(void)
{
	int ret;

	/* Mock FET disable failure (update_bits read fails) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_remove(&test_dev_storage);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_vin with I2C failure
 */
void test_ltc4284_read_vin_i2c_fail(void)
{
	int ret;
	uint16_t vin_mv;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_vin(&test_dev_storage, &vin_mv);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_iin with I2C failure
 */
void test_ltc4284_read_iin_i2c_fail(void)
{
	int ret;
	uint16_t iin_ma;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_iin(&test_dev_storage, &iin_ma);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_iin with NULL output
 */
void test_ltc4284_read_iin_null_output(void)
{
	int ret;

	ret = ltc4284_read_iin(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_vout with I2C failure
 */
void test_ltc4284_read_vout_i2c_fail(void)
{
	int ret;
	uint16_t vout_mv;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_vout(&test_dev_storage, &vout_mv);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_vout with NULL output
 */
void test_ltc4284_read_vout_null_output(void)
{
	int ret;

	ret = ltc4284_read_vout(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_power with I2C failure
 */
void test_ltc4284_read_power_i2c_fail(void)
{
	int ret;
	uint32_t power_mw;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_power(&test_dev_storage, &power_mw);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_power with NULL output
 */
void test_ltc4284_read_power_null_output(void)
{
	int ret;

	ret = ltc4284_read_power(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_energy with I2C write failure
 */
void test_ltc4284_read_energy_i2c_write_fail(void)
{
	int ret;
	uint64_t energy_mj;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_energy(&test_dev_storage, &energy_mj);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_energy with I2C read failure
 */
void test_ltc4284_read_energy_i2c_read_fail(void)
{
	int ret;
	uint64_t energy_mj;

	/* Mock I2C write success, read failure */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_energy(&test_dev_storage, &energy_mj);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_energy with NULL output
 */
void test_ltc4284_read_energy_null_output(void)
{
	int ret;

	ret = ltc4284_read_energy(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_temperature with I2C failure
 */
void test_ltc4284_read_temperature_i2c_fail(void)
{
	int ret;
	int16_t temp_mc;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_temperature(&test_dev_storage, &temp_mc);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test read_temperature with NULL output
 */
void test_ltc4284_read_temperature_null_output(void)
{
	int ret;

	ret = ltc4284_read_temperature(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_status with NULL output
 */
void test_ltc4284_read_status_null_output(void)
{
	int ret;

	ret = ltc4284_read_status(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test read_status with I2C failure
 */
void test_ltc4284_read_status_i2c_fail(void)
{
	int ret;
	uint8_t status;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_read_status(&test_dev_storage, &status);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test get_fault with NULL output
 */
void test_ltc4284_get_fault_null_output(void)
{
	int ret;

	ret = ltc4284_get_fault(&test_dev_storage, NULL);

	TEST_ASSERT_EQUAL(-EINVAL, ret);
}

/**
 * @brief Test get_fault with I2C failure
 */
void test_ltc4284_get_fault_i2c_fail(void)
{
	int ret;
	uint8_t faults;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_get_fault(&test_dev_storage, &faults);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test enable_fet with I2C failure
 */
void test_ltc4284_enable_fet_i2c_fail(void)
{
	int ret;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_enable_fet(&test_dev_storage, true);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test set_current_limit with I2C failure
 */
void test_ltc4284_set_current_limit_i2c_fail(void)
{
	int ret;

	/* Mock I2C failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_set_current_limit(&test_dev_storage, 10);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test store_config with I2C write failure
 */
void test_ltc4284_store_config_i2c_write_fail(void)
{
	int ret;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_store_config(&test_dev_storage);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test store_config with I2C read failure
 */
void test_ltc4284_store_config_i2c_read_fail(void)
{
	int ret;

	/* Mock write success, read failure */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	ret = ltc4284_store_config(&test_dev_storage);

	TEST_ASSERT_EQUAL(-EIO, ret);
}

/**
 * @brief Test restore_config with I2C failure
 */
void test_ltc4284_restore_config_i2c_fail(void)
{
	int ret;

	/* Mock I2C write failure */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	ret = ltc4284_restore_config(&test_dev_storage);

	TEST_ASSERT_EQUAL(-EIO, ret);
}
