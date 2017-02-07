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
#include "platform_drivers.h"
#include "spi_engine.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/***************************************************************************//**
* @brief spi_eng_write
*******************************************************************************/
int32_t spi_eng_write(spi_dev *dev,
				  	  uint32_t reg_addr,
					  uint32_t ui32data)
{
	Xil_Out32((dev->spi_baseaddr + reg_addr), ui32data);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_read
*******************************************************************************/
int32_t spi_eng_read(spi_dev *dev,
					 uint32_t reg_addr,
					 uint32_t *reg_data)
{
	*reg_data = Xil_In32((dev->spi_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_dma_write
*******************************************************************************/
int32_t spi_eng_dma_write(spi_dev *dev,
					  	  uint32_t reg_addr,
						  uint32_t reg_data)
{
	Xil_Out32((dev->spi_offload_tx_dma_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_dma_read
*******************************************************************************/
int32_t spi_eng_dma_read(spi_dev *dev,
					 	 uint32_t reg_addr,
						 uint32_t *reg_data)
{
	*reg_data = Xil_In32((dev->spi_offload_rx_dma_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_program_add_cmd
*******************************************************************************/
void spi_eng_program_add_cmd(spi_transfer_fifo *xfer,
						 	 uint16_t cmd)
{
	xfer->cmd_fifo[xfer->cmd_fifo_len] = cmd;
	xfer->cmd_fifo_len++;
}

/***************************************************************************//**
* @brief get_sleep_div
*******************************************************************************/
uint32_t spi_get_sleep_div(spi_dev *dev,
					   	   uint32_t sleep_time_ns)
{

	uint32_t sleep_div = 0;

	sleep_div = (dev->ref_clk_hz / 1000000 * sleep_time_ns / 1000) /
			((dev->clk_div + 1) * 2) - 1;

	return sleep_div;
}

/***************************************************************************//**
* @brief get_words_number
*******************************************************************************/
uint8_t spi_get_words_number(spi_dev *dev, uint8_t bytes_number)
{
	uint8_t words_number = 0;
	uint8_t	data_width	= dev->data_width / 8;
	(bytes_number > 1) ? (words_number = bytes_number / data_width) : (words_number = 1);

	return words_number;
}

/***************************************************************************//**
* @brief spi_eng_gen_cs
*******************************************************************************/
void spi_eng_gen_cs(spi_dev *dev,
					spi_transfer_fifo *xfer,
					bool assert)
{
	uint8_t mask = 0xff;

	if (!assert)
		mask ^= BIT(dev->chip_select);

	spi_eng_program_add_cmd(xfer, SPI_ENGINE_CMD_ASSERT(1, mask));
}

/***************************************************************************//**
* @brief spi_eng_gen_transfer
*******************************************************************************/
int32_t spi_eng_gen_transfer(spi_dev *dev,
						 	 spi_transfer_fifo *xfer,
							 bool write,
							 bool read,
							 uint8_t bytes_number)
{
	uint8_t words_number;

	words_number = spi_get_words_number(dev, bytes_number);

	spi_eng_program_add_cmd(xfer,
			SPI_ENGINE_CMD_TRANSFER(write,	// write
									read,	// read
									words_number - 1));	// words_number);
	return 0;
}

/***************************************************************************//**
* @brief spi_gen_sleep_ns
*******************************************************************************/
void spi_gen_sleep_ns(spi_dev *dev,
					  spi_transfer_fifo *xfer,
					  uint32_t sleep_time_ns)
{
	uint32_t sleep_div;

	sleep_div = spi_get_sleep_div(dev, sleep_time_ns); // default 5us
	// Wait for the device to do the conversion
	spi_eng_program_add_cmd(xfer,
			SPI_ENGINE_CMD_SLEEP(sleep_div));
}

/***************************************************************************//**
* @brief check_dma_config
*******************************************************************************/
uint8_t spi_check_dma_config(spi_dev *dev,
							 uint8_t rx,
							 uint8_t tx)
{
	if(dev->offload_configured) {
		if(rx && !dev->spi_offload_rx_support_en)
			return 0;

		if(tx && !dev->spi_offload_tx_support_en)
			return 0;
	}
	return 1;
}

/***************************************************************************//**
* @brief spi_eng_add_user_cmd
*******************************************************************************/
void spi_eng_add_user_cmd(spi_dev *dev,
						  spi_transfer_fifo *xfer,
						  uint32_t cmd)
{
	uint32_t cmd_msk = (0xF << 28);
	uint32_t param_msk = cmd & (~cmd_msk);
	uint32_t command;
	uint16_t param;

	command = (cmd & cmd_msk);
	param = (cmd & param_msk);

	switch(command) {
	case CS_DEASSERT:
		spi_eng_gen_cs(dev, xfer, false); // reset chip select
		break;

	case CS_ASSERT:
		spi_eng_gen_cs(dev, xfer, true); // set chip select
		break;

	case SLEEP_CMD:
		spi_gen_sleep_ns(dev, xfer, param); // Sleep
		break;

	case TRANSFER_R_CMD:
		if(spi_check_dma_config(dev, 1, 0)) {
			spi_eng_gen_transfer(dev, xfer, false, true, param); // read
			dev->rx_length = param;
		}
		else {
			printf("%s: DMA Rx not configured.\n", __func__);
			dev->rx_length = 0;
		}

		break;

	case TRANSFER_W_CMD:
		if(spi_check_dma_config(dev, 0, 1)) {
			spi_eng_gen_transfer(dev, xfer, true, false, param); // write
			dev->tx_length = param;
		}
		else {
			printf("%s: DMA Tx not configured.\n", __func__);
			dev->tx_length = 0;
		}
		break;

	case TRANSFER_R_W_CMD:
		if(spi_check_dma_config(dev, 1, 1)) {
			spi_eng_gen_transfer(dev, xfer, true, true, param); // read and write
			dev->tx_length = param;
			dev->rx_length = param;
		}
		else {
			printf("%s: DMA Rx and Tx not configured.\n", __func__);
			dev->tx_length = 0;
			dev->rx_length = 0;
		}
		break;

	default:
		break;
	}
}

/***************************************************************************//**
* @brief spi_eng_compile_message
*******************************************************************************/
int32_t spi_eng_compile_message(spi_dev *dev,
								spi_msg *msg,
								spi_transfer_fifo *xfer)
{
	uint32_t i, n = 0;

	n = msg->msg_cmd_len;

	// configure prescale
	spi_eng_program_add_cmd(xfer,
			SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CLK_DIV,
								  dev->clk_div));
	// SPI configuration (3W/CPOL/CPHA)
	spi_eng_program_add_cmd(xfer,
			SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CONFIG,
								dev->spi_config));

	for (i = 0; i < n; i++) {
		spi_eng_add_user_cmd(dev, xfer, msg->spi_msg_cmds[i]);
	}

	// SYNC
	spi_eng_program_add_cmd(xfer,
			SPI_ENGINE_CMD_SYNC(0));

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_transfer_message
*******************************************************************************/
int32_t spi_eng_transfer_message(spi_dev *dev, spi_msg *msg)
{
	uint32_t		  i, size;
	spi_transfer_fifo *xfer;
	uint8_t words_number;
	uint32_t data;
	uint8_t *reg_data;// = (uint8_t*)dev->rx_dma_startaddr;

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return -1;

	xfer->cmd_fifo_len = 0;
	spi_eng_compile_message(dev, msg, xfer);

	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(dev, SPI_ENGINE_REG_CMD_FIFO, xfer->cmd_fifo[i]);

	words_number = spi_get_words_number(dev, dev->rx_length);

	for(i = 0; i < words_number; i++) {
		spi_eng_read (dev, SPI_ENGINE_REG_SDI_DATA_FIFO, &data);

		*((uint8_t *)reg_data) = (uint8_t)((data & 0xFF00) >> 8); // MSB
        reg_data += sizeof(uint8_t);
        printf("%x\r\n", *reg_data);

        *((uint8_t *)reg_data) = (uint8_t)(data & 0xFF); // LSB
        reg_data += sizeof(uint8_t);
        printf("%x\r\n", *reg_data);
	}
	free(xfer);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_offload_load_msg
*******************************************************************************/
int32_t spi_eng_offload_load_msg(spi_dev *dev, spi_msg *msg)
{
	uint32_t i, size;
	spi_transfer_fifo *xfer;

	dev->rx_dma_startaddr = msg->rx_buf_addr;
	dev->tx_dma_startaddr = msg->tx_buf_addr;

	if(dev->spi_offload_rx_support_en || dev->spi_offload_tx_support_en)
		dev->offload_configured = 1;

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return -1;

	xfer->cmd_fifo_len = 0;

	spi_eng_compile_message(dev, msg, xfer);

	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(dev, SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), xfer->cmd_fifo[i]);

    free(xfer);

	return 0;
}

/***************************************************************************//**
* @brief spi_eng_transfer_multiple_msgs
*******************************************************************************/
int32_t spi_eng_transfer_multiple_msgs(spi_dev *dev, uint8_t no_of_messages)
{
	if(!dev->offload_configured)
		return -1;

	if(dev->rx_length) {
		dev->rx_length *= no_of_messages;

		spi_eng_dma_write(dev, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(dev, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(dev, DMAC_REG_IRQ_MASK, 0x0);

		spi_eng_dma_write(dev, DMAC_REG_IRQ_PENDING, 0xff);

		spi_eng_dma_write(dev, DMAC_REG_DEST_ADDRESS, dev->rx_dma_startaddr);
		spi_eng_dma_write(dev, DMAC_REG_DEST_STRIDE, 0x0);
		spi_eng_dma_write(dev, DMAC_REG_X_LENGTH, dev->rx_length - 1);
		spi_eng_dma_write(dev, DMAC_REG_Y_LENGTH, 0x0);

		spi_eng_dma_write(dev, DMAC_REG_START_TRANSFER, 0x1);
	}

	if(dev->tx_length) {
		dev->tx_length *= no_of_messages;

		spi_eng_dma_write(dev, DMAC_REG_CTRL, 0x0);
		spi_eng_dma_write(dev, DMAC_REG_CTRL, DMAC_CTRL_ENABLE);
		spi_eng_dma_write(dev, DMAC_REG_IRQ_MASK, 0x0);

		spi_eng_dma_write(dev, DMAC_REG_IRQ_PENDING, 0xff);

		spi_eng_dma_write(dev, DMAC_REG_SRC_ADDRESS, dev->tx_dma_startaddr);
		spi_eng_dma_write(dev, DMAC_REG_SRC_STRIDE, 0x0);
		spi_eng_dma_write(dev, DMAC_REG_X_LENGTH, dev->tx_length - 1);
		spi_eng_dma_write(dev, DMAC_REG_Y_LENGTH, 0x0);
		spi_eng_dma_write(dev, DMAC_REG_FLAGS, 0x1);

		spi_eng_dma_write(dev, DMAC_REG_START_TRANSFER, 0x1);
	}

	mdelay(100);
    spi_eng_write(dev, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);  // Enable SPI engine

	dev->offload_configured = 0;

    return 0;
}

/***************************************************************************//**
* @brief spi_eng_setup
*******************************************************************************/
int32_t spi_eng_setup(spi_dev **device,
				  	  spi_init_param init_param)
{
	spi_dev		*dev;

	dev = (spi_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->spi_baseaddr = init_param.spi_baseaddr;
	dev->spi_clk_hz = init_param.spi_clk_hz;
	dev->ref_clk_hz = init_param.ref_clk_hz;
	dev->chip_select = init_param.chip_select;
	dev->spi_config = init_param.spi_config;
	dev->data_width	= 16; // 2 bytes

	dev->clk_div = dev->ref_clk_hz / (2 * dev->spi_clk_hz) - 1;

	dev->spi_offload_rx_support_en = init_param.spi_offload_rx_support_en;
	dev->spi_offload_tx_support_en = init_param.spi_offload_tx_support_en;
	dev->spi_offload_tx_dma_baseaddr = init_param.spi_offload_tx_dma_baseaddr;
	dev->spi_offload_rx_dma_baseaddr = init_param.spi_offload_rx_dma_baseaddr;

	// perform a reset
	spi_eng_write(dev, SPI_ENGINE_REG_RESET, 0x01);
	usleep(100000);
	spi_eng_write(dev, SPI_ENGINE_REG_RESET, 0x00);

    *device = dev;
	return 0;
}
