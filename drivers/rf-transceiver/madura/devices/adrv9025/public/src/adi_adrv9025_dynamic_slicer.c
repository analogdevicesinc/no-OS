// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_dynamic_slicer.c
* \brief Contains dynamic slicer function implementation defined in adi_adrv9025_dynamic_slicer.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_dynamic_slicer.h"
#include "adi_adrv9025_hal.h"
#include "../../private/include/adrv9025_dynamic_slicer.h"
#include "../../private/include/adrv9025_rx.h"
#include "../../private/include/adrv9025_bf_rx.h"
#include "../../private/include/adrv9025_bf_orx.h"

#ifdef _RELEASE_BUILD_
#line __LINE__ "adi_adrv9025_dynamic_slicer.c"
#endif

int32_t adi_adrv9025_RxDynamicSlicerConfigSet(adi_adrv9025_Device_t*               device,
                                              adi_adrv9025_RxDynamicSlicerConfig_t rxDynamicSlicerCfg[],
                                              uint8_t                              arraySize)
{
    uint8_t                              rxDynamicSlicerConfigIndex = 0;
    uint8_t                              intDataResolution          = 0;
    uint8_t                              intDataFormat              = 0;
    adi_adrv9025_IntegerConfigSettings_t integerConfigSettings      = {ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx data format config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDynamicSlicerCfg);

#if ADI_ADRV9025_RX_RANGE_CHECK > 0
    adrv9025_RxDynamicSlicerConfigSetRangeCheck(device,
                                                rxDynamicSlicerCfg,
                                                arraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (rxDynamicSlicerConfigIndex = 0; rxDynamicSlicerConfigIndex < arraySize; rxDynamicSlicerConfigIndex++)
    {
        /* Resolve integer sample resolution and data format */
        integerConfigSettings.intSampleResolution = rxDynamicSlicerCfg[rxDynamicSlicerConfigIndex].intSampleResolution;
        adrv9025_RxIntSampleResFormatResolve(device,
                                             &integerConfigSettings,
                                             &intDataResolution,
                                             &intDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Disable gain compensation and reset the formatter settings to default */
        adrv9025_RxDataFormatGainCompDisable(device,
                                             rxDynamicSlicerCfg[rxDynamicSlicerConfigIndex].rxChannelMask,
                                             intDataFormat,
                                             intDataResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Setup Dynamic slicer config */
        adrv9025_RxDynamicSlicerEnableSet(device,
                                          rxDynamicSlicerCfg,
                                          rxDynamicSlicerCfg[rxDynamicSlicerConfigIndex].dynamicSlicerEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxDynamicSlicerConfigGet(adi_adrv9025_Device_t*                device,
                                              adi_adrv9025_RxChannels_e             rxChannel,
                                              adi_adrv9025_RxDynamicSlicerConfig_t* rxDynamicSlicerCfg)
{
    static const uint32_t ALL_RX_MASK  = (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK = (ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    uint8_t                  intDataResolution         = 0;
    uint8_t                  intDataFormat             = 0;
    uint8_t                  intEmbedSlicerPos         = 0;
    uint8_t                  intEmbedSlicerNumber      = 0;
    uint8_t                  intSlicerStepSize         = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddress  = (adrv9025_BfRxChanAddr_e)0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddress = (adrv9025_BfOrxChanAddr_e)0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check Rx dynamic slicer config pointer is not NULL*/
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDynamicSlicerCfg);

    /*Check that the requested rxChannel is exactly 1 valid channel*/
    if ((rxChannel != ADI_ADRV9025_RX1) &&
        (rxChannel != ADI_ADRV9025_RX2) &&
        (rxChannel != ADI_ADRV9025_RX3) &&
        (rxChannel != ADI_ADRV9025_RX4) &&
        (rxChannel != ADI_ADRV9025_ORX1) &&
        (rxChannel != ADI_ADRV9025_ORX2) &&
        (rxChannel != ADI_ADRV9025_ORX3) &&
        (rxChannel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx channel encountered while attempting to retrieve the dynamic slicer configuration");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((ALL_RX_MASK & (uint32_t)rxChannel) == (uint32_t)rxChannel)
    {
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitfieldAddress);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxDynamicSlicerModeEnBfGet(device,
                                            rxChannelBitfieldAddress,
                                            &rxDynamicSlicerCfg->dynamicSlicerEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxDynamicSlicerAgcSyncEnableBfGet(device,
                                                   rxChannelBitfieldAddress,
                                                   &rxDynamicSlicerCfg->dynamicSlicerAgcSyncEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxDynamicSlicerSyncHeadBfGet(device,
                                              rxChannelBitfieldAddress,
                                              &rxDynamicSlicerCfg->dynamicSlicerSyncHeader);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxIntDataResolutionBfGet(device,
                                          rxChannelBitfieldAddress,
                                          &intDataResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxIntDataFormatBfGet(device,
                                      rxChannelBitfieldAddress,
                                      &intDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxIntEmbedSlicerNumberBfGet(device,
                                             rxChannelBitfieldAddress,
                                             &intEmbedSlicerNumber);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxIntEmbedSlicerPosBfGet(device,
                                          rxChannelBitfieldAddress,
                                          &intEmbedSlicerPos);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxSlicerPinControlStepBfGet(device,
                                             rxChannelBitfieldAddress,
                                             &intSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((ALL_ORX_MASK & (uint32_t)rxChannel) == (uint32_t)rxChannel)
    {
        adrv9025_OrxBitfieldAddressGet(device,
                                       rxChannel,
                                       &orxChannelBitfieldAddress);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxDynamicSlicerModeEnBfGet(device,
                                             orxChannelBitfieldAddress,
                                             &rxDynamicSlicerCfg->dynamicSlicerEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxDynamicSlicerAgcSyncEnableBfGet(device,
                                                    orxChannelBitfieldAddress,
                                                    &rxDynamicSlicerCfg->dynamicSlicerAgcSyncEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxDynamicSlicerSyncHeadBfGet(device,
                                               orxChannelBitfieldAddress,
                                               &rxDynamicSlicerCfg->dynamicSlicerSyncHeader);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxIntDataResolutionBfGet(device,
                                           orxChannelBitfieldAddress,
                                           &intDataResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxIntDataFormatBfGet(device,
                                       orxChannelBitfieldAddress,
                                       &intDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxIntEmbedSlicerNumberBfGet(device,
                                              orxChannelBitfieldAddress,
                                              &intEmbedSlicerNumber);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxIntEmbedSlicerPosBfGet(device,
                                           orxChannelBitfieldAddress,
                                           &intEmbedSlicerPos);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxSlicerPinControlStepBfGet(device,
                                              orxChannelBitfieldAddress,
                                              &intSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adrv9025_RxIntSampleResFormatEncode(device,
                                        intDataResolution,
                                        intDataFormat,
                                        &rxDynamicSlicerCfg->intSampleResolution);
    ADI_ERROR_RETURN(device->common.error.newAction);

    adrv9025_RxIntEmbSlicerConfigEncode(device,
                                        rxChannel,
                                        rxDynamicSlicerCfg->dynamicSlicerEnable,
                                        intEmbedSlicerPos,
                                        intEmbedSlicerNumber,
                                        &rxDynamicSlicerCfg->intEmbeddedBits);
    ADI_ERROR_RETURN(device->common.error.newAction);

    rxDynamicSlicerCfg->intSlicerStepSize = (adi_adrv9025_IntSlicerStepSizes_e)intSlicerStepSize;
    rxDynamicSlicerCfg->rxChannelMask     = (uint32_t)rxChannel;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_RxDynamicSlicerDataResend(adi_adrv9025_Device_t* device,
                                               uint32_t               rxChannelMask)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]               = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]              = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint8_t                   DYNAMIC_SLICER_RESEND_ASSERT   = 1;
    static const uint8_t                   DYNAMIC_SLICER_RESEND_DEASSERT = 0;

    uint8_t                              rxChannelIndex            = 0;
    adrv9025_BfRxChanAddr_e              rxChannelBitfieldAddress  = (adrv9025_BfRxChanAddr_e)0;
    adrv9025_BfOrxChanAddr_e             orxChannelBitfieldAddress = (adrv9025_BfOrxChanAddr_e)0;
    adi_adrv9025_RxDynamicSlicerConfig_t rxDynamicSlicerConfig     = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0])); rxChannelIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelIndex])
        {
            adi_adrv9025_RxDynamicSlicerConfigGet(device,
                                                  RX_CHANNEL_ARR[rxChannelIndex],
                                                  &rxDynamicSlicerConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (rxDynamicSlicerConfig.dynamicSlicerEnable == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxDynamicSlicerConfig.dynamicSlicerEnable,
                                 "Dynamic slicer mode needs to be enabled to issue a dynamic slicer data resend request");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Resolve the baseaddress of the target Rx channel */
            adrv9025_RxBitfieldAddressGet(device,
                                          RX_CHANNEL_ARR[rxChannelIndex],
                                          &rxChannelBitfieldAddress);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Assert the dynamic slicer data resend signal */
            adrv9025_RxDynamicSlicerResendBfSet(device,
                                                rxChannelBitfieldAddress,
                                                DYNAMIC_SLICER_RESEND_ASSERT);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Deassert the dynamic slicer data resend signal */
            adrv9025_RxDynamicSlicerResendBfSet(device,
                                                rxChannelBitfieldAddress,
                                                DYNAMIC_SLICER_RESEND_DEASSERT);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    for (rxChannelIndex = 0; rxChannelIndex < (sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0])); rxChannelIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelIndex])
        {
            adi_adrv9025_RxDynamicSlicerConfigGet(device,
                                                  ORX_CHANNEL_ARR[rxChannelIndex],
                                                  &rxDynamicSlicerConfig);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (rxDynamicSlicerConfig.dynamicSlicerEnable == 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxDynamicSlicerConfig.dynamicSlicerEnable,
                                 "Dynamic slicer mode needs to be enabled to issue a dynamic slicer data resend request");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Resolve the baseaddress of the target ORx channel */
            adrv9025_OrxBitfieldAddressGet(device,
                                           ORX_CHANNEL_ARR[rxChannelIndex],
                                           &orxChannelBitfieldAddress);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Assert the dynamic slicer data resend signal */
            adrv9025_OrxDynamicSlicerResendBfSet(device,
                                                 orxChannelBitfieldAddress,
                                                 DYNAMIC_SLICER_RESEND_ASSERT);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Deassert the dynamic slicer data resend signal */
            adrv9025_OrxDynamicSlicerResendBfSet(device,
                                                 orxChannelBitfieldAddress,
                                                 DYNAMIC_SLICER_RESEND_DEASSERT);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return (device->common.error.newAction);
}
