/*******************************************************************************
 *   @file   spi_engine.c
 *   @brief  Core implementation of the SPI Engine Driver.
 *   @author Sergiu Cuciurean (sergiu.cuciurean@analog.com)
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

/* In debug mode the printf function used in displaying the messages is causing
significant delays */
//#define DEBUG_LEVEL 2

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>

#include "axi_dmac.h"
#include "axi_io.h"
#include "error.h"
#include "spi_engine.h"

/**
 * @brief Spi engine platform specific SPI platform ops structure
 */
const struct spi_platform_ops spi_eng_platform_ops = {
	.spi_ops_init = &spi_engine_init,
	.spi_ops_write_and_read = &spi_engine_write_and_read,
	.spi_ops_remove = &spi_engine_remove
};

/******************************************************************************/
/***************************** Static variables *******************************/
/******************************************************************************/

/**
 * @brief Static variable used to sync transfers
 *
 */
static uint8_t _sync_id = 0x01;

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Write SPI Engine's axi registers
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param reg_addr The address of the SPI Engine's axi register where the data
 * 	will be written
 * @param reg_data Data that will be written
 * @return int32_t This function allways returns SUCCESS
 */
static int32_t spi_engine_write(struct spi_engine_desc *desc,
				uint32_t reg_addr,
				uint32_t reg_data)
{
	axi_io_write(desc->spi_engine_baseaddr, reg_addr, reg_data);

	return SUCCESS;
}

/**
 * @brief Read SPI Engine's axi registers
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param reg_addr The address of the SPI Engine's axi register from where the
 * 	data where the data will be read
 * @param reg_data Pointer where the read that will be stored
 * @return int32_t This function allways returns SUCCESS
 */
static int32_t spi_engine_read(struct spi_engine_desc *desc,
			       uint32_t reg_addr,
			       uint32_t *reg_data)
{
	axi_io_read(desc->spi_engine_baseaddr, reg_addr, reg_data);
	return SUCCESS;
}

/**
 * @brief Set width of the transfered word over SPI
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param data_wdith The desired data width
 * 	The supported values are:
 * 		- 8
 * 		- 16
 * 		- 24
 * 		- 32
 */
static int32_t spi_engine_set_transfer_width(struct spi_desc *desc,
		uint8_t data_wdith)
{
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	if (data_wdith > desc_extra->max_data_width)
		desc_extra->data_width = desc_extra->max_data_width;
	else
		desc_extra->data_width = data_wdith;

	return SUCCESS;
}

/**
 * @brief Set the number of words transfered in a single transaction
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param bytes_number The number of bytes to be converted
 * @return uint8_t A number of words in which bytes_number can be grouped
 */
static uint8_t spi_get_words_number(struct spi_engine_desc *desc,
				    uint8_t bytes_number)
{
	uint8_t xfer_word_len;
	uint8_t words_number;

	xfer_word_len = desc->data_width / 8;
	words_number = bytes_number / xfer_word_len;

	if ((bytes_number % xfer_word_len) != 0)
		words_number++;

	return words_number;
}

/**
 * @brief Get the word length in bytes
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @return uint8_t Number of bytes that fit in one word
 */
static uint8_t spi_get_word_lenght(struct spi_engine_desc *desc)
{
	uint8_t word_lenght;

	word_lenght = desc->data_width / 8;

	return word_lenght;
}

/**
 * @brief Compute the prescaler used to set the sleep period.
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param sleep_time_ns The amount of time where the transfer hangs
 * @param sleep_div Clock prescaler
 * @return int32_t This function allways returns SUCCESS
 */
static int32_t spi_get_sleep_div(struct spi_desc *desc,
				 uint32_t sleep_time_ns,
				 uint32_t *sleep_div)
{
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	/*
	 * Engine Wiki:
	 *
	 * The frequency of the SCLK signal is derived from the
	 * module clock frequency using the following formula:
	 * f_sclk = f_clk / ((div + 1) * 2)
	 */

	*sleep_div = (desc->max_speed_hz / 1000000 * sleep_time_ns / 1000) /
		     ((eng_desc->clk_div + 1) * 2) - 1;

	return SUCCESS;
}

/**
 * @brief Create a new commands queue used in a spi transfer
 *
 * @param fifo Command buffer, usualy used in fifo mode
 * @param cmd Command with wich the buffer will be initiated
 * @return int32_t Failure if the memory allocation failed
 */
static int32_t spi_engine_queue_new_cmd(struct spi_engine_cmd_queue **fifo,
					uint32_t cmd)
{
	struct spi_engine_cmd_queue *local_fifo;

	local_fifo = (spi_engine_cmd_queue*)malloc(sizeof(*local_fifo));

	if(!local_fifo)
		return FAILURE;

	local_fifo->cmd = cmd;
	local_fifo->next = NULL;
	*fifo = local_fifo;

	return SUCCESS;
}

/**
 * @brief Add a command at the end of an existing queue
 *
 * @param fifo Command buffer, usualy used in fifo mode
 * @param cmd Command to be added
 */
static void spi_engine_queue_add_cmd(struct spi_engine_cmd_queue **fifo,
				     uint32_t cmd)
{
	struct spi_engine_cmd_queue *to_add = NULL;
	struct spi_engine_cmd_queue *local_fifo;

	local_fifo = *fifo;
	while (local_fifo->next != NULL) {
		// Get the last element
		local_fifo = local_fifo->next;
	}

	// Create a new element
	spi_engine_queue_new_cmd(&to_add, cmd);
	// Add as the next element
	local_fifo->next = to_add;
}

/**
 * @brief Add a command at the beginning of an existing queue
 *
 * @param fifo Command buffer, usualy used in fifo mode
 * @param cmd Command to be added
 */
static void spi_engine_queue_append_cmd(struct spi_engine_cmd_queue **fifo,
					uint32_t cmd)
{
	struct spi_engine_cmd_queue *to_add = NULL;

	// Create a new element
	spi_engine_queue_new_cmd(&to_add, cmd);
	// Interchange the addresses
	to_add->next = *fifo;
	*fifo = to_add;
}


/**
 * @brief Get the command from the beginning of the queue
 *
 * @param fifo Command buffer, usualy used in fifo mode
 * @param cmd Value of the command that was extracted
 * @return int32_t Return FAILURE if the queue is empty
 */
static int32_t spi_engine_queue_get_cmd(struct spi_engine_cmd_queue **fifo,
					uint32_t *cmd)
{
	struct spi_engine_cmd_queue *local_fifo;

	if (!*fifo)
		return FAILURE;

	local_fifo = *fifo;
	*cmd = local_fifo->cmd;
	if ((*fifo)->next) {
		*fifo = local_fifo->next;
		free(local_fifo);
	} else {
		free(*fifo);
		*fifo = NULL;
	}

	return SUCCESS;
}

/**
 * @brief Free the memory allocated by a queue
 *
 * @param fifo The queue that needs it's memory freed
 * @return int32_t This function allways return SUCCESS
 */
static int32_t spi_engine_queue_free(struct spi_engine_cmd_queue **fifo)
{
	if(*fifo && (*fifo)->next)
		spi_engine_queue_free(&(*fifo)->next);
	if((*fifo) != NULL) {
		free(*fifo);
		*fifo = NULL;
	}

	return SUCCESS;
}

/**
 * @brief Write the SPI engine's command fifo
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param cmd The command that will be written
 * @return int32_t Returns FAILURE if the the axi transfer failed
 */
static int32_t spi_engine_write_cmd_reg(struct spi_engine_desc *desc,
					uint32_t cmd)
{
	int32_t ret;

	/* Check if offload is enabled */
	if(desc->offload_config & (OFFLOAD_TX_EN | OFFLOAD_RX_EN)) {
		ret = spi_engine_write(desc,
				       SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0),
				       cmd);

	} else {
		ret = spi_engine_write(desc,
				       SPI_ENGINE_REG_CMD_FIFO,
				       cmd);
	}

	return ret;
}

/**
 * @brief Write a transfer command to the SPI engine's command fifo
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param read_write Read/Write operation flag
 * @param bytes_number Number of bytes to transfer
 * @return int32_t This function allways returns SUCCESS
 */
static int32_t spi_engine_transfer(struct spi_engine_desc *desc,
				   uint8_t read_write,
				   uint8_t bytes_number)
{
	uint8_t words_number;

	words_number = spi_get_words_number(desc, bytes_number);

	desc->offload_tx_len += words_number;

	/*
	 * Engine Wiki:
	 *
	 * https://wiki.analog.com/resources/fpga/peripherals/spi_engine
	 *
	 * The words number is zero based
	 */

	spi_engine_write_cmd_reg(desc,
				 SPI_ENGINE_CMD_TRANSFER(read_write,
						 words_number  - 1));

	return SUCCESS;
}

/**
 * @brief Change the state of the chip select port
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param assert Chip select state.
 * 		 The supported values are :
 * 			-true (HIGH)
 * 			-false (LOW)
 */
static void spi_engine_set_cs(struct spi_desc *desc,
			      bool assert)
{
	uint8_t			mask;
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	mask = 0xFF;
	/* Switch the state only of the selected chip select */
	if (!assert)
		mask ^= BIT(desc->chip_select);

	spi_engine_write_cmd_reg(eng_desc,
				 SPI_ENGINE_CMD_ASSERT(eng_desc->cs_delay,
						 mask));
}

/**
 * @brief Add a delay bewtheen the engine commands
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param sleep_time_ns Number of nanoseconds to sleep between commands
 */
static void spi_gen_sleep_ns(struct spi_desc *desc,
			     uint32_t sleep_time_ns)
{
	uint32_t 		sleep_div;
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	spi_get_sleep_div(desc, sleep_time_ns, &sleep_div);

	spi_engine_write_cmd_reg(eng_desc, SPI_ENGINE_CMD_SLEEP(sleep_div));
}

/**
 * @brief Spi engine command interpreter
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param cmd Command to send to the engine
 * @return int32_t - SUCCESS if the command is transfered
 *		   - FAILURE if the command format is invalid
 */
static int32_t spi_engine_write_cmd(struct spi_desc *desc,
				    uint32_t cmd)
{
	uint8_t				engine_command;
	uint8_t				parameter;
	uint8_t				modifier;
	struct spi_engine_desc		*desc_extra;

	desc_extra = desc->extra;

	engine_command = (cmd >> 12) & 0x0F;
	modifier = (cmd >> 8) & 0x0F;
	parameter = cmd & 0xFF;

	switch(engine_command) {
	case SPI_ENGINE_INST_TRANSFER:
		spi_engine_transfer(desc_extra, modifier, parameter);
		break;

	case SPI_ENGINE_INST_ASSERT:
		if(parameter == 0xFF) {
			/* Set the CS HIGH */
			spi_engine_set_cs(desc, true);
		} else if(parameter == 0x00) {
			/* Set the CS LOW */
			spi_engine_set_cs(desc, false);
		}
		break;

	/* The SYNC and SLEEP commands got the same value but different
	modifier */
	case SPI_ENGINE_INST_SYNC_SLEEP:
		/* SYNC instruction */
		if(modifier == 0x00) {
			spi_engine_write_cmd_reg(desc_extra, cmd);
		} else if(modifier == 0x01) {
			spi_gen_sleep_ns(desc, parameter);
		}
		break;
	case SPI_ENGINE_INST_CONFIG:
		spi_engine_write_cmd_reg(desc_extra, cmd);

		break;

	default:

		return FAILURE;
		break;
	}

	return SUCCESS;
}

/**
 * @brief Prepare the command queue before sending it to the engine
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Structure used to store the transfer messages
 * @return int32_t This function allways returns SUCCESS
 */
static int32_t spi_engine_compile_message(struct spi_desc *desc,
		struct spi_engine_msg *msg)
{
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	/* Set the data transfer length */
	spi_engine_queue_append_cmd(&msg->cmds,
				    SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_DATA_TRANSFER_LEN,
					    desc_extra->data_width));
	/*
	 * Configure the spi mode :
	 *	- 3 wire
	 *	- CPOL
	 *	- CPHA
	 */
	spi_engine_queue_append_cmd(&msg->cmds,
				    SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_REG_CONFIG,
					    desc->mode));

	/* Configure the prescaler */
	spi_engine_queue_append_cmd(&msg->cmds,
				    SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_REG_CLK_DIV,
					    desc_extra->clk_div));

	/* Add a sync command to signal that the transfer has finished */
	spi_engine_queue_add_cmd(&msg->cmds, SPI_ENGINE_CMD_SYNC(_sync_id));

	return SUCCESS;
}

/**
 * @brief Initiate a spi transfer
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Structure used to store the transfer messages
 * @return int32_t - SUCCESS if the transfer finished
 *		   - FAILURE if the memory allocation failed
 */
static int32_t spi_engine_transfer_message(struct spi_desc *desc,
		struct spi_engine_msg *msg)
{
	uint32_t		i;
	uint32_t		data;
	uint32_t		sync_id;
	bool 			offload_en;
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	spi_engine_compile_message(desc, msg);

	offload_en = (desc_extra->offload_config & OFFLOAD_TX_EN) |
		     (desc_extra->offload_config & OFFLOAD_RX_EN);

	/* Write the command fifo buffer */
	while(msg->cmds != NULL) {
		spi_engine_queue_get_cmd(&msg->cmds, &data);
		spi_engine_write_cmd(desc, data);
	}

	/* Write a number of tx_length WORDS on the SDO line */

	if(offload_en) {
		for(i = 0; i < desc_extra->offload_tx_len; i++)
			spi_engine_write(desc_extra,
					 SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0),
					 bswap_constant_32(msg->tx_buf[i]));

	} else {
		for(i = 0; i < msg->length; i++)
			spi_engine_write(desc_extra,
					 SPI_ENGINE_REG_SDO_DATA_FIFO,
					 msg->tx_buf[i]);
		do {
			spi_engine_read(desc_extra,
					SPI_ENGINE_REG_SYNC_ID,
					&sync_id);
		}
		/* Wait for the end sync signal */
		while(sync_id != _sync_id);
		_sync_id++;

		/* Read a number of rx_length WORDS from the SDI line and store
		them */
		for(i = 0; i < msg->length; i++) {
			spi_engine_read(desc_extra,
					SPI_ENGINE_REG_SDI_DATA_FIFO,
					&data);
			msg->rx_buf[i] = data;
		}
	}

	return SUCCESS;
}

/**
 * @brief Initialize the spi engine
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param param Structure containing the spi init parameters
 * @return int32_t - SUCCESS if the transfer finished
 *		   - FAILURE if the memory allocation failed
 */
int32_t spi_engine_init(struct spi_desc **desc,
			const struct spi_init_param *param)
{
	uint32_t			data_width;
	uint32_t			spi_engine_version;
	struct spi_engine_desc		*eng_desc;
	struct spi_engine_init_param	*spi_engine_init;

	if (!param) {
		return FAILURE;
	}

	*desc = malloc(sizeof(**desc));
	if(! *desc) {
		free(*desc);
		return FAILURE;
	}

	eng_desc = (struct spi_engine_desc*)malloc(sizeof(*eng_desc));

	if (!eng_desc)
		return FAILURE;

	spi_engine_init = param->extra;

	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->chip_select = param->chip_select;
	(*desc)->mode = param->mode;
	(*desc)->extra = eng_desc;

	eng_desc->offload_config = OFFLOAD_DISABLED;
	eng_desc->spi_engine_baseaddr = spi_engine_init->spi_engine_baseaddr;
	eng_desc->type = spi_engine_init->type;
	eng_desc->cs_delay = spi_engine_init->cs_delay;
	eng_desc->clk_div =  SPI_ENGINE_AXI_SPEED_HZ /
			     (2 * param->max_speed_hz) - 1;

	/* Perform a reset */
	spi_engine_write(eng_desc, SPI_ENGINE_REG_RESET, 0x01);
	usleep(1000);
	spi_engine_write(eng_desc, SPI_ENGINE_REG_RESET, 0x00);

	/* Get current data width */
	spi_engine_read(eng_desc, SPI_ENGINE_REG_DATA_WIDTH, &data_width);
	eng_desc->max_data_width = data_width;

	spi_engine_set_transfer_width(*desc, spi_engine_init->data_width);

	/* Get current data width */
	spi_engine_read(eng_desc, SPI_ENGINE_REG_VERSION, &spi_engine_version);

	printf("Spi engine v%u.%u.%u succesfully initialized.",
	       (spi_engine_version >> 16),
	       ((spi_engine_version >> 8) & 0xFF),
	       (spi_engine_version & 0xFF));

	return SUCCESS;
}

/**
 * @brief Write/read on the spi interface
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param data Pointer to data buffer
 * @param bytes_number Number of bytes to transfer
 * @return int32_t - SUCCESS if the transfer finished
 *		   - FAILURE if the memory allocation or transfer failed
 */
int32_t spi_engine_write_and_read(struct spi_desc *desc,
				  uint8_t *data,
				  uint16_t bytes_number)
{
	uint8_t 		i;
	uint8_t 		word_len;
	uint8_t 		words_number;
	int32_t 		ret;
	struct spi_engine_msg	msg;
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	words_number = spi_get_words_number(desc_extra, bytes_number);

	msg.cmds = (spi_engine_cmd_queue*)malloc(sizeof(*msg.cmds));
	if (!msg.cmds)
		return FAILURE;

	msg.tx_buf =(uint32_t*)calloc(words_number, sizeof(msg.tx_buf[0]));
	msg.rx_buf =(uint32_t*)calloc(words_number, sizeof(msg.rx_buf[0]));
	msg.length = words_number;

	/* Get the length of transfered word */
	word_len = spi_get_word_lenght(desc_extra);

	/* Make sure the CS is HIGH before starting a transaction */
	msg.cmds->next = NULL;
	msg.cmds->cmd = CS_HIGH;
	spi_engine_queue_add_cmd(&msg.cmds, CS_LOW);
	spi_engine_queue_add_cmd(&msg.cmds, WRITE_READ(bytes_number));
	spi_engine_queue_add_cmd(&msg.cmds, CS_HIGH);

	/* Pack the bytes into engine WORDS */
	for (i = 0; i < bytes_number; i++)
		msg.tx_buf[i / word_len] |= data[i] << (desc_extra->data_width-
							(i % word_len + 1) * 8);

	ret = spi_engine_transfer_message(desc, &msg);

	/* Skip the first byte ( dummy read byte ) */
	for (i = 1; i < bytes_number; i++)
		data[i - 1] = msg.rx_buf[(i) / word_len] >>
			      (desc_extra->data_width -
			       ((i) % word_len + 1) * 8);

	spi_engine_queue_free(&msg.cmds);
	free(msg.tx_buf);
	free(msg.rx_buf);

	return ret;
}

/**
 * @brief Initialize the SPI engine's offload module
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param param Structure containing the offload init parameters
 * @return int32_t This function allways returns SUCCESS
 */
int32_t spi_engine_offload_init(struct spi_desc *desc,
				const struct spi_engine_offload_init_param *param)
{
	struct spi_engine_desc	*eng_desc;
	struct axi_dmac_init	dmac_init;

	eng_desc = desc->extra;

	eng_desc->offload_config = param->offload_config;

	if(param->offload_config & OFFLOAD_TX_EN) {
		dmac_init.name = "DAC DMAC";
		dmac_init.base = param->tx_dma_baseaddr;
		dmac_init.direction = DMA_MEM_TO_DEV;
		dmac_init.flags = DMA_CYCLIC;
		axi_dmac_init(&eng_desc->offload_tx_dma, &dmac_init);
		if(!eng_desc->offload_tx_dma)
			return FAILURE;
	}
	if(param->offload_config & OFFLOAD_RX_EN) {
		dmac_init.name = "ADC DMAC";
		dmac_init.base = param->rx_dma_baseaddr;
		dmac_init.direction = DMA_DEV_TO_MEM;
		dmac_init.flags = DMA_CYCLIC;
		axi_dmac_init(&eng_desc->offload_rx_dma, &dmac_init);
		if(!eng_desc->offload_rx_dma)
			return FAILURE;
	}

	spi_engine_set_transfer_width(desc,32);

	return SUCCESS;
}

/**
 * @brief Initiate a SPI transfer in offload mode
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Offload message that get's to be transferred
 * @param no_samples Number of time the messages will be transferred
 * @return int32_t This function allways returns SUCCESS
 */
int32_t spi_engine_offload_transfer(struct spi_desc *desc,
				    struct spi_engine_offload_message msg,
				    uint32_t no_samples)
{
	struct spi_engine_msg	transfer;
	struct spi_engine_desc	*eng_desc;
	uint32_t 		i;
	uint8_t 		word_length;

	eng_desc = desc->extra;

	/* Check if offload is disabled */
	if(!((eng_desc->offload_config & OFFLOAD_TX_EN) |
	     (eng_desc->offload_config & OFFLOAD_RX_EN)))
		return FAILURE;

	eng_desc->offload_tx_len = 0;
	eng_desc->offload_rx_len = 0;

	transfer.cmds = (spi_engine_cmd_queue*)malloc(sizeof(*transfer.cmds));

	if (!transfer.cmds)
		return FAILURE;

	transfer.tx_buf = (uint32_t *)msg.commands_data;

	/* Load the commands into the message */
	transfer.cmds->next = NULL;
	transfer.cmds->cmd = msg.commands[0];
	i = 1;
	while(i < msg.no_commands) {
		spi_engine_queue_add_cmd(&transfer.cmds, msg.commands[i++]);

	}

	spi_engine_transfer_message(desc, &transfer);

	word_length = spi_get_word_lenght(eng_desc);
	if(eng_desc->offload_config & OFFLOAD_TX_EN) {
		axi_dmac_transfer(eng_desc->offload_tx_dma,
				  msg.tx_addr,
				  word_length * eng_desc->offload_tx_len *
				  no_samples);
	}

	if(eng_desc->offload_config & OFFLOAD_RX_EN) {
		axi_dmac_transfer(eng_desc->offload_rx_dma,
				  msg.rx_addr,
				  word_length * eng_desc->offload_tx_len *
				  no_samples);
	}

	usleep(1000);

	/* Start transfer */
	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);

	spi_engine_queue_free(&transfer.cmds);

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by spi_init().
 *
 * @param desc Decriptor containing SPI interface parameters
 * @return int32_t This function allways returns SUCCESS
 */
int32_t spi_engine_remove(struct spi_desc *desc)
{
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	if(eng_desc->offload_config & OFFLOAD_TX_EN)
		axi_dmac_remove(eng_desc->offload_tx_dma);
	if(eng_desc->offload_config & OFFLOAD_RX_EN)
		axi_dmac_remove(eng_desc->offload_rx_dma);
	free(desc->extra);
	free(desc);

	return SUCCESS;
}
