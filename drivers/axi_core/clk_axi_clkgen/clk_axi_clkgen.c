/***************************************************************************//**
 *   @file   clk_axi_clkgen.c
 *   @brief  Driver for the Analog Devices AXI CLKGEN.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#include "clk_axi_clkgen.h"
#ifndef USE_STANDARD_SPI
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_axi_io.h"

#define AXI_PCORE_VER(major, minor, letter)     ((major << 16) | (minor << 8) | letter)
#define AXI_PCORE_VER_MAJOR(version)    (version >> 16)
#define AXI_PCORE_VER_MINOR(version)    ((version >> 8) & 0xff)
#define AXI_PCORE_VER_LETTER(version)   (version & 0xff)

#define AXI_REG_VERSION                 0x0000
#define AXI_VERSION(x)                  (((x) & 0xffffffff) << 0)
#define AXI_VERSION_IS(x, y, z)         ((x) << 16 | (y) << 8 | (z))
#define AXI_VERSION_MAJOR(x)            ((x) >> 16)

#define AXI_REG_FPGA_INFO               0x001C
#define AXI_REG_FPGA_VOLTAGE            0x0140

#define AXI_INFO_FPGA_TECH(info)        ((info) >> 24)
#define AXI_INFO_FPGA_FAMILY(info)      (((info) >> 16) & 0xff)
#define AXI_INFO_FPGA_SPEED_GRADE(info) (((info) >> 8) & 0xff)
#define AXI_INFO_FPGA_DEV_PACKAGE(info) ((info) & 0xff)
#define AXI_INFO_FPGA_VOLTAGE(val)      ((val) & 0xffff)

#define AXI_CLKGEN_REG_RESETN		0x40
#define AXI_CLKGEN_MMCM_RESETN		NO_OS_BIT(1)
#define AXI_CLKGEN_RESETN			NO_OS_BIT(0)

#define AXI_CLKGEN_REG_STATUS		0x5c
#define AXI_CLKGEN_STATUS			NO_OS_BIT(0)

#define AXI_CLKGEN_REG_DRP_CNTRL	0x70
#define AXI_CLKGEN_DRP_CNTRL_SEL	NO_OS_BIT(29)
#define AXI_CLKGEN_DRP_CNTRL_READ	NO_OS_BIT(28)

#define AXI_CLKGEN_REG_DRP_STATUS	0x74
#define AXI_CLKGEN_DRP_STATUS_BUSY	NO_OS_BIT(16)

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

static const uint32_t axi_clkgen_filter_table[] = {
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

static const uint32_t axi_clkgen_lock_table[] = {
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

enum axi_fgpa_technology {
	AXI_FPGA_TECH_UNKNOWN = 0,
	AXI_FPGA_TECH_SERIES7,
	AXI_FPGA_TECH_ULTRASCALE,
	AXI_FPGA_TECH_ULTRASCALE_PLUS,
};

enum axi_fpga_family {
	AXI_FPGA_FAMILY_UNKNOWN = 0,
	AXI_FPGA_FAMILY_ARTIX,
	AXI_FPGA_FAMILY_KINTEX,
	AXI_FPGA_FAMILY_VIRTEX,
	AXI_FPGA_FAMILY_ZYNQ,
};

enum axi_fpga_speed_grade {
	AXI_FPGA_SPEED_UNKNOWN	= 0,
	AXI_FPGA_SPEED_1	= 10,
	AXI_FPGA_SPEED_1L	= 11,
	AXI_FPGA_SPEED_1H	= 12,
	AXI_FPGA_SPEED_1HV	= 13,
	AXI_FPGA_SPEED_1LV	= 14,
	AXI_FPGA_SPEED_2	= 20,
	AXI_FPGA_SPEED_2L	= 21,
	AXI_FPGA_SPEED_2LV	= 22,
	AXI_FPGA_SPEED_3	= 30,
};

/**
 * @brief axi_clkgen_write
 */
int32_t axi_clkgen_write(struct axi_clkgen *clkgen,
			 uint32_t reg_addr,
			 uint32_t reg_val)
{
	no_os_axi_io_write(clkgen->base, reg_addr, reg_val);

	return 0;
}

/**
 * @brief axi_clkgen_read
 */
int32_t axi_clkgen_read(struct axi_clkgen *clkgen,
			uint32_t reg_addr,
			uint32_t *reg_val)
{
	no_os_axi_io_read(clkgen->base, reg_addr, reg_val);

	return 0;
}

/**
 * @brief axi_clkgen_mmcm_read
 */
static void axi_clkgen_mmcm_read(struct axi_clkgen *clkgen,
				 uint32_t reg,
				 uint32_t *val)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		axi_clkgen_read(clkgen, AXI_CLKGEN_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	reg_val = AXI_CLKGEN_DRP_CNTRL_SEL |
		  AXI_CLKGEN_DRP_CNTRL_READ | (reg << 16);

	axi_clkgen_write(clkgen, AXI_CLKGEN_REG_DRP_CNTRL, reg_val);
	do {
		axi_clkgen_read(clkgen, AXI_CLKGEN_REG_DRP_STATUS, val);
	} while ((*val & AXI_CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	*val &= 0xffff;
}

/**
 * @brief axi_clkgen_mmcm_write
 */
void axi_clkgen_mmcm_write(struct axi_clkgen *clkgen,
			   uint32_t reg,
			   uint32_t val,
			   uint32_t mask)
{
	uint32_t timeout = 1000000;
	uint32_t reg_val;

	do {
		axi_clkgen_read(clkgen, AXI_CLKGEN_REG_DRP_STATUS, &reg_val);
	} while ((reg_val & AXI_CLKGEN_DRP_STATUS_BUSY) && --timeout);

	if (timeout == 0) {
		return;
	}

	if (mask != 0xffff) {
		axi_clkgen_mmcm_read(clkgen, reg, &reg_val);
		reg_val &= ~mask;
	} else {
		reg_val = 0;
	}

	reg_val |= AXI_CLKGEN_DRP_CNTRL_SEL | (reg << 16) | (val & mask);

	axi_clkgen_write(clkgen, AXI_CLKGEN_REG_DRP_CNTRL, reg_val);
}

/**
 * @brief axi_clkgen_lookup_filter
 */
static uint32_t axi_clkgen_lookup_filter(uint32_t m)
{
	if (m < 47)
		return axi_clkgen_filter_table[m];

	return 0x08008090;
}

/**
 * @brief axi_clkgen_lookup_lock
 */
static uint32_t axi_clkgen_lookup_lock(uint32_t m)
{
	if (m < 36)
		return axi_clkgen_lock_table[m];

	return 0x1f1f00fa;
}

/**
 * @brief axi_clkgen_setup_ranges
 */
static void axi_clkgen_setup_ranges(struct axi_clkgen *axi_clkgen,
				    uint32_t *fpfd_min, uint32_t *fpfd_max,
				    uint32_t *fvco_min, uint32_t *fvco_max)
{
	uint32_t reg_value;
	uint32_t tech, family, speed_grade, voltage;

	axi_clkgen_read(axi_clkgen, AXI_REG_FPGA_INFO, &reg_value);
	tech = AXI_INFO_FPGA_TECH(reg_value);
	family = AXI_INFO_FPGA_FAMILY(reg_value);
	speed_grade = AXI_INFO_FPGA_SPEED_GRADE(reg_value);

	axi_clkgen_read(axi_clkgen, AXI_REG_FPGA_VOLTAGE, &reg_value);
	voltage = AXI_INFO_FPGA_VOLTAGE(reg_value);

	switch (speed_grade) {
	case AXI_FPGA_SPEED_1 ... AXI_FPGA_SPEED_1LV:
		*fvco_max = 1200000;
		*fpfd_max = 450000;
		break;
	case AXI_FPGA_SPEED_2 ... AXI_FPGA_SPEED_2LV:
		*fvco_max = 1440000;
		*fpfd_max = 500000;
		if ((family == AXI_FPGA_FAMILY_KINTEX) |
		    (family == AXI_FPGA_FAMILY_ARTIX)) {
			if (voltage < 950) {
				*fvco_max = 1200000;
				*fpfd_max = 450000;
			}
		}
		break;
	case AXI_FPGA_SPEED_3:
		*fvco_max = 1600000;
		*fpfd_max = 550000;
		break;
	default:
		break;
	};

	if (tech == AXI_FPGA_TECH_ULTRASCALE_PLUS) {
		*fvco_max = 1600000;
		*fvco_min = 800000;
	}
}

/**
 * @brief axi_clkgen_calc_params
 */
void axi_clkgen_calc_params(struct axi_clkgen *axi_clkgen,
			    uint32_t fin,
			    uint32_t fout,
			    uint32_t *best_d,
			    uint32_t *best_m,
			    uint32_t *best_dout)
{
	uint32_t fpfd_min	= 10000;
	uint32_t fpfd_max	= 300000;
	uint32_t fvco_min	= 600000;
	uint32_t fvco_max	= 1200000;
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
	uint32_t pcore_version;

	axi_clkgen_read(axi_clkgen, AXI_REG_VERSION, &pcore_version);
	if (AXI_PCORE_VER_MAJOR(pcore_version) > 0x04)
		axi_clkgen_setup_ranges(axi_clkgen, &fpfd_min, &fpfd_max,
					&fvco_min, &fvco_max);

	fin /= 1000;
	fout /= 1000;

	best_f = 0x7fffffff;
	*best_d = 0;
	*best_m = 0;
	*best_dout = 0;

	d_min = no_os_max(NO_OS_DIV_ROUND_UP(fin, fpfd_max), 1);
	d_max = no_os_min(fin / fpfd_min, 80);

	m_min = no_os_max(NO_OS_DIV_ROUND_UP(fvco_min, fin) * d_min, 1);
	m_max = no_os_min(fvco_max * d_max / fin, 64);

	for (m = m_min; m <= m_max; m++) {
		_d_min = no_os_max(d_min, NO_OS_DIV_ROUND_UP(fin * m, fvco_max));
		_d_max = no_os_min(d_max, fin * m / fvco_min);

		for (d = _d_min; d <= _d_max; d++) {
			fvco = fin * m / d;
			dout = NO_OS_DIV_ROUND_CLOSEST(fvco, fout);
			dout = no_os_clamp(dout, 1, 128);
			f = fvco / dout;
			if (abs(f - (int32_t)fout) < abs(best_f - (int32_t)fout)) {
				best_f = f;
				*best_d = d;
				*best_m = m;
				*best_dout = dout;
				if (best_f == (int32_t)fout)
					return;
			}
		}
	}
}

/**
 * @brief axi_clkgen_calc_clk_params
 */
void axi_clkgen_calc_clk_params(uint32_t divider,
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

/**
 * @brief axi_clkgen_mmcm_enable
 */
static void axi_clkgen_mmcm_enable(struct axi_clkgen *clkgen,
				   bool enable)
{
	uint32_t val = AXI_CLKGEN_RESETN;

	if (enable)
		val |= AXI_CLKGEN_MMCM_RESETN;

	axi_clkgen_write(clkgen, AXI_CLKGEN_REG_RESETN, val);
}

/**
 * @brief axi_clkgen_set_rate
 */
int32_t axi_clkgen_set_rate(struct axi_clkgen *clkgen,
			    uint32_t rate)
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
	uint32_t reg_val;

	if (clkgen->parent_rate == 0 || rate == 0)
		return 0;

	axi_clkgen_calc_params(clkgen, clkgen->parent_rate, rate, &d, &m, &dout);

	if (d == 0 || dout == 0 || m == 0)
		return 0;

	filter = axi_clkgen_lookup_filter(m - 1);
	lock = axi_clkgen_lookup_lock(m - 1);

	axi_clkgen_mmcm_enable(clkgen, 0);

	axi_clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLKOUT0_1, (high << 6) | low, 0xefff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLKOUT0_2, (edge << 7) | (nocount << 6),
			      0x03ff);

	dout *= 4;
	axi_clkgen_calc_clk_params(dout, &low, &high, &edge, &nocount);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLKOUT1_1, (high << 6) | low, 0xefff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLKOUT1_2, (edge << 7) | (nocount << 6),
			      0x03ff);

	axi_clkgen_calc_clk_params(d, &low, &high, &edge, &nocount);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLK_DIV,
			      (edge << 13) | (nocount << 12) | (high << 6) | low, 0x3fff);

	axi_clkgen_calc_clk_params(m, &low, &high, &edge, &nocount);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLK_FB1, (high << 6) | low, 0xefff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_CLK_FB2, (edge << 7) | (nocount << 6),
			      0x03ff);

	axi_clkgen_mmcm_write(clkgen, MMCM_REG_LOCK1, lock & 0x3ff, 0x3ff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_LOCK2,
			      (((lock >> 16) & 0x1f) << 10) | 0x1, 0x7fff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_LOCK3,
			      (((lock >> 24) & 0x1f) << 10) | 0x3e9, 0x7fff);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_FILTER1, filter >> 16, 0x9900);
	axi_clkgen_mmcm_write(clkgen, MMCM_REG_FILTER2, filter, 0x9900);

	axi_clkgen_mmcm_enable(clkgen, 1);

	no_os_mdelay(10);

	axi_clkgen_read(clkgen, AXI_CLKGEN_REG_STATUS, &reg_val);
	if ((reg_val & AXI_CLKGEN_STATUS) == 0x0) {
		printf("%s: MMCM-PLL NOT locked (%"PRIu32" Hz)\n", clkgen->name, rate);
		return -1;

	} else {
		printf("%s: MMCM-PLL locked (%"PRIu32" Hz)\n", clkgen->name, rate);
	}

	return 0;
}

/**
 * @brief axi_clkgen_get_rate
 */
int32_t axi_clkgen_get_rate(struct axi_clkgen *clkgen, uint32_t *rate)
{
	uint32_t d, m, dout;
	uint32_t reg;
	uint64_t tmp;

	axi_clkgen_mmcm_read(clkgen, MMCM_REG_CLKOUT0_1, &reg);
	dout = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	axi_clkgen_mmcm_read(clkgen, MMCM_REG_CLK_DIV, &reg);
	d = (reg & 0x3f) + ((reg >> 6) & 0x3f);
	axi_clkgen_mmcm_read(clkgen, MMCM_REG_CLK_FB1, &reg);
	m = (reg & 0x3f) + ((reg >> 6) & 0x3f);

	if (d == 0 || dout == 0) {
		*rate = 0;
		return 0;
	}

	tmp = (uint64_t)(clkgen->parent_rate / d) * m;
	tmp = tmp / dout;

	if (tmp > 0xffffffff)
		*rate = 0xffffffff;

	*rate = (uint32_t)tmp;

	return 0;
}

/**
 * @brief axi_clkgen_init
 */
int32_t axi_clkgen_init(struct axi_clkgen **clk,
			const struct axi_clkgen_init *init)
{
	struct axi_clkgen *clkgen;

	clkgen = (struct axi_clkgen *)no_os_malloc(sizeof(*clkgen));
	if (!clkgen)
		return -1;

	clkgen->base = init->base;
	clkgen->name = init->name;
	clkgen->parent_rate = init->parent_rate;

	*clk = clkgen;

	return 0;
}

/**
 * @brief axi_clkgen_remove
 */
int32_t axi_clkgen_remove(struct axi_clkgen *clkgen)
{
	no_os_free(clkgen);

	return 0;
}
#else
int32_t axi_clkgen_set_rate(struct axi_clkgen *clkgen, uint32_t rate)
{
	return 0;
}
int32_t axi_clkgen_get_rate(struct axi_clkgen *clkgen, uint32_t *rate)
{
	return 0;
}
int32_t axi_clkgen_init(struct axi_clkgen **clk,
			const struct axi_clkgen_init *init)
{
	return 0;
}
int32_t axi_clkgen_remove(struct axi_clkgen *clkgen)
{
	return 0;
}
#endif
