
/***************************************************************************//**
 *   @file   ad9361_conv.c
 *   @brief  Implementation of AD9361 Conv Driver.
********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
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
#include <inttypes.h>
#include <string.h>
#include "ad9361.h"
#include "platform.h"

#ifndef AXI_ADC_NOT_PRESENT
/**
 * HDL loopback enable/disable.
 * @param phy The AD9361 state structure.
 * @param enable Enable/disable option.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9361_hdl_loopback(struct ad9361_rf_phy *phy, bool enable)
{
	struct axiadc_converter *conv = phy->adc_conv;
	struct axiadc_state *st = phy->adc_state;
	int32_t reg, addr, chan;

	uint32_t version = axiadc_read(st, 0x4000);

	/* Still there but implemented a bit different */
	if (PCORE_VERSION_MAJOR(version) > 7)
		addr = 0x4418;
	else
		addr = 0x4414;

	for (chan = 0; chan < conv->chip_info->num_channels; chan++) {
		reg = axiadc_read(st, addr + (chan) * 0x40);

		if (PCORE_VERSION_MAJOR(version) > 7) {
		/* FIXME: May cause problems if DMA is selected */
			if (enable)
				reg = 0x8;
			else
				reg = 0x0;
		} else {
		/* DAC_LB_ENB If set enables loopback of receive data */
			if (enable)
				reg |= BIT(1);
			else
				reg &= ~BIT(1);
		}
		axiadc_write(st, addr + (chan) * 0x40, reg);
	}

	return 0;
}

/**
 * Digital interface timing analysis.
 * @param phy The AD9361 state structure.
 * @param buf The buffer.
 * @param buflen The buffer length.
 * @return The size in case of success, negative error code otherwise.
 */
int32_t ad9361_dig_interface_timing_analysis(struct ad9361_rf_phy *phy,
	char *buf, int32_t buflen)
{
	struct axiadc_state *st = phy->adc_state;
	int32_t ret, i, j, chan, len = 0;
	uint8_t field[16][16];
	uint8_t rx;

	rx = ad9361_spi_read(phy->spi, REG_RX_CLOCK_DATA_DELAY);

	ad9361_bist_prbs(phy, BIST_INJ_RX);

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY,
				DATA_CLK_DELAY(j) | RX_DATA_DELAY(i));
			for (chan = 0; chan < 4; chan++)
				axiadc_write(st, ADI_REG_CHAN_STATUS(chan),
				ADI_PN_ERR | ADI_PN_OOS);

			mdelay(1);

			if (axiadc_read(st, ADI_REG_STATUS) & ADI_STATUS) {
				for (chan = 0, ret = 0; chan < 4; chan++)
					ret |= axiadc_read(st, ADI_REG_CHAN_STATUS(chan));
			}
			else {
				ret = 1;
			}

			field[i][j] = ret;
		}
	}

	ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY, rx);

	ad9361_bist_prbs(phy, BIST_DISABLE);

	len += snprintf(buf + len, buflen, "CLK: %"PRIu32" Hz 'o' = PASS\n",
		clk_get_rate(phy, phy->ref_clk_scale[RX_SAMPL_CLK]));
	len += snprintf(buf + len, buflen, "DC");
	for (i = 0; i < 16; i++)
		len += snprintf(buf + len, buflen, "%"PRIx32":", i);
	len += snprintf(buf + len, buflen, "\n");

	for (i = 0; i < 16; i++) {
		len += snprintf(buf + len, buflen, "%"PRIx32":", i);
		for (j = 0; j < 16; j++) {
			len += snprintf(buf + len, buflen, "%c ",
				(field[i][j] ? '.' : 'o'));
		}
		len += snprintf(buf + len, buflen, "\n");
	}
	len += snprintf(buf + len, buflen, "\n");

	return len;
}

/**
 * Digital tune.
 * @param phy The AD9361 state structure.
 * @param max_freq Maximum frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9361_dig_tune(struct ad9361_rf_phy *phy, uint32_t max_freq)
{
	struct axiadc_converter *conv = phy->adc_conv;
	struct axiadc_state *st = phy->adc_state;
	int32_t ret, i, j, k, chan, t, num_chan, err = 0;
	uint32_t s0, s1, c0, c1, tmp, saved = 0;
	uint8_t field[2][16];

	uint32_t hdl_dac_version = axiadc_read(st, 0x4000);

	if (phy->pdata->dig_interface_tune_skipmode == 2) {
	/* skip completely and use defaults */
		ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY,
				phy->pdata->port_ctrl.rx_clk_data_delay);

		ad9361_spi_write(phy->spi, REG_TX_CLOCK_DATA_DELAY,
				phy->pdata->port_ctrl.tx_clk_data_delay);

		return 0;
	}

	if (!phy->pdata->fdd) {
		ad9361_set_ensm_mode(phy, true, false);
		ad9361_ensm_force_state(phy, ENSM_STATE_FDD);
	} else {
		ad9361_ensm_force_state(phy, ENSM_STATE_ALERT);
		ad9361_ensm_restore_prev_state(phy);
	}

	num_chan = (conv->chip_info->num_channels > 4) ? 4 : conv->chip_info->num_channels;

	ad9361_bist_prbs(phy, BIST_INJ_RX);

	for (t = 0; t < 2; t++) {
		memset(field, 0, 32);
		for (k = 0; k < 2; k++) {
			if (max_freq)
				ad9361_set_trx_clock_chain_freq(phy, k ? max_freq : 10000000UL);
			for (i = 0; i < 2; i++) {
				for (j = 0; j < 16; j++) {
					ad9361_spi_write(phy->spi,
						REG_RX_CLOCK_DATA_DELAY + t,
						RX_DATA_DELAY(i == 0 ? j : 0) |
						DATA_CLK_DELAY(i ? j : 0));
					for (chan = 0; chan < num_chan; chan++)
						axiadc_write(st, ADI_REG_CHAN_STATUS(chan),
						ADI_PN_ERR | ADI_PN_OOS);
					mdelay(4);

					if ((t == 1) || (axiadc_read(st, ADI_REG_STATUS) & ADI_STATUS)) {
						for (chan = 0, ret = 0; chan < num_chan; chan++) {
							ret |= axiadc_read(st, ADI_REG_CHAN_STATUS(chan));
						}
					}
					else {
						ret = 1;
					}

					field[i][j] |= ret;
				}
			}
		}
#ifdef _DEBUG
		printk("SAMPL CLK: %"PRIu32"\n", clk_get_rate(phy, phy->ref_clk_scale[RX_SAMPL_CLK]));
		printk("  ");
		for (i = 0; i < 16; i++)
			printk("%"PRIx32":", i);
		printk("\n");

		for (i = 0; i < 2; i++) {
			printk("%"PRIx32":", i);
			for (j = 0; j < 16; j++) {
				printk("%c ", (field[i][j] ? '#' : 'o'));
			}
			printk("\n");
		}
		printk("\n");
#endif
		c0 = ad9361_find_opt(&field[0][0], 16, &s0);
		c1 = ad9361_find_opt(&field[1][0], 16, &s1);

		if (!c0 && !c1) {
			dev_err(&phy->spi->dev, "%s: Tuning %s FAILED!", __func__,
				t ? "TX" : "RX");
			err |= -EIO;
		}

		if (c1 > c0)
			ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY + t,
			DATA_CLK_DELAY(s1 + c1 / 2) |
			RX_DATA_DELAY(0));
		else
			ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY + t,
			DATA_CLK_DELAY(0) |
			RX_DATA_DELAY(s0 + c0 / 2));

		if (t == 0) {
			/* Now do the loopback and tune the digital out */
			ad9361_bist_prbs(phy, BIST_DISABLE);

			if (phy->pdata->dig_interface_tune_skipmode == 1) {
			/* skip TX */

				phy->pdata->port_ctrl.rx_clk_data_delay =
					ad9361_spi_read(phy->spi, REG_RX_CLOCK_DATA_DELAY);

				if (!phy->pdata->fdd) {
					ad9361_set_ensm_mode(phy, phy->pdata->fdd,
							     phy->pdata->ensm_pin_ctrl);
					ad9361_ensm_restore_prev_state(phy);
				}
				return 0;
			}

			ad9361_bist_loopback(phy, 1);

			for (chan = 0; chan < num_chan; chan++) {
				axiadc_write(st, ADI_REG_CHAN_CNTRL(chan),
					ADI_FORMAT_SIGNEXT | ADI_FORMAT_ENABLE |
					ADI_ENABLE | ADI_IQCOR_ENB);
				axiadc_set_pnsel(st, chan, ADC_PN_CUSTOM);
				if (PCORE_VERSION_MAJOR(hdl_dac_version) > 7)
				{
					axiadc_write(st, 0x4418 + (chan) * 0x40, 9);
					axiadc_write(st, 0x4044, 0x1);
				}
				else
					axiadc_write(st, 0x4414 + (chan) * 0x40, 1);

			}
			if (PCORE_VERSION_MAJOR(hdl_dac_version) < 8) {
				saved = tmp = axiadc_read(st, 0x4048);
				tmp &= ~0xF;
				tmp |= 1;
				axiadc_write(st, 0x4048, tmp);

			}
		} else {
			ad9361_bist_loopback(phy, 0);

			if (PCORE_VERSION_MAJOR(hdl_dac_version) < 8)
				axiadc_write(st, 0x4048, saved);

			for (chan = 0; chan < num_chan; chan++) {
				axiadc_write(st, ADI_REG_CHAN_CNTRL(chan),
					ADI_FORMAT_SIGNEXT | ADI_FORMAT_ENABLE |
					ADI_ENABLE | ADI_IQCOR_ENB);
				axiadc_set_pnsel(st, chan, ADC_PN9);
				if (PCORE_VERSION_MAJOR(hdl_dac_version) > 7)
				{
					axiadc_write(st, 0x4418 + (chan) * 0x40, 0);
					axiadc_write(st, 0x4044, 0x1);
				}
				else
					axiadc_write(st, 0x4414 + (chan) * 0x40, 0);

			}

			if (err == -EIO) {
				ad9361_spi_write(phy->spi, REG_RX_CLOCK_DATA_DELAY,
						phy->pdata->port_ctrl.rx_clk_data_delay);

				ad9361_spi_write(phy->spi, REG_TX_CLOCK_DATA_DELAY,
						phy->pdata->port_ctrl.tx_clk_data_delay);
				err = 0;
			} else {
				phy->pdata->port_ctrl.rx_clk_data_delay =
					ad9361_spi_read(phy->spi, REG_RX_CLOCK_DATA_DELAY);
				phy->pdata->port_ctrl.tx_clk_data_delay =
					ad9361_spi_read(phy->spi, REG_TX_CLOCK_DATA_DELAY);
			}

			if (!phy->pdata->fdd) {
				ad9361_set_ensm_mode(phy, phy->pdata->fdd, phy->pdata->ensm_pin_ctrl);
				ad9361_ensm_restore_prev_state(phy);
			}

			return err;
		}
	}

	return -EINVAL;
}

/**
* Setup the AD9361 device.
* @param phy The AD9361 state structure.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t ad9361_post_setup(struct ad9361_rf_phy *phy)
{
	struct axiadc_converter *conv = phy->adc_conv;
	struct axiadc_state *st = phy->adc_state;
	int32_t rx2tx2 = phy->pdata->rx2tx2;
	int32_t tmp, num_chan;
	int32_t i, ret;

	num_chan = (conv->chip_info->num_channels > 4) ? 4 : conv->chip_info->num_channels;

	axiadc_write(st, ADI_REG_CNTRL, rx2tx2 ? 0 : ADI_R1_MODE);
	tmp = axiadc_read(st, 0x4048);

	if (!rx2tx2) {
		axiadc_write(st, 0x4048, tmp | BIT(5)); /* R1_MODE */
		axiadc_write(st, 0x404c, 1); /* RATE */
	}
	else {
		tmp &= ~BIT(5);
		axiadc_write(st, 0x4048, tmp);
		axiadc_write(st, 0x404c, 3); /* RATE */
	}

	for (i = 0; i < num_chan; i++) {
		axiadc_write(st, ADI_REG_CHAN_CNTRL_1(i),
			ADI_DCFILT_OFFSET(0));
		axiadc_write(st, ADI_REG_CHAN_CNTRL_2(i),
			(i & 1) ? 0x00004000 : 0x40000000);
		axiadc_write(st, ADI_REG_CHAN_CNTRL(i),
			ADI_FORMAT_SIGNEXT | ADI_FORMAT_ENABLE |
			ADI_ENABLE | ADI_IQCOR_ENB);
	}

	ret = ad9361_dig_tune(phy, ((conv->chip_info->num_channels > 4) ||
		axiadc_read(st, 0x0004)) ? 0 : 61440000);
	if (ret < 0)
		return ret;

	ret = ad9361_set_trx_clock_chain(phy,
					 phy->pdata->rx_path_clks,
					 phy->pdata->tx_path_clks);

	ad9361_ensm_force_state(phy, ENSM_STATE_ALERT);
	ad9361_ensm_restore_prev_state(phy);

	return ret;
}
#else
/**
 * HDL loopback enable/disable.
 * @param phy The AD9361 state structure.
 * @param enable Enable/disable option.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9361_hdl_loopback(struct ad9361_rf_phy *phy, bool enable)
{
	return -ENODEV;
}

/**
* Setup the AD9361 device.
* @param phy The AD9361 state structure.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t ad9361_post_setup(struct ad9361_rf_phy *phy)
{
	return 0;
}
#endif
