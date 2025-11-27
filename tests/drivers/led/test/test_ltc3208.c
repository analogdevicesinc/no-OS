/***************************************************************************//**
 *   @file   test_ltc3208.c
 *   @brief  Unit tests for LTC3208 LED driver
 *   @author Edelweise Escala (edelweise.escala@analog.com)
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

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "unity.h"
#include "ltc3208.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_util.h"
#include "mock_no_os_delay.h"
#include <errno.h>
#include <stdlib.h>

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static struct ltc3208_dev *test_dev;
static struct ltc3208_init_param test_init_param;
static struct no_os_i2c_desc mock_i2c_desc;
static struct no_os_gpio_desc mock_gpio_desc;
static struct ltc3208_dev mock_dev;

/*******************************************************************************
 *    STUB CALLBACKS
 ******************************************************************************/

static int stub_i2c_init_success(struct no_os_i2c_desc **desc,
				 const struct no_os_i2c_init_param *param, int cmock_num_calls)
{
	*desc = &mock_i2c_desc;
	return 0;
}

static int stub_i2c_init_failure(struct no_os_i2c_desc **desc,
				 const struct no_os_i2c_init_param *param, int cmock_num_calls)
{
	return -EIO;
}

static int stub_gpio_get_success(struct no_os_gpio_desc **desc,
				 const struct no_os_gpio_init_param *param, int cmock_num_calls)
{
	*desc = &mock_gpio_desc;
	return 0;
}

static int stub_gpio_get_failure(struct no_os_gpio_desc **desc,
				 const struct no_os_gpio_init_param *param, int cmock_num_calls)
{
	return -EIO;
}

static int stub_gpio_direction_output_success(struct no_os_gpio_desc *desc,
		uint8_t value, int cmock_num_calls)
{
	return 0;
}

static int stub_i2c_remove_success(struct no_os_i2c_desc *desc,
				   int cmock_num_calls)
{
	return 0;
}

static int stub_gpio_remove_success(struct no_os_gpio_desc *desc,
				    int cmock_num_calls)
{
	return 0;
}

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void)
{
	test_dev = NULL;

	test_init_param.i2c_init_param.device_id = 0;
	test_init_param.i2c_init_param.slave_address = 0x7D;
	test_init_param.gpio_init_param.number = 1;
}

void tearDown(void)
{
	test_dev = NULL;
}

/*******************************************************************************
 *    INITIALIZATION TESTS
 ******************************************************************************/

/**
 * @brief Test successful initialization of LTC3208 device
 */
void test_ltc3208_init_success(void)
{
	int ret;
	struct ltc3208_dev *dev_ptr;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc3208_dev), &mock_dev);
	no_os_i2c_init_Stub(stub_i2c_init_success);
	no_os_gpio_get_Stub(stub_gpio_get_success);
	no_os_gpio_direction_output_Stub(stub_gpio_direction_output_success);

	ret = ltc3208_init(&dev_ptr, test_init_param);

	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev_ptr);
	TEST_ASSERT_EQUAL_PTR(&mock_dev, dev_ptr);
	TEST_ASSERT_EQUAL_PTR(&mock_i2c_desc, dev_ptr->i2c_desc);
	TEST_ASSERT_EQUAL_PTR(&mock_gpio_desc, dev_ptr->gpio_enrgbs_desc);
}

/**
 * @brief Test initialization failure when memory allocation fails
 */
void test_ltc3208_init_alloc_failure(void)
{
	int ret;
	struct ltc3208_dev *dev_ptr = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc3208_dev), NULL);

	ret = ltc3208_init(&dev_ptr, test_init_param);

	TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
}

/**
 * @brief Test initialization failure when I2C init fails
 */
void test_ltc3208_init_i2c_failure(void)
{
	int ret;
	struct ltc3208_dev *dev_ptr;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc3208_dev), &mock_dev);
	no_os_i2c_init_Stub(stub_i2c_init_failure);
	no_os_i2c_remove_Stub(stub_i2c_remove_success);
	no_os_gpio_remove_Stub(stub_gpio_remove_success);
	no_os_free_Expect(&mock_dev);

	ret = ltc3208_init(&dev_ptr, test_init_param);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test initialization failure when GPIO init fails
 */
void test_ltc3208_init_gpio_failure(void)
{
	int ret;
	struct ltc3208_dev *dev_ptr;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc3208_dev), &mock_dev);
	no_os_i2c_init_Stub(stub_i2c_init_success);
	no_os_gpio_get_Stub(stub_gpio_get_failure);
	no_os_i2c_remove_Stub(stub_i2c_remove_success);
	no_os_gpio_remove_Stub(stub_gpio_remove_success);
	no_os_free_Expect(&mock_dev);

	ret = ltc3208_init(&dev_ptr, test_init_param);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

void test_ltc3208_init_gpio_direction_failure(void)
{
	int ret;
	struct ltc3208_dev *dev_ptr = NULL;

	no_os_calloc_ExpectAndReturn(1, sizeof(struct ltc3208_dev), &mock_dev);
	no_os_i2c_init_Stub(stub_i2c_init_success);
	no_os_gpio_get_Stub(stub_gpio_get_success);
	no_os_gpio_direction_output_ExpectAndReturn(&mock_gpio_desc, NO_OS_GPIO_HIGH,
			-EIO);

	no_os_gpio_remove_Stub(stub_gpio_remove_success);
	no_os_i2c_remove_Stub(stub_i2c_remove_success);
	no_os_free_Expect(&mock_dev);

	ret = ltc3208_init(&dev_ptr, test_init_param);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    REMOVE TESTS
 ******************************************************************************/

/**
 * @brief Test successful removal of LTC3208 device
 */
void test_ltc3208_remove_success(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;
	dev.gpio_enrgbs_desc = &mock_gpio_desc;

	no_os_i2c_remove_Stub(stub_i2c_remove_success);
	no_os_gpio_remove_Stub(stub_gpio_remove_success);
	no_os_free_Expect(&dev);

	ret = ltc3208_remove(&dev);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test i2c fail removal of LTC3208 device
 */
void test_ltc3208_remove_i2c_fail(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;
	dev.gpio_enrgbs_desc = &mock_gpio_desc;

	no_os_i2c_remove_ExpectAndReturn(&mock_i2c_desc, -EIO);

	ret = ltc3208_remove(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test gpio fail removal of LTC3208 device
 */
void test_ltc3208_remove_enrgb_gpio_fail(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;
	dev.gpio_enrgbs_desc = &mock_gpio_desc;

	no_os_i2c_remove_Stub(stub_i2c_remove_success);
	no_os_gpio_remove_ExpectAndReturn(&mock_gpio_desc, -EIO);

	ret = ltc3208_remove(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test remove with NULL device pointer
 */
void test_ltc3208_remove_null_device(void)
{
	int ret;

	ret = ltc3208_remove(NULL);

	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/*******************************************************************************
 *    REGISTER WRITE TESTS
 ******************************************************************************/

/**
 * @brief Test successful register write
 */
void test_ltc3208_reg_write_success(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_MAIN, 0x55};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_reg_write(&dev, LTC3208_REG_MAIN, 0x55);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test register write with invalid register address
 */
void test_ltc3208_reg_write_invalid_address(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;

	ret = ltc3208_reg_write(&dev, 0xFF, 0x55);

	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    8-BIT DAC TESTS
 ******************************************************************************/

/**
 * @brief Test setting MAIN register with 8-bit DAC
 */
void test_ltc3208_set_8_bit_dac_main(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t current_level = 128;

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, current_level,
					 current_level);
	expected_buffer[1] = current_level;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_8_bit_dac(&dev, MAIN_REG, current_level);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting SUB register with 8-bit DAC
 */
void test_ltc3208_set_8_bit_dac_sub(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t current_level = 200;

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, current_level,
					 current_level);
	expected_buffer[1] = current_level;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_8_bit_dac(&dev, SUB_REG, current_level);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test 8-bit DAC with invalid register
 */
void test_ltc3208_set_8_bit_dac_invalid_register(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 100, 100);

	ret = ltc3208_set_8_bit_dac(&dev, GREEN_RED_REG, 100);

	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    4-BIT DAC TESTS
 ******************************************************************************/

/**
 * @brief Test setting GREEN_RED register with 4-bit DAC
 */
void test_ltc3208_set_4_bit_dac_green_red(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_RED_GREEN, 0};
	uint8_t high_level = 8;
	uint8_t low_level = 12;
	uint8_t combined = 0x8C;

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, high_level,
					 high_level << 4);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, low_level, low_level);
	expected_buffer[1] = combined;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_4_bit_dac(&dev, GREEN_RED_REG, high_level, low_level);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting AUX_BLUE register with 4-bit DAC
 */
void test_ltc3208_set_4_bit_dac_aux_blue(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_BLUE_AUX, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 15, 0xF0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 3, 0x03);
	expected_buffer[1] = 0xF3;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_4_bit_dac(&dev, AUX_BLUE_REG, 15, 3);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting CAM register with 4-bit DAC
 */
void test_ltc3208_set_4_bit_dac_cam(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_CAM_HIGH_LOW, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 10, 0xA0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 5, 0x05);
	expected_buffer[1] = 0xA5;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_4_bit_dac(&dev, CAM_REG, 10, 5);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test 4-bit DAC with invalid register
 */
void test_ltc3208_set_4_bit_dac_invalid_register(void)
{
	int ret;
	struct ltc3208_dev dev;

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 10, 0xA0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 5, 0x05);

	ret = ltc3208_set_4_bit_dac(&dev, SUB_REG, 10, 5);

	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

/*******************************************************************************
 *    AUX LED DAC TESTS
 ******************************************************************************/

/**
 * @brief Test setting all AUX LED DACs
 */
void test_ltc3208_set_all_aux_led_dac_success(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_AUX_DAC_SEL, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_AUX1_MASK, MAIN, MAIN);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX2_MASK, SUB, SUB << 2);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX3_MASK, AUX, AUX << 4);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX4_MASK, CAM, CAM << 6);
	expected_buffer[1] = (MAIN) | (SUB << 2) | (AUX << 4) | (CAM << 6);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_all_aux_led_dac(&dev, MAIN, SUB, AUX, CAM);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting a single AUX LED DAC
 */
void test_ltc3208_set_aux_led_dac_single(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_AUX_DAC_SEL, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.aux_dac[0] = MAIN;
	dev.aux_dac[1] = MAIN;
	dev.aux_dac[2] = MAIN;
	dev.aux_dac[3] = MAIN;

	no_os_field_prep_ExpectAndReturn(LTC3208_AUX1_MASK, MAIN, MAIN);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX2_MASK, SUB, SUB << 2);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX3_MASK, MAIN, MAIN << 4);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX4_MASK, MAIN, MAIN << 6);
	expected_buffer[1] = (MAIN) | (SUB << 2) | (MAIN << 4) | (MAIN << 6);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_aux_led_dac(&dev, SUB, 1);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting all AUX LED DACs when I2C write fails
 */
void test_ltc3208_set_all_aux_led_dac_i2c_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_AUX_DAC_SEL, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_AUX1_MASK, MAIN, MAIN);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX2_MASK, SUB, SUB << 2);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX3_MASK, AUX, AUX << 4);
	no_os_field_prep_ExpectAndReturn(LTC3208_AUX4_MASK, CAM, CAM << 6);
	expected_buffer[1] = (MAIN) | (SUB << 2) | (AUX << 4) | (CAM << 6);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, -EIO);

	ret = ltc3208_set_all_aux_led_dac(&dev, MAIN, SUB, AUX, CAM);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/*******************************************************************************
 *    OPTIONS UPDATE TESTS
 ******************************************************************************/

/**
 * @brief Test updating options register
 */
void test_ltc3208_update_options_success(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, true, 0x01);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, true, 0x10);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, true, 0x40);
	expected_buffer[1] = 0x51;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_update_options(&dev, true, false, true, false, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting CPO 2x mode
 */
void test_ltc3208_set_cpo_2x(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.options_cfg.is_force_2x = false;
	dev.options_cfg.is_force_1p5x = false;
	dev.options_cfg.is_dropout_disable = false;
	dev.options_cfg.is_cam_high = false;
	dev.options_cfg.is_sub_enable = false;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, true, 0x01);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, false, 0x00);
	expected_buffer[1] = 0x01;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_cpo_2x(&dev, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting CPO 1p5x mode
 */
void test_ltc3208_set_cpo_1p5x(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.options_cfg.is_force_2x = false;
	dev.options_cfg.is_force_1p5x = false;
	dev.options_cfg.is_dropout_disable = false;
	dev.options_cfg.is_cam_high = false;
	dev.options_cfg.is_sub_enable = false;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, true, 0x01);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, false, 0x00);
	expected_buffer[1] = 0x01;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_1p5x(&dev, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting dropout
 */
void test_ltc3208_set_droupout_disable(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.options_cfg.is_force_2x = false;
	dev.options_cfg.is_force_1p5x = false;
	dev.options_cfg.is_dropout_disable = false;
	dev.options_cfg.is_cam_high = false;
	dev.options_cfg.is_sub_enable = false;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, true, 0x01);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, false, 0x00);
	expected_buffer[1] = 0x01;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_droupout_disable(&dev, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting CAM high mode
 */
void test_ltc3208_set_cam_high(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.options_cfg.is_force_2x = false;
	dev.options_cfg.is_force_1p5x = false;
	dev.options_cfg.is_dropout_disable = false;
	dev.options_cfg.is_cam_high = false;
	dev.options_cfg.is_sub_enable = false;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, true, 0x20);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, false, 0x00);
	expected_buffer[1] = 0x20;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_cam_high(&dev, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test setting SUB enable mode
 */
void test_ltc3208_set_sub_enable(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t expected_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;
	dev.options_cfg.is_force_2x = false;
	dev.options_cfg.is_force_1p5x = false;
	dev.options_cfg.is_dropout_disable = false;
	dev.options_cfg.is_cam_high = false;
	dev.options_cfg.is_sub_enable = false;

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, false, 0x00);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, true, 0x40);
	expected_buffer[1] = 0x40;
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, expected_buffer, 2, 1, 0);

	ret = ltc3208_set_sub_enable(&dev, true);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/*******************************************************************************
 *    RESET TESTS
 ******************************************************************************/

/**
 * @brief Test device reset
 */
void test_ltc3208_reset_success(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t green_red_buffer[2] = {LTC3208_REG_RED_GREEN, 0};
	uint8_t blue_aux_buffer[2] = {LTC3208_REG_BLUE_AUX, 0};
	uint8_t cam_buffer[2] = {LTC3208_REG_CAM_HIGH_LOW, 0};
	uint8_t options_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, green_red_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, blue_aux_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, cam_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, options_buffer, 2, 1, 0);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @brief Test reset failure when MAIN register write fails
 */
void test_ltc3208_reset_main_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test reset failure when SUB register write fails
 */
void test_ltc3208_reset_sub_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test reset failure when GREEN_RED register write fails
 */
void test_ltc3208_reset_green_red_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t green_red_buffer[2] = {LTC3208_REG_RED_GREEN, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, green_red_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test reset failure when BLUE_AUX register write fails
 */
void test_ltc3208_reset_blue_aux_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t green_red_buffer[2] = {LTC3208_REG_RED_GREEN, 0};
	uint8_t blue_aux_buffer[2] = {LTC3208_REG_BLUE_AUX, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, green_red_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, blue_aux_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test reset failure when CAM register write fails
 */
void test_ltc3208_reset_cam_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t green_red_buffer[2] = {LTC3208_REG_RED_GREEN, 0};
	uint8_t blue_aux_buffer[2] = {LTC3208_REG_BLUE_AUX, 0};
	uint8_t cam_buffer[2] = {LTC3208_REG_CAM_HIGH_LOW, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, green_red_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, blue_aux_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, cam_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}

/**
 * @brief Test reset failure when OPTIONS register write fails
 */
void test_ltc3208_reset_options_reg_fail(void)
{
	int ret;
	struct ltc3208_dev dev;
	uint8_t main_buffer[2] = {LTC3208_REG_MAIN, 0};
	uint8_t sub_buffer[2] = {LTC3208_REG_SUB, 0};
	uint8_t green_red_buffer[2] = {LTC3208_REG_RED_GREEN, 0};
	uint8_t blue_aux_buffer[2] = {LTC3208_REG_BLUE_AUX, 0};
	uint8_t cam_buffer[2] = {LTC3208_REG_CAM_HIGH_LOW, 0};
	uint8_t options_buffer[2] = {LTC3208_REG_OPTIONS, 0};

	dev.i2c_desc = &mock_i2c_desc;

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, main_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_8_BIT_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, sub_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, green_red_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, blue_aux_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_HIGH_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_4_BIT_LOW_MASK, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, cam_buffer, 2, 1, 0);

	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_2X_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_CPO_1P5X_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_1_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_TEST_HOOK_2_MASK, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_DROPOUT_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_CAM_BIT, 0, 0);
	no_os_field_prep_ExpectAndReturn(LTC3208_RGB_BIT, 0, 0);
	no_os_i2c_write_ExpectAndReturn(&mock_i2c_desc, options_buffer, 2, 1, -EIO);

	ret = ltc3208_reset(&dev);

	TEST_ASSERT_EQUAL_INT(-EIO, ret);
}
