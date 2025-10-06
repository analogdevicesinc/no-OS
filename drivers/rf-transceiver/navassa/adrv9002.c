/***************************************************************************//**
 *   @file   adrv9002.c
 *   @brief  adrv9002 driver source.
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
#include <string.h>
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_clk.h"
#include "adrv9002.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_auxadc.h"
#include "adi_adrv9001_auxadc_types.h"
#include "adi_adrv9001_bbdc.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_cals_types.h"
#include "adi_adrv9001_dpd.h"
#include "adi_adrv9001_dpd_types.h"
#include "adi_common_types.h"
#include "adi_adrv9001_auxdac.h"
#include "adi_adrv9001_auxdac_types.h"
#include "adi_adrv9001_fh.h"
#include "adi_adrv9001_fh_types.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_gpio_types.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_orx.h"
#include "adi_adrv9001_powermanagement.h"
#include "adi_adrv9001_powermanagement_types.h"
#include "adi_adrv9001_profile_types.h"
#include "adi_adrv9001_profileutil.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_rx_gaincontrol.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_rx_types.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_ssi_types.h"
#include "adi_adrv9001_stream.h"
#include "adi_adrv9001_stream_types.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_tx.h"
#include "adi_adrv9001_tx_types.h"
#include "adi_adrv9001_txSettings_types.h"
#include "adi_adrv9001_utilities.h"
#include "adi_common_error_types.h"

#define ALL_RX_CHANNEL_MASK	(ADI_ADRV9001_RX1 | ADI_ADRV9001_RX2 | \
				 ADI_ADRV9001_ORX1 | ADI_ADRV9001_ORX2)

#define ADRV9002_PORT_BIT(c)	(((c)->idx * 2 + (c)->port) & 0x3)
#define ADRV9002_PORT_MASK(c)	NO_OS_BIT(ADRV9002_PORT_BIT(c))

#define ADRV9002_RX_MAX_GAIN_mdB	\
	((ADI_ADRV9001_RX_GAIN_INDEX_MAX - ADI_ADRV9001_RX_GAIN_INDEX_MIN) *	\
	 ADRV9002_RX_GAIN_STEP_mDB)
#define ADRV9002_RX_GAIN_STEP_mDB	500
/* ORx gain defines */
#define ADRV9002_ORX_GAIN_STEP_mDB	5000
#define ADRV9002_ORX_MIN_GAIN_IDX	ADI_ADRV9001_ORX_GAIN_INDEX_MIN
#define ADRV9002_ORX_MAX_GAIN_IDX	ADI_ADRV9001_ORX_GAIN_INDEX_MAX
/*
 * the Orx tables indexes are the same in a x2 step. And only the even index will actually
 * take effect in the device. That's why we divide by 2...
 */
#define ADRV9002_ORX_MAX_GAIN_DROP_mdB	\
	((ADI_ADRV9001_ORX_GAIN_INDEX_MAX - ADI_ADRV9001_ORX_GAIN_INDEX_MIN) / 2 \
	 * ADRV9002_ORX_GAIN_STEP_mDB)
#define ADRV9002_ORX_MIN_GAIN_mdB	({ \
	(ADRV9002_RX_MAX_GAIN_mdB - ADRV9002_ORX_MAX_GAIN_DROP_mdB); \
})

#define ADRV9002_STREAM_BINARY_SZ 	ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES
#define ADRV9002_PROFILE_MAX_SZ		73728
#define ADRV9002_HP_CLK_PLL_DAHZ	884736000
#define ADRV9002_NO_EXT_LO		0xff
#define ADRV9002_EXT_LO_FREQ_MIN	60000000
#define ADRV9002_EXT_LO_FREQ_MAX	12000000000ULL
#define ADRV9002_DEV_CLKOUT_MIN		(10 * 1000000)
#define ADRV9002_DEV_CLKOUT_MAX		(80 * 1000000)
/*
 * The entries are reported as %freq_min,%freq_max and we have max of 7 entries. 256 bytes should
 * be more than enough!
 */
#define ADRV9002_BIN_FH_ENTRIES_SZ	256
/* Frequency hopping */
#define ADRV9002_FH_TABLE_COL_SZ	7

#define ADRV9002_INIT_CALS_TIMEOUT_MS	(120 * 1000)

#define ADRV9002_PORT_SWITCH_IN_RANGE(port, carrier)	( \
	((carrier) >= (port)->minFreqPortB_Hz && (carrier) <= (port)->maxFreqPortB_Hz) || \
	((carrier) >= (port)->minFreqPortA_Hz && (carrier) <= (port)->maxFreqPortA_Hz))

/* IRQ Masks */
#define ADRV9002_GP_MASK_RX_DP_RECEIVE_ERROR		0x08000000
#define ADRV9002_GP_MASK_TX_DP_TRANSMIT_ERROR		0x04000000
#define ADRV9002_GP_MASK_RX_DP_READ_REQUEST_FROM_BBIC	0x02000000
#define ADRV9002_GP_MASK_TX_DP_WRITE_REQUEST_TO_BBIC	0x01000000
#define ADRV9002_GP_MASK_STREAM_PROCESSOR_3_ERROR	0x00100000
#define ADRV9002_GP_MASK_STREAM_PROCESSOR_2_ERROR	0x00080000
#define ADRV9002_GP_MASK_STREAM_PROCESSOR_1_ERROR	0x00040000
#define ADRV9002_GP_MASK_STREAM_PROCESSOR_0_ERROR	0x00020000
#define ADRV9002_GP_MASK_MAIN_STREAM_PROCESSOR_ERROR	0x00010000
#define ADRV9002_GP_MASK_LSSI_RX2_CLK_MCS		0x00008000
#define ADRV9002_GP_MASK_LSSI_RX1_CLK_MCS		0x00004000
#define ADRV9002_GP_MASK_CLK_1105_MCS_SECOND		0x00002000
#define ADRV9002_GP_MASK_CLK_1105_MCS			0x00001000
#define ADRV9002_GP_MASK_CLK_PLL_LOCK			0x00000800
#define ADRV9002_GP_MASK_AUX_PLL_LOCK			0x00000400
#define ADRV9002_GP_MASK_RF2_SYNTH_LOCK			0x00000200
#define ADRV9002_GP_MASK_RF_SYNTH_LOCK			0x00000100
#define ADRV9002_GP_MASK_CLK_PLL_LOW_POWER_LOCK		0x00000080
#define ADRV9002_GP_MASK_TX2_PA_PROTECTION_ERROR	0x00000040
#define ADRV9002_GP_MASK_TX1_PA_PROTECTION_ERROR	0x00000020
#define ADRV9002_GP_MASK_CORE_ARM_MONITOR_ERROR		0x00000010
#define ADRV9002_GP_MASK_CORE_ARM_CALIBRATION_ERROR	0x00000008
#define ADRV9002_GP_MASK_CORE_ARM_SYSTEM_ERROR		0x00000004
#define ADRV9002_GP_MASK_CORE_FORCE_GP_INTERRUPT	0x00000002
#define ADRV9002_GP_MASK_CORE_ARM_ERROR			0x00000001

#define ADRV9002_IRQ_MASK					\
	(ADRV9002_GP_MASK_CORE_ARM_ERROR |			\
	 ADRV9002_GP_MASK_CORE_FORCE_GP_INTERRUPT |		\
	 ADRV9002_GP_MASK_CORE_ARM_SYSTEM_ERROR |		\
	 ADRV9002_GP_MASK_CORE_ARM_CALIBRATION_ERROR |		\
	 ADRV9002_GP_MASK_CORE_ARM_MONITOR_ERROR |		\
	 ADRV9002_GP_MASK_TX1_PA_PROTECTION_ERROR |		\
	 ADRV9002_GP_MASK_TX2_PA_PROTECTION_ERROR |		\
	 ADRV9002_GP_MASK_CLK_PLL_LOW_POWER_LOCK |		\
	 ADRV9002_GP_MASK_RF_SYNTH_LOCK |			\
	 ADRV9002_GP_MASK_RF2_SYNTH_LOCK |			\
	 ADRV9002_GP_MASK_AUX_PLL_LOCK |			\
	 ADRV9002_GP_MASK_CLK_PLL_LOCK |			\
	 ADRV9002_GP_MASK_MAIN_STREAM_PROCESSOR_ERROR |		\
	 ADRV9002_GP_MASK_STREAM_PROCESSOR_0_ERROR |		\
	 ADRV9002_GP_MASK_STREAM_PROCESSOR_1_ERROR |		\
	 ADRV9002_GP_MASK_STREAM_PROCESSOR_2_ERROR |		\
	 ADRV9002_GP_MASK_STREAM_PROCESSOR_3_ERROR |		\
	 ADRV9002_GP_MASK_TX_DP_WRITE_REQUEST_TO_BBIC |		\
	 ADRV9002_GP_MASK_RX_DP_READ_REQUEST_FROM_BBIC |	\
	 ADRV9002_GP_MASK_TX_DP_TRANSMIT_ERROR |		\
	 ADRV9002_GP_MASK_RX_DP_RECEIVE_ERROR)

#define ADRV9002_RX_BIT_START		(ffs(ADI_ADRV9001_RX1) - 1)
#define ADRV9002_TX_BIT_START		(ffs(ADI_ADRV9001_TX1) - 1)
#define ADRV9002_ORX_BIT_START		(ffs(ADI_ADRV9001_ORX1) - 1)
#define ADRV9002_ELB_BIT_START		(ffs(ADI_ADRV9001_ELB1) - 1)

enum {
	ADRV9002_TX_A,
	ADRV9002_TX_B,
};

int __adrv9002_dev_err(const struct adrv9002_rf_phy *phy, const char *function,
		       const int line)
{
	int ret;

	printf("%s, %d: failed with \"%s\" (%d)\n", function, line,
	       phy->adrv9001->common.error.errormessage ?
	       phy->adrv9001->common.error.errormessage : "",
	       phy->adrv9001->common.error.errCode);

	switch (phy->adrv9001->common.error.errCode) {
	case ADI_COMMON_ERR_INV_PARAM:
	case ADI_COMMON_ERR_NULL_PARAM:
		ret = -EINVAL;
		break;
	case ADI_COMMON_ERR_API_FAIL:
		ret = -EFAULT;
		break;
	case ADI_COMMON_ERR_SPI_FAIL:
		ret = -EIO;
		break;
	case ADI_COMMON_ERR_MEM_ALLOC_FAIL:
		ret = -ENOMEM;
		break;
	default:
		ret = -EFAULT;
		break;
	}

	adi_common_ErrorClear(&phy->adrv9001->common);

	return ret;
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
	int c, ret;

	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		const struct adrv9002_chan *chann = phy->channels[c];

		/* RX2/TX2 can only be enabled if RX1/TX1 are also enabled */
		if (phy->rx2tx2 && chann->idx > ADRV9002_CHANN_1)
			break;

		if (!chann->enabled)
			continue;

		adrv9002_sync_gpio_toggle(phy);

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
			no_os_clk_set_rate(rx->tdd_clk, rate);
		}
	}

	return 0;
}

#define ADRV9002_MAX_CLK_NAME 79

static char *adrv9002_clk_set_dev_name(const struct adrv9002_rf_phy *phy,
				       char *dest, const char *name)
{
	size_t len = 0;

	if (!name)
		return NULL;

	if (*name == '-')
		len = snprintf(dest, ADRV9002_MAX_CLK_NAME, "%s", "adrv9002-phy");
	else
		*dest = '\0';

	return strncat(dest, name, ADRV9002_MAX_CLK_NAME - len);
}

static int32_t adrv9002_bb_recalc_rate(struct no_os_clk_desc *desc,
				       uint64_t *rate)
{
	struct adrv9002_rf_phy *adrv9002_dev;
	enum adrv9002_clocks source;

	adrv9002_dev = desc->dev_desc;

	if (!strcmp(desc->name, "adrv9002-phy-rx1_sampl_clk"))
		source = RX1_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-rx2_sampl_clk"))
		source = RX2_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tx1_sampl_clk"))
		source = TX1_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tx2_sampl_clk"))
		source = TX2_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tdd1_intf_clk"))
		source = TDD1_INTF_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tdd2_intf_clk"))
		source = TDD2_INTF_CLK;
	else
		return -EINVAL;

	*rate = adrv9002_dev->clk_priv[source].rate;

	return 0;
}

static int32_t adrv9002_bb_set_rate(struct no_os_clk_desc *desc,
				    uint64_t rate)
{
	struct adrv9002_rf_phy *adrv9002_dev;
	enum adrv9002_clocks source;

	adrv9002_dev = desc->dev_desc;

	if (!strcmp(desc->name, "adrv9002-phy-rx1_sampl_clk"))
		source = RX1_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-rx2_sampl_clk"))
		source = RX2_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tx1_sampl_clk"))
		source = TX1_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tx2_sampl_clk"))
		source = TX2_SAMPL_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tdd1_intf_clk"))
		source = TDD1_INTF_CLK;
	else if (!strcmp(desc->name, "adrv9002-phy-tdd2_intf_clk"))
		source = TDD2_INTF_CLK;
	else
		return -EINVAL;

	adrv9002_dev->clk_priv[source].rate = rate;

	return 0;
}

static int32_t adrv9002_bb_round_rate(struct no_os_clk_desc *desc,
				      uint64_t rate,
				      uint64_t *rounded_rate)
{
	pr_debug("%s: Rate %lu Hz", __func__, rate);

	*rounded_rate = rate;

	return 0;
}

static const struct no_os_clk_platform_ops adrv9002_bb_clk_ops = {
	.clk_round_rate = &adrv9002_bb_round_rate,
	.clk_set_rate = &adrv9002_bb_set_rate,
	.clk_recalc_rate = &adrv9002_bb_recalc_rate,
};

static const char * const clk_names[NUM_ADRV9002_CLKS] = {
	[RX1_SAMPL_CLK] = "adrv9002-phy-rx1_sampl_clk",
	[RX2_SAMPL_CLK] = "adrv9002-phy-rx2_sampl_clk",
	[TX1_SAMPL_CLK] = "adrv9002-phy-tx1_sampl_clk",
	[TX2_SAMPL_CLK] = "adrv9002-phy-tx2_sampl_clk",
	[TDD1_INTF_CLK] = "adrv9002-phy-tdd1_intf_clk",
	[TDD2_INTF_CLK] = "adrv9002-phy-tdd2_intf_clk"
};

static struct no_os_clk_desc *adrv9002_clk_register(struct adrv9002_rf_phy *phy,
		const char *name,
		const uint32_t source)
{
	struct adrv9002_clock *clk_priv = &phy->clk_priv[source];
	struct no_os_clk_init_param init = { 0 };
	struct no_os_clk_desc *clk;
	int ret;

	/* struct adrv9002_clock assignments */
	clk_priv->source = source;
	clk_priv->spi = phy->spi;
	clk_priv->phy = phy;

	init.name = clk_names[source];
	init.hw_ch_num = 1;
	init.platform_ops = &adrv9002_bb_clk_ops;
	init.dev_desc = phy;

	ret = no_os_clk_init(&clk, &init);
	if (NO_OS_IS_ERR(clk) || ret) {
		pr_err("Error registering clock=%d, err=%ld\n", source,
		       NO_OS_PTR_ERR(clk));
		return NO_OS_ERR_CAST(clk);
	}

	phy->clks[phy->n_clks++] = clk;

	return clk;
}

static void adrv9002_set_clk_rates(const struct adrv9002_rf_phy *phy)
{
	int c;

	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		const struct adrv9002_chan *chan = phy->channels[c];

		if (!chan->enabled)
			continue;

		no_os_clk_set_rate(chan->clk, chan->rate);
	}
}

enum lo_ext_info {
	LOEXT_FREQ,
};

static int adrv9002_gainidx_to_gain(int idx, int port)
{
	int gain;

	if (port  == ADI_RX) {
		idx = no_os_clamp(idx, ADRV9002_RX_MIN_GAIN_IDX, ADRV9002_RX_MAX_GAIN_IDX);
		gain = (idx - ADRV9002_RX_MIN_GAIN_IDX) * ADRV9002_RX_GAIN_STEP_mDB;
	} else {
		/* ADI_ORX - look at the ORX defines for why we have the div/mult by 2 */
		idx = no_os_clamp(idx, ADRV9002_ORX_MIN_GAIN_IDX, ADRV9002_ORX_MAX_GAIN_IDX);
		gain = (idx - ADRV9002_ORX_MIN_GAIN_IDX) / 2 * ADRV9002_ORX_GAIN_STEP_mDB +
		       ADRV9002_ORX_MIN_GAIN_mdB;
	}

	return gain;
}

static int adrv9002_gain_to_gainidx(int gain, int port)
{
	int temp;

	if (port  == ADI_RX) {
		gain = no_os_clamp(gain, 0, ADRV9002_RX_MAX_GAIN_mdB);
		temp = NO_OS_DIV_ROUND_CLOSEST(gain, ADRV9002_RX_GAIN_STEP_mDB);
		temp += ADRV9002_RX_MIN_GAIN_IDX;
	} else {
		/* ADI_ORX */
		gain = no_os_clamp(gain, ADRV9002_ORX_MIN_GAIN_mdB, ADRV9002_RX_MAX_GAIN_mdB);
		temp = NO_OS_DIV_ROUND_CLOSEST(gain - ADRV9002_ORX_MIN_GAIN_mdB,
					       ADRV9002_ORX_GAIN_STEP_mDB) * 2;
		temp += ADRV9002_ORX_MIN_GAIN_IDX;
	}

	return temp;
}

static int adrv9002_chan_to_state_poll(const struct adrv9002_rf_phy *phy,
				       const struct adrv9002_chan *c,
				       const adi_adrv9001_ChannelState_e state,
				       const int n_tries)
{
	int ret;
	adi_adrv9001_ChannelState_e __state;
	int try = 0;

	do {
		ret = adi_adrv9001_Radio_Channel_State_Get(phy->adrv9001, c->port, c->number,
				&__state);
		if (ret)
			return adrv9002_dev_err(phy);

		if (__state == state)
			break;
		no_os_udelay(1000);
	} while (++try < n_tries);

	if (try == n_tries)
			return -EBUSY;

	return 0;
}

static bool adrv9002_orx_enabled(const struct adrv9002_rf_phy *phy,
				 const struct adrv9002_chan *c)
{
	const struct adrv9002_rx_chan *rx;
	uint8_t value = 0;
	int ret;

	if (c->port == ADI_RX)
		rx = chan_to_rx(c);
	else
		rx = &phy->rx_channels[c->idx];

	if (!rx->orx_gpio)
		return false;

	ret = no_os_gpio_get_value(rx->orx_gpio, &value);
	if (ret)
		pr_err("GPIO value could not be read (%d)\n", ret);

	return !!value;
}

int adrv9002_channel_to_state(const struct adrv9002_rf_phy *phy,
			      struct adrv9002_chan *chann,
			      const adi_adrv9001_ChannelState_e state, const bool cache_state)
{
	int ret;
	adi_adrv9001_ChannelEnableMode_e mode;

	/* nothing to do */
	if (!chann->enabled)
		return 0;

	/*
	 * if ORx is enabled we are not expected to do any state transition on RX/TX in the
	 * same channel as that might have the non explicit side effect of breaking the
	 * capture in the ORx port. Hence, we should protect against that...
	 */
	if (adrv9002_orx_enabled(phy, chann))
		return -EPERM;

	ret = api_call(phy, adi_adrv9001_Radio_ChannelEnableMode_Get, chann->port,
		       chann->number, &mode);
	if (ret)
		return ret;

	/* we need to set it to spi */
	if (mode == ADI_ADRV9001_PIN_MODE) {
		ret = api_call(phy, adi_adrv9001_Radio_ChannelEnableMode_Set, chann->port,
			       chann->number, ADI_ADRV9001_SPI_MODE);
		if (ret)
			return ret;
	}

	if (cache_state) {
		ret = api_call(phy, adi_adrv9001_Radio_Channel_State_Get, chann->port,
			       chann->number, &chann->cached_state);
		if (ret)
			return ret;
	}

	ret = api_call(phy, adi_adrv9001_Radio_Channel_ToState, chann->port,
		       chann->number, state);
	if (ret)
		return ret;
	/*
	 * Make sure that the channel is really in the state we want as it might take time
	 * for the device to actually do the change (mainly when moving to rf_enabled).
	 */
	ret = adrv9002_chan_to_state_poll(phy, chann, state, 7);
	if (ret) {
		/*
		 * This is important when the device is in PIN mode as changing it to SPI
		 * might trigger a state change to rf_enabled. In that case it looks like the
		 * first call to @adi_adrv9001_Radio_Channel_ToState() is just ignored as the
		 * device is still busy. Hence we try one last time to move the channel to the
		 * desired state and double up the number of tries...
		 */
		pr_debug("Try to change to state(%d) again...\n", state);

		ret = api_call(phy, adi_adrv9001_Radio_Channel_ToState, chann->port,
			       chann->number, state);
		if (ret)
			return ret;

		ret = adrv9002_chan_to_state_poll(phy, chann, state, 14);
		if (ret)
			return ret;
	}

	if (mode == ADI_ADRV9001_SPI_MODE)
		return 0;

	/* restore enable mode */
	return api_call(phy, adi_adrv9001_Radio_ChannelEnableMode_Set,
			chann->port, chann->number, mode);
}

static struct adrv9002_chan *adrv9002_get_channel(struct adrv9002_rf_phy *phy,
		const int port, const int chann)
{
	if (port == ADI_TX)
		return &phy->tx_channels[chann].channel;

	return &phy->rx_channels[chann].channel;
}

enum {
	ADRV9002_HOP_1_TABLE_SEL,
	ADRV9002_HOP_2_TABLE_SEL,
	ADRV9002_HOP_1_TRIGGER,
	ADRV9002_HOP_2_TRIGGER,
	ADRV9002_INIT_CALS_RUN,
	ADRV9002_WARMBOOT_SEL,
	ADRV9002_MCS,
};

static const char * const adrv9002_hop_table[ADRV9002_FH_TABLES_NR + 1] = {
	"TABLE_A",
	"TABLE_B",
	"Unknown"
};

static const char * const adrv9002_init_cals_modes[] = {
	"off",
	"auto",
	"run"
};

static void adrv9002_port_enable(const struct adrv9002_rf_phy *phy,
				 const struct adrv9002_chan *c, bool enable)
{
	/*
	 * Handle port muxes. Always terminate the ports at 50ohm when disabling and only
	 * mux them again if the channel is enabled.
	 */
	if (!enable) {
		if (c->mux_ctl)
			no_os_gpio_set_value(c->mux_ctl, 0);
		if (c->mux_ctl_2)
			no_os_gpio_set_value(c->mux_ctl_2, 0);
	} else if (c->enabled) {
		bool ctl_assert = true;

		/* Make sure to respect any possible TX port selection given by userspace. */
		if (c->port == ADI_TX && chan_to_tx(c)->port_sel == ADRV9002_TX_B)
			ctl_assert = false;

		if (c->mux_ctl && ctl_assert)
			no_os_gpio_set_value(c->mux_ctl, 1);
		if (c->mux_ctl_2)
			no_os_gpio_set_value(c->mux_ctl_2, 1);
	}

	/*
	 * Nothing to do for channel 2 in rx2tx2 mode. The check is useful to have
	 * it in here if the outer loop is looping through all the channels.
	 */
	if (phy->rx2tx2 && c->idx > ADRV9002_CHANN_1)
		return;
	/*
	 * We always disable but let's not enable a port that is not enable in the profile.
	 * Same as above, the condition is needed if the outer loop is looping through all
	 * channels to enable/disable them. Might be a redundant in some cases where the
	 * caller already knows the state of the port.
	 */
	if (enable && !c->enabled)
		return;

	adrv9002_axi_interface_enable(phy, c->idx, c->port == ADI_TX, enable);
}

static int adrv9002_phy_rerun_cals_setup(struct adrv9002_rf_phy *phy,
		unsigned long port_mask,
		bool after)
{
	int ret, c;

	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		adi_adrv9001_ChannelState_e state = after ? phy->channels[c]->cached_state
						    : ADI_ADRV9001_CHANNEL_CALIBRATED;

		adrv9002_port_enable(phy, phy->channels[c], after);

		/* if the bit is set, then we already moved the port */
		if (no_os_test_bit(ADRV9002_PORT_BIT(phy->channels[c]), &port_mask))
			continue;

		ret = adrv9002_channel_to_state(phy, phy->channels[c], state, !after);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9002_phy_rerun_cals(struct adrv9002_rf_phy *phy,
				   struct adi_adrv9001_InitCals *init_cals, unsigned long port_mask)
{
	uint8_t error;
	int ret;

	if (!init_cals->chanInitCalMask[0] && !init_cals->chanInitCalMask[1])
		return 0;

	/* move the missing ports to calibrated state and disable all the axi cores */
	ret = adrv9002_phy_rerun_cals_setup(phy, port_mask, false);
	if (ret)
		return ret;

	pr_debug("Re-run init cals: mask: %08X, %08X\n",
		 init_cals->chanInitCalMask[0], init_cals->chanInitCalMask[1]);

	ret = api_call(phy, adi_adrv9001_cals_InitCals_Run, init_cals,
		       ADRV9002_INIT_CALS_TIMEOUT_MS, &error);
	if (ret)
		return ret;

	/* re-enable cores and re-set port states (those where the carrier is not being changed) */
	return adrv9002_phy_rerun_cals_setup(phy, port_mask, true);
}

/*
 * !\Note that this interface does not make much sense with RX port switch enabled. The reason
 * is that when in auto mode, the calibrations will run for the range of configured carriers and
 * we cannot configure any frequency outside of that range. And for manual mode, we will calibrate
 * the full range of frequencies. Hence, in theory, re-running the calibrations should not bring
 * any additional value and can take a lot of time... However, it also does not harm so that we are
 * keeping the interface (for simplicity) but take care as it can really take a lot of time.
 */
static int adrv9002_init_cals_set(struct adrv9002_rf_phy *phy, const char *buf)
{
	struct adi_adrv9001_InitCals cals = {0};
	unsigned int i;
	int c, ret = -1;

	for (i = 0; i < 3; i++) {
		if (adrv9002_init_cals_modes[i]
		    && strcmp(adrv9002_init_cals_modes[i], buf) == 0) {
			ret = 1;
			break;
		}
	}
	if (ret < 0)
		return ret;

	if (strcmp(adrv9002_init_cals_modes[ret], "run")) {
		if (!strcmp(adrv9002_init_cals_modes[ret], "auto"))
			phy->run_cals = true;
		else
			phy->run_cals = false;

		return 0;
	}

	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++)
		cals.chanInitCalMask[phy->channels[c]->idx] |= phy->channels[c]->lo_cals;

	return adrv9002_phy_rerun_cals(phy, &cals, 0);
}

static int adrv9002_warm_boot_name_save(struct adrv9002_rf_phy *phy,
					const char *buf)
{
	int ret;
	char *p;

	ret = strlcpy(phy->warm_boot.coeffs_name, buf,
		      sizeof(phy->warm_boot.coeffs_name));
	if (ret < 0)
		return ret;

	/* Strip trailing newline */
	p = phy->warm_boot.coeffs_name + strlen(phy->warm_boot.coeffs_name);
	if (*--p == '\n')
		*p = '\0';

	return 0;
}

static int adrv9002_mcs_run(struct adrv9002_rf_phy *phy, const char *buf)
{
	adi_adrv9001_RadioState_t radio = {0};
	unsigned int i;
	int ret, try = 0;

	if (!phy->curr_profile->sysConfig.mcsMode) {
		pr_err("Multi chip sync not enabled\n");
		return -524;
	}

	if (phy->mcs_run) {
		/*
		 * !\FIXME: Ugly hack but MCS only runs successful once and then always fails
		 * (get's stuck). Hence let's just not allow running more than once and print
		 * something so people can see this is a known thing.
		 */
		pr_err("Multi chip sync can only run once for now...\n");
		return -EPERM;
	}

	/* all channels need to be in calibrated state...*/
	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->channels); i++) {
		struct adrv9002_chan *c = phy->channels[i];

		ret = adrv9002_channel_to_state(phy, c, ADI_ADRV9001_CHANNEL_CALIBRATED, true);
		if (ret)
			return ret;
	}

	ret = api_call(phy, adi_adrv9001_Radio_ToMcsReady);
	if (ret)
		return ret;

	do {
		ret = adi_adrv9001_Radio_State_Get(phy->adrv9001, &radio);
		if (ret)
			return adrv9002_dev_err(phy);

		if (radio.mcsState == ADI_ADRV9001_ARMMCSSTATES_DONE)
			break;
		no_os_udelay(20 * 1000);
	} while (++try < 10 * 1000);


	if (ret)
		return __adrv9002_dev_err(phy, __func__, __LINE__);
	if (try == 10 * 1000)
			return -EBUSY;

	phy->mcs_run = true;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->channels); i++) {
		struct adrv9002_chan *c = phy->channels[i];

		ret = adrv9002_channel_to_state(phy, c, c->cached_state, false);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9002_set_ext_lo(const struct adrv9002_chan *c, uint64_t freq)
{
	uint64_t lo_freq;

	if (!c->ext_lo)
		return 0;

	lo_freq = freq * c->ext_lo->divider;
	if (lo_freq < ADRV9002_EXT_LO_FREQ_MIN || lo_freq > ADRV9002_EXT_LO_FREQ_MAX) {
		pr_err("Ext LO freq not in the [%d %llu] range\n",
		       ADRV9002_EXT_LO_FREQ_MIN, ADRV9002_EXT_LO_FREQ_MAX);
		return -EINVAL;
	}

	return no_os_clk_set_rate(c->ext_lo->clk,
				  no_os_div_u64(freq * c->ext_lo->scale.mult, c->ext_lo->scale.div));
}

static int adrv9002_phy_lo_set(struct adrv9002_rf_phy *phy,
			       struct adrv9002_chan *c,
			       struct adi_adrv9001_InitCals *init_cals, uint64_t freq)
{
	struct adi_adrv9001_Carrier lo_freq;
	int ret;

	ret = api_call(phy, adi_adrv9001_Radio_Carrier_Inspect, c->port, c->number,
		       &lo_freq);
	if (ret)
		return ret;

	if (c->port == ADI_RX && phy->port_switch.enable)  {
		if (phy->port_switch.manualRxPortSwitch) {
			/*
			 * Respect manual port selection in case we have it. The reason for doing
			 * this is that adi_adrv9001_Radio_Carrier_Inspect() always returns RXA.
			 */
			lo_freq.manualRxport = chan_to_rx(c)->manual_port;
		} else if (!ADRV9002_PORT_SWITCH_IN_RANGE(&phy->port_switch, freq)) {
			/*
			 * !note the API would also refuse to configure an out of range frequency
			 * but the error message is not really helpful...
			 */
			pr_err(
				"RX%u Carrier(%llu) out of range for port switch: RXA[%llu %llu], RXB[%llu %llu]\n",
				c->idx + 1, freq, phy->port_switch.minFreqPortA_Hz,
				phy->port_switch.maxFreqPortA_Hz, phy->port_switch.minFreqPortB_Hz,
				phy->port_switch.maxFreqPortB_Hz);
			return -EINVAL;
		}
	}

	ret = adrv9002_set_ext_lo(c, freq);
	if (ret)
		return ret;

	if (abs(freq - lo_freq.carrierFrequency_Hz) >= 100 * 1000000 && phy->run_cals)
		init_cals->chanInitCalMask[c->idx] |= c->lo_cals;

	lo_freq.carrierFrequency_Hz = freq;
	/*
	 * !\FIXME: This is an ugly workaround for being able to control carriers when a MCS
	 * enabled profile is loaded. When MCS is on, we get 'loGenOptimization = 2' which is
	 * not a valid value and hence the API call will fail...
	 */
	if (phy->curr_profile->sysConfig.mcsMode &&
	    lo_freq.loGenOptimization > ADI_ADRV9001_LO_GEN_OPTIMIZATION_POWER_CONSUMPTION)
		lo_freq.loGenOptimization = ADI_ADRV9001_LO_GEN_OPTIMIZATION_POWER_CONSUMPTION;

	return api_call(phy, adi_adrv9001_Radio_Carrier_Configure, c->port, c->number,
			&lo_freq);
}

static int adrv9002_phy_lo_set_ports(struct adrv9002_rf_phy *phy,
				     struct adrv9002_chan *c,
				     struct adi_adrv9001_InitCals *init_cals, uint64_t freq)
{
	int ret;
	int tx;

	if (c->port == ADI_RX) {
		int rx;

		for (rx = 0; rx < NO_OS_ARRAY_SIZE(phy->rx_channels); rx++) {
			if (c->lo != phy->rx_channels[rx].channel.lo)
				continue;

			ret = adrv9002_phy_lo_set(phy, &phy->rx_channels[rx].channel,
						  init_cals, freq);
			if (ret)
				return ret;
		}

		return 0;
	}

	for (tx = 0; tx < phy->chip->n_tx; tx++) {
		if (c->lo != phy->tx_channels[tx].channel.lo)
			continue;

		ret = adrv9002_phy_lo_set(phy, &phy->tx_channels[tx].channel, init_cals, freq);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9002_dgpio_config(const struct adrv9002_rf_phy *phy)
{
	struct adrv9002_gpio *dgpio = phy->adrv9002_gpios;
	int i, ret;

	for (i = 0; i < phy->ngpios; i++) {
		pr_debug("Set dpgio: %d, signal: %d\n",
			 dgpio[i].gpio.pin, dgpio[i].signal);

		ret = api_call(phy, adi_adrv9001_gpio_Configure, dgpio[i].signal,
			       &dgpio[i].gpio);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9001_rx_path_config(struct adrv9002_rf_phy *phy,
				   const adi_adrv9001_ChannelState_e state)
{
	int i, ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->rx_channels); i++) {
		struct adrv9002_rx_chan *rx = &phy->rx_channels[i];

		/* For each rx channel enabled */
		if (!rx->channel.enabled)
			continue;

		if (!rx->pin_cfg)
			goto agc_cfg;

		ret = api_call(phy, adi_adrv9001_Rx_GainControl_PinMode_Configure,
			       rx->channel.number, rx->pin_cfg);
		if (ret)
			return ret;

agc_cfg:
		ret = api_call(phy, adi_adrv9001_Rx_GainControl_Configure,
			       rx->channel.number, &rx->agc);
		if (ret)
			return ret;

		ret = api_call(phy, adi_adrv9001_Radio_Channel_ToState, ADI_RX,
			       rx->channel.number, state);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9002_tx_set_dac_full_scale(const struct adrv9002_rf_phy *phy)
{
	int i, ret = 0;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->tx_channels); i++) {
		const struct adrv9002_tx_chan *tx = &phy->tx_channels[i];

		if (!tx->channel.enabled || !tx->dac_boost_en)
			continue;

		ret = api_call(phy, adi_adrv9001_Tx_OutputPowerBoost_Set, tx->channel.number,
			       true);
		if (ret)
			return ret;
	}

	return ret;
}

static int adrv9002_dpd_ext_path_set(const struct adrv9002_rf_phy *phy)
{
	uint8_t init_calls_error;
	uint32_t delay, c;
	int ret;

	for (c = 0; c < phy->chip->n_tx; c++) {
		const struct adrv9002_tx_chan *tx = &phy->tx_channels[c];

		if (!tx->channel.enabled || !tx->ext_path_calib)
			continue;

		/* let's first measure the delay */
		ret = api_call(phy, adi_adrv9001_cals_ExternalPathDelay_Calibrate,
			       tx->channel.number, 60000, &init_calls_error, &delay);
		if (ret)
			return ret;

		ret = api_call(phy, adi_adrv9001_cals_ExternalPathDelay_Set,
			       tx->channel.number, delay);
		if (ret)
			return ret;
	}

	return 0;
}

static int adrv9002_tx_path_config(const struct adrv9002_rf_phy *phy,
				   const adi_adrv9001_ChannelState_e state)
{
	int i, ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->tx_channels); i++) {
		const struct adrv9002_tx_chan *tx = &phy->tx_channels[i];

		/* For each tx channel enabled */
		if (!tx->channel.enabled)
			continue;

		if (!tx->elb_en || !tx->dpd_init || !tx->dpd_init->enable)
			goto pin_cfg;

		ret = api_call(phy, adi_adrv9001_dpd_Configure, tx->channel.number, tx->dpd);
		if (ret)
			return ret;

pin_cfg:

		if (!tx->pin_cfg)
			goto rf_enable;

		ret = api_call(phy, adi_adrv9001_Tx_Attenuation_PinControl_Configure,
			       tx->channel.number, tx->pin_cfg);
		if (ret)
			return ret;

rf_enable:
		ret = api_call(phy, adi_adrv9001_Radio_Channel_ToState, ADI_TX,
			       tx->channel.number, state);
		if (ret)
			return ret;
	}

	return 0;
}

static const uint32_t adrv9002_init_cals_mask[16][2] = {
	/* Not a valid case. At least one channel should be enabled */
	[0] = {0, 0},
	/* tx2:0 rx2:0 tx1:0 rx1:1 */
	[1] = {0x1BE400, 0},
	/* tx2:0 rx2:0 tx1:1 rx1:0 */
	[2] = {0x1BE5F7, 0},
	/* tx2:0 rx2:0 tx1:1 rx1:1 */
	[3] = {0x1BE5F7, 0},
	/* tx2:0 rx2:1 tx1:0 rx1:0 */
	[4] = {0, 0x11E400},
	/* tx2:0 rx2:1 tx1:0 rx1:1 */
	[5] = {0x1BE400, 0x1BE400},
	/* tx2:0 rx2:1 tx1:1 rx1:0 */
	[6] = {0x1BE5F7, 0x1BE400},
	/* tx2:0 rx2:1 tx1:1 rx1:1 */
	[7] = {0x1BE5F7, 0x1BE400},
	/* tx2:1 rx2:0 tx1:0 rx1:0 */
	[8] = {0, 0x11E5F0},
	/* tx2:1 rx2:0 tx1:0 rx1:1 */
	[9] = {0x1BE400, 0x1BE5F0},
	/* tx2:1 rx2:0 tx1:1 rx1:0 */
	[10] = {0x1BE5F7, 0x1BE5F7},
	/* tx2:1 rx2:0 tx1:1 rx1:1 */
	[11] = {0x1BE5F7, 0x1BE5F7},
	/* tx2:1 rx2:1 tx1:0 rx1:0 */
	[12] = {0, 0x11E5F0},
	/* tx2:1 rx2:1 tx1:0 rx1:1 */
	[13] = {0x1BE400, 0x1BE5F0},
	/* tx2:1 rx2:1 tx1:1 rx1:0 */
	[14] = {0x1BE5F7, 0x1BE5F7},
	/* tx2:1 rx2:1 tx1:1 rx1:1 */
	[15] = {0x1BE5F7, 0x1BE5F7},
};

static void adrv9002_compute_init_cals(struct adrv9002_rf_phy *phy)
{
	int i, pos = 0;

	phy->init_cals.sysInitCalMask = 0;
	phy->init_cals.calMode = ADI_ADRV9001_INIT_CAL_MODE_ALL;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->channels); i++) {
		struct adrv9002_chan *c = phy->channels[i];

		if (!c->enabled)
			continue;

		if (c->port == ADI_RX)
			pos |= ADRV9002_PORT_MASK(c);
		else
			pos |= ADRV9002_PORT_MASK(c);
	}

	phy->init_cals.chanInitCalMask[0] = adrv9002_init_cals_mask[pos][0];
	phy->init_cals.chanInitCalMask[1] = adrv9002_init_cals_mask[pos][1];

	pr_debug("pos: %u, Chan1:%X, Chan2:%X", pos,
		 phy->init_cals.chanInitCalMask[0],
		 phy->init_cals.chanInitCalMask[1]);
}

static int adrv9002_ext_lo_validate(struct adrv9002_rf_phy *phy, int idx,
				    bool tx)
{
	adi_adrv9001_ClockSettings_t *clocks = &phy->curr_profile->clocks;
	adi_adrv9001_LoSel_e lo_selects[] = {
		clocks->rx1LoSelect, clocks->tx1LoSelect, clocks->rx2LoSelect, clocks->tx2LoSelect
	};
	adi_adrv9001_PllLoMode_e modes[] = { clocks->rfPll1LoMode, clocks->rfPll2LoMode };
	uint16_t dividers[] = { clocks->extLo1Divider, clocks->extLo2Divider };
	/* -1 since the enums start at 1 */
	unsigned int lo = lo_selects[idx * 2 + tx] - 1;

	if (lo >= NO_OS_ARRAY_SIZE(modes)) {
		/*
		 * Anything other than ADI_ADRV9001_LOSEL_LO{1|2} should be wrong...
		 */
		pr_err("Unknown LO(%u) on %s%d\n", lo, tx ? "TX" : "RX", idx + 1);
		return -EINVAL;
	}

	if (modes[lo] == ADI_ADRV9001_INT_LO1)
		return ADRV9002_NO_EXT_LO;

	/*
	 * Alright, if external LO is being set on the profile for this port, we need to have
	 * a matching clk to control.
	 */
	if (!phy->ext_los[lo].clk) {
		pr_err("Ext LO%d set for %s%d but not controlling clk provided via dts\n",
		       lo + 1, tx ? "TX" : "RX", idx + 1);
		return -EINVAL;
	}

	/* should never happen but we should also not blindly trust in the loaded profile... */
	if (!dividers[lo]) {
		pr_err("LO%d cannot have a divider of 0!\n", lo + 1);
		return -EINVAL;
	}

	phy->ext_los[lo].divider = dividers[lo];

	pr_debug("EXT LO%d being used for %s%d with div(%u)\n", lo + 1,
		 tx ? "TX" : "RX",
		 idx + 1, dividers[lo]);

	return lo;
}

static int adrv9002_rx_validate_profile(struct adrv9002_rf_phy *phy,
					unsigned int idx,
					const struct adi_adrv9001_RxChannelCfg *rx_cfg)
{
	if (phy->ssi_type != rx_cfg[idx].profile.rxSsiConfig.ssiType) {
		pr_err("SSI interface mismatch. PHY=%d, RX%d=%d\n",
		       phy->ssi_type, idx + 1, rx_cfg[idx].profile.rxSsiConfig.ssiType);
		return -EINVAL;
	}

	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS
	    && !rx_cfg[idx].profile.rxSsiConfig.ddrEn) {
		pr_err("RX%d: Single Data Rate port not supported for LVDS\n",
		       idx + 1);
		return -EINVAL;
	}

	if (rx_cfg[idx].profile.rxSsiConfig.strobeType ==
	    ADI_ADRV9001_SSI_LONG_STROBE) {
		pr_err("SSI interface Long Strobe not supported\n");
		return -EINVAL;
	}

	if (!phy->rx2tx2 || !idx)
		return 0;

	if (rx_cfg[idx].profile.rxOutputRate_Hz != phy->rx_channels[0].channel.rate) {
		pr_err("In rx2tx2, RX%d rate=%u must be equal to RX1, rate=%ld\n", idx + 1,
		       rx_cfg[idx].profile.rxOutputRate_Hz, phy->rx_channels[0].channel.rate);
		return -EINVAL;
	}

	if (!phy->rx_channels[0].channel.enabled) {
		pr_err("In rx2tx2, RX%d cannot be enabled while RX1 is disabled", idx + 1);
		return -EINVAL;
	}

	return 0;
}

static int adrv9002_tx_validate_profile(struct adrv9002_rf_phy *phy,
					unsigned int idx,
					const struct adi_adrv9001_TxProfile *tx_cfg)
{
	struct adrv9002_tx_chan *tx = &phy->tx_channels[idx];
	struct adrv9002_rx_chan *rx;

	/* check @tx_only comments in adrv9002.h to better understand the next checks */
	if (phy->ssi_type != tx_cfg[idx].txSsiConfig.ssiType) {
		pr_err("SSI interface mismatch. PHY=%d, TX%d=%d\n",
		       phy->ssi_type, idx + 1,  tx_cfg[idx].txSsiConfig.ssiType);
		return -EINVAL;
	}

	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS
	    && !tx_cfg[idx].txSsiConfig.ddrEn) {
		pr_err("TX%d: Single Data Rate port not supported for LVDS\n", idx + 1);
		return -EINVAL;
	}

	if (tx_cfg[idx].txSsiConfig.strobeType == ADI_ADRV9001_SSI_LONG_STROBE) {
		pr_err("SSI interface Long Strobe not supported\n");
		return -EINVAL;
	}

	if (phy->rx2tx2) {
		struct adrv9002_chan *rx1 = &phy->rx_channels[0].channel;
		struct adrv9002_chan *tx1 = &phy->tx_channels[0].channel;

		/*
		 * In rx2tx2 mode, if TX uses RX as the reference clock, we just need to
		 * validate against RX1 since in this mode RX2 cannot be enabled without RX1. The
		 * same goes for the rate that must be the same.
		 */
		if (tx->rx_ref_clk && !rx1->enabled) {
			/*
			 * pretty much means that in this case either all channels are
			 * disabled, which obviously does not make sense, or RX1 must
			 * be enabled...
			 */
			pr_err("In rx2tx2, TX%d cannot be enabled while RX1 is disabled",
			       idx + 1);
			return -EINVAL;
		}

		if (tx->rx_ref_clk  && tx_cfg[idx].txInputRate_Hz != rx1->rate) {
			/*
			 * pretty much means that in this case, all ports must have
			 * the same rate. We match against RX1 since RX2 can be disabled
			 * even if it does not make much sense to disable it in rx2tx2 mode
			 */
			pr_err("In rx2tx2, TX%d rate=%u must be equal to RX1, rate=%ld\n",
			       idx + 1, tx_cfg[idx].txInputRate_Hz, rx1->rate);
			return -EINVAL;
		}

		if (!tx->rx_ref_clk  && idx && tx_cfg[idx].txInputRate_Hz != tx1->rate) {
			pr_err("In rx2tx2, TX%d rate=%u must be equal to TX1, rate=%ld\n",
			       idx + 1, tx_cfg[idx].txInputRate_Hz, tx1->rate);
			return -EINVAL;
		}

		if (idx && !tx1->enabled) {
			pr_err("In rx2tx2, TX%d cannot be enabled while TX1 is disabled",
			       idx + 1);
			return -EINVAL;
		}

		return 0;
	}

	if (!tx->rx_ref_clk)
		return 0;

	/* Alright, RX clock is driving us... */
	rx = &phy->rx_channels[tx->rx_ref_clk - 1];
	if (!rx->channel.enabled) {
		pr_err("TX%d cannot be enabled while RX%d is disabled", idx + 1,
		       rx->channel.number);
		return -EINVAL;
	}

	if (tx_cfg[idx].txInputRate_Hz != rx->channel.rate) {
		pr_err("TX%d rate=%u must be equal to RX%d, rate=%ld\n", idx + 1,
		       tx_cfg[idx].txInputRate_Hz, rx->channel.number, rx->channel.rate);
		return -EINVAL;
	}

	return 0;
}

static void adrv9002_validate_device_clkout(struct adrv9002_rf_phy *phy,
		uint32_t devclk)
{
	unsigned long out_rate;

	/* validated internally by the API for disabled case */
	if (phy->dev_clkout_div == ADI_ADRV9001_DEVICECLOCKDIVISOR_BYPASS ||
	    phy->dev_clkout_div == ADI_ADRV9001_DEVICECLOCKDIVISOR_DISABLED)
		return;

	/*
	 * Ideally, this would be implemented with registering a clock provider for
	 * dev clkout. But given that there's no API to easily change the divider or
	 * to even get it and that ADI_ADRV9001_DEVICECLOCKDIVISOR_DISABLED pretty
	 * much makes the internal API to decide the divider to use, it would be
	 * cumbersome and far from ideal to implement this through CCF - we probably
	 * would have to not allow the DISABLED option and only have a fixed clock
	 * after profile load. Given all the limitations go the easy way. If there's
	 * enough motivation to implement this through CCF later on, we can propose
	 * some new internal APIs.
	 */
	out_rate = devclk >> phy->dev_clkout_div;
	if (out_rate < ADRV9002_DEV_CLKOUT_MIN || out_rate > ADRV9002_DEV_CLKOUT_MAX) {
		pr_debug("Invalid device output clk(%lu) not in [%lu %lu]\n",
			 out_rate, ADRV9002_DEV_CLKOUT_MIN, ADRV9002_DEV_CLKOUT_MAX);
		/*
		 * If we can't get a valid rate with the new devclk + divider, let's defer
		 * to the internal API to try and get a valid divider that puts us in the
		 * supported range. Not ideal if someone wants an exact output clocks but
		 * better than failing probe. A runtime parameter for a divider does not
		 * make sense either. Therefore, a workaround for those wanting to dynamically
		 * change the output clock (in an exact way) is to overwrite phy->dev_clkout_div
		 * in debugfs.
		 */
		phy->dev_clkout_div = ADI_ADRV9001_DEVICECLOCKDIVISOR_DISABLED;
	}
}

static int adrv9002_validate_device_clk(struct adrv9002_rf_phy *phy,
					const struct adi_adrv9001_ClockSettings *clk_ctrl)
{
	unsigned long rate;
	uint64_t new_rate;
	int ret;

	ret = no_os_clk_recalc_rate(phy->dev_clk, &rate);
	if (ret)
		return ret;
	if (rate == clk_ctrl->deviceClock_kHz * 1000)
		return 0;

	/*
	 * If they don't match let's try to set the desired ref clk. Furthermore, let's
	 * be strict about not rounding it. If someones specifies some clk in the
	 * profile, then we should be capable of getting exactly that exact rate.
	 *
	 * !NOTE: we may need some small hysteris though... but let's add one when and
	 * if we really need one.
	 */
	ret = no_os_clk_round_rate(phy->dev_clk, clk_ctrl->deviceClock_kHz * 1000,
				   &new_rate);
	if (new_rate < 0 || new_rate != clk_ctrl->deviceClock_kHz * 1000) {
		pr_err("Cannot set ref_clk to (%lu), got (%llu)\n",
		       clk_ctrl->deviceClock_kHz * 1000, new_rate);
		return ret < 0 ? new_rate : -EINVAL;
	}

	ret = no_os_clk_set_rate(phy->dev_clk, new_rate);
	if (ret)
		return ret;

	adrv9002_validate_device_clkout(phy, new_rate);

	return 0;
}

static int adrv9002_validate_profile(struct adrv9002_rf_phy *phy)
{
	const struct adi_adrv9001_RxChannelCfg *rx_cfg =
			phy->curr_profile->rx.rxChannelCfg;
	const struct adi_adrv9001_TxProfile *tx_cfg = phy->curr_profile->tx.txProfile;
	struct adi_adrv9001_ClockSettings *clks = &phy->curr_profile->clocks;
	unsigned long rx_mask = phy->curr_profile->rx.rxInitChannelMask;
	unsigned long tx_mask = phy->curr_profile->tx.txInitChannelMask;
	int i, lo, ret;

	ret = adrv9002_validate_device_clk(phy, clks);
	if (ret)
		return ret;

	for (i = 0; i < ADRV9002_CHANN_MAX; i++) {
		struct adrv9002_rx_chan *rx = &phy->rx_channels[i];

		/* rx validations */
		if (!no_os_test_bit(ADRV9002_RX_BIT_START + i, &rx_mask))
			continue;

		lo = adrv9002_ext_lo_validate(phy, i, false);
		if (lo < 0)
			return lo;

		ret = adrv9002_rx_validate_profile(phy, i, rx_cfg);
		if (ret)
			return ret;

		pr_debug("RX%d enabled\n", i + 1);
		rx->channel.power = true;
		rx->channel.enabled = true;
		rx->channel.nco_freq = 0;
		rx->channel.rate = rx_cfg[i].profile.rxOutputRate_Hz;
		if (lo < ADI_ADRV9001_LOSEL_LO2)
			rx->channel.ext_lo = &phy->ext_los[lo];
		rx->channel.lo = i ? clks->rx2LoSelect : clks->rx1LoSelect;
		rx->channel.lo_cals = ADI_ADRV9001_INIT_LO_RETUNE &
				      ~ADI_ADRV9001_INIT_CAL_TX_ALL;
	}

	for (i = 0; i < phy->chip->n_tx; i++) {
		struct adrv9002_tx_chan *tx = &phy->tx_channels[i];
		struct adrv9002_rx_chan *rx = &phy->rx_channels[i];

		if (!no_os_test_bit(ADRV9002_TX_BIT_START + i, &tx_mask))
			continue;

		lo = adrv9002_ext_lo_validate(phy, i, true);
		if (lo < 0)
			return lo;

		ret = adrv9002_tx_validate_profile(phy, i, tx_cfg);
		if (ret)
			return ret;

		pr_debug("TX%d enabled\n", i + 1);
		/* orx actually depends on whether or not TX is enabled and not RX */
		rx->orx_en = no_os_test_bit(ADRV9002_ORX_BIT_START + i, &rx_mask);
		if (rx->orx_en && phy->port_switch.enable) {
			pr_err("ORx%d and RX%d Port Switch cannot be both enabled\n",
			       i + 1, i + 1);
			return -EINVAL;
		}
		tx->channel.power = true;
		tx->channel.enabled = true;
		tx->channel.nco_freq = 0;
		tx->channel.rate = tx_cfg[i].txInputRate_Hz;
		tx->elb_en = no_os_test_bit(ADRV9002_ELB_BIT_START + i, &rx_mask);
		if (lo < ADI_ADRV9001_LOSEL_LO2)
			tx->channel.ext_lo = &phy->ext_los[lo];
		tx->channel.lo = i ? clks->tx2LoSelect : clks->tx1LoSelect;
		tx->channel.lo_cals = ADI_ADRV9001_INIT_LO_RETUNE &
				      ~ADI_ADRV9001_INIT_CAL_RX_ALL;
	}

	return 0;
}

static int adrv9002_power_mgmt_config(const struct adrv9002_rf_phy *phy)
{
	struct adi_adrv9001_PowerManagementSettings power_mgmt = {
		.ldoPowerSavingModes = {
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1, ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
			ADI_ADRV9001_LDO_POWER_SAVING_MODE_1
		}
	};

	return api_call(phy, adi_adrv9001_powermanagement_Configure, &power_mgmt);
}

static int adrv9002_digital_init(const struct adrv9002_rf_phy *phy)
{
	int spi_mode = ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252;
	int ret;
	uint8_t tx_mask = 0;
	int c;
	adi_adrv9001_RxChannelCfg_t *rx_cfg = phy->curr_profile->rx.rxChannelCfg;

	ret = adi_adrv9001_arm_AhbSpiBridge_Enable(phy->adrv9001);
	if (ret)
		return ret;

	/*
	 * If we find a custom stream, we will load that. Otherwise we will load the default one.
	 * Note that if we are in the middle of filling @phy->stream_buf with a new stream and we
	 * get somehow here, the default one will be used. Either way, after filling the stream, we
	 * __must__ write a new profile which will get us here again and we can then load then new
	 * stream.
	 */
	if (phy->stream_size == ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES)
		ret = api_call(phy, adi_adrv9001_Stream_Image_Write, 0, phy->stream_buf,
			       phy->stream_size, spi_mode);
	else
		ret = api_call(phy, adi_adrv9001_Utilities_StreamImage_Load,
			       "Navassa_Stream.bin", spi_mode);
	if (ret)
		return ret;

	/* program arm firmware */
	ret = api_call(phy, adi_adrv9001_Utilities_ArmImage_Load,
		       "Navassa_EvaluationFw.bin", spi_mode);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv9001_arm_Profile_Write, phy->curr_profile);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv9001_arm_PfirProfiles_Write, phy->curr_profile);
	if (ret)
		return ret;

	/* Load gain tables */
	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		const struct adrv9002_rx_chan *rx = &phy->rx_channels[c];
		const struct adrv9002_tx_chan *tx = &phy->tx_channels[c];
		adi_adrv9001_RxProfile_t *p = &rx_cfg[c].profile;
		adi_adrv9001_RxGainTableType_e t_type;
		const char *rx_table;

		if (p->gainTableType) {
			rx_table = "RxGainTable_GainCompensated.csv";
			t_type = ADI_ADRV9001_RX_GAIN_COMPENSATION_TABLE;
		} else {
			rx_table = "RxGainTable.csv";
			t_type = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE;
		}

		if (rx->orx_en || tx->channel.enabled) {
			ret = api_call(phy, adi_adrv9001_Utilities_RxGainTable_Load, ADI_ORX,
				       "ORxGainTable.csv", rx->channel.number, &p->lnaConfig,
				       t_type);
			if (ret)
				return ret;
		}

		if (tx->channel.enabled)
			tx_mask |= tx->channel.number;

		if (!rx->channel.enabled)
			continue;

		ret = api_call(phy, adi_adrv9001_Utilities_RxGainTable_Load, ADI_RX, rx_table,
			       rx->channel.number, &p->lnaConfig, t_type);
		if (ret)
			return ret;
	}

	if (tx_mask) {
		ret = api_call(phy, adi_adrv9001_Utilities_TxAttenTable_Load,
			       "TxAttenTable.csv", tx_mask);
		if (ret)
			return ret;
	}

	ret = adrv9002_power_mgmt_config(phy);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv9001_arm_Start);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv9001_arm_StartStatus_Check, 5000000);
}

static uint64_t adrv9002_get_init_carrier(const struct adrv9002_chan *c)
{
	const struct adrv9002_rf_phy *phy = chan_to_phy(c);
	uint64_t lo_freq, rate_scaled, rate;
	int ret;

	if (!c->ext_lo) {
		/* If no external LO, keep the same values as before */
		if (c->port == ADI_RX) {
			/*
			 * For RX, port switch needs to be taken into account. If in auto
			 * mode, the carrier needs to be in the given range.
			 */
			if (c->carrier)
				lo_freq = c->carrier;
			else
				lo_freq = 2300000000ULL;

			if (!phy->port_switch.enable || phy->port_switch.manualRxPortSwitch)
				return lo_freq;

			if (ADRV9002_PORT_SWITCH_IN_RANGE(&phy->port_switch, lo_freq))
				return lo_freq;

			pr_debug("RX%u LO(%llu) not in allowed range, Using(%llu)\n",
				 c->number, lo_freq, phy->port_switch.maxFreqPortA_Hz);
			/* just choose one valid value */
			return phy->port_switch.maxFreqPortA_Hz;
		}

		if (c->carrier)
			return c->carrier;

		return 2300000000ULL;
	}

	ret = no_os_clk_recalc_rate(c->ext_lo->clk, &rate);
	if (ret)
		return -EIO;
	rate_scaled = no_os_div_u64(rate * c->ext_lo->scale.div, c->ext_lo->scale.mult);
	lo_freq = NO_OS_DIV_ROUND_CLOSEST_ULL(rate_scaled, c->ext_lo->divider);
	/* if we have an external LO which does not fit in the port switch ranges just error out */
	if (!phy->port_switch.enable || phy->port_switch.manualRxPortSwitch ||
	    ADRV9002_PORT_SWITCH_IN_RANGE(&phy->port_switch, lo_freq))
		return lo_freq;

	pr_err("Port Switch enabled and RX%u LO(%llu) not in allowed range\n",
	       c->number, lo_freq);

	return -EINVAL;
}

static int adrv9002_ext_lna_set(const struct adrv9002_rf_phy *phy,
				struct adrv9002_rx_chan *rx)
{
	struct adi_adrv9001_RxChannelCfg *rx_cfg = phy->curr_profile->rx.rxChannelCfg;
	struct adi_adrv9001_RxProfile *p = &rx_cfg[rx->channel.idx].profile;
	int ret;

	if (!p->lnaConfig.externalLnaPresent)
		return 0;

	ret = api_call(phy, adi_adrv9001_Rx_ExternalLna_Configure, rx->channel.number,
		       &p->lnaConfig, p->gainTableType);
	if (ret)
		return ret;

	/* min gain index may have changed */
	rx->agc.minGainIndex =  p->lnaConfig.minGainIndex;
	/*
	 * Also make sure to update the AGC LNA settling delay. Otherwise we would overwrite it
	 * when configuring AGC.
	 */
	rx->agc.extLna.settlingDelay = p->lnaConfig.settlingDelay;
	return 0;
}

static int adrv9002_init_dpd(const struct adrv9002_rf_phy *phy,
			     const struct adrv9002_tx_chan *tx)
{
	if (!tx->elb_en || !tx->dpd_init || !tx->dpd_init->enable)
		return 0;

	return api_call(phy, adi_adrv9001_dpd_Initial_Configure, tx->channel.number,
			tx->dpd_init);
}

/*
 * All of these structures are taken from TES when exporting the default profile to C code. Consider
 * about having all of these configurable through devicetree.
 */
static int adrv9002_radio_init(struct adrv9002_rf_phy *phy)
{
	int ret;
	int chan;
	uint8_t channel_mask = (phy->curr_profile->tx.txInitChannelMask |
				phy->curr_profile->rx.rxInitChannelMask) & 0xFF;
	struct adi_adrv9001_PllLoopFilterCfg pll_loop_filter = {
		.effectiveLoopBandwidth_kHz = 0,
		.loopBandwidth_kHz = 300,
		.phaseMargin_degrees = 60,
		.powerScale = 5
	};
	struct adi_adrv9001_Carrier carrier = {0};

	ret = api_call(phy, adi_adrv9001_Radio_PllLoopFilter_Set,
		       ADI_ADRV9001_PLL_LO1, &pll_loop_filter);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv9001_Radio_PllLoopFilter_Set,
		       ADI_ADRV9001_PLL_LO2, &pll_loop_filter);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv9001_Radio_PllLoopFilter_Set,
		       ADI_ADRV9001_PLL_AUX, &pll_loop_filter);
	if (ret)
		return ret;

	// Values required so that the adi_adrv9001_Rx_PortSwitch_Configure passes
	// (these values are checked even if port switching is not enabled)
	phy->port_switch.minFreqPortA_Hz = 890000000;
	phy->port_switch.maxFreqPortA_Hz = 910000000;
	phy->port_switch.minFreqPortB_Hz = 1890000000;
	phy->port_switch.maxFreqPortB_Hz = 1910000000;

	// Required to make sure we can properly select port B for RX2 while RX1 is on A
	// or the other way around
	ret = api_call(phy, adi_adrv9001_Rx_PortSwitch_Configure, &phy->port_switch);
	if (ret)
		return ret;

	for (chan = 0; chan < NO_OS_ARRAY_SIZE(phy->channels); chan++) {
		struct adrv9002_chan *c = phy->channels[chan];
		struct adi_adrv9001_ChannelEnablementDelays en_delays;

		if (!c->enabled)
			continue;

		if (c->port == ADI_RX) {
			ret = adrv9002_ext_lna_set(phy, chan_to_rx(c));
			if (ret)
				return ret;
		}
		/*
		 * For some low rate profiles, the intermediate frequency is non 0.
		 * In these cases, forcing it 0, will cause a firmware error. Hence, we need to
		 * read what we have and make sure we just change the carrier frequency...
		 */
		ret = api_call(phy, adi_adrv9001_Radio_Carrier_Inspect, c->port,
			       c->number, &carrier);
		if (ret)
			return ret;

		carrier.carrierFrequency_Hz = adrv9002_get_init_carrier(c);
		ret = api_call(phy, adi_adrv9001_Radio_Carrier_Configure,
			       c->port, c->number, &carrier);
		if (ret)
			return ret;

		adrv9002_en_delays_ns_to_arm(phy, &c->en_delays_ns, &en_delays);

		ret = api_call(phy, adi_adrv9001_Radio_ChannelEnablementDelays_Configure,
			       c->port, c->number, &en_delays);
		if (ret)
			return ret;

		if (c->port == ADI_TX) {
			ret = adrv9002_init_dpd(phy, chan_to_tx(c));
			if (ret)
				return ret;
		}

		if (!phy->curr_profile->sysConfig.mcsMode)
			continue;

		ret = api_call(phy, adi_adrv9001_Mcs_ChannelMcsDelay_Set, c->port,
			       c->number, &c->mcs_delay);
		if (ret)
			return ret;
	}

	return api_call(phy, adi_adrv9001_arm_System_Program, channel_mask);
}

int adrv9002_setup(struct adrv9002_rf_phy *phy)
{
	int ret;
	adi_adrv9001_ChannelState_e init_state;
	uint8_t init_cals_error = 0;

	/* in TDD we cannot start with all ports enabled as RX/TX cannot be on at the same time */
	if (phy->curr_profile->sysConfig.duplexMode == ADI_ADRV9001_TDD_MODE)
		init_state = ADI_ADRV9001_CHANNEL_PRIMED;
	else
		init_state = ADI_ADRV9001_CHANNEL_RF_ENABLED;

	adi_common_ErrorClear(&phy->adrv9001->common);
	ret = api_call(phy, adi_adrv9001_HwOpen, adrv9002_spi_settings_get());
	if (ret)
		return ret;

	ret = adrv9002_validate_profile(phy);
	if (ret)
		return ret;

	adrv9002_compute_init_cals(phy);

	adrv9002_log_enable(&phy->adrv9001->common);

	ret = api_call(phy, adi_adrv9001_InitAnalog, phy->curr_profile,
		       phy->dev_clkout_div);
	if (ret)
		return ret;

	ret = adrv9002_digital_init(phy);
	if (ret)
		return ret;

	ret = adrv9002_radio_init(phy);
	if (ret)
		return ret;

	/* should be done before init calibrations */
	ret = adrv9002_tx_set_dac_full_scale(phy);
	if (ret)
		return ret;

	if (phy->curr_profile->sysConfig.fhModeOn) {
		ret = api_call(phy, adi_adrv9001_fh_Configure, &phy->fh);
		if (ret)
			return ret;
	}

	adrv9002_init_cals_set(phy, "auto");

	ret = api_call(phy, adi_adrv9001_cals_InitCals_Run, &phy->init_cals,
		       60000, &init_cals_error);
	if (ret)
		return ret;

	ret = adrv9002_dpd_ext_path_set(phy);
	if (ret)
		return ret;

	ret = adrv9001_rx_path_config(phy, init_state);
	if (ret)
		return ret;

	ret = adrv9002_tx_path_config(phy, init_state);
	if (ret)
		return ret;

	/* unmask IRQs */
	ret = api_call(phy, adi_adrv9001_gpio_GpIntMask_Set, ~ADRV9002_IRQ_MASK);
	if (ret)
		return ret;

	return adrv9002_dgpio_config(phy);
}

int adrv9002_intf_change_delay(const struct adrv9002_rf_phy *phy,
			       const int channel, uint8_t clk_delay,
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

adi_adrv9001_SsiTestModeData_e adrv9002_get_test_pattern(
	const struct adrv9002_rf_phy *phy,
	unsigned int chan, bool rx, bool stop)
{
	const struct adrv9002_chan *tx = &phy->tx_channels[chan].channel;

	if (stop)
		return ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL;
	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		return ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE;
	if (rx)
		return ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15;

	/*
	 * Some low rate profiles don't play well with prbs15. The reason is
	 * still unclear. We suspect that the chip error checker might have
	 * some time constrains and cannot reliable validate prbs15 full
	 * sequences in the test time. Using a shorter sequence fixes the
	 * problem...
	 *
	 * We use the same threshold as in the rx interface gain for narrow band.
	 */
	if (tx->rate < 1000000)
		return ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7;

	return ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15;
}

int adrv9002_check_tx_test_pattern(const struct adrv9002_rf_phy *phy,
				   const int chann)
{
	int ret;
	const struct adrv9002_chan *chan = &phy->tx_channels[chann].channel;
	struct adi_adrv9001_TxSsiTestModeCfg cfg = {0};
	struct adi_adrv9001_TxSsiTestModeStatus status = {0};
	adi_adrv9001_SsiDataFormat_e data_fmt = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA;

	cfg.testData = adrv9002_get_test_pattern(phy, chann, false, false);

	ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect,
		       chan->number, phy->ssi_type, data_fmt, &cfg, &status);
	if (ret)
		return ret;

	pr_debug("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u", chan->number,
		 status.dataError, status.fifoFull, status.fifoEmpty, status.strobeAlignError);

	/* only looking for data errors for now */
	if (status.dataError)
		return 1;

	if (!phy->rx2tx2)
		return 0;

	/* on rx2tx2 we will only get here on index 0 so the following is fine */
	chan = &phy->tx_channels[chann + 1].channel;
	if (!chan->enabled)
		return 0;

	memset(&status, 0, sizeof(status));
	ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect,
		       chan->number, phy->ssi_type, data_fmt, &cfg, &status);
	if (ret)
		return ret;

	pr_debug("[c%d]: d_e:%u, f_f:%u f_e:%u, s_e:%u", chan->number,
		 status.dataError, status.fifoFull, status.fifoEmpty, status.strobeAlignError);

	if (status.dataError)
		return 1;

	return 0;
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

		cfg.testData = adrv9002_get_test_pattern(phy, chann, false, stop);
		ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Configure,
			       chan->number, phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;

		if (!phy->rx2tx2)
			return 0;

		/* on rx2tx2 we will only get here on index 0 so the following is fine */
		chan = &phy->tx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Configure,
			       chan->number, phy->ssi_type, data_fmt, &cfg);
	} else {
		struct adi_adrv9001_RxSsiTestModeCfg cfg = {0};

		chan = &phy->rx_channels[chann].channel;

		cfg.testData = adrv9002_get_test_pattern(phy, chann, true, stop);
		ret = api_call(phy, adi_adrv9001_Ssi_Rx_TestMode_Configure,
			       chan->number, phy->ssi_type, data_fmt, &cfg);
		if (ret)
			return ret;

		if (!phy->rx2tx2)
			return 0;

		/* on rx2tx2 we will only get here on index 0 so the following is fine */
		chan = &phy->rx_channels[chann + 1].channel;
		if (!chan->enabled)
			return 0;

		ret = api_call(phy, adi_adrv9001_Ssi_Rx_TestMode_Configure,
			       chan->number, phy->ssi_type, data_fmt, &cfg);
	}

	return ret;
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
			pr_debug("RX: Got clk: %u, data: %u\n", clk_delay,
				 data_delay);
			delays.rxClkDelay[c->idx] = clk_delay;
			delays.rxIDataDelay[c->idx] = data_delay;
			delays.rxQDataDelay[c->idx] = data_delay;
			delays.rxStrobeDelay[c->idx] = data_delay;
		} else {
			pr_debug("TX: Got clk: %u, data: %u\n", clk_delay,
				 data_delay);
			delays.txClkDelay[c->idx] = clk_delay;
			delays.txIDataDelay[c->idx] = data_delay;
			delays.txQDataDelay[c->idx] = data_delay;
			delays.txStrobeDelay[c->idx] = data_delay;
		}
	}

	return api_call(phy, adi_adrv9001_Ssi_Delay_Configure, phy->ssi_type, &delays);

}

static void adrv9002_cleanup(struct adrv9002_rf_phy *phy)
{
	int i;

	for (i = 0; i < ADRV9002_CHANN_MAX; i++) {
		phy->rx_channels[i].orx_en = 0;
		/* make sure we have the ORx GPIO low */
		if (phy->rx_channels[i].orx_gpio)
			no_os_gpio_set_value(phy->rx_channels[i].orx_gpio, 0);
		phy->rx_channels[i].channel.enabled = 0;
		phy->rx_channels[i].channel.ext_lo = NULL;
		phy->rx_channels[i].channel.lo = 0;
		phy->rx_channels[i].channel.lo_cals = 0;
		phy->tx_channels[i].channel.enabled = 0;
		phy->tx_channels[i].channel.ext_lo = NULL;
		phy->tx_channels[i].channel.lo = 0;
		phy->tx_channels[i].channel.lo_cals = 0;
	}

	phy->profile_len = snprintf(phy->profile_buf, sizeof(phy->profile_buf),
				    "No profile loaded...\n");
	memset(&phy->adrv9001->devStateInfo, 0,
	       sizeof(phy->adrv9001->devStateInfo));

	/*
	 * By default, let's not run init cals for LO changes >= 100MHz to keep
	 * the same behavior as before (as doing it automatically is time consuming).
	 */
	phy->run_cals = false;
	phy->mcs_run = false;
}

static uint32_t adrv9002_get_arm_clk(const struct adrv9002_rf_phy *phy)
{
	struct adi_adrv9001_ClockSettings *clks = &phy->curr_profile->clocks;
	uint32_t sys_clk;

	/* HP clk PLL is 8.8GHz and LP is 4.4GHz */
	if (clks->clkPllVcoFreq_daHz == ADRV9002_HP_CLK_PLL_DAHZ)
		sys_clk = clks->clkPllVcoFreq_daHz / 48 * 10;
	else
		sys_clk = clks->clkPllVcoFreq_daHz / 24 * 10;

	return DIV_ROUND_CLOSEST(sys_clk, clks->armPowerSavingClkDiv);
}

void adrv9002_en_delays_ns_to_arm(const struct adrv9002_rf_phy *phy,
				  const struct adi_adrv9001_ChannelEnablementDelays *d_ns,
				  struct adi_adrv9001_ChannelEnablementDelays *d)
{
	uint32_t arm_clk = adrv9002_get_arm_clk(phy);

	d->fallToOffDelay = NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)arm_clk *
			    d_ns->fallToOffDelay, 1000000000);
	d->guardDelay = NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)arm_clk *
			d_ns->guardDelay, 1000000000);
	d->holdDelay = NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)arm_clk * d_ns->holdDelay,
			1000000000);
	d->riseToAnalogOnDelay = NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)arm_clk *
				 d_ns->riseToAnalogOnDelay,
				 1000000000);
	d->riseToOnDelay = NO_OS_DIV_ROUND_CLOSEST_ULL((uint64_t)arm_clk *
			   d_ns->riseToOnDelay, 1000000000);
}

void adrv9002_en_delays_arm_to_ns(const struct adrv9002_rf_phy *phy,
				  const struct adi_adrv9001_ChannelEnablementDelays *d,
				  struct adi_adrv9001_ChannelEnablementDelays *d_ns)
{
	uint32_t arm_clk = adrv9002_get_arm_clk(phy);

	d_ns->fallToOffDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(d->fallToOffDelay *
			       1000000000ULL, arm_clk);
	d_ns->guardDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(d->guardDelay * 1000000000ULL,
			   arm_clk);
	d_ns->holdDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(d->holdDelay * 1000000000ULL,
			  arm_clk);
	d_ns->riseToAnalogOnDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(
					    d->riseToAnalogOnDelay * 1000000000ULL,
					    arm_clk);
	d_ns->riseToOnDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(d->riseToOnDelay *
			      1000000000ULL, arm_clk);
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

static const char *const warm_boot[] = {
	"Disabled",
	"Enabled"
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
				    "TX1 DPD enable: %d\n"
				    "TX2 LO: %s\n"
				    "TX2 DPD enable: %d\n"
				    "RX1 Gain Table Type: %s\n"
				    "RX2 Gain Table Type: %s\n"
				    "RX Channel Mask: 0x%x\n"
				    "TX Channel Mask: 0x%x\n"
				    "Duplex Mode: %s\n"
				    "FH enable: %d\n"
				    "MCS mode: %s\n"
				    "WarmBoot: %s\n"
				    "SSI interface: %s\n", clks->deviceClock_kHz * 1000,
				    clks->clkPllVcoFreq_daHz * 10ULL, clks->armPowerSavingClkDiv,
				    lo_maps[clks->rx1LoSelect], lo_maps[clks->rx2LoSelect],
				    lo_maps[clks->tx1LoSelect],
				    phy->tx_channels[0].dpd_init && phy->tx_channels[0].elb_en,
				    lo_maps[clks->tx2LoSelect],
				    phy->tx_channels[1].dpd_init && phy->tx_channels[1].elb_en,
				    rx_gain_type[rx_cfg[ADRV9002_CHANN_1].profile.gainTableType],
				    rx_gain_type[rx_cfg[ADRV9002_CHANN_2].profile.gainTableType],
				    rx->rxInitChannelMask, tx->txInitChannelMask,
				    duplex[sys->duplexMode], sys->fhModeOn, mcs[sys->mcsMode],
				    warm_boot[sys->warmBootEnable], ssi[phy->ssi_type]);
}

/*
 * !\FIXME
 *
 * There's a very odd issue where the tx2 power is significantly higher than
 * tx1. The reason is far from being clear but it looks somehow to be related with
 * tuning and the SSI delays. Some workarounds tested were:
 *	issuing a sync (reg 0x44) on the DDS;
 *	re-enabling the DDS core.
 * Both options had to be done after tuning but they were only half fixing the issue.
 * Meaning that TX2 power decreased to a level closer to TX1 but still around 6dbs
 * higher. Hence, what seems to really fix the issue is to read the TX SSI status
 * on the device side and with testdata set to FIXED_PATTERN. Somehow that is making
 * hdl happy. Another thing that was noted was that doing this at every calibration
 * point (after configuring the delays), on TX2, lead to more reliable tuning results
 * (more noticeable on the LTE40 profile).
 *
 * Obviuosly, this is an awful workaround and we need to understand the root cause of
 * the issue and properly fix things. Hopefully this won't one those things where
 * "we fix it later" means never!
 *
 * Update: Now we do the fixup for all TX channels and for rx2tx2 mode.
 */
int adrv9002_tx_fixup(const struct adrv9002_rf_phy *phy, unsigned int chan)
{
	const struct adrv9002_chan *tx = &phy->tx_channels[chan].channel;
	struct  adi_adrv9001_TxSsiTestModeCfg ssi_cfg = {
		.testData = ADI_ADRV9001_SSI_TESTMODE_DATA_FIXED_PATTERN,
	};
	struct adi_adrv9001_TxSsiTestModeStatus dummy;

	return api_call(phy, adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect, tx->number,
			phy->ssi_type,
			ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, &ssi_cfg, &dummy);
}

int adrv9002_tx_fixup_all(const struct adrv9002_rf_phy *phy)
{
	int ret;
	uint32_t c;

	for (c = 0; c < phy->chip->n_tx; c++) {
		ret = adrv9002_tx_fixup(phy, c);
		if (ret)
			return ret;
	}

	return 0;
}

int adrv9002_init(struct adrv9002_rf_phy *phy,
		  struct adi_adrv9001_Init *profile)
{
	int ret, c;
	struct adrv9002_chan *chan;

	adrv9002_cleanup(phy);
	/*
	 * Disable all the cores as it might interfere with init calibrations
	 * and mux all ports to 50ohms (when aplicable).
	 */
	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		chan = phy->channels[c];

		/* nothing else to do if there's no TX2 */
		if (chan->port == ADI_TX && chan->idx >= phy->chip->n_tx)
			break;

		adrv9002_port_enable(phy, chan, false);
	}

	phy->curr_profile = profile;
	ret = adrv9002_setup(phy);
	if (ret) {
		/* try one more time */
		ret = adrv9002_setup(phy);
		if (ret)
			goto error;
	}

	adrv9002_set_clk_rates(phy);

	ret = adrv9002_ssi_configure(phy);
	if (ret)
		goto error;

	/* re-enable the cores and port muxes */
	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		chan = phy->channels[c];

		adrv9002_port_enable(phy, chan, true);
	}

	ret = adrv9002_intf_tuning(phy);
	if (ret) {
		pr_err("Interface tuning failed: %d\n", ret);
		goto error;
	}

	adrv9002_fill_profile_read(phy);

	return adrv9002_tx_fixup_all(phy);
error:
	/*
	 * Leave the device in a reset state in case of error. There's not much we can do if
	 * the API call fails, so we are just being verbose about it...
	 */
	api_call(phy, adi_adrv9001_HwOpen, adrv9002_spi_settings_get());
	adrv9002_cleanup(phy);

	return ret;
}

static int adrv9002_profile_load(struct adrv9002_rf_phy *phy)
{
	return adi_adrv9001_profileutil_Parse(phy->adrv9001, &phy->profile,
					      (char *)phy->profile_json, phy->profile_length);
}

static int adrv9002_init_cals_coeffs_name_get(struct adrv9002_rf_phy *phy)
{
	const char *init_cals;
	size_t copied = 0;

	/* Ignore if the prof	ile has no warmboot */
	if (!phy->profile.sysConfig.warmBootEnable)
		return 0;

	if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_CMOS)
		init_cals = phy->chip->cmos_cals;
	else
		init_cals = phy->chip->lvds_cals;


	copied = strlcpy(phy->warm_boot.coeffs_name, init_cals,
			 sizeof(phy->warm_boot.coeffs_name));

	return (copied >= sizeof(phy->warm_boot.coeffs_name)) ? -EIO : 0;
}

int adrv9002_post_init(struct adrv9002_rf_phy *phy)
{
	struct adi_common_ApiVersion api_version;
	struct adi_adrv9001_ArmVersion arm_version;
	struct adi_adrv9001_SiliconVersion silicon_version;
	struct adi_adrv9001_StreamVersion stream_version;
	int ret, c;

	/* register channels clocks */
	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		struct adrv9002_rx_chan *rx = &phy->rx_channels[c];
		struct adrv9002_chan *tx = &phy->tx_channels[c].channel;

		rx->channel.clk = adrv9002_clk_register(phy, clk_names[c], c);
		if (NO_OS_IS_ERR(rx->channel.clk))
			return NO_OS_PTR_ERR(rx->channel.clk);

		/*
		 * We do not support more TXs so break now. Note that in rx2tx2 mode, we
		 * set the clk pointer of a non existing channel but since it won't ever
		 * be used it's not a problem so let's keep the code simple in here.
		 */
		if (c >= phy->chip->n_tx)
			break;

		tx->clk = adrv9002_clk_register(phy, clk_names[c + TX1_SAMPL_CLK],
						c + TX1_SAMPL_CLK);
		if (NO_OS_IS_ERR(tx->clk))
			return NO_OS_PTR_ERR(tx->clk);

		rx->tdd_clk = adrv9002_clk_register(phy, clk_names[c + TDD1_INTF_CLK],
						    c + TDD1_INTF_CLK);
		if (NO_OS_IS_ERR(rx->tdd_clk))
			return NO_OS_PTR_ERR(rx->tdd_clk);

		if (phy->rx2tx2) {
			/* just point RX2/TX2 to RX1/TX1*/
			phy->rx_channels[c + 1].channel.clk = rx->channel.clk;
			phy->tx_channels[c + 1].channel.clk = tx->clk;
			break;
		}
	}

	ret = adrv9002_profile_load(phy);
	if (ret)
		return ret;

	/*
	 * Validate the output devclk for the default profile. Done once in here so that we don't
	 * have to do it everytime in adrv9002_validate_device_clk() even if the device clock did
	 * not changed between profiles.
	 */
	adrv9002_validate_device_clkout(phy,
					phy->profile.clocks.deviceClock_kHz * 1000);

	ret = adrv9002_init_cals_coeffs_name_get(phy);
	if (ret < 0)
		return ret;

	ret = adrv9002_init(phy, &phy->profile);
	if (ret)
		return ret;

	api_call(phy, adi_adrv9001_ApiVersion_Get, &api_version);
	api_call(phy, adi_adrv9001_arm_Version, &arm_version);
	api_call(phy, adi_adrv9001_SiliconVersion_Get, &silicon_version);
	api_call(phy, adi_adrv9001_Stream_Version, &stream_version);

	pr_info(
		"%s Rev %d.%d, Firmware %u.%u.%u,  Stream %u.%u.%u.%u,  API version: %u.%u.%u successfully initialized\n",
		"ADRV9002", silicon_version.major, silicon_version.minor,
		arm_version.majorVer, arm_version.minorVer, arm_version.maintVer,
		stream_version.majorVer, stream_version.minorVer,
		stream_version.maintVer, stream_version.buildVer, api_version.major,
		api_version.minor, api_version.patch);

	return 0;
}

int32_t adrv9002_dev_init(struct adrv9002_rf_phy *device,
			  const struct adrv9002_init_param *init_param)
{
	int c;

	device->chip = init_param->chip;
	device->dev_clk = init_param->dev_clk;

	device->rx2tx2 = device->chip->rx2tx2;

	/* initialize channel numbers and ports here since these will never change */
	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		device->rx_channels[c].channel.idx = c;
		device->rx_channels[c].channel.number = c + ADI_CHANNEL_1;
		device->rx_channels[c].channel.port = ADI_RX;
		device->channels[c * 2] = &device->rx_channels[c].channel;
		device->tx_channels[c].channel.idx = c;
		device->tx_channels[c].channel.number = c + ADI_CHANNEL_1;
		device->tx_channels[c].channel.port = ADI_TX;
		device->channels[c * 2 + 1] = &device->tx_channels[c].channel;
		/* Initialize AGC */
		memcpy(&device->rx_channels[c].agc, init_param->agcConfig_init_param,
		       sizeof(struct adi_adrv9001_GainControlCfg));
	}

	device->profile_json = init_param->profile;
	device->profile_length = init_param->profile_length;

	return 0;
}
