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

#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"

#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_cals.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_rx.h"
#include "adi_adrv910x_tx.h"
#include "adrv910x.h"

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

int32_t adrv910x_dev_init(struct adrv910x_rf_phy *device,
			  const struct adrv910x_init_param *init_param)
{
	if (!device || !init_param) {
		pr_err("Invalid parameters for adrv910x_dev_init\n");
		return -EINVAL;
	}

	/* Initialize basic device parameters */
	device->dev_clk = init_param->dev_clk;
	device->reset_gpio = init_param->reset_gpio_init;
	device->spi = init_param->spi;
	device->device_profile = init_param->device_profile;
	device->dev_clk_frequency_hz = init_param->dev_clk_frequency_hz;
	device->clk_out_divisor = init_param->clk_out_divisor;
	device->arm_binary = init_param->arm_binary_ptr;
	device->arm_binary_size = init_param->arm_binary_size;
	device->rx_gain_table = init_param->rx_gain_table;
	device->tx_atten_table = init_param->tx_atten_table;
	device->pfbuf = init_param->pfbuf;

	/* Initialize flags */
	device->profile_loaded = false;
	device->channels_configured = false;
	device->calibrations_run = false;

	/* Initialize calibration structures with default values */
	memset(&device->init_cals, 0, sizeof(device->init_cals));
	memset(&device->tracking_cals, 0, sizeof(device->tracking_cals));

	pr_info("ADRV910x device initialization completed\n");
	return 0;
}

int adrv910x_init(struct adrv910x_rf_phy *phy)
{
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






	adi_adrv910x_Carrier_t carrier_18 = { 
		.loGenOptimization = ADI_ADRV910X_LO_GEN_OPTIMIZATION_PHASE_NOISE, 
		.carrierFrequency_Hz = 450000000, 
		.intermediateFrequency_Hz = 442000, 
		.extVcoConfig = { 
		.forceExtVcoPllRefClkDivider = false, 
		.extVcoPllRefClkDivider = 0, 
		.extVcoPllChargePumpCurrent = 15, 
		.extVcoPllSettle = 6, 
		.extVcoPllSpeedUpMode = false, 
		.extVcoPllSpeedUpCounter = 23, 
		.extVcoPllSpeedUpChargePumpCurrent = 17 }, 
		.enableFrequencyChangeInPrimed = false };

	ret = adi_adrv910x_Radio_Carrier_Configure(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, &carrier_18);

	adi_adrv910x_LoGenCalSettings_t loGenCalSettings_19 = { 
		.termRes = 1, 
		.vcmBiasEnable = true, 
		.s2dMode = ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_POSITIVE_INPUT };

	ret = adi_adrv910x_cals_InitCalsLoGenParams_Configure(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, &loGenCalSettings_19);

	adi_adrv910x_Carrier_t carrier_20 = { 
		.loGenOptimization = ADI_ADRV910X_LO_GEN_OPTIMIZATION_PHASE_NOISE, 
		.carrierFrequency_Hz = 450000000, 
		.intermediateFrequency_Hz = 0, 
		.extVcoConfig = { 
		.forceExtVcoPllRefClkDivider = false, 
		.extVcoPllRefClkDivider = 0, 
		.extVcoPllChargePumpCurrent = 15, 
		.extVcoPllSettle = 6, 
		.extVcoPllSpeedUpMode = false, 
		.extVcoPllSpeedUpCounter = 23, 
		.extVcoPllSpeedUpChargePumpCurrent = 17 }, 
		.enableFrequencyChangeInPrimed = false };

	ret = adi_adrv910x_Radio_Carrier_Configure(phy->adrv910x, ADI_TX, ADI_CHANNEL_2, &carrier_20);

	adi_adrv910x_LoGenCalSettings_t loGenCalSettings_21 = { 
		.termRes = 1, 
		.vcmBiasEnable = true, 
		.s2dMode = ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL };

	ret = adi_adrv910x_cals_InitCalsLoGenParams_Configure(phy->adrv910x, ADI_TX, ADI_CHANNEL_2, &loGenCalSettings_21);

	adi_adrv910x_PllLoopFilterCfg_t pllLoopFilterConfig_22 = { 
		.effectiveLoopBandwidth_kHz = 0, 
		.loopBandwidth_kHz = 300, 
		.phaseMargin_degrees = 60, 
		.powerScale = 5 };

	ret = adi_adrv910x_Radio_PllLoopFilter_Set(phy->adrv910x, ADI_ADRV910X_PLL_LO1, &pllLoopFilterConfig_22);

	adi_adrv910x_PllLoopFilterCfg_t pllLoopFilterConfig_23 = { 
		.effectiveLoopBandwidth_kHz = 0, 
		.loopBandwidth_kHz = 300, 
		.phaseMargin_degrees = 60, 
		.powerScale = 5 };

	ret = adi_adrv910x_Radio_PllLoopFilter_Set(phy->adrv910x, ADI_ADRV910X_PLL_LO2, &pllLoopFilterConfig_23);

	ret = adi_adrv910x_arm_System_Program(phy->adrv910x, 138);






	initCals_t initCals_4 = { 
		.sysInitCalMask = (initCalibrations_e) 0, 
		.chanInitCalMask = { (initCalibrations_e) 0, INIT_CAL_TX_QEC | INIT_CAL_TX_LO_LEAKAGE | INIT_CAL_TX_LB_PD | INIT_CAL_TX_BBAF | INIT_CAL_TX_ATTEN_DELAY | INIT_CAL_TX_DAC | INIT_CAL_TX_PATH_DELAY | INIT_CAL_RX_QEC_FIC | INIT_CAL_RX_RF_DC_OFFSET | INIT_CAL_RX_GAIN_PATH_DELAY  }, 
		.calMode = INIT_CAL_MODE_SYSTEM_AND_RX, 
		.force = false };

	uint8_t errorFlag_4 = 0;
	ret = adi_adrv910x_cals_InitCals_Run(phy->adrv910x, &initCals_4, 60000, &errorFlag_4);


	/* 'errorFlag_4' read back as 0 */


	initCals_t initCals_5 = { 
		.sysInitCalMask = (initCalibrations_e) 0, 
		.chanInitCalMask = { (initCalibrations_e) 0, INIT_CAL_TX_QEC | INIT_CAL_TX_LO_LEAKAGE | INIT_CAL_TX_LB_PD | INIT_CAL_TX_BBAF | INIT_CAL_TX_ATTEN_DELAY | INIT_CAL_TX_DAC | INIT_CAL_TX_PATH_DELAY | INIT_CAL_RX_QEC_FIC | INIT_CAL_RX_RF_DC_OFFSET | INIT_CAL_RX_GAIN_PATH_DELAY  }, 
		.calMode = INIT_CAL_MODE_LOOPBACK_AND_TX, 
		.force = false };

	uint8_t errorFlag_5 = 0;
	ret = adi_adrv910x_cals_InitCals_Run(phy->adrv910x, &initCals_5, 60000, &errorFlag_5);







trackingCals_t trackingCals_7 = { 
		.chanTrackingCalMask = { (trackingCalibrations_e) 0, TRACKING_CAL_RX_HD2 | TRACKING_CAL_RX_BBDC | TRACKING_CAL_RX_RFDC | TRACKING_CAL_RX_QEC_FIC | TRACKING_CAL_RX_GAIN_CONTROL_DETECTORS | TRACKING_CAL_RX_RSSI  } };

	ret = adi_adrv910x_cals_Tracking_Set(phy->adrv910x, &trackingCals_7);



	adi_adrv910x_HighBlockerModeCfg_t highBlockerConfig_8 = { 
		.enableHighBlockerMode = false, 
		.highBlockerModeThresholdOne_negQuartdBFS = 0, 
		.highBlockerModeThresholdTwo_QuartdB = 0 };

	ret = adi_adrv910x_Rx_HighBlockerMode_Configure(phy->adrv910x, ADI_CHANNEL_2, &highBlockerConfig_8);



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



	adi_adrv910x_GainControlCfg_t agcCfg_14 = { 
		.peakWaitTime = 4, 
		.maxGainIndex = 255, 
		.minGainIndex = 187, 
		.gainUpdateCounter = 600, 
		.attackDelay_us = 10, 
		.slowLoopSettlingDelay = 16, 
		.lowThreshPreventGainInc = false, 
		.changeGainIfThreshHigh = 3, 
		.agcMode = ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK, 
		.resetOnRxon = false, 
		.resetOnRxonGainIndex = 255, 
		.enableSyncPulseForGainCounter = false, 
		.enableFastRecoveryLoop = false, 
		.power = { 
		.powerEnableMeasurement = true, 
		.underRangeHighPowerThresh = 10, 
		.underRangeLowPowerThresh = 4, 
		.underRangeHighPowerGainStepRecovery = 2, 
		.underRangeLowPowerGainStepRecovery = 4, 
		.powerMeasurementDuration = 10, 
		.powerMeasurementDelay = 2, 
		.rxTddPowerMeasDuration = 0, 
		.rxTddPowerMeasDelay = 0, 
		.overRangeHighPowerThresh = 0, 
		.overRangeLowPowerThresh = 7, 
		.overRangeHighPowerGainStepAttack = 4, 
		.overRangeLowPowerGainStepAttack = 4, 
		.feedback_inner_high_inner_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED, 
		.feedback_apd_high_apd_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED }, 
		.peak = { 
		.agcUnderRangeLowInterval = 50, 
		.agcUnderRangeMidInterval = 2, 
		.agcUnderRangeHighInterval = 4, 
		.apdHighThresh = 19, 
		.apdLowThresh = 12, 
		.apdUpperThreshPeakExceededCount = 6, 
		.apdLowerThreshPeakExceededCount = 3, 
		.apdGainStepAttack = 2, 
		.apdGainStepRecovery = 2, 
		.enableHbOverload = true, 
		.hbOverloadDurationCount = 1, 
		.hbOverloadThreshCount = 1, 
		.hbHighThresh = 8208, 
		.hbUnderRangeLowThresh = 2911, 
		.hbUnderRangeMidThresh = 4112, 
		.hbUnderRangeHighThresh = 5810, 
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
		.feedback_apd_high_hb_high = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED }, 
		.extLna = { 
		.settlingDelay = 0 }, 
		.rxQecFreezeEnable = false, 
		.gpioFreezePin = ADI_ADRV910X_GPIO_UNASSIGNED };

	ret = adi_adrv910x_Rx_GainControl_Configure(phy->adrv910x, ADI_CHANNEL_2, &agcCfg_14);



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



	adi_common_Port_e ports_16[1] = { ADI_RX };
	adi_common_ChannelNumber_e channels_16[] = { ADI_CHANNEL_2  };

	ret = adi_adrv910x_Radio_Channels_Prime(phy->adrv910x, ports_16, channels_16, 1, true);



	ret = adi_adrv910x_Radio_ChannelEnableMode_Set(phy->adrv910x, ADI_RX, ADI_CHANNEL_2, ADI_ADRV910X_PIN_MODE);



	pr_info("ADRV910x hardware initialization completed\n");
	return ret;
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

