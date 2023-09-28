// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_dynamic_slicer.c
* \brief Contains ADRV9025 dynamic slicer related private function implementation
*        defined in adrv9025_dynamic_slicer.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_error.h"
#include "../../private/include/adrv9025_dynamic_slicer.h"
#include "../../private/include/adrv9025_rx.h"
#include "../../private/include/adrv9025_bf_rx.h"
#include "../../private/include/adrv9025_bf_orx.h"

int32_t adrv9025_RxDynamicSlicerConfigSetRangeCheck(adi_adrv9025_Device_t*               device,
                                                    adi_adrv9025_RxDynamicSlicerConfig_t rxDynamicSlicerConfig[],
                                                    uint8_t                              arraySize)
{
    int32_t recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dynamicSlicerCfgIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dynamic slicer configuration struct array is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDynamicSlicerConfig);

    for (dynamicSlicerCfgIndex = 0; dynamicSlicerCfgIndex < arraySize; dynamicSlicerCfgIndex++)
    {
        if ((rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_2SCOMP) &&
            (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_SIGNED) &&
            (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_2SCOMP) &&
            (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
            (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_2SCOMP) &&
            (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution,
                             "Invalid integer sample resolution encountered during dynamic slicer config");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].dynamicSlicerEnable > 0)
        {
            if ((rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution == ADI_ADRV9025_INTEGER_16BIT_SIGNED) ||
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution == ADI_ADRV9025_INTEGER_12BIT_SIGNED))
            {
                if ((rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intEmbeddedBits != ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB) &&
                    (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intEmbeddedBits != ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution,
                                     "Invalid integer embedded bits config encountered during dynamic slicer config. For dyamic slicer 16/12 bit signed format 1 dynamic slicer bit at (MSB-1)/LSB can be embedded");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
            else if ((rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution == ADI_ADRV9025_INTEGER_16BIT_2SCOMP) ||
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution == ADI_ADRV9025_INTEGER_12BIT_2SCOMP))
            {
                if (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intEmbeddedBits != ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSampleResolution,
                                     "Invalid integer embedded bits config encountered during dynamic slicer config. For dyamic slicer 16/12 bit 2s complement format 1 dynamic bit at LSB can be embedded");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /* Range check the slicer step size */
            if ((rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_1DB) &&
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_2DB) &&
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_3DB) &&
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_4DB) &&
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_6DB) &&
                (rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_8DB))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxDynamicSlicerConfig[dynamicSlicerCfgIndex].intSlicerStepSize,
                                 "Invalid slicer step size encountered while attempting to configure the dynamic slicer. Valid step sizes are 1dB,2dB,3dB,4dB,6dB and 8dB");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDynamicSlicerEnableSet(adi_adrv9025_Device_t*                device,
                                          adi_adrv9025_RxDynamicSlicerConfig_t* dynamicSlicerCfg,
                                          uint8_t                               enable)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]                = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]               = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint8_t                   GAIN_COMP_ENABLE                = 1;
    static const uint8_t                   DYNAMIC_SLICER_MODE_ENABLE      = 1;
    static const uint8_t                   MAX_SLICER_VAL_OVERRIDE_ENABLE  = 1;
    static const uint8_t                   GAIN_COMP_DISABLE               = 0;
    static const uint8_t                   DYNAMIC_SLICER_MODE_DISABLE     = 0;
    static const uint8_t                   MAX_SLICER_VAL_OVERRIDE_DISABLE = 0;

    int32_t                              recoveryAction            = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                              rxChannelIndex            = 0;
    uint8_t                              intDataResolution         = 0;
    uint8_t                              intDataFormat             = 0;
    uint8_t                              intEmbedSlicer            = 0;
    uint8_t                              intEmbedSlicerPos         = 0;
    uint8_t                              intEmbedSlicerNumber      = 0;
    uint8_t                              staticSlicer3BitModeEn    = 0;
    uint8_t                              dynamicSlicerModeEn       = 0;
    uint8_t                              gainCompEn                = 0;
    uint8_t                              maxSlicerOverrideEnable   = 0;
    adrv9025_BfRxChanAddr_e              rxChannelBitfieldAddress  = (adrv9025_BfRxChanAddr_e)0;
    adrv9025_BfOrxChanAddr_e             orxChannelBitfieldAddress = (adrv9025_BfOrxChanAddr_e)0;
    adi_adrv9025_IntegerConfigSettings_t integerConfigSettings     = {ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dynamicSlicerCfg struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        dynamicSlicerCfg);

    /*Resolve Sample Resolution, int embedded bits and format*/
    integerConfigSettings.intSampleResolution = dynamicSlicerCfg->intSampleResolution;
    recoveryAction                            = adrv9025_RxIntSampleResFormatResolve(device,
                                                                                     &integerConfigSettings,
                                                                                     &intDataResolution,
                                                                                     &intDataFormat);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (enable > 0)
    {
        dynamicSlicerModeEn     = DYNAMIC_SLICER_MODE_ENABLE;
        gainCompEn              = GAIN_COMP_ENABLE;
        maxSlicerOverrideEnable = MAX_SLICER_VAL_OVERRIDE_ENABLE;
        recoveryAction          = adrv9025_RxIntEmbSlicerConfigResolve(device,
                                                                       ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO,
                                                                       dynamicSlicerCfg->intEmbeddedBits,
                                                                       &intEmbedSlicer,
                                                                       &intEmbedSlicerPos,
                                                                       &intEmbedSlicerNumber,
                                                                       &staticSlicer3BitModeEn);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        dynamicSlicerModeEn     = DYNAMIC_SLICER_MODE_DISABLE;
        gainCompEn              = GAIN_COMP_DISABLE;
        maxSlicerOverrideEnable = MAX_SLICER_VAL_OVERRIDE_DISABLE;
        recoveryAction          = adrv9025_RxIntEmbSlicerConfigResolve(device,
                                                                       ADI_ADRV9025_GAIN_COMPENSATION_DISABLED,
                                                                       ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS,
                                                                       &intEmbedSlicer,
                                                                       &intEmbedSlicerPos,
                                                                       &intEmbedSlicerNumber,
                                                                       &staticSlicer3BitModeEn);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0])); rxChannelIndex++)
    {
        if ((dynamicSlicerCfg->rxChannelMask & RX_CHANNEL_ARR[rxChannelIndex]) != ADI_ADRV9025_RXOFF)
        {
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelIndex],
                                                           &rxChannelBitfieldAddress);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxGainCompEnableBfSet(device,
                                                            rxChannelBitfieldAddress,
                                                            gainCompEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxDynamicSlicerModeEnBfSet(device,
                                                                 rxChannelBitfieldAddress,
                                                                 dynamicSlicerModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxDynamicSlicerSyncHeadBfSet(device,
                                                                   rxChannelBitfieldAddress,
                                                                   dynamicSlicerCfg->dynamicSlicerSyncHeader);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxDynamicSlicerAgcSyncEnableBfSet(device,
                                                                        rxChannelBitfieldAddress,
                                                                        dynamicSlicerCfg->dynamicSlicerAgcSyncEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntDataResolutionBfSet(device,
                                                               rxChannelBitfieldAddress,
                                                               intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntDataFormatBfSet(device,
                                                           rxChannelBitfieldAddress,
                                                           intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEmbedSlicerNumberBfSet(device,
                                                                  rxChannelBitfieldAddress,
                                                                  intEmbedSlicerNumber);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEmbedSlicerPosBfSet(device,
                                                               rxChannelBitfieldAddress,
                                                               intEmbedSlicerPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxSlicerPinControlStepBfSet(device,
                                                                  rxChannelBitfieldAddress,
                                                                  (uint8_t)dynamicSlicerCfg->intSlicerStepSize);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxMaxSlicerOverrideBfSet(device,
                                                               rxChannelBitfieldAddress,
                                                               maxSlicerOverrideEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0])); rxChannelIndex++)
    {
        if ((dynamicSlicerCfg->rxChannelMask & ORX_CHANNEL_ARR[rxChannelIndex]) == ORX_CHANNEL_ARR[rxChannelIndex])
        {
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelIndex],
                                                            &orxChannelBitfieldAddress);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxGainCompEnableBfSet(device,
                                                             orxChannelBitfieldAddress,
                                                             gainCompEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxDynamicSlicerModeEnBfSet(device,
                                                                  orxChannelBitfieldAddress,
                                                                  dynamicSlicerModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxDynamicSlicerSyncHeadBfSet(device,
                                                                    orxChannelBitfieldAddress,
                                                                    dynamicSlicerCfg->dynamicSlicerSyncHeader);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxDynamicSlicerAgcSyncEnableBfSet(device,
                                                                         orxChannelBitfieldAddress,
                                                                         dynamicSlicerCfg->dynamicSlicerAgcSyncEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntDataResolutionBfSet(device,
                                                                orxChannelBitfieldAddress,
                                                                intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntDataFormatBfSet(device,
                                                            orxChannelBitfieldAddress,
                                                            intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEmbedSlicerNumberBfSet(device,
                                                                   orxChannelBitfieldAddress,
                                                                   intEmbedSlicerNumber);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEmbedSlicerPosBfSet(device,
                                                                orxChannelBitfieldAddress,
                                                                intEmbedSlicerPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxSlicerPinControlStepBfSet(device,
                                                                   orxChannelBitfieldAddress,
                                                                   (uint8_t)dynamicSlicerCfg->intSlicerStepSize);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxMaxSlicerOverrideBfSet(device,
                                                                orxChannelBitfieldAddress,
                                                                maxSlicerOverrideEnable);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}
