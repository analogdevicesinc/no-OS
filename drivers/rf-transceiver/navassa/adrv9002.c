/***************************************************************************//**
 *   @file   adrv9002.c
 *   @brief  adrv9002 driver source.
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
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "adrv9002.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_auxadc.h"
#include "adi_adrv9001_auxadc_types.h"
#include "adi_adrv9001_bbdc.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_cals_types.h"
#include "adi_common_types.h"
#include "adi_adrv9001_auxdac.h"
#include "adi_adrv9001_auxdac_types.h"
#include "adi_adrv9001_fh.h"
#include "adi_adrv9001_fh_types.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_gpio_types.h"
#include "adi_adrv9001_orx.h"
#include "adi_adrv9001_powermanagement.h"
#include "adi_adrv9001_powermanagement_types.h"
#include "adi_adrv9001_profile_types.h"
#include "adi_adrv9001_profileutil.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_rx_gaincontrol.h"
#include "adi_adrv9001_rx_gaincontrol_types.h"
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
#include "adi_adrv9001_version.h"
#include "adi_common_error_types.h"

/* gpio0 starts at 1 in the API enum */
#define ADRV9002_DGPIO_MIN	(ADI_ADRV9001_GPIO_DIGITAL_00 - 1)
#define ADRV9002_DGPIO_MAX	(ADI_ADRV9001_GPIO_DIGITAL_15 - 1)

#define ALL_RX_CHANNEL_MASK	(ADI_ADRV9001_RX1 | ADI_ADRV9001_RX2 | \
				 ADI_ADRV9001_ORX1 | ADI_ADRV9001_ORX2)

#define ADRV9002_RX_EN(nr)	NO_OS_BIT(((nr) * 2) & 0x3)
#define ADRV9002_TX_EN(nr)	NO_OS_BIT(((nr) * 2 + 1) & 0x3)

#define ADRV9002_RX_MAX_GAIN_mdB	\
	((ADI_ADRV9001_RX_GAIN_INDEX_MAX - ADI_ADRV9001_RX_GAIN_INDEX_MIN) * ADRV9002_RX_GAIN_STEP_mDB)
#define ADRV9002_RX_GAIN_STEP_mDB	500

#define ADRV9002_STREAM_BINARY_SZ 	ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES
#define ADRV9002_HP_CLK_PLL_DAHZ	884736000
#define ADRV9002_NO_EXT_LO             0xff

/* Frequency hopping */
#define ADRV9002_FH_TABLE_COL_SZ	5

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

enum {
	ADRV9002_RX1_BIT_NR,
	ADRV9002_RX2_BIT_NR,
	ADRV9002_TX1_BIT_NR,
	ADRV9002_TX2_BIT_NR,
	ADRV9002_ORX1_BIT_NR,
	ADRV9002_ORX2_BIT_NR,
};

int __adrv9002_dev_err(const struct adrv9002_rf_phy *phy,
		       const char *function, const int line)
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

int adrv9002_channel_to_state(const struct adrv9002_rf_phy *phy,
			      struct adrv9002_chan *chann,
			      const adi_adrv9001_ChannelState_e state, const bool cache_state)
{
	int ret;
	adi_adrv9001_ChannelEnableMode_e mode;

	/* nothing to do */
	if (!chann->enabled)
		return 0;

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
	int ret = 0;
	unsigned int i;

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

static int adrv9002_tx_path_config(const struct adrv9002_rf_phy *phy,
				   const adi_adrv9001_ChannelState_e state)
{
	int i, ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->tx_channels); i++) {
		const struct adrv9002_tx_chan *tx = &phy->tx_channels[i];

		/* For each tx channel enabled */
		if (!tx->channel.enabled)
			continue;

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
	int pos = 0;
	unsigned int i;

	phy->init_cals.sysInitCalMask = 0;
	phy->init_cals.calMode = ADI_ADRV9001_INIT_CAL_MODE_ALL;

	for (i = 0; i < NO_OS_ARRAY_SIZE(phy->channels); i++) {
		struct adrv9002_chan *c = phy->channels[i];

		if (!c->enabled)
			continue;

		if (c->port == ADI_RX)
			pos |= ADRV9002_RX_EN(c->idx);
		else
			pos |= ADRV9002_TX_EN(c->idx);
	}

	phy->init_cals.chanInitCalMask[0] = adrv9002_init_cals_mask[pos][0];
	phy->init_cals.chanInitCalMask[1] = adrv9002_init_cals_mask[pos][1];

	pr_debug("pos: %u, Chan1:%X, Chan2:%X\n", pos,
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

static int adrv9002_validate_profile(struct adrv9002_rf_phy *phy)
{
	const struct adi_adrv9001_RxChannelCfg *rx_cfg =
			phy->curr_profile->rx.rxChannelCfg;
	const struct adi_adrv9001_TxProfile *tx_cfg = phy->curr_profile->tx.txProfile;
	uint32_t rx_mask = phy->curr_profile->rx.rxInitChannelMask;
	uint32_t tx_mask = phy->curr_profile->tx.txInitChannelMask;
	const uint32_t ports[ADRV9002_CHANN_MAX * 2 + ADI_ADRV9001_MAX_ORX_ONLY] = {
		ADRV9002_RX1_BIT_NR, ADRV9002_TX1_BIT_NR, ADRV9002_RX2_BIT_NR,
		ADRV9002_TX2_BIT_NR, ADRV9002_ORX1_BIT_NR, ADRV9002_ORX2_BIT_NR,
	};
	int i, lo;

	for (i = 0; i < ADRV9002_CHANN_MAX; i++) {
		struct adrv9002_chan *tx = &phy->tx_channels[i].channel;
		struct adrv9002_rx_chan *rx = &phy->rx_channels[i];

		/* rx validations */
		if (!no_os_test_bit(ports[i * 2], &rx_mask))
			goto tx;

		lo = adrv9002_ext_lo_validate(phy, i, false);
		if (lo < 0)
			return lo;

		if (phy->rx2tx2 && i &&
		    rx_cfg[i].profile.rxOutputRate_Hz != phy->rx_channels[0].channel.rate) {
			printf("In rx2tx2, RX%d rate=%u must be equal to RX1, rate=%ld\n",
			       i + 1, rx_cfg[i].profile.rxOutputRate_Hz,
			       phy->rx_channels[0].channel.rate);
			return -EINVAL;
		} else if (phy->rx2tx2 && i && !phy->rx_channels[0].channel.enabled) {
			printf("In rx2tx2, RX%d cannot be enabled while RX1 is disabled",
			       i + 1);
			return -EINVAL;
		} else if (phy->ssi_type != rx_cfg[i].profile.rxSsiConfig.ssiType) {
			printf("SSI interface mismatch. PHY=%d, RX%d=%d\n",
			       phy->ssi_type, i + 1, rx_cfg[i].profile.rxSsiConfig.ssiType);
			return -EINVAL;
		} else if (rx_cfg[i].profile.rxSsiConfig.strobeType ==
			   ADI_ADRV9001_SSI_LONG_STROBE) {
			printf("SSI interface Long Strobe not supported\n");
			return -EINVAL;
		} else if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS &&
			   !rx_cfg[i].profile.rxSsiConfig.ddrEn) {
			printf("RX%d: Single Data Rate port not supported for LVDS\n",
			       i + 1);
			return -EINVAL;
		}

		pr_debug("RX%d enabled\n", i + 1);
		rx->channel.power = true;
		rx->channel.enabled = true;
		rx->channel.nco_freq = 0;
		rx->channel.rate = rx_cfg[i].profile.rxOutputRate_Hz;
		if (lo < ADI_ADRV9001_LOSEL_LO2)
			rx->channel.ext_lo = &phy->ext_los[lo];
tx:
		/* tx validations*/
		if (!no_os_test_bit(ports[i * 2 + 1], &tx_mask))
			continue;

		if (i >= phy->chip->n_tx) {
			pr_err("TX%d not supported for this device\n", i + 1);
			return -EINVAL;
		}

		lo = adrv9002_ext_lo_validate(phy, i, true);
		if (lo < 0)
			return lo;

		/* check @tx_only comments in adrv9002.h to better understand the next checks */
		if (phy->ssi_type != tx_cfg[i].txSsiConfig.ssiType) {
			printf("SSI interface mismatch. PHY=%d, TX%d=%d\n",
			       phy->ssi_type, i + 1,  tx_cfg[i].txSsiConfig.ssiType);
			return -EINVAL;
		} else if (tx_cfg[i].txSsiConfig.strobeType == ADI_ADRV9001_SSI_LONG_STROBE) {
			printf("SSI interface Long Strobe not supported\n");
			return -EINVAL;
		} else if (phy->ssi_type == ADI_ADRV9001_SSI_TYPE_LVDS &&
			   !tx_cfg[i].txSsiConfig.ddrEn) {
			printf("TX%d: Single Data Rate port not supported for LVDS\n",
			       i + 1);
			return -EINVAL;
		} else if (phy->rx2tx2) {
			if (!phy->tx_only && !phy->rx_channels[0].channel.enabled) {
				/*
				 * pretty much means that in this case either all channels are
				 * disabled, which obviously does not make sense, or RX1 must
				 * be enabled...
				 */
				printf("In rx2tx2, TX%d cannot be enabled while RX1 is disabled",
				       i + 1);
				return -EINVAL;
			} else if (i && !phy->tx_channels[0].channel.enabled) {
				printf("In rx2tx2, TX%d cannot be enabled while TX1 is disabled",
				       i + 1);
				return -EINVAL;
			} else if (!phy->tx_only &&
				   tx_cfg[i].txInputRate_Hz != phy->rx_channels[0].channel.rate) {
				/*
				 * pretty much means that in this case, all ports must have
				 * the same rate. We match against RX1 since RX2 can be disabled
				 * even if it does not make much sense to disable it in rx2tx2 mode
				 */
				printf("In rx2tx2, TX%d rate=%u must be equal to RX1, rate=%ld\n",
				       i + 1, tx_cfg[i].txInputRate_Hz,
				       phy->rx_channels[0].channel.rate);
				return -EINVAL;
			} else if (phy->tx_only && i &&
				   tx_cfg[i].txInputRate_Hz != phy->tx_channels[0].channel.rate) {
				printf("In rx2tx2, TX%d rate=%u must be equal to TX1, rate=%ld\n",
				       i + 1, tx_cfg[i].txInputRate_Hz,
				       phy->tx_channels[0].channel.rate);
				return -EINVAL;
			}
		} else if (!phy->tx_only && !rx->channel.enabled) {
			printf("TX%d cannot be enabled while RX%d is disabled",
			       i + 1, i + 1);
			return -EINVAL;
		} else if (!phy->tx_only && tx_cfg[i].txInputRate_Hz != rx->channel.rate) {
			printf("TX%d rate=%u must be equal to RX%d, rate=%ld\n",
			       i + 1, tx_cfg[i].txInputRate_Hz, i + 1, rx->channel.rate);
			return -EINVAL;
		}

		pr_debug("TX%d enabled\n", i + 1);
		/* orx actually depends on whether or not TX is enabled and not RX */
		rx->orx_en = no_os_test_bit(ports[i + ADRV9002_CHANN_MAX * 2], &rx_mask);
		tx->power = true;
		tx->enabled = true;
		tx->nco_freq = 0;
		tx->rate = tx_cfg[i].txInputRate_Hz;
		if (lo < ADI_ADRV9001_LOSEL_LO2)
			tx->ext_lo = &phy->ext_los[lo];
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
	uint64_t lo_freq;
	uint64_t clk_rate, ret;

	if (!c->ext_lo) {
		/* If no external LO, keep the same values as before */
		if (c->port == ADI_RX)
			return 2400000000ULL;

		return 2450000000ULL;
	}

	ret = no_os_clk_recalc_rate(c->ext_lo->clk->clk_desc, &lo_freq);
	if (ret)
		pr_debug("Error getting lo frequency.\n");
	return lo_freq;
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

	return NO_OS_DIV_ROUND_CLOSEST(sys_clk, clks->armPowerSavingClkDiv);
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
				 d_ns->riseToAnalogOnDelay, 1000000000);
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
					    d->riseToAnalogOnDelay * 1000000000ULL, arm_clk);
	d_ns->riseToOnDelay = NO_OS_DIV_ROUND_CLOSEST_ULL(d->riseToOnDelay *
			      1000000000ULL, arm_clk);
}

/*
 * All of these structures are taken from TES when exporting the default profile to C code. Consider
 * about having all of these configurable through devicetree.
 */
static int adrv9002_radio_init(const struct adrv9002_rf_phy *phy)
{
	int ret;
	unsigned int chan;
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

	for (chan = 0; chan < NO_OS_ARRAY_SIZE(phy->channels); chan++) {
		const struct adrv9002_chan *c = phy->channels[chan];
		struct adi_adrv9001_ChannelEnablementDelays en_delays;

		if (!c->enabled)
			continue;

		/*
		 * For some low rate profiles, the intermediate frequency is non 0.
		 * In these cases, forcing it 0, will cause a firmware error. Hence, we need to
		 * read what we have and make sure we just change the carrier frequency...
		 */
		ret = api_call(phy, adi_adrv9001_Radio_Carrier_Inspect, c->port, c->number,
			       &carrier);
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
	}

	return api_call(phy, adi_adrv9001_arm_System_Program, channel_mask);
}

int adrv9002_setup(struct adrv9002_rf_phy *phy)
{
	int ret;
	unsigned int c;
	uint8_t init_cals_error = 0;
	adi_adrv9001_ChannelState_e init_state;
	struct adrv9002_chan *chan;

	/* initialize channel numbers and ports here since these will never change */
	for (c = 0; c < ADRV9002_CHANN_MAX; c++) {
		phy->rx_channels[c].channel.idx = c;
		phy->rx_channels[c].channel.number = c + ADI_CHANNEL_1;
		phy->rx_channels[c].channel.port = ADI_RX;
		phy->channels[c * 2] = &phy->rx_channels[c].channel;
		phy->tx_channels[c].channel.idx = c;
		phy->tx_channels[c].channel.number = c + ADI_CHANNEL_1;
		phy->tx_channels[c].channel.port = ADI_TX;
		phy->channels[c * 2 + 1] = &phy->tx_channels[c].channel;
	}

	/*
	 * Disable all the cores as it might interfere with init calibrations.
	 */
	for (c = 0; c < NO_OS_ARRAY_SIZE(phy->channels); c++) {
		chan = phy->channels[c];

		if (phy->rx2tx2 && chan->idx > ADRV9002_CHANN_1)
			break;

		/* nothing else to do if there's no TX2 */
		if (chan->port == ADI_TX && chan->idx >= phy->chip->n_tx)
			break;

		adrv9002_axi_interface_enable(phy, chan->idx, chan->port == ADI_TX, false);
	}

	/* in TDD we cannot start with all ports enabled as RX/TX cannot be on at the same time */
	if (phy->curr_profile->sysConfig.duplexMode == ADI_ADRV9001_TDD_MODE)
		init_state = ADI_ADRV9001_CHANNEL_PRIMED;
	else
		init_state = ADI_ADRV9001_CHANNEL_RF_ENABLED;

	phy->adrv9001 = &phy->adrv9001_device;
	phy->adrv9001->common.devHalInfo = &phy->hal;

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
		       ADI_ADRV9001_DEVICECLOCKDIVISOR_2);
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

	ret = api_call(phy, adi_adrv9001_cals_InitCals_Run, &phy->init_cals,
		       60000, &init_cals_error);
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
