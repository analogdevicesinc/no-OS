/***************************************************************************//**
 * @file test_dma.h
 * @brief CAPI DMA memory-to-memory test entry.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef TEST_DMA_H
#define TEST_DMA_H

/**
 * @brief Run all DMA test cases available on the mapped platform.
 * @return 0 if all executed cases passed, first non-zero test error otherwise.
 */
int test_dma(void);

#endif /* TEST_DMA_H */
