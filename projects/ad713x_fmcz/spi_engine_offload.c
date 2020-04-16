/*******************************************************************************
 *   @file   spi_engine_offload.c
 *   @brief  Implementation of SPI Engine offload feature.
 *   @author ADI
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <sleep.h>
#include "error.h"
#include "spi_engine_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static inline struct spi_desc_extra *cast_to_extra_desc(void* void_desc)
{
	return (struct spi_desc_extra*)void_desc;
}

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/*******************************************************************************
 *
 * @func	spi_eng_offload_init
 *
 * @brief	Initialize the spi engine and offload system
 *
 * @params
 *		desc		- Spi engine descriptor
 *		param		- Structure containing the spi init parameters
 *
 * @return			- SUCCESS if the transfer finished
 *				- FAILURE if the memory allocation failed
 *
 ******************************************************************************/
int32_t spi_eng_offload_init(struct spi_desc **desc,
			     const struct spi_init_param *param)
{
	uint32_t		data_width;
	struct spi_desc		*descriptor;
	struct spi_desc_extra	*desc_extra;
	struct spi_init_param_extra *init_extra;

	descriptor = (struct spi_desc *)malloc(sizeof(*descriptor));
	desc_extra = (struct spi_desc_extra*)malloc(sizeof(*desc_extra));

	if (!descriptor || !desc_extra)
		return FAILURE;

	init_extra = (struct spi_init_param_extra*)param->extra;

	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;

	desc_extra->rx_length = 0;
	desc_extra->tx_length = 0;

	desc_extra->clk_div =
		descriptor->max_speed_hz /
		(2 * init_extra->spi_clk_hz) - 1;

	desc_extra->spi_baseaddr =
		init_extra->spi_baseaddr;

	desc_extra->spi_clk_hz =
		init_extra->spi_clk_hz;

	desc_extra->spi_offload_rx_support_en =
		init_extra->spi_offload_rx_support_en;

	desc_extra->spi_offload_tx_support_en =
		init_extra->spi_offload_tx_support_en;

	desc_extra->spi_offload_tx_dma_baseaddr =
		init_extra->spi_offload_tx_dma_baseaddr;

	desc_extra->spi_offload_rx_dma_baseaddr =
		init_extra->spi_offload_rx_dma_baseaddr;

	/* Perform a reset */
	spi_eng_write(desc_extra, SPI_ENGINE_REG_RESET, 0x01);
	usleep(1000);
	spi_eng_write(desc_extra, SPI_ENGINE_REG_RESET, 0x00);

	/* Get current data width */
	spi_eng_read(desc_extra, SPI_ENGINE_REG_DATA_WIDTH, &data_width);
	desc_extra->max_data_width = data_width;
	desc_extra->data_width = data_width;

	descriptor->extra = desc_extra;
	*desc = descriptor;

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_offload_load_msg
 *
 * @brief	Prepare the command queue of the offload mode
 *
 * @params
 *		desc		- Spi engine descriptor
 *		msg		- Structure used to store the messages
 *
 * @return
 *
 ******************************************************************************/
int32_t spi_eng_offload_load_msg(spi_desc *desc, spi_eng_msg *msg)
{
	uint32_t 		i;
	uint32_t		size;
	uint8_t 		words_number;
	spi_eng_transfer_fifo	*xfer;
	spi_desc_extra		*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	desc_extra->rx_dma_startaddr = msg->rx_buf_addr;
	desc_extra->tx_dma_startaddr = msg->tx_buf_addr;

	/* Check if offload is supported */
	if(desc_extra->spi_offload_rx_support_en ||
	    desc_extra->spi_offload_tx_support_en) {
		desc_extra->offload_configured = 1;
	}

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_eng_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return SUCCESS;

	xfer->cmd_fifo_len = 0;

	spi_eng_compile_message(desc, msg, xfer);

	/* Writhe the offload command queue */
	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(desc_extra,
			      SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), xfer->cmd_fifo[i]);

	words_number = spi_get_words_number(desc_extra, desc_extra->tx_length);
	/* Writhe the SDO command queue */
	for(i = 0; i < words_number; i++)
		spi_eng_write(desc_extra,
			      SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0), msg->tx_buf[i]);

	free(xfer);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_transfer_multiple_msgs
 *
 * @brief	Initiate an offload transfer
 *
 * @params
 *		desc		- Spi engine descriptor
 *		no_of_messages	- Number of messages to send in offload mode
 *
 * @return			- SUCCESS if the transfer finished
 *				- FAILURE the offload mode isn't configured
 *
 ******************************************************************************/
int32_t spi_eng_transfer_multiple_msgs(spi_desc *desc, uint32_t no_of_messages)
{
	uint8_t alignment;
	spi_desc_extra		*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	if(!desc_extra->offload_configured)
		return FAILURE;

	if (desc_extra->data_width > 16)
		alignment = sizeof(uint32_t);
	else
		alignment = sizeof(uint16_t);

	if(desc_extra->rx_length) {
		desc_extra->rx_length = alignment * no_of_messages;

		spi_eng_dma_write(desc_extra, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(desc_extra, DMAC_REG_IRQ_MASK, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_IRQ_PENDING, 0xff);
		spi_eng_dma_write(desc_extra, DMAC_REG_DEST_ADDRESS,
				  desc_extra->rx_dma_startaddr);
		spi_eng_dma_write(desc_extra, DMAC_REG_DEST_STRIDE, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_X_LENGTH,
				  desc_extra->rx_length - 1);
		spi_eng_dma_write(desc_extra, DMAC_REG_Y_LENGTH, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_START_TRANSFER, 0x1);
	}

	if(desc_extra->tx_length) {
		desc_extra->tx_length = alignment * no_of_messages;

		spi_eng_dma_write(desc_extra, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(desc_extra, DMAC_REG_IRQ_MASK, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_IRQ_PENDING, 0xff);
		spi_eng_dma_write(desc_extra, DMAC_REG_SRC_ADDRESS,
				  desc_extra->tx_dma_startaddr);
		spi_eng_dma_write(desc_extra, DMAC_REG_SRC_STRIDE, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_X_LENGTH,
				  desc_extra->rx_length - 1);
		spi_eng_dma_write(desc_extra, DMAC_REG_Y_LENGTH, 0x0);
		spi_eng_dma_write(desc_extra, DMAC_REG_FLAGS, 0x1);
		spi_eng_dma_write(desc_extra, DMAC_REG_START_TRANSFER, 0x1);
	}

	usleep(1000);
	/* Enable SPI engine */
	spi_eng_write(desc_extra, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);

	desc_extra->offload_configured = 0;

	return SUCCESS;
}
