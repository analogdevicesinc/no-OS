/*******************************************************************************
 *   @file   spi_engine_private.h
 *   @brief  Private definitions of the SPI Engine
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

#ifndef SPI_ENGINE_PRIVATE_H
#define SPI_ENGINE_PRIVATE_H

#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
#include "util.h"

/******************************************************************************/
/************************** Spi Engine parameters *****************************/
/******************************************************************************/

#define SPI_ENGINE_AXI_SPEED_HZ			100000000UL

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
#define SPI_ENGINE_INST_CONFIG			0x02
#define SPI_ENGINE_INST_SYNC_SLEEP		0x03
#define SPI_ENGINE_INST_MISC			0x03
#define SPI_ENGINE_CMD_REG_CLK_DIV		0x00
#define SPI_ENGINE_CMD_REG_CONFIG		0x01
#define SPI_ENGINE_CMD_DATA_TRANSFER_LEN	0x02
#define SPI_ENGINE_MISC_SYNC			0x00
#define SPI_ENGINE_MISC_SLEEP			0x01
#define SPI_ENGINE_SYNC_TRANSFER_BEGIN		BIT(1)
#define SPI_ENGINE_SYNC_TRANSFER_END		BIT(2)
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
/******************************** DMAC registers ******************************/
/******************************************************************************/

#define DMAC_REG_IRQ_MASK			0x080
#define DMAC_REG_IRQ_PENDING			0x084
#define DMAC_REG_IRQ_SOURCE			0x088
#define DMAC_REG_CTRL				0x400
#define DMAC_REG_TRANSFER_ID			0x404
#define DMAC_REG_START_TRANSFER			0x408
#define DMAC_REG_FLAGS				0x40c
#define DMAC_REG_DEST_ADDRESS			0x410
#define DMAC_REG_SRC_ADDRESS			0x414
#define DMAC_REG_X_LENGTH			0x418
#define DMAC_REG_Y_LENGTH			0x41C
#define DMAC_REG_DEST_STRIDE			0x420
#define DMAC_REG_SRC_STRIDE			0x424
#define DMAC_REG_TRANSFER_DONE			0x428
#define DMAC_REG_ACTIVE_TRANSFER_ID 		0x42C
#define DMAC_REG_STATUS				0x430
#define DMAC_REG_CURRENT_DEST_ADDR		0x434
#define DMAC_REG_CURRENT_SRC_ADDR		0x438
#define DMAC_REG_DBG0				0x43C
#define DMAC_REG_DBG1				0x440

/******************************************************************************/
/*************************** DMAC register parameters *************************/
/******************************************************************************/

#define DMAC_CTRL_ENABLE			(1 << 0)
#define DMAC_CTRL_PAUSE				(1 << 1)
#define DMAC_IRQ_SOT				(1 << 0)
#define DMAC_IRQ_EOT				(1 << 1)

/******************************************************************************/
/**************************** Spi Engine commands *****************************/
/******************************************************************************/

#define SPI_ENGINE_INSTRUCTION_TRANSFER_W	BIT(0)
#define SPI_ENGINE_INSTRUCTION_TRANSFER_R	BIT(1)
#define SPI_ENGINE_INSTRUCTION_TRANSFER_RW	GENMASK(1,0)

#define SPI_ENGINE_REG_OFFLOAD_CTRL(x)		(0x100 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_STATUS(x)	(0x104 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_RESET(x)		(0x108 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_CMD_MEM(x)	(0x110 + (0x20 * x))
#define SPI_ENGINE_REG_OFFLOAD_SDO_MEM(x)	(0x114 + (0x20 * x))

#define SPI_ENGINE_CMD(inst, arg1, arg2) 				\
			(((inst & 0x03) << 12) | 			\
			((arg1 & 0x03) << 8) | arg2 )

#define SPI_ENGINE_CMD_TRANSFER(readwrite, n)				\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_TRANSFER,			\
			(readwrite), (n))

#define SPI_ENGINE_CMD_ASSERT(delay, cs) 				\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_ASSERT,				\
			(delay), (cs))

#define SPI_ENGINE_CMD_CONFIG(reg, val)					\
	SPI_ENGINE_CMD(SPI_ENGINE_INST_CONFIG,				\
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
/******************************* Debug macros *********************************/
/******************************************************************************/

/**
 * @brief Macro used to print debug messages
 * 
 */
#ifdef DEBUG_LEVEL
#define dev_dbg_reg(dev, format, ...){			\
	if (DEBUG_LEVEL >= 2{				\
		printf("%s : ", #dev);			\
		printf("%s : ", __func__);		\
		printf(format, ## __VA_ARGS__);		\
		printf("\n");				\
		}					\
	}
#else
#define dev_dbg_reg(dev, format, ...){}
#endif
/**
 * @brief Macro used to print debug messages
 * 
 */
#ifdef DEBUG_LEVEL
#define dev_dbg(dev, format, ...){			\
	if (DEBUG_LEVEL >= 1){				\
		printf("%s : ", #dev);			\
		printf("%s : ", __func__);		\
		printf(format, ## __VA_ARGS__);		\
		printf("\n");				\
		}					\
	}
#else
#define dev_dbg(dev, format, ...){}
#endif


/**
 * @brief Macro used to print device information
 * 
 */
#define dev_info(dev, format, ...){			\
	printf(format, ## __VA_ARGS__);			\
	printf("\n");					\
	}

/**
 * @brief Macro used to print debug errors
 * 
 */
#define dev_err(dev, format, ...){			\
	printf("%s : ", #dev);				\
	printf("%s : Device error:", __func__);		\
	printf(format, ## __VA_ARGS__);			\
	printf("\n");					\
	}

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct spi_engine_cmd_queue{
	uint32_t	cmd;
	struct		spi_engine_cmd_queue *next;
} spi_engine_cmd_queue;

typedef struct spi_engine_msg {
	uint32_t			*tx_buf;
	uint32_t			*rx_buf;
	uint32_t			length;
	struct spi_engine_cmd_queue	*cmds;
} spi_engine_msg;

#endif // SPI_ENGINE_PRIVATE_H
