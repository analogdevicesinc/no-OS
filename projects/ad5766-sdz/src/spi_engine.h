/***************************************************************************//**
 *   @file   spi_engine.h
 *   @brief  Header file of SPI Engine Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef SPI_ENGINE_H_
#define SPI_ENGINE_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define BIT(x)								(1UL << (x))

#define SPI_ENGINE_VERSION_MAJOR(x)			((x >> 16) & 0xff)
#define SPI_ENGINE_VERSION_MINOR(x)			((x >> 8) & 0xff)
#define SPI_ENGINE_VERSION_PATCH(x)			(x & 0xff)

#define SPI_ENGINE_REG_VERSION				0x00

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

#define min(x, y)	(((x) < (y)) ? (x) : (y))
#define max(x, y)	(((x) > (y)) ? (x) : (y))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint8_t		chip_select;
	uint8_t		cs_change;
	uint32_t	spi_clk;
	uint32_t	spi_config;
	uint8_t		tx_length;
	uint8_t		*tx_buf;
	uint8_t		rx_length;
	uint8_t		*rx_buf;
} spi_message;

typedef struct {
	uint32_t	length;
	uint16_t	instructions[];
} spi_engine_program;

typedef struct {
	uint32_t	baseaddr;
	uint32_t	spi_ref_clk;
	uint8_t		cmd_length;
	uint16_t	*cmd_buf;
	uint32_t	sync_id;
	uint32_t	completed_id;
	uint32_t	int_enable;
	spi_message	*msg;
	spi_engine_program *p;
} spi_engine;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t spi_engine_write(uint32_t reg_addr,
						 uint32_t reg_data);
int32_t spi_engine_setup(uint32_t baseaddr);
int32_t spi_engine_transfer_one_message(spi_message *msg);
void spi_engine_offload_enable(uint8_t enable);
int32_t spi_engine_offload_load_msg(spi_message *msg);
int32_t spi_engine_write_and_read(uint8_t ss,
						   uint8_t *data,
						   uint8_t bytes_number);
#endif
