/***************************************************************************//**
 *   @file   osal_cc.h
 *   @brief  Compiler compatibility macros — maps rt-labs OSAL abstractions
 *           to GCC equivalents.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2017 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef OSAL_CC_H
#define OSAL_CC_H

#include <assert.h>

#define CC_PACKED          __attribute__((packed))
#define CC_PACKED_BEGIN
#define CC_PACKED_END

#define CC_ASSERT(expr)    assert(expr)

#ifndef BIT
#define BIT(n)             (1u << (n))
#endif

#define NELEMENTS(arr)     (sizeof(arr) / sizeof((arr)[0]))

#endif /* OSAL_CC_H */
