/***************************************************************************//**
 *   @file   mmcm.c
 *   @brief  Implementation of MMCM Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <xparameters.h>
#include <xil_io.h>
#include "test.h"
#include "mmcm.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MIN(x, y)				(x < y ? x : y)
#define MAX(x, y) 				(x > y ? x : y)
#define DIV_ROUND_UP(x,y)		(((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y)	(uint32_t)(((double)x / y) + 0.5)
#define CLAMP(val, min, max)	(val < min ? min : (val > max ? max :val))
#define ABS(x)					(x < 0 ? -x : x)

static const uint32_t mmcm_filter_table[] =
{
	0x01001990, 0x01001190, 0x01009890, 0x01001890,
	0x01008890, 0x01009090, 0x01009090, 0x01009090,
	0x01009090, 0x01000890, 0x01000890, 0x01000890,
	0x08009090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01001090,
	0x01001090, 0x01001090, 0x01001090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x01008090, 0x01008090, 0x01008090,
	0x01008090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090, 0x08001090,
	0x08001090, 0x08001090, 0x08001090
};

static const uint32_t mmcm_lock_table[] =
{
	0x060603e8, 0x060603e8, 0x080803e8, 0x0b0b03e8,
	0x0e0e03e8, 0x111103e8, 0x131303e8, 0x161603e8,
	0x191903e8, 0x1c1c03e8, 0x1f1f0384, 0x1f1f0339,
	0x1f1f02ee, 0x1f1f02bc, 0x1f1f028a, 0x1f1f0271,
	0x1f1f023f, 0x1f1f0226, 0x1f1f020d, 0x1f1f01f4,
	0x1f1f01db, 0x1f1f01c2, 0x1f1f01a9, 0x1f1f0190,
	0x1f1f0190, 0x1f1f0177, 0x1f1f015e, 0x1f1f015e,
	0x1f1f0145, 0x1f1f0145, 0x1f1f012c, 0x1f1f012c,
	0x1f1f012c, 0x1f1f0113, 0x1f1f0113, 0x1f1f0113
};

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint32_t mmcm_addr;

/***************************************************************************//**
 * @brief mmcm_init
*******************************************************************************/
void mmcm_init(uint32_t sel)
{
	mmcm_addr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ?
					CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
}

/***************************************************************************//**
 * @brief mmcm_write
*******************************************************************************/
void mmcm_write(uint32_t reg,
				  uint32_t val)
{
	Xil_Out32(mmcm_addr + 0x4000 + reg, val);
}

/***************************************************************************//**
 * @brief mmcm_read
*******************************************************************************/
static void mmcm_read(uint32_t reg,
						uint32_t *val)
{
	*val = Xil_In32(mmcm_addr + 0x4000 + reg);
}

/***************************************************************************//**
 * @brief mmcm_read_reg
*******************************************************************************/
static void mmcm_read_reg(uint32_t reg,
							uint32_t *val)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		mmcm_read(AXI_MMCM_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_MMCM_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	reg_val = AXI_MMCM_DRP_CNTRL_SEL | AXI_MMCM_DRP_CNTRL_READ | (reg << 16);

	mmcm_write(AXI_MMCM_REG_DRP_CNTRL, reg_val);
	do {
		mmcm_read(AXI_MMCM_REG_DRP_STATUS, val);
	} while ((*val & AXI_MMCM_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	*val &= 0xffff;
}

/***************************************************************************//**
 * @brief mmcm_write_reg
*******************************************************************************/
void mmcm_write_reg(uint32_t reg,
					uint32_t val,
					uint32_t mask)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		mmcm_read(AXI_MMCM_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_MMCM_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	if (mask != 0xffff) {
		mmcm_read_reg(reg, &reg_val);
		reg_val &= ~mask;
	} else {
		reg_val = 0;
	}

	reg_val |= AXI_MMCM_DRP_CNTRL_SEL | (reg << 16) | (val & mask);

	mmcm_write(AXI_MMCM_REG_DRP_CNTRL, reg_val);
}

/***************************************************************************//**
 * @brief mmcm_lookup_filter
*******************************************************************************/
static uint32_t mmcm_lookup_filter(uint32_t m)
{
	if (m < 47)
		return mmcm_filter_table[m];

	return 0x08008090;
}

/***************************************************************************//**
 * @brief mmcm_lookup_lock
*******************************************************************************/
static uint32_t mmcm_lookup_lock(uint32_t m)
{
	if (m < 36)
		return mmcm_lock_table[m];

	return 0x1f1f00fa;
}

/***************************************************************************//**
 * @brief mmcm_calc_params
*******************************************************************************/
void mmcm_calc_params(uint32_t fin,
					  uint32_t fout,
					  uint32_t *best_d,
					  uint32_t *best_m,
					  uint32_t *best_dout)
{
	const uint32_t fpfd_min	= 10000;
	const uint32_t fpfd_max	= 300000;
	const uint32_t fvco_min	= 600000;
	const uint32_t fvco_max	= 1200000;
	uint32_t	   d		= 0;
	uint32_t	   d_min	= 0;
	uint32_t	   d_max	= 0;
	uint32_t	   _d_min	= 0;
	uint32_t	   _d_max	= 0;
	uint32_t	   m		= 0;
	uint32_t	   m_min	= 0;
	uint32_t	   m_max	= 0;
	uint32_t	   dout		= 0;
	uint32_t	   fvco		= 0;
	int32_t		   f		= 0;
	int32_t		   best_f	= 0;

	fin /= 1000;
	fout /= 1000;

	best_f = 0x7fffffff;
	*best_d = 0;
	*best_m = 0;
	*best_dout = 0;

	d_min = MAX(DIV_ROUND_UP(fin, fpfd_max), 1);
	d_max = MIN(fin / fpfd_min, 80);

	m_min = MAX(DIV_ROUND_UP(fvco_min, fin) * d_min, 1);
	m_max = MIN(fvco_max * d_max / fin, 64);

	for(m = m_min; m <= m_max; m++) {
		_d_min = MAX(d_min, DIV_ROUND_UP(fin * m, fvco_max));
		_d_max = MIN(d_max, fin * m / fvco_min);

		for (d = _d_min; d <= _d_max; d++) {
			fvco = fin * m / d;
			dout = DIV_ROUND_CLOSEST(fvco, fout);
			dout = CLAMP(dout, 1, 128);
			f = fvco / dout;
			if (ABS(f - fout) < ABS(best_f - fout)) {
				best_f = f;
				*best_d = d;
				*best_m = m;
				*best_dout = dout;
				if (best_f == fout)
					return;
			}
		}
	}
}

/***************************************************************************//**
 * @brief mmcm_calc_clk_params
*******************************************************************************/
void mmcm_calc_clk_params(uint32_t divider,
						  uint32_t *low,
						  uint32_t *high,
						  uint32_t *edge,
						  uint32_t *nocount)
{
	if (divider == 1)
		*nocount = 1;
	else
		*nocount = 0;
	*high = divider / 2;
	*edge = divider % 2;
	*low = divider - *high;
}

/***************************************************************************//**
 * @brief mmcm_enable
*******************************************************************************/
static void mmcm_enable(char enable)
{
	uint32_t val = AXI_MMCM_RESET_ENABLE;

	if (enable)
			val |= AXI_MMCM_RESET_MMCM_ENABLE;

	mmcm_write(AXI_MMCM_REG_RESET, val);
}

/***************************************************************************//**
 * @brief mmcm_set_rate
*******************************************************************************/
int32_t mmcm_set_rate(uint32_t rate,
					  uint32_t parent_rate)
{
	uint32_t d		 = 0;
	uint32_t m		 = 0;
	uint32_t dout	 = 0;
	uint32_t nocount = 0;
	uint32_t high	 = 0;
	uint32_t edge	 = 0;
	uint32_t low	 = 0;
	uint32_t filter  = 0;
	uint32_t lock	 = 0;

	if (parent_rate == 0 || rate == 0)
		return 0;

	mmcm_calc_params(parent_rate, rate, &d, &m, &dout);

	if (d == 0 || dout == 0 || m == 0)
		return 0;

	filter = mmcm_lookup_filter(m - 1);
	lock = mmcm_lookup_lock(m - 1);

	mmcm_enable(0);

	mmcm_calc_clk_params(dout, &low, &high, &edge, &nocount);
	mmcm_write_reg(MMCM_REG_CLKOUT0_1, (high << 6) | low, 0xefff);
	mmcm_write_reg(MMCM_REG_CLKOUT0_2, (edge << 7) | (nocount << 6), 0x03ff);

	dout *= 4;
	mmcm_calc_clk_params(dout, &low, &high, &edge, &nocount);
	mmcm_write_reg(MMCM_REG_CLKOUT1_1, (high << 6) | low, 0xefff);
	mmcm_write_reg(MMCM_REG_CLKOUT1_2, (edge << 7) | (nocount << 6), 0x03ff);

	mmcm_calc_clk_params(d, &low, &high, &edge, &nocount);
	mmcm_write_reg(MMCM_REG_CLK_DIV, (edge << 13) | (nocount << 12) | (high << 6) | low, 0x3fff);

	mmcm_calc_clk_params(m, &low, &high, &edge, &nocount);
	mmcm_write_reg(MMCM_REG_CLK_FB1, (high << 6) | low, 0xefff);
	mmcm_write_reg(MMCM_REG_CLK_FB2, (edge << 7) | (nocount << 6), 0x03ff);

	mmcm_write_reg(MMCM_REG_LOCK1, lock & 0x3ff, 0x3ff);
	mmcm_write_reg(MMCM_REG_LOCK2, (((lock >> 16) & 0x1f) << 10) | 0x1, 0x7fff);
	mmcm_write_reg(MMCM_REG_LOCK3, (((lock >> 24) & 0x1f) << 10) | 0x3e9, 0x7fff);
	mmcm_write_reg(MMCM_REG_FILTER1, filter >> 16, 0x9900);
	mmcm_write_reg(MMCM_REG_FILTER2, filter, 0x9900);

	mmcm_enable(1);

	return 0;
}

/***************************************************************************//**
 * @brief mmcm_get_rate
*******************************************************************************/
uint32_t mmcm_get_rate(uint32_t parent_rate)
{
	uint32_t d, m, dout;
	uint32_t reg;
	uint64_t tmp;

	mmcm_read_reg(MMCM_REG_CLKOUT0_1, &reg);
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	mmcm_read_reg(MMCM_REG_CLK_DIV, &reg);
	d = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	mmcm_read_reg(MMCM_REG_CLK_FB1, &reg);
	m = (reg & 0x3f) + ((reg >> 6) & 0x3f);

	if (d == 0 || dout == 0)
		return 0;

	tmp = (uint64_t)(parent_rate / d) * m;
	tmp = tmp / dout;

	if (tmp > 0xffffffff)
		return 0xffffffff;

	return (uint32_t)tmp;
}
