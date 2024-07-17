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

#include "xilinx_spi.h"
#include "spi_engine_private.h"
#include "axi_dmac.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define OFFLOAD_DISABLED		0x00
#define OFFLOAD_TX_EN			NO_OS_BIT(0)
#define OFFLOAD_RX_EN			NO_OS_BIT(1)
#define OFFLOAD_TX_RX_EN		OFFLOAD_TX_EN | OFFLOAD_RX_EN

#define SPI_ENGINE_MSG_QUEUE_END	0xFFFFFFFF

/* Spi engine commands */
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

/**
 * @struct spi_engine_init_param
 * @brief  Structure containing the init parameters needed by the SPI engine
 */
struct spi_engine_init_param {
	/** SPI engine reference clock */
	uint32_t	ref_clk_hz;
	/** Type of implementation */
	enum xil_spi_type	type;
	/** Base address where the HDL core is situated */
	uint32_t 		spi_engine_baseaddr;
	/** Delay between the CS toggle and the start of SCLK */
	uint32_t		cs_delay;
	/** Data with of one SPI transfer ( in bits ) */
	uint8_t			data_width;
};


/**
 * @struct spi_engine_desc
 * @brief  Structure representing an SPI engine device
 */
struct spi_engine_desc {
	/** SPI engine reference clock */
	uint32_t	ref_clk_hz;
	/** Type of implementation */
	enum xil_spi_type	type;
	/** Pointer to a DMAC used in transmission */
	struct axi_dmac		*offload_tx_dma;
	/** Pointer to a DMAC used in reception */
	struct axi_dmac		*offload_rx_dma;
	/** Transfer mode for Tx DMAC */
	enum cyclic_transfer cyclic;
	/** Offload's module transfer direction : TX, RX or both */
	uint8_t			offload_config;
	/** Number of words that the module has to send */
	uint8_t			offload_tx_len;
	/** Number of words that the module has to receive */
	uint8_t			offload_rx_len;
	/** Base address where the HDL core is situated */
	uint32_t		spi_engine_baseaddr;
	/** Base address where the RX DMAC core is situated */
	uint32_t		rx_dma_baseaddr;
	/** Base address where the TX DMAC core is situated */
	uint32_t		tx_dma_baseaddr;
	/** Delay between the CS toggle and the start of SCLK */
	uint8_t			cs_delay;
	/** Clock divider used in transmission delays */
	uint32_t		clk_div;
	/** Data with of one SPI transfer ( in bits ) */
	uint8_t			data_width;
	/** The maximum data width supported by the engine */
	uint8_t 		max_data_width;
};


/**
 * @struct spi_engine_offload_init_param
 * @brief  Structure containing the init parameters needed by the offload module
 */
struct spi_engine_offload_init_param {
	/** Base address where the RX DMAC core is situated */
	uint32_t	rx_dma_baseaddr;
	/** Base address where the TX DMAC core is situated */
	uint32_t	tx_dma_baseaddr;
	/** DMAC flags - if not initialized, the default value is DMA_CYCLIC */
	uint32_t	dma_flags;
	/** Offload's module transfer direction : TX, RX or both */
	uint8_t		offload_config;
};

/**
 * @struct spi_engine_offload_message
 * @brief  Structure representing an offload message
 */
struct spi_engine_offload_message {
	/** Pointer to the SPI engine commands buffer that will be sent.
	 * The available commands can be found at:
	 * https://wiki.analog.com/resources/fpga/peripherals/spi_engine/
	*/
	uint32_t *commands;
	/** Length of the SPI engine commands buffer */
	uint32_t no_commands;
	/** Pointer of the data that will be sent over spi */
	uint32_t *commands_data;
	/** The address where the data that will be transmitted is situated */
	uint32_t tx_addr;
	/** The address where the data that will be received is situated */
	uint32_t rx_addr;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief Spi engine platform specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops xil_spi_ops;

/* Write SPI Engine's axi registers */
int32_t spi_engine_write(struct spi_engine_desc *desc,
			 uint32_t reg_addr,
			 uint32_t reg_data);

/* Read SPI Engine's axi registers */
int32_t spi_engine_read(struct spi_engine_desc *desc,
			uint32_t reg_addr,
			uint32_t *reg_data);
/* Initialize the SPI engine device */
int32_t spi_engine_init(struct no_os_spi_desc **desc,
			const struct no_os_spi_init_param *param);

/* Write and read data over SPI using the SPI engine */
int32_t spi_engine_write_and_read(struct no_os_spi_desc *desc,
				  uint8_t *data,
				  uint16_t bytes_number);

/* Free the resources used by the SPI engine device */
int32_t spi_engine_remove(struct no_os_spi_desc *desc);

/* Initialize the SPI engine offload module */
int32_t spi_engine_offload_init(struct no_os_spi_desc *desc,
				const struct spi_engine_offload_init_param *param);

/* Write and read data over SPI using the offload module */
int32_t spi_engine_offload_transfer(struct no_os_spi_desc *desc,
				    struct spi_engine_offload_message msg,
				    uint32_t no_samples);

/* Set SPI transfer width */
int32_t spi_engine_set_transfer_width(struct no_os_spi_desc *desc,
				      uint8_t data_wdith);

/* Set SPI transfer speed */
void spi_engine_set_speed(struct no_os_spi_desc *desc,
			  uint32_t speed_hz);

#endif // SPI_ENGINE_H
