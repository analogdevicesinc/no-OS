/***************************************************************************//**
 *   @file   spi.c
 *   @brief  Implementation of SPI Engine Driver.
 *   @author ADI
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>
#include <xil_io.h>
#include <xscugic.h>
#include <xparameters.h>
#include <sleep.h>
#include "spi_engine.h"
#include "xil_printf.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/***************************************************************************//**
* @brief spi_eng_write
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_write(spi_eng_desc *desc,
		      uint32_t reg_addr,
		      uint32_t ui32data)
#else
int32_t spi_eng_write(spi_desc *desc,
		      uint32_t reg_addr,
		      uint32_t ui32data)
#endif
{
	Xil_Out32((desc->spi_baseaddr + reg_addr), ui32data);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_read
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_read(spi_eng_desc *desc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
#else
int32_t spi_eng_read(spi_desc *desc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
#endif
{
	*reg_data = Xil_In32((desc->spi_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_dma_write
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_dma_write(spi_eng_desc *desc,
			  uint32_t reg_addr,
			  uint32_t reg_data)
#else
int32_t spi_eng_dma_write(spi_desc *desc,
			  uint32_t reg_addr,
			  uint32_t reg_data)
#endif
{
	Xil_Out32((desc->spi_offload_tx_dma_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_dma_read
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_dma_read(spi_eng_desc *desc,
			 uint32_t reg_addr,
			 uint32_t *reg_data)
#else
int32_t spi_eng_dma_read(spi_desc *desc,
			 uint32_t reg_addr,
			 uint32_t *reg_data)
#endif
{
	*reg_data = Xil_In32((desc->spi_offload_rx_dma_baseaddr + reg_addr));

	return 0;
}

#ifdef DUAL_SPI
void spi_set_transfer_length(spi_eng_desc *desc, uint8_t data_length)
#else
void spi_set_transfer_length(spi_desc *desc, uint8_t data_length)
#endif
{
	if (data_length > desc->max_data_width)
		desc->data_width = desc->max_data_width;
	else
		desc->data_width = data_length;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
#ifdef DUAL_SPI
int32_t spi_eng_write_and_read(spi_eng_desc *desc,
			       uint8_t *data,
			       uint8_t bytes_number)
#else
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
#endif
{
	/*
	 * Note:  This function works like a classic SPI
	 */
	spi_eng_msg *msg;
	uint8_t i;
	uint32_t spi_eng_msg_cmds[4];
	uint32_t xfer_bytes_number;
	uint8_t data_width, data_width_bytes;
	int32_t ret;

	data_width = desc->max_data_width;
	data_width_bytes = data_width / 8;
	/*
	 * Each spi engine transaction equals data_width bytes
	 */
	xfer_bytes_number = (bytes_number % data_width_bytes) == 0 ?
			    bytes_number : bytes_number + 1;
	spi_eng_msg_cmds[0] = CS_ASSERT;
	spi_eng_msg_cmds[1] = CS_DEASSERT;
	spi_eng_msg_cmds[2] = TRANSFER_BYTES_R_W(xfer_bytes_number);
	spi_eng_msg_cmds[3] = CS_ASSERT;

	msg = (spi_eng_msg *)malloc(sizeof(*msg));
	if (!msg)
		return -1;

	msg->spi_msg_cmds = malloc(sizeof(spi_eng_msg_cmds));
	msg->spi_msg_cmds = spi_eng_msg_cmds;
	msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);

	// Init the rx and tx buffers with 0s
	for (i = 0; i < ARRAY_SIZE(msg->tx_buf); i++) {
		msg->tx_buf[i] = 0;
		msg->rx_buf[i] = 0;
	}

	for (i = 0; i < bytes_number; i++)
		msg->tx_buf[i/data_width_bytes] |= data[i] << (data_width - 8 - 8 * i);

	ret = spi_eng_transfer_message(desc, msg);
	usleep(1000000); // 1s

	for (i = 0; i < bytes_number; i++)
		data[i] = msg->rx_buf[i / data_width_bytes]; //>> (data_width - 8 - 8 * i);

	free(msg);

	return ret;
}

/***************************************************************************//**
* @brief spi_eng_program_add_cmd
*******************************************************************************/
void spi_eng_program_add_cmd(spi_eng_transfer_fifo *xfer,
			     uint16_t cmd)
{
	xfer->cmd_fifo[xfer->cmd_fifo_len] = cmd;
	xfer->cmd_fifo_len++;
}

/***************************************************************************//**
* @brief get_sleep_div
*******************************************************************************/
#ifdef DUAL_SPI
uint32_t spi_get_sleep_div(spi_eng_desc *desc,
			   uint32_t sleep_time_ns)
#else
uint32_t spi_get_sleep_div(spi_desc *desc,
			   uint32_t sleep_time_ns)
#endif
{
	uint32_t sleep_div = 0;

	sleep_div = (desc->ref_clk_hz / 1000000 * sleep_time_ns / 1000) /
		    ((desc->clk_div + 1) * 2) - 1;

	return sleep_div;
}

/***************************************************************************//**
* @brief get_words_number
*******************************************************************************/
#ifdef DUAL_SPI
uint8_t spi_get_words_number(spi_eng_desc *desc, uint8_t bytes_number)
#else
uint8_t spi_get_words_number(spi_desc *desc, uint8_t bytes_number)
#endif
{
	uint8_t words_number = 0;
	uint8_t	data_width	= desc->max_data_width;
	uint8_t data_width_bytes = desc->data_width / 8;

	/* If the number of bytes to transfer is lower than the data width, then
	 * the transfer is 1 word long
	 */
	if ((bytes_number * 8) < data_width)
		words_number = 1;
	else if (bytes_number % data_width_bytes)
		words_number = (bytes_number / data_width_bytes) + 1;
	else
		words_number = bytes_number / data_width_bytes;

	return words_number;
}

/***************************************************************************//**
* @brief spi_eng_gen_cs
*******************************************************************************/
#ifdef DUAL_SPI
void spi_eng_gen_cs(spi_eng_desc *desc,
		    spi_eng_transfer_fifo *xfer,
		    bool assert)
#else
void spi_eng_gen_cs(spi_desc *desc,
		    spi_eng_transfer_fifo *xfer,
		    bool assert)
#endif
{
	uint8_t mask = 0xff;

	if (!assert)
		mask ^= BIT(desc->chip_select);

	spi_eng_program_add_cmd(xfer, SPI_ENGINE_CMD_ASSERT(desc->cs_delay, mask));
}

/***************************************************************************//**
* @brief spi_eng_gen_transfer
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_gen_transfer(spi_eng_desc *desc,
			     spi_eng_transfer_fifo *xfer,
			     bool write,
			     bool read,
			     uint8_t bytes_number)
#else
int32_t spi_eng_gen_transfer(spi_desc *desc,
			     spi_eng_transfer_fifo *xfer,
			     bool write,
			     bool read,
			     uint8_t bytes_number)
#endif
{
	uint8_t words_number;

	words_number = spi_get_words_number(desc, bytes_number);

	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_TRANSFER(write,
						read,
						words_number - 1));
	return 0;
}

/***************************************************************************//**
* @brief spi_gen_sleep_ns
*******************************************************************************/
#ifdef DUAL_SPI
void spi_gen_sleep_ns(spi_eng_desc *desc,
		      spi_eng_transfer_fifo *xfer,
		      uint32_t sleep_time_ns)
#else
void spi_gen_sleep_ns(spi_desc *desc,
		      spi_eng_transfer_fifo *xfer,
		      uint32_t sleep_time_ns)
#endif
{
	uint32_t sleep_div;

	sleep_div = spi_get_sleep_div(desc, sleep_time_ns); // default 5us
	// Wait for the device to do the conversion
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_SLEEP(sleep_div));
}

/***************************************************************************//**
* @brief check_dma_config
*******************************************************************************/
#ifdef DUAL_SPI
uint8_t spi_check_dma_config(spi_eng_desc *desc,
			     uint8_t rx,
			     uint8_t tx)
#else
uint8_t spi_check_dma_config(spi_desc *desc,
			     uint8_t rx,
			     uint8_t tx)
#endif
{
	if(desc->offload_configured) {
		if(rx && !desc->spi_offload_rx_support_en)
			return 0;

		if(tx && !desc->spi_offload_tx_support_en)
			return 0;
	}
	return 1;
}

/***************************************************************************//**
* @brief spi_eng_add_user_cmd
*******************************************************************************/
#ifdef DUAL_SPI
void spi_eng_add_user_cmd(spi_eng_desc *desc,
			  spi_eng_transfer_fifo *xfer,
			  uint32_t cmd)
#else
void spi_eng_add_user_cmd(spi_desc *desc,
			  spi_eng_transfer_fifo *xfer,
			  uint32_t cmd)
#endif
{
	uint32_t cmd_msk = (0xF << 28);
	uint32_t param_msk = cmd & (~cmd_msk);
	uint32_t command;
	uint16_t param;

	command = (cmd & cmd_msk);
	param = (cmd & param_msk);

	switch(command) {
	case CS_DEASSERT:
		spi_eng_gen_cs(desc, xfer, false); // reset chip select
		break;

	case CS_ASSERT:
		spi_eng_gen_cs(desc, xfer, true); // set chip select
		break;

	case SLEEP_CMD:
		spi_gen_sleep_ns(desc, xfer, param); // Sleep
		break;

	case TRANSFER_R_CMD:
		if(spi_check_dma_config(desc, 1, 0)) {
			spi_eng_gen_transfer(desc, xfer, false, true, param); // read
			desc->rx_length = param;
		} else {
			printf("%s: DMA Rx not configured.\n", __func__);
			desc->rx_length = 0;
		}

		break;

	case TRANSFER_W_CMD:
		if(spi_check_dma_config(desc, 0, 1)) {
			spi_eng_gen_transfer(desc, xfer, true, false, param); // write
			desc->tx_length = param;
		} else {
			printf("%s: DMA Tx not configured.\n", __func__);
			desc->tx_length = 0;
		}
		break;

	case TRANSFER_R_W_CMD:
		if(spi_check_dma_config(desc, 1, 1)) {
			spi_eng_gen_transfer(desc, xfer, true, true, param); // read and write
			desc->tx_length = param;
			desc->rx_length = param;
		} else {
			printf("%s: DMA Rx and Tx not configured.\n", __func__);
			desc->tx_length = 0;
			desc->rx_length = 0;
		}
		break;

	default:
		break;
	}
}

/***************************************************************************//**
* @brief spi_eng_compile_message
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_compile_message(spi_eng_desc *desc,
				spi_eng_msg *msg,
				spi_eng_transfer_fifo *xfer)
#else
int32_t spi_eng_compile_message(spi_desc *desc,
				spi_eng_msg *msg,
				spi_eng_transfer_fifo *xfer)
#endif
{
	uint32_t i, n = 0;

	n = msg->msg_cmd_len;

	// configure prescale
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CLK_DIV,
						desc->clk_div));
	// SPI configuration (3W/CPOL/CPHA)
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CONFIG,
						desc->spi_config));

	// Data transfer length
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
						desc->data_width));


	for (i = 0; i < n; i++)
		spi_eng_add_user_cmd(desc, xfer, msg->spi_msg_cmds[i]);

	// SYNC
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_SYNC(0));

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_transfer_message
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_transfer_message(spi_eng_desc *desc, spi_eng_msg *msg)
#else
int32_t spi_eng_transfer_message(spi_desc *desc, spi_eng_msg *msg)
#endif
{
	spi_eng_transfer_fifo *xfer;
	uint8_t words_number;
	uint32_t size;
	uint32_t i;
	uint32_t data;

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_eng_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return -1;

	xfer->cmd_fifo_len = 0;
	spi_eng_compile_message(desc, msg, xfer);

	// CMD FIFO
	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(desc, SPI_ENGINE_REG_CMD_FIFO, xfer->cmd_fifo[i]);

	/*
	 * On each spi write command, one word is transfered. Typically 16 bits
	 * tx_length = param is deduced from TRANSFER_W(param)
	 */
	words_number = spi_get_words_number(desc, desc->tx_length);
	for(i = 0; i < words_number; i++)
		spi_eng_write(desc, SPI_ENGINE_REG_SDO_DATA_FIFO, msg->tx_buf[i]);

	/*
	 * On each spi read command, one word is transfered. Typically 16 bits.
	 * rx_length = param is deduced from TRANSFER_R(param)
	 */
	words_number = spi_get_words_number(desc, desc->rx_length);
	for(i = 0; i < words_number; i++) {
		spi_eng_read(desc, SPI_ENGINE_REG_SDI_DATA_FIFO, &data);
		msg->rx_buf[i] = data;
	}

	free(xfer);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_offload_load_msg
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_offload_load_msg(spi_eng_desc *desc, spi_eng_msg *msg)
#else
int32_t spi_eng_offload_load_msg(spi_desc *desc, spi_eng_msg *msg)
#endif
{
	uint32_t i, size;
	spi_eng_transfer_fifo *xfer;
	uint8_t words_number;

	desc->rx_dma_startaddr = msg->rx_buf_addr;
	desc->tx_dma_startaddr = msg->tx_buf_addr;

	if(desc->spi_offload_rx_support_en || desc->spi_offload_tx_support_en)
		desc->offload_configured = 1;

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_eng_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return -1;

	xfer->cmd_fifo_len = 0;

	spi_eng_compile_message(desc, msg, xfer);

	// CMD OFFLOAD
	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(desc, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), xfer->cmd_fifo[i]);

	// TX OFFLOAD
	words_number = spi_get_words_number(desc, desc->tx_length);
	for(i = 0; i < words_number; i++)
		spi_eng_write(desc, SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0), msg->tx_buf[i]);

	free(xfer);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_transfer_multiple_msgs
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_transfer_multiple_msgs(spi_eng_desc *desc,
				       uint32_t no_of_messages)
#else
int32_t spi_eng_transfer_multiple_msgs(spi_desc *desc, uint32_t no_of_messages)
#endif
{
	uint8_t alignment;

	if(!desc->offload_configured)
		return -1;

	if (desc->data_width > 16)
		alignment = sizeof(uint32_t);
	else
		alignment = sizeof(uint16_t);

	if(desc->rx_length) {
		desc->rx_length = alignment * no_of_messages;

		spi_eng_dma_write(desc, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(desc, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(desc, DMAC_REG_IRQ_MASK, 0x0);

		spi_eng_dma_write(desc, DMAC_REG_IRQ_PENDING, 0xff);

		spi_eng_dma_write(desc, DMAC_REG_DEST_ADDRESS, desc->rx_dma_startaddr);
		spi_eng_dma_write(desc, DMAC_REG_DEST_STRIDE, 0x0);
		spi_eng_dma_write(desc, DMAC_REG_X_LENGTH, desc->rx_length - 1);
		spi_eng_dma_write(desc, DMAC_REG_Y_LENGTH, 0x0);

		spi_eng_dma_write(desc, DMAC_REG_START_TRANSFER, 0x1);
	}

	if(desc->tx_length) {
		desc->tx_length = alignment * no_of_messages;

		spi_eng_dma_write(desc, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(desc, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(desc, DMAC_REG_IRQ_MASK, 0x0);

		spi_eng_dma_write(desc, DMAC_REG_IRQ_PENDING, 0xff);

		spi_eng_dma_write(desc, DMAC_REG_SRC_ADDRESS, desc->tx_dma_startaddr);
		spi_eng_dma_write(desc, DMAC_REG_SRC_STRIDE, 0x0);
		spi_eng_dma_write(desc, DMAC_REG_X_LENGTH, desc->rx_length - 1);
		spi_eng_dma_write(desc, DMAC_REG_Y_LENGTH, 0x0);
		spi_eng_dma_write(desc, DMAC_REG_FLAGS, 0x1);

		spi_eng_dma_write(desc, DMAC_REG_START_TRANSFER, 0x1);
	}

	usleep(100000);
	// Enable SPI engine
	spi_eng_write(desc, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);

	desc->offload_configured = 0;

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_init
*******************************************************************************/
#ifdef DUAL_SPI
int32_t spi_eng_init(spi_eng_desc **descriptor,
		     spi_eng_init_param init_param)
#else
int32_t spi_init(spi_desc **descriptor,
		 spi_init_param init_param)
#endif
{
#ifdef DUAL_SPI
	spi_eng_desc		*desc;
	desc = (spi_eng_desc *)malloc(sizeof(*desc));
#else
	spi_desc		*desc;
	desc = (spi_desc *)malloc(sizeof(*desc));
#endif
	if (!desc)
		return -1;

	uint32_t        data_width;

	desc->spi_baseaddr = init_param.spi_baseaddr;
	desc->spi_clk_hz = init_param.spi_clk_hz;
	desc->ref_clk_hz = init_param.ref_clk_hz;
	desc->chip_select = init_param.chip_select;
	desc->spi_config = init_param.spi_config;
	desc->rx_length = 0;
	desc->tx_length = 0;

	desc->clk_div = desc->ref_clk_hz / (2 * desc->spi_clk_hz) - 1;

	desc->spi_offload_rx_support_en = init_param.spi_offload_rx_support_en;
	desc->spi_offload_tx_support_en = init_param.spi_offload_tx_support_en;
	desc->spi_offload_tx_dma_baseaddr = init_param.spi_offload_tx_dma_baseaddr;
	desc->spi_offload_rx_dma_baseaddr = init_param.spi_offload_rx_dma_baseaddr;

	// perform a reset
	spi_eng_write(desc, SPI_ENGINE_REG_RESET, 0x01);
	usleep(100000);
	spi_eng_write(desc, SPI_ENGINE_REG_RESET, 0x00);

	spi_eng_read(desc, SPI_ENGINE_REG_DATA_WIDTH, &data_width);
	desc->max_data_width = data_width;
	spi_set_transfer_length(desc, desc->max_data_width);

	*descriptor = desc;
	return 0;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
#ifdef DUAL_SPI
int32_t spi_eng_remove(spi_eng_desc *desc)
#else
int32_t spi_remove(spi_desc *desc)
#endif
{
	free(desc);

	return 0;
}
