/***************************************************************************//**
 * @file test_framework.h
 * @brief Platform-agnostic test framework for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Test framework output function.
 * @param context - Output-specific context.
 * @param data - Data to transmit.
 * @param length - Number of bytes to transmit.
 * @return 0 in case of success, negative error code otherwise.
 */
typedef int (*test_framework_write_fn)(void *context, const uint8_t *data,
				       uint32_t length);

/**
 * @brief Test framework delay function.
 * @param context - Delay-specific context.
 * @param duration_us - Delay duration in microseconds.
 */
typedef void (*test_framework_wait_us_fn)(void *context,
		uint32_t duration_us);

/**
 * @brief Watchdog arm function for the whole-run timeout.
 *
 * The platform owns the timer. The framework calls this at run start to arm a
 * one-shot watchdog. When it expires the platform's handler must call
 * test_framework_on_timeout() and then halt the run.
 *
 * @param context - Watchdog-specific context.
 * @param timeout_ms - Timeout in milliseconds for the entire run.
 */
typedef void (*test_framework_watchdog_arm_fn)(void *context,
		uint32_t timeout_ms);

/**
 * @brief Watchdog disarm function.
 * @param context - Watchdog-specific context.
 */
typedef void (*test_framework_watchdog_disarm_fn)(void *context);

/**
 * @brief Test result counts.
 */
struct test_framework_count {
	/** Number of passing assertions/tests. */
	uint32_t passed;
	/** Number of failing assertions/tests. */
	uint32_t failed;
	/** Number of skipped tests. */
	uint32_t skipped;
};

/**
 * @brief Default whole-run timeout used when config->timeout_ms is 0.
 */
#define TEST_FRAMEWORK_DEFAULT_TIMEOUT_MS	10000U

/**
 * @brief Test framework configuration.
 */
struct test_framework_config {
	/** Label printed in the run start line. */
	const char *run_label;
	/** Output callback used for every structured test line. */
	test_framework_write_fn write;
	/** Opaque context passed to write(). */
	void *write_context;
	/** Optional delay callback used by wait macros. */
	test_framework_wait_us_fn wait_us;
	/** Opaque context passed to wait_us(). */
	void *wait_context;
	/* Whole-run timeout in milliseconds; 0 selects the default. */
	uint32_t timeout_ms;
	/* Optional watchdog hooks; when arm is NULL no timeout is enforced. */
	test_framework_watchdog_arm_fn watchdog_arm;
	test_framework_watchdog_disarm_fn watchdog_disarm;
	void *watchdog_context;
};

/**
 * @brief Initialize the test framework.
 * @param config - Framework configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int test_framework_init(const struct test_framework_config *config);

/**
 * @brief Remove the test framework configuration.
 */
void test_framework_remove(void);

/**
 * @brief Reset the current test context and counts.
 */
void test_framework_reset(void);

/**
 * @brief Start a test run, reset its counts and arm the watchdog.
 */
void test_framework_run_start(void);

/**
 * @brief Disarm the watchdog, print the final counts and end the run.
 */
void test_framework_run_end(void);

/**
 * @brief Select the current test module and test name.
 * @param module - Test module name.
 * @param name - Test name.
 */
void test_framework_begin(const char *module, const char *name);

/**
 * @brief Select the current test name.
 * @param name - Test name.
 */
void test_framework_set_name(const char *name);

/**
 * @brief Record the last reached source location (function and line).
 *
 * Used so a whole-run hang can report where execution stopped. Prefer the
 * TEST_CHECKPOINT() macro over calling this directly. Uses __func__ and
 * __LINE__ rather than __FILE__: on a microcontroller __FILE__ bakes the full
 * build-host path into flash, while __func__ is a small literal and __LINE__
 * costs nothing.
 *
 * @param func - Enclosing function name (__func__).
 * @param line - Source line number (__LINE__).
 */
void test_framework_checkpoint(const char *func, int line);

/**
 * @brief Report from the platform watchdog handler that the run hung.
 *
 * Prints the last test context, the last reached source location and the
 * partial counts. Safe to call once from the timeout handler; it does not
 * itself halt the processor.
 */
void test_framework_on_timeout(void);

/**
 * @brief Begin a UART test as a special case.
 *
 * UART tests can hang (for example when reconfiguring the console UART). This
 * announces the test and records a checkpoint BEFORE the body runs, so a hang
 * is attributable to this exact test. Prefer the TEST_BEGIN_UART() macro.
 *
 * @param backend - UART backend name.
 * @param name - Test name.
 * @param func - Enclosing function name (__func__).
 * @param line - Source line number (__LINE__).
 */
void test_framework_begin_uart(const char *backend, const char *name,
			       const char *func, int line);

/**
 * @brief Record and report a passing test.
 */
void test_framework_pass(void);

/**
 * @brief Record and report a failing test.
 * @param format - Failure detail format.
 */
void test_framework_fail(const char *format, ...);

/**
 * @brief Record and report a skipped test.
 * @param category - Optional skip category.
 * @param message - Skip reason.
 */
void test_framework_skip(const char *category, const char *message);

/**
 * @brief Report test information without changing the counts.
 * @param message - Information message.
 */
void test_framework_info(const char *message);

/**
 * @brief Report a named test value without changing the counts.
 * @param name - Value name.
 * @param value - Value to report.
 */
void test_framework_value(const char *name, uint64_t value);

/**
 * @brief Print a human-readable section marker.
 * @param name - Section name.
 */
void test_framework_section(const char *name);

/**
 * @brief Invoke the configured microsecond delay callback.
 * @param duration_us - Delay duration in microseconds.
 */
void test_framework_wait_us(uint32_t duration_us);

/**
 * @brief Read the current test counts.
 * @param count - Destination counts.
 */
void test_framework_get_count(struct test_framework_count *count);

/** @brief Skip category used when a hardware block is absent. */
#define SKIP_HW_ABSENT		"HW_ABSENT"
/** @brief Skip category used when an interrupt line is absent. */
#define SKIP_IRQ_ABSENT		"IRQ_ABSENT"
/** @brief Skip category used when a BSP symbol is absent. */
#define SKIP_BSP_ABSENT		"BSP_ABSENT"
/** @brief Skip category used when a test could disturb board state. */
#define SKIP_UNSAFE		"UNSAFE"
/** @brief Skip category used for intentionally unsupported behavior. */
#define SKIP_NOT_IMPLEMENTED	"NOT_IMPLEMENTED"
/** @brief Skip category used when board wiring or state prevents a test. */
#define SKIP_BOARD_STATE	"BOARD_STATE"
/** @brief Skip category used when a feature is disabled in this build. */
#define SKIP_FEATURE_DISABLED	"FEATURE_DISABLED"

/** @brief Test case entry for table-driven testing. */
struct test_case {
	/** Test name (passed to TEST_BEGIN). */
	const char *name;
	/** Test function (should return 0 on pass, non-zero on failure). */
	int (*run)(void);
	/** If true, skip this test; if false, run it. */
	bool skip;
};

/**
 * @brief Run a table of test cases, respecting skip flags.
 *
 * Iterates through the table, calling TEST_BEGIN for each entry:
 *   - skip == true      -> reported via test_framework_skip(SKIP_FEATURE_DISABLED).
 *   - run == NULL       -> reported as a SKIP_NOT_IMPLEMENTED skip.
 *   - otherwise         -> run() is executed.
 *
 * Every case is visited even if an earlier one fails, so the run summary stays
 * complete; the first non-zero run() result is returned.
 *
 * @param module - Test module name (passed to TEST_BEGIN).
 * @param cases - Array of test cases.
 * @param count - Number of entries in the cases array.
 * @return 0 if all pass/skip, first non-zero run() result otherwise.
 */
int test_framework_run_cases(const char *module, const struct test_case *cases,
			     size_t count);

/** @brief Select the current module and test name. */
#define TEST_BEGIN(module, name) \
	test_framework_begin((module), (name))

/**
 * @brief Record the current function and line for timeout diagnostics.
 */
#define TEST_CHECKPOINT() \
	test_framework_checkpoint(__func__, __LINE__)

/**
 * @brief Begin a UART test and checkpoint before the UART operation runs.
 *
 * UART may be the report transport, so this announces context before a
 * possible UART hang can silence the log.
 */
#define TEST_BEGIN_UART(backend, name) \
	test_framework_begin_uart((backend), (name), __func__, __LINE__)

/** @brief Record a passing test result. */
#define TEST_PASS() \
	test_framework_pass()

/** @brief Record a failing test result with a fixed message. */
#define TEST_FAIL(message) \
	test_framework_fail("%s", (message))

/** @brief Record a failing test result with got/expected values. */
#define TEST_FAIL_VAL(message, got, expected) \
	test_framework_fail("%s got=%lld expected=%lld", (message), \
			    (long long)(got), (long long)(expected))

/** @brief Record an uncategorized skip and return from the current test. */
#define TEST_SKIP(message) \
	do { \
		test_framework_skip(NULL, (message)); \
		return 0; \
	} while (0)

/** @brief Record a categorized skip and return from the current test. */
#define TEST_SKIP_CAT(category, message) \
	do { \
		test_framework_skip((category), (message)); \
		return 0; \
	} while (0)

/** @brief Print informational text without changing result counts. */
#define TEST_INFO(message) \
	test_framework_info((message))

/** @brief Print a named hexadecimal value without changing result counts. */
#define TEST_VALUE(name, value) \
	test_framework_value((name), (uint64_t)(value))

/** @brief Print a human-readable section marker. */
#define TEST_SECTION(name) \
	test_framework_section((name))

/** @brief Start a test run and reset result counts. */
#define TEST_RUN_START() \
	test_framework_run_start()

/** @brief Finish a test run and print the summary. */
#define TEST_RUN_END() \
	test_framework_run_end()

/** @brief Begin a backend contract test name. */
#define TEST_CONTRACT_BEGIN(module, backend, kind, detail) \
	TEST_BEGIN(module "." backend ".CONTRACT", kind "." detail)

/** @brief Assert that a condition is true and return on failure. */
#define TEST_ASSERT(condition, name) \
	do { \
		bool _test_result = (bool)(condition); \
		test_framework_set_name((name)); \
		if (!_test_result) { \
			test_framework_fail("assert line %d", __LINE__); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Assert that two integer-like values are equal. */
#define TEST_ASSERT_EQ(actual, expected, name) \
	do { \
		long long _test_actual = (long long)(actual); \
		long long _test_expected = (long long)(expected); \
		test_framework_set_name((name)); \
		if (_test_actual != _test_expected) { \
			test_framework_fail("got=%lld expected=%lld", \
					    _test_actual, _test_expected); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Assert that two integer-like values are different. */
#define TEST_ASSERT_NEQ(actual, expected, name) \
	do { \
		long long _test_actual = (long long)(actual); \
		long long _test_expected = (long long)(expected); \
		test_framework_set_name((name)); \
		if (_test_actual == _test_expected) { \
			test_framework_fail("got=%lld should_not_equal=%lld", \
					    _test_actual, _test_expected); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Assert that a value is inside an inclusive range. */
#define TEST_ASSERT_RANGE(value, minimum, maximum, name) \
	do { \
		long long _test_value = (long long)(value); \
		long long _test_minimum = (long long)(minimum); \
		long long _test_maximum = (long long)(maximum); \
		test_framework_set_name((name)); \
		if (_test_value < _test_minimum || \
		    _test_value > _test_maximum) { \
			test_framework_fail("value=%lld range=[%lld,%lld]", \
					    _test_value, _test_minimum, \
					    _test_maximum); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Internal helper used by relational assertion macros. */
#define TEST_ASSERT_COMPARE(actual, expected, name, invalid_op, relation) \
	do { \
		long long _test_actual = (long long)(actual); \
		long long _test_expected = (long long)(expected); \
		test_framework_set_name((name)); \
		if (_test_actual invalid_op _test_expected) { \
			test_framework_fail("got=%lld %s=%lld", _test_actual, \
					    (relation), _test_expected); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Assert that actual is greater than expected. */
#define TEST_ASSERT_GT(actual, expected, name) \
	TEST_ASSERT_COMPARE(actual, expected, name, <=, "expected_gt")

/** @brief Assert that actual is less than expected. */
#define TEST_ASSERT_LT(actual, expected, name) \
	TEST_ASSERT_COMPARE(actual, expected, name, >=, "expected_lt")

/** @brief Assert that actual is greater than or equal to expected. */
#define TEST_ASSERT_GTE(actual, expected, name) \
	TEST_ASSERT_COMPARE(actual, expected, name, <, "expected_gte")

/** @brief Assert that actual is less than or equal to expected. */
#define TEST_ASSERT_LTE(actual, expected, name) \
	TEST_ASSERT_COMPARE(actual, expected, name, >, "expected_lte")

/** @brief Accept either success or an explicitly unsupported operation. */
#define TEST_ASSERT_OK_OR_NOTSUP(result, name) \
	do { \
		int _test_result = (int)(result); \
		test_framework_set_name((name)); \
		if (_test_result != 0 && _test_result != -ENOTSUP) { \
			test_framework_fail("ret=%d expected 0 or -ENOTSUP", \
					    _test_result); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Poll a condition until it becomes true or the timeout expires. */
#define TEST_WAIT_UNTIL(condition, maximum_us, step_us) \
	do { \
		uint32_t _test_waited = 0U; \
		uint32_t _test_step = (uint32_t)(step_us); \
		uint32_t _test_maximum = (uint32_t)(maximum_us); \
		if (_test_step == 0U) \
			_test_step = 100U; \
		while (!(condition) && _test_waited < _test_maximum) { \
			test_framework_wait_us(_test_step); \
			_test_waited += _test_step; \
		} \
	} while (0)

/** @brief Assert that actual is within tolerance of expected. */
#define TEST_ASSERT_NEAR(actual, expected, tolerance, name) \
	do { \
		long long _test_actual = (long long)(actual); \
		long long _test_expected = (long long)(expected); \
		long long _test_tolerance = (long long)(tolerance); \
		long long _test_delta = _test_actual - _test_expected; \
		test_framework_set_name((name)); \
		if (_test_delta < 0) \
			_test_delta = -_test_delta; \
		if (_test_delta > _test_tolerance) { \
			test_framework_fail("got=%lld expected=%lld tolerance=%lld", \
					    _test_actual, _test_expected, \
					    _test_tolerance); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

/** @brief Assert that a counter moved in the requested direction. */
#define TEST_ASSERT_COUNTER_DIR(first, second, direction, name) \
	do { \
		unsigned long _test_first = (unsigned long)(first); \
		unsigned long _test_second = (unsigned long)(second); \
		int _test_direction = (int)(direction); \
		bool _test_valid; \
		test_framework_set_name((name)); \
		if (_test_direction > 0) \
			_test_valid = _test_second > _test_first; \
		else if (_test_direction < 0) \
			_test_valid = _test_second < _test_first; \
		else \
			_test_valid = _test_second != _test_first; \
		if (!_test_valid) { \
			test_framework_fail("first=%lu second=%lu direction=%d", \
					    _test_first, _test_second, \
					    _test_direction); \
			return -1; \
		} \
		test_framework_pass(); \
	} while (0)

#endif /* __TEST_FRAMEWORK_H__ */
