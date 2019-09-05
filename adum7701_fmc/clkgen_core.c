/***************************************************************************//**
 * @file clkgen_core.c
 * @brief Implementation of CLKGEN Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include "clkgen_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define CLKGEN_REG_RESETN			0x40
#define CLKGEN_MMCM_RESETN			(1 << 1)
#define CLKGEN_RESETN				(1 << 0)

#define CLKGEN_REG_STATUS			0x5c
#define CLKGEN_STATUS				(1 << 0)

#define CLKGEN_REG_DRP_CNTRL		0x70
#define CLKGEN_DRP_CNTRL_SEL		(1 << 29)
#define CLKGEN_DRP_CNTRL_READ		(1 << 28)

#define CLKGEN_REG_DRP_STATUS		0x74
#define CLKGEN_DRP_STATUS_BUSY		(1 << 16)

#define MMCM_REG_CLKOUT0_1			0x08
#define MMCM_REG_CLKOUT0_2			0x09
#define MMCM_REG_CLKOUT1_1			0x0A
#define MMCM_REG_CLKOUT1_2			0x0B
#define MMCM_REG_CLK_FB1			0x14
#define MMCM_REG_CLK_FB2			0x15
#define MMCM_REG_CLK_DIV			0x16
#define MMCM_REG_LOCK1				0x18
#define MMCM_REG_LOCK2				0x19
#define MMCM_REG_LOCK3				0x1a
#define MMCM_REG_FILTER1			0x4e
#define MMCM_REG_FILTER2			0x4f

#define MIN(x, y)				(x < y ? x : y)
#define MAX(x, y) 				(x > y ? x : y)
#define DIV_ROUND_UP(x,y)		(((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y)	(uint32_t)(((double)x / y) + 0.5)
#define CLAMP(val, min, max)	(val < min ? min : (val > max ? max :val))
#define ABS(x)					(x < 0 ? -x : x)

static const uint32_t clkgen_filter_table[] =
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

static const uint32_t clkgen_lock_table[] =
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

/***************************************************************************//**
 * @brief clkgen_write
 *******************************************************************************/
int32_t clkgen_write(clkgen_device *dev,
					 uint32_t reg_addr,
					 uint32_t reg_val)
{
	Xil_Out32((dev->base_addr + reg_addr), reg_val);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_read
 *******************************************************************************/
int32_t clkgen_read(clkgen_device *dev,
					uint32_t reg_addr,
					uint32_t *reg_val)
{
	*reg_val = Xil_In32(dev->base_addr + reg_addr);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_mmcm_read
*******************************************************************************/
static void clkgen_mmcm_read(clkgen_device *dev,
							 uint32_t reg,
							 uint32_t *val)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		clkgen_read(dev, CLKGEN_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	reg_val = CLKGEN_DRP_CNTRL_SEL | CLKGEN_DRP_CNTRL_READ | (reg << 16);

	clkgen_write(dev, CLKGEN_REG_DRP_CNTRL, reg_val);
	do {
		clkgen_read(dev, CLKGEN_REG_DRP_STATUS, val);
	} while ((*val & CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	*val &= 0xffff;
}

/***************************************************************************//**
 * @brief clkgen_mmcm_write
*******************************************************************************/
void clkgen_mmcm_write(clkgen_device *dev,
					   uint32_t reg,
					   uint32_t val,
					   uint32_t mask)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		clkgen_read(dev, CLKGEN_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	if (mask != 0xffff) {
		clkgen_mmcm_read(dev, reg, &reg_val);
		reg_val &= ~mask;
	} else {
		reg_val = 0;
	}

	reg_val |= CLKGEN_DRP_CNTRL_SEL | (reg << 16) | (val & mask);

	clkgen_write(dev, CLKGEN_REG_DRP_CNTRL, reg_val);
}

/***************************************************************************//**
 * @brief clkgen_lookup_filter
*******************************************************************************/
static uint32_t clkgen_lookup_filter(uint32_t m)
{
	if (m < 47)
		return clkgen_filter_table[m];

	return 0x08008090;
}

/***************************************************************************//**
 * @brief clkgen_lookup_lock
*******************************************************************************/
static uint32_t clkgen_lookup_lock(uint32_t m)
{
	if (m < 36)
		return clkgen_lock_table[m];

	return 0x1f1f00fa;
}

/***************************************************************************//**
 * @brief clkgen_calc_params
*******************************************************************************/
void clkgen_calc_params(uint32_t fin,
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
 * @brief clkgen_calc_clk_params
*******************************************************************************/
void clkgen_calc_clk_params(uint32_t divider,
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
 * @brief clkgen_mmcm_enable
*******************************************************************************/
static void clkgen_mmcm_enable(clkgen_device *dev,
							   char enable)
{
	uint32_t val = CLKGEN_RESETN;

	if (enable)
			val |= CLKGEN_MMCM_RESETN;

	clkgen_write(dev, CLKGEN_REG_RESETN, val);
}

/***************************************************************************//**
 * @brief clkgen_set_rate
*******************************************************************************/
int32_t clkgen_set_rate(clkgen_device *dev,
						uint32_t rate,
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

	clkgen_calc_params(parent_rate, rate, &d, &m, &dout);

	if (d == 0 || dout == 0 || m == 0)
		return 0;

	filter = clkgen_lookup_filter(m - 1);
	lock = clkgen_lookup_lock(m - 1);

	clkgen_mmcm_enable(dev, 0);

	clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	clkgen_mmcm_write(dev, MMCM_REG_CLKOUT0_1, (high << 6) | low, 0xefff);
	clkgen_mmcm_write(dev, MMCM_REG_CLKOUT0_2, (edge << 7) | (nocount << 6), 0x03ff);

	dout *= 4;
	clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	clkgen_mmcm_write(dev, MMCM_REG_CLKOUT1_1, (high << 6) | low, 0xefff);
	clkgen_mmcm_write(dev, MMCM_REG_CLKOUT1_2, (edge << 7) | (nocount << 6), 0x03ff);

	clkgen_calc_clk_params(d, &low, &high, &edge, &nocount);
	clkgen_mmcm_write(dev, MMCM_REG_CLK_DIV, (edge << 13) | (nocount << 12) | (high << 6) | low, 0x3fff);

	clkgen_calc_clk_params(m, &low, &high, &edge, &nocount);
	clkgen_mmcm_write(dev, MMCM_REG_CLK_FB1, (high << 6) | low, 0xefff);
	clkgen_mmcm_write(dev, MMCM_REG_CLK_FB2, (edge << 7) | (nocount << 6), 0x03ff);

	clkgen_mmcm_write(dev, MMCM_REG_LOCK1, lock & 0x3ff, 0x3ff);
	clkgen_mmcm_write(dev, MMCM_REG_LOCK2, (((lock >> 16) & 0x1f) << 10) | 0x1, 0x7fff);
	clkgen_mmcm_write(dev, MMCM_REG_LOCK3, (((lock >> 24) & 0x1f) << 10) | 0x3e9, 0x7fff);
	clkgen_mmcm_write(dev, MMCM_REG_FILTER1, filter >> 16, 0x9900);
	clkgen_mmcm_write(dev, MMCM_REG_FILTER2, filter, 0x9900);

	clkgen_mmcm_enable(dev, 1);

	return 0;
}

/***************************************************************************//**
 * @brief clkgen_get_rate
*******************************************************************************/
uint32_t clkgen_get_rate(clkgen_device *dev,
						 uint32_t parent_rate)
{
	uint32_t d, m, dout;
	uint32_t reg;
	uint64_t tmp;

	clkgen_mmcm_read(dev, MMCM_REG_CLKOUT0_1, &reg);
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	clkgen_mmcm_read(dev, MMCM_REG_CLK_DIV, &reg);
	d = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	clkgen_mmcm_read(dev, MMCM_REG_CLK_FB1, &reg);
	m = (reg & 0x3f) + ((reg >> 6) & 0x3f);

	if (d == 0 || dout == 0)
		return 0;

	tmp = (uint64_t)(parent_rate / d) * m;
	tmp = tmp / dout;

	if (tmp > 0xffffffff)
		return 0xffffffff;

	return (uint32_t)tmp;
}

/***************************************************************************//**
 * @brief clkgen_setup
 *******************************************************************************/
int32_t clkgen_setup(clkgen_device **clkgen_dev,
		clkgen_init_params init_params)
{
	clkgen_device	adc_clkgen;
	uint32_t	reg_val;
	int32_t		status = 0;

	adc_clkgen.base_addr = init_params.base_addr;
	clkgen_set_rate(&adc_clkgen,
			init_params.rate,
			init_params.parent_rate);

	mdelay(1);

	clkgen_read(&adc_clkgen, CLKGEN_REG_STATUS, &reg_val);
	if ((reg_val & CLKGEN_STATUS) == 0x0) {
		printf("clkgen_setup failed");
		status--;
	}

	return status;
}
