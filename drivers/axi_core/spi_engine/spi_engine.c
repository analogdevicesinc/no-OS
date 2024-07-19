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
#include <inttypes.h>

#include "axi_dmac.h"
#include "no_os_axi_io.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "spi_engine.h"

/**
 * @brief Spi engine platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops spi_eng_platform_ops = {
	.init = &spi_engine_init,
	.write_and_read = &spi_engine_write_and_read,
	.remove = &spi_engine_remove
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
 * @return int32_t This function allways returns 0
 */
int32_t spi_engine_write(struct spi_engine_desc *desc,
			 uint32_t reg_addr,
			 uint32_t reg_data)
{
	no_os_axi_io_write(desc->spi_engine_baseaddr, reg_addr, reg_data);

	return 0;
}

/**
 * @brief Read SPI Engine's axi registers
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param reg_addr The address of the SPI Engine's axi register from where the
 * 	data where the data will be read
 * @param reg_data Pointer where the read that will be stored
 * @return int32_t This function allways returns 0
 */
int32_t spi_engine_read(struct spi_engine_desc *desc,
			uint32_t reg_addr,
			uint32_t *reg_data)
{
	no_os_axi_io_read(desc->spi_engine_baseaddr, reg_addr, reg_data);
	return 0;
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
int32_t spi_engine_set_transfer_width(struct no_os_spi_desc *desc,
				      uint8_t data_wdith)
{
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	if (data_wdith > desc_extra->max_data_width)
		desc_extra->data_width = desc_extra->max_data_width;
	else
		desc_extra->data_width = data_wdith;

	return 0;
}

/**
 * @brief Set SPI engine clock frequency
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param speed_hz SPI engine transfer speed
 */
void spi_engine_set_speed(struct no_os_spi_desc *desc,
			  uint32_t speed_hz)
{
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	desc_extra->clk_div =  desc_extra->ref_clk_hz /
			       (2 * speed_hz) - 1;
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

	xfer_word_len = NO_OS_DIV_ROUND_UP(desc->data_width, 8);
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

	word_lenght = NO_OS_DIV_ROUND_UP(desc->data_width, 8);

	return word_lenght;
}

/**
 * @brief Compute the prescaler used to set the sleep period.
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param sleep_time_ns The amount of time where the transfer hangs
 * @param sleep_div Clock prescaler
 * @return int32_t This function allways returns 0
 */
static int32_t spi_get_sleep_div(struct no_os_spi_desc *desc,
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

	return 0;
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

	local_fifo = (spi_engine_cmd_queue*)no_os_malloc(sizeof(*local_fifo));

	if(!local_fifo)
		return -1;

	local_fifo->cmd = cmd;
	local_fifo->next = NULL;
	*fifo = local_fifo;

	return 0;
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
 * @return int32_t Return -1 if the queue is empty
 */
static int32_t spi_engine_queue_get_cmd(struct spi_engine_cmd_queue **fifo,
					uint32_t *cmd)
{
	struct spi_engine_cmd_queue *local_fifo;

	if (!*fifo)
		return -1;

	local_fifo = *fifo;
	*cmd = local_fifo->cmd;
	if ((*fifo)->next) {
		*fifo = local_fifo->next;
		no_os_free(local_fifo);
	} else {
		no_os_free(*fifo);
		*fifo = NULL;
	}

	return 0;
}

/**
 * @brief Free the memory allocated by a queue
 *
 * @param fifo The queue that needs it's memory freed
 * @return int32_t This function allways returns 0
 */
static int32_t spi_engine_queue_no_os_free(struct spi_engine_cmd_queue **fifo)
{
	if(*fifo && (*fifo)->next)
		spi_engine_queue_no_os_free(&(*fifo)->next);
	if((*fifo) != NULL) {
		no_os_free(*fifo);
		*fifo = NULL;
	}

	return 0;
}

/**
 * @brief Write the SPI engine's command fifo
 *
 * @param desc Decriptor containing SPI Engine's parameters
 * @param cmd The command that will be written
 * @return int32_t Returns -1 if the the axi transfer failed
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
 * @return int32_t This function allways returns 0
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

	return 0;
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
static void spi_engine_set_cs(struct no_os_spi_desc *desc,
			      bool assert)
{
	uint8_t			mask;
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	mask = 0xFF;
	/* Switch the state only of the selected chip select */
	if (!assert)
		mask ^= NO_OS_BIT(desc->chip_select);

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
static void spi_gen_sleep_ns(struct no_os_spi_desc *desc,
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
 * @return int32_t - 0 if the command is transfered
 *		   - -1 if the command format is invalid
 */
static int32_t spi_engine_write_cmd(struct no_os_spi_desc *desc,
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

		return -1;
	}

	return 0;
}

/**
 * @brief Prepare the command queue before sending it to the engine
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Structure used to store the transfer messages
 * @return int32_t This function allways returns 0
 */
static int32_t spi_engine_compile_message(struct no_os_spi_desc *desc,
		struct spi_engine_msg *msg)
{
	struct spi_engine_desc	*desc_extra;

	desc_extra = desc->extra;

	/* Configure the prescaler */
	spi_engine_queue_append_cmd(&msg->cmds,
				    SPI_ENGINE_CMD_CONFIG(
					    SPI_ENGINE_CMD_REG_CLK_DIV,
					    desc_extra->clk_div));

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

	/* Add a sync command to signal that the transfer has finished */
	spi_engine_queue_add_cmd(&msg->cmds, SPI_ENGINE_CMD_SYNC(_sync_id));

	return 0;
}

/**
 * @brief Initiate a spi transfer
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Structure used to store the transfer messages
 * @return int32_t - 0 if the transfer finished
 *		   - -1 if the memory allocation failed
 */
static int32_t spi_engine_transfer_message(struct no_os_spi_desc *desc,
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
					 msg->tx_buf[i]);

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

	return 0;
}

/**
 * @brief Initialize the spi engine
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param param Structure containing the spi init parameters
 * @return int32_t - 0 if the transfer finished
 *		   - -1 if the memory allocation failed
 */
int32_t spi_engine_init(struct no_os_spi_desc **desc,
			const struct no_os_spi_init_param *param)
{
	uint32_t			data_width;
	uint32_t			spi_engine_version;
	struct spi_engine_desc		*eng_desc;
	struct spi_engine_init_param	*spi_engine_init;

	if (!param) {
		return -1;
	}

	*desc = no_os_malloc(sizeof(**desc));
	if(! *desc) {
		no_os_free(*desc);
		return -1;
	}

	eng_desc = (struct spi_engine_desc*)no_os_malloc(sizeof(*eng_desc));

	if (!eng_desc)
		return -1;

	spi_engine_init = param->extra;

	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->chip_select = param->chip_select;
	(*desc)->mode = param->mode;
	(*desc)->extra = eng_desc;

	eng_desc->offload_config = OFFLOAD_DISABLED;
	eng_desc->spi_engine_baseaddr = spi_engine_init->spi_engine_baseaddr;
	eng_desc->type = spi_engine_init->type;
	eng_desc->cs_delay = spi_engine_init->cs_delay;
	eng_desc->ref_clk_hz = spi_engine_init->ref_clk_hz;
	eng_desc->clk_div =  eng_desc->ref_clk_hz /
			     (2 * param->max_speed_hz) - 1;

	/* Perform a reset */
	spi_engine_write(eng_desc, SPI_ENGINE_REG_RESET, 0x01);
	usleep(1000);
	spi_engine_write(eng_desc, SPI_ENGINE_REG_RESET, 0x00);

	/* Get current data width */
	spi_engine_read(eng_desc, SPI_ENGINE_REG_DATA_WIDTH, &data_width);
	/* Only the lower 16 bits are relevant for the actual data-width */
	eng_desc->max_data_width = no_os_field_get(SPI_ENGINE_REG_DATA_WIDTH_MSK,
				   data_width);

	spi_engine_set_transfer_width(*desc, spi_engine_init->data_width);

	/* Get current data width */
	spi_engine_read(eng_desc, SPI_ENGINE_REG_VERSION, &spi_engine_version);

	printf("Spi engine v%"PRIu32".%"PRIu32".%"PRIu32" succesfully initialized.\n",
	       (spi_engine_version >> 16),
	       ((spi_engine_version >> 8) & 0xFF),
	       (spi_engine_version & 0xFF));

	return 0;
}

/**
 * @brief Write/read on the spi interface
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param data Pointer to data buffer
 * @param bytes_number Number of bytes to transfer
 * @return int32_t - 0 if the transfer finished
 *		   - -1 if the memory allocation or transfer failed
 */
int32_t spi_engine_write_and_read(struct no_os_spi_desc *desc,
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

	/* If we want to access SPI interface and SPI engine offload module was
	 * activated, we need to disable it
	 * This is set in spi_engine_offload_init() */
	desc_extra->offload_config = OFFLOAD_DISABLED;
	/* This is set in spi_engine_offload_transfer() */
	spi_engine_write(desc_extra, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0);

	words_number = spi_get_words_number(desc_extra, bytes_number);

	msg.cmds = (spi_engine_cmd_queue*)no_os_malloc(sizeof(*msg.cmds));
	if (!msg.cmds)
		return -1;

	msg.tx_buf =(uint32_t*)no_os_calloc(words_number, sizeof(msg.tx_buf[0]));
	msg.rx_buf =(uint32_t*)no_os_calloc(words_number, sizeof(msg.rx_buf[0]));
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

	for (i = 0; i < bytes_number; i++)
		data[i] = msg.rx_buf[(i) / word_len] >>
			  (desc_extra->data_width -
			   ((i) % word_len + 1) * 8);

	spi_engine_queue_no_os_free(&msg.cmds);
	no_os_free(msg.tx_buf);
	no_os_free(msg.rx_buf);

	return ret;
}

/**
 * @brief Initialize the SPI engine's offload module
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param param Structure containing the offload init parameters
 * @return int32_t This function allways returns 0
 */
int32_t spi_engine_offload_init(struct no_os_spi_desc *desc,
				const struct spi_engine_offload_init_param *param)
{
	struct spi_engine_desc	*eng_desc;
	struct axi_dmac_init	dmac_init;

	eng_desc = desc->extra;

	eng_desc->offload_config = param->offload_config;

	if(!param->dma_flags) {
		eng_desc->cyclic = CYCLIC;
	} else {
		if(param->dma_flags & DMA_CYCLIC)
			eng_desc->cyclic = CYCLIC;
		else
			eng_desc->cyclic = NO;
	}

	dmac_init.irq_option = IRQ_DISABLED;
	if(param->offload_config & OFFLOAD_TX_EN) {
		dmac_init.name = "DAC DMAC";
		dmac_init.base = param->tx_dma_baseaddr;
		axi_dmac_init(&eng_desc->offload_tx_dma, &dmac_init);
		if(!eng_desc->offload_tx_dma)
			return -1;
	}
	if(param->offload_config & OFFLOAD_RX_EN) {
		dmac_init.name = "ADC DMAC";
		dmac_init.base = param->rx_dma_baseaddr;
		axi_dmac_init(&eng_desc->offload_rx_dma, &dmac_init);
		if(!eng_desc->offload_rx_dma)
			return -1;
	}

	return 0;
}

/**
 * @brief Initiate a SPI transfer in offload mode
 *
 * @param desc Decriptor containing SPI interface parameters
 * @param msg Offload message that get's to be transferred
 * @param no_samples Number of time the messages will be transferred
 * @return int32_t This function allways returns 0
 */
int32_t spi_engine_offload_transfer(struct no_os_spi_desc *desc,
				    struct spi_engine_offload_message msg,
				    uint32_t no_samples)
{
	struct spi_engine_msg	transfer;
	struct spi_engine_desc	*eng_desc;
	uint32_t 		i;

	eng_desc = desc->extra;

	/* Check if offload is disabled */
	if(!((eng_desc->offload_config & OFFLOAD_TX_EN) |
	     (eng_desc->offload_config & OFFLOAD_RX_EN)))
		return -1;

	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_RESET(0), 1);
	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_RESET(0), 0);

	eng_desc->offload_tx_len = 0;
	eng_desc->offload_rx_len = 0;

	transfer.cmds = (spi_engine_cmd_queue*)no_os_malloc(sizeof(*transfer.cmds));

	if (!transfer.cmds)
		return -1;

	transfer.tx_buf = msg.commands_data;

	/* Load the commands into the message */
	transfer.cmds->next = NULL;
	transfer.cmds->cmd = msg.commands[0];
	i = 1;
	while(i < msg.no_commands) {
		spi_engine_queue_add_cmd(&transfer.cmds, msg.commands[i++]);

	}

	spi_engine_transfer_message(desc, &transfer);

	/* Start transfer */
	spi_engine_write(eng_desc, SPI_ENGINE_REG_OFFLOAD_CTRL(0), 0x0001);
	if(eng_desc->offload_config & OFFLOAD_TX_EN) {
		struct axi_dma_transfer tx_transfer = {
			// Number of bytes to write/read
			.size = eng_desc->offload_tx_dma->width_src * eng_desc->offload_tx_len * no_samples,
			// Transfer done flag
			.transfer_done = 0,
			// Signal transfer mode
			.cyclic = eng_desc->cyclic,
			// Address of data source
			.src_addr = (uintptr_t)msg.tx_addr,
			// Address of data destination
			.dest_addr = 0
		};
		axi_dmac_transfer_start(eng_desc->offload_tx_dma, &tx_transfer);
	}

	if(eng_desc->offload_config & OFFLOAD_RX_EN) {
		struct axi_dma_transfer rx_transfer = {
			// Number of bytes to write/read
			.size = eng_desc->offload_rx_dma->width_src * eng_desc->offload_tx_len * no_samples,
			// Transfer done flag
			.transfer_done = 0,
			// Signal transfer mode
			.cyclic = NO,
			// Address of data source
			.src_addr = 0,
			// Address of data destination
			.dest_addr = (uintptr_t)msg.rx_addr
		};
		axi_dmac_transfer_start(eng_desc->offload_rx_dma, &rx_transfer);
		axi_dmac_transfer_wait_completion(eng_desc->offload_rx_dma, 500);
	}

	usleep(1000);

	spi_engine_queue_no_os_free(&transfer.cmds);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 *
 * @param desc Decriptor containing SPI interface parameters
 * @return int32_t This function allways returns 0
 */
int32_t spi_engine_remove(struct no_os_spi_desc *desc)
{
	struct spi_engine_desc	*eng_desc;

	eng_desc = desc->extra;

	if(eng_desc->offload_config & OFFLOAD_TX_EN)
		axi_dmac_remove(eng_desc->offload_tx_dma);
	if(eng_desc->offload_config & OFFLOAD_RX_EN)
		axi_dmac_remove(eng_desc->offload_rx_dma);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}
