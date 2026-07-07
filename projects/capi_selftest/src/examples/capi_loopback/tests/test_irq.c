/***************************************************************************//**
 * @file test_irq.c
 * @brief CAPI IRQ controller tests.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "parameters.h"
#include "test_framework.h"
#include "test_irq.h"

#ifndef IRQ_CTRL_IDENTIFIER

int test_irq(void)
{
	static const struct test_case stub[] = {
		{ "NOT_CONFIGURED", NULL, false },
	};

	return test_framework_run_cases("IRQ", stub, 1U);
}

#else /* IRQ_CTRL_IDENTIFIER defined — full implementation follows */

#include <errno.h>
#include "capi_irq.h"

#define IRQ_MODULE	"IRQ"

/*
 * The IRQ tests own the controller's lifetime for the duration of the run:
 * they bring it up and tear it down to exercise init/deinit. main.c has
 * already initialized the controller for the IRQ-backed async tests (SPI,
 * timer, I2C), so this test restores it to "initialized and globally
 * enabled" before returning — see irq_restore_controller().
 *
 * A local config is used rather than common_data's irq_config so the IRQ
 * test carries no dependency on any peripheral's config block.
 */
static struct capi_irq_config irq_test_config = {
	.irq_ctrl_id = IRQ_CTRL_IDENTIFIER,
	.extra = NULL,
};

static void irq_test_isr(void *arg)
{
	(void)arg;
}

/**
 * @brief Every entry point must be rejected before the controller is init'd.
 *
 * Brings the controller to a known-uninitialized state, then checks that each
 * API call returns -EINVAL until capi_irq_init() succeeds.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_errors_no_init(void)
{
	uint32_t status = 0U;

	TEST_SECTION("ERR_NO_INIT");
	/* Known-uninitialized state regardless of what ran before. */
	(void)capi_irq_global_disable();
	(void)capi_irq_deinit();

	TEST_ASSERT_EQ(capi_irq_init(NULL), -EINVAL, "NULL_CFG");
	TEST_ASSERT_EQ(capi_irq_deinit(), -EINVAL, "DEINIT_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_global_enable(), -EINVAL, "GLOBAL_EN_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_enable(0U), -EINVAL, "ENABLE_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_disable(0U), -EINVAL, "DISABLE_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_connect(0U, irq_test_isr, NULL), -EINVAL,
		       "CONNECT_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_clear_pending(0U), -EINVAL, "CLEAR_NOT_INIT");
	TEST_ASSERT_EQ(capi_irq_get_status(0U, &status), -EINVAL,
		       "STATUS_NOT_INIT");

	return 0;
}

/**
 * @brief Init has a single owner: a second init returns -EBUSY.
 * @return 0 on pass, negative error code on failure.
 */
static int irq_lifecycle(void)
{
	TEST_SECTION("LIFECYCLE");
	TEST_ASSERT_EQ(capi_irq_init(&irq_test_config), 0, "INIT");
	TEST_ASSERT_EQ(capi_irq_init(&irq_test_config), -EBUSY, "DOUBLE_INIT");
	TEST_ASSERT_EQ(capi_irq_deinit(), 0, "DEINIT");

	return 0;
}

/**
 * @brief Global enable/disable round-trip on an initialized controller.
 * @return 0 on pass, negative error code on failure.
 */
static int irq_global(void)
{
	TEST_SECTION("GLOBAL");
	TEST_ASSERT_EQ(capi_irq_init(&irq_test_config), 0, "INIT");
	TEST_ASSERT_EQ(capi_irq_global_enable(), 0, "GLOBAL_EN");
	TEST_ASSERT_EQ(capi_irq_global_disable(), 0, "GLOBAL_DIS");
	TEST_ASSERT_EQ(capi_irq_deinit(), 0, "DEINIT");

	return 0;
}

/**
 * @brief Connect / enable / status / disable on an initialized controller.
 *
 * Uses IRQ number 0, a valid input on the controller. No interrupt is
 * triggered here — how a source is raised is platform-specific and outside
 * the CAPI IRQ contract, so it is left out of the platform-neutral suite.
 * Argument validation is checked alongside.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_operations(void)
{
	uint32_t status = 0U;
	const uint32_t irq = 0U;

	TEST_SECTION("OPS");
	TEST_ASSERT_EQ(capi_irq_init(&irq_test_config), 0, "INIT");

	TEST_ASSERT_EQ(capi_irq_connect(irq, NULL, NULL), -EINVAL, "NULL_ISR");
	TEST_ASSERT_EQ(capi_irq_connect(irq, irq_test_isr, NULL), 0, "CONNECT");
	TEST_ASSERT_EQ(capi_irq_enable(irq), 0, "ENABLE");
	TEST_ASSERT_EQ(capi_irq_clear_pending(irq), 0, "CLEAR_PENDING");
	TEST_ASSERT_EQ(capi_irq_get_status(irq, &status), 0, "GET_STATUS");
	TEST_ASSERT_EQ(capi_irq_get_status(irq, NULL), -EINVAL, "STATUS_NULL_PTR");
	TEST_ASSERT_EQ(capi_irq_disable(irq), 0, "DISABLE");

	TEST_ASSERT_EQ(capi_irq_deinit(), 0, "DEINIT");

	return 0;
}

/**
 * @brief Restore the controller main.c set up, so async tests keep working.
 *
 * The lifecycle/ops subtests deinit the controller; the IRQ-backed async
 * tests that run after IRQ (SPI, timer, I2C) need it live again.
 *
 * @return 0 on pass, negative error code on failure.
 */
static int irq_restore_controller(void)
{
	TEST_SECTION("RESTORE");
	TEST_ASSERT_EQ(capi_irq_init(&irq_test_config), 0, "REINIT");
	TEST_ASSERT_EQ(capi_irq_global_enable(), 0, "GLOBAL_EN");

	return 0;
}

static const struct test_case irq_subtests[] = {
	{ "ERR_NO_INIT", irq_errors_no_init,   false },
	{ "LIFECYCLE",   irq_lifecycle,        false },
	{ "GLOBAL",      irq_global,           false },
	{ "OPS",         irq_operations,       false },
	{ "RESTORE",     irq_restore_controller, false },
};

int test_irq(void)
{
	return test_framework_run_cases(IRQ_MODULE, irq_subtests,
					sizeof(irq_subtests) / sizeof(irq_subtests[0]));
}

#endif /* IRQ_CTRL_IDENTIFIER */
