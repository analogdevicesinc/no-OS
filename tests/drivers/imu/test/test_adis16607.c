/***************************************************************************//**
 *   @file   test_adis16607.c
 *   @brief  Implementation of test_adis16607.c
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "adis.h"
#include "adis16607.h"
#include "test_adis.c"
#include "mock_no_os_delay.h"
#include "mock_no_os_util.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_spi.h"
#include "mock_no_os_alloc.h"
#include <errno.h>

/*******************************************************************************
 *    PUBLIC DATA
 ******************************************************************************/

const struct adis_chip_info *adis_chip_info = &adis16607_chip_info;
enum adis_device_id adis_dev_id = ADIS16607_3;

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void)
{
}

void tearDown(void)
{
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_adis16607_init(void)
{
	test_adis_init_1();
	test_adis_init_2();
	test_adis_init_4();
	test_adis_init_5();
}

void test_adis16607_remove(void)
{
	test_adis_remove_1();
	test_adis_remove_2();
}

void test_adis16607_initial_startup(void)
{
	test_adis_initial_startup_1();
	test_adis_initial_startup_2();
	test_adis_initial_startup_3();
}

void test_adis16607_read_reg(void)
{
	test_adis_read_reg();
}

/**
 * @brief Test that adis16607's custom write_reg rejects sizes other than
 * 2 bytes (unlike the generic write_reg path, which test_adis_write_reg_2
 * exercises for chips without a custom write_reg hook).
 */
void test_adis16607_write_reg_invalid_size(void)
{
	device_alloc.info = adis_chip_info;

	retval = adis_write_reg(&device_alloc, 0, 0, ADIS_1_BYTE_SIZE);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

void test_adis16607_write_reg(void)
{
	test_adis_write_reg_1();
	test_adis16607_write_reg_invalid_size();
}

void test_adis16607_read_field_s32(void)
{
	test_adis_read_field_s32();
}

void test_adis16607_read_diag_gyro1_failure(void)
{
	test_adis_read_diag_gyro1_failure_1();
	test_adis_read_diag_gyro1_failure_2();
	test_adis_read_diag_gyro1_failure_3();
}

void test_adis16607_read_diag_accl_failure(void)
{
	test_adis_read_diag_accl_failure_1();
	test_adis_read_diag_accl_failure_2();
	test_adis_read_diag_accl_failure_3();
}

void test_adis16607_read_diag_checksum_err(void)
{
	test_adis_read_diag_checksum_err_1();
	test_adis_read_diag_checksum_err_2();
}

void test_adis16607_read_x_gyro(void)
{
	test_adis_read_x_gyro_1();
	test_adis_read_x_gyro_2();
	test_adis_read_x_gyro_3();
}

void test_adis16607_read_y_gyro(void)
{
	test_adis_read_y_gyro_1();
	test_adis_read_y_gyro_2();
	test_adis_read_y_gyro_3();
}

void test_adis16607_read_z_gyro(void)
{
	test_adis_read_z_gyro_1();
	test_adis_read_z_gyro_2();
	test_adis_read_z_gyro_3();
}

void test_adis16607_read_x_accl(void)
{
	test_adis_read_x_accl_1();
	test_adis_read_x_accl_2();
	test_adis_read_x_accl_3();
}

void test_adis16607_read_y_accl(void)
{
	test_adis_read_y_accl_1();
	test_adis_read_y_accl_2();
	test_adis_read_y_accl_3();
}

void test_adis16607_read_z_accl(void)
{
	test_adis_read_z_accl_1();
	test_adis_read_z_accl_2();
	test_adis_read_z_accl_3();
}

void test_adis16607_read_temp_out(void)
{
	test_adis_read_temp_out_1();
	test_adis_read_temp_out_2();
	test_adis_read_temp_out_3();
}

void test_adis16607_read_time_stamp(void)
{
	test_adis_read_time_stamp_size16();
}

void test_adis16607_read_data_cntr(void)
{
	test_adis_read_data_cntr();
}

void test_adis16607_read_x_deltang(void)
{
	test_adis_read_x_deltang_1();
	test_adis_read_x_deltang_2();
	test_adis_read_x_deltang_3();
}

void test_adis16607_read_y_deltang(void)
{
	test_adis_read_y_deltang_1();
	test_adis_read_y_deltang_2();
	test_adis_read_y_deltang_3();
}

void test_adis16607_read_z_deltang(void)
{
	test_adis_read_z_deltang_1();
	test_adis_read_z_deltang_2();
	test_adis_read_z_deltang_3();
}

void test_adis16607_read_x_deltvel(void)
{
	test_adis_read_x_deltvel_1();
	test_adis_read_x_deltvel_2();
	test_adis_read_x_deltvel_3();
}

void test_adis16607_read_y_deltvel(void)
{
	test_adis_read_y_deltvel_1();
	test_adis_read_y_deltvel_2();
	test_adis_read_y_deltvel_3();
}

void test_adis16607_read_z_deltvel(void)
{
	test_adis_read_z_deltvel_1();
	test_adis_read_z_deltvel_2();
	test_adis_read_z_deltvel_3();
}

void test_adis16607_read_fifo_cnt(void)
{
	test_adis_read_fifo_cnt();
}

void test_adis16607_read_xg_bias(void)
{
	test_adis_read_xg_bias();
}

void test_adis16607_write_xg_bias(void)
{
	test_adis_write_xg_bias();
}

void test_adis16607_read_yg_bias(void)
{
	test_adis_read_yg_bias();
}

void test_adis16607_write_yg_bias(void)
{
	test_adis_write_yg_bias();
}

void test_adis16607_read_zg_bias(void)
{
	test_adis_read_zg_bias();
}

void test_adis16607_write_zg_bias(void)
{
	test_adis_write_zg_bias();
}

void test_adis16607_read_xa_bias(void)
{
	test_adis_read_xa_bias();
}

void test_adis16607_write_xa_bias(void)
{
	test_adis_write_xa_bias();
}

void test_adis16607_read_ya_bias(void)
{
	test_adis_read_ya_bias();
}

void test_adis16607_write_ya_bias(void)
{
	test_adis_write_ya_bias();
}

void test_adis16607_read_za_bias(void)
{
	test_adis_read_za_bias();
}

void test_adis16607_write_za_bias(void)
{
	test_adis_write_za_bias();
}

void test_adis16607_read_sync_mode(void)
{
	test_adis_read_sync_mode();
}

void test_adis16607_write_sync_mode(void)
{
	test_adis_write_sync_mode_1();
	test_adis_write_sync_mode_2();
	test_adis_write_sync_mode_3();
	test_adis_write_sync_mode_4();
	test_adis_write_sync_mode_5();
	test_adis_write_sync_mode_6();
	test_adis_write_sync_mode_7();
	test_adis_write_sync_mode_8();
	/* ADIS16607 does not support ADIS_SYNC_OUTPUT (sync_mode_max = ADIS_SYNC_SCALED). */
}

void test_adis16607_read_up_scale(void)
{
	test_adis_read_up_scale();
}

/**
 * @brief Test adis_write_up_scale with invalid up_scale parameter lower
 * limit. ADIS16607's custom read_sync_mode() does two register reads
 * (sync_mode field + GPIO sync-enable check), so unlike the generic path
 * exercised by test_adis_write_up_scale_2, it needs two distinct
 * no_os_get_unaligned_be16() return values to correctly resolve to
 * ADIS_SYNC_SCALED.
 */
void test_adis16607_write_up_scale_2(void)
{
	uint32_t up_scale = 0;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED - 1);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_up_scale with invalid up_scale parameter upper
 * limit. See test_adis16607_write_up_scale_2 for why this needs a
 * chip-specific mock sequence instead of test_adis_write_up_scale_3.
 */
void test_adis16607_write_up_scale_3(void)
{
	uint32_t up_scale = 2000;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED - 1);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

void test_adis16607_write_up_scale(void)
{
	test_adis_write_up_scale_1();
	test_adis16607_write_up_scale_2();
	test_adis16607_write_up_scale_3();
	test_adis_write_up_scale_4();
}

void test_adis16607_read_dec_rate(void)
{
	test_adis_read_dec_rate();
}

void test_adis16607_write_dec_rate(void)
{
	test_adis_write_dec_rate_1();
	test_adis_write_dec_rate_2();
	test_adis_write_dec_rate_3();
}

void test_adis16607_read_burst32(void)
{
	test_adis_read_burst32_1();
	test_adis_read_burst32_2();
}

void test_adis16607_write_burst32(void)
{
	test_adis_write_burst32_1();
	test_adis_write_burst32_2();
}

void test_adis16607_cmd_snsr_self_test(void)
{
	test_adis_cmd_snsr_self_test_1();
	test_adis_cmd_snsr_self_test_2();
}

void test_adis16607_cmd_fifo_flush(void)
{
	test_adis_cmd_fifo_flush();
}

void test_adis16607_cmd_sw_res(void)
{
	test_adis_cmd_sw_res_1();
	test_adis_cmd_sw_res_2();
}

void test_adis16607_read_firm_rev(void)
{
	test_adis_read_firm_rev();
}

void test_adis16607_read_prod_id(void)
{
	test_adis_read_prod_id();
}

void test_adis16607_read_serial_num(void)
{
	test_adis_read_serial_num();
}

void test_adis16607_read_burst_data(void)
{
	test_adis_read_burst_data_1();
	test_adis_read_burst_data_2();
	test_adis_read_burst_data_3();
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to
 * ADIS_SYNC_OUTPUT. See test_adis16607_write_up_scale_2 for why this needs
 * a chip-specific mock sequence instead of test_adis_update_ext_clk_freq_3.
 */
void test_adis16607_update_ext_clk_freq_3(void)
{
	uint32_t clk_freq = 100;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_OUTPUT - 1);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to
 * ADIS_SYNC_DIRECT with invalid clk_freq for lower limit.
 */
void test_adis16607_update_ext_clk_freq_4(void)
{
	uint32_t clk_freq = 0;
	uint32_t clk_freq_before = device_alloc.ext_clk;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT - 1);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq_before, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to
 * ADIS_SYNC_DIRECT with invalid clk_freq for upper limit.
 */
void test_adis16607_update_ext_clk_freq_5(void)
{
	uint32_t clk_freq = 10000;
	uint32_t clk_freq_before = device_alloc.ext_clk;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT - 1);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq_before, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to
 * ADIS_SYNC_DIRECT with valid clk_freq.
 */
void test_adis16607_update_ext_clk_freq_6(void)
{
	uint32_t clk_freq = 2000;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT - 1);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq, device_alloc.ext_clk);
}

void test_adis16607_update_ext_clk_freq(void)
{
	test_adis_update_ext_clk_freq_1();
	test_adis_update_ext_clk_freq_2();
	test_adis16607_update_ext_clk_freq_3();
	test_adis16607_update_ext_clk_freq_4();
	test_adis16607_update_ext_clk_freq_5();
	test_adis16607_update_ext_clk_freq_6();
}

/**
 * @brief Test adis_get_sync_clk_freq with sync mode set to
 * ADIS_SYNC_OUTPUT. See test_adis16607_write_up_scale_2 for why this needs
 * a chip-specific mock sequence instead of test_adis_get_sync_clk_freq_3.
 */
void test_adis16607_get_sync_clk_freq_3(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;
	device_alloc.int_clk = adis_chip_info->int_clk;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_OUTPUT - 1);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(adis_chip_info->int_clk, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_sync_clk_freq with sync mode set to
 * ADIS_SYNC_DIRECT.
 */
void test_adis16607_get_sync_clk_freq_4(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;

	device_alloc.ext_clk = 2100;
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(NO_OS_GENMASK(8, 6));
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT - 1);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(2100, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

void test_adis16607_get_sync_clk_freq(void)
{
	test_adis_get_sync_clk_freq_1();
	test_adis_get_sync_clk_freq_2();
	test_adis16607_get_sync_clk_freq_3();
	test_adis16607_get_sync_clk_freq_4();
}

void test_adis16607_get_anglvel_scale(void)
{
	test_adis_get_anglvel_scale_1();
	test_adis_get_anglvel_scale_2();
	test_adis_get_anglvel_scale_3();
	test_adis_get_anglvel_scale_4();
}

void test_adis16607_get_accl_scale(void)
{
	test_adis_get_accl_scale_1();
	test_adis_get_accl_scale_2();
	test_adis_get_accl_scale_3();
	test_adis_get_accl_scale_4();
}

void test_adis16607_get_deltaangl_scale(void)
{
	test_adis_get_deltaangl_scale_1();
	test_adis_get_deltaangl_scale_2();
	test_adis_get_deltaangl_scale_3();
	test_adis_get_deltaangl_scale_4();
}

void test_adis16607_get_deltavelocity_scale(void)
{
	test_adis_get_deltavelocity_scale_1();
	test_adis_get_deltavelocity_scale_2();
	test_adis_get_deltavelocity_scale_3();
	test_adis_get_deltavelocity_scale_4();
}

void test_adis16607_get_temp_scale(void)
{
	test_adis_get_temp_scale_1();
	test_adis_get_temp_scale_2();
	test_adis_get_temp_scale_3();
	test_adis_get_temp_scale_4();
}
