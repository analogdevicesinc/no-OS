/***************************************************************************//**
* @file dac_core.c
* @brief Implementation of DAC Core Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xil_io.h>
#include "dac_core.h"
#include "platform_drivers.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint32_t dac_baseaddr;

/***************************************************************************//**
* @brief dac_read
*******************************************************************************/
int32_t dac_read(uint32_t reg_addr, uint32_t *reg_data)
{
	*reg_data = Xil_In32((dac_baseaddr + 0x4000 + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief dac_write
*******************************************************************************/
int32_t dac_write(uint32_t reg_addr, uint32_t reg_data)
{
	Xil_Out32((dac_baseaddr + 0x4000 + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief dds_set_frequency
*******************************************************************************/
void dds_set_frequency(uint32_t chan, uint32_t freq)
{
	uint64_t val64;
	uint32_t reg;
	uint64_t dac_clk;
	uint32_t val;

	dac_read(DAC_REG_CLK_FREQ, &val);
	dac_clk = val;
	dac_read(DAC_REG_CLK_RATIO, &val);
	dac_clk *= val * 1525;

	dac_write(DAC_REG_CNTRL_1, 0);
	dac_read(DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~DAC_DDS_INCR(~0);
	val64 = (u64) freq * 0xFFFFULL;
	val64 = val64 / dac_clk;
	reg |= DAC_DDS_INCR(val64) | 1;
	dac_write(DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_write(DAC_REG_CNTRL_1, 0x1);
}

/***************************************************************************//**
* @brief dds_set_phase
*******************************************************************************/
void dds_set_phase(uint32_t chan, uint32_t phase)
{
	uint64_t val64;
	uint32_t reg;

	dac_write(DAC_REG_CNTRL_1, 0);
	dac_read(DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~DAC_DDS_INIT(~0);
	val64 = (u64) phase * 0x10000ULL + (360000 / 2);
	val64 = val64 / 360000;
	reg |= DAC_DDS_INIT(val64);
	dac_write(DAC_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_write(DAC_REG_CNTRL_1, 0x1);
}

/***************************************************************************//**
* @brief dds_set_scale
*******************************************************************************/
void dds_set_scale(uint32_t chan, double scale)
{
	uint32_t scale_reg;
	uint32_t sign_part;
	uint32_t int_part;
	uint32_t fract_part;

	if(scale >= 1.0)
	{
		sign_part = 0;
		int_part = 1;
		fract_part = 0;
		goto set_scale_reg;
	}
	if(scale <= -1.0)
	{
		sign_part = 1;
		int_part = 1;
		fract_part = 0;
		goto set_scale_reg;
	}
	if(scale < 0)
	{
		sign_part = 1;
		int_part = 0;
		scale *= -1;
		goto set_scale_reg;
	}
	sign_part = 0;
	int_part = 0;
	fract_part = (uint32_t)(scale * 0x4000);
set_scale_reg:
	scale_reg = (sign_part << 15) | (int_part << 14) | fract_part;

	dac_write(DAC_REG_CNTRL_1, 0);
	dac_write(DAC_REG_CHAN_CNTRL_1_IIOCHAN(chan), DAC_DDS_SCALE(scale_reg));
	dac_write(DAC_REG_CNTRL_1, 0x1);
}

/***************************************************************************//**
* @brief dac_setup
*******************************************************************************/
int32_t dac_setup(uint32_t baseaddr)
{
	dac_baseaddr = baseaddr;

	dac_write(DAC_REG_RSTN, 0x00);
	dac_write(DAC_REG_RSTN, 0x01);

	mdelay(1000);

	dac_write(DAC_REG_CHAN_CNTRL_7(0), 0);
	dac_write(DAC_REG_CHAN_CNTRL_7(1), 0);
	dac_write(DAC_REG_CNTRL_1, 0x0);
	dac_write(DAC_REG_CNTRL_1, 0x1);

	dds_set_frequency(0, 50000000);
	dds_set_phase(0, 90000);
	dds_set_scale(0, 0.25);
	dds_set_frequency(1, 50000000);
	dds_set_phase(1, 90000);
	dds_set_scale(1, 0.25);

	return 0;
}
