/***************************************************************************//**
 *   @file   util.c
 *   @brief  Implementation of Util Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#include "util.h"
#include "string.h"
#include "platform.h"

/******************************************************************************/
/*************************** Macros Definitions *******************************/
/******************************************************************************/
#define BITS_PER_LONG		32

/***************************************************************************//**
 * @brief clk_prepare_enable
*******************************************************************************/
int32_t clk_prepare_enable(struct clk *clk)
{
	if (clk) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/***************************************************************************//**
 * @brief clk_get_rate
*******************************************************************************/
uint32_t clk_get_rate(struct ad9361_rf_phy *phy,
					  struct refclk_scale *clk_priv)
{
	uint32_t rate = 0;
	uint32_t source;

	source = clk_priv->source;

	switch (source) {
		case TX_REFCLK:
		case RX_REFCLK:
		case BB_REFCLK:
			rate = ad9361_clk_factor_recalc_rate(clk_priv,
						phy->clk_refin->rate);
			break;
		case TX_RFPLL_INT:
		case RX_RFPLL_INT:
			rate = ad9361_rfpll_int_recalc_rate(clk_priv,
						phy->clks[clk_priv->parent_source]->rate);
			break;
		case RX_RFPLL_DUMMY:
		case TX_RFPLL_DUMMY:
			rate = ad9361_rfpll_dummy_recalc_rate(clk_priv);
			break;
		case TX_RFPLL:
		case RX_RFPLL:
			rate = ad9361_rfpll_recalc_rate(clk_priv);
			break;
		case BBPLL_CLK:
			rate = ad9361_bbpll_recalc_rate(clk_priv,
						phy->clks[clk_priv->parent_source]->rate);
			break;
		case ADC_CLK:
		case R2_CLK:
		case R1_CLK:
		case CLKRF_CLK:
		case RX_SAMPL_CLK:
		case DAC_CLK:
		case T2_CLK:
		case T1_CLK:
		case CLKTF_CLK:
		case TX_SAMPL_CLK:
			rate = ad9361_clk_factor_recalc_rate(clk_priv,
						phy->clks[clk_priv->parent_source]->rate);
			break;
		default:
			break;
	}

	return rate;
}

/***************************************************************************//**
 * @brief clk_set_rate
*******************************************************************************/
int32_t clk_set_rate(struct ad9361_rf_phy *phy,
					 struct refclk_scale *clk_priv,
					 uint32_t rate)
{
	uint32_t source;
	int32_t i;
	uint32_t round_rate;

	source = clk_priv->source;
	if(phy->clks[source]->rate != rate)
	{
		switch (source) {
			case TX_REFCLK:
			case RX_REFCLK:
			case BB_REFCLK:
				round_rate = ad9361_clk_factor_round_rate(clk_priv, rate,
								&phy->clk_refin->rate);
				ad9361_clk_factor_set_rate(clk_priv, round_rate,
						phy->clk_refin->rate);
				phy->clks[source]->rate = ad9361_clk_factor_recalc_rate(clk_priv,
												phy->clk_refin->rate);
				break;
			case TX_RFPLL_INT:
			case RX_RFPLL_INT:
				round_rate = ad9361_rfpll_int_round_rate(clk_priv, rate,
								&phy->clks[clk_priv->parent_source]->rate);
				ad9361_rfpll_int_set_rate(clk_priv, round_rate,
						phy->clks[clk_priv->parent_source]->rate);
				phy->clks[source]->rate = ad9361_rfpll_int_recalc_rate(clk_priv,
											phy->clks[clk_priv->parent_source]->rate);
				break;
			case RX_RFPLL_DUMMY:
			case TX_RFPLL_DUMMY:
				ad9361_rfpll_dummy_set_rate(clk_priv, rate);
				break;
			case TX_RFPLL:
			case RX_RFPLL:
				round_rate = ad9361_rfpll_round_rate(clk_priv, rate);
				ad9361_rfpll_set_rate(clk_priv, round_rate);
				phy->clks[source]->rate = ad9361_rfpll_recalc_rate(clk_priv);
				break;
			case BBPLL_CLK:
				round_rate = ad9361_bbpll_round_rate(clk_priv, rate,
								&phy->clks[clk_priv->parent_source]->rate);
				ad9361_bbpll_set_rate(clk_priv, round_rate,
					phy->clks[clk_priv->parent_source]->rate);
				phy->clks[source]->rate = ad9361_bbpll_recalc_rate(clk_priv,
											phy->clks[clk_priv->parent_source]->rate);
				phy->bbpll_initialized = true;
				break;
			case ADC_CLK:
			case R2_CLK:
			case R1_CLK:
			case CLKRF_CLK:
			case RX_SAMPL_CLK:
			case DAC_CLK:
			case T2_CLK:
			case T1_CLK:
			case CLKTF_CLK:
			case TX_SAMPL_CLK:
				round_rate = ad9361_clk_factor_round_rate(clk_priv, rate,
								&phy->clks[clk_priv->parent_source]->rate);
				ad9361_clk_factor_set_rate(clk_priv, round_rate,
						phy->clks[clk_priv->parent_source]->rate);
				phy->clks[source]->rate = ad9361_clk_factor_recalc_rate(clk_priv,
											phy->clks[clk_priv->parent_source]->rate);
				break;
			default:
				break;
		}
		for(i = BB_REFCLK; i < BBPLL_CLK; i++)
		{
			phy->clks[i]->rate = ad9361_clk_factor_recalc_rate(phy->ref_clk_scale[i],
									phy->clk_refin->rate);
		}
		phy->clks[BBPLL_CLK]->rate = ad9361_bbpll_recalc_rate(phy->ref_clk_scale[BBPLL_CLK],
										phy->clks[phy->ref_clk_scale[BBPLL_CLK]->parent_source]->rate);
		for(i = ADC_CLK; i < RX_RFPLL_INT; i++)
		{
			phy->clks[i]->rate = ad9361_clk_factor_recalc_rate(phy->ref_clk_scale[i],
									phy->clks[phy->ref_clk_scale[i]->parent_source]->rate);
		}
		for(i = RX_RFPLL_INT; i < RX_RFPLL_DUMMY; i++)
		{
			phy->clks[i]->rate = ad9361_rfpll_int_recalc_rate(phy->ref_clk_scale[i],
									phy->clks[phy->ref_clk_scale[i]->parent_source]->rate);
		}
		for(i = RX_RFPLL_DUMMY; i < RX_RFPLL; i++)
		{
			phy->clks[i]->rate = ad9361_rfpll_dummy_recalc_rate(phy->ref_clk_scale[i]);
		}
		for(i = RX_RFPLL; i < NUM_AD9361_CLKS; i++)
		{
			phy->clks[i]->rate = ad9361_rfpll_recalc_rate(phy->ref_clk_scale[i]);
		}
	} else {
		if ((source == BBPLL_CLK) && !phy->bbpll_initialized) {
			round_rate = ad9361_bbpll_round_rate(clk_priv, rate,
							&phy->clks[clk_priv->parent_source]->rate);
			ad9361_bbpll_set_rate(clk_priv, round_rate,
				phy->clks[clk_priv->parent_source]->rate);
			phy->clks[source]->rate = ad9361_bbpll_recalc_rate(clk_priv,
										phy->clks[clk_priv->parent_source]->rate);
			phy->bbpll_initialized = true;
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief int_sqrt
*******************************************************************************/
uint32_t int_sqrt(uint32_t x)
{
	uint32_t b, m, y = 0;

	if (x <= 1)
		return x;

	m = 1UL << (BITS_PER_LONG - 2);
	while (m != 0) {
		b = y + m;
		y >>= 1;

		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}

	return y;
}

/***************************************************************************//**
 * @brief ilog2
*******************************************************************************/
int32_t ilog2(int32_t x)
{
	int32_t A = !(!(x >> 16));
	int32_t count = 0;
	int32_t x_copy = x;

	count = count + (A << 4);

	x_copy = (((~A + 1) & (x >> 16)) + (~(~A + 1) & x));

	A = !(!(x_copy >> 8));
	count = count + (A << 3);
	x_copy = (((~A + 1) & (x_copy >> 8)) + (~(~A + 1) & x_copy));

	A = !(!(x_copy >> 4));
	count = count + (A << 2);
	x_copy = (((~A + 1) & (x_copy >> 4)) + (~(~A + 1) & x_copy));

	A = !(!(x_copy >> 2));
	count = count + (A << 1);
	x_copy = (((~A + 1) & (x_copy >> 2)) + (~(~A + 1) & x_copy));

	A = !(!(x_copy >> 1));
	count = count + A;

	return count;
}

/***************************************************************************//**
 * @brief do_div
*******************************************************************************/
uint64_t do_div(uint64_t* n, uint64_t base)
{
	uint64_t mod = 0;

	mod = *n % base;
	*n = *n / base;

	return mod;
}

/***************************************************************************//**
 * @brief find_first_bit
*******************************************************************************/
uint32_t find_first_bit(uint32_t word)
{
	int32_t num = 0;

	if ((word & 0xffff) == 0) {
			num += 16;
			word >>= 16;
	}
	if ((word & 0xff) == 0) {
			num += 8;
			word >>= 8;
	}
	if ((word & 0xf) == 0) {
			num += 4;
			word >>= 4;
	}
	if ((word & 0x3) == 0) {
			num += 2;
			word >>= 2;
	}
	if ((word & 0x1) == 0)
			num += 1;
	return num;
}

/***************************************************************************//**
 * @brief ERR_PTR
*******************************************************************************/
void * ERR_PTR(long error)
{
	return (void *) error;
}

/***************************************************************************//**
 * @brief zmalloc
*******************************************************************************/
void *zmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr)
		memset(ptr, 0, size);
	mdelay(1);

	return ptr;
}
