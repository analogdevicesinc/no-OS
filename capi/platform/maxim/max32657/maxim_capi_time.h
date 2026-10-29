/***************************************************************************//**
 *   @file   maxim_capi_time.h
 *   @brief  Header file for Time functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_TIME_H_
#define MAXIM_CAPI_TIME_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Advance the 1 ms system tick counter and service MXC_Delay().
 * @note Safe to call from ISR context.
 */
void max_capi_time_tick(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_TIME_H_ */
