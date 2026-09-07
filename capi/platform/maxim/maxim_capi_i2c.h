/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_i2c.h
 * @brief Maxim (MSDK) I2C backend, common to all classic-I2C parts.
 *
 * Public surface: the platform-specific extra-config struct and the ops table.
 * The backend is written against the MSDK public I2C API (MXC_I2C_Init,
 * MXC_I2C_MasterTransaction[Async], MXC_I2C_SetFrequency), which is uniform
 * across the classic-I2C MAX32xxx/MAX78xxx parts (identical mxc_i2c_req_t,
 * 3-arg MXC_I2C_Init). Parts that expose I2C through the I3C block instead
 * (e.g. MAX32657) are a separate capability variant and are NOT handled here.
 */

#ifndef MAXIM_CAPI_I2C_H_
#define MAXIM_CAPI_I2C_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "capi_i2c.h"
#include "maxim_capi_gpio.h"

/**
 * @struct max_capi_i2c_extra
 * @brief Maxim platform-specific extra configuration for I2C.
 */
struct max_capi_i2c_extra {
	/** Voltage level select for the I2C pads (VDDIO vs VDDIOH). */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config parameters (reserved; DMA path is a follow-up). */
	struct capi_dma_config *dma_config;
};

extern const struct capi_i2c_ops max_capi_i2c_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_I2C_H_ */
