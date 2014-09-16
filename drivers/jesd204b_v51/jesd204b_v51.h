/***************************************************************************//**
* @file jesd204b_v51.c
* @brief Header file of V51 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#ifndef JESD204B_V51_H_
#define JESD204B_V51_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define JESD204B_REG_VERSION			0x000
#define JESD204B_VERSION_MAJOR(x)		(((x) >> 24) & 0xFF)
#define JESD204B_VERSION_MINOR(x)		(((x) >> 16) & 0xFF)
#define JESD204B_VERSION_REV(x)			(((x) >> 8) & 0xFF)

#define JESD204B_REG_RESET 				0x004
#define JESD204B_RESET					(1 << 0)

#define JESD204B_REG_ILA_CTRL			0x008
#define JESD204B_ILA_EN 				(1 << 0)

#define JESD204B_REG_SCR_CTRL			0x00C
#define JESD204B_SCR_EN					(1 << 0)

#define JESD204B_REG_SYSREF_CTRL		0x010
#define JESD204B_ALWAYS_SYSREF_EN		(1 << 0)

#define JESD204B_REG_ILA_MFC			0x014
#define JESD204B_ILA_MFC(x)				(((x) - 1) & 0xFF)	/* TX only 4..256 */

#define JESD204B_REG_TEST_MODE_SEL		0x018
#define JESD204B_TEST_MODE_OFF			0	/* Normal operation */
#define JESD204B_TEST_MODE_K28_5		1	/* Send/Receive /K28.5/ indefinitely */
#define JESD204B_TEST_MODE_ILA			2	/* Synchronize as normal then send/receive repeated ILA sequences */
#define JESD204B_TEST_MODE_D21_5		3	/* Send/Receive /D21.5/ indefinitely */
#define JESD204B_TEST_MODE_RPAT			5	/* Send/Receive modified random pattern (RPAT) */
#define JESD204B_TEST_MODE_JSPAT		7	/* Send/Receive a scrambled jitter pattern (JSPAT) */

#define JESD204B_REG_SYNC_ERR_STAT				0x01C	/* RX only */
#define JESD204B_SYNC_ERR_NOT_IN_TAB(lane)		(1 << (0 + (lane) * 3))
#define JESD204B_SYNC_ERR_DISPARITY(lane)		(1 << (1 + (lane) * 3))
#define JESD204B_SYNC_ERR_UNEXPECTED_K(lane)	(1 << (2 + (lane) * 3))

#define JESD204B_REG_OCTETS_PER_FRAME 	0x020
#define JESD204B_OCTETS_PER_FRAME(x)	(((x) - 1) & 0xFF)	/* 1..256 */

#define JESD204B_REG_FRAMES_PER_MFRAME	0x024
#define JESD204B_FRAMES_PER_MFRAME(x)	(((x) - 1) & 0x1F)	/* 1..32 */

#define JESD204B_REG_LANES				0x028
#define JESD204B_LANES(x)				(((x) - 1) & 0x1F)	/* 1..32 */

#define JESD204B_REG_SUBCLASS			0x02C

#define JESD204B_REG_RX_BUF_DELAY		0x030	/* RX only */
#define JESD204B_RX_BUF_DELAY(x)		((x) & 0x1FFF)

#define JESD204B_REG_RX_LINK_CTRL		0x034	/* RX only */
#define JESD204B_LINK_TEST_EN			(1 << 0)
#define JESD204B_SYNC_ERR_REP_DIS		(1 << 8)

/* Per LANE Registers */

#define JESD204B_REG_LANE_VERSION(l)	(0x800 + ((l) * 0x40))
#define JESD204B_LANE_SUBCLASS(x)		(((x) >> 0) & 0x7)
#define JESD204B_LANE_JESDV(x)			(((x) >> 8) & 0x7)

#define JESD204B_REG_LANE_F(l)			(0x804 + ((l) * 0x40))
#define JESD204B_LANE_F(x)				((((x) >> 0) & 0xFF) + 1)

#define JESD204B_REG_LANE_K(l)			(0x808 + ((l) * 0x40))
#define JESD204B_LANE_K(x)				((((x) >> 0) & 0x1F) + 1)

#define JESD204B_REG_ID_L(l)			(0x80C + ((l) * 0x40))
#define JESD204B_LANE_DID(x)			(((x) >> 0) & 0xFF)
#define JESD204B_LANE_BID(x)			(((x) >> 8) & 0x1F)
#define JESD204B_LANE_LID(x)			(((x) >> 16) & 0x1F)
#define JESD204B_LANE_L(x)				((((x) >> 24) & 0x1F) + 1)

#define JESD204B_REG_M_N_ND_CS(l)		(0x810 + ((l) * 0x40))
#define JESD204B_LANE_M(x)				((((x) >> 0) & 0xFF) + 1)
#define JESD204B_LANE_N(x)				((((x) >> 8) & 0x1F) + 1)
#define JESD204B_LANE_ND(x)				((((x) >> 16) & 0x1F) + 1)
#define JESD204B_LANE_CS(x)				(((x) >> 24) & 0x3)

#define JESD204B_REG_SCR_S_HD_CF(l)		(0x814 + ((l) * 0x40))
#define JESD204B_LANE_SCR(x)			(((x) >> 0) & 0x1)
#define JESD204B_LANE_S(x)				((((x) >> 8) & 0x1F) + 1)
#define JESD204B_LANE_HD(x)				(((x) >> 16) & 0x1)
#define JESD204B_LANE_CF(x)				(((x) >> 24) & 0x1F)

#define JESD204B_REG_FCHK(l)			(0x818 + ((l) * 0x40))
#define JESD204B_LANE_FCHK(x)			(((x) >> 16) & 0xFF)

#define JESD204B_REG_SC2_ADJ_CTRL(l)	(0x81C + ((l) * 0x40))
#define JESD204B_LANE_ADJ_CNT(x)		(((x) >> 0) & 0xF)
#define JESD204B_LANE_PHASE_ADJ_REQ(x)	(((x) >> 8) & 0x1)
#define JESD204B_LANE_ADJ_CNT_DIR(x)	(((x) >> 16) & 0x1)

#define JESD204B_REG_TM_ERR_CNT(l)		(0x820 + ((l) * 0x40))
#define JESD204B_REG_TM_LINK_ERR_CNT(l)	(0x824 + ((l) * 0x40))
#define JESD204B_REG_TM_ILA_CNT(l)		(0x828 + ((l) * 0x40))
#define JESD204B_REG_TM_MFC_CNT(l)		(0x82C + ((l) * 0x40))
#define JESD204B_REG_TM_BUF_ADJ(l)		(0x830 + ((l) * 0x40))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct
{
	uint8_t		lanesync_enable;
	uint8_t		scramble_enable;
	uint8_t		sysref_always_enable;
	uint8_t		frames_per_multiframe;
	uint8_t		bytes_per_frame;
	uint8_t		subclass;
}jesd204b_state;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t jesd204b_read(uint32_t reg_addr, uint32_t *reg_data);
int32_t jesd204b_write(uint32_t reg_addr, uint32_t reg_data);
int32_t jesd204b_setup(uint32_t baseaddr, jesd204b_state setup_param);

#endif
