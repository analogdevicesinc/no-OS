/***************************************************************************//**
 *   @file   adis_generic_tests.c
 *   @brief  Implementation of adis_generic_tests.c
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "unity.h"
#include "adis.h"
#include "adis_internals.h"
#include "mock_no_os_delay.h"
#include "mock_no_os_util.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_spi.h"
#include "mock_no_os_alloc.h"
#include <errno.h>

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static struct adis_dev device_alloc;
static struct no_os_spi_desc spi_desc;
static struct adis_init_param ip;
static struct no_os_gpio_desc gpio_reset_desc;
static int retval;

/*******************************************************************************
 *    PUBLIC DATA
 ******************************************************************************/

extern enum adis_device_id adis_dev_id;
extern const struct adis_chip_info *adis_chip_info;

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

/**
 * @brief Test adis_init with unsuccessful memory allocation.
 */
void test_adis_init_1(void)
{
	struct adis_dev *device;
	ip.info = adis_chip_info;

	no_os_calloc_IgnoreAndReturn(NULL);
	retval = adis_init(&device, &ip);
	TEST_ASSERT_EQUAL_INT(-ENOMEM, retval);
}

/**
 * @brief Test adis_init with unsuccessful spi init.
 */
void test_adis_init_2(void)
{
	struct adis_dev *device;
	device_alloc.spi_desc = &spi_desc;
	ip.info = adis_chip_info;
	no_os_calloc_IgnoreAndReturn(&device_alloc);
	no_os_spi_init_IgnoreAndReturn(-1);
	no_os_free_Ignore();
	retval = adis_init(&device, &ip);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_init with unsuccessful gpio reset and has paging true.
 */
void test_adis_init_3(void)
{
	struct adis_dev *device;
	device_alloc.spi_desc = &spi_desc;
	device_alloc.gpio_reset = &gpio_reset_desc;
	ip.info = adis_chip_info;

	no_os_calloc_IgnoreAndReturn(&device_alloc);
	no_os_spi_init_IgnoreAndReturn(0);
	no_os_gpio_get_optional_IgnoreAndReturn(0);
	no_os_gpio_direction_output_IgnoreAndReturn(-1);
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_spi_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	retval = adis_init(&device, &ip);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_init with valid sync mode writing and gpio reset null.
 */
void test_adis_init_4(void)
{
	struct adis_dev *device;
	device_alloc.spi_desc = &spi_desc;
	device_alloc.gpio_reset = NULL;
	ip.sync_mode = 0;
	ip.info = adis_chip_info;

	no_os_calloc_IgnoreAndReturn(&device_alloc);
	no_os_spi_init_IgnoreAndReturn(0);
	no_os_gpio_get_optional_IgnoreAndReturn(-1);
	no_os_gpio_set_value_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_init(&device, &ip);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_init with invalid sync mode writing.
 */
void test_adis_init_5(void)
{
	struct adis_dev *device;
	device_alloc.spi_desc = &spi_desc;
	ip.sync_mode = 5;
	device_alloc.gpio_reset = &gpio_reset_desc;
	ip.info = adis_chip_info;

	no_os_calloc_IgnoreAndReturn(&device_alloc);
	no_os_spi_init_IgnoreAndReturn(0);
	no_os_gpio_get_optional_IgnoreAndReturn(0);
	no_os_gpio_direction_output_IgnoreAndReturn(0);
	no_os_gpio_set_value_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_spi_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	retval = adis_init(&device, &ip);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_remove with gpio_reset and spi_desc not null.
 */
void test_adis_remove_1(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.gpio_reset = &gpio_reset_desc;
	device_alloc.spi_desc = &spi_desc;

	no_os_gpio_remove_IgnoreAndReturn(0);
	no_os_spi_remove_IgnoreAndReturn(0);
	no_os_free_Ignore();
	adis_remove(&device_alloc);
}

/**
 * @brief Test adis_remove with gpio_reset and spi_desc null.
 */
void test_adis_remove_2(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.gpio_reset = NULL;
	device_alloc.spi_desc = NULL;

	no_os_free_Ignore();
	adis_remove(&device_alloc);
}

/**
 * @brief Test adis_adis_initial_startup with soft reset in case no reset pin is
 * configured with invalid transfer for adis_cmd_sw_res.
 */
void test_adis_initial_startup_1(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.gpio_reset = NULL;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_initial_startup(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_adis_initial_startup with soft reset in case no reset pin is
 * configured  with invalid transfer for adis_cmd_snsr_self_test
 */
void test_adis_initial_startup_2(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.gpio_reset = NULL;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_initial_startup(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_adis_initial_startup with soft reset in case no reset pin is
 * configured  with invalid transfer for adis_cmd_fls_mem_test.
 */
void test_adis_initial_startup_3(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.gpio_reset = NULL;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_initial_startup(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_reg with invalid size register write with current_page
 * different from page.
 */
void test_adis_read_reg(void)
{
	uint32_t val;
	device_alloc.info = adis_chip_info;
	device_alloc.current_page = 1;

	retval = adis_read_reg(&device_alloc, 0, &val, 5);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	device_alloc.current_page = 0;
}

/**
 * @brief Test adis_write_reg with invalid size register write with current_page
 * different from page.
 */
void test_adis_write_reg_1(void)
{
	device_alloc.info = adis_chip_info;
	device_alloc.current_page = 1;

	retval = adis_write_reg(&device_alloc, 0, 0, 5);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	device_alloc.current_page = 0;
}

/**
 * @brief Test adis_write_reg with size equal to 1.
 */
void test_adis_write_reg_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	retval = adis_write_reg(&device_alloc, 0, 0, 1);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_field_s32 with invalid SPI transfer.
 */
extern int adis_read_field_s32(struct adis_dev *adis, struct adis_field field,
			       int32_t *field_val);
void test_adis_read_field_s32(void)
{
	struct adis_field field;
	int32_t field_val;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_field_s32(&device_alloc,
				     device_alloc.info->field_map->x_gyro, &field_val);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_snsr_init_failure unsuccessful transfer.
 */
void test_adis_read_diag_snsr_init_failure_1(void)
{
	uint32_t snsr_init_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_snsr_init_failure(&device_alloc, &snsr_init_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_snsr_init_failure successful transfer flag true.
 */
void test_adis_read_diag_snsr_init_failure_2(void)
{
	uint32_t snsr_init_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_snsr_init_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_snsr_init_failure(&device_alloc, &snsr_init_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, snsr_init_failure);
}

/**
 * @brief Test adis_read_diag_snsr_init_failure successful transfer flag false.
 */
void test_adis_read_diag_snsr_init_failure_3(void)
{
	uint32_t snsr_init_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_snsr_init_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_snsr_init_failure(&device_alloc, &snsr_init_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, snsr_init_failure);
}

/**
 * @brief Test adis_read_diag_data_path_overrun unsuccessful transfer.
 */
void test_adis_read_diag_data_path_overrun_1(void)
{
	uint32_t data_path_overrun;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_data_path_overrun(&device_alloc, &data_path_overrun);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_data_path_overrun successful transfer flag true.
 */
void test_adis_read_diag_data_path_overrun_2(void)
{
	uint32_t data_path_overrun;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_data_path_overrun_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_data_path_overrun(&device_alloc, &data_path_overrun);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, data_path_overrun);
}

/**
 * @brief Test adis_read_diag_data_path_overrun successful transfer flag false.
 */
void test_adis_read_diag_data_path_overrun_3(void)
{
	uint32_t data_path_overrun;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_data_path_overrun_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_data_path_overrun(&device_alloc, &data_path_overrun);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, data_path_overrun);
}

/**
 * @brief Test adis_read_diag_fls_mem_update_failure unsuccessful transfer.
 */
void test_adis_read_diag_fls_mem_update_failure_1(void)
{
	uint32_t fls_mem_update_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_fls_mem_update_failure(&device_alloc,
			&fls_mem_update_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_fls_mem_update_failure successful transfer flag true.
 */
void test_adis_read_diag_fls_mem_update_failure_2(void)
{
	uint32_t fls_mem_update_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_fls_mem_update_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_fls_mem_update_failure(&device_alloc,
			&fls_mem_update_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, fls_mem_update_failure);
}

/**
 * @brief Test adis_read_diag_fls_mem_update_failure successful transfer flag false.
 */
void test_adis_read_diag_fls_mem_update_failure_3(void)
{
	uint32_t fls_mem_update_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_fls_mem_update_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_fls_mem_update_failure(&device_alloc,
			&fls_mem_update_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, fls_mem_update_failure);
}

/**
 * @brief Test adis_read_diag_spi_comm_err unsuccessful transfer.
 */
void test_adis_read_diag_spi_comm_err_1(void)
{
	uint32_t spi_comm_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_spi_comm_err(&device_alloc, &spi_comm_err);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_spi_comm_err successful transfer flag true.
 */
void test_adis_read_diag_spi_comm_err_2(void)
{
	uint32_t spi_comm_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_spi_comm_err_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_spi_comm_err(&device_alloc, &spi_comm_err);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, spi_comm_err);
}

/**
 * @brief Test adis_read_diag_spi_comm_err successful transfer flag false.
 */
void test_adis_read_diag_spi_comm_err_3(void)
{
	uint32_t spi_comm_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_spi_comm_err_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_spi_comm_err(&device_alloc, &spi_comm_err);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, spi_comm_err);
}

/**
 * @brief Test adis_read_diag_standby_mode unsuccessful transfer.
 */
void test_adis_read_diag_standby_mode_1(void)
{
	uint32_t standby_mode;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_standby_mode(&device_alloc, &standby_mode);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_standby_mode successful transfer flag true.
 */
void test_adis_read_diag_standby_mode_2(void)
{
	uint32_t standby_mode;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_standby_mode_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_standby_mode(&device_alloc, &standby_mode);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, standby_mode);
}

/**
 * @brief Test adis_read_diag_standby_mode successful transfer flag false.
 */
void test_adis_read_diag_standby_mode_3(void)
{
	uint32_t standby_mode;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_standby_mode_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_standby_mode(&device_alloc, &standby_mode);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, standby_mode);
}

/**
 * @brief Test adis_read_diag_snsr_failure unsuccessful transfer.
 */
void test_adis_read_diag_snsr_failure_1(void)
{
	uint32_t snsr_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_snsr_failure(&device_alloc, &snsr_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_snsr_failure successful transfer flag true.
 */
void test_adis_read_diag_snsr_failure_2(void)
{
	uint32_t snsr_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_snsr_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_snsr_failure(&device_alloc, &snsr_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, snsr_failure);
}

/**
 * @brief Test adis_read_diag_snsr_failure successful transfer flag false.
 */
void test_adis_read_diag_snsr_failure_3(void)
{
	uint32_t snsr_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_snsr_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_snsr_failure(&device_alloc, &snsr_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, snsr_failure);
}

/**
 * @brief Test adis_read_diag_mem_failure unsuccessful transfer.
 */
void test_adis_read_diag_mem_failure_1(void)
{
	uint32_t mem_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_mem_failure(&device_alloc, &mem_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_mem_failure successful transfer flag true.
 */
void test_adis_read_diag_mem_failure_2(void)
{
	uint32_t mem_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_mem_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_mem_failure(&device_alloc, &mem_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, mem_failure);
}

/**
 * @brief Test adis_read_diag_mem_failure successful transfer flag false.
 */
void test_adis_read_diag_mem_failure_3(void)
{
	uint32_t mem_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_mem_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_mem_failure(&device_alloc, &mem_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, mem_failure);
}

/**
 * @brief Test adis_read_diag_clk_err unsuccessful transfer.
 */
void test_adis_read_diag_clk_err_1(void)
{
	uint32_t clk_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_clk_err(&device_alloc, &clk_err);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_clk_err successful transfer flag true.
 */
void test_adis_read_diag_clk_err_2(void)
{
	uint32_t clk_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_clk_err_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_clk_err(&device_alloc, &clk_err);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, clk_err);
}

/**
 * @brief Test adis_read_diag_clk_err successful transfer flag false.
 */
void test_adis_read_diag_clk_err_3(void)
{
	uint32_t clk_err;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_clk_err_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_clk_err(&device_alloc, &clk_err);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, clk_err);
}

/**
 * @brief Test adis_read_diag_gyro1_failure unsuccessful transfer.
 */
void test_adis_read_diag_gyro1_failure_1(void)
{
	uint32_t gyro1_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_gyro1_failure(&device_alloc, &gyro1_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_gyro1_failure successful transfer flag true.
 */
void test_adis_read_diag_gyro1_failure_2(void)
{
	uint32_t gyro1_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_gyro1_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_gyro1_failure(&device_alloc, &gyro1_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, gyro1_failure);
}

/**
 * @brief Test adis_read_diag_gyro1_failure successful transfer flag false.
 */
void test_adis_read_diag_gyro1_failure_3(void)
{
	uint32_t gyro1_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_gyro1_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_gyro1_failure(&device_alloc, &gyro1_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, gyro1_failure);
}

/**
 * @brief Test adis_read_diag_gyro2_failure unsuccessful transfer.
 */
void test_adis_read_diag_gyro2_failure_1(void)
{
	uint32_t gyro2_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_gyro2_failure(&device_alloc, &gyro2_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_gyro2_failure successful transfer flag true.
 */
void test_adis_read_diag_gyro2_failure_2(void)
{
	uint32_t gyro2_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_gyro2_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_gyro2_failure(&device_alloc, &gyro2_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, gyro2_failure);
}

/**
 * @brief Test adis_read_diag_gyro2_failure successful transfer flag false.
 */
void test_adis_read_diag_gyro2_failure_3(void)
{
	uint32_t gyro2_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_gyro2_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_gyro2_failure(&device_alloc, &gyro2_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, gyro2_failure);
}

/**
 * @brief Test adis_read_diag_accl_failure unsuccessful transfer.
 */
void test_adis_read_diag_accl_failure_1(void)
{
	uint32_t accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_accl_failure(&device_alloc, &accl_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_accl_failure successful transfer flag true.
 */
void test_adis_read_diag_accl_failure_2(void)
{
	uint32_t accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_accl_failure(&device_alloc, &accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, accl_failure);
}

/**
 * @brief Test adis_read_diag_accl_failure successful transfer flag false.
 */
void test_adis_read_diag_accl_failure_3(void)
{
	uint32_t accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_accl_failure(&device_alloc, &accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, accl_failure);
}

/**
 * @brief Test adis_read_diag_x_gyro_accl_failure unsuccessful transfer.
 */
void test_adis_read_diag_x_axis_gyro_failure_1(void)
{
	uint32_t x_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_x_axis_gyro_failure(&device_alloc,
			&x_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_x_axis_gyro_failure successful transfer flag true.
 */
void test_adis_read_diag_x_axis_gyro_failure_2(void)
{
	uint32_t x_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_x_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_x_axis_gyro_failure(&device_alloc,
			&x_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, x_axis_gyro_failure);
}

/**
 * @brief Test adis_read_diag_x_axis_gyro_failure successful transfer flag false.
 */
void test_adis_read_diag_x_axis_gyro_failure_3(void)
{
	uint32_t x_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_x_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_x_axis_gyro_failure(&device_alloc,
			&x_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, x_axis_gyro_failure);
}

/**
 * @brief Test adis_read_diag_y_axis_gyro_failure unsuccessful transfer.
 */
void test_adis_read_diag_y_axis_gyro_failure_1(void)
{
	uint32_t y_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_y_axis_gyro_failure(&device_alloc,
			&y_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_y_axis_gyro_failure successful transfer flag true.
 */
void test_adis_read_diag_y_axis_gyro_failure_2(void)
{
	uint32_t y_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_y_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_y_axis_gyro_failure(&device_alloc,
			&y_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, y_axis_gyro_failure);
}

/**
 * @brief Test adis_read_diag_y_axis_gyro_failure successful transfer flag false.
 */
void test_adis_read_diag_y_axis_gyro_failure_3(void)
{
	uint32_t y_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_y_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_y_axis_gyro_failure(&device_alloc,
			&y_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, y_axis_gyro_failure);
}


/**
 * @brief Test adis_read_diag_z_axis_gyro_failure unsuccessful transfer.
 */
void test_adis_read_diag_z_axis_gyro_failure_1(void)
{
	uint32_t z_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_z_axis_gyro_failure(&device_alloc,
			&z_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_z_axis_gyro_failure successful transfer flag true.
 */
void test_adis_read_diag_z_axis_gyro_failure_2(void)
{
	uint32_t z_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_z_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_z_axis_gyro_failure(&device_alloc,
			&z_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, z_axis_gyro_failure);
}

/**
 * @brief Test adis_read_diag_z_axis_gyro_failure successful transfer flag false.
 */
void test_adis_read_diag_z_axis_gyro_failure_3(void)
{
	uint32_t z_axis_gyro_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_z_axis_gyro_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_z_axis_gyro_failure(&device_alloc,
			&z_axis_gyro_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, z_axis_gyro_failure);
}

/**
 * @brief Test adis_read_diag_x_axis_accl_failure unsuccessful transfer.
 */
void test_adis_read_diag_x_axis_accl_failure_1(void)
{
	uint32_t x_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_x_axis_accl_failure(&device_alloc,
			&x_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_x_axis_accl_failure successful transfer flag true.
 */
void test_adis_read_diag_x_axis_accl_failure_2(void)
{
	uint32_t x_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_x_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_x_axis_accl_failure(&device_alloc,
			&x_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, x_axis_accl_failure);
}

/**
 * @brief Test adis_read_diag_x_axis_accl_failure successful transfer flag false.
 */
void test_adis_read_diag_x_axis_accl_failure_3(void)
{
	uint32_t x_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_x_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_x_axis_accl_failure(&device_alloc,
			&x_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, x_axis_accl_failure);
}

/**
 * @brief Test adis_read_diag_y_axis_accl_failure unsuccessful transfer.
 */
void test_adis_read_diag_y_axis_accl_failure_1(void)
{
	uint32_t y_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_y_axis_accl_failure(&device_alloc,
			&y_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_y_axis_accl_failure successful transfer flag true.
 */
void test_adis_read_diag_y_axis_accl_failure_2(void)
{
	uint32_t y_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_y_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_y_axis_accl_failure(&device_alloc,
			&y_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, y_axis_accl_failure);
}

/**
 * @brief Test adis_read_diag_y_axis_accl_failure successful transfer flag false.
 */
void test_adis_read_diag_y_axis_accl_failure_3(void)
{
	uint32_t y_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_y_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_y_axis_accl_failure(&device_alloc,
			&y_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, y_axis_accl_failure);
}


/**
 * @brief Test adis_read_diag_z_axis_accl_failure unsuccessful transfer.
 */
void test_adis_read_diag_z_axis_accl_failure_1(void)
{
	uint32_t z_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_z_axis_accl_failure(&device_alloc,
			&z_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_z_axis_accl_failure successful transfer flag true.
 */
void test_adis_read_diag_z_axis_accl_failure_2(void)
{
	uint32_t z_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	/* Test successful transfer with flag set */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_z_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_z_axis_accl_failure(&device_alloc,
			&z_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, z_axis_accl_failure);
}

/**
 * @brief Test adis_read_diag_z_axis_accl_failure successful transfer flag false.
 */
void test_adis_read_diag_z_axis_accl_failure_3(void)
{
	uint32_t z_axis_accl_failure;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_z_axis_accl_failure_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_z_axis_accl_failure(&device_alloc,
			&z_axis_accl_failure);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, z_axis_accl_failure);
}

/**
 * @brief Test adis_read_diag_aduc_mcu_fault unsuccessful transfer.
 */
void test_adis_read_diag_aduc_mcu_fault_1(void)
{
	uint32_t aduc_mcu_fault;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_diag_aduc_mcu_fault(&device_alloc, &aduc_mcu_fault);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_diag_aduc_mcu_fault successful transfer flag true.
 */
void test_adis_read_diag_aduc_mcu_fault_2(void)
{
	uint32_t aduc_mcu_fault;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		device_alloc.info->field_map->diag_aduc_mcu_fault_mask);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_read_diag_aduc_mcu_fault(&device_alloc, &aduc_mcu_fault);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, aduc_mcu_fault);
}

/**
 * @brief Test adis_read_diag_aduc_mcu_fault successful transfer flag false.
 */
void test_adis_read_diag_aduc_mcu_fault_3(void)
{
	uint32_t aduc_mcu_fault;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(
		!device_alloc.info->field_map->diag_aduc_mcu_fault_mask);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_diag_aduc_mcu_fault(&device_alloc, &aduc_mcu_fault);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, aduc_mcu_fault);
}

/**
 * @brief Test adis_read_diag_checksum_err flag true.
 */
void test_adis_read_diag_checksum_err_1(void)
{
	uint32_t checksum_err;
	device_alloc.info = adis_chip_info;
	device_alloc.diag_flags.checksum_err = true;

	adis_read_diag_checksum_err(&device_alloc, &checksum_err);
	TEST_ASSERT_EQUAL_INT(true, checksum_err);
}

/**
 * @brief Test adis_read_diag_checksum_err flag false.
 */
void test_adis_read_diag_checksum_err_2(void)
{
	uint32_t checksum_err;
	device_alloc.info = adis_chip_info;
	device_alloc.diag_flags.checksum_err = false;

	adis_read_diag_checksum_err(&device_alloc, &checksum_err);
	TEST_ASSERT_EQUAL_INT(false, checksum_err);
}

/**
 * @brief Test adis_read_diag_fls_mem_wr_cnt_exceed flag true.
 */
void test_adis_read_diag_fls_mem_wr_cnt_exceed_1(void)
{
	uint32_t fls_mem_wr_cnt_exceed;
	device_alloc.info = adis_chip_info;
	device_alloc.diag_flags.fls_mem_wr_cnt_exceed = true;

	adis_read_diag_fls_mem_wr_cnt_exceed(&device_alloc, &fls_mem_wr_cnt_exceed);
	TEST_ASSERT_EQUAL_INT(true, fls_mem_wr_cnt_exceed);
}

/**
 * @brief Test adis_read_diag_fls_mem_wr_cnt_exceed flag false.
 */
void test_adis_read_diag_fls_mem_wr_cnt_exceed_2(void)
{
	uint32_t fls_mem_wr_cnt_exceed;
	device_alloc.info = adis_chip_info;
	device_alloc.diag_flags.fls_mem_wr_cnt_exceed = false;

	adis_read_diag_fls_mem_wr_cnt_exceed(&device_alloc, &fls_mem_wr_cnt_exceed);
	TEST_ASSERT_EQUAL_INT(false, fls_mem_wr_cnt_exceed);
}

/**
 * @brief Test adis_read_x_gyro successful SPI transfer FIFO enabled.
 */
void test_adis_read_x_gyro_1(void)
{
	int32_t x_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_x_gyro(&device_alloc, &x_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_gyro unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_x_gyro_2(void)
{
	int32_t x_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_x_gyro(&device_alloc, &x_gyro);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_gyro successful SPI transfer FIFO disabled.
 */
void test_adis_read_x_gyro_3(void)
{
	int32_t x_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_x_gyro(&device_alloc, &x_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_y_gyro successful SPI transfer FIFO enabled.
 */
void test_adis_read_y_gyro_1(void)
{
	int32_t y_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_y_gyro(&device_alloc, &y_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_gyro unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_y_gyro_2(void)
{
	int32_t y_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_y_gyro(&device_alloc, &y_gyro);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_gyro successful SPI transfer FIFO disabled.
 */
void test_adis_read_y_gyro_3(void)
{
	int32_t y_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_y_gyro(&device_alloc, &y_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_z_gyro successful SPI transfer FIFO enabled.
 */
void test_adis_read_z_gyro_1(void)
{
	int32_t z_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_z_gyro(&device_alloc, &z_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_gyro unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_z_gyro_2(void)
{
	int32_t z_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_z_gyro(&device_alloc, &z_gyro);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_gyro successful SPI transfer FIFO disabled.
 */
void test_adis_read_z_gyro_3(void)
{
	int32_t z_gyro;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_z_gyro(&device_alloc, &z_gyro);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_x_accl successful SPI transfer FIFO enabled.
 */
void test_adis_read_x_accl_1(void)
{
	int32_t x_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_x_accl(&device_alloc, &x_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_accl unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_x_accl_2(void)
{
	int32_t x_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_x_accl(&device_alloc, &x_accl);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_accl successful SPI transfer FIFO disabled.
 */
void test_adis_read_x_accl_3(void)
{
	int32_t x_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_x_accl(&device_alloc, &x_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_y_accl successful SPI transfer FIFO enabled.
 */
void test_adis_read_y_accl_1(void)
{
	int32_t y_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_y_accl(&device_alloc, &y_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_accl unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_y_accl_2(void)
{
	int32_t y_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_y_accl(&device_alloc, &y_accl);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_accl successful SPI transfer FIFO disabled.
 */
void test_adis_read_y_accl_3(void)
{
	int32_t y_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_y_accl(&device_alloc, &y_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_z_accl successful SPI transfer FIFO enabled.
 */
void test_adis_read_z_accl_1(void)
{
	int32_t z_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_z_accl(&device_alloc, &z_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_accl unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_z_accl_2(void)
{
	int32_t z_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_z_accl(&device_alloc, &z_accl);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_accl successful SPI transfer FIFO disabled.
 */
void test_adis_read_z_accl_3(void)
{
	int32_t z_accl;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_z_accl(&device_alloc, &z_accl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_temp_out successful SPI transfer FIFO enabled.
 */
void test_adis_read_temp_out_1(void)
{
	int32_t temp_out;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_temp_out(&device_alloc, &temp_out);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_temp_out unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_temp_out_2(void)
{
	int32_t temp_out;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_temp_out(&device_alloc, &temp_out);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_temp_out successful SPI transfer FIFO disabled.
 */
void test_adis_read_temp_out_3(void)
{
	int32_t temp_out;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_temp_out(&device_alloc, &temp_out);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_time_stamp on 2 bytes successful SPI transfer.
 */
void test_adis_read_time_stamp_size16(void)
{
	uint32_t time_stamp;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_time_stamp(&device_alloc, &time_stamp);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_time_stamp on 4 bytes successful SPI transfer.
 */
void test_adis_read_time_stamp_size32(void)
{
	uint32_t time_stamp;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_time_stamp(&device_alloc, &time_stamp);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_data_cntr successful SPI transfer.
 */
void test_adis_read_data_cntr(void)
{
	uint32_t data_ctnr;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_data_cntr(&device_alloc, &data_ctnr);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_x_deltang successful SPI transfer FIFO enabled.
 */
void test_adis_read_x_deltang_1(void)
{
	int32_t x_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_x_deltang(&device_alloc, &x_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_deltang unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_x_deltang_2(void)
{
	int32_t x_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_x_deltang(&device_alloc, &x_deltang);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_deltang successful SPI transfer FIFO disabled.
 */
void test_adis_read_x_deltang_3(void)
{
	int32_t x_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_x_deltang(&device_alloc, &x_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_y_deltang successful SPI transfer FIFO enabled.
 */
void test_adis_read_y_deltang_1(void)
{
	int32_t y_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_y_deltang(&device_alloc, &y_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_deltang unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_y_deltang_2(void)
{
	int32_t y_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_y_deltang(&device_alloc, &y_deltang);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_deltang successful SPI transfer FIFO disabled.
 */
void test_adis_read_y_deltang_3(void)
{
	int32_t y_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_y_deltang(&device_alloc, &y_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_z_deltang successful SPI transfer FIFO enabled.
 */
void test_adis_read_z_deltang_1(void)
{
	int32_t z_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_z_deltang(&device_alloc, &z_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_deltang unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_z_deltang_2(void)
{
	int32_t z_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_z_deltang(&device_alloc, &z_deltang);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_deltang successful SPI transfer FIFO disabled.
 */
void test_adis_read_z_deltang_3(void)
{
	int32_t z_deltang;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_z_deltang(&device_alloc, &z_deltang);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_x_deltvel successful SPI transfer FIFO enabled.
 */
void test_adis_read_x_deltvel_1(void)
{
	int32_t x_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_x_deltvel(&device_alloc, &x_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_deltvel unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_x_deltvel_2(void)
{
	int32_t x_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_x_deltvel(&device_alloc, &x_deltvel);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_x_deltvel successful SPI transfer FIFO disabled.
 */
void test_adis_read_x_deltvel_3(void)
{
	int32_t x_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_x_deltvel(&device_alloc, &x_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_y_deltvel successful SPI transfer FIFO enabled.
 */
void test_adis_read_y_deltvel_1(void)
{
	int32_t y_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_y_deltvel(&device_alloc, &y_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_deltvel unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_y_deltvel_2(void)
{
	int32_t y_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_y_deltvel(&device_alloc, &y_deltvel);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_y_deltvel successful SPI transfer FIFO disabled.
 */
void test_adis_read_y_deltvel_3(void)
{
	int32_t y_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_y_deltvel(&device_alloc, &y_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_z_deltvel successful SPI transfer FIFO enabled.
 */
void test_adis_read_z_deltvel_1(void)
{
	int32_t z_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	retval = adis_read_z_deltvel(&device_alloc, &z_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_deltvel unsuccessful SPI transfer FIFO enabled.
 */
void test_adis_read_z_deltvel_2(void)
{
	int32_t z_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = true;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_z_deltvel(&device_alloc, &z_deltvel);
	TEST_ASSERT_EQUAL_INT(-1, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_read_z_deltvel successful SPI transfer FIFO disabled.
 */
void test_adis_read_z_deltvel_3(void)
{
	int32_t z_deltvel;
	device_alloc.info = adis_chip_info;
	device_alloc.fifo_enabled = false;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_z_deltvel(&device_alloc, &z_deltvel);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fifo_cnt successful SPI transfer.
 */
void test_adis_read_fifo_cnt(void)
{
	uint32_t fifo_cnt;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_cnt(&device_alloc, &fifo_cnt);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_spi_chksum successful SPI transfer.
 */
void test_adis_read_spi_chksum(void)
{
	uint32_t checksum;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_spi_chksum(&device_alloc, &checksum);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_xg_bias successful SPI transfer.
 */
void test_adis_read_xg_bias(void)
{
	int32_t xg_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_xg_bias(&device_alloc, &xg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_xg_bias successful SPI transfer.
 */
void test_adis_write_xg_bias(void)
{
	int32_t xg_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_xg_bias(&device_alloc, xg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_yg_bias successful SPI transfer.
 */
void test_adis_read_yg_bias(void)
{
	int32_t yg_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_yg_bias(&device_alloc, &yg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_yg_bias successful SPI transfer.
 */
void test_adis_write_yg_bias(void)
{
	int32_t yg_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_yg_bias(&device_alloc, yg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_zg_bias successful SPI transfer.
 */
void test_adis_read_zg_bias(void)
{
	int32_t zg_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_zg_bias(&device_alloc, &zg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_zg_bias successful SPI transfer.
 */
void test_adis_write_zg_bias(void)
{
	int32_t zg_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_zg_bias(&device_alloc, zg_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_xa_bias successful SPI transfer.
 */
void test_adis_read_xa_bias(void)
{
	int32_t xa_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_xa_bias(&device_alloc, &xa_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_xa_bias successful SPI transfer.
 */
void test_adis_write_xa_bias(void)
{
	int32_t xa_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_xa_bias(&device_alloc, xa_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_ya_bias successful SPI transfer.
 */
void test_adis_read_ya_bias(void)
{
	int32_t ya_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_ya_bias(&device_alloc, &ya_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_ya_bias successful SPI transfer.
 */
void test_adis_write_ya_bias(void)
{
	int32_t ya_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_ya_bias(&device_alloc, ya_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_za_bias successful SPI transfer.
 */
void test_adis_read_za_bias(void)
{
	int32_t za_bias;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	no_os_sign_extend32_IgnoreAndReturn(0);
	no_os_find_last_set_bit_IgnoreAndReturn(0);
	retval = adis_read_za_bias(&device_alloc, &za_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_za_bias successful SPI transfer.
 */
void test_adis_write_za_bias(void)
{
	int32_t za_bias = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(100);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_za_bias(&device_alloc, za_bias);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fifo_en successful SPI transfer.
 */
void test_adis_read_fifo_en(void)
{
	uint32_t fifo_en;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_en(&device_alloc, &fifo_en);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_fifo_en successful SPI transfer with fifo enabled.
 */
void test_adis_write_fifo_en_1(void)
{
	uint32_t fifo_en = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_en(&device_alloc, fifo_en);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_write_fifo_en successful SPI transfer with fifo disabled.
 */
void test_adis_write_fifo_en_2(void)
{
	uint32_t fifo_en = 0;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_en(&device_alloc, fifo_en);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(false, device_alloc.fifo_enabled);
}

/**
 * @brief Test adis_write_fifo_en successful SPI transfer with invalid value.
 */
void test_adis_write_fifo_en_3(void)
{
	uint32_t fifo_en = 2;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_write_fifo_en(&device_alloc, fifo_en);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_fifo_overflow successful SPI transfer.
 */
void test_adis_read_fifo_overflow(void)
{
	uint32_t fifo_overflow;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_overflow(&device_alloc, &fifo_overflow);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_fifo_overflow successful SPI transfer.
 */
void test_adis_write_fifo_overflow(void)
{
	uint32_t fifo_overflow = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_overflow(&device_alloc, fifo_overflow);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fifo_wm_int_en successful SPI transfer.
 */
void test_adis_read_fifo_wm_int_en(void)
{
	uint32_t fifo_wm_int_en;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_wm_int_en(&device_alloc, &fifo_wm_int_en);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_fifo_wm_int_en successful SPI transfer.
 */
void test_adis_write_fifo_wm_int_en(void)
{
	uint32_t fifo_wm_int_en = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_wm_int_en(&device_alloc, fifo_wm_int_en);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fifo_wm_int_pol successful SPI transfer.
 */
void test_adis_read_fifo_wm_int_pol(void)
{
	uint32_t fifo_wm_int_pol;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_wm_int_pol(&device_alloc, &fifo_wm_int_pol);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_fifo_wm_int_pol successful SPI transfer.
 */
void test_adis_write_fifo_wm_int_pol(void)
{
	uint32_t fifo_wm_int_pol = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_wm_int_pol(&device_alloc, fifo_wm_int_pol);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fifo_wm_lvl successful SPI transfer.
 */
void test_adis_read_fifo_wm_lvl(void)
{
	uint32_t fifo_wm_lvl;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_fifo_wm_lvl(&device_alloc, &fifo_wm_lvl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_fifo_wm_lvl successful SPI transfer.
 */
void test_adis_write_fifo_wm_lvl(void)
{
	uint32_t fifo_wm_lvl = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(512);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_fifo_wm_lvl(&device_alloc, fifo_wm_lvl);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_filt_size_var_b successful SPI transfer.
 */
void test_adis_read_filt_size_var_b(void)
{
	uint32_t filt_size_var_b;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_filt_size_var_b(&device_alloc, &filt_size_var_b);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_filt_size_var_b successful SPI transfer.
 */
void test_adis_write_filt_size_var_b_1(void)
{
	uint32_t filt_size_var_b = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(adis_chip_info->filt_size_var_b_max);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_filt_size_var_b(&device_alloc, filt_size_var_b);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_filt_size_var_b invalid value.
 */
void test_adis_write_filt_size_var_b_2(void)
{
	uint32_t filt_size_var_b = adis_chip_info->filt_size_var_b_max + 1;
	device_alloc.info = adis_chip_info;

	retval = adis_write_filt_size_var_b(&device_alloc, filt_size_var_b);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_filt_size_var_b invalid write.
 */
void test_adis_write_filt_size_var_b_3(void)
{
	uint32_t filt_size_var_b = adis_chip_info->filt_size_var_b_max;
	device_alloc.info = adis_chip_info;

	filt_size_var_b = adis_chip_info->filt_size_var_b_max;
	no_os_field_get_IgnoreAndReturn(adis_chip_info->filt_size_var_b_max - 1);
	retval = adis_write_filt_size_var_b(&device_alloc, filt_size_var_b);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_gyro_meas_range successful SPI transfer.
 */
void test_adis_read_gyro_meas_range(void)
{
	uint32_t gyro_meas_range;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_gyro_meas_range(&device_alloc, &gyro_meas_range);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_dr_polarity successful SPI transfer.
 */
void test_adis_read_dr_polarity(void)
{
	uint32_t dr_polarity;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_dr_polarity(&device_alloc, &dr_polarity);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_dr_polarity successful SPI transfer.
 */
void test_adis_write_dr_polarity_1(void)
{
	uint32_t dr_polarity = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_dr_polarity(&device_alloc, dr_polarity);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_dr_polarity invalid value.
 */
void test_adis_write_dr_polarity_2(void)
{
	uint32_t dr_polarity = 2;
	device_alloc.info = adis_chip_info;

	retval = adis_write_dr_polarity(&device_alloc, dr_polarity);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_dr_polarity invalid write.
 */
void test_adis_write_dr_polarity_3(void)
{
	uint32_t dr_polarity = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_dr_polarity(&device_alloc, dr_polarity);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_sync_polarity successful SPI transfer.
 */
void test_adis_read_sync_polarity(void)
{
	uint32_t sync_polarity;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_sync_polarity(&device_alloc, &sync_polarity);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_polarity successful SPI transfer.
 */
void test_adis_write_sync_polarity_1(void)
{
	uint32_t sync_polarity = 1;
	device_alloc.info = adis_chip_info;
	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_sync_polarity(&device_alloc, sync_polarity);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_polarity invalid value.
 */
void test_adis_write_sync_polarity_2(void)
{
	uint32_t sync_polarity = 2;
	device_alloc.info = adis_chip_info;

	retval = adis_write_sync_polarity(&device_alloc, sync_polarity);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_sync_polarity invalid write.
 */
void test_adis_write_sync_polarity_3(void)
{
	uint32_t sync_polarity = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_sync_polarity(&device_alloc, sync_polarity);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_sync_mode successful SPI transfer.
 */
void test_adis_read_sync_mode(void)
{
	uint32_t sync_mode;
	device_alloc.info = adis_chip_info;
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_sync_mode(&device_alloc, &sync_mode);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_mode invalid value for sync mode.
 */
void test_adis_write_sync_mode_1(void)
{
	uint32_t sync_mode;
	uint32_t ext_clk = 0;
	device_alloc.info = adis_chip_info;

	sync_mode = adis_chip_info->sync_mode_max + 1;
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_DIRECT with invalid external
 * clock lower value.
 */
void test_adis_write_sync_mode_2(void)
{
	uint32_t sync_mode = ADIS_SYNC_DIRECT;
	uint32_t ext_clk =
		device_alloc.info->sync_clk_freq_limits[ADIS_SYNC_DIRECT].min_freq - 1;
	device_alloc.info = adis_chip_info;

	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_DIRECT with invalid external
 * clock upper value.
 */
void test_adis_write_sync_mode_3(void)
{
	uint32_t sync_mode = ADIS_SYNC_DIRECT;
	uint32_t ext_clk =
		device_alloc.info->sync_clk_freq_limits[ADIS_SYNC_DIRECT].max_freq + 1;
	device_alloc.info = adis_chip_info;

	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_DIRECT with valid external
 * clock.
 */
void test_adis_write_sync_mode_4(void)
{
	uint32_t sync_mode = ADIS_SYNC_DIRECT;
	uint32_t ext_clk =
		device_alloc.info->sync_clk_freq_limits[ADIS_SYNC_DIRECT].max_freq;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(device_alloc.ext_clk, ext_clk);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_SCALED with invalid external
 * clock lower value.
 */
void test_adis_write_sync_mode_5(void)
{
	uint32_t sync_mode = ADIS_SYNC_SCALED;
	uint32_t ext_clk =
		device_alloc.info->sync_clk_freq_limits[ADIS_SYNC_SCALED].min_freq - 1;
	device_alloc.info = adis_chip_info;

	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_SCALED with valid external
 * clock.
 */
void test_adis_write_sync_mode_6(void)
{
	uint32_t sync_mode = ADIS_SYNC_SCALED;
	uint32_t ext_clk =
		device_alloc.info->sync_clk_freq_limits[ADIS_SYNC_SCALED].min_freq;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_prep_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(0xFFFF); // up_scale maximum
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(device_alloc.ext_clk, ext_clk);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_SCALED with invalid transfer
 * for up_scale.
 */
void test_adis_write_sync_mode_7(void)
{
	uint32_t sync_mode = ADIS_SYNC_SCALED;
	uint32_t ext_clk = 10;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(0xFFFF); // up_scale maximum
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(device_alloc.ext_clk, ext_clk);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_DEFAULT successful transfer.
 */
void test_adis_write_sync_mode_8(void)
{
	uint32_t sync_mode = ADIS_SYNC_DEFAULT;
	uint32_t ext_clk = 10;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_field_prep_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_mode with ADIS_SYNC_OUTPUT successful transfer.
 */
void test_adis_write_sync_mode_9(void)
{
	uint32_t sync_mode = ADIS_SYNC_OUTPUT;
	uint32_t ext_clk = 10;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	no_os_field_prep_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	retval = adis_write_sync_mode(&device_alloc, sync_mode, ext_clk);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_sens_bw successful SPI transfer.
 */
void test_adis_read_sens_bw(void)
{
	uint32_t sens_bw;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_sens_bw(&device_alloc, &sens_bw);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sens_bw successful SPI transfer.
 */
void test_adis_write_sens_bw_1(void)
{
	uint32_t sens_bw = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	retval = adis_write_sens_bw(&device_alloc, sens_bw);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sens_bw invalid write.
 */
void test_adis_write_sens_bw_2(void)
{
	uint32_t sens_bw = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_sens_bw(&device_alloc, sens_bw);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_pt_of_perc_algnmt successful SPI transfer.
 */
void test_adis_read_pt_of_perc_algnmt(void)
{
	uint32_t pt_of_perc_algnmt;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_pt_of_perc_algnmt(&device_alloc, &pt_of_perc_algnmt);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_pt_of_perc_algnmt successful SPI transfer.
 */
void test_adis_write_pt_of_perc_algnmt_1(void)
{
	uint32_t pt_of_perc_algnmt = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_pt_of_perc_algnmt(&device_alloc, pt_of_perc_algnmt);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_pt_of_perc_algnmt invalid write.
 */
void test_adis_write_pt_of_perc_algnmt_2(void)
{
	uint32_t pt_of_perc_algnmt = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_pt_of_perc_algnmt(&device_alloc, pt_of_perc_algnmt);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_linear_accl_comp successful SPI transfer.
 */
void test_adis_read_linear_accl_comp(void)
{
	uint32_t linear_accl_comp;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_linear_accl_comp(&device_alloc, &linear_accl_comp);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_linear_accl_comp successful SPI transfer.
 */
void test_adis_write_linear_accl_comp_1(void)
{
	uint32_t linear_accl_comp = 1;
	device_alloc.info = adis_chip_info;
	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_linear_accl_comp(&device_alloc, linear_accl_comp);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_linear_accl_comp invalid write.
 */
void test_adis_write_linear_accl_comp_2(void)
{
	uint32_t linear_accl_comp = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_linear_accl_comp(&device_alloc, linear_accl_comp);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_burst_sel successful SPI transfer.
 */
void test_adis_read_burst_sel_1(void)
{
	uint32_t burst_sel;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_burst_sel(&device_alloc, &burst_sel);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_burst_sel unsuccessful SPI transfer.
 */
void test_adis_read_burst_sel_2(void)
{
	uint32_t burst_sel;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_burst_sel(&device_alloc, &burst_sel);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_write_burst_sel successful SPI transfer.
 */
void test_adis_write_burst_sel_1(void)
{
	uint32_t burst_sel = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_burst_sel(&device_alloc, burst_sel);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_burst_sel invalid write.
 */
void test_adis_write_burst_sel_2(void)
{
	uint32_t burst_sel = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_burst_sel(&device_alloc, burst_sel);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_burst32 successful SPI transfer.
 */
void test_adis_read_burst32_1(void)
{
	uint32_t burst32;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_burst32(&device_alloc, &burst32);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_burst32 unsuccessful SPI transfer.
 */
void test_adis_read_burst32_2(void)
{
	uint32_t burst32;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_burst32(&device_alloc, &burst32);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_write_burst32 successful SPI transfer.
 */
void test_adis_write_burst32_1(void)
{
	uint32_t burst32 = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_burst32(&device_alloc, burst32);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_burst32 invalid write.
 */
void test_adis_write_burst32_2(void)
{
	uint32_t burst32 = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_burst32(&device_alloc, burst32);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_timestamp32 successful SPI transfer.
 */
void test_adis_read_timestamp32(void)
{
	uint32_t timestamp32;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_timestamp32(&device_alloc, &timestamp32);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_timestamp32 successful SPI transfer.
 */
void test_adis_write_timestamp32_1(void)
{
	uint32_t timestamp32 = 1;
	device_alloc.info = adis_chip_info;
	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_timestamp32(&device_alloc, timestamp32);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_timestamp32 invalid write.
 */
void test_adis_write_timestamp32_2(void)
{
	uint32_t timestamp32 = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_timestamp32(&device_alloc, timestamp32);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_sync_4khz successful SPI transfer.
 */
void test_adis_read_sync_4khz(void)
{
	uint32_t sync_4khz;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_sync_4khz(&device_alloc, &sync_4khz);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_sync_4khz successful SPI transfer with value 1.
 */
void test_adis_write_sync_4khz_1(void)
{
	uint32_t sync_4khz = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_sync_4khz(&device_alloc, sync_4khz);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(device_alloc.int_clk, 4000);
}

/**
 * @brief Test adis_write_sync_4khz successful SPI transfer with value 0.
 */
void test_adis_write_sync_4khz_2(void)
{
	uint32_t sync_4khz = 0;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_sync_4khz(&device_alloc, sync_4khz);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(device_alloc.int_clk, 2000);
}

/**
 * @brief Test adis_write_sync_4khz invalid write.
 */
void test_adis_write_sync_4khz_3(void)
{
	uint32_t sync_4khz = 1;
	device_alloc.info = adis_chip_info;

	sync_4khz = 1;
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_write_sync_4khz(&device_alloc, sync_4khz);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_up_scale successful SPI transfer.
 */
void test_adis_read_up_scale(void)
{
	uint32_t up_scale;
	device_alloc.info = adis_chip_info;
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_up_scale(&device_alloc, &up_scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_up_scale unsuccessful reading of sync mode.
 */
void test_adis_write_up_scale_1(void)
{
	uint32_t up_scale = 1;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_write_up_scale with invalid up_scale parameter lower limit.
 */
void test_adis_write_up_scale_2(void)
{
	uint32_t up_scale = 0;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_up_scale with invalid up_scale parameter upper limit.
 */
void test_adis_write_up_scale_3(void)
{
	uint32_t up_scale = 2000;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_SCALED);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_SCALED);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_up_scale for sync mode != ADIS_SYNC_SCALED.
 */
void test_adis_write_up_scale_4(void)
{
	uint32_t up_scale = 0;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_up_scale(&device_alloc, up_scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_dec_rate successful SPI transfer.
 */
void test_adis_read_dec_rate(void)
{
	uint32_t dec_rate;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_dec_rate(&device_alloc, &dec_rate);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_dec_rate with invalid value.
 */
void test_adis_write_dec_rate_1(void)
{
	uint32_t dec_rate = adis_chip_info->dec_rate_max + 1;
	device_alloc.info = adis_chip_info;

	retval = adis_write_dec_rate(&device_alloc, dec_rate);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_dec_rate with valid value and unsuccessful SPI
 * transfer.
 */
void test_adis_write_dec_rate_2(void)
{
	uint32_t dec_rate = 10;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	no_os_field_get_IgnoreAndReturn(dec_rate);
	retval = adis_write_dec_rate(&device_alloc, dec_rate);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_write_dec_rate with valid value and successful SPI transfer.
 */
void test_adis_write_dec_rate_3(void)
{
	uint32_t dec_rate = 10;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(dec_rate);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	no_os_udelay_Ignore();
	retval = adis_write_dec_rate(&device_alloc, dec_rate);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_tbc with successful SPI transfer.
 */
void test_adis_read_bias_corr_tbc(void)
{
	uint32_t bias_corr_tbc;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_tbc(&device_alloc, &bias_corr_tbc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_tbc with invalid value.
 */
void test_adis_write_bias_corr_tbc_1(void)
{
	uint32_t bias_corr_tbc = adis_chip_info->bias_corr_tbc_max + 1;
	device_alloc.info = adis_chip_info;

	bias_corr_tbc = adis_chip_info->bias_corr_tbc_max + 1;
	retval = adis_write_bias_corr_tbc(&device_alloc, bias_corr_tbc);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_write_bias_corr_tbc with valid value.
 */
void test_adis_write_bias_corr_tbc_2(void)
{
	uint32_t bias_corr_tbc = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_tbc);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_tbc(&device_alloc, bias_corr_tbc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_xg with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_xg(void)
{
	uint32_t bias_corr_en_xg;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_xg(&device_alloc, &bias_corr_en_xg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_xg with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_xg(void)
{
	uint32_t bias_corr_en_xg = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_xg);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_xg(&device_alloc, bias_corr_en_xg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_yg with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_yg(void)
{
	uint32_t bias_corr_en_yg;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_yg(&device_alloc, &bias_corr_en_yg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_yg with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_yg(void)
{
	uint32_t bias_corr_en_yg = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_yg);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_yg(&device_alloc, bias_corr_en_yg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_zg with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_zg(void)
{
	uint32_t bias_corr_en_zg;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_zg(&device_alloc, &bias_corr_en_zg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_zg with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_zg(void)
{
	uint32_t bias_corr_en_zg = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_zg);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_zg(&device_alloc, bias_corr_en_zg);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_xa with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_xa(void)
{
	uint32_t bias_corr_en_xa;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_xa(&device_alloc, &bias_corr_en_xa);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_xa with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_xa(void)
{
	uint32_t bias_corr_en_xa = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_xa);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_xa(&device_alloc, bias_corr_en_xa);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_ya with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_ya(void)
{
	uint32_t bias_corr_en_ya;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_ya(&device_alloc, &bias_corr_en_ya);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_ya with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_ya(void)
{
	uint32_t bias_corr_en_ya = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_ya);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_ya(&device_alloc, bias_corr_en_ya);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_bias_corr_en_za with successful SPI transfer.
 */
void test_adis_read_bias_corr_en_za(void)
{
	uint32_t bias_corr_en_za;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_bias_corr_en_za(&device_alloc, &bias_corr_en_za);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_bias_corr_en_za with successful SPI transfer.
 */
void test_adis_write_bias_corr_en_za(void)
{
	uint32_t bias_corr_en_za = 1;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(bias_corr_en_za);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_bias_corr_en_za(&device_alloc, bias_corr_en_za);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_bias_corr_update with successful SPI transfer.
 */
void test_adis_cmd_bias_corr_update(void)
{
	device_alloc.info = adis_chip_info;
	no_os_spi_transfer_IgnoreAndReturn(0);
	retval = adis_cmd_bias_corr_update(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_fact_calib_restore with unsuccessful SPI transfer.
 */
void test_adis_cmd_fact_calib_restore_1(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_cmd_fact_calib_restore(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_cmd_fact_calib_restore with successful SPI transfer.
 */
void test_adis_cmd_fact_calib_restore_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	retval = adis_cmd_fact_calib_restore(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_snsr_self_test with unsuccessful SPI transfer.
 */
void test_adis_cmd_snsr_self_test_1(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_cmd_snsr_self_test(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_cmd_snsr_self_test with successful SPI transfer.
 */
void test_adis_cmd_snsr_self_test_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	retval = adis_cmd_snsr_self_test(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_fls_mem_update with unsuccessful SPI transfer.
 */
void test_adis_cmd_fls_mem_update_1(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_cmd_fls_mem_update(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_cmd_fls_mem_update with successful SPI transfer.
 */
void test_adis_cmd_fls_mem_update_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	no_os_get_unaligned_be32_IgnoreAndReturn(1);
	no_os_field_get_IgnoreAndReturn(1);
	retval = adis_cmd_fls_mem_update(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_fls_mem_test with unsuccessful SPI transfer.
 */
void test_adis_cmd_fls_mem_test_1(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_cmd_fls_mem_test(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_cmd_fls_mem_test with successful SPI transfer.
 */
void test_adis_cmd_fls_mem_test_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	retval = adis_cmd_fls_mem_test(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_fifo_flush with successful SPI transfer.
 */
void test_adis_cmd_fifo_flush(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	retval = adis_cmd_fifo_flush(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_cmd_sw_res with unsuccessful SPI transfer.
 */
void test_adis_cmd_sw_res_1(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_cmd_sw_res(&device_alloc);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_cmd_sw_res with successful SPI transfer.
 */
void test_adis_cmd_sw_res_2(void)
{
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_mdelay_Ignore();
	retval = adis_cmd_sw_res(&device_alloc);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_firm_rev with successful SPI read.
 */
void test_adis_read_firm_rev(void)
{
	uint32_t firm_rev;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_firm_rev(&device_alloc, &firm_rev);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_firm_d with successful SPI read.
 */
void test_adis_read_firm_d(void)
{
	uint32_t firm_d;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_firm_d(&device_alloc, &firm_d);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_firm_m with successful SPI read.
 */
void test_adis_read_firm_m(void)
{
	uint32_t firm_m;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_firm_m(&device_alloc, &firm_m);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_firm_y with successful SPI read.
 */
void test_adis_read_firm_y(void)
{
	uint32_t firm_y;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_firm_y(&device_alloc, &firm_y);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_prod_id with successful SPI read.
 */
void test_adis_read_prod_id(void)
{
	uint32_t prod_id;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_prod_id(&device_alloc, &prod_id);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_serial_num with successful SPI read.
 */
void test_adis_read_serial_num(void)
{
	uint32_t serial_num;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_serial_num(&device_alloc, &serial_num);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_usr_scr_1 with successful SPI read.
 */
void test_adis_read_usr_scr_1(void)
{
	uint32_t usr_scr_1;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_usr_scr_1(&device_alloc, &usr_scr_1);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_usr_scr_1 with successful SPI write.
 */
void test_adis_write_usr_scr_1(void)
{
	uint32_t usr_scr_1 = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(usr_scr_1);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_usr_scr_1(&device_alloc, usr_scr_1);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_usr_scr_2 with successful SPI read.
 */
void test_adis_read_usr_scr_2(void)
{
	uint32_t usr_scr_2;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_usr_scr_2(&device_alloc, &usr_scr_2);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_usr_scr_2 with successful SPI write.
 */
void test_adis_write_usr_scr_2(void)
{
	uint32_t usr_scr_2 = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(usr_scr_2);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_usr_scr_2(&device_alloc, usr_scr_2);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_usr_scr_3 with successful SPI read.
 */
void test_adis_read_usr_scr_3(void)
{
	uint32_t usr_scr_3;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);
	retval = adis_read_usr_scr_3(&device_alloc, &usr_scr_3);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_write_usr_scr_3 with successful SPI write.
 */
void test_adis_write_usr_scr_3(void)
{
	uint32_t usr_scr_3 = 100;
	device_alloc.info = adis_chip_info;

	no_os_field_get_IgnoreAndReturn(usr_scr_3);
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	no_os_field_prep_IgnoreAndReturn(0);
	retval = adis_write_usr_scr_3(&device_alloc, usr_scr_3);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_read_fls_mem_wr_cntr with unsuccessful SPI read.
 */
void test_adis_read_fls_mem_wr_cntr_1(void)
{
	uint32_t fls_mem_wr_cntr;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_fls_mem_wr_cntr(&device_alloc, &fls_mem_wr_cntr);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_fls_mem_wr_cntr with successful SPI read and with
 * write counter higher than exceeded value.
 */
void test_adis_read_fls_mem_wr_cntr_2(void)
{
	uint32_t fls_mem_wr_cntr;
	device_alloc.info = adis_chip_info;

	device_alloc.diag_flags.fls_mem_wr_cnt_exceed = false;
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(adis_chip_info->fls_mem_wr_cntr_max+1);
	retval = adis_read_fls_mem_wr_cntr(&device_alloc, &fls_mem_wr_cntr);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.diag_flags.fls_mem_wr_cnt_exceed);
}

/**
 * @brief Test adis_read_fls_mem_wr_cntr with successful SPI read and with
 * write counter lower than exceeded value.
 */
void test_adis_read_fls_mem_wr_cntr_3(void)
{
	uint32_t fls_mem_wr_cntr;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be32_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(adis_chip_info->fls_mem_wr_cntr_max);
	retval = adis_read_fls_mem_wr_cntr(&device_alloc, &fls_mem_wr_cntr);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.diag_flags.fls_mem_wr_cnt_exceed);
}

/**
 * @brief Test adis_read_burst_data with unsuccessful SPI burst read with
 * burst32 = 0 and burst_sel = 0 .
 */
void test_adis_read_burst_data_1(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;

	no_os_spi_write_and_read_IgnoreAndReturn(-1);
	retval = adis_read_burst_data(&device_alloc, &data,
				      device_alloc.burst32, device_alloc.burst_sel, false, true);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_burst_data with burst32 update request and invalid spi
 * transfer.
 */
void test_adis_read_burst_data_2(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;
	no_os_field_get_IgnoreAndReturn(!device_alloc.burst32);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_burst_data(&device_alloc, &data,
				      !device_alloc.burst32, device_alloc.burst_sel, false, true);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_burst_data with burst_sel update request and invalid spi
 * transfer.
 */
void test_adis_read_burst_data_3(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;
	no_os_field_get_IgnoreAndReturn(!device_alloc.burst_sel);
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_read_burst_data(&device_alloc, &data,
				      device_alloc.burst32, !device_alloc.burst_sel, false, true);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_read_burst_data with burst request requested with fifo
 */
void test_adis_read_burst_data_4(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;

	no_os_spi_write_and_read_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	retval = adis_read_burst_data(&device_alloc, &data,
				      device_alloc.burst32, device_alloc.burst_sel, true, true);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_read_burst_data with checksum error.
 */
void test_adis_read_burst_data_5(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;

	device_alloc.info = adis_chip_info;
	no_os_spi_write_and_read_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	retval = adis_read_burst_data(&device_alloc, &data,
				      device_alloc.burst32, device_alloc.burst_sel, false, true);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	TEST_ASSERT_EQUAL_INT(true, device_alloc.diag_flags.checksum_err);
}

/**
 * @brief Test adis_read_burst_data with burst request requested without fifo
 */
void test_adis_read_burst_data_6(void)
{
	device_alloc.info = adis_chip_info;
	struct adis_burst_data data;

	device_alloc.burst32 = 0;
	device_alloc.burst_sel = 0;

	no_os_spi_write_and_read_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(0);
	retval = adis_read_burst_data(&device_alloc, &data,
				      device_alloc.burst32, device_alloc.burst_sel, true, true);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_update_ext_clk_freq with unsuccessful SPI read for
 * sync mode.
 */
void test_adis_update_ext_clk_freq_1(void)
{
	uint32_t clk_freq = 100;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to ADIS_SYNC_DEFAULT.
 */
void test_adis_update_ext_clk_freq_2(void)
{
	uint32_t clk_freq = 100;
	device_alloc.info = adis_chip_info;

	/* Test sync mode ADIS_SYNC_DEFAULT */
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to ADIS_SYNC_OUTPUT.
 */
void test_adis_update_ext_clk_freq_3(void)
{
	uint32_t clk_freq = 100;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to ADIS_SYNC_DIRECT
 * with invalid clk_freq for lower limit.
 */
void test_adis_update_ext_clk_freq_4(void)
{
	uint32_t clk_freq = 0;
	uint32_t clk_freq_before = device_alloc.ext_clk;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq_before, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to ADIS_SYNC_DIRECT
 * with invalid clk_freq for upper limit.
 */
void test_adis_update_ext_clk_freq_5(void)
{
	uint32_t clk_freq = 10000;
	uint32_t clk_freq_before = device_alloc.ext_clk;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq_before, device_alloc.ext_clk);
}

/**
 * @brief Test adis_update_ext_clk_freq with sync mode set to ADIS_SYNC_DIRECT
 * with valid clk_freq.
 */
void test_adis_update_ext_clk_freq_6(void)
{
	uint32_t clk_freq = 2000;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	retval = adis_update_ext_clk_freq(&device_alloc, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
	TEST_ASSERT_EQUAL_INT(clk_freq, device_alloc.ext_clk);
}

/**
 * @brief Test adis_get_sync_clk_freq with unsuccessful SPI read for
 * sync mode.
 */
void test_adis_get_sync_clk_freq_1(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;

	/* Test invalid sync mode spi reading */
	no_os_spi_transfer_IgnoreAndReturn(-1);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(-1, retval);
}

/**
 * @brief Test adis_get_sync_clk_freq with sync mode set to ADIS_SYNC_DEFAULT.
 */
void test_adis_get_sync_clk_freq_2(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DEFAULT);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(2000, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_sync_clk_freq with sync mode set to ADIS_SYNC_OUTPUT.
 */
void test_adis_get_sync_clk_freq_3(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;

	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_OUTPUT);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(2000, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_sync_clk_freq with sync mode set to ADIS_SYNC_DIRECT.
 */
void test_adis_get_sync_clk_freq_4(void)
{
	uint32_t clk_freq;
	device_alloc.info = adis_chip_info;

	device_alloc.ext_clk = 2100;
	no_os_spi_transfer_IgnoreAndReturn(0);
	no_os_get_unaligned_be16_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	no_os_field_get_IgnoreAndReturn(ADIS_SYNC_DIRECT);
	retval = adis_get_sync_clk_freq(&device_alloc, &clk_freq);
	TEST_ASSERT_EQUAL_INT(2100, clk_freq);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_anglvel_scale with null device.
 */
void test_adis_get_anglvel_scale_1(void)
{
	struct adis_scale_fractional scale;
	retval = adis_get_anglvel_scale(NULL, &scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_anglvel_scale with null device and null scale.
 */
void test_adis_get_anglvel_scale_2(void)
{
	retval = adis_get_anglvel_scale(NULL, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_anglvel_scale with null scale.
 */
void test_adis_get_anglvel_scale_3(void)
{
	device_alloc.info = adis_chip_info;
	retval = adis_get_anglvel_scale(&device_alloc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_anglvel_scale with valid data.
 */
void test_adis_get_anglvel_scale_4(void)
{
	struct adis_scale_fractional scale;
	device_alloc.info = adis_chip_info;
	device_alloc.dev_id = adis_dev_id;
	retval = adis_get_anglvel_scale(&device_alloc, &scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_accl_scale with null device.
 */
void test_adis_get_accl_scale_1(void)
{
	struct adis_scale_fractional scale;
	retval = adis_get_accl_scale(NULL, &scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_accl_scale with null device and null scale.
 */
void test_adis_get_accl_scale_2(void)
{
	retval = adis_get_accl_scale(NULL, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_accl_scale with null scale.
 */
void test_adis_get_accl_scale_3(void)
{
	device_alloc.info = adis_chip_info;
	retval = adis_get_accl_scale(&device_alloc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_accl_scale with valid data.
 */
void test_adis_get_accl_scale_4(void)
{
	struct adis_scale_fractional scale;
	device_alloc.info = adis_chip_info;
	device_alloc.dev_id = adis_dev_id;
	retval = adis_get_accl_scale(&device_alloc, &scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_deltaangl_scale with null device.
 */
void test_adis_get_deltaangl_scale_1(void)
{
	struct adis_scale_fractional_log2 scale;
	retval = adis_get_deltaangl_scale(NULL, &scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltaangl_scale with null device and null scale.
 */
void test_adis_get_deltaangl_scale_2(void)
{
	retval = adis_get_deltaangl_scale(NULL, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltaangl_scale with null scale.
 */
void test_adis_get_deltaangl_scale_3(void)
{
	device_alloc.info = adis_chip_info;
	retval = adis_get_deltaangl_scale(&device_alloc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltaangl_scale with valid data.
 */
void test_adis_get_deltaangl_scale_4(void)
{
	struct adis_scale_fractional_log2 scale;
	device_alloc.info = adis_chip_info;
	device_alloc.dev_id = adis_dev_id;
	retval = adis_get_deltaangl_scale(&device_alloc, &scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_deltavelocity_scale with null device.
 */
void test_adis_get_deltavelocity_scale_1(void)
{
	struct adis_scale_fractional_log2 scale;
	retval = adis_get_deltavelocity_scale(NULL, &scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltavelocity_scale with null device and null scale.
 */
void test_adis_get_deltavelocity_scale_2(void)
{
	retval = adis_get_deltavelocity_scale(NULL, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltavelocity_scale with null scale.
 */
void test_adis_get_deltavelocity_scale_3(void)
{
	device_alloc.info = adis_chip_info;
	retval = adis_get_deltavelocity_scale(&device_alloc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_deltavelocity_scale with valid data.
 */
void test_adis_get_deltavelocity_scale_4(void)
{
	struct adis_scale_fractional_log2 scale;
	device_alloc.info = adis_chip_info;
	device_alloc.dev_id = adis_dev_id;
	retval = adis_get_deltavelocity_scale(&device_alloc, &scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}

/**
 * @brief Test adis_get_temp_scale with null device.
 */
void test_adis_get_temp_scale_1(void)
{
	struct adis_scale_fractional scale;
	retval = adis_get_temp_scale(NULL, &scale);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_temp_scale with null device and null scale.
 */
void test_adis_get_temp_scale_2(void)
{
	retval = adis_get_temp_scale(NULL, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_temp_scale with null scale.
 */
void test_adis_get_temp_scale_3(void)
{
	device_alloc.info = adis_chip_info;
	retval = adis_get_temp_scale(&device_alloc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, retval);
}

/**
 * @brief Test adis_get_temp_scale with valid data.
 */
void test_adis_get_temp_scale_4(void)
{
	struct adis_scale_fractional scale;
	device_alloc.info = adis_chip_info;
	device_alloc.dev_id = adis_dev_id;
	retval = adis_get_temp_scale(&device_alloc, &scale);
	TEST_ASSERT_EQUAL_INT(0, retval);
}
