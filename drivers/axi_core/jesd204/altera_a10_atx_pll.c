/***************************************************************************//**
 *   @file   altera_a10_atx_pll.c
 *   @brief  Driver for the Altera ATX PLL dynamic reconfiguration.
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
#include <limits.h>
#include "no_os_util.h"
#include "altera_a10_atx_pll.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define A10_ATX_PLL_PFD_MIN 61440    /*  61.44 Mhz */
#define A10_ATX_PLL_PFD_MAX 800000   /* 800.00 Mhz */
#define A10_ATX_PLL_VCO_MIN 7200000  /*   7.20 GHz */
#define A10_ATX_PLL_VCO_MAX 14400000 /*  14.40 GHz */

static const uint32_t altera_a10_atx_bands[] = {
	14200000, 13650000, 13150000, 13110000, 12600000,
	11900000, 11550000, 11400000, 11100000, 10600000,
	10400000,  9750000,  9350000,  9100000,  8900000,
	8800000,  8600000,  8200000,  7800000,  7400000,
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief altera_a10_atx_lookup_band
 */
static uint32_t altera_a10_atx_lookup_band(uint32_t fvco)
{
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(altera_a10_atx_bands); i++) {
		if (fvco > altera_a10_atx_bands[i]) {
			i = i % 8;
			if (i != 7)
				i++;
			return i;
		}
	}

	return 5;
}

/**
 * @brief altera_a10_atx_lookup_tank
 */
static uint32_t altera_a10_atx_lookup_tank(uint32_t fvco)
{
	if (fvco < 8800000)
		return 0;
	else if (fvco < 11400000)
		return 1;
	else
		return 2;
}

/**
 * @brief altera_a10_atx_lookup_cp_current
 */
static uint32_t altera_a10_atx_lookup_cp_current(uint32_t m)
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

/**
 * @brief altera_a10_atx_lookup_lf_resistance
 */
static uint32_t altera_a10_atx_lookup_lf_resistance(uint32_t m)
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

/**
 * @brief altera_a10_atx_calc_params
 */
static void altera_a10_atx_calc_params(uint32_t fref,
				       uint32_t fout, uint32_t *best_n, uint32_t *best_m,
				       uint32_t *best_l, uint32_t *best_fvco)
{
	uint32_t m, m_min, m_max;
	uint32_t n, l;
	uint32_t f, fvco, best_f;
	uint32_t pfd;

	*best_n = *best_m = *best_l = *best_fvco = 0;
	best_f = ULONG_MAX;

	m_min = no_os_max_t(uint32_t, NO_OS_DIV_ROUND_UP(A10_ATX_PLL_VCO_MIN / 2, fref),
			    8);
	m_max = no_os_min_t(uint32_t, A10_ATX_PLL_VCO_MAX / 2 * 8 / fref, 127);

	for (n = 1; n <= 8; n *= 2) {
		pfd = fref / n;
		if (pfd < A10_ATX_PLL_PFD_MIN)
			break;
		if (pfd > A10_ATX_PLL_VCO_MAX)
			continue;

		for (m = m_min; m <= m_max; m++) {
			fvco = fref * m * 2 / n;

			for (l = 1; l <= 16; l *= 2) {
				f = (fvco * 2) / l;

				if (abs(f - fout) < abs(best_f - fout)) {
					best_f = f;
					*best_n = n;
					*best_m = m;
					*best_l = l;
					*best_fvco = fvco;
					if (f == fout)
						return;
				}
			}
		}
	}
}

/**
 * @brief altera_a10_atx_pll_round_rate
 */
int32_t altera_a10_atx_pll_round_rate(struct adxcvr *xcvr,
				      uint32_t rate_khz)
{
	uint32_t n, m, l;
	uint32_t fvco;
	uint64_t tmp;

	altera_a10_atx_calc_params(xcvr->parent_rate_khz, rate_khz, &n, &m, &l, &fvco);

	if (n == 0 || m == 0 || l == 0)
		return -1;

	tmp = xcvr->parent_rate_khz * m * 4;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, l * n);

	return no_os_min_t(uint64_t, tmp, LONG_MAX);
}

/**
 * @brief altera_a10_atx_pll_set_rate
 */
int32_t altera_a10_atx_pll_set_rate(struct adxcvr *xcvr,
				    uint32_t rate_khz)
{
	uint32_t n, m, l;
	uint32_t fvco;
	uint32_t lfr, cpc, band, tank;

	altera_a10_atx_calc_params(xcvr->parent_rate_khz, rate_khz, &n, &m, &l, &fvco);

	if (n == 0 || m == 0 || l == 0)
		return -1;

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

	lfr = altera_a10_atx_lookup_lf_resistance(m);
	cpc = altera_a10_atx_lookup_cp_current(m);
	band = altera_a10_atx_lookup_band(fvco);
	tank = altera_a10_atx_lookup_tank(fvco);

	adxcvr_pre_lane_rate_change(xcvr);
	atx_pll_acquire_arbitration(xcvr);

	atx_pll_update(xcvr, 0x102, 0x1f, band | (tank << 3));
	atx_pll_update(xcvr, 0x104, 0x7f, ((cpc & 0x38) << 1) | 0x04 | lfr);
	atx_pll_update(xcvr, 0x105, 0x07, cpc & 0x7);

	atx_pll_update(xcvr, 0x107, 0x0c, n << 2);
	atx_pll_update(xcvr, 0x108, 0x07, l);
	atx_pll_write(xcvr, 0x109, m);

	atx_pll_update(xcvr, XCVR_REG_CALIB_ATX_PLL_EN,
		       XCVR_CALIB_ATX_PLL_EN_MASK, XCVR_CALIB_ATX_PLL_EN);
	atx_pll_release_arbitration(xcvr, true);

	atx_pll_calibration_check(xcvr);
	xcvr_calib_tx(xcvr);
	adxcvr_post_lane_rate_change(xcvr, rate_khz);

	xcvr->initial_recalc = false;

	return 0;
}

/**
 * @brief altera_a10_atx_pll_recalc_rate
 */
uint32_t altera_a10_atx_pll_recalc_rate(struct adxcvr *xcvr)
{
	uint32_t m, n, l;
	uint64_t tmp;
	uint32_t div0, div1;

	atx_pll_acquire_arbitration(xcvr);

	atx_pll_read(xcvr, 0x107, &div0);
	atx_pll_read(xcvr, 0x108, &div1);
	atx_pll_read(xcvr, 0x109, &m);

	atx_pll_release_arbitration(xcvr, false);

	n = 1 << ((div0 >> 2) & 0x3);
	l = 1 << (div1 & 0x7);

	tmp = xcvr->parent_rate_khz * m;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, l * n / 4);

	if (tmp != 0 && xcvr->initial_recalc)
		altera_a10_atx_pll_set_rate(xcvr, tmp);

	return no_os_min_t(uint64_t, tmp, ULONG_MAX);
}

