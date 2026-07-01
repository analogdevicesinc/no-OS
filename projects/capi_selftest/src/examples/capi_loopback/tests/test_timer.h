/***************************************************************************//**
 * @file test_timer.h
 * @brief CAPI timer loopback test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

/**
 * @brief Run all timer test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_timer(void);

#endif /* TEST_TIMER_H */
