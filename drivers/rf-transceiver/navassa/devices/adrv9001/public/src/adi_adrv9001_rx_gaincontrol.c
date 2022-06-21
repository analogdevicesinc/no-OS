/**
* \file
* \brief Contains AGC features related function implementation defined in
* adi_adrv9001_agc.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_rx_gaincontrol.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_rx.h"

#include "adrv9001_bf.h"
#include "adi_adrv9001_gpio.h"
#include "adrv9001_init.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_radio.h"

/* Forward declaration */
static __maybe_unused int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set_Validate(adi_adrv9001_Device_t *device,
                                                                        adi_common_ChannelNumber_e channel,
                                                                        uint8_t minGainIndex,
                                                                        uint8_t maxGainIndex);

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_Mode_Set_Validate(adi_adrv9001_Device_t *device,
                                        adi_common_ChannelNumber_e channel,
                                        adi_adrv9001_RxGainControlMode_e gainCtrlMode)
{
    ADI_ENTRY_EXPECT(device);
    ADI_RANGE_CHECK(device, gainCtrlMode, ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI, ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_Mode_Set(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_RxGainControlMode_e gainCtrlMode)
{
    static const adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;
    uint8_t bfVal = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_Mode_Set_Validate, device, channel, gainCtrlMode);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI ||
        gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN)
    {
        bfVal = 0;
    }
    else
    {
        bfVal = 2;
    }
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSetup_Set, device, instance, bfVal);

    if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, instance, true);
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainPinControl_Set, device, instance, false);
    }
    else if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, instance, true);
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainPinControl_Set, device, instance, true);
    }
    else if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, instance, false);
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainPinControl_Set, device, instance, false);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_Mode_Get_Validate(adi_adrv9001_Device_t *device,
                                        adi_common_ChannelNumber_e channel,
                                        adi_adrv9001_RxGainControlMode_e *gainCtrlMode)
{
    ADI_ENTRY_PTR_EXPECT(device, gainCtrlMode);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_Mode_Get(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_RxGainControlMode_e *gainCtrlMode)
{
    static const adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;
    uint8_t agcSetup = 0;
    uint8_t pinControl = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_Mode_Get_Validate, device, channel, gainCtrlMode);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSetup_Get, device, instance, &agcSetup);
    if (ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO == agcSetup)
    {
        *gainCtrlMode = ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO;
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainPinControl_Get, device, instance, &pinControl);
        if (true == pinControl)
        {
            *gainCtrlMode = ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN;
        }
        else
        {
            *gainCtrlMode = ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI;
        }
    }

    ADI_API_RETURN(device);
}


static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_Configure_Validate(adi_adrv9001_Device_t *device,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_GainControlCfg_t *agcCfg)
{
    static const uint8_t PEAK_WAIT_TIME_MAX = 0x1F;
    static const uint32_t GAIN_UPDATE_COUNTER_MAX = 0x003FFFFF;
    static const uint8_t UNDER_RANGE_HIGH_POWER_THRESH_MAX = 0x7F;
    static const uint8_t UNDER_RANGE_LOW_POWER_THRESH_MAX = 0x0F;
    static const uint8_t UNDER_RANGE_HIGH_POWER_GAIN_STEP_RECOVERY_MAX = 0x1F;
    static const uint8_t UNDER_RANGE_LOW_POWER_GAIN_STEP_RECOVERY_MAX = 0x1F;
    static const uint8_t POWER_MEASUREMENT_DURATION_MAX = 0x1F;
    static const uint8_t SLOW_LOOP_SETTLING_DELAY_MAX = 0x7F;
    static const uint8_t APD_HIGH_THRESH_MAX = 0x3F;
    static const uint8_t APD_LOW_THRESH_MAX = 0x3F;
    static const uint8_t APD_LOW_GAIN_STEP_ATTACK_MAX = 0x1F;
    static const uint8_t APD_GAIN_STEP_RECOVERY_MAX = 0x1F;
    static const uint8_t HB_OVER_LOAD_DURATION_CNT_MAX = 0x07;
    static const uint8_t HB_OVER_LOAD_THRESH_CNT_MAX = 0x0F;
    static const uint8_t HB_GAIN_STEP_HIGH_RECOVERY_MAX = 0x1F;
    static const uint8_t HB_GAIN_STEP_LOW_RECOVERY_MAX = 0x1F;
    static const uint8_t HB_GAIN_STEP_ATTACK_MAX = 0x1F;
    static const uint8_t HB_GAIN_STEP_MID_RECOVERY_MAX = 0x1F;
    static const uint8_t OVER_RANGE_HIGH_POWER_THRESH_MID_RECOVERY_MAX = 0x0F;
    static const uint8_t OVER_RANGE_LOW_POWER_THRESH_MID_RECOVERY_MAX = 0x7F;
    static const uint8_t OVER_RANGE_HIGH_POWER_GAIN_STEP_ATTACK_MAX = 0x1F;
    static const uint8_t OVER_RANGE_LOW_POWER_GAIN_STEP_ATTACK_MAX = 0x1F;
    static const uint8_t CHANGE_GAIN_IF_THRESH_HIGH_MAX = 0x03;
    static const uint8_t ATTACK_DELAY_MAX = 0x3F;
    static const uint8_t UNDER_RANGE_MID_INTERVAL_MAX = 0x3F;
    static const uint8_t UNDER_RANGE_HIGH_INTERVAL_MAX = 0x3F;
    static const uint8_t ENABLE_MAX = 0x01;
    static const uint16_t HB_THRESH_MAX = 0x3FFF;


    ADI_NULL_PTR_RETURN(&device->common, agcCfg);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_EXPECT(adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set_Validate, device, channel, agcCfg->minGainIndex, agcCfg->maxGainIndex);
    ADI_RANGE_CHECK(device, agcCfg->peakWaitTime,           0, PEAK_WAIT_TIME_MAX);
    ADI_RANGE_CHECK(device, agcCfg->gainUpdateCounter,      0, GAIN_UPDATE_COUNTER_MAX);
    ADI_RANGE_CHECK(device, agcCfg->attackDelay_us,         0, ATTACK_DELAY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->slowLoopSettlingDelay,  0, SLOW_LOOP_SETTLING_DELAY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->changeGainIfThreshHigh, 0, CHANGE_GAIN_IF_THRESH_HIGH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->agcMode, ADI_ADRV9001_RX_GAIN_CONTROL_DETECTION_MODE_PEAK_AND_POWER, ADI_ADRV9001_RX_GAIN_CONTROL_DETECTION_MODE_PEAK);
    ADI_RANGE_CHECK(device, agcCfg->resetOnRxonGainIndex, agcCfg->minGainIndex, agcCfg->maxGainIndex);

    /* Power Configuration register */
    ADI_RANGE_CHECK(device, agcCfg->power.underRangeHighPowerThresh,           0, UNDER_RANGE_HIGH_POWER_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.underRangeLowPowerThresh,            0, UNDER_RANGE_LOW_POWER_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.underRangeHighPowerGainStepRecovery, 0, UNDER_RANGE_HIGH_POWER_GAIN_STEP_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.underRangeLowPowerGainStepRecovery,  0, UNDER_RANGE_LOW_POWER_GAIN_STEP_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.powerMeasurementDuration,            0, POWER_MEASUREMENT_DURATION_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.overRangeHighPowerThresh,            0, OVER_RANGE_HIGH_POWER_THRESH_MID_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.overRangeLowPowerThresh,             0, OVER_RANGE_LOW_POWER_THRESH_MID_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.overRangeHighPowerGainStepAttack,    0, OVER_RANGE_HIGH_POWER_GAIN_STEP_ATTACK_MAX);
    ADI_RANGE_CHECK(device, agcCfg->power.overRangeLowPowerGainStepAttack,     0, OVER_RANGE_LOW_POWER_GAIN_STEP_ATTACK_MAX);

    /* Agc Peak */
    ADI_RANGE_CHECK(device, agcCfg->peak.agcUnderRangeMidInterval,  0, UNDER_RANGE_MID_INTERVAL_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.agcUnderRangeHighInterval, 0, UNDER_RANGE_HIGH_INTERVAL_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.apdHighThresh,             0, APD_HIGH_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.apdLowThresh,              0, APD_LOW_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.apdGainStepAttack,         0, APD_LOW_GAIN_STEP_ATTACK_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.apdGainStepRecovery,       0, APD_GAIN_STEP_RECOVERY_MAX);

    /* HB Configuration */
    ADI_RANGE_CHECK(device, agcCfg->peak.hbOverloadDurationCount,  0, HB_OVER_LOAD_DURATION_CNT_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbOverloadThreshCount,    0, HB_OVER_LOAD_THRESH_CNT_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbHighThresh,           0, HB_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbUnderRangeLowThresh,  0, HB_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbUnderRangeMidThresh,  0, HB_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbUnderRangeHighThresh, 0, HB_THRESH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbGainStepHighRecovery, 0, HB_GAIN_STEP_HIGH_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbGainStepLowRecovery,  0, HB_GAIN_STEP_LOW_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbGainStepMidRecovery,  0, HB_GAIN_STEP_MID_RECOVERY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbGainStepAttack,       0, HB_GAIN_STEP_ATTACK_MAX);
    ADI_RANGE_CHECK(device, agcCfg->peak.hbOverloadPowerMode,    0, ENABLE_MAX);

    ADI_RANGE_CHECK(device,
                    agcCfg->peak.feedback_apd_low_hb_low,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
        agcCfg->peak.feedback_apd_high_hb_high,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
                    agcCfg->power.feedback_inner_high_inner_low,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
                    agcCfg->power.feedback_apd_high_apd_low,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_Configure(adi_adrv9001_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_GainControlCfg_t *agcCfg)
{
    static const uint8_t HB_THRESH_CONFIG = 0x3;
    static const uint8_t POWER_INPUT_SELECT = 0x0;
    static const uint8_t POWER_LOG_SHIFT = 0x1;

    adrv9001_BfNvsRegmapRx_e rxAddr = ADRV9001_BF_RX1_CORE;
    adrv9001_BfNvsRegmapRxb_e rxbAddr = ADRV9001_BF_RXB1_CORE;
    adrv9001_BfAnalogRxMemMap_e anaAddr = ADRV9001_BF_RX1_ANA;
    uint8_t instanceIdx = 0;

    uint8_t bfValue = 0;
    uint8_t regId = 0;
    uint32_t threshCalc = 0;

    /* apdHighThresh and apdLowThresh have to be written to 7 registers */
    static uint8_t REGISTER_ID = 7;

    uint8_t controlMuxAddress = 0;
    adi_adrv9001_RxGainControlMode_e gainCtrlMode = ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI;

    static const uint8_t ADI_ADRV9001_GAIN_PEAK_ADDRESS = 0x28;
    static const uint8_t ADI_ADRV9001_GAIN_PEAK_POWER_ADDRESS = 0x30;

    /* GPIO variables */
    uint8_t gpioCrumb1_0 = 0;
    uint8_t gpioCrumb3_2 = 0;
    uint8_t gpioSource1_0 = 0;
    uint8_t gpioSource3_2 = 0;
    uint16_t gpioOutEn = 0;
    static const uint16_t GPIO_SOURCE_SEL_ADDR = 0x56;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_5_4 = 0x12; /* peak or peak&power */
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_7_6 = 0x13; /* peak or peak&power */
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_5_4 = 0x16; /* peak or peak&power */
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_7_6 = 0x17; /* peak or peak&power */
#ifdef __KERNEL__
    /* APD Low Frequency MITIGATION Mode Setup */
    static const uint8_t APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER = 3;
    static const uint8_t APD_LOW_FREQ_ERROR_MITIGATION_MODE = 1;

    static const uint32_t APD_LOW_FREQ_THRESH_SUBTRACTION_FACTOR = 33352;
    static const uint32_t APD_LOW_FREQ_THRESH_DIVISION_FACTOR = 133352;
    static const uint32_t APD_LOW_FREQ_THRESH_MULTIPLICATION_FACTOR = 100000;
#endif

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_Configure_Validate, device, channel, agcCfg);

    adi_common_channel_to_index(channel, &instanceIdx);
    rxAddr = nvsRegmapRxInstances[instanceIdx];
    rxbAddr = nvsRegmapRxbInstances[instanceIdx];
    anaAddr = analogRxMemMapInstances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcPeakWaitTime_Set,           device, rxbAddr, agcCfg->peakWaitTime);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMaximumGainIndex_Set,       device, rxbAddr, agcCfg->maxGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Set,       device, rxbAddr, agcCfg->minGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcGainUpdateCounter_Set,      device, rxbAddr, agcCfg->gainUpdateCounter);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAttackDelay_Set,            device, rxbAddr, agcCfg->attackDelay_us);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableFastRecoveryLoop_Set, device, rxbAddr, agcCfg->enableFastRecoveryLoop);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLowThsPreventGainInc_Set,   device, rxbAddr, agcCfg->lowThreshPreventGainInc);

    bfValue = agcCfg->changeGainIfThreshHigh & 0x01;
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcChangeGainIfUlbthHigh_Set, device, rxbAddr, bfValue);
    bfValue = (agcCfg->changeGainIfThreshHigh >> 1) & 0x01;
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcChangeGainIfAdcovrgHigh_Set, device, rxbAddr, bfValue);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcPeakThresholdGainControlMode_Set, device, rxbAddr, agcCfg->agcMode);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcResetOnRxon_Set,                   device, rxbAddr, agcCfg->resetOnRxon);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcResetOnRxonGainIndex_Set,          device, rxbAddr, agcCfg->resetOnRxonGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableSyncPulseForGainCounter_Set, device, rxbAddr, agcCfg->enableSyncPulseForGainCounter);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSlowLoopSettlingDelay_Set,         device, rxbAddr, agcCfg->slowLoopSettlingDelay);

    /* Agc Peak */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval0_Set,     device, rxbAddr, agcCfg->peak.agcUnderRangeLowInterval);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval1Mult_Set, device, rxbAddr, agcCfg->peak.agcUnderRangeMidInterval);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval2Mult_Set, device, rxbAddr, agcCfg->peak.agcUnderRangeHighInterval);

    for (regId = 0; regId < REGISTER_ID; regId++)
    {
        ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxBlockDetUlbth_Set, device, anaAddr, regId, agcCfg->peak.apdHighThresh);
        ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxBlockDetLlbth_Set, device, anaAddr, regId, agcCfg->peak.apdLowThresh);
    }

    ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxBlockDetDecay_Set,  device, anaAddr, 0);
    ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxTiaForceUpdate_Set, device, anaAddr, 1);
    ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxTiaForceUpdate_Set, device, anaAddr, 0);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlbThresholdExceededCounter_Set, device, rxbAddr, agcCfg->peak.apdUpperThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLlbThresholdExceededCounter_Set, device, rxbAddr, agcCfg->peak.apdLowerThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlbGainStep_Set,                 device, rxbAddr, agcCfg->peak.apdGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLlbGainStep_Set,                 device, rxbAddr, agcCfg->peak.apdGainStepRecovery);

    /*HB Configuration register*/
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxEnableDecOverload_Set,         device, rxAddr, agcCfg->peak.enableHbOverload);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadDurationCount_Set,  device, rxAddr, agcCfg->peak.hbOverloadDurationCount);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadThresholdCount_Set, device, rxAddr, agcCfg->peak.hbOverloadThreshCount);

    /* HB */
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadUpperThreshold_Set,     device, rxAddr, agcCfg->peak.hbHighThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadInt0LowerThreshold_Set, device, rxAddr, agcCfg->peak.hbUnderRangeLowThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadInt1LowerThreshold_Set, device, rxAddr, agcCfg->peak.hbUnderRangeMidThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadLowerThreshold_Set,     device, rxAddr, agcCfg->peak.hbUnderRangeHighThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcHighOvrgExceededCounter_Set,            device, rxbAddr, agcCfg->peak.hbUpperThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcLowOvrgExceededCounter_Set,             device, rxbAddr, agcCfg->peak.hbUnderRangeHighThreshExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowGainStep_Set,                       device, rxbAddr, agcCfg->peak.hbGainStepHighRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowInt0GainStep_Set,                   device, rxbAddr, agcCfg->peak.hbGainStepLowRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowInt1GainStep_Set,                   device, rxbAddr, agcCfg->peak.hbGainStepMidRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgHighGainStep_Set,                      device, rxbAddr, agcCfg->peak.hbGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadPowerMode_Set,                    device, rxAddr, agcCfg->peak.hbOverloadPowerMode);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecThresholdConfig_Set,                      device, rxAddr, HB_THRESH_CONFIG);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcovrgLowInt1Counter_Set,                 device, rxbAddr, agcCfg->peak.hbUnderRangeMidThreshExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcovrgLowInt0Counter_Set,                 device, rxbAddr, agcCfg->peak.hbUnderRangeLowThreshExceededCount);

#ifndef __KERNEL__
    /* APD Low Frequency MITIGATION Mode Setup */
    static const uint8_t APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER = 3;
    static const uint8_t APD_LOW_FREQ_ERROR_MITIGATION_MODE = 1;

    static const uint32_t APD_LOW_FREQ_THRESH_SUBTRACTION_FACTOR = 33352;
    static const uint32_t APD_LOW_FREQ_THRESH_DIVISION_FACTOR = 133352;
    static const uint32_t APD_LOW_FREQ_THRESH_MULTIPLICATION_FACTOR = 100000;
#endif
    /* The new formula should be:
       "decimated_data_overload_secondary_upper_threshold = Round((hbHighThresh - 0.33352)/1.33352)"
       The equation above is derived based on the fact that the 2nd high counter should be set 2.5dB below the hbHighThresh value.
       20*log10((hbHighThresh + 1)/2^15) - 20*log10((secondary_upper_threshold + 1)/2^15) = 2.5
    */
    threshCalc = (agcCfg->peak.hbHighThresh * APD_LOW_FREQ_THRESH_MULTIPLICATION_FACTOR) - APD_LOW_FREQ_THRESH_SUBTRACTION_FACTOR;
    threshCalc = DIV_ROUND_CLOSEST(threshCalc, APD_LOW_FREQ_THRESH_DIVISION_FACTOR);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecimatedDataOverloadSecondaryUpperThreshold_Set, device, rxbAddr, threshCalc);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_Adcovrg2ndHighCounter_Set,            device, rxbAddr, APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcApdLowFreqErrorMitigationMode_Set, device, rxbAddr, APD_LOW_FREQ_ERROR_MITIGATION_MODE);

    /* Power Configuration register */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerEnableMeas_Set,                 device, rxbAddr, agcCfg->power.powerEnableMeasurement);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerInputSelect_Set,                device, rxbAddr, POWER_INPUT_SELECT);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower1Threshold_Set,                 device, rxbAddr, agcCfg->power.underRangeLowPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower0Threshold_Set,                 device, rxbAddr, agcCfg->power.underRangeHighPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower0ThresholdExceededGainStep_Set, device, rxbAddr, agcCfg->power.underRangeHighPowerGainStepRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower1ThresholdExceededGainStep_Set, device, rxbAddr, agcCfg->power.underRangeLowPowerGainStepRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerMeasurementDuration_Set,        device, rxbAddr, agcCfg->power.powerMeasurementDuration);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerWaitDelay_Set,                  device, rxbAddr, agcCfg->power.powerMeasurementDelay);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlSigPowerMeasDuration_Set,          device, rxbAddr, agcCfg->power.rxTddPowerMeasDuration);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlSigPowerMeasDelay_Set,             device, rxbAddr, agcCfg->power.rxTddPowerMeasDelay);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLockLevel_Set,                       device, rxbAddr, agcCfg->power.overRangeLowPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper1Threshold_Set,                 device, rxbAddr, agcCfg->power.overRangeHighPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerLogShift_Set,                   device, rxbAddr, POWER_LOG_SHIFT);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper1ThresholdExceededGainStep_Set, device, rxbAddr, agcCfg->power.overRangeHighPowerGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper0ThresholdExceededGainStep_Set, device, rxbAddr, agcCfg->power.overRangeLowPowerGainStepAttack);

    /* External LNA */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_ExtLnaSettlingDelay_Set, device, rxbAddr, agcCfg->extLna.settlingDelay);

    if (agcCfg->agcMode == ADI_ADRV9001_RX_GAIN_CONTROL_DETECTION_MODE_PEAK)
    {
        controlMuxAddress = ADI_ADRV9001_GAIN_PEAK_ADDRESS;
    }
    else
    {
        controlMuxAddress = ADI_ADRV9001_GAIN_PEAK_POWER_ADDRESS;
    }

    ADI_EXPECT(adi_adrv9001_Rx_GainControl_Mode_Get, device, channel, &gainCtrlMode);

    if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, rxbAddr, true);
    }
    else if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, rxbAddr, true);
    }
    else if (gainCtrlMode == ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUseCountersForMgc_Set, device, rxbAddr, false);
    }

    /* Determine crumbs and sources to set */
    if (agcCfg->agcMode == ADI_ADRV9001_RX_GAIN_CONTROL_DETECTION_MODE_PEAK)
    {
        gpioCrumb1_0 = (uint8_t)agcCfg->peak.feedback_apd_low_hb_low;
        gpioCrumb3_2 = (uint8_t)agcCfg->peak.feedback_apd_high_hb_high;
        if (ADI_CHANNEL_1 == channel)
        {
            gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX1_5_4;
            gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX1_7_6;
        }
        else
        {
            gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX2_5_4;
            gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX2_7_6;
        }
    }
    else
    {
        gpioCrumb1_0 = (uint8_t)agcCfg->power.feedback_inner_high_inner_low;
        gpioCrumb3_2 = (uint8_t)agcCfg->power.feedback_apd_high_apd_low;
        if (ADI_CHANNEL_1 == channel)
        {
            gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX1_5_4;
            gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX1_7_6;
        }
        else
        {
            gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX2_5_4;
            gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX2_7_6;
        }
    }

    /* Configure pins as outputs */
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn |= (1 << (gpioCrumb1_0 * 2 - 1)) | (1 << (gpioCrumb1_0 * 2 - 2));
    gpioOutEn |= (1 << (gpioCrumb3_2 * 2 - 1)) | (1 << (gpioCrumb3_2 * 2 - 2));
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);

    /* Configure source */
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb1_0 - 1), gpioSource1_0);
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb3_2 - 1), gpioSource3_2);

    ADI_EXPECT(adrv9001_NvsRegmapRx_ControlOutMuxSel_Set, device, rxAddr, controlMuxAddress);

	/* Enable/Disable RXQEC Freeze*/
	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx1Fic2CaptPedSoftOverrideEnable_Set, device, agcCfg->rxQecFreezeEnable);
	}
	else
	{
		ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx2Fic2CaptPedSoftOverrideEnable_Set, device, agcCfg->rxQecFreezeEnable);
	}
    
	if (agcCfg->gpioFreezePin != ADI_ADRV9001_GPIO_UNASSIGNED)
	{
		/* Enable freeze mode */
		ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableGainFreeze_Set, device, rxbAddr, 0x1);

		/* Set up GPIO pins */
		ADI_EXPECT(adi_adrv9001_gpio_ManualInput_Configure, device, agcCfg->gpioFreezePin);
		if (ADI_CHANNEL_1 == channel)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioSelect_Set, device, (agcCfg->gpioFreezePin - 1));
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioMask_Set, device, 0);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioSelect_Set, device, (agcCfg->gpioFreezePin - 1));
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioMask_Set, device, 0);
		}
	}

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_Inspect_Validate(adi_adrv9001_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_GainControlCfg_t *agcCfg)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, agcCfg);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_Inspect(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_GainControlCfg_t *agcCfg)
{
    adrv9001_BfNvsRegmapRx_e rxAddr = ADRV9001_BF_RX1_CORE;
    adrv9001_BfNvsRegmapRxb_e rxbAddr = ADRV9001_BF_RXB1_CORE;
    adrv9001_BfAnalogRxMemMap_e anaAddr = ADRV9001_BF_RX1_ANA;
    uint8_t instanceIdx = 0;
    uint8_t bfValue = 0;
    uint8_t i = 0;

    static const uint16_t GPIO_SOURCE_SEL_ADDR = 0x56;
    enum
    {
        ADI_ADRV9001_GPIO_SOURCE_RX1_5_4 = 0x12, /* peak or peak&power */
        ADI_ADRV9001_GPIO_SOURCE_RX1_7_6 = 0x13, /* peak or peak&power */
        ADI_ADRV9001_GPIO_SOURCE_RX2_5_4 = 0x16, /* peak */
        ADI_ADRV9001_GPIO_SOURCE_RX2_7_6 = 0x17
    };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_Inspect_Validate, device, channel, agcCfg);

    adi_common_channel_to_index(channel, &instanceIdx);
    rxAddr = nvsRegmapRxInstances[instanceIdx];
    rxbAddr = nvsRegmapRxbInstances[instanceIdx];
    anaAddr = analogRxMemMapInstances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcPeakWaitTime_Get,      device, rxbAddr, &agcCfg->peakWaitTime);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMaximumGainIndex_Get,  device, rxbAddr, &agcCfg->maxGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Get,  device, rxbAddr, &agcCfg->minGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcGainUpdateCounter_Get, device, rxbAddr, &agcCfg->gainUpdateCounter);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAttackDelay_Get,       device, rxbAddr, &agcCfg->attackDelay_us);

    /* AGC Control register - Slowloop_config*/
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableFastRecoveryLoop_Get, device, rxbAddr, &bfValue);
    agcCfg->enableFastRecoveryLoop = (bool)bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLowThsPreventGainInc_Get, device, rxbAddr, &bfValue);
    agcCfg->lowThreshPreventGainInc = bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcChangeGainIfUlbthHigh_Get, device, rxbAddr, &bfValue);
    agcCfg->changeGainIfThreshHigh = (bfValue & 0x01);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcChangeGainIfAdcovrgHigh_Get, device, rxbAddr, &bfValue);
    agcCfg->changeGainIfThreshHigh |= ((bfValue << 1) & 0x02);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcPeakThresholdGainControlMode_Get, device, rxbAddr, &bfValue);
    agcCfg->agcMode = (adi_adrv9001_RxGainControlDetectionMode_e)bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcResetOnRxon_Get, device, rxbAddr, &bfValue);
    agcCfg->resetOnRxon = (bool)bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcResetOnRxonGainIndex_Get, device, rxbAddr, &agcCfg->resetOnRxonGainIndex);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableSyncPulseForGainCounter_Get, device, rxbAddr, &bfValue);
    agcCfg->enableSyncPulseForGainCounter = (bool)bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSlowLoopSettlingDelay_Get, device, rxbAddr, &agcCfg->slowLoopSettlingDelay);

    /* Agc Peak */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval0_Get,             device, rxbAddr, &agcCfg->peak.agcUnderRangeLowInterval);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval1Mult_Get,         device, rxbAddr, &agcCfg->peak.agcUnderRangeMidInterval);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUrangeInterval2Mult_Get,         device, rxbAddr, &agcCfg->peak.agcUnderRangeHighInterval);
    ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxBlockDetUlbth_Get,             device, anaAddr, 0, &agcCfg->peak.apdHighThresh);
    ADI_EXPECT(adrv9001_AnalogRxMemMap_OrxBlockDetLlbth_Get,             device, anaAddr, 0, &agcCfg->peak.apdLowThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlbThresholdExceededCounter_Get, device, rxbAddr, &agcCfg->peak.apdUpperThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLlbThresholdExceededCounter_Get, device, rxbAddr, &agcCfg->peak.apdLowerThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlbGainStep_Get,                 device, rxbAddr, &agcCfg->peak.apdGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLlbGainStep_Get,                 device, rxbAddr, &agcCfg->peak.apdGainStepRecovery);

    /*HB Configuration register*/
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxEnableDecOverload_Get, device, rxAddr, &bfValue);
    agcCfg->peak.enableHbOverload = (bool)bfValue;

    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadDurationCount_Get,                device, rxAddr,  &agcCfg->peak.hbOverloadDurationCount);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadThresholdCount_Get,               device, rxAddr,  &agcCfg->peak.hbOverloadThreshCount);

    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadUpperThreshold_Get,     device, rxAddr,  &agcCfg->peak.hbHighThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadInt0LowerThreshold_Get, device, rxAddr,  &agcCfg->peak.hbUnderRangeLowThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadInt1LowerThreshold_Get, device, rxAddr,  &agcCfg->peak.hbUnderRangeMidThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecimatedDataOverloadLowerThreshold_Get,     device, rxAddr,  &agcCfg->peak.hbUnderRangeHighThresh);

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcHighOvrgExceededCounter_Get,            device, rxbAddr, &agcCfg->peak.hbUpperThreshPeakExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcLowOvrgExceededCounter_Get,             device, rxbAddr, &agcCfg->peak.hbUnderRangeHighThreshExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowGainStep_Get,                       device, rxbAddr, &agcCfg->peak.hbGainStepHighRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowInt0GainStep_Get,                   device, rxbAddr, &agcCfg->peak.hbGainStepLowRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgLowInt1GainStep_Get,                   device, rxbAddr, &agcCfg->peak.hbGainStepMidRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcOvrgHighGainStep_Get,                      device, rxbAddr, &agcCfg->peak.hbGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRx_RxDecOverloadPowerMode_Get,                    device, rxAddr,  &agcCfg->peak.hbOverloadPowerMode);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcovrgLowInt1Counter_Get,                 device, rxbAddr, &agcCfg->peak.hbUnderRangeMidThreshExceededCount);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcAdcovrgLowInt0Counter_Get,                 device, rxbAddr, &agcCfg->peak.hbUnderRangeLowThreshExceededCount);

    /* Power Configuration register */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerEnableMeas_Get, device, rxbAddr, &bfValue);
    agcCfg->power.powerEnableMeasurement = (bool)bfValue;
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower1Threshold_Get,                 device, rxbAddr, &agcCfg->power.underRangeLowPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower0Threshold_Get,                 device, rxbAddr, &agcCfg->power.underRangeHighPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower0ThresholdExceededGainStep_Get, device, rxbAddr, &agcCfg->power.underRangeHighPowerGainStepRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLower1ThresholdExceededGainStep_Get, device, rxbAddr, &agcCfg->power.underRangeLowPowerGainStepRecovery);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerMeasurementDuration_Get,        device, rxbAddr, &agcCfg->power.powerMeasurementDuration);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPowerWaitDelay_Get,                  device, rxbAddr, &agcCfg->power.powerMeasurementDelay);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlSigPowerMeasDuration_Get,          device, rxbAddr, &agcCfg->power.rxTddPowerMeasDuration);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUlSigPowerMeasDelay_Get,             device, rxbAddr, &agcCfg->power.rxTddPowerMeasDelay);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcLockLevel_Get,                       device, rxbAddr, &agcCfg->power.overRangeLowPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper1Threshold_Get,                 device, rxbAddr, &agcCfg->power.overRangeHighPowerThresh);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper1ThresholdExceededGainStep_Get, device, rxbAddr, &agcCfg->power.overRangeHighPowerGainStepAttack);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcUpper0ThresholdExceededGainStep_Get, device, rxbAddr, &agcCfg->power.overRangeLowPowerGainStepAttack);

    /* External LNA */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_ExtLnaSettlingDelay_Get, device, rxbAddr, &agcCfg->extLna.settlingDelay);

    /* GPIO */
    agcCfg->peak.feedback_apd_high_hb_high = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->peak.feedback_apd_low_hb_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->power.feedback_apd_high_apd_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->power.feedback_inner_high_inner_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED;
    for (i = 0; i < ADI_ADRV9001_GPIO_PIN_CRUMB_15_14; i++)
    {
        ADRV9001_SPIREADBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + i), &bfValue); 
        if (agcCfg->agcMode == (adi_adrv9001_RxGainControlDetectionMode_e)(0))
        {
            switch (bfValue)
            {
            case ADI_ADRV9001_GPIO_SOURCE_RX1_5_4:  
                agcCfg->power.feedback_inner_high_inner_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);      
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX2_5_4:
                agcCfg->power.feedback_inner_high_inner_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);      
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX1_7_6:
                agcCfg->power.feedback_apd_high_apd_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX2_7_6:
                agcCfg->power.feedback_apd_high_apd_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (bfValue)
            {
            case ADI_ADRV9001_GPIO_SOURCE_RX1_5_4:  
                agcCfg->peak.feedback_apd_low_hb_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);    
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX2_5_4:
                agcCfg->peak.feedback_apd_low_hb_low = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX1_7_6:
                agcCfg->peak.feedback_apd_high_hb_high = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV9001_GPIO_SOURCE_RX2_7_6:
                agcCfg->peak.feedback_apd_high_hb_high = (adi_adrv9001_GpioPinCrumbSel_e)(i + 1);
                break;
            default:
                break;
            }
        }
    }

	/* Enable/Disable RXQEC Freeze flag*/
	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx1Fic2CaptPedSoftOverrideEnable_Get, device, &bfValue);
	}
	else
	{
		ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx2Fic2CaptPedSoftOverrideEnable_Get, device, &bfValue);
	}
	agcCfg->rxQecFreezeEnable = (bool)bfValue;

	/* GPIO Freeze Pin*/
	ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcEnableGainFreeze_Get, device, rxbAddr, &bfValue);
	if (bfValue == 0x1)
	{
		if (ADI_CHANNEL_1 == channel)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioSelect_Get, device, &bfValue);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioSelect_Get, device, &bfValue);
		}
		agcCfg->gpioFreezePin = (adi_adrv9001_GpioPin_e)(bfValue + 1);
	}
	else
	{
		agcCfg->gpioFreezePin = ADI_ADRV9001_GPIO_UNASSIGNED;
	}

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set_Validate(adi_adrv9001_Device_t *device,
                                                                        adi_common_ChannelNumber_e channel,
                                                                        uint8_t minGainIndex,
                                                                        uint8_t maxGainIndex)
{
    uint8_t minIndex = 0;
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Get, device, instance, &minIndex);

    ADI_RANGE_CHECK(device, minGainIndex, minIndex, (maxGainIndex - 1));
    ADI_RANGE_CHECK(device, maxGainIndex, (minGainIndex + 1), ADI_ADRV9001_RX_GAIN_INDEX_MAX);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set(adi_adrv9001_Device_t *device,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t minGainIndex,
                                                        uint8_t maxGainIndex)
{
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set_Validate, device, channel, minGainIndex, maxGainIndex);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Set, device, instance, minGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMaximumGainIndex_Set, device, instance, maxGainIndex);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Get_Validate(adi_adrv9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               uint8_t *minGainIndex,
                                               uint8_t *maxGainIndex)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, minGainIndex);
    ADI_NULL_PTR_RETURN(&device->common, maxGainIndex);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Get(adi_adrv9001_Device_t *device,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t *minGainIndex,
                                                        uint8_t *maxGainIndex)
{
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Get_Validate, device, channel, minGainIndex, maxGainIndex);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMaximumGainIndex_Get, device, instance, maxGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcMinimumGainIndex_Get, device, instance, minGainIndex);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_Reset_Validate(adi_adrv9001_Device_t *device,
                                     adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_Reset(adi_adrv9001_Device_t *device, adi_common_ChannelNumber_e channel)
{
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_Reset_Validate, device, channel);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSoftReset_Set, device, instance, true);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcSoftReset_Set, device, instance, false);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_PinMode_Configure_Validate(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_RxGainControlPinCfg_t *config)
{
    static const uint8_t MAX_STEP_SIZE = 8;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /* Note: config->minGainIndex and ->maxGainIndex are validated by the called function, no need to duplicate */

    ADI_RANGE_CHECK(device, config->incrementStepSize, 0, MAX_STEP_SIZE);
    ADI_RANGE_CHECK(device, config->decrementStepSize, 0, MAX_STEP_SIZE);

    ADI_RANGE_CHECK(device, config->incrementPin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
    ADI_RANGE_CHECK(device, config->decrementPin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
    if (config->incrementPin == config->decrementPin)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         config->decrementPin,
                         "Invalid pin assignment. config->incrementPin cannot be the same as config->decrementPin");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_PinMode_Configure(adi_adrv9001_Device_t *device,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_adrv9001_RxGainControlPinCfg_t *config)
{
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_PinMode_Configure_Validate, device, channel, config);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    /* Set gain parameters */
    ADI_EXPECT(adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Set, device, channel, config->minGainIndex, config->maxGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcIncrGainStepSize_Set, device, instance, (config->incrementStepSize - 1));
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcDecrGainStepSize_Set, device, instance, (config->decrementStepSize - 1));

    /* Set up GPIO pins */
    ADI_EXPECT(adi_adrv9001_gpio_ManualInput_Configure, device, config->incrementPin);
    ADI_EXPECT(adi_adrv9001_gpio_ManualInput_Configure, device, config->decrementPin);
    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1IncgainGpioSelect_Set, device, (config->incrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1DecgainGpioSelect_Set, device, (config->decrementPin - 1));

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1IncgainGpioMask_Set, device, 0);
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1DecgainGpioMask_Set, device, 0);
    }
    else if (ADI_CHANNEL_2 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2IncgainGpioSelect_Set, device, (config->incrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2DecgainGpioSelect_Set, device, (config->decrementPin - 1));

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2IncgainGpioMask_Set, device, 0);
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2DecgainGpioMask_Set, device, 0);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_Rx_GainControl_PinMode_Inspect_Validate(adi_adrv9001_Device_t *device,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv9001_RxGainControlPinCfg_t *config)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, config);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainControl_PinMode_Inspect(adi_adrv9001_Device_t *device,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_adrv9001_RxGainControlPinCfg_t *config)
{
    adrv9001_BfNvsRegmapRxb_e instances[] = { ADRV9001_BF_RXB1_CORE, ADRV9001_BF_RXB2_CORE };
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    uint8_t bfVal = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainControl_PinMode_Inspect_Validate, device, channel, config);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    /* Get gain parameters */
    ADI_EXPECT(adi_adrv9001_Rx_GainControl_MinMaxGainIndex_Get, device, channel, &config->minGainIndex, &config->maxGainIndex);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcIncrGainStepSize_Get, device, instance, &config->incrementStepSize);
    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcDecrGainStepSize_Get, device, instance, &config->decrementStepSize);
    config->incrementStepSize += 1;
    config->decrementStepSize += 1;

    /* Set up GPIO pins */
    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1IncgainGpioSelect_Get, device, &bfVal);
        config->incrementPin = (adi_adrv9001_GpioPin_e)(bfVal + 1);
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx1DecgainGpioSelect_Get, device, &bfVal);
        config->decrementPin = (adi_adrv9001_GpioPin_e)(bfVal + 1);
    }
    else if (ADI_CHANNEL_2 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2IncgainGpioSelect_Get, device, &bfVal);
        config->incrementPin = (adi_adrv9001_GpioPin_e)(bfVal + 1);
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Rx2DecgainGpioSelect_Get, device, &bfVal);
        config->decrementPin = (adi_adrv9001_GpioPin_e)(bfVal + 1);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}
