/***************************************************************************//**
 *   @file   dac_core.h
 *   @brief  Header file of DAC Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
#ifndef DAC_CORE_API_H_
#define DAC_CORE_API_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad9361.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADI_REG_VERSION			0x0000
#define ADI_VERSION(x)			(((x) & 0xffffffff) << 0)
#define VERSION_IS(x,y,z)		((x) << 16 | (y) << 8 | (z))
#define ADI_REG_ID				0x0004
#define ADI_ID(x)				(((x) & 0xffffffff) << 0)
#define ADI_REG_SCRATCH			0x0008
#define ADI_SCRATCH(x)			(((x) & 0xffffffff) << 0)

#define PCORE_VERSION_MAJOR(version)	(version >> 16)

#define ADI_REG_RSTN			0x0040
#define ADI_RSTN				(1 << 0)
#define ADI_MMCM_RSTN 			(1 << 1)

#define ADI_REG_RATECNTRL		0x004C
#define ADI_RATE(x)				(((x) & 0xFF) << 0)
#define ADI_TO_RATE(x)			(((x) >> 0) & 0xFF)

#define ADI_REG_CNTRL_1			0x0044
#define ADI_ENABLE				(1 << 0) /* v7.0 */
#define ADI_SYNC				(1 << 0) /* v8.0 */

#define ADI_REG_CNTRL_2			0x0048
#define ADI_PAR_TYPE			(1 << 7)
#define ADI_PAR_ENB				(1 << 6)
#define ADI_R1_MODE				(1 << 5)
#define ADI_DATA_FORMAT			(1 << 4)
#define ADI_DATA_SEL(x)			(((x) & 0xF) << 0) /* v7.0 */
#define ADI_TO_DATA_SEL(x)		(((x) >> 0) & 0xF) /* v7.0 */

#define ADI_REG_VDMA_FRMCNT		0x0084
#define ADI_VDMA_FRMCNT(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_VDMA_FRMCNT(x)	(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_VDMA_STATUS		0x0088
#define ADI_VDMA_OVF			(1 << 1)
#define ADI_VDMA_UNF			(1 << 0)

enum dds_data_select {
	DATA_SEL_DDS,
	DATA_SEL_SED,
	DATA_SEL_DMA,
	DATA_SEL_ZERO,	/* OUTPUT 0 */
	DATA_SEL_PN7,
	DATA_SEL_PN15,
	DATA_SEL_PN23,
	DATA_SEL_PN31,
	DATA_SEL_LB,	/* loopback data (ADC) */
	DATA_SEL_PNXX,	/* (Device specific) */
};

#define ADI_REG_CHAN_CNTRL_1_IIOCHAN(x)	(0x0400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define ADI_DDS_SCALE(x)				(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_SCALE(x)				(((x) >> 0) & 0xFFFF)

#define ADI_REG_CHAN_CNTRL_2_IIOCHAN(x)	(0x0404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define ADI_DDS_INIT(x)					(((x) & 0xFFFF) << 16)
#define ADI_TO_DDS_INIT(x)				(((x) >> 16) & 0xFFFF)
#define ADI_DDS_INCR(x)					(((x) & 0xFFFF) << 0)
#define ADI_TO_DDS_INCR(x)				(((x) >> 0) & 0xFFFF)

#define DDS_CHAN_TX1_I_F1	0
#define DDS_CHAN_TX1_I_F2	1
#define DDS_CHAN_TX1_Q_F1	2
#define DDS_CHAN_TX1_Q_F2	3
#define DDS_CHAN_TX2_I_F1	4
#define DDS_CHAN_TX2_I_F2	5
#define DDS_CHAN_TX2_Q_F1	6
#define DDS_CHAN_TX2_Q_F2	7

#define AXI_DMAC_REG_IRQ_MASK			0x80
#define AXI_DMAC_REG_IRQ_PENDING		0x84
#define AXI_DMAC_REG_IRQ_SOURCE			0x88

#define AXI_DMAC_REG_CTRL				0x400
#define AXI_DMAC_REG_TRANSFER_ID		0x404
#define AXI_DMAC_REG_START_TRANSFER		0x408
#define AXI_DMAC_REG_FLAGS				0x40c
#define AXI_DMAC_REG_DEST_ADDRESS		0x410
#define AXI_DMAC_REG_SRC_ADDRESS		0x414
#define AXI_DMAC_REG_X_LENGTH			0x418
#define AXI_DMAC_REG_Y_LENGTH			0x41c
#define AXI_DMAC_REG_DEST_STRIDE		0x420
#define AXI_DMAC_REG_SRC_STRIDE			0x424
#define AXI_DMAC_REG_TRANSFER_DONE		0x428
#define AXI_DMAC_REG_ACTIVE_TRANSFER_ID 0x42c
#define AXI_DMAC_REG_STATUS				0x430
#define AXI_DMAC_REG_CURRENT_DEST_ADDR	0x434
#define AXI_DMAC_REG_CURRENT_SRC_ADDR	0x438
#define AXI_DMAC_REG_DBG0				0x43c
#define AXI_DMAC_REG_DBG1				0x440

#define AXI_DMAC_CTRL_ENABLE			(1 << 0)
#define AXI_DMAC_CTRL_PAUSE				(1 << 1)

#define AXI_DMAC_IRQ_SOT				(1 << 0)
#define AXI_DMAC_IRQ_EOT				(1 << 1)

struct dds_state
{
	uint32_t	cached_freq[8];
	uint32_t	cached_phase[8];
	double		cached_scale[8];
	uint32_t	*dac_clk;
	uint32_t	pcore_version;
	uint32_t	num_dds_channels;
	bool		enable;
};

#define ADI_REG_CHAN_CNTRL_7(c)		(0x0418 + (c) * 0x40) /* v8.0 */
#define ADI_DAC_DDS_SEL(x)		(((x) & 0xF) << 0)
#define ADI_TO_DAC_DDS_SEL(x)		(((x) >> 0) & 0xF)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void dac_init(struct ad9361_rf_phy *phy, uint8_t data_sel);
void dds_set_frequency(uint32_t chan, uint32_t freq);
void dds_set_phase(uint32_t chan, uint32_t phase);
void dds_set_scale(uint32_t chan, double scale);
void dds_update(void);
int dac_datasel(int32_t chan, enum dds_data_select sel);

#endif
