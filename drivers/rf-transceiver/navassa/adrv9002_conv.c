/***************************************************************************//**
 *   @file   adrv9002_conv.c
 *   @brief  adrv9002 digital interface tuning utilities.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

/* TODO: move this register in axi_adc_core.h */
#define ADI_REG_CONFIG 			0x000C
#define ADI_IQCORRECTION_DISABLE	(1 << 0)
#define ADI_DCFILTER_DISABLE		(1 << 1)
#define ADI_DATAFORMAT_DISABLE		(1 << 2)
#define ADI_USERPORTS_DISABLE		(1 << 3)
#define ADI_MODE_1R1T			(1 << 4)
#define ADI_DELAY_CONTROL_DISABLE 	(1 << 5)
#define ADI_CMOS_OR_LVDS_N		(1 << 7)
#define ADI_PPS_RECEIVER_ENABLE		(1 << 8)
#define ADI_SCALECORRECTION_ONLY	(1 << 9)

#define ADI_REG_RSTN			0x0040
#define ADI_RSTN				(1 << 0)
#define ADI_MMCM_RSTN 			(1 << 1)

#define ADI_REG_CNTRL			0x0044
#define ADI_R1_MODE			(1 << 2)
#define ADI_DDR_EDGESEL			(1 << 1)
#define ADI_PIN_MODE			(1 << 0)

#define ADI_REG_CLK_RATIO		0x0058
#define ADI_CLK_RATIO(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_CLK_RATIO(x)		(((x) >> 0) & 0xFFFFFFFF)

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
#define TX_ONLY_MASK			NO_OS_BIT(10)
#define TX_ONLY(x)			FIELD_GET(TX_ONLY_MASK, x)

#define IS_CMOS(cfg)			((cfg) & (ADI_CMOS_OR_LVDS_N))

void adrv9002_axi_interface_enable(struct adrv9002_rf_phy *phy, const int chan,
				   const bool tx, const bool en)
{
	int off = 0;

	if (tx)
		off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
	else
		off = chan ? ADI_RX2_REG_OFF : 0;

	if (en)
		/* bring axi core out of reset */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_REG_RSTN),
			      ADI_RSTN | ADI_MMCM_RSTN);
	else
		/* reset axi core*/
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_REG_RSTN), 0);
}

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
		reg_ctrl = ADI_REG_CNTRL;
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

		axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, ADI_REG_CLK_RATIO), &divider);
		/* in LVDS, data type is always DDR */
		if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS)
			ddr = 1;
		rate = 32 / ((1 << n_lanes) * (1 + ddr) * divider) - 1;
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_RATE), rate);
	}

	return 0;
}

adi_adrv9001_SsiType_e adrv9002_axi_ssi_type_get(struct adrv9002_rf_phy *phy)
{
	uint32_t axi_config = 0;

	axi_adc_read(phy->rx1_adc, ADI_REG_CONFIG, &axi_config);
	if (IS_CMOS(axi_config))
		return ADI_ADRV9001_SSI_TYPE_CMOS;
	else
		return ADI_ADRV9001_SSI_TYPE_LVDS;
}

static void adrv9002_get_ssi_interface(const struct adrv9002_rf_phy *phy,
				       const int chann,
				       const bool tx, uint8_t *n_lanes, bool *cmos_ddr_en)
{
	if (tx) {
		adi_adrv9001_TxProfile_t *tx_cfg;

		tx_cfg = &phy->curr_profile->tx.txProfile[chann];
		*n_lanes = tx_cfg->txSsiConfig.numLaneSel;
		*cmos_ddr_en = tx_cfg->txSsiConfig.ddrEn;
	} else {
		adi_adrv9001_RxProfile_t *rx_cfg;

		rx_cfg = &phy->curr_profile->rx.rxChannelCfg[chann].profile;
		*n_lanes = rx_cfg->rxSsiConfig.numLaneSel;
		*cmos_ddr_en = rx_cfg->rxSsiConfig.ddrEn;
	}
}

static int adrv9002_ssi_configure(const struct adrv9002_rf_phy *phy)
{
	bool cmos_ddr;
	uint8_t n_lanes;
	int ret;
	unsigned int c;

	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		const struct adrv9002_chan *chann = phy->channels[c];

		/* RX2/TX2 can only be enabled if RX1/TX1 are also enabled */
		if (phy->rx2tx2 && chann->idx > ADRV9002_CHANN_1)
			break;

		if (!chann->enabled)
			continue;

		adrv9002_sync_gpio_toogle(phy);

		adrv9002_get_ssi_interface(phy, chann->idx, chann->port == ADI_TX, &n_lanes,
					   &cmos_ddr);
		ret = adrv9002_axi_interface_set(phy, n_lanes, cmos_ddr, chann->idx,
						 chann->port == ADI_TX);
		if (ret)
			return ret;

		/*
		 * We should set the tdd rate on TX's iterations since only at this point we
		 * have the up to date dds rate. Moreover it does not make sense to do any
		 * tdd configuration if both TX/RX on the same channel are not enabled.
		 */
		if (chann->port == ADI_TX) {
			const struct adrv9002_rx_chan *rx = &phy->rx_channels[chann->idx];
			unsigned long rate;

			if (!rx->channel.enabled)
				continue;

			rate = adrv9002_axi_dds_rate_get(phy, chann->idx) * rx->channel.rate;
			//ToDo Set rate for Tx only profiles
		}
	}

	return 0;
}

static void adrv9002_axi_tx_test_pattern_set(const struct adrv9002_rf_phy *phy,
		struct axi_adc *axi_dev,
		const int off,
		const int n_chan,
		uint32_t *ctrl_7)
{
	int c, sel;

	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		/* RAMP nibble */
		sel = 10;
	else
		/* pn7 */
		sel = 6;

	for (c = 0; c < n_chan; c++) {
		axi_adc_read(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), &ctrl_7[c]);
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), sel);
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CTRL_1), 1);
	}
}

static void adrv9002_axi_tx_test_pattern_restore(struct axi_adc *axi_dev,
		const int off,
		const int n_chan,
		const uint32_t *saved_ctrl_7)
{
	int c;

	for (c = 0; c < n_chan; c++)
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)),
			      saved_ctrl_7[c]);
}

int adrv9002_check_tx_test_pattern(const struct adrv9002_rf_phy *phy,
				   const int chann)
{
	int ret;
	const struct adrv9002_chan *chan = &phy->tx_channels[chann].channel;
	adi_adrv9001_SsiTestModeData_e test_data = phy->ssi_type ==
			ADI_ADRV9001_SSI_TYPE_CMOS ?
			ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE :
			ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7;
	struct adi_adrv9001_TxSsiTestModeCfg cfg = {0};
	struct adi_adrv9001_TxSsiTestModeStatus status = {0};
	adi_adrv9001_SsiDataFormat_e data_fmt = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA;

	cfg.testData = test_data;

	ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect,
		       chan->number, phy->ssi_type, data_fmt, &cfg, &status);
	if (ret)
		return ret;

	pr_debug("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u\n", chan->number,
		 status.dataError,
		 status.fifoFull, status.fifoEmpty, status.strobeAlignError);

	/* only looking for data errors for now */
	if (status.dataError)
		return 1;

	if (!phy->rx2tx2 || chann)
		return 0;

	chan = &phy->tx_channels[chann + 1].channel;
	if (!chan->enabled)
		return 0;

	memset(&status, 0, sizeof(status));
	ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect,
		       chan->number, phy->ssi_type, data_fmt, &cfg, &status);
	if (ret)
		return ret;

	pr_debug("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u\n", chan->number,
		 status.dataError, status.fifoFull, status.fifoEmpty, status.strobeAlignError);

	if (status.dataError)
		return 1;

	return 0;
}

static void adrv9002_axi_rx_test_pattern_pn_sel(const struct adrv9002_rf_phy
		*phy,
		struct axi_adc *axi_dev,
		const int off,
		const int n_chan)
{
	int c;
	enum axi_adc_pn_sel sel;

	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		sel = AXI_ADC_PN_RAMP_NIBBLE;
	else
		sel = AXI_ADC_PN15;

	for (c = 0; c < n_chan; c++)
		/*
		 * We are deliberately overwriting the DATA_SEL bits to DMA. If it's set to loopback
		 * (if some user was playing with near end loopback before updating the profile or
		 * reinitialize the device), TX tuning will fail...
		 */
		axi_adc_write(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)),
			      AXI_ADC_ADC_PN_SEL(sel));
}

static int adrv9002_axi_pn_check(const struct adrv9002_rf_phy *phy,
				 struct axi_adc *axi_dev, const int off)
{
	int n_chan = axi_dev->num_channels, chan;
	uint32_t reg;

	/* reset result */
	for (chan = 0; chan < n_chan; chan++)
		axi_adc_write(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(chan)),
			      AXI_ADC_PN_ERR | AXI_ADC_PN_OOS);

	no_os_udelay(5000);

	/* check for errors in any channel */
	for (chan = 0; chan < n_chan; chan++) {
		axi_adc_read(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(chan)), &reg);
		if (reg) {
			pr_debug("pn error in c:%d, reg: %02X\n", chan, reg);
			return 1;
		}
	}

	return 0;
}

int adrv9002_intf_change_delay(const struct adrv9002_rf_phy *phy,
			       const int channel,
			       uint8_t clk_delay,
			       uint8_t data_delay, const bool tx)
{
	struct adi_adrv9001_SsiCalibrationCfg delays = {0};

	pr_debug("Set intf delay clk:%u, d:%u, tx:%d c:%d\n", clk_delay,
		 data_delay, tx, channel);

	if (tx) {
		delays.txClkDelay[channel] = clk_delay;
		delays.txIDataDelay[channel] = data_delay;
		delays.txQDataDelay[channel] = data_delay;
		delays.txStrobeDelay[channel] = data_delay;
		if (phy->rx2tx2) {
			delays.txClkDelay[channel + 1] = clk_delay;
			delays.txIDataDelay[channel + 1] = data_delay;
			delays.txQDataDelay[channel + 1] = data_delay;
			delays.txStrobeDelay[channel + 1] = data_delay;
		}
	} else {
		delays.rxClkDelay[channel] = clk_delay;
		delays.rxIDataDelay[channel] = data_delay;
		delays.rxQDataDelay[channel] = data_delay;
		delays.rxStrobeDelay[channel] = data_delay;
		if (phy->rx2tx2) {
			delays.rxClkDelay[channel + 1] = clk_delay;
			delays.rxIDataDelay[channel + 1] = data_delay;
			delays.rxQDataDelay[channel + 1] = data_delay;
			delays.rxStrobeDelay[channel + 1] = data_delay;
		}
	}

	return api_call(phy, adi_adrv9001_Ssi_Delay_Configure, phy->ssi_type, &delays);
}

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

int adrv9002_intf_test_cfg(const struct adrv9002_rf_phy *phy, const int chann,
			   const bool tx,
			   const bool stop)
{
	int ret;
	const struct adrv9002_chan *chan;
	adi_adrv9001_SsiDataFormat_e data_fmt = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA;

	pr_debug("cfg test stop:%u, ssi:%d, c:%d, tx:%d\n", stop,
		 phy->ssi_type, chann, tx);

	if (tx) {
		struct adi_adrv9001_TxSsiTestModeCfg cfg = {0};
		chan = &phy->tx_channels[chann].channel;

		if (stop)
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL;
		else if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS)
			/*
			 * Some low rate profiles don't play well with prbs15. The reason is
			 * still unclear. We suspect that the chip error checker might have
			 * some time constrains and cannot reliable validate prbs15 full
			 * sequences in the test time. Using a shorter sequence fixes the
			 * problem...
			 */
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7;
		else
			/* CMOS */
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE;

		ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Configure, chan->number,
			       phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;

		if (!phy->rx2tx2)
			return 0;

		/* on rx2tx2 we will only get here on index 0 so the following is fine */
		chan = &phy->tx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Configure, chan->number,
			       phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;

	} else {
		struct adi_adrv9001_RxSsiTestModeCfg cfg = {0};
		chan = &phy->rx_channels[chann].channel;

		if (stop)
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL;
		else if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS)
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15;
		else
			/* CMOS */
			cfg.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE;

		ret = api_call(phy, adi_adrv9001_Ssi_Rx_TestMode_Configure, chan->number,
			       phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;

		if (!phy->rx2tx2)
			return 0;

		/* on rx2tx2 we will only get here on index 0 so the following is fine */
		chan = &phy->rx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = api_call(phy, adi_adrv9001_Ssi_Rx_TestMode_Configure, chan->number,
			       phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;
	}

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
	else {
#ifdef ADRV9002_RX2TX2
		*end = 4;
#else
		*end = 2;
#endif
	}
}

static
void adrv9002_axi_digital_tune_verbose(const struct adrv9002_rf_phy *phy,
				       uint8_t field[][8],
				       const bool tx,
				       const int channel)
{
	int i, j;
	char c;

	pr_debug("tuning: %s%d\n",
		 tx ? "TX" : "RX",
		 channel ? 2 : 1);
	pr_debug("  ");
	for (i = 0; i < 8; i++)
		pr_debug("%x%s", i, i == 7 ? "" : ":");
	pr_debug("\n");

	for (i = 0; i < 8; i++) {
		pr_debug("%x:", i);
		for (j = 0; j < 8; j++) {
			if (field[i][j])
				c = '#';
			else
				c = 'o';
			pr_debug("%c ", c);
		}
		pr_debug("\n");
	}
}

int adrv9002_axi_intf_tune(const struct adrv9002_rf_phy *phy, const bool tx,
			   const int chann,
			   uint8_t *clk_delay, uint8_t *data_delay)
{
	int ret, cnt, max_cnt = 0, off;
	uint8_t field[8][8] = {0};
	uint8_t clk, data;
	uint32_t saved_ctrl_7[4];
	int n_chan;

	adrv9002_axi_get_channel_range(phy, tx, &n_chan);
	if (tx) {
		off = chann ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;
		/* generate test pattern for tx test  */
		adrv9002_axi_tx_test_pattern_set(phy, phy->rx1_adc, off, n_chan, saved_ctrl_7);
	} else {
		off = chann ? ADI_RX2_REG_OFF : 0;
		adrv9002_axi_rx_test_pattern_pn_sel(phy, phy->rx1_adc, off, n_chan);
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
				ret = adrv9002_axi_pn_check(phy, phy->rx1_adc, off);
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
		adrv9002_axi_tx_test_pattern_restore(phy->rx1_adc, off, n_chan, saved_ctrl_7);

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

	//return 0;
	return max_cnt ? 0 : -EIO;
}

static int adrv9002_intf_tuning(const struct adrv9002_rf_phy *phy)
{
	struct adi_adrv9001_SsiCalibrationCfg delays = {0};
	int ret;
	uint8_t clk_delay, data_delay;
	int i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->channels); i++) {
		struct adrv9002_chan *c = phy->channels[i];

		if (!c->enabled)
			continue;
		if (phy->rx2tx2 && c->idx) {
			/*
			 * In rx2tx2 we should treat both channels as the same. Hence, we will run
			 * the test simultaneosly for both and configure the same delays.
			 */
			if (c->port == ADI_RX) {
				/* RX0 must be enabled, hence we can safely skip further tuning */
				delays.rxClkDelay[c->idx] = delays.rxClkDelay[0];
				delays.rxIDataDelay[c->idx] = delays.rxIDataDelay[0];
				delays.rxQDataDelay[c->idx] = delays.rxQDataDelay[0];
				delays.rxStrobeDelay[c->idx] = delays.rxStrobeDelay[0];
			} else {
				/* TX0 must be enabled, hence we can safely skip further tuning */
				delays.txClkDelay[c->idx] = delays.txClkDelay[0];
				delays.txIDataDelay[c->idx] = delays.txIDataDelay[0];
				delays.txQDataDelay[c->idx] = delays.txQDataDelay[0];
				delays.txStrobeDelay[c->idx] = delays.txStrobeDelay[0];
			}

			continue;
		}

		ret = adrv9002_axi_intf_tune(phy, c->port == ADI_TX, c->idx, &clk_delay,
					     &data_delay);
		if (ret)
			return ret;

		if (c->port == ADI_RX) {
			pr_debug("RX: Got clk: %u, data: %u\n", clk_delay, data_delay);
			delays.rxClkDelay[c->idx] = clk_delay;
			delays.rxIDataDelay[c->idx] = data_delay;
			delays.rxQDataDelay[c->idx] = data_delay;
			delays.rxStrobeDelay[c->idx] = data_delay;
		} else {
			pr_debug("TX: Got clk: %u, data: %u\n", clk_delay, data_delay);
			delays.txClkDelay[c->idx] = clk_delay;
			delays.txIDataDelay[c->idx] = data_delay;
			delays.txQDataDelay[c->idx] = data_delay;
			delays.txStrobeDelay[c->idx] = data_delay;
		}
	}

	return api_call(phy, adi_adrv9001_Ssi_Delay_Configure, phy->ssi_type, &delays);
}

static const char *const lo_maps[] = {
	"Unknown",
	"L01",
	"L02",
	"AUX LO"
};

static const char *const duplex[] = {
	"TDD",
	"FDD"
};

static const char *const ssi[] = {
	"Disabled",
	"CMOS",
	"LVDS"
};

static const char *const mcs[] = {
	"Disabled",
	"Enabled",
	"Enabled RFPLL Phase"
};

static const char *const rx_gain_type[] = {
	"Correction",
	"Compensated"
};

static void adrv9002_fill_profile_read(struct adrv9002_rf_phy *phy)
{
	struct adi_adrv9001_DeviceSysConfig *sys = &phy->curr_profile->sysConfig;
	struct adi_adrv9001_ClockSettings *clks = &phy->curr_profile->clocks;
	struct adi_adrv9001_RxSettings *rx = &phy->curr_profile->rx;
	struct adi_adrv9001_RxChannelCfg *rx_cfg = rx->rxChannelCfg;
	struct adi_adrv9001_TxSettings *tx = &phy->curr_profile->tx;

	phy->profile_len = snprintf(phy->profile_buf, sizeof(phy->profile_buf),
				    "Device clk(Hz): %d\n"
				    "Clk PLL VCO(Hz): %lld\n"
				    "ARM Power Saving Clk Divider: %d\n"
				    "RX1 LO: %s\n"
				    "RX2 LO: %s\n"
				    "TX1 LO: %s\n"
				    "TX2 LO: %s\n"
				    "RX1 Gain Table Type: %s\n"
				    "RX2 Gain Table Type: %s\n"
				    "RX Channel Mask: 0x%x\n"
				    "TX Channel Mask: 0x%x\n"
				    "Duplex Mode: %s\n"
				    "FH enable: %d\n"
				    "MCS mode: %s\n"
				    "SSI interface: %s\n", clks->deviceClock_kHz * 1000,
				    clks->clkPllVcoFreq_daHz * 10ULL, clks->armPowerSavingClkDiv,
				    lo_maps[clks->rx1LoSelect], lo_maps[clks->rx2LoSelect],
				    lo_maps[clks->tx1LoSelect], lo_maps[clks->tx2LoSelect],
				    rx_gain_type[rx_cfg[ADRV9002_CHANN_1].profile.gainTableType],
				    rx_gain_type[rx_cfg[ADRV9002_CHANN_2].profile.gainTableType],
				    rx->rxInitChannelMask, tx->txInitChannelMask,
				    duplex[sys->duplexMode], sys->fhModeOn, mcs[sys->mcsMode],
				    ssi[phy->ssi_type]);
}

int adrv9002_post_setup(struct adrv9002_rf_phy *phy)
{
	int i, ret;
	unsigned int c;
	struct adrv9002_chan *chan;

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

	for (i = 0; i < ADRV9002_CHANN_MAX; i++) {
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL_1(i),
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL_2(i),
			      (i & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CHAN_CNTRL(i),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}

	ret = adrv9002_ssi_configure(phy);
	if (ret)
		return ret;

	/* re-enable the cores */
	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		chan = phy->channels[c];

		if (phy->rx2tx2 && chan->idx > ADRV9002_CHANN_1)
			break;

		if (!chan->enabled)
			continue;

		adrv9002_axi_interface_enable(phy, chan->idx, chan->port == ADI_TX, true);
	}

	/* start interface tuning */
	ret = adrv9002_intf_tuning(phy);
	if (ret)
		return ret;

	adrv9002_fill_profile_read(phy);

	return 0;
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
