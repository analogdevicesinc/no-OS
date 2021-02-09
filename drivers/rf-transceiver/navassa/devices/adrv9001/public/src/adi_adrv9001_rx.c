/**
* \file
* \brief Contains Rx features related function implementation defined in
* adi_adrv9001_rx.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adi_adrv9001_rx.h"

/* ADI specific header files */
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_rx_gaincontrol.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_rx.h"
#include "adrv9001_validators.h"

/* Header files related to libraries */


/* System header files */


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/
#define ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX  8U

static int32_t __maybe_unused adi_adrv9001_Rx_GainTable_Write_Validate(adi_adrv9001_Device_t *device,
                                       uint32_t rxChannelMask,
                                       uint8_t  gainIndexOffset,
                                       adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize)
{
    static const uint8_t MAX_EXT_CTRL_WORD = 3;
    static const uint8_t MAX_ADC_TIA_GAIN  = 63;
    static const int16_t MIN_DIG_GAIN = -360; /*Dig gain is in the range -18dB to 50db*/
    static const int16_t MAX_DIG_GAIN = 1000; /*Dig gain is in the range -18dB to 50db*/

    uint16_t  gainIndex = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);

    /*Check that the gain index offset is within range*/
    ADI_RANGE_CHECK(device, gainIndexOffset, ADI_ADRV9001_MIN_RX_GAIN_TABLE_INDEX, ADI_ADRV9001_START_RX_GAIN_INDEX);

    /*Check the no. of gain indices parameter is valid*/
    if (arraySize > ((gainIndexOffset - ADI_ADRV9001_MIN_RX_GAIN_TABLE_INDEX) + 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "gainTableRow arraySize exceeds the limit. Valid range 0 to (gainIndexOffset - ADI_ADRV9001_MIN_RX_GAIN_TABLE_INDEX +1)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that rxChannelMask is valid*/
    ADI_RANGE_CHECK(device, rxChannelMask, ADI_CHANNEL_1, (ADI_CHANNEL_1 | ADI_CHANNEL_2));

    /*Check that Rx profile or ORx profile is valid*/
    if (((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_RX_PROFILE_VALID)) == 0) &&
        ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_ORX_PROFILE_VALID)) == 0) &&
        ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID)) == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                            ADI_COMMON_ERRSRC_API,
                            ADI_COMMON_ERR_INV_PARAM,
                            ADI_COMMON_ACT_ERR_CHECK_PARAM,
                            rxChannelMask,
                            "Unable to load gain table - Rx and ORx profiles are invalid.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range check individual gain table row entries*/
    for (gainIndex = 0; gainIndex < arraySize; gainIndex++)
    {
        /*Check that {TIA GAIN, ADC CTRL} are 0-63 */
        ADI_RANGE_CHECK(device, gainTableRows[gainIndex].adcTiaGain, 0, MAX_ADC_TIA_GAIN);

        /*Check that EXT_CTRL is in the range {0,1,2,3}*/
        ADI_RANGE_CHECK(device, gainTableRows[gainIndex].extControl, 0, MAX_EXT_CTRL_WORD);

        /*Check that digital gain is in the range -18dB to 50dB*/
        ADI_RANGE_CHECK(device, gainTableRows[gainIndex].digGain, MIN_DIG_GAIN, MAX_DIG_GAIN);
    }

    ADI_API_RETURN(device);
}


int32_t adi_adrv9001_Rx_GainTable_Write(adi_adrv9001_Device_t *device,
                                        uint32_t channelMask,
                                        uint8_t  gainIndexOffset,
                                        adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                        uint32_t arraySize)
{
    uint32_t baseIndex = 0;
    uint32_t baseAddress = 0;
    uint16_t numGainIndicesToWrite = 0;

    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[((ADI_ADRV9001_MAX_GAIN_TABLE_INDEX - ADI_ADRV9001_MIN_GAIN_TABLE_INDEX) + 1) * ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX] = { 0 };
    static const uint8_t DEC_POWER_CONFIG1 = 0x09; /* dec_power_log_shift | dec_power_enable_meas */
    static const uint8_t DIGITAL_GAIN_CONFIG2 = 0x01; /* digital_gain_enable */

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainTable_Write_Validate, device, channelMask, gainIndexOffset, gainTableRows, arraySize);

    /*Calculate base index for the config*/
    numGainIndicesToWrite = arraySize;
    baseIndex = (gainIndexOffset - (numGainIndicesToWrite - 1));

    /*Format Gain Table Entries*/
    ADI_EXPECT(adrv9001_RxGainTableFormat, device, gainTableRows, &armDmaData[0], numGainIndicesToWrite);

    /*Resolve the RX Channel SRAM to program*/
    /*If Rx1 Channel Mask Set by user for this config, load Rx1 gain table*/
    if (ADRV9001_BF_EQUAL(channelMask, ADI_CHANNEL_1))
    {
        /* Enable ARM clock to access Rx1/ORx1 gain table memory */
        ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX1_CORE, true);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, true);

        /*Resolve Rx1 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_1 + (baseIndex * ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        ADI_MSG_EXPECT("Error writing to ARM DMA while loading Rx Gain Table @ base address 0x73300000",
                           adrv9001_DmaMemWrite,
                           device,
                           baseAddress,
                           &armDmaData[0],
                           (uint32_t)(numGainIndicesToWrite * ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX),
                           ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

        ADRV9001_SPIWRITEBYTE(device,
                              "RX1_DEC_POWR_CONFIG_1",
                              (ADRV9001_ADDR_CH1_RXB + ADRV9001_ADDR_RXB_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9001_SPIWRITEBYTE(device,
                              "RX1_DIGITAL_GAIN_CONFIG2",
                              (ADRV9001_ADDR_CH1_RX + ADRV9001_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);

        ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX1_CORE, false);
    }

    /*If Rx2 Channel Mask Set by user for this config, load Rx2 gain table*/
    if (ADRV9001_BF_EQUAL(channelMask, ADI_CHANNEL_2))
    {
        /* Enable ARM clock to access Rx2/ORx2 gain table memory */
        ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX2_CORE, true);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, true);

        /*Resolve Rx2 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_2 + (baseIndex * ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        ADI_MSG_EXPECT("Error writing to ARM DMA while loading Rx Gain Table @ base address 0x73400000",
                           adrv9001_DmaMemWrite,
                           device,
                           baseAddress,
                           &armDmaData[0],
                           (uint32_t)(numGainIndicesToWrite * ADI_ADRV9001_NUM_BYTES_PER_RX_GAIN_INDEX),
                           ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

        ADRV9001_SPIWRITEBYTE(device,
                              "RX2_DEC_POWR_CONFIG_1",
                              (ADRV9001_ADDR_CH2_RXB + ADRV9001_ADDR_RXB_DEC_POWER_CONFIG1_OFFSET),
                              DEC_POWER_CONFIG1);

        ADRV9001_SPIWRITEBYTE(device,
                              "RX2_DIGITAL_GAIN_CONFIG2",
                              (ADRV9001_ADDR_CH2_RX + ADRV9001_ADDR_RX_DIGITAL_GAIN_CONFIG2_OFFSET),
                              DIGITAL_GAIN_CONFIG2);

        ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX2_CORE, false);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_GainTable_Read_Validate(adi_adrv9001_Device_t *device,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint8_t  gainIndexOffset,
                                                                    adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                                                    uint32_t arraySize,
                                                                    uint16_t *numGainIndicesRead)
{
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);

    /* numGainIndicesRead is the actual no.of gain indices read from SRAM(output).A NULL can be passed
    * if the value of no.of gain indices actually read is not required.
    */
    // ADI_NULL_PTR_RETURN(&device->common, numGainIndicesRead);

    /*Check that the channel requested is valid*/
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (arraySize > ((ADI_ADRV9001_MAX_GAIN_TABLE_INDEX - ADI_ADRV9001_MIN_GAIN_TABLE_INDEX) + 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid arraySize parameter requested for Rx Channel Gain Table Read. Valid range is 0 to (ADI_ADRV9001_MAX_GAIN_TABLE_INDEX - ADI_ADRV9001_MIN_GAIN_TABLE_INDEX) + 1)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_RANGE_CHECK(device, gainIndexOffset, ADI_ADRV9001_RX_GAIN_INDEX_MIN, ADI_ADRV9001_RX_GAIN_INDEX_MAX);
    
    if (((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_RX_PROFILE_VALID)) == 0) &&
        ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_ORX_PROFILE_VALID)) == 0) &&
        ((ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID)) == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannelMask,
                         "Unable to read gain table - profiles are invalid.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainTable_Read(adi_adrv9001_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                       uint8_t  gainIndexOffset,
                                       adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize,
                                       uint16_t *numGainIndicesRead)
{
    static const uint32_t NUM_BYTES_PER_GAIN_INDEX = 8;

    uint32_t baseAddress = 0;
    uint32_t baseIndex = 0;
    uint16_t maxGainIndices = 0;
    uint16_t numGainIndicesToRead = 0;
    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[ADI_ADRV9001_GAIN_TABLE_ARRAY_SIZE] = { 0 };

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);
    /* numGainIndicesRead is the actual no.of gain indices read from SRAM(output).A NULL can be passed
    * if the value of no.of gain indices actually read is not required.
    */

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainTable_Read_Validate, device,
                           channel, gainIndexOffset, gainTableRows, arraySize, numGainIndicesRead);

    maxGainIndices = (gainIndexOffset - ADI_ADRV9001_RX_GAIN_INDEX_MIN) + 1;
    /*Calculate no. of indices to read and the base address for the config*/
    if (channel == ADI_CHANNEL_1)
    {
        baseAddress = (uint32_t)ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_1;
    }
    else if (channel == ADI_CHANNEL_2)
    {
        baseAddress = (uint32_t)ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_2;

    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid Channel requested for gain table read. Valid channels include Rx1-Rx2");
        ADI_API_RETURN(device);
    }

    if (arraySize >= maxGainIndices)
    {
        numGainIndicesToRead = maxGainIndices;
    }
    else
    {
        numGainIndicesToRead = arraySize;
    }
    baseIndex = (gainIndexOffset - (numGainIndicesToRead - 1));
    baseAddress += baseIndex * NUM_BYTES_PER_GAIN_INDEX;

    /* Enable ARM clock to access Rx1/ORx1 gain table memory */
    ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX1_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, true);

    /* Enable ARM clock to access Rx2/ORx2 gain table memory */
    ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX2_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, true);

    /*Read Gain Table Data for the requested channel via ARM DMA*/
    ADI_MSG_EXPECT("Error Reading Gain Table ARM DMA",
                   adrv9001_DmaMemRead,
                   device,
                   baseAddress,
                   &armDmaData[0],
                       (numGainIndicesToRead * NUM_BYTES_PER_GAIN_INDEX),
                       ADRV9001_ARM_MEM_READ_AUTOINCR);

    /* Disable ARM clock to Rx1/ORx1 and Rx2/ORx2 gain table memory access */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX1_CORE, false);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv9001_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, ADRV9001_BF_RX2_CORE, false);

    /*Parse gain table data obtained in ARM DMA data format to an rx gain table row entry datastructure memory*/
    ADI_MSG_EXPECT("Error parsing gain table data",
                       adrv9001_RxGainTableParse,
                       device,
                       &gainTableRows[0],
                       &armDmaData[0],
                       numGainIndicesToRead);

    /* numGainIndicesRead is the actual no.of gain indices read from SRAM(output).A NULL can be passed
     * if the value of no.of gain indices actually read is not required.
     */
    /*Update no. of gain indices read*/
    if (numGainIndicesRead != NULL)
    {
        *numGainIndicesRead = numGainIndicesToRead;
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_Gain_Set_Validate(adi_adrv9001_Device_t *device,
                                                                 adi_common_ChannelNumber_e channel,
                                                                 uint8_t gainIndex,
                                                                 adi_adrv9001_RxGainControlMode_e *gainCtrlMode)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    uint8_t chan_idx = 0;

    /* Check for valid channel */
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_idx);
    
    /* Check that Rx profile is valid */
    if (0 == ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_idx]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Gain index set requested for an Rx Channel but Rx profile is invalid in the device structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check that gain indices are within range for the channel selected*/
    ADI_RANGE_CHECK(device, gainIndex, ADI_ADRV9001_RX_GAIN_INDEX_MIN, ADI_ADRV9001_RX_GAIN_INDEX_MAX);
    
    /* Save the current gain control mode and set to the required mode */
    ADI_EXPECT(adi_adrv9001_Rx_GainControl_Mode_Get, device, channel, gainCtrlMode);
    if (ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO == *gainCtrlMode)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid Rx gain control mode to set Rx Gain");
    }
    else if (ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN == *gainCtrlMode)
    {
        ADI_EXPECT(adi_adrv9001_Channel_Validate, device, channel);

        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                state,
                "Invalid channel state to set gain index in ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN mode.");
        }
    }
    else /* ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI */
    {
        /* No action taken */
    }

    ADI_API_RETURN(device)
}

int32_t adi_adrv9001_Rx_Gain_Set(adi_adrv9001_Device_t *device,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t gainIndex)
{
    adrv9001_BfNvsRegmapRxb_e baseAddr = ADRV9001_BF_RXB1_CORE;
    adi_adrv9001_RxGainControlMode_e gainCtrlMode = ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI;
    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_Gain_Set_Validate, device, channel, gainIndex, &gainCtrlMode);

    if (ADI_CHANNEL_2 == channel)
    {
        baseAddr = ADRV9001_BF_RXB2_CORE;
    }

    if (ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN == gainCtrlMode)
    {
        ADI_EXPECT(adi_adrv9001_Rx_GainControl_Mode_Set, device, channel, ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI);
    }

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndex_Set, device, baseAddr, gainIndex);

    /* Restore the gain control mode */
    if (ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI != gainCtrlMode)
    {
        ADI_EXPECT(adi_adrv9001_Rx_GainControl_Mode_Set, device, channel, gainCtrlMode);
    }

    ADI_API_RETURN(device)
}

static int32_t __maybe_unused adi_adrv9001_Rx_Gain_Get_Validate(adi_adrv9001_Device_t *device,
                                                                adi_common_ChannelNumber_e channel,
                                                                uint8_t *gainIndex)
{
    ADI_NULL_PTR_RETURN(&device->common, gainIndex);

    /*Check that the requested channel is valid*/
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /*Check that Rx profile is valid in current config*/
    if ((device->devStateInfo.profilesValid & ADI_ADRV9001_RX_PROFILE_VALID) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Rx Gain index read requested but Rx profile is invalid in device structure");
    }

    ADI_API_RETURN(device)
}

int32_t adi_adrv9001_Rx_Gain_Get(adi_adrv9001_Device_t *device, adi_common_ChannelNumber_e channel, uint8_t *gainIndex)
{
    adrv9001_BfNvsRegmapRxb_e baseAddr = ADRV9001_BF_RXB1_CORE;
    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_Gain_Get_Validate, device, channel, gainIndex);

    if (ADI_CHANNEL_2 == channel)
    {
        baseAddr = ADRV9001_BF_RXB2_CORE;
    }

    ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcGainIndex_Get, device, baseAddr, gainIndex);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_Rssi_Read_Validate(adi_adrv9001_Device_t *device,
                                                                 adi_adrv9001_RxRssiStatus_t *rxRssiStatus,
                                                                 adi_common_ChannelNumber_e rxChannel)
{
    ADI_NULL_PTR_RETURN(&device->common, rxRssiStatus);

    /* Check that the requested channel mask is valid */
    ADI_RANGE_CHECK(device, rxChannel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_Rssi_Read(adi_adrv9001_Device_t *device,
                                  adi_common_ChannelNumber_e channel,
                                  adi_adrv9001_RxRssiStatus_t *rxRssiStatus)
{
    uint8_t armExtData[2] = { 0 };
    uint8_t armReadBack[6] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_Rssi_Read_Validate, device, rxRssiStatus, channel);

    armExtData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    armExtData[1] = ADRV9001_ARM_OBJECTID_RSSI;

    /* send ARM GET opcode */
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &armExtData[0], sizeof(armExtData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_GET_OPCODE,
                                        armExtData[1],
                                        ADI_ADRV9001_READ_RSSI_TIMEOUT_US,
                                        ADI_ADRV9001_READ_RSSI_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false)

    rxRssiStatus->linearPower_mantissa = (((uint16_t)armReadBack[0]) |
                                          ((uint16_t)armReadBack[1] << 8));

    rxRssiStatus->linearPower_exponent = (((uint16_t)armReadBack[2]) |
                                          ((uint16_t)armReadBack[3] << 8));

    rxRssiStatus->power_mdB = (((uint16_t)armReadBack[4]) |
                               ((uint16_t)armReadBack[5] << 8));

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_DecimatedPower_Get(adi_adrv9001_Device_t *device,
                                           adi_common_ChannelNumber_e channel,
                                           uint16_t *rxDecPower_mdBFS)
{
    adrv9001_BfNvsRegmapRxb_e instance = ADRV9001_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;
    uint8_t bfValue = 0;

    static const uint8_t RX_DEC_POWER_MULT_mdB = 250; /* 250 = 1000 * 0.25dB */
    static const uint8_t DEC_MAX_POWER = 252;

    ADI_ENTRY_PTR_EXPECT(device, rxDecPower_mdBFS);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = nvsRegmapRxbInstances[instanceIdx];

    /*
     * This register contains the decimated power readback value for the requested Rx channel. Resolution is 250mdB.
     * This register must be written(any value) to generate a strobe that will latch the value to be read back.
     */
    ADRV9001_SPIWRITEBYTE(device, "RX_DECIMATED_PWR", (instance + ADRV9001_ADDR_RX_DECIMATED_PWR_OFFSET), 0x0F);

    /* Read decimated power for the given channel */
    ADI_EXPECT(adrv9001_NvsRegmapRxb_DecPower_Get, device, instance, &bfValue);

    /*
     * Checking the DECIMATED Power range. The valid range is from 0 to -63 dB.
     * With 0.25 dB step, the bfValue can't be greater than 252.
     * (i.e. with bfValue == 100 -> DEC power is (100 / 4) = 25 dB,
     * negative sign is implicit so it is -25 dB. */
    if (bfValue > DEC_MAX_POWER)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_SPI_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         channel,
                         "Decimated Power is out of range.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *rxDecPower_mdBFS = (((uint16_t)bfValue) * RX_DEC_POWER_MULT_mdB);

    ADI_API_RETURN(device);
}

static int32_t adi_adrv9001_Rx_InterfaceGain_Validate(adi_adrv9001_Device_t *device,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_adrv9001_RxGainTableType_e gainTableType,
                                                      adi_adrv9001_RxInterfaceGain_e gain)
{
    uint8_t chan_index = 0;
    static const uint32_t RX_OUTPUT_RATE_kHZ = 1000;

    adi_adrv9001_RxInterfaceGain_e rxInterfaceGainMin = ADI_ADRV9001_RX_INTERFACE_GAIN_18_DB;
    adi_adrv9001_RxInterfaceGain_e rxInterfaceGainMax = ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_36_DB;

    adi_common_channel_to_index(channel, &chan_index);
    
    if (device->devStateInfo.rxOutputRate_kHz[chan_index] < RX_OUTPUT_RATE_kHZ)
    {
        if (gainTableType == ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE)
        {
            rxInterfaceGainMin = ADI_ADRV9001_RX_INTERFACE_GAIN_18_DB;
            rxInterfaceGainMax = ADI_ADRV9001_RX_INTERFACE_GAIN_0_DB;
        }
        else
        {
            rxInterfaceGainMin = ADI_ADRV9001_RX_INTERFACE_GAIN_18_DB;
            rxInterfaceGainMax = ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_36_DB;
        }
    }
    else
    {
        if (gainTableType == ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE)
        {
            rxInterfaceGainMin = ADI_ADRV9001_RX_INTERFACE_GAIN_0_DB;
            rxInterfaceGainMax = ADI_ADRV9001_RX_INTERFACE_GAIN_0_DB;
        }
        else
        {
            rxInterfaceGainMin = ADI_ADRV9001_RX_INTERFACE_GAIN_0_DB;
            rxInterfaceGainMax = ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_36_DB;
        }

    }

    ADI_RANGE_CHECK(device, gain, rxInterfaceGainMin, rxInterfaceGainMax);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_InterfaceGain_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                               adi_common_ChannelNumber_e channel,
                                                                               adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, rxInterfaceGainCtrl);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->updateInstance,
                    ADI_ADRV9001_RX_INTERFACE_GAIN_UPDATE_TIMING_NEXT_FRAME,
                    ADI_ADRV9001_RX_INTERFACE_GAIN_UPDATE_TIMING_NOW);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->controlMode,
                    ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_AUTOMATIC,
                    ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_MANUAL);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->gainTableType,
                    ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE,
                    ADI_ADRV9001_RX_GAIN_COMPENSATION_TABLE);

    /* Perform Range check of allowed gain value */
    ADI_EXPECT(adi_adrv9001_Rx_InterfaceGain_Validate,
               device,
               channel,
               rxInterfaceGainCtrl->gainTableType,
               rxInterfaceGainCtrl->gain);
    
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(device, 
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Error attempting to configure Rx Interface gain. Specified channel must be in CALIBRATED state");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_InterfaceGain_Configure(adi_adrv9001_Device_t *device,
                                                adi_common_ChannelNumber_e channel,
                                                adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_InterfaceGain_Configure_Validate, device, channel, rxInterfaceGainCtrl);

    armData[0] = (uint8_t)(rxInterfaceGainCtrl->updateInstance);
    armData[1] = (uint8_t)(rxInterfaceGainCtrl->controlMode);
    armData[2] = (uint8_t)(rxInterfaceGainCtrl->gainTableType);
    armData[3] = (uint8_t)(rxInterfaceGainCtrl->gain);

    /* Write RX interface gain control parameters to ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
    extData[1] = ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN_CONTROL;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_InterfaceGain_Set_Validate(adi_adrv9001_Device_t *device,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv9001_RxInterfaceGain_e gain)
{
    adi_adrv9001_RxInterfaceGainCtrl_t rxInterfaceGainCtrl = { 0 };
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_EXPECT(adi_adrv9001_Rx_InterfaceGain_Inspect, device, channel, &rxInterfaceGainCtrl);

    /* adi_adrv9001_RxInterfaceGain_Set() is allowed only in Manual mode */
    if (rxInterfaceGainCtrl.controlMode != ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_MANUAL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxInterfaceGainCtrl->gainControlMode,
                         "Invalid gainControlMode - must be in manual mode to set gain");

        ADI_API_RETURN(device);
    }

    /* Perform Range check of allowed gain value */
    ADI_EXPECT(adi_adrv9001_Rx_InterfaceGain_Validate, device, channel, rxInterfaceGainCtrl.gainTableType, gain);
    
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if ((ADI_ADRV9001_CHANNEL_PRIMED != state)
     && (ADI_ADRV9001_CHANNEL_RF_ENABLED != state))
    {
        ADI_ERROR_REPORT(device, 
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Error attempting to configure Rx Interface gain. Specified channel must be in PRIMED or RF_ENABLED state");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_InterfaceGain_Set(adi_adrv9001_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_RxInterfaceGain_e gain)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_InterfaceGain_Set_Validate, device, channel, gain);

    armData[0] = (uint8_t)gain;

    /* Write RX interface gain control parameters to ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain command */
    extData[1] = ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_InterfaceGain_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, device, &currentState);

    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, rxInterfaceGainCtrl);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (ADI_ADRV9001_CHANNEL_STANDBY == currentState.channelStates[port_index][chan_index])
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Error while attempting to inspect Rx Interface gain. Specified channel is in wrong state.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_InterfaceGain_Inspect(adi_adrv9001_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_InterfaceGain_Inspect_Validate, device, channel, rxInterfaceGainCtrl);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
    extData[1] = ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN_CONTROL;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               ADRV9001_ARM_MEM_READ_AUTOINCR)

    rxInterfaceGainCtrl->updateInstance = (adi_adrv9001_RxInterfaceGainUpdateTiming_e)armReadBack[0];
    rxInterfaceGainCtrl->controlMode    = (adi_adrv9001_RxInterfaceGainCtrlMode_e)armReadBack[1];
    rxInterfaceGainCtrl->gainTableType  = (adi_adrv9001_RxGainTableType_e)armReadBack[2];
    rxInterfaceGainCtrl->gain           = (adi_adrv9001_RxInterfaceGain_e)armReadBack[3];

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_InterfaceGain_Get_Validate(adi_adrv9001_Device_t *device,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv9001_RxInterfaceGain_e *gain)
{
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, device, &currentState);

    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);

    /* Check device pointer and gain pointer are not null */
    ADI_ENTRY_PTR_EXPECT(device, gain);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (ADI_ADRV9001_CHANNEL_STANDBY == currentState.channelStates[port_index][chan_index])
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Error while attempting to get Rx Interface gain. Specified channel is in wrong state.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_InterfaceGain_Get(adi_adrv9001_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_RxInterfaceGain_e *gain)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_InterfaceGain_Get_Validate, device, channel, gain);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
    extData[1] = ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_RX_INTERFACE_CONTROL_INTERVAL_US);

    /* read the ARM memory to get Rx interface Gain */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               ADRV9001_ARM_MEM_READ_AUTOINCR)

    *gain = (adi_adrv9001_RxInterfaceGain_e)armReadBack[0];

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adrv9001_Rx_FrequencyCorrection_Set_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           int32_t frequencyOffset_Hz)
{
    uint8_t chan_index = 0;
    int32_t maxFrequency_Hz = 0;
    static const int32_t MAGIC_NUMBER_DIVISOR = 8;
    static const int32_t ABSOLUTE_MAX_FREQUENCY_HZ = 20000;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    maxFrequency_Hz  = KILO_TO_BASE_UNIT(device->devStateInfo.rxOutputRate_kHz[chan_index]) / MAGIC_NUMBER_DIVISOR;
    maxFrequency_Hz = maxFrequency_Hz < ABSOLUTE_MAX_FREQUENCY_HZ ? maxFrequency_Hz : ABSOLUTE_MAX_FREQUENCY_HZ;

    ADI_RANGE_CHECK(device, frequencyOffset_Hz, -maxFrequency_Hz, maxFrequency_Hz);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_FrequencyCorrection_Set(adi_adrv9001_Device_t *device,
                                                adi_common_ChannelNumber_e channel,
                                                int32_t frequencyOffset_Hz,
                                                bool immediate)
{
    uint8_t extData[2] = { 0 };
    uint8_t armData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adrv9001_Rx_FrequencyCorrection_Set_Validate, device, channel, frequencyOffset_Hz);

    armData[0] = (uint8_t)((frequencyOffset_Hz >> 0) & 0xFF);
    armData[1] = (uint8_t)((frequencyOffset_Hz >> 8) & 0xFF);
    armData[2] = (uint8_t)((frequencyOffset_Hz >> 16) & 0xFF);
    armData[3] = (uint8_t)((frequencyOffset_Hz >> 24) & 0xFF);
    armData[4] = (uint8_t)immediate;

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4)

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_RX_FREQCORRECTION;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        ADRV9001_ARM_HIGHPRIORITY_SET_RX_FREQCORRECTION,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_AdcSwitchEnable_Set_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (!(ADI_ADRV9001_CHANNEL_STANDBY == state) &&
        !(ADI_ADRV9001_CHANNEL_CALIBRATED == state))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Specified channel must be in either STANDBY or CALIBRATED state to enable ADC dynamic switch.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_AdcSwitchEnable_Set(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            bool enable)
{
    uint8_t armData[5] = { 0 };
    uint8_t extData[3] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_AdcSwitchEnable_Set_Validate, device, channel);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = (uint8_t)enable;

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_CONFIG;
    extData[2] = ADRV9001_ARM_OBJECTID_ADC_SWITCHING;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, device, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(device);

}

static int32_t __maybe_unused adi_adrv9001_Rx_AdcSwitchEnable_Get_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           bool *enable)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, enable);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_AdcSwitchEnable_Get(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            bool *enable)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_AdcSwitchEnable_Get_Validate, device, channel, enable);

    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, device, ADRV9001_ARM_OBJECTID_ADC_SWITCHING, channelMask, offset, armReadBack, sizeof(armReadBack))

    *enable = (bool)armReadBack[0];

    ADI_API_RETURN(device);

}

static int32_t __maybe_unused adi_adrv9001_Rx_AdcSwitch_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           adi_adrv9001_AdcSwitchCfg_t *switchConfig)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Specified channel must be in CALIBRATED state to configure ADC dynamic switch settings.");
        ADI_API_RETURN(device)
    }

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, switchConfig);
    ADI_RANGE_CHECK(device, switchConfig->adcSwitchMode, ADI_ADRV9001_ADC_SWITCH_IMMEDIATE, ADI_ADRV9001_ADC_SWITCH_PINCONTROL_DISABLED);
    ADI_RANGE_CHECK(device, switchConfig->adcSwitchGpio.pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);

    if ((ADI_ADRV9001_ADC_SWITCH_PINCONTROL_ENABLED == switchConfig->adcSwitchMode) &&
        (ADI_ADRV9001_GPIO_UNASSIGNED == switchConfig->adcSwitchGpio.pin))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            NULL,
            "If adcSwitchMode is PINCONTROL_ENABLED then adcSwitchGpio pin must not be UNASSIGNED.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_AdcSwitch_Configure(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_AdcSwitchCfg_t *switchConfig)
{
    uint8_t armData[1] = { 0 };
    uint8_t extData[3] = { 0 };
    adi_adrv9001_GpioSignal_e gpioSignal = ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL1;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_AdcSwitch_Configure_Validate, device, channel, switchConfig);

    armData[0] = (uint8_t)switchConfig->adcSwitchMode;

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write,
        device,
        (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET,
        &armData[0],
        sizeof(armData),
        ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_CHANNEL_PERFORM_ADC_SWITCH;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
        ADRV9001_ARM_SET_OPCODE,
        extData[1],
        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
        ADI_ADRV9001_DEFAULT_INTERVAL_US);

    if (ADI_CHANNEL_2 == channel)
    {
        gpioSignal = ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL2;
    }

    if ((switchConfig->adcSwitchMode != ADI_ADRV9001_ADC_SWITCH_IMMEDIATE) &&
        (ADI_ADRV9001_GPIO_UNASSIGNED != switchConfig->adcSwitchGpio.pin))
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, device, gpioSignal, &(switchConfig->adcSwitchGpio));
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_AdcSwitch_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv9001_AdcSwitchCfg_t *switchConfig)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, switchConfig);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_AdcSwitch_Inspect(adi_adrv9001_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_AdcSwitchCfg_t *switchConfig)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t extData[5] = { 0 };
    adi_adrv9001_GpioSignal_e gpioSignal = ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL1;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_AdcSwitch_Inspect_Validate, device, channel, switchConfig);

    /* Invoke the GET command */
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_CHANNEL_PERFORM_ADC_SWITCH;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
        ADRV9001_ARM_GET_OPCODE,
        extData[1],
        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
        ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* Read and parse the data */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, ADRV9001_ADDR_ARM_MAILBOX_GET, armReadBack, sizeof(armReadBack), false);
    switchConfig->adcSwitchMode = (adi_adrv9001_AdcTypeSwitchMode_e)armReadBack[0];

    if (ADI_CHANNEL_2 == channel)
    {
        gpioSignal = ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL2;
    }

    if (switchConfig->adcSwitchMode != ADI_ADRV9001_ADC_SWITCH_IMMEDIATE)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, gpioSignal, &(switchConfig->adcSwitchGpio));
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Rx_AdcType_Get_Validate(adi_adrv9001_Device_t *device,
                                                                   adi_common_ChannelNumber_e channel,
                                                                   adi_adrv9001_AdcType_e *adcType)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, adcType);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_AdcType_Get(adi_adrv9001_Device_t *device,
                                    adi_common_ChannelNumber_e channel,
                                    adi_adrv9001_AdcType_e *adcType)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_AdcType_Get_Validate, device, channel, adcType);

    /* Invoke the GET command */
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_GET_CURRENT_ADC_TYPE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
        ADRV9001_ARM_GET_OPCODE,
        extData[1],
        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
        ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* Read and parse the data */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, ADRV9001_ADDR_ARM_MAILBOX_GET, armReadBack, sizeof(armReadBack), false);
    *adcType = (adi_adrv9001_AdcType_e)armReadBack[0];

    ADI_API_RETURN(device);

}

static int32_t __maybe_unused adi_adrv9001_Rx_GainIndex_Gpio_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                adi_adrv9001_GainIndexPinCfg_t *gainIndexPinCfgchannel)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, gainIndexPinCfgchannel);
    ADI_RANGE_CHECK(device, gainIndexPinCfgchannel->gainIndex_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device, gainIndexPinCfgchannel->gainIndex_03_02, ADI_ADRV9001_GPIO_PIN_CRUMB_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device, gainIndexPinCfgchannel->gainIndex_05_04, ADI_ADRV9001_GPIO_PIN_CRUMB_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device, gainIndexPinCfgchannel->gainIndex_07_06, ADI_ADRV9001_GPIO_PIN_CRUMB_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            currentState.channelStates[port_index][chan_index],
            "Specified channel must be in CALIBRATED state to configure GPIO for gain index.");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Rx_GainIndex_Gpio_Configure(adi_adrv9001_Device_t *device,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_GainIndexPinCfg_t *gainIndexPinCfgchannel)
{
    adrv9001_BfNvsRegmapRx_e instance = ADRV9001_BF_RX1_CORE;
    uint8_t instanceIdx = 0;

    /* GPIO variables */
    uint8_t gpioCrumb1_0 = 0;
    uint8_t gpioCrumb3_2 = 0;
    uint8_t gpioCrumb5_4 = 0;
    uint8_t gpioCrumb7_6 = 0;

    uint8_t gpioSource1_0 = 0;
    uint8_t gpioSource3_2 = 0;
    uint8_t gpioSource5_4 = 0;
    uint8_t gpioSource7_6 = 0;

    uint16_t gpioOutEn = 0;

    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_1_0 = 0x10;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_3_2 = 0x11;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_5_4 = 0x12;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX1_7_6 = 0x13;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_1_0 = 0x14;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_3_2 = 0x15;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_5_4 = 0x16;
    static const uint8_t ADI_ADRV9001_GPIO_SOURCE_RX2_7_6 = 0x17;

    static const int8_t GAIN_INDEX_MUX_ADDRESS = 0x26;
    static const uint16_t GPIO_SOURCE_SEL_ADDR = 0x56;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Rx_GainIndex_Gpio_Configure_Validate, device, channel, gainIndexPinCfgchannel);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = nvsRegmapRxInstances[instanceIdx];

    gpioCrumb1_0 = (uint8_t)gainIndexPinCfgchannel->gainIndex_01_00;
    gpioCrumb3_2 = (uint8_t)gainIndexPinCfgchannel->gainIndex_03_02;
    gpioCrumb5_4 = (uint8_t)gainIndexPinCfgchannel->gainIndex_05_04;
    gpioCrumb7_6 = (uint8_t)gainIndexPinCfgchannel->gainIndex_07_06;

    if (ADI_CHANNEL_1 == channel)
    {
        gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX1_1_0;
        gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX1_3_2;
        gpioSource5_4 = ADI_ADRV9001_GPIO_SOURCE_RX1_5_4;
        gpioSource7_6 = ADI_ADRV9001_GPIO_SOURCE_RX1_7_6;
    }
    else
    {
        gpioSource1_0 = ADI_ADRV9001_GPIO_SOURCE_RX2_1_0;
        gpioSource3_2 = ADI_ADRV9001_GPIO_SOURCE_RX2_3_2;
        gpioSource5_4 = ADI_ADRV9001_GPIO_SOURCE_RX2_5_4;
        gpioSource7_6 = ADI_ADRV9001_GPIO_SOURCE_RX2_7_6;
    }

    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn |= (1 << (gpioCrumb1_0 * 2 - 1)) | (1 << (gpioCrumb1_0 * 2 - 2));
    gpioOutEn |= (1 << (gpioCrumb3_2 * 2 - 1)) | (1 << (gpioCrumb3_2 * 2 - 2));
    gpioOutEn |= (1 << (gpioCrumb5_4 * 2 - 1)) | (1 << (gpioCrumb5_4 * 2 - 2));
    gpioOutEn |= (1 << (gpioCrumb7_6 * 2 - 1)) | (1 << (gpioCrumb7_6 * 2 - 2));
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);

    /* Configure source */
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb1_0 - 1), gpioSource1_0);
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb3_2 - 1), gpioSource3_2);
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb5_4 - 1), gpioSource5_4);
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb7_6 - 1), gpioSource7_6);

    ADI_EXPECT(adrv9001_NvsRegmapRx_ControlOutMuxSel_Set, device, instance, GAIN_INDEX_MUX_ADDRESS);

    ADI_API_RETURN(device);
}