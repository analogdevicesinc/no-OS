/***************************************************************************//**
 * @file dac_core.c
 * @brief Implementation of DAC Core Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "dac_core.h"

/***************************************************************************//**
 * @brief dac_read
 *******************************************************************************/
int32_t dac_read(struct dac_core *core,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core->base_address + 0x4000 + reg_addr));

	if (*reg_data == 0xDEADDEAD) {
		#ifdef DEBUG
			ad_printf("dac_read faild for register: %x", reg_addr);
		#endif
		return -1;
	} else {
		return 0;
	}
}

/***************************************************************************//**
 * @brief dac_write
 *******************************************************************************/
int32_t dac_write(struct dac_core *core,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	ad_reg_write((core->base_address + 0x4000 + reg_addr), reg_data);

		#ifdef DEBUG
			uint32_t reg_data_r;
			return dac_read(core, reg_addr, &reg_data_r);
		#endif

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_frequency
 *******************************************************************************/

// freq is in Hz (i.e. set to 1*1000*1000 for 1 MHz)

int32_t dac_set_frequency(struct dac_core *core, int32_t chan, uint32_t freq)
{
	int32_t i;
	uint32_t reg;
	uint64_t dac_tone;
	uint64_t dac_clk;
	struct dac_channel *dac_chan;

	dac_read(core, DAC_REG_CLK_FREQ, &reg);
	dac_clk = reg * ((100*1000*1000)/65535);
	dac_read(core, DAC_REG_CLK_RATIO, &reg);
	dac_clk *= reg;
	dac_tone = (uint64_t) freq * 0xFFFFULL;
	dac_tone = dac_tone / dac_clk;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	for (i = 0; i < core->no_of_channels; i++) {
		if ((chan < 0) || (chan == i)) {
			dac_read(core, DAC_REG_DDS_INIT_INCR((i*2)+0), &reg);
			reg = (reg & ~DAC_DDS_INCR(~0)) | DAC_DDS_INCR(dac_tone) | 1;
			dac_write(core, DAC_REG_DDS_INIT_INCR((i*2)+0), reg);
			dac_read(core, DAC_REG_DDS_INIT_INCR((i*2)+1), &reg);
			reg = (reg & ~DAC_DDS_INCR(~0)) | DAC_DDS_INCR(dac_tone) | 1;
			dac_write(core, DAC_REG_DDS_INIT_INCR((i*2)+1), reg);
			dac_chan = &core->channels[i];
			dac_chan->dds_frequency_0 = freq;
			dac_chan->dds_frequency_1 = freq;
		}
	}
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);
	return(0);
}

/***************************************************************************//**
 * @brief dds_set_phase
 *******************************************************************************/

// phase is in milli(?) angles scaled to 1000 (i.e. 90*1000 is 90 degrees (pi/2))

int32_t dac_set_phase(struct dac_core *core, int32_t chan, uint32_t phase)
{
	int32_t i;
	uint32_t reg;
	uint64_t dac_phase;
	struct dac_channel *dac_chan;

	dac_phase = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	dac_phase = dac_phase / 360000;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	for (i = 0; i < core->no_of_channels; i++) {
		if ((chan < 0) || (chan == i)) {
			dac_read(core, DAC_REG_DDS_INIT_INCR((i*2)+0), &reg);
			reg = (reg & ~DAC_DDS_INIT(~0)) | DAC_DDS_INIT(dac_phase);
			dac_write(core, DAC_REG_DDS_INIT_INCR((i*2)+0), reg);
			dac_read(core, DAC_REG_DDS_INIT_INCR((i*2)+1), &reg);
			reg = (reg & ~DAC_DDS_INIT(~0)) | DAC_DDS_INIT(dac_phase);
			dac_write(core, DAC_REG_DDS_INIT_INCR((i*2)+1), reg);
			dac_chan = &core->channels[i];
			dac_chan->dds_phase_0 = phase;
			dac_chan->dds_phase_1 = phase;
		}
	}
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);
	return(0);
}

/***************************************************************************//**
 * @brief dds_set_scale
 *******************************************************************************/

// scale is in micro units (i.e. 1*1000*1000 is 1.0)
// scale range is (-1.99 to +1.99) - hardware supports -2.0, but we ignore it.
// do not get fancy, above 1.0 (or below -1.0) will mess the dds tones.
// dds always runs 16bits-- so unless your data path handles it- stay within -1 to +1.

int32_t dac_set_scale(struct dac_core *core, int32_t chan, int32_t scale_micro_units)
{
	int32_t i;
	uint32_t dac_scale;
	struct dac_channel *dac_chan;

	dac_scale = scale_micro_units;
	if (scale_micro_units < 0)
		dac_scale = scale_micro_units * -1;
	if (dac_scale >= 1999000)
		dac_scale = 1999000;
	dac_scale = (uint32_t)(((uint64_t)dac_scale * 0x4000) / 1000000);
	if (scale_micro_units < 0)
		dac_scale = dac_scale | 0x8000;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	for (i = 0; i < core->no_of_channels; i++) {
		if ((chan < 0) || (chan == i)) {
			dac_write(core, DAC_REG_DDS_SCALE((i*2)+0), DAC_DDS_SCALE(dac_scale));
			dac_write(core, DAC_REG_DDS_SCALE((i*2)+1), DAC_DDS_SCALE(dac_scale));
			dac_chan = &core->channels[i];
			dac_chan->dds_scale_0 = scale_micro_units;
			dac_chan->dds_scale_1 = scale_micro_units;
		}
	}
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);
	return(0);
}

/***************************************************************************//**
 * @brief dac_data_src_sel
 *******************************************************************************/

int32_t dac_set_source(struct dac_core *core, int32_t chan, dac_data_src src)
{
	int32_t i;
	struct dac_channel *dac_chan;

	dac_write(core, DAC_REG_SYNC_CONTROL, 0);
	for (i = 0; i < core->no_of_channels; i++) {
		if ((chan < 0) || (chan == i)) {
			dac_write(core, DAC_REG_DATA_SELECT(i), src);
			dac_chan = &core->channels[i];
			dac_chan->sel = src;
		}
	}
	dac_write(core, DAC_REG_SYNC_CONTROL, DAC_SYNC);
	return(0);
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/
int32_t dac_setup(struct dac_core *core)
{
	uint32_t reg_data;
	uint32_t dac_clock;

	dac_write(core, DAC_REG_RSTN, 0x00);
	dac_write(core, DAC_REG_TIMER, 0x20000);
	dac_write(core, DAC_REG_RSTN, 0x03);

	reg_data = 1;
	while (reg_data > 0) {
		dac_read(core, DAC_REG_TIMER, &reg_data);
	}

	dac_read(core, DAC_REG_STATUS, &reg_data);
	if (reg_data == 0x0) {
		ad_printf("%s DAC Core Status errors.\n", __func__);
		return -1;
	}

	dac_read(core, DAC_REG_CLK_FREQ, &dac_clock);
	dac_read(core, DAC_REG_CLK_RATIO, &reg_data);
	dac_clock = (dac_clock * reg_data * 100) + 0x7fff;
	dac_clock = dac_clock >> 16;

	ad_printf("%s dac core initialized (%d MHz).\n", __func__, dac_clock);

	dac_channel_setup(core);
	return 0;
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/
int32_t dac_channel_init(struct dac_core *core)
{
	int32_t i;
	struct dac_channel *dac_chan;

	/* initialize crazy stuff (ideally setup frequencies based on clock) */

	for (i = 0; i < core->no_of_channels; i++) {
		dac_chan = &core->channels[i];
		dac_chan->dds_frequency_0 = (i+1)*11*1000*1000 + 1*1000*1000;
		dac_chan->dds_phase_0 = ((i%2) == 0) ? 0*1000 : 90*1000;
		dac_chan->dds_scale_0 = 500*1000;
		dac_chan->dds_frequency_1 = (i+1)*11*1000*1000 + 1*1000*1000;
		dac_chan->dds_phase_1 = ((i%2) == 0) ? 0*1000 : 90*1000;
		dac_chan->dds_scale_1 = 500*1000;
		dac_chan->dds_dual_tone = 1;
		dac_chan->pat_data = ((i%4) == 0) ? 0x93928180 : (((i%4) == 1) ? 0xb7b6a5a4 :
			(((i%4) == 2) ? 0xdbdac9c8 : 0xf0f1e2e3));
		dac_chan->sel = DAC_SRC_DDS;
	}

	return(0);
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/

int32_t dac_channel_setup(struct dac_core *core)
{
	int32_t i;
	struct dac_channel *dac_chan;

	for (i = 0; i < core->no_of_channels; i++) {
		dac_chan = &core->channels[i];
		dac_set_frequency(core, i, dac_chan->dds_frequency_0);
		dac_set_phase(core, i, dac_chan->dds_phase_0);
		dac_set_scale(core, i, dac_chan->dds_scale_0);
		dac_write(core, DAC_REG_DATA_PATTERN(i), dac_chan->pat_data);
		dac_set_source(core, i, dac_chan->sel);
	}
	return(0);
}
