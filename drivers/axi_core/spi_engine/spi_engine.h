/*******************************************************************************
 *   @file   spi_engine.h
 *   @brief  Header file of SPI Engine core features.
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
*******************************************************************************/

#ifndef SPI_ENGINE_H
#define SPI_ENGINE_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "spi_extra.h"
#include "spi_engine_private.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define OFFLOAD_DISABLED		0x00
#define OFFLOAD_TX_EN			BIT(0)
#define OFFLOAD_RX_EN			BIT(1)
#define OFFLOAD_TX_RX_EN		OFFLOAD_TX_EN | OFFLOAD_RX_EN

#define SPI_ENGINE_MSG_QUEUE_END	0xFFFFFFFF

#define	WRITE(no_bytes)			((SPI_ENGINE_INST_TRANSFER << 12) |\
	(SPI_ENGINE_INSTRUCTION_TRANSFER_W << 8) | no_bytes)

#define	READ(no_bytes)			((SPI_ENGINE_INST_TRANSFER << 12) |\
	(SPI_ENGINE_INSTRUCTION_TRANSFER_R << 8) | no_bytes)

#define	WRITE_READ(no_bytes)		((SPI_ENGINE_INST_TRANSFER << 12) |\
	(SPI_ENGINE_INSTRUCTION_TRANSFER_RW << 8) | no_bytes)

#define SLEEP(time)			SPI_ENGINE_CMD_SLEEP(time & 0xF)

/* The delay and chip select parmeters are passed over the engine descriptor
and will be used inside the function call */
#define CS_HIGH				SPI_ENGINE_CMD_ASSERT(0x03, 0xFF)
#define CS_LOW				SPI_ENGINE_CMD_ASSERT(0x03, 0x00)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct spi_engine_init_param {
	enum xil_spi_type	type;
	uint32_t 		spi_engine_baseaddr;
	uint32_t		cs_delay;
	uint8_t			data_width;
} spi_engine_init_param;

typedef struct spi_engine_desc {
	enum xil_spi_type	type;
	struct axi_dmac		*offload_tx_dma;
	struct axi_dmac		*offload_rx_dma;
	uint8_t			offload_config;
	uint8_t			offload_tx_len;
	uint8_t			offload_rx_len;
	uint32_t		spi_engine_baseaddr;
	uint32_t		rx_dma_baseaddr;
	uint32_t		tx_dma_baseaddr;
	uint8_t			cs_delay;
	uint32_t		clk_div;
	uint8_t			data_width;
	uint8_t 		max_data_width;
} spi_engine_desc;

typedef struct spi_engine_offload_init_param {
	uint32_t	rx_dma_baseaddr;
	uint32_t	tx_dma_baseaddr;
	uint8_t		offload_config;
} spi_engine_offload_init_param;

typedef struct spi_engine_offload_message {
	uint32_t *commands;
	uint32_t no_commands;
	uint8_t *commands_data;
	uint32_t tx_addr;
	uint32_t rx_addr;
} spi_engine_offload_message;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t spi_engine_init(struct spi_desc **desc,
			const struct spi_init_param *param);

int32_t spi_engine_write_and_read(spi_desc *desc,
				  uint8_t *data,
				  uint8_t bytes_number);

int32_t spi_engine_remove(spi_desc *desc);

/* Sets the length of one WORD used in the spi transfer.*/
int32_t spi_engine_set_transfer_wdith(spi_desc *desc,
				      uint8_t data_wdith);

int32_t spi_engine_offload_init(struct spi_desc *desc,
			     const struct spi_engine_offload_init_param *param);

int32_t spi_engine_offload_transfer(struct spi_desc *desc,
				    struct spi_engine_offload_message msg,
				    uint32_t no_samples);

#endif // SPI_ENGINE_H
