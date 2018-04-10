/***************************************************************************//**
 * @file clkgen_core.c
 * @brief Implementation of CLKGEN Core Driver.
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
#include <stdbool.h>
#include "mykonos.h"
#include "clkgen_core.h"

#define FPLL_REG_C_COUNTER0			0x10D
#define FPLL_REG_C_COUNTER1			0x10E
#define FPLL_REG_C_COUNTER2			0x10F
#define FPLL_REG_C_COUNTER3			0x110
#define FPLL_REG_C_COUNTER4			0x111

#define FPLL_REG_DIV_COUNTER0		0x12B
#define FPLL_REG_DIV_COUNTER1		0x12C

#define A10_FPLL_PFD_MIN	25000    /* 25.000 MHz */
#define A10_FPLL_PFD_MAX	60000    /* 60.000 MHz */
#define A10_FPLL_VCO_MIN	4800000  /*  4.800 GHz */
#define A10_FPLL_VCO_MAX	14025000 /* 14.025 GHz */

#define BIT(bit)			(1UL << (bit))
#define UINT32_T_MAX    	0xffffffff

#define min(x,y)			((x) < (y) ? (x) : (y))
#define min_t(type, x, y)	(type) min((type) (x), (type) (y))
#define max(x,y)			((x) > (y) ? (x) : (y))
#define max_t(type, x, y)	(type) max((type) (x), (type) (y))
#define clamp(val, min_val, max_val)			\
	(max(min((val), (max_val)), (min_val)))
#define clamp_t(type, val, min_val, max_val)	\
	(type) clamp((type) (val), (type) (min_val), (type) (max_val))
#define __round_mask(x, y)	((__typeof__(x))((y)-1))
#define round_up(x, y)		((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y)	((x) & ~__round_mask(x, y))

#define DIV_ROUND_UP(x, y)			(((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y)		(((x) + (y) / 2) / (y))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_address;
} a10_fpll;

/***************************************************************************//**
 * @brief altera_a10_fpll_read
 *******************************************************************************/
static uint32_t altera_a10_fpll_read(a10_fpll *fpll,
									uint32_t reg_addr)
{
	return IORD_32DIRECT(fpll->base_address, reg_addr * 4);
}

/***************************************************************************//**
 * @brief altera_a10_fpll_write
 *******************************************************************************/
static void altera_a10_fpll_write(a10_fpll *fpll,
								  uint32_t reg_addr,
								  uint32_t reg_data)
{
	IOWR_32DIRECT(fpll->base_address, reg_addr * 4, reg_data);;
}

/***************************************************************************//**
 * @brief altera_a10_fpll_update
 *******************************************************************************/
static void altera_a10_fpll_update(a10_fpll *fpll,
								   uint32_t addr,
								   uint32_t mask,
								   uint32_t val)
{
	uint32_t rval;

	rval = altera_a10_fpll_read(fpll, addr);
	rval &= ~mask;
	rval |= val;
	altera_a10_fpll_write(fpll, addr, rval);
}

/***************************************************************************//**
 * @brief altera_a10_acquire_arbitration
 *******************************************************************************/
static uint32_t altera_a10_acquire_arbitration(a10_fpll *fpll)
{
	uint32_t timeout = 0;
	uint32_t status;

	altera_a10_fpll_write(fpll, 0x00, 0x2);

	do {
		status = altera_a10_fpll_read(fpll, 0x280);
		if ((status & BIT(2)) == 0) {
			printf("%s: Acquired arbitration: %lu us\n", __func__,
				timeout * 10);
			return 0;
		}
		udelay(10);
	} while (timeout++ < 10000);

	printf("%s: Failed to acquire arbitration\n", __func__);

	return 0;
}

/***************************************************************************//**
 * @brief altera_a10_release_arbitration
 *******************************************************************************/
static void altera_a10_release_arbitration(a10_fpll *fpll,
										   bool run_calibration)
{
	altera_a10_fpll_write(fpll, 0x00, run_calibration ? 0x1 : 0x3);
}

/***************************************************************************//**
 * @brief altera_a10_fpll_lookup_lf_resistance
 *******************************************************************************/
static uint32_t altera_a10_fpll_lookup_lf_resistance(uint32_t fvco,
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

/***************************************************************************//**
 * @brief altera_a10_fpll_lookup_cp_current
 *******************************************************************************/
static uint32_t altera_a10_fpll_lookup_cp_current(uint32_t fvco,
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

/***************************************************************************//**
 * @brief altera_a10_fpll_lookup_cp_current
 *******************************************************************************/
static int32_t altera_a10_fpll_calc_params(uint32_t fref,
										   uint32_t fout,
										   uint32_t *best_n,
										   uint32_t *best_m,
										   uint32_t *best_c0,
										   uint32_t *best_fvco)
{
	uint32_t n, n_min, n_max, _n_min, _n_max;
	uint32_t m, m_min, m_max;
	uint32_t c0;
	uint32_t f, fvco, best_f;

	*best_n = *best_m = *best_c0 = *best_fvco = 0;

	fref /= 1000;
	fout /= 1000;

	best_f = UINT32_T_MAX;

	n_min = max_t(uint32_t, DIV_ROUND_UP(fref, A10_FPLL_PFD_MAX), 1);
	n_max = min_t(uint32_t, fref / A10_FPLL_PFD_MIN, 31);

	m_min = max_t(uint32_t, DIV_ROUND_UP(A10_FPLL_VCO_MIN / 2, fref) * n_min, 8);
	m_max = min_t(uint32_t, A10_FPLL_VCO_MAX / 2 * n_max / fref, 127);
	m_min = round_up(m_min, 2);

	for (m = m_min; m <= m_max; m += 2) {
		_n_min = max_t(uint32_t, n_min, DIV_ROUND_UP(fref * m, A10_FPLL_VCO_MAX / 2));
		_n_max = min_t(uint32_t, n_max, fref * m / (A10_FPLL_VCO_MIN / 2));

		for (n = _n_min; n <= _n_max; n++) {
			fvco = fref * m * 2 / n;

			c0 = DIV_ROUND_CLOSEST(fvco, fout * 4);
			c0 = clamp_t(uint32_t, c0, 1, 512);
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

/***************************************************************************//**
 * @brief altera_a10_fpll_pll_calibration_check
 *******************************************************************************/
static int32_t altera_a10_fpll_pll_calibration_check(a10_fpll *fpll)
{
	uint32_t timeout = 0;
	uint32_t val;

	/* Wait max 100ms for cal_busy to de-assert */
	do {
		udelay(200);

		/* Read FPLL calibration status from capability register */
		val = altera_a10_fpll_read(fpll, 0x280);
		if ((val & 0x02) == 0x00) {
			printf("%s: FPLL PLL calibration OK (%lu us)\n",
				__func__, timeout * 200);
			return 0;
		}
	} while (timeout++ < 50);

	printf("%s: FPLL PLL calibration FAILED\n", __func__);

	return 1;
}

/***************************************************************************//**
 * @brief altera_a10_fpll_set_rate
 *******************************************************************************/
static int32_t altera_a10_fpll_set_rate(a10_fpll *fpll,
										uint32_t rate,
										uint32_t parent_rate)
{
	uint32_t feedback;
	uint32_t n, m, c0;
	uint32_t fvco;
	uint32_t div0, div1;
	uint32_t lfr, cpc;

	altera_a10_fpll_calc_params(parent_rate, rate, &n, &m, &c0, &fvco);

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

	return 0;
}

/***************************************************************************//**
 * @brief altera_a10_fpll_enable
 *******************************************************************************/
static int32_t altera_a10_fpll_enable(a10_fpll *fpll)
{
	altera_a10_fpll_write(fpll, 0x2e0, 0x2);

	return 0;
}

/***************************************************************************//**
 * @brief altera_a10_fpll_disable
 *******************************************************************************/
static void altera_a10_fpll_disable(a10_fpll *fpll)
{
	altera_a10_fpll_write(fpll, 0x2e0, 0x3);
}

/***************************************************************************//**
 * @brief clkgen_setup
 *******************************************************************************/
int32_t clkgen_setup(mykonosDevice_t *myk_dev)
{
	a10_fpll rx_dev_clk_pll;
	a10_fpll tx_dev_clk_pll;
	a10_fpll rx_os_dev_clk_pll;

	rx_dev_clk_pll.base_address =AD9371_RX_JESD204_LINK_PLL_RECONFIG_BASE;
	tx_dev_clk_pll.base_address = AD9371_TX_JESD204_LINK_PLL_RECONFIG_BASE;
	rx_os_dev_clk_pll.base_address = AD9371_RX_OS_JESD204_LINK_PLL_RECONFIG_BASE;

	altera_a10_fpll_disable(&rx_dev_clk_pll);
	altera_a10_fpll_disable(&tx_dev_clk_pll);
	altera_a10_fpll_disable(&rx_os_dev_clk_pll);

	altera_a10_fpll_set_rate(&rx_dev_clk_pll,
					myk_dev->rx->rxProfile->iqRate_kHz * 1000,
					myk_dev->clocks->deviceClock_kHz * 1000);
	altera_a10_fpll_set_rate(&tx_dev_clk_pll,
					myk_dev->tx->txProfile->iqRate_kHz * 500,
					myk_dev->clocks->deviceClock_kHz * 1000);
	altera_a10_fpll_set_rate(&rx_os_dev_clk_pll,
					myk_dev->obsRx->orxProfile->iqRate_kHz * 500,
					myk_dev->clocks->deviceClock_kHz * 1000);

	altera_a10_fpll_enable(&rx_dev_clk_pll);
	altera_a10_fpll_enable(&tx_dev_clk_pll);
	altera_a10_fpll_enable(&rx_os_dev_clk_pll);

	return 0;
}

