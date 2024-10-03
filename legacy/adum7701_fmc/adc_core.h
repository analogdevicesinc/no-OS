/***************************************************************************//**
 * @file adc_core.h
 * @brief Header file of ADC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
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
#ifndef ADC_CORE_H_
#define ADC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADC_REG_ID			0x0004
#define ADC_ID(x)			(((x) & 0xffffffff) << 0)

#define ADC_REG_RSTN			0x0040
#define ADC_MMCM_RSTN			(1 << 1)
#define ADC_RSTN			(1 << 0)

#define ADC_REG_CNTRL			0x0044
#define ADC_R1_MODE			(1 << 2)
#define ADC_DDR_EDGESEL			(1 << 1)
#define ADC_PIN_MODE			(1 << 0)

#define ADC_REG_CLK_FREQ		0x0054
#define ADC_CLK_FREQ(x)			(((x) & 0xFFFFFFFF) << 0)
#define ADC_TO_CLK_FREQ(x)		(((x) >> 0) & 0xFFFFFFFF)

#define ADC_REG_CLK_RATIO		0x0058
#define ADC_CLK_RATIO(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADC_TO_CLK_RATIO(x)		(((x) >> 0) & 0xFFFFFFFF)

#define ADC_REG_STATUS			0x005C
#define ADC_MUX_PN_ERR			(1 << 3)
#define ADC_MUX_PN_OOS			(1 << 2)
#define ADC_MUX_OVER_RANGE		(1 << 1)
#define ADC_STATUS			(1 << 0)

#define ADC_REG_DELAY_CNTRL		0x0060
#define ADC_DELAY_SEL			(1 << 17)
#define ADC_DELAY_RWN			(1 << 16)
#define ADC_DELAY_ADDRESS(x) 		(((x) & 0xFF) << 8)
#define ADC_TO_DELAY_ADDRESS(x) 	(((x) >> 8) & 0xFF)
#define ADC_DELAY_WDATA(x)		(((x) & 0x1F) << 0)
#define ADC_TO_DELAY_WDATA(x)		(((x) >> 0) & 0x1F)

#define ADC_REG_CHAN_CNTRL(c)		(0x0400 + (c) * 0x40)
#define ADC_PN_SEL			(1 << 10)
#define ADC_IQCOR_ENB			(1 << 9)
#define ADC_DCFILT_ENB			(1 << 8)
#define ADC_FORMAT_SIGNEXT		(1 << 6)
#define ADC_FORMAT_TYPE			(1 << 5)
#define ADC_FORMAT_ENABLE		(1 << 4)
#define ADC_PN23_TYPE			(1 << 1)
#define ADC_ENABLE			(1 << 0)

#define ADC_REG_CHAN_STATUS(c)		(0x0404 + (c) * 0x40)
#define ADC_PN_ERR			(1 << 2)
#define ADC_PN_OOS			(1 << 1)
#define ADC_OVER_RANGE			(1 << 0)

#define ADC_REG_CHAN_CNTRL_3(c)		(0x0418 + (c) * 0x40)
#define ADC_ADC_PN_SEL(x)		(((x) & 0xF) << 16)
#define ADC_TO_ADC_PN_SEL(x)		(((x) >> 16) & 0xF)
#define ADC_ADC_DATA_SEL(x)		(((x) & 0xF) << 0)
#define ADC_TO_ADC_DATA_SEL(x)		(((x) >> 0) & 0xF)

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

enum adc_pn_sel {
	ADC_PN9 = 0,
	ADC_PN23A = 1,
	ADC_PN7 = 4,
	ADC_PN15 = 5,
	ADC_PN23 = 6,
	ADC_PN31 = 7,
	ADC_PN_CUSTOM = 9,
	ADC_PN_END = 10,
};

typedef struct {
	uint32_t base_address;
	uint8_t	 master;
	uint8_t	 no_of_channels;
	uint8_t	 resolution;
	uint32_t start_address;
} adc_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t adc_read(adc_core *core,
				 uint32_t reg_addr,
				 uint32_t *reg_data);
int32_t adc_write(adc_core *core,
				  uint32_t reg_addr,
				  uint32_t reg_data);
int32_t adc_setup(adc_core *core);
int32_t adc_set_pnsel(adc_core *core,
					  uint8_t channel,
					  enum adc_pn_sel sel);
int32_t adc_pn_mon(adc_core *core,
				   enum adc_pn_sel sel);
int32_t adc_ramp_test(adc_core *core,
					  uint8_t no_of_cores,
					  uint32_t no_of_samples,
					  uint32_t start_address);
int32_t adc_capture(adc_core *core,
					uint32_t size);
#endif
