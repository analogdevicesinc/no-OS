/***************************************************************************//**
 *   @file   clk_altera_a10_fpll.c
 *   @brief  Driver for the Altera FPLL.
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
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include "io.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "clk_altera_a10_fpll.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define FPLL_REG_C_COUNTER0			0x10D
#define FPLL_REG_C_COUNTER1			0x10E
#define FPLL_REG_C_COUNTER2			0x10F
#define FPLL_REG_C_COUNTER3			0x110
#define FPLL_REG_C_COUNTER4			0x111

#define FPLL_REG_DIV_COUNTER0		0x12B
#define FPLL_REG_DIV_COUNTER1		0x12C

#define A10_FPLL_PFD_MIN 25000    /* 25.000 MHz */
#define A10_FPLL_PFD_MAX 60000    /* 60.000 MHz */
#define A10_FPLL_VCO_MIN 4800000  /*  4.800 GHz */
#define A10_FPLL_VCO_MAX 14025000 /* 14.025 GHz */

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief altera_a10_fpll_read
 */
uint32_t altera_a10_fpll_read(struct altera_a10_fpll *fpll,
			      uint32_t addr)
{
	return IORD_32DIRECT(fpll->base, addr * 4);
}

/**
 * @brief altera_a10_fpll_write
 */
void altera_a10_fpll_write(struct altera_a10_fpll *fpll,
			   uint32_t addr, uint32_t val)
{
	IOWR_32DIRECT(fpll->base, addr * 4, val);
}

/**
 * @brief altera_a10_fpll_update
 */
void altera_a10_fpll_update(struct altera_a10_fpll *fpll,
			    uint32_t addr, uint32_t mask, uint32_t val)
{
	uint32_t rval;

	rval = altera_a10_fpll_read(fpll, addr);
	rval &= ~mask;
	rval |= val;
	altera_a10_fpll_write(fpll, addr, rval);
}

/**
 * @brief altera_a10_acquire_arbitration
 */
uint32_t altera_a10_acquire_arbitration(struct altera_a10_fpll *fpll)
{
	uint32_t timeout = 0;
	uint32_t status;

	altera_a10_fpll_write(fpll, 0x00, 0x2);

	do {
		status = altera_a10_fpll_read(fpll, 0x280);
		if ((status & NO_OS_BIT(2)) == 0)
			return 0;
		no_os_udelay(10);
	} while (timeout++ < 10000);

	printf("%s: Failed to acquire arbitration\n", fpll->name);

	return 0;
}

/**
 * @brief altera_a10_release_arbitration
 */
void altera_a10_release_arbitration(struct altera_a10_fpll *fpll,
				    bool run_calibration)
{
	altera_a10_fpll_write(fpll, 0x00, run_calibration ? 0x1 : 0x3);
}

/**
 * @brief altera_a10_fpll_lookup_lf_resistance
 */
uint32_t altera_a10_fpll_lookup_lf_resistance(uint32_t fvco,
		uint32_t m)
{
	if (fvco < 9000000) {
		switch (m) {
		case 0 ... 15:
			return 2;
		case 16 ... 31:
			return 0;
		case 32 ... 71:
			return 1;
		default:
			return 3;
		}
	} else if (fvco < 12000000) {
		switch (m) {
		case 0 ... 15:
			return 2;
		case 16 ... 31:
			return 0;
		case 32 ... 71:
			return 1;
		default:
			return 3;
		}
	} else {
		switch (m) {
		case 0 ... 23:
			return 0;
		case 24 ... 55:
			return 1;
		default:
			return 3;
		}
	}
}

/**
 * @brief altera_a10_fpll_lookup_cp_current
 */
int32_t altera_a10_fpll_lookup_cp_current(uint32_t fvco,
		uint32_t m)
{
	if (fvco < 9000000) {
		switch (m) {
		case 0 ... 15:
			return 29;
		case 16 ... 23:
			return 28;
		case 24 ... 31:
			return 29;
		case 32 ... 47:
			return 28;
		case 48 ... 55:
			return 29;
		case 56 ... 71:
			return 36;
		case 72 ... 103:
			return 29;
		default:
			return 36;
		}
	} else if (fvco < 12000000) {
		switch (m) {
		case 0 ... 15:
			return 36;
		case 16 ... 23:
			return 29;
		case 24 ... 31:
			return 36;
		case 32 ... 39:
			return 28;
		case 40 ... 55:
			return 29;
		case 56 ... 79:
			return 36;
		case 80 ... 87:
			return 29;
		default:
			return 36;
		}
	} else {
		switch (m) {
		case 0 ... 15:
			return 28;
		case 16 ... 23:
			return 36;
		case 24 ... 39:
			return 29;
		case 40 ... 55:
			return 36;
		case 56 ... 63:
			return 29;
		default:
			return 36;
		}
	}
}

/**
 * @brief altera_a10_fpll_calc_params
 */
int32_t altera_a10_fpll_calc_params(uint32_t fref,
				    uint32_t fout, uint32_t *best_n, uint32_t *best_m,
				    uint32_t *best_c0, uint32_t *best_fvco)
{
	uint32_t n, n_min, n_max, _n_min, _n_max;
	uint32_t m, m_min, m_max;
	uint32_t c0;
	uint32_t f, fvco, best_f;

	*best_n = *best_m = *best_c0 = *best_fvco = 0;

	fref /= 1000;
	fout /= 1000;

	best_f = ULONG_MAX;

	n_min = no_os_max_t(uint32_t, NO_OS_DIV_ROUND_UP(fref, A10_FPLL_PFD_MAX), 1);
	n_max = no_os_min_t(uint32_t, fref / A10_FPLL_PFD_MIN, 31);

	m_min = no_os_max_t(uint32_t, NO_OS_DIV_ROUND_UP(A10_FPLL_VCO_MIN / 2,
			    fref) * n_min,
			    8);
	m_max = no_os_min_t(uint32_t, A10_FPLL_VCO_MAX / 2 * n_max / fref, 127);
	m_min = no_os_round_up(m_min, 2);

	for (m = m_min; m <= m_max; m += 2) {
		_n_min = no_os_max_t(uint32_t, n_min, NO_OS_DIV_ROUND_UP(fref * m,
				     A10_FPLL_VCO_MAX / 2));
		_n_max = no_os_min_t(uint32_t, n_max, fref * m / (A10_FPLL_VCO_MIN / 2));

		for (n = _n_min; n <= _n_max; n++) {
			fvco = fref * m * 2 / n;

			c0 = NO_OS_DIV_ROUND_CLOSEST(fvco, fout * 4);
			c0 = no_os_clamp_t(uint32_t, c0, 1, 512);
			f = fvco / (c0 * 4);

			if (abs(f - fout) < abs(best_f - fout)) {
				best_f = f;
				*best_n = n;
				*best_m = m;
				*best_c0 = c0;
				*best_fvco = fvco;
				if (f == fout)
					return 0;
			}
		}
	}

	return -1;
}

/**
 * @brief altera_a10_fpll_round_rate
 */
int32_t altera_a10_fpll_round_rate(struct altera_a10_fpll *fpll,
				   uint32_t rate)
{
	uint32_t n, m, c0;
	uint32_t fvco;
	uint64_t tmp;

	altera_a10_fpll_calc_params(fpll->parent_rate, rate, &n, &m, &c0, &fvco);

	if (n == 0 || m == 0 || c0 == 0)
		return -1;

	tmp = (uint64_t)fpll->parent_rate * m;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, c0 * n * 2);

	return no_os_min_t(uint64_t, tmp, LONG_MAX);
}

/**
 * @brief altera_a10_fpll_pll_calibration_check
 */
int32_t altera_a10_fpll_pll_calibration_check(struct altera_a10_fpll *fpll)
{
	uint32_t timeout = 0;
	uint32_t val;

	/* Wait max 100ms for cal_busy to de-assert */
	do {
		no_os_udelay(200);

		/* Read FPLL calibration status from capability register */
		val = altera_a10_fpll_read(fpll, 0x280);
		if ((val & 0x02) == 0x00) {
			printf("%s: FPLL PLL calibration OK (%"PRIu32" us)\n", fpll->name,
			       timeout * 200);
			return 0;
		}
	} while (timeout++ < 50);

	printf("%s: FPLL PLL calibration FAILED\n", fpll->name);

	return 1;
}

/**
 * @brief altera_a10_fpll_set_rate
 */
int32_t altera_a10_fpll_set_rate(struct altera_a10_fpll *fpll, uint32_t rate)
{
	uint32_t feedback;
	uint32_t n, m, c0;
	uint32_t fvco;
	uint32_t div0, div1;
	uint32_t lfr, cpc;

	altera_a10_fpll_calc_params(fpll->parent_rate, rate, &n, &m, &c0, &fvco);

	if (n == 0 || m == 0 || c0 == 0)
		return -1;

	lfr = altera_a10_fpll_lookup_lf_resistance(fvco, m);
	cpc = altera_a10_fpll_lookup_cp_current(fvco, m);

	if (c0 >= 512)
		c0 = 0;

	div0 = m & 0xff;
	div1 = ((n << 3) & 0xf8);

	altera_a10_acquire_arbitration(fpll);

	altera_a10_fpll_write(fpll, FPLL_REG_DIV_COUNTER0, div0);
	altera_a10_fpll_write(fpll, FPLL_REG_DIV_COUNTER1, div1);

	div0 = c0 & 0xff;
	div1 = (c0 & 0x100) >> 4;

	altera_a10_fpll_write(fpll, FPLL_REG_C_COUNTER2, m / 2);
	altera_a10_fpll_write(fpll, FPLL_REG_C_COUNTER3, div0);
	altera_a10_fpll_update(fpll, FPLL_REG_C_COUNTER4, 0x0f, div1);

	altera_a10_fpll_update(fpll, 0x133, 0x0c, lfr << 2);
	altera_a10_fpll_update(fpll, 0x134, 0x70, (cpc & 0x7) << 4);
	altera_a10_fpll_update(fpll, 0x135, 0x07, (cpc & 0x38) >> 3);

	/* Calibration needs to run with internal feedback */
	feedback = altera_a10_fpll_read(fpll, 0x126);
	altera_a10_fpll_write(fpll, 0x126, feedback | 1);

	altera_a10_fpll_update(fpll, 0x100, 0x2, 0x2);
	altera_a10_release_arbitration(fpll, true);
	altera_a10_fpll_pll_calibration_check(fpll);

	/* Restore original feedback configuration */
	altera_a10_fpll_write(fpll, 0x126, feedback);

	fpll->initial_recalc = false;

	return 0;
}

/**
 * @brief altera_a10_fpll_recalc_rate
 */
uint32_t altera_a10_fpll_recalc_rate(struct altera_a10_fpll *fpll)
{
	uint32_t m, n, c0;
	uint64_t tmp;
	uint32_t div0, div1;

	altera_a10_acquire_arbitration(fpll);

	div0 = altera_a10_fpll_read(fpll, FPLL_REG_DIV_COUNTER0);
	div1 = altera_a10_fpll_read(fpll, FPLL_REG_DIV_COUNTER1);

	n = (div1 >> 3) & 0x1f;
	m = div0 | ((div1 & 0x1) << 8);

	if (m == 0)
		m = 512;

	div0 = altera_a10_fpll_read(fpll, FPLL_REG_C_COUNTER3);
	div1 = altera_a10_fpll_read(fpll, FPLL_REG_C_COUNTER4);

	altera_a10_release_arbitration(fpll, false);

	c0 = div0 | ((div1 & 0x8) << 5);

	if (c0 == 0 || n == 0)
		return 0;

	tmp = (uint64_t)fpll->parent_rate * m;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, c0 * n * 2);

	/*
	 * Recalc configuration in case ref clock is not the same as in the FPGA
	 * project.
	 */
	if (tmp != 0 && fpll->initial_recalc)
		altera_a10_fpll_set_rate(fpll, tmp);

	return no_os_min_t(uint64_t, tmp, ULONG_MAX);
}

/**
 * @brief altera_a10_fpll_is_enabled
 */
int32_t altera_a10_fpll_is_enabled(struct altera_a10_fpll *fpll)
{
	return altera_a10_fpll_read(fpll, 0x2e0) != 0x3;
}

/**
 * @brief altera_a10_fpll_enable
 */
int32_t altera_a10_fpll_enable(struct altera_a10_fpll *fpll)
{
	altera_a10_fpll_write(fpll, 0x2e0, 0x2);

	return 0;
}

/**
 * @brief altera_a10_fpll_disable
 */
void altera_a10_fpll_disable(struct altera_a10_fpll *fpll)
{
	altera_a10_fpll_write(fpll, 0x2e0, 0x3);
}

/**
 * @brief altera_a10_fpll_init
 */
int32_t altera_a10_fpll_init(struct altera_a10_fpll **a10_fpll,
			     const struct altera_a10_fpll_init *init)
{
	struct altera_a10_fpll *fpll;

	fpll = (struct altera_a10_fpll *)no_os_malloc(sizeof(*fpll));
	if (!fpll)
		return -1;

	fpll->base = init->base;
	fpll->name = init->name;
	fpll->parent_rate = init->parent_rate;

	*a10_fpll = fpll;

	return 0;
}

/**
 * @brief altera_a10_fpll_remove
 */
int32_t altera_a10_fpll_remove(struct altera_a10_fpll *fpll)
{
	no_os_free(fpll);

	return 0;
}
