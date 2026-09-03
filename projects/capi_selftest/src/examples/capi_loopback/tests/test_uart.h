/***************************************************************************//**
 * @file test_uart.h
 * @brief CAPI UART external-loopback test group.
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_UART_H
#define TEST_UART_H

/**
 * @brief Run all UART test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_uart(void);

#endif /* TEST_UART_H */
