/*******************************************************************************
 *   @file   maxim_capi_i2c_priv.h
 *   @brief  Header file for I2C private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_I2C_PRIV_H_
#define MAXIM_CAPI_I2C_PRIV_H_

#include "maxim_capi_i2c.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define MAX_CAPI_I2C_TARGET_RX_BUFFER_SIZE	256
#define MAX_CAPI_I2C_FIFO_DEPTH			32

#define MAX_CAPI_I2C_DMA_TIMEOUT			1000
#define MAX_CAPI_I2C_DMA_CALLBACK_CLEANUP_TIMEOUT	10000

/**
 * @enum max_capi_i2c_target_state_enum
 * @brief State of I2C target mode
 */
enum max_capi_i2c_target_state_enum {
	/** Listening for address */
	MAX_CAPI_I2C_TARGET_STATE_IDLE,
	/** Receiving from controller */
	MAX_CAPI_I2C_TARGET_STATE_RX,
	/** Transmitting to controller */
	MAX_CAPI_I2C_TARGET_STATE_TX,
};

/**
 * @struct max_capi_i2c_target_state
 * @brief State data for I2C target mode
 */
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

/**
 * @enum max_capi_i2c_async_state_enum
 * @brief State of I2C controller async mode
 */
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

/**
 * @struct max_capi_i2c_async_state
 * @brief State data for I2C controller async mode
 */
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

/**
 * @struct max_capi_i2c_priv
 * @brief Private structure for I2C
 */
struct max_capi_i2c_priv {
	/** I2C controller handle */
	struct capi_i2c_controller_handle *i2c_handle;
	/** I2C device ID; only 0 is valid for MAX32657 */
	uint32_t identifier;
	/** DMA controller handle */
	struct capi_dma_handle *dma_handle;
	/** Voltage level of the I3C peripheral */
	enum max_capi_gpio_vssel vssel;
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
	/** DMA completion flag */
	volatile bool dma_completed;
	/** TX DMA channel */
	struct capi_dma_chan *dma_channel_tx;
	/** RX DMA channel */
	struct capi_dma_chan *dma_channel_rx;
	/** Async transfer flag */
	volatile bool async_transfer_in_progress;
	/** Whether to enable IRQ connection during init */
	bool use_irq;

};

#define CAPI_I2C_CONTROLLER_HANDLE_MAXIM_INIT()		\
	(&(struct capi_i2c_controller_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_i2c_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_I2C_PRIV_H_ */
