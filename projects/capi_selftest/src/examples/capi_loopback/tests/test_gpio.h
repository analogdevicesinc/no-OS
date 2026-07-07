/***************************************************************************//**
 * @file test_gpio.h
 * @brief CAPI GPIO loopback test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_GPIO_H
#define TEST_GPIO_H

/**
 * @brief Run all GPIO test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_gpio(void);

#endif /* TEST_GPIO_H */
