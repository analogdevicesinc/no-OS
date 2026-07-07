/***************************************************************************//**
 * @file test_i2c.h
 * @brief CAPI I2C loopback test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_I2C_H
#define TEST_I2C_H

/**
 * @brief Run all I2C test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_i2c(void);

#endif /* TEST_I2C_H */
