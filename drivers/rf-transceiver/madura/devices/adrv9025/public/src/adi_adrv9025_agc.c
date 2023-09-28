// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_agc.c
* \brief Contains AGC features related function implementation defined in
* adi_adrv9025_agc.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_agc.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_gpio.h"
#include "../../private/include/adrv9025_bf_rx.h"
#include "../../private/include/adrv9025_bf_analog_rx_mem_map.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_agc.c"
#endif

int32_t adi_adrv9025_AgcCfgSetRangeCheck(adi_adrv9025_Device_t* device,
                                         adi_adrv9025_AgcCfg_t  agcConfig[],
                                         uint8_t                arraySize)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t  MINIMUM_NUM_AGC_CONFIGS                       = 1;
    static const uint8_t  MAXIMUM_NUM_AGC_CONFIGS                       = 4;
    static const uint8_t  AGC_PEAK_WAIT_TIME_MAX                        = 0x1F;
    static const uint32_t AGC_GAIN_UPDATE_COUNTER_MAX                   = 0x003FFFFF;
    static const uint8_t  UNDER_RANGE_HIGH_POWER_THRESH_MAX             = 0x7F;
    static const uint8_t  UNDER_RANGE_LOW_POWER_THRESH_MAX              = 0x1F;
    static const uint8_t  UNDER_RANGE_HIGH_POWER_GAIN_STEP_RECOVERY_MAX = 0x1F;
    static const uint8_t  UNDER_RANGE_LOW_POWER_GAIN_STEP_RECOVERY_MAX  = 0x1F;
    static const uint8_t  POWER_MEASUREMENT_DURATION_MAX                = 0x1F;
    static const uint8_t  AGC_SLOW_LOOP_SETTLING_DELAY_MAX              = 0x7F;
    static const uint8_t  APD_HIGH_THRESH_MAX                           = 0x3F;
    static const uint8_t  APD_LOW_GAIN_MODE_HIGH_THRESH_MAX             = 0x3F;
    static const uint8_t  APD_LOW_THRESH_MAX                            = 0x3F;
    static const uint8_t  APD_LOW_GAIN_MODE_LOW_THRESH_MAX              = 0x3F;
    static const uint8_t  APD_LOW_GAIN_STEP_ATTACK_MAX                  = 0x1F;
    static const uint8_t  APD_GAIN_STEP_RECOVERY_MAX                    = 0x1F;
    static const uint8_t  HB2_OVER_LOAD_DURATION_CNT_MAX                = 0x07;
    static const uint8_t  HB2_OVER_LOAD_THRESH_CNT_MAX                  = 0x0F;
    static const uint16_t HB2_HIGH_THRESH_MAX                           = 0x3FFF;
    static const uint16_t HB2_UNDER_RANGE_LOW_THRESH_MAX                = 0x3FFF;
    static const uint16_t HB2_UNDER_RANGE_MID_THRESH_MAX                = 0x3FFF;
    static const uint16_t HB2_UNDER_RANGE_HIGH_THRESH_MAX               = 0x3FFF;
    static const uint8_t  HB2_GAIN_STEP_HIGH_RECOVERY_MAX               = 0x1F;
    static const uint8_t  HB2_GAIN_STEP_LOW_RECOVERY_MAX                = 0x1F;
    static const uint8_t  HB2_GAIN_STEP_ATTACK_MAX                      = 0x1F;
    static const uint8_t  HB2_GAIN_STEP_MID_RECOVERY_MAX                = 0x1F;
    static const uint8_t  HB2_THRESH_CONFIG                             = 0x03;
    static const uint8_t  OVER_RANGE_HIGH_POWER_THRESH_MID_RECOVERY_MAX = 0x7F;
    static const uint8_t  OVER_RANGE_LOW_POWER_THRESH_MID_RECOVERY_MAX  = 0x0F;
    static const uint8_t  OVER_RANGE_HIGH_POWER_GAIN_STEP_ATTACK_MAX    = 0x1F;
    static const uint8_t  OVER_RANGE_LOW_POWER_GAIN_STEP_ATTACK_MAX     = 0x1F;
    static const uint8_t  POWER_INPUT_SELECT_MAX                        = 0x03;
    static const uint8_t  AGC_CHANGE_GAIN_IF_THRESH_HIGH_MAX            = 0x03;
    static const uint8_t  AGC_RX_ATTACK_DELAY_MAX                       = 0x3F;
    static const uint8_t  AGC_UNDER_RANGE_MID_INTERVAL_MAX              = 0x3F;
    static const uint8_t  AGC_UNDER_RANGE_HIGH_INTERVAL_MAX             = 0x3F;
    static const uint8_t  AGC_ENABLE_MAX                                = 0x01;
    static const uint8_t  AGC_RXCHANNEL_MAX                             = 0x0F;

    uint8_t configIndex = 0;
    uint8_t i           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, agcConfig);

    if (arraySize < MINIMUM_NUM_AGC_CONFIGS)
    {
        /* invalid number of configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid number of AGC configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (arraySize > MAXIMUM_NUM_AGC_CONFIGS)
    {
        /* Unexpected number of configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         arraySize,
                         "WARNING: Number of AGC configs is larger than expected.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (configIndex = 0; configIndex < arraySize; configIndex++)
    {
        uint32_t rxChannelMask = agcConfig[configIndex].rxChannelMask;
        if ((rxChannelMask > AGC_RXCHANNEL_MAX) || (rxChannelMask == 0))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannelMask,
                             "Invalid rxChannelMask. Valid range (1/2/4/8/15)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            if ((rxChannelMask & ADI_ADRV9025_RX1) == ADI_ADRV9025_RX1)
            {
                rxChannelMask &= ~ADI_ADRV9025_RX1;

                /* range check the gain against the max and min expected values */
                if ((agcConfig[configIndex].agcRxMaxGainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex)
                    || (agcConfig[configIndex].agcRxMinGainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     baseAddr,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX2) == ADI_ADRV9025_RX2)
            {
                rxChannelMask &= ~ADI_ADRV9025_RX2;
                /* range check the gain against the max and min expected values */
                if ((agcConfig[configIndex].agcRxMaxGainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex)
                    || (agcConfig[configIndex].agcRxMinGainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     baseAddr,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX3) == ADI_ADRV9025_RX3)
            {
                rxChannelMask &= ~ADI_ADRV9025_RX3;
                /* range check the gain against the max and min expected values */
                if ((agcConfig[configIndex].agcRxMaxGainIndex > device->devStateInfo.gainIndexes.rx3MaxGainIndex)
                    || (agcConfig[configIndex].agcRxMinGainIndex < device->devStateInfo.gainIndexes.rx3MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     baseAddr,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX4) == ADI_ADRV9025_RX4)
            {
                rxChannelMask &= ~ADI_ADRV9025_RX4;
                /* range check the gain against the max and min expected values */
                if ((agcConfig[configIndex].agcRxMaxGainIndex > device->devStateInfo.gainIndexes.rx4MaxGainIndex)
                    || (agcConfig[configIndex].agcRxMinGainIndex < device->devStateInfo.gainIndexes.rx4MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     baseAddr,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else
            {
                break;
            }

            /* performing range check for gain update time */
            if (agcConfig[configIndex].agcGainUpdateCounter > AGC_GAIN_UPDATE_COUNTER_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcGainUpdateCounter out of range (0-4194303)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* performing AGC peak wait time value check */
            if (agcConfig[configIndex].agcPeakWaitTime > AGC_PEAK_WAIT_TIME_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeakWaitTime is out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcRxAttackDelay > AGC_RX_ATTACK_DELAY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcRxAttackDelay out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* AGC Control register checks and write */
            if (agcConfig[configIndex].agcLowThreshPreventGainInc > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcLowThreshPreventGainInc out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcChangeGainIfThreshHigh > AGC_CHANGE_GAIN_IF_THRESH_HIGH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcChangeGainIfThreshHigh out of range (0-3)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeakThreshGainControlMode > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeakThreshGainControlMode out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcResetOnRxon > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcResetOnRxon out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcEnableSyncPulseForGainCounter > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcEnableSyncPulseForGainCounter out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcEnableFastRecoveryLoop > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcEnableFastRecoveryLoop out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcSlowLoopSettlingDelay > AGC_SLOW_LOOP_SETTLING_DELAY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcSlowLoopSettlingDelay out of range (0-127)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Agc Peak */
            /* performing range check for gain update time */
            if (agcConfig[configIndex].agcPeak.agcUnderRangeMidInterval > AGC_UNDER_RANGE_MID_INTERVAL_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.agcUnderRangeMidInterval is out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.agcUnderRangeHighInterval > AGC_UNDER_RANGE_HIGH_INTERVAL_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.agcUnderRangeHighInterval is out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdHighThresh > APD_HIGH_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdHighThresh out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdLowGainModeHighThresh > APD_LOW_GAIN_MODE_HIGH_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdLowGainModeHighThresh out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdLowThresh > APD_LOW_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdLowThresh out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdLowGainModeLowThresh > APD_LOW_GAIN_MODE_LOW_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdLowGainModeLowThresh out of range (0-63)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdGainStepAttack > APD_LOW_GAIN_STEP_ATTACK_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdGainStepAttack out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.apdGainStepRecovery > APD_GAIN_STEP_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.apdGainStepRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* HB2 Configuration */
            if (agcConfig[configIndex].agcPeak.enableHb2Overload > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.enableHb2Overload out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2OverloadDurationCnt > HB2_OVER_LOAD_DURATION_CNT_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2OverloadDurationCnt out of range (0-7)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2OverloadThreshCnt > HB2_OVER_LOAD_THRESH_CNT_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2OverloadDurationCnt out of range (0-15)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2HighThresh > HB2_HIGH_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2HighThresh out of range (0-16383)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2UnderRangeLowThresh > HB2_UNDER_RANGE_LOW_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2UnderRangeLowThresh out of range (0-16383)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2UnderRangeMidThresh > HB2_UNDER_RANGE_MID_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2UnderRangeMidThresh out of range (0-16383)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2UnderRangeHighThresh > HB2_UNDER_RANGE_HIGH_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2UnderRangeHighThresh out of range (0-16383)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2GainStepHighRecovery > HB2_GAIN_STEP_HIGH_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2GainStepHighRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2GainStepLowRecovery > HB2_GAIN_STEP_LOW_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2GainStepLowRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2GainStepMidRecovery > HB2_GAIN_STEP_MID_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2GainStepMidRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2GainStepAttack > HB2_GAIN_STEP_ATTACK_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2GainStepAttack out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2OverloadPowerMode > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2OverloadPowerMode out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPeak.hb2ThreshConfig != HB2_THRESH_CONFIG)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPeak.hb2ThreshConfig out of range (3)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Power Configuration register */
            if (agcConfig[configIndex].agcPower.powerEnableMeasurement > AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.powerEnableMeasurement out of range (0-1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.powerInputSelect > POWER_INPUT_SELECT_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.powerInputSelect out of range (0-3)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.underRangeHighPowerThresh > UNDER_RANGE_HIGH_POWER_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.underRangeHighPowerThresh out of range (0-127)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.underRangeLowPowerThresh > UNDER_RANGE_LOW_POWER_THRESH_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.underRangeLowPowerThresh out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.underRangeHighPowerGainStepRecovery > UNDER_RANGE_HIGH_POWER_GAIN_STEP_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.underRangeHighPowerGainStepRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.underRangeLowPowerGainStepRecovery > UNDER_RANGE_LOW_POWER_GAIN_STEP_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.underRangeLowPowerGainStepRecovery out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.powerMeasurementDuration > POWER_MEASUREMENT_DURATION_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.powerMeasurementDuration out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.overRangeHighPowerThresh > OVER_RANGE_HIGH_POWER_THRESH_MID_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.overRangeHighPowerThresh out of range (0-127)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.overRangeLowPowerThresh > OVER_RANGE_LOW_POWER_THRESH_MID_RECOVERY_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.overRangeLowPowerThresh out of range (0-15)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.powerLogShift != AGC_ENABLE_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.powerLogShift out of range (1)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.overRangeHighPowerGainStepAttack > OVER_RANGE_HIGH_POWER_GAIN_STEP_ATTACK_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.overRangeHighPowerGainStepAttack out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (agcConfig[configIndex].agcPower.overRangeLowPowerGainStepAttack > OVER_RANGE_LOW_POWER_GAIN_STEP_ATTACK_MAX)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 baseAddr,
                                 "agcPower.overRangeLowPowerGainStepAttack out of range (0-31)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adi_adrv9025_AgcCfgSet(adi_adrv9025_Device_t* device,
                               adi_adrv9025_AgcCfg_t  agcConfig[],
                               uint8_t                arraySize)
{
    adrv9025_BfRxChanAddr_e             baseAddr       = ADRV9025_BF_RX_CH0;
    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH0;
    uint8_t                             bfValue        = 0;
    uint8_t                             configIndex    = 0;
    uint8_t                             i              = 0;
    uint8_t                             refClockCycles = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        agcConfig);

    if (arraySize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid number of AGC configs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

#if ADI_ADRV9025_AGC_RANGE_CHECK > 0
    adi_adrv9025_AgcCfgSetRangeCheck(device,
                                     agcConfig,
                                     arraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (configIndex = 0; configIndex < arraySize; configIndex++)
    {
        uint32_t rxChannelMask   = agcConfig[configIndex].rxChannelMask;
        uint32_t rxChannelSelect = 0;
        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            if ((rxChannelMask & ADI_ADRV9025_RX1) == ADI_ADRV9025_RX1)
            {
                baseAddr       = ADRV9025_BF_RX_CH0;
                baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH0;
                rxChannelMask &= ~ADI_ADRV9025_RX1;
                rxChannelSelect = ADI_ADRV9025_RX1;
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX2) == ADI_ADRV9025_RX2)
            {
                baseAddr       = ADRV9025_BF_RX_CH1;
                baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH1;
                rxChannelMask &= ~ADI_ADRV9025_RX2;
                rxChannelSelect = ADI_ADRV9025_RX2;
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX3) == ADI_ADRV9025_RX3)
            {
                baseAddr       = ADRV9025_BF_RX_CH2;
                baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH2;
                rxChannelMask &= ~ADI_ADRV9025_RX3;
                rxChannelSelect = ADI_ADRV9025_RX3;
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX4) == ADI_ADRV9025_RX4)
            {
                baseAddr       = ADRV9025_BF_RX_CH3;
                baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH3;
                rxChannelMask &= ~ADI_ADRV9025_RX4;
                rxChannelSelect = ADI_ADRV9025_RX4;
            }
            else
            {
                break;
            }


            /* Read the reference clock cycles for the given channel */
            adrv9025_RxReferenceClockCyclesBfGet(device, baseAddr, &refClockCycles);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Configure delay counter base rate with reference clock cycle value */
            adrv9025_RxAgcDelayCounterBaseRateBfSet(device, baseAddr, refClockCycles);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcPeakWaitTimeBfSet(device,
                                            baseAddr,
                                            agcConfig[configIndex].agcPeakWaitTime);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* writing Rx max and min gain indices */
            adrv9025_RxAgcMaximumGainIndexBfSet(device,
                                                baseAddr,
                                                agcConfig[configIndex].agcRxMaxGainIndex);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcMinimumGainIndexBfSet(device,
                                                baseAddr,
                                                agcConfig[configIndex].agcRxMinGainIndex);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcGainUpdateCounterBfSet(device,
                                                 baseAddr,
                                                 agcConfig[configIndex].agcGainUpdateCounter);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAttackDelayBfSet(device,
                                           baseAddr,
                                           agcConfig[configIndex].agcRxAttackDelay);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* AGC Control register - Slowloop_config*/
            adrv9025_RxAgcEnableFastRecoveryLoopBfSet(device,
                                                      baseAddr,
                                                      agcConfig[configIndex].agcEnableFastRecoveryLoop);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Fast recovery loop is enabled for peak AGC mode and disabled for AGC power mode */
            if (agcConfig[configIndex].agcEnableFastRecoveryLoop > 0)
            {
                device->devStateInfo.rxAgcPeakModeMask |= rxChannelSelect;
            }
            else
            {
                device->devStateInfo.rxAgcPeakModeMask &= ~rxChannelSelect;
            }

            adrv9025_RxAgcLowThsPreventGainIncBfSet(device,
                                                    baseAddr,
                                                    agcConfig[configIndex].agcLowThreshPreventGainInc);
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = agcConfig[configIndex].agcChangeGainIfThreshHigh & 0x01;
            adrv9025_RxAgcChangeGainIfUlbthHighBfSet(device,
                                                     baseAddr,
                                                     bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = (agcConfig[configIndex].agcChangeGainIfThreshHigh >> 1) & 0x01;
            adrv9025_RxAgcChangeGainIfAdcovrgHighBfSet(device,
                                                       baseAddr,
                                                       bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcPeakThresholdGainControlModeBfSet(device,
                                                            baseAddr,
                                                            agcConfig[configIndex].agcPeakThreshGainControlMode);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcResetOnRxonBfSet(device,
                                           baseAddr,
                                           agcConfig[configIndex].agcResetOnRxon);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcEnableSyncPulseForGainCounterBfSet(device,
                                                             baseAddr,
                                                             agcConfig[configIndex].agcEnableSyncPulseForGainCounter);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcSlowLoopSettlingDelayBfSet(device,
                                                     baseAddr,
                                                     agcConfig[configIndex].agcSlowLoopSettlingDelay);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAdcResetGainStepBfSet(device,
                                                baseAddr,
                                                agcConfig[configIndex].agcAdcResetGainStep);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcSlowloopFastGainChangeBlockEnableBfSet(device,
                                                                 baseAddr,
                                                                 agcConfig[configIndex].agcSlowloopFastGainChangeBlockEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Agc Peak */
            adrv9025_RxAgcUrangeInterval0BfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPeak.agcUnderRangeLowInterval);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUrangeInterval1MultBfSet(device,
                                                   baseAddr,
                                                   agcConfig[configIndex].agcPeak.agcUnderRangeMidInterval);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUrangeInterval2MultBfSet(device,
                                                   baseAddr,
                                                   agcConfig[configIndex].agcPeak.agcUnderRangeHighInterval);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Digital: adrv9025_RxUpperLevelBlockerThresholdBfSet */
            adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet(device,
                                                        baseAddrAnalog,
                                                        0,
                                                        agcConfig[configIndex].agcPeak.apdHighThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Digital: adrv9025_RxUpperLevelBlockerThresholdTiaLowGainModeBfSet */
            adrv9025_AnalogRxMemMapRxBlockDetUlbthBfSet(device,
                                                        baseAddrAnalog,
                                                        1,
                                                        agcConfig[configIndex].agcPeak.apdLowGainModeHighThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Digital: adrv9025_RxLowerLevelBlockerThresholdBfSet */
            adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet(device,
                                                        baseAddrAnalog,
                                                        0,
                                                        agcConfig[configIndex].agcPeak.apdLowThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Digital: adrv9025_RxLowerLevelBlockerThresholdTiaLowGainModeBfSet */
            adrv9025_AnalogRxMemMapRxBlockDetLlbthBfSet(device,
                                                        baseAddrAnalog,
                                                        1,
                                                        agcConfig[configIndex].agcPeak.apdLowGainModeLowThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = 0;
            adrv9025_AnalogRxMemMapRxBlockDetDecayBfSet(device,
                                                        baseAddrAnalog,
                                                        bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);
            bfValue = 1;
            adrv9025_AnalogRxMemMapRxTiaForceUpdateBfSet(device,
                                                         baseAddrAnalog,
                                                         bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);
            bfValue = 0;
            adrv9025_AnalogRxMemMapRxTiaForceUpdateBfSet(device,
                                                         baseAddrAnalog,
                                                         bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUlbThresholdExceededCounterBfSet(device,
                                                           baseAddr,
                                                           agcConfig[configIndex].agcPeak.apdUpperThreshPeakExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLlbThresholdExceededCounterBfSet(device,
                                                           baseAddr,
                                                           agcConfig[configIndex].agcPeak.apdLowerThreshPeakExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUlbGainStepBfSet(device,
                                           baseAddr,
                                           agcConfig[configIndex].agcPeak.apdGainStepAttack);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLlbGainStepBfSet(device,
                                           baseAddr,
                                           agcConfig[configIndex].agcPeak.apdGainStepRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*HB2 Configuration register*/
            adrv9025_RxEnableDecOverloadBfSet(device,
                                              baseAddr,
                                              agcConfig[configIndex].agcPeak.enableHb2Overload);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecOverloadDurationCountBfSet(device,
                                                     baseAddr,
                                                     agcConfig[configIndex].agcPeak.hb2OverloadDurationCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecOverloadThresholdCountBfSet(device,
                                                      baseAddr,
                                                      agcConfig[configIndex].agcPeak.hb2OverloadThreshCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* HB2 */
            adrv9025_RxDecimatedDataOverloadUpperThresholdBfSet(device,
                                                                baseAddr,
                                                                agcConfig[configIndex].agcPeak.hb2HighThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecimatedDataOverloadInt0LowerThresholdBfSet(device,
                                                                    baseAddr,
                                                                    agcConfig[configIndex].agcPeak.hb2UnderRangeLowThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecimatedDataOverloadInt1LowerThresholdBfSet(device,
                                                                    baseAddr,
                                                                    agcConfig[configIndex].agcPeak.hb2UnderRangeMidThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecimatedDataOverloadLowerThresholdBfSet(device,
                                                                baseAddr,
                                                                agcConfig[configIndex].agcPeak.hb2UnderRangeHighThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAdcHighOvrgExceededCounterBfSet(device,
                                                          baseAddr,
                                                          agcConfig[configIndex].agcPeak.hb2UpperThreshPeakExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAdcLowOvrgExceededCounterBfSet(device,
                                                         baseAddr,
                                                         agcConfig[configIndex].agcPeak.hb2UnderRangeHighThreshExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcOvrgLowGainStepBfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPeak.hb2GainStepHighRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcOvrgLowInt0GainStepBfSet(device,
                                                   baseAddr,
                                                   agcConfig[configIndex].agcPeak.hb2GainStepLowRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcOvrgLowInt1GainStepBfSet(device,
                                                   baseAddr,
                                                   agcConfig[configIndex].agcPeak.hb2GainStepMidRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcOvrgHighGainStepBfSet(device,
                                                baseAddr,
                                                agcConfig[configIndex].agcPeak.hb2GainStepAttack);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecOverloadPowerModeBfSet(device,
                                                 baseAddr,
                                                 agcConfig[configIndex].agcPeak.hb2OverloadPowerMode);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* agcConfig[configIndex].agcPeak.hb2ThreshConfig */
            bfValue = 3; /* Fixed to 3 */
            adrv9025_RxDecThresholdConfigBfSet(device,
                                               baseAddr,
                                               bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAdcovrgLowInt1CounterBfSet(device,
                                                     baseAddr,
                                                     agcConfig[configIndex].agcPeak.hb2UnderRangeMidThreshExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcAdcovrgLowInt0CounterBfSet(device,
                                                     baseAddr,
                                                     agcConfig[configIndex].agcPeak.hb2UnderRangeLowThreshExceededCnt);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* APD Low Frequency MITIGATION Mode Setup */
            static const uint8_t  APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER = 3;
            static const uint8_t  APD_LOW_FREQ_ERROR_MITIGATION_MODE    = 1;
            static const uint16_t APD_LOW_FREQ_THRESH_OFFSET            = 6;

            adrv9025_RxDecimatedDataOverloadSecondaryUpperThresholdBfSet(device,
                                                                         baseAddr,
                                                                         (agcConfig[configIndex].agcPeak.hb2HighThresh - APD_LOW_FREQ_THRESH_OFFSET));
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER;
            adrv9025_RxAdcovrg2ndHighCounterBfSet(device,
                                                  baseAddr,
                                                  bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = APD_LOW_FREQ_ERROR_MITIGATION_MODE;
            adrv9025_RxAgcApdLowFreqErrorMitigationModeBfSet(device,
                                                             baseAddr,
                                                             bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Power Configuration register */
            adrv9025_RxDecPowerEnableMeasBfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPower.powerEnableMeasurement);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecPowerInputSelectBfSet(device,
                                                baseAddr,
                                                agcConfig[configIndex].agcPower.powerInputSelect);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLower0ThresholdBfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPower.underRangeHighPowerThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLower1ThresholdBfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPower.underRangeLowPowerThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLower0ThresholdExceededGainStepBfSet(device,
                                                               baseAddr,
                                                               agcConfig[configIndex].agcPower.underRangeHighPowerGainStepRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLower1ThresholdExceededGainStepBfSet(device,
                                                               baseAddr,
                                                               agcConfig[configIndex].agcPower.underRangeLowPowerGainStepRecovery);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxDecPowerMeasurementDurationBfSet(device,
                                                        baseAddr,
                                                        agcConfig[configIndex].agcPower.powerMeasurementDuration);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUlSigPowerMeasDurationBfSet(device,
                                                      baseAddr,
                                                      agcConfig[configIndex].agcPower.rxTddPowerMeasDuration);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUlSigPowerMeasDelayBfSet(device,
                                                   baseAddr,
                                                   agcConfig[configIndex].agcPower.rxTddPowerMeasDelay);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcLockLevelBfSet(device,
                                         baseAddr,
                                         agcConfig[configIndex].agcPower.overRangeHighPowerThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUpper1ThresholdBfSet(device,
                                               baseAddr,
                                               agcConfig[configIndex].agcPower.overRangeLowPowerThresh);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* agcConfig[configIndex].agcPower.powerLogShift */
            bfValue = 1; /* Fixed to 1 */
            adrv9025_RxDecPowerLogShiftBfSet(device,
                                             baseAddr,
                                             bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUpper0ThresholdExceededGainStepBfSet(device,
                                                               baseAddr,
                                                               agcConfig[configIndex].agcPower.overRangeHighPowerGainStepAttack);
            ADI_ERROR_RETURN(device->common.error.newAction);

            adrv9025_RxAgcUpper1ThresholdExceededGainStepBfSet(device,
                                                               baseAddr,
                                                               agcConfig[configIndex].agcPower.overRangeLowPowerGainStepAttack);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AgcCfgGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_AgcCfg_t*    agcConfig)
{
    adrv9025_BfRxChanAddr_e             baseAddr       = ADRV9025_BF_RX_CH0;
    adrv9025_BfAnalogRxMemMapChanAddr_e baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH0;
    uint8_t                             bfValue        = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        agcConfig);

    if (rxChannel == ADI_ADRV9025_RX1)
    {
        baseAddr       = ADRV9025_BF_RX_CH0;
        baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH0;
    }
    else if (rxChannel == ADI_ADRV9025_RX2)
    {
        baseAddr       = ADRV9025_BF_RX_CH1;
        baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH1;
    }
    else if (rxChannel == ADI_ADRV9025_RX3)
    {
        baseAddr       = ADRV9025_BF_RX_CH2;
        baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH2;
    }
    else if (rxChannel == ADI_ADRV9025_RX4)
    {
        baseAddr       = ADRV9025_BF_RX_CH3;
        baseAddrAnalog = ADRV9025_BF_ANALOG_RX_CH3;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid rxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    agcConfig->rxChannelMask = (uint32_t)rxChannel;

    adrv9025_RxAgcPeakWaitTimeBfGet(device,
                                    baseAddr,
                                    &agcConfig->agcPeakWaitTime);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     baseAddr,
                     "Error while attemtpting to get Rx Agc Config");
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcMaximumGainIndexBfGet(device,
                                        baseAddr,
                                        &agcConfig->agcRxMaxGainIndex);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcMinimumGainIndexBfGet(device,
                                        baseAddr,
                                        &agcConfig->agcRxMinGainIndex);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcGainUpdateCounterBfGet(device,
                                         baseAddr,
                                         &agcConfig->agcGainUpdateCounter);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAttackDelayBfGet(device,
                                   baseAddr,
                                   &agcConfig->agcRxAttackDelay);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* AGC Control register - Slowloop_config*/
    adrv9025_RxAgcEnableFastRecoveryLoopBfGet(device,
                                              baseAddr,
                                              &agcConfig->agcEnableFastRecoveryLoop);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLowThsPreventGainIncBfGet(device,
                                            baseAddr,
                                            &agcConfig->agcLowThreshPreventGainInc);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcChangeGainIfUlbthHighBfGet(device,
                                             baseAddr,
                                             &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    agcConfig->agcChangeGainIfThreshHigh = (bfValue & 0x01);

    adrv9025_RxAgcChangeGainIfAdcovrgHighBfGet(device,
                                               baseAddr,
                                               &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    agcConfig->agcChangeGainIfThreshHigh |= ((bfValue << 1) & 0x02);

    adrv9025_RxAgcPeakThresholdGainControlModeBfGet(device,
                                                    baseAddr,
                                                    &agcConfig->agcPeakThreshGainControlMode);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcResetOnRxonBfGet(device,
                                   baseAddr,
                                   &agcConfig->agcResetOnRxon);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcEnableSyncPulseForGainCounterBfGet(device,
                                                     baseAddr,
                                                     &agcConfig->agcEnableSyncPulseForGainCounter);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcSlowLoopSettlingDelayBfGet(device,
                                             baseAddr,
                                             &agcConfig->agcSlowLoopSettlingDelay);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAdcResetGainStepBfGet(device,
                                        baseAddr,
                                        &agcConfig->agcAdcResetGainStep);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Agc Peak */
    adrv9025_RxAgcUrangeInterval0BfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPeak.agcUnderRangeLowInterval);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUrangeInterval1MultBfGet(device,
                                           baseAddr,
                                           &agcConfig->agcPeak.agcUnderRangeMidInterval);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUrangeInterval2MultBfGet(device,
                                           baseAddr,
                                           &agcConfig->agcPeak.agcUnderRangeHighInterval);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Digital: adrv9025_RxUpperLevelBlockerThresholdBfGet */
    adrv9025_AnalogRxMemMapRxBlockDetUlbthBfGet(device,
                                                baseAddrAnalog,
                                                0,
                                                &agcConfig->agcPeak.apdHighThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Digital: adrv9025_RxUpperLevelBlockerThresholdTiaLowGainModeBfGet */
    adrv9025_AnalogRxMemMapRxBlockDetUlbthBfGet(device,
                                                baseAddrAnalog,
                                                1,
                                                &agcConfig->agcPeak.apdLowGainModeHighThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Digital: adrv9025_RxLowerLevelBlockerThresholdBfGet */
    adrv9025_AnalogRxMemMapRxBlockDetLlbthBfGet(device,
                                                baseAddrAnalog,
                                                0,
                                                &agcConfig->agcPeak.apdLowThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Digital: adrv9025_RxLowerLevelBlockerThresholdTiaLowGainModeBfGet */
    adrv9025_AnalogRxMemMapRxBlockDetLlbthBfGet(device,
                                                baseAddrAnalog,
                                                1,
                                                &agcConfig->agcPeak.apdLowGainModeLowThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUlbThresholdExceededCounterBfGet(device,
                                                   baseAddr,
                                                   &agcConfig->agcPeak.apdUpperThreshPeakExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLlbThresholdExceededCounterBfGet(device,
                                                   baseAddr,
                                                   &agcConfig->agcPeak.apdLowerThreshPeakExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUlbGainStepBfGet(device,
                                   baseAddr,
                                   &agcConfig->agcPeak.apdGainStepAttack);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLlbGainStepBfGet(device,
                                   baseAddr,
                                   &agcConfig->agcPeak.apdGainStepRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*HB2 Configuration register*/
    adrv9025_RxEnableDecOverloadBfGet(device,
                                      baseAddr,
                                      &agcConfig->agcPeak.enableHb2Overload);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecOverloadDurationCountBfGet(device,
                                             baseAddr,
                                             &agcConfig->agcPeak.hb2OverloadDurationCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecOverloadThresholdCountBfGet(device,
                                              baseAddr,
                                              &agcConfig->agcPeak.hb2OverloadThreshCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecimatedDataOverloadUpperThresholdBfGet(device,
                                                        baseAddr,
                                                        &agcConfig->agcPeak.hb2HighThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecimatedDataOverloadInt0LowerThresholdBfGet(device,
                                                            baseAddr,
                                                            &agcConfig->agcPeak.hb2UnderRangeLowThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecimatedDataOverloadInt1LowerThresholdBfGet(device,
                                                            baseAddr,
                                                            &agcConfig->agcPeak.hb2UnderRangeMidThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecimatedDataOverloadLowerThresholdBfGet(device,
                                                        baseAddr,
                                                        &agcConfig->agcPeak.hb2UnderRangeHighThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAdcHighOvrgExceededCounterBfGet(device,
                                                  baseAddr,
                                                  &agcConfig->agcPeak.hb2UpperThreshPeakExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAdcLowOvrgExceededCounterBfGet(device,
                                                 baseAddr,
                                                 &agcConfig->agcPeak.hb2UnderRangeHighThreshExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcOvrgLowGainStepBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPeak.hb2GainStepHighRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcOvrgLowInt0GainStepBfGet(device,
                                           baseAddr,
                                           &agcConfig->agcPeak.hb2GainStepLowRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcOvrgLowInt1GainStepBfGet(device,
                                           baseAddr,
                                           &agcConfig->agcPeak.hb2GainStepMidRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcOvrgHighGainStepBfGet(device,
                                        baseAddr,
                                        &agcConfig->agcPeak.hb2GainStepAttack);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecOverloadPowerModeBfGet(device,
                                         baseAddr,
                                         &agcConfig->agcPeak.hb2OverloadPowerMode);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecThresholdConfigBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPeak.hb2ThreshConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAdcovrgLowInt1CounterBfGet(device,
                                             baseAddr,
                                             &agcConfig->agcPeak.hb2UnderRangeMidThreshExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcAdcovrgLowInt0CounterBfGet(device,
                                             baseAddr,
                                             &agcConfig->agcPeak.hb2UnderRangeLowThreshExceededCnt);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Power Configuration register */
    adrv9025_RxDecPowerEnableMeasBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPower.powerEnableMeasurement);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecPowerInputSelectBfGet(device,
                                        baseAddr,
                                        &agcConfig->agcPower.powerInputSelect);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecPowerLogShiftBfGet(device,
                                     baseAddr,
                                     &agcConfig->agcPower.powerLogShift);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLower0ThresholdBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPower.underRangeHighPowerThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLower1ThresholdBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPower.underRangeLowPowerThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLower0ThresholdExceededGainStepBfGet(device,
                                                       baseAddr,
                                                       &agcConfig->agcPower.underRangeHighPowerGainStepRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLower1ThresholdExceededGainStepBfGet(device,
                                                       baseAddr,
                                                       &agcConfig->agcPower.underRangeLowPowerGainStepRecovery);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxDecPowerMeasurementDurationBfGet(device,
                                                baseAddr,
                                                &agcConfig->agcPower.powerMeasurementDuration);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUlSigPowerMeasDurationBfGet(device,
                                              baseAddr,
                                              &agcConfig->agcPower.rxTddPowerMeasDuration);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUlSigPowerMeasDelayBfGet(device,
                                           baseAddr,
                                           &agcConfig->agcPower.rxTddPowerMeasDelay);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcLockLevelBfGet(device,
                                 baseAddr,
                                 &agcConfig->agcPower.overRangeHighPowerThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUpper1ThresholdBfGet(device,
                                       baseAddr,
                                       &agcConfig->agcPower.overRangeLowPowerThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUpper0ThresholdExceededGainStepBfGet(device,
                                                       baseAddr,
                                                       &agcConfig->agcPower.overRangeHighPowerGainStepAttack);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxAgcUpper1ThresholdExceededGainStepBfGet(device,
                                                       baseAddr,
                                                       &agcConfig->agcPower.overRangeLowPowerGainStepAttack);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AgcDualBandCfgSet(adi_adrv9025_Device_t*        device,
                                       adi_adrv9025_AgcDualBandCfg_t rxAgcCtrlDualBand[],
                                       uint8_t                       arraySize)
{
    UNUSED_PARA(device);
    UNUSED_PARA(rxAgcCtrlDualBand);
    UNUSED_PARA(arraySize);
    return 0;
}

int32_t adi_adrv9025_AgcDualBandCfgGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_RxChannels_e      rxChannel,
                                       adi_adrv9025_AgcDualBandCfg_t* rxAgcCtrlDualBand)
{
    UNUSED_PARA(device);
    UNUSED_PARA(rxChannel);
    UNUSED_PARA(rxAgcCtrlDualBand);
    return 0;
}

int32_t adi_adrv9025_AgcDualBandLnaGpioOutGet(adi_adrv9025_Device_t*         device,
                                              adi_adrv9025_RxChannels_e      rxChannel,
                                              adi_adrv9025_DualBandLnaCfg_t* rxDualBandLnaControls)
{
    UNUSED_PARA(device);
    UNUSED_PARA(rxChannel);
    UNUSED_PARA(rxDualBandLnaControls);
    return 0;
}

int32_t adi_adrv9025_AgcGainIndexRangeSet(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_AgcGainRange_t agcGainRange[],
                                          uint8_t                     arraySize)
{
    adrv9025_BfRxChanAddr_e baseAddr      = ADRV9025_BF_RX_CH0;
    uint8_t                 bfValue       = 0;
    uint8_t                 configIndex   = 0;
    uint8_t                 i             = 0;
    uint32_t                rxChannelMask = 0;
    uint8_t                 maxGainIndex  = 0;
    uint8_t                 minGainIndex  = 0;

    static uint8_t const AGC_RXCHANNEL_MAX = 0x0F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        agcGainRange);

    if (arraySize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid arraySize");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (configIndex = 0; configIndex < arraySize; configIndex++)
    {
        rxChannelMask = agcGainRange[configIndex].rxChannelMask;

        if ((rxChannelMask > AGC_RXCHANNEL_MAX) || (rxChannelMask == 0))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannelMask,
                             "Invalid rxChannelMask. Valid range (1/2/4/8/15)");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Ensure that Max Gain Index is always greater than Min Gain Index */
        maxGainIndex = agcGainRange[configIndex].maxGainIndex;
        minGainIndex = agcGainRange[configIndex].minGainIndex;
        if (minGainIndex >= maxGainIndex)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             maxGainIndex,
                             "Min Gain Index cannot be greater than Max Gain Index Param");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            if ((rxChannelMask & ADI_ADRV9025_RX1) == ADI_ADRV9025_RX1)
            {
                baseAddr = ADRV9025_BF_RX_CH0;
                rxChannelMask &= ~ADI_ADRV9025_RX1;
                /* range check the gain against the max and min expected values */
                if ((maxGainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex)
                    || (minGainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     maxGainIndex,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX2) == ADI_ADRV9025_RX2)
            {
                baseAddr = ADRV9025_BF_RX_CH1;
                rxChannelMask &= ~ADI_ADRV9025_RX2;
                /* range check the gain against the max and min expected values */
                if ((maxGainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex)
                    || (minGainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     maxGainIndex,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX3) == ADI_ADRV9025_RX3)
            {
                baseAddr = ADRV9025_BF_RX_CH2;
                rxChannelMask &= ~ADI_ADRV9025_RX3;
                /* range check the gain against the max and min expected values */
                if ((maxGainIndex > device->devStateInfo.gainIndexes.rx3MaxGainIndex)
                    || (minGainIndex < device->devStateInfo.gainIndexes.rx3MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     maxGainIndex,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxChannelMask & ADI_ADRV9025_RX4) == ADI_ADRV9025_RX4)
            {
                baseAddr = ADRV9025_BF_RX_CH3;
                rxChannelMask &= ~ADI_ADRV9025_RX4;
                /* range check the gain against the max and min expected values */
                if ((maxGainIndex > device->devStateInfo.gainIndexes.rx4MaxGainIndex)
                    || (minGainIndex < device->devStateInfo.gainIndexes.rx4MinGainIndex))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     maxGainIndex,
                                     "Gain index less than minimum index or greater than maximum gain index");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else
            {
                break;
            }

            /* Writing Rx1 max and min gain indices to the SPI registers*/
            bfValue = agcGainRange[configIndex].maxGainIndex;
            adrv9025_RxAgcMaximumGainIndexBfSet(device,
                                                baseAddr,
                                                bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);

            bfValue = agcGainRange[configIndex].minGainIndex;
            adrv9025_RxAgcMinimumGainIndexBfSet(device,
                                                baseAddr,
                                                bfValue);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AgcGainIndexRangeGet(adi_adrv9025_Device_t*       device,
                                          adi_adrv9025_RxChannels_e    rxChannel,
                                          adi_adrv9025_AgcGainRange_t* agcGainRange)
{
    adrv9025_BfRxChanAddr_e baseAddr = ADRV9025_BF_RX_CH0;
    uint8_t                 bfValue  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        agcGainRange);

    if (rxChannel == ADI_ADRV9025_RX1)
    {
        baseAddr = ADRV9025_BF_RX_CH0;
    }
    else if (rxChannel == ADI_ADRV9025_RX2)
    {
        baseAddr = ADRV9025_BF_RX_CH1;
    }
    else if (rxChannel == ADI_ADRV9025_RX3)
    {
        baseAddr = ADRV9025_BF_RX_CH2;
    }
    else if (rxChannel == ADI_ADRV9025_RX4)
    {
        baseAddr = ADRV9025_BF_RX_CH3;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         baseAddr,
                         "Invalid rxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    agcGainRange->rxChannelMask = rxChannel;

    /* Writing Rx1 max and min gain indices to the SPI registers*/
    adrv9025_RxAgcMaximumGainIndexBfGet(device,
                                        baseAddr,
                                        &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    agcGainRange->maxGainIndex = bfValue;

    adrv9025_RxAgcMinimumGainIndexBfGet(device,
                                        baseAddr,
                                        &bfValue);
    ADI_ERROR_RETURN(device->common.error.newAction);
    agcGainRange->minGainIndex = bfValue;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AgcReset(adi_adrv9025_Device_t* device,
                              uint32_t               rxChannelMask)
{
    adrv9025_BfRxChanAddr_e baseAddr = ADRV9025_BF_RX_CH0;
    uint8_t                 i        = 0;

    static const uint8_t AGC_RESET         = 1;
    static const uint8_t AGC_RESET_CLEAR   = 0;
    static const uint8_t AGC_RXCHANNEL_MAX = 0x0F;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if ((rxChannelMask > AGC_RXCHANNEL_MAX) || (rxChannelMask == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Invalid rxChannelMask. Valid range (1/2/4/8/15)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
    {
        if ((rxChannelMask & ADI_ADRV9025_RX1) == ADI_ADRV9025_RX1)
        {
            baseAddr = ADRV9025_BF_RX_CH0;
            rxChannelMask &= ~ADI_ADRV9025_RX1;
        }
        else if ((rxChannelMask & ADI_ADRV9025_RX2) == ADI_ADRV9025_RX2)
        {
            baseAddr = ADRV9025_BF_RX_CH1;
            rxChannelMask &= ~ADI_ADRV9025_RX2;
        }
        else if ((rxChannelMask & ADI_ADRV9025_RX3) == ADI_ADRV9025_RX3)
        {
            baseAddr = ADRV9025_BF_RX_CH2;
            rxChannelMask &= ~ADI_ADRV9025_RX3;
        }
        else if ((rxChannelMask & ADI_ADRV9025_RX4) == ADI_ADRV9025_RX4)
        {
            baseAddr = ADRV9025_BF_RX_CH3;
            rxChannelMask &= ~ADI_ADRV9025_RX4;
        }
        else
        {
            break;
        }

        adrv9025_RxAgcSoftResetBfSet(device,
                                     baseAddr,
                                     AGC_RESET);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxAgcSoftResetBfSet(device,
                                     baseAddr,
                                     AGC_RESET_CLEAR);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AgcOverloadIndicatorGpioSet(adi_adrv9025_Device_t* device,
                                                 uint8_t                overloadEnable)
{
    /* Check that the passed device pointer is not NULL */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    /* Add entry to the API log */
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    uint8_t               sharedResourceAcqReleaseStatus = 0x00;    /* Will be passed as reference, used to confirm that the shared resource has been acquired */
    uint32_t              sharedResourceId               = 0x00;    /* To iterate over ADRV9025_GPIO for loop */
    static const uint32_t gpioOutputMask                 = 0x3FFC0; /* 111111111111000000 mask to set pins to output */
    static const uint32_t gpioSrcCtrl                    = 0xEEEE0; /* 11101110111011100000 sets the output source control for each set of four GPIO pins */
    static const uint32_t gpioSrcCtrlReset               = 0x00000; /* Sets the output source control for each set of four GPIO pins */
    int32_t               recoveryAction                 = ADI_COMMON_ACT_ERR_RESET_FULL;
    adrv9025_FeatureID_e  sharedResourceFeatureId        = ADRV9025_FEATURE_UNUSED;

    int32_t sharedResourceArr[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

    if (overloadEnable)
    {
        /* Iterate throught pins ADRV9025_GPIO_06 to ADRV9025_GPIO_17 */
        /* Get the current shared resource feature ID of each pin */
        /* If the pin is not in an unused state i.e ADRV9025_FEATURE_UNUSED */
        /* Set feature flag to false and break from the loop */
        for (sharedResourceId = (uint32_t)ADRV9025_GPIO_06; sharedResourceId < (uint32_t)ADRV9025_GPIO_18; sharedResourceId++)
        {
            adrv9025_SharedResourceFeatureGet(device, (adrv9025_SharedResourceID_e)sharedResourceId, &sharedResourceFeatureId);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* The calling application is responsible for setting the shared resource to ADRV9025_FEATURE_UNUSED */
            /* If the shared resource is not ADRV9025_FEATURE_UNUSED, API can not proceed */
            if (sharedResourceFeatureId != ADRV9025_FEATURE_UNUSED)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 sharedResourceAcqReleaseStatus,
                                 "GPIO pins are not in ADRV9025_FEATURE_UNUSED state");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Acquire the shared resource ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION */
        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         ADI_ADRV9025_NUM_GPIOS_IN_AGCOOVERLOADINDICATOR_FEATURE,
                                                         ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION,
                                                         &sharedResourceAcqReleaseStatus);

        /* Confirm that the shared resource has been acquired */
        /* If the shared resource has not been acquired - exception will be thrown */
        if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             sharedResourceAcqReleaseStatus,
                             "Failed to acquire the shared rescource for GPIO overload");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Set the direction of all pins to output */
        if (adi_adrv9025_GpioOutputDirSet(device, gpioOutputMask) != ADI_COMMON_ACT_NO_ACTION)
        {
            /* Failed to set the direction of the GPIOs */
            /* Release the shared resource ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             ADI_ADRV9025_NUM_GPIOS_IN_AGCOOVERLOADINDICATOR_FEATURE,
                                                             ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION,
                                                             &sharedResourceAcqReleaseStatus);

            /* Check the status after attempting the release */
            if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
            {
                /* GPIO output direction failed and the feature has failed to be released from shared resource */
                /* ADI_COMMON_ACT_ERR_RESET_FULL is now required to release the shared resource and set ADRV9025_FEATURE_UNUSED */
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_FULL,
                                 sharedResourceAcqReleaseStatus,
                                 "Unable to set GPIO output direction & Failed to release Overload GPIO feature from shared resource");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             dpdCaptureDoneStatusPin,
                             "Unable to set GPIO output direction for GPIO overload status indication");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adi_adrv9025_GpioOutSourceCtrlSet(device, gpioSrcCtrl);

        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            /* Release the shared resource ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             ADI_ADRV9025_NUM_GPIOS_IN_AGCOOVERLOADINDICATOR_FEATURE,
                                                             ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION,
                                                             &sharedResourceAcqReleaseStatus);

            /* Check the status after attempting the release */
            if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
            {
                /* GPIO output source control set has failed and the feature has failed to be released from shared resource */
                /* ADI_COMMON_ACT_ERR_RESET_FULL is now required to release the shared resource and set ADRV9025_FEATURE_UNUSED */
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_FULL,
                                 sharedResourceAcqReleaseStatus,
                                 "Failed to set GPIO output source control set & Failed to release Overload GPIO feature from shared resource");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             gpioSrcCtrl,
                             "Failed to set GPIO output source control");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    } // end of if (overloadEnable)
    else
    {
        /* Iterate throught pins ADRV9025_GPIO_06 to ADRV9025_GPIO_17 */
        /* Get the current shared resource feature ID of each pin */
        /* If the pin is not in an overload feature state i.e ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION */
        /* Throw exception */
        for (sharedResourceId = (uint32_t)ADRV9025_GPIO_06; sharedResourceId < (uint32_t)ADRV9025_GPIO_18; sharedResourceId++)
        {
            adrv9025_SharedResourceFeatureGet(device, (adrv9025_SharedResourceID_e)sharedResourceId, &sharedResourceFeatureId);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* If the shared resource is not ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION, API can not proceed */
            if (sharedResourceFeatureId != ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 sharedResourceAcqReleaseStatus,
                                 "GPIO pins are not in ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION state, API cannot release feature");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Release the shared resource ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION */
        recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         ADI_ADRV9025_NUM_GPIOS_IN_AGCOOVERLOADINDICATOR_FEATURE,
                                                         ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION,
                                                         &sharedResourceAcqReleaseStatus);

        /* Check the status after attempting the release */
        if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             sharedResourceAcqReleaseStatus,
                             "Failed to release Overload GPIO feature from shared resource");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        recoveryAction = adi_adrv9025_GpioOutSourceCtrlSet(device, gpioSrcCtrlReset);

        if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             gpioSrcCtrlReset,
                             "Failed to set GPIO output source control");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    return (device->common.error.newAction);
}