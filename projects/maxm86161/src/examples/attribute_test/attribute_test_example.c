/***************************************************************************//**
 *   @file   attribute_test_example.c
 *   @brief  On-target integration test for the MAXM86161 driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
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
********************************************************************************
 * Hardware-in-the-loop (HIL) integration test for the MAXM86161.
 *
 * This example mirrors the offline unit-test suite in
 * tests/maxm86161/test/test_maxm86161.c, but runs against a *real* device over
 * the live I2C bus instead of CMock stubs. Where the unit tests assert the
 * driver produced the right mock traffic, this harness performs full
 * write-then-read-back round trips: every setter is followed by its matching
 * getter, confirming the value actually reached the silicon and reads back
 * unchanged. It also re-checks the parameter-validation and FIFO-decode logic
 * that the unit tests cover, plus a short live acquisition sanity check that
 * only a real sensor can exercise.
 *
 * A tiny Unity-style assertion layer keeps the test bodies readable and prints
 * a PASS/FAIL line per check followed by a summary, so the same test-case
 * structure as the unit suite is preserved on-target. The run is
 * non-destructive: the original device state is not restored, so re-flash or
 * power-cycle before switching to a measurement example.
 *******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "common_data.h"
#include "maxm86161.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*=============================================================================
 * Minimal on-target Unity-style test framework
 *
 * Ceedling/CMock are host-only, so on the target we provide a compact
 * assertion layer that tallies checks and reports a summary. Test bodies use
 * the TEST_ASSERT_* macros exactly like the offline unit tests do.
 *===========================================================================*/

static unsigned int tests_run;
static unsigned int checks_passed;
static unsigned int checks_failed;
static bool current_test_failed;

#define TEST_FAIL(fmt, ...) do {					\
		checks_failed++;					\
		current_test_failed = true;				\
		pr_err("    ASSERT FAIL (%s:%d): " fmt "\n",		\
		       __func__, __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define TEST_PASS() do {						\
		checks_passed++;					\
	} while (0)

#define TEST_ASSERT_TRUE(cond) do {					\
		if (cond)						\
			TEST_PASS();					\
		else							\
			TEST_FAIL("expected true: %s", #cond);		\
	} while (0)

#define TEST_ASSERT_FALSE(cond) do {					\
		if (!(cond))						\
			TEST_PASS();					\
		else							\
			TEST_FAIL("expected false: %s", #cond);		\
	} while (0)

#define TEST_ASSERT_EQUAL(expected, actual) do {			\
		long _e = (long)(expected);				\
		long _a = (long)(actual);				\
		if (_e == _a)						\
			TEST_PASS();					\
		else							\
			TEST_FAIL("expected %ld, got %ld", _e, _a);	\
	} while (0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
	TEST_ASSERT_EQUAL(expected, actual)

#define TEST_ASSERT_NOT_NULL(ptr) do {					\
		if ((ptr) != NULL)					\
			TEST_PASS();					\
		else							\
			TEST_FAIL("expected non-NULL: %s", #ptr);	\
	} while (0)

/* Run a single test function, tracking per-test pass/fail state. */
#define RUN_TEST(dev, fn) run_test(dev, fn, #fn)

typedef void (*maxm86161_test_fn)(struct maxm86161_dev *dev);

static void run_test(struct maxm86161_dev *dev, maxm86161_test_fn fn,
		     const char *name)
{
	tests_run++;
	current_test_failed = false;
	fn(dev);
	if (current_test_failed)
		pr_info("[FAIL] %s\n", name);
	else
		pr_info("[PASS] %s\n", name);
}

/*=============================================================================
 * Parameter Validation Tests (no bus traffic; mirror the unit suite)
 *===========================================================================*/

static void test_reg_read_null_data(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_reg_read(dev, 0x00, NULL));
}

static void test_set_sample_rate_invalid(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_sample_rate(dev, 0x20));
}

static void test_set_integration_time_invalid(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_integration_time(dev, 4));
}

static void test_set_fifo_watermark_invalid(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_fifo_watermark(dev, 128));
}

static void test_set_led_sequence_invalid_slot(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL,
			  maxm86161_set_led_sequence(dev, 7,
					  MAXM86161_LED_SRC_IR));
}

static void test_set_led_pulse_amplitude_invalid_led(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL,
			  maxm86161_set_led_pulse_amplitude(dev, 0, 0x20));
	TEST_ASSERT_EQUAL(-EINVAL,
			  maxm86161_set_led_pulse_amplitude(dev, 4, 0x20));
}

static void test_set_burst_mode_invalid_rate(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_burst_mode(dev, true, 4));
}

static void test_memory_read_invalid_addr(struct maxm86161_dev *dev)
{
	uint8_t data;

	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_memory_read(dev, 0x180, &data));
}

static void test_hires_dac_invalid_slot(struct maxm86161_dev *dev)
{
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_hires_dac(dev, 0, true, 0x10));
	TEST_ASSERT_EQUAL(-EINVAL, maxm86161_set_hires_dac(dev, 7, true, 0x10));
}

/*=============================================================================
 * PPG Configuration Round-Trip Tests (write then read back over real I2C)
 *===========================================================================*/

static void test_sample_rate_roundtrip(struct maxm86161_dev *dev)
{
	uint8_t val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_sample_rate(dev, 0x03));
	TEST_ASSERT_EQUAL(0, maxm86161_get_sample_rate(dev, &val));
	TEST_ASSERT_EQUAL(0x03, val);
}

static void test_integration_time_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_ppg_tint val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_integration_time(dev,
			  MAXM86161_TINT_117_3_US));
	TEST_ASSERT_EQUAL(0, maxm86161_get_integration_time(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_TINT_117_3_US, val);
}

static void test_adc_range_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_adc_range val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_adc_range(dev,
			  MAXM86161_ADC_RGE_16UA));
	TEST_ASSERT_EQUAL(0, maxm86161_get_adc_range(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_ADC_RGE_16UA, val);
}

static void test_sample_averaging_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_smp_ave val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_sample_averaging(dev,
			  MAXM86161_SMP_AVE_8));
	TEST_ASSERT_EQUAL(0, maxm86161_get_sample_averaging(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_SMP_AVE_8, val);
}

static void test_alc_disable_roundtrip(struct maxm86161_dev *dev)
{
	bool val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_alc_disable(dev, true));
	TEST_ASSERT_EQUAL(0, maxm86161_get_alc_disable(dev, &val));
	TEST_ASSERT_TRUE(val);

	TEST_ASSERT_EQUAL(0, maxm86161_set_alc_disable(dev, false));
	TEST_ASSERT_EQUAL(0, maxm86161_get_alc_disable(dev, &val));
	TEST_ASSERT_FALSE(val);
}

static void test_add_offset_roundtrip(struct maxm86161_dev *dev)
{
	bool val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_add_offset(dev, true));
	TEST_ASSERT_EQUAL(0, maxm86161_get_add_offset(dev, &val));
	TEST_ASSERT_TRUE(val);

	TEST_ASSERT_EQUAL(0, maxm86161_set_add_offset(dev, false));
	TEST_ASSERT_EQUAL(0, maxm86161_get_add_offset(dev, &val));
	TEST_ASSERT_FALSE(val);
}

static void test_led_settling_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_led_settling val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_settling(dev,
			  MAXM86161_LED_SETLNG_8_US));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_settling(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_LED_SETLNG_8_US, val);
}

static void test_digital_filter_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_dig_filt_sel val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_digital_filter(dev,
			  MAXM86161_DIG_FILT_FDM));
	TEST_ASSERT_EQUAL(0, maxm86161_get_digital_filter(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_DIG_FILT_FDM, val);
}

static void test_pd_bias_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_pd_bias val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_pd_bias(dev,
			  MAXM86161_PD_BIAS_65_130PF));
	TEST_ASSERT_EQUAL(0, maxm86161_get_pd_bias(dev, &val));
	TEST_ASSERT_EQUAL(MAXM86161_PD_BIAS_65_130PF, val);
}

/*=============================================================================
 * LED Configuration Round-Trip Tests
 *===========================================================================*/

static void test_led_sequence_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_led_src val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 1,
			  MAXM86161_LED_SRC_IR));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_sequence(dev, 1, &val));
	TEST_ASSERT_EQUAL(MAXM86161_LED_SRC_IR, val);

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 5,
			  MAXM86161_LED_SRC_DIRECT_AMB));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_sequence(dev, 5, &val));
	TEST_ASSERT_EQUAL(MAXM86161_LED_SRC_DIRECT_AMB, val);
}

static void test_led_pulse_amplitude_roundtrip(struct maxm86161_dev *dev)
{
	uint8_t val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_pulse_amplitude(dev, 1, 0x7F));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_pulse_amplitude(dev, 1, &val));
	TEST_ASSERT_EQUAL(0x7F, val);
}

static void test_led_pilot_pa_roundtrip(struct maxm86161_dev *dev)
{
	uint8_t val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_pilot_pa(dev, 0x3F));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_pilot_pa(dev, &val));
	TEST_ASSERT_EQUAL(0x3F, val);
}

static void test_led_range_roundtrip(struct maxm86161_dev *dev)
{
	enum maxm86161_led_range val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_range(dev, 2,
			  MAXM86161_LED_RGE_93MA));
	TEST_ASSERT_EQUAL(0, maxm86161_get_led_range(dev, 2, &val));
	TEST_ASSERT_EQUAL(MAXM86161_LED_RGE_93MA, val);
}

static void test_active_led_count(struct maxm86161_dev *dev)
{
	uint8_t count;

	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 1,
			  MAXM86161_LED_SRC_IR));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 2,
			  MAXM86161_LED_SRC_RED));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 3,
			  MAXM86161_LED_SRC_GREEN));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 4,
			  MAXM86161_LED_SRC_NONE));
	TEST_ASSERT_EQUAL(0, maxm86161_get_active_led_count(dev, &count));
	TEST_ASSERT_EQUAL(3, count);
}

/*=============================================================================
 * FIFO Configuration Round-Trip Tests
 *===========================================================================*/

static void test_fifo_watermark_roundtrip(struct maxm86161_dev *dev)
{
	uint8_t val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_fifo_watermark(dev, 15));
	TEST_ASSERT_EQUAL(0, maxm86161_get_fifo_watermark(dev, &val));
	TEST_ASSERT_EQUAL(15, val);
}

static void test_fifo_rollover_roundtrip(struct maxm86161_dev *dev)
{
	bool val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_fifo_rollover(dev, true));
	TEST_ASSERT_EQUAL(0, maxm86161_get_fifo_rollover(dev, &val));
	TEST_ASSERT_TRUE(val);
}

static void test_fifo_a_full_type_roundtrip(struct maxm86161_dev *dev)
{
	bool val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_fifo_a_full_type(dev, true));
	TEST_ASSERT_EQUAL(0, maxm86161_get_fifo_a_full_type(dev, &val));
	TEST_ASSERT_TRUE(val);
}

static void test_fifo_flush(struct maxm86161_dev *dev)
{
	uint8_t count;

	TEST_ASSERT_EQUAL(0, maxm86161_fifo_flush(dev));
	TEST_ASSERT_EQUAL(0, maxm86161_get_fifo_count(dev, &count));
}

/*=============================================================================
 * FIFO Sample Decode Tests (pure logic; mirror the unit suite)
 *===========================================================================*/

static void test_decode_fifo_sample(struct maxm86161_dev *dev)
{
	uint8_t raw[3] = {0x0C, 0x80, 0x00};
	uint8_t tag;
	uint32_t data;

	(void)dev;
	TEST_ASSERT_EQUAL(0, maxm86161_decode_fifo_sample(raw, &tag, &data));
	TEST_ASSERT_EQUAL(0x01, tag);
	TEST_ASSERT_EQUAL(0x48000, data);
}

static void test_decode_fifo_invalid_tag(struct maxm86161_dev *dev)
{
	uint8_t raw[3] = {0xF0, 0x00, 0x00};
	uint8_t tag;
	uint32_t data;

	(void)dev;
	TEST_ASSERT_EQUAL(0, maxm86161_decode_fifo_sample(raw, &tag, &data));
	TEST_ASSERT_EQUAL(MAXM86161_TAG_INVALID, tag);
}

static void test_decode_fifo_sample_null(struct maxm86161_dev *dev)
{
	uint8_t tag;
	uint32_t data;

	(void)dev;
	TEST_ASSERT_EQUAL(-EINVAL,
			  maxm86161_decode_fifo_sample(NULL, &tag, &data));
}

/*=============================================================================
 * Advanced Feature Round-Trip Tests
 *===========================================================================*/

static void test_prox_threshold_roundtrip(struct maxm86161_dev *dev)
{
	uint8_t val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_prox_threshold(dev, 0x10));
	TEST_ASSERT_EQUAL(0, maxm86161_get_prox_threshold(dev, &val));
	TEST_ASSERT_EQUAL(0x10, val);
}

static void test_picket_fence_roundtrip(struct maxm86161_dev *dev)
{
	struct maxm86161_picket_fence_config set_cfg = {
		.enable = true,
		.order = true,
		.iir_tc = 1,
		.iir_init_value = 2,
		.threshold_sigma_mult = 1,
	};
	struct maxm86161_picket_fence_config get_cfg = {0};

	TEST_ASSERT_EQUAL(0, maxm86161_set_picket_fence(dev, &set_cfg));
	TEST_ASSERT_EQUAL(0, maxm86161_get_picket_fence(dev, &get_cfg));
	TEST_ASSERT_TRUE(get_cfg.enable);
	TEST_ASSERT_TRUE(get_cfg.order);
	TEST_ASSERT_EQUAL(set_cfg.iir_tc, get_cfg.iir_tc);
	TEST_ASSERT_EQUAL(set_cfg.iir_init_value, get_cfg.iir_init_value);
	TEST_ASSERT_EQUAL(set_cfg.threshold_sigma_mult,
			  get_cfg.threshold_sigma_mult);
}

static void test_hires_dac_roundtrip(struct maxm86161_dev *dev)
{
	bool override;
	uint8_t dac_val;

	TEST_ASSERT_EQUAL(0, maxm86161_set_hires_dac(dev, 1, true, 0x2A));
	TEST_ASSERT_EQUAL(0, maxm86161_get_hires_dac(dev, 1, &override,
			  &dac_val));
	TEST_ASSERT_TRUE(override);
	TEST_ASSERT_EQUAL(0x2A, dac_val);
}

static void test_die_temperature_read(struct maxm86161_dev *dev)
{
	int32_t temperature;

	TEST_ASSERT_EQUAL(0, maxm86161_read_die_temperature(dev,
			  &temperature));
	TEST_ASSERT_TRUE(temperature >= 0);
	TEST_ASSERT_TRUE(temperature < 60 * MAXM86161_DIE_TEMP_SCALE_UC);
	pr_info("    die temperature: %.2f C\n",
		temperature / (double)MAXM86161_DIE_TEMP_SCALE_UC);
}

static void test_interrupt_status_clear(struct maxm86161_dev *dev)
{
	uint16_t status;

	TEST_ASSERT_EQUAL(0, maxm86161_clear_interrupt_status(dev));
	TEST_ASSERT_EQUAL(0, maxm86161_get_interrupt_status(dev, &status));
}

/*=============================================================================
 * Live Acquisition Sanity Check (only a real sensor can exercise this)
 *===========================================================================*/

static void test_live_acquisition(struct maxm86161_dev *dev)
{
	uint8_t fifo_buf[MAXM86161_FIFO_DEPTH * MAXM86161_FIFO_DATA_SIZE];
	uint8_t fifo_count;
	uint8_t tag;
	uint32_t data;
	bool got_samples = false;
	int i;

	/* Minimal working PPG setup: one IR slot, mid drive current. */
	TEST_ASSERT_EQUAL(0, maxm86161_set_integration_time(dev,
			  MAXM86161_TINT_117_3_US));
	TEST_ASSERT_EQUAL(0, maxm86161_set_adc_range(dev,
			  MAXM86161_ADC_RGE_16UA));
	TEST_ASSERT_EQUAL(0, maxm86161_set_sample_averaging(dev,
			  MAXM86161_SMP_AVE_1));
	TEST_ASSERT_EQUAL(0, maxm86161_set_sample_rate(dev, 0));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_range(dev, 1,
			  MAXM86161_LED_RGE_124MA));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_pulse_amplitude(dev, 1, 0x7F));
	TEST_ASSERT_EQUAL(0, maxm86161_set_led_sequence(dev, 1,
			  MAXM86161_LED_SRC_RED));
	TEST_ASSERT_EQUAL(0, maxm86161_set_fifo_watermark(dev, 15));
	TEST_ASSERT_EQUAL(0, maxm86161_fifo_flush(dev));
	TEST_ASSERT_EQUAL(0, maxm86161_clear_interrupt_status(dev));
	TEST_ASSERT_EQUAL(0, maxm86161_set_shutdown(dev, false));

	/* Poll a few times for the FIFO to accumulate samples. */
	for (i = 0; i < 10 && !got_samples; i++) {
		no_os_mdelay(200);

		if (maxm86161_get_fifo_count(dev, &fifo_count))
			continue;
		if (fifo_count == 0)
			continue;

		if (maxm86161_read_fifo_data(dev, fifo_buf, fifo_count))
			continue;

		TEST_ASSERT_EQUAL(0, maxm86161_decode_fifo_sample(fifo_buf,
				  &tag, &data));
		got_samples = true;
		pr_info("    acquired %u samples, first tag=0x%02X data=%u\n",
			fifo_count, tag, data);
	}

	TEST_ASSERT_TRUE(got_samples);

	maxm86161_set_shutdown(dev, true);
}

/*=============================================================================
 * Test Runner
 *===========================================================================*/

int example_main(void)
{
	struct maxm86161_dev *dev;
	int ret;

	pr_info("MAXM86161 Attribute Integration Test\n");

	ret = maxm86161_init(&dev, &maxm86161_ip);
	if (ret) {
		pr_err("MAXM86161 init failed: %d\n", ret);
		return ret;
	}

	pr_info("MAXM86161 initialized. Part ID: 0x%02X Rev: 0x%02X\n",
		dev->part_id, dev->rev_id);

	pr_info("--- Parameter Validation ---\n");
	RUN_TEST(dev, test_reg_read_null_data);
	RUN_TEST(dev, test_set_sample_rate_invalid);
	RUN_TEST(dev, test_set_integration_time_invalid);
	RUN_TEST(dev, test_set_fifo_watermark_invalid);
	RUN_TEST(dev, test_set_led_sequence_invalid_slot);
	RUN_TEST(dev, test_set_led_pulse_amplitude_invalid_led);
	RUN_TEST(dev, test_set_burst_mode_invalid_rate);
	RUN_TEST(dev, test_memory_read_invalid_addr);
	RUN_TEST(dev, test_hires_dac_invalid_slot);

	pr_info("--- PPG Configuration Round-Trip ---\n");
	RUN_TEST(dev, test_sample_rate_roundtrip);
	RUN_TEST(dev, test_integration_time_roundtrip);
	RUN_TEST(dev, test_adc_range_roundtrip);
	RUN_TEST(dev, test_sample_averaging_roundtrip);
	RUN_TEST(dev, test_alc_disable_roundtrip);
	RUN_TEST(dev, test_add_offset_roundtrip);
	RUN_TEST(dev, test_led_settling_roundtrip);
	RUN_TEST(dev, test_digital_filter_roundtrip);
	RUN_TEST(dev, test_pd_bias_roundtrip);

	pr_info("--- LED Configuration Round-Trip ---\n");
	RUN_TEST(dev, test_led_sequence_roundtrip);
	RUN_TEST(dev, test_led_pulse_amplitude_roundtrip);
	RUN_TEST(dev, test_led_pilot_pa_roundtrip);
	RUN_TEST(dev, test_led_range_roundtrip);
	RUN_TEST(dev, test_active_led_count);

	pr_info("--- FIFO Configuration Round-Trip ---\n");
	RUN_TEST(dev, test_fifo_watermark_roundtrip);
	RUN_TEST(dev, test_fifo_rollover_roundtrip);
	RUN_TEST(dev, test_fifo_a_full_type_roundtrip);
	RUN_TEST(dev, test_fifo_flush);

	pr_info("--- FIFO Sample Decode ---\n");
	RUN_TEST(dev, test_decode_fifo_sample);
	RUN_TEST(dev, test_decode_fifo_invalid_tag);
	RUN_TEST(dev, test_decode_fifo_sample_null);

	pr_info("--- Advanced Features ---\n");
	RUN_TEST(dev, test_prox_threshold_roundtrip);
	RUN_TEST(dev, test_picket_fence_roundtrip);
	RUN_TEST(dev, test_hires_dac_roundtrip);
	RUN_TEST(dev, test_die_temperature_read);
	RUN_TEST(dev, test_interrupt_status_clear);

	pr_info("--- Live Acquisition ---\n");
	RUN_TEST(dev, test_live_acquisition);

	pr_info("================================================\n");
	pr_info("Tests run: %u | Checks passed: %u | failed: %u\n",
		tests_run, checks_passed, checks_failed);
	pr_info("RESULT: %s\n", checks_failed ? "FAIL" : "PASS");
	pr_info("================================================\n");

	maxm86161_remove(dev);

	return checks_failed ? -EIO : 0;
}
