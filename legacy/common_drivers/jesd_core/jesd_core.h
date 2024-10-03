/***************************************************************************//**
 * @file jesd_core.h
 * @brief Header file of Jesd Core.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef JESD_CORE_H_
#define JESD_CORE_H_

#include "platform_drivers.h"

/******************************************************************************/
/************************ JESD204 Core Definitions ****************************/
/******************************************************************************/
#define JESD204_REG_TRX_VERSION				0x00
#define JESD204_REG_TRX_RESET				0x04
#define JESD204_REG_TRX_ILA_SUPPORT			0x08
#define JESD204_REG_TRX_SCRAMBLING			0x0c
#define JESD204_REG_TRX_SYSREF_HANDLING			0x10
#define JESD204_REG_TX_ILA_MULTIFRAMES			0x14
#define JESD204_REG_TRX_TEST_MODES			0x18
#define JESD204_REG_RX_LINK_ERROR_STATUS		0x1c
#define JESD204_REG_TRX_OCTETS_PER_FRAME		0x20
#define JESD204_REG_TRX_FRAMES_PER_MULTIFRAME		0x24
#define JESD204_REG_TRX_LANES_IN_USE			0x28
#define JESD204_REG_TRX_SUBCLASS_MODE			0x2c
#define JESD204_REG_TRX_SYNC_STATUS			0x38

/* JESD204_REG_TRX_RESET */
#define JESD204_TRX_GT_WDT_DIS				(1 << 16)
#define JESD204_TRX_RESET				(1 << 0)

/* JESD204_REG_TRX_ILA_SUPPORT */
#define JESD204_TRX_ILA_EN				(1 << 0)

/* JESD204_REG_TRX_SCRAMBLING */
#define JESD204_TRX_SCR_EN				(1 << 0)

/* JESD204_REg_TRX_SYSREF_HANDLING */
#define JESD204_TRX_SYSREF_ALWAYSON			(1 << 0)
#define JESD204_TRX_SYSREF_DELAY(x)			(((x) & 0xf) << 8)
#define JESD204_TRX_SYSREF_ONRESYNC			(1 << 16)

/* JESD204_REG_RX_LINK_ERROR_STATUS */
#define JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM		(1 << 31)
#define JESD204_RX_LINK_SYSREF_LMFC_ALARM		(1 << 30)
#define JESD204_RX_LINK_BUFF_OVF_ALARM			(1 << 29)
#define JESD204_RX_LINK_K_CH_ERR(l)			((1 << 2) << (3 * (l)))
#define JESD204_RX_LINK_DISP_ERR(l)			((1 << 1) << (3 * (l)))
#define JESD204_RX_LINK_NOT_IN_TBL_ERR(l)		((1 << 0) << (3 * (l)))

/* JESD204_REG_TRX_OCTETS_PER_FRAME */
#define JESD204_TRX_OCTETS_PER_FRAME(x)			(((x) - 1) & 0xff)

/* JESD204_REG_TRX_FRAMES_PER_MULTIFRAME */
#define JESD204_TRX_FRAMES_PER_MULTIFRAME(x)		(((x) - 1) & 0x1f)

/* JESD204_REG_TRX_SUBCLASS_MODE */
#define JESD204_TRX_SUBCLASS_MODE(x)			((x) & 0x3)

/* JESD204_REG_TRX_SYNC_STATUS */
#define JESD204_TRX_SYSREF_CAPTURED			(1 << 16)
#define JESD204_TRX_SYNC_ACHIEVED			(1 << 0)

// ADI JESD

#define JESD204_REG_VERSION				0x00
#define JESD204_REG_ID					0x04
#define JESD204_REG_SCRATCH				0x08
#define JESD204_REG_MAGIC				0x0c

#define JESD204_REG_SYNTH_NUM_LANES			0x10
#define JESD204_REG_SYNTH_DATA_PATH_WIDTH		0x14

#define JESD204_REG_IRQ_ENABLE				0x80
#define JESD204_REG_IRQ_PENDING				0x84
#define JESD204_REG_IRQ_SOURCE				0x88

#define JESD204_REG_LINK_DISABLE			0xc0
#define JESD204_REG_LINK_STATE				0xc4
#define JESD204_REG_LINK_CLK_RATIO			0xc8

#define JESD204_REG_SYSREF_CONF				0x100
#define JESD204_REG_SYSREF_LMFC_OFFSET			0x104
#define JESD204_REG_SYSREF_STATUS			0x108

#define JESD204_REG_LANES_ENABLE			0x200
#define JESD204_REG_LINK_CONF0				0x210
#define JESD204_REG_LINK_CONF1				0x214
#define JESD204_REG_LINK_CONF2				0x240
#define JESD204_REG_LINK_STATUS				0x280

#define JESD204_RX_REG_SYNTH_ELASTIC_BUFFER_SIZE	0x40

#define JESD204_RX_REG_LANE_STATUS(x)		(((x) * 32) + 0x300)
#define JESD204_RX_REG_LANE_LATENCY(x)		(((x) * 32) + 0x304)
#define JESD204_RX_REG_ILAS(x, y)		(((x) * 32 + (y) * 4) + 0x310)

#define JESD204_RX_MAGIC (('2' << 24) | ('0' << 16) | ('4' << 8) | ('R'))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	EXTERN,
	INTERN
} sys_ref_type;

typedef struct {
  uint32_t		base_address;
  uint8_t		rx_tx_n;
  uint8_t		scramble_enable;
  uint8_t		lanes_per_device;
  uint8_t		octets_per_frame;
  uint8_t		frames_per_multiframe;
  uint8_t		converters_per_device;
  uint8_t		resolution;
  uint8_t		bits_per_sample;
  uint8_t		high_density;
  uint8_t		subclass_mode;
  sys_ref_type		sysref_type;
  uint32_t		sysref_gpio_pin;
} jesd_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t jesd_read(jesd_core *jesd, uint32_t reg_addr, uint32_t *reg_data);
int32_t jesd_write(jesd_core *jesd, uint32_t reg_addr, uint32_t reg_data);

int32_t jesd_setup(jesd_core *jesd);
int32_t jesd_status(jesd_core *jesd);
int32_t axi_jesd204_rx_status_read(jesd_core *jesd);
int32_t axi_jesd204_tx_status_read(jesd_core *jesd);
int32_t axi_jesd204_rx_laneinfo_read(jesd_core *jesd, uint32_t lane);
int32_t jesd_sysref_control(jesd_core *jesd, uint32_t enable);

#endif
