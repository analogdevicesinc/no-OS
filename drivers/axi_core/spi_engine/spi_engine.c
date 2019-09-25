/*******************************************************************************
 *   @file   spi_engine.c
 *   @brief  Implementation of SPI Engine Driver.
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>
#include "axi_io.h"
#include "error.h"
#include "spi.h"
#include "spi_engine.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

static inline struct spi_desc_extra *cast_to_extra_desc(void* void_desc)
{
	return (struct spi_desc_extra*)void_desc;
}

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/*******************************************************************************
 *
 * @func	spi_eng_write
 *
 * @brief	Write the axi registers of the SPI Engine.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		reg_addr	- Axi register where the data will be written
 *		reg_data	- Data that will be written
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_write(spi_desc_extra *desc,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
	axi_io_write(desc->spi_baseaddr, reg_addr, reg_data);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_read
 *
 * @brief	Read the axi registers of the SPI Engine.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		reg_addr	- Axi register from where the data will be read
 *		reg_data	- Pointer where the data will be stored
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_read(spi_desc_extra *desc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	axi_io_read(desc->spi_baseaddr, reg_addr, reg_data);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_dma_write
 *
 * @brief	Write the axi registers of the DMA inside the SPI Engine.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		reg_addr	- Axi register where the data will be written
 *		reg_data	- Data that will be written
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_dma_write(spi_desc_extra *desc,
			  uint32_t reg_addr,
			  uint32_t reg_data)
{
	axi_io_write(desc->spi_offload_tx_dma_baseaddr, reg_addr, reg_data);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_dma_read
 *
 * @brief	Read the axi registers of the DMA inside the SPI Engine.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		reg_addr	- Axi register from where the data will be read
 *		reg_data	- Pointer where the data will be stored
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_dma_read(spi_desc_extra *desc,
			 uint32_t reg_addr,
			 uint32_t *reg_data)
{
	axi_io_read(desc->spi_offload_rx_dma_baseaddr, reg_addr, reg_data);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_set_transfer_length
 *
 * @brief	Sets the length of one WORD used in the spi transfer.
 *
 * @params
 *		desc		- Spi engine descriptor
 *		data_length	- The desired data length
 *
 * @return	void
 *
 ******************************************************************************/
void spi_set_transfer_length(spi_desc *desc, uint8_t data_length)
{
	spi_desc_extra	*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	if (data_length > desc_extra->max_data_width) {
		desc_extra->data_width = desc_extra->max_data_width;
	} else {
		desc_extra->data_width = data_length;
	}
}

/*******************************************************************************
 *
 * @func	spi_get_words_number
 *
 * @brief	Used to convert bytes in spi engine WORDS based on the system
 *		configuration.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		bytes_number	- The number of bytes to be cnoverted
 *
 * @return	words_number	- A number of words in which bytes_number can
 *				be grouped
 *
 ******************************************************************************/
uint8_t spi_get_words_number(spi_desc_extra *desc, uint8_t bytes_number)
{
	uint8_t xfer_word_len;
	uint8_t words_number;

	xfer_word_len = desc->data_width / 8;
	words_number = bytes_number / xfer_word_len;

	if ((bytes_number % xfer_word_len) != 0)
		words_number++;

	return words_number;
}

/*******************************************************************************
 *
 * @func	spi_get_word_lenght
 *
 * @brief	Get the word lenght in bytes.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *
 * @return			- Number of bytes that fit in one word
 *
 ******************************************************************************/
uint8_t spi_get_word_lenght(spi_desc_extra *desc)
{
	return desc->data_width / 8;
}

/*******************************************************************************
 *
 * @func	spi_check_dma_config
 *
 * @brief	Check if both tx and rx channels of the DMA are enabled while
 *		offload mode is cnofigured.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		rx		- Offload flag for rx channel
 *		tx		- Offload flag for tx channel
 *
 * @return			- SUCCESS if both channels are configured
 *				- FAILURE if contrary
 *
 ******************************************************************************/
int32_t spi_check_dma_config(spi_desc_extra *desc,
			     uint8_t rx,
			     uint8_t tx)
{
	if(desc->offload_configured) {
		if(rx && !desc->spi_offload_rx_support_en)
			return FAILURE;

		if(tx && !desc->spi_offload_tx_support_en)
			return FAILURE;
	}

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_get_sleep_div
 *
 * @brief	This function computes the prescaler used to set the sleep
 *		period.
 *
 * @params
 *		desc		- Spi engine descriptor
 *		sleep_time_ns	- Offload flag for rx channel
 *		sleep_div	- Pointer where the computed value
 * 				will be written
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_get_sleep_div(spi_desc *desc,
			  uint32_t sleep_time_ns,
			  uint32_t *sleep_div)
{
	spi_desc_extra	*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	/*
	 * Engine Wiki:
	 *
	 * The frequency of the SCLK signal is derived from the
	 * module clock frequency using the following formula:
	 * f_sclk = f_clk / ((div + 1) * 2)
	 */

	*sleep_div = (desc->max_speed_hz / 1000000 * sleep_time_ns / 1000) /
		     ((desc_extra->clk_div + 1) * 2) - 1;

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_program_add_cmd
 *
 * @brief	Add a command to the CMD_FIFO buffer
 *
 * @params
 *		xfer		- Fifo transfer structure
 *		cmd		- The command that will be added to the buffer
 *
 * @return	void
 *
 ******************************************************************************/
void spi_eng_program_add_cmd(spi_eng_transfer_fifo *xfer,
			     uint16_t cmd)
{
	xfer->cmd_fifo[xfer->cmd_fifo_len] = cmd;
	xfer->cmd_fifo_len++;
}

/*******************************************************************************
 *
 * @func	spi_eng_gen_transfer
 *
 * @brief	Initiate an spi transfer.
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *		xfer		- Fifo transfer structure
 *		write		- Write transaction flag
 *		read		- Read transaction flag
 *		bytes_number	- Number of bytes to transfer
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_gen_transfer(spi_desc_extra *desc,
			     spi_eng_transfer_fifo *xfer,
			     bool write,
			     bool read,
			     uint8_t bytes_number)
{
	uint8_t words_number;

	words_number = spi_get_words_number(desc, bytes_number) - 1;

	/*
	 * Engine Wiki:
	 *
	 * The words number is zero based
	 */

	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_TRANSFER(write, read, words_number));

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_gen_cs
 *
 * @brief	Change the state of the chip select port
 *
 * @params
 *		desc		- Spi engine descriptor
 *		xfer		- Fifo transfer structure
 *		assert		- Switch HIGH/LOW the chip select
 *
 * @return	void
 *
 ******************************************************************************/
void spi_eng_gen_cs(spi_desc *desc,
		    spi_eng_transfer_fifo *xfer,
		    bool assert)
{
	uint8_t		mask = 0xff;
	spi_desc_extra	*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	/* Switch the state only of the selected chip select */
	if (!assert)
		mask ^= BIT(desc->chip_select);

	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_ASSERT(desc_extra->cs_delay, mask));
}

/*******************************************************************************
 *
 * @func	spi_gen_sleep_ns
 *
 * @brief	Add a delay bewtheen the engine commands
 *
 * @params
 *		desc		- Spi engine descriptor
 *		xfer		- Fifo transfer structure
 *		sleep_time_ns	- Number of nanoseconds
 *
 * @return	void
 *
 ******************************************************************************/
void spi_gen_sleep_ns(spi_desc *desc,
		      spi_eng_transfer_fifo *xfer,
		      uint32_t sleep_time_ns)
{
	uint32_t sleep_div;

	spi_get_sleep_div(desc, sleep_time_ns, &sleep_div);
	/* Wait for the device to do the conversion */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_SLEEP(sleep_div));
}

/*******************************************************************************
 *
 * @func	spi_eng_add_user_cmd
 *
 * @brief	Spi engine command interpreter
 *
 * @params
 *		desc		- Spi engine descriptor
 *		xfer		- Fifo transfer structure
 *		cmd		- Command to send to the engine
 *
 * @return	void
 *
 ******************************************************************************/
void spi_eng_add_user_cmd(spi_desc *desc,
			  spi_eng_transfer_fifo *xfer,
			  uint32_t cmd)
{
	uint32_t	cmd_msk = (0xF << 28);
	uint32_t	param_msk = cmd & (~cmd_msk);
	uint32_t	command;
	uint16_t	param;
	spi_desc_extra	*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	command = (cmd & cmd_msk);
	param = (cmd & param_msk);

	switch(command) {
	case CS_DEASSERT:
		/* Reet the CS */
		spi_eng_gen_cs(desc, xfer, false);
		break;

	case CS_ASSERT:
		/* Set the CS */
		spi_eng_gen_cs(desc, xfer, true);
		break;

	case SLEEP_CMD:
		/* Sleep */
		spi_gen_sleep_ns(desc, xfer, param);
		break;

	case TRANSFER_R_CMD:
		if(spi_check_dma_config(desc_extra, 1, 0) == SUCCESS) {
			/* Read */
			spi_eng_gen_transfer(desc_extra,
					     xfer, false, true, param);
			desc_extra->rx_length = param;
		} else {
			printf("%s: DMA Rx not configured.\n", __func__);
			desc_extra->rx_length = 0;
		}

		break;

	case TRANSFER_W_CMD:
		if(spi_check_dma_config(desc_extra, 0, 1) == SUCCESS) {
			/* Write */
			spi_eng_gen_transfer(desc_extra,
					     xfer, true, false, param);
			desc_extra->tx_length = param;
		} else {
			printf("%s: DMA Tx not configured.\n", __func__);
			desc_extra->tx_length = 0;
		}
		break;

	case TRANSFER_R_W_CMD:
		if(spi_check_dma_config(desc_extra, 1, 1) == SUCCESS) {
			/* Read and write */
			spi_eng_gen_transfer(desc_extra,
					     xfer, true, true, param);
			desc_extra->tx_length = param;
			desc_extra->rx_length = param;
		} else {
			printf("%s: DMA Rx and Tx not configured.\n", __func__);
			desc_extra->tx_length = 0;
			desc_extra->rx_length = 0;
		}
		break;

	default:
		break;
	}
}

/*******************************************************************************
 *
 * @func	spi_eng_compile_message
 *
 * @brief	Prepare the command queue before sending it to the engine
 *
 * @params
 *		desc		- Spi engine descriptor
 *		msg		- Structure used to store the messages
 *		xfer		- Fifo transfer structure
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_eng_compile_message(spi_desc *desc,
				spi_eng_msg *msg,
				spi_eng_transfer_fifo *xfer)
{
	uint32_t	i;
	uint32_t	n;
	spi_desc_extra	*desc_extra;

	desc_extra = cast_to_extra_desc(desc->extra);

	n = msg->msg_cmd_len;

	/* Configure the prescaler */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CLK_DIV,
						desc_extra->clk_div));
	/*
	 * Configure the spi mode :
	 *	- 3 wire
	 *	- CPOL
	 *	- CPHA
	 */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CONFIG,
						desc->mode));

	/* Set the data transfer length */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
						desc_extra->data_width));

	/* Add a sync command to signal that the transfer has started */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_SYNC(SPI_ENGINE_SYNC_TRANSFER_BEGIN));

	/* Add the rest of the commands */
	for (i = 0; i < n; i++)
		spi_eng_add_user_cmd(desc, xfer, msg->spi_msg_cmds[i]);

	/* Add a sync command to signal that the transfer has ended */
	spi_eng_program_add_cmd(xfer,
				SPI_ENGINE_CMD_SYNC(SPI_ENGINE_SYNC_TRANSFER_END));

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_eng_transfer_message
 *
 * @brief	Initiate a spi transfer
 *
 * @params
 *		desc		- Spi engine descriptor
 *		msg		- Structure used to store the messages
 *
 * @return			- SUCCESS if the transfer finished
 *				- FAILURE if the memory allocation failed
 *
 ******************************************************************************/
int32_t spi_eng_transfer_message(spi_desc *desc, spi_eng_msg *msg)
{
	uint32_t	size;
	uint32_t	i;
	uint32_t	data;
	uint32_t	sync_id;
	spi_desc_extra	*desc_extra;
	spi_eng_transfer_fifo *xfer;

	desc_extra = cast_to_extra_desc(desc->extra);

	size = sizeof(*xfer->cmd_fifo) * (msg->msg_cmd_len + 3);

	xfer = (spi_eng_transfer_fifo *)malloc(sizeof(*xfer) + size);
	if (!xfer)
		return FAILURE;

	xfer->cmd_fifo_len = 0;

	spi_eng_compile_message(desc, msg, xfer);

	/* Write the command fifo buffer */
	for (i = 0; i < xfer->cmd_fifo_len; i++)
		spi_eng_write(desc_extra,
			      SPI_ENGINE_REG_CMD_FIFO, xfer->cmd_fifo[i]);

	/* Write a number of tx_length WORDS on the SDO line */
	for(i = 0; i < desc_extra->tx_length; i++)
		spi_eng_write(desc_extra,
			      SPI_ENGINE_REG_SDO_DATA_FIFO, msg->tx_buf[i]);

	/* Wait for all the transactions to finish */
	do {
		spi_eng_read(desc_extra, SPI_ENGINE_REG_SYNC_ID, &sync_id);
	}
	/* Wait for the end sync signal */
	while(sync_id != SPI_ENGINE_SYNC_TRANSFER_END);

	/* Read a number of rx_length WORDS from the SDI line and store them */
	for(i = 0; i < desc_extra->rx_length; i++) {
		spi_eng_read(desc_extra,
			     SPI_ENGINE_REG_SDI_DATA_FIFO, &data);
		msg->rx_buf[i] = data;
	}

	free(xfer);

	return SUCCESS;
}

/*******************************************************************************
 *
 * @func	spi_init
 *
 * @brief	Initialize the spi engine
 *
 * @params
 *		desc		- Spi engine descriptor
 *		param		- Structure containing the spi init parameters
 *
 * @return			- SUCCESS if the transfer finished
 *				- FAILURE if the memory allocation failed
 *
 ******************************************************************************/
int32_t spi_init(struct spi_desc **desc,
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
 * @func	spi_write_and_read
 *
 * @brief	Write/read on the spi interface
 *
 * @params
 *		desc		- Spi engine descriptor
 *		data		- Pointer to data buffer
 *		bytes_number	- Number of bytes to transfer
 *

 * @return			- SUCCESS if the transfer finished
 *				- FAILURE if the memory allocation
 *				or transfer failed
 *
 ******************************************************************************/
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	uint8_t 	i;
	uint8_t 	xfer_word_len;
	uint8_t 	xfer_words_number;
	uint32_t	spi_eng_msg_cmds[4];
	int32_t 	ret;
	spi_eng_msg	*msg;
	spi_desc_extra	*desc_extra;

	desc_extra = desc->extra;

	xfer_words_number = spi_get_words_number(desc_extra, bytes_number);

	/* Make sure the CS is HIGH before starting a transaction */
	spi_eng_msg_cmds[0] = CS_ASSERT;
	spi_eng_msg_cmds[1] = CS_DEASSERT;
	spi_eng_msg_cmds[2] = TRANSFER_BYTES_R_W(xfer_words_number);
	spi_eng_msg_cmds[3] = CS_ASSERT;

	msg = (spi_eng_msg *)malloc(sizeof(*msg));
	if (!msg)
		return FAILURE;

	msg->spi_msg_cmds = spi_eng_msg_cmds;
	msg->msg_cmd_len = ARRAY_SIZE(spi_eng_msg_cmds);

	msg->tx_buf =(uint32_t*)malloc(xfer_words_number * sizeof(msg->tx_buf[0]));
	msg->rx_buf =(uint32_t*)malloc(xfer_words_number * sizeof(msg->rx_buf[0]));

	/* Init the rx and tx buffers with 0s */
	for (i = 0; i < xfer_words_number; i++) {
		msg->tx_buf[i] = 0;
		msg->rx_buf[i] = 0;
	}

	xfer_word_len = spi_get_word_lenght(desc_extra);

	/* Pack the bytes into engine WORDS */
	for (i = 0; i < bytes_number; i++)
		msg->tx_buf[i / xfer_word_len] |=
			data[i] << (desc_extra->data_width -
				    (i % xfer_word_len + 1) * 8);

	ret = spi_eng_transfer_message(desc, msg);

	/* Skip the first byte ( dummy read byte ) */
	for (i = 1; i < bytes_number; i++)
		data[i - 1] = msg->rx_buf[(i) / xfer_word_len] >>
			      (desc_extra->data_width - ((i) % xfer_word_len + 1) * 8);

	free(msg->tx_buf);
	free(msg->rx_buf);
	free(msg);

	return ret;
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

/*******************************************************************************
 *
 * @func	spi_remove
 *
 * @brief	Free the resources allocated by spi_init().
 *
 * @params
 *		desc		- Spi engine descriptor extra parameters
 *
 * @return			- This function allways returns SUCCESS
 *
 ******************************************************************************/
int32_t spi_remove(spi_desc *desc)
{
	free(desc->extra);
	free(desc);

	return SUCCESS;
}
