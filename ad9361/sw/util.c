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
#include "xspips.h"
#include "xgpiops.h"
#include "util.h"

/******************************************************************************/
/*************************** Macros Definitions *******************************/
/******************************************************************************/
#define BITS_PER_LONG		32
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
XSpiPs_Config	*spi_config;
XSpiPs			spi_instance;
XGpioPs_Config	*gpio_config;
XGpioPs			gpio_instance;

/***************************************************************************//**
 * @brief udelay
*******************************************************************************/
void udelay(unsigned long usecs)
{
	usleep(usecs);
}

/***************************************************************************//**
 * @brief mdelay
*******************************************************************************/
void mdelay(unsigned long msecs)
{
	usleep(msecs * 1000);
}

/***************************************************************************//**
 * @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t  clk_pha,
				 uint8_t  clk_pol)
{
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;
	uint8_t  byte		 = 0;

	spi_config = XSpiPs_LookupConfig(device_id);
	base_addr = spi_config->BaseAddress;
	XSpiPs_CfgInitialize(&spi_instance, spi_config, base_addr);

	control_val = XSPIPS_CR_SSFORCE_MASK |
				  XSPIPS_CR_SSCTRL_MASK |
				  4 << XSPIPS_CR_PRESC_SHIFT |
				  (clk_pha ? XSPIPS_CR_CPHA_MASK : 0) |
				  (clk_pol ? XSPIPS_CR_CPOL_MASK : 0) |
				  XSPIPS_CR_MSTREN_MASK;

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET, control_val);

	for(byte = 0; byte < 128; byte++)
	{
		XSpiPs_ReadReg(base_addr, XSPIPS_RXD_OFFSET);
	}

	return SUCCESS;
}

/***************************************************************************//**
 * @brief spi_read
*******************************************************************************/
int32_t spi_read(uint8_t *data,
				 uint8_t bytes_number)
{
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;
	uint32_t status	  	 = 0;
	uint32_t cnt		 = 0;

	base_addr = spi_config->BaseAddress;
	control_val = XSpiPs_ReadReg(base_addr, XSPIPS_CR_OFFSET);

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET,
					control_val & ~(1 << XSPIPS_CR_SSCTRL_SHIFT));

	XSpiPs_WriteReg(base_addr, XSPIPS_TXWR_OFFSET, 0x01);

	XSpiPs_WriteReg(base_addr, XSPIPS_SR_OFFSET, XSPIPS_IXR_TXOW_MASK);
	XSpiPs_WriteReg(base_addr, XSPIPS_IER_OFFSET, XSPIPS_IXR_TXOW_MASK);

	while(cnt < bytes_number)
	{
		XSpiPs_WriteReg(base_addr, XSPIPS_TXD_OFFSET, data[cnt]);
		cnt++;
	}

	XSpiPs_WriteReg(base_addr, XSPIPS_ER_OFFSET, XSPIPS_ER_ENABLE_MASK);

	do
	{
		status = XSpiPs_ReadReg(base_addr, XSPIPS_SR_OFFSET);
	}
	while((status & XSPIPS_IXR_TXOW_MASK) == 0x0);

	XSpiPs_WriteReg(base_addr, XSPIPS_SR_OFFSET, XSPIPS_IXR_TXOW_MASK);

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET, control_val);

	cnt = 0;
	while(cnt < bytes_number)
	{
		data[cnt] = XSpiPs_ReadReg(base_addr, XSPIPS_RXD_OFFSET);
		cnt++;
	}

	XSpiPs_WriteReg(base_addr, XSPIPS_ER_OFFSET, 0x0);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief spi_write_then_read
*******************************************************************************/
int spi_write_then_read(const unsigned char *txbuf, unsigned n_tx,
						unsigned char *rxbuf, unsigned n_rx)
{
	uint8_t buffer[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00};
	uint8_t byte;

	for(byte = 0; byte < n_tx; byte++)
	{
		buffer[byte] = (unsigned char)txbuf[byte];
	}
	spi_read(buffer, n_tx + n_rx);
	for(byte = n_tx; byte < n_tx + n_rx; byte++)
	{
		rxbuf[byte - n_tx] = buffer[byte];
	}

	return SUCCESS;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
void gpio_init(uint32_t device_id)
{
	gpio_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&gpio_instance, gpio_config, gpio_config->BaseAddr);
}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
void gpio_direction(uint8_t pin, uint8_t direction)
{
	XGpioPs_SetDirectionPin(&gpio_instance, pin, direction);
	XGpioPs_SetOutputEnablePin(&gpio_instance, pin, 1);
}

/***************************************************************************//**
 * @brief gpio_data
*******************************************************************************/
void gpio_data(uint8_t pin, uint8_t data)
{
	XGpioPs_WritePin(&gpio_instance, pin, data);
}

/***************************************************************************//**
 * @brief gpio_is_valid
*******************************************************************************/
bool gpio_is_valid(int number)
{
	return 1;
}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
void gpio_set_value(unsigned gpio, int value)
{
	gpio_data(gpio, value);
}

/***************************************************************************//**
 * @brief clk_prepare_enable
*******************************************************************************/
int clk_prepare_enable(struct clk *clk)
{
	return 0;
}

/***************************************************************************//**
 * @brief clk_get_rate
*******************************************************************************/
unsigned long clk_get_rate(struct ad9361_rf_phy *phy,
						   struct refclk_scale *clk_priv)
{
	unsigned long rate;
	u32 source;

	source = clk_priv->source;

	switch (source) {
		case TX_REFCLK:
		case RX_REFCLK:
		case BB_REFCLK:
			rate = ad9361_clk_factor_recalc_rate(clk_priv,
						phy->clk_refin->rate);
			break;
		case TX_RFPLL:
		case RX_RFPLL:
			rate = ad9361_rfpll_recalc_rate(clk_priv,
						phy->clks[clk_priv->parent_source]->rate);
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
int clk_set_rate(struct ad9361_rf_phy *phy,
				 struct refclk_scale *clk_priv,
				 unsigned long rate)
{
	u32 source;
	int i;
	unsigned long round_rate;

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
			case TX_RFPLL:
			case RX_RFPLL:
				round_rate = ad9361_rfpll_round_rate(clk_priv, rate,
								&phy->clks[clk_priv->parent_source]->rate);
				ad9361_rfpll_set_rate(clk_priv, round_rate,
						phy->clks[clk_priv->parent_source]->rate);
				phy->clks[source]->rate = ad9361_rfpll_recalc_rate(clk_priv,
											phy->clks[clk_priv->parent_source]->rate);
				break;
			case BBPLL_CLK:
				round_rate = ad9361_bbpll_round_rate(clk_priv, rate,
								&phy->clks[clk_priv->parent_source]->rate);
				ad9361_bbpll_set_rate(clk_priv, round_rate,
					phy->clks[clk_priv->parent_source]->rate);
				phy->clks[source]->rate = ad9361_bbpll_recalc_rate(clk_priv,
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
		for(i = ADC_CLK; i < RX_RFPLL; i++)
		{
			phy->clks[i]->rate = ad9361_clk_factor_recalc_rate(phy->ref_clk_scale[i],
									phy->clks[phy->ref_clk_scale[i]->parent_source]->rate);
		}
		for(i = RX_RFPLL; i < NUM_AD9361_CLKS; i++)
		{
			phy->clks[i]->rate = ad9361_rfpll_recalc_rate(phy->ref_clk_scale[i],
									phy->clks[phy->ref_clk_scale[i]->parent_source]->rate);
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief int_sqrt
*******************************************************************************/
unsigned long int_sqrt(unsigned long x)
{
	unsigned long b, m, y = 0;

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
int ilog2(int x)
{
	int A = !(!(x >> 16));
	int count = 0;
	int x_copy = x;

	count = count + (A<<4);

	x_copy = (((~A + 1) & (x >> 16)) + (~(~A+1) & x));

	A = !(!(x_copy >> 8));
	count = count + (A<<3);
	x_copy = (((~A + 1) & (x_copy >> 8)) + (~(~A+1) & x_copy));

	A = !(!(x_copy >> 4));
	count = count + (A<<2);
	x_copy = (((~A + 1) & (x_copy >> 4)) + (~(~A+1) & x_copy));

	A = !(!(x_copy >> 2));
	count = count + (A<<1);
	x_copy = (((~A + 1) & (x_copy >> 2)) + (~(~A+1) & x_copy));

	A = !(!(x_copy >> 1));
	count = count + A;

	return count;
}
