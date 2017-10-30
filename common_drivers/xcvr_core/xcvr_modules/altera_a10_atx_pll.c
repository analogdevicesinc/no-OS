/*******************************************************************************
 * @file altera_a10_atx_pll.c
 * @brief Implementation of a10-atx_pll dynamic reconfiguration driver.
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

#include "altera_a10_atx_pll.h"
#include "altera_pll_common.h"

/******************************************************************************/
/******************************************************************************/
const uint32_t atx_bands[] = {
	14200000, 13650000, 13150000, 13110000, 12600000,
	11900000, 11550000, 11400000, 11100000, 10600000,
	10400000,  9750000,  9350000,  9100000,  8900000,
	 8800000,  8600000,  8200000,  7800000,  7400000,
};

/*******************************************************************************
* @brief atx_lookup_band
*******************************************************************************/
uint32_t atx_lookup_band(uint32_t fvco)
{
	uint32_t i;

	for (i = 0; i < ARRAY_SIZE(atx_bands); i++) {
		if (fvco > atx_bands[i]) {
			i = i % 8;
			if (i != 7)
				i++;
			return i;
		}
	}

	return 5;
}

/*******************************************************************************
* @brief atx_lookup_tank
*******************************************************************************/
uint32_t atx_lookup_tank(uint32_t fvco)
{
	if (fvco < 8800000)
		return 0;
	else if (fvco < 11400000)
		return 1;
	else
		return 2;
}

/*******************************************************************************
* @brief atx_lookup_cp_current
*******************************************************************************/
uint32_t atx_lookup_cp_current(uint32_t m)
{
	switch (m) {
	case 0 ... 20:
		return 0x1a;
	case 21 ... 30:
		return 0x1c;
	case 31 ... 40:
		return 0x1d;
	case 41 ... 60:
		return 0x25;
	case 61 ... 70:
		return 0x26;
	case 71 ... 90:
		return 0x1d;
	case 91 ... 100:
		return 0x25;
	default:
		return 0x26;
	}
}

/*******************************************************************************
* @brief atx_lookup_lf_resistance
*******************************************************************************/
uint32_t atx_lookup_lf_resistance(uint32_t m)
{
	switch (m) {
	case 0 ... 12:
		return 0;
	case 13 ... 20:
		return 1;
	case 21 ... 30:
		return 0;
	case 31 ... 70:
		return 1;
	default:
		return 2;
	}
}

/*******************************************************************************
* @brief atx_calc_params
*******************************************************************************/
void atx_calc_params(uint32_t fref_khz,
	uint32_t fout_khz, uint32_t *best_n, uint32_t *best_m,
	uint32_t *best_l, uint32_t *best_fvco)
{
	uint32_t m, m_min, m_max;
	uint32_t n, l;
	uint32_t f, fvco, best_f;
	uint32_t pfd;

	*best_n = *best_m = *best_l = *best_fvco = 0;
	best_f = ULONG_MAX;

	m_min = max_t(uint32_t, DIV_ROUND_UP(A10_ATX_PLL_VCO_MIN / 2, fref_khz), 8);
	m_max = min_t(uint32_t, (A10_ATX_PLL_VCO_MAX / 2 * 8 / fref_khz), 127);

	for (n = 1; n <= 8; n *= 2) {
		pfd = fref_khz / n;
		if (pfd < A10_ATX_PLL_PFD_MIN)
			break;
		if (pfd > A10_ATX_PLL_VCO_MAX)
			continue;

		for (m = m_min; m <= m_max; m++) {
			fvco = fref_khz * m * 2 / n;

			for (l = 1; l <= 16; l *= 2) {
				f = (fvco * 2) / l;

				if (abs(f - fout_khz) < abs(best_f - fout_khz)) {
					best_f = f;
					*best_n = n;
					*best_m = m;
					*best_l = l/2;
					*best_fvco = fvco;
					if (f == fout_khz)
						return;
				}
			}
		}
	}
}

/*******************************************************************************
* @brief atx_pll_round_rate
*******************************************************************************/
uint32_t atx_pll_round_rate(uint32_t l_rate_kbps, uint32_t fref_khz)
{
	uint32_t n, m, l, fout_khz;
	uint32_t fvco;
	uint32_t tmp;

	fout_khz = l_rate_kbps / 2;

	atx_calc_params(fref_khz, fout_khz, &n, &m, &l, &fvco);

	if (n == 0 || m == 0 || l == 0)
		return -1;

	tmp = (uint32_t)fref_khz * m * 4;
	tmp = DIV_ROUND_CLOSEST_ull(tmp, l * n);

	return min_t(uint32_t, tmp, LONG_MAX);
}

/*******************************************************************************
* @brief atx_pll_set_rate
*******************************************************************************/
uint32_t atx_pll_set_rate(xcvr_core *core,
	uint32_t l_rate_kbps, uint32_t fref_khz)
{
	uint32_t n, m, l, fout_khz;
	uint32_t fvco;
	uint32_t lfr, cpc, band, tank;

	fout_khz = l_rate_kbps / 2;

	atx_calc_params(fref_khz, fout_khz, &n, &m, &l, &fvco);

#ifdef DEBUG
	printf("\nATX PLL:\n");
	printf("\tfref_khz: %d\n", fref_khz);
	printf("\tlane rate: %d\n", l_rate_kbps);
	printf("\tn: %d\n", n);
	printf("\tm: %d\n", m);
	printf("\tl: %d\n", l);
	printf("\tfvco: %d\n", fvco);
#endif

	if (n == 0 || m == 0 || l == 0)
		return -1;
	// encodings
	switch (n) {
	case 1:
		n = 0;
		break;
	case 2:
		n = 1;
		break;
	case 4:
		n = 2;
		break;
	default:
		n = 3;
		break;
	}

	switch (l) {
	case 1:
		l = 0;
		break;
	case 2:
		l = 1;
		break;
	case 4:
		l = 2;
		break;
	case 8:
		l = 3;
		break;
	default:
		l = 4;
		break;
	}

	lfr = atx_lookup_lf_resistance(m);
	cpc = atx_lookup_cp_current(m);
	band = atx_lookup_band(fvco);
	tank = atx_lookup_tank(fvco);

	altera_a10_acquire_arbitration(&(core->dev.atx_pll));

	pll_update(&(core->dev.atx_pll), 0x102, 0x1f, band | (tank << 3));
	pll_update(&(core->dev.atx_pll), 0x104, 0x7f, ((cpc & 0x38) << 1) | 0x04 | lfr);
	pll_update(&(core->dev.atx_pll), 0x105, 0x07, cpc & 0x7);

	pll_update(&(core->dev.atx_pll), 0x107, 0x0c, n << 2);
	pll_update(&(core->dev.atx_pll), 0x108, 0x07, l);
	a10_pll_write(&(core->dev.atx_pll), 0x109, m);

	pll_update(&(core->dev.atx_pll), XCVR_REG_CALIB_PLL_EN,
		XCVR_CALIB_PLL_EN_MASK, XCVR_CALIB_PLL_EN);

	altera_a10_release_arbitration(&(core->dev.atx_pll), TRUE);

	pll_calibration_check(&(core->dev.atx_pll));

	core->dev.atx_pll.initial_recalc = FALSE;

	return 0;
}

/*******************************************************************************
* @brief atx_pll_recalc_rate
*******************************************************************************/
uint32_t atx_pll_recalc_rate(xcvr_core *core,
	uint32_t fref_khz)
{
	uint32_t m, n, l;
	uint32_t tmp;
	uint32_t div0, div1;

	altera_a10_acquire_arbitration(&(core->dev.atx_pll));

	a10_pll_read(&(core->dev.atx_pll), 0x107, &div0);
	a10_pll_read(&(core->dev.atx_pll), 0x108, &div1);
	a10_pll_read(&(core->dev.atx_pll), 0x109, &m);

	altera_a10_release_arbitration(&(core->dev.atx_pll), FALSE);

	n = 1 << ((div0 >> 2) & 0x3);
	l = 1 << (div1 & 0x7);

	tmp = (uint32_t)fref_khz * m;
	tmp = DIV_ROUND_CLOSEST_ull(tmp, l*n/2);

	if (tmp != 0 && core->dev.atx_pll.initial_recalc)
		atx_pll_set_rate(core, tmp, fref_khz);

	return min_t(uint32_t, tmp * 2, ULONG_MAX);
}

#endif

/******************************************************************************/
/******************************************************************************/
