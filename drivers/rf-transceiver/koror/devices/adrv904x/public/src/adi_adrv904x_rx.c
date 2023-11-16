/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_rx.c
* \brief Contains Rx features related function implementation defined in
* adi_adrv904x_rx.h
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_rx.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_radioctrl.h"

#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/bf/adrv904x_bf_rx_funcs.h"
#include "../../private/bf/adrv904x_bf_rx_ddc.h"
#include "../../private/bf/adrv904x_bf_rx_dig.h"
#include "../../private/bf/adrv904x_bf_orx_dig.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"

#include "../../private/bf/adrv904x_bf_cddc_funcs.h"
#include "../../private/bf/adrv904x_bf_cddc_hb_dpath.h"
#include "../../private/include/adrv904x_carrier_reconfigure.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_t.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_dfe_pwr_mtr_t.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_RX

#define ADI_ADRV904X_NUM_GPIOS_IN_RX_GAIN_CTRL_PIN_FEATURE 2U
#define ADI_ADRV904X_NUM_GPIOS_IN_RX_EXT_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV904X_NUM_GPIOS_IN_RX_DUALBAND_CTRL_WORD_OUTPUT_FEATURE 8U
#define ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX  8U
#define MAX_ORX_CHANNEL_ID 1U
#define ADC_TEST_GEN_SEL_BYBASS       0x0U
#define ADC_TEST_GEN_SEL_SINE         0x3U
#define ADC_TEST_GEN_EN_SEL_SPI       0x1U
#define ADC_TEST_GEN_CLK_ADC          0x0U



#define MAX_CDDC_NCO_FREQ_KHZ         (8388607) /* 0x7FFFFF */
#define MAX_CDDC_NCO_PHASE_DEGREES    (360)

static const uint8_t MAX_ORX_CHANNEL_MASK = (1U << (MAX_ORX_CHANNEL_ID + 1U)) - 1U;

/* Simple mapping from ORx channel id to corresponding OrxWest regmap addr */
static const adrv904x_BfActrlOrxWestRegmapChanAddr_e chanIdToOrxWestRegmapChanAddr[] = 
{ 
    ADRV904X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP,
    ADRV904X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP
};


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableWrite(adi_adrv904x_Device_t* const        device,
                                                               const uint32_t                      rxChannelMask,
                                                               const uint8_t                       gainIndexOffset,
                                                               const adi_adrv904x_RxGainTableRow_t gainTableRow[],
                                                               const uint32_t                      arraySize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t baseIndex = 0U;
    uint32_t baseAddress[1] = { 0U };
    uint16_t numGainIndicesToWrite = arraySize;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    uint8_t cpuDmaData[((ADI_ADRV904X_MAX_GAIN_TABLE_INDEX - ADI_ADRV904X_MIN_GAIN_TABLE_INDEX) + 1U) * ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX] = { 0U };
    adrv904x_BfRxDdcChanAddr_e ddcBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint32_t chanId = 0U;
    adi_adrv904x_SpiCache_t spiCache = { { 0U }, 0U, 0U, 0U };

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

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxGainTableWriteRangeCheck(device, rxChannelMask, gainIndexOffset, gainTableRow, numGainIndicesToWrite);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableWriteRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Calculate base index for the config*/
    baseIndex = (gainIndexOffset - (numGainIndicesToWrite - 1U));

    /*Format Gain Table Entries*/
    recoveryAction = adrv904x_RxGainTableFormat(device, gainTableRow, &cpuDmaData[0U], numGainIndicesToWrite);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableFormat Issue");
        goto cleanup;
    }

    /*Resolve the RX Channel SRAM to program*/
    /*If Rx Channel Mask Set by user for this config, load Rx gain table*/
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_RX_ONLY; ++chanId)
    {
        adi_adrv904x_RxChannels_e rxChannel = (adi_adrv904x_RxChannels_e) (rxChannelMask & (1U << chanId));
        if (rxChannel > 0U)
        {
            /*Resolve Rx1 Gain Table SRAM load start address*/
            baseAddress[0U] = rxGainTableBaseAddr[chanId] + (baseIndex * ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX);
        
            /*Write to the SRAM via ARM DMA*/
                            recoveryAction = adi_adrv904x_Registers32bOnlyWrite(device,
                                                                    &spiCache,
                                                                    baseAddress[0U],
                                                                    &cpuDmaData[0U],
                                                                    (numGainIndicesToWrite * ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing gain table.");
                    goto cleanup;
                }
                        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, rxChannel, ADI_ADRV904X_RX_DDC_BAND0, &ddcBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxDdc_DigitalGainEnable_BfSet(device, &spiCache, ddcBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while writing Gain Comp Config0 DDC0");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, rxChannel, ADI_ADRV904X_RX_DDC_BAND1, &ddcBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_RxDdc_DigitalGainEnable_BfSet(device, &spiCache, ddcBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue while writing Gain Comp Config0 DDC1");
                goto cleanup;
            }
        }
    }

    recoveryAction = adi_adrv904x_SpiFlush(device, spiCache.data, &spiCache.count);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue After CPU Mailbox Buffer Write");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableRead(adi_adrv904x_Device_t* const    device,
                                                              const adi_adrv904x_RxChannels_e rxChannel,
                                                              const uint8_t                   gainIndexOffset,
                                                              adi_adrv904x_RxGainTableRow_t   gainTableRow[],
                                                              const uint32_t                  arraySize,
                                                              uint16_t* const                 numGainIndicesRead)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t baseAddress = 0U;
    uint32_t maxReadGainIndices = arraySize;
    uint16_t numGainIndicesReadVal = 0U;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    uint8_t cpuAhbData[((ADI_ADRV904X_MAX_GAIN_TABLE_INDEX - ADI_ADRV904X_MIN_GAIN_TABLE_INDEX) + 1U) * ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX];

    ADI_LIBRARY_MEMSET(&cpuAhbData, 0, sizeof(cpuAhbData));

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainTableRow, cleanup);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxGainTableReadRangeCheck(device,
                                                            rxChannel,
                                                            gainIndexOffset,
                                                            gainTableRow,
                                                            maxReadGainIndices);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableReadRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Calculate no. of indices to read and the base address for the config*/
    recoveryAction = adrv904x_RxGainTableReadParamsCompute( device,
                                                            rxChannel,
                                                            maxReadGainIndices,
                                                            gainIndexOffset,
                                                            &numGainIndicesReadVal,
                                                            &baseAddress);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainTableReadParamsCompute Issue");
        goto cleanup;
    }

    /*Read Gain Table Data for the requested channel via ARM DMA*/
    recoveryAction = adi_adrv904x_Registers32bOnlyRead(device,
                                                       NULL,
                                                       baseAddress,
                                                       &cpuAhbData[0U],
                                                       (numGainIndicesReadVal * ADI_ADRV904X_NUM_BYTES_PER_RX_GAIN_INDEX));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error Reading Gain Table ARM DMA");
        goto cleanup;
    }

    /*Parse gain table data obtained in ARM DMA data format to an rx gain table row entry data structure memory*/
    recoveryAction = adrv904x_RxGainTableParse(device, &gainTableRow[0U], &cpuAhbData[0], numGainIndicesReadVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainTableExtCtrlPinsSet(adi_adrv904x_Device_t* const    device,
                                                                        const uint32_t                  rxChannelMask,
                                                                        const uint32_t                  channelEnable)
{
        #define NUM_GPIO_PER_CHANNEL 2U
    
    static const adi_adrv904x_GpioAnaPinSel_e gpioLut[ADI_ADRV904X_MAX_RX_ONLY][NUM_GPIO_PER_CHANNEL] = {
        { ADI_ADRV904X_GPIO_ANA_00, ADI_ADRV904X_GPIO_ANA_01 },
        { ADI_ADRV904X_GPIO_ANA_02, ADI_ADRV904X_GPIO_ANA_03 },
        { ADI_ADRV904X_GPIO_ANA_04, ADI_ADRV904X_GPIO_ANA_05 },
        { ADI_ADRV904X_GPIO_ANA_06, ADI_ADRV904X_GPIO_ANA_07 },
        { ADI_ADRV904X_GPIO_ANA_08, ADI_ADRV904X_GPIO_ANA_09 },
        { ADI_ADRV904X_GPIO_ANA_10, ADI_ADRV904X_GPIO_ANA_11 },
        { ADI_ADRV904X_GPIO_ANA_12, ADI_ADRV904X_GPIO_ANA_13 },
        { ADI_ADRV904X_GPIO_ANA_14, ADI_ADRV904X_GPIO_ANA_15 }
    };
    
    static const adi_adrv904x_GpioSignal_e sigLut[ADI_ADRV904X_MAX_RX_ONLY][NUM_GPIO_PER_CHANNEL] = {
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_0, ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_1 }
    };
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t currentChannelMask = 0U;
    uint8_t chIdx = 0U;
    uint8_t gpioIdx = 0U;
    adi_adrv904x_GpioSignal_e getSig = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t getChanMask = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /*Check that rxChannelMask is valid*/
    if (rxChannelMask > (uint32_t)( ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 |
                                    ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7 ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannelMask,
                                "Invalid Rx Channel mask parameter. Valid Rx channel masks include 0x0-0xff");
        goto cleanup;
    }
    
    /*Check that channelEnable is Valid*/
    if (channelEnable > (uint32_t)( ADI_ADRV904X_RX0 | ADI_ADRV904X_RX1 | ADI_ADRV904X_RX2 | ADI_ADRV904X_RX3 |
                                    ADI_ADRV904X_RX4 | ADI_ADRV904X_RX5 | ADI_ADRV904X_RX6 | ADI_ADRV904X_RX7 ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                channelEnable,
                                "Invalid channelEnable mask parameter. Valid Enable Channel bitmasks 0x0-0xff");
        goto cleanup;
    }
    
    /* If no channels were selected, this is an allowed noop  */
    if (rxChannelMask  == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
    
        /*Iterate through rxChannelMask, enabling/disabling selected channels*/
        for (chIdx = 0U; chIdx < ADI_ADRV904X_MAX_RX_ONLY; chIdx++)
        {
            /* Update currentChannelMask enum var for this chIdx */
            currentChannelMask = (adi_adrv904x_RxChannels_e)(1U << chIdx);
        
            /*Check if this channel should be affected*/
            if ((currentChannelMask & rxChannelMask) != 0U)
            {
            
                /* Enable Analog GPIOs for this channel */
                if ((currentChannelMask & channelEnable) != 0U)
                {   
                    for (gpioIdx = 0U; gpioIdx < NUM_GPIO_PER_CHANNEL; gpioIdx++)
                    {
                        /*Route signals for this channel to Analog GPIOs*/
                        recoveryAction = adrv904x_GpioAnalogSignalSet(  device, 
                                                                        gpioLut[chIdx][gpioIdx],
                                                                        sigLut[chIdx][gpioIdx],
                                                                        0U);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
                        recoveryAction = adrv904x_GpioAnalogSignalGet(  device, 
                                                                        gpioLut[chIdx][gpioIdx],
                                                                       &getSig,
                                                                       &getChanMask);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioAnalogSignalGet Issue");
                            goto cleanup;
                        }
                    
                        /* If the expected signal is currently routed, release/disconnect the pin */
                        if (getSig == sigLut[chIdx][gpioIdx])
                        {
                            recoveryAction = adrv904x_GpioAnalogSignalRelease(  device,
                                                                                gpioLut[chIdx][gpioIdx],
                                                                                sigLut[chIdx][gpioIdx],
                                                                                0U);
                            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxMinMaxGainIndexSet(adi_adrv904x_Device_t* const    device,
                                                                   const uint32_t                  rxChannelMask,
                                                                   const uint8_t                   minGainIndex,
                                                                   const uint8_t                   maxGainIndex)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxMinMaxGainIndexSetRangeCheck(device, rxChannelMask, minGainIndex, maxGainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxMinMaxGainIndexSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Update device gain table min and max gain indices*/
    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX0) == (uint32_t)ADI_ADRV904X_RX0)
    {
        device->devStateInfo.gainIndexes.rx0MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx0MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX1) == (uint32_t)ADI_ADRV904X_RX1)
    {
        device->devStateInfo.gainIndexes.rx1MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx1MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX2) == (uint32_t)ADI_ADRV904X_RX2)
    {
        device->devStateInfo.gainIndexes.rx2MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx2MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX3) == (uint32_t)ADI_ADRV904X_RX3)
    {
        device->devStateInfo.gainIndexes.rx3MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx3MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX4) == (uint32_t)ADI_ADRV904X_RX4)
    {
        device->devStateInfo.gainIndexes.rx4MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx4MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX5) == (uint32_t)ADI_ADRV904X_RX5)
    {
        device->devStateInfo.gainIndexes.rx5MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx5MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX6) == (uint32_t)ADI_ADRV904X_RX6)
    {
        device->devStateInfo.gainIndexes.rx6MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx6MinGainIndex = minGainIndex;
    }

    if ((rxChannelMask & (uint32_t)ADI_ADRV904X_RX7) == (uint32_t)ADI_ADRV904X_RX7)
    {
        device->devStateInfo.gainIndexes.rx7MaxGainIndex = maxGainIndex;
        device->devStateInfo.gainIndexes.rx7MinGainIndex = minGainIndex;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    goto cleanup;   /* Added to deal with Compiler Warning when ADI_ADRV904X_RX_RANGE_CHECK = 0 */

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainSet(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_RxGain_t     rxGain[],
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

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e           halError        = ADI_HAL_ERR_PARAM;
    
    /* Hardcoded 18 byte data array for first spi transaction with placeholders for addresses and data
       dataPageSetup[0]  : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGE_31TO24) address
       dataPageSetup[1]  : 0x26U lsb of paging register (ADRV904X_ADDR_SPI0_PAGE_31TO24) address
       dataPageSetup[2]  : 0x60U first byte of slice register address
       dataPageSetup[3]  : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGE_23TO16) address
       dataPageSetup[4]  : 0x27U lsb of paging register (ADRV904X_ADDR_SPI0_PAGE_23TO16) address
       dataPageSetup[5]  : 0x00U PLACEHOLDER - Next byte of slice register address
       dataPageSetup[6]  : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGE_15TO8) address
       dataPageSetup[7]  : 0x28U lsb of paging register (ADRV904X_ADDR_SPI0_PAGE_15TO8) address
       dataPageSetup[8]  : 0x00U Next byte of slice register address
       dataPageSetup[9]  : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGE_7TO0) address
       dataPageSetup[10] : 0x29U lsb of paging register (ADRV904X_ADDR_SPI0_PAGE_7TO0) address
       dataPageSetup[11] : 0x50U Next byte of slice register address
       dataPageSetup[12] : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGING_CONTROL) address
       dataPageSetup[13] : 0x2AU lsb of paging register (ADRV904X_ADDR_SPI0_PAGING_CONTROL) address
       dataPageSetup[14] : 0x00U Data to write to paging congtrol register
       dataPageSetup[15] : 0x40U msb of calculated address depending on paging config and spi write polarity
       dataPageSetup[16] : 0x01U lsb of calculated address depending on paging config and spi write polarity
       dataPageSetup[17] : 0x00U PLACEHOLDER - will be replaced with gain index value for that given slice
    */
    uint8_t dataPageSetup[] = { 0x01U, 0x26U, rxFuncFirstByte, 0x01U, 0x27U, rxFuncSecondByte, 0x01U, 0x28U, rxFuncThirdByte, 0x01U, 0x29U, rxFuncLastByte, 0x01U, 0x2aU, 0x00U, 0x40U, 0x01U, 0x00U};

    /* Hardcoded 6 byte data array for every subsequent spi transaction
       data[0]  : 0x01U msb of paging register (ADRV904X_ADDR_SPI0_PAGE_31TO24) address
       data[1]  : 0x27U lsb of paging register (ADRV904X_ADDR_SPI0_PAGE_31TO24) address
       data[2]  : 0x00U PLACEHOLDER - Byte of slice register address which can be calculated based on which channel you are dealing with
       data[3]  : 0x40U msb of calculated address depending on paging config and spi write polarity
       data[4]  : 0x01U lsb of calculated address depending on paging config and spi write polarity
       data[5]  : 0x00U PLACEHOLDER - will be replaced with gain index value for that given slice
    */
    uint8_t data[] = { 0x01U, 0x27U, 0x00U, 0x40U, 0x01U, 0x00U };
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0U
    recoveryAction = adrv904x_RxGainSetRangeCheck(device, rxGain, arraySize);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Update manual gain index setting for the requested channel */
    for (configIndex = 0U; configIndex < arraySize; ++configIndex)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(rxGain[configIndex].rxChannelMask, chanSel))
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
                        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
                        ADI_PARAM_ERROR_REPORT(&device->common,
                                               recoveryAction,
                                               dataPageSetup,
                                               "Error writing rx gain value");
                        goto cleanup;
                    }
                    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
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
                        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
                        ADI_PARAM_ERROR_REPORT(&device->common,
                                               recoveryAction,
                                               data,
                                               "Error writing rx gain value");
                        goto cleanup;
                    }
                    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxMgcGainGet(adi_adrv904x_Device_t* const    device,
                                                           const adi_adrv904x_RxChannels_e rxChannel,
                                                           adi_adrv904x_RxGain_t * const   rxGain)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxGainGetRangeCheck(device, rxChannel, rxGain);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainGetRangeCheck Issue");
        goto cleanup;
    }
#endif
    
    /*Check for null rxGain pointer*/
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGain, cleanup);
    
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_RxFuncs_AgcManualGainIndex_BfGet(device,
                                                               NULL,
                                                               rxFuncsBaseAddr,
                                                               &rxGain->gainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Rx manual gain index for channel");
        goto cleanup;
    }

    /* Set rxChannelMask to a valid Rx Channel */
    rxGain->rxChannelMask = (uint32_t)rxChannel;
        
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainGet(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_RxChannels_e rxChannel,
                                                        adi_adrv904x_RxGain_t * const   rxGain)
{
        static const uint8_t LATCH_READBACK_DUMMY_DATA = 0xFFU;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    uint8_t latchReadback = 0;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxGainGetRangeCheck(device, rxChannel, rxGain);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxGainGetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Check for null rxGain pointer*/
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxGain, cleanup);
    
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx funcs address get issue");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncs_AgcEnableGainIndexUpdate_BfGet(device, NULL, rxFuncsBaseAddr, &latchReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading enable gain index update bit");
        goto cleanup;
    }

    if (latchReadback == ADI_TRUE) 
    {
        recoveryAction = adrv904x_RxFuncs_AgcGainIndex_BfSet(device,
                                                             NULL,
                                                             rxFuncsBaseAddr,
                                                             LATCH_READBACK_DUMMY_DATA);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing dummy data to latch gain index readback");
            goto cleanup;
        }
    }
    
    recoveryAction = adrv904x_RxFuncs_AgcGainIndex_BfGet(device,
                                                         NULL,
                                                         rxFuncsBaseAddr,
                                                         &rxGain->gainIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading AGC gain index");
        goto cleanup;
    }

    /* Set rxChannelMask to a valid Rx Channel */
    rxGain->rxChannelMask = (uint32_t)rxChannel;
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDataFormatGet(adi_adrv904x_Device_t * const         device,
                                                              const adi_adrv904x_RxChannels_e       rxChannel,
                                                              adi_adrv904x_RxDataFormatRt_t * const rxDataFormat)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxDataFormat);
    
    /* Check if the requested rxChannel is valid Rx or ORx channel. */
    if ((rxChannel != ADI_ADRV904X_RX0)  &&
        (rxChannel != ADI_ADRV904X_RX1)  &&
        (rxChannel != ADI_ADRV904X_RX2)  &&
        (rxChannel != ADI_ADRV904X_RX3)  &&
        (rxChannel != ADI_ADRV904X_RX4)  &&
        (rxChannel != ADI_ADRV904X_RX5)  &&
        (rxChannel != ADI_ADRV904X_RX6)  &&
        (rxChannel != ADI_ADRV904X_RX7)  &&
        (rxChannel != ADI_ADRV904X_ORX0) &&
        (rxChannel != ADI_ADRV904X_ORX1) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for RxDataFormatGet");
        goto cleanup;
    }
    
    /* Retrieve the format selection with adrv904x_RxDataFormatSelectGet */
    recoveryAction = adrv904x_RxDataFormatSelectGet(device,
                                                    rxChannel,
                                                   &rxDataFormat->rxDataFormat.formatSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat formatSelect");
        goto cleanup;
    }
    
    /* Retrieve the floating point format configuration with adrv904x_RxDataFormatFloatingPointGet */
    recoveryAction = adrv904x_RxDataFormatFloatingPointGet(  device,
                                                             rxChannel, 
                                                            &rxDataFormat->rxDataFormat.floatingPointConfig);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat floatingPointConfig");
        goto cleanup;
    }

    
    /* Retrieve the integer format configuration with adrv904x_RxDataFormatIntegerGet */
    recoveryAction = adrv904x_RxDataFormatIntegerGet(  device,
                                                       rxChannel,
                                                      &rxDataFormat->rxDataFormat.integerConfigSettings,
                                                      &rxDataFormat->rxDataFormat.slicerConfigSettings);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat integerConfigSettings and slicerConfigSettings");
        goto cleanup;
    }
    
    /* Retrieve the embedded overload indicator format configuration with adrv904x_RxDataFormatEmbOvldMonitorGet */
    recoveryAction = adrv904x_RxDataFormatEmbOvldMonitorGet(  device,
                                                              rxChannel,
                                                             &rxDataFormat->rxDataFormat.embOvldMonitorSettings);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat embOvldMonitorSettings");
        goto cleanup;
    }
    
    /* Retrieve the external LNA gain compensation and gain temperature compensation with adrv904x_RxGainCompExtLnaGet and adrv904x_RxGainCompTempEnableGet */
    recoveryAction = adrv904x_RxGainCompExtLnaGet(  device,
                                                    rxChannel,
                                                   &rxDataFormat->rxDataFormat.externalLnaGain);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat externalLnaGain");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxGainCompTempEnableGet(  device,
                                                        rxChannel,
                                                       &rxDataFormat->rxDataFormat.tempCompensationEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting RxDataFormat tempCompensationEnable");
        goto cleanup;
    }
    
    /* Populate the rxChannelMask field of the output structure with the specified channel. */
    rxDataFormat->rxChannelMask = rxChannel;
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcDataFormatSet(adi_adrv904x_Device_t* const          device,
                                                            const adi_adrv904x_CddcDataFormatRt_t cddcDataFormat[],
                                                            const uint32_t                        arraySize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Null pointer checks */
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcDataFormat, cleanup);
    /* Range Check inputs */
#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    recoveryAction = adrv904x_CddcDataFormatSetRangeCheck(device, cddcDataFormat, arraySize);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "cddcDataFormat range check failure.");
        goto cleanup;
    }
#endif

    /* Iterate over arraySize elements of cddcDataFormat */
    for (uint32_t configIdx = 0U; configIdx < arraySize; configIdx++)
    {

        switch (cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect)
        {

            /*If Floating Point mode selected, Configure floating point*/
        case ADI_ADRV904X_CDDC_FLOATING_POINT:
            recoveryAction = adrv904x_CddcDataFormatFloatingPointSet(device,
                cddcDataFormat[configIdx].rxChannelMask,
                &cddcDataFormat[configIdx].cddcDataFormat.cddcFloatingPointConfig);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting CddcDataFormat cddcFloatingPointConfig");
                goto cleanup;
            }
            break;

            /* Integer and slicer config settings */
        case ADI_ADRV904X_CDDC_INTEGER:
            recoveryAction = adrv904x_CddcDataFormatIntegerSet(device,
                cddcDataFormat[configIdx].rxChannelMask,
                cddcDataFormat[configIdx].cddcDataFormat.cddcFormatSelect,
                &cddcDataFormat[configIdx].cddcDataFormat.cddcIntegerConfigSettings,
                &cddcDataFormat[configIdx].cddcDataFormat.cddcSlicerConfigSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting CddcDataFormat cddcIntegerConfigSettings and cddcSlicerConfigSettings");
                goto cleanup;
            }
            break;

        case ADI_ADRV904X_CDDC_SLICER:
            recoveryAction = adrv904x_CddcDataFormatSlicerSet(device, 
                                                              cddcDataFormat[configIdx].rxChannelMask,
                                                             &cddcDataFormat[configIdx].cddcDataFormat.cddcSlicerConfigSettings);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting CddcDataFormat cddcSlicerConfigSettings ");
                goto cleanup;
            }
            break;

        case ADI_ADRV904X_CDDC_EMBED_OVERLOAD_MONITOR_DATA:   /* Embedded overload indicator settings are currently not implemented */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_GOTO(&device->common, cleanup);
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_GOTO(&device->common, cleanup);
            break;
        }
    }
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcDataFormatGet(adi_adrv904x_Device_t * const           device,
                                                                const adi_adrv904x_RxChannels_e         rxChannel,
                                                                adi_adrv904x_CddcDataFormatRt_t * const cddcDataFormat)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    /* Null pointer checks */
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcDataFormat, cleanup);

    /* Check if the requested rxChannel is valid Rx or ORx channel. */
    if ((rxChannel != ADI_ADRV904X_RX0)  &&
        (rxChannel != ADI_ADRV904X_RX1)  &&
        (rxChannel != ADI_ADRV904X_RX2)  &&
        (rxChannel != ADI_ADRV904X_RX3)  &&
        (rxChannel != ADI_ADRV904X_RX4)  &&
        (rxChannel != ADI_ADRV904X_RX5)  &&
        (rxChannel != ADI_ADRV904X_RX6)  &&
        (rxChannel != ADI_ADRV904X_RX7)  &&
        (rxChannel != ADI_ADRV904X_ORX0) &&
        (rxChannel != ADI_ADRV904X_ORX1) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                rxChannel,
                                "Invalid Rx Channel Requested for CddcDataFormatGet");
        goto cleanup;
    }
    
    /* Retrieve the format selection with adrv904x_CddcDataFormatSelectGet */
    recoveryAction = adrv904x_CddcDataFormatSelectGet(device,
                                                      rxChannel,
                                                     &cddcDataFormat->cddcDataFormat.cddcFormatSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting CddcDataFormat formatSelect");
        goto cleanup;
    }
    
    /* Retrieve the floating point format configuration with adrv904x_CddcDataFormatFloatingPointGet */
    recoveryAction = adrv904x_CddcDataFormatFloatingPointGet(device,
                                                             rxChannel, 
                                                            &cddcDataFormat->cddcDataFormat.cddcFloatingPointConfig);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting CddcDataFormat floatingPointConfig");
        goto cleanup;
    }

    
    /* Retrieve the integer format configuration with adrv904x_CddcDataFormatIntegerGet */
    recoveryAction = adrv904x_CddcDataFormatIntegerGet(device,
                                                       rxChannel,
                                                      &cddcDataFormat->cddcDataFormat.cddcIntegerConfigSettings,
                                                      &cddcDataFormat->cddcDataFormat.cddcSlicerConfigSettings);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting CddcDataFormat integerConfigSettings and slicerConfigSettings");
        goto cleanup;
    }
    
    /* Retrieve the embedded overload indicator format configuration with adrv904x_CddcDataFormatEmbOvldMonitorGet */
    recoveryAction = adrv904x_CddcDataFormatEmbOvldMonitorGet(device,
                                                              rxChannel,
                                                             &cddcDataFormat->cddcDataFormat.cddcEmbOvldMonitorSettings);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error getting CddcDataFormat embOvldMonitorSettings");
        goto cleanup;
    }
    
    /* Populate the rxChannelMask field of the output structure with the specified channel. */
    cddcDataFormat->rxChannelMask = rxChannel;
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxSlicerPositionGet(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_RxChannels_e    rxChannel,
                                                                  uint8_t* const                     slicerPosition)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsChanAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, slicerPosition, cleanup);
    
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
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, rxChannel, "Invalid Rx Channel Requested for reading slicer position");
        goto cleanup;
    }
    
    /* Get Rx Funcs Bitfield Address */
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(  device,
                                                          rxChannel,
                                                         &rxFuncsChanAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to retrieve Rx Funcs base address");
        goto cleanup; 
    }
    
    /* Get Slicer Position from Bitfield */
    recoveryAction =  adrv904x_RxFuncs_RxdpSlicerPosition_BfGet(device,
                                                                NULL,
                                                                rxFuncsChanAddr,
                                                                slicerPosition);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(  &device->common, recoveryAction, "RxdpSlicerPosition readback issue");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxLoSourceGet(adi_adrv904x_Device_t * const   device,
                                                            const adi_adrv904x_RxChannels_e rxChannel,
                                                            adi_adrv904x_LoSel_e * const    rxLoSource)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check Rx data format config pointer is not NULL*/
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxLoSource, cleanup);
    
    /* Check that the requested rxChannel is valid */
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
                                "Invalid Rx channel selected for LO source mapping read back. Valid Rx channels are Rx0-Rx7.");
        goto cleanup;
    }
    
    /* Check that rx profile is valid in current config */
    if (((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) != ADI_ADRV904X_RX_PROFILE_VALID)
        || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 rxChannel,
                                 "LO source read back requested for an Rx channel but Rx profile is invalid or channel not initialized in the device structure");
        goto cleanup;
    }
    
    /* Readback Rx LO source selection */
    recoveryAction = adrv904x_RxLoSourceGet(device, rxChannel, rxLoSource);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving Lo Source for Rx channel.");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxNcoShifterSet(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_RxNcoConfig_t* const rxNcoConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_RxNcoConfig_t rxNcoInfo;
    adrv904x_RxNcoConfigResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;
    uint8_t channelMaskRet = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxNcoConfig, cleanup);
    ADI_LIBRARY_MEMSET(&rxNcoInfo, 0, sizeof(rxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (rxNcoConfig->bandSelect >= ADRV904X_DDC_NUM_BAND)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    rxNcoInfo.chanSelect = (uint8_t) rxNcoConfig->chanSelect;
    rxNcoInfo.bandSelect = (adi_adrv904x_DdcNumber_t) rxNcoConfig->bandSelect;
    rxNcoInfo.enable = (uint8_t) rxNcoConfig->enable;
    rxNcoInfo.phase = (uint32_t) ADRV904X_HTOCL(rxNcoConfig->phase);
    rxNcoInfo.frequencyKhz = (int32_t) ADRV904X_HTOCL(rxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_RX_NCO,
                                                (void*)&rxNcoInfo,
                                                sizeof(rxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxNcoShifterGet(adi_adrv904x_Device_t* const                    device,
                                                              adi_adrv904x_RxNcoConfigReadbackResp_t* const   rxRbConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_RxNcoConfigReadback_t rxNcoCfgRbCmd;
    adi_adrv904x_RxNcoConfigReadbackResp_t rxNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_NO_ERROR;
    uint32_t cpuType = 0U;
    uint32_t chIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&rxNcoCfgRbCmd, 0, sizeof(rxNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&rxNcoCfgGetCmdRsp, 0, sizeof(rxNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated */
    if (rxRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_RXOFF || 
        rxRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_RX7    || 
        (((rxRbConfig->chanSelect - 1) & rxRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (rxRbConfig->bandSelect >= ADRV904X_DDC_NUM_BAND)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    /* Get CPU assigned to this channel */
    for (chIdx = 0U; chIdx < ADI_ADRV904X_MAX_CHANNELS; chIdx++)
    {
        if (rxRbConfig->chanSelect == ((uint8_t)1U << chIdx))
        {
            cpuType = device->initExtract.rxTxCpuConfig[chIdx];
            break;
        }
    }

    /* Prepare the command payload */
    rxNcoCfgRbCmd.chanSelect = (uint8_t) rxRbConfig->chanSelect;
    rxNcoCfgRbCmd.bandSelect = (adi_adrv904x_DdcNumber_t) rxRbConfig->bandSelect;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         (adi_adrv904x_CpuType_e)cpuType,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_RX_NCO,
                                         (void*)&rxNcoCfgRbCmd,
                                         sizeof(rxNcoCfgRbCmd),
                                         (void*)&rxNcoCfgGetCmdRsp,
                                         sizeof(rxNcoCfgGetCmdRsp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)rxNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
    }

    if (rxNcoCfgGetCmdRsp.chanSelect == rxNcoCfgRbCmd.chanSelect)
    {
        /* Extract the command-specific response from the response payload */
        rxRbConfig->enabled = (uint8_t) rxNcoCfgGetCmdRsp.enabled;
        rxRbConfig->phase = (uint32_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.phase); 
        rxRbConfig->frequencyKhz = (int32_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.frequencyKhz);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->chanSelect, "Channel is not handled by the CPU");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoSet(adi_adrv904x_Device_t* const                device,
                                                        const adi_adrv904x_ORxNcoConfig_t * const   orxNcoConfig)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ORxNcoConfig_t orxNcoInfo;
    adrv904x_ORxNcoConfigResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e     cmdStatus       = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuErrorCode_t cpuErrorCode    = 0U;
    uint32_t                    cpuTypeIdx      = 0U;
    uint8_t                     channelMaskRet  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&orxNcoInfo, 0, sizeof(orxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    /* Prepare the command payload */
    orxNcoInfo.chanSelect = (uint8_t) orxNcoConfig->chanSelect;
    orxNcoInfo.ncoSelect = (uint8_t) orxNcoConfig->ncoSelect;
    orxNcoInfo.enable = (uint8_t) orxNcoConfig->enable;
    orxNcoInfo.phase = (uint32_t) ADRV904X_HTOCL(orxNcoConfig->phase);
    orxNcoInfo.frequencyKhz = (int32_t) ADRV904X_HTOCL(orxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_ORX_NCO,
                                                (void*)&orxNcoInfo,
                                                sizeof(orxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoSet_v2(adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_ORxNcoConfig_t * const   orxNcoConfig)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ORxNcoConfig_t orxNcoInfo;
    adrv904x_ORxNcoConfigResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e     cmdStatus       = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuErrorCode_t cpuErrorCode    = 0U;
    uint32_t                    cpuTypeIdx      = 0U;
    uint8_t                     channelMaskRet  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&orxNcoInfo, 0, sizeof(orxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    /* Prepare the command payload */
    orxNcoInfo.chanSelect = (uint8_t) orxNcoConfig->chanSelect;
    orxNcoInfo.ncoSelect = (uint8_t) orxNcoConfig->ncoSelect;
    orxNcoInfo.enable = (uint8_t) orxNcoConfig->enable;
    orxNcoInfo.phase = (uint32_t) ADRV904X_HTOCL(orxNcoConfig->phase);
    orxNcoInfo.frequencyKhz = (int32_t) ADRV904X_HTOCL(orxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_ORX_NCO_V2,
                                                (void*)&orxNcoInfo,
                                                sizeof(orxNcoInfo),
                                                (void*)&cmdRsp,
                                                sizeof(cmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoGet(adi_adrv904x_Device_t* const                    device,
                                                        adi_adrv904x_ORxNcoConfigReadbackResp_t* const  orxRbConfig)
{
        adi_adrv904x_ErrAction_e                recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_ORxNcoConfigReadback_t         orxNcoCfgRbCmd;
    adi_adrv904x_ORxNcoConfigReadbackResp_t orxNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e                 cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                cpuTypeIdx          = 0U;
    uint8_t                                 channelMaskRet      = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxRbConfig, cleanup);
    ADI_LIBRARY_MEMSET(&orxNcoCfgRbCmd, 0, sizeof(orxNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&orxNcoCfgGetCmdRsp, 0, sizeof(orxNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (orxRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_RXOFF || 
        orxRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_RX7    || 
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
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_GET_ORX_NCO,
                                                (void*)&orxNcoCfgRbCmd,
                                                sizeof(orxNcoCfgRbCmd),
                                                (void*)&orxNcoCfgGetCmdRsp,
                                                sizeof(orxNcoCfgGetCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)orxNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
        }

        channelMaskRet = orxNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == orxNcoCfgRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            orxRbConfig->enabled = (uint8_t) orxNcoCfgGetCmdRsp.enabled;
            orxRbConfig->ncoSelect = (uint8_t) orxNcoCfgGetCmdRsp.ncoSelect;
            orxRbConfig->phase = (uint32_t) ADRV904X_CTOHL(orxNcoCfgGetCmdRsp.phase);
            orxRbConfig->frequencyKhz = (int32_t) ADRV904X_CTOHL(orxNcoCfgGetCmdRsp.frequencyKhz);
            goto cleanup;
        }
    }
    /* if didn't go to cleanup, then some error occurred.*/
    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "Firmware did not execute cmd on all channels requested");


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCddcNcoSet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_RxCddcNcoConfig_t* const rxNcoConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_RxCddcNcoConfig_t rxNcoInfo;
    adi_adrv904x_RxCddcNcoConfigReadbackResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;
    uint8_t channelMaskRet = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxNcoConfig, cleanup);
    ADI_LIBRARY_MEMSET(&rxNcoInfo, 0, sizeof(rxNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (rxNcoConfig->chanSelect == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (rxNcoConfig->carrierSelect == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->carrierSelect, "Invalid carrier provided.");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxCddcCarrierCheck(device, rxNcoConfig->chanSelect, rxNcoConfig->carrierSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxCddcCarrierCheck issue");
        goto cleanup;
    }

    if (rxNcoConfig->phase > MAX_CDDC_NCO_PHASE_DEGREES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->phase, "Invalid phase provided.");
        goto cleanup;
    }

    if ((rxNcoConfig->frequencyKhz > MAX_CDDC_NCO_FREQ_KHZ) ||
        (rxNcoConfig->frequencyKhz < -MAX_CDDC_NCO_FREQ_KHZ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxNcoConfig->frequencyKhz, "Invalid frequency provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    rxNcoInfo.chanSelect    = (uint8_t) rxNcoConfig->chanSelect;
    rxNcoInfo.carrierSelect = (uint8_t) rxNcoConfig->carrierSelect;
    rxNcoInfo.enable        = (uint8_t) rxNcoConfig->enable;
    rxNcoInfo.phase         = (uint32_t) ADRV904X_HTOCL(rxNcoConfig->phase);

    /* NOTE: Because the Rx CDDC flip is reversed, must change the sign */
    rxNcoInfo.frequencyKhz  = -(int32_t) ADRV904X_HTOCL(rxNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_SET_RX_CDDC_NCO,
                                             (void*)&rxNcoInfo,
                                             sizeof(rxNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == rxNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCddcNcoGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_RxCddcNcoConfigReadbackResp_t* const rxRbConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_RxCddcNcoConfig_t rxNcoCfgRbCmd;
    adi_adrv904x_RxCddcNcoConfigReadbackResp_t rxNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;
    uint8_t channelMaskRet = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&rxNcoCfgRbCmd, 0, sizeof(rxNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&rxNcoCfgGetCmdRsp, 0, sizeof(rxNcoCfgGetCmdRsp));

    /* Testing if channel mask has more than two channels activated*/
    if (rxRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_RXOFF || 
        rxRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_RX7    || 
        (((rxRbConfig->chanSelect - 1) & rxRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Testing if carrier mask has more than two carrier activated */
    if ((rxRbConfig->carrierSelect == (uint8_t)ADI_ADRV904X_RX_CARRIER_NONE) || 
        (rxRbConfig->carrierSelect > (uint8_t)ADI_ADRV904X_RX_CARRIER_7)  || 
        (((rxRbConfig->carrierSelect - 1) & rxRbConfig->carrierSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxRbConfig->carrierSelect, "Invalid carrierSelect provided.");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxCddcCarrierCheck(device, rxRbConfig->chanSelect, rxRbConfig->carrierSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxCddcCarrierCheck issue");
        goto cleanup;
    }

    /* Prepare the command payload */
    rxNcoCfgRbCmd.chanSelect    = (uint8_t) rxRbConfig->chanSelect;
    rxNcoCfgRbCmd.carrierSelect = (uint8_t) rxRbConfig->carrierSelect;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_GET_RX_CDDC_NCO,
                                             (void*)&rxNcoCfgRbCmd,
                                             sizeof(rxNcoCfgRbCmd),
                                             (void*)&rxNcoCfgGetCmdRsp,
                                             sizeof(rxNcoCfgGetCmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)rxNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
        }

        channelMaskRet = rxNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == rxNcoCfgRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            rxRbConfig->chanSelect    = (uint8_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.chanSelect);
            rxRbConfig->carrierSelect = (uint8_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.carrierSelect);
            rxRbConfig->enabled       = (uint8_t) rxNcoCfgGetCmdRsp.enabled;
            rxRbConfig->phase         = (uint32_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.phase); 

            /* NOTE: Because the Rx CDDC flip is reversed, must change the sign */
            rxRbConfig->frequencyKhz  = -(int32_t) ADRV904X_CTOHL(rxNcoCfgGetCmdRsp.frequencyKhz);
            break;
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerCfgSet(adi_adrv904x_Device_t * const device,
                                                                     adi_adrv904x_RxDecimatedPowerCfg_t rxDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxChanAddr_e rxDigBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_DIG;
    adrv904x_BfRxDdcChanAddr_e ddcAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxDecPowerCfg, cleanup);

    if (numOfDecPowerCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfDecPowerCfgs, "Invalid Number of Dec Power Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_DecPowerCfgRangeCheck(device, &rxDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Dec power range Check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(rxDecPowerCfg[cfgIdx].rxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxFuncsBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx func address");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &rxDigBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Rx Channel used to determine rx dig address");
                    goto cleanup;
                }
                
                if (rxDecPowerCfg[cfgIdx].decPowerControl == ADI_ADRV904X_DEC_POWER_MEAS_OFF)
                {
                    /* Manual(SPI) control, user disables the dec power measurement */
                    decPowerEnable = 0U;
                    decPowerAgcSelect = 0U;
                    decPowerClockEnable = 0U;
                }
                else if (rxDecPowerCfg[cfgIdx].decPowerControl == ADI_ADRV904X_DEC_POWER_MEAS_ON)
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
                recoveryAction = adrv904x_RxDig_Hb2OutClkDivideRatio_BfGet(device, NULL, rxDigBaseAddr, &hb2OutClkDivRatio);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read hb2 out clk divide ratio");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxDig_DecpwrClkDivideRatio_BfSet(device, NULL, rxDigBaseAddr, hb2OutClkDivRatio);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power clk divide ratio");
                    goto cleanup;
                }

                recoveryAction = adrv904x_RxDig_DecpwrClkEnable_BfSet(device, NULL, rxDigBaseAddr, decPowerClockEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set dec power clk enable bit");
                    goto cleanup;
                }

                /* 2 - Main path Dec power configuration */
                if (ADRV904X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK))
                {
                    /* Disable Dec Power While Changing Configs */
                    recoveryAction = adrv904x_RxFuncs_DecPowerEnable_BfSet(device, NULL, rxFuncsBaseAddr, 0U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerDataSel_BfSet(device, NULL, rxFuncsBaseAddr, rxDecPowerCfg[cfgIdx].powerInputSelect);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write signal source for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerMeasurementDuration_BfSet(device, NULL, rxFuncsBaseAddr, rxDecPowerCfg[cfgIdx].powerMeasurementDuration);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write decimated power measurement duration");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfSet(device, NULL, rxFuncsBaseAddr, decPowerAgcSelect);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerTddModeEnable_BfSet(device, NULL, rxFuncsBaseAddr, TDD_MODE_ENABLE_DEFAULT_VAL);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerStartDelayCounter_BfSet(device, NULL, rxFuncsBaseAddr, TDD_MODE_DELAY_DEFAULT_VAL);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_RxFuncs_DecPowerEnable_BfSet(device, NULL, rxFuncsBaseAddr, decPowerEnable);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                        goto cleanup;
                    }
                }

                /* 3 - Configure Band Decimated power blocks */
                ddcMask = 0U;
                if (ADRV904X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK))
                {
                    ddcMask |= (1U << (uint32_t)ADI_ADRV904X_RX_DDC_BAND0);
                }
                
                if (ADRV904X_BF_EQUAL(rxDecPowerCfg[cfgIdx].measBlockSelectMask, (uint32_t)ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK))
                {
                    ddcMask |= (1U << (uint32_t)ADI_ADRV904X_RX_DDC_BAND1);
                }
                
                for (ddcSelection = 0U; ddcSelection < NUMBER_OF_RX_DDC; ddcSelection++)
                {
                    if (ADRV904X_BF_EQUAL(ddcMask, (uint32_t)(1U << ddcSelection)))
                    {
                        /* Get Rx DDC Bitfield Address */
                        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device,
                                                                          (adi_adrv904x_RxChannels_e)(chanSel),
                                                                          (adi_adrv904x_RxDdcs_e)ddcSelection,
                                                                          &ddcAddr);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Rx ddc bit field address");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_RxDdc_DecPowerMeasurementDuration_BfSet(device, NULL, ddcAddr, rxDecPowerCfg[cfgIdx].powerMeasurementDuration);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write decimated power measurement duration");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv904x_RxDdc_DecPowerEnable_BfSet(device, NULL, ddcAddr, decPowerEnable);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfSet(device, NULL, ddcAddr, decPowerAgcSelect);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv904x_RxDdc_DecPowerTddModeEnable_BfSet(device, NULL, ddcAddr, TDD_MODE_ENABLE_DEFAULT_VAL);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to enable Tdd mode for decimated power measurement");
                            goto cleanup;
                        }
                    
                        recoveryAction = adrv904x_RxDdc_DecPowerStartDelayCounter_BfSet(device, NULL, ddcAddr, TDD_MODE_DELAY_DEFAULT_VAL);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerCfgGet(adi_adrv904x_Device_t * const device,
                                                                     const adi_adrv904x_RxChannels_e rxChannel,
                                                                     const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                     adi_adrv904x_RxDecimatedPowerCfg_t * const rxDecPowerCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxDdcChanAddr_e ddcAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t decPowerEnable = 0U;
    uint8_t decPowerAgcSelect = 0U;
    adi_adrv904x_RxDdcs_e ddcSelection = ADI_ADRV904X_RX_DDC_BAND0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxDecPowerCfg, cleanup);
    
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
        goto cleanup;
    }
    
    if ((decPowerBlockSelection != ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK) &&
        (decPowerBlockSelection != ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK) &&
        (decPowerBlockSelection != ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK))
    {
        /* Invalid decimated power block selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, decPowerBlockSelection, "Invalid decimated power block selection");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device, rxChannel, &rxFuncsBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx func address");
        goto cleanup;
    }

    rxDecPowerCfg->rxChannelMask = (uint32_t)rxChannel;
    
    if (decPowerBlockSelection == ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK)
    {
        recoveryAction = adrv904x_RxFuncs_DecPowerDataSel_BfGet(device, NULL, rxFuncsBaseAddr, &rxDecPowerCfg->powerInputSelect);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read signal source for decimated power measurement");
            goto cleanup;
        }

        recoveryAction = adrv904x_RxFuncs_DecPowerMeasurementDuration_BfGet(device, NULL, rxFuncsBaseAddr, &rxDecPowerCfg->powerMeasurementDuration);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read decimated power measurement duration");
            goto cleanup;
        }

        recoveryAction = adrv904x_RxFuncs_DecPowerEnable_BfGet(device, NULL, rxFuncsBaseAddr, &decPowerEnable);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
            goto cleanup;
        }

        recoveryAction = adrv904x_RxFuncs_DecPowerEnSpiOrAgcSelect_BfGet(device, NULL, rxFuncsBaseAddr, &decPowerAgcSelect);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
            goto cleanup;
        }

        rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK;
    }
    else
    {
        if (decPowerBlockSelection == ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV904X_RX_DDC_BAND0;
            rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK;
        }
        else
        {
            ddcSelection = ADI_ADRV904X_RX_DDC_BAND1;
            rxDecPowerCfg->measBlockSelectMask = (uint8_t)ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK;
        }
        
        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, rxChannel, ddcSelection, &ddcAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
            goto cleanup;
        }
        
        rxDecPowerCfg->powerInputSelect = 0U; /* Input selection is not supported for DDC */

        recoveryAction = adrv904x_RxDdc_DecPowerMeasurementDuration_BfGet(device, NULL, ddcAddr, &rxDecPowerCfg->powerMeasurementDuration);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read decimated power measurement duration");
            goto cleanup;
        }
                    
        recoveryAction = adrv904x_RxDdc_DecPowerEnable_BfGet(device, NULL, ddcAddr, &decPowerEnable);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power enable bit");
            goto cleanup;
        }
                    
        recoveryAction = adrv904x_RxDdc_DecPowerEnSpiOrAgcSelect_BfGet(device, NULL, ddcAddr, &decPowerAgcSelect);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write dec power AGC select bit");
            goto cleanup;
        }
    }

    if ((decPowerEnable == 0U) && (decPowerAgcSelect == 0U))
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV904X_DEC_POWER_MEAS_OFF;
    }
    else if ((decPowerEnable == 1U) && (decPowerAgcSelect == 0U))
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV904X_DEC_POWER_MEAS_ON;
    }
    else if (decPowerAgcSelect == 1U)
    {
        rxDecPowerCfg->decPowerControl = ADI_ADRV904X_DEC_POWER_AGC_MEAS;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid dec power measurement configuration is detected");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDecimatedPowerGet(adi_adrv904x_Device_t * const device,
                                                                  const adi_adrv904x_RxChannels_e rxChannel,
                                                                  const adi_adrv904x_DecPowerMeasurementBlock_e decPowerBlockSelection,
                                                                  uint8_t * const powerReadBack)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxFuncsBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_FUNCS;
    adrv904x_BfRxDdcChanAddr_e ddcAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    adi_adrv904x_RxDdcs_e ddcSelection = ADI_ADRV904X_RX_DDC_BAND0;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, powerReadBack, cleanup);

    if ((rxChannel != ADI_ADRV904X_RX0) &&
        (rxChannel != ADI_ADRV904X_RX1) &&
        (rxChannel != ADI_ADRV904X_RX2) &&
        (rxChannel != ADI_ADRV904X_RX3) &&
        (rxChannel != ADI_ADRV904X_RX4) &&
        (rxChannel != ADI_ADRV904X_RX5) &&
        (rxChannel != ADI_ADRV904X_RX6) &&
        (rxChannel != ADI_ADRV904X_RX7)
       )
    {
        /* Invalid channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid channel selection");
        goto cleanup;
    }

    if ((rxChannel == ADI_ADRV904X_ORX0) ||
        (rxChannel == ADI_ADRV904X_ORX1))
    {
    }
    else if (decPowerBlockSelection == ADI_ADRV904X_DEC_POWER_MAIN_PATH_MEAS_BLOCK)
    {
        recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device,
                                                            rxChannel,
                                                            &rxFuncsBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Rx Channel used to determine rx func address");
            goto cleanup;
        }

        recoveryAction = adrv904x_RxFuncs_DecPowerReadback_BfSet(device,
                                                                 NULL,
                                                                 rxFuncsBaseAddr,
                                                                 1U);
        if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
            goto cleanup ;
        }

        recoveryAction = adrv904x_RxFuncs_DecPowerValue_BfGet(device,
                                                              NULL,
                                                              rxFuncsBaseAddr,
                                                              powerReadBack);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading main path decimated power for selected Rx channel");
            goto cleanup;
        }
    }
    else
    {
        if (decPowerBlockSelection == ADI_ADRV904X_DEC_POWER_BAND_A_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV904X_RX_DDC_BAND0;
        
        }
        else if (decPowerBlockSelection == ADI_ADRV904X_DEC_POWER_BAND_B_MEAS_BLOCK)
        {
            ddcSelection = ADI_ADRV904X_RX_DDC_BAND1;
        }
        else
        {
            /* Invalid decPowerBlockSelection selection */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid decPowerBlockSelection selection");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device,
                                                          rxChannel,
                                                          ddcSelection,
                                                          &ddcAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read DDC address");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_RxDdc_DecPowerValueReadback_BfSet(device,
                                                                    NULL,
                                                                    ddcAddr,
                                                                    1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_RxDdc_DecPowerValue_BfGet(device,
                                                            NULL,
                                                            ddcAddr,
                                                            powerReadBack);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading band decimated power for selected Rx channel");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAttenSet(adi_adrv904x_Device_t* const device,
                                                          const uint32_t channelMask,
                                                          const uint8_t attenDb)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0U;
    uint8_t trmAtten = 0U;
    uint8_t trmAttenBwCapEn = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    if (channelMask > MAX_ORX_CHANNEL_MASK)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            channelMask,
            "Invalid channelMask");
        goto cleanup;
    }
    
    if (attenDb > ADI_ADRV904X_MAX_ORX_ATTEN_DB)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            attenDb,
            "attenDb outside maximum range");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_ORxAttenDbToRegValues(device, attenDb, &trmAtten, &trmAttenBwCapEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adrv904x_ORxAttenDbToRegValues failed");
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
        recoveryAction = adrv904x_ActrlOrxWestRegmap_TrmAttenPd_BfSet(device, NULL, chanIdToOrxWestRegmapChanAddr[chanId], 0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenPd bitfield");
            goto cleanup;
        }        
        
        /* ensure channel's trm_cm_cap_en is always set 1 */
        recoveryAction = adrv904x_ActrlOrxWestRegmap_TrmAttenCmCapEn_BfSet(device, NULL, chanIdToOrxWestRegmapChanAddr[chanId], 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenCmCapEn bitfield");
            goto cleanup;
        }        
        
        /* Set this ORx channel's trm_atten bitfield */
        recoveryAction = adrv904x_ActrlOrxWestRegmap_TrmAtten_BfSet(device,
                                                                    NULL,
                                                                    chanIdToOrxWestRegmapChanAddr[chanId],
                                                                    (adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten_e)trmAtten);
        
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ORX attenuator trimmer set issue");
            goto cleanup;
        }

        /* Set this ORx channel's trm_atten_bw_cap - NOTE: the register is called trm_atten_spare2 */
        recoveryAction = adrv904x_ActrlOrxWestRegmap_TrmAttenSpare2_BfSet(device,
                                                                          NULL,
                                                                          chanIdToOrxWestRegmapChanAddr[chanId],
                                                                          trmAttenBwCapEn);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting TrmAttenSpare2 bitfield");
            goto cleanup;
        }        
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAttenGet(adi_adrv904x_Device_t* const  device,
                                                          const uint8_t                 channelId,
                                                          uint8_t* const                attenDb)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten_e trmAtten = ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_0DB;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (channelId > MAX_ORX_CHANNEL_ID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                channelId,
                                "Invalid channelId");
        goto cleanup;
    }

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, attenDb, cleanup);
    
    /* Get the channel's attenuation */
    recoveryAction = adrv904x_ActrlOrxWestRegmap_TrmAtten_BfGet(device,
                                                                NULL,
                                                                chanIdToOrxWestRegmapChanAddr[channelId],
                                                                &trmAtten);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "ORX attenuator trimmer get issue");
        goto cleanup;
    }

    recoveryAction = adrv904x_ORxTrmAttenToDb(device, trmAtten, attenDb);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            ADI_NO_VARIABLE,
                            "adrv904x_ORxTrmAttenToDb failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainCtrlModeSet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_RxGainCtrlModeCfg_t  gainCtrlModeCfg[],
                                                                const uint32_t                          arraySize)
{
        adi_adrv904x_ErrAction_e     recoveryAction    = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxChannelBitfieldAddr;
    uint32_t                     gainCtrlModeIndex = 0U;
    uint32_t                     rxChannelIndex    = 0U;
    uint32_t                     rxChannel         = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainCtrlModeCfg, cleanup);

    if (arraySize == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, arraySize, "Invalid number of gain control mode array");
        goto cleanup;
    }

    /* check each item in the input array, make sure the parameters are valid */
    for (gainCtrlModeIndex = 0; gainCtrlModeIndex < arraySize; gainCtrlModeIndex++)
    {
        /* rxChannelMask should larger than 0 and less than 0x100 */
        if ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask == 0U) ||
            ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask, "Invalid Rx channel mask");
            goto cleanup;
        }

        /* gainCtrlMode just support MGC and AGC type */
        if ((gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode != ADI_ADRV904X_MGC) &&
            (gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode != ADI_ADRV904X_AGC))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode, "Invalid Rx channel gain control mode");
            goto cleanup;
        }
    }

    for (gainCtrlModeIndex = 0U; gainCtrlModeIndex < arraySize; gainCtrlModeIndex++)
    {
        for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV904X_MAX_RX_ONLY; rxChannelIndex++)
        {
            rxChannel = 1U << rxChannelIndex;
            if ((gainCtrlModeCfg[gainCtrlModeIndex].rxChannelMask & rxChannel) == rxChannel)
            {
                /* get the Rx channel function address */
                recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device,
                                                                    (adi_adrv904x_RxChannels_e)rxChannel,
                                                                    &rxChannelBitfieldAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel function address");
                    goto cleanup;
                }

                /* set gain control mode */
                recoveryAction = adrv904x_RxFuncs_AgcSetup_BfSet(device,
                                                                 NULL,
                                                                 rxChannelBitfieldAddr,
                                                                 (uint8_t)gainCtrlModeCfg[gainCtrlModeIndex].gainCtrlMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx channel gain control mode set failure");
                    goto cleanup;
                }
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxGainCtrlModeGet(adi_adrv904x_Device_t* const     device,
                                                                const adi_adrv904x_RxChannels_e  rxChannel,
                                                                adi_adrv904x_RxGainCtrlMode_e*   gainCtrlMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxFuncsChanAddr_e rxChannelBitfieldAddr;
    uint8_t                      value          = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainCtrlMode, cleanup);

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
        goto cleanup;
    }

    /* get the Rx channel function address */
    recoveryAction = adrv904x_RxFuncsBitfieldAddressGet(device,
                                                        rxChannel,
                                                        &rxChannelBitfieldAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel function address");
        goto cleanup;
    }

    /* set gain control mode */
    recoveryAction = adrv904x_RxFuncs_AgcSetup_BfGet(device,
                                                     NULL,
                                                     rxChannelBitfieldAddr,
                                                     &value);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Rx channel gain control mode get failure");
        goto cleanup;
    }

    /* update gain control mode information */
    *gainCtrlMode = (adi_adrv904x_RxGainCtrlMode_e)value;

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTempGainCompSet(adi_adrv904x_Device_t* const  device,
                                                                const uint32_t                rxChannelMask,
                                                                const int8_t                  gainValue)
{
        adi_adrv904x_ErrAction_e    recoveryAction        = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e  rxChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint32_t                    rxChannelIndex        = 0U;
    uint32_t                    rxChannel             = 0U;
    uint32_t                    ddcBand               = 0U;
    uint8_t                     value                 = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* rxChannelMask should larger than 0 and less than 0x100 */
    if ((rxChannelMask == 0U) ||
        ((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel mask");
        goto cleanup;
    }

    /* check that gainValue is within allowable range */
    if ((gainValue < -63) ||
        (gainValue > 63))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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

    for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV904X_MAX_RX_ONLY; rxChannelIndex++)
    {
        rxChannel = 1U << rxChannelIndex;
        if ((rxChannelMask & rxChannel) == rxChannel)
        {
            /* if an Rx channel mask is set, resolve the corresponding Rx DDC Bitfield Addresses (2 DDCs per channel)*/
            for (ddcBand = 0U; ddcBand < ADI_ADRV904X_MAX_RX_DDC_BANDS; ddcBand++)
            {
                recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, 
                                                                  (adi_adrv904x_RxChannels_e)rxChannel,
                                                                  (adi_adrv904x_RxDdcs_e)ddcBand,
                                                                  &rxChannelBitfieldAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, ddcBand, "get Rx channel DDC base address failure");
                    goto cleanup;
                }

                /* set temperature gain value */
                recoveryAction = adrv904x_RxDdc_RxTempGainComp_BfSet(device,
                                                                     NULL,
                                                                     rxChannelBitfieldAddr,
                                                                     value);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "set Rx channel temperature gain value failure");
                    goto cleanup;
                }
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTempGainCompGet(adi_adrv904x_Device_t* const     device,
                                                                const adi_adrv904x_RxChannels_e  rxChannel,
                                                                int8_t* const                    gainValue)
{
        adi_adrv904x_ErrAction_e    recoveryAction        = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDdcChanAddr_e  rxChannelBitfieldAddr = ADRV904X_BF_SLICE_RX_0__RX_DDC_0_;
    uint8_t                     value                 = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gainValue, cleanup);

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
        goto cleanup;
    }

    /* get the Rx channel DDC0 address */
    recoveryAction = adrv904x_RxDdcBitfieldAddressGet(device, 
                                                      rxChannel,
                                                      ADI_ADRV904X_RX_DDC_BAND0,
                                                      &rxChannelBitfieldAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "get Rx channel DDC0 address failure");
        goto cleanup;
    }

    /* get temperature gain value */
    recoveryAction = adrv904x_RxDdc_RxTempGainComp_BfGet(device,
                                                         NULL,
                                                         rxChannelBitfieldAddr,
                                                         &value);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTestDataSet(adi_adrv904x_Device_t* device,
                                                            const uint32_t rxChannelMask,
                                                            const adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg)   
{
        /* The full range of generator selection options are not made available via the API we just use these
     * two internally */
    uint8_t rxChannelIndex = 0U;
    uint32_t chanSel = 0U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDigChanAddr_e rxDigBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_DIG;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxTestDataCfg, cleanup);
    
    if ((rxChannelMask == 0U) ||
        ((rxChannelMask > (uint32_t) ADI_ADRV904X_RX_MASK_ALL)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel mask");
        goto cleanup;
    }

    for (rxChannelIndex = 0U; rxChannelIndex < ADI_ADRV904X_MAX_RX_ONLY; rxChannelIndex++)
    {
        chanSel = 1U << rxChannelIndex;
        adi_adrv904x_RxChannels_e rxChannel = (adi_adrv904x_RxChannels_e)(chanSel);
        if ((rxChannelMask & (uint32_t) rxChannel) == 0)
        {
            /* This channel is not set in the rxChannelMask arg - skip it */
            continue;
        }
            
        recoveryAction = adrv904x_RxBitfieldAddressGet(device, rxChannel, &rxDigBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx Channel used to determine rx dig address");
            goto cleanup;
        }
    
        if (rxTestDataCfg->enable == 0U)
        {
            /* Disable the test signal */
            
            /* Set register-control to disable test signal*/
            recoveryAction = adrv904x_RxDig_AdcTestGenEnSpi_BfSet(device, NULL, rxDigBaseAddr, 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator enable SPI.");
                goto cleanup;
            }                

            /* Set test signal enablement to be controlled by register (not GPIO) */
            recoveryAction = adrv904x_RxDig_AdcTestGenEnSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_EN_SEL_SPI);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable Rx ADC test signal. Could not set ADC test generator enable select.");
                goto cleanup;
            }
        }
        else
        {
            /* Enable the test signal */
            
            /* First range check the freq */
            if (rxTestDataCfg->sineFreq > ADI_ADRV904X_FS_DIV_5 || rxTestDataCfg->sineFreq < ADI_ADRV904X_FS_DIV_40)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxTestDataCfg->sineFreq, "Invalid frequency");
                goto cleanup;   
            }
        
            /* Always use full-scale sine - other signal types are not exposed via the API */
            recoveryAction = adrv904x_RxDig_AdcTestGenSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_SEL_SINE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator select.");
                goto cleanup;
            }
            
            /* Always set the test signal frequency relative to ADC clock */            
            recoveryAction = adrv904x_RxDig_AdcTestClkSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_CLK_ADC);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable ADC test generator clock select.");
                goto cleanup;
            }
                                
            /* Set the test signal frequency */
            recoveryAction = adrv904x_RxDig_AdcTestGenSineFreq_BfSet(device, NULL, rxDigBaseAddr, (uint8_t) rxTestDataCfg->sineFreq);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator sine freq.");
                goto cleanup;
            }
            
            /* Set register-control to enable test signal*/
            recoveryAction = adrv904x_RxDig_AdcTestGenEnSpi_BfSet(device, NULL, rxDigBaseAddr, 1U); 
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set ADC test generator enable SPI.");
                goto cleanup;
            }
            
            /* Set test signal enablement to be controlled by register (not GPIO) */
            recoveryAction = adrv904x_RxDig_AdcTestGenEnSel_BfSet(device, NULL, rxDigBaseAddr, ADC_TEST_GEN_EN_SEL_SPI);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set test generator enable select.");
                goto cleanup;
            }            
        }
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTestDataGet(adi_adrv904x_Device_t* device,
                                                            const adi_adrv904x_RxChannels_e rxChannel,
                                                            adi_adrv904x_RxTestDataCfg_t* const rxTestDataCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDigChanAddr_e rxDigBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_DIG;
    
    /* Hold bitfield values read back */
    uint8_t genEnSel = 0U; /* Set adc_test_gen_en source to SPI */
    uint8_t genSel   = 0U; /* Test signal type: Full scale, bypass etc */
    uint8_t clkSel   = 0U; /* Send test data into ADC iface FIFO */
    uint8_t genEnSpi = 0U; /* Start sending test data */  
    uint8_t tmpByte  = 0U; /* Temp storage */ 
    adi_adrv904x_AdcTestSineFreq_e genSineFreq = ADI_ADRV904X_FS_DIV_40; /* Test signal frequency */ 
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxTestDataCfg, cleanup);
        
    recoveryAction = adrv904x_RxBitfieldAddressGet(device, rxChannel, &rxDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannel, "Invalid Rx channel used to determine rx dig address");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_RxDig_AdcTestGenSel_BfGet(device, NULL, rxDigBaseAddr, &genSel);        
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator select.");
        goto cleanup;        
    }
    
    recoveryAction = adrv904x_RxDig_AdcTestClkSel_BfGet(device, NULL, rxDigBaseAddr, &clkSel);    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator clock select.");
        goto cleanup;
    }

    recoveryAction = adrv904x_RxDig_AdcTestGenEnSel_BfGet(device, NULL, rxDigBaseAddr, &genEnSel);    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator enable select.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_RxDig_AdcTestGenSineFreq_BfGet(device, NULL, rxDigBaseAddr, &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read ADC test generator sine freq.");
        goto cleanup;
    }

    genSineFreq = (adi_adrv904x_AdcTestSineFreq_e)tmpByte;
    
    recoveryAction = adrv904x_RxDig_AdcTestGenEnSpi_BfGet(device, NULL, rxDigBaseAddr, &genEnSpi);    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxLoPowerDownSet(adi_adrv904x_Device_t* const    device, 
                                                               const adi_adrv904x_RxChannels_e rxChannelMask,
                                                               const uint8_t                   enable)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t rxScratch7Addr                     = 0U;
    uint8_t  i                                  = 0U;

    static const uint32_t rxChanToScratch7Addr[ADI_ADRV904X_MAX_RX_ONLY] = {
        ADRV904X_ADDR_RX0_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX1_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX2_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX3_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX4_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX5_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX6_STREAM_SCRATCH7,
        ADRV904X_ADDR_RX7_STREAM_SCRATCH7,
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if ((rxChannelMask < ADI_ADRV904X_RX0) || (rxChannelMask >= ADI_ADRV904X_ORX0))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxChannelMask, "Invalid Rx channel passed - must be from Rx0 up to Rx7.");
        goto cleanup;
    }

    if ((enable != ADI_DISABLE) && (enable != ADI_ENABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enable, "Invalid enable value passed - must be ADI_ENABLE or ADI_DISABLE");
        goto cleanup;
    }
    
    for (i = 0U; i < ADI_ADRV904X_MAX_RX_ONLY; i++)
    {
        if (((uint32_t)rxChannelMask & (1U << i)) == (1U << i))
        {
            rxScratch7Addr = rxChanToScratch7Addr[i];

            /* Stream scratch pad bit is set to *skip* power down so is logical inversion of *enable* power down */
            recoveryAction = adi_adrv904x_Register32Write(  device,
                                                            NULL,
                                                            rxScratch7Addr,
                                                            (uint32_t)(!enable),
                                                            1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing on stream scratch register.");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiCfgSet(adi_adrv904x_Device_t * const   device,
                                                                  adi_adrv904x_RxCarrierRssiCfg_t rxCarrierRssiCfg[],
                                                                  const uint32_t                  numCfgs)                                                            
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierRssiCfg, cleanup);

    if (numCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCfgs, "Invalid Number of Rssi Configurations");
        goto cleanup;
    }

#if ADI_ADRV904X_RX_RANGE_CHECK > 0
    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_RxCarrierRssiCfgRangeCheck(device, &rxCarrierRssiCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx Carrier Rssi Cfg range check Error Reported");
            goto cleanup;
        }
    }
#endif

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numCfgs; ++cfgIdx)
    {
        for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
        {
            rxSel = 1U << rxIdx;
            if ((rxCarrierRssiCfg[cfgIdx].rxCarrierMask.rxChannelMask & rxSel) > 0)
            {
                /* Convert the rxSel to the base address value required by the bitfield functions */
                recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                    goto cleanup;
                }

                for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
                {
                    carrierSel = 1U << carrierIdx;
                    if ((rxCarrierRssiCfg[cfgIdx].rxCarrierMask.carrierMask & carrierSel) > 0)
                    {
                        /* rssi_mode */
                        recoveryAction = adrv904x_CddcFuncs_RssiMode_BfSet(device,
                                                                           NULL,
                                                                           cddcFuncsChanBaseAddr,
                                                                           carrierIdx,
                                                                           rxCarrierRssiCfg[cfgIdx].measMode);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi measurement mode selection");
                            goto cleanup;
                        }

                        /* rssi_start_delay */
                        recoveryAction = adrv904x_CddcFuncs_RssiStartDelay_BfSet(device,
                                                                                 NULL,
                                                                                 cddcFuncsChanBaseAddr,
                                                                                 carrierIdx,
                                                                                 rxCarrierRssiCfg[cfgIdx].startDelay);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi start delay");
                            goto cleanup;
                        }

                        /* rssi_wait_delay */
                        recoveryAction = adrv904x_CddcFuncs_RssiWaitDelay_BfSet(device,
                                                                                NULL,
                                                                                cddcFuncsChanBaseAddr,
                                                                                carrierIdx,
                                                                                rxCarrierRssiCfg[cfgIdx].waitDelay);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi wait delay");
                            goto cleanup;
                        }

                        /* rssi_continuous_meas_delay */
                        recoveryAction = adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfSet(device,
                                                                                          NULL,
                                                                                          cddcFuncsChanBaseAddr,
                                                                                          carrierIdx,
                                                                                          rxCarrierRssiCfg[cfgIdx].pauseDelay);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi pause delay");
                            goto cleanup;
                        }

                        /* rssi_default_mode */
                        recoveryAction = adrv904x_CddcFuncs_RssiDefaultMode_BfSet(device,
                                                                                  NULL,
                                                                                  cddcFuncsChanBaseAddr,
                                                                                  carrierIdx,
                                                                                  rxCarrierRssiCfg[cfgIdx].opMode);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi op mode");
                            goto cleanup;
                        }
                
                        /* rssi_meas_duration */
                        recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration0_BfSet(device,
                                                                                    NULL,
                                                                                    cddcFuncsChanBaseAddr,
                                                                                    carrierIdx,
                                                                                    rxCarrierRssiCfg[cfgIdx].duration0);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi duration 0");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration1_BfSet(device,
                                                                                    NULL,
                                                                                    cddcFuncsChanBaseAddr,
                                                                                    carrierIdx,
                                                                                    rxCarrierRssiCfg[cfgIdx].duration1);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi duration 1");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration2_BfSet(device,
                                                                                    NULL,
                                                                                    cddcFuncsChanBaseAddr,
                                                                                    carrierIdx,
                                                                                    rxCarrierRssiCfg[cfgIdx].duration2);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi duration 2");
                            goto cleanup;
                        }

                        
                        recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration3_BfSet(device,
                                                                                    NULL,
                                                                                    cddcFuncsChanBaseAddr,
                                                                                    carrierIdx,
                                                                                    rxCarrierRssiCfg[cfgIdx].duration3);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi duration 3");
                            goto cleanup;
                        }

                        /* rssi_mul_factor */
                        recoveryAction = adrv904x_CddcFuncs_RssiMulFactor0_BfSet(device,
                                                                                 NULL,
                                                                                 cddcFuncsChanBaseAddr,
                                                                                 carrierIdx,
                                                                                 rxCarrierRssiCfg[cfgIdx].mulFactor0);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi mul factor 0");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_CddcFuncs_RssiMulFactor1_BfSet(device,
                                                                                 NULL,
                                                                                 cddcFuncsChanBaseAddr,
                                                                                 carrierIdx,
                                                                                 rxCarrierRssiCfg[cfgIdx].mulFactor1);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi mul factor 1");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_CddcFuncs_RssiMulFactor2_BfSet(device,
                                                                                 NULL,
                                                                                 cddcFuncsChanBaseAddr,
                                                                                 carrierIdx,
                                                                                 rxCarrierRssiCfg[cfgIdx].mulFactor2);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi mul factor 2");
                            goto cleanup;
                        }

                        recoveryAction = adrv904x_CddcFuncs_RssiMulFactor3_BfSet(device,
                                                                                 NULL,
                                                                                 cddcFuncsChanBaseAddr,
                                                                                 carrierIdx,
                                                                                 rxCarrierRssiCfg[cfgIdx].mulFactor3);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi mul factor 3");
                            goto cleanup;
                        }

                        /* rssi_reset_on_gain_change 1bit */
                        recoveryAction = adrv904x_CddcFuncs_RssiResetOnGainChange_BfSet(device,
                                                                                        NULL,
                                                                                        cddcFuncsChanBaseAddr,
                                                                                        carrierIdx,
                                                                                        rxCarrierRssiCfg[cfgIdx].resetAtGainChange);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi reset at gain change");
                            goto cleanup;
                        }

                        /* rssi_pin_mode */
                        recoveryAction = adrv904x_CddcFuncs_RssiPinMode_BfSet(device,
                                                                              NULL,
                                                                              cddcFuncsChanBaseAddr,
                                                                              carrierIdx,
                                                                              rxCarrierRssiCfg[cfgIdx].fddPinMode);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi fdd pin mode");
                            goto cleanup;
                        }

                        /* rssi_trigger_source_select 1bit */
                        recoveryAction = adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfSet(device,
                                                                                          NULL,
                                                                                          cddcFuncsChanBaseAddr,
                                                                                          carrierIdx,
                                                                                          rxCarrierRssiCfg[cfgIdx].radioSeqTrigEnable);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi radio seq trig enable");
                            goto cleanup;
                        }

                        /* rssi_rs_trigger_select 3bit */
                        recoveryAction = adrv904x_CddcFuncs_RssiRsTriggerSelect_BfSet(device,
                                                                                      NULL,
                                                                                      cddcFuncsChanBaseAddr,
                                                                                      carrierIdx,
                                                                                      rxCarrierRssiCfg[cfgIdx].radioSeqTrigSelect);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi radio seq trig select");
                            goto cleanup;
                        }
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiCfgGet(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                  adi_adrv904x_RxCarrierRssiCfg_t * const    rxCarrierRssiCfg)
                                                
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint8_t  bfValue = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierRssiCfg, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:
            carrierIdx  = 0U;
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            carrierIdx  = 1U;
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            carrierIdx  = 2U;
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            carrierIdx  = 3U;
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            carrierIdx  = 4U;
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            carrierIdx  = 5U;
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            carrierIdx  = 6U;
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            carrierIdx  = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            goto cleanup;
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
        goto cleanup;
    }

    /* Read out the configurations */ 
    /* Convert the rxSel to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask), &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Channel used to determine SPI address");
        goto cleanup;
    }

    /* rssi_mode */
    recoveryAction = adrv904x_CddcFuncs_RssiMode_BfGet(device,
                                                       NULL,
                                                       cddcFuncsChanBaseAddr,
                                                       carrierIdx,
                                                       &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi measurement mode selection");
        goto cleanup;
    }
    rxCarrierRssiCfg->measMode = (adi_adrv904x_RxCarrierRssiMeasMode_e)bfValue;

    /* rssi_start_delay */
    recoveryAction = adrv904x_CddcFuncs_RssiStartDelay_BfGet(device,
                                                             NULL,
                                                             cddcFuncsChanBaseAddr,
                                                             carrierIdx,
                                                             &rxCarrierRssiCfg->startDelay);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi start delay");
        goto cleanup;
    }

    /* rssi_wait_delay */
    recoveryAction = adrv904x_CddcFuncs_RssiWaitDelay_BfGet(device,
                                                            NULL,
                                                            cddcFuncsChanBaseAddr,
                                                            carrierIdx,
                                                            &rxCarrierRssiCfg->waitDelay);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi wait delay");
        goto cleanup;
    }

    /* rssi_meas_delay */
    recoveryAction = adrv904x_CddcFuncs_RssiContinuousMeasDelay_BfGet(device,
                                                                      NULL,
                                                                      cddcFuncsChanBaseAddr,
                                                                      carrierIdx,
                                                                      &rxCarrierRssiCfg->pauseDelay);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi pause delay");
        goto cleanup;
    }

    /* rssi_default_mode */
    recoveryAction = adrv904x_CddcFuncs_RssiDefaultMode_BfGet(device,
                                                              NULL,
                                                              cddcFuncsChanBaseAddr,
                                                              carrierIdx,
                                                              &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi op mode");
        goto cleanup;
    }
    rxCarrierRssiCfg->opMode = (adi_adrv904x_RxCarrierRssiOpMode_e)bfValue;
                
    /* rssi_meas_duration */
    recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration0_BfGet(device,
                                                                NULL,
                                                                cddcFuncsChanBaseAddr,
                                                                carrierIdx,
                                                                &rxCarrierRssiCfg->duration0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi duration 0");
        goto cleanup;
    }

    recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration1_BfGet(device,
                                                                NULL,
                                                                cddcFuncsChanBaseAddr,
                                                                carrierIdx,
                                                                &rxCarrierRssiCfg->duration1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi duration 1");
        goto cleanup;
    }

    recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration2_BfGet(device,
                                                                NULL,
                                                                cddcFuncsChanBaseAddr,
                                                                carrierIdx,
                                                                &rxCarrierRssiCfg->duration2);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi duration 2");
        goto cleanup;
    }

                        
    recoveryAction = adrv904x_CddcFuncs_RssiMeasDuration3_BfGet(device,
                                                                NULL,
                                                                cddcFuncsChanBaseAddr,
                                                                carrierIdx,
                                                                &rxCarrierRssiCfg->duration3);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi duration 3");
        goto cleanup;
    }

    /* rssi_mul_factor */
    recoveryAction = adrv904x_CddcFuncs_RssiMulFactor0_BfGet(device,
                                                             NULL,
                                                             cddcFuncsChanBaseAddr,
                                                             carrierIdx,
                                                             &rxCarrierRssiCfg->mulFactor0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi mul factor 0");
        goto cleanup;
    }

    recoveryAction = adrv904x_CddcFuncs_RssiMulFactor1_BfGet(device,
                                                             NULL,
                                                             cddcFuncsChanBaseAddr,
                                                             carrierIdx,
                                                             &rxCarrierRssiCfg->mulFactor1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi mul factor 1");
        goto cleanup;
    }

    recoveryAction = adrv904x_CddcFuncs_RssiMulFactor2_BfGet(device,
                                                             NULL,
                                                             cddcFuncsChanBaseAddr,
                                                             carrierIdx,
                                                             &rxCarrierRssiCfg->mulFactor2);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi mul factor 2");
        goto cleanup;
    }

    recoveryAction = adrv904x_CddcFuncs_RssiMulFactor3_BfGet(device,
                                                             NULL,
                                                             cddcFuncsChanBaseAddr,
                                                             carrierIdx,
                                                             &rxCarrierRssiCfg->mulFactor3);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi mul factor 3");
        goto cleanup;
    }

    /* rssi_reset_on_gain_change 1bit */
    recoveryAction = adrv904x_CddcFuncs_RssiResetOnGainChange_BfGet(device,
                                                                    NULL,
                                                                    cddcFuncsChanBaseAddr,
                                                                    carrierIdx,
                                                                    &rxCarrierRssiCfg->resetAtGainChange);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi reset at gain change");
        goto cleanup;
    }

    /* rssi_pin_mode */
    recoveryAction = adrv904x_CddcFuncs_RssiPinMode_BfGet(device,
                                                          NULL,
                                                          cddcFuncsChanBaseAddr,
                                                          carrierIdx,
                                                          &rxCarrierRssiCfg->fddPinMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi fdd pin mode");
        goto cleanup;
    }

    /* rssi_trigger_source_select 1bit */
    recoveryAction = adrv904x_CddcFuncs_RssiTriggerSourceSelect_BfGet(device,
                                                                      NULL,
                                                                      cddcFuncsChanBaseAddr,
                                                                      carrierIdx,
                                                                      &rxCarrierRssiCfg->radioSeqTrigEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi radio seq trig enable");
        goto cleanup;
    }

    /* rssi_rs_trigger_select 3bit */
    recoveryAction = adrv904x_CddcFuncs_RssiRsTriggerSelect_BfGet(device,
                                                                  NULL,
                                                                  cddcFuncsChanBaseAddr,
                                                                  carrierIdx,
                                                                  &rxCarrierRssiCfg->radioSeqTrigSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi radio seq trig select");
        goto cleanup;
    }

    rxCarrierRssiCfg->rxCarrierMask.carrierMask = rxCarrierMask->carrierMask;
    rxCarrierRssiCfg->rxCarrierMask.rxChannelMask = rxCarrierMask->rxChannelMask;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiEnable(adi_adrv904x_Device_t * const              device,
                                                                  const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                  const uint32_t rxCarrierEnable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;
    uint8_t  rssiEn = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);

    if (((rxCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (rxCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierMask->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
        (rxCarrierMask->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierEnable & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierEnable,
                               "Invalid Rx carrier enable mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Write out the enable */
    for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
    {
        rxSel = 1U << rxIdx;
        if ((rxCarrierMask->rxChannelMask & rxSel) > 0)
        {
            /* Convert the rxSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((rxCarrierMask->carrierMask & carrierSel) > 0)
                {
                    rssiEn = 0U;
                    if ((rxCarrierEnable & carrierSel) > 0)
                    {
                        rssiEn = 1U;
                    }

                    /* rssi_enable */
                    recoveryAction = adrv904x_CddcFuncs_RssiEnable_BfSet(device,
                                                                         NULL,
                                                                         cddcFuncsChanBaseAddr,
                                                                         carrierIdx,
                                                                         rssiEn);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi enable selection");
                        goto cleanup;
                    }

                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiManualStart(adi_adrv904x_Device_t * const              device,
                                                                       const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;
    uint8_t  rssiStart = 1U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);

    if (((rxCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
        (rxCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierMask->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
         (rxCarrierMask->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Write out the manual start */
    for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
    {
        rxSel = 1U << rxIdx;
        if ((rxCarrierMask->rxChannelMask & rxSel) > 0)
        {
            /* Convert the rxSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((rxCarrierMask->carrierMask & carrierSel) > 0)
                {
                    recoveryAction = adrv904x_CddcFuncs_RssiManualMeas_BfSet(device,
                                                                             NULL,
                                                                             cddcFuncsChanBaseAddr,
                                                                             carrierIdx,
                                                                             rssiStart);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi manual start");
                        goto cleanup;
                    }

                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiManualPause(adi_adrv904x_Device_t * const              device,
                                                                       const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                       const uint32_t rxCarrierPause)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;
    uint8_t  rssiPause = 0U;

    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);

    if (((rxCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (rxCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierMask->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
       (rxCarrierMask->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierPause & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierPause,
                               "Invalid Rx carrier pause mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }


    /* Write out the manual pause */
    for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
    {
        rxSel = 1U << rxIdx;
        if ((rxCarrierMask->rxChannelMask & rxSel) > 0)
        {
            /* Convert the rxSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcFuncsChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((rxCarrierMask->carrierMask & carrierSel) > 0)
                {
                    rssiPause = 0U;
                    if ((rxCarrierPause & carrierSel) > 0)
                    {
                        rssiPause = 1U;
                    }

                    recoveryAction = adrv904x_CddcFuncs_RssiManualPause_BfSet(device,
                                                                              NULL,
                                                                              cddcFuncsChanBaseAddr,
                                                                              carrierIdx,
                                                                              rssiPause);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi manual pause");
                        goto cleanup;
                    }

                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiGpioCfgSet(adi_adrv904x_Device_t * const                     device,
                                                                      const adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t chanIdx = 0U;
    uint32_t carrierIdx = 0U;
    adi_adrv904x_GpioSignal_e cddcSignals[ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL] = {
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_0,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_1,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_2,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_3,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_4,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_5,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_6,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_7
    };

    adi_adrv904x_GpioPinSel_e pinSelect = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioCfg, cleanup);

    /* Check all GPIO are valid */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; chanIdx++)
    {            
        /* Check if enable pin is valid */
        if (gpioCfg->rxChannel[chanIdx].enablePin > ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Enable GPIO pin selected. To disable, please select ADI_ADRV904X_GPIO_INVALID");
            goto cleanup;
        }
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL; carrierIdx++)
        {
            /* Check if ready pin is valid */
            if (gpioCfg->rxChannel[chanIdx].readyPin[carrierIdx] > ADI_ADRV904X_GPIO_INVALID)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, carrierIdx, "Invalid Ready GPIO pin selected. To disable, please select ADI_ADRV904X_GPIO_INVALID");
                goto cleanup;
            }
        }
    }

    /* Release all rssi GPIO if valid */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; chanIdx++)
    {    
        /* Find the signal for this gpio index */
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL; carrierIdx++)
        {
            recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, cddcSignals[carrierIdx], (adi_adrv904x_Channels_e)(1U << chanIdx));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       carrierIdx,
                                       "GpioSignalFind issue");
                goto cleanup;
            }

            /* Release if pinSelect is valid */
            if (pinSelect != ADI_ADRV904X_GPIO_INVALID)
            {
                recoveryAction = adrv904x_GpioSignalRelease(device, pinSelect, cddcSignals[carrierIdx], (adi_adrv904x_Channels_e)(1U << chanIdx));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common,
                                           recoveryAction,
                                           carrierIdx,
                                           "GpioSignalRelease issue");
                    goto cleanup;
                }
            }
        }
  
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE, (adi_adrv904x_Channels_e)(1U << chanIdx));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   chanIdx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        /* Release if pinSelect is valid */
        if (pinSelect != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease(device, pinSelect, ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE, (adi_adrv904x_Channels_e)(1U << chanIdx));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       chanIdx,
                                       "GpioSignalRelease issue");
                goto cleanup;
            }
        }
    }

    /* Config RSSI GPIO if valid */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; chanIdx++)
    {
        /* Set if pinSelect is valid */
        if (gpioCfg->rxChannel[chanIdx].enablePin != ADI_ADRV904X_GPIO_INVALID)
        {    
            pinSelect = gpioCfg->rxChannel[chanIdx].enablePin;
            /* Set the signal for this gpio */
            recoveryAction = adrv904x_GpioSignalSet(device, pinSelect, ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE, 1U<< chanIdx);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       chanIdx,
                                       "GpioSignalSet issue");
                goto cleanup;
            }
        }

        /* Set if pinSelect is valid */
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL; carrierIdx++)
        {
        
            if (gpioCfg->rxChannel[chanIdx].readyPin[carrierIdx] != ADI_ADRV904X_GPIO_INVALID)
            {    
                pinSelect = gpioCfg->rxChannel[chanIdx].readyPin[carrierIdx];

                /* Set the signal for this gpio */
                recoveryAction = adrv904x_GpioSignalSet(device, pinSelect, cddcSignals[carrierIdx], 1U << chanIdx);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common,
                                           recoveryAction,
                                           carrierIdx,
                                           "GpioSignalSet issue");
                    goto cleanup;
                }
            }
        }
    
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiGpioCfgGet(adi_adrv904x_Device_t * const               device,
                                                                      adi_adrv904x_RxCarrierRssiGpioCfg_t * const gpioCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t chanIdx = 0U;
    uint32_t carrierIdx = 0U;
    adi_adrv904x_GpioSignal_e cddcSignals[ADI_ADRV904X_MAX_RX_ONLY] = {
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_0,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_1,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_2,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_3,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_4,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_5,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_6,
        ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_7
    };

    adi_adrv904x_GpioPinSel_e pinSelect = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioCfg, cleanup);

    /* Find all rssi pin */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_RX_ONLY; chanIdx++)
    {    
        /* Find the signal for this gpio index */
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS_PER_CHANNEL; carrierIdx++)
        {
            recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, cddcSignals[carrierIdx], (adi_adrv904x_Channels_e)(1U << chanIdx));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       carrierIdx,
                                       "GpioSignalFind issue");
                goto cleanup;
            }

            gpioCfg->rxChannel[chanIdx].readyPin[carrierIdx] = pinSelect;
        }

        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE, (adi_adrv904x_Channels_e)(1U << chanIdx));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   chanIdx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        gpioCfg->rxChannel[chanIdx].enablePin = pinSelect;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierRssiPowerRead(adi_adrv904x_Device_t* const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t* const rxCarrierMask,
                                                                     int32_t* const                            gain_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint64_t  bfValue = 0U;
    adrv904x_BfCddcFuncsChanAddr_e cddcFuncsChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gain_mdB, cleanup);

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:
            carrierIdx = 0U;
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            carrierIdx = 1U;
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            carrierIdx = 2U;
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            carrierIdx = 3U;
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            carrierIdx = 4U;
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            carrierIdx = 5U;
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            carrierIdx = 6U;
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            carrierIdx = 7U;
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            goto cleanup;
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
        goto cleanup;
    }

    /* Read out the configurations */ 
    /* Convert the rxSel to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcFuncsBitfieldAddressGet(  device,
                                                            (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask),
                                                            &cddcFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Channel used to determine SPI address");
        goto cleanup;
    }

    /* rssi__latch_linear_power_readback */
    recoveryAction = adrv904x_CddcFuncs_RssiLatchLinearPowerReadback_BfSet(device,
                                                                           NULL,
                                                                           cddcFuncsChanBaseAddr,
                                                                           carrierIdx,
                                                                           1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting rssi latch linear power readback");
        goto cleanup;
    }

    /* rssi_linear_power_readback 
     * The BF function will read 2 32-bit registers and combine them into 64 bit bfValue value
     */
    recoveryAction = adrv904x_CddcFuncs_RssiLinearPowerReadback_BfGet(device,
                                                                      NULL,
                                                                      cddcFuncsChanBaseAddr,
                                                                      carrierIdx,
                                                                      &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting rssi linear power readback");
        goto cleanup;
    }

    /* The bfValue is in 0.36 bit format. Convert it to mdb */
    //*gain_mdB = 10 * log10(bfValue / pow(2,36)) * 1000;
    *gain_mdB = 10000;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierGainAdjustSet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                     const int32_t                              gain_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t rxIdx = 0U;     
    uint32_t rxSel = 0U;
    uint32_t  bfValue = 0U;
    const int32_t DIG_GAIN_MIN = -90000;
    const int32_t DIG_GAIN_MAX = 36000;
    const uint32_t DIG_GAIN_MULT = 65535U;
    adrv904x_BfCddcHbDpathChanAddr_e cddcHbDpathChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);

    if (((rxCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_RX_CARRIER_ALL)) != 0U) || 
         (rxCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_RX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->carrierMask,
                               "Invalid Rx carrier mask is selected. Valid values are any combinations of RxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((rxCarrierMask->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U) || 
       (rxCarrierMask->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxCarrierMask->rxChannelMask,
                               "Invalid Rx channel mask is selected. Valid values are any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if ((gain_mdB < DIG_GAIN_MIN) || (gain_mdB > DIG_GAIN_MAX))
    {
        /* Invalid gain_mdB selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gain_mdB, "Invalid gain_mdB selection. Valid value are from -90000mdB to 36000mdB");
        goto cleanup;
    }

    /* Convert from mdB to 7.16. (reg value = 10**(value in mdB/1000/20)) * 2^16) */
    //bfValue = (uint32_t)((double)pow(10, (double)gain_mdB / 1000U / 20U) * DIG_GAIN_MULT);
    bfValue = 100;

    /* Write out the enable */
    for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
    {
        rxSel = 1U << rxIdx;
        if ((rxCarrierMask->rxChannelMask & rxSel) > 0U)
        {
            /* Convert the rxSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CddcHbDpathBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxSel), &cddcHbDpathChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxSel, "Invalid Rx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_RX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((rxCarrierMask->carrierMask & carrierSel) > 0U)
                {
                    switch (carrierSel)
                    {
                        case ADI_ADRV904X_RX_CARRIER_0:            
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain0_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_1:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain1_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_2:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain2_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_3:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain3_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_4:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain4_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_5:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain5_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_6:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain6_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_RX_CARRIER_7:
                            recoveryAction = adrv904x_CddcHbDpath_CarrierGain7_BfSet(device,
                                                                                     NULL,
                                                                                     cddcHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        default:
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common,
                                                   recoveryAction,
                                                   rxCarrierMask->carrierMask,
                                                   "Invalid Rx carrier selection");
                            goto cleanup;
                            break;
                    }

                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting carrier gain");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_CddcHbDpath_CarrierGainEnable_BfSet(device,
                                                                                  NULL,
                                                                                  cddcHbDpathChanBaseAddr,
                                                                                  carrierIdx,
                                                                                  1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting carrier gain enable");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierGainAdjustGet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_RxCarrierMask_t * const rxCarrierMask,
                                                                     int32_t * const                            gain_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t  bfValue = 0U;
    const uint32_t DIG_GAIN_MULT = 65535U;

    adrv904x_BfCddcHbDpathChanAddr_e cddcHbDpathChanBaseAddr = ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gain_mdB, cleanup);

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
        goto cleanup;
    }

    *gain_mdB = 0;

    /* Read out the configurations */ 
    /* Convert the rxCarrierMask->rxChannelMask to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CddcHbDpathBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(rxCarrierMask->rxChannelMask), &cddcHbDpathChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierMask->rxChannelMask, "Invalid Rx Channel used to determine SPI address");
        goto cleanup;
    }

    switch (rxCarrierMask->carrierMask)
    {
        case ADI_ADRV904X_RX_CARRIER_0:            
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain0_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_1:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain1_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_2:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain2_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_3:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain3_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_4:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain4_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_5:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain5_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_6:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain6_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_RX_CARRIER_7:
            recoveryAction = adrv904x_CddcHbDpath_CarrierGain7_BfGet(device,
                                                                     NULL,
                                                                     cddcHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   rxCarrierMask->carrierMask,
                                   "Invalid Rx carrier selection");
            goto cleanup;
            break;
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Rx Carrier Gain.");
        goto cleanup;
    }

    if (bfValue != 0U)
    {
        /* Convert from 7.16 to mdB.  value in mdB = (1000*20*log10(reg value/2^16)) */
        //*gain_mdB = (int32_t)(1000U * 20U * log10((double)bfValue / DIG_GAIN_MULT));
        *gain_mdB = 12;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierReconfigureWithFilterSelect(  adi_adrv904x_Device_t* const                                device,
                                                                                            adi_adrv904x_CarrierJesdCfg_t* const                        jesdCfg,
                                                                                            adi_adrv904x_CarrierRadioCfg_t                              rxCarrierConfigs[],
                                                                                            const adi_adrv904x_CarrierChannelFilterApplicationSel_t     rxCarrierChannelFilterApplicationSel[],
                                                                                            const uint32_t                                              numCarrierProfiles)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ChannelFilterCfg_t rxCarrierChannelFilter[ADI_ADRV904X_MAX_NUM_PROFILES];
    uint32_t carrierIdx = 0U;
    uint32_t profileIdx = 0U;
    uint32_t coeffIdx = 0U;
    int32_t coeffTableIdx = 0U;
    const int16_t *coeffTable = NULL;
    int16_t coeffTableSize = 0;
    uint32_t numberOfFilterTaps = 0U;
    uint8_t asymmetricFilterTaps = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierChannelFilterApplicationSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, jesdCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(&rxCarrierChannelFilter, 0, sizeof(rxCarrierChannelFilter));
 
    if (numCarrierProfiles > ADI_ADRV904X_MAX_NUM_PROFILES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCarrierProfiles, "Up to four profiles allowed.");
        goto cleanup;
    }
    
    for (profileIdx = 0U; profileIdx < numCarrierProfiles; profileIdx++)
    {
        /* Reset coefficient index for each profile */
        coeffIdx = 0U;
        
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            if (rxCarrierConfigs[profileIdx].carriers[carrierIdx].enable == 1U)
            {
                recoveryAction = adrv904x_ChannelFilterCoefsGet( device,
                                                                rxCarrierChannelFilterApplicationSel[profileIdx].channelFilterApplicationSel[carrierIdx],
                                                                &rxCarrierConfigs[profileIdx].carriers[carrierIdx],
                                                                &coeffTable,
                                                                &coeffTableSize,
                                                                &numberOfFilterTaps,
                                                                &asymmetricFilterTaps,
                                                                ADI_TRUE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting carrier chanel filter");
                    goto cleanup;
                }
        
                for (coeffTableIdx = 0U; coeffTableIdx < coeffTableSize; coeffTableIdx++)
                {
                    rxCarrierChannelFilter[profileIdx].coeffs[coeffIdx] = coeffTable[coeffTableIdx];
                    coeffIdx++;
                    if (coeffIdx >= ADI_ADRV904X_NUM_CF_COEFFICIENTS)
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, coeffIdx, "Carrier coefficients did not fit in the space provided");
                        goto cleanup;
                    }
                }
                rxCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx] = numberOfFilterTaps;
                rxCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx] = asymmetricFilterTaps; 
            }
        }
    }
    
    recoveryAction = adi_adrv904x_RxDynamicCarrierReconfigure(  device,
                                                                jesdCfg,
                                                                &rxCarrierConfigs[0],
                                                                &rxCarrierChannelFilter[0],
                                                                numCarrierProfiles);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
        goto cleanup;
    }

cleanup:

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxDynamicCarrierReconfigure(  adi_adrv904x_Device_t* const            device,
                                                                            adi_adrv904x_CarrierJesdCfg_t* const    jesdCfg,
                                                                            adi_adrv904x_CarrierRadioCfg_t          rxCarrierConfigs[],
                                                                            adi_adrv904x_ChannelFilterCfg_t         rxCarrierChannelFilter[],
                                                                            const uint32_t                          numCarrierProfiles)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char coeffLoadBuf[sizeof(adi_adrv904x_ChannelFilterLoadMaxSize_t)];
    adi_adrv904x_ChannelFilterLoad_t* const channelFilterLoad = (adi_adrv904x_ChannelFilterLoad_t*)&coeffLoadBuf;
    adrv904x_CarrierDynamicReconfigInternalCfg_t rxCarrierInternalCfg;
    adi_adrv904x_ChannelFilterResp_t chFilterCmdRsp;
    adi_adrv904x_ChannelFilterLoadResp_t chFilterLoadCmdRsp;
    uint32_t rxIdx = 0U;
    uint32_t rxSel = 0U;
    uint32_t bandIdx = 0U;
    uint32_t carrierIdx = 0U;
    uint32_t coeffLoadIdx = 0U;
    uint32_t profileIdx = 0U;
    uint32_t coeffAddressOffset = 0U;
    uint32_t coeffIdx = 0U;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adi_adrv904x_RxTxLoFreqReadback_t rxTxLoReadback;
    adi_adrv904x_RxNcoConfigReadbackResp_t rxRbConfig;
    uint32_t currentBandCenter_kHz = 0U;
    adrv904x_CarrierDynamicReconfigProfileCfg_t* pProfileCfg = NULL;
    adrv904x_CarrierInitialCfg_t* pInitialCfg = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierChannelFilter, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, jesdCfg, cleanup);

    ADI_LIBRARY_MEMSET(&chFilterLoadCmdRsp, 0, sizeof(chFilterLoadCmdRsp));
    ADI_LIBRARY_MEMSET(&rxCarrierInternalCfg, 0, sizeof(rxCarrierInternalCfg));
    ADI_LIBRARY_MEMSET(&rxTxLoReadback, 0, sizeof(rxTxLoReadback));
    ADI_LIBRARY_MEMSET(&rxRbConfig, 0, sizeof(rxRbConfig));
 
    if (numCarrierProfiles > ADI_ADRV904X_MAX_NUM_PROFILES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCarrierProfiles, "Up to four profiles allowed.");
        goto cleanup;
    }
    
    /* Reset the internal structures for each profile */
    recoveryAction = adrv904x_InternalReconfigStructInit(   device,
                                                            &rxCarrierInternalCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Could not initialize struct");
        goto cleanup;
    }
    
    recoveryAction = adi_adrv904x_RxTxLoFreqGet(device,
                                                &rxTxLoReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RxTxLoFreqGet failed");
        goto cleanup;
    }

    for (profileIdx = 0U; profileIdx < numCarrierProfiles; profileIdx++)
    {
        pProfileCfg = &rxCarrierInternalCfg.profileCfgs[profileIdx];
        pInitialCfg = &(pProfileCfg->initialCfg);
        
        /* rxChannelMask should larger than 0 and less than 0x100 */
        if ((rxCarrierConfigs[profileIdx].channelMask == 0U) ||
            ((rxCarrierConfigs[profileIdx].channelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierConfigs[profileIdx].channelMask, "Invalid chanSelect provided.");
            goto cleanup;
        }
        
        /* 
         * For each channel in this profile verify it was initialized with the same profile.
         * The band settings were extracted on a channel basis. We'll find the first channel that's assigned to this profile
         * and use it's band setting. Then verify the remaining channels in this profile match.
         * 
         * */
        for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
        {
            rxSel = 1U << rxIdx;
            if ((rxCarrierConfigs[profileIdx].channelMask & rxSel) > 0U)
            {
                if (device->initExtract.chanAssign[rxIdx] != profileIdx)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxIdx, "Channels cannot be reconfigured using a different profile than the profile select they were initialized.");
                    goto cleanup;
                }
                
                for (bandIdx = 0U; bandIdx < ADI_ADRV904X_DDC_NUM_BAND; bandIdx++)
                {
                    if (device->initExtract.rx.rxChannelCfg[rxIdx].bandSettings[bandIdx].enabled == ADI_TRUE)
                    {
                        ADI_LIBRARY_MEMSET(&rxRbConfig, 0, sizeof(rxRbConfig));
                        
                        rxRbConfig.bandSelect = bandIdx;
                        rxRbConfig.chanSelect = rxSel;
    
                        recoveryAction = adi_adrv904x_RxNcoShifterGet(  device, &rxRbConfig);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RxTxLoFreqGet failed");
                            goto cleanup;
                        }
                    
                        currentBandCenter_kHz = rxTxLoReadback.rxFreq_Khz[rxIdx] - rxRbConfig.frequencyKhz;
                        
                        if (pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz == 0U)
                        {
                            /* Copy the band settings but use the NCO shift value to shift the center frequency based on the current runtime settings */
                            ADI_LIBRARY_MEMCPY( &(pInitialCfg->bandSettings[bandIdx]), 
                                                &(device->initExtract.rx.rxChannelCfg[rxIdx].bandSettings[bandIdx]), 
                                                sizeof(pInitialCfg->bandSettings[bandIdx]));
                            pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz = currentBandCenter_kHz;
                            pInitialCfg->maxSlot = device->initExtract.rx.rxChannelCfg[rxIdx].maxSlot;
                        }
                        else if (pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz != currentBandCenter_kHz)
                        {
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rxCarrierConfigs[profileIdx].channelMask, "Each channel in a single profile must have matching band center frequencies, e.g. all NCOs in a single profile must be the same value.");
                            goto cleanup;
                        }  
                    }
                }
            }
        }
        
        recoveryAction = adrv904x_RxCarrierBandSorting( device,
                                                        &rxCarrierConfigs[profileIdx],
                                                        pProfileCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid band selection");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_RxCarrierNcoReconfig( device,
                                                        &rxCarrierConfigs[profileIdx],
                                                        pProfileCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid NCO settings");
            goto cleanup;
        } 
        
        recoveryAction = adrv904x_CarrierJesdParametersCalculate(   device,
                                                                    &rxCarrierConfigs[profileIdx],
                                                                    pProfileCfg,
                                                                    ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid JESD settings");
            goto cleanup;
        }
        
		recoveryAction = adrv904x_CarrierRxJesdConfigSet(   device,
															rxCarrierConfigs[profileIdx].channelMask,
															pProfileCfg);
		if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
		{
			ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
			goto cleanup;
		}
        
        /* Reset the buffer each profile. channelFilterLoad maps to this buffer */
        ADI_LIBRARY_MEMSET(&coeffLoadBuf, 0, sizeof(coeffLoadBuf));
        
        /* Perform endianness correction and load channel filter coefficients */
        channelFilterLoad->coeffIdx = 0U;
        for (coeffIdx = 0U; coeffIdx < ADI_ADRV904X_NUM_CF_COEFFICIENTS; coeffIdx++)
        {
            coeffAddressOffset = coeffLoadIdx * sizeof(int16_t);
            ADI_LIBRARY_MEMCPY((void*)((uint8_t*)channelFilterLoad + sizeof(adi_adrv904x_ChannelFilterLoad_t) + coeffAddressOffset), (void*)(&rxCarrierChannelFilter[profileIdx].coeffs[coeffIdx]), sizeof(int16_t));
            coeffLoadIdx++;
        
            /* Once we fill up enough coefficients or we're on the last coefficient send to fw and clear out the buffer */
            if ((coeffLoadIdx == ADI_ADRV904X_CHAN_FILTER_COEFF_LOAD_LEN) || (coeffIdx == (ADI_ADRV904X_NUM_CF_COEFFICIENTS - 1U)))
            {
                /* Using coeffLoadIdx after incremented to give us the number of coefficients actually loaded */
                channelFilterLoad->coeffNum = (uint16_t)ADRV904X_HTOCL(coeffLoadIdx);
    
                /* Send command and receive response */
                recoveryAction = adrv904x_CpuCmdSend(   device,
                                                        ADI_ADRV904X_CPU_TYPE_0,
                                                        ADRV904X_LINK_ID_0,
                                                        ADRV904X_CPU_CMD_ID_LOAD_CHANNEL_FILTER_COEFFS,
                                                        (void*)channelFilterLoad,
                                                        sizeof(coeffLoadBuf),
                                                        (void*)&chFilterLoadCmdRsp,
                                                        sizeof(chFilterLoadCmdRsp),
                                                        &cmdStatus);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterLoadCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
                }
            
                ADI_LIBRARY_MEMSET(&coeffLoadBuf, 0, sizeof(coeffLoadBuf));
            
                channelFilterLoad->coeffIdx = coeffIdx + 1;
                coeffLoadIdx = 0U;
            }
        }
        
        /* Perform endianness correction and load channel filter configs */
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            rxCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx] = (uint32_t)ADRV904X_HTOCL(rxCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx]);
            rxCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx] = rxCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx];
        }
    
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_LOAD_CHANNEL_FILTER_CFG,
                                                (void*)&(rxCarrierChannelFilter[profileIdx].carrierFilterCfg),
                                                sizeof(rxCarrierChannelFilter[profileIdx].carrierFilterCfg),
                                                (void*)&chFilterLoadCmdRsp,
                                                sizeof(chFilterLoadCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterLoadCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        
        /* Endianness correction */
        rxCarrierConfigs[profileIdx].channelMask = (uint32_t)ADRV904X_HTOCL(rxCarrierConfigs[profileIdx].channelMask);
    
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            rxCarrierConfigs[profileIdx].carriers[carrierIdx].enable = rxCarrierConfigs[profileIdx].carriers[carrierIdx].enable;
            rxCarrierConfigs[profileIdx].carriers[carrierIdx].sampleRate_kHz = (uint32_t)ADRV904X_HTOCL(rxCarrierConfigs[profileIdx].carriers[carrierIdx].sampleRate_kHz);
            rxCarrierConfigs[profileIdx].carriers[carrierIdx].ibw_kHz = (uint32_t)ADRV904X_HTOCL(rxCarrierConfigs[profileIdx].carriers[carrierIdx].ibw_kHz);
            rxCarrierConfigs[profileIdx].carriers[carrierIdx].centerFrequency_kHz = (uint32_t)ADRV904X_HTOCL(rxCarrierConfigs[profileIdx].carriers[carrierIdx].centerFrequency_kHz);
        }
        
        /* Reset for each profile */
        ADI_LIBRARY_MEMSET(&chFilterCmdRsp, 0, sizeof(chFilterCmdRsp));
        
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_RX_CARRIER_RECONFIGURE,
                                                (void*)&rxCarrierConfigs[profileIdx],
                                                sizeof(adi_adrv904x_CarrierRadioCfg_t),
                                                (void*)&chFilterCmdRsp,
                                                sizeof(chFilterCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        
        recoveryAction = adrv904x_CddcDelayCalculate(   device,
                                                        &rxCarrierConfigs[profileIdx],
                                                        pProfileCfg,
                                                        &chFilterCmdRsp.carrierFilterOutCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with delay calculation");
            goto cleanup;
        }
        
        for (rxIdx = 0U; rxIdx < ADI_ADRV904X_MAX_RX_ONLY; rxIdx++)
        {
            rxSel = 1U << rxIdx;
            if ((rxCarrierConfigs[profileIdx].channelMask & rxSel) > 0U)
            {
                recoveryAction = adrv904x_RxCarrierConfigSet(   device,
                                                                rxIdx,
                                                                pProfileCfg);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
                    goto cleanup;
                }
            
                /* TPGSWE-7944: Skip following write due to BfSet bugs */

                /* Note: The following API TxCarrierDelaySet() is reported not working.
                recoveryAction = adrv904x_RxCarrierDelaySet(   device,
                                                                rxIdx,
                                                                &rxCarrierInternalCfg.delayCfg);
                                                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                                                    {
                                                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring delay params");
                                                        goto cleanup;
                                            }
                                            */
            
                    
                ADI_LIBRARY_MEMCPY(&device->initExtract.rxCarrierConfigs[rxIdx], &rxCarrierConfigs[profileIdx], sizeof(adi_adrv904x_CarrierRadioCfg_t));
                ADI_LIBRARY_MEMCPY(&device->initExtract.rx.rxChannelCfg[rxIdx].carrierRuntimeSettings, &pProfileCfg->carrierCfgs, sizeof(adi_adrv904x_CarrierRuntime_t));
            }
        }
    }
    
    recoveryAction = adrv904x_CalculateRxSampleXbarSlotConfig(device, jesdCfg, &rxCarrierInternalCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        return recoveryAction;
    }
	
	recoveryAction = adrv904x_RxJesdConfigSet( device,
											   &rxCarrierInternalCfg);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
		goto cleanup;
	}
		
	/* Update stored JESD settings */
	ADI_LIBRARY_MEMCPY(&device->initExtract.rxCarrierJesdCfg, jesdCfg, sizeof(adi_adrv904x_CarrierJesdCfg_t));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierSettingsGet( adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t rxChannel,
                                                                    adi_adrv904x_CarrierRadioCfg_t* const rxCarrierConfigs,
                                                                    adi_adrv904x_CarrierRuntime_t* const  carrierRuntimeSettings)
{
    
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, carrierRuntimeSettings, cleanup);
    
    ADI_LIBRARY_MEMCPY(rxCarrierConfigs, &device->initExtract.rxCarrierConfigs[rxChannel], sizeof(adi_adrv904x_CarrierRadioCfg_t));
    ADI_LIBRARY_MEMCPY(carrierRuntimeSettings, &device->initExtract.rx.rxChannelCfg[rxChannel].carrierRuntimeSettings, sizeof(adi_adrv904x_CarrierRuntime_t));
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierJesdCfgGet( adi_adrv904x_Device_t* const    device,
    adi_adrv904x_CarrierJesdCfg_t * const rxCarrierJesdCfg)
{
    
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierJesdCfg, cleanup);
    
    ADI_LIBRARY_MEMCPY(rxCarrierJesdCfg, &device->initExtract.rxCarrierJesdCfg, sizeof(adi_adrv904x_CarrierJesdCfg_t));
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxCarrierLatencyGet(  adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_RxChannels_e rxChannel,
                                                                    adi_adrv904x_CarrierReconfigLatencyCfg_t* const rxCarrierLatencyCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxCarrierLatencyCfg, cleanup);

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
        goto cleanup;
    }


    ADI_LIBRARY_MEMCPY((adi_adrv904x_CarrierReconfigLatencyCfg_t*)rxCarrierLatencyCfg,
                       (adi_adrv904x_CarrierReconfigLatencyCfg_t*)&(device->devStateInfo.rxCarrierLatencyCfg[adrv904x_RxChannelsToId(rxChannel)]),
                       sizeof(adi_adrv904x_CarrierReconfigLatencyCfg_t));

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiConfigSet(adi_adrv904x_Device_t* const          device,
                                                              const uint16_t                        meterMask,
                                                              const uint32_t                        channelMask,
                                                              const adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrRssiConfigSet_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;
    uint8_t ddcMeter  = ADI_FALSE;
    uint8_t cddcMeter = ADI_FALSE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrRssiCfg, cleanup);

    (void) ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    (void) ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    if ((meterMask & 0x3u) != 0u)
    {
        ddcMeter = ADI_TRUE;
    }
    if ((meterMask & 0x3FCu) != 0u)
    {
        cddcMeter = ADI_TRUE;
    }
    if ((ddcMeter == ADI_TRUE) && (cddcMeter == ADI_TRUE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               meterMask,
                               "Invalid RSSI meter mask (expect DDC or CDDC mask not both) ");
        goto cleanup;
    }

    if ((ddcMeter == ADI_FALSE) && (cddcMeter == ADI_FALSE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               meterMask,
                               "Invalid RSSI meter mask (cannot be 0) ");
        goto cleanup;
    }

    recoveryAction = adrv904x_ValidateMask((uint8_t)channelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelMask,
                               "Invalid channel mask");
        goto cleanup;
    }

    if (ddcMeter == ADI_TRUE)
    {
        if (pPwrMtrRssiCfg->gaintableStepSize > 0x3FU)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   pPwrMtrRssiCfg->gaintableStepSize,
                                   "RSSI gain table step size. Valid values are <= 0x3F");
            goto cleanup;
        }
    }

    if (pPwrMtrRssiCfg->opMode >= 2u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->opMode,
                               "RSSI operation mode is invalid. Valid values are 0 and 1");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->measMode > 2u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->measMode,
                               "RSSI measurement mode is invalid. Valid values are 0 - 2");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->startDelay > 0x1FFFU)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->startDelay,
                               "RSSI startDelay is invalid. Valid values are <= 0x1FFF");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->contDelay > 0x1FFFU)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->contDelay,
                               "RSSI contDelay is invalid. Valid values are <= 0x1FFF");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->waitDelay > 0xFFFFFFU)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->waitDelay,
                               "RSSI waitDelay is invalid. Valid values are <= 0xFFFFFF");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->fddPinMode > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->fddPinMode,
                               "RSSI fddPinMode is invalid. Valid values are 0 and 1");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->duration0 > 31U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->duration0,
                               "RSSI duration0 is invalid. Valid values are 0 - 31");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->duration1 > 31U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->duration1,
                               "RSSI duration1 is invalid. Valid values are 0 - 31");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->duration2 > 31U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->duration2,
                               "RSSI duration2 is invalid. Valid values are 0 - 31");
        goto cleanup;
    }

    if (pPwrMtrRssiCfg->duration3 > 31U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pPwrMtrRssiCfg->duration3,
                               "RSSI duration3 is invalid. Valid values are 0 - 31");
        goto cleanup;
    }

    cmdStruct.meterMask                                = meterMask;
    cmdStruct.chanMask                                 = (uint8_t)channelMask;
    cmdStruct.pwrMtrRssiCfg.enableGainCalc             = (uint8_t)pPwrMtrRssiCfg->enableGainCalc;
    cmdStruct.pwrMtrRssiCfg.gaintableStepSize          = (uint8_t)pPwrMtrRssiCfg->gaintableStepSize;
    cmdStruct.pwrMtrRssiCfg.resetAtGainChange          = (uint8_t)pPwrMtrRssiCfg->resetAtGainChange;
    cmdStruct.pwrMtrRssiCfg.radioSeqPinModeEnable      = (uint8_t)pPwrMtrRssiCfg->radioSeqPinModeEnable;
    cmdStruct.pwrMtrRssiCfg.opMode                     = (uint8_t)pPwrMtrRssiCfg->opMode;
    cmdStruct.pwrMtrRssiCfg.measMode                   = (uint8_t)pPwrMtrRssiCfg->measMode;
    cmdStruct.pwrMtrRssiCfg.startDelay                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->startDelay);
    cmdStruct.pwrMtrRssiCfg.contDelay                  = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->contDelay);
    cmdStruct.pwrMtrRssiCfg.waitDelay                  = (uint32_t)ADRV904X_HTOCL(pPwrMtrRssiCfg->waitDelay);
    cmdStruct.pwrMtrRssiCfg.fddPinMode                 = (uint8_t)pPwrMtrRssiCfg->fddPinMode;
    cmdStruct.pwrMtrRssiCfg.duration0                  = (uint8_t)pPwrMtrRssiCfg->duration0;
    cmdStruct.pwrMtrRssiCfg.duration1                  = (uint8_t)pPwrMtrRssiCfg->duration1;
    cmdStruct.pwrMtrRssiCfg.duration2                  = (uint8_t)pPwrMtrRssiCfg->duration2;
    cmdStruct.pwrMtrRssiCfg.duration3                  = (uint8_t)pPwrMtrRssiCfg->duration3;
    cmdStruct.pwrMtrRssiCfg.mulFactor0                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->mulFactor0);
    cmdStruct.pwrMtrRssiCfg.mulFactor1                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->mulFactor1);
    cmdStruct.pwrMtrRssiCfg.mulFactor2                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->mulFactor2);
    cmdStruct.pwrMtrRssiCfg.mulFactor3                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfg->mulFactor3);

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_SET,
                                            (uint8_t*)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t*)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiConfigGet(adi_adrv904x_Device_t* const    device,
                                                              const uint16_t                  meterSel,
                                                              const uint32_t                  channelSel,
                                                              adi_adrv904x_RxRssiCfg_t* const pPwrMtrRssiCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrRssiConfigGet_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrRssiConfigGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcDfePwrMtrRssiCfg_t *pPwrMtrRssiCfgResp = &respStruct.pwrMtrRssiCfg;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrRssiCfg, cleanup);

    (void) ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    (void) ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Only one meter allowed */
    if (!((meterSel == 0x001u) || /* DDC_0  */
          (meterSel == 0x002u) || /* DDC_1  */
          (meterSel == 0x004u) || /* CDDC_0 */
          (meterSel == 0x008u) || /* CDDC_1 */
          (meterSel == 0x010u) || /* CDDC_2 */
          (meterSel == 0x020u) || /* CDDC_3 */
          (meterSel == 0x040u) || /* CDDC_4 */
          (meterSel == 0x080u) || /* CDDC_5 */
          (meterSel == 0x100u) || /* CDDC_6 */
          (meterSel == 0x200u)))  /* CDDC_7 */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               meterSel,
                               "Invalid meter type - only expect one meter");
        goto cleanup;
    }

    recoveryAction = adrv904x_ValidateMask((uint8_t)channelSel, 1u); /* don't allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelSel,
                               "Invalid channel mask");
        goto cleanup;
    }

    cmdStruct.meterMask = meterSel;
    cmdStruct.chanMask  = channelSel;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_CONFIG_GET,
                                            (uint8_t*)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t*)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    pPwrMtrRssiCfg->channelMask                = (uint8_t)channelSel;
    pPwrMtrRssiCfg->enableGainCalc             = (uint8_t)pPwrMtrRssiCfgResp->enableGainCalc;
    pPwrMtrRssiCfg->gaintableStepSize          = (uint8_t)pPwrMtrRssiCfgResp->gaintableStepSize;
    pPwrMtrRssiCfg->resetAtGainChange          = (uint8_t)pPwrMtrRssiCfgResp->resetAtGainChange;
    pPwrMtrRssiCfg->radioSeqPinModeEnable      = (uint8_t)pPwrMtrRssiCfgResp->radioSeqPinModeEnable;
    pPwrMtrRssiCfg->opMode                     = (adi_adrv904x_RxDdcRssiOpMode_e)pPwrMtrRssiCfgResp->opMode;
    pPwrMtrRssiCfg->measMode                   = (adi_adrv904x_RxDdcRssiMeasMode_e)pPwrMtrRssiCfgResp->measMode;
    pPwrMtrRssiCfg->startDelay                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->startDelay);
    pPwrMtrRssiCfg->contDelay                  = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->contDelay);
    pPwrMtrRssiCfg->waitDelay                  = (uint32_t)ADRV904X_HTOCL(pPwrMtrRssiCfgResp->waitDelay);
    pPwrMtrRssiCfg->fddPinMode                 = (uint8_t)pPwrMtrRssiCfgResp->fddPinMode;
    pPwrMtrRssiCfg->duration0                  = (uint8_t)pPwrMtrRssiCfgResp->duration0;
    pPwrMtrRssiCfg->duration1                  = (uint8_t)pPwrMtrRssiCfgResp->duration1;
    pPwrMtrRssiCfg->duration2                  = (uint8_t)pPwrMtrRssiCfgResp->duration2;
    pPwrMtrRssiCfg->duration3                  = (uint8_t)pPwrMtrRssiCfgResp->duration3;
    pPwrMtrRssiCfg->mulFactor0                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->mulFactor0);
    pPwrMtrRssiCfg->mulFactor1                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->mulFactor1);
    pPwrMtrRssiCfg->mulFactor2                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->mulFactor2);
    pPwrMtrRssiCfg->mulFactor3                 = (uint16_t)ADRV904X_HTOCS(pPwrMtrRssiCfgResp->mulFactor3);

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiEnable(adi_adrv904x_Device_t* const device,
                                                           const uint16_t               meterMask,
                                                           const uint32_t               channelMask,
                                                           const uint8_t                enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrRssiEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    (void) ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    (void) ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    if ((meterMask == 0u) || (meterMask > 0x3FF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               meterMask,
                               "Invalid meter mask - expect 0x1 to 0x3FF");
        goto cleanup;
    }

    /* Validate channelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelMask,
                               "Invalid channel mask");
        goto cleanup;
    }


    cmdStruct.meterMask = meterMask;
    cmdStruct.chanMask  = channelMask;
    cmdStruct.enable    = enable;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_ENABLE,
                                            (uint8_t*)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t*)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxRssiReadBack(adi_adrv904x_Device_t* const device,
                                                             const uint16_t               meterSel,
                                                             const uint32_t               channelSel,
                                                             float* const                 pPwrMeasDb,
                                                             float* const                 pPwrMeasLinear)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrRssiReadback_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrRssiReadbackResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    (void) ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    (void) ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeasDb, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeasLinear, cleanup)

    /* Only one meter allowed */
    if (!((meterSel == 0x001u) || /* DDC_0  */
          (meterSel == 0x002u) || /* DDC_1  */
          (meterSel == 0x004u) || /* CDDC_0 */
          (meterSel == 0x008u) || /* CDDC_1 */
          (meterSel == 0x010u) || /* CDDC_2 */
          (meterSel == 0x020u) || /* CDDC_3 */
          (meterSel == 0x040u) || /* CDDC_4 */
          (meterSel == 0x080u) || /* CDDC_5 */
          (meterSel == 0x100u) || /* CDDC_6 */
          (meterSel == 0x200u)))  /* CDDC_7 */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               meterSel,
                               "Invalid RSSI meter mask");
        goto cleanup;
    }

    recoveryAction = adrv904x_ValidateMask((uint8_t)channelSel, 1u); /* Only one allowed */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelSel,
                               "Invalid channel mask");
        goto cleanup;
    }

    cmdStruct.chanMask  = channelSel;
    cmdStruct.meterMask = meterSel;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_RSSI_READBACK,
                                            (uint8_t*)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t*)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (   (respStruct.chanMask != cmdStruct.chanMask)
        || (respStruct.meterMask != cmdStruct.meterMask))
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "response mismatch.");
        goto cleanup;
    }

    *pPwrMeasDb     = (float)respStruct.pwrMeasDb / 4.0f;
    //*pPwrMeasLinear = 10 * log10(respStruct.pwrMeasLinear / pow(2,36));
    *pPwrMeasLinear = 10;
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
