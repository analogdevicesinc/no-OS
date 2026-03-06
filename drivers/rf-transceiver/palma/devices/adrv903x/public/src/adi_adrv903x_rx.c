/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_rx.c
* \brief Contains Rx features related function implementation defined in
* adi_adrv903x_rx.h
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_rx.h"
#include "adi_adrv903x_cpu.h"
#include "adi_adrv903x_hal.h"
#include "adi_adrv903x_radioctrl.h"

#include "../../private/include/adrv903x_rx.h"
#include "../../private/include/adrv903x_tx.h"
#include "../../private/include/adrv903x_gpio.h"
#include "../../private/bf/adrv903x_bf_rx_funcs.h"
#include "../../private/bf/adrv903x_bf_rx_ddc.h"
#include "../../private/bf/adrv903x_bf_rx_dig.h"
#include "../../private/bf/adrv903x_bf_orx_dig.h"
#include "../../private/include/adrv903x_cpu.h"
#include "../../private/include/adrv903x_reg_addr_macros.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PUBLIC_RX

#define ADI_ADRV903X_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE 2U
#define ADI_ADRV903X_NUM_GPIOS_IN_RX_EXT_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV903X_NUM_GPIOS_IN_RX_DUALBAND_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX  8U
#define MAX_ORX_CHANNEL_ID 1U
#define ADC_TEST_GEN_SEL_BYBASS       0x0U
#define ADC_TEST_GEN_SEL_SINE         0x3U
#define ADC_TEST_GEN_EN_SEL_SPI       0x1U
#define ADC_TEST_GEN_CLK_ADC          0x0U


static const uint8_t MAX_ORX_CHANNEL_MASK = (1U << (MAX_ORX_CHANNEL_ID + 1U)) - 1U;

/* Simple mapping from ORx channel id to corresponding OrxWest regmap addr */
static const adrv903x_BfActrlOrxWestRegmapChanAddr_e chanIdToOrxWestRegmapChanAddr[] = 
{ 
    ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP,
    ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP
};


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableWrite(adi_adrv903x_Device_t* const        device,
                                                               const uint32_t                      rxChannelMask,
                                                               const uint8_t                       gainIndexOffset,
                                                               const adi_adrv903x_RxGainTableRow_t gainTableRow[],
                                                               const uint32_t                      arraySize)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t baseIndex = 0U;
    uint32_t baseAddress[1] = { 0U };
    uint16_t numGainIndicesToWrite = arraySize;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint8_t, cpuDmaData, ((ADI_ADRV903X_MAX_GAIN_TABLE_INDEX - ADI_ADRV903X_MIN_GAIN_TABLE_INDEX) + 1U) * ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX);
    adrv903x_BfRxDdcChanAddr_e ddcBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;
    uint32_t chanId = 0U;
    adi_adrv903x_SpiCache_t spiCache = { { 0U }, 0U, 0U, 0U };

    const uint32_t rxGainTableBaseAddr[] = 
    { 
        ADI_ADRV903X_RX0_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX1_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX2_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX3_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX4_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX5_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX6_GAIN_TABLE_BASEADDR,
        ADI_ADRV903X_RX7_GAIN_TABLE_BASEADDR
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuDmaData, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxGainTableWriteRangeCheck(device, rxChannelMask, gainIndexOffset, gainTableRow, numGainIndicesToWrite);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableWriteRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Calculate base index for the config*/
    baseIndex = (gainIndexOffset - (numGainIndicesToWrite - 1U));

    /*Format Gain Table Entries*/
    recoveryAction = adrv903x_RxGainTableFormat(device, gainTableRow, &cpuDmaData[0U], numGainIndicesToWrite);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableFormat Issue");
        goto cleanup;
    }

    /*Resolve the RX Channel SRAM to program*/
    /*If Rx Channel Mask Set by user for this config, load Rx gain table*/
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_RX_ONLY; ++chanId)
    {
        adi_adrv903x_RxChannels_e rxChannel = (adi_adrv903x_RxChannels_e) (rxChannelMask & (1U << chanId));
        if (rxChannel > 0U)
        {
            /*Resolve Rx1 Gain Table SRAM load start address*/
            baseAddress[0U] = rxGainTableBaseAddr[chanId] + (baseIndex * ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX);
        
            /*Write to the SRAM via ARM DMA*/
                            recoveryAction = adi_adrv903x_Registers32bOnlyWrite(device,
                                                                    &spiCache,
                                                                    baseAddress[0U],
                                                                    &cpuDmaData[0U],
                                                                    (numGainIndicesToWrite * ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX));
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing gain table.");
                    goto cleanup;
                }
                        recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device, rxChannel, ADI_ADRV903X_RX_DDC_BAND0, &ddcBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
                goto cleanup;
            }
            
            recoveryAction = adrv903x_RxDdc_DigitalGainEnable_BfSet(device, &spiCache, ddcBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while writing Gain Comp Config0 DDC0");
                goto cleanup;
            }
            
            recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device, rxChannel, ADI_ADRV903X_RX_DDC_BAND1, &ddcBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
                goto cleanup;
            }
            
            recoveryAction = adrv903x_RxDdc_DigitalGainEnable_BfSet(device, &spiCache, ddcBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while writing Gain Comp Config0 DDC1");
                goto cleanup;
            }
        }
    }

    recoveryAction = adi_adrv903x_SpiFlush(device, spiCache.data, &spiCache.count);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue After CPU Mailbox Buffer Write");
        goto cleanup;
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableRead(adi_adrv903x_Device_t* const    device,
                                                              const adi_adrv903x_RxChannels_e rxChannel,
                                                              const uint8_t                   gainIndexOffset,
                                                              adi_adrv903x_RxGainTableRow_t   gainTableRow[],
                                                              const uint32_t                  arraySize,
                                                              uint16_t* const                 numGainIndicesRead)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    uint32_t baseAddress = 0U;
    uint32_t maxReadGainIndices = arraySize;
    uint16_t numGainIndicesReadVal = 0U;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    ADI_PLATFORM_LARGE_ARRAY_ALLOC(uint8_t, cpuAhbData, ((ADI_ADRV903X_MAX_GAIN_TABLE_INDEX - ADI_ADRV903X_MIN_GAIN_TABLE_INDEX) + 1U) * ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX);

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cpuAhbData, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxGainTableReadRangeCheck(device,
                                                            rxChannel,
                                                            gainIndexOffset,
                                                            gainTableRow,
                                                            maxReadGainIndices);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableReadRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Calculate no. of indices to read and the base address for the config*/
    recoveryAction = adrv903x_RxGainTableReadParamsCompute( device,
                                                            rxChannel,
                                                            maxReadGainIndices,
                                                            gainIndexOffset,
                                                            &numGainIndicesReadVal,
                                                            &baseAddress);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableReadParamsCompute Issue");
        goto cleanup;
    }

    /*Read Gain Table Data for the requested channel via ARM DMA*/
    recoveryAction = adi_adrv903x_Registers32bOnlyRead(device,
                                                       NULL,
                                                       baseAddress,
                                                       &cpuAhbData[0U],
                                                       (numGainIndicesReadVal * ADI_ADRV903X_NUM_BYTES_PER_RX_GAIN_INDEX));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error Reading Gain Table ARM DMA");
        goto cleanup;
    }

    /*Parse gain table data obtained in ARM DMA data format to an rx gain table row entry data structure memory*/
    recoveryAction = adrv903x_RxGainTableParse(device, &gainTableRow[0U], &cpuAhbData[0], numGainIndicesReadVal);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableParse Issue");
        goto cleanup;
    }

    /*Update no. of gain indices read*/
    if (numGainIndicesRead != NULL)
    {
        *numGainIndicesRead = numGainIndicesReadVal;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainTableExtCtrlPinsSet(adi_adrv903x_Device_t* const    device,
                                                                        const uint32_t                  rxChannelMask,
                                                                        const uint32_t                  channelEnable)
{
        #define NUM_GPIO_PER_CHANNEL 2U
    
    static const adi_adrv903x_GpioAnaPinSel_e gpioLut[ADI_ADRV903X_MAX_RX_ONLY][NUM_GPIO_PER_CHANNEL] = {
        { ADI_ADRV903X_GPIO_ANA_00, ADI_ADRV903X_GPIO_ANA_01 },
        { ADI_ADRV903X_GPIO_ANA_02, ADI_ADRV903X_GPIO_ANA_03 },
        { ADI_ADRV903X_GPIO_ANA_04, ADI_ADRV903X_GPIO_ANA_05 },
        { ADI_ADRV903X_GPIO_ANA_06, ADI_ADRV903X_GPIO_ANA_07 },
        { ADI_ADRV903X_GPIO_ANA_08, ADI_ADRV903X_GPIO_ANA_09 },
        { ADI_ADRV903X_GPIO_ANA_10, ADI_ADRV903X_GPIO_ANA_11 },
        { ADI_ADRV903X_GPIO_ANA_12, ADI_ADRV903X_GPIO_ANA_13 },
        { ADI_ADRV903X_GPIO_ANA_14, ADI_ADRV903X_GPIO_ANA_15 }
    };
    
    static const adi_adrv903x_GpioSignal_e sigLut[ADI_ADRV903X_MAX_RX_ONLY][NUM_GPIO_PER_CHANNEL] = {
        { ADI_ADRV903X_GPIO_SIGNAL_RX0_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX0_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX1_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX1_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX2_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX2_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX3_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX3_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX4_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX4_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX5_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX5_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX6_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX6_EXT_CONTROL_1 },
        { ADI_ADRV903X_GPIO_SIGNAL_RX7_EXT_CONTROL_0, ADI_ADRV903X_GPIO_SIGNAL_RX7_EXT_CONTROL_1 }
    };
    
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t currentChannelMask = 0U;
    uint8_t chIdx = 0U;
    uint8_t gpioIdx = 0U;
    adi_adrv903x_GpioSignal_e getSig = ADI_ADRV903X_GPIO_SIGNAL_UNUSED;
    uint32_t getChanMask = 0U;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    /*Check that rxChannelMask is valid*/
    if (rxChannelMask > (uint32_t)( ADI_ADRV903X_RX0 | ADI_ADRV903X_RX1 | ADI_ADRV903X_RX2 | ADI_ADRV903X_RX3 |
                                    ADI_ADRV903X_RX4 | ADI_ADRV903X_RX5 | ADI_ADRV903X_RX6 | ADI_ADRV903X_RX7 ))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx Channel mask parameter. Valid Rx channel masks include 0x0-0xff");
        goto cleanup;
    }
    
    /*Check that channelEnable is Valid*/
    if (channelEnable > (uint32_t)( ADI_ADRV903X_RX0 | ADI_ADRV903X_RX1 | ADI_ADRV903X_RX2 | ADI_ADRV903X_RX3 |
                                    ADI_ADRV903X_RX4 | ADI_ADRV903X_RX5 | ADI_ADRV903X_RX6 | ADI_ADRV903X_RX7 ))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                channelEnable,
                                "Invalid channelEnable mask parameter. Valid Enable Channel bitmasks 0x0-0xff");
        goto cleanup;
    }
    
    /* If no channels were selected, this is an allowed noop  */
    if (rxChannelMask  == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }
    else
    {
    
        /*Iterate through rxChannelMask, enabling/disabling selected channels*/
        for (chIdx = 0U; chIdx < ADI_ADRV903X_MAX_RX_ONLY; chIdx++)
        {
            /* Update currentChannelMask enum var for this chIdx */
            currentChannelMask = (adi_adrv903x_RxChannels_e)(1U << chIdx);
        
            /*Check if this channel should be affected*/
            if ((currentChannelMask & rxChannelMask) != 0U)
            {
            
                /* Enable Analog GPIOs for this channel */
                if ((currentChannelMask & channelEnable) != 0U)
                {   
                    for (gpioIdx = 0U; gpioIdx < NUM_GPIO_PER_CHANNEL; gpioIdx++)
                    {
                        /*Route signals for this channel to Analog GPIOs*/
                        recoveryAction = adrv903x_GpioAnalogSignalSet(  device, 
                                                                        gpioLut[chIdx][gpioIdx],
                                                                        sigLut[chIdx][gpioIdx],
                                                                        0U);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioAnalogSignalSet Issue");
                            goto cleanup;
                        }
                    } /* end of gpioIdx for loop */
            
                } /* end Enable block */
                /* Disable Analog GPIOs for this channel */
                else
                {
                
                    for (gpioIdx = 0U; gpioIdx < NUM_GPIO_PER_CHANNEL; gpioIdx++)
                    {
                        /* Check if the GPIO has the associated Signal routed. */    
                        recoveryAction = adrv903x_GpioAnalogSignalGet(  device, 
                                                                        gpioLut[chIdx][gpioIdx],
                                                                       &getSig,
                                                                       &getChanMask);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioAnalogSignalGet Issue");
                            goto cleanup;
                        }
                    
                        /* If the expected signal is currently routed, release/disconnect the pin */
                        if (getSig == sigLut[chIdx][gpioIdx])
                        {
                            recoveryAction = adrv903x_GpioAnalogSignalRelease(  device,
                                                                                gpioLut[chIdx][gpioIdx],
                                                                                sigLut[chIdx][gpioIdx],
                                                                                0U);
                            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                            {
                                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioAnalogSignalRelease Issue");
                                goto cleanup;
                            }
                        }

                    } /* end of gpioIdx for loop */
                
                } /* end Disable block */
            
            } /* End if channel-affected block */

        } /* end rxChannel for loop */
        
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxMinMaxGainIndexSet(adi_adrv903x_Device_t* const    device,
                                                                   const uint32_t                  rxChannelMask,
                                                                   const uint8_t                   minGainIndex,
                                                                   const uint8_t                   maxGainIndex)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxMinMaxGainIndexSetRangeCheck(device, rxChannelMask, minGainIndex, maxGainIndex);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMinMaxGainIndexSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Update device gain table min and max gain indices*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX0) == (uint32_t)ADI_ADRV903X_RX0)
    {
        device->devStateInfo.gainIndexes.rx0MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx0MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX1) == (uint32_t)ADI_ADRV903X_RX1)
    {
        device->devStateInfo.gainIndexes.rx1MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx1MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX2) == (uint32_t)ADI_ADRV903X_RX2)
    {
        device->devStateInfo.gainIndexes.rx2MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx2MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX3) == (uint32_t)ADI_ADRV903X_RX3)
    {
        device->devStateInfo.gainIndexes.rx3MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx3MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX4) == (uint32_t)ADI_ADRV903X_RX4)
    {
        device->devStateInfo.gainIndexes.rx4MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx4MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX5) == (uint32_t)ADI_ADRV903X_RX5)
    {
        device->devStateInfo.gainIndexes.rx5MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx5MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX6) == (uint32_t)ADI_ADRV903X_RX6)
    {
        device->devStateInfo.gainIndexes.rx6MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx6MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV903X_RX7) == (uint32_t)ADI_ADRV903X_RX7)
    {
        device->devStateInfo.gainIndexes.rx7MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx7MinGainIndex = minGainIndex;
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    goto cleanup;   /* Added to deal with Compiler Warning when ADI_ADRV903X_RX_RANGE_CHECK = 0 */

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainSet(adi_adrv903x_Device_t* const    device,
                                                        const adi_adrv903x_RxGain_t     rxGain[],
                                                        const uint32_t                  arraySize)
{
        static const uint32_t RX0_FUNC_ADDR_BASE     = 0x60030050U;
    static const uint8_t  DATA_TRANSACTION_1_SIZE = 18U;
    static const uint8_t  DATA_TRANSACTION_2_SIZE = 6U;

    uint32_t configIndex = 0U;
    uint32_t chanIdx     = 0U;
    uint32_t chanSel     = 0U;

    /* Used to track whether its our first write or not, used to limit number of spi bytes being transmitted */
    uint8_t firstWrite     = 0U; 

    uint8_t rxFuncFirstByte  = (uint8_t)(RX0_FUNC_ADDR_BASE >> 24U); 
    uint8_t rxFuncSecondByte = 0x00U;
    uint8_t rxFuncThirdByte  = 0x00U;
    uint8_t rxFuncLastByte   = (uint8_t)(RX0_FUNC_ADDR_BASE & 0xFFU); 

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;
    
    /* Hardcoded 18 byte data array for first spi transaction with placeholders for addresses and data
       dataPageSetup[0]  : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGE_31TO24) address
       dataPageSetup[1]  : 0x26U lsb of paging register (ADRV903X_ADDR_SPI0_PAGE_31TO24) address
       dataPageSetup[2]  : 0x60U first byte of slice register address
       dataPageSetup[3]  : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGE_23TO16) address
       dataPageSetup[4]  : 0x27U lsb of paging register (ADRV903X_ADDR_SPI0_PAGE_23TO16) address
       dataPageSetup[5]  : 0x00U PLACEHOLDER - Next byte of slice register address
       dataPageSetup[6]  : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGE_15TO8) address
       dataPageSetup[7]  : 0x28U lsb of paging register (ADRV903X_ADDR_SPI0_PAGE_15TO8) address
       dataPageSetup[8]  : 0x00U Next byte of slice register address
       dataPageSetup[9]  : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGE_7TO0) address
       dataPageSetup[10] : 0x29U lsb of paging register (ADRV903X_ADDR_SPI0_PAGE_7TO0) address
       dataPageSetup[11] : 0x50U Next byte of slice register address
       dataPageSetup[12] : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGING_CONTROL) address
       dataPageSetup[13] : 0x2AU lsb of paging register (ADRV903X_ADDR_SPI0_PAGING_CONTROL) address
       dataPageSetup[14] : 0x00U Data to write to paging congtrol register
       dataPageSetup[15] : 0x40U msb of calculated address depending on paging config and spi write polarity
       dataPageSetup[16] : 0x01U lsb of calculated address depending on paging config and spi write polarity
       dataPageSetup[17] : 0x00U PLACEHOLDER - will be replaced with gain index value for that given slice
    */
    uint8_t dataPageSetup[] = { 0x01U, 0x26U, rxFuncFirstByte, 0x01U, 0x27U, rxFuncSecondByte, 0x01U, 0x28U, rxFuncThirdByte, 0x01U, 0x29U, rxFuncLastByte, 0x01U, 0x2aU, 0x00U, 0x40U, 0x01U, 0x00U};

    /* Hardcoded 6 byte data array for every subsequent spi transaction
       data[0]  : 0x01U msb of paging register (ADRV903X_ADDR_SPI0_PAGE_31TO24) address
       data[1]  : 0x27U lsb of paging register (ADRV903X_ADDR_SPI0_PAGE_31TO24) address
       data[2]  : 0x00U PLACEHOLDER - Byte of slice register address which can be calculated based on which channel you are dealing with
       data[3]  : 0x40U msb of calculated address depending on paging config and spi write polarity
       data[4]  : 0x01U lsb of calculated address depending on paging config and spi write polarity
       data[5]  : 0x00U PLACEHOLDER - will be replaced with gain index value for that given slice
    */
    uint8_t data[] = { 0x01U, 0x27U, 0x00U, 0x40U, 0x01U, 0x00U };
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0U
    recoveryAction = adrv903x_RxGainSetRangeCheck(device, rxGain, arraySize);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Update manual gain index setting for the requested channel */
    for (configIndex = 0U; configIndex < arraySize; ++configIndex)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(rxGain[configIndex].rxChannelMask, chanSel))
            {
                /* calculate part of address related to given channel id */
                rxFuncSecondByte = (uint8_t)(3U + (chanIdx << 4U));
                device->devStateInfo.currentPageStartingAddress = (uint32_t)((rxFuncFirstByte << 24)  + (rxFuncSecondByte << 16) + (rxFuncThirdByte << 8) + rxFuncLastByte);
                /* check if its the first time we've called this function so we can setup the paging registers */
                if (firstWrite == 0U)
                {
                    dataPageSetup[5U]  = rxFuncSecondByte;
                    dataPageSetup[17U] = rxGain[configIndex].gainIndex;
                    halError = adi_hal_SpiWrite(device->common.devHalInfo, dataPageSetup, DATA_TRANSACTION_1_SIZE);
                    if (halError != ADI_HAL_ERR_OK)
                    {
                        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
                        ADI_PARAM_ERROR_REPORT(&device->common,
                                               recoveryAction,
                                               dataPageSetup,
                                               "Error writing rx gain value");
                        goto cleanup;
                    }
                    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
                    /* update first write to signify we have wrote the data at least once and paging registers are configured correctly */
                    firstWrite = 1U;
                }
                else
                {
                    /* update data array to point to correct slice and with correct gain index for given configIndex */
                    data[2U] = rxFuncSecondByte;
                    data[5U] = rxGain[configIndex].gainIndex;
                    halError = adi_hal_SpiWrite(device->common.devHalInfo, data, DATA_TRANSACTION_2_SIZE);
                    if (halError != ADI_HAL_ERR_OK)
                    {
                        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
                        ADI_PARAM_ERROR_REPORT(&device->common,
                                               recoveryAction,
                                               data,
                                               "Error writing rx gain value");
                        goto cleanup;
                    }
                    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
                }
            }
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxMgcGainGet(adi_adrv903x_Device_t* const    device,
                                                           const adi_adrv903x_RxChannels_e rxChannel,
                                                           adi_adrv903x_RxGain_t * const   rxGain)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxGainGetRangeCheck(device, rxChannel, rxGain);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainGetRangeCheck Issue");
        goto cleanup;
    }
#endif
    
    /*Check for null rxGain pointer*/
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGain, cleanup);
    
    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        goto cleanup;
    }
                
    recoveryAction = adrv903x_RxFuncs_AgcManualGainIndex_BfGet(device,
                                                               NULL,
                                                               rxFuncsBaseAddr,
                                                               &rxGain->gainIndex);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Rx manual gain index for channel");
        goto cleanup;
    }

    /* Set rxChannelMask to a valid Rx Channel */
    rxGain->rxChannelMask = (uint32_t)rxChannel;
        
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainGet(adi_adrv903x_Device_t* const    device,
                                                        const adi_adrv903x_RxChannels_e rxChannel,
                                                        adi_adrv903x_RxGain_t * const   rxGain)
{
        static const uint8_t LATCH_READBACK_DUMMY_DATA = 0xFFU;
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    uint8_t latchReadback = 0;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

#if ADI_ADRV903X_RX_RANGE_CHECK > 0
    recoveryAction = adrv903x_RxGainGetRangeCheck(device, rxChannel, rxGain);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainGetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Check for null rxGain pointer*/
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxGain, cleanup);
    
    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx funcs address get issue");
        goto cleanup;
    }

    recoveryAction = adrv903x_RxFuncs_AgcEnableGainIndexUpdate_BfGet(device, NULL, rxFuncsBaseAddr, &latchReadback);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading enable gain index update bit");
        goto cleanup;
    }

    if (latchReadback == ADI_TRUE) 
    {
        recoveryAction = adrv903x_RxFuncs_AgcGainIndex_BfSet(device,
                                                             NULL,
                                                             rxFuncsBaseAddr,
                                                             LATCH_READBACK_DUMMY_DATA);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dummy data to latch gain index readback");
            goto cleanup;
        }
    }
    
    recoveryAction = adrv903x_RxFuncs_AgcGainIndex_BfGet(device,
                                                         NULL,
                                                         rxFuncsBaseAddr,
                                                         &rxGain->gainIndex);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading AGC gain index");
        goto cleanup;
    }

    /* Set rxChannelMask to a valid Rx Channel */
    rxGain->rxChannelMask = (uint32_t)rxChannel;
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDataFormatGet(adi_adrv903x_Device_t * const         device,
                                                              const adi_adrv903x_RxChannels_e       rxChannel,
                                                              adi_adrv903x_RxDataFormatRt_t * const rxDataFormat)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, rxDataFormat);
    
    /* Check if the requested rxChannel is valid Rx or ORx channel. */
    if ((rxChannel != ADI_ADRV903X_RX0)  &&
        (rxChannel != ADI_ADRV903X_RX1)  &&
        (rxChannel != ADI_ADRV903X_RX2)  &&
        (rxChannel != ADI_ADRV903X_RX3)  &&
        (rxChannel != ADI_ADRV903X_RX4)  &&
        (rxChannel != ADI_ADRV903X_RX5)  &&
        (rxChannel != ADI_ADRV903X_RX6)  &&
        (rxChannel != ADI_ADRV903X_RX7)  &&
        (rxChannel != ADI_ADRV903X_ORX0) &&
        (rxChannel != ADI_ADRV903X_ORX1) )
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxDataFormatGet");
        goto cleanup;
    }
    
    /* Retrieve the format selection with adrv903x_RxDataFormatSelectGet */
    recoveryAction = adrv903x_RxDataFormatSelectGet(device,
                                                    rxChannel,
                                                   &rxDataFormat->rxDataFormat.formatSelect);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat formatSelect");
        goto cleanup;
    }
    
    /* Retrieve the floating point format configuration with adrv903x_RxDataFormatFloatingPointGet */
    recoveryAction = adrv903x_RxDataFormatFloatingPointGet(  device,
                                                             rxChannel, 
                                                            &rxDataFormat->rxDataFormat.floatingPointConfig);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat floatingPointConfig");
        goto cleanup;
    }

    
    /* Retrieve the integer format configuration with adrv903x_RxDataFormatIntegerGet */
    recoveryAction = adrv903x_RxDataFormatIntegerGet(  device,
                                                       rxChannel,
                                                      &rxDataFormat->rxDataFormat.integerConfigSettings,
                                                      &rxDataFormat->rxDataFormat.slicerConfigSettings);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat integerConfigSettings and slicerConfigSettings");
        goto cleanup;
    }
    
    /* Retrieve the embedded overload indicator format configuration with adrv903x_RxDataFormatEmbOvldMonitorGet */
    recoveryAction = adrv903x_RxDataFormatEmbOvldMonitorGet(  device,
                                                              rxChannel,
                                                             &rxDataFormat->rxDataFormat.embOvldMonitorSettings);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat embOvldMonitorSettings");
        goto cleanup;
    }
    
    /* Retrieve the external LNA gain compensation and gain temperature compensation with adrv903x_RxGainCompExtLnaGet and adrv903x_RxGainCompTempEnableGet */
    recoveryAction = adrv903x_RxGainCompExtLnaGet(  device,
                                                    rxChannel,
                                                   &rxDataFormat->rxDataFormat.externalLnaGain);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat externalLnaGain");
        goto cleanup;
    }

    recoveryAction = adrv903x_RxGainCompTempEnableGet(  device,
                                                        rxChannel,
                                                       &rxDataFormat->rxDataFormat.tempCompensationEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat tempCompensationEnable");
        goto cleanup;
    }
    
    /* Populate the rxChannelMask field of the output structure with the specified channel. */
    rxDataFormat->rxChannelMask = rxChannel;
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSlicerPositionGet(adi_adrv903x_Device_t* const        device,
                                                                  const adi_adrv903x_RxChannels_e    rxChannel,
                                                                  uint8_t* const                     slicerPosition)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsChanAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, slicerPosition, cleanup);
    
    /* Check that the requested rxChannel is valid. ORx not allowed */
    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, rxChannel, "Invalid Rx Channel Requested for reading slicer position");
        goto cleanup;
    }
    
    /* Get Rx Funcs Bitfield Address */
    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(  device,
                                                          rxChannel,
                                                         &rxFuncsChanAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
        goto cleanup; 
    }
    
    /* Get Slicer Position from Bitfield */
    recoveryAction =  adrv903x_RxFuncs_RxdpSlicerPosition_BfGet(device,
                                                                NULL,
                                                                rxFuncsChanAddr,
                                                                slicerPosition);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(  &device->common, recoveryAction, "RxdpSlicerPosition readback issue");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxLoSourceGet(adi_adrv903x_Device_t * const   device,
                                                            const adi_adrv903x_RxChannels_e rxChannel,
                                                            adi_adrv903x_LoSel_e * const    rxLoSource)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    /* Check Rx data format config pointer is not NULL*/
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxLoSource, cleanup);
    
    /* Check that the requested rxChannel is valid */
    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, 
                                 recoveryAction, 
                                 rxChannel, 
                                "Invalid Rx channel selected for LO source mapping read back. Valid Rx channels are Rx0-Rx7.");
        goto cleanup;
    }
    
    /* Check that rx profile is valid in current config */
    if (((device->devStateInfo.profilesValid & ADI_ADRV903X_RX_PROFILE_VALID) != ADI_ADRV903X_RX_PROFILE_VALID)
        || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannel,
                                 "LO source read back requested for an Rx channel but Rx profile is invalid or channel not initialized in the device structure");
        goto cleanup;
    }
    
    /* Readback Rx LO source selection */
    recoveryAction = adrv903x_RxLoSourceGet(device, rxChannel, rxLoSource);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving Lo Source for Rx channel.");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxNcoShifterSet(adi_adrv903x_Device_t* const            device,
                                                              const adi_adrv903x_RxNcoConfig_t* const rxNcoConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_RxNcoConfig_t rxNcoInfo;
    adrv903x_RxNcoConfigResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;
    uint8_t channelMaskRet = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxNcoConfig, cleanup);
    ADI_LIBRARY_MEMSET(&rxNcoInfo, 0, sizeof(rxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (rxNcoConfig->bandSelect >= ADRV903X_DDC_NUM_BAND)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    rxNcoInfo.chanSelect = (uint8_t) rxNcoConfig->chanSelect;
    rxNcoInfo.bandSelect = (adi_adrv903x_DdcNumber_t) rxNcoConfig->bandSelect;
    rxNcoInfo.enable = (uint8_t) rxNcoConfig->enable;
    rxNcoInfo.phase = (uint32_t) ADRV903X_HTOCL(rxNcoConfig->phase);
    rxNcoInfo.frequencyKhz = (int32_t) ADRV903X_HTOCL(rxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_SET_RX_NCO,
                                                (void*)&rxNcoInfo,
                                                sizeof(rxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == rxNcoInfo.chanSelect)
        {
            goto cleanup;
        }
    }
    /* if didn't go to cleanup, then some error occurred.*/
    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "Firmware did not execute cmd on all channels requested");

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxNcoShifterGet(adi_adrv903x_Device_t* const                    device,
                                                              adi_adrv903x_RxNcoConfigReadbackResp_t* const   rxRbConfig)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_RxNcoConfigReadback_t rxNcoCfgRbCmd;
    adi_adrv903x_RxNcoConfigReadbackResp_t rxNcoCfgGetCmdRsp;
    adrv903x_CpuCmdStatus_e cmdStatus = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e cpuErrorCode = ADRV903X_CPU_NO_ERROR;
    uint32_t cpuType = 0U;
    uint32_t chIdx = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&rxNcoCfgRbCmd, 0, sizeof(rxNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&rxNcoCfgGetCmdRsp, 0, sizeof(rxNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated */
    if (rxRbConfig->chanSelect == (uint8_t) ADI_ADRV903X_RXOFF || 
        rxRbConfig->chanSelect > (uint8_t) ADI_ADRV903X_RX7    || 
        (((rxRbConfig->chanSelect - 1) & rxRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (rxRbConfig->bandSelect >= ADRV903X_DDC_NUM_BAND)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    /* Get CPU assigned to this channel */
    for (chIdx = 0U; chIdx < ADI_ADRV903X_MAX_CHANNELS; chIdx++)
    {
        if (rxRbConfig->chanSelect == ((uint8_t)1U << chIdx))
        {
            cpuType = device->initExtract.rxTxCpuConfig[chIdx];
            break;
        }
    }

    /* Prepare the command payload */
    rxNcoCfgRbCmd.chanSelect = (uint8_t) rxRbConfig->chanSelect;
    rxNcoCfgRbCmd.bandSelect = (adi_adrv903x_DdcNumber_t) rxRbConfig->bandSelect;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         (adi_adrv903x_CpuType_e)cpuType,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_GET_RX_NCO,
                                         (void*)&rxNcoCfgRbCmd,
                                         sizeof(rxNcoCfgRbCmd),
                                         (void*)&rxNcoCfgGetCmdRsp,
                                         sizeof(rxNcoCfgGetCmdRsp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)rxNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
    }

    if (rxNcoCfgGetCmdRsp.chanSelect == rxNcoCfgRbCmd.chanSelect)
    {
        /* Extract the command-specific response from the response payload */
        rxRbConfig->enabled = (uint8_t) rxNcoCfgGetCmdRsp.enabled;
        rxRbConfig->phase = (uint32_t) ADRV903X_CTOHL(rxNcoCfgGetCmdRsp.phase); 
        rxRbConfig->frequencyKhz = (int32_t) ADRV903X_CTOHL(rxNcoCfgGetCmdRsp.frequencyKhz);
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->chanSelect, "Channel is not handled by the CPU");
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoSet(adi_adrv903x_Device_t* const                device,
                                                        const adi_adrv903x_ORxNcoConfig_t * const   orxNcoConfig)
{
        adi_adrv903x_ErrAction_e    recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_ORxNcoConfig_t orxNcoInfo;
    adrv903x_ORxNcoConfigResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e     cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode    = 0U;
    uint32_t                    cpuTypeIdx      = 0U;
    uint8_t                     channelMaskRet  = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&orxNcoInfo, 0, sizeof(orxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    /* Prepare the command payload */
    orxNcoInfo.chanSelect = (uint8_t) orxNcoConfig->chanSelect;
    orxNcoInfo.ncoSelect = (uint8_t) orxNcoConfig->ncoSelect;
    orxNcoInfo.enable = (uint8_t) orxNcoConfig->enable;
    orxNcoInfo.phase = (uint32_t) ADRV903X_HTOCL(orxNcoConfig->phase);
    orxNcoInfo.frequencyKhz = (int32_t) ADRV903X_HTOCL(orxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_SET_ORX_NCO,
                                                (void*)&orxNcoInfo,
                                                sizeof(orxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == orxNcoInfo.chanSelect)
        {
            goto cleanup;
        }
    }
    /* if didn't go to cleanup, then some error occurred.*/
    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "Firmware did not execute cmd on all channels requested");

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoSet_v2(adi_adrv903x_Device_t* const                device,
                                                            const adi_adrv903x_ORxNcoConfig_t * const   orxNcoConfig)
{
        adi_adrv903x_ErrAction_e    recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_ORxNcoConfig_t orxNcoInfo;
    adrv903x_ORxNcoConfigResp_t cmdRsp;
    adrv903x_CpuCmdStatus_e     cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adi_adrv903x_CpuErrorCode_t cpuErrorCode    = 0U;
    uint32_t                    cpuTypeIdx      = 0U;
    uint8_t                     channelMaskRet  = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&orxNcoInfo, 0, sizeof(orxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    /* Prepare the command payload */
    orxNcoInfo.chanSelect = (uint8_t) orxNcoConfig->chanSelect;
    orxNcoInfo.ncoSelect = (uint8_t) orxNcoConfig->ncoSelect;
    orxNcoInfo.enable = (uint8_t) orxNcoConfig->enable;
    orxNcoInfo.phase = (uint32_t) ADRV903X_HTOCL(orxNcoConfig->phase);
    orxNcoInfo.frequencyKhz = (int32_t) ADRV903X_HTOCL(orxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_SET_ORX_NCO_V2,
                                                (void*)&orxNcoInfo,
                                                sizeof(orxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == orxNcoInfo.chanSelect)
        {
            goto cleanup;
        }
    }
    /* if didn't go to cleanup, then some error occurred.*/
    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "Firmware did not execute cmd on all channels requested");

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxNcoGet(adi_adrv903x_Device_t* const                    device,
                                                        adi_adrv903x_ORxNcoConfigReadbackResp_t* const  orxRbConfig)
{
        adi_adrv903x_ErrAction_e                recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_ORxNcoConfigReadback_t         orxNcoCfgRbCmd;
    adi_adrv903x_ORxNcoConfigReadbackResp_t orxNcoCfgGetCmdRsp;
    adrv903x_CpuCmdStatus_e                 cmdStatus           = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                 cpuErrorCode        = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                cpuTypeIdx          = 0U;
    uint8_t                                 channelMaskRet      = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxRbConfig, cleanup);
    ADI_LIBRARY_MEMSET(&orxNcoCfgRbCmd, 0, sizeof(orxNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&orxNcoCfgGetCmdRsp, 0, sizeof(orxNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (orxRbConfig->chanSelect == (uint8_t) ADI_ADRV903X_RXOFF || 
        orxRbConfig->chanSelect > (uint8_t) ADI_ADRV903X_RX7    || 
        (((orxRbConfig->chanSelect - 1) & orxRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, orxRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    orxNcoCfgRbCmd.chanSelect = (uint8_t) orxRbConfig->chanSelect;
    orxNcoCfgRbCmd.ncoSelect = (uint8_t) orxRbConfig->ncoSelect;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(   device,
                                                (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                                ADRV903X_LINK_ID_0,
                                                ADRV903X_CPU_CMD_ID_GET_ORX_NCO,
                                                (void*)&orxNcoCfgRbCmd,
                                                sizeof(orxNcoCfgRbCmd),
                                                (void*)&orxNcoCfgGetCmdRsp,
                                                sizeof(orxNcoCfgGetCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)orxNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
        }

        channelMaskRet = orxNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == orxNcoCfgRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            orxRbConfig->enabled = (uint8_t) orxNcoCfgGetCmdRsp.enabled;
            orxRbConfig->phase = (uint32_t) ADRV903X_CTOHL(orxNcoCfgGetCmdRsp.phase);
            orxRbConfig->frequencyKhz = (int32_t) ADRV903X_CTOHL(orxNcoCfgGetCmdRsp.frequencyKhz);
            orxRbConfig->status = (uint32_t) ADRV903X_CTOHL(orxNcoCfgGetCmdRsp.status); 
            goto cleanup;
        }
    }
    /* if didn't go to cleanup, then some error occurred.*/
    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "Firmware did not execute cmd on all channels requested");


cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerCfgSet(adi_adrv903x_Device_t * const device,
                                                                     adi_adrv903x_RxDecimatedPowerCfg_t rxDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    static const uint8_t TDD_MODE_ENABLE_DEFAULT_VAL = 1U;
    static const uint16_t TDD_MODE_DELAY_DEFAULT_VAL = 0U;
    static const uint8_t NUMBER_OF_RX_DDC = 2U;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint8_t decPowerEnable = 0U;
    uint8_t decPowerAgcSelect = 0U;
    uint8_t decPowerClockEnable = 0U;
    uint8_t hb2OutClkDivRatio = 0U;
    uint8_t ddcSelection = 0U;
    uint32_t ddcMask = 0U;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    adrv903x_BfRxChanAddr_e rxDigBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_DIG;
    adrv903x_BfRxDdcChanAddr_e ddcAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxDecPowerCfg, cleanup);

    if (numOfDecPowerCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfDecPowerCfgs, "Invalid Number of Dec Power Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        recoveryAction = adrv903x_DecPowerCfgRangeCheck(device, &rxDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Dec power range Check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(rxDecPowerCfg[cfgIdx].rxChannelMask, chanSel))
            {
                recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device, (adi_adrv903x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx func address");
                    goto cleanup;
                }

                recoveryAction = adrv903x_RxBitfieldAddressGet(device, (adi_adrv903x_RxChannels_e)(chanSel), &rxDigBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx dig address");
                    goto cleanup;
                }
                
                if (rxDecPowerCfg[cfgIdx].decPowerControl == ADI_ADRV903X_DEC_POWER_MEAS_OFF)
                {
                    /* Manual(SPI) control, user disables the dec power measurement */
                    decPowerEnable = 0U;
                    decPowerAgcSelect = 0U;
                    decPowerClockEnable = 0U;
                }
                else if (rxDecPowerCfg[cfgIdx].decPowerControl == ADI_ADRV903X_DEC_POWER_MEAS_ON)
                {
                    /* Manual(SPI) control, user enables the dec power measurement */
                    decPowerEnable = 1U;
                    decPowerAgcSelect = 0U;
                    decPowerClockEnable = 1U;
                }
                else
                {
                    /* AGC controls the decimated power, decPowerEnable is a don't care for this case */
                    decPowerEnable = 0U;
                    decPowerAgcSelect = 1U;
                    decPowerClockEnable = 1U;
                }

                /* 1 - Clock configuration for decimated power block */
                recoveryAction = adrv903x_RxDig_Hb2OutClkDivideRatio_BfGet(device, NULL, rxDigBaseAddr, &hb2OutClkDivRatio);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 out clk divide ratio");
                    goto cleanup;
                }

                recoveryAction = adrv903x_RxDig_DecpwrClkDivideRatio_BfSet(device, NULL, rxDigBaseAddr, hb2OutClkDivRatio);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power clk divide ratio");
                    goto cleanup;
                }

                recoveryAction = adrv903x_RxDig_DecpwrClkEnable_BfSet(device, NULL, rxDigBaseAddr, decPowerClockEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set dec power clk enable bit");
                    goto cleanup;
                }

                /* 2 - Main path Dec power configuration */
                if (ADRV903X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV903X_DEC_POWER_MAIN_PATH_MEAS_BLOCK))
                {
                    /* Disable Dec Power While Changing Configs */
                    recoveryAction = adrv903x_RxFuncs_DecPowerEnable_BfSet(device, NULL, rxFuncsBaseAddr, 0U);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerDataSel_BfSet(device, NULL, rxFuncsBaseAddr, rxDecPowerCfg[cfgIdx].powerInputSelect);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write signal source for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerMeasurementDuration_BfSet(device, NULL, rxFuncsBaseAddr, rxDecPowerCfg[cfgIdx].powerMeasurementDuration);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write decimated power measurement duration");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfSet(device, NULL, rxFuncsBaseAddr, decPowerAgcSelect);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerTddModeEnable_BfSet(device, NULL, rxFuncsBaseAddr, TDD_MODE_ENABLE_DEFAULT_VAL);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerStartDelayCounter_BfSet(device, NULL, rxFuncsBaseAddr, TDD_MODE_DELAY_DEFAULT_VAL);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv903x_RxFuncs_DecPowerEnable_BfSet(device, NULL, rxFuncsBaseAddr, decPowerEnable);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                        goto cleanup;
                    }
                }

                /* 3 - Configure Band Decimated power blocks */
                ddcMask = 0U;
                if (ADRV903X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV903X_DEC_POWER_BAND_A_MEAS_BLOCK))
                {
                    ddcMask |= (1U << (uint32_t)ADI_ADRV903X_RX_DDC_BAND0);
                }
                
                if (ADRV903X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV903X_DEC_POWER_BAND_B_MEAS_BLOCK))
                {
                    ddcMask |= (1U << (uint32_t)ADI_ADRV903X_RX_DDC_BAND1);
                }
                
                for (ddcSelection = 0U; ddcSelection < NUMBER_OF_RX_DDC; ddcSelection++)
                {
                    if (ADRV903X_BF_EQUAL(ddcMask, (uint32_t)(1U << ddcSelection)))
                    {
                        /* Get Rx DDC Bitfield Address */
                        recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device,
                                                                          (adi_adrv903x_RxChannels_e)(chanSel),
                                                                          (adi_adrv903x_RxDdcs_e)ddcSelection,
                                                                          &ddcAddr);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Rx ddc bit field address");
                            goto cleanup;
                        }

                        recoveryAction = adrv903x_RxDdc_DecPowerMeasurementDuration_BfSet(device, NULL, ddcAddr, rxDecPowerCfg[cfgIdx].powerMeasurementDuration);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write decimated power measurement duration");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv903x_RxDdc_DecPowerEnable_BfSet(device, NULL, ddcAddr, decPowerEnable);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv903x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet(device, NULL, ddcAddr, decPowerAgcSelect);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv903x_RxDdc_DecPowerTddModeEnable_BfSet(device, NULL, ddcAddr, TDD_MODE_ENABLE_DEFAULT_VAL);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv903x_RxDdc_DecPowerStartDelayCounter_BfSet(device, NULL, ddcAddr, TDD_MODE_DELAY_DEFAULT_VAL);
                        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                            goto cleanup;
                        }
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerCfgGet(adi_adrv903x_Device_t * const device,
                                                                     const adi_adrv903x_RxChannels_e rxChannel,
                                                                     const adi_adrv903x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                     adi_adrv903x_RxDecimatedPowerCfg_t * const rxDecPowerCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    adrv903x_BfRxDdcChanAddr_e ddcAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t decPowerEnable = 0U;
    uint8_t decPowerAgcSelect = 0U;
    adi_adrv903x_RxDdcs_e ddcSelection = ADI_ADRV903X_RX_DDC_BAND0;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxDecPowerCfg, cleanup);
    
    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }
    
    if ((decPowerBlockSelection != ADI_ADRV903X_DEC_POWER_MAIN_PATH_MEAS_BLOCK) &&
        (decPowerBlockSelection != ADI_ADRV903X_DEC_POWER_BAND_A_MEAS_BLOCK) &&
        (decPowerBlockSelection != ADI_ADRV903X_DEC_POWER_BAND_B_MEAS_BLOCK))
    {
        /* Invalid decimated power block selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, decPowerBlockSelection, "Invalid decimated power block selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        goto cleanup;
    }

    rxDecPowerCfg->rxChannelMask = (uint32_t)rxChannel;
    
    if (decPowerBlockSelection == ADI_ADRV903X_DEC_POWER_MAIN_PATH_MEAS_BLOCK)
    {
        recoveryAction = adrv903x_RxFuncs_DecPowerDataSel_BfGet(device, NULL, rxFuncsBaseAddr, &rxDecPowerCfg->powerInputSelect);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read signal source for decimated power measurement");
            goto cleanup;
        }

        recoveryAction = adrv903x_RxFuncs_DecPowerMeasurementDuration_BfGet(device, NULL, rxFuncsBaseAddr, &rxDecPowerCfg->powerMeasurementDuration);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read decimated power measurement duration");
            goto cleanup;
        }

        recoveryAction = adrv903x_RxFuncs_DecPowerEnable_BfGet(device, NULL, rxFuncsBaseAddr, &decPowerEnable);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
            goto cleanup;
        }

        recoveryAction = adrv903x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfGet(device, NULL, rxFuncsBaseAddr, &decPowerAgcSelect);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
            goto cleanup;
        }

        rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV903X_DEC_POWER_MAIN_PATH_MEAS_BLOCK;
    }
    else
    {
        if (decPowerBlockSelection == ADI_ADRV903X_DEC_POWER_BAND_A_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV903X_RX_DDC_BAND0;
            rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV903X_DEC_POWER_BAND_A_MEAS_BLOCK;
        }
        else
        {
            ddcSelection = ADI_ADRV903X_RX_DDC_BAND1;
            rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV903X_DEC_POWER_BAND_B_MEAS_BLOCK;
        }
        
        recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device, rxChannel, ddcSelection, &ddcAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
            goto cleanup;
        }
        
        rxDecPowerCfg->powerInputSelect = 0U; /* Input selection is not supported for DDC */

        recoveryAction = adrv903x_RxDdc_DecPowerMeasurementDuration_BfGet(device, NULL, ddcAddr, &rxDecPowerCfg->powerMeasurementDuration);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read decimated power measurement duration");
            goto cleanup;
        }
                    
        recoveryAction = adrv903x_RxDdc_DecPowerEnable_BfGet(device, NULL, ddcAddr, &decPowerEnable);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
            goto cleanup;
        }
                    
        recoveryAction = adrv903x_RxDdc_DecPowerEnSpiOrAgcSelect_BfGet(device, NULL, ddcAddr, &decPowerAgcSelect);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
            goto cleanup;
        }
    }

    if ((decPowerEnable == 0U) && (decPowerAgcSelect == 0U))
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV903X_DEC_POWER_MEAS_OFF;
    }
    else if ((decPowerEnable == 1U) && (decPowerAgcSelect == 0U))
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV903X_DEC_POWER_MEAS_ON;
    }
    else if (decPowerAgcSelect == 1U)
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV903X_DEC_POWER_AGC_MEAS;
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid dec power measurement configuration is detected");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ORxDecimatedPowerCfgSet(adi_adrv903x_Device_t * const device,
                                                                      adi_adrv903x_ORxDecimatedPowerCfg_t orxDecPowerCfg[],
                                                                      const uint32_t numOfDecPowerCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    static const adi_adrv903x_RxChannels_e ORX_CHANNEL_ARR[] = { ADI_ADRV903X_ORX0, ADI_ADRV903X_ORX1 };
    static const uint32_t ORX_CHANNEL_ARR_SIZE               = sizeof(ORX_CHANNEL_ARR) / sizeof(ORX_CHANNEL_ARR[0U]);
    static const uint8_t HB2_INPUT_SELECT  = 0U;
    static const uint8_t HB2_OUTPUT_SELECT = 1U;
    
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint8_t clkDivRatio = 0U;
    
    adrv903x_BfOrxDigChanAddr_e orxDigBaseAddr = ADRV903X_BF_SLICE_ORX_0__ORX_DIG;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxDecPowerCfg, cleanup);

    if (numOfDecPowerCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfDecPowerCfgs, "Invalid Number of Dec Power Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        recoveryAction = adrv903x_OrxDecPowerCfgRangeCheck(device, &orxDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Orx Dec power range check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ORX_CHANNEL_ARR_SIZE; chanIdx++)
        {
            if ((orxDecPowerCfg[cfgIdx].orxChannelMask & (uint32_t)ORX_CHANNEL_ARR[chanIdx]) == (uint32_t)ORX_CHANNEL_ARR[chanIdx])
            {
                recoveryAction = adrv903x_OrxBitfieldAddressGet(device, ORX_CHANNEL_ARR[chanIdx], &orxDigBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid ORx Channel used to determine orx dig address");
                    goto cleanup;
                }

                /* Read clock divider value to be configured for measurement block based upon input source selection */
                if (orxDecPowerCfg[cfgIdx].powerInputSelect == HB2_INPUT_SELECT)
                {
                    recoveryAction = adrv903x_OrxDig_Hb2InClkDivideRatio_BfGet(device,
                                                                               NULL, 
                                                                               orxDigBaseAddr,
                                                                               &clkDivRatio);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb2 input clk divide ratio");
                        goto cleanup;
                    }
                }
                else if (orxDecPowerCfg[cfgIdx].powerInputSelect == HB2_OUTPUT_SELECT)
                {
                    recoveryAction = adrv903x_OrxDig_Hb2OutClkDivideRatio_BfGet(device,
                                                                                NULL,
                                                                                orxDigBaseAddr,
                                                                                &clkDivRatio);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb2 output clk divide ratio");
                        goto cleanup;
                    }
                }
                else
                {
                    recoveryAction = adrv903x_OrxDig_Hb1OutClkDivideRatio_BfGet(device,
                                                                                NULL,
                                                                                orxDigBaseAddr,
                                                                                &clkDivRatio);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading hb1 output clk divide ratio");
                        goto cleanup;
                    }
                }
                
                /* Set clk divide ratio for power measurement block */
                recoveryAction = adrv903x_OrxDig_PnpClkDivideRatio_BfSet(device,
                                                                         NULL,
                                                                         orxDigBaseAddr,
                                                                         clkDivRatio);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Pnp clock divide ratio");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_OrxDig_PwrMeasInputSel_BfSet(device,
                                                                       NULL,
                                                                       orxDigBaseAddr,
                                                                       orxDecPowerCfg[cfgIdx].powerInputSelect);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement input selection");
                    goto cleanup;
                }

                recoveryAction = adrv903x_OrxDig_DecpwrCount_BfSet(device,
                                                                   NULL,
                                                                   orxDigBaseAddr,
                                                                   orxDecPowerCfg[cfgIdx].powerMeasurementDuration);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_OrxDig_StreamprocPnpClkEnable_BfSet(device,
                                                                              NULL,
                                                                              orxDigBaseAddr,
                                                                              orxDecPowerCfg[cfgIdx].measurementEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement clk enable bit");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_OrxDig_PwrMeasEnable_BfSet(device,
                                                                     NULL,
                                                                     orxDigBaseAddr,
                                                                     orxDecPowerCfg[cfgIdx].measurementEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement enable bit");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ORxDecimatedPowerCfgGet(adi_adrv903x_Device_t * const device,
                                                                      const adi_adrv903x_RxChannels_e orxChannel,
                                                                      adi_adrv903x_ORxDecimatedPowerCfg_t * const orxDecPowerCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfOrxDigChanAddr_e orxDigBaseAddr = ADRV903X_BF_SLICE_ORX_0__ORX_DIG;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, orxDecPowerCfg, cleanup);


    if ((orxChannel != ADI_ADRV903X_ORX0) &&
        (orxChannel != ADI_ADRV903X_ORX1))
    {
        /* Invalid Orx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, orxChannel, "Invalid Orx channel selection");
        goto cleanup;
    }

    /* Read back the configuration of selected channel */
    recoveryAction = adrv903x_OrxBitfieldAddressGet(device, orxChannel, &orxDigBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid ORx Channel used to determine orx dig address");
        goto cleanup;
    }

    recoveryAction = adrv903x_OrxDig_PwrMeasInputSel_BfGet(device,
                                                           NULL,
                                                           orxDigBaseAddr,
                                                           &orxDecPowerCfg->powerInputSelect);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement input selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_OrxDig_DecpwrCount_BfGet(device,
                                                       NULL,
                                                       orxDigBaseAddr,
                                                       &orxDecPowerCfg->powerMeasurementDuration);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement duration");
        goto cleanup;
    }
                
    recoveryAction = adrv903x_OrxDig_PwrMeasEnable_BfGet(device,
                                                         NULL,
                                                         orxDigBaseAddr,
                                                         &orxDecPowerCfg->measurementEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement enable bit");
        goto cleanup;
    }

    orxDecPowerCfg->orxChannelMask = (uint32_t)orxChannel;
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxDecimatedPowerGet(adi_adrv903x_Device_t * const device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel,
                                                                  const adi_adrv903x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                  uint8_t * const powerReadBack)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_FUNCS;
    adrv903x_BfRxDdcChanAddr_e ddcAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;
    adi_adrv903x_RxDdcs_e ddcSelection = ADI_ADRV903X_RX_DDC_BAND0;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, powerReadBack, cleanup);

    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7)
                                         &&
        (rxChannel != ADI_ADRV903X_ORX0) && /* ORx only supported on ADRV903X */
        (rxChannel != ADI_ADRV903X_ORX1)
       )
    {
        /* Invalid channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid channel selection");
        goto cleanup;
    }

    if ((rxChannel == ADI_ADRV903X_ORX0) ||
        (rxChannel == ADI_ADRV903X_ORX1))
    {
        adrv903x_BfOrxDigChanAddr_e orxDigBaseAddr = ADRV903X_BF_SLICE_ORX_0__ORX_DIG;

        /* ORx channel is selected */
        recoveryAction = adrv903x_OrxBitfieldAddressGet(device, rxChannel, &orxDigBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid ORx Channel used to determine orx dig address");
            goto cleanup;
        }

        recoveryAction = adrv903x_OrxDig_DecpwrValueReadback_BfSet(device,
                                                                   NULL,
                                                                   orxDigBaseAddr,
                                                                   1U);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
            goto cleanup;
        }

        recoveryAction = adrv903x_OrxDig_Decpwr_BfGet(device,
                                                      NULL,
                                                      orxDigBaseAddr,
                                                      powerReadBack);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated power for selected Orx channel");
            goto cleanup;
        }
    }
    else if (decPowerBlockSelection == ADI_ADRV903X_DEC_POWER_MAIN_PATH_MEAS_BLOCK)
    {
        recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                            rxChannel,
                                                            &rxFuncsBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Rx Channel used to determine rx func address");
            goto cleanup;
        }

        recoveryAction = adrv903x_RxFuncs_DecPowerReadback_BfSet(device,
                                                                 NULL,
                                                                 rxFuncsBaseAddr,
                                                                 1U);
        if(recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
            goto cleanup ;
        }

        recoveryAction = adrv903x_RxFuncs_DecPowerValue_BfGet(device,
                                                              NULL,
                                                              rxFuncsBaseAddr,
                                                              powerReadBack);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading main path decimated power for selected Rx channel");
            goto cleanup;
        }
    }
    else
    {
        if (decPowerBlockSelection == ADI_ADRV903X_DEC_POWER_BAND_A_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV903X_RX_DDC_BAND0;
        
        }
        else if (decPowerBlockSelection == ADI_ADRV903X_DEC_POWER_BAND_B_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV903X_RX_DDC_BAND1;
        }
        else
        {
            /* Invalid decPowerBlockSelection selection */
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid decPowerBlockSelection selection");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device,
                                                          rxChannel,
                                                          ddcSelection,
                                                          &ddcAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_RxDdc_DecPowerValueReadback_BfSet(device,
                                                                    NULL,
                                                                    ddcAddr,
                                                                    1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_RxDdc_DecPowerValue_BfGet(device,
                                                            NULL,
                                                            ddcAddr,
                                                            powerReadBack);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading band decimated power for selected Rx channel");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxAttenSet(adi_adrv903x_Device_t* const device,
                                                          const uint32_t channelMask,
                                                          const uint8_t attenDb)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0U;
    uint8_t trmAtten = 0U;
    uint8_t trmAttenBwCapEn = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    if (channelMask > MAX_ORX_CHANNEL_MASK)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            channelMask,
            "Invalid channelMask");
        goto cleanup;
    }
    
    if (attenDb > ADI_ADRV903X_MAX_ORX_ATTEN_DB)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            attenDb,
            "attenDb outside maximum range");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_ORxAttenDbToRegValues(device, attenDb, &trmAtten, &trmAttenBwCapEn);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adrv903x_ORxAttenDbToRegValues failed");
        goto cleanup;
    }
        
    /* for each channel in channelMask */
    for (chanId = 0U; chanId <= MAX_ORX_CHANNEL_ID; chanId++)
    {
        if ((channelMask & (1 << chanId)) == 0)
        {
            /* Skip this channel */
            continue;
        }
        
        /* ensure channel's trm_atten_pd is always set 0 */
        recoveryAction = adrv903x_ActrlOrxWestRegmap_TrmAttenPd_BfSet(device, NULL, chanIdToOrxWestRegmapChanAddr[chanId], 0U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenPd bitfield");
            goto cleanup;
        }        
        
        /* ensure channel's trm_cm_cap_en is always set 1 */
        recoveryAction = adrv903x_ActrlOrxWestRegmap_TrmAttenCmCapEn_BfSet(device, NULL, chanIdToOrxWestRegmapChanAddr[chanId], 1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenCmCapEn bitfield");
            goto cleanup;
        }        
        
        /* Set this ORx channel's trm_atten bitfield */
        recoveryAction = adrv903x_ActrlOrxWestRegmap_TrmAtten_BfSet(device,
                                                                    NULL,
                                                                    chanIdToOrxWestRegmapChanAddr[chanId],
                                                                    (adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e)trmAtten);
        
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ORX attenuator trimmer set issue");
            goto cleanup;
        }

        /* Set this ORx channel's trm_atten_bw_cap - NOTE: the register is called trm_atten_spare2 */
        recoveryAction = adrv903x_ActrlOrxWestRegmap_TrmAttenSpare2_BfSet(device,
                                                                          NULL,
                                                                          chanIdToOrxWestRegmapChanAddr[chanId],
                                                                          trmAttenBwCapEn);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenSpare2 bitfield");
            goto cleanup;
        }        
    }
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);    
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_OrxAttenGet(adi_adrv903x_Device_t* const  device,
                                                          const uint8_t                 channelId,
                                                          uint8_t* const                attenDb)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e trmAtten = ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_0DB;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    if (channelId > MAX_ORX_CHANNEL_ID)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                channelId,
                                "Invalid channelId");
        goto cleanup;
    }

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, attenDb, cleanup);
    
    /* Get the channel's attenuation */
    recoveryAction = adrv903x_ActrlOrxWestRegmap_TrmAtten_BfGet(device,
                                                                NULL,
                                                                chanIdToOrxWestRegmapChanAddr[channelId],
                                                                &trmAtten);
    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV903X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "ORX attenuator trimmer get issue");
        goto cleanup;
    }

    recoveryAction = adrv903x_ORxTrmAttenToDb(device, trmAtten, attenDb);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV903X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "adrv903x_ORxTrmAttenToDb failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainCtrlModeSet(adi_adrv903x_Device_t* const            device,
                                                                const adi_adrv903x_RxGainCtrlModeCfg_t  gainCtrlModeCfg[],
                                                                const uint32_t                          arraySize)
{
        adi_adrv903x_ErrAction_e     recoveryAction    = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxChannelBitfieldAddr;
    uint32_t                     gainCtrlModeIndex = 0U;
    uint32_t                     rxChannelIndex    = 0U;
    uint32_t                     rxChannel         = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, gainCtrlModeCfg, cleanup);

    if (arraySize == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arraySize, "Invalid number of gain control mode array");
        goto cleanup;
    }

    /* check each item in the input array, make sure the parameters are valid */
    for (gainCtrlModeIndex = 0; gainCtrlModeIndex < arraySize; gainCtrlModeIndex++)
    {
        /* rxChannelMask should larger than 0 and less than 0x100 */
        if ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask == 0U) ||
            ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask & (~(uint32_t)ADI_ADRV903X_RX_MASK_ALL)) != 0U))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask, "Invalid Rx channel mask");
            goto cleanup;
        }

        /* gainCtrlMode just support MGC and AGC type */
        if ((gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode != ADI_ADRV903X_MGC) &&
            (gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode != ADI_ADRV903X_AGC))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode, "Invalid Rx channel gain control mode");
            goto cleanup;
        }
    }

    for (gainCtrlModeIndex = 0U; gainCtrlModeIndex < arraySize; gainCtrlModeIndex++)
    {
        for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV903X_MAX_RX_ONLY; rxChannelIndex++)
        {
            rxChannel = 1U << rxChannelIndex;
            if ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask & rxChannel) == rxChannel)
            {
                /* get the Rx channel function address */
                recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                                    (adi_adrv903x_RxChannels_e)rxChannel,
                                                                    &rxChannelBitfieldAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel function address");
                    goto cleanup;
                }

                /* set gain control mode */
                recoveryAction = adrv903x_RxFuncs_AgcSetup_BfSet(device,
                                                                 NULL,
                                                                 rxChannelBitfieldAddr,
                                                                 (uint8_t)gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx channel gain control mode set failure");
                    goto cleanup;
                }
            }
        }
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxGainCtrlModeGet(adi_adrv903x_Device_t* const     device,
                                                                const adi_adrv903x_RxChannels_e  rxChannel,
                                                                adi_adrv903x_RxGainCtrlMode_e*   gainCtrlMode)
{
        adi_adrv903x_ErrAction_e     recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxFuncsChanAddr_e rxChannelBitfieldAddr;
    uint8_t                      value          = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, gainCtrlMode, cleanup);

    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }

    /* get the Rx channel function address */
    recoveryAction = adrv903x_RxFuncsBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &rxChannelBitfieldAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel function address");
        goto cleanup;
    }

    /* set gain control mode */
    recoveryAction = adrv903x_RxFuncs_AgcSetup_BfGet(device,
                                                     NULL,
                                                     rxChannelBitfieldAddr,
                                                     &value);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx channel gain control mode get failure");
        goto cleanup;
    }

    /* update gain control mode information */
    *gainCtrlMode = (adi_adrv903x_RxGainCtrlMode_e)value;

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTempGainCompSet(adi_adrv903x_Device_t* const  device,
                                                                const uint32_t                rxChannelMask,
                                                                const int8_t                  gainValue)
{
        adi_adrv903x_ErrAction_e    recoveryAction        = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxDdcChanAddr_e  rxChannelBitfieldAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;
    uint32_t                    rxChannelIndex        = 0U;
    uint32_t                    rxChannel             = 0U;
    uint32_t                    ddcBand               = 0U;
    uint8_t                     value                 = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    /* rxChannelMask should larger than 0 and less than 0x100 */
    if ((rxChannelMask == 0U) ||
        ((rxChannelMask & (~(uint32_t)ADI_ADRV903X_RX_MASK_ALL)) != 0U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel mask");
        goto cleanup;
    }

    /* check that gainValue is within allowable range */
    if ((gainValue < -63) ||
        (gainValue > 63))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainValue, "Invalid Rx channel temperature gain value");
        goto cleanup;
    }
    else
    {
        /* register expects sign-magnitude value hence if value is negative, need to convert out of two's compliment and set the sign bit (bit 7) to 1 */
        if (gainValue < 0)
        {
            value = (uint8_t)((gainValue * (-1)) | 0x40);
        }
        else
        {
            value = (uint8_t)gainValue;
        }
    }

    for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV903X_MAX_RX_ONLY; rxChannelIndex++)
    {
        rxChannel = 1U << rxChannelIndex;
        if ((rxChannelMask & rxChannel) == rxChannel)
        {
            /* if an Rx channel mask is set, resolve the corresponding Rx DDC Bitfield Addresses (2 DDCs per channel)*/
            for (ddcBand = 0U; ddcBand < ADI_ADRV903X_MAX_RX_DDC_BANDS; ddcBand++)
            {
                recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device, 
                                                                  (adi_adrv903x_RxChannels_e)rxChannel,
                                                                  (adi_adrv903x_RxDdcs_e)ddcBand,
                                                                  &rxChannelBitfieldAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ddcBand, "get Rx channel DDC base address failure");
                    goto cleanup;
                }

                /* set temperature gain value */
                recoveryAction = adrv903x_RxDdc_RxTempGainComp_BfSet(device,
                                                                     NULL,
                                                                     rxChannelBitfieldAddr,
                                                                     value);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "set Rx channel temperature gain value failure");
                    goto cleanup;
                }
            }
        }
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTempGainCompGet(adi_adrv903x_Device_t* const     device,
                                                                const adi_adrv903x_RxChannels_e  rxChannel,
                                                                int8_t* const                    gainValue)
{
        adi_adrv903x_ErrAction_e    recoveryAction        = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxDdcChanAddr_e  rxChannelBitfieldAddr = ADRV903X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t                     value                 = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, gainValue, cleanup);

    if ((rxChannel != ADI_ADRV903X_RX0) &&
        (rxChannel != ADI_ADRV903X_RX1) &&
        (rxChannel != ADI_ADRV903X_RX2) &&
        (rxChannel != ADI_ADRV903X_RX3) &&
        (rxChannel != ADI_ADRV903X_RX4) &&
        (rxChannel != ADI_ADRV903X_RX5) &&
        (rxChannel != ADI_ADRV903X_RX6) &&
        (rxChannel != ADI_ADRV903X_RX7))
    {
        /* Invalid Rx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel selection");
        goto cleanup;
    }

    /* get the Rx channel DDC0 address */
    recoveryAction = adrv903x_RxDdcBitfieldAddressGet(device, 
                                                      rxChannel,
                                                      ADI_ADRV903X_RX_DDC_BAND0,
                                                      &rxChannelBitfieldAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "get Rx channel DDC0 address failure");
        goto cleanup;
    }

    /* get temperature gain value */
    recoveryAction = adrv903x_RxDdc_RxTempGainComp_BfGet(device,
                                                         NULL,
                                                         rxChannelBitfieldAddr,
                                                         &value);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "get Rx channel temperature gain value failure");
        goto cleanup;
    }

    /* convert from 7-bit sign magnitude format to 8-bit two's complement 
     * sign magnitude format: sign bit + data bit (amplitude value), not use complement code
     */
    if ((value & 0x40U) > 0)
    {
        /* handle the negative case */
        *gainValue = (int8_t)(value & 0x3FU) * (-1);
    }
    else
    {
        /* handle the positive case */
        *gainValue = (int8_t)value;
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTestDataSet(adi_adrv903x_Device_t* device,
                                                            const uint32_t rxChannelMask,
                                                            const adi_adrv903x_RxTestDataCfg_t* const rxTestDataCfg)   
{
        /* The full range of generator selection options are not made available via the API we just use these
     * two internally */
    uint8_t rxChannelIndex = 0U;
    uint32_t chanSel = 0U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxDigChanAddr_e rxDigBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_DIG;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxTestDataCfg, cleanup);
    
    if ((rxChannelMask == 0U) ||
        ((rxChannelMask > (uint32_t) ADI_ADRV903X_RX_MASK_ALL)))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel mask");
        goto cleanup;
    }

    for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV903X_MAX_RX_ONLY; rxChannelIndex++)
    {
        chanSel = 1U << rxChannelIndex;
        adi_adrv903x_RxChannels_e rxChannel = (adi_adrv903x_RxChannels_e)(chanSel);
        if ((rxChannelMask & (uint32_t) rxChannel) == 0)
        {
            /* This channel is not set in the rxChannelMask arg - skip it */
            continue;
        }
            
        recoveryAction = adrv903x_RxBitfieldAddressGet(device, rxChannel, &rxDigBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx dig address");
            goto cleanup;
        }
    
        if (rxTestDataCfg->enable == 0U)
        {
            /* Disable the test signal */
            
            /* Set register-control to disable test signal*/
            recoveryAction = adrv903x_RxDig_AdcTestGenEnSpi_BfSet(device, NULL, rxDigBaseAddr, 0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator enable SPI.");
                goto cleanup;
            }                

            /* Set test signal enablement to be controlled by register (not GPIO) */
            recoveryAction = adrv903x_RxDig_AdcTestGenEnSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_EN_SEL_SPI);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable Rx ADC test signal. Could not set ADC test generator enable select.");
                goto cleanup;
            }
        }
        else
        {
            /* Enable the test signal */
            
            /* First range check the freq */
            if (rxTestDataCfg->sineFreq > ADI_ADRV903X_FS_DIV_5 || rxTestDataCfg->sineFreq < ADI_ADRV903X_FS_DIV_40)
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxTestDataCfg->sineFreq, "Invalid frequency");
                goto cleanup;   
            }
        
            /* Always use full-scale sine - other signal types are not exposed via the API */
            recoveryAction = adrv903x_RxDig_AdcTestGenSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_SEL_SINE);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator select.");
                goto cleanup;
            }
            
            /* Always set the test signal frequency relative to ADC clock */            
            recoveryAction = adrv903x_RxDig_AdcTestClkSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_CLK_ADC);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable ADC test generator clock select.");
                goto cleanup;
            }
                                
            /* Set the test signal frequency */
            recoveryAction = adrv903x_RxDig_AdcTestGenSineFreq_BfSet(device, NULL, rxDigBaseAddr, (uint8_t) rxTestDataCfg->sineFreq);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator sine freq.");
                goto cleanup;
            }
            
            /* Set register-control to enable test signal*/
            recoveryAction = adrv903x_RxDig_AdcTestGenEnSpi_BfSet(device, NULL, rxDigBaseAddr, 1U); 
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator enable SPI.");
                goto cleanup;
            }
            
            /* Set test signal enablement to be controlled by register (not GPIO) */
            recoveryAction = adrv903x_RxDig_AdcTestGenEnSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_EN_SEL_SPI);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set test generator enable select.");
                goto cleanup;
            }            
        }
    }
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxTestDataGet(adi_adrv903x_Device_t* device,
                                                            const adi_adrv903x_RxChannels_e rxChannel,
                                                            adi_adrv903x_RxTestDataCfg_t* const rxTestDataCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfRxDigChanAddr_e rxDigBaseAddr = ADRV903X_BF_SLICE_RX_0__RX_DIG;
    
    /* Hold bitfield values read back */
    uint8_t genEnSel = 0U; /* Set adc_test_gen_en source to SPI */
    uint8_t genSel   = 0U; /* Test signal type: Full scale, bypass etc */
    uint8_t clkSel   = 0U; /* Send test data into ADC iface FIFO */
    uint8_t genEnSpi = 0U; /* Start sending test data */  
    uint8_t tmpByte  = 0U; /* Temp storage */ 
    adi_adrv903x_AdcTestSineFreq_e genSineFreq = ADI_ADRV903X_FS_DIV_40; /* Test signal frequency */ 
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxTestDataCfg, cleanup);
        
    recoveryAction = adrv903x_RxBitfieldAddressGet(device, rxChannel, &rxDigBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel used to determine rx dig address");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_RxDig_AdcTestGenSel_BfGet(device, NULL, rxDigBaseAddr, &genSel);        
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator select.");
        goto cleanup;        
    }
    
    recoveryAction = adrv903x_RxDig_AdcTestClkSel_BfGet(device, NULL, rxDigBaseAddr, &clkSel);    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator clock select.");
        goto cleanup;
    }

    recoveryAction = adrv903x_RxDig_AdcTestGenEnSel_BfGet(device, NULL, rxDigBaseAddr, &genEnSel);    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator enable select.");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_RxDig_AdcTestGenSineFreq_BfGet(device, NULL, rxDigBaseAddr, &tmpByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator sine freq.");
        goto cleanup;
    }

    genSineFreq = (adi_adrv903x_AdcTestSineFreq_e)tmpByte;
    
    recoveryAction = adrv903x_RxDig_AdcTestGenEnSpi_BfGet(device, NULL, rxDigBaseAddr, &genEnSpi);    
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator enable SPI.");
        goto cleanup;
    }
    
    /* Return 'enabled' only if the settings are exactly as the setter function would enable */
    if ((genSel == ADC_TEST_GEN_SEL_SINE) &&
        (clkSel != 0U) &&
        (genEnSel == ADC_TEST_GEN_EN_SEL_SPI) &&
        (genEnSpi == 1U))
    {
        rxTestDataCfg->enable = 1U;
        rxTestDataCfg->sineFreq = genSineFreq;        
    }
    else
    {
        rxTestDataCfg->enable = 0U;        
    }
        
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxLoPowerDownSet(adi_adrv903x_Device_t* const    device, 
                                                               const adi_adrv903x_RxChannels_e rxChannelMask,
                                                               const uint8_t                   enable)
{
        adi_adrv903x_ErrAction_e recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t rxScratch7Addr                     = 0U;
    uint8_t  i                                  = 0U;

    static const uint32_t rxChanToScratch7Addr[ADI_ADRV903X_MAX_RX_ONLY] = {
        ADRV903X_ADDR_RX0_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX1_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX2_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX3_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX4_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX5_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX6_STREAM_SCRATCH7,
        ADRV903X_ADDR_RX7_STREAM_SCRATCH7,
    };

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    if ((rxChannelMask < ADI_ADRV903X_RX0) || (rxChannelMask >= ADI_ADRV903X_ORX0))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel passed - must be from Rx0 up to Rx7.");
        goto cleanup;
    }

    if ((enable != ADI_DISABLE) && (enable != ADI_ENABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enable, "Invalid enable value passed - must be ADI_ENABLE or ADI_DISABLE");
        goto cleanup;
    }
    
    for (i = 0U; i < ADI_ADRV903X_MAX_RX_ONLY; i++)
    {
        if (((uint32_t)rxChannelMask & (1U << i)) == (1U << i))
        {
            rxScratch7Addr = rxChanToScratch7Addr[i];

            /* Stream scratch pad bit is set to *skip* power down so is logical inversion of *enable* power down */
            recoveryAction = adi_adrv903x_Register32Write(  device,
                                                            NULL,
                                                            rxScratch7Addr,
                                                            (uint32_t)(!enable),
                                                            1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register.");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSpurBaseBandFreqSet(adi_adrv903x_Device_t* const     device, 
                                                                    const adi_adrv903x_RxChannels_e  rxChannelMask,
                                                                    const int32_t                    bbFreqKhz, 
                                                                    const uint8_t                    enable)
{
        adi_adrv903x_ErrAction_e                recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_RxSpurFreqSet_t            rxSpurConfig;
    adi_adrv903x_RxSpurFreqConfigResp_t     cmdRsp;
    adrv903x_CpuCmdStatus_e                 cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                 cpuErrorCode    = ADRV903X_CPU_NO_ERROR;
    uint32_t                                cpuTypeIdx      = 0U;

    /* Check Device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
        
    if ((enable != ADI_DISABLE) && (enable != ADI_ENABLE))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enable, "Invalid enable value passed - must be ADI_ENABLE or ADI_DISABLE");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(&rxSpurConfig, 0, sizeof(rxSpurConfig)); 
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    /* Prepare the command Payload */
    rxSpurConfig.rxChannelMask    = ADRV903X_HTOCL(rxChannelMask);
    rxSpurConfig.baseBandFreqKHz  = ADRV903X_HTOCL(bbFreqKhz);
    rxSpurConfig.enable           = enable;

    /* For each CPU, send the command, wait for a response, and process any errors. */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and Receive Response */
        recoveryAction = adrv903x_CpuCmdSend(device, 
                                            (adi_adrv903x_CpuType_e)cpuTypeIdx, 
                                            ADRV903X_LINK_ID_0, 
                                            ADRV903X_CPU_CMD_RX_SPUR_BASE_BAND_FREQ_SET, 
                                            (void*)&rxSpurConfig, 
                                            sizeof(rxSpurConfig),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)ADRV903X_CTOHL(cmdRsp.status), cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_RxSpurBaseBandFreqGet(adi_adrv903x_Device_t* const               device,
                                                                    const adi_adrv903x_RxChannels_e            rxChannelMask,
                                                                    adi_adrv903x_RxSpurFreqConfigResp_t* const rxSpurRbConfig)
{
        adi_adrv903x_ErrAction_e                recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_RxSpurFreqConfigResp_t     cmdRsp;
    adi_adrv903x_RxSpurFreqGet_t            rxSpurGetCmd; 
    adrv903x_CpuCmdStatus_e                 cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                 cpuErrorCode    = ADRV903X_CPU_NO_ERROR;
    uint32_t                                cpuTypeIdx      = 0U;
    uint8_t                                 channelMaskRet  = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device); 
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, rxSpurRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));
    ADI_LIBRARY_MEMSET(&rxSpurGetCmd, 0, sizeof(rxSpurGetCmd));

    /* rxChannelMask must contain a single channel */
    if (rxChannelMask == ADI_ADRV903X_RX0 ||
        rxChannelMask == ADI_ADRV903X_RX1 ||
        rxChannelMask == ADI_ADRV903X_RX2 ||
        rxChannelMask == ADI_ADRV903X_RX3 ||
        rxChannelMask == ADI_ADRV903X_RX4 ||
        rxChannelMask == ADI_ADRV903X_RX5 ||
        rxChannelMask == ADI_ADRV903X_RX6 ||
        rxChannelMask == ADI_ADRV903X_RX7)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    }
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "rxChannelMask parameter is invalid.");
        goto cleanup; 
    }

    /* Prepare Command Payload */
    rxSpurGetCmd.rxChannelMask = ADRV903X_HTOCL(rxChannelMask);

    /* For each CPU, send the command, and verify the response. */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t)ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(device,
                                            (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_RX_SPUR_BASE_BAND_FREQ_GET,
                                            (void*)&rxSpurGetCmd,
                                            sizeof(rxSpurGetCmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)ADRV903X_CTOHL(cmdRsp.status), cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
       
        channelMaskRet = ADRV903X_CTOHL(cmdRsp.rxChannelMask);
        if (channelMaskRet == rxChannelMask)
        {
            rxSpurRbConfig->baseBandFreqKHz = ADRV903X_CTOHL(cmdRsp.baseBandFreqKHz);
            rxSpurRbConfig->status          = ADRV903X_CTOHL(cmdRsp.status);
            rxSpurRbConfig->enabled         = cmdRsp.enabled;
            break;
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV903X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV903X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
         
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

