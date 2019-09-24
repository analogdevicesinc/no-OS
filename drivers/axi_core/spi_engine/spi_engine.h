/*******************************************************************************
 *   @file   spi_engine.h
 *   @brief  Header file of SPI Engine Driver.
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
*******************************************************************************/

#ifndef SPI_ENGINE_H
#define SPI_ENGINE_H

/******************************************************************************/
/********************************* Macros *************************************/
/******************************************************************************/

#ifndef BIT
#define BIT(x)		(1UL << (x))
#endif

#ifndef GENMASK
#define GENMASK(h, l)	(((~0UL) << (l)) & (~0UL >> (32 - 1 - (h))))
#endif

/******************************************************************************/
/*************************** Spi Engine registers *****************************/
/******************************************************************************/

#define SPI_ENGINE_REG_VERSION			0x00
#define SPI_ENGINE_REG_DATA_WIDTH		0x0C
#define SPI_ENGINE_REG_RESET			0x40
#define SPI_ENGINE_REG_INT_ENABLE		0x80
#define SPI_ENGINE_REG_INT_PENDING		0x84
#define SPI_ENGINE_REG_INT_SOURCE		0x88
#define SPI_ENGINE_REG_SYNC_ID			0xC0
#define SPI_ENGINE_REG_CMD_FIFO_ROOM		0xD0
#define SPI_ENGINE_REG_SDO_FIFO_ROOM		0xD4
#define SPI_ENGINE_REG_SDI_FIFO_LEVEL		0xD8
#define SPI_ENGINE_REG_CMD_FIFO 		0xE0
#define SPI_ENGINE_REG_SDO_DATA_FIFO		0xE4
#define SPI_ENGINE_REG_SDI_DATA_FIFO		0xE8
#define SPI_ENGINE_REG_SDI_DATA_FIFO_PEEK	0xEC

/******************************************************************************/
/************************ Spi Engine register parameters **********************/
/******************************************************************************/

#define SPI_ENGINE_INST_TRANSFER		0x00
#define SPI_ENGINE_INST_ASSERT			0x01
#define SPI_ENGINE_INST_WRITE			0x02
#define SPI_ENGINE_INST_MISC			0x03
#define SPI_ENGINE_CMD_REG_CLK_DIV		0x00
#define SPI_ENGINE_CMD_REG_CONFIG		0x01
#define SPI_ENGINE_CMD_DATA_TRANSFER_LEN	0x02
#define SPI_ENGINE_MISC_SYNC			0x00
#define SPI_ENGINE_MISC_SLEEP			0x01
#define SPI_ENGINE_SYNC_TRANSFER_BEGIN		0x01
#define SPI_ENGINE_SYNC_TRANSFER_END		0x02
#define SPI_ENGINE_INT_CMD_ALMOST_EMPTY		BIT(0)
#define SPI_ENGINE_INT_SDO_ALMOST_EMPTY		BIT(1)
#define SPI_ENGINE_INT_SDI_ALMOST_FULL		BIT(2)
#define SPI_ENGINE_INT_SYNC			BIT(3)
#define SPI_ENGINE_OFFLOAD_CTRL_ENABLE		BIT(0)
#define SPI_ENGINE_OFFLOAD_STATUS_ENABLED	BIT(0)
#define SPI_ENGINE_CONFIG_CPHA			BIT(0)
#define SPI_ENGINE_CONFIG_CPOL			BIT(1)
#define SPI_ENGINE_CONFIG_3WIRE			BIT(2)
#define SPI_ENGINE_VERSION_MAJOR(x) 		((x >> 16) & 0xff)
#define SPI_ENGINE_VERSION_MINOR(x) 		((x >> 8) & 0xff)
#define SPI_ENGINE_VERSION_PATCH(x) 		(x & 0xff)

/******************************************************************************/
/**************************** Spi Engine commands *****************************/
/******************************************************************************/

#define CS_DEASSERT				0
#define CS_ASSERT				BIT(28)
#define SLEEP_CMD				BIT(29)
#define TRANSFER_R_CMD				GENMASK(29,28)
#define TRANSFER_W_CMD				BIT(30)
#define TRANSFER_R_W_CMD			(BIT(30) | BIT(28))

#define SPI_ENGINE_REG_OFFLOAD_CTRL(x)		(0x100 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_STATUS(x)	(0x104 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_RESET(x)		(0x108 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_CMD_MEM(x)	(0x110 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_SDO_MEM(x)	(0x114 + (0x20 * x))

#define SLEEP(x)				(SLEEP_CMD |		\
						(x & ( ~(0xF << 28) )))

#define TRANSFER_BYTES_R(x)			(TRANSFER_R_CMD |	\
						(x & 0xF))

#define TRANSFER_BYTES_W(x)			(TRANSFER_W_CMD |	\
						(x & 0xF))

#define TRANSFER_BYTES_R_W(x)			(TRANSFER_R_W_CMD |	\
						(x & 0xF))

#define SPI_ENGINE_CMD(inst, arg1, arg2) 				\
			(((inst & 0x03) << 12) | 			\
			((arg1 & 0x03) << 8) | (arg2))

#define SPI_ENGINE_CMD_TRANSFER(write, read, n) 			\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_TRANSFER,			\
			((read) << 1 | (write)), (n))

#define SPI_ENGINE_CMD_ASSERT(delay, cs) 				\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_ASSERT,				\
			(delay), (cs))

#define SPI_ENGINE_CMD_WRITE(reg, val)					\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_WRITE,				\
			(reg), (val))

#define SPI_ENGINE_CMD_SLEEP(delay) 					\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_MISC, 				\
			SPI_ENGINE_MISC_SLEEP, 				\
			(delay))

#define SPI_ENGINE_CMD_SYNC(id) 					\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_MISC, 				\
			SPI_ENGINE_MISC_SYNC, 				\
			(id))

/******************************************************************************/
/******************************** DMAC registers ******************************/
/******************************************************************************/

#define DMAC_REG_IRQ_MASK		0x080
#define DMAC_REG_IRQ_PENDING		0x084
#define DMAC_REG_IRQ_SOURCE		0x088
#define DMAC_REG_CTRL			0x400
#define DMAC_REG_TRANSFER_ID		0x404
#define DMAC_REG_START_TRANSFER		0x408
#define DMAC_REG_FLAGS			0x40c
#define DMAC_REG_DEST_ADDRESS		0x410
#define DMAC_REG_SRC_ADDRESS		0x414
#define DMAC_REG_X_LENGTH		0x418
#define DMAC_REG_Y_LENGTH		0x41C
#define DMAC_REG_DEST_STRIDE		0x420
#define DMAC_REG_SRC_STRIDE		0x424
#define DMAC_REG_TRANSFER_DONE		0x428
#define DMAC_REG_ACTIVE_TRANSFER_ID 	0x42C
#define DMAC_REG_STATUS			0x430
#define DMAC_REG_CURRENT_DEST_ADDR	0x434
#define DMAC_REG_CURRENT_SRC_ADDR	0x438
#define DMAC_REG_DBG0			0x43C
#define DMAC_REG_DBG1			0x440

/******************************************************************************/
/*************************** DMAC register parameters *************************/
/******************************************************************************/

#define DMAC_CTRL_ENABLE		(1 << 0)
#define DMAC_CTRL_PAUSE 		(1 << 1)
#define DMAC_IRQ_SOT			(1 << 0)
#define DMAC_IRQ_EOT			(1 << 1)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct spi_init_param_extra {
	uint32_t 	spi_clk_hz;
	uint32_t 	spi_baseaddr;
	uint8_t 	cs_delay;
	uint8_t 	spi_offload_rx_support_en;
	uint32_t	spi_offload_rx_dma_baseaddr;
	uint8_t 	spi_offload_tx_support_en;
	uint32_t	spi_offload_tx_dma_baseaddr;
	uint32_t	spi_clk_hz_reg_access;
} spi_init_param_extra;

typedef struct spi_eng_init_param {
	uint32_t	max_speed_hz;
	uint8_t 	chip_select;
	enum spi_mode	mode;
	struct spi_init_param_extra 	extra;
} spi_eng_init_param;

typedef struct spi_desc_extra {
	uint32_t 	spi_clk_hz;
	uint32_t	spi_baseaddr;
	uint8_t 	cs_delay;
	uint8_t 	spi_offload_rx_support_en;
	uint32_t	spi_offload_rx_dma_baseaddr;
	uint8_t 	spi_offload_tx_support_en;
	uint32_t	spi_offload_tx_dma_baseaddr;
	uint32_t	rx_dma_startaddr;
	uint32_t	tx_dma_startaddr;
	uint32_t	rx_length;
	uint32_t	tx_length;
	uint32_t	spi_clk_hz_reg_access;
	uint32_t	clk_div;
	uint8_t 	offload_configured;
	uint8_t 	data_width;
	uint8_t 	max_data_width;
} spi_desc_extra;

typedef struct spi_eng_desc {
	uint32_t	max_speed_hz;
	uint8_t 	chip_select;
	enum spi_mode	mode;
	struct spi_desc_extra	extra;
} spi_eng_desc;

typedef struct spi_eng_msg {
	uint32_t	tx_buf_addr;
	uint32_t	rx_buf_addr;
	uint32_t	*rx_buf;
	uint32_t	*tx_buf;
	uint32_t	*spi_msg_cmds;
	uint8_t 	msg_cmd_len;
} spi_eng_msg;

typedef struct spi_eng_transfer_fifo {
	uint32_t	cmd_fifo_len;
	uint16_t	cmd_fifo[];
} spi_eng_transfer_fifo;

/* Sets the length of one WORD used in the spi transfer.*/
void 	spi_set_transfer_length(spi_desc *desc, uint8_t data_length);

/* Prepare the command queue of the offload mode */
int32_t	spi_eng_offload_load_msg(spi_desc *desc, spi_eng_msg *msg);

/* Initiate an offload transfer */
int32_t	spi_eng_transfer_multiple_msgs(spi_desc *desc, uint32_t no_of_messages);

#endif // SPI_ENGINE_H
