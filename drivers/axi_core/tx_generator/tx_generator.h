/***************************************************************************//**
 *   @file   tx_generator.h
 *   @brief  Driver for the ADI tx_generator AXI IP.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef TX_GENERATOR_H_
#define TX_GENERATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"

/******************************************************************************/
/* tx_generator_regmap_cmd registers (byte addresses)                         */
/******************************************************************************/
#define TX_GEN_REG_VERSION		0x00
#define TX_GEN_REG_ID			0x04
#define TX_GEN_REG_SCRATCH		0x08
#define TX_GEN_REG_FPGA_INFO		0x1C
#define TX_GEN_REG_RESET_CTRL		0x40
#define TX_GEN_REG_CLK_COUNT		0x54
#define TX_GEN_REG_CLK_RATIO		0x58
#define TX_GEN_REG_STATUS		0x5C
#define TX_GEN_REG_CMD_DATA		0xC0
#define TX_GEN_REG_CMD_CTRL		0xC4

/******************************************************************************/
/* up_tpl_common registers (byte addresses)                                   */
/******************************************************************************/
#define TX_GEN_REG_PROFILE_SEL		0x100
#define TX_GEN_REG_NUM_PROFILES		0x104
#define TX_GEN_REG_JESD_M_L_S_F(p)	(0x140 + (p) * 0x08)
#define TX_GEN_REG_JESD_NP_N(p)	(0x144 + (p) * 0x08)

/******************************************************************************/
/* RESET_CTRL bit fields                                                      */
/******************************************************************************/
#define TX_GEN_MMCM_RSTN		NO_OS_BIT(1)
#define TX_GEN_RSTN			NO_OS_BIT(0)

/******************************************************************************/
/* STATUS bit fields                                                          */
/******************************************************************************/
#define TX_GEN_STATUS_OK		NO_OS_BIT(0)

/******************************************************************************/
/* CMD_CTRL bit fields                                                        */
/******************************************************************************/
#define TX_GEN_CMD_ADDR_MSK		NO_OS_GENMASK(31, 16)
#define TX_GEN_CMD_NUMWORDS_MSK		NO_OS_GENMASK(15, 10)
#define TX_GEN_CMD_OP_MSK		NO_OS_GENMASK(9, 8)
#define TX_GEN_CMD_STREAM_SRC		NO_OS_BIT(1)
#define TX_GEN_CMD_START		NO_OS_BIT(0)

enum tx_gen_cmd_op {
	TX_GEN_CMD_OP_NOP		= 0,
	TX_GEN_CMD_OP_READ		= 1,
	TX_GEN_CMD_OP_STREAMING_READ	= 2,
	TX_GEN_CMD_OP_WRITE		= 3,
};

struct tx_generator_dev {
	const char *name;
	uint32_t base;
	uint64_t clock_hz;
	uint32_t num_channels;
};

struct tx_generator_init_param {
	const char *name;
	uint32_t base;
	uint32_t num_channels;
};

int32_t tx_generator_read(struct tx_generator_dev *dev,
			  uint32_t reg_addr, uint32_t *reg_data);

int32_t tx_generator_write(struct tx_generator_dev *dev,
			   uint32_t reg_addr, uint32_t reg_data);

int32_t tx_generator_init(struct tx_generator_dev **dev,
			  const struct tx_generator_init_param *init);

int32_t tx_generator_remove(struct tx_generator_dev *dev);

int32_t tx_generator_cmd_write(struct tx_generator_dev *dev,
			       uint16_t cmd_addr, uint8_t cmd_numwords,
			       enum tx_gen_cmd_op cmd_op, uint32_t cmd_data);

int32_t tx_generator_cmd_set_stream_source(struct tx_generator_dev *dev,
					   bool dma);

int32_t tx_generator_load_custom_data(struct tx_generator_dev *dev,
				      const uint32_t *custom_data_iq,
				      uint32_t custom_tx_count,
				      uint32_t address);

#endif /* TX_GENERATOR_H_ */
