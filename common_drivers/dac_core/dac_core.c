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
int32_t dac_read(dac_core core,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.dac_baseaddr + 0x4000 + reg_addr));

	return 0;
}

/***************************************************************************//**
 * @brief dac_write
 *******************************************************************************/
int32_t dac_write(dac_core core,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	ad_reg_write((core.dac_baseaddr + 0x4000 + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_frequency
 *******************************************************************************/
int32_t dds_set_frequency(dac_core core,
		uint32_t chan,
		uint32_t freq)
{
	uint32_t pcore_version;
	uint32_t val;
	uint64_t val64;
	uint32_t reg;
	uint64_t dac_clk;

	dac_read(core, DAC_REG_CLK_FREQ, &val);
	dac_clk = val;
	dac_read(core, DAC_REG_CLK_RATIO, &val);
	dac_clk *= val * 1525;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, 0);

	dac_read(core, DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~DAC_DDS_INCR(~0);
	val64 = (uint64_t) freq * 0xFFFFULL;
	val64 = val64 / dac_clk;
	reg |= DAC_DDS_INCR(val64) | 1;
	dac_write(core, DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, DAC_ENABLE);
	else
		dac_write(core, DAC_REG_CNTRL_1, DAC_SYNC);

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_phase
 *******************************************************************************/
int32_t dds_set_phase(dac_core core,
		uint32_t chan,
		uint32_t phase)
{
	uint32_t pcore_version;
	uint64_t val64;
	uint32_t reg;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, 0);

	dac_read(core, DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~DAC_DDS_INIT(~0);
	val64 = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	val64 = val64 / 360000;
	reg |= DAC_DDS_INIT(val64);
	dac_write(core, DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, DAC_ENABLE);
	else
		dac_write(core, DAC_REG_CNTRL_1, DAC_SYNC);

	return 0;
}

/***************************************************************************//**
 * @brief dds_set_scale
 *******************************************************************************/
int32_t dds_set_scale(dac_core core,
		uint32_t chan,
		int32_t scale_micro_units)
{
	uint32_t pcore_version;
	uint32_t scale_reg;
	uint32_t sign_part;
	uint32_t int_part;
	uint32_t fract_part;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) > 6) {
		if (scale_micro_units >= 1000000) {
			sign_part = 0;
			int_part = 1;
			fract_part = 0;
			goto set_scale_reg;
		}
		if (scale_micro_units <= -1000000) {
			sign_part = 1;
			int_part = 1;
			fract_part = 0;
			goto set_scale_reg;
		}
		if (scale_micro_units < 0) {
			sign_part = 1;
			int_part = 0;
			scale_micro_units *= -1;
		} else {
			sign_part = 0;
			int_part = 0;
		}
		fract_part = (uint32_t)(((uint64_t)scale_micro_units * 0x4000) / 1000000);
set_scale_reg:
		scale_reg = (sign_part << 15) | (int_part << 14) | fract_part;
	} else {
		if (scale_micro_units >= 1000000) {
			scale_reg = 0;
			scale_micro_units = 1000000;
		}
		if (scale_micro_units <= 0) {
			scale_reg = 0;
			scale_micro_units = 0;
		}
		fract_part = (uint32_t)(scale_micro_units);
		scale_reg = 500000 / fract_part;
	}

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, 0);

	dac_write(core, DAC_REG_CHAN_CNTRL_1_IIOCHAN(chan), DAC_DDS_SCALE(scale_reg));
	if (DAC_PCORE_VERSION_MAJOR(pcore_version) < 8)
		dac_write(core, DAC_REG_CNTRL_1, DAC_ENABLE);
	else
		dac_write(core, DAC_REG_CNTRL_1, DAC_SYNC);

	return 0;
}

/***************************************************************************//**
 * @brief dac_data_src_sel
 *******************************************************************************/
int32_t dac_data_src_sel(dac_core core,
		int32_t chan,
		dac_data_src src)
{
	uint32_t pcore_version;
	int32_t i;

	dac_read(core, DAC_REG_VERSION, &pcore_version);

	if (DAC_PCORE_VERSION_MAJOR(pcore_version) > 7)
		if (chan < 0) /* ALL */
			for (i = 0; i < (core.no_of_channels * 2); i++)
				dac_write(core, DAC_REG_CHAN_CNTRL_7(i), src);
		else
			dac_write(core, DAC_REG_CHAN_CNTRL_7(chan), src);
	else {
		uint32_t reg;

		switch(src) {
			case DAC_SRC_DDS:
			case DAC_SRC_SED:
			case DAC_SRC_DMA:
				dac_read(core, DAC_REG_CNTRL_2, &reg);
				reg &= ~DAC_DATA_SEL(~0);
				reg |= DAC_DATA_SEL(src);
				dac_write(core, DAC_REG_CNTRL_2, reg);
				break;
			default:
				return -1;
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief dac_setup
 *******************************************************************************/
int32_t dac_setup(dac_core core)
{
	uint32_t status;

	dac_write(core, DAC_REG_RSTN, 0x00);
	dac_write(core, DAC_REG_RSTN, 0x03);

	mdelay(100);

	dac_read(core, DAC_REG_STATUS, &status);
	if (status == 0x0) {
		ad_printf("DAC Core Status errors.\n\r");

		return -1;
	} else {
		ad_printf("DAC Core successfully initialized.\n\r");

		return 0;
	}
}
