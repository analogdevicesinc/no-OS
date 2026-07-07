/***************************************************************************//**
 *   @file   k_data.c
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
#include <string.h>
#include "k_data.h"
#include "axi_dmac.h"
#include "tx_generator.h"
#include "axi_adc_core.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_axi_io.h"
#include "no_os_print_log.h"

/* data-offload register offsets (from basic_example.c) */
#define K_OFFLOAD_REG_RESET	0x084	/* bit0 = sw_resetn */
#define K_OFFLOAD_REG_CONTROL	0x088	/* bit1 = oneshot, bit0 = bypass */
#define K_OFFLOAD_REG_TLEN	0x01C	/* transfer length (bytes-1) */
#define K_OFFLOAD_REG_SYNC	0x104
#define K_OFFLOAD_REG_FSM	0x200	/* [4:0]=wr_fsm, [11:8]=rd_fsm */

/* rx_analyzer CMD response registers (from basic_example.c) */
#define K_RXANLZ_REG_CMD_DATA		0x0C0
#define K_RXANLZ_REG_CMD_ADDR_OP	0x0C4

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int k_data_init(struct k_data **dev, const struct k_data_init_param *init)
{
	struct k_data *d;

	if (!dev || !init)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->tx_dmac		= init->tx_dmac;
	d->rx_dmac		= init->rx_dmac;
	d->tx_cmd_dmac		= init->tx_cmd_dmac;
	d->rx_cmd_dmac		= init->rx_cmd_dmac;
	d->tx_generator		= init->tx_generator;
	d->rx_adc		= init->rx_adc;
	d->tx_offload_base	= init->tx_offload_base;
	d->rx_offload_base	= init->rx_offload_base;
	d->tx_ddr_base		= init->tx_ddr_base;
	d->rx_ddr_base		= init->rx_ddr_base;
	d->tx_cmd_ddr_base	= init->tx_cmd_ddr_base;
	d->rx_cmd_ddr_base	= init->rx_cmd_ddr_base;
	d->tx_ddr_size		= init->tx_ddr_size;
	d->rx_ddr_size		= init->rx_ddr_size;
	d->num_channels		= init->num_channels;
	d->dma_beat_bytes	= init->dma_beat_bytes ? init->dma_beat_bytes : 4;

	*dev = d;

	return 0;
}

int k_data_remove(struct k_data *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_free(dev);

	return 0;
}

/******************************************************************************/
/* Data-offload control                                                       */
/******************************************************************************/
int k_data_tx_arm_offload(struct k_data *dev, enum k_tx_mode mode,
			  uint32_t tlen_bytes)
{
	uint32_t ctrl;

	if (!dev)
		return -EINVAL;

	/* bit1 = oneshot; cyclic is the hardware default (oneshot cleared) */
	ctrl = (mode == K_TX_ONESHOT) ? 0x02 : 0x00;

	no_os_axi_io_write(dev->tx_offload_base, K_OFFLOAD_REG_RESET, 0x00);
	no_os_mdelay(1);
	no_os_axi_io_write(dev->tx_offload_base, K_OFFLOAD_REG_CONTROL, ctrl);
	no_os_axi_io_write(dev->tx_offload_base, K_OFFLOAD_REG_TLEN, tlen_bytes);
	no_os_axi_io_write(dev->tx_offload_base, K_OFFLOAD_REG_SYNC, 0x00);
	no_os_axi_io_write(dev->tx_offload_base, K_OFFLOAD_REG_RESET, 0x01);

	return 0;
}

int k_data_tx_poll_offload_idle(struct k_data *dev, int timeout_ms)
{
	uint32_t fsm;

	if (!dev)
		return -EINVAL;

	do {
		no_os_mdelay(1);
		no_os_axi_io_read(dev->tx_offload_base, K_OFFLOAD_REG_FSM, &fsm);
	} while (((fsm & 0x1F) != 1 || ((fsm >> 8) & 0xF) != 1) && --timeout_ms);

	return timeout_ms ? 0 : -ETIMEDOUT;
}

int k_data_rx_arm_offload(struct k_data *dev)
{
	if (!dev)
		return -EINVAL;

	/* RX offload set to bypass (bit0), per basic_example RX bring-up */
	no_os_axi_io_write(dev->rx_offload_base, K_OFFLOAD_REG_RESET, 0x00);
	no_os_mdelay(1);
	no_os_axi_io_write(dev->rx_offload_base, K_OFFLOAD_REG_CONTROL, 0x01);
	no_os_axi_io_write(dev->rx_offload_base, K_OFFLOAD_REG_RESET, 0x01);

	return 0;
}

/******************************************************************************/
/* Bitstream data path                                                        */
/*                                                                            */
/* TODO(phase 3): implement chunked ping-pong staging for multi-MB buffers.   */
/* This skeleton stages a single block up to the DDR window size.             */
/******************************************************************************/
int k_data_tx_block(struct k_data *dev, const void *data, uint32_t len,
		    enum k_stream_id sid, enum k_tx_mode mode, int timeout_ms)
{
	(void)sid;	/* TODO: stamp stream id into word[0] bits[1:0] */

	if (!dev || !data || !len)
		return -EINVAL;
	if (len > dev->tx_ddr_size)
		return -EINVAL;

	/* TODO:
	 *  1. memcpy(data) into tx_ddr_base staging window
	 *  2. Xil_DCacheFlushRange(tx_ddr_base, len)
	 *  3. k_data_tx_arm_offload(dev, mode, len - 1)
	 *  4. axi_dmac_transfer_start(tx_dmac, {src=tx_ddr_base, size=len})
	 *  5. axi_dmac_transfer_wait_completion(tx_dmac, timeout_ms)
	 *  6. k_data_tx_poll_offload_idle(dev, ...)
	 *  7. update stats (tx_bytes, tx_xfers, last_tx_us)
	 */
	(void)mode;
	(void)timeout_ms;

	return -ENOSYS;
}

int k_data_rx_block(struct k_data *dev, void *data, uint32_t len,
		    enum k_stream_id *sid_out, int timeout_ms)
{
	if (!dev || !data || !len)
		return -EINVAL;
	if (len > dev->rx_ddr_size)
		return -EINVAL;

	/* TODO:
	 *  1. axi_dmac_transfer_start(rx_dmac, {dest=rx_ddr_base, size=len})
	 *  2. axi_dmac_transfer_wait_completion(rx_dmac, timeout_ms)
	 *  3. Xil_DCacheInvalidateRange(rx_ddr_base, len)
	 *  4. decode stream id from word[0] bits[1:0] -> *sid_out
	 *  5. memcpy staging -> data
	 *  6. update stats (rx_bytes, rx_xfers, last_rx_us)
	 */
	(void)sid_out;
	(void)timeout_ms;

	return -ENOSYS;
}

/******************************************************************************/
/* Chip register access via the tx_generator CMD channel                      */
/******************************************************************************/
int k_data_reg_write(struct k_data *dev, uint16_t addr, uint32_t val)
{
	if (!dev || !dev->tx_generator)
		return -EINVAL;

	return tx_generator_cmd_write(dev->tx_generator, addr, 1,
				      TX_GEN_CMD_OP_WRITE, val);
}

int k_data_reg_read(struct k_data *dev, uint16_t addr, uint32_t *val)
{
	uint32_t data, addr_op;

	if (!dev || !dev->tx_generator || !dev->rx_adc || !val)
		return -EINVAL;

	/* Issue the READ command over the CMD channel. */
	tx_generator_cmd_write(dev->tx_generator, addr, 1,
			       TX_GEN_CMD_OP_READ, 0);

	/* TODO: proper completion handshake instead of a fixed delay. */
	no_os_mdelay(1);

	/* Read the response the rx_analyzer latched. */
	axi_adc_read(dev->rx_adc, K_RXANLZ_REG_CMD_DATA, &data);
	axi_adc_read(dev->rx_adc, K_RXANLZ_REG_CMD_ADDR_OP, &addr_op);

	/* TODO: validate ((addr_op >> 16) & 0xFFFF) == addr echo. */
	(void)addr_op;
	*val = data;

	return 0;
}

int k_data_reg_read_streaming(struct k_data *dev, uint16_t addr,
			      uint8_t numwords, uint32_t *vals)
{
	if (!dev || !dev->tx_generator || !vals || !numwords)
		return -EINVAL;

	/* TODO: OP_STREAMING_READ over tx_cmd_dmac, capture numwords words
	 * from rx_cmd_ddr via rx_cmd_dmac. */
	(void)addr;
	(void)numwords;

	return -ENOSYS;
}
