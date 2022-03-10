/***************************************************************************//**
 *   @file   altera_a10_cdr_pll.c
 *   @brief  Driver for the Altera CDR/CMU PLL dynamic reconfiguration.
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
#include "altera_a10_cdr_pll.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define A10_CDR_PLL_PFD_MIN 50000    /*  50.00 Mhz */
#define A10_CDR_PLL_PFD_MAX 800000   /* 800.00 Mhz */
#define A10_CDR_PLL_VCO_MIN 4900000  /*   4.90 GHz */
#define A10_CDR_PLL_VCO_MAX 14150000 /*  14.15 GHz */

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief altera_a10_cdr_lookup_vco_speed
 */
uint32_t altera_a10_cdr_lookup_vco_speed(uint32_t fvco)
{
	if (fvco < 7000000)
		return 3;
	else if (fvco < 12500000)
		return 2;
	else
		return 0;
}

/**
 * @brief altera_a10_cdr_lookup_vco_speed_fix
 */
uint32_t altera_a10_cdr_lookup_vco_speed_fix(uint32_t fvco)
{
	if (fvco < 6060000)
		return 60;
	else if (fvco < 7060000)
		return 74;
	else if (fvco < 9060000)
		return 90;
	else if (fvco < 11350000)
		return 116;
	else if (fvco < 12760000)
		return 150;
	else if (fvco < 13460000)
		return 159;
	else
		return 174;
}

/**
 * @brief altera_a10_cdr_lookup_cp_current_pd
 */
uint32_t altera_a10_cdr_lookup_cp_current_pd(uint32_t fvco,
		uint32_t lpd)
{
	if (lpd == 1 && fvco < 12000000)
		return 3;
	else
		return 4;
}

/**
 * @brief altera_a10_cdr_lookup_clklow_div
 */
uint32_t altera_a10_cdr_lookup_clklow_div(uint32_t fref)
{
	if (fref > 400000)
		return 1;
	else
		return 0;
}

/**
 * @brief altera_a10_cdr_lookup_fastlock
 */
uint32_t altera_a10_cdr_lookup_fastlock(uint32_t lpd)
{
	if (lpd == 16)
		return 1;
	else
		return 0;
}

/**
 * @brief altera_a10_cdr_lookup_lf_resistor_pd
 */
uint32_t altera_a10_cdr_lookup_lf_resistor_pd(uint32_t fvco,
		uint32_t lpd)
{
	if (fvco < 7000000) {
		switch (lpd) {
		case 1:
			return 2;
		case 2:
		case 4:
			return 3;
		default:
			return 0;
		}
	} else {
		switch (lpd) {
		case 1:
		case 2:
		case 4:
			return 3;
		default:
			return 0;
		}
	}
}

/**
 * @brief altera_a10_cdr_lookup_lf_resistor_pfd
 */
uint32_t altera_a10_cdr_lookup_lf_resistor_pfd(uint32_t fvco,
		uint32_t m, uint32_t lpfd)
{
	if (fvco < 7000000) {
		switch (m*lpfd) {
		case 8 ... 45:
			return 2;
		default:
			return 3;
		}
	} else if (fvco < 9000000) {
		switch (m*lpfd) {
		case 8 ... 57:
			return 2;
		case 58 ... 71:
			return 3;
		case 72 ... 85:
			return 2;
		default:
			return 3;
		}
	} else if (fvco < 12000000) {
		switch (m*lpfd) {
		case 8 ... 27:
			return 1;
		case 28 ... 71:
			return 2;
		case 72 ... 85:
			return 3;
		case 86 ... 129:
			return 2;
		default:
			return 3;
		}
	} else {
		switch (m*lpfd) {
		case 8 ... 27:
			return 1;
		case 28 ... 57:
			return 2;
		default:
			return 3;
		}
	}
}

/**
 * @brief altera_a10_cdr_lookup_cp_current_pfd
 */
uint32_t altera_a10_cdr_lookup_cp_current_pfd(uint32_t fvco,
		uint32_t m, uint32_t lpfd)
{
	if (fvco < 7000000) {
		switch (m*lpfd) {
		case 0 ... 35:
			return 2;
		case 36 ... 45:
			return 3;
		case 46 ... 71:
			return 1;
		case 72 ... 111:
			return 2;
		default:
			return 3;
		}
	} else if (fvco < 9000000) {
		switch (m*lpfd) {
		case 0 ... 27:
			return 1;
		case 28 ... 45:
			return 2;
		case 46 ... 57:
			return 3;
		case 58 ... 71:
			return 1;
		case 72 ... 85:
			return 4;
		case 86 ... 129:
			return 2;
		case 130 ... 141:
			return 3;
		case 142 ... 181:
			return 2;
		default:
			return 3;
		}
	} else if (fvco < 12000000) {
		switch (m*lpfd) {
		case 0 ... 27:
			return 3;
		case 28 ... 35:
			return 1;
		case 36 ... 71:
			return 2;
		case 72 ... 85:
			return 1;
		case 86 ... 129:
			return 4;
		default:
			return 2;
		}
	} else {
		switch (m*lpfd) {
		case 0 ... 27:
			return 4;
		case 28 ... 35:
			return 1;
		case 36 ... 45:
			return 2;
		case 46 ... 57:
			return 3;
		case 58 ... 71:
			return 1;
		case 72 ... 95:
			return 2;
		default:
			return 3;
		}
	}
}

/**
 * @brief altera_a10_cdr_calc_params
 */
void altera_a10_cdr_calc_params(uint32_t fref,
				uint32_t fout, uint32_t *best_n, uint32_t *best_m,
				uint32_t *best_lpfd, uint32_t *best_lpd,
				uint32_t *best_fvco)
{
	uint32_t m, m_min, m_max;
	uint32_t n, lpd, lpfd, lpfd_min;
	uint32_t fvco, target_fvco;
	uint32_t pfd;

	*best_n = *best_m = *best_lpfd = *best_lpd = *best_fvco = 0;


	fout /= 2;

	for (lpd = 1; lpd < 16; lpd *= 2) {
		if (fout >= A10_CDR_PLL_VCO_MIN / lpd)
			break;
	}

	target_fvco = fout * lpd;

	if (fout * lpd > 5200000)
		lpfd_min = 2;
	else
		lpfd_min = 1;

	m_min = no_os_max_t(uint32_t, NO_OS_DIV_ROUND_UP(A10_CDR_PLL_VCO_MIN / 2, fref),
			    8);
	m_max = no_os_min_t(uint32_t, A10_CDR_PLL_VCO_MAX * 8 / lpfd_min / fref, 127);

	for (n = 1; n <= 8; n *= 2) {
		pfd = fref / n;
		if (pfd < A10_CDR_PLL_PFD_MIN)
			break;
		if (pfd > A10_CDR_PLL_PFD_MAX)
			continue;

		for (lpfd = lpfd_min; lpfd <= 2; lpfd++) {
			for (m = m_min; m <= m_max; m++) {
				fvco = fref * m * lpfd / n;

				if (abs(fvco - target_fvco) < abs(*best_fvco - target_fvco)) {
					*best_n = n;
					*best_m = m;
					*best_lpfd = lpfd;
					*best_fvco = fvco;
					*best_lpd = lpd;
					if (target_fvco == fvco)
						return;
				}
			}
		}
	}
}

/**
 * @brief altera_a10_cdr_pll_round_rate
 */
long altera_a10_cdr_pll_round_rate(struct adxcvr *xcvr,
				   uint32_t rate_khz)
{
	uint32_t n, m, lpfd, lpd;
	uint32_t fvco;
	uint64_t tmp;

	altera_a10_cdr_calc_params(xcvr->parent_rate_khz, rate_khz, &n, &m, &lpfd,
				   &lpd, &fvco);

	if (n == 0 || m == 0 || lpfd == 0 || lpd == 0)
		return -1;

	tmp = xcvr->parent_rate_khz * m * lpfd * 2;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, n * lpd);

	return no_os_min_t(uint64_t, tmp, LONG_MAX);
}

/**
 * @brief altera_a10_cdr_pll_set_rate
 */
int32_t altera_a10_cdr_pll_set_rate(struct adxcvr *xcvr,
				    uint32_t rate_khz)
{
	uint32_t n, m, lpfd, lpd;
	uint32_t fvco;
	uint32_t i;
	uint32_t vco_speed, vco_speed_fix;
	uint32_t cp_current_pd, lfr_pd;
	uint32_t cp_current_pfd, lfr_pfd;
	uint32_t clkdiv_low;
	uint32_t fast_lock;

	altera_a10_cdr_calc_params(xcvr->parent_rate_khz, rate_khz, &n, &m, &lpfd, &lpd,
				   &fvco);

	if (n == 0 || m == 0 || lpfd == 0 || lpd == 0)
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

	switch (lpd) {
	case 1:
		lpd = 1;
		break;
	case 2:
		lpd = 3;
		break;
	case 4:
		lpd = 4;
		break;
	case 8:
		lpd = 5;
		break;
	default:
		lpd = 6;
		break;
	}

	switch (lpfd) {
	case 1:
		lpfd = 2;
		break;
	default:
		lpfd = 3;
		break;
	}

	vco_speed = altera_a10_cdr_lookup_vco_speed(fvco);
	vco_speed_fix = altera_a10_cdr_lookup_vco_speed_fix(fvco);
	clkdiv_low = altera_a10_cdr_lookup_clklow_div(xcvr->parent_rate_khz);
	fast_lock = altera_a10_cdr_lookup_fastlock(lpd);
	cp_current_pd = altera_a10_cdr_lookup_cp_current_pd(fvco, lpd);
	lfr_pd = altera_a10_cdr_lookup_lf_resistor_pd(fvco, lpd);
	cp_current_pfd = altera_a10_cdr_lookup_cp_current_pfd(fvco, m, lpfd);
	lfr_pfd = altera_a10_cdr_lookup_lf_resistor_pfd(fvco, m, lpfd);

	adxcvr_pre_lane_rate_change(xcvr);

	for (i = 0; i < xcvr->lanes_per_link; i++) {
		adxcfg_acquire_arbitration(xcvr, i);

		adxcfg_update(xcvr, i, 0x132, 0x05,
			      ((vco_speed_fix & 0x80) >> 7) | ((vco_speed_fix & 0x40) >> 4));
		adxcfg_update(xcvr, i, 0x133, 0xe0, cp_current_pd << 5);
		adxcfg_update(xcvr, i, 0x134, 0x40, (vco_speed_fix & 0x10) << 2);
		adxcfg_update(xcvr, i, 0x135, 0x4f,
			      (vco_speed_fix & 0x20) << 1 | (lfr_pd << 2) | lfr_pfd);
		adxcfg_update(xcvr, i, 0x136, 0x0f, vco_speed_fix & 0xf);
		adxcfg_update(xcvr, i, 0x137, 0x7c, vco_speed << 2);
		adxcfg_update(xcvr, i, 0x139, 0xbf,
			      (fast_lock << 7) | (cp_current_pd << 3) | cp_current_pfd);

		adxcfg_write(xcvr, i, 0x13a, (clkdiv_low << 6) | (lpd << 3) | lpfd);
		adxcfg_write(xcvr, i, 0x13b, m);
		adxcfg_update(xcvr, i, 0x13c, 0x0c, n << 2);

		adxcfg_update(xcvr, i, XCVR_REG_CALIB_PMA_EN,
			      XCVR_CALIB_CMU_CDR_PLL_EN_MASK,
			      XCVR_CALIB_CMU_CDR_PLL_EN);

		adxcfg_update(xcvr, i, XCVR_REG_RATE_SWITCH_FLAG,
			      XCVR_RATE_SWITCH_FLAG_MASK,
			      XCVR_RATE_SWITCH_FLAG_RATE_SWITCH);

		/*
		 * Disable tx_cal_busy and enable rx_cal_busy output through
		 * capability register.
		 */
		adxcfg_update(xcvr, i, XCVR_REG_CAPAB_PMA,
			      XCVR_CAPAB_RX_CAL_BUSY_EN_MASK |
			      XCVR_CAPAB_TX_CAL_BUSY_EN_MASK,
			      XCVR_CAPAB_RX_CAL_BUSY_EN |
			      XCVR_CAPAB_TX_CAL_BUSY_DIS);

		adxcfg_release_arbitration(xcvr, i, true);

		adxcfg_calibration_check(xcvr, i, false);
	}

	adxcvr_post_lane_rate_change(xcvr, rate_khz);

	xcvr->initial_recalc = false;

	return 0;
}

/**
 * @brief altera_a10_cdr_pll_recalc_rate
 */
uint32_t altera_a10_cdr_pll_recalc_rate(struct adxcvr *xcvr)
{
	uint32_t m, n, lpd, lpfd;
	uint64_t tmp;
	uint32_t div0, div1;

	adxcfg_acquire_arbitration(xcvr, 0);

	adxcfg_read(xcvr, 0, 0x13a, &div0);
	adxcfg_read(xcvr, 0, 0x13b, &m);
	adxcfg_read(xcvr, 0, 0x13c, &div1);

	adxcfg_release_arbitration(xcvr, 0, false);

	switch ((div0 >> 3) & 0x7) {
	case 1:
		lpd = 1;
		break;
	case 3:
		lpd = 2;
		break;
	case 4:
		lpd = 4;
		break;
	case 5:
		lpd = 8;
		break;
	default:
		lpd = 16;
		break;
	}

	switch (div0 & 0x7) {
	case 2:
		lpfd = 1;
		break;
	default:
		lpfd = 2;
		break;
	}

	n = 1 << ((div1 >> 2) & 0x3);

	tmp = xcvr->parent_rate_khz * m * lpfd * 2;
	tmp = NO_OS_DIV_ROUND_CLOSEST_ULL(tmp, n * lpd);

	if (tmp != 0 && xcvr->initial_recalc)
		altera_a10_cdr_pll_set_rate(xcvr, tmp);

	return no_os_min_t(uint64_t, tmp, ULONG_MAX);
}

