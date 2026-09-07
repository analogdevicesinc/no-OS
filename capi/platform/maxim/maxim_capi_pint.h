/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_pint.h
 * @brief Maxim (MSDK) PINT (pin interrupt) backend: one common file for all parts.
 *
 * Implements the neutral capi_pint contract on top of MSDK's MXC_GPIO_*
 * interrupt API. A PINT "port" maps 1:1 to an MSDK GPIO instance; the port id
 * indexes MXC_GPIO_GET_GPIO()/MXC_GPIO_GET_IRQ(), so the same source covers
 * single- and multi-port parts without a TARGET_NUM branch.
 */

#ifndef MAXIM_CAPI_PINT_H_
#define MAXIM_CAPI_PINT_H_

#include "capi_pint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** PINT ops table for the Maxim GPIO interrupt controller. */
extern const struct capi_pint_ops max_capi_pint_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_PINT_H_ */
