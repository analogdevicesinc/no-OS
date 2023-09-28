// SPDX-License-Identifier: GPL-2.0
/**
 * \file adrv9025_rx.c
 * \brief Contains ADRV9025 Rx related private function implementations
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "../../private/include/adrv9025_rx.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_bf_pll_mem_map.h"
#include "../../private/include/adrv9025_bf_analog_orx_mem_map.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_gpio.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "../../private/include/adrv9025_init.h"

#define ADI_ADRV9025_NUM_GPIOS_RX_INT_SLICER_CTRL_OUT_FEATURE 4
#define ADI_ADRV9025_NUM_GPIOS_RX_EXT_SLICER_CTRL_WORD_FEATURE 3

int32_t adrv9025_RxGainTableFormat(adi_adrv9025_Device_t*         device,
                                   adi_adrv9025_RxGainTableRow_t* gainTablePtr,
                                   uint8_t*                       formattedGainTablePtr,
                                   uint16_t                       numGainIndicesInTable)
{
    static const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8;
    static const uint16_t DIG_GAIN_LSB_MASK              = 0x00FF;
    static const uint16_t DIG_GAIN_MSB_MASK              = 0x0300;
    static const uint8_t  DIG_GAIN_MSB_SHIFT             = 8;
    static const uint16_t DIG_GAIN_SIGN_MASK             = 0x0400;
    static const uint8_t  DIG_GAIN_SIGN_SHIFT            = 8;
    static const uint8_t  RXFE_GAIN_LSB_MASK             = 0x1F;
    static const uint8_t  RXFE_GAIN_LSB_SHIFT            = 3;
    static const uint8_t  RXFE_GAIN_MSB_MASK             = 0xE0;
    static const uint8_t  RXFE_GAIN_MSB_SHIFT            = 5;
    static const uint16_t PHASE_OFFSET_LSB_MASK          = 0x00FF;
    static const uint16_t PHASE_OFFSET_MSB_MASK          = 0xFF00;
    static const uint8_t  PHASE_OFFSET_MSB_SHIFT         = 8;
    static const uint8_t  EXT_CTRL_MASK                  = 0x03;
    static const uint8_t  ADC_TIA_GAIN_MASK              = 0x03;
    static const uint8_t  ADC_TIA_GAIN_SHIFT             = 2;

    int32_t  recoveryAction     = ADI_COMMON_ACT_NO_ACTION;
    uint32_t gainIndex          = 0;
    uint32_t formattedGainIndex = 0;
    uint16_t sign               = 0;
    uint16_t magnitude          = 0;

    /*Perform NULL checks on pointers*/
    if (gainTablePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gainTablePtr,
                         "Null gainTablePtr encountered");
        return device->common.error.newAction;
    }

    if (formattedGainTablePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         formattedGainTablePtr,
                         "Null formattedGainTablePtr encountered");
        return device->common.error.newAction;
    }

    /*Format gain table*/
    for (gainIndex = 0; gainIndex < numGainIndicesInTable; gainIndex++)
    {
        /*Format Gain Table Data as follows :
        No. of Bytes per Gain Table Entry = 8
        @Addr 8 x gainIndex     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
        @Addr 8 x gainIndex + 4 : {12'b0, tia_control, adc_control, ext_control[1:0], phase_offset[15:0]}
        */
        formattedGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);

        if (gainTablePtr[gainIndex].digGain < 0)
        {
            sign      = DIG_GAIN_SIGN_MASK; /* set bit 11 */
            magnitude = (((~(uint16_t)gainTablePtr[gainIndex].digGain) + 1) & 0x3FF);
        }
        else
        {
            sign      = 0;
            magnitude = gainTablePtr[gainIndex].digGain & 0x3FF;
        }

        formattedGainTablePtr[formattedGainIndex]     = (uint8_t)(magnitude & DIG_GAIN_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 1] = (uint8_t)((sign & DIG_GAIN_SIGN_MASK) >> DIG_GAIN_SIGN_SHIFT) |
        (uint8_t)((magnitude & DIG_GAIN_MSB_MASK) >> DIG_GAIN_MSB_SHIFT) |
        (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_LSB_MASK) << RXFE_GAIN_LSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 2] = (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_MSB_MASK) >> RXFE_GAIN_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 3] = 0;
        formattedGainTablePtr[formattedGainIndex + 4] = (uint8_t)(gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 5] = (uint8_t)((gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_MSB_MASK) >> PHASE_OFFSET_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 6] = (uint8_t)(gainTablePtr[gainIndex].extControl & EXT_CTRL_MASK) |
        (uint8_t)((gainTablePtr[gainIndex].adcTiaGain & ADC_TIA_GAIN_MASK) << ADC_TIA_GAIN_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 7] = 0;
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainTableParse(adi_adrv9025_Device_t*         device,
                                  adi_adrv9025_RxGainTableRow_t* gainTablePtr,
                                  uint8_t*                       armDmaDataGainTablePtr,
                                  uint16_t                       numGainIndicesInTable)
{
    static const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8;
    static const uint16_t DIG_GAIN_MSB_MASK              = 0x0003;
    static const uint8_t  DIG_GAIN_MSB_SHIFT             = 8;
    static const uint8_t  RXFE_GAIN_LSB_MASK             = 0xF8;
    static const uint8_t  RXFE_GAIN_LSB_SHIFT            = 3;
    static const uint8_t  RXFE_GAIN_MSB_MASK             = 0x07;
    static const uint8_t  RXFE_GAIN_MSB_SHIFT            = 5;
    static const uint8_t  PHASE_OFFSET_MSB_SHIFT         = 8;
    static const uint8_t  EXT_CTRL_MASK                  = 0x03;
    static const uint8_t  ADC_TIA_GAIN_MASK              = 0x0C;
    static const uint8_t  ADC_TIA_GAIN_SHIFT             = 2;
    /*11th bit for digital gain is the sign bit*/
    static const uint8_t DIG_GAIN_SIGN_MASK = 0x04;
    static const int16_t DIG_GAIN_SIGN_EXT  = 0xFC00;

    int32_t  recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint32_t gainIndex           = 0;
    uint32_t armDmaDataGainIndex = 0;
    int16_t  digGain             = 0;
    uint16_t magnitude           = 0;

    /*Perform NULL checks on pointers*/
    if (gainTablePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         gainTablePtr,
                         "Null gainTablePtr encountered");
        return device->common.error.newAction;
    }

    if (armDmaDataGainTablePtr == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_NULL_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armDmaDataGainTablePtr,
                         "Null armDmaDataGainTablePtr encountered");
        return device->common.error.newAction;
    }

    /*Parse Gain Table Data in ARM DMA format and store it in gain table row struct*/
    for (gainIndex = 0; gainIndex < numGainIndicesInTable; gainIndex++)
    {
        /*Parse Gain Table Data :
        No. of Bytes per Gain Table Entry = 8
        @armDmaDataGainIndex[8 x gainIndex]    : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
        @armDmaDataGainIndex[8 x gainIndex + 4]: {12'b0, tia_control, adc_control, ext_control[1:0], phase_offset[15:0]}
        */
        armDmaDataGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);

        magnitude = ((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex] |
            (((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & DIG_GAIN_MSB_MASK) << DIG_GAIN_MSB_SHIFT));
        if ((armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & DIG_GAIN_SIGN_MASK) == DIG_GAIN_SIGN_MASK)
        {
            /*Since digGain is a 11bit no. for ARM, 11th bit is the signed bit. However since it is of signed half word type for API use, hence has to be sign extended*/
            digGain = (int16_t)(~(uint16_t)(magnitude - 1));
            digGain |= DIG_GAIN_SIGN_EXT;
        }
        else
        {
            digGain = (int16_t)magnitude;
        }

        gainTablePtr[gainIndex].digGain  = digGain;
        gainTablePtr[gainIndex].rxFeGain = (((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & RXFE_GAIN_LSB_MASK) >> RXFE_GAIN_LSB_SHIFT) |
        (((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 2] & RXFE_GAIN_MSB_MASK) << RXFE_GAIN_MSB_SHIFT);
        gainTablePtr[gainIndex].phaseOffset = (uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 4] |
        ((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 5] << PHASE_OFFSET_MSB_SHIFT);
        gainTablePtr[gainIndex].extControl = (uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 6] & EXT_CTRL_MASK;
        gainTablePtr[gainIndex].adcTiaGain = ((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 6] & ADC_TIA_GAIN_MASK) >> ADC_TIA_GAIN_SHIFT;
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainTableReadParamsCompute(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_RxChannels_e rxChannel,
                                              uint16_t                  maxReadGainIndices,
                                              uint8_t                   gainIndexOffset,
                                              uint16_t*                 numGainIndicesToRead,
                                              uint32_t*                 baseAddr)
{
    static const uint32_t NUM_BYTES_PER_RX_GAIN_INDEX = 8;

    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t maxGainIndices = 0;
    uint32_t baseIndex      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Check that numGainIndicesToRead is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        numGainIndicesToRead);

    /*Check that baseAddr is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        baseAddr);

    /*Calculate base index for the config*/
    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.rx1MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_RX1_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.rx2MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_RX2_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    case ADI_ADRV9025_RX3:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.rx3MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_RX3_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.rx4MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_RX4_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    case ADI_ADRV9025_ORX1: /*Fall Through*/
    case ADI_ADRV9025_ORX2:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.orx1orx2MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_ORX1_ORX2_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    case ADI_ADRV9025_ORX3: /*Fall Through*/
    case ADI_ADRV9025_ORX4:
    {
        maxGainIndices = (gainIndexOffset - device->devStateInfo.gainIndexes.orx3orx4MinGainIndex) + 1;

        if (maxReadGainIndices >= maxGainIndices)
        {
            *numGainIndicesToRead = maxGainIndices;
        }
        else
        {
            *numGainIndicesToRead = maxReadGainIndices;
        }

        baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1));
        *baseAddr = (uint32_t)ADI_ADRV9025_ORX3_ORX4_GAIN_TABLE_BASEADDR + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainCtrlPinReportError(adi_adrv9025_Device_t* device,
                                          int32_t                sharedResourceArr[],
                                          int32_t                recoveryAction,
                                          const char*            customError)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                       = ADRV9025_FEATURE_RX_GAIN_CTRL_PIN;
    static const uint32_t             NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE = 2U;

    int32_t recAct                = ADI_COMMON_ACT_NO_ACTION;
    uint8_t resourceReleaseStatus = ADI_FAILURE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        sharedResourceArr);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recAct = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recAct = adrv9025_SharedResourcesRelease(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE,
                                                 THIS_FEATURE_ID,
                                                 &resourceReleaseStatus);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resouce for GPIO gain ctrl Increment/Decrement Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         customError);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recAct;
}

int32_t adrv9025_RxExtCtrlPinReportError(adi_adrv9025_Device_t* device,
                                         int32_t                sharedResourceArr[],
                                         uint8_t                numSharedResources,
                                         int32_t                recoveryAction,
                                         const char*            customError)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT;

    int32_t recAct                = ADI_COMMON_ACT_NO_ACTION;
    uint8_t resourceReleaseStatus = ADI_FAILURE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        sharedResourceArr);

    /*If error encountered at any point during API execution, release acquired shared resource*/
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recAct = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recAct = adrv9025_SharedResourcesRelease(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 numSharedResources,
                                                 THIS_FEATURE_ID,
                                                 &resourceReleaseStatus);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release shared Analog GPIO during enabling of Rx ext ctrl output. Please Check if the GPIOs in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         customError);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recAct;
}

int32_t adrv9025_RxDualbandLnaCtrlPinReportError(adi_adrv9025_Device_t* device,
                                                 int32_t                sharedResourceArr[],
                                                 uint8_t                numSharedResources,
                                                 int32_t                recoveryAction,
                                                 const char*            customError)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_RX_DUALBAND_LNA_CTRL_WORD_OUTPUT;

    int32_t recAct                = ADI_COMMON_ACT_NO_ACTION;
    uint8_t resourceReleaseStatus = ADI_FAILURE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        sharedResourceArr);

    /*If error encountered at any point during API execution, release acquired shared resource*/
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recAct = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recAct = adrv9025_SharedResourcesRelease(device,
                                                 ADRV9025_SHARED_RESOURCE_GPIO,
                                                 &sharedResourceArr[0],
                                                 numSharedResources,
                                                 THIS_FEATURE_ID,
                                                 &resourceReleaseStatus);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release shared Analog GPIO during enabling of Rx dualband ctrl output. Please Check if the GPIOs are in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         customError);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recAct;
}

int32_t adrv9025_RxBitfieldAddressGet(adi_adrv9025_Device_t*    device,
                                      adi_adrv9025_RxChannels_e rxChannel,
                                      adrv9025_BfRxChanAddr_e*  rxChannelBitfieldAddr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        rxChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Channel not in device initializedChannels for adrv9025_RxBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
        *rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;
        break;

    case ADI_ADRV9025_RX2:
        *rxChannelBitfieldAddr = ADRV9025_BF_RX_CH1;
        break;

    case ADI_ADRV9025_RX3:
        *rxChannelBitfieldAddr = ADRV9025_BF_RX_CH2;
        break;

    case ADI_ADRV9025_RX4:
        *rxChannelBitfieldAddr = ADRV9025_BF_RX_CH3;
        break;

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid rxChannel parameter for adrv9025_RxBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    return recoveryAction;
}

int32_t adrv9025_OrxBitfieldAddressGet(adi_adrv9025_Device_t*    device,
                                       adi_adrv9025_RxChannels_e orxChannel,
                                       adrv9025_BfOrxChanAddr_e* orxChannelBitfieldAddr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        orxChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)orxChannel) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Channel not in device initializedChannels for adrv9025_OrxBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (orxChannel)
    {
    case ADI_ADRV9025_ORX1: /*Fall Through*/
    case ADI_ADRV9025_ORX2:
        *orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;
        break;

    case ADI_ADRV9025_ORX3: /*Fall Through*/
    case ADI_ADRV9025_ORX4:
        *orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH1;
        break;

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid orxChannel parameter for adrv9025_OrxBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    return recoveryAction;
}

int32_t adrv9025_RxAnalogBitfieldAddressGet(adi_adrv9025_Device_t*               device,
                                            adi_adrv9025_RxChannels_e            rxChannel,
                                            adrv9025_BfAnalogRxMemMapChanAddr_e* rxChannelAnalogBitfieldAddr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        rxChannelAnalogBitfieldAddr);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
        *rxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_RX_CH0;
        break;

    case ADI_ADRV9025_RX2:
        *rxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_RX_CH1;
        break;

    case ADI_ADRV9025_RX3:
        *rxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_RX_CH2;
        break;

    case ADI_ADRV9025_RX4:
        *rxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_RX_CH3;
        break;

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid rxChannel parameter for adrv9025_RxAnalogBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    return recoveryAction;
}

int32_t adrv9025_OrxAnalogBitfieldAddressGet(adi_adrv9025_Device_t*                device,
                                             adi_adrv9025_RxChannels_e             orxChannel,
                                             adrv9025_BfAnalogOrxMemMapChanAddr_e* orxChannelAnalogBitfieldAddr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        orxChannelAnalogBitfieldAddr);

    switch (orxChannel)
    {
    case ADI_ADRV9025_ORX1: /*Fall Through*/
    case ADI_ADRV9025_ORX2:
        *orxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_ORX_CH0;
        break;

    case ADI_ADRV9025_ORX3: /*Fall Through*/
    case ADI_ADRV9025_ORX4:
        *orxChannelAnalogBitfieldAddr = ADRV9025_BF_ANALOG_ORX_CH1;
        break;

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         orxChannel,
                         "Invalid orxChannel parameter for adrv9025_OrxAnalogBitfieldAddressGet() function");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    return recoveryAction;
}

uint16_t adrv9025_RxAddrDecode(adi_adrv9025_Device_t* device,
                               uint32_t               channelMask)
{
    if (device == NULL)
    {
        return 0;
    }

    if ((device->devStateInfo.initializedChannels & channelMask) != channelMask)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "Channel not in device initializedChannels for adrv9025_RxAddrDecode() function");
        return (0);
    }

    switch (channelMask)
    {
    case ADI_ADRV9025_RX1:
        return ADRV9025_ADDR_CH0_RX;
    case ADI_ADRV9025_RX2:
        return ADRV9025_ADDR_CH1_RX;
    case ADI_ADRV9025_RX3:
        return ADRV9025_ADDR_CH2_RX;
    case ADI_ADRV9025_RX4:
        return ADRV9025_ADDR_CH3_RX;
    case (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4):
        return ADRV9025_ADDR_ALL_RX;
    case ADI_ADRV9025_ORX1:
        return ADRV9025_ADDR_CH0_ORX;
    case ADI_ADRV9025_ORX2:
        return ADRV9025_ADDR_CH0_ORX;
    case ADI_ADRV9025_ORX3:
        return ADRV9025_ADDR_CH1_ORX;
    case ADI_ADRV9025_ORX4:
        return ADRV9025_ADDR_CH1_ORX;
    case ADI_ADRV9025_LB12:
        return ADRV9025_ADDR_CH0_ORX;
    case ADI_ADRV9025_LB34:
        return ADRV9025_ADDR_CH1_ORX;
    default:
        return (0);
    }
}

int32_t adrv9025_RxFirWrite(adi_adrv9025_Device_t* device,
                            uint32_t               rxChanMask,
                            int8_t                 gain_dB,
                            uint8_t                numFirCoefs,
                            int16_t                coefs[],
                            uint8_t                arraySize)
{
    uint8_t  firBankSel    = 0;
    uint8_t  i             = 0;
    uint8_t  j             = 0;
    uint8_t  numTapsReg    = 0;
    uint8_t  nTapMul       = 1;
    uint8_t  maxNumTaps    = 0;
    uint8_t  filterGain    = 0;
    uint8_t  pfirCoeffCtl  = 0;
    uint16_t baseaddr      = 0;
    uint8_t  filterConfig  = 0;
    uint8_t  rxPfirBankSel = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        coefs);

    ADRV9025_BUGINFO(__FUNCTION__);

    static const uint8_t autoInc      = ADI_ADRV9025_PFIR_COEFF_AUTO_INCR;
    static const uint8_t RXFIR_BANKA  = 0x01;
    static const uint8_t ORXFIR_BANKA = 0x02;
    static const uint8_t ORXFIR_BANKB = 0x04;
    static const uint8_t ORXFIR_BANKC = 0x08;

    static const uint8_t BANKA = 0;
    static const uint8_t BANKB = 1;
    static const uint8_t BANKC = 2;

    static const uint8_t RXFIR_MAXTAPS = 72;
    static const uint8_t RXFIR_MULTAPS = 24;

    if (adrv9025_RxAddrDecode(device,
                              rxChanMask) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChanMask,
                         "Invalid RxChannel");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Error checking, #taps less than max number of taps allowed.*/
    if (numFirCoefs == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (arraySize == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "arraySize is zero");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* firBankSelect and assignment of maxNumTaps, nTapMul and address firAddr.*/
    switch (rxChanMask)
    {
    case ADI_ADRV9025_RX1: /* fall through */
    case ADI_ADRV9025_RX2: /* fall through */
    case ADI_ADRV9025_RX3: /* fall through */
    case ADI_ADRV9025_RX4: /* fall through */
    case (ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4):
        maxNumTaps = RXFIR_MAXTAPS;
        firBankSel = RXFIR_BANKA;
        nTapMul    = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_ORX1: /* fall through */
    case ADI_ADRV9025_ORX3: /* fall through */
        maxNumTaps = RXFIR_MAXTAPS;
        firBankSel = ORXFIR_BANKA;
        nTapMul    = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_ORX2: /* fall through */
    case ADI_ADRV9025_ORX4: /* fall through */
        maxNumTaps = RXFIR_MAXTAPS;
        firBankSel = ORXFIR_BANKB;
        nTapMul    = RXFIR_MULTAPS;
        break;

    case ADI_ADRV9025_LB12: /* fall through */
    case ADI_ADRV9025_LB34: /* fall through */
        maxNumTaps = RXFIR_MAXTAPS;
        firBankSel = ORXFIR_BANKC;
        nTapMul    = RXFIR_MULTAPS;
        break;

    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChanMask,
                         "Undefined Rx ChannelMask");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Verification for the structure numFirCoefs */
    if (numFirCoefs > maxNumTaps)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is too large");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (numFirCoefs > arraySize)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "NumFirCoef is larger than arraySize");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Calculate register value for number of Taps */
    if (((numFirCoefs % nTapMul) == 0) && (numFirCoefs > 0))
    {
        numTapsReg = (numFirCoefs / nTapMul) - 1;
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numFirCoefs,
                         "Invalid numFirCoefs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (firBankSel == RXFIR_BANKA)
    {
        pfirCoeffCtl = ADRV9025_PFIR_COEFF_PROG_CLK_EN | ADRV9025_PFIR_COEFF_WR_ENABLE;
        if (autoInc != 0)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
        }

        baseaddr = adrv9025_RxAddrDecode(device,
                                         rxChanMask);
        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* pfir_coeff_addr */
        j = 0;
        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                              j);

        for (i = 0; i < numFirCoefs; i++)
        {
            /* pfir_coeff_data LSB */
            ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_DATA_LSB",
                                  (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_DATA_OFFSET),
                                  (uint8_t)(coefs[i]));
            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
            /* pfir_coeff_data MSB */
            ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_DATA_MSB",
                                  (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_DATA_OFFSET),
                                  (uint8_t)(coefs[i] >> 8));
            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
        }

        /* disable pfir_coeff_prog_clk_en */
        /* disable pfir_coeff_wr_enable */

        pfirCoeffCtl = 0;
        ADRV9025_SPIWRITEBYTE("RX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* Set filter gain to be used */
        switch (gain_dB)
        {
        case ADRV9025_FIR_GAIN_NEG12_DB:
            filterGain = 0x00;
            break;
        case ADRV9025_FIR_GAIN_NEG6_DB:
            filterGain = 0x01;
            break;
        case ADRV9025_FIR_GAIN_0_DB:
            filterGain = 0x02;
            break;
        case ADRV9025_FIR_GAIN_POS6_DB:
            filterGain = 0x03;
            break;
        default:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gain_dB,
                             "Invalid FirGain");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        filterConfig = 0;
        /* rx_pfir_Taps */
        filterConfig |= ADRV9025_BF_ENCODE(numTapsReg,
                                           ADRV9025_RX_PFIR_TAPS_MASK,
                                           ADRV9025_RX_PFIR_TAPS_SHIFT);

        /* rx_pfir_gain */
        filterConfig |= ADRV9025_BF_ENCODE(filterGain,
                                           ADRV9025_RX_PFIR_GAIN_MASK,
                                           ADRV9025_RX_PFIR_GAIN_SHIFT);

        ADRV9025_SPIWRITEBYTE("RX_PFIR_SETTINGS",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_SETTINGS_OFFSET),
                              filterConfig);
    }

    if (firBankSel & (ORXFIR_BANKA | ORXFIR_BANKB | ORXFIR_BANKC))
    {
        pfirCoeffCtl = ADRV9025_PFIR_COEFF_PROG_CLK_EN | ADRV9025_PFIR_COEFF_WR_ENABLE;
        if (autoInc != 0)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_AUTO_INCR;
        }
        /*pfir_coeff_rx_b_sel */

        if (firBankSel & ORXFIR_BANKA)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_A_SEL;
        }

        if (firBankSel & ORXFIR_BANKB)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_B_SEL;
        }

        if (firBankSel & ORXFIR_BANKC)
        {
            pfirCoeffCtl |= ADRV9025_PFIR_COEFF_RX_C_SEL;
        }

        baseaddr = adrv9025_RxAddrDecode(device,
                                         rxChanMask);
        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* pfir_coeff_addr */
        j = 0;
        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                              j);

        for (i = 0; i < numFirCoefs; i++)
        {
            /* pfir_coeff_data LSB */
            ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_DATA_LSB",
                                  (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_DATA_OFFSET),
                                  (uint8_t)(coefs[i]));
            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }

            /* pfir_coeff_data MSB */
            ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_DATA_MSB",
                                  (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_DATA_OFFSET),
                                  (uint8_t)(coefs[i] >> 8));
            if (autoInc == 0)
            {
                j++;
                ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_ADDR",
                                      (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_ADDR_OFFSET),
                                      j);
            }
        }

        /* disable pfir_coeff_prog_clk_en */
        /* disable pfir_coeff_wr_enable */
        pfirCoeffCtl = 0;
        ADRV9025_SPIWRITEBYTE("ORX_PFIR_COEFF_CTL",
                              (baseaddr + ADRV9025_ADDR_ORX_PFIR_COEFF_CTL_OFFSET),
                              pfirCoeffCtl);

        /* Set filter gain to be used */
        switch (gain_dB)
        {
        case ADRV9025_FIR_GAIN_NEG12_DB:
            filterGain = 0x00;
            break;
        case ADRV9025_FIR_GAIN_NEG6_DB:
            filterGain = 0x01;
            break;
        case ADRV9025_FIR_GAIN_0_DB:
            filterGain = 0x02;
            break;
        case ADRV9025_FIR_GAIN_POS6_DB:
            filterGain = 0x03;
            break;
        default:
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             gain_dB,
                             "Invalid FirGain");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        filterConfig = 0;

        /* Write FIR #taps and gain */
        /* orx_pfir_Taps */
        /* rx_pfir_Taps */
        filterConfig |= ADRV9025_BF_ENCODE(numTapsReg,
                                           ADRV9025_RX_PFIR_TAPS_MASK,
                                           ADRV9025_RX_PFIR_TAPS_SHIFT);

        /* rx_pfir_gain */
        filterConfig |= ADRV9025_BF_ENCODE(filterGain,
                                           ADRV9025_RX_PFIR_GAIN_MASK,
                                           ADRV9025_RX_PFIR_GAIN_SHIFT);

        /* pfir_coeff_rx_b_sel */
        if (firBankSel & ORXFIR_BANKA)
        {
            ADRV9025_SPIWRITEBYTE("ORX_PFIR_CONFIG_SETTING_BANK_A",
                                  (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKA),
                                  filterConfig);
        }

        /* pfir_coeff_rx_b_sel */
        if (firBankSel & ORXFIR_BANKB)
        {
            ADRV9025_SPIWRITEBYTE("ORX_PFIR_CONFIG_SETTING_BANK_B",
                                  (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKB),
                                  filterConfig);
        }

        if (firBankSel & ORXFIR_BANKC)
        {
            ADRV9025_SPIWRITEBYTE("ORX_PFIR_CONFIG_SETTING_BANK_C",
                                  (baseaddr + ADRV9025_ADDR_RX_PFIR_CONFIG_SETTINGSN_OFFSET + BANKC),
                                  filterConfig);
        }

        /* Set Mode mapping for Obs Rx1 FIR  Bank A */
        rxPfirBankSel = ADRV9025_BF_ENCODE(BANKA,
                                           ADRV9025_PFIR_CONFIG_FOR_ORX_1_MASK,
                                           ADRV9025_PFIR_CONFIG_FOR_ORX_1_SHIFT);
        /* Set Mode mapping for Obs Rx2 FIR  Bank B */
        rxPfirBankSel |= ADRV9025_BF_ENCODE(BANKB,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_2_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_ORX_2_SHIFT);
        /* Set Mode mapping for Loopback  Bank C */
        rxPfirBankSel |= ADRV9025_BF_ENCODE(BANKC,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_1_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_1_SHIFT);
        rxPfirBankSel |= ADRV9025_BF_ENCODE(BANKC,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_2_MASK,
                                            ADRV9025_PFIR_CONFIG_FOR_LOOPBACK_2_SHIFT);

        ADRV9025_SPIWRITEBYTE("ORX_PFIR_BANK_SEL",
                              (baseaddr + ADRV9025_ADDR_RX_PFIR_BANK_SEL_OFFSET),
                              rxPfirBankSel);
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_RxDataFormatSetFloatingPointRangeCheck(adi_adrv9025_Device_t*                      device,
                                                        adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        floatingPointConfig);

    /* Check floating point format selected is valid */
    if ((floatingPointConfig->fpDataFormat != ADI_ADRV9025_FP_FORMAT_SIGN_SIGNIFICAND_EXP) &&
        (floatingPointConfig->fpDataFormat != ADI_ADRV9025_FP_FORMAT_SIGN_EXP_SIGNIFICAND))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpDataFormat,
                         "Invalid floating point data format selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check floating point round mode is valid */
    if ((floatingPointConfig->fpRoundMode != ADI_ADRV9025_ROUND_TO_EVEN) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV9025_ROUNDTOWARDS_POSITIVE) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV9025_ROUNDTOWARDS_NEGATIVE) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV9025_ROUNDTOWARDS_ZERO) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV9025_ROUND_FROM_EVEN))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpRoundMode,
                         "Invalid floating point round mode selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check floating point no. of exponent bits is valid */
    if ((floatingPointConfig->fpNumExpBits != ADI_ADRV9025_2_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV9025_3_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV9025_4_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV9025_5_EXPONENTBITS))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpNumExpBits,
                         "Invalid no. of floating point exponent bits selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check floating point attenuation steps are valid */
    if ((floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_24DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_18DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_12DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_6DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_0DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_MINUS6DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_MINUS12DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV9025_FPATTEN_MINUS18DB))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpAttenSteps,
                         "Invalid floating point attenuation step selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check floating point encoded NaN selection is valid*/
    if ((floatingPointConfig->fpEncodeNan != ADI_ADRV9025_FP_FORMAT_NAN_ENCODE_DISABLE) &&
        (floatingPointConfig->fpEncodeNan != ADI_ADRV9025_FP_FORMAT_NAN_ENCODE_ENABLE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpEncodeNan,
                         "Invalid floating point encode NaN selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check floating point encoded NaN selection is valid*/
    if ((floatingPointConfig->fpHideLeadingOne != ADI_ADRV9025_FP_FORMAT_HIDE_LEADING_ONE_DISABLE) &&
        (floatingPointConfig->fpHideLeadingOne != ADI_ADRV9025_FP_FORMAT_HIDE_LEADING_ONE_ENABLE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         floatingPointConfig->fpHideLeadingOne,
                         "Invalid floating point hide leading one selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatSetIntegerRangeCheck(adi_adrv9025_Device_t*                device,
                                                  uint32_t                              rxChannelMask,
                                                  adi_adrv9025_RxDataFormatModes_e      formatSelect,
                                                  adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                                  adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings)
{
    static const uint8_t SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE = 0x0A;
    static const uint8_t SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE = 0x0B;
    static const uint8_t SOURCE_CTRL_UNUSED                   = 0x00;

    int32_t recoveryAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t gpioSourceCtrl  = 0;
    uint8_t gpioSourceCtrl2 = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettings);

    /* Check slicer configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        slicerConfigSettings);

    /* Check that internal slicer step sizes are correct */
    if ((formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
        (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
        (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO))
    {
        /* Check that selected integer formatter internal slicer embedded bits mode is valid */
        if ((integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             integerConfigSettings->intEmbeddedBits,
                             "Invalid integer formatter internal slicer embedded bits selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Check that integer format resolution is valid */
        if ((integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_2SCOMP) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_2SCOMP) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_2SCOMP) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             integerConfigSettings->intSampleResolution,
                             "Invalid integer formatter sample resolution selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Embedding 1 bit slicer at MSB only supports 12 bit/16 bit/24 bit signed integer sample resolution */
        if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_12BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             integerConfigSettings->intSampleResolution,
                             "Invalid integer formatter sample resolution. Valid sample resolution is 12-bit signed for embedding 1 slicer bit at MSB");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Embedding 2 bit slicer at MSB for a 3-bit slicer mode only supports Signed representation in 16/24 bits */
        if (integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER)
        {
            if ((integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
                (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 integerConfigSettings->intSampleResolution,
                                 "Invalid integer formatter sample resolution. Valid sample resolution is 16/24-bit signed for embedding 2 slicer bits at MSB for a 3-bit slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Embedding 2 bit slicer at MSB for a 4-bit slicer mode only supports Signed representation in 16/24 bits */
        if (integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER)
        {
            if ((integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_SIGNED) &&
                (integerConfigSettings->intSampleResolution != ADI_ADRV9025_INTEGER_24BIT_SIGNED))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 integerConfigSettings->intSampleResolution,
                                 "Invalid integer formatter sample resolution. Valid sample resolution is 16/24-bit signed for embedding 2 slicer bits at MSB for a 4-bit slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Check that integer format parity is valid */
        if ((integerConfigSettings->intParity != ADI_ADRV9025_3BIT_SLICER_EVEN_PARITY) &&
            (integerConfigSettings->intParity != ADI_ADRV9025_3BIT_SLICER_ODD_PARITY) &&
            (integerConfigSettings->intParity != ADI_ADRV9025_NO_PARITY))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             integerConfigSettings->intParity,
                             "Invalid integer formatter parity option selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Check that the use case is valid for embedding parity bit */
        if ((integerConfigSettings->intParity == ADI_ADRV9025_3BIT_SLICER_EVEN_PARITY) || (integerConfigSettings->intParity ==
            ADI_ADRV9025_3BIT_SLICER_ODD_PARITY))
        {
            /* Parity is valid only in 3 bit internal embedded slicer mode */
            if (formatSelect != ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 formatSelect,
                                 "Invalid integer formatter mode selected for parity support. Parity is supported only in 3 bit internal slicer modes");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Parity is valid only in 3 bit slicer mode */
            if ((integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) &&
                (integerConfigSettings->intEmbeddedBits != ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 formatSelect,
                                 "Invalid integer formatter mode selected for parity support. Parity is supported only in 3 bit internal slicer modes");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Parity is valid only in 3 bit embedded slicer mode. 12 bit resolution does not support 3 bit parity */
            if ((integerConfigSettings->intSampleResolution == ADI_ADRV9025_INTEGER_12BIT_2SCOMP) ||
                (integerConfigSettings->intSampleResolution == ADI_ADRV9025_INTEGER_12BIT_SIGNED))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 integerConfigSettings->intSampleResolution,
                                 "Invalid integer formatter sampling resolution mode for parity config. Parity is supported only for 16 and 24 bit resolutions");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        if ((slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_1DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_2DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_3DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_4DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_6DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV9025_INTSLICER_STEPSIZE_8DB))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             slicerConfigSettings->intSlicerStepSize,
                             "Invalid internal slicer step size selected for integer format internal slicer mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Check the source ctrl on Rx Slicer Posn observation GPIOs for 4 Pin mode */
        if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannelMask,
                                 "ORx channel is only supported for ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /*Check that Rx1 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx1 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx2 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx2 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx3 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
            {
                recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx3 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx4 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
            {
                recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx4 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }

        /*Check the source ctrl on Rx Slicer Posn observation GPIOs for 3 Pin mode*/
        if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
                ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannelMask,
                                 "ORx channel is only supported for ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /*Check that Rx1 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl2);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx1 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                if ((gpioSourceCtrl2 != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl2 != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl2,
                                     "GPIO for Rx1 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx2 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx2 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx3 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx3 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /*Check that Rx4 slicer position GPIO source ctrl is not set to another feature*/
            if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
            {
                recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                         ADRV9025_BF_CORE,
                                                                                         &gpioSourceCtrl);
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfGet(device,
                                                                              ADRV9025_BF_CORE,
                                                                              &gpioSourceCtrl2);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if ((gpioSourceCtrl != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx4 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                if ((gpioSourceCtrl2 != SOURCE_CTRL_UNUSED) &&
                    (gpioSourceCtrl2 != SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE))
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     gpioSourceCtrl,
                                     "GPIO for Rx4 slicer position output in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    else if (formatSelect == ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER)
    {
        if (((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX1) == (uint32_t)ADI_ADRV9025_ORX1) ||
            ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX2) == (uint32_t)ADI_ADRV9025_ORX2) ||
            ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX3) == (uint32_t)ADI_ADRV9025_ORX3) ||
            ((rxChannelMask & (uint32_t)ADI_ADRV9025_ORX4) == (uint32_t)ADI_ADRV9025_ORX4))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannelMask,
                             "ORx channel is only supported for ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Check that if the format selected is external slicer, external slicer step sizes are valid */
        if ((slicerConfigSettings->extSlicerStepSize != ADI_ADRV9025_EXTSLICER_STEPSIZE_1DB) &&
            (slicerConfigSettings->extSlicerStepSize != ADI_ADRV9025_EXTSLICER_STEPSIZE_2DB) &&
            (slicerConfigSettings->extSlicerStepSize != ADI_ADRV9025_EXTSLICER_STEPSIZE_3DB) &&
            (slicerConfigSettings->extSlicerStepSize != ADI_ADRV9025_EXTSLICER_STEPSIZE_4DB) &&
            (slicerConfigSettings->extSlicerStepSize != ADI_ADRV9025_EXTSLICER_STEPSIZE_6DB))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             slicerConfigSettings->extSlicerStepSize,
                             "Invalid external slicer step size selected for integer format external slicer mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Check that if an external slicer mode for Rx1 is selected, the corresponding external slicer GPIO is valid */
        if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX1) == (uint32_t)ADI_ADRV9025_RX1)
        {
            if (slicerConfigSettings->rx1ExtSlicerGpioSelect >= ADI_ADRV9025_EXTSLICER_RX_GPIO_INVALID)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 slicerConfigSettings->rx1ExtSlicerGpioSelect,
                                 "Invalid external slicer GPIO pins selected for Rx1 integer format external slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Check that if an external slicer mode for Rx2 is selected, the corresponding external slicer GPIO is valid */
        if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX2) == (uint32_t)ADI_ADRV9025_RX2)
        {
            if (slicerConfigSettings->rx2ExtSlicerGpioSelect >= ADI_ADRV9025_EXTSLICER_RX_GPIO_INVALID)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 slicerConfigSettings->rx2ExtSlicerGpioSelect,
                                 "Invalid external slicer GPIO pins selected for Rx2 integer format external slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Check that if an external slicer mode for Rx3 is selected, the corresponding external slicer GPIO is valid */
        if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX3) == (uint32_t)ADI_ADRV9025_RX3)
        {
            if (slicerConfigSettings->rx3ExtSlicerGpioSelect >= ADI_ADRV9025_EXTSLICER_RX_GPIO_INVALID)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 slicerConfigSettings->rx3ExtSlicerGpioSelect,
                                 "Invalid external slicer GPIO pins selected for Rx3 integer format external slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        /* Check that if an external slicer mode for Rx4 is selected, the corresponding external slicer GPIO is valid */
        if ((rxChannelMask & (uint32_t)ADI_ADRV9025_RX4) == (uint32_t)ADI_ADRV9025_RX4)
        {
            if (slicerConfigSettings->rx4ExtSlicerGpioSelect >= ADI_ADRV9025_EXTSLICER_RX_GPIO_INVALID)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 slicerConfigSettings->rx4ExtSlicerGpioSelect,
                                 "Invalid external slicer GPIO pins selected for Rx4 integer format external slicer mode");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatSetEmbOvldMonitorRangeCheck(adi_adrv9025_Device_t*                           device,
                                                         adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings,
                                                         adi_adrv9025_RxIntSampleResolution_e             intSampleResolution)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check embedded overload monitor configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        embOvldConfigSettings);

    if (intSampleResolution != ADI_ADRV9025_INTEGER_16BIT_2SCOMP)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         intSampleResolution,
                         "Embedded overload monitoring Rx data format is only supported with integer 16Bit - 2s Complement format");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         embOvldConfigSettings->embeddedMonitorSrcLsbI,
                         "Invalid embedded monitor source selected to be embedded at LSB posn - I sample");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI,
                         "Invalid embedded monitor source selected to be embedded at LSB + 1 posn - I sample");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         embOvldConfigSettings->embeddedMonitorSrcLsbQ,
                         "Invalid embedded monitor source selected to be embedded at LSB posn - Q sample");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ,
                         "Invalid embedded monitor source selected to be embedded at LSB + 1 posn - Q sample");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorApdHighSrcSel != ADI_ADRV9025_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorApdHighSrcSel != ADI_ADRV9025_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_CNTR_EXCEEDED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             embOvldConfigSettings->embeddedMonitorApdHighSrcSel,
                             "Analog Peak Detector high thresh selected for embedded monitoring but APD high source is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorApdLowSrcSel != ADI_ADRV9025_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorApdLowSrcSel != ADI_ADRV9025_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_CNTR_EXCEEDED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             embOvldConfigSettings->embeddedMonitorApdLowSrcSel,
                             "Analog Peak Detector low thresh selected for embedded monitoring but APD low source is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorHb2HighSrcSel != ADI_ADRV9025_HB2_HIGH_SRC_OVRG_HIGH) &&
            (embOvldConfigSettings->embeddedMonitorHb2HighSrcSel != ADI_ADRV9025_HB2_HIGH_SRC_OVRG_HIGH_CNTR_EXCEEDED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             embOvldConfigSettings->embeddedMonitorHb2HighSrcSel,
                             "Digital HB2 Peak Detector high thresh selected for embedded monitoring but HB2 high source is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT0) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT1) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_THRESH_CNTR_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT0_THRESH_CNTR_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV9025_HB2_LOW_SRC_OVRG_LOW_INT1_THRESH_CNTR_EXCEEDED))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             embOvldConfigSettings->embeddedMonitorHb2LowSrcSel,
                             "Digital HB2 Peak Detector low thresh selected for embedded monitoring but HB2 low source is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatSelectGet(adi_adrv9025_Device_t*            device,
                                       adi_adrv9025_RxChannels_e         rxChannel,
                                       adi_adrv9025_RxDataFormatModes_e* rxDataFormat)
{
    static const uint8_t ENABLE_FLOATING_POINT_FORMAT  = 1;
    static const uint8_t ENABLE_GAIN_COMPENSATION      = 1;
    static const uint8_t ENABLE_RX_EXT_SLICER          = 1;
    static const uint8_t ENABLE_RX_INT_EMBEDDED_SLICER = 1;

    int32_t                                         recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                                         gainCompEn             = 0;
    uint8_t                                         floatingPointEn        = 0;
    uint8_t                                         rxExtSlicerModeEn      = 0;
    uint8_t                                         intEmbedSlicer         = 0;
    adrv9025_BfRxChanAddr_e                         rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e                        orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;
    adi_adrv9025_EmbOverloadMonitorConfigSettings_t embOvldMonitorSettings = {ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check floating point configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        rxDataFormat);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxGainCompEnableBfGet(device,
                                                        rxChannelBitfieldAddr,
                                                        &gainCompEn);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (gainCompEn != ENABLE_GAIN_COMPENSATION)
        {
            recoveryAction = adrv9025_RxDataFormatEmbOvldMonitorGet(device,
                                                                    rxChannel,
                                                                    &embOvldMonitorSettings);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if ((embOvldMonitorSettings.embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED))
            {
                *rxDataFormat = ADI_ADRV9025_GAIN_COMPENSATION_DISABLED;
            }
            else
            {
                *rxDataFormat = ADI_ADRV9025_EMBED_OVERLOAD_MONITOR_DATA;
            }
        }
        else
        {
            recoveryAction = adrv9025_RxFpEnBfGet(device,
                                                  rxChannelBitfieldAddr,
                                                  &floatingPointEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (floatingPointEn == ENABLE_FLOATING_POINT_FORMAT)
            {
                *rxDataFormat = ADI_ADRV9025_GAIN_WITH_FLOATING_POINT;
            }
            else
            {
                recoveryAction = adrv9025_RxSlicerPinControlModeBfGet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      &rxExtSlicerModeEn);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (rxExtSlicerModeEn == ENABLE_RX_EXT_SLICER)
                {
                    *rxDataFormat = ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER;
                }
                else
                {
                    recoveryAction = adrv9025_RxIntEmbedSlicerBfGet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    &intEmbedSlicer);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    if (intEmbedSlicer == ENABLE_RX_INT_EMBEDDED_SLICER)
                    {
                        *rxDataFormat = ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO;
                    }
                    else
                    {
                        recoveryAction = adrv9025_RxInternalSlicerGpioEnableGet(device,
                                                                                rxChannel,
                                                                                rxDataFormat,
                                                                                NULL);
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }
                }
            }
        }
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxGainCompEnableBfGet(device,
                                                         orxChannelBitfieldAddr,
                                                         &gainCompEn);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (gainCompEn != ENABLE_GAIN_COMPENSATION)
        {
            *rxDataFormat = ADI_ADRV9025_GAIN_COMPENSATION_DISABLED;
        }
        else
        {
            recoveryAction = adrv9025_OrxFpEnBfGet(device,
                                                   orxChannelBitfieldAddr,
                                                   &floatingPointEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (floatingPointEn == ENABLE_FLOATING_POINT_FORMAT)
            {
                *rxDataFormat = ADI_ADRV9025_GAIN_WITH_FLOATING_POINT;
            }
            else
            {
                recoveryAction = adrv9025_OrxIntEmbedSlicerBfGet(device,
                                                                 orxChannelBitfieldAddr,
                                                                 &intEmbedSlicer);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (intEmbedSlicer == ENABLE_RX_INT_EMBEDDED_SLICER)
                {
                    *rxDataFormat = ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO;
                }
                else
                {
                    *rxDataFormat = ADI_ADRV9025_GAIN_COMPENSATION_DISABLED;
                }
            }
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel,
					     "rxChannel input parameter is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);

    }


    return recoveryAction;
}

int32_t adrv9025_RxDataFormatFloatingPointSet(adi_adrv9025_Device_t*                      device,
                                              uint32_t                                    rxChannelMask,
                                              adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]             = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]            = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE          = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint32_t                  ORX_CHANNEL_ARR_SIZE         = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0]);
    static const uint8_t                   ENABLE_FLOATING_POINT_FORMAT = 1;
    static const uint8_t                   ENABLE_GAIN_COMPENSATION     = 1;
    static const uint8_t                   DEFAULT_INT_DATA_FORMAT      = 0;
    static const uint8_t                   DEFAULT_INT_DATA_RESOLUTION  = 1;

    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                 rxChannelArrIndex      = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check floating point configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        floatingPointConfig);

    /*Disable gain compensation and set integer format register to default*/
    recoveryAction = adrv9025_RxDataFormatGainCompDisable(device,
                                                          rxChannelMask,
                                                          DEFAULT_INT_DATA_FORMAT,
                                                          DEFAULT_INT_DATA_RESOLUTION);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Iterate through channel mask and if set configure the corresponding Rx floating point formatter*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Set Floating Point Configs*/
            recoveryAction = adrv9025_RxFpRoundModeBfSet(device,
                                                         rxChannelBitfieldAddr,
                                                         (adrv9025_BfRxFpRoundMode_e)floatingPointConfig->fpRoundMode);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxFpFloatDataFormatBfSet(device,
                                                               rxChannelBitfieldAddr,
                                                               (uint8_t)floatingPointConfig->fpDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxFpNanEncEnBfSet(device,
                                                        rxChannelBitfieldAddr,
                                                        (uint8_t)floatingPointConfig->fpEncodeNan);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxFpExponentBitsBfSet(device,
                                                            rxChannelBitfieldAddr,
                                                            (adrv9025_BfRxFpExponentBits_e)floatingPointConfig->fpNumExpBits);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxFpHideLeadingOnesBfSet(device,
                                                               rxChannelBitfieldAddr,
                                                               (uint8_t)floatingPointConfig->fpHideLeadingOne);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Set Floating point attenuation*/
            recoveryAction = adrv9025_RxFpIntDataAttenBfSet(device,
                                                            rxChannelBitfieldAddr,
                                                            (uint8_t)floatingPointConfig->fpAttenSteps);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Floating Point Format on the requested channel*/
            recoveryAction = adrv9025_RxFpEnBfSet(device,
                                                  rxChannelBitfieldAddr,
                                                  ENABLE_FLOATING_POINT_FORMAT);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Gain Compensation*/
            recoveryAction = adrv9025_RxGainCompEnableBfSet(device,
                                                            rxChannelBitfieldAddr,
                                                            ENABLE_GAIN_COMPENSATION);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Iterate through channel mask and if set configure the corresponding ORx floating point formatter*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < ORX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelArrIndex],
                                                            &orxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Set Floating Point Configs*/
            recoveryAction = adrv9025_OrxFpRoundModeBfSet(device,
                                                          orxChannelBitfieldAddr,
                                                          (adrv9025_BfOrxFpRoundMode_e)floatingPointConfig->fpRoundMode);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxFpFloatDataFormatBfSet(device,
                                                                orxChannelBitfieldAddr,
                                                                (uint8_t)floatingPointConfig->fpDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxFpNanEncEnBfSet(device,
                                                         orxChannelBitfieldAddr,
                                                         (uint8_t)floatingPointConfig->fpEncodeNan);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxFpExponentBitsBfSet(device,
                                                             orxChannelBitfieldAddr,
                                                             (adrv9025_BfOrxFpExponentBits_e)floatingPointConfig->fpNumExpBits);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxFpHideLeadingOnesBfSet(device,
                                                                orxChannelBitfieldAddr,
                                                                (uint8_t)floatingPointConfig->fpHideLeadingOne);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Set Floating point attenuation*/
            recoveryAction = adrv9025_OrxFpIntDataAttenBfSet(device,
                                                             orxChannelBitfieldAddr,
                                                             (uint8_t)floatingPointConfig->fpAttenSteps);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Floating Point Format on the requested channel*/
            recoveryAction = adrv9025_OrxFpEnBfSet(device,
                                                   orxChannelBitfieldAddr,
                                                   ENABLE_FLOATING_POINT_FORMAT);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Gain Compensation*/
            recoveryAction = adrv9025_OrxGainCompEnableBfSet(device,
                                                             orxChannelBitfieldAddr,
                                                             ENABLE_GAIN_COMPENSATION);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatFloatingPointGet(adi_adrv9025_Device_t*                      device,
                                              adi_adrv9025_RxChannels_e                   rxChannel,
                                              adi_adrv9025_FloatingPointConfigSettings_t* floatingPointConfig)

{
    int32_t                       recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e       rxChannelBitfieldAddr  = ADRV9025_BF_ALL_RX_CHANNELS;
    adrv9025_BfOrxChanAddr_e      orxChannelBitfieldAddr = ADRV9025_BF_ALL_ORX_CHANNELS;
    adrv9025_BfRxFpRoundMode_e    fpRoundModeBf          = ADRV9025_BF_RX_ROUNDTIESTOEVEN;
    uint8_t                       fpDataFormatBf         = 0;
    uint8_t                       fpEncodeNanBf          = 0;
    adrv9025_BfRxFpExponentBits_e fpNumExpBitsBf         = ADRV9025_BF_RX_RX1_EXP_2;
    uint8_t                       fpHideLeadingOneBf     = 0;
    uint8_t                       fpAttenStepsBf         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        floatingPointConfig);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        /*Resolve bit field base address for the channel*/
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Get Floating Point Configs*/
        /*Not sending references of struct members directly to the Get function as it updates only LSB (1byte)
        of the members where as struct members are 4 bytes wide by default unless compiler settings are changed*/
        recoveryAction = adrv9025_RxFpRoundModeBfGet(device,
                                                     rxChannelBitfieldAddr,
                                                     &fpRoundModeBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxFpFloatDataFormatBfGet(device,
                                                           rxChannelBitfieldAddr,
                                                           &fpDataFormatBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxFpNanEncEnBfGet(device,
                                                    rxChannelBitfieldAddr,
                                                    &fpEncodeNanBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxFpExponentBitsBfGet(device,
                                                        rxChannelBitfieldAddr,
                                                        &fpNumExpBitsBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxFpHideLeadingOnesBfGet(device,
                                                           rxChannelBitfieldAddr,
                                                           &fpHideLeadingOneBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Get Floating point attenuation*/
        recoveryAction = adrv9025_RxFpIntDataAttenBfGet(device,
                                                        rxChannelBitfieldAddr,
                                                        &fpAttenStepsBf);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        /*Resolve bit field base address for the channel*/
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Get Floating Point Configs*/
        /*Not sending references of struct members directly to the Get function as it updates only LSB (1byte)
        of the members where as struct members are 4 bytes wide by default unless compiler settings are changed*/
        recoveryAction = adrv9025_OrxFpRoundModeBfGet(device,
                                                      orxChannelBitfieldAddr,
                                                      (adrv9025_BfOrxFpRoundMode_e*)&fpRoundModeBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxFpFloatDataFormatBfGet(device,
                                                            orxChannelBitfieldAddr,
                                                            &fpDataFormatBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxFpNanEncEnBfGet(device,
                                                     orxChannelBitfieldAddr,
                                                     &fpEncodeNanBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxFpExponentBitsBfGet(device,
                                                         orxChannelBitfieldAddr,
                                                         (adrv9025_BfOrxFpExponentBits_e*)&fpNumExpBitsBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxFpHideLeadingOnesBfGet(device,
                                                            orxChannelBitfieldAddr,
                                                            &fpHideLeadingOneBf);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Get Floating point attenuation*/
        recoveryAction = adrv9025_OrxFpIntDataAttenBfGet(device,
                                                         orxChannelBitfieldAddr,
                                                         &fpAttenStepsBf);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Update the structure with read back data*/
    floatingPointConfig->fpRoundMode      = (adi_adrv9025_FpRoundModes_e)fpRoundModeBf;
    floatingPointConfig->fpDataFormat     = (adi_adrv9025_FpFloatDataFormat_e)fpDataFormatBf;
    floatingPointConfig->fpEncodeNan      = (adi_adrv9025_FpNanEncode_e)fpEncodeNanBf;
    floatingPointConfig->fpNumExpBits     = (adi_adrv9025_FpExponentModes_e)fpNumExpBitsBf;
    floatingPointConfig->fpHideLeadingOne = (adi_adrv9025_FpHideLeadingOne_e)fpHideLeadingOneBf;
    floatingPointConfig->fpAttenSteps     = (adi_adrv9025_FpAttenSteps_e)fpAttenStepsBf;

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatIntegerSet(adi_adrv9025_Device_t*                device,
                                        uint32_t                              rxChannelMask,
                                        adi_adrv9025_RxDataFormatModes_e      formatSelect,
                                        adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                        adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings)

{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]              = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]             = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE           = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint32_t                  ORX_CHANNEL_ARR_SIZE          = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0]);
    static const uint8_t                   INT_DATA_FORMAT_2S_COMPLEMENT = 0;
    static const uint8_t                   INT_DATA_RESOLUTION_16BITS    = 1;
    static const uint8_t                   INT_EMBED_SLICER_DISABLE      = 0;
    static const uint8_t                   INT_EMBED_SLICER_POS_MSB      = 0;
    static const uint8_t                   INT_EMBED_SLICER_2_BITS       = 0;
    static const uint8_t                   INT_PARTIY_SUPPORT_DISABLE    = 0;
    static const uint8_t                   INT_PARITY_EVEN               = 0;
    static const uint8_t                   ENABLE_GAIN_COMPENSATION      = 1;
    static const uint8_t                   SLICER_PIN_CTRL_MODE_INTERNAL = 0;
    static const uint8_t                   SLICER_PIN_CTRL_MODE_EXTERNAL = 1;
    static const uint8_t                   DEFAULT_INT_DATA_FORMAT       = 0;
    static const uint8_t                   DEFAULT_INT_DATA_RESOLUTION   = 1;

    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                 rxChannelArrIndex      = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_ALL_RX_CHANNELS;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ALL_ORX_CHANNELS;
    uint8_t                  intEvenParity          = INT_PARITY_EVEN;
    uint8_t                  intParitySupport       = INT_PARTIY_SUPPORT_DISABLE;
    uint8_t                  intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
    uint8_t                  intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
    uint8_t                  intEmbedSlicer         = INT_EMBED_SLICER_DISABLE;
    uint8_t                  intDataResolution      = INT_DATA_RESOLUTION_16BITS;
    uint8_t                  intDataFormat          = INT_DATA_FORMAT_2S_COMPLEMENT;
    uint8_t                  slicerPinCtrlMode      = SLICER_PIN_CTRL_MODE_INTERNAL;
    uint8_t                  slicer3bitModeEn       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettings);

    /* Check slicer configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        slicerConfigSettings);

    /*Disable Gain Compensation and formatter settings for requested channels*/
    recoveryAction = adrv9025_RxDataFormatGainCompDisable(device,
                                                          rxChannelMask,
                                                          DEFAULT_INT_DATA_FORMAT,
                                                          DEFAULT_INT_DATA_RESOLUTION);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Iterate through channel mask and if set configure integer formatter settings on the corresponding Rx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask bit is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* If external slicer selected, resolve GPIOs and ensure that GPIOs are not already in use */
            if (formatSelect == ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER)
            {
                recoveryAction = adrv9025_RxExternalSlicerGpioEnable(device,
                                                                     RX_CHANNEL_ARR[rxChannelArrIndex],
                                                                     slicerConfigSettings);
                ADI_ERROR_RETURN(device->common.error.newAction);

                recoveryAction = adrv9025_RxExternalSlicerPinControlStepBfSet(device,
                                                                              rxChannelBitfieldAddr,
                                                                              (uint8_t)slicerConfigSettings->extSlicerStepSize);
                ADI_ERROR_RETURN(device->common.error.newAction);

                slicerPinCtrlMode = SLICER_PIN_CTRL_MODE_EXTERNAL;
            }
                /* If internal slicer with GPIOs selected, configure GPIO output source ctrl */
            else if ((formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
                (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE))
            {
                recoveryAction = adrv9025_RxInternalSlicerGpioEnable(device,
                                                                     formatSelect,
                                                                     RX_CHANNEL_ARR[rxChannelArrIndex]);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Resolve slicer and slicer position configs */
            recoveryAction = adrv9025_RxIntEmbSlicerConfigResolve(device,
                                                                  formatSelect,
                                                                  integerConfigSettings->intEmbeddedBits,
                                                                  &intEmbedSlicer,
                                                                  &intEmbedSlicerPos,
                                                                  &intEmbedSlicerNumber,
                                                                  &slicer3bitModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Resolve Sample Resolution*/
            recoveryAction = adrv9025_RxIntSampleResFormatResolve(device,
                                                                  integerConfigSettings,
                                                                  &intDataResolution,
                                                                  &intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Resolve Parity selection */
            recoveryAction = adrv9025_RxIntParitySupportResolve(device,
                                                                integerConfigSettings,
                                                                &intParitySupport,
                                                                &intEvenParity);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Rx Gain Compensation for the requested Rx channel*/
            recoveryAction = adrv9025_RxGainCompEnableBfSet(device,
                                                            rxChannelBitfieldAddr,
                                                            ENABLE_GAIN_COMPENSATION);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntDataFormatBfSet(device,
                                                           rxChannelBitfieldAddr,
                                                           intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntDataResolutionBfSet(device,
                                                               rxChannelBitfieldAddr,
                                                               intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEmbedSlicerBfSet(device,
                                                            rxChannelBitfieldAddr,
                                                            intEmbedSlicer);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEmbedSlicerPosBfSet(device,
                                                               rxChannelBitfieldAddr,
                                                               intEmbedSlicerPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEmbedSlicerNumberBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  intEmbedSlicerNumber);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntParitySupportBfSet(device,
                                                              rxChannelBitfieldAddr,
                                                              intParitySupport);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntEvenParityBfSet(device,
                                                           rxChannelBitfieldAddr,
                                                           intEvenParity);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxSlicerPinControlModeBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  slicerPinCtrlMode);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntSlicerLsbOnQBfSet(device,
                                                             rxChannelBitfieldAddr,
                                                             (uint8_t)integerConfigSettings->intEmbeddedPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxStatic3bitSlicerModeEnBfSet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    slicer3bitModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (formatSelect != ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER)
            {
                recoveryAction = adrv9025_RxSlicerPinControlStepBfSet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      (uint8_t)slicerConfigSettings->intSlicerStepSize);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
            {
                if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) ||
                    (integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER))
                {
                    device->devStateInfo.rxChannel3bitSlicerMode |= (uint8_t)RX_CHANNEL_ARR[rxChannelArrIndex];
                }
                else
                {
                    device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)RX_CHANNEL_ARR[rxChannelArrIndex];
                }
            }
            else
            {
                device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)RX_CHANNEL_ARR[rxChannelArrIndex];
            }
        }
    }

    /*Iterate through channel mask and if set configure integer formatter settings on the corresponding ORx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < ORX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask bit is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelArrIndex],
                                                            &orxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* External Slicer mode is invalid for ORx channel */
            if (formatSelect == ADI_ADRV9025_GAIN_WITH_EXTERNAL_SLICER)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 formatSelect,
                                 "External slicer mode not supported for ORx channels");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
                /* If internal slicer with GPIOs selected, configure GPIO output source ctrl */
            else if ((formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
                (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 formatSelect,
                                 "Internal slicer mode not supported for ORx channels");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Resolve slicer and slicer position configs */
            recoveryAction = adrv9025_RxIntEmbSlicerConfigResolve(device,
                                                                  formatSelect,
                                                                  integerConfigSettings->intEmbeddedBits,
                                                                  &intEmbedSlicer,
                                                                  &intEmbedSlicerPos,
                                                                  &intEmbedSlicerNumber,
                                                                  &slicer3bitModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Resolve Sample Resolution*/
            recoveryAction = adrv9025_RxIntSampleResFormatResolve(device,
                                                                  integerConfigSettings,
                                                                  &intDataResolution,
                                                                  &intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Resolve Parity selection */
            recoveryAction = adrv9025_RxIntParitySupportResolve(device,
                                                                integerConfigSettings,
                                                                &intParitySupport,
                                                                &intEvenParity);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Enable Rx Gain Compensation for the requested ORx channel*/
            recoveryAction = adrv9025_OrxGainCompEnableBfSet(device,
                                                             orxChannelBitfieldAddr,
                                                             ENABLE_GAIN_COMPENSATION);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntDataFormatBfSet(device,
                                                            orxChannelBitfieldAddr,
                                                            intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntDataResolutionBfSet(device,
                                                                orxChannelBitfieldAddr,
                                                                intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEmbedSlicerBfSet(device,
                                                             orxChannelBitfieldAddr,
                                                             intEmbedSlicer);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEmbedSlicerPosBfSet(device,
                                                                orxChannelBitfieldAddr,
                                                                intEmbedSlicerPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEmbedSlicerNumberBfSet(device,
                                                                   orxChannelBitfieldAddr,
                                                                   intEmbedSlicerNumber);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntParitySupportBfSet(device,
                                                               orxChannelBitfieldAddr,
                                                               intParitySupport);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntEvenParityBfSet(device,
                                                            orxChannelBitfieldAddr,
                                                            intEvenParity);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntSlicerLsbOnQBfSet(device,
                                                              orxChannelBitfieldAddr,
                                                              (uint8_t)integerConfigSettings->intEmbeddedPos);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxSlicerPinControlStepBfSet(device,
                                                                   orxChannelBitfieldAddr,
                                                                   (uint8_t)slicerConfigSettings->intSlicerStepSize);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxStatic3bitSlicerModeEnBfSet(device,
                                                                     orxChannelBitfieldAddr,
                                                                     slicer3bitModeEn);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
            {
                if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) ||
                    (integerConfigSettings->intEmbeddedBits == ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER))
                {
                    device->devStateInfo.rxChannel3bitSlicerMode |= (uint8_t)ORX_CHANNEL_ARR[rxChannelArrIndex];
                }
                else
                {
                    device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)ORX_CHANNEL_ARR[rxChannelArrIndex];
                }
            }
            else
            {
                device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)ORX_CHANNEL_ARR[rxChannelArrIndex];
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatIntegerGet(adi_adrv9025_Device_t*                device,
                                        adi_adrv9025_RxChannels_e             rxChannel,
                                        adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                        adi_adrv9025_SlicerConfigSettings_t*  slicerConfigSettings)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT = 0;
    static const uint8_t INT_DATA_RESOLUTION_16BITS    = 1;
    static const uint8_t INT_EMBED_SLICER_DISABLE      = 0;
    static const uint8_t INT_EMBED_SLICER_POS_MSB      = 0;
    static const uint8_t INT_EMBED_SLICER_2_BITS       = 0;
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE    = 0;
    static const uint8_t INT_PARITY_EVEN               = 0;
    static const uint8_t SLICER_PIN_CTRL_MODE_INTERNAL = 0;

    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_ALL_RX_CHANNELS;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ALL_ORX_CHANNELS;
    uint8_t                  intEvenParity          = INT_PARITY_EVEN;
    uint8_t                  intParitySupport       = INT_PARTIY_SUPPORT_DISABLE;
    uint8_t                  intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
    uint8_t                  intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
    uint8_t                  intEmbedSlicer         = INT_EMBED_SLICER_DISABLE;
    uint8_t                  intDataResolution      = INT_DATA_RESOLUTION_16BITS;
    uint8_t                  intDataFormat          = INT_DATA_FORMAT_2S_COMPLEMENT;
    uint8_t                  slicerPinCtrlMode      = SLICER_PIN_CTRL_MODE_INTERNAL;
    uint8_t                  extSlicerStepSize      = 0;
    uint8_t                  intSlicerStepSize      = 0;
    uint8_t                  rxExtSlicerGpioSel     = 0;
    uint8_t                  intSlicerLsbOnQ        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettings);

    /* Check slicer configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        slicerConfigSettings);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntDataFormatBfGet(device,
                                                       rxChannelBitfieldAddr,
                                                       &intDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntDataResolutionBfGet(device,
                                                           rxChannelBitfieldAddr,
                                                           &intDataResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntEmbedSlicerBfGet(device,
                                                        rxChannelBitfieldAddr,
                                                        &intEmbedSlicer);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntEmbedSlicerPosBfGet(device,
                                                           rxChannelBitfieldAddr,
                                                           &intEmbedSlicerPos);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntEmbedSlicerNumberBfGet(device,
                                                              rxChannelBitfieldAddr,
                                                              &intEmbedSlicerNumber);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntParitySupportBfGet(device,
                                                          rxChannelBitfieldAddr,
                                                          &intParitySupport);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntEvenParityBfGet(device,
                                                       rxChannelBitfieldAddr,
                                                       &intEvenParity);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxSlicerPinControlModeBfGet(device,
                                                              rxChannelBitfieldAddr,
                                                              &slicerPinCtrlMode);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxExternalSlicerPinControlStepBfGet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      &extSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxSlicerPinControlStepBfGet(device,
                                                              rxChannelBitfieldAddr,
                                                              &intSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfGet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      &rxExtSlicerGpioSel);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxIntSlicerLsbOnQBfGet(device,
                                                         rxChannelBitfieldAddr,
                                                         &intSlicerLsbOnQ);
        ADI_ERROR_RETURN(device->common.error.newAction);

        switch (rxChannel)
        {
        case(ADI_ADRV9025_RX1):
            slicerConfigSettings->rx1ExtSlicerGpioSelect = (adi_adrv9025_RxExtSlicerGpioSel_e)rxExtSlicerGpioSel;
            break;
        case(ADI_ADRV9025_RX2):
            slicerConfigSettings->rx2ExtSlicerGpioSelect = (adi_adrv9025_RxExtSlicerGpioSel_e)rxExtSlicerGpioSel;
            break;
        case(ADI_ADRV9025_RX3):
            slicerConfigSettings->rx3ExtSlicerGpioSelect = (adi_adrv9025_RxExtSlicerGpioSel_e)rxExtSlicerGpioSel;
            break;
        case(ADI_ADRV9025_RX4):
            slicerConfigSettings->rx4ExtSlicerGpioSelect = (adi_adrv9025_RxExtSlicerGpioSel_e)rxExtSlicerGpioSel;
            break;
        default:
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Invalid Rx Channel encountered while attempting to encode Rx Integer settings. Valid Rx Channels Rx1-Rx4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        }

        slicerConfigSettings->extSlicerStepSize = (adi_adrv9025_ExtSlicerStepSizes_e)extSlicerStepSize;
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntDataFormatBfGet(device,
                                                        orxChannelBitfieldAddr,
                                                        &intDataFormat);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntDataResolutionBfGet(device,
                                                            orxChannelBitfieldAddr,
                                                            &intDataResolution);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntEmbedSlicerBfGet(device,
                                                         orxChannelBitfieldAddr,
                                                         &intEmbedSlicer);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntEmbedSlicerPosBfGet(device,
                                                            orxChannelBitfieldAddr,
                                                            &intEmbedSlicerPos);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntEmbedSlicerNumberBfGet(device,
                                                               orxChannelBitfieldAddr,
                                                               &intEmbedSlicerNumber);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntParitySupportBfGet(device,
                                                           orxChannelBitfieldAddr,
                                                           &intParitySupport);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntEvenParityBfGet(device,
                                                        orxChannelBitfieldAddr,
                                                        &intEvenParity);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxSlicerPinControlStepBfGet(device,
                                                               orxChannelBitfieldAddr,
                                                               &intSlicerStepSize);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxIntSlicerLsbOnQBfGet(device,
                                                          orxChannelBitfieldAddr,
                                                          &intSlicerLsbOnQ);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel parameter encountered for Integer Format settings read back. Valid Rx channels are Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_RxIntEmbSlicerConfigEncode(device,
                                                         rxChannel,
                                                         intEmbedSlicer,
                                                         intEmbedSlicerPos,
                                                         intEmbedSlicerNumber,
                                                         &integerConfigSettings->intEmbeddedBits);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_RxIntSampleResFormatEncode(device,
                                                         intDataResolution,
                                                         intDataFormat,
                                                         &integerConfigSettings->intSampleResolution);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_RxIntParitySupportEncode(device,
                                                       rxChannel,
                                                       intParitySupport,
                                                       intEvenParity,
                                                       &integerConfigSettings->intParity);
    ADI_ERROR_RETURN(device->common.error.newAction);

    slicerConfigSettings->intSlicerStepSize = (adi_adrv9025_IntSlicerStepSizes_e)intSlicerStepSize;
    integerConfigSettings->intEmbeddedPos   = (adi_adrv9025_RxSlicerEmbeddedPos_e)intSlicerLsbOnQ;

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatEmbOvldMonitorSet(adi_adrv9025_Device_t*                           device,
                                               uint32_t                                         rxChannelMask,
                                               adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]              = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE           = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint8_t                   DEFAULT_INT_DATA_FORMAT       = 0; /* Int data format - 2s complement */
    static const uint8_t                   DEFAULT_INT_DATA_RESOLUTION   = 1; /* Int sample resolution - 16 bit */
    static const uint8_t                   LSB_POS_I_CHANNEL_ID          = 0;
    static const uint8_t                   LSB_PLUS_ONE_POS_I_CHANNEL_ID = 1;
    static const uint8_t                   LSB_POS_Q_CHANNEL_ID          = 0;
    static const uint8_t                   LSB_PLUS_ONE_POS_Q_CHANNEL_ID = 1;

    int32_t                 recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                rxChannelArrIndex     = 0;
    uint8_t                 enable                = 0;
    adrv9025_BfRxChanAddr_e rxChannelBitfieldAddr = ADRV9025_BF_RX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that embedded overload configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        embOvldConfigSettings);

    /*Disable gain compensation and set integer format register to default*/
    recoveryAction = adrv9025_RxDataFormatGainCompDisable(device,
                                                          rxChannelMask,
                                                          DEFAULT_INT_DATA_FORMAT,
                                                          DEFAULT_INT_DATA_RESOLUTION);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /*Iterate through channel mask and if set configure the corresponding Rx embedded overload indicator formatter*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /* If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address */
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_HIGH_THRESH))
            {
                /* Set the APD High Threshold exceeded indicator source */
                recoveryAction = adrv9025_RxApdHighSrcSelectBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  (uint8_t)embOvldConfigSettings->embeddedMonitorApdHighSrcSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_APD_LOW_THRESH))
            {
                /* Set the APD Low Threshold exceeded indicator source */
                recoveryAction = adrv9025_RxApdLowSrcSelectBfSet(device,
                                                                 rxChannelBitfieldAddr,
                                                                 (uint8_t)embOvldConfigSettings->embeddedMonitorApdLowSrcSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_HIGH_THRESH))
            {
                /* Set the HB2 High Threshold exceeded indicator source */
                recoveryAction = adrv9025_RxHb2HighSrcSelectBfSet(device,
                                                                  rxChannelBitfieldAddr,
                                                                  (uint8_t)embOvldConfigSettings->embeddedMonitorHb2HighSrcSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV9025_RX_EMB_MON_SRC_HB2_LOW_THRESH))
            {
                /* Set the HB2 Low Threshold exceeded indicator source */
                recoveryAction = adrv9025_RxHb2LowSrcSelectBfSet(device,
                                                                 rxChannelBitfieldAddr,
                                                                 (uint8_t)embOvldConfigSettings->embeddedMonitorHb2LowSrcSel);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* I Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatIBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_POS_I_CHANNEL_ID,
                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbQ);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* I Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatIBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_PLUS_ONE_POS_I_CHANNEL_ID,
                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Q Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatQBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_POS_Q_CHANNEL_ID,
                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbI);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Q Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatQBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_PLUS_ONE_POS_Q_CHANNEL_ID,
                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set invert Hb2 flag config */
            enable         = (embOvldConfigSettings->invertHb2Flag == 0) ? 0 : 1;
            recoveryAction = adrv9025_RxInvertHb2LowBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          enable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while writing invert Hb2 Flag configuration");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set invert Apd2 flag config */
            enable         = (embOvldConfigSettings->invertApdFlag == 0) ? 0 : 1;
            recoveryAction = adrv9025_RxInvertApdLowBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          enable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while writing invert Apd Flag configuration");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatEmbOvldMonitorGet(adi_adrv9025_Device_t*                           device,
                                               adi_adrv9025_RxChannels_e                        rxChannel,
                                               adi_adrv9025_EmbOverloadMonitorConfigSettings_t* embOvldConfigSettings)
{
    static const uint8_t LSB_POS_I_CHANNEL_ID          = 0;
    static const uint8_t LSB_PLUS_ONE_POS_I_CHANNEL_ID = 1;
    static const uint8_t LSB_POS_Q_CHANNEL_ID          = 0;
    static const uint8_t LSB_PLUS_ONE_POS_Q_CHANNEL_ID = 1;

    int32_t                 recoveryAction          = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e rxChannelBitfieldAddr   = ADRV9025_BF_RX_CH0;
    uint8_t                 apdHighSrcRegVal        = 0;
    uint8_t                 apdLowSrcRegVal         = 0;
    uint8_t                 hb2HighSrcRegVal        = 0;
    uint8_t                 hb2LowSrcRegVal         = 0;
    uint8_t                 lsbIMonSrcRegVal        = 0;
    uint8_t                 lsbQMonSrcRegVal        = 0;
    uint8_t                 lsbPlusOneIMonSrcRegVal = 0;
    uint8_t                 lsbPlusOneQMonSrcRegVal = 0;
    uint8_t                 invertHb2Flag           = 0;
    uint8_t                 invertApdFlag           = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that embedded overload configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        embOvldConfigSettings);

    /* If the target Rx channel is not an ORx channel, retrieve the embedded overload indicator config */
    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get the APD High Threshold exceeded indicator source */
        recoveryAction = adrv9025_RxApdHighSrcSelectBfGet(device,
                                                          rxChannelBitfieldAddr,
                                                          &apdHighSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get the APD Low Threshold exceeded indicator source */
        recoveryAction = adrv9025_RxApdLowSrcSelectBfGet(device,
                                                         rxChannelBitfieldAddr,
                                                         &apdLowSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get the HB2 High Threshold exceeded indicator source */
        recoveryAction = adrv9025_RxHb2HighSrcSelectBfGet(device,
                                                          rxChannelBitfieldAddr,
                                                          &hb2HighSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get the HB2 Low Threshold exceeded indicator source */
        recoveryAction = adrv9025_RxHb2LowSrcSelectBfGet(device,
                                                         rxChannelBitfieldAddr,
                                                         &hb2LowSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get I Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv9025_RxRxMonFormatIBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      LSB_POS_I_CHANNEL_ID,
                                                      &lsbQMonSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get I Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv9025_RxRxMonFormatIBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      LSB_PLUS_ONE_POS_I_CHANNEL_ID,
                                                      &lsbPlusOneQMonSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get Q Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv9025_RxRxMonFormatQBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      LSB_POS_Q_CHANNEL_ID,
                                                      &lsbIMonSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get Q Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv9025_RxRxMonFormatQBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      LSB_PLUS_ONE_POS_Q_CHANNEL_ID,
                                                      &lsbPlusOneIMonSrcRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get invert Hb2 flag config */
        recoveryAction = adrv9025_RxInvertHb2LowBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      &invertHb2Flag);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading invert Hb2 Flag configuration");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Get invert Apd flag config */
        recoveryAction = adrv9025_RxInvertApdLowBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      &invertApdFlag);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading invert Apd Flag configuration");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Update the pointer to embOvldConfigSettings with data retrieved from the device */
        embOvldConfigSettings->embeddedMonitorApdHighSrcSel  = (adi_adrv9025_RxApdHighEmbeddedMonitorSrc_e)apdHighSrcRegVal;
        embOvldConfigSettings->embeddedMonitorApdLowSrcSel   = (adi_adrv9025_RxApdLowEmbeddedMonitorSrc_e)apdLowSrcRegVal;
        embOvldConfigSettings->embeddedMonitorHb2HighSrcSel  = (adi_adrv9025_RxHb2HighEmbeddedMonitorSrc_e)hb2HighSrcRegVal;
        embOvldConfigSettings->embeddedMonitorHb2LowSrcSel   = (adi_adrv9025_RxHb2LowEmbeddedMonitorSrc_e)hb2LowSrcRegVal;
        embOvldConfigSettings->embeddedMonitorSrcLsbI        = (adi_adrv9025_RxEmbeddedMonitorSrc_e)lsbIMonSrcRegVal;
        embOvldConfigSettings->embeddedMonitorSrcLsbQ        = (adi_adrv9025_RxEmbeddedMonitorSrc_e)lsbQMonSrcRegVal;
        embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI = (adi_adrv9025_RxEmbeddedMonitorSrc_e)lsbPlusOneIMonSrcRegVal;
        embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ = (adi_adrv9025_RxEmbeddedMonitorSrc_e)lsbPlusOneQMonSrcRegVal;
        embOvldConfigSettings->invertHb2Flag                 = invertHb2Flag;
        embOvldConfigSettings->invertApdFlag                 = invertApdFlag;
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntEmbSlicerConfigResolve(adi_adrv9025_Device_t*              device,
                                             adi_adrv9025_RxDataFormatModes_e    formatSelect,
                                             adi_adrv9025_RxSlicerEmbeddedBits_e intEmbeddedBits,
                                             uint8_t*                            intEmbedSlicer,
                                             uint8_t*                            intEmbedSlicerPos,
                                             uint8_t*                            intEmbedSlicerNumber,
                                             uint8_t*                            staticSlicer3bitModeEn)
{
    static const uint8_t INT_EMBED_SLICER_ENABLE       = 1;
    static const uint8_t INT_EMBED_SLICER_DISABLE      = 0;
    static const uint8_t INT_EMBED_SLICER_POS_MSB      = 0;
    static const uint8_t INT_EMBED_SLICER_POS_LSB      = 1;
    static const uint8_t INT_EMBED_SLICER_1_BIT        = 1;
    static const uint8_t INT_EMBED_SLICER_2_BITS       = 0;
    static const uint8_t INT_SLICER_3_BIT_MODE_ENABLE  = 1;
    static const uint8_t INT_SLICER_3_BIT_MODE_DISABLE = 0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that write back pointers are not nullptrs */
    ADI_NULL_PTR_RETURN(&device->common,
                        intEmbedSlicer);
    ADI_NULL_PTR_RETURN(&device->common,
                        intEmbedSlicerPos);
    ADI_NULL_PTR_RETURN(&device->common,
                        intEmbedSlicerNumber);

    if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
    {
        /* Resolve slicer and slicer position configs */
        switch (intEmbeddedBits)
        {
        case ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_DISABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_1_BIT;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_1_BIT;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_ENABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_ENABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
            break;
        }

        case ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER:
        {
            *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
            *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
            *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
            break;
        }

        default:
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             integerConfigSettings->intEmbeddedBits,
                             "Invalid integer formatter embedded slicer mode config selected");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        }
    }
    else
    {
        /*For other data format modes set int embedded slicer config to default*/
        *intEmbedSlicer       = INT_EMBED_SLICER_DISABLE;
        *intEmbedSlicerPos    = INT_EMBED_SLICER_POS_MSB;
        *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;

        /*For a 3 pin internal slicer mode - enable the 3 pin slicer mode, for other modes, disable it*/
        if (formatSelect == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_ENABLE;
        }
        else
        {
            *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntEmbSlicerConfigEncode(adi_adrv9025_Device_t*               device,
                                            adi_adrv9025_RxChannels_e            rxChannel,
                                            uint8_t                              intEmbedSlicer,
                                            uint8_t                              intEmbedSlicerPos,
                                            uint8_t                              intEmbedSlicerNumber,
                                            adi_adrv9025_RxSlicerEmbeddedBits_e* integerConfigSettingsEmbSlicer)
{
    static const uint8_t INT_EMBED_SLICER_DISABLE = 0;
    static const uint8_t INT_EMBED_SLICER_POS_MSB = 0;
    static const uint8_t INT_EMBED_SLICER_POS_LSB = 1;
    static const uint8_t INT_EMBED_SLICER_1_BIT   = 1;
    static const uint8_t INT_EMBED_SLICER_2_BITS  = 0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration for Embedded Slicer setting enum is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettingsEmbSlicer);

    if (intEmbedSlicer == INT_EMBED_SLICER_DISABLE)
    {
        *integerConfigSettingsEmbSlicer = ADI_ADRV9025_NO_EMBEDDED_SLICER_BITS;
    }
    else
    {
        if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_1_SLICERBIT_AT_LSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_1_SLICERBIT_AT_MSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            if ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel)
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER;
            }
            else
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER;
            }
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            if ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel)
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER;
            }
            else
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV9025_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER;
            }
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intEmbedSlicer,
                             "Invalid combination of embed slicer,pos,number combination encountered while attempting to encode integer embed slicer settings");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntSampleResFormatResolve(adi_adrv9025_Device_t*                      device,
                                             const adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                             uint8_t*                                    intDataResolution,
                                             uint8_t*                                    intDataFormat)

{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT    = 0;
    static const uint8_t INT_DATA_FORMAT_SIGNED_MAGNITUDE = 1;
    static const uint8_t INT_DATA_RESOLUTION_12BITS       = 0;
    static const uint8_t INT_DATA_RESOLUTION_16BITS       = 1;
    static const uint8_t INT_DATA_RESOLUTION_24BITS       = 2;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettings);

    /* Check that write back pointers are not nullptrs */
    ADI_NULL_PTR_RETURN(&device->common,
                        intDataResolution);
    ADI_NULL_PTR_RETURN(&device->common,
                        intDataFormat);

    /*Resolve Sample Resolution*/
    switch (integerConfigSettings->intSampleResolution)
    {
    case ADI_ADRV9025_INTEGER_12BIT_2SCOMP:
    {
        *intDataResolution = INT_DATA_RESOLUTION_12BITS;
        *intDataFormat     = INT_DATA_FORMAT_2S_COMPLEMENT;
        break;
    }

    case ADI_ADRV9025_INTEGER_12BIT_SIGNED:
    {
        *intDataResolution = INT_DATA_RESOLUTION_12BITS;
        *intDataFormat     = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
        break;
    }

    case ADI_ADRV9025_INTEGER_16BIT_2SCOMP:
    {
        *intDataResolution = INT_DATA_RESOLUTION_16BITS;
        *intDataFormat     = INT_DATA_FORMAT_2S_COMPLEMENT;
        break;
    }

    case ADI_ADRV9025_INTEGER_16BIT_SIGNED:
    {
        *intDataResolution = INT_DATA_RESOLUTION_16BITS;
        *intDataFormat     = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
        break;
    }

    case ADI_ADRV9025_INTEGER_24BIT_2SCOMP:
    {
        *intDataResolution = INT_DATA_RESOLUTION_24BITS;
        *intDataFormat     = INT_DATA_FORMAT_2S_COMPLEMENT;
        break;
    }

    case ADI_ADRV9025_INTEGER_24BIT_SIGNED:
    {
        *intDataResolution = INT_DATA_RESOLUTION_24BITS;
        *intDataFormat     = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         integerConfigSettings->intSampleResolution,
                         "Invalid integer formatter data resolution selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntSampleResFormatEncode(adi_adrv9025_Device_t*                device,
                                            uint8_t                               intDataResolution,
                                            uint8_t                               intDataFormat,
                                            adi_adrv9025_RxIntSampleResolution_e* intSampleResolution)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT    = 0;
    static const uint8_t INT_DATA_FORMAT_SIGNED_MAGNITUDE = 1;
    static const uint8_t INT_DATA_RESOLUTION_12BITS       = 0;
    static const uint8_t INT_DATA_RESOLUTION_16BITS       = 1;
    static const uint8_t INT_DATA_RESOLUTION_24BITS       = 2;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format resolution enum is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        intSampleResolution);

    if ((intDataResolution == INT_DATA_RESOLUTION_12BITS) &&
        (intDataFormat == INT_DATA_FORMAT_2S_COMPLEMENT))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_12BIT_2SCOMP;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_12BITS) &&
        (intDataFormat == INT_DATA_FORMAT_SIGNED_MAGNITUDE))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_12BIT_SIGNED;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_16BITS) &&
        (intDataFormat == INT_DATA_FORMAT_2S_COMPLEMENT))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_16BIT_2SCOMP;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_16BITS) &&
        (intDataFormat == INT_DATA_FORMAT_SIGNED_MAGNITUDE))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_16BIT_SIGNED;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_24BITS) &&
        (intDataFormat == INT_DATA_FORMAT_2S_COMPLEMENT))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_24BIT_2SCOMP;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_24BITS) &&
        (intDataFormat == INT_DATA_FORMAT_SIGNED_MAGNITUDE))
    {
        *intSampleResolution = ADI_ADRV9025_INTEGER_24BIT_SIGNED;
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntParitySupportResolve(adi_adrv9025_Device_t*                      device,
                                           const adi_adrv9025_IntegerConfigSettings_t* integerConfigSettings,
                                           uint8_t*                                    intParitySupport,
                                           uint8_t*                                    intEvenParity)

{
    static const uint8_t INT_PARTIY_SUPPORT_ENABLE  = 1;
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE = 0;
    static const uint8_t INT_PARITY_EVEN            = 0;
    static const uint8_t INT_PARITY_ODD             = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerConfigSettings);

    /* Check that write back pointers are not nullptrs */
    ADI_NULL_PTR_RETURN(&device->common,
                        intParitySupport);
    ADI_NULL_PTR_RETURN(&device->common,
                        intEvenParity);

    switch (integerConfigSettings->intParity)
    {
    case ADI_ADRV9025_3BIT_SLICER_ODD_PARITY:
    {
        *intParitySupport = INT_PARTIY_SUPPORT_ENABLE;
        *intEvenParity    = INT_PARITY_ODD;
        break;
    }

    case ADI_ADRV9025_3BIT_SLICER_EVEN_PARITY:
    {
        *intParitySupport = INT_PARTIY_SUPPORT_ENABLE;
        *intEvenParity    = INT_PARITY_EVEN;
        break;
    }

    case ADI_ADRV9025_NO_PARITY:
    {
        *intParitySupport = INT_PARTIY_SUPPORT_DISABLE;
        *intEvenParity    = INT_PARITY_EVEN;
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         integerConfigSettings->intParity,
                         "Invalid integer formatter parity selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    return recoveryAction;
}

int32_t adrv9025_RxIntParitySupportEncode(adi_adrv9025_Device_t*      device,
                                          adi_adrv9025_RxChannels_e   rxChannel,
                                          uint8_t                     intParitySupport,
                                          uint8_t                     intEvenParity,
                                          adi_adrv9025_RxIntParity_e* integerParity)
{
    static const uint8_t INT_PARTIY_SUPPORT_ENABLE = 1;
    static const uint8_t INT_PARITY_EVEN           = 0;
    static const uint8_t INT_PARITY_ODD            = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check integer format configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        integerParity);

    if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
        (intEvenParity == INT_PARITY_ODD) &&
        ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel))
    {
        *integerParity = ADI_ADRV9025_3BIT_SLICER_ODD_PARITY;
    }
    else if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
        (intEvenParity == INT_PARITY_EVEN) &&
        ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel))
    {
        *integerParity = ADI_ADRV9025_3BIT_SLICER_EVEN_PARITY;
    }
    else
    {
        *integerParity = ADI_ADRV9025_NO_PARITY;
    }

    return recoveryAction;
}

int32_t adrv9025_RxInternalSlicerGpioEnable(adi_adrv9025_Device_t*           device,
                                            adi_adrv9025_RxDataFormatModes_e intSlicerFormatSel,
                                            adi_adrv9025_RxChannels_e        rxChannel)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                                = ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT;
    static const uint32_t             NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE = 4;
    static const uint32_t             NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE = 3;
    static const uint8_t              SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE           = 0x0A;
    static const uint8_t              SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE           = 0x0B;

    int32_t  recoveryAction                                                           = ADI_COMMON_ACT_NO_ACTION;
    int32_t  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_RX_INT_SLICER_CTRL_OUT_FEATURE] = {0};
    uint8_t  resourceAcqReleaseStatus                                                 = ADI_FAILURE;
    uint32_t gpioOutputDirMask                                                        = 0;
    uint32_t numberOfGpioPerChannel                                                   = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_11;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_10;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_09;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_08;
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_06; /* RX1_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_07; /* RX1_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_08; /* RX1_slicer[2] */
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for enabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_15;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_14;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_13;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_12;
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_09; /* RX2_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_10; /* RX2_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_11; /* RX2_slicer[2] */
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for enabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX3:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_07;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_06;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_05;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_04;
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_05) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_04);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_12; /* RX3_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_13; /* RX3_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_14; /* RX3_slicer[2] */
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_03;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_02;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_01;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_00;
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_03) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_02) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_01) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_00);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_15; /* RX4_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_16; /* RX4_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_17; /* RX4_slicer[2] */
            gpioOutputDirMask    = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_16) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_17);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for enabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel selected for internal slicer ctrl word observation GPIO output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Acquire Shared GPIOs to ensure that they are not in use by another feature*/
    recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                     ADRV9025_SHARED_RESOURCE_GPIO,
                                                     &sharedResourceArr[0],
                                                     numberOfGpioPerChannel,
                                                     THIS_FEATURE_ID,
                                                     &resourceAcqReleaseStatus);

    if (resourceAcqReleaseStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Unable to acquire GPIOs for internal slicer ctrl word observation use. Please check if the GPIOs are already in use");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*If GPIOs are available set the source ctrl to observe Rx slicer position*/
    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx1 slicer position GPIO[11:8] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx1 slicer position GPIO[6] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx1 slicer position GPIO[8:7] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx2 slicer position GPIO[15:12] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx2 slicer position GPIO[11:9] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX3:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx3 slicer position GPIO[7:4] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx3 slicer position GPIO[14:12] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_4PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[3:0] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[16:15] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfSet(device,
                                                                          ADRV9025_BF_CORE,
                                                                          SOURCE_CTRL_RX_SLICER_POSN_3PIN_MODE);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[17] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel selected for internal slicer ctrl word observation GPIO output");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Set the direction as output for selected Rx channel Slicer Position observation*/
    if ((recoveryAction = adi_adrv9025_GpioOutputDirSet(device,
                                                        gpioOutputDirMask)) != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recoveryAction = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Release Shared GPIOs if setting of GPIO Input Dir is unsuccessful*/
        recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         numberOfGpioPerChannel,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Unable to release GPIOs for internal slicer observation while attempting to exit the function due to GPIO dir set error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxInternalSlicerGpioDisable(adi_adrv9025_Device_t*           device,
                                             adi_adrv9025_RxChannels_e        rxChannel,
                                             adi_adrv9025_RxDataFormatModes_e intSlicerFormatSel)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                                = ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT;
    static const uint32_t             NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE = 4;
    static const uint32_t             NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE = 3;
    static const uint8_t              SOURCE_CTRL_DEFAULT                            = 0x00;

    int32_t  recoveryAction                                                           = ADI_COMMON_ACT_NO_ACTION;
    int32_t  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_RX_INT_SLICER_CTRL_OUT_FEATURE] = {0};
    uint8_t  resourceAcqReleaseStatus                                                 = ADI_FAILURE;
    uint32_t gpioInputDirMask                                                         = 0;
    uint32_t numberOfGpioPerChannel                                                   = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_11;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_10;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_09;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_08;
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_06; /* RX1_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_07; /* RX1_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_08; /* RX1_slicer[2] */
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for disabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_15;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_14;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_13;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_12;
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_09; /* RX2_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_10; /* RX2_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_11; /* RX2_slicer[2] */
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for disabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX3:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_07;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_06;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_05;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_04;
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_05) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_04);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_12; /* RX3_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_13; /* RX3_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_14; /* RX3_slicer[2] */
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for disabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_03;
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_02;
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_01;
            sharedResourceArr[3] = ADI_ADRV9025_GPIO_00;
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_03) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_02) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_01) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_00);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_4PIN_MODE;
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            sharedResourceArr[0] = ADI_ADRV9025_GPIO_15; /* RX4_slicer[0] */
            sharedResourceArr[1] = ADI_ADRV9025_GPIO_16; /* RX4_slicer[1] */
            sharedResourceArr[2] = ADI_ADRV9025_GPIO_17; /* RX4_slicer[2] */
            gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_16) |
            ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_17);

            numberOfGpioPerChannel = NUM_SHARED_INT_OBS_GPIOS_PER_CHANNEL_3PIN_MODE;
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             intSlicerFormatSel,
                             "Invalid Rx data format mode selected for disabling internal slicer GPIOs.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel selected for internal slicer ctrl word observation GPIO output disable");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Acquire Shared GPIOs to ensure that they are not in use by another feature*/
    recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                     ADRV9025_SHARED_RESOURCE_GPIO,
                                                     &sharedResourceArr[0],
                                                     numberOfGpioPerChannel,
                                                     THIS_FEATURE_ID,
                                                     &resourceAcqReleaseStatus);

    if (resourceAcqReleaseStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Unable to release requested GPIOs for internal slicer ctrl word observation use. Please check if the GPIOs are in use by another feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         device->common.error.errSource,
                         device->common.error.errCode,
                         device->common.error.newAction,
                         device->common.error.varName,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*If GPIOs are available clear the source ctrl for observe Rx slicer position*/
    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to clear source ctrl of Rx1 slicer position GPIO[11:8] - 4 Pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx1 slicer position GPIO[6] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx1 slicer position GPIO[7:8] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX2:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx2 slicer position GPIO[15:12] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx2 slicer position GPIO[11:9] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX3:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx3 slicer position GPIO[7:4] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx3 slicer position GPIO[14:12] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }

    case ADI_ADRV9025_RX4:
    {
        if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[3:0] - 4pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if (intSlicerFormatSel == ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                     ADRV9025_BF_CORE,
                                                                                     SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[16:15] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfSet(device,
                                                                          ADRV9025_BF_CORE,
                                                                          SOURCE_CTRL_DEFAULT);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Error while attempting to set source ctrl for Rx4 slicer position GPIO[17] - 3pin mode");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel selected for internal slicer ctrl word observation GPIO output disable");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    /*Set the direction as input for selected Rx channel Slicer Position observation on release*/
    if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                       gpioInputDirMask)) != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recoveryAction = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* Re-Acquire Shared GPIOs if setting of GPIO Input Dir is unsuccessful*/
        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         numberOfGpioPerChannel,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcqReleaseStatus);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Unable to re-acquire GPIOs for internal slicer observation while attempting to exit the function due to GPIO dir set error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             device->common.error.errSource,
                             device->common.error.errCode,
                             device->common.error.newAction,
                             device->common.error.varName,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxInternalSlicerGpioEnableGet(adi_adrv9025_Device_t*            device,
                                               adi_adrv9025_RxChannels_e         rxChannel,
                                               adi_adrv9025_RxDataFormatModes_e* intSlicerFormatSel,
                                               uint8_t*                          enableStatus)
{
    static const uint8_t SOURCE_CTRL_RX_SLICER_POSN_4BIT_MODE = 0x0A;
    static const uint8_t SOURCE_CTRL_RX_SLICER_POSN_3BIT_MODE = 0x0B;

    int32_t recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t sourceCtrlGet4PinMode  = 0;
    uint8_t sourceCtrlGet3PinMode1 = 0;
    uint8_t sourceCtrlGet3PinMode2 = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    switch (rxChannel)
    {
    case ADI_ADRV9025_RX1:
    {
        recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet4PinMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl of Rx1 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet3PinMode1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl for Rx1 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet3PinMode2);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl for Rx1 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        break;
    }

    case ADI_ADRV9025_RX2:
    {
        recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet4PinMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl of Rx2 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet3PinMode1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl for Rx2 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        break;
    }

    case ADI_ADRV9025_RX3:
    {
        recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet4PinMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl of Rx3 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet3PinMode1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl for Rx3 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction)

        break;
    }

    case ADI_ADRV9025_RX4:
    {
        recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet4PinMode);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to get source ctrl of Rx4 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 &sourceCtrlGet3PinMode1);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set source ctrl for Rx4 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfGet(device,
                                                                      ADRV9025_BF_CORE,
                                                                      &sourceCtrlGet3PinMode2);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to set source ctrl for Rx4 slicer position GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);

        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel selected for internal slicer ctrl word observation GPIO output status get");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    if (sourceCtrlGet4PinMode == SOURCE_CTRL_RX_SLICER_POSN_4BIT_MODE)
    {
        if (intSlicerFormatSel != NULL)
        {
            *intSlicerFormatSel = ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE;
        }
        if (enableStatus != NULL)
        {
            *enableStatus = 1;
        }
    }
    else if ((sourceCtrlGet3PinMode1 == SOURCE_CTRL_RX_SLICER_POSN_3BIT_MODE) ||
        (sourceCtrlGet3PinMode2 == SOURCE_CTRL_RX_SLICER_POSN_3BIT_MODE))
    {
        if (intSlicerFormatSel != NULL)
        {
            *intSlicerFormatSel = ADI_ADRV9025_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE;
        }
        if (enableStatus != NULL)
        {
            *enableStatus = 1;
        }
    }
    else
    {
        if (intSlicerFormatSel != NULL)
        {
            *intSlicerFormatSel = ADI_ADRV9025_GAIN_COMPENSATION_DISABLED;
        }
        if (enableStatus != NULL)
        {
            *enableStatus = 0;
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxExternalSlicerGpioEnable(adi_adrv9025_Device_t*                     device,
                                            adi_adrv9025_RxChannels_e                  rxChannel,
                                            const adi_adrv9025_SlicerConfigSettings_t* slicerConfigSettings)

{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                       = ADRV9025_FEATURE_RX_EXT_SLICER_CTRL;
    static const uint32_t             NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL = 3;

    int32_t                           recoveryAction                                                            = ADI_COMMON_ACT_NO_ACTION;
    int32_t                           sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_RX_EXT_SLICER_CTRL_WORD_FEATURE] = {0};
    adi_adrv9025_RxExtSlicerGpioSel_e rxExtSlicerGpioSel                                                        = ADI_ADRV9025_EXTSLICER_RX_GPIO_DISABLE;
    adrv9025_BfRxChanAddr_e           rxChannelBitfieldAddr                                                     = ADRV9025_BF_ALL_RX_CHANNELS;
    //adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ALL_ORX_CHANNELS;
    uint8_t  resourceAcqReleaseStatus = ADI_FAILURE;
    uint32_t gpioInputDirMask         = 0;
    uint8_t  isOrxChannel             = (uint8_t)ADI_FALSE;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check slicer configuration struct is not a nullptr */
    ADI_NULL_PTR_RETURN(&device->common,
                        slicerConfigSettings);

    /*Resolve External Slicer Ctrl GPIO sel*/
    if (rxChannel == ADI_ADRV9025_RX1)
    {
        rxExtSlicerGpioSel = slicerConfigSettings->rx1ExtSlicerGpioSelect;
    }
    else if (rxChannel == ADI_ADRV9025_RX2)
    {
        rxExtSlicerGpioSel = slicerConfigSettings->rx2ExtSlicerGpioSelect;
    }
    else if (rxChannel == ADI_ADRV9025_RX3)
    {
        rxExtSlicerGpioSel = slicerConfigSettings->rx3ExtSlicerGpioSelect;
    }
    else if (rxChannel == ADI_ADRV9025_RX4)
    {
        rxExtSlicerGpioSel = slicerConfigSettings->rx4ExtSlicerGpioSelect;
    }
        //else if (rxChannel == ADI_ADRV9025_ORX1)
        //{
        //    rxExtSlicerGpioSel = slicerConfigSettings->orx1ExtSlicerGpioSelect;
        //    isOrxChannel = ADI_TRUE;
        //}
        //else if (rxChannel == ADI_ADRV9025_ORX2)
        //{
        //    rxExtSlicerGpioSel = slicerConfigSettings->orx2ExtSlicerGpioSelect;
        //    isOrxChannel = ADI_TRUE;
        //}
        //else if (rxChannel == ADI_ADRV9025_ORX3)
        //{
        //    rxExtSlicerGpioSel = slicerConfigSettings->orx3ExtSlicerGpioSelect;
        //    isOrxChannel = ADI_TRUE;
        //}
        //else if (rxChannel == ADI_ADRV9025_ORX4)
        //{
        //    rxExtSlicerGpioSel = slicerConfigSettings->orx4ExtSlicerGpioSelect;
        //    isOrxChannel = ADI_TRUE;
        //}
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxExtSlicerGpioSel,
                         "Invalid Rx Channel encountered while attempting to configure External slicer for Rx Data Formatter. Valid channels include Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (rxExtSlicerGpioSel)
    {
    case ADI_ADRV9025_EXTSLICER_RX_GPIO_2_DOWNTO_0:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_00;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_01;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_02;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_00) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_01) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_02);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_5_DOWNTO_3:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_03;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_04;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_05;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_03) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_04) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_05);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_8_DOWNTO_6:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_06;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_07;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_08;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_11_DOWNTO_9:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_09;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_10;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_11;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_14_DOWNTO_12:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_12;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_13;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_14;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_17_DOWNTO_15:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_15;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_16;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_17;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_16) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_17);
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxExtSlicerGpioSel,
                         "Invalid integer formatter External ctrl GPIO selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Acquire Shared GPIOs to ensure that they are not in use by another feature*/
    recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                     ADRV9025_SHARED_RESOURCE_GPIO,
                                                     &sharedResourceArr[0],
                                                     NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                     THIS_FEATURE_ID,
                                                     &resourceAcqReleaseStatus);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (resourceAcqReleaseStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Unable to acquire GPIOs for external slicer ctrl word input use. Please check if the GPIOs are already in use");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Set the direction of external ctrl GPIOs as input*/
    if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                       gpioInputDirMask)) != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recoveryAction = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Release Shared GPIOs if setting of GPIO Input Dir is unsuccessful*/
        recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcqReleaseStatus);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Unable to release GPIOs for external slicer ctrl word input use while attempting to exit the function due to GPIO dir set error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         rxChannel,
                         "Error while attempting to set GPIO input direction while attempting to enable external slicer GPIO");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (isOrxChannel == (uint8_t)ADI_FALSE)
    {
        /*Resolve the corresponding Rx Bitfield Address*/
        if ((recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                            rxChannel,
                                                            &rxChannelBitfieldAddr)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /*Clear the error so that clean up functions execute*/
            recoveryAction = adrv9025_ErrorClear(&device->common);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Release Shared GPIOs if decoding of Rx bitfield address is unsuccessful*/
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                             THIS_FEATURE_ID,
                                                             &resourceAcqReleaseStatus);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannel,
                                 "Unable to release GPIOs for external slicer ctrl word input use while attempting to exit the function due to Rx bitfield addr decoding error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Invalid Rx Channel encountered while attempting to decode Rx bitfield address");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set the external ctrl GPIO select*/
        if ((recoveryAction = adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfSet(device,
                                                                           rxChannelBitfieldAddr,
                                                                           (uint8_t)rxExtSlicerGpioSel)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /*Clear the error so that clean up functions execute*/
            recoveryAction = adrv9025_ErrorClear(&device->common);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Release Shared GPIOs if setting of Rx slicer pin ctrl GPIO bitfield is unsuccessful*/
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                             THIS_FEATURE_ID,
                                                             &resourceAcqReleaseStatus);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannel,
                                 "Unable to release GPIOs for external slicer ctrl word input use while attempting to exit the function due to slicer pin ctrl bitfield set error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Error while attempting to set Rx slicer pin ctrl bitfield");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    //else
    //{
    //    /*Resolve the corresponding ORx Bitfield Address*/
    //    if ((recoveryAction = adrv9025_OrxBitfieldAddressGet(device, rxChannel, &orxChannelBitfieldAddr)) != ADI_COMMON_ACT_NO_ACTION)
    //    {
    //        /*Release Shared GPIOs if decoding of ORx bitfield address is unsuccessful*/
    //        recoveryAction = adrv9025_SharedResourcesRelease(device, ADRV9025_SHARED_RESOURCE_GPIO, &sharedResourceArr[0], NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
    //                                                 THIS_FEATURE_ID, &resourceAcqReleaseStatus);
    //        ADI_ERROR_RETURN(device->common.error.newAction);

    //        if (resourceAcqReleaseStatus == ADI_FAILURE)
    //        {
    //            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ACT_ERR_CHECK_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel,
    //                                  "Unable to release GPIOs for external slicer ctrl word input use while attempting to exit the function due to ORx bitfield addr decoding error");
    //            ADI_ERROR_RETURN(device->common.error.newAction);
    //        }

    //        ADI_ERROR_RETURN(device->common.error.newAction);
    //    }

    //    /*Set the external ctrl GPIO select*/
    //    if ((recoveryAction = adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfSet(device, orxChannelBitfieldAddr, (uint8_t)rxExtSlicerGpioSel)) != ADI_COMMON_ACT_NO_ACTION)
    //    {
    //        /*Release Shared GPIOs if setting of ORx slicer pin ctrl GPIO bitfield is unsuccessful*/
    //        recoveryAction = adrv9025_SharedResourcesRelease(device, ADRV9025_SHARED_RESOURCE_GPIO, &sharedResourceArr[0], NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
    //                                                 THIS_FEATURE_ID, &resourceAcqReleaseStatus);
    //        ADI_ERROR_RETURN(device->common.error.newAction);

    //        if (resourceAcqReleaseStatus == ADI_FAILURE)
    //        {
    //            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ACT_ERR_CHECK_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel,
    //                                  "Unable to release GPIOs for external slicer ctrl word input use while attempting to exit the function due to slicer pin ctrl bitfield set error");
    //            ADI_ERROR_RETURN(device->common.error.newAction);
    //        }

    //        ADI_ERROR_RETURN(device->common.error.newAction);
    //    }
    //}

    return recoveryAction;
}

int32_t adrv9025_RxExternalSlicerGpioDisable(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_RxChannels_e rxChannel)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID                       = ADRV9025_FEATURE_RX_EXT_SLICER_CTRL;
    static const uint32_t             NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL = 3;
    static const uint8_t              DEFAULT_GPIO_SELECT_VALUE             = 0;
    static const uint32_t             ALL_RX_MASK                           = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 |
        ADI_ADRV9025_RX4);
    static const uint32_t ALL_ORX_MASK = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 |
        ADI_ADRV9025_ORX4);

    int32_t                  recoveryAction                                                            = ADI_COMMON_ACT_NO_ACTION;
    int32_t                  sharedResourceArr[ADI_ADRV9025_NUM_GPIOS_RX_EXT_SLICER_CTRL_WORD_FEATURE] = {0};
    uint8_t                  rxExtSlicerGpioSelBf                                                      = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr                                                     = ADRV9025_BF_ALL_RX_CHANNELS;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr                                                    = ADRV9025_BF_ALL_ORX_CHANNELS;
    uint8_t                  resourceAcqReleaseStatus                                                  = ADI_FAILURE;
    uint32_t                 gpioInputDirMask                                                          = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

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
                         "Invalid Rx channel encountered while attempting to disable External ctrl slicer GPIOs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfGet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      &rxExtSlicerGpioSelBf);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (((uint32_t)rxChannel & ALL_ORX_MASK) > 0)
    {
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfGet(device,
                                                                       orxChannelBitfieldAddr,
                                                                       &rxExtSlicerGpioSelBf);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx channel encountered while attempting to disable External ctrl slicer GPIOs");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (rxExtSlicerGpioSelBf)
    {
    case ADI_ADRV9025_EXTSLICER_RX_GPIO_2_DOWNTO_0:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_00;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_01;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_02;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_00) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_01) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_02);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_5_DOWNTO_3:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_03;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_04;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_05;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_03) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_04) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_05);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_8_DOWNTO_6:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_06;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_07;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_08;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_06) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_07) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_08);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_11_DOWNTO_9:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_09;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_10;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_11;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_09) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_10) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_11);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_14_DOWNTO_12:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_12;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_13;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_14;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_12) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_13) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_14);
        break;
    }

    case ADI_ADRV9025_EXTSLICER_RX_GPIO_17_DOWNTO_15:
    {
        sharedResourceArr[0] = ADI_ADRV9025_GPIO_15;
        sharedResourceArr[1] = ADI_ADRV9025_GPIO_16;
        sharedResourceArr[2] = ADI_ADRV9025_GPIO_17;
        gpioInputDirMask     = ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_15) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_16) |
        ((uint32_t)0x00000001 << ADI_ADRV9025_GPIO_17);
        break;
    }

    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxExtSlicerGpioSel,
                         "Invalid integer formatter External ctrl GPIO selection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /*Release Shared GPIOs to ensure that they are not in use by another feature*/
    recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                     ADRV9025_SHARED_RESOURCE_GPIO,
                                                     &sharedResourceArr[0],
                                                     NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                     THIS_FEATURE_ID,
                                                     &resourceAcqReleaseStatus);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (resourceAcqReleaseStatus == ADI_FAILURE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Unable to release GPIOs for external slicer ctrl word input use. Please check if the GPIOs are already in use");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Set the External Ctrl GPIO select register to default*/
    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        /*Set the external ctrl GPIO select to default */
        if ((recoveryAction = adrv9025_RxRxdpSlicerPinCntrlGpioSelectBfSet(device,
                                                                           rxChannelBitfieldAddr,
                                                                           DEFAULT_GPIO_SELECT_VALUE)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /*Clear the error so that clean up functions execute*/
            recoveryAction = adrv9025_ErrorClear(&device->common);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Release Shared GPIOs if setting of Rx slicer pin ctrl GPIO bitfield is unsuccessful*/
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourceArr[0],
                                                             NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                             THIS_FEATURE_ID,
                                                             &resourceAcqReleaseStatus);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 rxChannel,
                                 "Unable to re-acquire GPIOs for external slicer ctrl word input use while attempting to exit the function due to slicer pin ctrl bitfield set error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Error while attempting to write to Rx slicer ctrl bitfield");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        /*Set the external ctrl GPIO select to default */
        //     if ((recoveryAction = adrv9025_OrxRxdpSlicerPinCntrlGpioSelectBfSet(device, orxChannelBitfieldAddr, DEFAULT_GPIO_SELECT_VALUE)) != ADI_COMMON_ACT_NO_ACTION)
        //     {
        ///*Clear the error so that clean up functions execute*/
        //recoveryAction = adrv9025_ErrorClear(&device->common);
        //ADI_ERROR_RETURN(device->common.error.newAction);

        //         /*Release Shared GPIOs if setting of ORx slicer pin ctrl GPIO bitfield is unsuccessful*/
        //         recoveryAction = adrv9025_SharedResourcesRelease(device, ADRV9025_SHARED_RESOURCE_GPIO, &sharedResourceArr[0], NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
        //                                                  THIS_FEATURE_ID, &resourceAcqReleaseStatus);
        //         ADI_ERROR_RETURN(device->common.error.newAction);

        //         if (resourceAcqReleaseStatus == ADI_FAILURE)
        //         {
        //             ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel,
        //                                   "Unable to re-acquire GPIOs for external slicer ctrl word input use while attempting to exit the function due to slicer pin ctrl bitfield set error");
        //             ADI_ERROR_RETURN(device->common.error.newAction);
        //         }

        //ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, rxChannel,
        //	                  "Error while attempting to write to ORx slicer ctrl bitfield");
        //         ADI_ERROR_RETURN(device->common.error.newAction);
        //     }
    }

    /*Set the direction of external ctrl GPIOs as input*/
    if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                       gpioInputDirMask)) != ADI_COMMON_ACT_NO_ACTION)
    {
        /*Clear the error so that clean up functions execute*/
        recoveryAction = adrv9025_ErrorClear(&device->common);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /*Re-acquire Shared GPIOs if setting of GPIO Input Dir is unsuccessful*/
        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         NUM_SHARED_EXT_CTRL_GPIOS_PER_CHANNEL,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcqReleaseStatus);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Unable to re-acquire GPIOs for external slicer ctrl word input use while attempting to exit the function due to GPIO dir set error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Error while attempting to set GPIO direction to input in adrv9025_RxExternalSlicerGpioDisable function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxDataFormatGainCompDisable(adi_adrv9025_Device_t* device,
                                             uint32_t               rxChannelMask,
                                             uint8_t                intDataFormat,
                                             uint8_t                intDataResolution)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]                = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]               = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE             = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint32_t                  ORX_CHANNEL_ARR_SIZE            = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0]);
    static const uint8_t                   DISABLE_GAIN_COMPENSATION       = 0;
    static const uint8_t                   DISABLE_FLOATING_POINT          = 0;
    static const uint8_t                   DISABLE_INT_SLICER_EXT_PIN_CTRL = 0;
    static const uint8_t                   DEFAULT_INT_EMBED_SLICER        = 0;
    static const uint8_t                   DEFAULT_INT_EMBED_SLICER_POS    = 0;
    static const uint8_t                   DEFAULT_INT_EMBED_SLICER_NUM    = 0;
    static const uint8_t                   DEFAULT_INT_PARITY_SUPPORT      = 0;
    static const uint8_t                   DEFAULT_INT_EVEN_PARITY         = 0;
    static const uint8_t                   LSB_POS_I_CHANNEL_ID            = 0;
    static const uint8_t                   LSB_PLUS_ONE_POS_I_CHANNEL_ID   = 1;
    static const uint8_t                   LSB_POS_Q_CHANNEL_ID            = 0;
    static const uint8_t                   LSB_PLUS_ONE_POS_Q_CHANNEL_ID   = 1;

    int32_t                          recoveryAction                 = ADI_COMMON_ACT_NO_ACTION;
    uint32_t                         rxChannelArrIndex              = 0;
    uint8_t                          gainCompEnStatus               = 0;
    uint8_t                          extPinCtrlStatus               = 0;
    uint8_t                          intEmbedStatus                 = 0;
    uint8_t                          floatingPointEnStatus          = 0;
    uint8_t                          internalSlicerGpioEnableStatus = 0;
    adi_adrv9025_RxDataFormatModes_e internalSlicerMode             = ADI_ADRV9025_GAIN_COMPENSATION_DISABLED;
    adrv9025_BfRxChanAddr_e          rxChannelBitfieldAddr          = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e         orxChannelBitfieldAddr         = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Iterate through channel mask and if set disable gain compensation on the corresponding channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Please note that even though gain compensation is disabled,
            the embedded slicer bits have to be explicitly set to 0 failing
            which the received data will contain slicer bits if it was previously
            enabled */

            recoveryAction = adrv9025_RxIntDataFormatBfSet(device,
                                                           rxChannelBitfieldAddr,
                                                           intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxIntDataResolutionBfSet(device,
                                                               rxChannelBitfieldAddr,
                                                               intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxGainCompEnableBfGet(device,
                                                            rxChannelBitfieldAddr,
                                                            &gainCompEnStatus);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Disable I Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatIBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_POS_I_CHANNEL_ID,
                                                          ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Disable I Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatIBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_PLUS_ONE_POS_I_CHANNEL_ID,
                                                          ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Disable Q Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatQBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_POS_Q_CHANNEL_ID,
                                                          ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Disable Q Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv9025_RxRxMonFormatQBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          LSB_PLUS_ONE_POS_Q_CHANNEL_ID,
                                                          ADI_ADRV9025_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (gainCompEnStatus > 0)
            {
                recoveryAction = adrv9025_RxFpEnBfGet(device,
                                                      rxChannelBitfieldAddr,
                                                      &floatingPointEnStatus);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (floatingPointEnStatus > 0)
                {
                    /*Disable floating point formatter*/
                    recoveryAction = adrv9025_RxFpEnBfSet(device,
                                                          rxChannelBitfieldAddr,
                                                          DISABLE_FLOATING_POINT);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                else
                {
                    recoveryAction = adrv9025_RxSlicerPinControlModeBfGet(device,
                                                                          rxChannelBitfieldAddr,
                                                                          &extPinCtrlStatus);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    if (extPinCtrlStatus > 0)
                    {
                        /*Unassign and release shared GPIOs*/
                        recoveryAction = adrv9025_RxExternalSlicerGpioDisable(device,
                                                                              RX_CHANNEL_ARR[rxChannelArrIndex]);
                        ADI_ERROR_RETURN(device->common.error.newAction);

                        /*Disable Ext Slicer Pin Ctrl*/
                        recoveryAction = adrv9025_RxSlicerPinControlModeBfSet(device,
                                                                              rxChannelBitfieldAddr,
                                                                              DISABLE_INT_SLICER_EXT_PIN_CTRL);
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }

                    recoveryAction = adrv9025_RxIntEmbedSlicerBfGet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    &intEmbedStatus);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_RxInternalSlicerGpioEnableGet(device,
                                                                            RX_CHANNEL_ARR[rxChannelArrIndex],
                                                                            &internalSlicerMode,
                                                                            &internalSlicerGpioEnableStatus);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    if ((intEmbedStatus == 0) && (internalSlicerGpioEnableStatus > 0))
                    {
                        recoveryAction = adrv9025_RxInternalSlicerGpioDisable(device,
                                                                              RX_CHANNEL_ARR[rxChannelArrIndex],
                                                                              internalSlicerMode);
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }

                    /*Set Slicer format configs to default*/
                    recoveryAction = adrv9025_RxIntEmbedSlicerBfSet(device,
                                                                    rxChannelBitfieldAddr,
                                                                    DEFAULT_INT_EMBED_SLICER);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_RxIntEmbedSlicerPosBfSet(device,
                                                                       rxChannelBitfieldAddr,
                                                                       DEFAULT_INT_EMBED_SLICER_POS);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_RxIntEmbedSlicerNumberBfSet(device,
                                                                          rxChannelBitfieldAddr,
                                                                          DEFAULT_INT_EMBED_SLICER_NUM);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_RxIntParitySupportBfSet(device,
                                                                      rxChannelBitfieldAddr,
                                                                      DEFAULT_INT_PARITY_SUPPORT);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_RxIntEvenParityBfSet(device,
                                                                   rxChannelBitfieldAddr,
                                                                   DEFAULT_INT_EVEN_PARITY);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /*Disable Gain Compensation*/
                recoveryAction = adrv9025_RxGainCompEnableBfSet(device,
                                                                rxChannelBitfieldAddr,
                                                                DISABLE_GAIN_COMPENSATION);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    /* Disable ORx channel settings if selected */
    for (rxChannelArrIndex = 0; rxChannelArrIndex < ORX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelArrIndex],
                                                            &orxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /*Please note that even though gain compensation is disabled,
            the embedded slicer bits have to be explicitly set to 0 failing
            which the received data will contain slicer bits if it was previously
            enabled */

            recoveryAction = adrv9025_OrxIntDataFormatBfSet(device,
                                                            orxChannelBitfieldAddr,
                                                            intDataFormat);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxIntDataResolutionBfSet(device,
                                                                orxChannelBitfieldAddr,
                                                                intDataResolution);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxGainCompEnableBfGet(device,
                                                             orxChannelBitfieldAddr,
                                                             &gainCompEnStatus);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (gainCompEnStatus > 0)
            {
                recoveryAction = adrv9025_OrxFpEnBfGet(device,
                                                       orxChannelBitfieldAddr,
                                                       &floatingPointEnStatus);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (floatingPointEnStatus > 0)
                {
                    /*Disable floating point formatter*/
                    recoveryAction = adrv9025_OrxFpEnBfSet(device,
                                                           orxChannelBitfieldAddr,
                                                           DISABLE_FLOATING_POINT);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
                else
                {
                    /*Set Slicer format configs to default*/
                    recoveryAction = adrv9025_OrxIntEmbedSlicerBfSet(device,
                                                                     orxChannelBitfieldAddr,
                                                                     DEFAULT_INT_EMBED_SLICER);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_OrxIntEmbedSlicerPosBfSet(device,
                                                                        orxChannelBitfieldAddr,
                                                                        DEFAULT_INT_EMBED_SLICER_POS);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_OrxIntEmbedSlicerNumberBfSet(device,
                                                                           orxChannelBitfieldAddr,
                                                                           DEFAULT_INT_EMBED_SLICER_NUM);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_OrxIntParitySupportBfSet(device,
                                                                       orxChannelBitfieldAddr,
                                                                       DEFAULT_INT_PARITY_SUPPORT);
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    recoveryAction = adrv9025_OrxIntEvenParityBfSet(device,
                                                                    orxChannelBitfieldAddr,
                                                                    DEFAULT_INT_EVEN_PARITY);
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /*Disable Gain Compensation*/
                recoveryAction = adrv9025_OrxGainCompEnableBfSet(device,
                                                                 orxChannelBitfieldAddr,
                                                                 DISABLE_GAIN_COMPENSATION);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainCompExtLnaSet(adi_adrv9025_Device_t* device,
                                     uint32_t               rxChannelMask,
                                     uint8_t                externalLnaGain)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]     = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]    = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE  = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint32_t                  ORX_CHANNEL_ARR_SIZE = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0]);

    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                  rxChannelArrIndex      = 0;
    uint8_t                  externalLnaGainEn      = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    externalLnaGainEn = (externalLnaGain > 0) ? 1 : 0;

    /*Iterate through channel mask and if set write the ext gain bitfield of the corresponding Rx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxGainCompForExtGainBfSet(device,
                                                                rxChannelBitfieldAddr,
                                                                externalLnaGainEn);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Iterate through channel mask and if set write the ext gain bitfield of the corresponding ORx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < ORX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelArrIndex],
                                                            &orxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxGainCompForExtGainBfSet(device,
                                                                 orxChannelBitfieldAddr,
                                                                 externalLnaGainEn);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainCompExtLnaGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_RxChannels_e rxChannel,
                                     uint8_t*                  externalLnaGain)
{
    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        externalLnaGain);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxGainCompForExtGainBfGet(device,
                                                            rxChannelBitfieldAddr,
                                                            externalLnaGain);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxGainCompForExtGainBfGet(device,
                                                             orxChannelBitfieldAddr,
                                                             externalLnaGain);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx channel encountered while attempting to retrieve Ext LNA gain Enable");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainCompTempEnableSet(adi_adrv9025_Device_t* device,
                                         uint32_t               rxChannelMask,
                                         uint8_t                tempCompensationEnable)
{
    static const adi_adrv9025_RxChannels_e RX_CHANNEL_ARR[]     = {ADI_ADRV9025_RX1, ADI_ADRV9025_RX2, ADI_ADRV9025_RX3, ADI_ADRV9025_RX4};
    static const adi_adrv9025_RxChannels_e ORX_CHANNEL_ARR[]    = {ADI_ADRV9025_ORX1, ADI_ADRV9025_ORX2, ADI_ADRV9025_ORX3, ADI_ADRV9025_ORX4};
    static const uint32_t                  RX_CHANNEL_ARR_SIZE  = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const uint32_t                  ORX_CHANNEL_ARR_SIZE = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0]);

    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                  rxChannelArrIndex      = 0;
    uint8_t                  tempCompensationEn     = 0;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    tempCompensationEn = (tempCompensationEnable > 0) ? 1 : 0;

    /*Iterate through channel mask and if set write the ext gain bitfield of the corresponding Rx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < RX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)RX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                           RX_CHANNEL_ARR[rxChannelArrIndex],
                                                           &rxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_RxGainCompForTempGainBfSet(device,
                                                                 rxChannelBitfieldAddr,
                                                                 tempCompensationEn);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Iterate through channel mask and if set write the ext gain bitfield of the corresponding ORx channel*/
    for (rxChannelArrIndex = 0; rxChannelArrIndex < ORX_CHANNEL_ARR_SIZE; rxChannelArrIndex++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex]) == (uint32_t)ORX_CHANNEL_ARR[rxChannelArrIndex])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx Bitfield Address*/
            recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                            ORX_CHANNEL_ARR[rxChannelArrIndex],
                                                            &orxChannelBitfieldAddr);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_OrxGainCompForTempGainBfSet(device,
                                                                  orxChannelBitfieldAddr,
                                                                  tempCompensationEn);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxGainCompTempEnableGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         uint8_t*                  tempCompensationEnable)
{
    int32_t                  recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfRxChanAddr_e  rxChannelBitfieldAddr  = ADRV9025_BF_RX_CH0;
    adrv9025_BfOrxChanAddr_e orxChannelBitfieldAddr = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        tempCompensationEnable);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2) ||
        (rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        recoveryAction = adrv9025_RxBitfieldAddressGet(device,
                                                       rxChannel,
                                                       &rxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_RxGainCompForTempGainBfGet(device,
                                                             rxChannelBitfieldAddr,
                                                             tempCompensationEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((rxChannel == ADI_ADRV9025_ORX1) ||
        (rxChannel == ADI_ADRV9025_ORX2) ||
        (rxChannel == ADI_ADRV9025_ORX3) ||
        (rxChannel == ADI_ADRV9025_ORX4))
    {
        recoveryAction = adrv9025_OrxBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &orxChannelBitfieldAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_OrxGainCompForTempGainBfGet(device,
                                                              orxChannelBitfieldAddr,
                                                              tempCompensationEnable);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx channel encountered while attempting to retrieve temparature gain compensation Enable");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_RxDecPowerGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ORX12_MASK  = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint32_t ORX34_MASK  = (uint32_t)(ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /*Check that the requested rxChannel is valid*/
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
                         "Invalid Rx Channel Requested for manual rx dec power read");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that rx profile is valid in current config*/
    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        if (((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == 0)
            || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Dec Power read requested for an Rx channel but Rx profile is invalid or channel not initialized in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Check that orx profile is valid in current config*/
    if (((uint32_t)rxChannel & (ORX12_MASK | ORX34_MASK)) > 0)
    {
        if (((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == 0)
            || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "Dec Power read requested for an ORx channel but ORx profile is invalid or channel not initialized in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxLoSourceGetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel,
                                         adi_adrv9025_LoSel_e*     rxLoSource)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint32_t ORX12_MASK  = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint32_t ORX34_MASK  = (uint32_t)(ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxLoSource);

    /*Check that the requested rxChannel is valid*/
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
                         "Invalid Rx channel selected for LO source mapping read back. Valid Rx channels are Rx1-Rx4, ORx1-ORx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that rx profile is valid in current config*/
    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0)
    {
        if (((device->devStateInfo.profilesValid & ADI_ADRV9025_RX_PROFILE_VALID) == 0)
            || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "LO source read back requested for an Rx channel but Rx profile is invalid or channel not initialized in the device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /*Check that orx profile is valid in current config*/
    if (((uint32_t)rxChannel & (ORX12_MASK | ORX34_MASK)) > 0)
    {
        if (((device->devStateInfo.profilesValid & ADI_ADRV9025_ORX_PROFILE_VALID) == 0)
            || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             rxChannel,
                             "LO source read back requested for an ORx channel but ORx profile is invalid or channel not initialized in the device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxLoSourceGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_RxChannels_e rxChannel,
                               adi_adrv9025_LoSel_e*     rxLoSource)
{
    static const uint8_t RX_LO2_MASK = 0x01;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t rxtxLoMuxReg   = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxLoSource);

    if ((rxChannel == ADI_ADRV9025_RX1) ||
        (rxChannel == ADI_ADRV9025_RX2))
    {
        /*Read the Rx Tx Lo Mux reg from the west side of the chip (Aux PLL regs)*/
        recoveryAction = adrv9025_PllMemMapRx12SelLo2BfGet(device,
                                                           ADRV9025_BF_AUX_PLL,
                                                           &rxtxLoMuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if ((rxChannel == ADI_ADRV9025_RX3) ||
        (rxChannel == ADI_ADRV9025_RX4))
    {
        /*Read the Rx Tx Lo Mux reg from the west side of the chip (Aux PLL regs)*/
        recoveryAction = adrv9025_PllMemMapRx34SelLo2BfGet(device,
                                                           ADRV9025_BF_AUX_PLL,
                                                           &rxtxLoMuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxtxLoMuxReg & RX_LO2_MASK) == 1)
    {
        *rxLoSource = ADI_ADRV9025_LOSEL_LO2;
    }
    else
    {
        *rxLoSource = ADI_ADRV9025_LOSEL_LO1;
    }

    return recoveryAction;
}

int32_t adrv9025_OrxLoSourceGet(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_RxChannels_e rxChannel,
                                adi_adrv9025_LoSel_e*     rxLoSource)
{
    static const uint32_t ORX_LOMUX_SEL_MASK = 0x01;
    static const uint32_t ORX12_MASK         = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2);
    static const uint32_t ORX34_MASK         = (uint32_t)(ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t orxLomuxReg    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxLoSource);

    if (((uint32_t)rxChannel & ORX12_MASK) > 0)
    {
        recoveryAction = adrv9025_AnalogOrxMemMapLomuxSelBfGet(device,
                                                               ADRV9025_BF_ANALOG_ORX_CH0,
                                                               &orxLomuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if ((orxLomuxReg & ORX_LOMUX_SEL_MASK) == ORX_LOMUX_SEL_MASK)
        {
            *rxLoSource = ADI_ADRV9025_LOSEL_AUXLO;
        }
        else
        {
            recoveryAction = adrv9025_TxLoSourceGet(device,
                                                    ADI_ADRV9025_TX1,
                                                    rxLoSource);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else if (((uint32_t)rxChannel & ORX34_MASK) > 0)
    {
        recoveryAction = adrv9025_AnalogOrxMemMapLomuxSelBfGet(device,
                                                               ADRV9025_BF_ANALOG_ORX_CH1,
                                                               &orxLomuxReg);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if ((orxLomuxReg & ORX_LOMUX_SEL_MASK) == ORX_LOMUX_SEL_MASK)
        {
            *rxLoSource = ADI_ADRV9025_LOSEL_AUXLO;
        }
        else
        {
            recoveryAction = adrv9025_TxLoSourceGet(device,
                                                    ADI_ADRV9025_TX3,
                                                    rxLoSource);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_RxAgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*  device,
                                                 adrv9025_BfRxChanAddr_e rxChannelBitFieldAddr)
{
    static const uint32_t AGC_GAIN_INDEX_OFFSET = 0x9E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dummyValue     = 0xFF;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               rxChannelBitFieldAddr + AGC_GAIN_INDEX_OFFSET,
                                               dummyValue);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);

    return recoveryAction;
}

int32_t adrv9025_OrxOrx1AgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e orxChannelBitFieldAddr)
{
    static const uint32_t AGC_GAIN_INDEX_OFFSET_ORX1 = 0xBB;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dummyValue     = 0xFF;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               orxChannelBitFieldAddr + AGC_GAIN_INDEX_OFFSET_ORX1,
                                               dummyValue);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);

    return recoveryAction;
}

int32_t adrv9025_OrxOrx2AgcEnableGainIndexUpdateLatch(adi_adrv9025_Device_t*   device,
                                                      adrv9025_BfOrxChanAddr_e orxChannelBitFieldAddr)
{
    static const uint32_t AGC_GAIN_INDEX_OFFSET_ORX2 = 0xBC;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t dummyValue     = 0xFF;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               orxChannelBitFieldAddr + AGC_GAIN_INDEX_OFFSET_ORX2,
                                               dummyValue);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     device->common.error.varName,
                     device->common.error.errormessage);

    return recoveryAction;
}
