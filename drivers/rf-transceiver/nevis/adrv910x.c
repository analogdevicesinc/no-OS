/***************************************************************************//**
 *   @file   adrv910x.c
 *   @brief  adrv910x driver source.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <string.h>

#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_cals.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_rx.h"
#include "adi_adrv910x_tx.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_ssi.h"
#include "adrv910x.h"

#define ADRV910X_RX_BIT_START		0	/* ADI_ADRV910X_RX1 */
#define ADRV910X_TX_BIT			2	/* ADI_ADRV910X_TX1 */
#define ADRV910X_TXNB_BIT		3	/* ADI_ADRV910X_TXNB */

#define ADRV910X_RX_GAIN_STEP_MDB	500
#define ADRV910X_RX_MAX_GAIN_MDB	\
	((ADI_ADRV910X_RX_GAIN_INDEX_MAX - ADI_ADRV910X_RX_GAIN_INDEX_MIN) * \
	 ADRV910X_RX_GAIN_STEP_MDB)

/* Gives the port position in an array of ADRV910X_CHAN_MAX + 1 */
#define ADRV910X_PORT_POS(chan)		((chan)->port * 2 + (chan)->idx)

/* Same defaults the Linux driver uses when there is no AGC devicetree node */
static const adi_adrv910x_GainControlCfg_t adrv910x_agc_defaults = {
	.peakWaitTime = 4,
	.maxGainIndex = ADI_ADRV910X_RX_GAIN_INDEX_MAX,
	.minGainIndex = ADI_ADRV910X_RX_GAIN_INDEX_MIN,
	.gainUpdateCounter = 11520,
	.attackDelay_us = 10,
	.slowLoopSettlingDelay = 16,
	.changeGainIfThreshHigh = 3,
	.agcMode = 1,
	.resetOnRxonGainIndex = ADI_ADRV910X_RX_GAIN_INDEX_MAX,
	.power = {
		.powerEnableMeasurement = true,
		.underRangeHighPowerThresh = 10,
		.underRangeLowPowerThresh = 4,
		.underRangeHighPowerGainStepRecovery = 2,
		.underRangeLowPowerGainStepRecovery = 4,
		.powerMeasurementDuration = 10,
		.powerMeasurementDelay = 2,
		.overRangeLowPowerThresh = 7,
		.overRangeHighPowerGainStepAttack = 4,
		.overRangeLowPowerGainStepAttack = 4,
		.feedback_inner_high_inner_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_apd_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
	},
	.peak = {
		.agcUnderRangeLowInterval = 50,
		.agcUnderRangeMidInterval = 2,
		.agcUnderRangeHighInterval = 4,
		.apdHighThresh = 21,
		.apdLowThresh = 12,
		.apdUpperThreshPeakExceededCount = 6,
		.apdLowerThreshPeakExceededCount = 3,
		.apdGainStepAttack = 2,
		.apdGainStepRecovery = 0,
		.enableHbOverload = true,
		.hbOverloadDurationCount = 1,
		.hbOverloadThreshCount = 1,
		.hbHighThresh = 13044,
		.hbUnderRangeLowThresh = 5826,
		.hbUnderRangeMidThresh = 8230,
		.hbUnderRangeHighThresh = 7335,
		.hbUpperThreshPeakExceededCount = 6,
		.hbUnderRangeHighThreshExceededCount = 3,
		.hbGainStepHighRecovery = 2,
		.hbGainStepLowRecovery = 6,
		.hbGainStepMidRecovery = 4,
		.hbGainStepAttack = 2,
		.hbOverloadPowerMode = 0,
		.hbUnderRangeMidThreshExceededCount = 3,
		.hbUnderRangeLowThreshExceededCount = 3,
		.feedback_apd_low_hb_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_hb_high = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
	},
	.gpioFreezePin = ADI_ADRV910X_GPIO_UNASSIGNED,
};

int __adrv910x_dev_err(const struct adrv910x_rf_phy *phy, const char *function,
		       const int line)
{
	int ret;

	printf("%s, %d: failed with \"%s\" (%d)\n", function, line,
	       phy->adrv910x->common.error.errormessage ?
	       phy->adrv910x->common.error.errormessage : "",
	       phy->adrv910x->common.error.errCode);

	switch (phy->adrv910x->common.error.errCode) {
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

	adi_common_ErrorClear(&phy->adrv910x->common);

	return ret;
}

/* Linux adrv9104_channel_to_state_cache(), without the enable GPIO handling */
int adrv910x_channel_to_state_cache(struct adrv910x_rf_phy *phy,
				    const struct adrv910x_chan *chan,
				    adi_adrv910x_ChannelState_e state,
				    adi_adrv910x_ChannelState_e *curr)
{
	adi_adrv910x_ChannelEnableMode_e pin_mode;
	adi_adrv910x_ChannelState_e curr_state;
	int ret, tries;

	/* We can only move one state at a time, so act on the current one */
	ret = api_call(phy, adi_adrv910x_Radio_Channel_State_Get, chan->port,
		       chan->number, &curr_state);
	if (ret)
		return ret;
	/* Report the current state even when there is nothing to do */
	if (curr)
		*curr = curr_state;
	if (state == curr_state)
		return 0;

	ret = api_call(phy, adi_adrv910x_Radio_ChannelEnableMode_Get,
		       chan->port, chan->number, &pin_mode);
	if (ret)
		return ret;

	/* In pin mode the enable pin, which we do not control, owns the state */
	if (pin_mode == ADI_ADRV910X_PIN_MODE) {
		pr_err("Can't change state! Pin mode enabled for p:%u c:%u\n",
		       chan->port, chan->number);
		return -EPERM;
	}

	switch (state) {
	case ADI_ADRV910X_CHANNEL_CALIBRATED:
		if (curr_state == ADI_ADRV910X_CHANNEL_RF_ENABLED) {
			ret = api_call(phy, adi_adrv910x_Radio_Channel_EnableRf,
				       chan->port, chan->number, false);
			if (ret)
				return ret;
		}

		return api_call(phy, adi_adrv910x_Radio_Channel_Prime,
				chan->port, chan->number, false);
	case ADI_ADRV910X_CHANNEL_PRIMED:
		/* Either we need to prime or to disable RF */
		if (curr_state == ADI_ADRV910X_CHANNEL_RF_ENABLED) {
			ret = api_call(phy, adi_adrv910x_Radio_Channel_EnableRf,
				       chan->port, chan->number, false);
			if (ret)
				return ret;
		}

		return api_call(phy, adi_adrv910x_Radio_Channel_Prime,
				chan->port, chan->number, true);
	case ADI_ADRV910X_CHANNEL_RF_ENABLED:
		if (curr_state == ADI_ADRV910X_CHANNEL_CALIBRATED) {
			ret = api_call(phy, adi_adrv910x_Radio_Channel_Prime,
				       chan->port, chan->number, true);
			if (ret)
				return ret;
		}

		/* At this point, we must be primed */
		ret = api_call(phy, adi_adrv910x_Radio_Channel_EnableRf,
			       chan->port, chan->number, true);
		if (ret)
			return ret;

		/* The transition takes some time, poll until we get there */
		for (tries = 0; tries < 10; tries++) {
			ret = api_call(phy, adi_adrv910x_Radio_Channel_State_Get,
				       chan->port, chan->number, &curr_state);
			if (ret)
				return ret;
			if (curr_state == state)
				return 0;

			no_os_mdelay(1);
		}

		return -ETIMEDOUT;
	default:
		return -EINVAL;
	}
}

int adrv910x_channel_to_state(struct adrv910x_rf_phy *phy,
			      const struct adrv910x_chan *chan,
			      adi_adrv910x_ChannelState_e state)
{
	return adrv910x_channel_to_state_cache(phy, chan, state, NULL);
}

/* Linux adrv9104_validate_rx_profile()/adrv9104_validate_tx_profile() */
static int adrv910x_validate_ssi(const ssiConfig_t *ssi, const char *name)
{
	if (ssi->ssiType == SSI_TYPE_LVDS && !ssi->ddrEn) {
		pr_err("%s: Single Data Rate port not supported for LVDS\n", name);
		return -EINVAL;
	}

	if (ssi->strobeType == SSI_LONG_STROBE) {
		pr_err("%s: SSI Long strobe not supported\n", name);
		return -EINVAL;
	}

	return 0;
}

/* Linux adrv9104_validate_profile(): find out which ports the profile enables */
static int adrv910x_validate_profile(struct adrv910x_rf_phy *phy)
{
	const deviceProfile_t *profile = &phy->device_profile->profilePS1;
	struct adrv910x_tx *tx = &phy->tx_channel;
	uint32_t chan_mask = profile->chanConfig;
	int ret, c;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		struct adrv910x_rx *rx = &phy->rx_channels[c];

		if (!(chan_mask & NO_OS_BIT(ADRV910X_RX_BIT_START + c)))
			continue;

		ret = adrv910x_validate_ssi(&profile->rxConfig[c].rxSsiConfig,
					    c ? "RX2" : "RX1");
		if (ret)
			return ret;

		rx->channel.enabled = true;
		rx->channel.rate = profile->rxConfig[c].rxOutputRate;
		rx->channel.lo = profile->rxConfig[c].LoSelect;
		phy->ssi_type = profile->rxConfig[c].rxSsiConfig.ssiType;
	}

	if (!(chan_mask & NO_OS_BIT(ADRV910X_TX_BIT)) &&
	    !(chan_mask & NO_OS_BIT(ADRV910X_TXNB_BIT)))
		return 0;

	/* The API treats the narrowband TX as channel 2 */
	tx->txnb = !!(chan_mask & NO_OS_BIT(ADRV910X_TXNB_BIT));
	tx->channel.idx = tx->txnb ? 1 : 0;
	tx->channel.number = tx->txnb ? ADI_CHANNEL_2 : ADI_CHANNEL_1;

	ret = adrv910x_validate_ssi(&profile->txConfig[tx->channel.idx].txSsiConfig,
				    "TX");
	if (ret)
		return ret;

	tx->channel.enabled = true;
	tx->channel.rate = profile->txConfig[tx->channel.idx].txInputRate;
	tx->channel.lo = profile->txConfig[tx->channel.idx].LoSelect;
	phy->ssi_type = profile->txConfig[tx->channel.idx].txSsiConfig.ssiType;

	return 0;
}

/* Linux adrv9104_carrier_set(): only change the frequency of the carrier */
static int adrv910x_carrier_set(struct adrv910x_rf_phy *phy,
				const struct adrv910x_chan *chan,
				uint64_t freq_hz, uint64_t *old_freq_hz)
{
	adi_adrv910x_Carrier_t carrier = { 0 };
	int ret;

	ret = api_call(phy, adi_adrv910x_Radio_Carrier_Inspect, chan->port,
		       chan->number, &carrier);
	if (ret)
		return ret;

	if (old_freq_hz)
		*old_freq_hz = carrier.carrierFrequency_Hz;

	carrier.carrierFrequency_Hz = freq_hz;

	return api_call(phy, adi_adrv910x_Radio_Carrier_Configure, chan->port,
			chan->number, &carrier);
}

/* Linux adrv9104_radio_init(), first half: carriers of the enabled ports */
static int adrv910x_radio_carriers_init(struct adrv910x_rf_phy *phy)
{
	uint64_t carrier_hz;
	int ret, c;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *chan = phy->channels[c];

		if (!chan->enabled)
			continue;

		if (chan->carrier_hz)
			carrier_hz = chan->carrier_hz;
		else
			carrier_hz = chan->port == ADI_TX ? 2450000000ULL :
				     2400000000ULL;

		ret = adrv910x_carrier_set(phy, chan, carrier_hz, NULL);
		if (ret)
			return ret;
	}

	return 0;
}

/* Linux adrv9104_radio_init(), second half: PLL loop filters and programming */
static int adrv910x_radio_pll_init(struct adrv910x_rf_phy *phy)
{
	adi_adrv910x_PllLoopFilterCfg_t pll_loop_filter = {
		.effectiveLoopBandwidth_kHz = 0,
		.loopBandwidth_kHz = 300,
		.phaseMargin_degrees = 60,
		.powerScale = 5
	};
	int ret;

	ret = api_call(phy, adi_adrv910x_Radio_PllLoopFilter_Set,
		       ADI_ADRV910X_PLL_LO1, &pll_loop_filter);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Radio_PllLoopFilter_Set,
		       ADI_ADRV910X_PLL_LO2, &pll_loop_filter);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_arm_System_Program,
			phy->device_profile->profilePS1.chanConfig);
}

/* Linux adrv9104_tx_set_dac_full_scale() */
static int adrv910x_tx_set_dac_full_scale(struct adrv910x_rf_phy *phy)
{
	const struct adrv910x_tx *tx = &phy->tx_channel;

	if (!tx->channel.enabled || !tx->dac_boost_en)
		return 0;

	return api_call(phy, adi_adrv910x_Tx_OutputPowerBoost_Set, true);
}

/* Init calibration masks, indexed by the enabled ports (same table as Linux) */
static const uint32_t adrv910x_init_cals_mask[16][2] = {
	/* Not a valid case. At least one channel should be enabled */
	[0] = { 0, 0 },
	/* txnb:0 rx2:0 tx1:0 rx1:1 */
	[1] = { 0x1bc400, 0 },
	/* txnb:0 rx2:0 tx1:1 rx1:0 */
	[2] = { 0x1bc5f7, 0 },
	/* txnb:0 rx2:0 tx1:1 rx1:1 */
	[3] = { 0x1bc5f7, 0 },
	/* txnb:0 rx2:1 tx1:0 rx1:0 */
	[4] = { 0, 0x1ac400 },
	/* txnb:0 rx2:1 tx1:0 rx1:1 */
	[5] = { 0x1bc400, 0x1ac400 },
	/* txnb:0 rx2:1 tx1:1 rx1:0 */
	[6] = { 0x1bc5f7, 0x1ac400 },
	/* txnb:0 rx2:1 tx1:1 rx1:1 */
	[7] = { 0x1bc5f7, 0x1ac400 },
	/* txnb:1 rx2:0 tx1:0 rx1:0 */
	[8] = { 0, 0x1ac5f7 },
	/* txnb:1 rx2:0 tx1:0 rx1:1 */
	[9] = { 0x1bc400, 0x1ac5f7 },
	/* txnb:1 rx2:0 tx1:1 rx1:0 */
	[10] = { 0x1bc5f7, 0x1ac5f7 },
	/* txnb:1 rx2:0 tx1:1 rx1:1 */
	[11] = { 0x1bc5f7, 0x1ac5f7 },
	/* txnb:1 rx2:1 tx1:0 rx1:0 */
	[12] = { 0, 0x1ac5f7 },
	/* txnb:1 rx2:1 tx1:0 rx1:1 */
	[13] = { 0x1bc400, 0x1ac5f7 },
	/* txnb:1 rx2:1 tx1:1 rx1:0 */
	[14] = { 0x1bc5f7, 0x1ac5f7 },
	/* txnb:1 rx2:1 tx1:1 rx1:1 */
	[15] = { 0x1bc5f7, 0x1ac5f7 },
};

/* Linux adrv9104_run_init_cals() */
static int adrv910x_run_init_cals(struct adrv910x_rf_phy *phy)
{
	initCals_t init_cals = { 0 };
	uint8_t errors = 0;
	uint32_t pos = 0;
	int ret, c;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *chan = phy->channels[c];

		if (chan->enabled)
			pos |= NO_OS_BIT(chan->idx * 2 + chan->port);
	}

	init_cals.chanInitCalMask[0] = adrv910x_init_cals_mask[pos][0];
	init_cals.chanInitCalMask[1] = adrv910x_init_cals_mask[pos][1];

	ret = api_call(phy, adi_adrv910x_cals_InitCals_Run, &init_cals, 60000,
		       &errors);
	if (ret)
		pr_err("Init calibrations failed: %d, error flag: 0x%02x\n", ret,
		       errors);

	return ret;
}

/* Linux adrv9104_rx_path_config() */
static int adrv910x_rx_path_config(struct adrv910x_rf_phy *phy,
				   adi_adrv910x_ChannelState_e state)
{
	int ret, c;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		struct adrv910x_rx *rx = &phy->rx_channels[c];

		if (!rx->channel.enabled)
			continue;

		ret = api_call(phy, adi_adrv910x_Rx_GainControl_Configure,
			       rx->channel.number, &rx->agc);
		if (ret)
			return ret;

		ret = adrv910x_channel_to_state(phy, &rx->channel, state);
		if (ret)
			return ret;
	}

	return 0;
}

/* Linux adrv9104_cleanup() */
static void adrv910x_cleanup(struct adrv910x_rf_phy *phy)
{
	int c;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		phy->channels[c]->enabled = false;
		phy->channels[c]->lo = MAX_NUM_LOS;
	}

	memset(&phy->adrv910x->devStateInfo, 0,
	       sizeof(phy->adrv910x->devStateInfo));
}

/* Linux adrv9104_rf_init() */
static int adrv910x_rf_init(struct adrv910x_rf_phy *phy)
{
	adi_adrv910x_ChannelState_e init_state;
	int ret = 0;


	if (!phy) {
		pr_err("Invalid phy parameter\n");
		return -EINVAL;
	}

	pr_info("Starting ADRV910x hardware initialization...\n");

	/* Clear any previous errors */
	adi_common_ErrorClear(&phy->adrv910x->common);

	/* Perform hardware open - this initializes SPI and basic hardware */
	ret = api_call(phy, adi_adrv910x_HwOpen, adrv910x_spi_settings_get());
	if (ret) {
		pr_err("Hardware open failed: %d\n", ret);
		return ret;
	}

	ret = adrv910x_validate_profile(phy);
	if (ret)
		return ret;

#ifdef ADI_COMMON_VERBOSE
	phy->adrv910x->common.error.logEnable = true;
#endif

	ret = adi_adrv910x_InitAnalog(phy->adrv910x, &phy->device_profile->profilePS1, ADI_ADRV910X_DEVICECLOCKDIVISOR_BYPASS);
	if (ret) {
			pr_err("init analogfailed: %d\n", ret);
			return ret;
	}

	ret = adi_adrv910x_arm_AhbSpiBridge_Enable(phy->adrv910x);
	if (ret) {
			pr_err("arm_AhbSpiBridge_Enable failed: %d\n", ret);
			return ret;
	}

	ret = adi_adrv910x_arm_Image_Write(phy->adrv910x, 0, (const uint8_t *)phy->arm_binary, phy->arm_binary_size, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
	if (ret) {
		pr_err("failed to write arm binary: %d\n", ret);
		pr_err("init, 146: failed with \"%s\" (%d)\n",
			       phy->adrv910x->common.error.errormessage ?
			       phy->adrv910x->common.error.errormessage : "",
			       phy->adrv910x->common.error.errCode);
		return ret;
	}

	ret = adi_adrv910x_arm_Profile_Write(phy->adrv910x, phy->device_profile, ADI_PS1);
	if (ret) {
		pr_err("failed to write profile to arm: %d\n", ret);
		return ret;
	}

	/* Write PFIR profiles */
	ret = adi_adrv910x_arm_PfirProfiles_Write(phy->adrv910x, &phy->pfbuf);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("PFIR profiles write failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Write RX gain tables (skip ORX channels, write only RX channels) */
	ret = adi_adrv910x_Rx_GainTable_Write(phy->adrv910x, ADI_RX, ADI_CHANNEL_1, 255, phy->rx_gain_table, 69, ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("RX channel 1 gain table write failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	ret = adi_adrv910x_Rx_GainTable_Write(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, 255, phy->rx_gain_table, 69, ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("RX channel 2 gain table write failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Write TX attenuation table */
	ret = adi_adrv910x_Tx_AttenuationTable_Write(phy->adrv910x, 138, 0, phy->tx_atten_table, 960);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("TX attenuation table write failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Start ARM processor */
	ret = adi_adrv910x_arm_Start(phy->adrv910x, ADI_PS1);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("ARM start failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Check ARM start status */
	ret = adi_adrv910x_arm_StartStatus_Check(phy->adrv910x, 5000000);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("ARM start status check failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Set TX attenuation */
	ret = adi_adrv910x_Tx_Attenuation_Set(phy->adrv910x, 150);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("TX attenuation set failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	/* Program ARM system */
	ret = adi_adrv910x_arm_System_Program(phy->adrv910x, 138);
	if (ret != ADI_COMMON_ERR_OK) {
		pr_err("ARM system program failed: %d\n", ret);
		return adrv910x_dev_err(phy);
	}

	ret = adrv910x_radio_carriers_init(phy);
	if (ret)
		return ret;

	adi_adrv910x_LoGenCalSettings_t loGenCalSettings_19 = { 
		.termRes = 1, 
		.vcmBiasEnable = true, 
		.s2dMode = ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_POSITIVE_INPUT };

	ret = adi_adrv910x_cals_InitCalsLoGenParams_Configure(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, &loGenCalSettings_19);

	adi_adrv910x_LoGenCalSettings_t loGenCalSettings_21 = { 
		.termRes = 1, 
		.vcmBiasEnable = true, 
		.s2dMode = ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL };

	ret = adi_adrv910x_cals_InitCalsLoGenParams_Configure(phy->adrv910x, ADI_TX, ADI_CHANNEL_2, &loGenCalSettings_21);

	ret = adrv910x_radio_pll_init(phy);
	if (ret)
		return ret;

	/* Must be called before init cals */
	ret = adrv910x_tx_set_dac_full_scale(phy);
	if (ret)
		return ret;

	ret = adrv910x_run_init_cals(phy);
	if (ret)
		return ret;

trackingCals_t trackingCals_7 = { 
		.chanTrackingCalMask = { (trackingCalibrations_e) 0, TRACKING_CAL_RX_HD2 | TRACKING_CAL_RX_BBDC | TRACKING_CAL_RX_RFDC | TRACKING_CAL_RX_QEC_FIC | TRACKING_CAL_RX_GAIN_CONTROL_DETECTORS | TRACKING_CAL_RX_RSSI  } };

	ret = adi_adrv910x_cals_Tracking_Set(phy->adrv910x, &trackingCals_7);



	adi_adrv910x_ImageIDCfg_t highBlockerConfig_8 = { 
		.enableHighBlockerMode = false, 
		.highBlockerModeThresholdOne_negQuartdBFS = 0, 
		.highBlockerModeThresholdTwo_QuartdB = 0 };

	ret = adi_adrv910x_Rx_ImageID_Configure(phy->adrv910x, ADI_CHANNEL_2, &highBlockerConfig_8);



	adi_adrv910x_RxrfdcLoidCfg_t loidConfig_9 = { 
		.loidEnable = true, 
		.loidDisableTrackingHighThreshold_negdBFS = 40, 
		.loidReenableTrackingLowThreshold_negdBFS = 60 };

	ret = adi_adrv910x_Rx_Loid_Configure(phy->adrv910x, ADI_CHANNEL_2, &loidConfig_9);



	ret = adi_adrv910x_Rx_Gain_Set(phy->adrv910x, ADI_CHANNEL_2, 255);



	ret = adi_adrv910x_Rx_GainControl_Mode_Set(phy->adrv910x, ADI_CHANNEL_2, ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI);



	adi_adrv910x_TxAttenuationConfig_t config_12 = { 
		.disableTxOnPllUnlock = false, 
		.txAttenStepSize = ADI_ADRV910X_TXATTEN_0P05_DB, 
		.attenMode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI };

	ret = adi_adrv910x_Tx_Attenuation_Configure(phy->adrv910x, &config_12);



	ret = adi_adrv910x_Tx_Attenuation_Set(phy->adrv910x, 10000);


	adi_adrv910x_RxInterfaceGainCtrl_t rxInterfaceGainConfig_15 = { 
		.updateInstance = ADI_ADRV910X_RX_INTERFACE_GAIN_UPDATE_TIMING_NOW, 
		.controlMode = ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_MANUAL, 
		.gain = ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB, 
		.rssiDuration = 255, 
		.rssiMovingAverageDuration = 10, 
		.gainControlAutomaticThreshold_dBFS = -2, 
		.signalPAR = 15, 
		.enableFastAttack = false };

	ret = adi_adrv910x_Rx_InterfaceGain_Configure(phy->adrv910x, ADI_CHANNEL_2, &rxInterfaceGainConfig_15);


	/* In TDD we cannot start with all ports enabled as RX/TX cannot be on at the same time */
	if (phy->device_profile->profilePS1.sysConfig.duplexMode == TDD)
		init_state = ADI_ADRV910X_CHANNEL_PRIMED;
	else
		init_state = ADI_ADRV910X_CHANNEL_RF_ENABLED;

	ret = adrv910x_rx_path_config(phy, init_state);
	if (ret)
		return ret;

	if (phy->tx_channel.channel.enabled) {
		ret = adrv910x_channel_to_state(phy, &phy->tx_channel.channel,
						init_state);
		if (ret)
			return ret;
	}


	ret = adi_adrv910x_Radio_ChannelEnableMode_Set(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, ADI_ADRV910X_PIN_MODE);



	pr_info("ADRV910x hardware initialization completed\n");
	return ret;
}

/* Linux __adrv9104_init() */
static int __adrv910x_init(struct adrv910x_rf_phy *phy)
{
	int ret;

	if (!phy->device_profile) {
		pr_err("No device profile\n");
		return -EINVAL;
	}

	adrv910x_cleanup(phy);

	ret = adrv910x_rf_init(phy);
	if (ret) {
		/* Leave the device in a reset state in case of error */
		api_call(phy, adi_adrv910x_HwReset);
		adrv910x_cleanup(phy);
	}

	return ret;
}

/*
 * Linux adrv9104_probe(). The init parameters take the place of the devicetree
 * and of the profile file, and are all copied to the phy before the cleanup.
 */
int adrv910x_init(struct adrv910x_rf_phy *phy,
		  const struct adrv910x_init_param *init_param)
{
	int c;

	if (!phy || !init_param) {
		pr_err("Invalid parameters for adrv910x_init\n");
		return -EINVAL;
	}

	/* Only done when the application did not set up the device itself */
	if (!phy->adrv910x) {
		phy->adrv910x = &phy->adrv910x_device;
		phy->adrv910x->common.devHalInfo = &phy->hal;
	}

	phy->spi = init_param->spi;
	phy->reset_gpio = init_param->reset_gpio_init;

	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		phy->channels[c] = &phy->rx_channels[c].channel;
		phy->rx_channels[c].channel.idx = c;
		phy->rx_channels[c].channel.port = ADI_RX;
		phy->rx_channels[c].channel.number = c + ADI_CHANNEL_1;
	}

	/*
	 * Index and number are not set as those depend on whether tx or txnb
	 * is enabled, see adrv910x_validate_profile().
	 */
	phy->channels[ADRV910X_TX] = &phy->tx_channel.channel;
	phy->tx_channel.channel.port = ADI_TX;

	phy->dev_clk = init_param->dev_clk;
	phy->dev_clk_frequency_hz = init_param->dev_clk_frequency_hz;
	phy->clk_out_divisor = init_param->clk_out_divisor;

	/* Linux adrv9104_profile_load() and the firmware files */
	phy->device_profile = init_param->device_profile;
	phy->pfbuf = init_param->pfbuf;
	phy->arm_binary = init_param->arm_binary_ptr;
	phy->arm_binary_size = init_param->arm_binary_size;
	phy->rx_gain_table = init_param->rx_gain_table;
	phy->tx_atten_table = init_param->tx_atten_table;

	/* Linux adrv9104_fw_parse() */
	for (c = 0; c < ADRV910X_RX_MAX; c++) {
		phy->rx_channels[c].channel.carrier_hz = init_param->rx_carrier_hz[c];
		phy->rx_channels[c].agc = init_param->rx_agc[c] ?
					  *init_param->rx_agc[c] : adrv910x_agc_defaults;
	}

	phy->tx_channel.channel.carrier_hz = init_param->tx_carrier_hz;
	phy->tx_channel.dac_boost_en = init_param->tx_dac_boost_en;
	phy->rerun_cals = init_param->rerun_cals_on_lo_retune;

	phy->profile_loaded = false;
	phy->channels_configured = false;
	phy->calibrations_run = false;
	memset(&phy->init_cals, 0, sizeof(phy->init_cals));
	memset(&phy->tracking_cals, 0, sizeof(phy->tracking_cals));

	return __adrv910x_init(phy);
}

/*
 * Linux adrv9104_init_with_profile(): re-initialize with a new profile. The
 * rest of the configuration is the one adrv910x_init() left in the phy.
 */
int adrv910x_init_with_profile(struct adrv910x_rf_phy *phy,
			       adi_adrv910x_Profiles_t *profile,
			       const pfirBuffer_t *pfbuf)
{
	if (!phy || !phy->adrv910x)
		return -EINVAL;

	if (profile)
		phy->device_profile = profile;
	if (pfbuf)
		phy->pfbuf = *pfbuf;

	return __adrv910x_init(phy);
}

/* Linux adrv9104_get_rf_chan_from_iio() */
static int adrv910x_chan_get(struct adrv910x_rf_phy *phy, uint32_t port,
			     struct adrv910x_chan **chan)
{
	if (!phy || port >= ADRV910X_CHAN_MAX)
		return -EINVAL;

	if (!phy->channels[port] || !phy->channels[port]->enabled)
		return -ENODEV;

	*chan = phy->channels[port];

	return 0;
}

/* Same as adrv910x_chan_get() but for attributes that only exist on RX */
static int adrv910x_rx_chan_get(struct adrv910x_rf_phy *phy, uint32_t port,
				struct adrv910x_chan **chan)
{
	if (port >= ADRV910X_RX_MAX)
		return -EINVAL;

	return adrv910x_chan_get(phy, port, chan);
}

/*
 * Linux adrv9104_hardware_gain_set(). RX: gain in mdB above the minimum gain
 * index. TX: attenuation as a negative gain in mdB.
 */
int adrv910x_hardware_gain_set(struct adrv910x_rf_phy *phy, uint32_t port,
			       int32_t gain_mdb)
{
	struct adrv910x_chan *chan;
	uint8_t idx;
	int ret;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	if (chan->port == ADI_TX) {
		if (gain_mdb > 0 || gain_mdb < -(int32_t)UINT16_MAX)
			return -EINVAL;

		return api_call(phy, adi_adrv910x_Tx_Attenuation_Set, -gain_mdb);
	}

	gain_mdb = no_os_clamp(gain_mdb, 0, ADRV910X_RX_MAX_GAIN_MDB);
	idx = NO_OS_DIV_ROUND_CLOSEST(gain_mdb, ADRV910X_RX_GAIN_STEP_MDB) +
	      ADI_ADRV910X_RX_GAIN_INDEX_MIN;

	return api_call(phy, adi_adrv910x_Rx_Gain_Set, chan->number, idx);
}

/* Linux adrv9104_hardware_gain_get() */
int adrv910x_hardware_gain_get(struct adrv910x_rf_phy *phy, uint32_t port,
			       int32_t *gain_mdb)
{
	struct adrv910x_chan *chan;
	uint16_t atten_mdb;
	uint8_t idx;
	int ret;

	if (!gain_mdb)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	if (chan->port == ADI_TX) {
		ret = api_call(phy, adi_adrv910x_Tx_Attenuation_Get, &atten_mdb);
		if (ret)
			return ret;

		*gain_mdb = -(int32_t)atten_mdb;

		return 0;
	}

	ret = api_call(phy, adi_adrv910x_Rx_Gain_Get, chan->number, &idx);
	if (ret)
		return ret;

	idx = no_os_clamp(idx, ADI_ADRV910X_RX_GAIN_INDEX_MIN,
			  ADI_ADRV910X_RX_GAIN_INDEX_MAX);
	*gain_mdb = (idx - ADI_ADRV910X_RX_GAIN_INDEX_MIN) *
		    ADRV910X_RX_GAIN_STEP_MDB;

	return 0;
}

/* Linux adrv9104_phy_read_sample_rate() */
int adrv910x_sampling_frequency_get(struct adrv910x_rf_phy *phy, uint32_t port,
				    uint32_t *rate_hz)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!rate_hz)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	*rate_hz = chan->rate;

	return 0;
}

/* Linux adrv9104_rerun_init_cals(), without the backend disable/enable */
static int adrv910x_rerun_init_cals(struct adrv910x_rf_phy *phy,
				    const struct adrv910x_chan *chan,
				    initCals_t *init_cals)
{
	adi_adrv910x_ChannelState_e state[ADRV910X_CHAN_MAX + 1];
	uint8_t error = 0;
	int ret, c;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *__chan = phy->channels[c];

		/* If on the same LO, it must be moved already */
		if (!__chan->enabled || __chan->lo == chan->lo)
			continue;

		ret = adrv910x_channel_to_state_cache(phy, __chan,
						      ADI_ADRV910X_CHANNEL_CALIBRATED,
						      &state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	ret = api_call(phy, adi_adrv910x_cals_InitCals_Run, init_cals, 6000,
		       &error);
	if (ret)
		return ret;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *__chan = phy->channels[c];

		if (!__chan->enabled || __chan->lo == chan->lo)
			continue;

		ret = adrv910x_channel_to_state(phy, __chan,
						state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Linux adrv9104_carrier_freq_set(). The device has two LOs, mapped to the
 * ports by the profile:
 *  - all the ports sharing the LO must be in the calibrated state for the
 *    PLL to re-tune;
 *  - RX ports sharing an LO are moved together, as they cannot run at
 *    different carriers. TX and RX are never moved together;
 *  - the port being changed is the last one to go back to its state, since
 *    in TDD the transition from calibrated to primed re-locks the PLL to the
 *    carrier of that port.
 */
int adrv910x_carrier_freq_set(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint64_t freq_hz)
{
	adi_adrv910x_ChannelState_e state[ADRV910X_CHAN_MAX + 1];
	struct adrv910x_chan *chan, *__chan;
	initCals_t init_cals = { 0 };
	uint64_t old_hz = freq_hz, diff_hz;
	int ret, c;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		__chan = phy->channels[c];
		if (!__chan->enabled || __chan->lo != chan->lo)
			continue;

		ret = adrv910x_channel_to_state_cache(phy, __chan,
						      ADI_ADRV910X_CHANNEL_CALIBRATED,
						      &state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	if (chan->port == ADI_TX) {
		ret = adrv910x_carrier_set(phy, chan, freq_hz, &old_hz);
		if (ret)
			return ret;

		/* Run on RX/TX */
		init_cals.chanInitCalMask[chan->idx] = INIT_LO_RETUNE;
	} else {
		/* Only run on RX */
		for (c = 0; c < ADRV910X_RX_MAX; c++) {
			__chan = &phy->rx_channels[c].channel;
			if (!__chan->enabled || __chan->lo != chan->lo)
				continue;

			ret = adrv910x_carrier_set(phy, __chan, freq_hz, &old_hz);
			if (ret)
				return ret;

			init_cals.chanInitCalMask[__chan->idx] |=
				INIT_LO_RETUNE & ~INIT_CAL_TX_ALL;
		}
	}

	diff_hz = freq_hz > old_hz ? freq_hz - old_hz : old_hz - freq_hz;
	if (diff_hz >= 100000000ULL && phy->rerun_cals) {
		ret = adrv910x_rerun_init_cals(phy, chan, &init_cals);
		if (ret)
			return ret;
	}

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		__chan = phy->channels[c];
		/* The port being changed goes last, see above */
		if (__chan == chan || !__chan->enabled || __chan->lo != chan->lo)
			continue;

		ret = adrv910x_channel_to_state(phy, __chan,
						state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	return adrv910x_channel_to_state(phy, chan,
					 state[ADRV910X_PORT_POS(chan)]);
}

/* Linux adrv9104_carrier_freq_get() */
int adrv910x_carrier_freq_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint64_t *freq_hz)
{
	adi_adrv910x_Carrier_t carrier = { 0 };
	struct adrv910x_chan *chan;
	int ret;

	if (!freq_hz)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Radio_Carrier_Inspect, chan->port,
		       chan->number, &carrier);
	if (ret)
		return ret;

	*freq_hz = carrier.carrierFrequency_Hz;

	return 0;
}

/* Linux adrv9104_rf_bandwidth_get() */
int adrv910x_rf_bandwidth_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint32_t *bw_hz)
{
	const deviceProfile_t *profile;
	struct adrv910x_chan *chan;
	int ret;

	if (!bw_hz)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	profile = &phy->device_profile->profilePS1;
	if (chan->port == ADI_RX)
		*bw_hz = profile->rxConfig[chan->idx].primaryBw_Hz;
	else
		*bw_hz = profile->txConfig[chan->idx].primaryBw_Hz;

	return 0;
}

/* Linux adrv9104_rssi_get() */
int adrv910x_rssi_get(struct adrv910x_rf_phy *phy, uint32_t port,
		      uint32_t *rssi_mdb)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!rssi_mdb)
		return -EINVAL;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Rx_Rssi_Read, chan->number, rssi_mdb);
}

/* Linux adrv9104_track_cals_get(). 'mask' is one or more TRACKING_CAL_RX_* */
int adrv910x_track_cal_get(struct adrv910x_rf_phy *phy, uint32_t port,
			   uint32_t mask, bool *enable)
{
	trackingCals_t track_cals;
	struct adrv910x_chan *chan;
	int ret;

	if (!enable)
		return -EINVAL;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_cals_Tracking_Get, &track_cals);
	if (ret)
		return ret;

	*enable = !!(mask & track_cals.chanTrackingCalMask[chan->idx]);

	return 0;
}

/*
 * Linux adrv9104_track_cals_set(): all the enabled ports must be in the
 * calibrated state to change the tracking calibrations.
 */
int adrv910x_track_cal_set(struct adrv910x_rf_phy *phy, uint32_t port,
			   uint32_t mask, bool enable)
{
	adi_adrv910x_ChannelState_e state[ADRV910X_CHAN_MAX + 1];
	trackingCals_t track_cals;
	struct adrv910x_chan *chan;
	int ret, c;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_cals_Tracking_Get, &track_cals);
	if (ret)
		return ret;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *__chan = phy->channels[c];

		if (!__chan->enabled)
			continue;

		ret = adrv910x_channel_to_state_cache(phy, __chan,
						      ADI_ADRV910X_CHANNEL_CALIBRATED,
						      &state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	if (enable)
		track_cals.chanTrackingCalMask[chan->idx] |= mask;
	else
		track_cals.chanTrackingCalMask[chan->idx] &= ~mask;

	ret = api_call(phy, adi_adrv910x_cals_Tracking_Set, &track_cals);
	if (ret)
		return ret;

	for (c = 0; c < ADRV910X_CHAN_MAX; c++) {
		const struct adrv910x_chan *__chan = phy->channels[c];

		if (!__chan->enabled)
			continue;

		ret = adrv910x_channel_to_state(phy, __chan,
						state[ADRV910X_PORT_POS(__chan)]);
		if (ret)
			return ret;
	}

	return 0;
}

/* Linux adrv9104_get_port_en_mode() */
int adrv910x_port_en_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      adi_adrv910x_ChannelEnableMode_e *mode)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!mode)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Radio_ChannelEnableMode_Get,
			chan->port, chan->number, mode);
}

/* Linux adrv9104_set_port_en_mode() */
int adrv910x_port_en_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
			      adi_adrv910x_ChannelEnableMode_e mode)
{
	struct adrv910x_chan *chan;
	int ret;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Radio_ChannelEnableMode_Set,
			chan->port, chan->number, mode);
}

/* Linux adrv9104_get_ensm_mode() */
int adrv910x_ensm_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
			   adi_adrv910x_ChannelState_e *state)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!state)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Radio_Channel_State_Get, chan->port,
			chan->number, state);
}

/* Linux adrv9104_set_ensm_mode(): calibrated, primed or rf_enabled */
int adrv910x_ensm_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
			   adi_adrv910x_ChannelState_e state)
{
	struct adrv910x_chan *chan;
	int ret;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return adrv910x_channel_to_state(phy, chan, state);
}

/* Linux adrv9104_get_agc_mode() */
int adrv910x_gain_control_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
				   adi_adrv910x_RxGainControlMode_e *mode)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!mode)
		return -EINVAL;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Rx_GainControl_Mode_Get, chan->number,
			mode);
}

/* Linux adrv9104_set_agc_mode() */
int adrv910x_gain_control_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
				   adi_adrv910x_RxGainControlMode_e mode)
{
	struct adrv910x_chan *chan;
	int ret;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Rx_GainControl_Mode_Set, chan->number,
			mode);
}

/* Linux adrv9104_get_digital_gain_ctl_mode() */
int adrv910x_digital_gain_control_mode_get(struct adrv910x_rf_phy *phy,
		uint32_t port, adi_adrv910x_RxInterfaceGainCtrlMode_e *mode)
{
	adi_adrv910x_RxInterfaceGainCtrl_t gain_ctrl = { 0 };
	adi_adrv910x_RxGainTableType_e dummy;
	struct adrv910x_chan *chan;
	int ret;

	if (!mode)
		return -EINVAL;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Rx_InterfaceGain_Inspect, chan->number,
		       &gain_ctrl, &dummy);
	if (ret)
		return ret;

	*mode = gain_ctrl.controlMode;

	return 0;
}

/*
 * Linux adrv9104_set_digital_gain_ctl_mode(): the port must be in the
 * calibrated state to configure the interface gain, so move it there and back.
 */
int adrv910x_digital_gain_control_mode_set(struct adrv910x_rf_phy *phy,
		uint32_t port, adi_adrv910x_RxInterfaceGainCtrlMode_e mode)
{
	adi_adrv910x_RxInterfaceGainCtrl_t gain_ctrl = { 0 };
	adi_adrv910x_RxGainTableType_e dummy;
	adi_adrv910x_ChannelState_e state;
	struct adrv910x_chan *chan;
	int ret;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Rx_InterfaceGain_Inspect, chan->number,
		       &gain_ctrl, &dummy);
	if (ret)
		return ret;

	gain_ctrl.controlMode = mode;

	ret = adrv910x_channel_to_state_cache(phy, chan,
					      ADI_ADRV910X_CHANNEL_CALIBRATED,
					      &state);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Rx_InterfaceGain_Configure,
		       chan->number, &gain_ctrl);
	if (ret)
		return ret;

	return adrv910x_channel_to_state(phy, chan, state);
}

/* Linux adrv9104_get_atten_control_mode() */
int adrv910x_atten_control_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
				    adi_adrv910x_TxAttenuationControlMode_e *mode)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!mode || port != ADRV910X_TX)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Tx_AttenuationMode_Get, mode);
}

/*
 * Linux adrv9104_set_atten_control_mode(): the port must be in the calibrated
 * state to change the attenuation mode, so move it there and back.
 */
int adrv910x_atten_control_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
				    adi_adrv910x_TxAttenuationControlMode_e mode)
{
	adi_adrv910x_ChannelState_e state;
	struct adrv910x_chan *chan;
	int ret;

	if (port != ADRV910X_TX)
		return -EINVAL;

	ret = adrv910x_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	ret = adrv910x_channel_to_state_cache(phy, chan,
					      ADI_ADRV910X_CHANNEL_CALIBRATED,
					      &state);
	if (ret)
		return ret;

	ret = api_call(phy, adi_adrv910x_Tx_AttenuationMode_Set, mode);
	if (ret)
		return ret;

	return adrv910x_channel_to_state(phy, chan, state);
}

/* Linux adrv9104_phy_reg_access() */
int adrv910x_reg_read(struct adrv910x_rf_phy *phy, uint16_t reg, uint8_t *val)
{
	if (!phy || !val)
		return -EINVAL;

	return api_call(phy, adi_adrv910x_spi_Byte_Read, reg, val);
}

int adrv910x_reg_write(struct adrv910x_rf_phy *phy, uint16_t reg, uint8_t val)
{
	if (!phy)
		return -EINVAL;

	return api_call(phy, adi_adrv910x_spi_Byte_Write, reg, val);
}

/* Linux debugfs "api_version" */
int adrv910x_api_version_get(struct adrv910x_rf_phy *phy,
			     adi_common_ApiVersion_t *version)
{
	if (!phy || !version)
		return -EINVAL;

	return api_call(phy, adi_adrv910x_apiVersion_get, version);
}

/* Linux debugfs "rx[1|2]_agc_config": AGC settings as read from the device */
int adrv910x_rx_agc_get(struct adrv910x_rf_phy *phy, uint32_t port,
			adi_adrv910x_GainControlCfg_t *agc)
{
	struct adrv910x_chan *chan;
	int ret;

	if (!agc)
		return -EINVAL;

	ret = adrv910x_rx_chan_get(phy, port, &chan);
	if (ret)
		return ret;

	return api_call(phy, adi_adrv910x_Rx_GainControl_Inspect, chan->number,
			agc);
}

/* Linux debugfs "ssi_delays" */
int adrv910x_ssi_delays_set(struct adrv910x_rf_phy *phy,
			    adi_adrv910x_SsiCalibrationCfg_t *delays)
{
	if (!phy || !delays)
		return -EINVAL;

	return api_call(phy, adi_adrv910x_Ssi_Delay_Configure, phy->ssi_type,
			delays);
}

int adrv910x_ssi_delays_get(struct adrv910x_rf_phy *phy,
			    adi_adrv910x_SsiCalibrationCfg_t *delays)
{
	if (!phy || !delays)
		return -EINVAL;

	return api_call(phy, adi_adrv910x_Ssi_Delay_Inspect, phy->ssi_type,
			delays);
}

int adrv910x_load_profile(struct adrv910x_rf_phy *phy, deviceProfile_t *profile)
{
	int ret = 0;

	// if (!phy || !profile) {
	// 	pr_err("Invalid parameters for profile loading\n");
	// 	return -EINVAL;
	// }

	// pr_info("Loading device profile...\n");

	// /* Store the profile reference */
	// phy->device_profile = profile;

	// /* Initialize analog components with the profile */
	// ret = api_call(phy, adi_adrv910x_InitAnalog, profile, phy->clk_out_divisor);
	// if (ret) {
	// 	pr_err("Analog initialization failed: %d\n", ret);
	// 	return ret;
	// }

	// phy->profile_loaded = true;
	// pr_info("Device profile loaded successfully\n");
	return ret;
}

int adrv910x_configure_channels(struct adrv910x_rf_phy *phy)
{
	int ret = 0;

	if (!phy) {
		pr_err("Invalid phy parameter\n");
		return -EINVAL;
	}

	if (!phy->profile_loaded) {
		pr_err("Profile must be loaded before configuring channels\n");
		return -EINVAL;
	}

	if (phy->channels_configured) {
		pr_info("Channels already configured, skipping...\n");
		return 0;
	}

	pr_info("Configuring channels...\n");

	/* Channel configuration would typically involve:
	 * - Setting channel states (STANDBY, CALIBRATED, PRIMED, RF_ENABLED)
	 * - Configuring RX/TX channel parameters
	 * - Setting up GPIO configurations for channel control
	 *
	 * For ADRV9104 (1Tx2Rx), we need to configure:
	 * - 1 TX channel
	 * - 2 RX channels (1 narrowband, 1 wideband)
	 */

	/* Set all channels to STANDBY state initially */
	/* This would be device-specific based on the profile */

	phy->channels_configured = true;
	pr_info("Channel configuration completed\n");
	return ret;
}

int adrv910x_run_init_calibrations(struct adrv910x_rf_phy *phy)
{
	int ret = 0;
	uint8_t error_flag = 0;

	if (!phy) {
		pr_err("Invalid phy parameter\n");
		return -EINVAL;
	}

	if (!phy->channels_configured) {
		pr_err("Channels must be configured before running calibrations\n");
		return -EINVAL;
	}

	if (phy->calibrations_run) {
		pr_info("Calibrations already run, skipping...\n");
		return 0;
	}

	pr_info("Building default initialization calibrations...\n");

	/* Build default init calibrations */
	ret = adi_adrv910x_cals_InitCalsBuildDefault(&phy->init_cals);
	if (ret) {
		pr_err("Failed to build default calibrations: %d\n", ret);
		return ret;
	}

	pr_info("Running initialization calibrations...\n");

	/* Run initialization calibrations with 10-second timeout */
	ret = api_call(phy, adi_adrv910x_cals_InitCals_Run, &phy->init_cals, 10000, &error_flag);
	if (ret) {
		pr_err("Initialization calibrations failed: %d, error flag: 0x%02x\n", ret, error_flag);
		return ret;
	}

	if (error_flag != 0) {
		pr_err("Initialization calibrations completed with errors: 0x%02x\n", error_flag);
		return -EFAULT;
	}

	phy->calibrations_run = true;
	pr_info("Initialization calibrations completed successfully\n");
	return ret;
}

int adrv910x_setup_tracking_calibrations(struct adrv910x_rf_phy *phy)
{
	int ret = 0;

	if (!phy) {
		pr_err("Invalid phy parameter\n");
		return -EINVAL;
	}

	if (!phy->calibrations_run) {
		pr_err("Init calibrations must be run before setting up tracking calibrations\n");
		return -EINVAL;
	}

	pr_info("Setting up tracking calibrations...\n");

	/* Configure default tracking calibrations
	 * For ADRV9104, typical tracking calibrations include:
	 * - RX QEC (Quadrature Error Correction)
	 * - TX QEC
	 * - LO Leakage Correction
	 * - RX RSSI calibration
	 * - TX DPD (Digital Predistortion) for narrowband
	 */

	/* Enable basic tracking calibrations - this would be customized
	 * based on the specific use case and requirements */

	ret = api_call(phy, adi_adrv910x_cals_Tracking_Set, &phy->tracking_cals);
	if (ret) {
		pr_err("Failed to set tracking calibrations: %d\n", ret);
		return ret;
	}

	pr_info("Tracking calibrations setup completed\n");
	return ret;
}

int adrv910x_setup(struct adrv910x_rf_phy *phy)
{
	int ret = 0;

	// if (!phy) {
	// 	pr_err("Invalid phy parameter\n");
	// 	return -EINVAL;
	// }

	// pr_info("Starting complete ADRV910x setup sequence...\n");

	// /* Step 1: Hardware initialization */
	// ret = adrv910x_init(phy);
	// if (ret) {
	// 	pr_err("Hardware initialization failed: %d\n", ret);
	// 	return ret;
	// }

	// /* Step 2: Load device profile */
	// if (phy->device_profile) {
	// 	ret = adrv910x_load_profile(phy, phy->device_profile);
	// 	if (ret) {
	// 		pr_err("Profile loading failed: %d\n", ret);
	// 		return ret;
	// 	}
	// } else {
	// 	pr_info("No device profile provided, skipping profile loading\n");
	// }

	// /* Step 3: Configure channels */
	// ret = adrv910x_configure_channels(phy);
	// if (ret) {
	// 	pr_err("Channel configuration failed: %d\n", ret);
	// 	return ret;
	// }

	// /* Step 4: Run initialization calibrations */
	// ret = adrv910x_run_init_calibrations(phy);
	// if (ret) {
	// 	pr_err("Initialization calibrations failed: %d\n", ret);
	// 	return ret;
	// }

	// /* Step 5: Setup tracking calibrations */
	// ret = adrv910x_setup_tracking_calibrations(phy);
	// if (ret) {
	// 	pr_err("Tracking calibrations setup failed: %d\n", ret);
	// 	return ret;
	// }

	// pr_info("ADRV910x setup sequence completed successfully\n");
	// pr_info("Device ready for operation:\n");
	// pr_info("  - Profile loaded: %s\n", phy->profile_loaded ? "Yes" : "No");
	// pr_info("  - Channels configured: %s\n", phy->channels_configured ? "Yes" : "No");
	// pr_info("  - Calibrations run: %s\n", phy->calibrations_run ? "Yes" : "No");

	return ret;
}

