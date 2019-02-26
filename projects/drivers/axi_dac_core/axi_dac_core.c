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
#include <inttypes.h>
#include "platform_drivers.h"
#ifdef ALTERA_PLATFORM
#include "io.h"
#else
#include "xil_io.h"
#include "xil_cache.h"
#endif
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

#define AXI_DAC_REG_CHAN_CNTRL_6(c)		(0x0414 + (c) * 0x40)
#define AXI_DAC_IQCOR_ENB				(1 << 2)

#define AXI_DAC_REG_CHAN_CNTRL_7(c)		(0x0418 + (c) * 0x40)
#define AXI_DAC_DAC_DDS_SEL(x)			(((x) & 0xF) << 0)
#define AXI_DAC_TO_DAC_DDS_SEL(x)		(((x) >> 0) & 0xF)

#define AXI_DAC_REG_CHAN_CNTRL_8(c)		(0x041C + (c) * 0x40)
#define AXI_DAC_IQCOR_COEFF_1(x)		(((x) & 0xFFFF) << 16)
#define AXI_DAC_TO_IQCOR_COEFF_1(x)		(((x) >> 16) & 0xFFFF)
#define AXI_DAC_IQCOR_COEFF_2(x)		(((x) & 0xFFFF) << 0)
#define AXI_DAC_TO_IQCOR_COEFF_2(x)		(((x) >> 0) & 0xFFFF)

const uint16_t sine_lut[128] = {
	0x000, 0x064, 0x0C8, 0x12C, 0x18F, 0x1F1, 0x252, 0x2B1,
	0x30F, 0x36B, 0x3C5, 0x41C, 0x471, 0x4C3, 0x512, 0x55F,
	0x5A7, 0x5ED, 0x62E, 0x66C, 0x6A6, 0x6DC, 0x70D, 0x73A,
	0x763, 0x787, 0x7A7, 0x7C2, 0x7D8, 0x7E9, 0x7F5, 0x7FD,
	0x7FF, 0x7FD, 0x7F5, 0x7E9, 0x7D8, 0x7C2, 0x7A7, 0x787,
	0x763, 0x73A, 0x70D, 0x6DC, 0x6A6, 0x66C, 0x62E, 0x5ED,
	0x5A7, 0x55F, 0x512, 0x4C3, 0x471, 0x41C, 0x3C5, 0x36B,
	0x30F, 0x2B1, 0x252, 0x1F1, 0x18F, 0x12C, 0xC8,  0x64,
	0x000, 0xF9B, 0xF37, 0xED3, 0xE70, 0xE0E, 0xDAD, 0xD4E,
	0xCF0, 0xC94, 0xC3A, 0xBE3, 0xB8E, 0xB3C, 0xAED, 0xAA0,
	0xA58, 0xA12, 0x9D1, 0x993, 0x959, 0x923, 0x8F2, 0x8C5,
	0x89C, 0x878, 0x858, 0x83D, 0x827, 0x816, 0x80A, 0x802,
	0x800, 0x802, 0x80A, 0x816, 0x827, 0x83D, 0x858, 0x878,
	0x89C, 0x8C5, 0x8F2, 0x923, 0x959, 0x993, 0x9D1, 0xA12,
	0xA58, 0xAA0, 0xAED, 0xB3C, 0xB8E, 0xBE3, 0xC3A, 0xC94,
	0xCF0, 0xD4E, 0xDAD, 0xE0E, 0xE70, 0xED3, 0xF37, 0xF9B
};

/***************************************************************************//**
 * @brief axi_dac_read
 *******************************************************************************/
int32_t axi_dac_read(struct axi_dac *dac,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
#ifdef ALTERA_PLATFORM
	*reg_data = IORD_32DIRECT(dac->base, reg_addr);
#else
	*reg_data = Xil_In32((dac->base + reg_addr));
#endif

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_write
 *******************************************************************************/
int32_t axi_dac_write(struct axi_dac *dac,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
#ifdef ALTERA_PLATFORM
	IOWR_32DIRECT(dac->base, reg_addr, reg_data);
#else
	Xil_Out32((dac->base + reg_addr), reg_data);
#endif

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_dac_set_datasel
*******************************************************************************/
int32_t axi_dac_set_datasel(struct axi_dac *dac,
			    int32_t chan,
			    enum axi_dac_data_sel sel)
{
	int32_t i;

	if (chan < 0)
		for (i = 0; i < dac->num_channels; i++)
			axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_7(i), sel);
	else
		axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_7(chan), sel);

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

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
 * @brief axi_dac_dds_get_frequency
 *
 * freq is in Hz (i.e. set to 1*1000*1000 for 1 MHz)
 *******************************************************************************/
int32_t axi_dac_dds_get_frequency(struct axi_dac *dac,
				  uint32_t chan, uint32_t *freq)
{
	uint32_t reg;
	uint64_t val64;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);
	reg = (reg & AXI_DAC_DDS_INCR(~0));
	val64 = (uint64_t) reg * dac->clock_hz;
	do_div(&val64, 0xFFFF);
	*freq = val64;

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
 * @brief axi_dac_dds_get_phase
 *
 * phase is in milli angles scaled to 1000 (i.e. 90*1000 is 90 degrees (pi/2))
 *******************************************************************************/
int32_t axi_dac_dds_get_phase(struct axi_dac *dac,
			      uint32_t chan, uint32_t *phase)
{
	uint64_t val64;
	uint32_t reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);
	reg = (reg & AXI_DAC_DDS_INIT(~0));
	reg = AXI_DAC_TO_DDS_INIT(reg);
	val64 = reg * 360000ULL + (0x10000 / 2);
	do_div(&val64, 0x10000);
	*phase = val64;

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
 * @brief axi_dac_dds_get_scale
 *
 * scale is in micro units (i.e. 1*1000*1000 is 1.0)
 *******************************************************************************/
int32_t axi_dac_dds_get_scale(struct axi_dac *dac,
			      uint32_t chan,
			      int32_t *scale_micro_units)
{
	int32_t sign;
	uint32_t scale_reg;

	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
	axi_dac_read(dac, AXI_DAC_REG_DDS_SCALE(chan),
		     &scale_reg);
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);
	scale_reg = AXI_DAC_TO_DDS_SCALE(scale_reg);
	sign = (scale_reg & 0x8000) ? -1 : 1;
	scale_reg &= ~0x8000;
	scale_reg = ((uint64_t)scale_reg * 1000000) / 0x4000;
	*scale_micro_units = (int32_t)scale_reg * sign;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief dds_to_signed_mag_fmt
*******************************************************************************/
uint32_t axi_dac_dds_to_signed_mag_fmt(int32_t val,
				       int32_t val2)
{
	uint32_t i;
	uint64_t val64;

	switch (val) {
	case 1:
		i = 0x4000;
		break;
	case -1:
		i = 0xC000;
		break;
	case 0:
		i = 0;
		if (val2 < 0) {
			i = 0x8000;
			val2 *= -1;
		}
		break;
	default:
		i = 0;
	}

	val64 = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
	do_div(&val64, 1000000UL);

	return i | val64;
}

/***************************************************************************//**
 * @brief dds_from_signed_mag_fmt
*******************************************************************************/
void axi_dac_dds_from_signed_mag_fmt(uint32_t val,
				     int32_t *r_val,
				     int32_t *r_val2)
{
	uint64_t val64;
	int32_t sign;

	if (val & 0x8000)
		sign = -1;
	else
		sign = 1;

	if (val & 0x4000)
		*r_val = 1 * sign;
	else
		*r_val = 0;

	val &= ~0xC000;

	val64 = val * 1000000ULL + (0x4000 / 2);
	do_div(&val64, 0x4000);

	if (*r_val == 0)
		*r_val2 = val64 * sign;
	else
		*r_val2 = val64;
}

/***************************************************************************//**
 * @brief dds_set_calib_scale_phase
*******************************************************************************/
int32_t axi_dac_dds_set_calib_phase_scale(struct axi_dac *dac,
		uint32_t phase,
		uint32_t chan,
		int32_t val,
		int32_t val2)
{
	uint32_t reg;
	uint32_t i;

	i = axi_dac_dds_to_signed_mag_fmt(val, val2);

	axi_dac_read(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), &reg);

	if (!((chan + phase) % 2)) {
		reg &= ~AXI_DAC_IQCOR_COEFF_1(~0);
		reg |= AXI_DAC_IQCOR_COEFF_1(i);
	} else {
		reg &= ~AXI_DAC_IQCOR_COEFF_2(~0);
		reg |= AXI_DAC_IQCOR_COEFF_2(i);
	}
	axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), reg);
	axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_6(chan), AXI_DAC_IQCOR_ENB);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief dds_get_calib_scale_phase
*******************************************************************************/
int32_t axi_dac_dds_get_calib_phase_scale(struct axi_dac *dac,
		uint32_t phase,
		uint32_t chan,
		int32_t *val,
		int32_t *val2)
{
	uint32_t reg;

	axi_dac_read(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), &reg);

	if (!((phase + chan) % 2)) {
		reg = AXI_DAC_TO_IQCOR_COEFF_1(reg);
	} else {
		reg = AXI_DAC_TO_IQCOR_COEFF_2(reg);
	}

	axi_dac_dds_from_signed_mag_fmt(reg, val, val2);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief dds_set_calib_scale
*******************************************************************************/
int32_t axi_dac_dds_set_calib_scale(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t val,
				    int32_t val2)
{
	return axi_dac_dds_set_calib_phase_scale(dac, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief dds_get_calib_scale
*******************************************************************************/
int32_t axi_dac_dds_get_calib_scale(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t *val,
				    int32_t *val2)
{
	return axi_dac_dds_get_calib_phase_scale(dac, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief dds_set_calib_phase
*******************************************************************************/
int32_t axi_dac_dds_set_calib_phase(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t val,
				    int32_t val2)
{
	return axi_dac_dds_set_calib_phase_scale(dac, 1, chan, val, val2);
}

/***************************************************************************//**
 * @brief dds_get_calib_phase
*******************************************************************************/
int32_t axi_dac_dds_get_calib_phase(struct axi_dac *dac,
				    uint32_t chan,
				    int32_t *val,
				    int32_t *val2)
{
	return axi_dac_dds_get_calib_phase_scale(dac, 1, chan, val, val2);
}

/***************************************************************************//**
 * @brief axi_dac_set_sine_lut
*******************************************************************************/
uint32_t axi_dac_set_sine_lut(struct axi_dac *dac,
			      uint32_t address)
{
	uint32_t length;
	uint32_t tx_count;
	uint32_t index_mem;
	uint32_t index;
	uint32_t index_i1;
	uint32_t index_q1;
	uint32_t index_i2;
	uint32_t index_q2;
	uint32_t data_i1;
	uint32_t data_q1;
	uint32_t data_i2;
	uint32_t data_q2;
	tx_count = sizeof(sine_lut) / sizeof(uint16_t);
	if(dac->num_channels == 4) {
		for(index = 0, index_mem = 0; index < (tx_count * 2);
		    index += 2, index_mem += 2) {
			index_i1 = index;
			index_q1 = index + (tx_count / 2);
			if(index_q1 >= (tx_count * 2))
				index_q1 -= (tx_count * 2);
			data_i1 = (sine_lut[index_i1 / 2] << 20);
			data_q1 = (sine_lut[index_q1 / 2] << 4);
#ifdef ALTERA_PLATFORM
			IOWR_32DIRECT(address + index_mem * 4, data_i1 | data_q1);
#else
			Xil_Out32(address + index_mem * 4, data_i1 | data_q1);
#endif

			index_i2 = index_i1;
			index_q2 = index_q1;
			if(index_i2 >= (tx_count * 2))
				index_i2 -= (tx_count * 2);
			if(index_q2 >= (tx_count * 2))
				index_q2 -= (tx_count * 2);
			data_i2 = (sine_lut[index_i2 / 2] << 20);
			data_q2 = (sine_lut[index_q2 / 2] << 4);
#ifdef ALTERA_PLATFORM
			IOWR_32DIRECT(address + (index_mem + 1) * 4, data_i2 | data_q2);
#else
			Xil_Out32(address + (index_mem + 1) * 4, data_i2 | data_q2);
#endif
		}
	} else {
		for(index = 0; index < tx_count; index += 1) {
			index_i1 = index;
			index_q1 = index + (tx_count / 4);
			if(index_q1 >= tx_count)
				index_q1 -= tx_count;
			data_i1 = (sine_lut[index_i1] << 20);
			data_q1 = (sine_lut[index_q1] << 4);
#ifdef ALTERA_PLATFORM
			IOWR_32DIRECT(address + index * 4, data_i1 | data_q1);
#else
			Xil_Out32(address + index * 4, data_i1 | data_q1);
#endif
		}
	}
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif
	length = tx_count * dac->num_channels * 2;
	return length;
}

/***************************************************************************//**
 * @brief dac_datasel
*******************************************************************************/
int32_t axi_dac_datasel(struct axi_dac *dac, int32_t chan,
			enum axi_dac_data_sel sel)
{
	int32_t i;
	if (chan < 0) { /* ALL */
		for (i = 0; i < dac->num_channels; i++) {
			axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_8(i), sel);
		}
	} else {
		axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), sel);
	}
	axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);
	return 0;
}

/***************************************************************************//**
 * @brief axi_dac_set_buff
*******************************************************************************/
int32_t axi_dac_set_buff(struct axi_dac *dac,
			 uint32_t address,
			 uint16_t *buff,
			 uint32_t buff_size)
{
	uint32_t index;
	uint32_t data_i;
	uint32_t data_q;

	for(index = 0; index < buff_size; index += 2) {
		data_i = (buff[index]);
		data_q = (buff[index + 1] << 16);
#ifdef ALTERA_PLATFORM
		IOWR_32DIRECT(address, index * 2, data_i | data_q);
#else
		Xil_Out32(address + index * 2, data_i | data_q);
#endif
	}

#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif

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

	printf("%s: Successfully initialized (%"PRIu64" Hz)\n",
	       dac->name, dac->clock_hz);

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
