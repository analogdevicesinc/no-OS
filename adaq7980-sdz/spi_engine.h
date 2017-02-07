/***************************************************************************//**
 *   @file   spi.h
 *   @brief  Header file of SPI Engine Driver.
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
#ifndef SPI_H_
#define SPI_H_


/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/******************************************************************************/
#define BIT(x)								(1UL << (x))

#define SPI_ENGINE_VERSION_MAJOR(x)			((x >> 16) & 0xff)
#define SPI_ENGINE_VERSION_MINOR(x)			((x >> 8) & 0xff)
#define SPI_ENGINE_VERSION_PATCH(x)			(x & 0xff)

#define SPI_ENGINE_REG_VERSION				0x00

#define SPI_ENGINE_REG_DATA_WIDTH			0x0C

#define SPI_ENGINE_REG_RESET				0x40

#define SPI_ENGINE_REG_INT_ENABLE			0x80
#define SPI_ENGINE_REG_INT_PENDING			0x84
#define SPI_ENGINE_REG_INT_SOURCE			0x88

#define SPI_ENGINE_REG_SYNC_ID				0xc0

#define SPI_ENGINE_REG_CMD_FIFO_ROOM		0xd0
#define SPI_ENGINE_REG_SDO_FIFO_ROOM		0xd4
#define SPI_ENGINE_REG_SDI_FIFO_LEVEL		0xd8

#define SPI_ENGINE_REG_CMD_FIFO				0xe0
#define SPI_ENGINE_REG_SDO_DATA_FIFO		0xe4
#define SPI_ENGINE_REG_SDI_DATA_FIFO		0xe8
#define SPI_ENGINE_REG_SDI_DATA_FIFO_PEEK	0xec

#define SPI_ENGINE_REG_OFFLOAD_CTRL(x)		(0x100 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_STATUS(x)	(0x104 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_RESET(x)		(0x108 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_CMD_MEM(x)	(0x110 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_SDO_MEM(x)	(0x114 + (0x20 * x))

#define SPI_ENGINE_INT_CMD_ALMOST_EMPTY		BIT(0)
#define SPI_ENGINE_INT_SDO_ALMOST_EMPTY		BIT(1)
#define SPI_ENGINE_INT_SDI_ALMOST_FULL		BIT(2)
#define SPI_ENGINE_INT_SYNC					BIT(3)

#define SPI_ENGINE_OFFLOAD_CTRL_ENABLE		BIT(0)
#define SPI_ENGINE_OFFLOAD_STATUS_ENABLED	BIT(0)

#define SPI_ENGINE_CONFIG_CPHA				BIT(0)
#define SPI_ENGINE_CONFIG_CPOL				BIT(1)
#define SPI_ENGINE_CONFIG_3WIRE				BIT(2)

#define SPI_ENGINE_INST_TRANSFER			0x0
#define SPI_ENGINE_INST_ASSERT				0x1
#define SPI_ENGINE_INST_WRITE				0x2
#define SPI_ENGINE_INST_MISC				0x3

#define SPI_ENGINE_CMD_REG_CLK_DIV			0x0
#define SPI_ENGINE_CMD_REG_CONFIG			0x1

#define SPI_ENGINE_MISC_SYNC				0x0
#define SPI_ENGINE_MISC_SLEEP				0x1

#define SPI_ENGINE_CMD(inst, arg1, arg2) \
	(((inst) << 12) | ((arg1) << 8) | (arg2))

#define SPI_ENGINE_CMD_TRANSFER(write, read, n) \
	SPI_ENGINE_CMD(SPI_ENGINE_INST_TRANSFER, ((read) << 1 | (write)), (n))
#define SPI_ENGINE_CMD_ASSERT(delay, cs) \
	SPI_ENGINE_CMD(SPI_ENGINE_INST_ASSERT, (delay), (cs))
#define SPI_ENGINE_CMD_WRITE(reg, val) \
	SPI_ENGINE_CMD(SPI_ENGINE_INST_WRITE, (reg), (val))
#define SPI_ENGINE_CMD_SLEEP(delay) \
	SPI_ENGINE_CMD(SPI_ENGINE_INST_MISC, SPI_ENGINE_MISC_SLEEP, (delay))
#define SPI_ENGINE_CMD_SYNC(id) \
	SPI_ENGINE_CMD(SPI_ENGINE_INST_MISC, SPI_ENGINE_MISC_SYNC, (id))


#define DMAC_REG_IRQ_MASK			0x80
#define DMAC_REG_IRQ_PENDING		0x84
#define DMAC_REG_IRQ_SOURCE			0x88

#define DMAC_REG_CTRL				0x400
#define DMAC_REG_TRANSFER_ID		0x404
#define DMAC_REG_START_TRANSFER		0x408
#define DMAC_REG_FLAGS				0x40c
#define DMAC_REG_DEST_ADDRESS		0x410
#define DMAC_REG_SRC_ADDRESS		0x414
#define DMAC_REG_X_LENGTH			0x418
#define DMAC_REG_Y_LENGTH			0x41c
#define DMAC_REG_DEST_STRIDE		0x420
#define DMAC_REG_SRC_STRIDE			0x424
#define DMAC_REG_TRANSFER_DONE		0x428
#define DMAC_REG_ACTIVE_TRANSFER_ID 0x42c
#define DMAC_REG_STATUS				0x430
#define DMAC_REG_CURRENT_DEST_ADDR	0x434
#define DMAC_REG_CURRENT_SRC_ADDR	0x438
#define DMAC_REG_DBG0				0x43c
#define DMAC_REG_DBG1				0x440

#define DMAC_CTRL_ENABLE			(1 << 0)
#define DMAC_CTRL_PAUSE				(1 << 1)

#define DMAC_IRQ_SOT				(1 << 0)
#define DMAC_IRQ_EOT				(1 << 1)


#define CS_DEASSERT				0 << 28

#define CS_ASSERT				1 << 28

#define	SLEEP_CMD				2 << 28
#define SLEEP(x)				(SLEEP_CMD | (x & ( ~(0xF << 28) )))

#define TRANSFER_R_CMD			3 << 28
#define TRANSFER_R(x)			(TRANSFER_R_CMD | (x & 0xF))

#define	TRANSFER_W_CMD			4 << 28
#define	TRANSFER_W(x)			(TRANSFER_W_CMD | (x & 0xF))

#define	TRANSFER_R_W_CMD		5 << 28
#define	TRANSFER_R_W(x)			(TRANSFER_R_W_CMD | (x & 0xF))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct {
	uint32_t	spi_baseaddr;
	uint8_t		chip_select;
	uint8_t		spi_config;
	uint32_t 	spi_clk_hz;
	uint32_t	ref_clk_hz;
	uint32_t	clk_div;
	uint32_t	rx_length;
	uint32_t	tx_length;
	uint8_t 	spi_offload_rx_support_en;
	uint32_t	spi_offload_rx_dma_baseaddr;
	uint32_t	rx_dma_startaddr;
	uint8_t		spi_offload_tx_support_en;
	uint32_t	spi_offload_tx_dma_baseaddr;
	uint32_t	tx_dma_startaddr;
	uint8_t		offload_configured;
	uint8_t		data_width;
} spi_dev;

typedef struct {
	uint32_t 	spi_baseaddr;
	uint8_t		chip_select;
	uint8_t		spi_config;
	uint32_t 	spi_clk_hz;
	uint32_t	ref_clk_hz;
	uint8_t 	spi_offload_rx_support_en;
	uint32_t	spi_offload_rx_dma_baseaddr;
	uint8_t		spi_offload_tx_support_en;
	uint32_t	spi_offload_tx_dma_baseaddr;
} spi_init_param;

typedef struct {
	uint32_t		tx_buf_addr;
	uint32_t		rx_buf_addr;
	uint32_t		*spi_msg_cmds;
	uint8_t			msg_cmd_len;
} spi_msg;


typedef struct {
	uint32_t		cmd_fifo_len;
	uint16_t		cmd_fifo[];
} spi_transfer_fifo;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t spi_eng_write(spi_dev *dev,
				  	  uint32_t reg_addr,
					  uint32_t ui32data);

int32_t spi_eng_read(spi_dev *dev,
					 uint32_t reg_addr,
					 uint32_t *reg_data);

int32_t spi_eng_dma_write(spi_dev *dev,
					  	  uint32_t reg_addr,
						  uint32_t reg_data);

int32_t spi_eng_dma_read(spi_dev *dev,
						 uint32_t reg_addr,
						 uint32_t *reg_data);


int32_t spi_eng_setup(spi_dev **device,
				  	  spi_init_param init_param);

int32_t spi_eng_offload_load_msg(spi_dev *dev, spi_msg *msg);

int32_t spi_eng_transfer_message(spi_dev *dev, spi_msg *msg);

int32_t spi_eng_transfer_multiple_msgs(spi_dev *dev, uint8_t no_of_messages);

#endif // SPI_H_
