/***************************************************************************//**
 *   @file   adrv9002_conv.c
 *   @brief  adrv9002 digital interface tuning utilities.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "parameters.h"
#include "adrv9002.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_ssi_types.h"

#define ADI_RX2_REG_OFF			0x1000
#define ADI_TX1_REG_OFF			0x2000
#define ADI_TX2_REG_OFF			0x4000
#define ADI_TX_REG_RATE			0x4c
#define ADI_TX_REG_CTRL_2		0x48
#define ADI_TX_REG_CHAN_CTRL_7(c)	(0x0418 + (c) * 0x40)
#define ADI_TX_REG_CTRL_1		0x44
#define R1_MODE				NO_OS_BIT(2)
#define TX_R1_MODE			NO_OS_BIT(5)

#define AIM_AXI_REG(off, addr)		((off) + (addr))
#define	NUM_LANES_MASK			NO_OS_GENMASK(12, 8)
#define NUM_LANES(x)			no_os_field_prep(NUM_LANES_MASK, x)
#define SDR_DDR_MASK			NO_OS_BIT(16)
#define SDR_DDR(x)			no_os_field_prep(SDR_DDR_MASK, x)
#define TX_REF_CLK_MASK			NO_OS_GENMASK(11, 10)
#define TX_REF_CLK(x)			no_os_field_get(TX_REF_CLK_MASK, x)
#define TX_FLEX_REF_CLK_MASK			NO_OS_BIT(14)
#define TX_FLEX_REF_CLK(x)		no_os_field_get(TX_FLEX_REF_CLK_MASK, x)
#define IS_CMOS(cfg)			((cfg) & (AXI_ADC_CMOS_OR_LVDS_N))

int adrv9002_axi_interface_set(const struct adrv9002_rf_phy *phy,
			       const uint8_t n_lanes,
			       const bool cmos_ddr,
			       const int channel,
			       const bool tx)
{
	uint32_t reg_ctrl, reg_value = 0, off, divider;
	uint8_t rate;

	if (tx) {
		off = channel ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
		reg_ctrl = ADI_TX_REG_CTRL_2;
	} else {
		off = channel ? ADI_RX2_REG_OFF : 0;
		reg_ctrl = AXI_ADC_REG_CNTRL;
	}

	axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, reg_ctrl), &reg_value);
	reg_value &= ~(NUM_LANES_MASK | SDR_DDR_MASK);

	switch (n_lanes) {
	case ADI_ADRV9001_SSI_1_LANE:
		reg_value |= NUM_LANES(1);
		if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
			reg_value |= SDR_DDR(!cmos_ddr);
		break;
	case ADI_ADRV9001_SSI_2_LANE:
		if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
			return -EINVAL;

		reg_value |= NUM_LANES(2);
		break;
	case ADI_ADRV9001_SSI_4_LANE:
		if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS)
			return -EINVAL;

		reg_value |= NUM_LANES(4);
		reg_value |= SDR_DDR(!cmos_ddr);
		break;
	default:
		return -EINVAL;
	}

	axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, reg_ctrl), reg_value);
	if (tx) {
		uint32_t ddr = cmos_ddr;

		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CLK_RATIO), &divider);
		/* in LVDS, data type is always DDR */
		if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS)
			ddr = 1;
		rate = 32 / ((1 << n_lanes) * (1 + ddr) * divider) - 1;
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_RATE), rate);
	}

	return 0;
}

static const char * const adrv9002_tx_clk[ADRV9002_RX2_REF_CLK + 1] = {
	"OWN REF", "RX1 REF", "RX2 REF"
};

int adrv9002_post_setup(struct adrv9002_rf_phy *phy)
{
	uint32_t axi_config = 0;
	uint32_t num_chan;
	int i;

	num_chan = phy->chip->num_channels;

	if (!phy->rx2tx2) {
		/* set R1_MODE to 1 rf channel in all channels */
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CNTRL, R1_MODE);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(ADI_RX2_REG_OFF, AXI_ADC_REG_CNTRL),
			      R1_MODE);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(ADI_TX1_REG_OFF, ADI_TX_REG_CTRL_2),
			      TX_R1_MODE);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(ADI_TX2_REG_OFF, ADI_TX_REG_CTRL_2),
			      TX_R1_MODE);
	}

	for (i = 0; i < num_chan; i++) {
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL_1(i),
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL_2(i),
			      (i & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL(i),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}

	axi_adc_read(phy->rx1_adc, AXI_ADC_REG_CONFIG, &axi_config);
	if (!IS_CMOS(axi_config))
		/*
		 * This whole mechanism of doing the device setup() in this
		 * post_setup() hook is ugly. We need to find a mechanism to find out the AXI
		 * interface type when probing the transceiver...
		 */
		phy->ssi_type = ADI_ADRV9001_SSI_TYPE_LVDS;
	else
		phy->ssi_type = ADI_ADRV9001_SSI_TYPE_CMOS;

	/* Get tx reference clock. It maybe be driven by it's own reference clock, RX1 or RX2 */
	for (i = 0; i < phy->chip->n_tx; i++) {
		unsigned int addr_off = i ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
		struct adrv9002_tx_chan *tx = &phy->tx_channels[i];

		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(addr_off, AXI_ADC_REG_CONFIG),
			     &axi_config);
		/*
		 * Does the HW supports fully configurable TX clocks assignments? If not, fallback
		 * to the old behavior. That is, 0 on bit 10 still means TX own reference and 1
		 * means driven by the RX on the same channel.
		 */
		if (TX_FLEX_REF_CLK(axi_config)) {
			tx->rx_ref_clk = TX_REF_CLK(axi_config);
			/*
			 * Sanity check as this is directly used to dereference RX ports from
			 * the channel array.
			 */
			if (tx->rx_ref_clk > ADRV9002_RX2_REF_CLK)
				return -EINVAL;
		} else {
			/* only bit 10 matters in legacy designs */
			tx->rx_ref_clk = TX_REF_CLK(axi_config) & NO_OS_BIT(0);
			if (tx->rx_ref_clk && i)
				tx->rx_ref_clk = ADRV9002_RX1_REF_CLK + i;
		}

		pr_debug("Tx%d SSI clk driven by %s\n", tx->channel.number,
			 adrv9002_tx_clk[tx->rx_ref_clk]);
	}

	return adrv9002_post_init(phy);
}

#ifdef DEBUG
static void adrv9002_axi_digital_tune_verbose(const struct adrv9002_rf_phy *phy,
		uint8_t field[][8],
		const bool tx, const int channel)
{
	int i, j;
	char c;
	const struct adrv9002_chan *ch;

	if (tx)
		ch = &phy->tx_channels[channel].channel;
	else
		ch = &phy->rx_channels[channel].channel;

	pr_info("SAMPL CLK: %lu tuning: %s%d\n",
		clk_get_rate(ch->clk), tx ? "TX" : "RX",
		channel ? 2 : 1);
	pr_info("  ");
	for (i = 0; i < 8; i++)
		pr_info("%x%s", i, i == 7 ? "" : ":");
	pr_info("\n");

	for (i = 0; i < 8; i++) {
		pr_info("%x:", i);
		for (j = 0; j < 8; j++) {
			if (field[i][j])
				c = '#';
			else
				c = 'o';
			pr_info("%c ", c);
		}
		pr_info("\n");
	}
}
#else
static void adrv9002_axi_digital_tune_verbose(const struct adrv9002_rf_phy *phy,
		uint8_t field[][8],
		const bool tx, const int channel)
{
}
#endif

static int adrv9002_axi_find_point(const uint8_t *field, const uint8_t sz,
				   uint8_t *data_start)
{
	int i = sz;
	int cnt = 0, start = -1, max_cnt = 0;

	for (i = 0; i < sz; i++) {
		if (!field[i]) {
			if (start == -1)
				start = i;

			cnt++;
		} else {
			if (cnt > max_cnt) {
				max_cnt = cnt;
				*data_start = start;
			}

			start = -1;
			cnt = 0;
		}
	}

	if (cnt > max_cnt) {
		max_cnt = cnt;
		*data_start = start;
	}

	if (!max_cnt)
		return -EIO;

	return max_cnt;
}

static int adrv9002_axi_pn_check(const struct adrv9002_rf_phy *phy,
				 const int chan,
				 const int n_chan)
{
	unsigned int off = chan ? ADI_RX2_REG_OFF : 0;
	uint32_t reg;
	int c;

	/* reset result */
	for (c = 0; c < n_chan; c++)
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(c)),
			      AXI_ADC_PN_ERR | AXI_ADC_PN_OOS);

	no_os_udelay(5000);

	/* check for errors in any channel */
	for (c = 0; c < n_chan; c++) {
		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(c)), &reg);
		if (reg) {
			pr_debug("pn error in c:%d, reg: %02X\n", chan, reg);
			return 1;
		}
	}

	return 0;
}

static int adrv9002_axi_tx_test_pattern_set(const struct adrv9002_rf_phy *phy,
		const int chan,
		const int n_chan, uint32_t *ctrl_7)
{
	unsigned int off = off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
	adi_adrv9001_SsiTestModeData_e pattern;
	int c, sel;

	pattern = adrv9002_get_test_pattern(phy, chan, false, false);
	if (pattern == ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE)
		sel = 10;
	else if (pattern == ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15)
		sel = 7;
	else if (pattern == ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7)
		sel = 6;
	else
		return -EINVAL;

	for (c = 0; c < n_chan; c++) {
		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)),
			     &ctrl_7[c]);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), sel);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_CTRL_1), 1);
	}

	return 0;
}

static void adrv9002_axi_tx_test_pattern_restore(struct axi_adc *axi_dev,
		const int chan, const int n_chan,
		const uint32_t *saved_ctrl_7)
{
	unsigned int off = off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
	int c;

	for (c = 0; c < n_chan; c++)
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)),
			      saved_ctrl_7[c]);
}

static int adrv9002_axi_rx_test_pattern_pn_sel(const struct adrv9002_rf_phy
		*phy,
		const int chan, const int n_chan)
{
	unsigned int off = chan ? ADI_RX2_REG_OFF : 0;
	adi_adrv9001_SsiTestModeData_e pattern;
	int c;
	enum axi_adc_pn_sel sel;

	pattern = adrv9002_get_test_pattern(phy, chan, true, false);
	if (pattern == ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE)
		sel = AXI_ADC_PN_RAMP_NIBBLE;
	else if (pattern == ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15)
		sel = AXI_ADC_PN15;
	else
		return -EINVAL;

	for (c = 0; c < n_chan; c++)
		/*
		 * We are deliberately overwriting the DATA_SEL bits to DMA. If it's set to loopback
		 * (if some user was playing with near end loopback before updating the profile or
		 * reinitialize the device), TX tuning will fail...
		 */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)),
			      AXI_ADC_ADC_PN_SEL(sel));

	return 0;
}

static void adrv9002_axi_get_channel_range(const struct adrv9002_rf_phy *phy,
		bool tx,
		int *end)
{
	/*
	 * The point here is that we only want to generate and check test patterns for enabled
	 * channels. If in !rx2tx2 we only get here if the channel is enabled so just use
	 * all the @conv channels for the test. In rx2tx2 mode, we will run the test
	 * at the same time for both channels if both are enabled. However, if RX2/TX2 is
	 * disabled we do not want to check for that so that we tweak @end to only go over
	 * the first channel (1 phy channel == 2 hdl channels). RX2/TX2 start at index 2
	 * in the channels array, so we use @tx to get the right one...
	 */
	if (phy->rx2tx2 && !phy->channels[tx + 2]->enabled)
		*end = 2;
	else
		*end = phy->chip->num_channels;
}

int adrv9002_axi_intf_tune(const struct adrv9002_rf_phy *phy, const bool tx,
			   const int chann,
			   uint8_t *clk_delay, uint8_t *data_delay)
{
	int ret, cnt, max_cnt = 0;
	uint8_t field[8][8] = {0};
	uint8_t clk, data;
	uint32_t saved_ctrl_7[4];
	int n_chan;

	adrv9002_axi_get_channel_range(phy, tx, &n_chan);
	if (tx) {
		/* generate test pattern for tx test  */
		ret = adrv9002_axi_tx_test_pattern_set(phy, chann, n_chan, saved_ctrl_7);
		if (ret)
			return ret;
	} else {
		ret = adrv9002_axi_rx_test_pattern_pn_sel(phy, chann, n_chan);
		if (ret)
			return ret;

		/* start test */
		ret = adrv9002_intf_test_cfg(phy, chann, tx, false);
		if (ret)
			return ret;
	}

	for (clk = 0; clk < NO_OS_ARRAY_SIZE(field); clk++) {
		for (data = 0; data < sizeof(*field); data++) {
			ret = adrv9002_intf_change_delay(phy, chann, clk, data, tx);
			if (ret < 0)
				return ret;

			if (tx) {
				if (phy->rx2tx2)
					ret = adrv9002_tx_fixup_all(phy);
				else
					ret = adrv9002_tx_fixup(phy, chann);
				if (ret)
					return ret;
				/*
				 * we need to restart the tx test for every iteration since it's
				 * the only way to reset the counters.
				 */
				ret = adrv9002_intf_test_cfg(phy, chann, tx, false);
				if (ret)
					return ret;
			}
			/* check result */
			if (!tx)
				ret = adrv9002_axi_pn_check(phy, chann, n_chan);
			else
				ret = adrv9002_check_tx_test_pattern(phy, chann);

			field[clk][data] |= ret;
		}
	}

	adrv9002_axi_digital_tune_verbose(phy, field, tx, chann);

	/* stop test */
	ret = adrv9002_intf_test_cfg(phy, chann, tx, true);
	if (ret)
		return ret;

	/* stop tx pattern */
	if (tx)
		adrv9002_axi_tx_test_pattern_restore(phy->rx1_adc, chann, n_chan,
						     saved_ctrl_7);//ToDo rx2?

	for (clk = 0; clk < NO_OS_ARRAY_SIZE(field); clk++) {
		cnt = adrv9002_axi_find_point(&field[clk][0], sizeof(*field), &data);
		if (cnt < 0)
			continue;

		if (cnt > max_cnt) {
			max_cnt = cnt;
			*clk_delay = clk;
			*data_delay = data + max_cnt / 2;
		}
	}

	return max_cnt ? 0 : -EIO;
}

void adrv9002_axi_interface_enable(const struct adrv9002_rf_phy *phy,
				   const int chan, const bool tx,
				   const bool en)
{
	int off = 0;

	if (tx)
		off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
	else
		off = chan ? ADI_RX2_REG_OFF : 0;

	if (en)
		/* bring axi core out of reset */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_RSTN),
			      AXI_ADC_RSTN | AXI_ADC_MMCM_RSTN);
	else
		/* reset axi core*/
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_RSTN), 0);
}

int adrv9002_axi_tx_test_pattern_cfg(struct adrv9002_rf_phy *phy,
				     const int channel,
				     const adi_adrv9001_SsiTestModeData_e data)
{
	int off, start, n_chan, c, sel;

	if (phy->rx2tx2) {
		off = ADI_TX1_REG_OFF;
		start = channel * 2;
		/* I and Q channels */
		n_chan = start + 2;
	} else {
		off = channel ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
		start = 0;
		n_chan = phy->chip->num_channels;
	}

	switch (data) {
	case ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL:
		/* DATA_SEL_DDS */
		sel = 0;
		break;
	case ADI_ADRV9001_SSI_TESTMODE_DATA_FIXED_PATTERN:
		/* DATA_SEL_SED */
		sel = 1;
		break;
	case ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE:
		/* DATA_SEL_RAMP_NIBBLE */
		sel = 10;
		break;
	case ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT:
		/* DATA_SEL_RAMP_16 */
		;
		sel = 11;
		break;
	case ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15:
		/* DATA_SEL_PN15 */
		sel = 7;
		break;
	case ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7:
		/* DATA_SEL_PN7 */
		sel = 6;
		break;
	default:
		return -EINVAL;
	}

	for (c = start; c < n_chan; c++)
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), sel);

	axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_CTRL_1), 1);

	return 0;
}

void adrv9002_axi_hdl_loopback(struct adrv9002_rf_phy *phy, int channel,
			       bool enable)
{
	int off, start, n_chan, c;
	uint32_t reg;

	if (phy->rx2tx2) {
		start = channel * 2;
		/* I and Q channels */
		n_chan = start + 2;
		off = 0;
	} else {
		start = 0;
		n_chan = phy->chip->num_channels;
		off = channel ? ADI_RX2_REG_OFF : 0;
	}

	for (c = start; c < n_chan; c++) {
		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)), &reg);
		reg = (reg & ~AXI_ADC_ADC_DATA_SEL(~0)) | AXI_ADC_ADC_DATA_SEL(enable);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)), reg);
	}
}

uint32_t adrv9002_axi_dds_rate_get(const struct adrv9002_rf_phy *phy,
				   const int chan)
{
	uint32_t reg;
	const int off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;

	/* the rate is decremented by one when configured on the core */
	axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_RATE), &reg);
	return reg + 1;
}
