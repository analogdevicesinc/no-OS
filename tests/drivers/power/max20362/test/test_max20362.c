/***************************************************************************//**
 *   @file   test_max20362.c
 *   @brief  Unit tests for MAX20362 Driver (max20362.c)
 *   @author Wilford Abilay (wilford.abilay@analog.com)
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

#include "unity.h"
#include "max20362.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_delay.h"
#include "mock_no_os_util.h"
#include <string.h>
#include <errno.h>

/*******************************************************************************
 *    PRIVATE VARIABLES
 ******************************************************************************/

static struct no_os_i2c_desc test_i2c_desc;
static struct max20362_init_param test_init_param;

static uint8_t test_read_val;
static int i2c_write_fail_on_call;

char os_malloc[512] = {0};

/*******************************************************************************
 *    MOCK CALLBACK FUNCTIONS
 ******************************************************************************/

/**
 * @brief Mock callback for no_os_i2c_read - returns single byte
 */
static int32_t i2c_read_cb(struct no_os_i2c_desc *desc,
			   uint8_t *data, uint8_t bytes_number,
			   uint8_t stop_bit, int cmock_num_calls)
{
	if (data && bytes_number > 0)
		data[0] = test_read_val;
	return 0;
}

/**
 * @brief Mock callback for no_os_i2c_read during init - chip ID verification
 */
static int32_t i2c_read_init_cb(struct no_os_i2c_desc *desc,
				uint8_t *data, uint8_t bytes_number,
				uint8_t stop_bit, int cmock_num_calls)
{
	if (data && bytes_number > 0)
		data[0] = MAX20362_CHIP_ID_VAL;
	return 0;
}

/**
 * @brief Mock callback for no_os_i2c_read - returns wrong chip ID
 */
static int32_t i2c_read_wrong_id_cb(struct no_os_i2c_desc *desc,
				    uint8_t *data, uint8_t bytes_number,
				    uint8_t stop_bit, int cmock_num_calls)
{
	if (data && bytes_number > 0)
		data[0] = 0xFF;
	return 0;
}

/**
 * @brief Mock callback for no_os_i2c_write - fails on a specific call number
 */
static int32_t i2c_write_fail_at_cb(struct no_os_i2c_desc *desc,
				    uint8_t *data, uint8_t bytes_number,
				    uint8_t stop_bit, int cmock_num_calls)
{
	if (cmock_num_calls == i2c_write_fail_on_call)
		return -EIO;
	return 0;
}

/**
 * @brief Mock callback for no_os_i2c_init - sets the desc pointer
 */
static int32_t i2c_init_cb(struct no_os_i2c_desc **desc,
			   const struct no_os_i2c_init_param *param,
			   int cmock_num_calls)
{
	if (desc)
		*desc = &test_i2c_desc;
	return 0;
}

/*******************************************************************************
 *    TEST SETUP AND TEARDOWN
 ******************************************************************************/

void setUp(void)
{
	memset(&test_i2c_desc, 0, sizeof(test_i2c_desc));
	memset(&test_init_param, 0, sizeof(test_init_param));
	test_read_val = 0;

	test_init_param.i2c_init.device_id = 0;
	test_init_param.i2c_init.slave_address = MAX20362_PMIC_I2C_ADDR;
}

void tearDown(void)
{
}

/*******************************************************************************
 *    REGISTER ACCESS TESTS
 ******************************************************************************/

void test_max20362_reg_write_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_reg_write(&dev, MAX20362_REG_BBST_VSET, 0x24);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_reg_write_null_dev(void)
{
	int ret = max20362_reg_write(NULL, 0x00, 0x00);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_reg_write_null_i2c(void)
{
	struct max20362_dev dev = {.i2c_desc = NULL};

	int ret = max20362_reg_write(&dev, 0x00, 0x00);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_reg_write_i2c_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_reg_write(&dev, 0x00, 0x00);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_reg_read_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t val = 0;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0xAB;

	int ret = max20362_reg_read(&dev, MAX20362_REG_CHIP_ID, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0xAB, val);
}

void test_max20362_reg_read_null_dev(void)
{
	uint8_t val;

	int ret = max20362_reg_read(NULL, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_reg_read_null_i2c(void)
{
	struct max20362_dev dev = {.i2c_desc = NULL};
	uint8_t val;

	int ret = max20362_reg_read(&dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_reg_read_null_val(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_reg_read(&dev, 0x00, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_reg_read_write_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t val;

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_reg_read(&dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_reg_read_read_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t val;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);

	int ret = max20362_reg_read(&dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_reg_update_bits_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* Read returns 0xF0, mask=0x0F, val=0x05 => write 0xF5 */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0xF0;

	int ret = max20362_reg_update_bits(&dev, MAX20362_REG_BBST_CFG0,
					   0x0F, 0x05);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_reg_update_bits_null_dev(void)
{
	int ret = max20362_reg_update_bits(NULL, 0x00, 0xFF, 0x00);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_reg_update_bits_read_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_reg_update_bits(&dev, 0x01, 0x0F, 0x05);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    DEVICE INIT/REMOVE TESTS
 ******************************************************************************/

void test_max20362_init_success(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.buck_boost_voltage_uv = 3300000;
	test_init_param.cap_voltage_uv = 5000000;
	test_init_param.ldo_voltage_uv = 1800000;
	test_init_param.input_current_limit_ma = 20;
	test_init_param.bbat_vdrop = MAX20362_BBAT_VDROP_100MV;
	test_init_param.buck_boost_enable = false;
	test_init_param.ldo_enable = false;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
}

void test_max20362_init_null_device(void)
{
	int ret = max20362_init(NULL, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_init_null_param(void)
{
	struct max20362_dev *dev = NULL;

	int ret = max20362_init(&dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_init_alloc_failure(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev), NULL);

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
}

void test_max20362_init_i2c_failure(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_IgnoreAndReturn(-EIO);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_init_wrong_chip_id(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_wrong_id_cb);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_init_chip_id_read_failure(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	/* chip ID reg_read: addr write succeeds but read fails */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(-EIO);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_remove_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* disable buck-boost: update_bits */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* disable ldo: update_bits */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* clear_all_interrupts: 3 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* i2c_remove */
	no_os_i2c_remove_ExpectAndReturn(&test_i2c_desc, 0);
	no_os_free_Ignore();

	int ret = max20362_remove(&dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_remove_null_dev(void)
{
	int ret = max20362_remove(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/*******************************************************************************
 *    BUCK-BOOST CONTROL TESTS
 ******************************************************************************/

void test_max20362_enable_buck_boost_on(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_enable_buck_boost(&dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_enable_buck_boost_off(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = MAX20362_BBSTCFG_BBSTENA_MSK;

	int ret = max20362_enable_buck_boost(&dev, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_buck_boost_voltage_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* write voltage */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* lock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	/* 3.3V: (3300000 - 1500000) / 50000 = 36 = 0x24 */
	int ret = max20362_set_buck_boost_voltage(&dev, 3300000);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_buck_boost_voltage_null_dev(void)
{
	int ret = max20362_set_buck_boost_voltage(NULL, 3300000);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_buck_boost_voltage_below_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_buck_boost_voltage(&dev, 1000000);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_buck_boost_voltage_above_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_buck_boost_voltage(&dev, 6000000);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_buck_boost_voltage_min_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_buck_boost_voltage(&dev, MAX20362_BBOUT_MIN_UV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_buck_boost_voltage_max_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_buck_boost_voltage(&dev, MAX20362_BBOUT_MAX_UV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_buck_boost_mode_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_config_buck_boost_mode(&dev, false, true, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_buck_boost_mode_all_enabled(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_config_buck_boost_mode(&dev, true, true, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_buck_boost_discharge_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_config_buck_boost_discharge(&dev, true, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_buck_boost_discharge_both(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_config_buck_boost_discharge(&dev, true, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

/*******************************************************************************
 *    LDO CONTROL TESTS
 ******************************************************************************/

void test_max20362_enable_ldo_on(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_enable_ldo(&dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_enable_ldo_off(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = MAX20362_LDOCFG_ENA_MSK;

	int ret = max20362_enable_ldo(&dev, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_voltage_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 1.8V: (1800000 - 900000) / 100000 = 9 */
	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_voltage_below_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_ldo_voltage(&dev, 800000);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_ldo_voltage_above_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_ldo_voltage(&dev, 4100000);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_ldo_voltage_min_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_ldo_voltage(&dev, MAX20362_LDO_MIN_UV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_voltage_max_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_ldo_voltage(&dev, MAX20362_LDO_MAX_UV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_ldo_mode_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_config_ldo_mode(&dev, true, false, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_config_ldo_mode_all_disabled(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0xFF;

	int ret = max20362_config_ldo_mode(&dev, false, false, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_input_source_bbout(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_ldo_input_source(&dev, MAX20362_LDO_SOURCE_BBOUT);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_input_source_batt(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = MAX20362_LDOCFG_BBSTSUP_MSK;

	int ret = max20362_set_ldo_input_source(&dev, MAX20362_LDO_SOURCE_BATT);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_input_source_cap(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_ldo_input_source(&dev, MAX20362_LDO_SOURCE_CAP);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_input_source_invalid(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_ldo_input_source(&dev,
						(enum max20362_ldo_source)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    DVS CONTROL TESTS
 ******************************************************************************/

void test_max20362_set_dvs_mode_i2c(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	no_os_udelay_Ignore();

	int ret = max20362_set_dvs_mode(&dev, MAX20362_DVS_SOURCE_I2C);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_dvs_mode_pspi(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x01);
	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	no_os_udelay_Ignore();

	int ret = max20362_set_dvs_mode(&dev, MAX20362_DVS_SOURCE_PSPI);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_dvs_mode_round_robin(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x02);
	no_os_field_prep_IgnoreAndReturn(0x04);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	no_os_udelay_Ignore();

	int ret = max20362_set_dvs_mode(&dev, MAX20362_DVS_SOURCE_ROUND_ROBIN);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_dvs_mode_null_dev(void)
{
	int ret = max20362_set_dvs_mode(NULL, MAX20362_DVS_SOURCE_I2C);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_dvs_mode_invalid_source(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_dvs_mode(&dev, (enum max20362_dvs_source)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {2500000, 3000000, 3300000, 3600000};

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* 4 voltage writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* RR size config: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* lock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable RR: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 4);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_dvs_rr_table_null_dev(void)
{
	uint32_t voltages[] = {3300000};

	int ret = max20362_set_dvs_rr_table(NULL, voltages, 1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_dvs_rr_table_null_voltages(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_dvs_rr_table(&dev, NULL, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_count_zero(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {3300000};

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_count_exceeds_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[21];

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 21);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_voltage_below_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {1000000}; /* Below 1.5V minimum */

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_voltage_above_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {6000000}; /* Above 5.5V maximum */

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_dvs_rr_table_voltage_min_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {MAX20362_BBOUT_MIN_UV};

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* 1 voltage writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* RR size config: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* lock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable RR: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_dvs_rr_table_voltage_max_boundary(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {MAX20362_BBOUT_MAX_UV};

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* 1 voltage writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* RR size config: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* lock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable RR: update_bits (read + write) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

/*******************************************************************************
 *    CAPACITOR VOLTAGE TESTS
 ******************************************************************************/

void test_max20362_set_cap_voltage_500mv_step(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_field_prep_IgnoreAndReturn(0x05);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 5.0V with 500mV steps: (5000000-2500000)/500000 = 5 */
	int ret = max20362_set_cap_voltage(&dev, 5000000, MAX20362_VCAP_STEP_500MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_250mv_step(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x10);
	no_os_field_prep_IgnoreAndReturn(0x06);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 3.1V with 250mV steps: (3100000-1600000)/250000 = 6 */
	int ret = max20362_set_cap_voltage(&dev, 3100000, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_125mv_step(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x20);
	no_os_field_prep_IgnoreAndReturn(0x06);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 1.9V with 125mV steps */
	int ret = max20362_set_cap_voltage(&dev, 1900000, MAX20362_VCAP_STEP_125MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_125mv_step_above_upper_limit(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x20);
	no_os_field_prep_IgnoreAndReturn(0x0F);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* Above 2.9V with 125mV step => capped at max vset */
	int ret = max20362_set_cap_voltage(&dev, 3000000, MAX20362_VCAP_STEP_125MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_null_dev(void)
{
	int ret = max20362_set_cap_voltage(NULL, 3000000, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_cap_voltage_invalid_step(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_cap_voltage(&dev, 3000000, 3);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_cap_voltage_below_global_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_cap_voltage(&dev, 1500000, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_cap_voltage_above_global_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_cap_voltage(&dev, 10000000, MAX20362_VCAP_STEP_500MV);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_cap_voltage_500mv_below_step_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* 2.0V is within global range but below 500mV step min (2.5V) */
	int ret = max20362_set_cap_voltage(&dev, 2000000, MAX20362_VCAP_STEP_500MV);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_cap_voltage_125mv_below_step_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* 1.6V is within global range but below 125mV step min (1.65V) */
	int ret = max20362_set_cap_voltage(&dev, 1600000, MAX20362_VCAP_STEP_125MV);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    CURRENT LIMIT TESTS
 ******************************************************************************/

void test_max20362_set_input_current_limit_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 25mA: reg_val = 25 - 5 = 20 = 0x14 */
	int ret = max20362_set_input_current_limit(&dev, 25);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_input_current_limit_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_input_current_limit(&dev, MAX20362_ILIM_MIN_MA);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_input_current_limit_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_input_current_limit(&dev, MAX20362_ILIM_MAX_MA);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_input_current_limit_below_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_input_current_limit(&dev, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_set_input_current_limit_above_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_input_current_limit(&dev, 51);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    BATTERY VOLTAGE DROP TESTS
 ******************************************************************************/

void test_max20362_set_bbat_vdrop_55mv(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_bbat_vdrop(&dev, MAX20362_BBAT_VDROP_55MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_bbat_vdrop_100mv(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x40);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_bbat_vdrop(&dev, MAX20362_BBAT_VDROP_100MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_bbat_vdrop_150mv(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x80);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_bbat_vdrop(&dev, MAX20362_BBAT_VDROP_150MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_bbat_vdrop_200mv(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0xC0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	int ret = max20362_set_bbat_vdrop(&dev, MAX20362_BBAT_VDROP_200MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_bbat_vdrop_invalid(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	int ret = max20362_set_bbat_vdrop(&dev, (enum max20362_bbat_vdrop)4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    STATUS AND INTERRUPT TESTS
 ******************************************************************************/

void test_max20362_get_status_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t status;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x25;

	int ret = max20362_get_status(&dev, &status);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0x25, status);
}

void test_max20362_get_int_status_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t status;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x30;

	int ret = max20362_get_int_status(&dev, &status);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0x30, status);
}

void test_max20362_get_ldo_int_status_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t status;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x0F;

	int ret = max20362_get_ldo_int_status(&dev, &status);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0x0F, status);
}

void test_max20362_get_ingen_int_status_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint8_t status;

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x1D;

	int ret = max20362_get_ingen_int_status(&dev, &status);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0x1D, status);
}

void test_max20362_set_int_mask_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_int_mask(&dev, 0xCF);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ldo_int_mask_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_ldo_int_mask(&dev, 0x0F);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_ingen_int_mask_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_ingen_int_mask(&dev, 0x1D);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_clear_all_interrupts_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* 3 writes: main INT, LDO_INT, INGEN_INT */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_clear_all_interrupts(&dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_clear_all_interrupts_first_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_clear_all_interrupts(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_clear_all_interrupts_second_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_clear_all_interrupts(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_clear_all_interrupts_third_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_clear_all_interrupts(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    LOCK/UNLOCK TESTS
 ******************************************************************************/

void test_max20362_unlock_registers_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* Write LOCK_MSK=0x00, write LOCK_UNLOCK=0x55 */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_unlock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_unlock_registers_first_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_unlock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_unlock_registers_second_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_unlock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_lock_registers_success(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* Write LOCK_UNLOCK=0xAA, write LOCK_MSK=0x01 */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_lock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_lock_registers_first_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_lock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_lock_registers_second_write_fails(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_lock_registers(&dev);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    INIT WITH ENABLE PATHS
 ******************************************************************************/

void test_max20362_init_with_buck_boost_enable(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.buck_boost_enable = true;
	test_init_param.ldo_enable = false;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
}

void test_max20362_init_with_ldo_enable(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.ldo_enable = true;
	test_init_param.buck_boost_enable = false;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
}

void test_max20362_init_with_both_enables(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.buck_boost_enable = true;
	test_init_param.ldo_enable = true;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);
}

/*******************************************************************************
 *    INIT ERROR PATH TESTS
 ******************************************************************************/

void test_max20362_init_bbat_vdrop_failure(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	/*
	 * Write call sequence (no optional voltages/currents):
	 * 0: chip ID read (addr write)
	 * 1: clear INT
	 * 2: clear LDO_INT
	 * 3: clear INGEN_INT
	 * 4: bbat_vdrop update_bits addr write (read phase)
	 * 5: bbat_vdrop update_bits value write -> FAIL
	 */
	i2c_write_fail_on_call = 5;
	no_os_i2c_write_Stub(i2c_write_fail_at_cb);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_init_buck_boost_voltage_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.buck_boost_voltage_uv = 999999;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_init_cap_voltage_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.cap_voltage_uv = 1000000;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_init_ldo_voltage_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.ldo_voltage_uv = 500000;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    NULL DEV GUARD TESTS
 ******************************************************************************/

void test_max20362_config_buck_boost_mode_null_dev(void)
{
	int ret = max20362_config_buck_boost_mode(NULL, false, false, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_config_buck_boost_discharge_null_dev(void)
{
	int ret = max20362_config_buck_boost_discharge(NULL, false, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_enable_ldo_null_dev(void)
{
	int ret = max20362_enable_ldo(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_ldo_voltage_null_dev(void)
{
	int ret = max20362_set_ldo_voltage(NULL, 1800000);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_ldo_input_source_null_dev(void)
{
	int ret = max20362_set_ldo_input_source(NULL, MAX20362_LDO_SOURCE_BBOUT);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_enable_buck_boost_null_dev(void)
{
	int ret = max20362_enable_buck_boost(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_get_status_null_dev(void)
{
	uint8_t status;

	int ret = max20362_get_status(NULL, &status);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_get_int_status_null_dev(void)
{
	uint8_t status;

	int ret = max20362_get_int_status(NULL, &status);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_get_ldo_int_status_null_dev(void)
{
	uint8_t status;

	int ret = max20362_get_ldo_int_status(NULL, &status);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_get_ingen_int_status_null_dev(void)
{
	uint8_t status;

	int ret = max20362_get_ingen_int_status(NULL, &status);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_int_mask_null_dev(void)
{
	int ret = max20362_set_int_mask(NULL, 0xFF);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_ldo_int_mask_null_dev(void)
{
	int ret = max20362_set_ldo_int_mask(NULL, 0xFF);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_ingen_int_mask_null_dev(void)
{
	int ret = max20362_set_ingen_int_mask(NULL, 0xFF);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_clear_all_interrupts_null_dev(void)
{
	int ret = max20362_clear_all_interrupts(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_unlock_registers_null_dev(void)
{
	int ret = max20362_unlock_registers(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_lock_registers_null_dev(void)
{
	int ret = max20362_lock_registers(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_input_current_limit_null_dev(void)
{
	int ret = max20362_set_input_current_limit(NULL, 25);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_bbat_vdrop_null_dev(void)
{
	int ret = max20362_set_bbat_vdrop(NULL, MAX20362_BBAT_VDROP_55MV);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/*******************************************************************************
 *    CAPACITOR VOLTAGE CLAMPING TESTS
 ******************************************************************************/

void test_max20362_set_cap_voltage_500mv_clamp_to_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_field_prep_IgnoreAndReturn(0x0E);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 9.5V with 500mV steps: (9500000-2500000)/500000 = 14 = VSET_MAX */
	int ret = max20362_set_cap_voltage(&dev, 9500000, MAX20362_VCAP_STEP_500MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_250mv_clamp_to_max(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x10);
	no_os_field_prep_IgnoreAndReturn(0x0F);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 5.35V with 250mV steps: (5350000-1600000)/250000 = 15 = VSET_MAX */
	int ret = max20362_set_cap_voltage(&dev, 5350000, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_250mv_below_step_min(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* 1.6V is the global min but 250mV step min is 1.6V - it should pass
	 * at exactly 1.6V but fail below it. Test value within global range
	 * but computed vset overflow: actually 1.6V is the min for 250mV */
	int ret = max20362_set_cap_voltage(&dev, 1599999, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    DVS MODE FAILURE PATH TESTS
 ******************************************************************************/

void test_max20362_set_dvs_mode_update_bits_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_field_prep_IgnoreAndReturn(0x00);
	/* reg_update_bits will read then fail on write */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_dvs_mode(&dev, MAX20362_DVS_SOURCE_I2C);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    DVS RR TABLE FAILURE PATH TESTS
 ******************************************************************************/

void test_max20362_set_dvs_rr_table_unlock_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {3300000};

	/* unlock first write fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_dvs_rr_table_write_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {3300000, 3600000};

	/* unlock: 2 writes succeed */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* first voltage write succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* second voltage write fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);
	/* lock: 2 writes (error cleanup) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 2);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_dvs_rr_table_rr_size_config_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};
	uint32_t voltages[] = {3300000};

	/* unlock: 2 writes */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* 1 voltage write */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* RR size config: update_bits read succeeds, write fails */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	no_os_i2c_write_IgnoreAndReturn(-EIO);
	/* lock: 2 writes (error cleanup) */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);

	int ret = max20362_set_dvs_rr_table(&dev, voltages, 1);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    LDO VOLTAGE FAILURE PATH TESTS
 ******************************************************************************/

void test_max20362_set_ldo_voltage_read_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* reg_read inside set_ldo_voltage fails at the i2c_write (addr phase) */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_ldo_voltage_disable_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* reg_read for ldo_cfg succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x01;
	/* enable_ldo(false) -> update_bits read succeeds but write fails */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_ldo_voltage_vset_write_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* reg_read for ldo_cfg succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* enable_ldo(false): update_bits succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* reg_write for LDO_VSET fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_ldo_voltage_enable_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* reg_read for ldo_cfg succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;
	/* enable_ldo(false): update_bits succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* reg_write for LDO_VSET succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable_ldo(true): update_bits read succeeds, write fails */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_set_ldo_voltage_restore_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* reg_read for ldo_cfg succeeds with ENA bit set */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = MAX20362_LDOCFG_ENA_MSK;
	/* enable_ldo(false): update_bits succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* reg_write for LDO_VSET succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable_ldo(true): succeeds */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	/* enable_ldo(restore=true): update_bits read succeeds, write fails */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_voltage(&dev, 1800000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    INIT EARLY ERROR PATH TESTS
 ******************************************************************************/

void test_max20362_init_clear_interrupts_failure(void)
{
	struct max20362_dev *dev = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	/* chip ID read: write addr + read succeed */
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	/* clear_all_interrupts first write fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_init_current_limit_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.input_current_limit_ma = 99;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max20362_init_buck_boost_enable_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.buck_boost_enable = true;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	/*
	 * Write call sequence with buck_boost_enable=true, no voltages:
	 * 0: chip ID read (addr write)
	 * 1: clear INT
	 * 2: clear LDO_INT
	 * 3: clear INGEN_INT
	 * 4: bbat_vdrop update_bits addr write (read phase)
	 * 5: bbat_vdrop update_bits value write
	 * 6: enable_buck_boost update_bits addr write (read phase)
	 * 7: enable_buck_boost update_bits value write -> FAIL
	 */
	i2c_write_fail_on_call = 7;
	no_os_i2c_write_Stub(i2c_write_fail_at_cb);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_max20362_init_ldo_enable_failure(void)
{
	struct max20362_dev *dev = NULL;

	test_init_param.ldo_enable = true;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct max20362_dev),
				     (void *)os_malloc);
	no_os_i2c_init_Stub(i2c_init_cb);
	no_os_i2c_read_Stub(i2c_read_init_cb);
	no_os_field_prep_IgnoreAndReturn(0);
	/*
	 * Write call sequence with ldo_enable=true, no voltages:
	 * 0: chip ID read (addr write)
	 * 1: clear INT
	 * 2: clear LDO_INT
	 * 3: clear INGEN_INT
	 * 4: bbat_vdrop update_bits addr write (read phase)
	 * 5: bbat_vdrop update_bits value write
	 * 6: enable_ldo update_bits addr write (read phase)
	 * 7: enable_ldo update_bits value write -> FAIL
	 */
	i2c_write_fail_on_call = 7;
	no_os_i2c_write_Stub(i2c_write_fail_at_cb);
	no_os_i2c_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();

	int ret = max20362_init(&dev, &test_init_param);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    BUCK-BOOST VOLTAGE UNLOCK FAILURE TEST
 ******************************************************************************/

void test_max20362_set_buck_boost_voltage_unlock_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* unlock first write fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_buck_boost_voltage(&dev, 3300000);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    LDO CONFIG NULL DEV AND FAILURE TESTS
 ******************************************************************************/

void test_max20362_config_ldo_mode_null_dev(void)
{
	int ret = max20362_config_ldo_mode(NULL, false, false, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max20362_set_ldo_input_source_failure(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	/* update_bits: read fails */
	no_os_i2c_write_IgnoreAndReturn(-EIO);

	int ret = max20362_set_ldo_input_source(&dev, MAX20362_LDO_SOURCE_BBOUT);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    CAPACITOR VOLTAGE ADDITIONAL CLAMPING TESTS
 ******************************************************************************/

void test_max20362_set_cap_voltage_500mv_above_max_clamps(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x00);
	no_os_field_prep_IgnoreAndReturn(0x0E);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* 9.0V with 500mV steps: (9000000-2500000)/500000 = 13, under max */
	/* Use 9500000 which gives vset=14=VSET_MAX exactly */
	int ret = max20362_set_cap_voltage(&dev, MAX20362_VCAP_500MV_MAX_UV,
					   MAX20362_VCAP_STEP_500MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_max20362_set_cap_voltage_250mv_above_range_clamps(void)
{
	struct max20362_dev dev = {.i2c_desc = &test_i2c_desc};

	no_os_field_prep_IgnoreAndReturn(0x10);
	no_os_field_prep_IgnoreAndReturn(0x0F);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_Stub(i2c_read_cb);
	test_read_val = 0x00;

	/* Value above 250mV range max (5350000) but within global max (9.5V)
	 * Should clamp to VSET_MAX=15 */
	int ret = max20362_set_cap_voltage(&dev, 6000000, MAX20362_VCAP_STEP_250MV);
	TEST_ASSERT_EQUAL_INT(0, ret);
}
