/***************************************************************************//**
* @file jesd_core.h
* @brief Header file of Jesd Core.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2016(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#ifndef JESD_CORE_H_
#define JESD_CORE_H_

#include "platform_drivers.h"

/******************************************************************************/
/************************ JESD204 Core Definitions ****************************/
/******************************************************************************/
#define JESD204_REG_TRX_VERSION			0x00
#define JESD204_REG_TRX_RESET				0x04
#define JESD204_REG_TRX_ILA_SUPPORT			0x08
#define JESD204_REG_TRX_SCRAMBLING			0x0c
#define JESD204_REG_TRX_SYSREF_HANDLING		0x10
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

/* JESD204_REG_RX_LINK_ERROR_STATUS */
#define JESD204_RX_LINK_LANE_ALIGN_ERR_ALARM		(1 << 31)
#define JESD204_RX_LINK_SYSREF_LMFC_ALARM		(1 << 30)
#define JESD204_RX_LINK_BUFF_OVF_ALARM			(1 << 29)
#define JESD204_RX_LINK_K_CH_ERR(l)			((1 << 2) << (3 * (l)))
#define JESD204_RX_LINK_DISP_ERR(l)			((1 << 1) << (3 * (l)))
#define JESD204_RX_LINK_NOT_IN_TBL_ERR(l)		((1 << 0) << (3 * (l)))

/* JESD204_REG_TRX_OCTETS_PER_FRAME */
#define JESD204_TRX_OCTETS_PER_FRAME(x)		(((x) - 1) & 0xff)

/* JESD204_REG_TRX_FRAMES_PER_MULTIFRAME */
#define JESD204_TRX_FRAMES_PER_MULTIFRAME(x)		(((x) - 1) & 0x1f)

/* JESD204_REG_TRX_SUBCLASS_MODE */
#define JESD204_TRX_SUBCLASS_MODE(x)			((x) & 0x3)

/* JESD204_REG_TRX_SYNC_STATUS */
#define JESD204_TRX_SYSREF_CAPTURED			(1 << 16)
#define JESD204_TRX_SYNC_ACHIEVED			(1 << 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	EXTERN,
	INTERN
} sys_ref_type;

typedef struct {
	uint32_t		base_address;
	uint8_t			rx_tx_n;
  uint8_t     scramble_enable;
	uint8_t			octets_per_frame;
	uint8_t			frames_per_multiframe;
	uint8_t			subclass_mode;
	sys_ref_type		sysref_type;
	uint32_t		sysref_gpio_pin;
} jesd_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t jesd_read(jesd_core core, uint32_t reg_addr, uint32_t *reg_data);
int32_t jesd_write(jesd_core core, uint32_t reg_addr, uint32_t reg_data);

int32_t jesd_setup(jesd_core core);
int32_t jesd_status(jesd_core core);
int32_t jesd_sysref_control(jesd_core core, uint32_t enable);

#endif
