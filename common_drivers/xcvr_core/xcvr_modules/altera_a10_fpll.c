/*******************************************************************************
 * @file altera_a10_fpll.c
 * @brief Implementation of a10-fpll dynamic reconfiguration driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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

#ifdef ALTERA
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "altera_a10_fpll.h"

/*******************************************************************************
* @brief fpll_lookup_lf_resistance
*******************************************************************************/
uint32_t fpll_lookup_lf_resistance(uint32_t fvco,
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

/*******************************************************************************
* @brief fpll_lookup_cp_current
*******************************************************************************/
int32_t fpll_lookup_cp_current(uint32_t fvco,
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

/*******************************************************************************
* @brief fpll_calc_params
*******************************************************************************/
int32_t fpll_calc_params(uint32_t fref_khz,
	uint32_t fout_khz, uint32_t *best_n, uint32_t *best_m,
	uint32_t *best_c0, uint32_t *best_fvco)
{
	uint32_t n, n_min, n_max, _n_min, _n_max;
	uint32_t m, m_min, m_max;
	uint32_t c0;
	uint32_t f, fvco, best_f;

	*best_n = *best_m = *best_c0 = *best_fvco = 0;

	best_f = ULONG_MAX;

	n_min = max_t(uint32_t, DIV_ROUND_UP(fref_khz, A10_FPLL_PFD_MAX), 1);
	n_max = min_t(uint32_t, fref_khz / A10_FPLL_PFD_MIN, 31);

	m_min = max_t(uint32_t, DIV_ROUND_UP(A10_FPLL_VCO_MIN / 2, fref_khz) * n_min, 8);
	m_max = min_t(uint32_t, A10_FPLL_VCO_MAX / 2 * n_max / fref_khz, 127);
	m_min = ROUND_UP(m_min, 2);

	for (m = m_min; m < m_max; m += 1) {
		_n_min = max_t(uint32_t, n_min, DIV_ROUND_UP(fref_khz * m, A10_FPLL_VCO_MAX / 2));
		_n_max = min_t(uint32_t, n_max, fref_khz * m / (A10_FPLL_VCO_MIN / 2));

		for (n = _n_min; n <= _n_max; n++) {
			fvco = fref_khz * m * 2 / n;

			c0 = DIV_ROUND_CLOSEST(fvco, fout_khz * 4);
			c0 = clamp_t(uint32_t, c0, 1, 512);
			f = fvco / (c0 * 4);

			if (abs(f - fout_khz) < abs(best_f - fout_khz)) {
				best_f = f;
				*best_n = n;
				*best_m = m;
				*best_c0 = c0;
				*best_fvco = fvco;
				if (f == fout_khz)
					return 0;
			}
		}
	}

	return -1;
}

/*******************************************************************************
* @brief fpll_round_rate
*******************************************************************************/
uint32_t fpll_round_rate(uint32_t fout_khz, uint32_t fref_khz)
{
	uint32_t n, m, c0;
	uint32_t fvco;
	uint32_t tmp;

	fpll_calc_params(fref_khz, fout_khz, &n, &m, &c0, &fvco);

	if (n == 0 || m == 0 || c0 == 0)
		return -1;

	tmp = (uint32_t)fref_khz * m;
	tmp = DIV_ROUND_CLOSEST_ull(tmp, c0 * n * 2);

	// fout frequency
	return min_t(uint32_t, tmp, LONG_MAX);
}

/*******************************************************************************
* @brief fpll_set_rate
*******************************************************************************/
int32_t fpll_set_rate(xcvr_pll *fpll, uint32_t fout_khz,
	uint32_t fref_khz)
{
	uint32_t n, m, c0;
	uint32_t fvco;
	uint32_t div0, div1;
	uint32_t lfr, cpc;

	fpll_calc_params(fref_khz, fout_khz, &n, &m, &c0, &fvco);

#ifdef DEBUG
	printf("\nFPLL PLL:\n");
	printf("\tfref_khz: %d\n", fref_khz);
	printf("\tfout_khz: %d\n", fout_khz);
	printf("\tn: %d\n", n);
	printf("\tm: %d\n", m);
	printf("\tc0: %d\n", c0);
	printf("\tfvco: %d\n", fvco);
#endif

	if (n == 0 || m == 0 || c0 == 0)
		return -1;

	lfr = fpll_lookup_lf_resistance(fvco, m);
	cpc = fpll_lookup_cp_current(fvco, m);

	if (c0 >= 512)
		c0 = 0;

	div0 = m & 0xff;
	div1 = ((n << 3) & 0xf8);

	altera_a10_acquire_arbitration(fpll);

	a10_pll_write(fpll, FPLL_REG_DIV_COUNTER0, div0);
	a10_pll_write(fpll, FPLL_REG_DIV_COUNTER1, div1);

	div0 = c0 & 0xff;
	div1 = (c0 & 0x100) >> 4;

	a10_pll_write(fpll, FPLL_REG_C_COUNTER2, m / 2);
	a10_pll_write(fpll, FPLL_REG_C_COUNTER3, div0);
	pll_update(fpll, FPLL_REG_C_COUNTER4, 0x0f, div1);

	pll_update(fpll, 0x133, 0x0c, lfr << 2);
	pll_update(fpll, 0x134, 0x70, (cpc & 0x7) << 4);
	pll_update(fpll, 0x135, 0x07, (cpc & 0x38) >> 3);

	pll_update(fpll, 0x100, 0x2, 0x2);
	altera_a10_release_arbitration(fpll, TRUE);
	pll_calibration_check(fpll);

	fpll->initial_recalc = FALSE;

	return 0;
}

/*******************************************************************************
* @brief fpll_recalc_rate
*******************************************************************************/
uint32_t fpll_recalc_rate(xcvr_pll *fpll,
	uint32_t fref_khz)
{
	uint32_t m, n, c0;
	uint32_t tmp;
	uint32_t div0, div1;

	altera_a10_acquire_arbitration(fpll);

	 a10_pll_read(fpll, FPLL_REG_DIV_COUNTER0, &div0);
	 a10_pll_read(fpll, FPLL_REG_DIV_COUNTER1, &div1);

	n = (div1 >> 3) & 0x1f;
	m = div0 | ((div1 & 0x1) << 8);

	if (m == 0)
		m = 512;

	 a10_pll_read(fpll, FPLL_REG_C_COUNTER3, &div0);
	 a10_pll_read(fpll, FPLL_REG_C_COUNTER4, &div1);

	altera_a10_release_arbitration(fpll, FALSE);

	c0 = div0 | ((div1 & 0x8) << 5);

	if (c0 == 0 || n == 0)
		return 0;

	// round rate
	tmp = (uint32_t)fref_khz * m;
	tmp = DIV_ROUND_CLOSEST(tmp, c0 * n * 2);

	/*
	 * Recalc configuration in case ref clock is not the same as in the FPGA
	 * project.
	 */
	if (tmp != 0 && fpll->initial_recalc)
		fpll_set_rate(fpll, tmp, fref_khz);

	return min_t(uint32_t, tmp, ULONG_MAX);
}

#endif

/*******************************************************************************/
/*******************************************************************************/
