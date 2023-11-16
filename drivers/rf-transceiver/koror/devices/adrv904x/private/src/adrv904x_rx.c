/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_rx.c
 * \brief Contains ADRV904X Rx related private function implementations
 *
 * ADRV904X API Version: 2.9.0.4
 */

#include "../../private/include/adrv904x_rx.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/bf/adrv904x_bf_rx_funcs.h"
#include "../../private/bf/adrv904x_bf_rx_ddc.h"
#include "../../private/bf/adrv904x_bf_orx_dig.h"
#include "../../private/bf/adrv904x_bf_rx_dig.h"
#include "../../private/bf/adrv904x_bf_pll_mem_map.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_cpu.h"

#include "../../private/bf/adrv904x_bf_cddc_funcs.h"
#include "../../private/bf/adrv904x_bf_cddc_hb_dpath.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_RX

static const adi_adrv904x_GpioSignal_e adrv904x_RxInternalSlicerOutputSignals[ADI_ADRV904X_MAX_RX_INT_SLICER_BITS] = { 
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_0,
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_1,
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_2,
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_3
};

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainTableFormat(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_RxGainTableRow_t gainTablePtr[],
                                                            uint8_t                             formattedGainTablePtr[],
                                                            const uint16_t                      numGainIndicesInTable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8U;
    const uint16_t DIG_GAIN_LSB_MASK = 0x00FFU;
    const uint16_t DIG_GAIN_MSB_MASK = 0x0300U;
    const uint8_t  DIG_GAIN_MSB_SHIFT = 8U;
    const uint16_t DIG_GAIN_SIGN_MASK = 0x0400U;
    const uint8_t  DIG_GAIN_SIGN_SHIFT = 8U;
    const uint8_t  RXFE_GAIN_LSB_MASK = 0x1FU;
    const uint8_t  RXFE_GAIN_LSB_SHIFT = 3U;
    const uint8_t  RXFE_GAIN_MSB_MASK = 0xE0U;
    const uint8_t  RXFE_GAIN_MSB_SHIFT = 5U;
    const uint16_t PHASE_OFFSET_LSB_MASK = 0x00FFU;
    const uint16_t PHASE_OFFSET_MSB_MASK = 0xFF00U;
    const uint8_t  PHASE_OFFSET_MSB_SHIFT = 8U;
    const uint8_t  EXT_CTRL_MASK = 0x03U;

    uint32_t gainIndex = 0U;
    uint32_t formattedGainIndex = 0U;
    uint16_t sign = 0U;
    uint16_t magnitude = 0U;
    

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gainTablePtr);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, formattedGainTablePtr);
    
    /*Format gain table*/
    for (gainIndex = 0U; gainIndex < numGainIndicesInTable; ++gainIndex)
    {
        /*Format Gain Table Data as follows :
        No. of Bytes per Gain Table Entry = 8
        @Addr 8 x gainIndex     : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
        @Addr 8 x gainIndex + 4 : {14'b0, ext_control[1:0], phase_offset[15:0]}
        */
        formattedGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);
        
        if (gainTablePtr[gainIndex].digGain < 0)
        {
            sign = DIG_GAIN_SIGN_MASK; /* set bit 11 */
            magnitude = (((~(uint16_t)gainTablePtr[gainIndex].digGain) + 1U) & 0x3FFU);
        }
        else
        {
            sign = 0U;
            magnitude = gainTablePtr[gainIndex].digGain & 0x3FFU;
        }

        /* The Gain Table is in a '32b-only' region so must be written via Registers32bOnlyWrite() which treats data as uint32
         * and applies endianess conversion accordingly. So we layout the memory buffer differently depending on host endianess. */
#if ADRV904X_LITTLE_ENDIAN
        formattedGainTablePtr[formattedGainIndex]      = (uint8_t)(magnitude & DIG_GAIN_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 1U] = (uint8_t)((sign & DIG_GAIN_SIGN_MASK) >> DIG_GAIN_SIGN_SHIFT) |
                                                         (uint8_t)((magnitude & DIG_GAIN_MSB_MASK) >> DIG_GAIN_MSB_SHIFT) |
                                                         (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_LSB_MASK) << RXFE_GAIN_LSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 2U] = (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_MSB_MASK) >> RXFE_GAIN_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 3U] = 0U;
        formattedGainTablePtr[formattedGainIndex + 4U] = (uint8_t)(gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 5U] = (uint8_t)((gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_MSB_MASK) >> PHASE_OFFSET_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 6U] = (uint8_t)(gainTablePtr[gainIndex].extControl & EXT_CTRL_MASK);
        formattedGainTablePtr[formattedGainIndex + 7U] = 0U;
#else
        formattedGainTablePtr[formattedGainIndex + 3U] = (uint8_t)(magnitude & DIG_GAIN_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 2U] = (uint8_t)((sign & DIG_GAIN_SIGN_MASK) >> DIG_GAIN_SIGN_SHIFT) |
                                                         (uint8_t)((magnitude & DIG_GAIN_MSB_MASK) >> DIG_GAIN_MSB_SHIFT) |
                                                         (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_LSB_MASK) << RXFE_GAIN_LSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 1U] = (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_MSB_MASK) >> RXFE_GAIN_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex]      = 0U;
        formattedGainTablePtr[formattedGainIndex + 7U] = (uint8_t)(gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 6U] = (uint8_t)((gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_MSB_MASK) >> PHASE_OFFSET_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 5U] = (uint8_t)(gainTablePtr[gainIndex].extControl & EXT_CTRL_MASK);
        formattedGainTablePtr[formattedGainIndex + 4U] = 0U;
#endif
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;


    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainTableReadParamsCompute(adi_adrv904x_Device_t* const    device,
                                                                       const adi_adrv904x_RxChannels_e rxChannel,
                                                                       const uint32_t                  maxReadGainIndices,
                                                                       const uint8_t                   gainIndexOffset,
                                                                       uint16_t* const                 numGainIndicesToRead,
                                                                       uint32_t* const                 baseAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    const uint32_t NUM_BYTES_PER_RX_GAIN_INDEX = 8U;

    uint16_t maxGainIndices = 0U;
    uint32_t baseIndex = 0U;
    int32_t rxChannelIndex = 0U;
    uint8_t minGainIndex = 0U;

    const uint32_t rxGainTableBaseAddr[] = 
    { 
        ADI_ADRV904X_RX0_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX1_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX2_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX3_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX4_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX5_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX6_GAIN_TABLE_BASEADDR,
        ADI_ADRV904X_RX7_GAIN_TABLE_BASEADDR
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, numGainIndicesToRead);
    
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, baseAddr);
    
    switch (rxChannel)
    {
        case ADI_ADRV904X_RX0:
            rxChannelIndex = 0U;
            minGainIndex = device->devStateInfo.gainIndexes.rx0MinGainIndex;
            break;

        case ADI_ADRV904X_RX1:
            rxChannelIndex = 1U;
            minGainIndex = device->devStateInfo.gainIndexes.rx1MinGainIndex;
            break;

        case ADI_ADRV904X_RX2:
            rxChannelIndex = 2U;
            minGainIndex = device->devStateInfo.gainIndexes.rx2MinGainIndex;
            break;

        case ADI_ADRV904X_RX3: 
            rxChannelIndex = 3U;
            minGainIndex = device->devStateInfo.gainIndexes.rx3MinGainIndex;
            break;

        case ADI_ADRV904X_RX4: 
            rxChannelIndex = 4U;
            minGainIndex = device->devStateInfo.gainIndexes.rx4MinGainIndex;
            break;

        case ADI_ADRV904X_RX5: 
            rxChannelIndex = 5U;
            minGainIndex = device->devStateInfo.gainIndexes.rx5MinGainIndex;
            break;

        case ADI_ADRV904X_RX6: 
            rxChannelIndex = 6U;
            minGainIndex = device->devStateInfo.gainIndexes.rx6MinGainIndex;
            break;

        case ADI_ADRV904X_RX7: 
            rxChannelIndex = 7U;
            minGainIndex = device->devStateInfo.gainIndexes.rx7MinGainIndex;
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxChannel,
                                   "Invalid Rx Channel Requested for gain table read. Valid Rx channels include Rx0-Rx7.");
            return recoveryAction;
            break;
    }

    maxGainIndices = (gainIndexOffset - minGainIndex) + 1U;

    if (maxReadGainIndices >= maxGainIndices)
    {
        *numGainIndicesToRead = maxGainIndices;
    }
    else
    {
        *numGainIndicesToRead = maxReadGainIndices;
    }

    baseIndex = (gainIndexOffset - (*numGainIndicesToRead - 1U));
    *baseAddr = (uint32_t)rxGainTableBaseAddr[rxChannelIndex] + (baseIndex * NUM_BYTES_PER_RX_GAIN_INDEX);

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainTableParse(adi_adrv904x_Device_t* const    device,
                                                           adi_adrv904x_RxGainTableRow_t   gainTablePtr[],
                                                           const uint8_t                   cpuDmaDataGainTablePtr[],
                                                           const uint16_t                  numGainIndicesInTable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8U;
    const uint16_t DIG_GAIN_MSB_MASK = 0x0003U;
    const uint8_t  DIG_GAIN_MSB_SHIFT = 8U;
    const uint8_t  RXFE_GAIN_LSB_MASK = 0xF8U;
    const uint8_t  RXFE_GAIN_LSB_SHIFT = 3U;
    const uint8_t  RXFE_GAIN_MSB_MASK = 0x07U;
    const uint8_t  RXFE_GAIN_MSB_SHIFT = 5U;
    const uint8_t  PHASE_OFFSET_MSB_SHIFT = 8U;
    const uint8_t  EXT_CTRL_MASK = 0x03U;
    /*11th bit for digital gain is the sign bit*/
    const uint8_t DIG_GAIN_SIGN_MASK = 0x04U;
    const int16_t DIG_GAIN_SIGN_EXT = (int16_t)0xFC00;

    uint32_t gainIndex = 0U;
    uint32_t cpuDmaDataGainIndex = 0U;
    int16_t digGain  = 0;
    uint16_t magnitude = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gainTablePtr);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cpuDmaDataGainTablePtr);

    /*Parse Gain Table Data in ARM DMA format and store it in gain table row struct*/
    for (gainIndex = 0U; gainIndex < numGainIndicesInTable; ++gainIndex)
    {
        /*Parse Gain Table Data :
        No. of Bytes per Gain Table Entry = 8
        @cpuDmaDataGainIndex[8 x gainIndex]    : {13'b0 , rx_fe_gain[7:0], dig_gain[10:0]}
        @cpuDmaDataGainIndex[8 x gainIndex + 4]: {14'b0,  ext_control[1:0], phase_offset[15:0]}
        */
        cpuDmaDataGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);
        
        magnitude = ((uint16_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex] |
                    (((uint16_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 1] & DIG_GAIN_MSB_MASK) << DIG_GAIN_MSB_SHIFT));
        if ((cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 1] & DIG_GAIN_SIGN_MASK) == DIG_GAIN_SIGN_MASK)
        {
            /*Since digGain is a 11bit no. for ARM, 11th bit is the signed bit. However since it is of signed half word type for API use, hence has to be sign extended*/
            digGain = (int16_t)(~(uint16_t)(magnitude - 1U));
            digGain |= DIG_GAIN_SIGN_EXT;
        }
        else
        {
            digGain = (int16_t)magnitude;
        }   
        
        gainTablePtr[gainIndex].digGain = digGain;
        gainTablePtr[gainIndex].rxFeGain = (((uint8_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 1] & RXFE_GAIN_LSB_MASK) >> RXFE_GAIN_LSB_SHIFT) |
                                           (((uint8_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 2] & RXFE_GAIN_MSB_MASK) << RXFE_GAIN_MSB_SHIFT);
        gainTablePtr[gainIndex].phaseOffset = (uint16_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 4] |
                                              ((uint16_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 5] << PHASE_OFFSET_MSB_SHIFT);
        gainTablePtr[gainIndex].extControl = (uint8_t)cpuDmaDataGainTablePtr[cpuDmaDataGainIndex + 6] & EXT_CTRL_MASK;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainSetRangeCheck(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_RxGain_t     rxGain[],
                                                              const uint32_t                  arraySize)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 | ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);

    uint32_t configIndex = 0U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /*Check for null rxGain pointer*/
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxGain);

    /*Check that no. of Rx gain settings is valid*/
    if (arraySize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                arraySize,
                                "Parameter arraySize set to 0. Minimum no. of configs should be at least 1");
        return recoveryAction;
    }

    /*Range check manual gain index setting for the requested channel*/
    for (configIndex = 0U; configIndex < arraySize; ++configIndex)
    {
        
        /*Check that rxChannelMask is not out of range*/
        if ((rxGain[configIndex].rxChannelMask == 0U) || (rxGain[configIndex].rxChannelMask > ALL_RX_MASK))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                recoveryAction,
                rxGain[configIndex].rxChannelMask,
                "Invalid Rx Channel Mask encountered for manual rx gain index setting");
            return recoveryAction;
        }
        
        /*Check that Rx profile is valid*/
        if ((rxGain[configIndex].rxChannelMask & ALL_RX_MASK) > 0U)
        {
            if ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) == 0U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].rxChannelMask,
                                        "Rx gain index set requested for an Rx Channel but Rx profile is invalid in the device structure");
                return recoveryAction;
            }
        }

        /*Check that rxChannelMask is not out of range*/
        if (rxGain[configIndex].rxChannelMask > (ALL_RX_MASK))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxGain[configIndex].rxChannelMask,
                                    "Invalid Rx Channel Mask encountered for manual rx gain index setting");
            return recoveryAction;
        }

        /*Check that gain indices are within range for the channel selected*/
        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX0) == (uint32_t)ADI_ADRV904X_RX0)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx0MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx0MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx0 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX1) == (uint32_t)ADI_ADRV904X_RX1)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx1MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx1MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx1 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX2) == (uint32_t)ADI_ADRV904X_RX2)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx2MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx2MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx2 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX3) == (uint32_t)ADI_ADRV904X_RX3)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx3MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx3MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx3 manual gain index setting is out of range");
                return recoveryAction;
            }
        }
        
        
        /*Check that gain indices are within range for the channel selected*/
        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX4) == (uint32_t)ADI_ADRV904X_RX4)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx4MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx4MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx4 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX5) == (uint32_t)ADI_ADRV904X_RX5)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx5MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx5MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx5 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX6) == (uint32_t)ADI_ADRV904X_RX6)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx6MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx6MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx6 manual gain index setting is out of range");
                return recoveryAction;
            }
        }

        if ((rxGain[configIndex].rxChannelMask & (uint32_t)ADI_ADRV904X_RX7) == (uint32_t)ADI_ADRV904X_RX7)
        {
            if ((rxGain[configIndex].gainIndex > device->devStateInfo.gainIndexes.rx7MaxGainIndex) ||
                (rxGain[configIndex].gainIndex < device->devStateInfo.gainIndexes.rx7MinGainIndex))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxGain[configIndex].gainIndex,
                                        "Rx7 manual gain index setting is out of range");
                return recoveryAction;
            }
        }
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainGetRangeCheck(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              adi_adrv904x_RxGain_t* const    rxGain)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 | ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Check for null rxGain pointer*/
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxGain);
    
    /*Check that the requested rxChannel is valid*/
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for rx gain read");
        return recoveryAction;
    }

    /*Check that rx profile is valid in current config*/
    if (((uint32_t)rxChannel & ALL_RX_MASK) > 0U)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) == 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Gain index read requested for an Rx channel but Rx profile is invalid in device structure");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxBitfieldAddressGet(adi_adrv904x_Device_t* const    device,
                                                               const adi_adrv904x_RxChannels_e rxChannel,
                                                               adrv904x_BfRxChanAddr_e* const  rxChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                NULL,
                                "Channel not in device initializedChannels for adrv904x_RxBitfieldAddressGet() function");
        return recoveryAction;
    }

    switch (rxChannel)
    {
        case ADI_ADRV904X_RX0:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH0;
            break;

        case ADI_ADRV904X_RX1:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH1;
            break;

        case ADI_ADRV904X_RX2:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH2;
            break;

        case ADI_ADRV904X_RX3:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH3;
            break;

        case ADI_ADRV904X_RX4:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH4;
            break;

        case ADI_ADRV904X_RX5:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH5;
            break;

        case ADI_ADRV904X_RX6:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH6;
            break;

        case ADI_ADRV904X_RX7:
            *rxChannelBitfieldAddr = ADRV904X_BF_RX_CH7;
            break;

        default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxChannel,
                                        "Invalid rxChannel parameter for adrv904x_RxBitfieldAddressGet() function");
                return recoveryAction;
            }
    }

    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxBitfieldAddressGet(adi_adrv904x_Device_t* const       device,
                                                                const adi_adrv904x_RxChannels_e    rxChannel,
                                                                adrv904x_BfOrxDigChanAddr_e* const orxChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, orxChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                NULL,
                                "Channel not in device initializedChannels for adrv904x_OrxBitfieldAddressGet() function");
        return recoveryAction;
    }

    switch (rxChannel)
    {
        case ADI_ADRV904X_ORX0:
            *orxChannelBitfieldAddr = ADRV904X_BF_SLICE_ORX_0__ORX_DIG;
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            break;

        case ADI_ADRV904X_ORX1:
            *orxChannelBitfieldAddr = ADRV904X_BF_SLICE_ORX_1__ORX_DIG;
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            break;

        default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxChannel,
                                        "Invalid rxChannel parameter for adrv904x_RxBitfieldAddressGet() function");
                return recoveryAction;
            }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxAnalogBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                     const adi_adrv904x_RxChannels_e rxChannel,
                                                                     adrv904x_BfTdrDpathTopChanAddr_e* const rxAnalogChannelIBitfieldAddr,
                                                                     adrv904x_BfTdrDpathTopChanAddr_e* const rxAnalogChannelQBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield I channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxAnalogChannelIBitfieldAddr);
    
    /* Check rx bitfield Q channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxAnalogChannelQBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               NULL,
                               "Channel not in device initializedChannels");
        return recoveryAction;
    }

    switch (rxChannel)
    {
    case ADI_ADRV904X_RX0:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_0__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX1:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_1__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_1__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX2:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_2__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_2__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
    case ADI_ADRV904X_RX3:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_3__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_3__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX4:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_4__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_4__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX5:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_5__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_5__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX6:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_6__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_6__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    case ADI_ADRV904X_RX7:
        *rxAnalogChannelIBitfieldAddr = ADRV904X_BF_SLICE_RX_7__ADC32_ANALOG_REGS_TDR_DPATH_TOP_0_;
        *rxAnalogChannelQBitfieldAddr = ADRV904X_BF_SLICE_RX_7__ADC32_ANALOG_REGS_TDR_DPATH_TOP_1_;
        break;
            
    default:
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxChannel,
                                   "Invalid rxChannel parameter ");
            return recoveryAction;
        }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;    
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxFuncsBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_RxChannels_e rxChannel,
                                                                    adrv904x_BfRxFuncsChanAddr_e* const rxFuncsChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxFuncsChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            NULL,
            "Channel not in device initializedChannels");
        return recoveryAction;
    }

    switch (rxChannel)
    {
        case ADI_ADRV904X_RX0:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX1:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_1__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX2:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_2__RX_FUNCS;
            break;
        case ADI_ADRV904X_RX3:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_3__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX4:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_4__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX5:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_5__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX6:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_6__RX_FUNCS;
            break;
            
        case ADI_ADRV904X_RX7:
            *rxFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_7__RX_FUNCS;
            break;
            
        default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                    recoveryAction,
                    rxChannel,
                    "Invalid rxChannel parameter ");
                return recoveryAction;
            }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;    
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDdcBitfieldAddressGet(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_RxChannels_e     rxChannel,
                                                                  const adi_adrv904x_RxDdcs_e         ddcBand,
                                                                  adrv904x_BfRxDdcChanAddr_e* const   rxDdcBitfieldAddr)
{
    static const adrv904x_BfRxDdcChanAddr_e mapArr[ADI_ADRV904X_MAX_RX_ONLY][ADI_ADRV904X_MAX_RX_DDC_BANDS] =
    {
        { ADRV904X_BF_SLICE_RX_0__RX_DDC_0_, ADRV904X_BF_SLICE_RX_0__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_1__RX_DDC_0_, ADRV904X_BF_SLICE_RX_1__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_2__RX_DDC_0_, ADRV904X_BF_SLICE_RX_2__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_3__RX_DDC_0_, ADRV904X_BF_SLICE_RX_3__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_4__RX_DDC_0_, ADRV904X_BF_SLICE_RX_4__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_5__RX_DDC_0_, ADRV904X_BF_SLICE_RX_5__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_6__RX_DDC_0_, ADRV904X_BF_SLICE_RX_6__RX_DDC_1_ },
        { ADRV904X_BF_SLICE_RX_7__RX_DDC_0_, ADRV904X_BF_SLICE_RX_7__RX_DDC_1_ } 
    };

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t rxIdx = 0U;
    uint32_t bandIdx = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxDdcBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 NULL,
                                 "Channel not in device initialized Channels");
        return recoveryAction;
    }
        
    /* Get rx channel idx from enum */
    switch (rxChannel)
    {
    case ADI_ADRV904X_RX0:
        rxIdx = 0U;
        break;
    case ADI_ADRV904X_RX1:
        rxIdx = 1U;
        break;
    case ADI_ADRV904X_RX2:
        rxIdx = 2U;
        break;
    case ADI_ADRV904X_RX3:
        rxIdx = 3U;
        break;
    case ADI_ADRV904X_RX4:
        rxIdx = 4U;
        break;
    case ADI_ADRV904X_RX5:
        rxIdx = 5U;
        break;
    case ADI_ADRV904X_RX6:
        rxIdx = 6U;
        break;
    case ADI_ADRV904X_RX7:
        rxIdx = 7U;
        break;
    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannel,
                                 "Invalid rxChannel parameter");
        return recoveryAction;
    }
    
    /* Get ddc band idx from enum */
    switch (ddcBand)
    {
    case ADI_ADRV904X_RX_DDC_BAND0:
        bandIdx = 0U;
        break;
    case ADI_ADRV904X_RX_DDC_BAND1:
        bandIdx = 1U;
        break;
    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 ddcBand,
                                 "Invalid RX DDC band parameter. Must be Band0 or Band1.");
        return recoveryAction;
    }
    
    /* Get base address for this rx channel and ddc band combo */    
    *rxDdcBitfieldAddr = mapArr[rxIdx][bandIdx];

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSet(adi_adrv904x_Device_t * const       device,
                                                          const adi_adrv904x_RxDataFormatRt_t rxDataFormat[],
                                                          const uint32_t                      arraySize)
{
    static const uint8_t GAIN_COMP_EXT_LNA_DISABLE = 0U;
    static const uint8_t GAIN_COMP_TEMP_DISABLE = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t intDataResolution = 0U;
    uint8_t intDataFormat = 0U;
    uint32_t tmpMask = 0U;
    uint32_t configIdx = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxDataFormat);
    
    /* Range Check inputs */
#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxDataFormatSetRangeCheck(device, rxDataFormat, arraySize);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxDataFormat range check failure.");
        return recoveryAction;
    }
#endif
    
    /* Iterate over arraySize elements of rxDataFormat */
    for ( configIdx = 0U; configIdx < arraySize; configIdx++ )
    {
        
        switch (rxDataFormat[configIdx].rxDataFormat.formatSelect)
        {
        
        /*If Floating Point mode selected, Configure floating point*/
        case ADI_ADRV904X_GAIN_WITH_FLOATING_POINT:
            recoveryAction = adrv904x_RxDataFormatFloatingPointSet(  device,
                                                                     rxDataFormat[configIdx].rxChannelMask,
                                                                    &rxDataFormat[configIdx].rxDataFormat.floatingPointConfig);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxDataFormat floatingPointConfig");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompExtLnaSet(device,
                                                          rxDataFormat[configIdx].rxChannelMask,
                                                          rxDataFormat[configIdx].rxDataFormat.externalLnaGain);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompExtLna");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompTempEnableSet(device,
                                                              rxDataFormat[configIdx].rxChannelMask,
                                                              rxDataFormat[configIdx].rxDataFormat.tempCompensationEnable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompTempEnable");
                return recoveryAction;
            }
            break;

        /* Integer and slicer config settings */
        case ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO:
        case ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE:
        case ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE:
        case ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE:
            recoveryAction = adrv904x_RxDataFormatIntegerSet(device, 
                                                             rxDataFormat[configIdx].rxChannelMask,
                                                             rxDataFormat[configIdx].rxDataFormat.formatSelect,
                                                             &rxDataFormat[configIdx].rxDataFormat.integerConfigSettings,
                                                             &rxDataFormat[configIdx].rxDataFormat.slicerConfigSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxDataFormat integerConfigSettings and slicerConfigSettings");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompExtLnaSet(device,
                                                          rxDataFormat[configIdx].rxChannelMask,
                                                          rxDataFormat[configIdx].rxDataFormat.externalLnaGain);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompExtLna");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompTempEnableSet(device,
                                                              rxDataFormat[configIdx].rxChannelMask,
                                                              rxDataFormat[configIdx].rxDataFormat.tempCompensationEnable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompTempEnable");
                return recoveryAction;
            }
            break;

        /* Embedded overload indicator settings */
        case ADI_ADRV904X_EMBED_OVERLOAD_MONITOR_DATA:
            recoveryAction = adrv904x_RxDataFormatEmbOvldMonitorSet(device,
                                                                    rxDataFormat[configIdx].rxChannelMask,
                                                                    &rxDataFormat[configIdx].rxDataFormat.embOvldMonitorSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxDataFormat embOvldMonitorSettings");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompExtLnaSet(device,
                                                          rxDataFormat[configIdx].rxChannelMask,
                                                          rxDataFormat[configIdx].rxDataFormat.externalLnaGain);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompExtLna");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompTempEnableSet(device,
                                                              rxDataFormat[configIdx].rxChannelMask,
                                                              rxDataFormat[configIdx].rxDataFormat.tempCompensationEnable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompTempEnable");
                return recoveryAction;
            }
            break;

        /* If gain compensation disable is requested set integer formatter regs to default */
        case ADI_ADRV904X_GAIN_COMPENSATION_DISABLED:
            /* Resolve Sample Resolution and format */
            recoveryAction = adrv904x_RxIntSampleResFormatResolve(device,
                                                                  &rxDataFormat[configIdx].rxDataFormat.integerConfigSettings,
                                                                  &intDataResolution,
                                                                  &intDataFormat);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error resolving RxIntSampleResFormat");
                return recoveryAction;
            }

            /* Disable gain compensation and reset the formatter settings to default */
            recoveryAction = adrv904x_RxDataFormatGainCompDisable(device,
                                                                  rxDataFormat[configIdx].rxChannelMask,
                                                                  intDataFormat,
                                                                  intDataResolution);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error disabling RxDataFormatGainComp");
                return recoveryAction;
            }

            /* Disable Gain Comp by tmp and ext lna, but only applicable for Rx Channels */
            tmpMask = rxDataFormat[configIdx].rxChannelMask & ADI_ADRV904X_RX_MASK_ALL;
            recoveryAction = adrv904x_RxGainCompTempEnableSet(device,
                                                              tmpMask, 
                                                              GAIN_COMP_TEMP_DISABLE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompTempEnable");
                return recoveryAction;
            }

            recoveryAction = adrv904x_RxGainCompExtLnaSet(device,
                                                          tmpMask,
                                                          GAIN_COMP_EXT_LNA_DISABLE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting RxGainCompExtLna");
                return recoveryAction;
            }
            break;
            
        /* If External Slicer mode is selected, throw an error */
        /* Other modes, throw an error */
        case ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER:
        default:
            ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_RETURN(&device->common);
            break;
        }

                /* Always enable ORx0/1 6dB Gain enable in ORx Data formatter to account for 6dB loss in ORx ADC NCO */
        /* only write if ORx0/1 channel mask set */
        if((rxDataFormat[configIdx].rxChannelMask & ADI_ADRV904X_ORX0) == ADI_ADRV904X_ORX0)
        {
            recoveryAction = adrv904x_OrxDig_Orx6dbGainEnable_BfSet(device, NULL, ADRV904X_BF_SLICE_ORX_0__ORX_DIG, 1);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
                return recoveryAction;
            }
            
        }

        /* Always enable ORx0/1 6dB Gain enable in ORx Data formatter to account for 6dB loss in ORx ADC NCO */
        /* only write if ORx0/1 channel mask set */
        if ((rxDataFormat[configIdx].rxChannelMask & ADI_ADRV904X_ORX1) == ADI_ADRV904X_ORX1)
        {
            recoveryAction = adrv904x_OrxDig_Orx6dbGainEnable_BfSet(device, NULL, ADRV904X_BF_SLICE_ORX_1__ORX_DIG, 1);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
                return recoveryAction;
            }
        }
        
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSetRangeCheck(adi_adrv904x_Device_t * const       device,
                                                                    const adi_adrv904x_RxDataFormatRt_t rxDataFormat[],
                                                                    const uint32_t                      arraySize)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 | 
                                                   ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    static const uint32_t ALL_ORX_MASK = (uint32_t)(ADI_ADRV904X_ORX0 | ADI_ADRV904X_ORX1);
    static const uint32_t MINIMUM_NUM_FORMATTERS = 1U;
    static const uint32_t MAXIMUM_NUM_FORMATTERS = 10U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t configIdx = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxDataFormat);
    
    /* Verify that arraySize is > 1 and <= the maximum number of unique formatters. */
    /* If not, throw an error indicating whether the minimum or maximum was exceeded. */
    if (arraySize < MINIMUM_NUM_FORMATTERS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, arraySize, "Invalid number of Rx formatters. Less than minimum allowed");
        return recoveryAction;
    }
    if (arraySize > MAXIMUM_NUM_FORMATTERS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, arraySize, "Invalid number of Rx formatters. Exceeded than maximum allowed");
        return recoveryAction;
    }

    /* Iterate over arraySize elements of rxDataFormat */
    for ( configIdx = 0U; configIdx < arraySize; configIdx++ )
    {
        /* Verify that the rxChannelMask field is not empty. */
        if (rxDataFormat[configIdx].rxChannelMask == ADI_ADRV904X_RXOFF)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, 
                                     recoveryAction, 
                                     device->devStateInfo.profilesValid,
                                     "Empty Rx channel mask selected for Rx data format configuration");
            return recoveryAction;
        }
        
        /* If any RX channels are selected, verify that the RX profile is marked valid in the device profilesValid field. */
        if (((rxDataFormat[configIdx].rxChannelMask & ALL_RX_MASK) > 0U) &&
            ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) != ADI_ADRV904X_RX_PROFILE_VALID))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     device->devStateInfo.profilesValid,
                                     "Rx channel selected for Rx data format configuration but Rx profile is invalid");
            return recoveryAction;
        }
        
        /* If any ORX channels are selected: */ 
        if ( (rxDataFormat[configIdx].rxChannelMask & ALL_ORX_MASK) > 0U )
        {
            /* Verify that the ORX profile is marked valid in the device profilesValid field. */
            if ( (device->devStateInfo.profilesValid & ADI_ADRV904X_ORX_PROFILE_VALID) != ADI_ADRV904X_ORX_PROFILE_VALID )
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         device->devStateInfo.profilesValid,
                                         "ORx channel selected for Rx data format configuration but ORx profile is invalid");
                                   
                return recoveryAction;
            }
            
            /* Verify that the ORX profile has formatSelect = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED. */
            if (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_COMPENSATION_DISABLED)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         rxDataFormat[configIdx].rxDataFormat.formatSelect,
                                         "Invalid format selected for ORx data format configuration. ORx only supports ADI_ADRV904X_GAIN_COMPENSATION_DISABLED");
                return recoveryAction;
            }
        }
        
        /* Verify that format selected is valid */
        if ((rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_COMPENSATION_DISABLED) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_FLOATING_POINT) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER) &&
            (rxDataFormat[configIdx].rxDataFormat.formatSelect != ADI_ADRV904X_EMBED_OVERLOAD_MONITOR_DATA))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     rxDataFormat[configIdx].rxDataFormat.formatSelect,
                                     "Invalid Rx Data Format selected");
            return recoveryAction;
        }
        
        /* If selected format is ADI_ADRV904X_GAIN_COMPENSATION_DISABLED, verify that the selected integer mode is valid */
        if (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_COMPENSATION_DISABLED)
        {
            if ((rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_2SCOMP) &&
                (rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_SIGNED) &&
                (rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP) &&
                (rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution,
                                         "Incorrect Integer format and resolution selected while attempting to disable gain compensation");
                return recoveryAction;
            }
        }
        
        /* If selected format is ADI_ADRV904X_GAIN_WITH_FLOATING_POINT, range check floating point configuration */
        else if (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_FLOATING_POINT)
        {
            recoveryAction = adrv904x_RxDataFormatSetFloatingPointRangeCheck(  device, 
                                                                              &rxDataFormat[configIdx].rxDataFormat.floatingPointConfig);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in floating point mode range check");
                return recoveryAction;

            }
        }
        
        /* If selected format is one of the internal slicer modes, range check integer mode configuration */
        else if ((rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER) ||
                 (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE) ||
                 (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
                 (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
                 (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO))
        {
            recoveryAction = adrv904x_RxDataFormatSetIntegerRangeCheck(  device, 
                                                                         rxDataFormat[configIdx].rxChannelMask,
                                                                         rxDataFormat[configIdx].rxDataFormat.formatSelect,
                                                                        &rxDataFormat[configIdx].rxDataFormat.integerConfigSettings,
                                                                        &rxDataFormat[configIdx].rxDataFormat.slicerConfigSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in integer mode range check");
                return recoveryAction;

            }
        }

        /* If selected format is the embedded overload monitor, range check overload monitor format settings */
        else if (rxDataFormat[configIdx].rxDataFormat.formatSelect == ADI_ADRV904X_EMBED_OVERLOAD_MONITOR_DATA)
        {
            recoveryAction = adrv904x_RxDataFormatSetEmbOvldMonitorRangeCheck(  device, 
                                                                               &rxDataFormat[configIdx].rxDataFormat.embOvldMonitorSettings, 
                                                                                rxDataFormat[configIdx].rxDataFormat.integerConfigSettings.intSampleResolution);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in embedded overload monitor range check");
                return recoveryAction;

            }
        }
    } /* end of configIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatSetRangeCheck(adi_adrv904x_Device_t* const          device,
                                                                      const adi_adrv904x_CddcDataFormatRt_t cddcDataFormat[],
                                                                      const uint32_t                        arraySize)
{
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 |
                                                   ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    static const uint32_t MINIMUM_NUM_FORMATTERS = 1U;
    static const uint32_t MAXIMUM_NUM_FORMATTERS = 10U;
    static const uint8_t  MAXIMUM_EMB_OVERLOAD_VALUE = 15U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t configIdx = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcDataFormat);

    /* Verify that arraySize is > 1 and <= the maximum number of unique formatters. */
    /* If not, throw an error indicating whether the minimum or maximum was exceeded. */
    if (arraySize < MINIMUM_NUM_FORMATTERS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arraySize, "Invalid number of Rx formatters. Less than minimum allowed");
        return recoveryAction;
    }
    if (arraySize > MAXIMUM_NUM_FORMATTERS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arraySize, "Invalid number of Rx formatters. Exceeded than maximum allowed");
        return recoveryAction;
    }

    /* Iterate over arraySize elements of rxDataFormat */
    for (configIdx = 0U; configIdx < arraySize; configIdx++)
    {
        /* Verify that the rxChannelMask field is not empty. */
        if (cddcDataFormat[configIdx].rxChannelMask == ADI_ADRV904X_RXOFF)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "Empty Rx channel mask selected for CDDC data format configuration");
            return recoveryAction;
        }

        /* If any RX channels are selected, verify that the RX profile is marked valid in the device profilesValid field. */
        if (((cddcDataFormat[configIdx].rxChannelMask & ALL_RX_MASK) > 0U) &&
            ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) != ADI_ADRV904X_RX_PROFILE_VALID)) 
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "Rx channel selected for CDDC data format configuration but Rx profile is invalid");
            return recoveryAction;
        }
        if (cddcDataFormat[configIdx].rxChannelMask > ALL_RX_MASK)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                device->devStateInfo.profilesValid,
                "Invalid Rx channel selected for CDDC data format configuration");
            return recoveryAction;
        }

        /* Verify that format selected is valid */
        if ((cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect != ADI_ADRV904X_CDDC_DISABLED) &&
            (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect != ADI_ADRV904X_CDDC_FLOATING_POINT) &&
            (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect != ADI_ADRV904X_CDDC_INTEGER) &&
            (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect != ADI_ADRV904X_CDDC_SLICER) &&
            (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect != ADI_ADRV904X_CDDC_EMBED_OVERLOAD_MONITOR_DATA))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect,
                "Invalid CDDC Data Format selected");
            return recoveryAction;
        }

        /* If selected format is ADI_ADRV904X_GAIN_COMPENSATION_DISABLED, verify that the selected integer mode is valid */
        if (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect == ADI_ADRV904X_CDDC_DISABLED)
        {
            if ((cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_2SCOMP) &&
                (cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_SIGNED) &&
                (cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP) &&
                (cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings.intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                    recoveryAction,
                    cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings.intSampleResolution,
                    "Incorrect Integer format and resolution selected");
                return recoveryAction;
            }
        }

        /* If selected format is ADI_ADRV904X_CDDC_FLOATING_POINT, range check floating point configuration */
        else if (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect == ADI_ADRV904X_CDDC_FLOATING_POINT)
        {
            recoveryAction = adrv904x_CddcDataFormatSetFloatingPointRangeCheck(device,
                &cddcDataFormat[configIdx].cddcDataFormat.cddcFloatingPointConfig);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in CDDC floating point mode range check");
                return recoveryAction;
            }
        }

        /* If selected format is the embedded overload monitor, range check overload monitor format settings */
        else if (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect == ADI_ADRV904X_CDDC_EMBED_OVERLOAD_MONITOR_DATA)
        {
            if ((cddcDataFormat[configIdx].cddcDataFormat.cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbI > MAXIMUM_EMB_OVERLOAD_VALUE) ||
                (cddcDataFormat[configIdx].cddcDataFormat.cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbQ > MAXIMUM_EMB_OVERLOAD_VALUE) ||
                (cddcDataFormat[configIdx].cddcDataFormat.cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI > MAXIMUM_EMB_OVERLOAD_VALUE) ||
                (cddcDataFormat[configIdx].cddcDataFormat.cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ > MAXIMUM_EMB_OVERLOAD_VALUE))
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error in CDDC overload monitor range check");
                return recoveryAction;
            }
        }
    } /* end of configIdx for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSetFloatingPointRangeCheck(adi_adrv904x_Device_t * const                            device,
                                                                                 const adi_adrv904x_FloatingPointConfigSettings_t * const floatingPointConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, floatingPointConfig);
    
    /* Check floating point format selected is valid */
    if ((floatingPointConfig->fpDataFormat != ADI_ADRV904X_FP_FORMAT_SIGN_SIGNIFICAND_EXP) &&
        (floatingPointConfig->fpDataFormat != ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpDataFormat,
                                 "Invalid floating point data format selected");
        return recoveryAction;
    }

    /* Check floating point round mode is valid */
    if ((floatingPointConfig->fpRoundMode != ADI_ADRV904X_ROUND_TO_EVEN) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_POSITIVE) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_NEGATIVE) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_ZERO) &&
        (floatingPointConfig->fpRoundMode != ADI_ADRV904X_ROUND_FROM_EVEN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpRoundMode,
                                 "Invalid floating point round mode selected");
        return recoveryAction;
    }

    /* Check floating point no. of exponent bits is valid */
    if ((floatingPointConfig->fpNumExpBits != ADI_ADRV904X_2_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV904X_3_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV904X_4_EXPONENTBITS) &&
        (floatingPointConfig->fpNumExpBits != ADI_ADRV904X_5_EXPONENTBITS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpNumExpBits,
                                 "Invalid number of floating point exponent bits selected");
        return recoveryAction;
    }

    /* Check floating point attenuation steps are valid */
    if ((floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_24DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_18DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_12DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_6DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_0DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_MINUS6DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_MINUS12DB) &&
        (floatingPointConfig->fpAttenSteps != ADI_ADRV904X_FPATTEN_MINUS18DB))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpAttenSteps,
                                 "Invalid floating point attenuation step selected");
        return recoveryAction;
    }

    /* Check floating point encoded NaN selection is valid */
    if ((floatingPointConfig->fpEncodeNan != ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE) &&
        (floatingPointConfig->fpEncodeNan != ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpEncodeNan,
                                 "Invalid floating point encode NaN selection");
        return recoveryAction;
    }

    /*Check floating point encoded NaN selection is valid*/
    if ((floatingPointConfig->fpHideLeadingOne != ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE) &&
        (floatingPointConfig->fpHideLeadingOne != ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 floatingPointConfig->fpHideLeadingOne,
                                 "Invalid floating point hide leading one selection");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatSetFloatingPointRangeCheck(adi_adrv904x_Device_t * const                                device,
                                                                                   const adi_adrv904x_CddcFloatingPointConfigSettings_t * const cddcFloatingPointConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcFloatingPointConfig);
    
    /* Check floating point format selected is valid */
    if ((cddcFloatingPointConfig->cddcFpDataFormat != ADI_ADRV904X_FP_FORMAT_SIGN_SIGNIFICAND_EXP) &&
        (cddcFloatingPointConfig->cddcFpDataFormat != ADI_ADRV904X_FP_FORMAT_SIGN_EXP_SIGNIFICAND))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 cddcFloatingPointConfig->cddcFpDataFormat,
                                 "Invalid floating point data format selected");
        return recoveryAction;
    }

    /* Check floating point round mode is valid */
    if ((cddcFloatingPointConfig->cddcFpRoundMode != ADI_ADRV904X_ROUND_TO_EVEN) &&
        (cddcFloatingPointConfig->cddcFpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_POSITIVE) &&
        (cddcFloatingPointConfig->cddcFpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_NEGATIVE) &&
        (cddcFloatingPointConfig->cddcFpRoundMode != ADI_ADRV904X_ROUNDTOWARDS_ZERO) &&
        (cddcFloatingPointConfig->cddcFpRoundMode != ADI_ADRV904X_ROUND_FROM_EVEN))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 cddcFloatingPointConfig->cddcFpRoundMode,
                                 "Invalid floating point round mode selected");
        return recoveryAction;
    }

    /* Check floating point no. of exponent bits is valid */
    if ((cddcFloatingPointConfig->cddcFpNumExpBits != ADI_ADRV904X_2_EXPONENTBITS) &&
        (cddcFloatingPointConfig->cddcFpNumExpBits != ADI_ADRV904X_3_EXPONENTBITS) &&
        (cddcFloatingPointConfig->cddcFpNumExpBits != ADI_ADRV904X_4_EXPONENTBITS) &&
        (cddcFloatingPointConfig->cddcFpNumExpBits != ADI_ADRV904X_5_EXPONENTBITS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 cddcFloatingPointConfig->cddcFpNumExpBits,
                                 "Invalid number of floating point exponent bits selected");
        return recoveryAction;
    }

    /* Check floating point encoded NaN selection is valid */
    if ((cddcFloatingPointConfig->cddcFpEncodeNan != ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_DISABLE) &&
        (cddcFloatingPointConfig->cddcFpEncodeNan != ADI_ADRV904X_FP_FORMAT_NAN_ENCODE_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 cddcFloatingPointConfig->cddcFpEncodeNan,
                                 "Invalid floating point encode NaN selection");
        return recoveryAction;
    }

    /*Check floating point encoded NaN selection is valid*/
    if ((cddcFloatingPointConfig->cddcFpHideLeadingOne != ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_DISABLE) &&
        (cddcFloatingPointConfig->cddcFpHideLeadingOne != ADI_ADRV904X_FP_FORMAT_HIDE_LEADING_ONE_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 cddcFloatingPointConfig->cddcFpHideLeadingOne,
                                 "Invalid floating point hide leading one selection");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSetIntegerRangeCheck(adi_adrv904x_Device_t * const                      device,
                                                                           const uint32_t                                     rxChannelMask,
                                                                           const adi_adrv904x_RxDataFormatModes_e             formatSelect,
                                                                           const adi_adrv904x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                           const adi_adrv904x_SlicerConfigSettings_t * const  slicerConfigSettings)
{
    /*
    static const uint32_t ALL_RX_MASK = (uint32_t)(ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 | 
                                                   ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7);
    */
    static const uint32_t ALL_ORX_MASK = (uint32_t)(ADI_ADRV904X_ORX0 | ADI_ADRV904X_ORX1);
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t slicerBit = 0U;
    uint8_t numRxChannels = 0U;
    uint8_t i = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, slicerConfigSettings);
    
    /* If the selected format is one of the internal slicer modes:
        a.    Verify integer mode configuration against the constraints in Table 5.2.4 2, otherwise throw an error. 
            Note: Only ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode is valid if any ORx channels are selected.
        b.    If the selected format is ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2-3 or 4PIN_MODE,
            verify that the specified GPIO mode for each RX channel is not configured for use by another feature. Otherwise throw an error.
    */
    /* If the selected format is one of the internal slicer modes: */
    if( (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE) || 
        (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) || 
        (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
        (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO))
    {
        /* Verify integer mode configuration against constraints */
        /* Note: Only ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode is valid if any ORx channels are selected. */
         /* Check that selected integer formatter internal slicer embedded bits mode is valid */
        if ((integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER) &&
            (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     integerConfigSettings->intEmbeddedBits,
                                     "Invalid integer formatter internal slicer embedded bits selected");
            return recoveryAction;
        }

        /* Check that integer format resolution is valid */
        if ((integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_2SCOMP) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     integerConfigSettings->intSampleResolution,
                                     "Invalid integer formatter sample resolution selected");
            return recoveryAction;
        }

        /* Embedding 1 bit slicer at MSB only supports 12 bit/16 bit signed integer sample resolution */
        if ((integerConfigSettings->intEmbeddedBits     == ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_12BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     integerConfigSettings->intSampleResolution,
                                     "Invalid integer formatter sample resolution. Valid sample resolution is 12-bit signed for embedding 1 slicer bit at MSB");
            return recoveryAction;
        }

        /* Embedding 2 bit slicer at MSB for a 3-bit slicer mode only supports Signed representation in 16 bit resolution*/
        if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
        {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         integerConfigSettings->intSampleResolution,
                                         "Invalid integer formatter sample resolution. Valid sample resolution is 16 signed for embedding 2 slicer bits at MSB for a 3-bit slicer mode");
                return recoveryAction;
        }

        /* Embedding 2 bit slicer at MSB for a 4-bit slicer mode only supports Signed representation in 16 bit resolution */
        if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED))
        {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         integerConfigSettings->intSampleResolution,
                                         "Invalid integer formatter sample resolution. Valid sample resolution is 16 signed for embedding 2 slicer bits at MSB for a 4-bit slicer mode");
                return recoveryAction;
        }

        /* Embedding 2 bit slicer at LSB for a 3-bit slicer mode only supports 16 bit resolution, either signed or twos comp representation */
        if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP))
        {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         integerConfigSettings->intSampleResolution,
                                         "Invalid integer formatter sample resolution. Valid sample resolution is 16 signed or twos comp for embedding 2 slicer bits at LSB for a 3-bit slicer mode");
                return recoveryAction;
        }

        /* Embedding 2 bit slicer at LSB for a 4-bit slicer mode only supports 16 bit resolution, either signed or twos comp representation */
        if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_SIGNED) &&
            (integerConfigSettings->intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP))
        {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         integerConfigSettings->intSampleResolution,
                                         "Invalid integer formatter sample resolution. Valid sample resolution is 16 signed or twos comp for embedding 2 slicer bits at LSB for a 4-bit slicer mode");
                return recoveryAction;
        }

        /* Check that integer format parity is valid */
        if ((integerConfigSettings->intParity != ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY) &&
            (integerConfigSettings->intParity != ADI_ADRV904X_3BIT_SLICER_ODD_PARITY) &&
            (integerConfigSettings->intParity != ADI_ADRV904X_NO_PARITY))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     integerConfigSettings->intParity,
                                     "Invalid integer formatter parity option selected");
            return recoveryAction;
        }

        /* Check that the use case is valid for embedding parity bit */
        if ((integerConfigSettings->intParity == ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY) || 
            (integerConfigSettings->intParity == ADI_ADRV904X_3BIT_SLICER_ODD_PARITY))
        {
            /* Parity is valid only in 3 bit internal embedded slicer mode */
            if (formatSelect != ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         formatSelect,
                                         "Invalid integer formatter mode selected for parity support. Parity is supported only in 3 bit internal slicer modes");
                return recoveryAction;
            }

            /* Parity is valid only in 3 bit slicer mode */
            if ((integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) &&
                (integerConfigSettings->intEmbeddedBits != ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         formatSelect,
                                         "Invalid integer formatter mode selected for parity support. Parity is supported only in 3 bit internal slicer modes");
                return recoveryAction;
            }

            /* Parity is valid only in 3 bit embedded slicer mode. 12 bit resolution does not support 3 bit parity */
            if ((integerConfigSettings->intSampleResolution == ADI_ADRV904X_INTEGER_12BIT_2SCOMP) ||
                (integerConfigSettings->intSampleResolution == ADI_ADRV904X_INTEGER_12BIT_SIGNED))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         integerConfigSettings->intSampleResolution,
                                         "Invalid integer formatter sampling resolution mode for parity config. Parity is supported only for 16 resolution");
                return recoveryAction;
            }
        }

        /* Check that integer slicer embedded position is valid */
        if (integerConfigSettings->intEmbeddedPos != ADI_ADRV904X_LOWER_NIBBLE_ON_Q &&
            integerConfigSettings->intEmbeddedPos != ADI_ADRV904X_LOWER_NIBBLE_ON_I)
        {
            
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   slicerConfigSettings->intSlicerStepSize,
                                   "Invalid internal slicer embedded position selected");
            return recoveryAction;
        }

        /* Check that integer format step size is valid */
        if ((slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_1DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_2DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_3DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_4DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_6DB) &&
            (slicerConfigSettings->intSlicerStepSize != ADI_ADRV904X_INTSLICER_STEPSIZE_8DB))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     slicerConfigSettings->intSlicerStepSize,
                                     "Invalid internal slicer step size selected for integer format internal slicer mode");
            return recoveryAction;
        }

        /*Check the source ctrl on Rx Slicer Posn observation GPIOs for 4 Pin mode */
        if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE)
        {
            if ((rxChannelMask & ALL_ORX_MASK) > 0U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         rxChannelMask,
                                         "ORx channel is only supported for ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO mode");
                return recoveryAction;
            }

            /* Confirm Rx channel mask is only trying to set GPIOs for a single Rx channel */
            numRxChannels = 0U;
            for (i = 0U; i < 8U; i++)
            {
                numRxChannels += ((rxChannelMask >> i) & 0x01);
            }
            
            if (numRxChannels != 1U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                    recoveryAction,
                    numRxChannels,
                    "Only 1 Rx data formatter channel can be configured at a time when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE");
                return recoveryAction;
            }
            
            /* If any Rx channels in channel mask */
            for (slicerBit = 0U; slicerBit < 4U; slicerBit++)
            {
                if (slicerConfigSettings->intSlicerGpioSelect[slicerBit] >= ADI_ADRV904X_GPIO_INVALID)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                                recoveryAction,
                                                slicerConfigSettings->intSlicerGpioSelect[slicerBit],
                                                "GPIO must be valid for Internal Slicer output bits[3:0] when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE");
                    return recoveryAction;
                }
            }
        }
        
        /*Check the source ctrl on Rx Slicer Posn observation GPIOs for 3 Pin mode*/
        if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE)
        {
            /* Confirm Rx channel mask is only trying to set GPIOs for a single Rx channel */
            numRxChannels = 0U;
            for (i = 0U; i < 8U; i++)
            {
                numRxChannels += ((rxChannelMask >> i) & 0x01);
            }
            
            if (numRxChannels != 1U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                    recoveryAction,
                    numRxChannels,
                    "Only 1 Rx data formatter channel can be configured at a time when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE");
                return recoveryAction;
            }
            
            for (slicerBit = 0U; slicerBit < 3U; slicerBit++)
            {
                if (slicerConfigSettings->intSlicerGpioSelect[slicerBit] >= ADI_ADRV904X_GPIO_INVALID)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                                recoveryAction,
                                                slicerConfigSettings->intSlicerGpioSelect[slicerBit],
                                                "GPIO must be valid for Internal Slicer output bits[2:0] when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE");
                    return recoveryAction;
                }
            }
            
        }
        
        /*Check the source ctrl on Rx Slicer Posn observation GPIOs for 2 Pin mode*/
        if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE)
        {
            /* Confirm Rx channel mask is only trying to set GPIOs for a single Rx channel */
            numRxChannels = 0U;
            for (i = 0U; i < 8U; i++)
            {
                numRxChannels += ((rxChannelMask >> i) & 0x01);
            }
            
            if (numRxChannels != 1U)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       numRxChannels,
                                       "Only 1 Rx data formatter channel can be configured at a time when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE");
                return recoveryAction;
            }
            
            for (slicerBit = 0U; slicerBit < 2U; slicerBit++)
            {
                if (slicerConfigSettings->intSlicerGpioSelect[slicerBit] >= ADI_ADRV904X_GPIO_INVALID)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                           recoveryAction,
                                           slicerConfigSettings->intSlicerGpioSelect[slicerBit],
                                           "GPIO must be valid for Internal Slicer output bits[1:0] when formatSelect = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE");
                    return recoveryAction;
                }
            }
            
        }

    }
    
    /* If the selected format is ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER: */
    /* Throw an error indicating that the mode is not supported          */
    else if (formatSelect == ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER)
    {
        ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_RETURN(&device->common);
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSetEmbOvldMonitorRangeCheck(adi_adrv904x_Device_t * const                                 device,
                                                                                  const adi_adrv904x_EmbOverloadMonitorConfigSettings_t * const embOvldConfigSettings,
                                                                                  const adi_adrv904x_RxIntSampleResolution_e                    intSampleResolution)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, embOvldConfigSettings);
    
    /* The embedded overload monitor is only supported in 16bit / twos-complement format */
    if (intSampleResolution != ADI_ADRV904X_INTEGER_16BIT_2SCOMP)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 intSampleResolution,
                                 "Embedded overload monitoring Rx data format is only supported with integer 16Bit - 2s Complement format");
        return recoveryAction;
    }
    
    /* Verify that embeddedMonitorSrcLsbI field is a valid enum */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbI != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 embOvldConfigSettings->embeddedMonitorSrcLsbI,
                                 "Invalid embedded monitor source selected to be embedded at LSB posn - I sample");
        return recoveryAction;
    }

    /* Verify that embeddedMonitorSrcLsbPlusOneI field is a valid enum */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI,
                                 "Invalid embedded monitor source selected to be embedded at LSB + 1 posn - I sample");
        return recoveryAction;
    }

    /* Verify that embeddedMonitorSrcLsbQ field is a valid enum */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 embOvldConfigSettings->embeddedMonitorSrcLsbQ,
                                 "Invalid embedded monitor source selected to be embedded at LSB posn - Q sample");
        return recoveryAction;
    }

    /* Verify that embeddedMonitorSrcLsbPlusOneQ field is a valid enum */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) &&
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ != ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ,
                                 "Invalid embedded monitor source selected to be embedded at LSB + 1 posn - Q sample");
        return recoveryAction;
    }
    
    /* If any embedded monitor bit is set to ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH, verify embeddedMonitorApdHighSrcSel field */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorApdHighSrcSel != ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorApdHighSrcSel != ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_CNTR_EXCEEDED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     embOvldConfigSettings->embeddedMonitorApdHighSrcSel,
                                     "Analog Peak Detector high thresh selected for embedded monitoring but APD high source is invalid");
            return recoveryAction;
        }
    }

    /* If any embedded monitor bit is set to ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH, verify embeddedMonitorApdLowSrcSel field */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorApdLowSrcSel != ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorApdLowSrcSel != ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_CNTR_EXCEEDED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     embOvldConfigSettings->embeddedMonitorApdLowSrcSel,
                                     "Analog Peak Detector low thresh selected for embedded monitoring but APD low source is invalid");
            return recoveryAction;
        }
    }

    /* If any embedded monitor bit is set to ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH, verify embeddedMonitorHb2HighSrcSel field */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorHb2HighSrcSel != ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH) &&
            (embOvldConfigSettings->embeddedMonitorHb2HighSrcSel != ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH_CNTR_EXCEEDED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     embOvldConfigSettings->embeddedMonitorHb2HighSrcSel,
                                     "Digital HB2 Peak Detector high thresh selected for embedded monitoring but HB2 high source is invalid");
            return recoveryAction;
        }
    }

    /* If any embedded monitor bit is set to ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH, verify embeddedMonitorHb2LowSrcSel field */
    if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
        (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
    {
        if ((embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT0) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT1) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_THRESH_CNTR_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT0_THRESH_CNTR_EXCEEDED) &&
            (embOvldConfigSettings->embeddedMonitorHb2LowSrcSel != ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW_INT1_THRESH_CNTR_EXCEEDED))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     embOvldConfigSettings->embeddedMonitorHb2LowSrcSel,
                                     "Digital HB2 Peak Detector low thresh selected for embedded monitoring but HB2 low source is invalid");
            return recoveryAction;
        }
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatGainCompDisable(adi_adrv904x_Device_t * const device,
                                                                      const uint32_t                rxChannelMask,
                                                                      const uint8_t                 intDataFormat,
                                                                      const uint8_t                 intDataResolution)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[]   = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                  ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE                 = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0U]);
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[]      = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE                = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    static const adi_adrv904x_RxChannels_e ORX_CHANNEL_ARR[]  = { ADI_ADRV904X_ORX0, ADI_ADRV904X_ORX1 };
    static const uint32_t ORX_CHANNEL_ARR_SIZE                = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0U]);
    static const uint8_t DEFAULT_INT_EMBED_SLICER             = 0U;
    static const uint8_t DEFAULT_INT_EMBED_SLICER_POS         = 0U;
    static const uint8_t DEFAULT_INT_EMBED_SLICER_NUM         = 0U;
    static const uint8_t DEFAULT_INT_PARITY_SUPPORT           = 0U;
    static const uint8_t DEFAULT_INT_EVEN_PARITY              = 0U;
    static const uint8_t DISABLE_GAIN_COMPENSATION            = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction                     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t gainCompEnStatus                                  = 0U;
    
    adrv904x_BfRxDdcChanAddr_e baseAddrArr[ADI_ADRV904X_MAX_RX_DDC_BANDS] = { ADRV904X_BF_SLICE_RX_0__RX_DDC_0_, 
                                                                              ADRV904X_BF_SLICE_RX_0__RX_DDC_1_ };
    adrv904x_BfOrxDigChanAddr_e orxDigBaseAddr = (adrv904x_BfOrxDigChanAddr_e) 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Iterate through channel mask and if set disable gain compensation on the corresponding channel */
    for (uint32_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            /*If an Rx channel mask is set, resolve the corresponding Rx DDC Bitfield Addresses (2 DDCs per channel)*/
            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                    RX_CHANNEL_ARR[chanIdx],
                                                                    RX_DDC_BAND_ARR[bandIdx],
                                                                   &baseAddrArr[bandIdx]);                
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }
            } /* end bandIdx for loop */

            /*Please note that even though gain compensation is disabled,
            the embedded slicer bits have to be explicitly set to 0 failing
            which the received data will contain slicer bits if it was previously
            enabled */
            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                recoveryAction = adrv904x_RxDdc_IntDataFormat_BfSet(device,
                                                                    NULL,
                                                                    baseAddrArr[bandIdx],
                                                                    intDataFormat);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(  &device->common, recoveryAction, "IntDataFormat Write issue");
                    return recoveryAction;
                }
                
                
                recoveryAction = adrv904x_RxDdc_IntDataResolution_BfSet(device,
                                                                        NULL,
                                                                        baseAddrArr[bandIdx],
                                                                        intDataResolution);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataResolution Write issue");
                    return recoveryAction;
                }
                
                for (uint8_t embMonBitIdx = 0U; embMonBitIdx < 2U; embMonBitIdx++)
                {
                
                    /* Disable I Sample LSB Posn Embedded overload indicator config */
                    recoveryAction = adrv904x_RxDdc_RxMonFormatI_BfSet(device,
                                                                       NULL,
                                                                       baseAddrArr[bandIdx],
                                                                       embMonBitIdx,
                                                                       (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatI_0 Write issue");
                        return recoveryAction;
                    }
                
                    /* Disable Q Sample LSB + 1 Posn Embedded overload indicator config */
                    recoveryAction = adrv904x_RxDdc_RxMonFormatQ_BfSet(device,
                                                                       NULL,
                                                                       baseAddrArr[bandIdx],
                                                                       embMonBitIdx,
                                                                       (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatQ_1 Write issue");
                        return recoveryAction;
                    }
                } /* end embMonBitIdx for loop */
                
                /* Disable any Floating Point Modes */
                recoveryAction = adrv904x_RxDdc_FpEn_BfSet( device,
                                                            NULL,
                                                            baseAddrArr[bandIdx],
                                                            ADI_DISABLE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "FpEn Write issue");
                    return recoveryAction;
                }
                
            } /* end bandIdx for loop */
            
            /* Check if gain comp is enabled. Read Band0 instance only, assuming all bands are set equivalently */
            recoveryAction = adrv904x_RxDdc_GainCompEnable_BfGet(device,
                                                                 NULL,
                                                                 baseAddrArr[0],
                                                                 &gainCompEnStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompEnable Readback issue");
                return recoveryAction;
            }
            gainCompEnStatus = gainCompEnStatus ? 1U : 0U;
            
            
            if (gainCompEnStatus)
            {
                for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
                {
                    /*Disable Ext Slicer Pin Ctrl*/
                    recoveryAction = adrv904x_RxDdc_SlicerPinControlMode_BfSet(device,
                                                                               NULL,
                                                                               baseAddrArr[bandIdx],
                                                                               0U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SlicerPinControlMode Write issue");
                        return recoveryAction;
                    }
                
                    /* Set Embedded Slicer format configs to default */
                    recoveryAction = adrv904x_RxDdc_IntEmbedSlicer_BfSet(device,
                                                                         NULL,
                                                                         baseAddrArr[bandIdx],
                                                                         DEFAULT_INT_EMBED_SLICER);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicer Write issue");
                        return recoveryAction;
                    }
                    
                    recoveryAction = adrv904x_RxDdc_IntEmbedSlicerPos_BfSet(device,
                                                                            NULL,
                                                                            baseAddrArr[bandIdx],
                                                                            DEFAULT_INT_EMBED_SLICER_POS);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerPos Write issue");
                        return recoveryAction;
                    }
                    
                    recoveryAction = adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet(device,
                                                                               NULL,
                                                                               baseAddrArr[bandIdx],
                                                                               DEFAULT_INT_EMBED_SLICER_NUM);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerNumber Write issue");
                        return recoveryAction;
                    }
                    
                    recoveryAction = adrv904x_RxDdc_IntParitySupport_BfSet(device,
                                                                           NULL,
                                                                           baseAddrArr[bandIdx],
                                                                           DEFAULT_INT_PARITY_SUPPORT);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntParitySupport Write issue");
                        return recoveryAction;
                    }
                    recoveryAction = adrv904x_RxDdc_IntEvenParity_BfSet(device,
                                                                        NULL,
                                                                        baseAddrArr[bandIdx],
                                                                        DEFAULT_INT_EVEN_PARITY);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEvenParity Write issue");
                        return recoveryAction;
                    }
                    
                    /*Disable Gain Compensation*/
                    recoveryAction = adrv904x_RxDdc_GainCompEnable_BfSet(device,
                                                                         NULL,
                                                                         baseAddrArr[bandIdx],
                                                                         DISABLE_GAIN_COMPENSATION);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompEnable Write issue");
                        return recoveryAction;
                    }
                } /* end bandIdx for loop */
            } /* end if gainCompEnStatus */
            
            /* Disable any currently routed GPIOs for this Rx Channel's Internal Slicer */
            recoveryAction = adrv904x_RxInternalSlicerGpioDisable(device, RX_CHANNEL_ARR[chanIdx]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error disabling Rx Internal Slicer GPIOs.");
                return recoveryAction;
            }
            
        } /* end if Rx chanMask set */
    } /* end Rx chanIdx for loop */
    
    /* ORx has no gain compensation, slicer, or embedded monitor data options. */
    /* Only basic int format/resolution options exist. */
    for (uint32_t chanIdx = 0U; chanIdx < ORX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)ORX_CHANNEL_ARR[chanIdx]) == (uint32_t)ORX_CHANNEL_ARR[chanIdx])
        {
            /* Get ORxDig base address for this channel */
            recoveryAction = adrv904x_OrxBitfieldAddressGet(  device,
                                                              ORX_CHANNEL_ARR[chanIdx],
                                                             &orxDigBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve ORx Dig base address");
                return recoveryAction;
            }
            
            /* Set Int Data Format */
            recoveryAction =  adrv904x_OrxDig_IntDataFormat_BfSet(device,
                                                                  NULL,
                                                                  orxDigBaseAddr,
                                                                  intDataFormat);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntDataFormat Write issue");
                return recoveryAction;
            }
            
            /* Set Int Data Resolution */
            recoveryAction =  adrv904x_OrxDig_IntDataResolution_BfSet(device,
                                                                      NULL,
                                                                      orxDigBaseAddr,
                                                                      intDataResolution);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "intDataResolution Write issue");
                return recoveryAction;
            }
            
        } /* end if ORx chanMask set */
    } /* end ORx chanIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainCompExtLnaSet(adi_adrv904x_Device_t * const device,
                                                              const uint32_t                rxChannelMask,
                                                              const uint8_t                 externalLnaGain)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[]    = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t tmpEnable                     = externalLnaGain ? ADI_ENABLE : ADI_DISABLE;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( (rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }
    
    /* Iterate through channel mask and if set write the ext gain bitfield for both DDCs int he given Rx Channel*/
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            
            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                /* Get Rx DDC Bitfield Address */
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                    RX_CHANNEL_ARR[chanIdx],
                                                                    RX_DDC_BAND_ARR[bandIdx],
                                                                   &baseAddr);                    
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }
                
                /* Set bitfield */
                recoveryAction = adrv904x_RxDdc_GainCompForExtGain_BfSet(device,
                                                                         NULL,
                                                                         baseAddr,
                                                                         tmpEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompForExtGain Write issue");
                    return recoveryAction;
                }
                
            } /* end bandIdx for loop */
            
        } /* end rxChannelMask set */
        
    } /* end chIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}
                                                             
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainCompTempEnableSet(adi_adrv904x_Device_t * const device,
                                                                  const uint32_t                rxChannelMask,
                                                                  const uint8_t                 tempCompensationEnable)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[]    = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t tmpEnable                     = tempCompensationEnable ? ADI_ENABLE : ADI_DISABLE;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }
    
    /* Iterate through channel mask and if set write the ext gain bitfield for both DDCs int he given Rx Channel*/
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ( (rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            
            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                /* Get Rx DDC Bitfield Address */
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                    RX_CHANNEL_ARR[chanIdx],
                                                                    RX_DDC_BAND_ARR[bandIdx],
                                                                   &baseAddr);                
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }
                
                /* Set bitfield */
                recoveryAction = adrv904x_RxDdc_GainCompForTempGain_BfSet(device,
                                                                          NULL,
                                                                          baseAddr,
                                                                          tmpEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompForTempGain Write issue");
                    return recoveryAction;
                }
                
            } /* end bandIdx for loop */
            
        } /* end rxChannelMask set */
        
    } /* end chIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatFloatingPointSet(adi_adrv904x_Device_t * const                            device,
                                                                       const uint32_t                                           rxChannelMask,
                                                                       const adi_adrv904x_FloatingPointConfigSettings_t * const floatingPointConfig)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[]    = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    static const uint8_t DEFAULT_INT_DATA_FORMAT            = 0U;
    static const uint8_t DEFAULT_INT_DATA_RESOLUTION        = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, floatingPointConfig);
    
    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }
    
    /* Disable Gain Compensation */
    recoveryAction = adrv904x_RxDataFormatGainCompDisable( device,
                                                           rxChannelMask,
                                                           DEFAULT_INT_DATA_FORMAT,
                                                           DEFAULT_INT_DATA_RESOLUTION);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(  &device->common, recoveryAction, "Error while attempting to Disable Gain Compensation");
        return recoveryAction;
    }
    
    
    /* Iterate through channel mask and if set configure the corresponding Rx floating point formatter */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            
            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                /* Get Rx DDC Bitfield Address */
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, 
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  RX_DDC_BAND_ARR[bandIdx],
                                                                  &baseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }
                
                /* Set Floating Point Configs */
                recoveryAction = adrv904x_RxDdc_FpRoundMode_BfSet(device,
                                                                  NULL,
                                                                  baseAddr,
                                                                  (adrv904x_Bf_RxDdc_FpRoundMode_e)floatingPointConfig->fpRoundMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpRoundMode Write issue");
                    return recoveryAction;
                }
                
                recoveryAction = adrv904x_RxDdc_FpFloatDataFormat_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        floatingPointConfig->fpDataFormat);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpFloatDataFormat Write issue");
                    return recoveryAction;
                }
                
                recoveryAction = adrv904x_RxDdc_FpNanEncEn_BfSet(device,
                                                                 NULL,
                                                                 baseAddr,
                                                                 floatingPointConfig->fpEncodeNan);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpNanEncEn Write issue");
                    return recoveryAction;
                }
                
                recoveryAction = adrv904x_RxDdc_FpExponentBits_BfSet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     (adrv904x_Bf_RxDdc_FpExponentBits_e)floatingPointConfig->fpNumExpBits);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpExponentBits Write issue");
                    return recoveryAction;
                }
                
                recoveryAction = adrv904x_RxDdc_FpHideLeadingOnes_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        floatingPointConfig->fpHideLeadingOne);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpHideLeadingOnes Write issue");
                    return recoveryAction;
                }
                
                recoveryAction = adrv904x_RxDdc_FpIntDataAtten_BfSet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     floatingPointConfig->fpAttenSteps);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpIntDataAtten Write issue");
                    return recoveryAction;
                }
                
                
                /* Enable Floating Point Format on the requested channel */
                recoveryAction = adrv904x_RxDdc_FpEn_BfSet(device,
                                                           NULL,
                                                           baseAddr,
                                                           ADI_ENABLE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpEn Write issue");
                    return recoveryAction;
                }
                
                /* Enable Gain Compensation */
                recoveryAction = adrv904x_RxDdc_GainCompEnable_BfSet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     ADI_ENABLE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "GainCompEnable Write issue");
                    return recoveryAction;
                }
            } /* end of bandIdx for loop */
        } /* end of rxChannel mask if block */
    } /* end of RxChannel for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatFloatingPointSet(adi_adrv904x_Device_t * const                            device,
                                                                         const uint32_t                                           rxChannelMask,
                                                                         const adi_adrv904x_CddcFloatingPointConfigSettings_t * const floatingPointConfig)
{

    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, floatingPointConfig);

    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }

    /* Iterate through channel mask and if set configure the corresponding Rx floating point formatter */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {

            /* Get CDDC Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                return recoveryAction;
            }

            /* Set Floating Point Configs */
            recoveryAction = adrv904x_CddcFuncs_FormatterFpRoundMode_BfSet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                          (adrv904x_Bf_RxDdc_FpRoundMode_e)floatingPointConfig->cddcFpRoundMode);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpRoundMode Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfSet(device,
                                                                                 NULL,
                                                                                 baseAddr,
                                                                                 floatingPointConfig->cddcFpDataFormat);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpFloatDataFormat Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterFpNanEncEn_BfSet(device,
                                                                          NULL,
                                                                          baseAddr,
                                                                          floatingPointConfig->cddcFpEncodeNan);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpNanEncEn Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterFpExponentBits_BfSet(device,
                                                                              NULL,
                                                                              baseAddr,
                                                                              (adrv904x_Bf_RxDdc_FpExponentBits_e)floatingPointConfig->cddcFpNumExpBits);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpExponentBits Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfSet(device,
                                                                                 NULL,
                                                                                 baseAddr,
                                                                                 floatingPointConfig->cddcFpHideLeadingOne);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpHideLeadingOnes Write issue");
                return recoveryAction;
            }

            /* Enable Floating Point Format on the requested channel */
            recoveryAction = adrv904x_CddcFuncs_FormatterFpEn_BfSet(device,
                                                                    NULL,
                                                                    baseAddr,
                                                                    floatingPointConfig->cddcFpEn);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpEn Write issue");
                return recoveryAction;
            }

        } /* end of rxChannel mask if block */
    } /* end of RxChannel for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatIntegerSet(adi_adrv904x_Device_t* const                        device,
                                                                 const uint32_t                                      rxChannelMask,
                                                                 const adi_adrv904x_RxDataFormatModes_e              formatSelect,
                                                                 const adi_adrv904x_IntegerConfigSettings_t* const   integerConfigSettings,
                                                                 const adi_adrv904x_SlicerConfigSettings_t* const    slicerConfigSettings)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[]    = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);

    static const uint8_t DEFAULT_INT_DATA_FORMAT            = 0U;
    static const uint8_t DEFAULT_INT_DATA_RESOLUTION        = 1U;
    static const uint8_t SLICER_PIN_CTRL_MODE_INTERNAL      = 0U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t intEmbedSlicer                = 0U;
    uint8_t intEmbedSlicerPos             = 0U;
    uint8_t intEmbedSlicerNumber          = 0U;
    uint8_t slicer3bitModeEn              = 0U;
    uint8_t intDataResolution             = 0U;
    uint8_t intDataFormat                 = 0U;
    uint8_t intParitySupport              = 0U;
    uint8_t intEvenParity                 = 0U;
    uint8_t maxSlicerValue                = 0U;
    uint8_t maxSlicerOverride             = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, slicerConfigSettings);

    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }

    /* Disable Gain Compensation */
    recoveryAction = adrv904x_RxDataFormatGainCompDisable(device,
                                                          rxChannelMask,
                                                          DEFAULT_INT_DATA_FORMAT,
                                                          DEFAULT_INT_DATA_RESOLUTION);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(  &device->common, recoveryAction, "Error while attempting to Disable Gain Compensation");
        return recoveryAction;
    }

    /* Resolve the slicer configuration */
    recoveryAction = adrv904x_RxIntEmbSlicerConfigResolve(device,
                                                          formatSelect,
                                                          integerConfigSettings->intEmbeddedBits,
                                                          integerConfigSettings->intParity,
                                                          &intEmbedSlicer, 
                                                          &intEmbedSlicerPos, 
                                                          &intEmbedSlicerNumber, 
                                                          &slicer3bitModeEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer embedded slicer config");
        return recoveryAction;
    }

    /* Resolve sample resolution */
    recoveryAction = adrv904x_RxIntSampleResFormatResolve(device,
                                                          integerConfigSettings,
                                                          &intDataResolution,
                                                          &intDataFormat);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer sample resolution");
        return recoveryAction;
    }

    /* Resolve parity selection */
    recoveryAction = adrv904x_RxIntParitySupportResolve(device,
                                                        integerConfigSettings,
                                                        &intParitySupport,
                                                        &intEvenParity);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer parity support");
        return recoveryAction;
    }

    /* Iterate through channel mask and clear any previously set Internal Slicer Word GPIOs for corresponding Rx channels */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            if ((formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
                (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
                (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE))
            {
                /* Disable any GPIOs currently being used to route Internal Slicer word bits for this RxChannel*/
                recoveryAction = adrv904x_RxInternalSlicerGpioDisable(device, RX_CHANNEL_ARR[chanIdx]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error disconnecting Rx Internal Slicer Word bits from previous GPIO pins.");
                    return recoveryAction;
                }
            }
        }
    }

    /* Iterate through channel mask and if set configure integer formatter settings on all DDCs of the corresponding Rx channel */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {

            /* If external slicer selected, throw an error indicated mode is not supported */
            if (formatSelect == ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER)
            {
                ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_RETURN(&device->common);
            }

            /* If format is ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE/3PIN_MODE, configure GPIOs */
            else if ((formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) ||
                     (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) ||
                     (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE))
            {
                recoveryAction = adrv904x_RxInternalSlicerGpioEnable(device,
                                                                     RX_CHANNEL_ARR[chanIdx],
                                                                     formatSelect, 
                                                                     slicerConfigSettings);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to enable GPIOs for internal slicer observation");
                    return recoveryAction;
                }
            }

            for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
            {
                /* Get Rx DDC Bitfield Address */
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, 
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  RX_DDC_BAND_ARR[bandIdx],
                                                                  &baseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }

                /* Enable Rx Gain Compensation and configure format for the requested Rx Channel */
                recoveryAction = adrv904x_RxDdc_GainCompEnable_BfSet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     ADI_ENABLE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "GainCompEnable Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntDataFormat_BfSet(device,
                                                                    NULL,
                                                                    baseAddr,
                                                                    intDataFormat);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntDataFormat Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntDataResolution_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        intDataResolution);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntDataResolution Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntEmbedSlicer_BfSet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     intEmbedSlicer);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicer Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntEmbedSlicerPos_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        intEmbedSlicerPos);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicerPos Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntEmbedSlicerNumber_BfSet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                           intEmbedSlicerNumber);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicerNumber Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntParitySupport_BfSet(device,
                                                                       NULL,
                                                                       baseAddr,
                                                                       intParitySupport);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntParitySupport Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntEvenParity_BfSet(device,
                                                                    NULL,
                                                                    baseAddr,
                                                                    intEvenParity);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEvenParity Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_SlicerPinControlMode_BfSet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                           SLICER_PIN_CTRL_MODE_INTERNAL);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "SlicerPinControlMode Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_IntSlicerLsbOnQ_BfSet(device,
                                                                      NULL,
                                                                      baseAddr,
                                                                      (uint8_t)integerConfigSettings->intEmbeddedPos);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntSlicerLsbOnQ Write issue");
                    return recoveryAction;
                }

                recoveryAction = adrv904x_RxDdc_Static3bitSlicerModeEn_BfSet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                             slicer3bitModeEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Static3bitSlicerModeEn Write issue");
                    return recoveryAction;
                }

                /* Configure pin control step size */
                recoveryAction = adrv904x_RxDdc_SlicerPinControlStep_BfSet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                           (uint8_t)slicerConfigSettings->intSlicerStepSize);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Static3bitSlicerModeEn Write issue");
                    return recoveryAction;
                }

                /* If format is ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO */
                /* and intEmbeddedBits is ADI_ADRV904X_EMBED_2_SLICERBITS_AT_[MSB/LSB]_3_BIT_SLICER, */
                /* then set 3bit slicer mode bit for the rxChannel in the device handle. */
                /* Otherwise clear bit in device handle */
                if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO)
                {
                    if ((integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER) ||
                        (integerConfigSettings->intEmbeddedBits == ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER))
                    {
                        device->devStateInfo.rxChannel3bitSlicerMode |= (uint8_t)RX_CHANNEL_ARR[chanIdx];
                    }
                    else
                    {
                        device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)RX_CHANNEL_ARR[chanIdx];
                    }
                }
                else
                {
                    device->devStateInfo.rxChannel3bitSlicerMode &= ~(uint8_t)RX_CHANNEL_ARR[chanIdx];
                }

                maxSlicerOverride = ADI_FALSE;
                if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE) 
                {
                    maxSlicerValue = 3U;
                    maxSlicerOverride = ADI_TRUE;
                }
                else if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE) 
                {
                    maxSlicerValue = 7U;
                    maxSlicerOverride = ADI_TRUE;
                }
                else if (formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE) 
                {
                    maxSlicerValue = 15U;
                    maxSlicerOverride = ADI_TRUE;
                }

                /* Configure max slicer override value */
                recoveryAction = adrv904x_RxDdc_MaxSlicerOverride_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        maxSlicerOverride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "MaxSlicerOverride Write issue");
                    return recoveryAction;
                }

                /* Configure max slicer override value */
                recoveryAction = adrv904x_RxDdc_MaxSlicer_BfSet(device,
                                                                NULL,
                                                                baseAddr,
                                                                maxSlicerValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "MaxSlicer Write issue");
                    return recoveryAction;
                }

            } /* end bandIdx for loop */
            
        } /* end rxChannelMask set */

    } /* end chIdx for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatIntegerSet(adi_adrv904x_Device_t * const                          device,
                                                                   const uint32_t                                         rxChannelMask,
                                                                   const adi_adrv904x_CddcDataFormatModes_e               formatSelect,
                                                                   const adi_adrv904x_CddcIntegerConfigSettings_t * const cddcIntegerConfigSettings,
                                                                   const adi_adrv904x_CddcSlicerConfigSettings_t * const  cddcSlicerConfigSettings)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
    uint8_t intEmbedSlicer                = 0U;
    uint8_t intEmbedSlicerPos             = 0U;
    uint8_t intEmbedSlicerNumber          = 0U;
    uint8_t slicer3bitModeEn              = 0U;
    uint8_t intDataResolution             = 0U;
    uint8_t intDataFormat                 = 0U;
    uint8_t intParitySupport              = 0U;
    uint8_t intEvenParity                 = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcIntegerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcSlicerConfigSettings);

    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }

    /* Resolve the slicer configuration */
    recoveryAction = adrv904x_CddcIntEmbSlicerConfigResolve(device,
                                                            formatSelect,
                                                            cddcIntegerConfigSettings->intEmbeddedBits,
                                                            cddcIntegerConfigSettings->intParity,
                                                            &intEmbedSlicer, 
                                                            &intEmbedSlicerPos, 
                                                            &intEmbedSlicerNumber, 
                                                            &slicer3bitModeEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer embedded slicer config");
        return recoveryAction;
    }

    /* Resolve sample resolution */
    recoveryAction = adrv904x_CddcIntSampleResFormatResolve(device,
                                                            cddcIntegerConfigSettings,
                                                            &intDataResolution,
                                                            &intDataFormat);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer sample resolution");
        return recoveryAction;
    }

    /* Resolve parity selection */
    recoveryAction = adrv904x_CddcIntParitySupportResolve(device,
                                                          cddcIntegerConfigSettings,
                                                         &intParitySupport,
                                                         &intEvenParity);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to resolve integer parity support");
        return recoveryAction;
    }

    /* Iterate through channel mask and if set configure integer formatter settings on all CDDCs of the corresponding Rx channel */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            /* Get Get CDDC Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC base address");
                return recoveryAction;
            }

            if (cddcSlicerConfigSettings->cddcSlicerCarrierSelect > 7u)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcSlicerCarrierSelect out of range (should be 0-7)");
                return recoveryAction;
            }

            recoveryAction = adi_adrv904x_Register32Write(device,
                                                          NULL,
                                                          ((uint32_t) baseAddr + 0x163U),
                                                          (uint32_t) cddcSlicerConfigSettings->cddcSlicerCarrierSelect,
                                                          0xFFU);

            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue with cddcSlicerCarrierSelect Select");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntDataFormat_BfSet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                             intDataFormat);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntDataFormat Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntDataResolution_BfSet(device,
                                                                                 NULL,
                                                                                 baseAddr,
                                                                                 intDataResolution);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntDataResolution Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfSet(device,
                                                                              NULL,
                                                                              baseAddr,
                                                                              intEmbedSlicer);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicer Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfSet(device,
                                                                                 NULL,
                                                                                 baseAddr,
                                                                                 intEmbedSlicerPos);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicerPos Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfSet(device,
                                                                                    NULL,
                                                                                    baseAddr,
                                                                                    intEmbedSlicerNumber);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEmbedSlicerNumber Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntParitySupport_BfSet(device,
                                                                                NULL,
                                                                                baseAddr,
                                                                                intParitySupport);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntParitySupport Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntEvenParity_BfSet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                             intEvenParity);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntEvenParity Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfSet(device,
                                                                               NULL,
                                                                               baseAddr,
                                                                               cddcIntegerConfigSettings->intEmbeddedPos);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "IntSlicerLsbOnQ Write issue");
                return recoveryAction;
            }
        } /* end rxChannelMask set */
    } /* end chIdx for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatSlicerSet(adi_adrv904x_Device_t * const                          device,
                                                                  const uint32_t                                         rxChannelMask,
                                                                  const adi_adrv904x_CddcSlicerConfigSettings_t * const  cddcSlicerConfigSettings)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcSlicerConfigSettings);

    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }

    /* Iterate through channel mask and if set configure integer formatter settings on all CDDCs of the corresponding Rx channel */
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ((rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            /* Get Get CDDC Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC base address");
                return recoveryAction;
            }

            recoveryAction =  adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfSet(device,
                                                                                           NULL,
                                                                                           baseAddr,
                                                                                           cddcSlicerConfigSettings->cddcEnSlicerAgcSync);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcEnSlicerAgcSync Write issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet(device,
                                                                                      NULL,
                                                                                      baseAddr,
                                                                                      cddcSlicerConfigSettings->cddcEn3BitSlicerMode);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcEn3BitSlicerMode Readback issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterCarrierSelect_BfSet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                             cddcSlicerConfigSettings->cddcFormatterCarrierSelect);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcFormatterCarrierSelect Readback issue");
                return recoveryAction;
            }

            if (cddcSlicerConfigSettings->cddcSlicerCarrierSelect > 7u)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcSlicerCarrierSelect out of range (should be 0-7)");
                return recoveryAction;
            }

            recoveryAction = adi_adrv904x_Register32Write(device,
                                                          NULL,
                                                          ((uint32_t) baseAddr + 0x163U),
                                                          (uint32_t) cddcSlicerConfigSettings->cddcSlicerCarrierSelect,
                                                          0xFFU);

            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue with cddcSlicerCarrierSelect Select");
                return recoveryAction;
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatEmbOvldMonitorSet(adi_adrv904x_Device_t * const                                 device,
                                                                        const uint32_t                                                rxChannelMask,
                                                                        const adi_adrv904x_EmbOverloadMonitorConfigSettings_t * const embOvldConfigSettings)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);

    static const uint8_t DEFAULT_INT_DATA_FORMAT            = 0U; /* Int data format - 2s complement */
    static const uint8_t DEFAULT_INT_DATA_RESOLUTION        = 1U; /* Int sample resolution - 16 bit */
    
    adi_adrv904x_ErrAction_e recoveryAction                   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e funcsAddr                  = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    uint8_t enable                                          = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, embOvldConfigSettings);
    
    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }
    
    /*Disable gain compensation and set integer format register to default*/
    recoveryAction = adrv904x_RxDataFormatGainCompDisable(  device,
                                                            rxChannelMask,
                                                            DEFAULT_INT_DATA_FORMAT,
                                                            DEFAULT_INT_DATA_RESOLUTION);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to disable gain compensation");
        return recoveryAction;
    }
    
    /* Iterate through channel mask and if set emb overload bitfields for both DDCs in the given Rx Channel*/
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ( (rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            /* Get Rx Funcs Bitfield Address */
            recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(  device,
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                 &funcsAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
                return recoveryAction;
            }
            /* Change recoveryAction value to ADI_ADRV904X_ERR_ACT_CHECK_PARAM so if the embOvldConfigSettings parameters 
             * aren't valid for any of the following if-conditions there will be a detectable error */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            
            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_APD_HIGH_THRESH))
            {
                /* Set the APD High Threshold exceeded indicator source */
                recoveryAction =  adrv904x_RxFuncs_ApdHighSrcSelect_BfSet(device,
                                                                          NULL,
                                                                          funcsAddr,
                                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorApdHighSrcSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "ApdHighSrcSelect Write issue");
                    return recoveryAction;
                }
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_APD_LOW_THRESH))
            {
                /* Set the APD Low Threshold exceeded indicator source */
                recoveryAction =  adrv904x_RxFuncs_ApdLowSrcSelect_BfSet(device,
                                                                         NULL,
                                                                         funcsAddr,
                                                                         (uint8_t)embOvldConfigSettings->embeddedMonitorApdLowSrcSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "ApdLowSrcSelect Write issue");
                    return recoveryAction;
                }
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_HIGH_THRESH))
            {
                /* Set the HB2 High Threshold exceeded indicator source */
                recoveryAction =  adrv904x_RxFuncs_Hb2HighSrcSelect_BfSet(device,
                                                                          NULL,
                                                                          funcsAddr,
                                                                          (uint8_t)embOvldConfigSettings->embeddedMonitorHb2HighSrcSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Hb2HighSrcSelect Write issue");
                    return recoveryAction;
                }
            }

            if ((embOvldConfigSettings->embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH) ||
                (embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_HB2_LOW_THRESH))
            {
                /* Set the HB2 Low Threshold exceeded indicator source */
                recoveryAction =  adrv904x_RxFuncs_Hb2LowSrcSelect_BfSet(device,
                                                                         NULL,
                                                                         funcsAddr,
                                                                         (uint8_t)embOvldConfigSettings->embeddedMonitorHb2LowSrcSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Hb2LowSrcSelect Write issue");
                    return recoveryAction;
                }
            }
            /* If recoveryAction wasn't altered in the previous if-conditions, there must be an error in the configuration */
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, embOvldConfigSettings, "Wrong Monitor Source on LSB I and LSB Q or LSB +1 I and LSB + 1 Q");
                return recoveryAction;
            }
            
            /* Set invert Hb2 flag config */
            enable = (embOvldConfigSettings->invertHb2Flag == 0U) ? 0U : 1U;
            recoveryAction =  adrv904x_RxFuncs_InvertHb2Low_BfSet(device,
                                                                  NULL,
                                                                  funcsAddr,
                                                                  enable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while writing invert Hb2 Flag configuration");
                return recoveryAction;
            }

            /* Set invert Apd2 flag config */
            enable = (embOvldConfigSettings->invertApdFlag == 0U) ? 0U : 1U;
            recoveryAction =  adrv904x_RxFuncs_InvertApdLow_BfSet(device,
                                                                  NULL,
                                                                  funcsAddr,
                                                                  enable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while writing invert Apd Flag configuration");
                return recoveryAction;
            }
            
            recoveryAction = (adi_adrv904x_ErrAction_e)adrv904x_RxEmbeddedOverloadBitsSet(  device,
                                                                                            chanIdx,
                                                                                            (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbI,
                                                                                            (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI,
                                                                                            (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbQ,
                                                                                            (uint8_t)embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing embedded overload bits");
                return recoveryAction;
            }

        } /* end rxChannelMask set */
        
    } /* end chIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatEmbOvldMonitorSet(adi_adrv904x_Device_t * const                                     device,
                                                                          const uint32_t                                                    rxChannelMask,
                                                                          const adi_adrv904x_CddcEmbOverloadMonitorConfigSettings_t * const cddcEmbOvldConfigSettings,
                                                                          const adi_adrv904x_CddcSlicerConfigSettings_t * const             cddcSlicerConfigSettings)
{
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7 };
    static const uint32_t RX_CHANNEL_ARR_SIZE               = sizeof(RX_CHANNEL_ARR) / sizeof(RX_CHANNEL_ARR[0]);
    
    adi_adrv904x_ErrAction_e recoveryAction                 = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e funcsAddr                = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcEmbOvldConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcSlicerConfigSettings);
    
    /* Verify that rxChannelMask selects only valid Rx Channels. No ORx */
    if ( ((rxChannelMask & (uint32_t)(ADI_ADRV904X_RX_MASK_ALL)) != rxChannelMask) ||
         (rxChannelMask == ADI_ADRV904X_RXOFF) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannelMask,
                                 "Invalid rxChannelMask parameter");
        return recoveryAction;
    }
    
    /* Iterate through channel mask and if set emb overload bitfields for both DDCs in the given Rx Channel*/
    for (uint8_t chanIdx = 0U; chanIdx < RX_CHANNEL_ARR_SIZE; chanIdx++)
    {
        if ( (rxChannelMask & (uint32_t)RX_CHANNEL_ARR[chanIdx]) == (uint32_t)RX_CHANNEL_ARR[chanIdx])
        {
            /* Get Rx Funcs Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  RX_CHANNEL_ARR[chanIdx],
                                                                  &funcsAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC Funcs base address");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterStaticSlicerAgcSyncEnable_BfSet(device,
                                                                                         NULL,
                                                                                         funcsAddr,
                                                                                         cddcSlicerConfigSettings->cddcEnSlicerAgcSync);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling AGC Sync for CDD slicer mode");
                return recoveryAction;
            }

            recoveryAction = adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfSet(device,
                                                                                      NULL,
                                                                                      funcsAddr,
                                                                                      cddcSlicerConfigSettings->cddcEn3BitSlicerMode);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling 3bit CDDC slicer mode");
                return recoveryAction;
            }
            
            recoveryAction = adrv904x_CddcEmbeddedOverloadBitsSet(device,
                                                                  chanIdx,
                                                                  (uint8_t)cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbI,
                                                                  (uint8_t)cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI,
                                                                  (uint8_t)cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbQ,
                                                                  (uint8_t)cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing embedded overload bits");
                return recoveryAction;
            }

        } /* end rxChannelMask set */
        
    } /* end chIdx for loop */
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxEmbeddedOverloadBitsSet(adi_adrv904x_Device_t * const device,
                                                                    uint8_t chanIdx,
                                                                    uint8_t lsbIData,
                                                                    uint8_t lsbIPlusData,
                                                                    uint8_t lsbQData,
                                                                    uint8_t lsbQPlusData)
{
    adi_adrv904x_ErrAction_e recoveryAction                   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7};
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[] = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    static const uint8_t LSB_POS_I_CHANNEL_ID               = 0U;
    static const uint8_t LSB_PLUS_ONE_POS_I_CHANNEL_ID      = 1U;
    static const uint8_t LSB_POS_Q_CHANNEL_ID               = 0U;
    static const uint8_t LSB_PLUS_ONE_POS_Q_CHANNEL_ID      = 1U;
    adrv904x_BfRxChanAddr_e rxDigBaseAddr                   = ADRV904X_BF_SLICE_RX_0__RX_DIG;
    adrv904x_BfRxDdcChanAddr_e ddcAddr                      = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t routClkDividerVal                               = 0U;
    
    /* Set all instances of rx_ddc bitfields */
    for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
    {
        /* Get Rx DDC Bitfield Address */
        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                          RX_CHANNEL_ARR[chanIdx],
                                                          RX_DDC_BAND_ARR[bandIdx],
                                                          &ddcAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
            return recoveryAction;
        }
                
        /* I Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv904x_RxDdc_RxMonFormatI_BfSet(device,
                                                           NULL,
                                                           ddcAddr,
                                                           LSB_POS_I_CHANNEL_ID,
                                                           lsbIData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatI_0 Write issue");
            return recoveryAction;
        }
                
        /* I Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv904x_RxDdc_RxMonFormatI_BfSet(device,
                                                           NULL,
                                                           ddcAddr,
                                                           LSB_PLUS_ONE_POS_I_CHANNEL_ID,
                                                           lsbIPlusData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatI_1 Write issue");
            return recoveryAction;
        }
                
        /* Q Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv904x_RxDdc_RxMonFormatQ_BfSet(device,
                                                           NULL,
                                                           ddcAddr,
                                                           LSB_POS_Q_CHANNEL_ID,
                                                           lsbQData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatQ_0 Write issue");
            return recoveryAction;
        }
                
        /* Q Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv904x_RxDdc_RxMonFormatQ_BfSet(device,
                                                           NULL,
                                                           ddcAddr,
                                                           LSB_PLUS_ONE_POS_Q_CHANNEL_ID,
                                                           lsbQPlusData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatQ_1 Write issue");
            return recoveryAction;
        }

    } /* end bandIdx for loop */
    
    /* Enable the JESD clock for embedded overload bits if there at least one bit is being used */
    if ((lsbIData != (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) ||
        (lsbQData != (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) ||
        (lsbIPlusData != (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) ||
        (lsbQPlusData != (uint8_t)ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED)) 
    {
        recoveryAction = adrv904x_RxBitfieldAddressGet(device, RX_CHANNEL_ARR[chanIdx], &rxDigBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx dig address");
            return recoveryAction;
        }
        
        recoveryAction = adrv904x_RxDig_RoutClkDivideRatio_BfGet(device, NULL, rxDigBaseAddr, &routClkDividerVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read rout clock divider value");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDig_Ddc1Hb1OutClkDivideRatio_BfSet(device, NULL, rxDigBaseAddr, routClkDividerVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write ddc1 hb1 out clock div ratio");
            return recoveryAction;
        }

        /* This is to provide smon block a clock source */
        recoveryAction = adrv904x_RxDig_StreamprocDdc1Hb1OutClkEnable_BfSet(device, NULL, rxDigBaseAddr, 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set ddc1 hb1 out clock enable");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcEmbeddedOverloadBitsSet(adi_adrv904x_Device_t * const device,
                                                                      uint8_t chanIdx,
                                                                      uint8_t lsbIData,
                                                                      uint8_t lsbIPlusData,
                                                                      uint8_t lsbQData,
                                                                      uint8_t lsbQPlusData)
{
    adi_adrv904x_ErrAction_e recoveryAction                   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    
    static const adi_adrv904x_RxChannels_e RX_CHANNEL_ARR[] = { ADI_ADRV904X_RX0, ADI_ADRV904X_RX1, ADI_ADRV904X_RX2, ADI_ADRV904X_RX3, 
                                                                ADI_ADRV904X_RX4, ADI_ADRV904X_RX5, ADI_ADRV904X_RX6, ADI_ADRV904X_RX7};
    static const adi_adrv904x_RxDdcs_e RX_DDC_BAND_ARR[] = { ADI_ADRV904X_RX_DDC_BAND0, ADI_ADRV904X_RX_DDC_BAND1 };
    static const uint32_t RX_DDC_BAND_ARR_SIZE              = sizeof(RX_DDC_BAND_ARR) / sizeof(RX_DDC_BAND_ARR[0U]);
    adrv904x_BfCddcFuncsChanAddr_e cddcAddr                 = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Set all instances of rx_ddc bitfields */
    for (uint32_t bandIdx = 0U; bandIdx < RX_DDC_BAND_ARR_SIZE; bandIdx++)
    {
        /* Get Rx DDC Bitfield Address */
        recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, 
                                                              RX_CHANNEL_ARR[chanIdx],
                                                              &cddcAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
            return recoveryAction;
        }
                
        /* I Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfSet(device,
                                                                         NULL,
                                                                         cddcAddr,
                                                                         lsbIData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatI_0 Write issue");
            return recoveryAction;
        }
                
        /* I Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfSet(device,
                                                                         NULL,
                                                                         cddcAddr,
                                                                         lsbIPlusData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatI_1 Write issue");
            return recoveryAction;
        }
                
        /* Q Sample LSB Posn Embedded overload indicator config */
        recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfSet(device,
                                                                         NULL,
                                                                         cddcAddr,
                                                                         lsbQData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatQ_0 Write issue");
            return recoveryAction;
        }
                
        /* Q Sample LSB + 1 Posn Embedded overload indicator config */
        recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfSet(device,
                                                                         NULL,
                                                                         cddcAddr,
                                                                         lsbQPlusData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatQ_1 Write issue");
            return recoveryAction;
        }

    } /* end bandIdx for loop */

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntSampleResFormatResolve(adi_adrv904x_Device_t * const                      device,
                                                                      const adi_adrv904x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                      uint8_t * const                                    intDataResolution,
                                                                      uint8_t * const                                    intDataFormat)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT = 0U;
    static const uint8_t INT_DATA_FORMAT_SIGNED_MAGNITUDE = 1U;
    static const uint8_t INT_DATA_RESOLUTION_12BITS = 0U;
    static const uint8_t INT_DATA_RESOLUTION_16BITS = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intDataResolution);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intDataFormat);
    
    /* Resolve Sample Resolution */
    switch (integerConfigSettings->intSampleResolution)
    {
        case ADI_ADRV904X_INTEGER_12BIT_2SCOMP: 
        {
            *intDataResolution = INT_DATA_RESOLUTION_12BITS;
            *intDataFormat = INT_DATA_FORMAT_2S_COMPLEMENT;
            break;
        }

        case ADI_ADRV904X_INTEGER_12BIT_SIGNED: 
        {
            *intDataResolution = INT_DATA_RESOLUTION_12BITS;
            *intDataFormat = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
            break;
        }

        case ADI_ADRV904X_INTEGER_16BIT_2SCOMP: 
        {
            *intDataResolution = INT_DATA_RESOLUTION_16BITS;
            *intDataFormat = INT_DATA_FORMAT_2S_COMPLEMENT;
            break;
        }

        case ADI_ADRV904X_INTEGER_16BIT_SIGNED: 
        {
            *intDataResolution = INT_DATA_RESOLUTION_16BITS;
            *intDataFormat = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
            break;
        }

        default: 
        {   
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction, 
                                     integerConfigSettings->intSampleResolution,
                                     "Invalid integer formatter data resolution selected");
            return recoveryAction;
            
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcIntSampleResFormatResolve(adi_adrv904x_Device_t* const                          device,
                                                                        const adi_adrv904x_CddcIntegerConfigSettings_t* const cddcIntegerConfigSettings,
                                                                        uint8_t* const                                        intDataResolution,
                                                                        uint8_t* const                                        intDataFormat)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT = 0U;
    static const uint8_t INT_DATA_FORMAT_SIGNED_MAGNITUDE = 1U;
    static const uint8_t INT_DATA_RESOLUTION_12BITS = 0U;
    static const uint8_t INT_DATA_RESOLUTION_16BITS = 1U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcIntegerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intDataResolution);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intDataFormat);

    /* Resolve Sample Resolution */
    switch (cddcIntegerConfigSettings->intSampleResolution)
    {
        case ADI_ADRV904X_INTEGER_12BIT_2SCOMP:
        {
            *intDataResolution = INT_DATA_RESOLUTION_12BITS;
            *intDataFormat = INT_DATA_FORMAT_2S_COMPLEMENT;
            break;
        }

        case ADI_ADRV904X_INTEGER_12BIT_SIGNED:
        {
            *intDataResolution = INT_DATA_RESOLUTION_12BITS;
            *intDataFormat = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
            break;
        }

        case ADI_ADRV904X_INTEGER_16BIT_2SCOMP:
        {
            *intDataResolution = INT_DATA_RESOLUTION_16BITS;
            *intDataFormat = INT_DATA_FORMAT_2S_COMPLEMENT;
            break;
        }

        case ADI_ADRV904X_INTEGER_16BIT_SIGNED:
        {
            *intDataResolution = INT_DATA_RESOLUTION_16BITS;
            *intDataFormat = INT_DATA_FORMAT_SIGNED_MAGNITUDE;
            break;
        }

        default:
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cddcIntegerConfigSettings->intSampleResolution,
                "Invalid integer formatter data resolution selected");
            return recoveryAction;

        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxInternalSlicerGpioDisable(adi_adrv904x_Device_t * const   device,
                                                                      const adi_adrv904x_RxChannels_e rxChannel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t slicerBit = 0U;
    adi_adrv904x_GpioPinSel_e tmpPin = ADI_ADRV904X_GPIO_INVALID;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Get channel index from enum */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction, 
                                 rxChannel,
                                 "Invalid Rx channel selected for GPIOs to be disabled. Must be in range RX0-RX7.");
        return recoveryAction;
    }
    
    /* Release any of the slicer bits from previous GPIOs */
    for (slicerBit = 0U; slicerBit < ADI_ADRV904X_MAX_RX_INT_SLICER_BITS; slicerBit++)
    {
        recoveryAction = adrv904x_GpioSignalFind(  device,
                                                  &tmpPin,
                                                   adrv904x_RxInternalSlicerOutputSignals[slicerBit],
                                                   (adi_adrv904x_Channels_e)rxChannel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error searching for previous GPIO that was routing Rx Internal Slicer Word bit.");
            return recoveryAction;
        }
        
        if (tmpPin != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease( device,
                                                         tmpPin,
                                                         adrv904x_RxInternalSlicerOutputSignals[slicerBit],
                                                         (uint32_t)rxChannel);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing previous GPIO that was routing Rx Internal Slicer Word bit.");
                return recoveryAction;
            }
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxInternalSlicerGpioEnable(adi_adrv904x_Device_t * const                       device,
                                                                     const adi_adrv904x_RxChannels_e                     rxChannel,
                                                                     const adi_adrv904x_RxDataFormatModes_e              intSlicerFormatSel,
                                                                     const adi_adrv904x_SlicerConfigSettings_t* const    slicerConfigSettings)
{   
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t NUM_BITS = 0U;
    uint8_t slicerBit = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Get channel index from enum */
    switch (rxChannel)
    {
        case (ADI_ADRV904X_RX0): /* Fall through */
        case (ADI_ADRV904X_RX1): /* Fall through */
        case (ADI_ADRV904X_RX2): /* Fall through */
        case (ADI_ADRV904X_RX3): /* Fall through */
        case (ADI_ADRV904X_RX4): /* Fall through */
        case (ADI_ADRV904X_RX5): /* Fall through */
        case (ADI_ADRV904X_RX6): /* Fall through */
        case (ADI_ADRV904X_RX7):
            break;
        default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction, 
                                         rxChannel,
                                         "Invalid Rx channel selected for GPIOs to be enabled. Must be in range RX0-RX7.");
                return recoveryAction;
    }

    /* Set number of Slicer bits required by chosen IntSlicerFormatSel */
    switch (intSlicerFormatSel)
    {
        case (ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE):
            NUM_BITS = 4U;
            break;
        case (ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE):
            NUM_BITS = 3U;
            break;
        case (ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE):
            NUM_BITS = 2U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction, 
                                     intSlicerFormatSel,
                                     "Invalid Rx Internal Slicer Format selected for GPIO setup. Must be 4PIN Mode or 3PIN mode to enable GPIOs.");
            return recoveryAction;
    }
    
    /* Route selected GPIOs for RX Internal Slicer Word bits, for this Rx Channel */
    for (slicerBit = 0U; slicerBit < NUM_BITS; slicerBit++)
    {
        recoveryAction = adrv904x_GpioSignalSet( device,
                                                 slicerConfigSettings->intSlicerGpioSelect[slicerBit],
                                                 adrv904x_RxInternalSlicerOutputSignals[slicerBit],
                                                 (uint32_t)rxChannel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error routing Rx Internal Slicer Word bit to GPIO pin.");
            return recoveryAction;
        }
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntEmbSlicerConfigResolve(adi_adrv904x_Device_t * const             device,
                                                                      const adi_adrv904x_RxDataFormatModes_e    formatSelect,
                                                                      const adi_adrv904x_RxSlicerEmbeddedBits_e intEmbeddedBits,
                                                                      const adi_adrv904x_RxIntParity_e          parity,
                                                                      uint8_t * const                           intEmbedSlicer,
                                                                      uint8_t * const                           intEmbedSlicerPos,
                                                                      uint8_t * const                           intEmbedSlicerNumber,
                                                                      uint8_t * const                           staticSlicer3bitModeEn)
{
    static const uint8_t INT_EMBED_SLICER_DISABLE      = 0U;
    static const uint8_t INT_EMBED_SLICER_ENABLE       = 1U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB      = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_LSB      = 1U;
    static const uint8_t INT_EMBED_SLICER_2_BITS       = 0U;
    static const uint8_t INT_EMBED_SLICER_1_BIT        = 1U;
    static const uint8_t INT_SLICER_3_BIT_MODE_DISABLE = 0U;
    static const uint8_t INT_SLICER_3_BIT_MODE_ENABLE  = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicer);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicerPos);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicerNumber);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, staticSlicer3bitModeEn);
    
    if ( formatSelect == ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO )
    {
        /* Resolve slicer and slicer position configs */
        switch (intEmbeddedBits)
        {
            case ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS:
                *intEmbedSlicer         = INT_EMBED_SLICER_DISABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_1_BIT;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_1_BIT;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = (parity == ADI_ADRV904X_NO_PARITY) ?
                                          INT_SLICER_3_BIT_MODE_ENABLE :
                                          INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = (parity == ADI_ADRV904X_NO_PARITY) ?
                                          INT_SLICER_3_BIT_MODE_ENABLE :
                                          INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER:
                *intEmbedSlicer         = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos      = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber   = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         intEmbeddedBits,
                                         "Invalid integer formatter embedded slicer mode config selected");
                return recoveryAction;
                break;
        }
    }
    else
    {
        /* For other data format modes set int embedded slicer config to default */
        *intEmbedSlicer       = INT_EMBED_SLICER_DISABLE;
        *intEmbedSlicerPos    = INT_EMBED_SLICER_POS_MSB;
        *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
        *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcIntEmbSlicerConfigResolve(adi_adrv904x_Device_t* const              device,
                                                                        const adi_adrv904x_CddcDataFormatModes_e  formatSelect,
                                                                        const adi_adrv904x_RxSlicerEmbeddedBits_e intEmbeddedBits,
                                                                        const adi_adrv904x_RxIntParity_e          parity,
                                                                        uint8_t* const                            intEmbedSlicer,
                                                                        uint8_t* const                            intEmbedSlicerPos,
                                                                        uint8_t* const                            intEmbedSlicerNumber,
                                                                        uint8_t* const                            staticSlicer3bitModeEn)
{
    static const uint8_t INT_EMBED_SLICER_DISABLE = 0U;
    static const uint8_t INT_EMBED_SLICER_ENABLE = 1U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_LSB = 1U;
    static const uint8_t INT_EMBED_SLICER_2_BITS = 0U;
    static const uint8_t INT_EMBED_SLICER_1_BIT = 1U;
    static const uint8_t INT_SLICER_3_BIT_MODE_DISABLE = 0U;
    static const uint8_t INT_SLICER_3_BIT_MODE_ENABLE = 1U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicer);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicerPos);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEmbedSlicerNumber);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, staticSlicer3bitModeEn);

    if (formatSelect == ADI_ADRV904X_CDDC_INTEGER)
    {
        /* Resolve slicer and slicer position configs */
        switch (intEmbeddedBits)
        {
            case ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS:
                *intEmbedSlicer = INT_EMBED_SLICER_DISABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_1_BIT;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_1_BIT;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = (parity == ADI_ADRV904X_NO_PARITY) ?
                    INT_SLICER_3_BIT_MODE_ENABLE :
                    INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = (parity == ADI_ADRV904X_NO_PARITY) ?
                    INT_SLICER_3_BIT_MODE_ENABLE :
                    INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_MSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            case ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER:
                *intEmbedSlicer = INT_EMBED_SLICER_ENABLE;
                *intEmbedSlicerPos = INT_EMBED_SLICER_POS_LSB;
                *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
                *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                    recoveryAction,
                    intEmbeddedBits,
                    "Invalid integer formatter embedded slicer mode config selected");
                return recoveryAction;
                break;
        }
    }
    else
    {
        /* For other data format modes set int embedded slicer config to default */
        *intEmbedSlicer = INT_EMBED_SLICER_DISABLE;
        *intEmbedSlicerPos = INT_EMBED_SLICER_POS_MSB;
        *intEmbedSlicerNumber = INT_EMBED_SLICER_2_BITS;
        *staticSlicer3bitModeEn = INT_SLICER_3_BIT_MODE_DISABLE;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntParitySupportResolve(adi_adrv904x_Device_t * const                      device,
                                                                    const adi_adrv904x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                    uint8_t * const                                    intParitySupport,
                                                                    uint8_t * const                                    intEvenParity)
{
    static const uint8_t INT_PARITY_SUPPORT_DISABLE = 0U;
    static const uint8_t INT_PARITY_SUPPORT_ENABLE  = 1U;
    static const uint8_t INT_PARITY_EVEN            = 0U;
    static const uint8_t INT_PARITY_ODD             = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intParitySupport);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEvenParity);
    
    switch (integerConfigSettings->intParity)
    {
        case ADI_ADRV904X_3BIT_SLICER_ODD_PARITY: 
            *intParitySupport = INT_PARITY_SUPPORT_ENABLE;
            *intEvenParity    = INT_PARITY_ODD;
            break;
        case ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY: 
            *intParitySupport = INT_PARITY_SUPPORT_ENABLE;
            *intEvenParity    = INT_PARITY_EVEN;
            break;
        case ADI_ADRV904X_NO_PARITY: 
            *intParitySupport = INT_PARITY_SUPPORT_DISABLE;
            *intEvenParity    = INT_PARITY_EVEN;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     integerConfigSettings->intParity,
                                     "Invalid integer formatter parity selection");
            return recoveryAction;
            break;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcIntParitySupportResolve(adi_adrv904x_Device_t* const                          device,
                                                                      const adi_adrv904x_CddcIntegerConfigSettings_t* const cddcIntegerConfigSettings,
                                                                      uint8_t* const                                        intParitySupport,
                                                                      uint8_t* const                                        intEvenParity)
{
    static const uint8_t INT_PARITY_SUPPORT_DISABLE = 0U;
    static const uint8_t INT_PARITY_SUPPORT_ENABLE = 1U;
    static const uint8_t INT_PARITY_EVEN = 0U;
    static const uint8_t INT_PARITY_ODD = 1U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcIntegerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intParitySupport);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intEvenParity);

    switch (cddcIntegerConfigSettings->intParity)
    {
        case ADI_ADRV904X_3BIT_SLICER_ODD_PARITY:
            *intParitySupport = INT_PARITY_SUPPORT_ENABLE;
            *intEvenParity = INT_PARITY_ODD;
            break;
        case ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY:
            *intParitySupport = INT_PARITY_SUPPORT_ENABLE;
            *intEvenParity = INT_PARITY_EVEN;
            break;
        case ADI_ADRV904X_NO_PARITY:
            *intParitySupport = INT_PARITY_SUPPORT_DISABLE;
            *intEvenParity = INT_PARITY_EVEN;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cddcIntegerConfigSettings->intParity,
                "Invalid integer formatter parity selection");
            return recoveryAction;
            break;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatSelectGet(adi_adrv904x_Device_t * const            device,
                                                                const adi_adrv904x_RxChannels_e          rxChannel,
                                                                adi_adrv904x_RxDataFormatModes_e * const rxDataFormat)
{
    adi_adrv904x_ErrAction_e recoveryAction               = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr                 = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t gainCompEnStatus                            = 0U;
    uint8_t floatingPointEn                             = 0U;
    uint8_t rxExtSlicerModeEn                           = 0U;
    uint8_t intEmbedSlicer                              = 0U;
    
    adi_adrv904x_EmbOverloadMonitorConfigSettings_t embOvldMonitorSettings = { ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                                                                               ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED, 
                                                                               ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                                                                               ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                                                                               ADI_ADRV904X_HB2_LOW_SRC_OVRG_LOW,
                                                                               ADI_ADRV904X_HB2_HIGH_SRC_OVRG_HIGH,
                                                                               ADI_ADRV904X_APD_LOW_SRC_LOWER_LEVEL_BLOCKER_EXCEEDED,
                                                                               ADI_ADRV904X_APD_HIGH_SRC_UPPER_LEVEL_BLOCKER_EXCEEDED,
                                                                               0U, 
                                                                               0U };
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxDataFormat);
    
    /* Check if the requested rxChannel is valid Rx channel. */
    if ((rxChannel == ADI_ADRV904X_RX0) ||
        (rxChannel == ADI_ADRV904X_RX1) ||
        (rxChannel == ADI_ADRV904X_RX2) ||
        (rxChannel == ADI_ADRV904X_RX3) ||
        (rxChannel == ADI_ADRV904X_RX4) ||
        (rxChannel == ADI_ADRV904X_RX5) ||
        (rxChannel == ADI_ADRV904X_RX6) ||
        (rxChannel == ADI_ADRV904X_RX7))
    {
    
        /* Get Rx DDC Bitfield Address. Band0 only and assume all other bands are equivalent */
        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                            rxChannel,
                                                            ADI_ADRV904X_RX_DDC_BAND0,
                                                           &baseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
            return recoveryAction;
        }
       
        
        /* Check if gain comp is enabled. Read Band0 instance only, assuming all bands are set equivalently */
        recoveryAction = adrv904x_RxDdc_GainCompEnable_BfGet(device,
                                                             NULL,
                                                             baseAddr,
                                                             &gainCompEnStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompEnable Readback issue");
            return recoveryAction;
        }
        gainCompEnStatus = gainCompEnStatus ? ADI_ENABLE : ADI_DISABLE;
        
        /* If gain compensation is not enabled, determine if there embedded overload monitor data is being used */
        if (gainCompEnStatus == ADI_DISABLE)
        {
            
            recoveryAction = adrv904x_RxDataFormatEmbOvldMonitorGet(device, rxChannel, &embOvldMonitorSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve embOvldMonitorSettings");
                return recoveryAction;
            }

            if ((embOvldMonitorSettings.embeddedMonitorSrcLsbI        == ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbQ        == ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI == ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED) &&
                (embOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ == ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED))
            {
                *rxDataFormat = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED;
            }
            else
            {
                *rxDataFormat = ADI_ADRV904X_EMBED_OVERLOAD_MONITOR_DATA;
            }
            
        }
        
        /* If gain compensation is enabled, check if other modes are being used */
        else
        {
            /* Check if floating point mode is enabled */
            recoveryAction = adrv904x_RxDdc_FpEn_BfGet(device,
                                                       NULL,
                                                       baseAddr,
                                                       &floatingPointEn);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Floating Point Enable Readback issue");
                return recoveryAction;
            }
            
            if (floatingPointEn)
            {
                *rxDataFormat = ADI_ADRV904X_GAIN_WITH_FLOATING_POINT;
            }
            else
            {
                /* Check if Rx Slicer Pin Control Mode = External */
                recoveryAction = adrv904x_RxDdc_SlicerPinControlMode_BfGet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                           &rxExtSlicerModeEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SlicerPinControlMode Readback issue");
                    return recoveryAction;
                }
                
                if (rxExtSlicerModeEn)
                {
                    *rxDataFormat = ADI_ADRV904X_GAIN_WITH_EXTERNAL_SLICER;
                }
                else
                {
                    recoveryAction = adrv904x_RxDdc_IntEmbedSlicer_BfGet(device,
                                                                         NULL,
                                                                         baseAddr,
                                                                         &intEmbedSlicer);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxIntEmbedSlicer Readback issue");
                        return recoveryAction;
                    }

                    if (intEmbedSlicer)
                    {
                        *rxDataFormat = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_NOGPIO;
                    }
                    else
                    {
                        recoveryAction = adrv904x_RxInternalSlicerGpioEnableGet( device,
                                                                                 rxChannel,
                                                                                 rxDataFormat,
                                                                                 NULL);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve rxDataFormat");
                            return recoveryAction;
                        }
                    }
                }
                
            }
        }
    
    } /* End valid Rx Channel if block */
    
    /* Check if the requested rxChannel is valid ORx channel */
    else if ((rxChannel == ADI_ADRV904X_ORX0) ||
             (rxChannel == ADI_ADRV904X_ORX1) )
    {   
        /* Only possible format for ORx channels is ADI_ADRV904X_GAIN_COMPENSATION_DISABLED */
        *rxDataFormat = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED;
    } /* End valid ORx Channel if block */
    
    /* Otherwise, throw an error for invalid Rx channel */
    else {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxDataFormatSelectGet");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatSelectGet(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxChannels_e            rxChannel,
                                                                  adi_adrv904x_CddcDataFormatModes_e * const cddcDataFormat)
{
    adi_adrv904x_ErrAction_e recoveryAction             = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    adi_adrv904x_CddcEmbOverloadMonitorConfigSettings_t cddcEmbOvldMonitorSettings = {ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                                                                                      ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED, 
                                                                                      ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED,
                                                                                      ADI_ADRV904X_RX_EMB_MON_SRC_NO_MON_DATA_EMBEDDED};
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcDataFormat);
    
    /* Check if the requested rxChannel is valid Rx channel. */
    if ((rxChannel == ADI_ADRV904X_RX0) ||
        (rxChannel == ADI_ADRV904X_RX1) ||
        (rxChannel == ADI_ADRV904X_RX2) ||
        (rxChannel == ADI_ADRV904X_RX3) ||
        (rxChannel == ADI_ADRV904X_RX4) ||
        (rxChannel == ADI_ADRV904X_RX5) ||
        (rxChannel == ADI_ADRV904X_RX6) ||
        (rxChannel == ADI_ADRV904X_RX7))
    {
        recoveryAction = adrv904x_CddcDataFormatEmbOvldMonitorGet(device, rxChannel, &cddcEmbOvldMonitorSettings);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve cddcEmbOvldMonitorSettings");
            return recoveryAction;
        }

        if ((cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbI        != 0u) &&
            (cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbQ        != 0u) &&
            (cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneI != 0u) &&
            (cddcEmbOvldMonitorSettings.embeddedMonitorSrcLsbPlusOneQ != 0u))
        {
            *cddcDataFormat = ADI_ADRV904X_CDDC_EMBED_OVERLOAD_MONITOR_DATA;
        }
    
    } /* End valid Rx Channel if block */
    
    /* Otherwise, throw an error for invalid Rx channel */
    else {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for CddcDataFormatSelectGet");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatFloatingPointGet(adi_adrv904x_Device_t * const                      device,
                                                                       const adi_adrv904x_RxChannels_e                    rxChannel,
                                                                       adi_adrv904x_FloatingPointConfigSettings_t * const floatingPointConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction               = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr                 = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    
    adrv904x_Bf_RxDdc_FpRoundMode_e fpRoundModeBf       = ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOEVEN;
    uint8_t fpDataFormatBf                              = 0U;
    uint8_t fpEncodeNanBf                               = 0U;
    adrv904x_Bf_RxDdc_FpExponentBits_e fpNumExpBitsBf   = ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_2;
    uint8_t fpHideLeadingOneBf                          = 0U;
    uint8_t fpAttenStepsBf                              = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, floatingPointConfig);
    
    /* Check that the requested rxChannel is valid*/
    switch( rxChannel ) 
    {
        case ADI_ADRV904X_RX0:
        case ADI_ADRV904X_RX1:
        case ADI_ADRV904X_RX2:
        case ADI_ADRV904X_RX3:
        case ADI_ADRV904X_RX4:
        case ADI_ADRV904X_RX5:
        case ADI_ADRV904X_RX6:
        case ADI_ADRV904X_RX7:
        {
            /* Get Rx DDC Bitfield Address */
            recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                rxChannel,
                                                                ADI_ADRV904X_RX_DDC_BAND0,
                                                               &baseAddr);                
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                return recoveryAction;
            }
            
            /* Get bitfield values */
            recoveryAction =  adrv904x_RxDdc_FpRoundMode_BfGet(device,
                                                               NULL,
                                                               baseAddr,
                                                               &fpRoundModeBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpRoundMode readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_RxDdc_FpFloatDataFormat_BfGet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     &fpDataFormatBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpFloatDataFormat readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_RxDdc_FpNanEncEn_BfGet(device,
                                                              NULL,
                                                              baseAddr,
                                                              &fpEncodeNanBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpNanEncEn readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_RxDdc_FpExponentBits_BfGet(device,
                                                                  NULL,
                                                                  baseAddr,
                                                                  &fpNumExpBitsBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpExponentBits readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_RxDdc_FpHideLeadingOnes_BfGet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     &fpHideLeadingOneBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpHideLeadingOnes readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_RxDdc_FpIntDataAtten_BfGet(device,
                                                                  NULL,
                                                                  baseAddr,
                                                                  &fpAttenStepsBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpIntDataAtten readback issue");
                return recoveryAction;
            }
            
            break;
        }
        
        case ADI_ADRV904X_ORX0:
        case ADI_ADRV904X_ORX1:
            /* Do nothing */
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Invalid Rx Channel Requested for RxDataFormatFloatingPointGet read");
            return recoveryAction;
    }
    
    /* Update the structure with read back data */
    floatingPointConfig->fpRoundMode        = (adi_adrv904x_FpRoundModes_e)fpRoundModeBf;
    floatingPointConfig->fpDataFormat       = (adi_adrv904x_FpFloatDataFormat_e)fpDataFormatBf;
    floatingPointConfig->fpEncodeNan        = (adi_adrv904x_FpNanEncode_e)fpEncodeNanBf;
    floatingPointConfig->fpNumExpBits       = (adi_adrv904x_FpExponentModes_e)fpNumExpBitsBf;
    floatingPointConfig->fpHideLeadingOne   = (adi_adrv904x_FpHideLeadingOne_e)fpHideLeadingOneBf;
    floatingPointConfig->fpAttenSteps       = (adi_adrv904x_FpAttenSteps_e)fpAttenStepsBf;
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatFloatingPointGet(adi_adrv904x_Device_t * const                          device,
                                                                         const adi_adrv904x_RxChannels_e                        rxChannel,
                                                                         adi_adrv904x_CddcFloatingPointConfigSettings_t * const cddcFloatingPointConfig)
{
    adi_adrv904x_ErrAction_e recoveryAction             = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr             = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
    
    uint8_t fpRoundModeBf                               = ADRV904X_BF_RX_DDC_FP_ROUND_MODE_ROUNDTIESTOEVEN;
    uint8_t fpDataFormatBf                              = 0U;
    uint8_t fpEncodeNanBf                               = 0U;
    uint8_t fpNumExpBitsBf                              = ADRV904X_BF_RX_DDC_FP_EXPONENT_BITS_RX1_EXP_2;
    uint8_t fpHideLeadingOneBf                          = 0U;
    uint8_t fpEn                                        = 0u;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcFloatingPointConfig);
    
    /* Check that the requested rxChannel is valid*/
    switch( rxChannel ) 
    {
        case ADI_ADRV904X_RX0:
        case ADI_ADRV904X_RX1:
        case ADI_ADRV904X_RX2:
        case ADI_ADRV904X_RX3:
        case ADI_ADRV904X_RX4:
        case ADI_ADRV904X_RX5:
        case ADI_ADRV904X_RX6:
        case ADI_ADRV904X_RX7:
        {
            /* Get CDDC Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  rxChannel,
                                                                 &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC base address");
                return recoveryAction;
            }
            
            /* Get bitfield values */
            recoveryAction =  adrv904x_CddcFuncs_FormatterFpRoundMode_BfGet(device,
                                                                            NULL,
                                                                            baseAddr,
                                                                           &fpRoundModeBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpRoundMode readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_CddcFuncs_FormatterFpFloatDataFormat_BfGet(device,
                                                                                  NULL,
                                                                                  baseAddr,
                                                                                 &fpDataFormatBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpFloatDataFormat readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_CddcFuncs_FormatterFpNanEncEn_BfGet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                          &fpEncodeNanBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpNanEncEn readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_CddcFuncs_FormatterFpExponentBits_BfGet(device,
                                                                               NULL,
                                                                               baseAddr,
                                                                              &fpNumExpBitsBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpExponentBits readback issue");
                return recoveryAction;
            }
            
            recoveryAction =  adrv904x_CddcFuncs_FormatterFpHideLeadingOnes_BfGet(device,
                                                                                  NULL,
                                                                                  baseAddr,
                                                                                 &fpHideLeadingOneBf);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "FpHideLeadingOnes readback issue");
                return recoveryAction;
            }

            recoveryAction =  adrv904x_CddcFuncs_FormatterFpEn_BfGet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                    &fpEn);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "fpEn readback issue");
                return recoveryAction;
            }

            
            break;
        }
        
        case ADI_ADRV904X_ORX0:
        case ADI_ADRV904X_ORX1:
            /* Do nothing */
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Invalid Rx Channel Requested for RxDataFormatFloatingPointGet read");
            return recoveryAction;
    }
    
    /* Update the structure with read back data */
    cddcFloatingPointConfig->cddcFpRoundMode        = (adi_adrv904x_FpRoundModes_e)fpRoundModeBf;
    cddcFloatingPointConfig->cddcFpDataFormat       = (adi_adrv904x_FpFloatDataFormat_e)fpDataFormatBf;
    cddcFloatingPointConfig->cddcFpEncodeNan        = (adi_adrv904x_FpNanEncode_e)fpEncodeNanBf;
    cddcFloatingPointConfig->cddcFpNumExpBits       = (adi_adrv904x_FpExponentModes_e)fpNumExpBitsBf;
    cddcFloatingPointConfig->cddcFpHideLeadingOne   = (adi_adrv904x_FpHideLeadingOne_e)fpHideLeadingOneBf;
    cddcFloatingPointConfig->cddcFpEn               = fpEn;
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatIntegerGet(adi_adrv904x_Device_t * const                device,
                                                                 const adi_adrv904x_RxChannels_e              rxChannel,
                                                                 adi_adrv904x_IntegerConfigSettings_t * const integerConfigSettings,
                                                                 adi_adrv904x_SlicerConfigSettings_t * const  slicerConfigSettings)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT = 0U;
    static const uint8_t INT_DATA_RESOLUTION_16BITS    = 1U;
    static const uint8_t INT_EMBED_SLICER_DISABLE      = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB      = 0U;
    static const uint8_t INT_EMBED_SLICER_2_BITS       = 0U;
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE    = 0U;
    static const uint8_t INT_PARITY_EVEN               = 0U;
    static const uint8_t SLICER_PIN_CTRL_MODE_INTERNAL = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr        = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    adrv904x_BfOrxDigChanAddr_e orxDigBaseAddr = (adrv904x_BfOrxDigChanAddr_e) 0U;
    uint8_t slicerBit = 0U;
    
    /* Preset local variables to default settings */
    uint8_t intEvenParity                 = INT_PARITY_EVEN;
    uint8_t intParitySupport              = INT_PARTIY_SUPPORT_DISABLE;
    uint8_t intEmbedSlicerNumber          = INT_EMBED_SLICER_2_BITS;
    uint8_t intEmbedSlicerPos             = INT_EMBED_SLICER_POS_MSB;
    uint8_t intEmbedSlicer                = INT_EMBED_SLICER_DISABLE;
    uint8_t intDataResolution             = INT_DATA_RESOLUTION_16BITS;
    uint8_t intDataFormat                 = INT_DATA_FORMAT_2S_COMPLEMENT;
    uint8_t slicerPinCtrlMode             = SLICER_PIN_CTRL_MODE_INTERNAL;
    uint8_t extSlicerStepSize             = 0U;
    uint8_t intSlicerStepSize             = 0U; 
    uint8_t intSlicerLsbOnQ               = 0U;
    
    adi_adrv904x_GpioPinSel_e intSlicerPins[ADI_ADRV904X_MAX_RX_INT_SLICER_BITS] = {  
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID
     };
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, slicerConfigSettings);
    
    /* Check if the requested rxChannel is a RX Channel. */
    if ((rxChannel == ADI_ADRV904X_RX0) ||
        (rxChannel == ADI_ADRV904X_RX1) ||
        (rxChannel == ADI_ADRV904X_RX2) ||
        (rxChannel == ADI_ADRV904X_RX3) ||
        (rxChannel == ADI_ADRV904X_RX4) ||
        (rxChannel == ADI_ADRV904X_RX5) ||
        (rxChannel == ADI_ADRV904X_RX6) ||
        (rxChannel == ADI_ADRV904X_RX7) )
    {
    
        /* Get Rx DDC Bitfield Address. Band0 only and assume all other bands are equivalent */
        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                            rxChannel,
                                                            ADI_ADRV904X_RX_DDC_BAND0,
                                                           &baseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
            return recoveryAction;
        }

        /* Get bitfield values */
        recoveryAction = adrv904x_RxDdc_IntDataFormat_BfGet(device,
                                                            NULL,
                                                            baseAddr,
                                                            &intDataFormat);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataFormat Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_IntDataResolution_BfGet(device,
                                                                NULL,
                                                                baseAddr,
                                                                &intDataResolution);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataResolution Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_IntEmbedSlicer_BfGet(device,
                                                             NULL,
                                                             baseAddr,
                                                             &intEmbedSlicer);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicer Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_IntEmbedSlicerPos_BfGet(device,
                                                                NULL,
                                                                baseAddr,
                                                                &intEmbedSlicerPos);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerPos Readback issue");
            return recoveryAction;
        }
    
        recoveryAction = adrv904x_RxDdc_IntEmbedSlicerNumber_BfGet(device,
                                                                   NULL,
                                                                   baseAddr,
                                                                   &intEmbedSlicerNumber);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerNumber Readback issue");
            return recoveryAction;
        }


        recoveryAction = adrv904x_RxDdc_IntParitySupport_BfGet(device,
                                                               NULL,
                                                               baseAddr,
                                                               &intParitySupport);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntParitySupport Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_IntEvenParity_BfGet(device,
                                                            NULL,
                                                            baseAddr,
                                                            &intEvenParity);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEvenParity Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_SlicerPinControlMode_BfGet(device,
                                                                   NULL,
                                                                   baseAddr,
                                                                   &slicerPinCtrlMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SlicerPinControlMode Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_ExternalSlicerPinControlStep_BfGet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                           &extSlicerStepSize);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ExternalSlicerPinControlStep Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_SlicerPinControlStep_BfGet(device,
                                                                   NULL,
                                                                   baseAddr,
                                                                   &intSlicerStepSize);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SlicerPinControlStep Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_RxDdc_IntSlicerLsbOnQ_BfGet(device,
                                                              NULL,
                                                              baseAddr,
                                                              &intSlicerLsbOnQ);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntSlicerLsbOnQ Readback issue");
            return recoveryAction;
        }
        
        
        /* Find GPIOs being used for Rx Internal Slicer bits */
        for(slicerBit = 0U ; slicerBit < ADI_ADRV904X_MAX_RX_INT_SLICER_BITS ; slicerBit++)
        {
            recoveryAction = adrv904x_GpioSignalFind(  device,
                                                      &intSlicerPins[slicerBit],
                                                       adrv904x_RxInternalSlicerOutputSignals[slicerBit],
                                                       (adi_adrv904x_Channels_e)rxChannel);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error finding previous GPIO that was routing Rx Internal Slicer Word bit.");
                return recoveryAction;
            }
        }
        
        /* Get chanIdx from the valid rxChannel enum */
        switch (rxChannel)
        {
        case ADI_ADRV904X_RX0: /* Fall through */
        case ADI_ADRV904X_RX1: /* Fall through */
        case ADI_ADRV904X_RX2: /* Fall through */
        case ADI_ADRV904X_RX3: /* Fall through */
        case ADI_ADRV904X_RX4: /* Fall through */
        case ADI_ADRV904X_RX5: /* Fall through */
        case ADI_ADRV904X_RX6: /* Fall through */
        case ADI_ADRV904X_RX7: /* Fall through */
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     rxChannel,
                                     "Invalid Rx Channel Requested for RxDataFormatIntegerGet");
            return recoveryAction;
        }
    
        /* Populate struct fields with values */
        
        /* Store internal slicer step size into intSlicerStepSize field of slicer config settings struct */
        slicerConfigSettings->intSlicerStepSize = (adi_adrv904x_IntSlicerStepSizes_e)intSlicerStepSize;
        
        /* Store external slicer step size in slicerConfigSettings struct */
        slicerConfigSettings->extSlicerStepSize = (adi_adrv904x_ExtSlicerStepSizes_e)extSlicerStepSize;
        
        /* Store internal slicer word GPIOs*/
        for (slicerBit = 0U; slicerBit < ADI_ADRV904X_MAX_RX_INT_SLICER_BITS; slicerBit++)
        {
            slicerConfigSettings->intSlicerGpioSelect[slicerBit] = intSlicerPins[slicerBit];
        }
        
        /* Store external slicer control GPIOs. Not supported so the results are static */
        slicerConfigSettings->extSlicerGpioSelect = ADI_ADRV904X_EXTSLICER_RX_GPIO_DISABLE;
    
    
        /* Encode embedded slicer config and store in integer config settings struct */
        recoveryAction = adrv904x_RxIntEmbSlicerConfigEncode(  device, 
                                                             rxChannel,
                                                             intEmbedSlicer,
                                                             intEmbedSlicerPos,
                                                             intEmbedSlicerNumber,
                                                             &integerConfigSettings->intEmbeddedBits);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer embedded slicer settings");
            return recoveryAction;
        }
    
        /* Encode parity config and store in integer config settings struct */
        recoveryAction = adrv904x_RxIntParitySupportEncode(  device, 
                                                           rxChannel,
                                                           intParitySupport,
                                                           intEvenParity,
                                                           &integerConfigSettings->intParity);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer parity settings");
            return recoveryAction;
        }
    
        /* Store embedded slicer position into the intEmbeddedPos field of integer config settings struct */
        integerConfigSettings->intEmbeddedPos = (adi_adrv904x_RxSlicerEmbeddedPos_e)intSlicerLsbOnQ;

        
    } /* end of Rx Channel if block */
        
    /* Check if the requested rxChannel is an ORx Channel. */
    else if((rxChannel == ADI_ADRV904X_ORX0) ||
            (rxChannel == ADI_ADRV904X_ORX1))
    {
    
        /* Get ORxDig base address for this channel */
        recoveryAction = adrv904x_OrxBitfieldAddressGet(  device,
                                                        rxChannel,
                                                        &orxDigBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve ORx Dig base address");
            return recoveryAction;
        }
    
        /* Get bitfield values */
        recoveryAction =  adrv904x_OrxDig_IntDataFormat_BfGet(device,
                                                              NULL,
                                                              orxDigBaseAddr,
                                                              &intDataFormat);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataFormat Readback issue");
            return recoveryAction;
        }
    
        recoveryAction =  adrv904x_OrxDig_IntDataResolution_BfGet(device,
                                                                  NULL,
                                                                  orxDigBaseAddr,
                                                                  &intDataResolution);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataResolution Readback issue");
            return recoveryAction;
        }
    
        /* set default values in unused fields for ORx channels */
        slicerConfigSettings->extSlicerStepSize = ADI_ADRV904X_EXTSLICER_STEPSIZE_1DB;
        slicerConfigSettings->intSlicerStepSize = ADI_ADRV904X_INTSLICER_STEPSIZE_1DB;
        integerConfigSettings->intEmbeddedBits = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS;
        integerConfigSettings->intParity = ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY;
        integerConfigSettings->intEmbeddedPos = ADI_ADRV904X_LOWER_NIBBLE_ON_I;

    } /* End of ORx channel if block */
    
    /* If rxChannel is not valid, throw an error */
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               rxChannel,
                               "Invalid Rx Channel Requested for RxDataFormatIntegerGet");
        return recoveryAction;
    }
    

    
    /* Encode the integer sample resolution and format and store in the integer config settings struct */
    /* Note: For both Rx and ORx (if ORx is eventually added here) */
    recoveryAction = adrv904x_RxIntSampleResFormatEncode(  device, 
                                                           intDataResolution,
                                                           intDataFormat,
                                                          &integerConfigSettings->intSampleResolution);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer sample resolution format");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatIntegerGet(adi_adrv904x_Device_t * const                    device,
                                                                   const adi_adrv904x_RxChannels_e                  rxChannel,
                                                                   adi_adrv904x_CddcIntegerConfigSettings_t * const cddcIntegerConfigSettings,
                                                                   adi_adrv904x_CddcSlicerConfigSettings_t * const  cddcSlicerConfigSettings)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT = 0U;
    static const uint8_t INT_DATA_RESOLUTION_16BITS    = 1U;
    static const uint8_t INT_EMBED_SLICER_DISABLE      = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB      = 0U;
    static const uint8_t INT_EMBED_SLICER_2_BITS       = 0U;
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE    = 0U;
    static const uint8_t INT_PARITY_EVEN               = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr      = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
    
    /* Preset local variables to default settings */
    uint8_t intEvenParity                 = INT_PARITY_EVEN;
    uint8_t intParitySupport              = INT_PARTIY_SUPPORT_DISABLE;
    uint8_t intEmbedSlicerNumber          = INT_EMBED_SLICER_2_BITS;
    uint8_t intEmbedSlicerPos             = INT_EMBED_SLICER_POS_MSB;
    uint8_t intEmbedSlicer                = INT_EMBED_SLICER_DISABLE;
    uint8_t intDataResolution             = INT_DATA_RESOLUTION_16BITS;
    uint8_t intDataFormat                 = INT_DATA_FORMAT_2S_COMPLEMENT;
    uint8_t intSlicerLsbOnQ               = 0U;
    uint8_t enSlicerAgcSync               = 0U;
    uint8_t en3BitSlicerMode              = 0U;
    uint8_t formatterCarrierSelect        = 0U;
    uint32_t slicerCarrierSelect          = 0U;
    
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcIntegerConfigSettings);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcSlicerConfigSettings);
    
    /* Check if the requested rxChannel is a RX Channel. */
    if ((rxChannel == ADI_ADRV904X_RX0) ||
        (rxChannel == ADI_ADRV904X_RX1) ||
        (rxChannel == ADI_ADRV904X_RX2) ||
        (rxChannel == ADI_ADRV904X_RX3) ||
        (rxChannel == ADI_ADRV904X_RX4) ||
        (rxChannel == ADI_ADRV904X_RX5) ||
        (rxChannel == ADI_ADRV904X_RX6) ||
        (rxChannel == ADI_ADRV904X_RX7) )
    {
    
        /* Get CDDC Bitfield Address */
        recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                              rxChannel,
                                                             &baseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC base address");
            return recoveryAction;
        }

        /* Get bitfield values */
        recoveryAction = adrv904x_CddcFuncs_FormatterIntDataFormat_BfGet(device,
                                                                         NULL,
                                                                         baseAddr,
                                                                        &intDataFormat);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataFormat Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterIntDataResolution_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &intDataResolution);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntDataResolution Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicer_BfGet(device,
                                                                          NULL,
                                                                          baseAddr,
                                                                         &intEmbedSlicer);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicer Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicerPos_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &intEmbedSlicerPos);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerPos Readback issue");
            return recoveryAction;
        }
    
        recoveryAction = adrv904x_CddcFuncs_FormatterIntEmbedSlicerNumber_BfGet(device,
                                                                                NULL,
                                                                                baseAddr,
                                                                               &intEmbedSlicerNumber);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEmbedSlicerNumber Readback issue");
            return recoveryAction;
        }


        recoveryAction = adrv904x_CddcFuncs_FormatterIntParitySupport_BfGet(device,
                                                                            NULL,
                                                                            baseAddr,
                                                                           &intParitySupport);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntParitySupport Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterIntEvenParity_BfGet(device,
                                                                         NULL,
                                                                         baseAddr,
                                                                        &intEvenParity);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntEvenParity Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterIntSlicerLsbOnQ_BfGet(device,
                                                                           NULL,
                                                                           baseAddr,
                                                                          &intSlicerLsbOnQ);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "IntSlicerLsbOnQ Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterDynamicSlicerAgcSyncEnable_BfGet(device,
                                                                                      NULL,
                                                                                      baseAddr,
                                                                                     &enSlicerAgcSync);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcEnSlicerAgcSync Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterStatic3bitSlicerModeEn_BfGet(device,
                                                                                  NULL,
                                                                                  baseAddr,
                                                                                 &en3BitSlicerMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcEn3BitSlicerMode Readback issue");
            return recoveryAction;
        }

        recoveryAction = adrv904x_CddcFuncs_FormatterCarrierSelect_BfGet(device,
                                                                         NULL,
                                                                         baseAddr,
                                                                        &formatterCarrierSelect);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcFormatterCarrierSelect Readback issue");
            return recoveryAction;
        }

        recoveryAction = adi_adrv904x_Register32Read(device,
                                                     NULL,
                                                     ((uint32_t) baseAddr + 0x163U),
                                                    &slicerCarrierSelect,
                                                     0xFFU);
        if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcSlicerCarrierSelect Readback issue");
            return recoveryAction;
        }

        /* Populate struct fields with values */
        /* Encode embedded slicer config and store in integer config settings struct */
        recoveryAction = adrv904x_CddcIntEmbSlicerConfigEncode(device,
                                                               rxChannel,
                                                               intEmbedSlicer,
                                                               intEmbedSlicerPos,
                                                               intEmbedSlicerNumber,
                                                              &cddcIntegerConfigSettings->intEmbeddedBits);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer embedded slicer settings");
            return recoveryAction;
        }
    
        /* Encode parity config and store in integer config settings struct */
        recoveryAction = adrv904x_CddcIntParitySupportEncode(device,
                                                             rxChannel,
                                                             intParitySupport,
                                                             intEvenParity,
                                                            &cddcIntegerConfigSettings->intParity);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer parity settings");
            return recoveryAction;
        }
    
        /* Store embedded slicer position into the intEmbeddedPos field of integer config settings struct */
        cddcIntegerConfigSettings->intEmbeddedPos = (adi_adrv904x_RxSlicerEmbeddedPos_e)intSlicerLsbOnQ;
        /* Store slicer config settings into struct */
        cddcSlicerConfigSettings->cddcSlicerCarrierSelect = (uint8_t)(slicerCarrierSelect);
        cddcSlicerConfigSettings->cddcFormatterCarrierSelect = formatterCarrierSelect;
        cddcSlicerConfigSettings->cddcEn3BitSlicerMode = en3BitSlicerMode;
        cddcSlicerConfigSettings->cddcEnSlicerAgcSync = enSlicerAgcSync;

    } /* end of Rx Channel if block */
    
    /* If rxChannel is not valid, throw an error */
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               rxChannel,
                               "Invalid Rx Channel Requested for RxDataFormatIntegerGet");
        return recoveryAction;
    }
    
    /* Encode the integer sample resolution and format and store in the integer config settings struct */
    recoveryAction = adrv904x_RxIntSampleResFormatEncode(device, 
                                                         intDataResolution,
                                                         intDataFormat,
                                                        &cddcIntegerConfigSettings->intSampleResolution);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to encode integer sample resolution format");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDataFormatEmbOvldMonitorGet(adi_adrv904x_Device_t* const                            device,
                                                                        const adi_adrv904x_RxChannels_e                         rxChannel,
                                                                        adi_adrv904x_EmbOverloadMonitorConfigSettings_t* const  embOvldConfigSettings)
{
    static const uint8_t LSB_POS_I_CHANNEL_ID           = 0U;
    static const uint8_t LSB_PLUS_ONE_POS_I_CHANNEL_ID  = 1U;
    static const uint8_t LSB_POS_Q_CHANNEL_ID           = 0U;
    static const uint8_t LSB_PLUS_ONE_POS_Q_CHANNEL_ID  = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction               = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e funcsAddr              = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxDdcChanAddr_e ddcAddr                  = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t apdHighSrcRegVal                            = 0U;
    uint8_t apdLowSrcRegVal                             = 0U;
    uint8_t hb2HighSrcRegVal                            = 0U;
    uint8_t hb2LowSrcRegVal                             = 0U;
    uint8_t invertHb2Flag                               = 0U;
    uint8_t invertApdFlag                               = 0U;
    uint8_t lsbIMonSrcRegVal                            = 0U;
    uint8_t lsbQMonSrcRegVal                            = 0U;
    uint8_t lsbPlusOneIMonSrcRegVal                     = 0U;
    uint8_t lsbPlusOneQMonSrcRegVal                     = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, embOvldConfigSettings);
    
    /* Check that the requested rxChannel is valid*/
    switch( rxChannel ) 
    {
        case ADI_ADRV904X_RX0:
        case ADI_ADRV904X_RX1:
        case ADI_ADRV904X_RX2:
        case ADI_ADRV904X_RX3:
        case ADI_ADRV904X_RX4:
        case ADI_ADRV904X_RX5:
        case ADI_ADRV904X_RX6:
        case ADI_ADRV904X_RX7:
        {
            /* Get Rx Funcs Bitfield Address */
            recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(  device,
                                                                  rxChannel,
                                                                 &funcsAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
                return recoveryAction;
            }
            
            /* Get Rx DDC Bitfield Address */
            recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                rxChannel,
                                                                ADI_ADRV904X_RX_DDC_BAND0,
                                                               &ddcAddr);                
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                return recoveryAction;
            }
            
            /* Get bitfield values */
            
            /* Get the APD High Threshold exceeded indicator source */
            recoveryAction =  adrv904x_RxFuncs_ApdHighSrcSelect_BfGet(device,
                                                                      NULL,
                                                                      funcsAddr,
                                                                      &apdHighSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "ApdHighSrcSelect readback issue");
                return recoveryAction;
            }
            
            /* Get the APD Low Threshold exceeded indicator source */
            recoveryAction =  adrv904x_RxFuncs_ApdLowSrcSelect_BfGet(device,
                                                                     NULL,
                                                                     funcsAddr,
                                                                     &apdLowSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "ApdLowSrcSelect readback issue");
                return recoveryAction;
            }
            
            /* Get the HB2 High Threshold exceeded indicator source */
            recoveryAction =  adrv904x_RxFuncs_Hb2HighSrcSelect_BfGet(device,
                                                                      NULL,
                                                                      funcsAddr,
                                                                      &hb2HighSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Hb2HighSrcSelect readback issue");
                return recoveryAction;
            }
            
            /* Get the HB2 Low Threshold exceeded indicator source */
            recoveryAction =  adrv904x_RxFuncs_Hb2LowSrcSelect_BfGet(device,
                                                                     NULL,
                                                                     funcsAddr,
                                                                     &hb2LowSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Hb2LowSrcSelect readback issue");
                return recoveryAction;
            }
            
            /* Get invert Hb2 flag config */
            recoveryAction =  adrv904x_RxFuncs_InvertHb2Low_BfGet(device,
                                                                  NULL,
                                                                  funcsAddr,
                                                                  &invertHb2Flag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading invert Hb2 Flag configuration");
                return recoveryAction;
            }
            
            /* Get invert Apd flag config */
            recoveryAction =  adrv904x_RxFuncs_InvertApdLow_BfGet(device,
                                                                  NULL,
                                                                  funcsAddr,
                                                                  &invertApdFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading invert Apd Flag configuration");
                return recoveryAction;
            }
            
            /* Get I Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv904x_RxDdc_RxMonFormatI_BfGet(device,
                                                               NULL,
                                                               ddcAddr,
                                                               LSB_POS_I_CHANNEL_ID,
                                                               &lsbIMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatI_0 readback issue");
                return recoveryAction;
            }
            
            /* Get I Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv904x_RxDdc_RxMonFormatI_BfGet(device,
                                                               NULL,
                                                               ddcAddr,
                                                               LSB_PLUS_ONE_POS_I_CHANNEL_ID,
                                                               &lsbPlusOneIMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatI_1 readback issue");
                return recoveryAction;
            }
            
            
            /* Get Q Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv904x_RxDdc_RxMonFormatQ_BfGet(device,
                                                               NULL,
                                                               ddcAddr,
                                                               LSB_POS_Q_CHANNEL_ID,
                                                               &lsbQMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatQ_0 readback issue");
                return recoveryAction;
            }
            
            /* Get Q Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv904x_RxDdc_RxMonFormatQ_BfGet(device,
                                                               NULL,
                                                               ddcAddr,
                                                               LSB_PLUS_ONE_POS_Q_CHANNEL_ID,
                                                               &lsbPlusOneQMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMonFormatQ_1 readback issue");
                return recoveryAction;
            }
            
            break;
        }
        
        case ADI_ADRV904X_ORX0:
        case ADI_ADRV904X_ORX1:
            /* Do nothing */
            break;
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Invalid Rx Channel Requested for RxDataFormatEmbOvldMonitorGet read");
            return recoveryAction;
    }
    
    /* Update the pointer to embOvldConfigSettings with data retrieved from the device */
    embOvldConfigSettings->embeddedMonitorApdHighSrcSel     = (adi_adrv904x_RxApdHighEmbeddedMonitorSrc_e)apdHighSrcRegVal;
    embOvldConfigSettings->embeddedMonitorApdLowSrcSel      = (adi_adrv904x_RxApdLowEmbeddedMonitorSrc_e)apdLowSrcRegVal;
    embOvldConfigSettings->embeddedMonitorHb2HighSrcSel     = (adi_adrv904x_RxHb2HighEmbeddedMonitorSrc_e)hb2HighSrcRegVal;
    embOvldConfigSettings->embeddedMonitorHb2LowSrcSel      = (adi_adrv904x_RxHb2LowEmbeddedMonitorSrc_e)hb2LowSrcRegVal;
    embOvldConfigSettings->invertHb2Flag                    = invertHb2Flag;
    embOvldConfigSettings->invertApdFlag                    = invertApdFlag;
    embOvldConfigSettings->embeddedMonitorSrcLsbI           = (adi_adrv904x_RxEmbeddedMonitorSrc_e)lsbIMonSrcRegVal;
    embOvldConfigSettings->embeddedMonitorSrcLsbQ           = (adi_adrv904x_RxEmbeddedMonitorSrc_e)lsbQMonSrcRegVal;
    embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI    = (adi_adrv904x_RxEmbeddedMonitorSrc_e)lsbPlusOneIMonSrcRegVal;
    embOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ    = (adi_adrv904x_RxEmbeddedMonitorSrc_e)lsbPlusOneQMonSrcRegVal;
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDataFormatEmbOvldMonitorGet(adi_adrv904x_Device_t* const                                device,
                                                                          const adi_adrv904x_RxChannels_e                             rxChannel,
                                                                          adi_adrv904x_CddcEmbOverloadMonitorConfigSettings_t* const  cddcEmbOvldConfigSettings)
{    
    adi_adrv904x_ErrAction_e recoveryAction             = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCddcFuncsChanAddr_e baseAddr             = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
    uint8_t lsbIMonSrcRegVal                            = 0U;
    uint8_t lsbQMonSrcRegVal                            = 0U;
    uint8_t lsbPlusOneIMonSrcRegVal                     = 0U;
    uint8_t lsbPlusOneQMonSrcRegVal                     = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcEmbOvldConfigSettings);
    
    /* Check that the requested rxChannel is valid*/
    switch( rxChannel ) 
    {
        case ADI_ADRV904X_RX0:
        case ADI_ADRV904X_RX1:
        case ADI_ADRV904X_RX2:
        case ADI_ADRV904X_RX3:
        case ADI_ADRV904X_RX4:
        case ADI_ADRV904X_RX5:
        case ADI_ADRV904X_RX6:
        case ADI_ADRV904X_RX7:
        {

            /* Get CDDC Bitfield Address */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device,
                                                                  rxChannel,
                                                                 &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve CDDC base address");
                return recoveryAction;
            }
             
            /* I Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatI0_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &lsbIMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatI_0 Read issue");
                return recoveryAction;
            }

            /* I Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatI1_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &lsbPlusOneIMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatI_1 Read issue");
                return recoveryAction;
            }

            /* Q Sample LSB Posn Embedded overload indicator config */
            recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatQ0_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &lsbQMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatQ_0 Read issue");
                return recoveryAction;
            }
                
            /* Q Sample LSB + 1 Posn Embedded overload indicator config */
            recoveryAction = adrv904x_CddcFuncs_FormatterRxMonFormatQ1_BfGet(device,
                                                                             NULL,
                                                                             baseAddr,
                                                                            &lsbPlusOneQMonSrcRegVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CDDC MonFormatQ_1 Read issue");
                return recoveryAction;
            }
          
            break;
        }
        
        case ADI_ADRV904X_ORX0:
        case ADI_ADRV904X_ORX1:
            /* Do nothing */
            break;
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Invalid Rx Channel Requested for RxDataFormatEmbOvldMonitorGet read");
            return recoveryAction;
    }
    
    /* Update the pointer to embOvldConfigSettings with data retrieved from the device */
    cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbI           = lsbIMonSrcRegVal;
    cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbQ           = lsbQMonSrcRegVal;
    cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbPlusOneI    = lsbPlusOneIMonSrcRegVal;
    cddcEmbOvldConfigSettings->embeddedMonitorSrcLsbPlusOneQ    = lsbPlusOneQMonSrcRegVal;
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainCompExtLnaGet(adi_adrv904x_Device_t * const   device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              uint8_t * const                 externalLnaGain)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t tmpRead                       = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, externalLnaGain);
    
    switch (rxChannel)
    {
        /* Check if the requested rxChannel is a valid Rx Channel */
        case ADI_ADRV904X_RX0:
        case ADI_ADRV904X_RX1:
        case ADI_ADRV904X_RX2:
        case ADI_ADRV904X_RX3:
        case ADI_ADRV904X_RX4:
        case ADI_ADRV904X_RX5:
        case ADI_ADRV904X_RX6:
        case ADI_ADRV904X_RX7:
        {
            /* Get Rx DDC Bitfield Address. Band0 only and assume all other bands are equivalent */
            recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                rxChannel,
                                                                ADI_ADRV904X_RX_DDC_BAND0,
                                                               &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                return recoveryAction;
            }
            
            /* Get bitfield */
            recoveryAction = adrv904x_RxDdc_GainCompForExtGain_BfGet(device,
                                                                     NULL,
                                                                     baseAddr,
                                                                     &tmpRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompForExtGain Read issue");
                return recoveryAction;
            }
            
            /* Set return value based on bitfield readback value */
            *externalLnaGain = tmpRead ? ADI_ENABLE : ADI_DISABLE;
            
            break;
        }
        
        /* Check if the requested rxChannel is a valid ORx Channel */
        case ADI_ADRV904X_ORX0:
        case ADI_ADRV904X_ORX1:
            /* Set return value to ADI_DISABLE */
            *externalLnaGain = ADI_DISABLE;
            break;
        
        /* Otherwise throw an error */
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannel,
                                    "Invalid Rx Channel Requested for RxGainCompExtLnaGet");
            return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainCompTempEnableGet(adi_adrv904x_Device_t * const   device,
                                                                  const adi_adrv904x_RxChannels_e rxChannel,
                                                                  uint8_t * const                 tempCompensationEnable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e baseAddr   = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t tmpRead                       = 0U;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, tempCompensationEnable);
    
    switch (rxChannel)
        {
            /* Check if the requested rxChannel is a valid Rx Channel */
            case ADI_ADRV904X_RX0:
            case ADI_ADRV904X_RX1:
            case ADI_ADRV904X_RX2:
            case ADI_ADRV904X_RX3:
            case ADI_ADRV904X_RX4:
            case ADI_ADRV904X_RX5:
            case ADI_ADRV904X_RX6:
            case ADI_ADRV904X_RX7:
            {
                /* Get Rx DDC Bitfield Address. Band0 only and assume all other bands are equivalent */
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(  device, 
                                                                    rxChannel,
                                                                    ADI_ADRV904X_RX_DDC_BAND0,
                                                                   &baseAddr);                
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx DDC base address");
                    return recoveryAction;
                }
                
                /* Get bitfield */
                recoveryAction = adrv904x_RxDdc_GainCompForTempGain_BfGet(device,
                                                                          NULL,
                                                                          baseAddr,
                                                                          &tmpRead);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GainCompForTempGain Read issue");
                    return recoveryAction;
                }
                
                /* Set return value based on bitfield readback value */
                *tempCompensationEnable = tmpRead ? ADI_ENABLE : ADI_DISABLE;
                
                break;
            }
            
            /* Check if the requested rxChannel is a valid ORx Channel */
            case ADI_ADRV904X_ORX0:
            case ADI_ADRV904X_ORX1:
                /* Set return value to ADI_DISABLE */
                *tempCompensationEnable = ADI_DISABLE;
                break;
            
            /* Otherwise throw an error */
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        rxChannel,
                                        "Invalid Rx Channel Requested for RxGainCompTempEnableGet");
                return recoveryAction;
        }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxInternalSlicerGpioEnableGet(adi_adrv904x_Device_t * const            device,
                                                                        const adi_adrv904x_RxChannels_e          rxChannel,
                                                                        adi_adrv904x_RxDataFormatModes_e * const intSlicerFormatSel,
                                                                        uint8_t * const                          enableStatus)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t slicerBit = 0U;
    adi_adrv904x_GpioPinSel_e foundPins[ADI_ADRV904X_MAX_RX_INT_SLICER_BITS] = { 
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID,
        ADI_ADRV904X_GPIO_INVALID
    };
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intSlicerFormatSel);

    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxInternalSlicerGpioEnableGet read");
        return recoveryAction;
    }
    
    
    /* Search for each slicerBit signal for this channel on any GPIO */
    for (slicerBit = 0U; slicerBit < ADI_ADRV904X_MAX_RX_INT_SLICER_BITS; slicerBit++)
    {
        recoveryAction = adrv904x_GpioSignalFind(  device,
                                                  &foundPins[slicerBit],
                                                   adrv904x_RxInternalSlicerOutputSignals[slicerBit],
                                                   (adi_adrv904x_Channels_e)rxChannel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error finding previous GPIO that was routing Rx Internal Slicer Word bit.");
            return recoveryAction;
        }
    }
    
    /* If bits[3:0] found, currently in 4pin mode */
    if ((foundPins[3U] != ADI_ADRV904X_GPIO_INVALID) &&
        (foundPins[2U] != ADI_ADRV904X_GPIO_INVALID) &&
        (foundPins[1U] != ADI_ADRV904X_GPIO_INVALID) &&
        (foundPins[0U] != ADI_ADRV904X_GPIO_INVALID))
    {
        *intSlicerFormatSel = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_4PIN_MODE;
        if (enableStatus != NULL) 
        {
            *enableStatus = ADI_ENABLE;
        }
    }
    
    /* Else If bits[2:0] found, currently in 3pin mode */
    else if ((foundPins[2U] != ADI_ADRV904X_GPIO_INVALID) &&
             (foundPins[1U] != ADI_ADRV904X_GPIO_INVALID) &&
             (foundPins[0U] != ADI_ADRV904X_GPIO_INVALID))
    {
        *intSlicerFormatSel = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_3PIN_MODE;
        if (enableStatus != NULL) 
        {
            *enableStatus = ADI_ENABLE;
        }
    }
    
    /* Else If bits[1:0] found, currently in 2pin mode */
    else if((foundPins[1U] != ADI_ADRV904X_GPIO_INVALID) &&
            (foundPins[0U] != ADI_ADRV904X_GPIO_INVALID))
    {
        *intSlicerFormatSel = ADI_ADRV904X_GAIN_WITH_INTERNAL_SLICER_2PIN_MODE;
        if (enableStatus != NULL) 
        {
            *enableStatus = ADI_ENABLE;
        }
    }
    
    /* Else, currently GAIN COMP DISABLED mode*/
    else
    {
        *intSlicerFormatSel = ADI_ADRV904X_GAIN_COMPENSATION_DISABLED;
        if (enableStatus != NULL) 
        {
            *enableStatus = ADI_DISABLE;
        }
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntEmbSlicerConfigEncode(adi_adrv904x_Device_t * const               device,
                                                                     const adi_adrv904x_RxChannels_e             rxChannel,
                                                                     const uint8_t                               intEmbedSlicer,
                                                                     const uint8_t                               intEmbedSlicerPos,
                                                                     const uint8_t                               intEmbedSlicerNumber,
                                                                     adi_adrv904x_RxSlicerEmbeddedBits_e * const integerConfigSettingsEmbSlicer)
{
    static const uint8_t INT_EMBED_SLICER_DISABLE = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_LSB = 1U;
    static const uint8_t INT_EMBED_SLICER_2_BITS  = 0U;
    static const uint8_t INT_EMBED_SLICER_1_BIT   = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettingsEmbSlicer);
    
    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxIntEmbSlicerConfigEncode");
        return recoveryAction;
    }
    
    
    if (intEmbedSlicer == INT_EMBED_SLICER_DISABLE)
    {
        *integerConfigSettingsEmbSlicer = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS;
    }
    else
    {
        if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
                 (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
                 (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            if ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel)
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER;
            }
            else
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_4_BIT_SLICER;
            }            
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
                 (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            if ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel)
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER;
            }
            else
            {
                *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_4_BIT_SLICER;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     intEmbedSlicer,
                                     "Invalid combination of embed slicer,pos,number combination encountered while attempting to encode integer embed slicer settings");
            return recoveryAction;
        }
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcIntEmbSlicerConfigEncode(adi_adrv904x_Device_t* const               device,
    const adi_adrv904x_RxChannels_e             rxChannel,
    const uint8_t                               intEmbedSlicer,
    const uint8_t                               intEmbedSlicerPos,
    const uint8_t                               intEmbedSlicerNumber,
    adi_adrv904x_RxSlicerEmbeddedBits_e* const integerConfigSettingsEmbSlicer)
{
    static const uint8_t INT_EMBED_SLICER_DISABLE = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_MSB = 0U;
    static const uint8_t INT_EMBED_SLICER_POS_LSB = 1U;
    static const uint8_t INT_EMBED_SLICER_2_BITS = 0U;
    static const uint8_t INT_EMBED_SLICER_1_BIT = 1U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerConfigSettingsEmbSlicer);

    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxChannel,
            "Invalid Rx Channel Requested for RxIntEmbSlicerConfigEncode");
        return recoveryAction;
    }


    if (intEmbedSlicer == INT_EMBED_SLICER_DISABLE)
    {
        *integerConfigSettingsEmbSlicer = ADI_ADRV904X_NO_EMBEDDED_SLICER_BITS;
    }
    else
    {
        if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_1_SLICERBIT_AT_LSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_1_BIT))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_1_SLICERBIT_AT_MSB;
        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_LSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_LSB_3_BIT_SLICER;

        }
        else if ((intEmbedSlicerPos == INT_EMBED_SLICER_POS_MSB) &&
            (intEmbedSlicerNumber == INT_EMBED_SLICER_2_BITS))
        {
            *integerConfigSettingsEmbSlicer = ADI_ADRV904X_EMBED_2_SLICERBITS_AT_MSB_3_BIT_SLICER;
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                intEmbedSlicer,
                "Invalid combination of embed slicer,pos,number combination encountered while attempting to encode integer embed slicer settings");
            return recoveryAction;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntSampleResFormatEncode(adi_adrv904x_Device_t * const                device,
                                                                     const uint8_t                                intDataResolution,
                                                                     const uint8_t                                intDataFormat,
                                                                     adi_adrv904x_RxIntSampleResolution_e * const intSampleResolution)
{
    static const uint8_t INT_DATA_FORMAT_2S_COMPLEMENT    = 0U;
    static const uint8_t INT_DATA_FORMAT_SIGNED_MAGNITUDE = 1U;
    static const uint8_t INT_DATA_RESOLUTION_12BITS       = 0U;
    static const uint8_t INT_DATA_RESOLUTION_16BITS       = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, intSampleResolution);

    /* Map int resolution and format to intSampleResolution enum value */
    if ((intDataResolution == INT_DATA_RESOLUTION_12BITS) &&
        (intDataFormat == INT_DATA_FORMAT_2S_COMPLEMENT))
    {
        *intSampleResolution = ADI_ADRV904X_INTEGER_12BIT_2SCOMP;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_12BITS) &&
             (intDataFormat == INT_DATA_FORMAT_SIGNED_MAGNITUDE))
    {
        *intSampleResolution = ADI_ADRV904X_INTEGER_12BIT_SIGNED;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_16BITS) &&
             (intDataFormat == INT_DATA_FORMAT_2S_COMPLEMENT))
    {
        *intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_2SCOMP;
    }
    else if ((intDataResolution == INT_DATA_RESOLUTION_16BITS) &&
             (intDataFormat == INT_DATA_FORMAT_SIGNED_MAGNITUDE))
    {
        *intSampleResolution = ADI_ADRV904X_INTEGER_16BIT_SIGNED;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               intSampleResolution,
                               "Unexpected resolution and format values encountered while attempting to encode integer sample resolution settings");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxIntParitySupportEncode(adi_adrv904x_Device_t * const      device,
                                                                   const adi_adrv904x_RxChannels_e    rxChannel,
                                                                   const uint8_t                      intParitySupport,
                                                                   const uint8_t                      intEvenParity,
                                                                   adi_adrv904x_RxIntParity_e * const integerParity)
{
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE = 0U;
    static const uint8_t INT_PARTIY_SUPPORT_ENABLE  = 1U;
    static const uint8_t INT_PARITY_EVEN            = 0U;
    static const uint8_t INT_PARITY_ODD             = 1U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerParity);
    
    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxIntParitySupportEncode");
        return recoveryAction;
    }
    
    /* Check inputs intParitySupport and intEventParity for valid values */
    if ((intParitySupport != INT_PARTIY_SUPPORT_DISABLE) &&
        (intParitySupport != INT_PARTIY_SUPPORT_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction, 
                                 intParitySupport,
                                 "Invalid intParitySupport value encountered while attempting to encode integer parity settings");
        return recoveryAction;
    }
    
    if ((intEvenParity != INT_PARITY_EVEN) &&
        (intEvenParity != INT_PARITY_ODD))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction, 
                                 intEvenParity,
                                 "Invalid intEvenParity value encountered while attempting to encode integer parity settings");
        return recoveryAction;
    }
    
    /* Map int parity support and even/odd to integerParity enum value */
    if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
        (intEvenParity == INT_PARITY_ODD) &&
        ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel))
    {
        *integerParity = ADI_ADRV904X_3BIT_SLICER_ODD_PARITY;
    }
    else if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
             (intEvenParity == INT_PARITY_EVEN) &&
             ((device->devStateInfo.rxChannel3bitSlicerMode & (uint8_t)rxChannel) == (uint8_t)rxChannel))
    {
        *integerParity = ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY;
    }
    else
    {
        *integerParity = ADI_ADRV904X_NO_PARITY;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcIntParitySupportEncode(adi_adrv904x_Device_t* const      device,
    const adi_adrv904x_RxChannels_e    rxChannel,
    const uint8_t                      intParitySupport,
    const uint8_t                      intEvenParity,
    adi_adrv904x_RxIntParity_e* const integerParity)
{
    static const uint8_t INT_PARTIY_SUPPORT_DISABLE = 0U;
    static const uint8_t INT_PARTIY_SUPPORT_ENABLE = 1U;
    static const uint8_t INT_PARITY_EVEN = 0U;
    static const uint8_t INT_PARITY_ODD = 1U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, integerParity);

    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            rxChannel,
            "Invalid Rx Channel Requested for RxIntParitySupportEncode");
        return recoveryAction;
    }

    /* Check inputs intParitySupport and intEventParity for valid values */
    if ((intParitySupport != INT_PARTIY_SUPPORT_DISABLE) &&
        (intParitySupport != INT_PARTIY_SUPPORT_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            intParitySupport,
            "Invalid intParitySupport value encountered while attempting to encode integer parity settings");
        return recoveryAction;
    }

    if ((intEvenParity != INT_PARITY_EVEN) &&
        (intEvenParity != INT_PARITY_ODD))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            intEvenParity,
            "Invalid intEvenParity value encountered while attempting to encode integer parity settings");
        return recoveryAction;
    }

    /* Map int parity support and even/odd to integerParity enum value */
    if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
        (intEvenParity == INT_PARITY_ODD))

    {
        *integerParity = ADI_ADRV904X_3BIT_SLICER_ODD_PARITY;
    }
    else if ((intParitySupport == INT_PARTIY_SUPPORT_ENABLE) &&
        (intEvenParity == INT_PARITY_EVEN))
    {
        *integerParity = ADI_ADRV904X_3BIT_SLICER_EVEN_PARITY;
    }
    else
    {
        *integerParity = ADI_ADRV904X_NO_PARITY;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxLoSourceGet(adi_adrv904x_Device_t* const device,
                                                        const adi_adrv904x_RxChannels_e rxChannel,
                                                        adi_adrv904x_LoSel_e* const rxLoSource)
{
    static const uint8_t RX_LO_CURRENT_MASK = 0x01U;
    uint8_t rxtxLoMuxReg   = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxLoSource);

    if ((rxChannel == ADI_ADRV904X_RX0) ||
        (rxChannel == ADI_ADRV904X_RX1) ||
        (rxChannel == ADI_ADRV904X_RX2) ||
        (rxChannel == ADI_ADRV904X_RX3))
    {
        /*Read the Rx Lo Mux reg from the east side of the chip */
        recoveryAction = adrv904x_PllMemMap_SelRxLo_BfGet(device,
                                                          NULL,
                                                          ADRV904X_BF_DIGITAL_CORE_EAST_RFPLL,
                                                          &rxtxLoMuxReg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to retrieve Rx Lo Selection.");
            return recoveryAction;
        }
        
        /*For East Side, 1 = LO0, 0 = LO1*/
        if ((rxtxLoMuxReg & RX_LO_CURRENT_MASK) == RX_LO_CURRENT_MASK)
        {
            *rxLoSource = ADI_ADRV904X_LOSEL_LO0;
        }
        else
        {
            *rxLoSource = ADI_ADRV904X_LOSEL_LO1;
        }
    }
    else if ((rxChannel == ADI_ADRV904X_RX4) ||
        (rxChannel == ADI_ADRV904X_RX5) ||
        (rxChannel == ADI_ADRV904X_RX6) ||
        (rxChannel == ADI_ADRV904X_RX7))
    {
        /*Read the Rx Lo Mux reg from the west side of the chip */
        recoveryAction = adrv904x_PllMemMap_SelRxLo_BfGet(device,
                                                          NULL,
                                                          ADRV904X_BF_DIGITAL_CORE_WEST_RFPLL,
                                                          &rxtxLoMuxReg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "PllMemMap_SelRxLo_Bfget issue");
            return recoveryAction;
        }
        
        /*For West Side, 1 = LO1, 0 = LO0*/
        if ((rxtxLoMuxReg & RX_LO_CURRENT_MASK) == RX_LO_CURRENT_MASK)
        {
            *rxLoSource = ADI_ADRV904X_LOSEL_LO1;
        }
        else
        {
            *rxLoSource = ADI_ADRV904X_LOSEL_LO0;
        }
        
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, rxChannel, "Invalid Rx Channel parameter. Valid Rx channel must be Rx0-Rx7");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxMinMaxGainIndexSetRangeCheck(adi_adrv904x_Device_t* const    device,
                                                                         const uint32_t                  rxChannelMask,
                                                                         const uint8_t                   minGainIndex,
                                                                         const uint8_t                   maxGainIndex)
{
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
        /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Check that rxChannelMask is valid*/
    if (rxChannelMask == ADI_ADRV904X_RXOFF)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx0-Rx7");
        return recoveryAction;
    }
    
    if (rxChannelMask > ADI_ADRV904X_RX_MASK_ALL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx0-Rx7");
        return recoveryAction;
    }

    /*Check that requested min gain index does not exceed max gain index*/
    if (minGainIndex >= maxGainIndex)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                minGainIndex,
                                "minGainIndex should be less than maxGainIndex");
        return recoveryAction;
    }

    /*Check that requested min gain index is not less than the absolute min gain index possible for this device*/
#if ADI_ADRV904X_MIN_GAIN_TABLE_INDEX > 0
    if(minGainIndex < ADI_ADRV904X_MIN_GAIN_TABLE_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                minGainIndex,
                                "minGainIndex should be greater than or equal to ADI_ADRV904X_MIN_GAIN_TABLE_INDEX");
        return recoveryAction;
    }
#endif
    /*Check that requested max gain index is not greater than the absolute max gain index possible for this device*/
#if ADI_ADRV904X_MAX_GAIN_TABLE_INDEX < 255
    if(maxGainIndex > ADI_ADRV904X_MAX_GAIN_TABLE_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                maxGainIndex,
                                "maxGainIndex should be less than or equal to ADI_ADRV904X_MAX_GAIN_TABLE_INDEX");
        return recoveryAction;
    }
#endif
    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainTableWriteRangeCheck(adi_adrv904x_Device_t* const        device,
                                                                     const uint32_t                      rxChannelMask,
                                                                     const uint8_t                       gainIndexOffset,
                                                                     const adi_adrv904x_RxGainTableRow_t gainTableRow[],
                                                                     const uint32_t                      arraySize)
{


    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    const uint8_t MAX_EXT_CTRL_WORD = 3U;
    const int16_t MIN_DIG_GAIN = -360; /*Dig gain is in the range -18dB to 50db*/
    const int16_t MAX_DIG_GAIN = 1000; /*Dig gain is in the range -18dB to 50db*/

    uint32_t  gainIndex = 0U;
    
        /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gainTableRow);


    /*Check the no. of gain indices parameter is not 0*/
    if (arraySize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                arraySize,
                                "arraySize parameter is 0. Valid values from 1 - 256");
        return recoveryAction;
    }

    /*Check that the gain index offset is within range*/
#if ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX > 0
    if (gainIndexOffset < ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
                               "Invalid gainIndexOffset. Parameter gainIndexOffset should be greater than ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX");
        return recoveryAction;
    }
#endif
#if ADI_ADRV904X_START_RX_GAIN_INDEX < 255
    if (gainIndexOffset > ADI_ADRV904X_START_RX_GAIN_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
        "Invalid gainIndexOffset. Parameter gainIndexOffset should be smaller than ADI_ADRV904X_START_RX_GAIN_INDEX");
        return recoveryAction;
    }
#endif

    /*Check the no. of gain indices parameter is valid*/
    if (arraySize > (uint32_t)((gainIndexOffset - ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX) + 1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                arraySize,
                                "gainTableRow arraySize exceeds the limit. Valid range ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX to gainIndexOffset");
        return recoveryAction;
    }

    /*Check that rxChannelMask is valid*/
    
    if ((rxChannelMask == ADI_ADRV904X_RXOFF) || (rxChannelMask > ADI_ADRV904X_RX_MASK_ALL))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx Channel mask parameter. Valid Rx channel masks include Rx0-Rx7");
        return recoveryAction;
    }

    /*Check that if Rx gain table is required to be loaded, Rx Profile is valid*/
    if  ((rxChannelMask & ADI_ADRV904X_RX_MASK_ALL) > 0)
    {
        if ((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) == 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    rxChannelMask,
                                    "Rx gain table selected for programming but Rx profile is invalid in device structure");
            return recoveryAction;
        }
    }

    /*Range check individual gain table row entries*/
    for (gainIndex = 0; gainIndex < arraySize; gainIndex++)
    {
        /*Check that EXT_CTRL is in the range {0,1,2,3}*/
        if (gainTableRow[gainIndex].extControl > MAX_EXT_CTRL_WORD)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    gainTableRow[gainIndex].extControl,
                                    "Rx Gain Table extControl parameter exceeds the limit. Allowed values are 0,1,2,3");
            return recoveryAction;
        }

        /*Check that digital gain is in the range -18dB to 50dB*/
        if ((gainTableRow[gainIndex].digGain < MIN_DIG_GAIN) ||
            (gainTableRow[gainIndex].digGain > MAX_DIG_GAIN))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    gainTableRow[gainIndex].digGain,
                                    "Rx Gain Table digGain parameter exceeds the limit. Allowed values between -18dB to 50dB(-360 to +1000)");
            return recoveryAction;
        }
    }

    return ADI_ADRV904X_ERR_ACT_NONE;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxGainTableReadRangeCheck(adi_adrv904x_Device_t* const        device,
                                                                    const adi_adrv904x_RxChannels_e     rxChannel,
                                                                    const uint8_t                       gainIndexOffset,
                                                                    const adi_adrv904x_RxGainTableRow_t gainTableRow[],
                                                                    const uint32_t                      arraySize)
{


    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gainTableRow);


    /*Check the no. of gain indices parameter is not 0*/
    if (arraySize == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                arraySize,
                                "arraySize parameter is 0. Valid values from 1 - 256");
        return recoveryAction;
    }

    /*Check that the gain index offset is within range*/
#if ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX > 0
    if (gainIndexOffset < ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
                               "Invalid gainIndexOffset. Parameter gainIndexOffset should be greater than ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX");
        return recoveryAction;
    }
#endif
#if ADI_ADRV904X_START_RX_GAIN_INDEX < 255
    if (gainIndexOffset > ADI_ADRV904X_START_RX_GAIN_INDEX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gainIndexOffset,
                               "Invalid gainIndexOffset. Parameter gainIndexOffset should be smaller than ADI_ADRV904X_START_RX_GAIN_INDEX");
        return recoveryAction;
    }
#endif

    /*Check the no. of gain indices parameter is valid*/
    if (arraySize > (uint32_t)((gainIndexOffset - ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX) + 1U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                arraySize,
                                "gainTableRow arraySize exceeds the limit. Valid range ADI_ADRV904X_MIN_RX_GAIN_TABLE_INDEX to gainIndexOffset");
        return recoveryAction;
    }

    /*Check that the requested rxChannel is valid*/
    if ((rxChannel != ADI_ADRV904X_RX0)  &&
        (rxChannel != ADI_ADRV904X_RX1)  &&
        (rxChannel != ADI_ADRV904X_RX2)  &&
        (rxChannel != ADI_ADRV904X_RX3)  &&
        (rxChannel != ADI_ADRV904X_RX4)  &&
        (rxChannel != ADI_ADRV904X_RX5)  &&
        (rxChannel != ADI_ADRV904X_RX6)  &&
        (rxChannel != ADI_ADRV904X_RX7)  )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested.Valid Rx channels include Rx0-Rx7");
        return recoveryAction;
    }
    
    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_DecPowerCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_RxDecimatedPowerCfg_t* const decPowerCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, decPowerCfg);

    static const uint8_t MAX_POWER_INPUT_SELECT = 3U;
    static const uint8_t MAX_MEASUREMENT_DURATION = 31U;
    uint32_t ALL_DEC_POWER_SELECT = (uint32_t)ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK |
                                    (uint32_t)ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK |
                                    (uint32_t)ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK;

    if (((decPowerCfg->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || (decPowerCfg->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if ((decPowerCfg->decPowerControl != ADI_ADRV904X_DEC_POWER_MEAS_OFF) &&
        (decPowerCfg->decPowerControl != ADI_ADRV904X_DEC_POWER_MEAS_ON) &&
        (decPowerCfg->decPowerControl != ADI_ADRV904X_DEC_POWER_AGC_MEAS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->decPowerControl,
                               "Invalid decPowerControl selection.");
        return recoveryAction;
    }
    
    if (decPowerCfg->powerInputSelect > MAX_POWER_INPUT_SELECT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerInputSelect,
                               "Invalid powerInputSelect selection.");
        return recoveryAction;
    }

    if (((decPowerCfg->measBlockSelectMask & (~(uint8_t)ALL_DEC_POWER_SELECT)) != 0U) ||
         (decPowerCfg->measBlockSelectMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->measBlockSelectMask,
                               "Invalid measBlockSelectMask is selected.");
        return recoveryAction;
    }
    
    if (decPowerCfg->powerMeasurementDuration > MAX_MEASUREMENT_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerMeasurementDuration,
                               "Invalid powerMeasurementDuration selection.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_OrxDecPowerCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_ORxDecimatedPowerCfg_t* const decPowerCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, decPowerCfg);

    static const uint8_t ENABLE_MAX = 1U;
    static const uint8_t MAX_POWER_INPUT_SELECT = 2U;
    static const uint8_t MAX_MEASUREMENT_DURATION = 21U;

    if (((decPowerCfg->orxChannelMask & (~(uint32_t)ADI_ADRV904X_ORX_MASK_ALL)) != 0U) || (decPowerCfg->orxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->orxChannelMask,
                               "Invalid ORx channel is selected. Valid values are any combinations of ORx0/1");
        return recoveryAction;
    }
    
    if (decPowerCfg->measurementEnable > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->measurementEnable,
                               "Invalid measurementEnable selection.");
        return recoveryAction;
    }
    
    if (decPowerCfg->powerInputSelect > MAX_POWER_INPUT_SELECT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerInputSelect,
                               "Invalid powerInputSelect selection.");
        return recoveryAction;
    }

    if (decPowerCfg->powerMeasurementDuration > MAX_MEASUREMENT_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerMeasurementDuration,
                               "Invalid powerMeasurementDuration selection.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxHb2OverloadCfgSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              uint8_t enableHb2Overload,
                                                              uint8_t hb2OverloadSignalSelection,
                                                              uint8_t hb2OverloadPowerMode)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint8_t MAX_HB2_CFG_INPUT = 1U;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    static const uint64_t HB2_OVERLOAD_USE_RIN = 0x90; 
    static const uint32_t HB2_OVERLOAD_USE_RIN_MASK = 1U << 24;
    static const uint32_t HB2_OVERLOAD_USE_RIN_DISABLE = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        return recoveryAction;
    }
    
    if (enableHb2Overload > MAX_HB2_CFG_INPUT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enableHb2Overload,
                               "Invalid enableHb2Overload selection.");
        return recoveryAction;
    }
    
	if (hb2OverloadSignalSelection >= (uint8_t) ADI_ADRV904X_MAX_SIG_MON_SRC_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hb2OverloadSignalSelection,
                               "Invalid hb2OverloadSignalSelection selection.");
        return recoveryAction;
    }

    if (hb2OverloadPowerMode > MAX_HB2_CFG_INPUT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hb2OverloadPowerMode,
                               "Invalid hb2OverloadPowerMode selection.");
        return recoveryAction;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        return recoveryAction;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadEnAgc_BfSet(device, NULL, rxFuncsBaseAddr, enableHb2Overload);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write hb2 enable bit");
        return recoveryAction;
    }

	{
		recoveryAction = adrv904x_RxFuncs_Hb2OverloadUseHb2In_BfSet(device, NULL, rxFuncsBaseAddr, hb2OverloadSignalSelection);
		if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
		{
			ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write hb2 input selection bit");
			return recoveryAction;
		}		
		/* Set hb2_overload_use_rin to 0 */
		recoveryAction = adi_adrv904x_Register32Write(device,
				                                      NULL,
				                                      rxFuncsBaseAddr+HB2_OVERLOAD_USE_RIN,
				                                      HB2_OVERLOAD_USE_RIN_DISABLE,
				                                      HB2_OVERLOAD_USE_RIN_MASK);
		if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
		{
			ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write HB2 input selection bit");
			return recoveryAction;
		}
	}  
	
    recoveryAction = adrv904x_RxFuncs_OverloadPowerModeAgc_BfSet(device, NULL, rxFuncsBaseAddr, hb2OverloadPowerMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write hb2 power mode bit");
        return recoveryAction;
    }    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxHb2OverloadCfgGet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              uint8_t * const enableHb2Overload,
                                                              uint8_t * const hb2OverloadSignalSelection,
                                                              uint8_t * const hb2OverloadPowerMode)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    static const uint64_t overload_config_addr = 0x90;
    static const uint32_t HB2_OVERLOAD_USE_RIN_MASK = 1U << 24;
    uint32_t hb2_overload_use_rin = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, enableHb2Overload);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, hb2OverloadSignalSelection);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, hb2OverloadPowerMode);

    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        return recoveryAction;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        return recoveryAction;
    }

    recoveryAction = adrv904x_RxFuncs_OverloadEnAgc_BfGet(device, NULL, rxFuncsBaseAddr, enableHb2Overload);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 enable bit");
        return recoveryAction;
    }
    
    if ((device->devStateInfo.deviceSiRev & 0xF0U) >= 0xB0U)
    {
        /* Write the new bf set for Palao B0 (hb2_overload_use_rin) */
        recoveryAction = adi_adrv904x_Register32Read(device,
                                                    NULL,
                                                    rxFuncsBaseAddr + overload_config_addr,
                                                    &hb2_overload_use_rin,
                                                    HB2_OVERLOAD_USE_RIN_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2_overload_use_rin bit");
            return recoveryAction;
        }
        if (hb2_overload_use_rin == HB2_OVERLOAD_USE_RIN_MASK)
        {
            *hb2OverloadSignalSelection = 2U;
        }
        else
        {
            recoveryAction = adrv904x_RxFuncs_Hb2OverloadUseHb2In_BfGet(device, NULL, rxFuncsBaseAddr, hb2OverloadSignalSelection);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 input selection bit");
                return recoveryAction;
            }
        }
    }
    else
    {
        recoveryAction = adrv904x_RxFuncs_Hb2OverloadUseHb2In_BfGet(device, NULL, rxFuncsBaseAddr, hb2OverloadSignalSelection);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 input selection bit");
            return recoveryAction;
        }
    }

    recoveryAction = adrv904x_RxFuncs_OverloadPowerModeAgc_BfGet(device, NULL, rxFuncsBaseAddr, hb2OverloadPowerMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 power mode bit");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_ORxAttenDbToRegValues(adi_adrv904x_Device_t* const device,
                                                                const uint8_t attenDb,
                                                                uint8_t* const trmAtten,    
                                                                uint8_t* const trmAttenBwCapEn)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, trmAtten);
    
    /* Requested ORx atten to bitfield value mappings */
    /* TODO - yoda provides a mapping from attenDb to corresponding trmAtten bitfield value by way of the adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten_e enumeration. 
     * Unfortunately that mapping is currently incorrect. See TPGRADSWL-10152 and the 'Palau ORX Atten Users Guide'. */    

    /* NOTE: The FW has these LUTs too. So, any LUT changes must be made in both places. */
    const uint8_t orxAttenDbToTrmAtten[ADI_ADRV904X_MAX_ORX_ATTEN_DB + 1] = {0x00, 0x01, 0x02, 0x03,
                                                                             0x04, 0x05, 0x08, 0x09,
                                                                             0x0A, 0x0B, 0x0C, 0x0D,
                                                                             0x10, 0x11, 0x12, 0x13,
                                                                             0x14, 0x15};
    const uint8_t orxAttenDbToTrmAttenBwCapEn[ADI_ADRV904X_MAX_ORX_ATTEN_DB + 1] = {0x00, 0x00, 0x00, 0x00,
                                                                                    0x00, 0x00, 0x06, 0x06,
                                                                                    0x06, 0x06, 0x06, 0x06,
                                                                                    0x0F, 0x0F, 0x0F, 0x0F,
                                                                                    0x0F, 0x0F};

    if (attenDb > ADI_ADRV904X_MAX_ORX_ATTEN_DB)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            attenDb,
            "attenDb outside maximum range");
        return recoveryAction;
    }
    
    *trmAtten = orxAttenDbToTrmAtten[attenDb];
    *trmAttenBwCapEn = orxAttenDbToTrmAttenBwCapEn[attenDb];
    
    return ADI_ADRV904X_ERR_ACT_NONE;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_ORxTrmAttenToDb(adi_adrv904x_Device_t* const device,
                                                          const uint8_t trmAtten,
                                                          uint8_t* const attenDb)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, attenDb);

    /* TODO - yoda provides a mapping from attenDb to corresponding trmAtten bitfield value by way of the
     * adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten_e enumeration. Unfortunately that mapping is currently incorrect. See
     * TPGRADSWL-10152 and the 'Palau ORX Atten Users Guide'.
     */
        
    /* 
     * TrmAtten ->  orxAttenDb
     * ----------+------------
     * hex  |dec |  dB
     * ----------|------------
     *  0x00  0  |  0
     *  0x01  1  |  1
     *  0x02  2  |  2
     *  0x03  3  |  3
     *  0x04  4  |  4
     *  0x05  5  |  5 
     *  0x06  6  |  5 #
     *  0x07  7  |  6 #
     *  0x08  8  |  6
     *  0x09  9  |  7
     *  0x0A 10  |  8
     *  0x0B 11  |  9
     *  0x0C 12  |  10
     *  0x0D 13  |  11
     *  0x0E 14  |  11 #
     *  0x0F 15  |  12 #
     *  0x10 16  |  12
     *  0x11 17  |  13 
     *  0x12 18  |  14
     *  0x13 19  |  15
     *  0x14 20  |  16
     *  0x15 21  |  17
     * '#' indicates a value interpolated from the TrmAtten <- orxAttenDb mapping to avoid confusing holes in the
     * TrmAtten -> orxAttenDb mapping.
     */
    
    /* Resolve bitfield value to result in dB. Return an error if there is no definition of a conversion to dB from the value */
    if (trmAtten <= 5U)
    {
        *attenDb = trmAtten;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else if ((8U <= trmAtten) &&
        (trmAtten <= 13U))
    {
        *attenDb = trmAtten - 2U;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else if ((16U <= trmAtten) &&
        (trmAtten <= 21U))
    {
        *attenDb = trmAtten - 4U;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 attenDb,
                                 "Invalid value for trmAtten. Cannot resolve to atten in dB.");
        return recoveryAction;    
    }
    
    return recoveryAction;
}

ADI_API uint8_t adrv904x_RxChannelsToId(const adi_adrv904x_RxChannels_e rxChannel)
{
    switch (rxChannel)
    {
    case ADI_ADRV904X_RX0:
        return 0;
        break;

    case ADI_ADRV904X_RX1:
        return 1;
        break;

    case ADI_ADRV904X_RX2:
        return 2;
        break;

    case ADI_ADRV904X_RX3:
        return 3;
        break;

    case ADI_ADRV904X_RX4:
        return 4;
        break;

    case ADI_ADRV904X_RX5:
        return 5;
        break;

    case ADI_ADRV904X_RX6:
        return 6;
        break;

    case ADI_ADRV904X_RX7:
        return 7;
        break;

    default:
        return ADI_ADRV904X_MAX_RX_ONLY;
        break;
    }
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcFuncsBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_RxChannels_e rxChan,
                                                                      adrv904x_BfCddcFuncsChanAddr_e* const cddcFuncsChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check cddc bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcFuncsChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChan) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               NULL,
                               "Channel not in device initializedChannels for adrv904x_CddcFuncsBitfieldAddressGet() function");
        return recoveryAction;
    }

    switch (rxChan)
    {
        case ADI_ADRV904X_RX0:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX1:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX2:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS;
            break;

        case ADI_ADRV904X_RX3:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX4:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX5:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX6:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        case ADI_ADRV904X_RX7:
            *cddcFuncsChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS;
            break;
            
        default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       rxChan,
                                       "Invalid rxChan parameter ");
                return recoveryAction;
            }
            break;
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;    
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcHbDpathBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_RxChannels_e rxChan,
                                                                        adrv904x_BfCddcHbDpathChanAddr_e* const cddcHbDpathChannelBitfieldAddr)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check cddc bitfield channel address pointer is not null */
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cddcHbDpathChannelBitfieldAddr);

    if ((device->devStateInfo.initializedChannels & (uint32_t)rxChan) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               NULL,
                               "Channel not in device initializedChannels for adrv904x_CddcHbDpathBitfieldAddressGet() function");
        return recoveryAction;
    }

    switch (rxChan)
    {
        case ADI_ADRV904X_RX0:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX1:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX2:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH;
            break;

        case ADI_ADRV904X_RX3:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX4:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX5:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX6:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        case ADI_ADRV904X_RX7:
            *cddcHbDpathChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH;
            break;
            
        default:
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       rxChan,
                                       "Invalid rxChan parameter ");
                return recoveryAction;
            }
            break;
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;    
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierRssiCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                     const adi_adrv904x_RxCarrierRssiCfg_t* const rssiCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rssiCfg);

    static const uint8_t  ENABLE_MAX = 1U;
    static const uint16_t MAX_START_DELAY = 0x1FFFU;
    static const uint32_t MAX_WAIT_DELAY = 0xFFFFFFU;
    static const uint16_t MAX_PAUSE_DELAY = 0x3FFFU;
    static const uint8_t  MAX_DURATION = 0x1FU;
    static const uint16_t MAX_MULFACTOR = 0x03FFU;
    static const uint8_t  MAX_TRIGSELECT = 0x07U;
    static const uint32_t MAX_CAPTURE_TIME_USEC = 500000U; /* 0.5 sec */

    if (((rssiCfg->rxCarrierMask.carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (rssiCfg->rxCarrierMask.carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->rxCarrierMask.carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (((rssiCfg->rxCarrierMask.rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
       (rssiCfg->rxCarrierMask.rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->rxCarrierMask.rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if ((rssiCfg->measMode != ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_IMMEDIATE_FDD) &&
        (rssiCfg->measMode != ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_TDD_SINGLE) &&
        (rssiCfg->measMode != ADI_ADRV904X_RX_CARRIER_RSSI_MEAS_MODE_TDD_CONTINUOUS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->measMode,
                               "Invalid Rssi measMode selection.");
        return recoveryAction;
    }

    
    if ((rssiCfg->startDelay > MAX_START_DELAY) ||
        (rssiCfg->startDelay == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->startDelay,
                               "Invalid startDelay selection.");
        return recoveryAction;
    }

    if ((rssiCfg->waitDelay > MAX_WAIT_DELAY) ||
        (rssiCfg->waitDelay == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->waitDelay,
                               "Invalid waitDelay selection.");
        return recoveryAction;
    }

    if ((rssiCfg->pauseDelay > MAX_PAUSE_DELAY) ||
        (rssiCfg->pauseDelay == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->pauseDelay,
                               "Invalid pauseDelay selection.");
        return recoveryAction;
    }

    if ((rssiCfg->opMode != ADI_ADRV904X_RX_CARRIER_RSSI_OP_MODE_SUMPOWER2) &&
       (rssiCfg->opMode != ADI_ADRV904X_RX_CARRIER_RSSI_OP_MODE_POWER2))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->opMode,
                               "Invalid Rssi opMode selection.");
        return recoveryAction ;
    }

    if (rssiCfg->duration0 > MAX_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->duration0,
                               "Invalid duration0 selection.");
        return recoveryAction;
    }

    if (rssiCfg->duration1 > MAX_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->duration1,
                               "Invalid duration1 selection.");
        return recoveryAction;
    }

    if (rssiCfg->duration2 > MAX_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->duration2,
                               "Invalid duration2 selection.");
        return recoveryAction;
    }

    if (rssiCfg->duration3 > MAX_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->duration3,
                               "Invalid duration3 selection.");
        return recoveryAction;
    }

    if (rssiCfg->mulFactor0 > MAX_MULFACTOR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->mulFactor0,
                               "Invalid mulFactor0 selection.");
        return recoveryAction;
    }

    if (rssiCfg->mulFactor1 > MAX_MULFACTOR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->mulFactor1,
                               "Invalid mulFactor1 selection.");
        return recoveryAction;
    }

    if (rssiCfg->mulFactor2 > MAX_MULFACTOR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->mulFactor2,
                               "Invalid mulFactor2 selection.");
        return recoveryAction;
    }

    if (rssiCfg->mulFactor3 > MAX_MULFACTOR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->mulFactor3,
                               "Invalid mulFactor3 selection.");
        return recoveryAction;
    }

    if (rssiCfg->resetAtGainChange > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->resetAtGainChange,
                               "Invalid resetAtGainChange selection.");
        return recoveryAction;
    }

    if (rssiCfg->fddPinMode > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->fddPinMode,
                               "Invalid fddPinMode selection.");
        return recoveryAction;
    }
        
    if (rssiCfg->radioSeqTrigEnable > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->radioSeqTrigEnable,
                               "Invalid radioSeqTrigEnable selection.");
        return recoveryAction;
    }

    if (rssiCfg->radioSeqTrigSelect > MAX_TRIGSELECT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->radioSeqTrigSelect,
                               "Invalid radioSeqTrigSelect selection.");
        return recoveryAction;
    }

    if (rssiCfg->captureTimeUsec > MAX_CAPTURE_TIME_USEC)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rssiCfg->captureTimeUsec,
                               "Invalid captureTimeUsec selection.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerTddModeSet(adi_adrv904x_Device_t * const device,
                                                                      const uint32_t                rxChannelMask,
                                                                      const uint32_t                rxChannelTddMode)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;  
    uint32_t chanSel = 0U;
    uint8_t  tddMode = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if (((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
         (rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; chanIdx++)
    {
        chanSel = 1U << chanIdx;
        if ((rxChannelMask & chanSel) > 0U)
        {
            /* Convert the chanSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &cddcFuncsChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Rx Channel used to determine SPI address");
                return recoveryAction;
            }

            tddMode = 0U;
            if ((rxChannelTddMode & chanSel) > 0U)
            {
                tddMode = 1U;
            }
            recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfSet(device,
                                                                                        NULL,
                                                                                        cddcFuncsChanBaseAddr,
                                                                                        tddMode);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Dec power tdd mode enable");
                return recoveryAction;
            }
                
        }
    }

 
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerTddModeGet(adi_adrv904x_Device_t * const device,
                                                                      const uint32_t                rxChannelMask,
                                                                      uint32_t * const              rxChannelTddMode)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t tddMode = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxChannelTddMode);

    if ((rxChannelMask != (uint32_t)ADI_ADRV904X_RX0) && 
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX1) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX2) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX3) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX4) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX5) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX6) &&
        (rxChannelMask != (uint32_t)ADI_ADRV904X_RX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx mask is selected. Valid value is one of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    /* Convert the RxChannelMask to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxChannelMask), &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Error while getting Cddc SPI address");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementTddModeEnable_BfGet(device,
                                                                                NULL,
                                                                                cddcFuncsChanBaseAddr,
                                                                                &tddMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Dec power tdd mode enable");
        return recoveryAction;
    }

    *rxChannelTddMode = (uint32_t)tddMode;
 
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxDecPowerCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const adrv904x_RxCarrierDecPowerCfg_t* const decPowerCfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, decPowerCfg);

    static const uint8_t ENABLE_MAX = 1U;
    static const uint8_t MAX_POWER_INPUT_SELECT = 2U;
    static const uint8_t MAX_MEASUREMENT_DURATION = 31U;

    if (((decPowerCfg->rxCarrierMask.carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (decPowerCfg->rxCarrierMask.carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->rxCarrierMask.carrierMask,
                               "Invalid Rx carrier is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        return recoveryAction;
    }


    if (((decPowerCfg->rxCarrierMask.rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
       (decPowerCfg->rxCarrierMask.rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->rxCarrierMask.rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (decPowerCfg->measEnable > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->measEnable,
                               "Invalid measEnable selection.");
        return recoveryAction;
    }
    
    if (decPowerCfg->inputSelect > MAX_POWER_INPUT_SELECT)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->inputSelect,
                               "Invalid inputSelect selection.");
        return recoveryAction;
    }

    if (decPowerCfg->powerMeasurementDuration > MAX_MEASUREMENT_DURATION)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerMeasurementDuration,
                               "Invalid powerMeasurementDuration selection.");
        return recoveryAction;
    }

    if (decPowerCfg->peakToPowerMode > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->peakToPowerMode,
                               "Invalid peakToPowerMode selection.");
        return recoveryAction;
    }
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerCfgSet(adi_adrv904x_Device_t * const   device,
                                                                  adrv904x_RxCarrierDecPowerCfg_t rxCarrierDecPowerCfg[],
                                                                  const uint32_t                  numCfgs)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t carrierIdx = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxCarrierDecPowerCfg);

    if (numCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCfgs, "Invalid Number of Dec Power Configurations");
        return recoveryAction;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_RxDecPowerCfgRangeCheck(device, &rxCarrierDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx Carrier Dec power range check Error Reported");
            return recoveryAction;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numCfgs; ++cfgIdx)
    {
        for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
        {
            rxSel = 1U << rxIdx;
            if ((rxCarrierDecPowerCfg[cfgIdx].rxCarrierMask.rxChannelMask & rxSel) > 0U)
            {
                /* Convert the rxSel to the base address value required by the bitfield functions */
                recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                    return recoveryAction;
                }

                for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
                {
                    recoveryAction = adrv904x_CddcFuncs_CddcPowerInputSelect_BfSet(device,
                                                                                   NULL,
                                                                                   cddcFuncsChanBaseAddr,
                                                                                   carrierIdx,
                                                                                   rxCarrierDecPowerCfg[cfgIdx].inputSelect);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement input selection");
                        return recoveryAction;
                    }
                
                    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfSet(device,
                                                                                           NULL,
                                                                                           cddcFuncsChanBaseAddr,
                                                                                           carrierIdx,
                                                                                           rxCarrierDecPowerCfg[cfgIdx].powerMeasurementDuration);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement duration");
                        return recoveryAction;
                    }

                    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfSet(device,
                                                                                                       NULL,
                                                                                                       cddcFuncsChanBaseAddr,
                                                                                                       carrierIdx,
                                                                                                       rxCarrierDecPowerCfg[cfgIdx].dpdInOutPwrMeasContDlyCntr);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement continue delay counter");
                        return recoveryAction;
                    }
                
                    recoveryAction = adrv904x_CddcFuncs_CddcPeakToPowerMode_BfSet(device,
                                                                                  NULL,
                                                                                  cddcFuncsChanBaseAddr,
                                                                                  carrierIdx,
                                                                                  rxCarrierDecPowerCfg[cfgIdx].peakToPowerMode);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting peak to power mode");
                        return recoveryAction;
                    }

                    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfSet(device,
                                                                                         NULL,
                                                                                         cddcFuncsChanBaseAddr,
                                                                                         carrierIdx,
                                                                                         rxCarrierDecPowerCfg[cfgIdx].measEnable);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement enable bit");
                        return recoveryAction;
                    }
                }
            }
        }
    }

 
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerCfgGet(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                  adrv904x_RxCarrierDecPowerCfg_t * const    cfg)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxCarrierMask);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, cfg);

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:
            carrierIdx    = 0U;
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            carrierIdx    = 1U;
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            carrierIdx    = 2U;
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            carrierIdx    = 3U;
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            carrierIdx    = 4U;
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            carrierIdx    = 5U;
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            carrierIdx    = 6U;
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            carrierIdx    = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            return recoveryAction;
            break;
    }

    if ((rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX0) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX1) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX2) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX3) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX4) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX5) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX6) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx channel selection");
        return recoveryAction;
    }


    /* Read out the configurations */
    /* Convert the rxSel to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask), &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Carrier used to determine SPI address");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CddcFuncs_CddcPowerInputSelect_BfGet(device,
                                                                   NULL,
                                                                   cddcFuncsChanBaseAddr,
                                                                   carrierIdx,
                                                                   &cfg->inputSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement input selection");
        return recoveryAction;
    }
                
    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementDuration_BfGet(device,
                                                                           NULL,
                                                                           cddcFuncsChanBaseAddr,
                                                                           carrierIdx,
                                                                           &cfg->powerMeasurementDuration);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement duration");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementContinueDelayCounter_BfGet(device,
                                                                                       NULL,
                                                                                       cddcFuncsChanBaseAddr,
                                                                                       carrierIdx,
                                                                                       &cfg->dpdInOutPwrMeasContDlyCntr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement continue delay counter");
        return recoveryAction;
    }
                
    recoveryAction = adrv904x_CddcFuncs_CddcPeakToPowerMode_BfGet(device,
                                                                  NULL,
                                                                  cddcFuncsChanBaseAddr,
                                                                  carrierIdx,
                                                                  &cfg->peakToPowerMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting peak to power mode");
        return recoveryAction;
    }

    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementEnable_BfGet(device,
                                                                         NULL,
                                                                         cddcFuncsChanBaseAddr,
                                                                         carrierIdx,
                                                                         &cfg->measEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement enable bit");
        return recoveryAction;
    }
   
    cfg->rxCarrierMask.carrierMask = rxCarrierMask->carrierMask;
    cfg->rxCarrierMask.rxChannelMask = rxCarrierMask->rxChannelMask;
 
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerPause(adi_adrv904x_Device_t * const              device,
                                                                 const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                 const uint32_t rxCarrierPause)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;
    uint8_t  powerPause = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxCarrierMask);

    if (((rxCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (rxCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (((rxCarrierMask->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
       (rxCarrierMask->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }

    if (((rxCarrierPause & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierPause,
                               "Invalid Rx carrier pause mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        return recoveryAction;
    }


    /* Write out the power measurement pause */
    for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
    {
        rxSel = 1U << rxIdx;
        if ((rxCarrierMask->rxChannelMask & rxSel) > 0U)
        {
            /* Convert the rxSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                return recoveryAction;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((rxCarrierMask->carrierMask & carrierSel) > 0U)
                {
                    powerPause = 0U;
                    if ((rxCarrierPause & carrierSel) > 0U)
                    {
                        powerPause = 1U;
                    }

                    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementPause_BfSet(device,
                                                                                        NULL,
                                                                                        cddcFuncsChanBaseAddr,
                                                                                        carrierIdx,
                                                                                        powerPause);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi manual pause");
                        return recoveryAction;
                    }

                }
            }
        }
    }

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerGet(adi_adrv904x_Device_t * const              device,
                                                               const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                               uint8_t * const                            avgPower)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint8_t  bfValue = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxCarrierMask);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, avgPower);

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:
            carrierIdx    = 0U;
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            carrierIdx    = 1U;
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            carrierIdx    = 2U;
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            carrierIdx    = 3U;
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            carrierIdx    = 4U;
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            carrierIdx    = 5U;
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            carrierIdx    = 6U;
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            carrierIdx    = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            return recoveryAction;
            break;
    }

    if ((rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX0) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX1) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX2) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX3) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX4) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX5) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX6) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx channel selection");
        return recoveryAction;
    }

    /* Read out the configurations */ 
    /* Convert the rxSel to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask), &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Channel used to determine SPI address");
        return recoveryAction;
    }
    
    /* cddc_power_measurement_readback*/
    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet(device,
                                                                           NULL,
                                                                           cddcFuncsChanBaseAddr,
                                                                           carrierIdx,
                                                                           1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement readback");
        return recoveryAction;
    }

    /* cddc_power */
    recoveryAction = adrv904x_CddcFuncs_CddcPower_BfGet(device,
                                                        NULL,
                                                        cddcFuncsChanBaseAddr,
                                                        carrierIdx,
                                                        &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting power readback");
        return recoveryAction;
    }

    *avgPower = bfValue;

 
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDecPowerAndPeakGet(adi_adrv904x_Device_t * const              device,
                                                                      const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                      uint8_t * const                            avgPower,
                                                                      uint8_t * const                            peakPower)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint8_t  bfValue = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxCarrierMask);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, avgPower);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, peakPower);

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:
            carrierIdx    = 0U;
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            carrierIdx    = 1U;
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            carrierIdx    = 2U;
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            carrierIdx    = 3U;
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            carrierIdx    = 4U;
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            carrierIdx    = 5U;
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            carrierIdx    = 6U;
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            carrierIdx    = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            return recoveryAction;
            break;
    }

    if ((rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX0) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX1) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX2) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX3) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX4) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX5) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX6) &&
        (rxCarrierMask->rxChannelMask != ADI_ADRV904X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx channel selection");
        return recoveryAction;
    }

    /* Read out the configurations */ 
    /* Convert the rxSel to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask), &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Channel used to determine SPI address");
        return recoveryAction;
    }
    
    /* cddc_power_measurement_readback*/
    recoveryAction = adrv904x_CddcFuncs_CddcPowerMeasurementReadback_BfSet(device,
                                                                           NULL,
                                                                           cddcFuncsChanBaseAddr,
                                                                           carrierIdx,
                                                                           1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement readback");
        return recoveryAction;
    }

    /* cddc_power */
    recoveryAction = adrv904x_CddcFuncs_CddcPower_BfGet(device,
                                                        NULL,
                                                        cddcFuncsChanBaseAddr,
                                                        carrierIdx,
                                                        &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting power readback");
        return recoveryAction;
    }

    *avgPower = bfValue;

    /* cddc_power_largest_peak */
    recoveryAction = adrv904x_CddcFuncs_CddcPowerLargestPeak_BfGet(device,
                                                                   NULL,
                                                                   cddcFuncsChanBaseAddr,
                                                                   carrierIdx,
                                                                   &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting power largest readback");
        return recoveryAction;
    }

    *peakPower = bfValue;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCddcCarrierCheck(adi_adrv904x_Device_t* const device,
                                                             const uint8_t chanSelect,
                                                             const uint8_t carrierSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t idx = 0U;
    uint8_t  bfValue = 0U;
    uint32_t cducBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH;
    uint32_t stride = ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH - ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Scan through each channel to validate the carrier mask */
    for (idx = 0u; idx < ADRV904X_NUM_TXRX_CHAN; idx++)
    {
        if (chanSelect & (1u << idx))
        {
            cducBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH + (idx * stride);
            bfValue = 0U;
            recoveryAction = adrv904x_CddcHbDpath_CarrierEnable_BfGet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCddcHbDpathChanAddr_e)cducBaseAddr,
                                                                      &bfValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
                return recoveryAction;
            }

            if ((carrierSelect & (uint32_t)bfValue) == 0x0u)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "requested carrier mask contains invalid carriers");
                return recoveryAction;
            }
        }
    }

    return recoveryAction;
}


