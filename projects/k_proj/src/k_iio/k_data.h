/***************************************************************************//**
 *   @file   k_data.h
 *   @brief  K project data-plane engine: DDR staging, DMA, data-offload.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef K_DATA_H_
#define K_DATA_H_

#include <stdint.h>
#include <stdbool.h>

struct axi_dmac;
struct tx_generator_dev;
struct axi_adc;

/******************************************************************************/
/* Stream identifiers carried in bits[1:0] of word[0] of a stream.            */
/******************************************************************************/
enum k_stream_id {
	K_STREAM_VEC	= 1,	/* vector stream */
	K_STREAM_ASYNC	= 2,	/* async stream (RX) */
	K_STREAM_PASS	= 3,	/* passthrough stream */
};

/**
 * @enum k_tx_mode
 * @brief How the data-offload replays a TX transfer.
 */
enum k_tx_mode {
	K_TX_ONESHOT	= 0,	/* play each block once, return to idle */
	K_TX_CYCLIC	= 1,	/* hardware repeats the last block forever */
};

/**
 * @enum k_payload_mode
 * @brief How the TX/RX payload is interpreted by the IIO layer.
 */
enum k_payload_mode {
	K_PAYLOAD_SAMPLES	= 0,	/* interleaved per-channel 16-bit samples */
	K_PAYLOAD_RAW		= 1,	/* opaque octet stream, no scan model */
};

/**
 * @struct k_data_stats
 * @brief  Shared runtime counters, published by the k-time IIO device.
 */
struct k_data_stats {
	uint64_t	last_tx_us;
	uint64_t	last_rx_us;
	uint64_t	tx_bytes;
	uint64_t	rx_bytes;
	uint32_t	tx_xfers;
	uint32_t	rx_xfers;
};

/**
 * @struct k_data_init_param
 * @brief  Everything k_data needs to own the data plane.
 */
struct k_data_init_param {
	/* DMA engines (created by the app, owned here after init) */
	struct axi_dmac		*tx_dmac;
	struct axi_dmac		*rx_dmac;
	struct axi_dmac		*tx_cmd_dmac;
	struct axi_dmac		*rx_cmd_dmac;
	/* TPL / CMD front-ends */
	struct tx_generator_dev	*tx_generator;
	struct axi_adc		*rx_adc;
	/* data-offload base addresses */
	uint32_t		tx_offload_base;
	uint32_t		rx_offload_base;
	/* DDR staging windows (physical addresses in low DDR) */
	uint32_t		tx_ddr_base;
	uint32_t		rx_ddr_base;
	uint32_t		tx_cmd_ddr_base;
	uint32_t		rx_cmd_ddr_base;
	/* size of each staging window in bytes */
	uint32_t		tx_ddr_size;
	uint32_t		rx_ddr_size;
	/* geometry */
	uint32_t		num_channels;
	uint32_t		dma_beat_bytes;	/* L*64b lane width, for alignment */
};

/**
 * @struct k_data
 * @brief  Data-plane engine handle.
 */
struct k_data {
	struct axi_dmac		*tx_dmac;
	struct axi_dmac		*rx_dmac;
	struct axi_dmac		*tx_cmd_dmac;
	struct axi_dmac		*rx_cmd_dmac;
	struct tx_generator_dev	*tx_generator;
	struct axi_adc		*rx_adc;

	uint32_t		tx_offload_base;
	uint32_t		rx_offload_base;
	uint32_t		tx_ddr_base;
	uint32_t		rx_ddr_base;
	uint32_t		tx_cmd_ddr_base;
	uint32_t		rx_cmd_ddr_base;
	uint32_t		tx_ddr_size;
	uint32_t		rx_ddr_size;

	uint32_t		num_channels;
	uint32_t		dma_beat_bytes;

	struct k_data_stats	stats;
};

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int k_data_init(struct k_data **dev, const struct k_data_init_param *init);
int k_data_remove(struct k_data *dev);

/******************************************************************************/
/* Data-offload control (lifted from basic_example do_arm_oneshot/poll_idle)  */
/******************************************************************************/
int k_data_tx_arm_offload(struct k_data *dev, enum k_tx_mode mode,
			  uint32_t tlen_bytes);
int k_data_tx_poll_offload_idle(struct k_data *dev, int timeout_ms);
int k_data_rx_arm_offload(struct k_data *dev);

/******************************************************************************/
/* Bitstream data path (blocks staged through DDR + DMA)                       */
/*                                                                            */
/* A "block" is one chunk of a (possibly much larger) client buffer. The IIO  */
/* submit() callbacks drive these one block at a time; k_data handles cache    */
/* maintenance, offload arming and DMA completion.                            */
/******************************************************************************/

/* Copy one TX block into the staging window and play it out.
 * data/len already prepared by the caller (interleaved or raw). */
int k_data_tx_block(struct k_data *dev, const void *data, uint32_t len,
		    enum k_stream_id sid, enum k_tx_mode mode, int timeout_ms);

/* Capture one RX block from the JESD RX path into the caller buffer.
 * On return *sid_out (if non-NULL) holds the decoded stream id. */
int k_data_rx_block(struct k_data *dev, void *data, uint32_t len,
		    enum k_stream_id *sid_out, int timeout_ms);

/******************************************************************************/
/* Chip register access via the tx_generator CMD channel (the chip "API")     */
/******************************************************************************/
int k_data_reg_write(struct k_data *dev, uint16_t addr, uint32_t val);
int k_data_reg_read(struct k_data *dev, uint16_t addr, uint32_t *val);
int k_data_reg_read_streaming(struct k_data *dev, uint16_t addr,
			      uint8_t numwords, uint32_t *vals);

#endif /* K_DATA_H_ */
