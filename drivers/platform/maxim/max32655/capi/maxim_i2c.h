/***************************************************************************//**
 *   @file   maxim_i2c.h
 *   @brief  Header file of the I2C driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAXIM_CAPI_I2C_H
#define MAXIM_CAPI_I2C_H

#include "capi_i2c.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAX32655 I2C CAPI private handle structure
 */
struct maxim_capi_i2c_handle {
	/** I2C instance identifier (0-2) */
	uint32_t instance;
	/** I2C registers pointer */
	mxc_i2c_regs_t *regs;
	/** I2C request structure */
	mxc_i2c_req_t req;
	/** Clock frequency in Hz */
	uint32_t clk_freq_hz;
	/** Controller mode: true=initiator, false=target */
	bool initiator;
	/** Target device address (when acting as target) */
	uint8_t address;
	/** Callback function */
	capi_i2c_callback callback;
	/** Callback argument */
	void *callback_arg;
	/** Current transfer in progress */
	bool transfer_active;
};

/**
 * @brief Static allocation macro for MAX32655 I2C CAPI handle
 *
 * Usage:
 *   struct capi_i2c_controller_handle *handle = CAPI_I2C_HANDLE;
 */
#define CAPI_I2C_HANDLE &(struct capi_i2c_controller_handle){ \
	.priv = &(struct maxim_capi_i2c_handle){} \
}

/**
 * @brief MAX32655 I2C CAPI operations structure
 */
extern const struct capi_i2c_ops maxim_i2c_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_I2C_H */