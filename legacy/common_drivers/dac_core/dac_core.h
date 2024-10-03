/***************************************************************************//**
 * @file dac_core.h
 * @brief Header file of DAC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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
#ifndef DAC_CORE_H_
#define DAC_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DAC_REG_VERSION				0x0000
#define DAC_VERSION(x)				(((x) & 0xffffffff) << 0)
#define DAC_VERSION_IS(x,y,z)			((x) << 16 | (y) << 8 | (z))
#define DAC_PCORE_VERSION_MAJOR(x)		((x) >> 16)

#define DAC_REG_ID				0x0004
#define DAC_ID(x)				(((x) & 0xffffffff) << 0)

#define DAC_REG_SCRATCH				0x0008
#define DAC_SCRATCH(x)				(((x) & 0xffffffff) << 0)

#define DAC_REG_RSTN				0x040
#define DAC_MMCM_RSTN				(1 << 1)
#define DAC_RSTN				(1 << 0)

#define DAC_REG_SYNC_CONTROL			0x0044
#define DAC_SYNC				(1 << 0) /* v8.0 */

#define DAC_REG_DATA_CONTROL			0x0048
#define DAC_PAR_TYPE				(1 << 7)
#define DAC_PAR_ENB				(1 << 6)
#define DAC_R1_MODE				(1 << 5)
#define DAC_DATA_FORMAT				(1 << 4)
#define DAC_DATA_SEL(x)				(((x) & 0xF) << 0) /* v7.0 */
#define DAC_TO_DATA_SEL(x)			(((x) >> 0) & 0xF) /* v7.0 */

#define DAC_REG_RATECNTRL			0x004C
#define DAC_RATE(x)				(((x) & 0xFF) << 0)
#define DAC_TO_RATE(x)				(((x) >> 0) & 0xFF)

#define DAC_REG_FRAME_CONTROL			0x0050
#define DAC_FRAME				(1 << 0) /* v8.0 */

#define DAC_REG_CLK_FREQ			0x0054
#define DAC_CLK_FREQ(x)				(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_FREQ(x)			(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_CLK_RATIO			0x0058
#define DAC_CLK_RATIO(x)			(((x) & 0xFFFFFFFF) << 0)
#define DAC_TO_CLK_RATIO(x)			(((x) >> 0) & 0xFFFFFFFF)

#define DAC_REG_STATUS				0x005C
#define DAC_MUX_PN_ERR				(1 << 3)
#define DAC_MUX_PN_OOS				(1 << 2)
#define DAC_MUX_OVER_RANGE			(1 << 1)
#define DAC_STATUS				(1 << 0)

#define DAC_REG_DDS_SCALE(x)		        (0x0400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_SCALE(x)			(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_SCALE(x)			(((x) >> 0) & 0xFFFF)

#define DAC_REG_DDS_INIT_INCR(x)		(0x0404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define DAC_DDS_INIT(x)				(((x) & 0xFFFF) << 16)
#define DAC_TO_DDS_INIT(x)			(((x) >> 16) & 0xFFFF)
#define DAC_DDS_INCR(x)				(((x) & 0xFFFF) << 0)
#define DAC_TO_DDS_INCR(x)			(((x) >> 0) & 0xFFFF)

#define DAC_REG_DATA_PATTERN(c)			(0x0410 + (c) * 0x40) /* v8.0 */

#define DAC_REG_DATA_SELECT(c)			(0x0418 + (c) * 0x40) /* v8.0 */
#define DAC_DATA_SELECT(x)			(((x) & 0xF) << 0)
#define DAC_TO_DATA_SELECT(x)			(((x) >> 0) & 0xF)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	DAC_SRC_DDS,
	DAC_SRC_SED,
	DAC_SRC_DMA,
	DAC_SRC_ZERO,	// Output 0
	DAC_SRC_PN7,
	DAC_SRC_PN15,
	DAC_SRC_PN23,
	DAC_SRC_PN31,
	DAC_SRC_LB,	// Loopback data (ADC)
	DAC_SRC_PNXX,	// Device specific
} dac_data_src;

typedef struct {
	uint32_t dds_frequency_0;       // in hz (1000*1000 for MHz)
	uint32_t dds_phase_0;           // in milli(?) angles (90*1000 for 90 degrees = pi/2)
	int32_t dds_scale_0;            // in micro units (1.0*1000*1000 is 1.0)
	uint32_t dds_frequency_1;       // in hz (1000*1000 for MHz)
	uint32_t dds_phase_1;           // in milli(?) angles (90*1000 for 90 degrees = pi/2)
	int32_t dds_scale_1;            // in micro units (1.0*1000*1000 is 1.0)
	uint32_t dds_dual_tone;         // if using single tone for this channel, set to 0x0
	uint32_t pat_data;              // if using SED/debug that sort of thing
	dac_data_src sel;               // set to one of the enumerated type above.
} dac_channel;

typedef struct {
	uint32_t base_address;
	uint8_t	 resolution;
	uint8_t	 no_of_channels;
	dac_channel *channels;
} dac_core;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t dac_read(dac_core *core, uint32_t reg_addr, uint32_t *reg_data);
int32_t dac_write(dac_core *core, uint32_t reg_addr, uint32_t reg_data);

int32_t dac_setup(dac_core *core);
int32_t dac_data_setup(dac_core *core);

int32_t dds_set_frequency(dac_core *core, uint32_t chan, uint32_t freq);
int32_t dds_set_phase(dac_core *core, uint32_t chan, uint32_t phase);
int32_t dds_set_scale(dac_core *core, uint32_t chan, int32_t scale_micro_units);
int32_t dac_data_src_sel(dac_core *core, int32_t chan, dac_data_src src);

#endif
