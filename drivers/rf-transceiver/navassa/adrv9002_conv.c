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
#include "util.h"
#include "delay.h"
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

#define ADI_RX2_REG_OFF			0x1000
#define ADI_TX1_REG_OFF			0x2000
#define ADI_TX2_REG_OFF			0x4000
#define ADI_TX_REG_RATE			0x4c
#define ADI_TX_REG_CTRL_2		0x48
#define ADI_TX_REG_CHAN_CTRL_7(c)	(0x0418 + (c) * 0x40)
#define ADI_TX_REG_CTRL_1		0x44
#define R1_MODE				BIT(2)
#define TX_R1_MODE			BIT(5)

#define AIM_AXI_REG(off, addr)		((off) + (addr))
#define	NUM_LANES_MASK			GENMASK(12, 8)
#define NUM_LANES(x)			field_prep(NUM_LANES_MASK, x)
#define SDR_DDR_MASK			BIT(16)
#define SDR_DDR(x)			field_prep(SDR_DDR_MASK, x)

#define IS_CMOS(cfg)			((cfg) & (ADI_CMOS_OR_LVDS_N))

void adrv9002_axi_interface_enable(struct adrv9002_rf_phy *phy, const int chan,
				   const bool en)
{
	const int rx_off = chan ? ADI_RX2_REG_OFF : 0;
	const int tx_off = chan ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;

	if (en) {
		/* bring axi core out of reset */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(rx_off, AXI_ADC_REG_RSTN),
			      AXI_ADC_RSTN | AXI_ADC_MMCM_RSTN);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(tx_off, AXI_ADC_REG_RSTN),
			      AXI_ADC_RSTN | AXI_ADC_MMCM_RSTN);
	} else {
		/* reset axi core*/
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(rx_off, AXI_ADC_REG_RSTN), 0);
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(tx_off, AXI_ADC_REG_RSTN), 0);
	}
}

int adrv9002_interface_validate(const struct adrv9002_rf_phy *phy,
				struct axi_adc *axi_dev,
				const uint8_t ssi_intf)
{
	uint32_t axi_config = 0;

	axi_adc_read(axi_dev, ADI_REG_CONFIG, &axi_config);
	if (IS_CMOS(axi_config) && ssi_intf == ADI_ADRV9001_SSI_TYPE_LVDS) {
		printf("AXI interface is CMOS and device profile is LVDS...\n");
		return -EINVAL;
	} else if (!IS_CMOS(axi_config) &&
		   ssi_intf == ADI_ADRV9001_SSI_TYPE_CMOS) {
		printf("AXI interface is LVDS and device profile is CMOS...\n");
		return -EINVAL;
	}

	return 0;
}

int adrv9002_interface_config(struct axi_adc *axi_dev, const uint8_t n_lanes,
			      const uint8_t ssi_intf, const bool cmos_ddr,
			      const int channel)
{
	uint32_t reg_ctrl = 0, tx_reg_ctrl;
	uint8_t rate;
	const int rx_off = channel ? ADI_RX2_REG_OFF : 0;
	const int tx_off = channel ? ADI_TX2_REG_OFF : ADI_TX1_REG_OFF;

	axi_adc_read(axi_dev, AIM_AXI_REG(rx_off, AXI_ADC_REG_CNTRL), &reg_ctrl);
	axi_adc_read(axi_dev, AIM_AXI_REG(tx_off, ADI_TX_REG_CTRL_2), &tx_reg_ctrl);

	reg_ctrl &= ~(NUM_LANES_MASK | SDR_DDR_MASK);
	tx_reg_ctrl &= ~(NUM_LANES_MASK | SDR_DDR_MASK);

	switch (n_lanes) {
	case ADI_ADRV9001_SSI_1_LANE:
		reg_ctrl |= NUM_LANES(1);
		tx_reg_ctrl |= NUM_LANES(1);
		if (ssi_intf == ADI_ADRV9001_SSI_TYPE_CMOS) {
			rate = cmos_ddr ? 3 : 7;
			reg_ctrl |= SDR_DDR(!cmos_ddr);
			tx_reg_ctrl |= SDR_DDR(!cmos_ddr);
		} else {
			rate = 3;
		}
		break;
	case ADI_ADRV9001_SSI_2_LANE:
		if (ssi_intf == ADI_ADRV9001_SSI_TYPE_CMOS)
			return -EINVAL;

		reg_ctrl |= NUM_LANES(2);
		tx_reg_ctrl |= NUM_LANES(2);
		rate = 1;
		break;
	case ADI_ADRV9001_SSI_4_LANE:
		if (ssi_intf == ADI_ADRV9001_SSI_TYPE_LVDS)
			return -EINVAL;

		reg_ctrl |= NUM_LANES(4);
		tx_reg_ctrl |= NUM_LANES(4);
		rate = cmos_ddr ? 0 : 1;
		reg_ctrl |= SDR_DDR(!cmos_ddr);
		tx_reg_ctrl |= SDR_DDR(!cmos_ddr);
		break;
	default:
		return -EINVAL;
	}

	axi_adc_write(axi_dev, AIM_AXI_REG(rx_off, AXI_ADC_REG_CNTRL), reg_ctrl);
	axi_adc_write(axi_dev, AIM_AXI_REG(tx_off, ADI_TX_REG_RATE), rate);
	axi_adc_write(axi_dev, AIM_AXI_REG(tx_off, ADI_TX_REG_CTRL_2), tx_reg_ctrl);

	return 0;
}

int adrv9002_axi_interface_set(struct adrv9002_rf_phy *phy,
			       const uint8_t n_lanes,
			       const uint8_t ssi_intf, const bool cmos_ddr,
			       const int channel)
{
	int ret;

	ret = adrv9002_interface_validate(phy, phy->rx1_adc, ssi_intf);
	if (ret)
		return ret;

	return adrv9002_interface_config(phy->rx1_adc, n_lanes, ssi_intf, cmos_ddr,
					 channel);
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

void adrv9002_get_ssi_interface(struct adrv9002_rf_phy *phy, const int chann,
				uint8_t *ssi_intf, uint8_t *n_lanes, bool *cmos_ddr_en)
{
	/*
	 * Using the RX profile since with TX, we can have, for example, TX1 disabled
	 * while RX1 is enabled (the other way around is not permitted). Since this API
	 * only looks to the channel, we would return invalid values in such a case...
	 */
	adi_adrv9001_RxProfile_t *rx_cfg;
	/*
	 * We only look for one port. Although theoretical possible, we are
	 * assuming that ports on the same channel have the same number of lanes
	 * and, obviously, the same interface type
	 */
	rx_cfg = &phy->curr_profile->rx.rxChannelCfg[chann].profile;
	*ssi_intf = rx_cfg->rxSsiConfig.ssiType;
	*n_lanes = rx_cfg->rxSsiConfig.numLaneSel;
	*cmos_ddr_en = rx_cfg->rxSsiConfig.cmosDdrEn;
}

int adrv9002_ssi_configure(struct adrv9002_rf_phy *phy)
{
	bool cmos_ddr;
	uint8_t n_lanes, ssi_intf;
	int c, ret;

	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		/*
		 * Care only about RX because TX cannot be enabled while the RX on the
		 * same channel is disabled. This will also work in rx2tx2 mode since we
		 * only care at channel 1 and RX1 must be enabled. However, TX1 can be
		 * disabled which would lead to problems since we would no configure channel 1...
		 */
		struct adrv9002_chan *chan = &phy->rx_channels[c].channel;

		if (!chan->enabled)
			continue;

		/* the SSI settings should be done with the core in reset */
		adrv9002_axi_interface_enable(phy, c, false);
		if (phy->rx2tx2)
			adrv9002_sync_gpio_toogle(phy);

		adrv9002_get_ssi_interface(phy, c, &ssi_intf, &n_lanes, &cmos_ddr);
		ret = adrv9002_axi_interface_set(phy, n_lanes, ssi_intf, cmos_ddr, c);
		if (ret)
			return ret;

		adrv9002_axi_interface_enable(phy, c, true);

		if (phy->rx2tx2)
			break;
	}

	return 0;
}

static void adrv9002_axi_tx_test_pattern_set(struct axi_adc *axi_dev,
		const int off,
		uint32_t *ctrl_7, const adi_adrv9001_SsiType_e ssi_type)
{
	int n_chan = axi_dev->num_channels;
	int c, sel;

	if (ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		/* RAMP nibble */
		sel = 10;
	else
		/* pn15 */
		sel = 7;

	for (c = 0; c < n_chan; c++) {
		axi_adc_read(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), &ctrl_7[c]);
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)), sel);
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CTRL_1), 1);
	}
}

static void adrv9002_axi_tx_test_pattern_restore(struct axi_adc *axi_dev,
		const int off,
		const uint32_t *saved_ctrl_7)
{
	int n_chan = axi_dev->num_channels;
	int c;

	for (c = 0; c < n_chan; c++)
		axi_adc_write(axi_dev, AIM_AXI_REG(off, ADI_TX_REG_CHAN_CTRL_7(c)),
			      saved_ctrl_7[c]);
}

int adrv9002_check_tx_test_pattern(struct adrv9002_rf_phy *phy, const int chann,
				   const adi_adrv9001_SsiType_e ssi_type)
{
	int ret;
	struct adrv9002_chan *chan = &phy->tx_channels[chann].channel;
	adi_adrv9001_SsiTestModeData_e test_data = ssi_type ==
			ADI_ADRV9001_SSI_TYPE_CMOS ?
			ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE :
			ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15;
	struct adi_adrv9001_TxSsiTestModeCfg cfg = {0};
	struct adi_adrv9001_TxSsiTestModeStatus status = {0};

	cfg.testData = test_data;

	ret = adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect(phy->adrv9001, chan->number,
			ssi_type,
			ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
			&cfg, &status);
	if (ret)
		return adrv9002_dev_err(phy);

#ifdef DEBUG
	printf("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u", chan->number, status.dataError,
	       status.fifoFull, status.fifoEmpty, status.strobeAlignError);
#endif

	/* only looking for data errors for now */
	if (status.dataError)
		return 1;

	if (!phy->rx2tx2 || chann)
		return 0;

	chan = &phy->tx_channels[chann + 1].channel;
	if (!chan->enabled)
		return 0;

	memset(&status, 0, sizeof(status));
	ret = adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect(phy->adrv9001, chan->number,
			ssi_type,
			ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
			&cfg, &status);
	if (ret)
		return adrv9002_dev_err(phy);

#ifdef DEBUG
	printf("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u", chan->number,
	       status.dataError, status.fifoFull, status.fifoEmpty, status.strobeAlignError);
#endif

	if (status.dataError)
		return 1;

	return 0;
}

static void adrv9002_axi_rx_test_pattern_pn_sel(struct axi_adc *axi_dev,
		const int off,
		const adi_adrv9001_SsiType_e ssi_type)
{
	int n_chan = axi_dev->num_channels;
	int c;
	uint32_t reg;
	enum axi_adc_pn_sel sel;

	if (ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		sel = AXI_ADC_PN_RAMP_NIBBLE;
	else
		sel = AXI_ADC_PN15;

	for (c = 0; c < n_chan; c++) {
		axi_adc_read(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)), &reg);
		reg = (reg & ~AXI_ADC_ADC_PN_SEL(~0)) | AXI_ADC_ADC_PN_SEL(sel);
		axi_adc_write(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL_3(c)), reg);
	}
}

static int adrv9002_axi_pn_check(struct adrv9002_rf_phy *phy,
				 struct axi_adc *axi_dev, const int off)
{
	int n_chan = axi_dev->num_channels, chan;
	uint32_t reg;

	/* reset result */
	for (chan = 0; chan < n_chan; chan++)
		axi_adc_write(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(chan)),
			      AXI_ADC_PN_ERR | AXI_ADC_PN_OOS);

	udelay(5000);

	/* check for errors in any channel */
	for (chan = 0; chan < n_chan; chan++) {
		/*
		 * Dont search for errors on disabled ports. We won't have any
		 * test signal in that case, so the core will treat it as an error
		 */
		if (!phy->rx_channels[chan / 2].channel.enabled) {
#ifdef DEBUG
			printf("Ignore pn error in c:%d\n", chan);
#endif
			continue;
		}

		axi_adc_read(axi_dev, AIM_AXI_REG(off, AXI_ADC_REG_CHAN_STATUS(chan)), &reg);
		if (reg) {
#ifdef DEBUG
			printf("pn error in c:%d\n", chan);
#endif
			return 1;
		}
	}

	return 0;
}

int adrv9002_intf_change_delay(struct adrv9002_rf_phy *phy, const int channel,
			       uint8_t clk_delay,
			       uint8_t data_delay, const bool tx, const adi_adrv9001_SsiType_e ssi_type)
{
	struct adi_adrv9001_SsiCalibrationCfg delays = {0};
	int ret;

#ifdef DEBUG
	printf("Set intf delay clk:%u, d:%u, tx:%d c:%d\n", clk_delay,
	       data_delay, tx, channel);
#endif

	if (tx) {
		delays.txClkDelay[channel] = clk_delay;
		delays.txIDataDelay[channel] = data_delay;
		delays.txQDataDelay[channel] = data_delay;
		delays.txStrobeDelay[channel] = data_delay;
		if (phy->rx2tx2 && !channel) {
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

	ret = adi_adrv9001_Ssi_Delay_Configure(phy->adrv9001, ssi_type, &delays);
	if (ret)
		return adrv9002_dev_err(phy);

	return 0;
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

int adrv9002_intf_test_cfg(struct adrv9002_rf_phy *phy, const int chann,
			   const bool tx,
			   const bool stop, const adi_adrv9001_SsiType_e ssi_type)
{
	int ret;
	struct adrv9002_chan *chan;
	adi_adrv9001_SsiTestModeData_e test_data = ssi_type ==
			ADI_ADRV9001_SSI_TYPE_CMOS ?
			ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE :
			ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15;

#ifdef DEBUG
	printf("cfg test stop:%u, ssi:%d, c:%d, tx:%d\n", stop, ssi_type, chann,
	       tx);
#endif

	if (tx) {
		struct adi_adrv9001_TxSsiTestModeCfg cfg = {0};
		chan = &phy->tx_channels[chann].channel;

		cfg.testData = stop ? ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL : test_data;
		ret = adi_adrv9001_Ssi_Tx_TestMode_Configure(phy->adrv9001, chan->number,
				ssi_type,
				ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
				&cfg);
		if (ret)
			return adrv9002_dev_err(phy);

		if (!phy->rx2tx2 || chann)
			return 0;

		chan = &phy->tx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = adi_adrv9001_Ssi_Tx_TestMode_Configure(phy->adrv9001, chan->number,
				ssi_type,
				ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
				&cfg);
		if (ret)
			return adrv9002_dev_err(phy);

	} else {
		struct adi_adrv9001_RxSsiTestModeCfg cfg = {0};
		chan = &phy->rx_channels[chann].channel;

		cfg.testData = stop ? ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL : test_data;
		ret = adi_adrv9001_Ssi_Rx_TestMode_Configure(phy->adrv9001, chan->number,
				ssi_type,
				ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
				&cfg);
		if (ret)
			return adrv9002_dev_err(phy);

		if (!phy->rx2tx2)
			return 0;

		/* on rx2tx2 RX1 must be enabled so we are fine in assuming chan=0 at this point */
		chan = &phy->rx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = adi_adrv9001_Ssi_Rx_TestMode_Configure(phy->adrv9001, chan->number,
				ssi_type,
				ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
				&cfg);
		if (ret)
			return adrv9002_dev_err(phy);
	}

	return 0;
}

int adrv9002_axi_intf_tune(struct adrv9002_rf_phy *phy, const bool tx,
			   const int chann,
			   const adi_adrv9001_SsiType_e ssi_type, uint8_t *clk_delay, uint8_t *data_delay)
{
	int ret, cnt, max_cnt = 0, off;
	uint8_t field[8][8] = {0};
	uint8_t clk, data;
	uint32_t saved_ctrl_7[4];

	if (tx) {
		if (phy->rx2tx2 || !chann)
			off = ADI_TX1_REG_OFF;
		else
			off = ADI_TX2_REG_OFF;

		/* generate test pattern for tx test  */
		adrv9002_axi_tx_test_pattern_set(phy->rx1_adc, off, saved_ctrl_7, ssi_type);
	} else {
		if (phy->rx2tx2 || !chann)
			off = 0;
		else
			off = ADI_RX2_REG_OFF;

		adrv9002_axi_rx_test_pattern_pn_sel(phy->rx1_adc, off, ssi_type);

		/* start test */
		ret = adrv9002_intf_test_cfg(phy, chann, tx, false, ssi_type);
		if (ret)
			return ret;
	}

	for (clk = 0; clk < ARRAY_SIZE(field); clk++) {
		for (data = 0; data < sizeof(*field); data++) {
			ret = adrv9002_intf_change_delay(phy, chann, clk, data, tx, ssi_type);
			if (ret < 0)
				return ret;

			if (tx) {
				/*
				 * we need to restart the tx test for every iteration since it's
				 * the only way to reset the counters.
				 */
				ret = adrv9002_intf_test_cfg(phy, chann, tx, false, ssi_type);
				if (ret)
					return ret;
			}
			/* check result */
			if (!tx)
				ret = adrv9002_axi_pn_check(phy, phy->rx1_adc, off);
			else
				ret = adrv9002_check_tx_test_pattern(phy, chann, ssi_type);

			field[clk][data] |= ret;

			if (tx) {
				ret = adrv9002_intf_test_cfg(phy, chann, tx, true, ssi_type);
				if (ret)
					return ret;
			}
		}
	}

	if (!tx) {
		/* stop test */
		ret = adrv9002_intf_test_cfg(phy, chann, tx, true, ssi_type);
		if (ret)
			return ret;
	} else {
		/* stop tx pattern */
		adrv9002_axi_tx_test_pattern_restore(phy->rx1_adc, off, saved_ctrl_7);
	}

	for (clk = 0; clk < ARRAY_SIZE(field); clk++) {
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

int adrv9002_intf_tuning(struct adrv9002_rf_phy *phy)
{
	struct adi_adrv9001_SsiCalibrationCfg delays = {0};
	int ret;
	uint8_t clk_delay, data_delay;
	struct adrv9002_chan *chan;
	adi_adrv9001_SsiType_e ssi_type = adrv9002_axi_ssi_type_get(phy);
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(phy->rx_channels); i++) {
		chan = &phy->rx_channels[i].channel;

		if (!chan->enabled)
			continue;

		ret = adrv9002_axi_intf_tune(phy, false, i, ssi_type, &clk_delay, &data_delay);
		if (ret)
			return ret;

#ifdef DEBUG
		printf("RX: Got clk: %u, data: %u\n", clk_delay, data_delay);
#endif
		delays.rxClkDelay[i] = clk_delay;
		delays.rxIDataDelay[i] = data_delay;
		delays.rxQDataDelay[i] = data_delay;
		delays.rxStrobeDelay[i] = data_delay;
		/*
		 * In rx2tx2 we should treat RX1/RX2 as the same. Hence, we will run
		 * the test simultaneosly for both ports and configure the same delays.
		 * Moreover rx2 cannot be enabled while rx1 is disabled...
		 */
		if (phy->rx2tx2) {
			/* set RX2 delays */
			delays.rxClkDelay[i + 1] = clk_delay;
			delays.rxIDataDelay[i + 1] = data_delay;
			delays.rxQDataDelay[i + 1] = data_delay;
			delays.rxStrobeDelay[i + 1] = data_delay;
			break;
		}
	}

	for (i = 0; i < ARRAY_SIZE(phy->tx_channels); i++) {
		chan = &phy->tx_channels[i].channel;

		if (!chan->enabled)
			continue;

		ret = adrv9002_axi_intf_tune(phy, true, i, ssi_type, &clk_delay, &data_delay);
		if (ret)
			return ret;
#ifdef DEBUG
		printf("TX: Got clk: %u, data: %u\n", clk_delay, data_delay);
#endif
		delays.txClkDelay[i] = clk_delay;
		delays.txIDataDelay[i] = data_delay;
		delays.txQDataDelay[i] = data_delay;
		delays.txStrobeDelay[i] = data_delay;
		/*
		 * In rx2tx2 we should treat TX1/TX2 as the same. Hence, we will run
		 * the test simultaneosly for both ports and configure the same delays.
		 */
		if (phy->rx2tx2) {
			if (!i) {
				/* set TX2 delays */
				delays.txClkDelay[i + 1] = clk_delay;
				delays.txIDataDelay[i + 1] = data_delay;
				delays.txQDataDelay[i + 1] = data_delay;
				delays.txStrobeDelay[i + 1] = data_delay;
			}
			break;
		}
	}

	ret = adi_adrv9001_Ssi_Delay_Configure(phy->adrv9001, ssi_type, &delays);
	if (ret)
		return adrv9002_dev_err(phy);

	return 0;
}

int adrv9002_post_setup(struct adrv9002_rf_phy *phy)
{
	int i, ret;

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

	/* start interface tuning */
	return adrv9002_intf_tuning(phy);
}
