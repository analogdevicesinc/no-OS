/***************************************************************************//**
 *   @file   adrv910x_conv.c
 *   @brief  adrv910x AXI cores (axi_adrv9001) interface configuration.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#include "axi_adc_core.h"
#include "adrv910x.h"

/*
 * The RX1, RX2 and TX cores are register windows of the same axi_adrv9001 IP,
 * so, as in adrv9002_conv.c, all of them are reached through phy->rx1_adc.
 */
#define ADI_RX2_REG_OFF			0x1000
#define ADI_TX1_REG_OFF			0x2000
#define ADI_TX_REG_RATE			0x4c
#define ADI_TX_REG_CTRL_2		0x48
#define R1_MODE				NO_OS_BIT(2)
#define TX_R1_MODE			NO_OS_BIT(5)

#define AIM_AXI_REG(off, addr)		((off) + (addr))
#define NUM_LANES_MASK			NO_OS_GENMASK(12, 8)
#define NUM_LANES(x)			no_os_field_prep(NUM_LANES_MASK, x)
/* 1 selects SDR, 0 selects DDR. Only used in CMOS, LVDS is always DDR. */
#define SDR_DDR_MASK			NO_OS_BIT(16)
#define SDR_DDR(x)			no_os_field_prep(SDR_DDR_MASK, x)
#define IS_CMOS(cfg)			((cfg) & (AXI_ADC_CMOS_OR_LVDS_N))
#define TX_REF_CLK_MASK			NO_OS_GENMASK(11, 10)
#define TX_REF_CLK(x)			no_os_field_get(TX_REF_CLK_MASK, x)
#define TX_FLEX_REF_CLK_MASK		NO_OS_BIT(14)
#define TX_FLEX_REF_CLK(x)		no_os_field_get(TX_FLEX_REF_CLK_MASK, x)

static const char * const adrv910x_tx_clk[] = {
	"OWN REF", "RX1 REF", "RX2 REF"
};

/*
 * Same check as in adrv9002_post_setup()/adrv9002_validate_profile(): find out
 * which clock drives the TX SSI in the HDL. When it is the SSI clock of an RX
 * port, TX only works if both interfaces run at the same rate.
 */
static int adrv910x_axi_tx_ref_clk_check(const struct adrv910x_rf_phy *phy)
{
	const deviceProfile_t *profile = &phy->device_profile->profilePS1;
	const struct adrv910x_chan *tx = &phy->tx_channel.channel;
	uint32_t axi_config = 0, ref_clk, tx_rate, rx_rate;

	axi_adc_read(phy->rx1_adc,
		     AIM_AXI_REG(ADI_TX1_REG_OFF, AXI_ADC_REG_CONFIG),
		     &axi_config);

	if (TX_FLEX_REF_CLK(axi_config))
		ref_clk = TX_REF_CLK(axi_config);
	else
		/* only bit 10 matters in legacy designs */
		ref_clk = TX_REF_CLK(axi_config) & NO_OS_BIT(0);

	if (ref_clk >= NO_OS_ARRAY_SIZE(adrv910x_tx_clk))
		return -EINVAL;

	pr_info("TX SSI clk driven by %s (TX core config 0x%08lx)\n",
		adrv910x_tx_clk[ref_clk], (unsigned long)axi_config);

	if (!ref_clk)
		return 0;

	tx_rate = profile->txConfig[tx->idx].txInterfaceSampleRate;
	rx_rate = profile->rxConfig[ref_clk - 1].rxInterfaceSampleRate;

	if (!phy->rx_channels[ref_clk - 1].channel.enabled) {
		pr_err("TX SSI clk is driven by RX%lu, which the profile does not enable\n",
		       (unsigned long)ref_clk);
		return -EINVAL;
	}

	if (tx_rate != rx_rate) {
		pr_err("TX SSI clk is driven by RX%lu: TX rate %lu Hz and RX rate %lu Hz must match\n",
		       (unsigned long)ref_clk, (unsigned long)tx_rate,
		       (unsigned long)rx_rate);
		return -EINVAL;
	}

	return 0;
}

static uint32_t adrv910x_axi_port_off(uint32_t port)
{
	switch (port) {
	case ADRV910X_RX2:
		return ADI_RX2_REG_OFF;
	case ADRV910X_TX:
		return ADI_TX1_REG_OFF;
	default:
		return 0;
	}
}

/**
 * @brief Configure the SSI of the AXI core of a port.
 * @param phy - The device structure.
 * @param n_lanes - Number of lanes, as in the profile (ssiNumLane_e).
 * @param cmos_ddr - CMOS DDR enable, as in the profile.
 * @param port - ADRV910X_RX1, ADRV910X_RX2 or ADRV910X_TX.
 * @return 0 in case of success, negative error code otherwise.
 */
int adrv910x_axi_interface_set(const struct adrv910x_rf_phy *phy,
			       const uint8_t n_lanes, const bool cmos_ddr,
			       const uint32_t port)
{
	uint32_t reg_ctrl, reg_value = 0, off, divider = 0, lanes, div;
	const bool tx = port == ADRV910X_TX;

	if (!phy || !phy->rx1_adc || port >= ADRV910X_CHAN_MAX)
		return -EINVAL;

	off = adrv910x_axi_port_off(port);
	reg_ctrl = tx ? ADI_TX_REG_CTRL_2 : AXI_ADC_REG_CNTRL;

	axi_adc_read(phy->rx1_adc, AIM_AXI_REG(off, reg_ctrl), &reg_value);
	reg_value &= ~(NUM_LANES_MASK | SDR_DDR_MASK);

	switch (n_lanes) {
	case SSI_1_LANE:
		lanes = 1;
		if (phy->ssi_type == SSI_TYPE_CMOS)
			reg_value |= SDR_DDR(!cmos_ddr);
		break;
	case SSI_2_LANE:
		if (phy->ssi_type == SSI_TYPE_CMOS)
			return -EINVAL;

		lanes = 2;
		break;
	case SSI_4_LANE:
		if (phy->ssi_type == SSI_TYPE_LVDS)
			return -EINVAL;

		lanes = 4;
		reg_value |= SDR_DDR(!cmos_ddr);
		break;
	default:
		return -EINVAL;
	}

	reg_value |= NUM_LANES(lanes);
	axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, reg_ctrl), reg_value);

	if (tx) {
		uint32_t ddr = cmos_ddr;

		axi_adc_read(phy->rx1_adc,
			     AIM_AXI_REG(off, AXI_ADC_REG_CLK_RATIO), &divider);
		/* in LVDS, data type is always DDR */
		if (phy->ssi_type == SSI_TYPE_LVDS)
			ddr = 1;

		div = lanes * (1 + ddr) * divider;
		if (!div || div > 32) {
			pr_err("TX: invalid AXI clock ratio %lu\n",
			       (unsigned long)divider);
			return -EINVAL;
		}

		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, ADI_TX_REG_RATE),
			      32 / div - 1);
	}

	return 0;
}

/**
 * @brief Take the AXI core of a port out of reset or put it in reset.
 * @param phy - The device structure.
 * @param port - ADRV910X_RX1, ADRV910X_RX2 or ADRV910X_TX.
 * @param en - true to bring the core out of reset, false to reset it.
 */
void adrv910x_axi_interface_enable(const struct adrv910x_rf_phy *phy,
				   const uint32_t port, const bool en)
{
	uint32_t off = adrv910x_axi_port_off(port);

	if (!phy || !phy->rx1_adc)
		return;

	if (en)
		/* bring axi core out of reset */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_RSTN),
			      AXI_ADC_RSTN | AXI_ADC_MMCM_RSTN);
	else
		/* reset axi core */
		axi_adc_write(phy->rx1_adc, AIM_AXI_REG(off, AXI_ADC_REG_RSTN), 0);
}

/**
 * @brief Configure the AXI cores of the ports the profile enables, as the
 *        profile says, and take them out of reset. Called by the driver at
 *        the end of the device initialization.
 * @param phy - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adrv910x_axi_ssi_configure(struct adrv910x_rf_phy *phy)
{
	const deviceProfile_t *profile;
	uint32_t axi_config = 0;
	uint32_t num_chan, port, off;
	uint32_t i;
	int ret;

	if (!phy || !phy->rx1_adc || !phy->device_profile)
		return -EINVAL;

	profile = &phy->device_profile->profilePS1;
	num_chan = phy->rx1_adc->num_channels;

	axi_adc_read(phy->rx1_adc, AXI_ADC_REG_CONFIG, &axi_config);
	if (!!IS_CMOS(axi_config) != (phy->ssi_type == SSI_TYPE_CMOS)) {
		pr_err("HDL is built for %s, the profile is not\n",
		       IS_CMOS(axi_config) ? "CMOS" : "LVDS");
		return -EINVAL;
	}

	/* set R1_MODE to 1 rf channel in all cores */
	axi_adc_write(phy->rx1_adc, AXI_ADC_REG_CNTRL, R1_MODE);
	axi_adc_write(phy->rx1_adc, AIM_AXI_REG(ADI_RX2_REG_OFF, AXI_ADC_REG_CNTRL),
		      R1_MODE);
	axi_adc_write(phy->rx1_adc, AIM_AXI_REG(ADI_TX1_REG_OFF, ADI_TX_REG_CTRL_2),
		      TX_R1_MODE);

	for (port = 0; port < ADRV910X_CHAN_MAX; port++) {
		const struct adrv910x_chan *chan = phy->channels[port];
		const ssiConfig_t *ssi;

		if (!chan || !chan->enabled)
			continue;

		if (port == ADRV910X_TX) {
			/*
			 * Reported but not fatal: RX still works with a TX
			 * clocked at the wrong rate.
			 */
			adrv910x_axi_tx_ref_clk_check(phy);
		}

		if (port == ADRV910X_TX)
			ssi = &profile->txConfig[chan->idx].txSsiConfig;
		else
			ssi = &profile->rxConfig[chan->idx].rxSsiConfig;

		ret = adrv910x_axi_interface_set(phy, ssi->numLaneSel,
						 ssi->ddrEn, port);
		if (ret)
			return ret;

		adrv910x_axi_interface_enable(phy, port, true);

		if (port == ADRV910X_TX)
			continue;

		off = adrv910x_axi_port_off(port);
		for (i = 0; i < num_chan; i++)
			axi_adc_write(phy->rx1_adc,
				      AIM_AXI_REG(off, AXI_ADC_REG_CHAN_CNTRL(i)),
				      AXI_ADC_FORMAT_SIGNEXT |
				      AXI_ADC_FORMAT_ENABLE | AXI_ADC_ENABLE);
	}

	no_os_mdelay(100);

	return 0;
}
