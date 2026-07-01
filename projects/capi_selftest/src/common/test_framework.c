/***************************************************************************//**
 * @file test_framework.c
 * @brief Platform-agnostic test framework source for capi_selftest project.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
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

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "test_framework.h"

#define TEST_FRAMEWORK_BUFFER_SIZE	256U

static struct test_framework_config framework_config;
static struct test_framework_count framework_count;
static const char *current_module = "";
static const char *current_name = "";
static const char *last_func = "";
static int last_line;
static bool framework_initialized;
static bool run_active;
static bool timeout_reported;

/**
 * @brief Format and emit one framework line through the configured writer.
 */
static void test_framework_print(const char *format, ...)
{
	char buffer[TEST_FRAMEWORK_BUFFER_SIZE];
	va_list args;
	int length;

	if (!framework_initialized || framework_config.write == NULL)
		return;

	va_start(args, format);
	length = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if (length <= 0)
		return;

	if ((uint32_t)length >= sizeof(buffer))
		length = sizeof(buffer) - 1U;

	framework_config.write(framework_config.write_context,
			       (const uint8_t *)buffer, (uint32_t)length);
}

int test_framework_init(const struct test_framework_config *config)
{
	if (config == NULL || config->write == NULL)
		return -EINVAL;

	framework_config = *config;
	framework_initialized = true;
	test_framework_reset();

	return 0;
}

void test_framework_remove(void)
{
	memset(&framework_config, 0, sizeof(framework_config));
	memset(&framework_count, 0, sizeof(framework_count));
	current_module = "";
	current_name = "";
	last_func = "";
	last_line = 0;
	framework_initialized = false;
	run_active = false;
	timeout_reported = false;
}

void test_framework_reset(void)
{
	memset(&framework_count, 0, sizeof(framework_count));
	current_module = "";
	current_name = "";
	last_func = "";
	last_line = 0;
}

void test_framework_run_start(void)
{
	const char *run_label = framework_config.run_label;
	uint32_t timeout_ms = framework_config.timeout_ms;

	test_framework_reset();
	timeout_reported = false;
	run_active = true;

	if (run_label == NULL)
		run_label = "UNKNOWN";

	if (timeout_ms == 0U)
		timeout_ms = TEST_FRAMEWORK_DEFAULT_TIMEOUT_MS;

	test_framework_print("\r\n[RUN:START] %s\r\n", run_label);

	/* Arm the platform watchdog for the whole run. */
	if (framework_config.watchdog_arm != NULL)
		framework_config.watchdog_arm(framework_config.watchdog_context,
					      timeout_ms);
}

void test_framework_run_end(void)
{
	uint32_t total = framework_count.passed + framework_count.failed +
			 framework_count.skipped;

	/* The run finished on its own: disarm the watchdog. */
	if (framework_config.watchdog_disarm != NULL)
		framework_config.watchdog_disarm(framework_config.watchdog_context);

	run_active = false;

	test_framework_print("[RUN:SUMMARY] PASS=%lu FAIL=%lu SKIP=%lu TOTAL=%lu\r\n",
			     (unsigned long)framework_count.passed,
			     (unsigned long)framework_count.failed,
			     (unsigned long)framework_count.skipped,
			     (unsigned long)total);
	test_framework_print("[RUN:END]\r\n");
}

void test_framework_checkpoint(const char *func, int line)
{
	last_func = func != NULL ? func : "";
	last_line = line;
}

void test_framework_on_timeout(void)
{
	uint32_t total;

	/* Guard against repeated handler entry. */
	if (timeout_reported || !run_active)
		return;

	timeout_reported = true;
	run_active = false;

	total = framework_count.passed + framework_count.failed +
		framework_count.skipped;

	test_framework_print("\r\n[RUN:TIMEOUT] hang in %s.%s\r\n",
			     current_module, current_name);
	test_framework_print("[RUN:LAST_LINE] %s:%d\r\n",
			     last_func[0] != '\0' ? last_func : "unknown",
			     last_line);
	test_framework_print("[RUN:SUMMARY] PASS=%lu FAIL=%lu SKIP=%lu TOTAL=%lu\r\n",
			     (unsigned long)framework_count.passed,
			     (unsigned long)framework_count.failed,
			     (unsigned long)framework_count.skipped,
			     (unsigned long)total);
	test_framework_print("[RUN:END]\r\n");
}

void test_framework_begin(const char *module, const char *name)
{
	current_module = module != NULL ? module : "";
	current_name = name != NULL ? name : "";
}

void test_framework_set_name(const char *name)
{
	current_name = name != NULL ? name : "";
}

void test_framework_begin_uart(const char *backend, const char *name,
			       const char *func, int line)
{
	current_module = "UART";
	current_name = name != NULL ? name : "";

	/* Announce and checkpoint BEFORE the body, so a UART hang is
	 * attributable to this exact test. */
	test_framework_checkpoint(func, line);
	test_framework_print("[TEST:%s.%s] BEGIN_UART backend=%s\r\n",
			     current_module, current_name,
			     backend != NULL ? backend : "");
}

void test_framework_pass(void)
{
	framework_count.passed++;
	test_framework_print("[TEST:%s.%s] PASS\r\n", current_module,
			     current_name);
}

void test_framework_fail(const char *format, ...)
{
	char details[TEST_FRAMEWORK_BUFFER_SIZE];
	va_list args;
	int length;

	framework_count.failed++;

	if (format == NULL) {
		test_framework_print("[TEST:%s.%s] FAIL\r\n", current_module,
				     current_name);
		return;
	}

	va_start(args, format);
	length = vsnprintf(details, sizeof(details), format, args);
	va_end(args);

	if (length <= 0) {
		test_framework_print("[TEST:%s.%s] FAIL\r\n", current_module,
				     current_name);
		return;
	}

	test_framework_print("[TEST:%s.%s] FAIL: %s\r\n", current_module,
			     current_name, details);
}

void test_framework_skip(const char *category, const char *message)
{
	framework_count.skipped++;

	if (message == NULL)
		message = "";

	if (category == NULL) {
		test_framework_print("[TEST:%s.%s] SKIP: %s\r\n",
				     current_module, current_name, message);
		return;
	}

	test_framework_print("[TEST:%s.%s] SKIP: %s [SKIP_CAT:%s]\r\n",
			     current_module, current_name, message, category);
}

void test_framework_info(const char *message)
{
	test_framework_print("[INFO:%s.%s] %s\r\n", current_module,
			     current_name, message != NULL ? message : "");
}

void test_framework_value(const char *name, uint64_t value)
{
	test_framework_print("[VAL:%s.%s.%s] 0x%llX\r\n", current_module,
			     current_name, name != NULL ? name : "",
			     (unsigned long long)value);
}

void test_framework_section(const char *name)
{
	test_framework_print("\r\n--- %s ---\r\n", name != NULL ? name : "");
}

void test_framework_wait_us(uint32_t duration_us)
{
	if (framework_config.wait_us != NULL)
		framework_config.wait_us(framework_config.wait_context,
					 duration_us);
}

void test_framework_get_count(struct test_framework_count *count)
{
	if (count != NULL)
		*count = framework_count;
}

int test_framework_run_cases(const char *module, const struct test_case *cases,
			     size_t count)
{
	int first_error = 0;

	if (cases == NULL)
		return -EINVAL;

	for (size_t i = 0U; i < count; i++) {
		test_framework_begin(module, cases[i].name);

		if (cases[i].skip) {
			test_framework_skip(SKIP_FEATURE_DISABLED,
					    "feature disabled in this build");
			continue;
		}

		/* A NULL runner is treated as a skip, never a crash. */
		if (cases[i].run == NULL) {
			test_framework_skip(SKIP_NOT_IMPLEMENTED,
					    "no runner provided");
			continue;
		}

		/*
		 * Keep going after a failing subtest: remember the first error
		 * but still run the rest so the run summary stays complete.
		 */
		int ret = cases[i].run();
		if (ret != 0 && first_error == 0)
			first_error = ret;
	}

	return first_error;
}
