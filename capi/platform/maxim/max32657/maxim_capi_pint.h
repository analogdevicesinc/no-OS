/***************************************************************************//**
 *   @file   maxim_capi_pint.h
 *   @brief  Header for the Maxim CAPI PINT (pin interrupt) driver.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_PINT_H_
#define MAXIM_CAPI_PINT_H_

#include "capi_pint.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** PINT ops table for the MAX32657 GPIO controller. */
extern const struct capi_pint_ops max_capi_pint_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_PINT_H_ */
