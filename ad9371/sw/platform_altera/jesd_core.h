/***************************************************************************//**
 * @file jesd_core.h
 * @brief Header file of JESD Core Driver.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <system.h>
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Registers Description */

/* ADXCVR Registers */

#define ADXCVR_REG_VERSION				0x0000
#define VERSION(major, minor, letter)	((major << 16) | (minor << 8) | letter)
#define VERSION_MAJOR(version)			(version >> 16)
#define VERSION_MINOR(version)			((version >> 8) & 0xff)
#define VERSION_LETTER(version)			(version & 0xff)

#define ADXCVR_REG_ID				0x0004

#define ADXCVR_REG_SCRATCH			0x0008

#define ADXCVR_REG_RESETN			0x0010
#define ADXCVR_RESETN				(1 << 0)

#define ADXCVR_REG_STATUS			0x0014
#define ADXCVR_STATUS				(1 << 0)

#define ADXCVR_REG_STATUS2			0x0018
#define ADXCVR_STATUS2_XCVR(x) 		(1 << x)

/* XCVR Registers */

#define XCVR_REG_ARBITRATION(link)		(0x000 | ((link) << 16))
#define XCVR_ARBITRATION_MASK			0xFF
#define XCVR_ARBITRATION_GET_AVMM		0x02
#define XCVR_ARBITRATION_RELEASE_AVMM	0x01

#define XCVR_REG_CALIB_ATX_PLL_EN(link)	(0x400 | ((link) << 16))
#define XCVR_CALIB_ATX_PLL_EN_MASK		0x01
#define XCVR_CALIB_ATX_PLL_EN			0x01

#define XCVR_REG_CAPAB_ATX_PLL_STAT(link)	(0xA00 | ((link) << 16))
#define XCVR_CAPAB_ATX_PLL_CAL_BSY_MASK		0x02
#define XCVR_CAPAB_ATX_PLL_CAL_DONE			0x00

#define XCVR_REG_CALIB_PMA_EN(link)		(0x400 | ((link) << 16))
#define XCVR_CALIB_TX_TERM_VOD_MASK		0x20
#define XCVR_CALIB_TX_TERM_VOD_EN		0x20
#define XCVR_CALIB_CMU_CDR_PLL_EN_MASK	0x02
#define XCVR_CALIB_CMU_CDR_PLL_EN		0x02

#define XCVR_REG_CAPAB_PMA(link)		(0xA04 | ((link) << 16))
#define XCVR_CAPAB_RX_CAL_BUSY_EN_MASK	0x20
#define XCVR_CAPAB_RX_CAL_BUSY_EN		0x20
#define XCVR_CAPAB_RX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_RX_CAL_BUSY_MASK		0x02
#define XCVR_CAPAB_RX_CAL_DONE			0x00
#define XCVR_CAPAB_TX_CAL_BUSY_EN_MASK	0x10
#define XCVR_CAPAB_TX_CAL_BUSY_EN		0x10
#define XCVR_CAPAB_TX_CAL_BUSY_DIS		0x00
#define XCVR_CAPAB_TX_CAL_BUSY_MASK		0x01
#define XCVR_CAPAB_TX_CAL_DONE			0x00

#define XCVR_REG_RATE_SWITCH_FLAG(link)			(0x598 | ((link) << 16))
#define XCVR_RATE_SWITCH_FLAG_MASK				0x80
#define XCVR_RATE_SWITCH_FLAG_NO_RATE_SWITCH	0x80

/* JESD204 Registers */

#define JESD204_REG_LINK_DISABLE	0xc0
#define JESD204_REG_LINK_CONF0		0x210

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_addr;
} jesd_device;

typedef struct {
	uint32_t 	xcvr_base_addr;
	uint32_t	atx_pll_base_addr;
	uint32_t 	adxcfg_base_addr[4];
	uint8_t		link_num;
	uint8_t 	lanes_per_link;
} xcvr_device;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
uint8_t atx_pll_calib(xcvr_device *dev);
 uint8_t xcvr_calib_tx(xcvr_device *dev);
int32_t xcvr_setup(mykonosDevice_t *myk_dev);
int32_t jesd_setup(mykonosDevice_t *myk_dev);
int32_t jesd_tx_enable(mykonosDevice_t *myk_dev);
int32_t jesd_rx_enable(mykonosDevice_t *myk_dev);

#endif /* JESD_CORE_H_ */
