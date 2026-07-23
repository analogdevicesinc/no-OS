/***************************************************************************//**
 * @file test_irq.h
 * @brief CAPI IRQ controller test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_IRQ_H
#define TEST_IRQ_H

/**
 * @brief Run all IRQ controller test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_irq(void);

#endif /* TEST_IRQ_H */
