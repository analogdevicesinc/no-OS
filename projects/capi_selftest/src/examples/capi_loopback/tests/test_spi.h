/***************************************************************************//**
 * @file test_spi.h
 * @brief CAPI SPI loopback test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_SPI_H_
#define TEST_SPI_H_

/**
 * @brief Run all SPI test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_spi(void);

#endif /* TEST_SPI_H_ */
