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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
const uint16_t sine_lut_1[128] = {
		0x0000, 0x0640, 0x0C80, 0x12C0, 0x18F0, 0x1F10, 0x2520, 0x2B10,
		0x30F0, 0x36B0, 0x3C50, 0x41C0, 0x4710, 0x4C30, 0x5120, 0x55F0,
		0x5A70, 0x5ED0, 0x62E0, 0x66C0, 0x6A60, 0x6DC0, 0x70D0, 0x73A0,
		0x7630, 0x7870, 0x7A70, 0x7C20, 0x7D80, 0x7E90, 0x7F50, 0x7FD0,
		0x7FF0, 0x7FD0, 0x7F50, 0x7E90, 0x7D80, 0x7C20, 0x7A70, 0x7870,
		0x7630, 0x73A0, 0x70D0, 0x6DC0, 0x6A60, 0x66C0, 0x62E0, 0x5ED0,
		0x5A70, 0x55F0, 0x5120, 0x4C30, 0x4710, 0x41C0, 0x3C50, 0x36B0,
		0x30F0, 0x2B10, 0x2520, 0x1F10, 0x18F0, 0x12C0, 0x0C80, 0x0640,
		0x0000, 0xF9B0, 0xF370, 0xED30, 0xE700, 0xE0E0, 0xDAD0, 0xD4E0,
		0xCF00, 0xC940, 0xC3A0, 0xBE30, 0xB8E0, 0xB3C0, 0xAED0, 0xAA00,
		0xA580, 0xA120, 0x9D10, 0x9930, 0x9590, 0x9230, 0x8F20, 0x8C50,
		0x89C0, 0x8780, 0x8580, 0x83D0, 0x8270, 0x8160, 0x80A0, 0x8020,
		0x8000, 0x8020, 0x80A0, 0x8160, 0x8270, 0x83D0, 0x8580, 0x8780,
		0x89C0, 0x8C50, 0x8F20, 0x9230, 0x9590, 0x9930, 0x9D10, 0xA120,
		0xA580, 0xAA00, 0xAED0, 0xB3C0, 0xB8E0, 0xBE30, 0xC3A0, 0xC940,
		0xCF00, 0xD4E0, 0xDAD0, 0xE0E0, 0xE700, 0xED30, 0xF370, 0xF9B0,
};

const uint16_t sine_lut_2[128] = {
		0x7FF0, 0x7FD0, 0x7F50, 0x7E90, 0x7D80, 0x7C20, 0x7A70, 0x7870,
		0x7630, 0x73A0, 0x70D0, 0x6DC0, 0x6A60, 0x66C0, 0x62E0, 0x5ED0,
		0x5A70, 0x55F0, 0x5120, 0x4C30, 0x4710, 0x41C0, 0x3C50, 0x36B0,
		0x30F0, 0x2B10, 0x2520, 0x1F10, 0x18F0, 0x12C0, 0x0C80, 0x0640,
		0x0000, 0xF9B0, 0xF370, 0xED30, 0xE700, 0xE0E0, 0xDAD0, 0xD4E0,
		0xCF00, 0xC940, 0xC3A0, 0xBE30, 0xB8E0, 0xB3C0, 0xAED0, 0xAA00,
		0xA580, 0xA120, 0x9D10, 0x9930, 0x9590, 0x9230, 0x8F20, 0x8C50,
		0x89C0, 0x8780, 0x8580, 0x83D0, 0x8270, 0x8160, 0x80A0, 0x8020,
		0x8000, 0x8020, 0x80A0, 0x8160, 0x8270, 0x83D0, 0x8580, 0x8780,
		0x89C0, 0x8C50, 0x8F20, 0x9230, 0x9590, 0x9930, 0x9D10, 0xA120,
		0xA580, 0xAA00, 0xAED0, 0xB3C0, 0xB8E0, 0xBE30, 0xC3A0, 0xC940,
		0xCF00, 0xD4E0, 0xDAD0, 0xE0E0, 0xE700, 0xED30, 0xF370, 0xF9B0,
		0x0000, 0x0640, 0x0C80, 0x12C0, 0x18F0, 0x1F10, 0x2520, 0x2B10,
		0x30F0, 0x36B0, 0x3C50, 0x41C0, 0x4710, 0x4C30, 0x5120, 0x55F0,
		0x5A70, 0x5ED0, 0x62E0, 0x66C0, 0x6A60, 0x6DC0, 0x70D0, 0x73A0,
		0x7630, 0x7870, 0x7A70, 0x7C20, 0x7D80, 0x7E90, 0x7F50, 0x7FD0,
};

/***************************************************************************//**
* @brief dac_read
*******************************************************************************/
int32_t dac_read(dac_core core,
				 uint32_t reg_addr,
				 uint32_t *reg_data)
{
	*reg_data = Xil_In32((core.dac_baseaddr + 0x4000 + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief dac_write
*******************************************************************************/
int32_t dac_write(dac_core core,
				  uint32_t reg_addr,
				  uint32_t reg_data)
{
	Xil_Out32((core.dac_baseaddr + 0x4000 + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief dac_dmac_read
*******************************************************************************/
int32_t dac_dmac_read(dac_core core,
					  uint32_t reg_addr,
					  uint32_t *reg_data)
{
	*reg_data = Xil_In32(core.dmac_baseaddr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief dac_dmac_write
*******************************************************************************/
int32_t dac_dmac_write(dac_core core,
					   uint32_t reg_addr,
					   uint32_t reg_data)
{
	Xil_Out32(core.dmac_baseaddr + reg_addr, reg_data);

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
* @brief dac_dma_setup
*******************************************************************************/
int32_t dac_dma_setup(dac_core core, uint32_t start_address)
{
	uint32_t no_of_samples;
	uint32_t index;
	uint32_t index_mem = 0;

	no_of_samples = sizeof(sine_lut_1) / sizeof(typeof(sine_lut_1[0]));

	for (index = 0; index < no_of_samples; index ++) {
		switch (core.no_of_channels) {
		case 1:
			Xil_Out16(start_address + index_mem * 2, sine_lut_1[index]);
			index_mem += 1;
			break;
		case 2:
			Xil_Out16(start_address + (index_mem + 0) * 2, sine_lut_1[index]);
			Xil_Out16(start_address + (index_mem + 1) * 2, sine_lut_2[index]);
			index_mem += 2;
			break;
		case 4:
			Xil_Out16(start_address + (index_mem + 0) * 2, sine_lut_1[index]);
			Xil_Out16(start_address + (index_mem + 1) * 2, sine_lut_2[index]);
			Xil_Out16(start_address + (index_mem + 2) * 2, sine_lut_1[index]);
			Xil_Out16(start_address + (index_mem + 3) * 2, sine_lut_2[index]);
			index_mem += 4;
			break;
		default:
			xil_printf("Unsupported mode.\n\r");
			return -1;
		}
	}
	Xil_DCacheFlush();

	dac_dmac_write(core, DAC_DMAC_REG_CTRL, 0);
	dac_dmac_write(core, DAC_DMAC_REG_CTRL, DAC_DMAC_ENABLE);
	dac_dmac_write(core, DAC_DMAC_REG_SRC_ADDRESS, start_address);
	dac_dmac_write(core, DAC_DMAC_REG_SRC_STRIDE, 0x0);
	dac_dmac_write(core, DAC_DMAC_REG_X_LENGTH,
			(core.no_of_channels * sizeof(sine_lut_1)) - 1);
	dac_dmac_write(core, DAC_DMAC_REG_Y_LENGTH, 0x0);
	dac_dmac_write(core, DAC_DMAC_REG_FLAGS,
			core.fifo_present ? DAC_DMAC_FLAGS_TLAST : DAC_DMAC_FLAGS_CYCLIC);
	dac_dmac_write(core, DAC_DMAC_REG_START_TRANSFER, DAC_DMAC_ENABLE);

	dac_data_src_sel(core, -1, DAC_SRC_DMA);

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
		xil_printf("DAC Core Status errors.\n\r");

		return -1;
	} else {
		xil_printf("DAC Core successfully initialized.\n\r");

		return 0;
	}
}
