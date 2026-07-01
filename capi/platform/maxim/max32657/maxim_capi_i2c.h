/*******************************************************************************
 *   @file   maxim_capi_i2c.h
 *   @brief  Header file for I2C functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef MAXIM_CAPI_I2C_H_
#define MAXIM_CAPI_I2C_H_

#include "maxim_capi_gpio.h"

#define MAX_CAPI_I2C_TARGET_RX_BUFFER_SIZE	256
#define MAX_CAPI_I2C_FIFO_DEPTH			32

#define MAX_CAPI_I2C_DMA_TIMEOUT			1000
#define MAX_CAPI_I2C_DMA_CALLBACK_CLEANUP_TIMEOUT	10000

enum max_capi_i2c_speed {
	/** 100 kHz */
	MAX_CAPI_I2C_SPEED_STANDARD =	 100000,
	/** 400 kHz */
	MAX_CAPI_I2C_SPEED_FAST = 	 400000,
	/** 1 MHz */
	MAX_CAPI_I2C_SPEED_FAST_PLUS =	1000000,
	/** 3.4 MHz - NOT SUPPORTED */
	MAX_CAPI_I2C_SPEED_HIGH =	3400000,
	/** 5 MHz - NOT SUPPORTED */
	MAX_CAPI_I2C_SPEED_ULTRA =	5000000,
};

enum max_capi_i2c_target_state_enum {
	/** Listening for address */
	MAX_CAPI_I2C_TARGET_STATE_IDLE,
	/** Receiving from controller */
	MAX_CAPI_I2C_TARGET_STATE_RX,
	/** Transmitting to controller */
	MAX_CAPI_I2C_TARGET_STATE_TX,
};

struct max_capi_i2c_target_state {
	/** Target mode active */
	bool enabled;
	/** Target address */
	uint16_t address;
	/** 10-bit address */
	bool b10addr;
	/** State enum */
	enum max_capi_i2c_target_state_enum state;
	/** Temporary RX buffer */
	uint8_t rx_buffer[MAX_CAPI_I2C_TARGET_RX_BUFFER_SIZE];
	/** Bytes received */
	uint16_t rx_count;
	/** TX data from callback */
	const uint8_t *tx_buffer;
	/** Bytes transmitted */
	uint16_t tx_count;
	/** Total TX length */
	uint16_t tx_length;
};

enum max_capi_i2c_async_state_enum {
	/** No async in progress */
	MAX_CAPI_I2C_ASYNC_STATE_IDLE,
	/** Transmitting sub-address */
	MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR,
	/** Transmitting data */
	MAX_CAPI_I2C_ASYNC_STATE_TX_DATA,
	/** Receiving data */
	MAX_CAPI_I2C_ASYNC_STATE_RX_DATA,
	/** Waiting for transaction completion */
	MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE,
	/** Error occurred */
	MAX_CAPI_I2C_ASYNC_STATE_ERROR,
};

struct max_capi_i2c_async_state {
	/** Async state */
	volatile enum max_capi_i2c_async_state_enum state;
	/** Target I2C address */
	uint16_t target_addr;
	/** Sub-address buffer */
	uint8_t *subaddr_buf;
	/** Sub-address length remaining */
	uint16_t subaddr_len;
	/** Sub-address write index */
	uint16_t subaddr_idx;
	/** Data buffer */
	uint8_t *data_buf;
	/** Data length remaining */
	uint16_t data_len;
	/** Data read/write index */
	uint16_t data_idx;
	/** Send STOP condition */
	bool send_stop;
	/** Operation result (0 on success, negative error code otherwise) */
	int result;
	/** Transmit (true), or Receive (false) */
	bool is_transmit;
	/** DMA allocated buffer storage */
	uint8_t *dma_allocated_buffer;
};

struct max_capi_i2c_priv {
	/** I2C controller handle */
	struct capi_i2c_controller_handle *i2c_handle;
	/** I2C device ID; only 0 is valid for MAX32657 */
	uint32_t identifier;
	/** DMA controller handle */
	struct capi_dma_handle *dma_handle;
	/** For storing the platform-specific config */
	struct max_capi_i2c_extra *extra;
	/** DMA callback function */
	capi_i2c_callback callback;
	/** DMA callback arg */
	void *callback_arg;
	/** Flag to prevent DMA race condition */
	volatile bool callback_active;
	/** Clock frequency in Hz */
	uint32_t freq;
	/** Target mode state */
	struct max_capi_i2c_target_state *target;
	/** Async state */
	struct max_capi_i2c_async_state *async;
};

struct max_capi_i2c_extra {
	/** Voltage level of the I3C peripheral */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config parameters */
	struct capi_dma_config *dma_config;
};

extern const struct capi_i2c_ops max_capi_i2c_ops;

#endif /* MAXIM_CAPI_I2C_H_ */
