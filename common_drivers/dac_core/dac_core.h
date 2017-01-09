/***************************************************************************//**
* @file dac_core.h
* @brief Header file of DAC Core Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014-2016(c) Analog Devices, Inc.
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
#ifndef DAC_CORE_H_
#define DAC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <altxil_base.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DAC_REG_VERSION					0x0000
#define DAC_VERSION(x)					(((x) & 0xffffffff) << 0)
#define DAC_VERSION_IS(x,y,z)			((x) << 16 | (y) << 8 | (z))
#define DAC_PCORE_VERSION_MAJOR(x)		((x) >> 16)

#define DAC_REG_ID						0x0004
#define DAC_ID(x)						(((x) & 0xffffffff) << 0)

#define DAC_REG_SCRATCH					0x0008
#define DAC_SCRATCH(x)					(((x) & 0xffffffff) << 0)

#define DAC_REG_RSTN					0x040
#define DAC_MMCM_RSTN					(1 << 1)
#define DAC_RSTN						(1 << 0)

#define DAC_REG_CNTRL_1					0x0044
#define DAC_ENABLE						(1 << 0) /* v7.0 */
#define DAC_SYNC						(1 << 0) /* v8.0 */

#define DAC_REG_CNTRL_2					0x0048
#define DAC_PAR_TYPE					(1 << 7)
#define DAC_PAR_ENB						(1 << 6)
#define DAC_R1_MODE						(1 << 5)
#define DAC_DATA_FORMAT					(1 << 4)
#define DAC_DATA_SEL(x)					(((x) & 0xF) << 0) /* v7.0 */
#define DAC_TO_DATA_SEL(x)				(((x) >> 0) & 0xF) /* v7.0 */

#define DAC_REG_RATECNTRL				0x004C
#define DAC_RATE(x)						(((x) & 0xFF) << 0)
#define DAC_TO_RATE(x)					(((x) >> 0) & 0xFF)

#define DAC_REG_CLK_FREQ				0x0054
#define DAC_CLK_FREQ(x)					(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_FREQ(x)				(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_CLK_RATIO				0x0058
#define DAC_CLK_RATIO(x)				(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_RATIO(x)				(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_STATUS					0x005C
#define DAC_MUX_PN_ERR					(1 << 3)
#define DAC_MUX_PN_OOS					(1 << 2)
#define DAC_MUX_OVER_RANGE				(1 << 1)
#define DAC_STATUS						(1 << 0)

#define DAC_REG_CHAN_CNTRL_1_IIOCHAN(x)	(0x0400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_SCALE(x)				(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_SCALE(x)				(((x) >> 0) & 0xFFFF)

#define DAC_REG_CHAN_CNTRL_2_IIOCHAN(x)	(0x0404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_INIT(x)					(((x) & 0xFFFF) << 16)
#define DAC_TO_DDS_INIT(x)				(((x) >> 16) & 0xFFFF)
#define DAC_DDS_INCR(x)					(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_INCR(x)				(((x) >> 0) & 0xFFFF)

#define DAC_REG_CHAN_CNTRL_7(c)			(0x0418 + (c) * 0x40) /* v8.0 */
#define DAC_DAC_DDS_SEL(x)				(((x) & 0xF) << 0)
#define DAC_TO_DAC_DDS_SEL(x)			(((x) >> 0) & 0xF)

#define DAC_DMAC_REG_IRQ_MASK			0x80
#define DAC_DMAC_REG_IRQ_PENDING		0x84
#define DAC_DMAC_REG_IRQ_SOURCE			0x88
#define DAC_DMAC_REG_CTRL				0x400
#define DAC_DMAC_REG_TRANSFER_ID		0x404
#define DAC_DMAC_REG_START_TRANSFER		0x408
#define DAC_DMAC_REG_FLAGS				0x40c
#define DAC_DMAC_REG_DEST_ADDRESS		0x410
#define DAC_DMAC_REG_SRC_ADDRESS		0x414
#define DAC_DMAC_REG_X_LENGTH			0x418
#define DAC_DMAC_REG_Y_LENGTH			0x41c
#define DAC_DMAC_REG_DEST_STRIDE		0x420
#define DAC_DMAC_REG_SRC_STRIDE			0x424
#define DAC_DMAC_REG_TRANSFER_DONE		0x428
#define DAC_DMAC_REG_ACTIVE_TRANSFER_ID 0x42c
#define DAC_DMAC_REG_STATUS				0x430
#define DAC_DMAC_REG_CURRENT_DEST_ADDR	0x434
#define DAC_DMAC_REG_CURRENT_SRC_ADDR	0x438
#define DAC_DMAC_REG_DBG0				0x43c
#define DAC_DMAC_REG_DBG1				0x440

#define DAC_DMAC_ENABLE					(1 << 0)
#define DAC_DMAC_PAUSE					(1 << 1)

#define DAC_DMAC_IRQ_SOT				(1 << 0)
#define DAC_DMAC_IRQ_EOT				(1 << 1)

#define DAC_DMAC_FLAGS_CYCLIC			(1 << 0)
#define DAC_DMAC_FLAGS_TLAST			(1 << 1)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t dac_baseaddr;
	uint32_t dmac_baseaddr;
	uint8_t	 no_of_channels;
	uint8_t	 resolution;
	uint8_t	 fifo_present;
} dac_core;

typedef enum {
	DAC_SRC_DDS,
	DAC_SRC_SED,
	DAC_SRC_DMA,
	DAC_SRC_ZERO,	// Output 0
	DAC_SRC_PN7,
	DAC_SRC_PN15,
	DAC_SRC_PN23,
	DAC_SRC_PN31,
	DAC_SRC_LB,		// Loopback data (ADC)
	DAC_SRC_PNXX,	// Device specific
} dac_data_src;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t dac_read(dac_core core,
				 uint32_t reg_addr,
				 uint32_t *reg_data);
int32_t dac_write(dac_core core,
				  uint32_t reg_addr,
				  uint32_t reg_data);
int32_t dac_dmac_read(dac_core core,
					  uint32_t reg_addr,
					  uint32_t *reg_data);
int32_t dac_dmac_write(dac_core core,
					   uint32_t reg_addr,
					   uint32_t reg_data);
int32_t dac_setup(dac_core core);
int32_t dds_set_frequency(dac_core core,
						  uint32_t chan,
						  uint32_t freq);
int32_t dds_set_phase(dac_core core,
					  uint32_t chan,
					  uint32_t phase);
int32_t dds_set_scale(dac_core core,
					  uint32_t chan,
					  int32_t scale_micro_units);
int32_t dac_data_src_sel(dac_core core,
						 int32_t chan,
						 dac_data_src src);
int32_t dac_dma_setup(dac_core core,
					  uint32_t start_address);
#endif
