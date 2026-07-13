/***************************************************************************//**
 *   @file   test_max30009.c
 *   @brief  Unit tests for max30009 LED driver
 *   @author Edelweise Escala (edelweise.escala@analog.com)
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
#include "unity.h"
#include "max30009.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_spi.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_util.h"
#include "mock_no_os_alloc.h"
#include "mock_no_os_delay.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* ── helpers ─────────────────────────────────────────────────────────────── */

static struct max30009_dev *g_dev;

/* Real calloc/free stubs so the driver can allocate memory */
static void *stub_calloc(size_t nmemb, size_t size)
{
	return calloc(nmemb, size);
}

static void stub_free(void *ptr)
{
	free(ptr);
}

/*
 * SPI write-and-read stub.
 * The driver uses: buf[0]=addr, buf[1]=R/W flag, buf[2+]=data.
 * For a read the stub puts a fixed byte (0x42 or 0x00) in buf[2].
 */
static uint8_t g_spi_read_val;
static uint8_t g_last_i2c_reg; /* tracks register for smart I2C read stubs */

/*
 * SPI stub: for reads, returns g_spi_read_val for most registers.
 * For register 0xFF (PART_ID), always returns MAX30009_PART_ID_VALUE.
 * For STATUS1 register (0x00), returns bits indicating PLL lock.
 */
static int32_t stub_spi_write_and_read(struct no_os_spi_desc *desc,
				       uint8_t *buf, uint16_t len)
{
	(void)desc;
	if (len >= 3 && buf[1] == 0x80) {
		uint8_t reg = buf[0];
		if (reg == MAX30009_REG_PART_ID)
			buf[2] = MAX30009_PART_ID_VALUE;
		else if (reg == MAX30009_REG_STATUS1)
			buf[2] = MAX30009_STATUS1_FREQ_LOCK_MSK |
				 MAX30009_STATUS1_PHASE_LOCK_MSK;
		else
			buf[2] = g_spi_read_val;
	}
	return 0;
}

/* SPI stub that always returns 0x00 regardless of register (wrong part ID) */
static int32_t stub_spi_always_zero(struct no_os_spi_desc *desc,
				    uint8_t *buf, uint16_t len)
{
	(void)desc;
	if (len >= 3 && buf[1] == 0x80)
		buf[2] = 0x00;
	return 0;
}

/* Smart I2C write stub: captures register address for the subsequent read */
static int32_t stub_i2c_write_capture(struct no_os_i2c_desc *desc,
				      uint8_t *data, uint8_t bytes_number,
				      uint8_t stop_bit)
{
	(void)desc;
	(void)bytes_number;
	(void)stop_bit;
	if (bytes_number >= 1)
		g_last_i2c_reg = data[0];
	return 0;
}

/* Smart I2C read stub: returns PART_ID for 0xFF, lock bits for 0x00 */
static int32_t stub_i2c_read(struct no_os_i2c_desc *desc,
			     uint8_t *data, uint8_t bytes,
			     uint8_t stop)
{
	(void)desc;
	(void)bytes;
	(void)stop;
	if (g_last_i2c_reg == MAX30009_REG_PART_ID)
		data[0] = MAX30009_PART_ID_VALUE;
	else if (g_last_i2c_reg == MAX30009_REG_STATUS1)
		data[0] = MAX30009_STATUS1_FREQ_LOCK_MSK |
			  MAX30009_STATUS1_PHASE_LOCK_MSK;
	else
		data[0] = g_spi_read_val;
	return 0;
}

/* SPI init stub: fills *desc with a non-NULL pointer so reg_access routes SPI */
static struct no_os_spi_desc g_fake_spi_desc;
static int32_t stub_spi_init(struct no_os_spi_desc **desc,
			     const struct no_os_spi_init_param *param)
{
	(void)param;
	*desc = &g_fake_spi_desc;
	return 0;
}

static struct no_os_i2c_desc g_fake_i2c_desc;
static int32_t stub_i2c_init(struct no_os_i2c_desc **desc,
			     const struct no_os_i2c_init_param *param)
{
	(void)param;
	*desc = &g_fake_i2c_desc;
	return 0;
}

/* Build a minimal SPI-mode device on the heap (bypasses init) */
static struct max30009_dev *make_spi_dev(void)
{
	struct max30009_dev *dev = calloc(1, sizeof(*dev));
	if (!dev)
		return NULL;
	/* give it a fake (non-NULL) spi_desc so reg_access routes to SPI */
	dev->spi_desc = (struct no_os_spi_desc *)(uintptr_t)0xDEAD;
	dev->ref_clk_freq = 32768;
	return dev;
}

/* Build a minimal I2C-mode device on the heap */
static struct max30009_dev *make_i2c_dev(void)
{
	struct max30009_dev *dev = calloc(1, sizeof(*dev));
	if (!dev)
		return NULL;
	dev->i2c_desc = (struct no_os_i2c_desc *)(uintptr_t)0xBEEF;
	dev->ref_clk_freq = 32768;
	return dev;
}

/* ── setUp / tearDown ────────────────────────────────────────────────────── */

void setUp(void)
{
	g_dev = NULL;
	g_spi_read_val = 0x00;

	/* Default: all platform calls succeed */
	no_os_calloc_Stub(stub_calloc);
	no_os_free_Stub(stub_free);

	no_os_spi_init_IgnoreAndReturn(0);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
	no_os_spi_remove_IgnoreAndReturn(0);

	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);
	no_os_i2c_remove_IgnoreAndReturn(0);

	no_os_gpio_get_IgnoreAndReturn(0);
	no_os_gpio_direction_output_IgnoreAndReturn(0);
	no_os_gpio_direction_input_IgnoreAndReturn(0);
	no_os_gpio_remove_IgnoreAndReturn(0);

	no_os_field_prep_IgnoreAndReturn(0);
	no_os_field_get_IgnoreAndReturn(0);

	no_os_mdelay_Ignore();
}

void tearDown(void)
{
	if (g_dev) {
		free(g_dev);
		g_dev = NULL;
	}
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_init
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_init_null_device_returns_einval(void)
{
	struct max30009_init_param p = { 0 };
	int ret = max30009_init(NULL, &p);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_init_null_param_returns_einval(void)
{
	struct max30009_dev *dev = NULL;
	int ret = max30009_init(&dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_init_spi_success(void)
{
	struct max30009_dev *dev = NULL;
	struct max30009_init_param p = { 0 };

	p.use_i2c = false;

	/* SPI init stub fills dev->spi_desc; SPI read stub is register-aware */
	no_os_spi_init_Stub(stub_spi_init);
	no_os_spi_write_and_read_Stub(stub_spi_write_and_read);

	int ret = max30009_init(&dev, &p);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);

	max30009_remove(dev);

	/* restore */
	no_os_spi_init_IgnoreAndReturn(0);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_init_i2c_success(void)
{
	struct max30009_dev *dev = NULL;
	struct max30009_init_param p = { 0 };

	p.use_i2c = true;

	no_os_i2c_init_Stub(stub_i2c_init);
	no_os_i2c_write_Stub(stub_i2c_write_capture);
	no_os_i2c_read_Stub(stub_i2c_read);

	int ret = max30009_init(&dev, &p);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_NOT_NULL(dev);

	max30009_remove(dev);

	no_os_i2c_init_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);
}

void test_max30009_init_spi_fail_propagates(void)
{
	struct max30009_dev *dev = NULL;
	struct max30009_init_param p = { 0 };
	p.use_i2c = false;

	no_os_spi_init_IgnoreAndReturn(-EIO);

	int ret = max30009_init(&dev, &p);
	TEST_ASSERT_NOT_EQUAL(0, ret);
	TEST_ASSERT_NULL(dev);

	/* restore */
	no_os_spi_init_IgnoreAndReturn(0);
}

void test_max30009_init_part_id_mismatch(void)
{
	struct max30009_dev *dev = NULL;
	struct max30009_init_param p = { 0 };
	p.use_i2c = false;

	no_os_spi_init_Stub(stub_spi_init);
	/* stub_spi_always_zero makes part-ID read return 0x00 → mismatch */
	no_os_spi_write_and_read_Stub(stub_spi_always_zero);

	int ret = max30009_init(&dev, &p);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
	TEST_ASSERT_NULL(dev);

	no_os_spi_init_IgnoreAndReturn(0);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_init_gpio_fail(void)
{
	struct max30009_dev *dev = NULL;
	struct max30009_init_param p = { 0 };
	struct no_os_gpio_init_param int_gpio = { 0 };
	p.use_i2c = false;
	p.int_gpio_init = &int_gpio;

	/* SPI init fills dev->spi_desc */
	no_os_spi_init_Stub(stub_spi_init);

	/* gpio_get for int_gpio fails */
	no_os_gpio_get_IgnoreAndReturn(-EIO);

	int ret = max30009_init(&dev, &p);
	TEST_ASSERT_NOT_EQUAL(0, ret);
	TEST_ASSERT_NULL(dev);

	/* restore */
	no_os_spi_init_IgnoreAndReturn(0);
	no_os_gpio_get_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_remove
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_remove_null_device(void)
{
	int ret = max30009_remove(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_remove_spi_success(void)
{
	g_dev = make_spi_dev();
	/* bioz_enable writes to reg → SPI → ignore */
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_remove(g_dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	g_dev = NULL; /* already freed by remove */

	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_remove_i2c_success(void)
{
	g_dev = make_i2c_dev();
	no_os_i2c_write_IgnoreAndReturn(0);
	no_os_i2c_read_IgnoreAndReturn(0);

	int ret = max30009_remove(g_dev);
	TEST_ASSERT_EQUAL_INT(0, ret);
	g_dev = NULL;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_reg_read / write / update
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_reg_read_spi_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t val = 0;
	/* Use reg 0x20 (BIOZ_CONFIG1) - not overridden by smart stub */
	g_spi_read_val = 0xAB;
	no_os_spi_write_and_read_Stub(stub_spi_write_and_read);

	int ret = max30009_reg_read(dev, 0x20, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0xAB, val);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_reg_read_i2c_success(void)
{
	struct max30009_dev *dev = make_i2c_dev();
	uint8_t val = 0;
	g_spi_read_val = 0x55;
	no_os_i2c_read_StubWithCallback(stub_i2c_read);

	int ret = max30009_reg_read(dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_HEX8(0x55, val);

	free(dev);
	no_os_i2c_read_IgnoreAndReturn(0);
}

void test_max30009_reg_read_no_interface_einval(void)
{
	struct max30009_dev dev = { 0 }; /* no spi_desc, no i2c_desc */
	uint8_t val = 0;
	int ret = max30009_reg_read(&dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_reg_write_spi_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_reg_write(dev, 0x11, 0x42);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_reg_write_i2c_success(void)
{
	struct max30009_dev *dev = make_i2c_dev();

	int ret = max30009_reg_write(dev, 0x11, 0x01);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
}

void test_max30009_reg_write_i2c_calloc_fail(void)
{
	struct max30009_dev *dev = make_i2c_dev();

	/* calloc returns NULL on write path */
	no_os_calloc_IgnoreAndReturn(NULL);

	int ret = max30009_reg_write(dev, 0x11, 0x01);
	TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);

	free(dev);
	no_os_calloc_Stub(stub_calloc);
}

void test_max30009_reg_update_spi_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	g_spi_read_val = 0x00;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_reg_update(dev, 0x20, 0x04, 1);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_reg_update_read_fail(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_IgnoreAndReturn(-EIO);

	int ret = max30009_reg_update(dev, 0x20, 0x04, 1);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_reg_read_multiple_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t buf[3] = { 0 };
	g_spi_read_val = 0xCC;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_reg_read_multiple(dev, 0x0C, buf, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_reg_write_multiple_success(void)
{
	struct max30009_dev *dev = make_i2c_dev();
	uint8_t data[2] = { 0x11, 0x22 };

	int ret = max30009_reg_write_multiple(dev, 0x20, data, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_verify_part_id
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_verify_part_id_null(void)
{
	int ret = max30009_verify_part_id(NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_verify_part_id_correct(void)
{
	struct max30009_dev *dev = make_spi_dev();
	g_spi_read_val = MAX30009_PART_ID_VALUE;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_verify_part_id(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_verify_part_id_wrong(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_Stub(stub_spi_always_zero);

	int ret = max30009_verify_part_id(dev);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_soft_reset
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_soft_reset_null(void)
{
	int ret = max30009_soft_reset(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_soft_reset_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	g_spi_read_val = MAX30009_STATUS1_FREQ_LOCK_MSK |
			 MAX30009_STATUS1_PHASE_LOCK_MSK;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_soft_reset(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_set_power_mode
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_set_power_mode_null(void)
{
	int ret = max30009_set_power_mode(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_power_mode_shutdown(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_power_mode(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_power_mode_active(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_power_mode(dev, false);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_get_status
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_get_status_null_device(void)
{
	struct max30009_status st = { 0 };
	int ret = max30009_get_status(NULL, &st);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_status_null_status(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_get_status(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_get_status_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_status st = { 0 };

	/* smart stub returns FREQ_LOCK|PHASE_LOCK (0x0A) for STATUS1 (reg 0x00),
	 * and g_spi_read_val for STATUS2 (reg 0x01) */
	g_spi_read_val = 0x00; /* STATUS2 has no flags set */
	no_os_spi_write_and_read_Stub(stub_spi_write_and_read);

	int ret = max30009_get_status(dev, &st);
	TEST_ASSERT_EQUAL_INT(0, ret);
	/* Smart stub returns FREQ_LOCK + PHASE_LOCK bits for STATUS1 */
	TEST_ASSERT_TRUE(st.freq_lock);
	TEST_ASSERT_TRUE(st.phase_lock);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_status_read_fail(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_status st = { 0 };

	no_os_spi_write_and_read_IgnoreAndReturn(-EIO);

	int ret = max30009_get_status(dev, &st);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_check_lead_off
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_check_lead_off_null_device(void)
{
	bool leads_on = false;
	int ret = max30009_check_lead_off(NULL, &leads_on);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_check_lead_off_null_ptr(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_check_lead_off(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_check_lead_off_leads_on(void)
{
	struct max30009_dev *dev = make_spi_dev();
	bool leads_on = false;
	g_spi_read_val = MAX30009_STATUS2_LON_MSK;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_check_lead_off(dev, &leads_on);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(leads_on);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_check_lead_off_leads_off(void)
{
	struct max30009_dev *dev = make_spi_dev();
	bool leads_on = true;
	g_spi_read_val = 0x00;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_check_lead_off(dev, &leads_on);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_FALSE(leads_on);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * FIFO operations
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_fifo_flush_null(void)
{
	int ret = max30009_fifo_flush(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_fifo_flush_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_fifo_flush(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_fifo_get_count_null_device(void)
{
	uint16_t cnt = 0;
	int ret = max30009_fifo_get_count(NULL, &cnt);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_fifo_get_count_null_count(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_fifo_get_count(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_fifo_get_count_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint16_t cnt = 0;
	/* cnt1 has MSB bit set (bit7=1) → count bit9 = 1, cnt2 = 0x05 */
	g_spi_read_val = 0x80; /* cnt1 */
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_fifo_get_count(dev, &cnt);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_fifo_get_ovf_count_null_device(void)
{
	uint16_t cnt = 0;
	int ret = max30009_fifo_get_ovf_count(NULL, &cnt);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_fifo_get_ovf_count_null_count(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_fifo_get_ovf_count(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_fifo_get_ovf_count_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint16_t cnt = 0;
	g_spi_read_val = 0x3F; /* 6 overflow samples */
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_fifo_get_ovf_count(dev, &cnt);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_read_fifo_data_null_device(void)
{
	uint8_t buf[3] = { 0 };
	int ret = max30009_read_fifo_data(NULL, buf, 3);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_read_fifo_data_null_buf(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_read_fifo_data(dev, NULL, 3);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_read_fifo_data_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t buf[3] = { 0 };
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_read_fifo_data(dev, buf, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * max30009_fifo_decode_sample
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_fifo_decode_sample_null_raw(void)
{
	struct max30009_fifo_sample s = { 0 };
	int ret = max30009_fifo_decode_sample(NULL, &s);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_fifo_decode_sample_null_sample(void)
{
	uint8_t raw[3] = { 0x10, 0x00, 0x00 };
	int ret = max30009_fifo_decode_sample(raw, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_fifo_decode_in_phase(void)
{
	/* TAG=0x1, DATA=0x00001 (positive 1) */
	uint8_t raw[3] = { 0x10, 0x00, 0x01 };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_IN_PHASE, s.type);
	TEST_ASSERT_EQUAL_INT(1, s.data);
}

void test_max30009_fifo_decode_quadrature(void)
{
	/* TAG=0x2, DATA=0x00002 */
	uint8_t raw[3] = { 0x20, 0x00, 0x02 };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_QUADRATURE, s.type);
}

void test_max30009_fifo_decode_invalid_all_ones(void)
{
	uint8_t raw[3] = { 0xFF, 0xFF, 0xFF };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_INVALID, s.type);
}

void test_max30009_fifo_decode_marker(void)
{
	/* 0xFFFFFE */
	uint8_t raw[3] = { 0xFF, 0xFF, 0xFE };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_MARKER, s.type);
}

void test_max30009_fifo_decode_negative_data(void)
{
	/* TAG=0x1, DATA=0x80000 (sign bit set → negative) */
	uint8_t raw[3] = { 0x18, 0x00, 0x00 };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_IN_PHASE, s.type);
	TEST_ASSERT_TRUE(s.data < 0); /* sign-extended → negative */
}

void test_max30009_fifo_decode_unknown_tag(void)
{
	/* TAG=0x3 (unknown), not 0xFFFFFF or 0xFFFFFE */
	uint8_t raw[3] = { 0x30, 0x00, 0x01 };
	struct max30009_fifo_sample s = { 0 };

	int ret = max30009_fifo_decode_sample(raw, &s);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_FIFO_SAMPLE_INVALID, s.type);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * FIFO pointer / watermark / rollover / markers
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_get_fifo_pointers_null_device(void)
{
	uint8_t w, r;
	int ret = max30009_get_fifo_pointers(NULL, &w, &r);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_fifo_pointers_null_write(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t r;
	int ret = max30009_get_fifo_pointers(dev, NULL, &r);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_get_fifo_pointers_null_read(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t w;
	int ret = max30009_get_fifo_pointers(dev, &w, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_get_fifo_pointers_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t w = 0, r = 0;
	g_spi_read_val = 0x10;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_get_fifo_pointers(dev, &w, &r);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_fifo_watermark_null(void)
{
	int ret = max30009_set_fifo_watermark(NULL, 10);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_fifo_watermark_zero(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_fifo_watermark(dev, 0);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_fifo_watermark_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_fifo_watermark(dev, 10);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_fifo_watermark_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t wm = 0;
	g_spi_read_val = MAX30009_FIFO_DEPTH - 10; /* reg = 256 - watermark */
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_get_fifo_watermark(dev, &wm);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(10, wm);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_fifo_rollover_null(void)
{
	int ret = max30009_set_fifo_rollover(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_fifo_rollover_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_fifo_rollover(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_a_full_type_null(void)
{
	int ret = max30009_set_a_full_type(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_insert_fifo_marker_null(void)
{
	int ret = max30009_insert_fifo_marker(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_insert_fifo_marker_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_insert_fifo_marker(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_clear_fifo_status_null(void)
{
	int ret = max30009_clear_fifo_status(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_clear_fifo_status_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_clear_fifo_status(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * System/PLL functions
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_timing_system_reset_null(void)
{
	int ret = max30009_timing_system_reset(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_timing_system_reset_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_timing_system_reset(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_master_mode_null(void)
{
	int ret = max30009_set_master_mode(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_master_mode_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_master_mode(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_configure_int_pin_null(void)
{
	int ret = max30009_configure_int_pin(NULL, 0, 0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_configure_int_pin_invalid_func(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_configure_int_pin(dev, 4, 0); /* func_cfg > 3 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_configure_int_pin_invalid_out(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_configure_int_pin(dev, 0, 4); /* out_cfg > 3 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_configure_int_pin_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_configure_int_pin(dev, 1, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_configure_trig_pin_null(void)
{
	int ret = max30009_configure_trig_pin(NULL, 0, 0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_configure_trig_pin_invalid_input(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_configure_trig_pin(dev, 2, 0); /* input_cfg > 1 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_configure_trig_pin_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_configure_trig_pin(dev, 1, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_configure_i2c_broadcast_null(void)
{
	int ret = max30009_configure_i2c_broadcast(NULL, true, 0x10);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_configure_i2c_broadcast_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_configure_i2c_broadcast(dev, true, 0x10);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_disable_i2c_null(void)
{
	int ret = max30009_disable_i2c(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_disable_i2c_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_disable_i2c(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * PLL dividers / enable / lock window / clock source / fine-tune
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_pll_set_dividers_null(void)
{
	int ret = max30009_pll_set_dividers(NULL, 427, MAX30009_NDIV_512,
					    MAX30009_KDIV_1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_pll_set_dividers_invalid_range(void)
{
	/* ref_clk=32768, mdiv=0 → pll_clk=32768 < 14MHz → -EINVAL */
	struct max30009_dev *dev = make_spi_dev();
	dev->ref_clk_freq = 32768;
	int ret = max30009_pll_set_dividers(dev, 0, MAX30009_NDIV_512,
					    MAX30009_KDIV_1);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_pll_set_dividers_success(void)
{
	/* ref_clk=32768 * (427+1) = 14,024,704 Hz ≈ 14MHz, in range */
	struct max30009_dev *dev = make_spi_dev();
	dev->ref_clk_freq = 32768;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_pll_set_dividers(dev, 427, MAX30009_NDIV_512,
					    MAX30009_KDIV_1);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(427, dev->pll_config.mdiv);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_pll_enable_null(void)
{
	int ret = max30009_pll_enable(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_pll_enable_disable(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_pll_enable(dev, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_FALSE(dev->pll_config.pll_enabled);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_pll_enable_enable_locks(void)
{
	struct max30009_dev *dev = make_spi_dev();
	/* status reg has FREQ_LOCK + PHASE_LOCK set → lock immediately */
	g_spi_read_val = MAX30009_STATUS1_FREQ_LOCK_MSK |
			 MAX30009_STATUS1_PHASE_LOCK_MSK;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_pll_enable(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(dev->pll_config.pll_enabled);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_pll_set_lock_window_null(void)
{
	int ret = max30009_pll_set_lock_window(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_pll_set_lock_window_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_pll_set_lock_window(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_clock_source_null(void)
{
	int ret = max30009_set_clock_source(NULL, false, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_clock_source_32768(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_clock_source(dev, false, true);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(32768, dev->ref_clk_freq);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_clock_source_32000(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_clock_source(dev, false, false);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(32000, dev->ref_clk_freq);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_clock_fine_tune_null(void)
{
	int ret = max30009_clock_fine_tune(NULL, 5);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_clock_fine_tune_out_of_range(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_clock_fine_tune(dev, 32); /* > 31 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_clock_fine_tune_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_clock_fine_tune(dev, 15);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * BioZ enable / channel / bandgap
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_bioz_enable_null(void)
{
	int ret = max30009_bioz_enable(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_bioz_enable_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_bioz_enable(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_bioz_enable_disable(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_bioz_enable(dev, false);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_bioz_i_enable_null(void)
{
	int ret = max30009_bioz_i_enable(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_bioz_i_enable_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_bioz_i_enable(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_bioz_q_enable_null(void)
{
	int ret = max30009_bioz_q_enable(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_bioz_q_enable_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_bioz_q_enable(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_bioz_bg_enable_null(void)
{
	int ret = max30009_bioz_bg_enable(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_bioz_bg_enable_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_bioz_bg_enable(dev, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * OSR / filter setters
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_set_dac_osr_null(void)
{
	int ret = max30009_set_dac_osr(NULL, MAX30009_DAC_OSR_32);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_dac_osr_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_dac_osr(dev, (enum max30009_dac_osr)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_dac_osr_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_dac_osr(dev, MAX30009_DAC_OSR_128);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_DAC_OSR_128, dev->bioz_config.dac_osr);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_adc_osr_null(void)
{
	int ret = max30009_set_adc_osr(NULL, MAX30009_ADC_OSR_64);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_adc_osr_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_adc_osr(dev, (enum max30009_adc_osr)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_adc_osr_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_adc_osr(dev, MAX30009_ADC_OSR_256);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_digital_hpf_null(void)
{
	int ret = max30009_set_digital_hpf(NULL, MAX30009_DHPF_BYPASS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_digital_hpf_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_digital_hpf(dev, (enum max30009_dhpf)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_digital_hpf_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_digital_hpf(dev, MAX30009_DHPF_0_002_SR);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_digital_lpf_null(void)
{
	int ret = max30009_set_digital_lpf(NULL, MAX30009_DLPF_BYPASS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_digital_lpf_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_digital_lpf(dev, (enum max30009_dlpf)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_digital_lpf_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_digital_lpf(dev, MAX30009_DLPF_0_25_SR);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_analog_hpf_null(void)
{
	int ret = max30009_set_analog_hpf(NULL, MAX30009_AHPF_BYPASS);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_analog_hpf_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_analog_hpf(dev, (enum max30009_ahpf)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_analog_hpf_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_analog_hpf(dev, MAX30009_AHPF_1KHZ);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Drive mode / current / resistor / lead-off / bias
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_set_drive_mode_null(void)
{
	int ret = max30009_set_drive_mode(NULL, MAX30009_DRIVE_SINE_CURRENT);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_drive_mode_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_drive_mode(dev, (enum max30009_drive_mode)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_drive_mode_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_drive_mode(dev, MAX30009_DRIVE_HBRIDGE);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_drive_current_detailed_null(void)
{
	int ret = max30009_set_drive_current_detailed(NULL, 1, 1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_drive_current_detailed_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_drive_current_detailed(dev, 4, 0); /* > 3 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_drive_current_detailed_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_drive_current_detailed(dev, 2, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_external_resistor_null(void)
{
	int ret = max30009_enable_external_resistor(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_rapid_lead_off_null(void)
{
	int ret = max30009_enable_rapid_lead_off(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_fast_start_null(void)
{
	int ret = max30009_enable_fast_start(NULL, true, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_fast_start_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_enable_fast_start(dev, true, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_ina_mode_null(void)
{
	int ret = max30009_set_ina_mode(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_disable_differential_mode_null(void)
{
	int ret = max30009_disable_differential_mode(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_amp_range_null(void)
{
	int ret = max30009_set_amp_range(NULL, 1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_amp_range_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_amp_range(dev, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_amp_range_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_amp_range(dev, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_amp_bandwidth_null(void)
{
	int ret = max30009_set_amp_bandwidth(NULL, 1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_amp_bandwidth_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_amp_bandwidth(dev, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_amp_bandwidth_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_amp_bandwidth(dev, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_external_cap_null(void)
{
	int ret = max30009_enable_external_cap(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_dc_restore_null(void)
{
	int ret = max30009_enable_dc_restore(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_drive_reset_null(void)
{
	int ret = max30009_enable_drive_reset(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_dac_reset_null(void)
{
	int ret = max30009_enable_dac_reset(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_comparison_mode_null(void)
{
	int ret = max30009_set_comparison_mode(NULL, 1);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_comparison_mode_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_comparison_mode(dev, 4);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_comparison_mode_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_comparison_mode(dev, 2);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_bioz_threshold_null(void)
{
	int ret = max30009_enable_bioz_threshold(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_bioz_thresholds_null(void)
{
	int ret = max30009_set_bioz_thresholds(NULL, 0x10, 0xF0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_bioz_thresholds_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_bioz_thresholds(dev, 0x10, 0xF0);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_demod_clk_phases_null(void)
{
	int ret = max30009_set_demod_clk_phases(NULL, true, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_demod_clk_phases_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_demod_clk_phases(dev, true, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_ina_chopping_null(void)
{
	int ret = max30009_enable_ina_chopping(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_channel_freq_select_null(void)
{
	int ret = max30009_set_channel_freq_select(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_standby_mode_null(void)
{
	int ret = max30009_enable_standby_mode(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Calibration / MUX
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_enable_calibration_null(void)
{
	int ret = max30009_enable_calibration(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_calibration_mux_null(void)
{
	int ret = max30009_enable_calibration_mux(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_calibration_connect_only_null(void)
{
	int ret = max30009_calibration_connect_only(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_bia_load_resistor_null(void)
{
	int ret = max30009_set_bia_load_resistor(NULL, MAX30009_BIA_900_OHM);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_bia_load_resistor_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_bia_load_resistor(dev, (enum max30009_bia_rsel)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_bia_load_resistor_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_bia_load_resistor(dev, MAX30009_BIA_600_OHM);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_bia_bist_null(void)
{
	int ret = max30009_enable_bia_bist(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_gsr_load_resistor_null(void)
{
	int ret = max30009_set_gsr_load_resistor(NULL, MAX30009_GSR_101K_OHM);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_gsr_load_resistor_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_gsr_load_resistor(dev, (enum max30009_gsr_rsel)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_gsr_load_resistor_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_gsr_load_resistor(dev, MAX30009_GSR_505K_OHM);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_gsr_load_null(void)
{
	int ret = max30009_enable_gsr_load(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_internal_load_null(void)
{
	int ret = max30009_enable_internal_load(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_external_load_null(void)
{
	int ret = max30009_enable_external_load(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_assign_electrodes_null(void)
{
	int ret = max30009_assign_electrodes(NULL, 0, 0, 0, 0);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_assign_electrodes_invalid_bip(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_assign_electrodes(dev, 3, 0, 0, 0); /* bip > 2 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_assign_electrodes_invalid_bin(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_assign_electrodes(dev, 0, 3, 0, 0); /* bin > 2 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_assign_electrodes_invalid_drvp(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_assign_electrodes(dev, 0, 0, 4, 0); /* drvp > 3 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_assign_electrodes_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_assign_electrodes(dev, 1, 1, 2, 3);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_read_bist_error_null_device(void)
{
	uint8_t err = 0;
	int ret = max30009_read_bist_error(NULL, &err);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_read_bist_error_null_ptr(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_read_bist_error(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_read_bist_error_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	uint8_t err = 0;
	g_spi_read_val = 0x05;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_read_bist_error(dev, &err);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Lead-off / bias
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_lead_off_config_null(void)
{
	int ret = max30009_lead_off_config(NULL, true, 5);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_lead_off_config_invalid_threshold(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_lead_off_config(dev, true, 16); /* > 15 */
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_lead_off_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_lead_off_config(dev, true, 8);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_lead_off_current_null(void)
{
	int ret = max30009_set_lead_off_current(NULL, MAX30009_LOFF_10NA);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_lead_off_current_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_lead_off_current(dev,
						(enum max30009_loff_current)99);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_lead_off_current_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_lead_off_current(dev, MAX30009_LOFF_50NA);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_lead_off_polarity_null(void)
{
	int ret = max30009_set_lead_off_polarity(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_external_lead_off_null(void)
{
	int ret = max30009_enable_external_lead_off(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_drive_oor_detect_null(void)
{
	int ret = max30009_enable_drive_oor_detect(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_ultra_low_power_lead_on_null(void)
{
	int ret = max30009_enable_ultra_low_power_lead_on(NULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_get_detailed_lead_status_null_device(void)
{
	struct max30009_lead_status st = { 0 };
	int ret = max30009_get_detailed_lead_status(NULL, &st);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_detailed_lead_status_null_status(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_get_detailed_lead_status(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_get_detailed_lead_status_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_lead_status st = { 0 };
	/* LON + BIOZ_OVER */
	g_spi_read_val = MAX30009_STATUS2_LON_MSK |
			 MAX30009_STATUS2_BIOZ_OVER_MSK;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_get_detailed_lead_status(dev, &st);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(st.leads_on);
	TEST_ASSERT_TRUE(st.bioz_over);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_set_lead_bias_null(void)
{
	int ret = max30009_set_lead_bias(NULL, MAX30009_RBIAS_50M_OHM,
					 true, false);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_set_lead_bias_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_lead_bias(dev, MAX30009_RBIAS_RESERVED,
					 true, false);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_lead_bias_valid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_lead_bias(dev, MAX30009_RBIAS_100M_OHM,
					 true, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Aggregate config set/get
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_set_pll_config_null_device(void)
{
	struct max30009_pll_config cfg = { 0 };
	int ret = max30009_set_pll_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_pll_config_null_cfg(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_pll_config(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_pll_config_invalid_fine_tune(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_pll_config cfg = { 0 };
	cfg.clk_fine_tune = 32; /* > 31 */
	int ret = max30009_set_pll_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_pll_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_pll_config cfg = { 0 };
	cfg.clk_fine_tune = 10;
	cfg.use_32768_hz = true;
	cfg.mdiv = 427;
	cfg.ndiv = MAX30009_NDIV_512;
	cfg.kdiv = MAX30009_KDIV_1;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_pll_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_pll_config_null_device(void)
{
	struct max30009_pll_config cfg = { 0 };
	int ret = max30009_get_pll_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_pll_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_pll_config cfg = { 0 };
	dev->pll_config.mdiv = 427;

	int ret = max30009_get_pll_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(427, cfg.mdiv);

	free(dev);
}

void test_max30009_set_fifo_config_null_device(void)
{
	struct max30009_fifo_config cfg = { .watermark = 10 };
	int ret = max30009_set_fifo_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_fifo_config_null_cfg(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_fifo_config(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_fifo_config_zero_watermark(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_fifo_config cfg = { 0 };
	cfg.watermark = 0;
	int ret = max30009_set_fifo_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_fifo_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_fifo_config cfg = { 0 };
	cfg.watermark = 5;
	cfg.fifo_rollover = true;
	cfg.fifo_stat_clr = true;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_fifo_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_fifo_config_null_device(void)
{
	struct max30009_fifo_config cfg = { 0 };
	int ret = max30009_get_fifo_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_fifo_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_fifo_config cfg = { 0 };
	dev->fifo_config.watermark = 8;

	int ret = max30009_get_fifo_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(8, cfg.watermark);

	free(dev);
}

void test_max30009_set_bioz_config_null_device(void)
{
	struct max30009_bioz_config cfg = { 0 };
	int ret = max30009_set_bioz_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_bioz_config_null_cfg(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_bioz_config(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_bioz_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_bioz_config cfg = { 0 };
	cfg.dac_osr = MAX30009_DAC_OSR_64;
	cfg.adc_osr = MAX30009_ADC_OSR_64;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_bioz_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_bioz_config_null_device(void)
{
	struct max30009_bioz_config cfg = { 0 };
	int ret = max30009_get_bioz_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_bioz_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_bioz_config cfg = { 0 };
	dev->bioz_config.gain = MAX30009_BIOZ_GAIN_10;

	int ret = max30009_get_bioz_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_EQUAL_INT(MAX30009_BIOZ_GAIN_10, cfg.gain);

	free(dev);
}

void test_max30009_set_bioz_mux_config_null_device(void)
{
	struct max30009_bioz_mux_config cfg = { 0 };
	int ret = max30009_set_bioz_mux_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_bioz_mux_config_null_cfg(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_bioz_mux_config(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_bioz_mux_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_bioz_mux_config cfg = { 0 };
	cfg.bmux_rsel = MAX30009_BIA_900_OHM;
	cfg.mux_en = true;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_bioz_mux_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_bioz_mux_config_null_device(void)
{
	struct max30009_bioz_mux_config cfg = { 0 };
	int ret = max30009_get_bioz_mux_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_bioz_mux_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_bioz_mux_config cfg = { 0 };
	dev->bioz_mux_config.mux_en = true;

	int ret = max30009_get_bioz_mux_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(cfg.mux_en);

	free(dev);
}

void test_max30009_set_lead_detect_config_null_device(void)
{
	struct max30009_lead_detect_config cfg = { 0 };
	int ret = max30009_set_lead_detect_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_set_lead_detect_config_null_cfg(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_set_lead_detect_config(dev, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_set_lead_detect_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_lead_detect_config cfg = { 0 };
	cfg.en_loff_det = true;
	cfg.loff_thresh = 5;
	cfg.rbias_value = MAX30009_RBIAS_50M_OHM;
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_set_lead_detect_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_get_lead_detect_config_null_device(void)
{
	struct max30009_lead_detect_config cfg = { 0 };
	int ret = max30009_get_lead_detect_config(NULL, &cfg);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
}

void test_max30009_get_lead_detect_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	struct max30009_lead_detect_config cfg = { 0 };
	dev->lead_detect_config.en_loff_det = true;

	int ret = max30009_get_lead_detect_config(dev, &cfg);
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT_TRUE(cfg.en_loff_det);

	free(dev);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Interrupts
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_interrupt_config_null(void)
{
	int ret = max30009_interrupt_config(NULL, 0xFF, 0xFF);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_interrupt_config_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_interrupt_config(dev, 0xA0, 0x80);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_interrupt_null(void)
{
	int ret = max30009_enable_interrupt(NULL, MAX30009_INT_A_FULL, true);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_enable_interrupt_invalid(void)
{
	struct max30009_dev *dev = make_spi_dev();
	int ret = max30009_enable_interrupt(dev,
					    (enum max30009_interrupt)99, true);
	TEST_ASSERT_EQUAL_INT(-EINVAL, ret);
	free(dev);
}

void test_max30009_enable_interrupt_a_full(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_enable_interrupt(dev, MAX30009_INT_A_FULL, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_interrupt_fifo_rdy(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_enable_interrupt(dev,
					    MAX30009_INT_FIFO_DATA_RDY, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_interrupt_lon(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_enable_interrupt(dev, MAX30009_INT_LON, true);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_enable_interrupt_dc_loff_nl(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_enable_interrupt(dev,
					    MAX30009_INT_DC_LOFF_NL, false);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_clear_status_null(void)
{
	int ret = max30009_clear_status(NULL);
	TEST_ASSERT_EQUAL_INT(-ENODEV, ret);
}

void test_max30009_clear_status_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	int ret = max30009_clear_status(dev);
	TEST_ASSERT_EQUAL_INT(0, ret);

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}


/* ═══════════════════════════════════════════════════════════════════════════
 * SPI calloc failure path
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_max30009_spi_reg_access_calloc_fail(void)
{
	struct max30009_dev *dev = make_spi_dev();

	no_os_calloc_IgnoreAndReturn(NULL);

	int ret = max30009_reg_write(dev, 0x11, 0x01);
	TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);

	free(dev);
	no_os_calloc_Stub(stub_calloc);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * I2C write failure propagation
 * ═══════════════════════════════════════════════════════════════════════════ */

static int32_t stub_i2c_write_fail(struct no_os_i2c_desc *desc,
				   uint8_t *data, uint8_t bytes_number,
				   uint8_t stop_bit)
{
	(void)desc;
	(void)data;
	(void)bytes_number;
	(void)stop_bit;
	return -EIO;
}

void test_max30009_i2c_write_fail_propagates(void)
{
	struct max30009_dev *dev = make_i2c_dev();
	uint8_t val = 0;
	no_os_i2c_write_Stub(stub_i2c_write_fail);

	int ret = max30009_reg_read(dev, 0x00, &val);
	TEST_ASSERT_EQUAL_INT(-EIO, ret);

	free(dev);
	no_os_i2c_write_IgnoreAndReturn(0);
}

void test_max30009_additional_interrupt_types(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	/* Exercise all remaining interrupt enum cases */
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_FREQ_UNLOCK, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_FREQ_LOCK, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_PHASE_UNLOCK, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_PHASE_LOCK, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_BIOZ_OVER, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_BIOZ_UNDR, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_DRV_OOR, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_DC_LOFF_PH, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_DC_LOFF_PL, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_interrupt(dev,
			      MAX30009_INT_DC_LOFF_NH, true));

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}

void test_max30009_simple_bool_functions_success(void)
{
	struct max30009_dev *dev = make_spi_dev();
	no_os_spi_write_and_read_StubWithCallback(stub_spi_write_and_read);

	TEST_ASSERT_EQUAL_INT(0, max30009_enable_external_resistor(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_rapid_lead_off(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_set_ina_mode(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_disable_differential_mode(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_external_cap(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_dc_restore(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_drive_reset(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_dac_reset(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_bioz_threshold(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_ina_chopping(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_set_channel_freq_select(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_standby_mode(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_calibration(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_calibration_mux(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_calibration_connect_only(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_bia_bist(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_gsr_load(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_internal_load(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_external_load(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_set_lead_off_polarity(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_external_lead_off(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_drive_oor_detect(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_enable_ultra_low_power_lead_on(dev,
			      true));
	TEST_ASSERT_EQUAL_INT(0, max30009_set_a_full_type(dev, true));
	TEST_ASSERT_EQUAL_INT(0, max30009_disable_i2c(dev));
	TEST_ASSERT_EQUAL_INT(0, max30009_set_master_mode(dev, false));

	free(dev);
	no_os_spi_write_and_read_IgnoreAndReturn(0);
}
