/***************************************************************************//**
 *   @file   axi_dac_core.c
 *   @brief  Driver for the Analog Devices AXI-DAC-CORE module.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "platform_drivers.h"
#include "util.h"
#include "axi_dac_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_DAC_REG_RSTN				0x40
#define AXI_DAC_MMCM_RSTN				BIT(1)
#define AXI_DAC_RSTN					BIT(0)

#define AXI_DAC_REG_SYNC_CONTROL		0x44
#define AXI_DAC_SYNC					BIT(0)

#define AXI_DAC_REG_RATECNTRL			0x4C
#define AXI_DAC_RATE(x)					(((x) & 0xFF) << 0)
#define AXI_DAC_TO_RATE(x)				(((x) >> 0) & 0xFF)

#define AXI_DAC_REG_CLK_FREQ			0x54
#define AXI_DAC_CLK_FREQ(x)				(((x) & 0xFFFFFFFF) << 0)
#define AXI_DAC_TO_CLK_FREQ(x)			(((x) >> 0) & 0xFFFFFFFF)

#define AXI_DAC_REG_CLK_RATIO			0x58
#define AXI_DAC_CLK_RATIO(x)			(((x) & 0xFFFFFFFF) << 0)
#define AXI_DAC_TO_CLK_RATIO(x)			(((x) >> 0) & 0xFFFFFFFF)

#define AXI_DAC_REG_STATUS				0x005C
#define AXI_DAC_MUX_PN_ERR				BIT(3)
#define AXI_DAC_MUX_PN_OOS				BIT(2)
#define AXI_DAC_MUX_OVER_RANGE			BIT(1)
#define AXI_DAC_STATUS					BIT(0)

#define AXI_DAC_REG_DDS_SCALE(x)		(0x400 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define AXI_DAC_DDS_SCALE(x)			(((x) & 0xFFFF) << 0)
#define AXI_DAC_TO_DDS_SCALE(x)			(((x) >> 0) & 0xFFFF)

#define AXI_DAC_REG_DDS_INIT_INCR(x)	(0x404 + ((x) >> 1) * 0x40 + ((x) & 1) * 0x8)
#define AXI_DAC_DDS_INIT(x)				(((x) & 0xFFFF) << 16)
#define AXI_DAC_TO_DDS_INIT(x)			(((x) >> 16) & 0xFFFF)
#define AXI_DAC_DDS_INCR(x)				(((x) & 0xFFFF) << 0)
#define AXI_DAC_TO_DDS_INCR(x)			(((x) >> 0) & 0xFFFF)

#define AXI_DAC_REG_DATA_SELECT(c)		(0x0418 + (c) * 0x40)
#define AXI_DAC_DATA_SELECT(x)			(((x) & 0xF) << 0)
#define AXI_DAC_TO_DATA_SELECT(x)		(((x) >> 0) & 0xF)

/***************************************************************************//**
 * @brief axi_dac_read
 *******************************************************************************/
int32_t axi_dac_read(struct axi_dac *dac,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	*reg_data = Xil_In32((dac->base + reg_addr));

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_write
 *******************************************************************************/
int32_t axi_dac_write(struct axi_dac *dac,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
	Xil_Out32((dac->base + reg_addr), reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief dds_set_frequency
 *
 * freq is in Hz (i.e. set to 1*1000*1000 for 1 MHz)
 *******************************************************************************/
int32_t axi_dac_dds_set_frequency(struct axi_dac *dac,
				  uint32_t chan, uint32_t freq_hz)
{
	uint64_t val64;
	uint32_t reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) freq_hz * 0xFFFFULL;
	val64 = val64 / dac->clock_hz;
	reg = (reg & ~AXI_DAC_DDS_INCR(~0)) | AXI_DAC_DDS_INCR(val64) | 1;
	axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_dds_set_phase
 *
 * phase is in milli angles scaled to 1000 (i.e. 90*1000 is 90 degrees (pi/2))
 *******************************************************************************/
int32_t axi_dac_dds_set_phase(struct axi_dac *dac,
			      uint32_t chan, uint32_t phase)
{
	uint64_t val64;
	uint32_t reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	val64 = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	val64 = val64 / 360000;
	reg = (reg & ~AXI_DAC_DDS_INIT(~0)) | AXI_DAC_DDS_INIT(val64);
	axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_dds_set_scale
 *
 * scale is in micro units (i.e. 1*1000*1000 is 1.0)
 *******************************************************************************/
int32_t axi_dac_dds_set_scale(struct axi_dac *dac,
			      uint32_t chan,
			      int32_t scale_micro_units)
{
	uint32_t scale_reg;

	scale_reg = scale_micro_units;
	if (scale_micro_units < 0)
		scale_reg = scale_micro_units * -1;
	if (scale_reg >= 1999000)
		scale_reg = 1999000;
	scale_reg = (uint32_t)(((uint64_t)scale_reg * 0x4000) / 1000000);
	if (scale_micro_units < 0)
		scale_reg = scale_reg | 0x8000;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_write(dac, AXI_DAC_REG_DDS_SCALE(chan),
		      AXI_DAC_DDS_SCALE(scale_reg));
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_init
 *******************************************************************************/
int32_t axi_dac_init(struct axi_dac **dac_core,
		     const struct axi_dac_init *init)
{
	struct axi_dac *dac;
	uint32_t reg_data;
	uint32_t freq;
	uint32_t ratio;
	uint8_t ch;

	dac = (struct axi_dac *)malloc(sizeof(*dac));
	if (!dac)
		return FAILURE;

	dac->name = init->name;
	dac->base = init->base;
	dac->num_channels = init->num_channels;

	axi_dac_write(dac, AXI_DAC_REG_RSTN, 0);
	axi_dac_write(dac, AXI_DAC_REG_RSTN,
		      AXI_DAC_MMCM_RSTN | AXI_DAC_RSTN);

	mdelay(100);

	axi_dac_read(dac, AXI_DAC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		printf("%s: Status errors\n", dac->name);
		goto error;
	}

	axi_dac_write(dac, AXI_DAC_REG_RATECNTRL, AXI_DAC_RATE(3));

	axi_dac_read(dac, AXI_DAC_REG_CLK_FREQ, &freq);
	axi_dac_read(dac, AXI_DAC_REG_CLK_RATIO, &ratio);
	dac->clock_hz = freq * ratio;
	dac->clock_hz = (dac->clock_hz * 390625) >> 8;

	for (ch = 0; ch < dac->num_channels; ch++) {
		axi_dac_dds_set_frequency(dac, ((ch*2)+0), 3*1000*1000);
		axi_dac_dds_set_frequency(dac, ((ch*2)+1), 3*1000*1000);
		axi_dac_dds_set_phase(dac, ((ch*2)+0), (ch % 2) ? 0 : 90000);
		axi_dac_dds_set_phase(dac, ((ch*2)+1), (ch % 2) ? 0 : 90000);
		axi_dac_dds_set_scale(dac, ((ch*2)+0), 50*1000);
		axi_dac_dds_set_scale(dac, ((ch*2)+1), 50*1000);
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((ch*2)+0), 0);
		axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((ch*2)+1), 0);
	}
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

	printf("%s: Successfully initialized (%lu Hz)\n", dac->name, dac->clock_hz);

	*dac_core = dac;

	return SUCCESS;
error:
	free(dac);

	return FAILURE;
}

/***************************************************************************//**
 * @brief axi_dac_remove
 *******************************************************************************/
int32_t axi_dac_remove(struct axi_dac *dac)
{
	free(dac);

	return SUCCESS;
}
