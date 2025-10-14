/**
* \file
* \brief Receiver (Rx) functions
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_rx.h"
#include "adrv910x_rx_gain_table.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_common.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_rx.h"
#include "adi_adrv910x_user.h"
#include "object_ids.h"

#ifdef __ICCARM__
#pragma diag_suppress=Pe186
#endif

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainTable_Write_Validate(adi_adrv910x_Device_t *device,
                                       adi_common_Port_e port,
                                       adi_common_ChannelNumber_e channel,
                                       uint8_t  gainIndexOffset,
                                       adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize,
                                       adi_adrv910x_RxGainTableType_e gainTableType)
{
    static const uint8_t MAX_EXT_CTRL_WORD = 3;
    static const uint8_t MAX_ADC_TIA_GAIN  = 63;
    static const int16_t MIN_DIG_GAIN = -360; /*Dig gain is in the range -18dB to 50db*/
    static const int16_t MAX_DIG_GAIN = 1000; /*Dig gain is in the range -18dB to 50db*/
    static const uint8_t ABSOLUTE_MIN_INDEX = 187;

    uint16_t  gainIndex = 0;
    uint16_t totalGainSteps = 0;
    uint8_t i = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);

    /*Check that the gain index offset is within range*/
    ADI_RANGE_CHECK(device, gainIndexOffset, ADI_ADRV910X_MIN_RX_GAIN_TABLE_INDEX, ADI_ADRV910X_START_RX_GAIN_INDEX);

    if (!((ADI_RX == port) || (ADI_ORX == port)))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            port,
            "adi_adrv910x_Rx_GainTable_Write() is valid for only Rx and ORx ports");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Check the no. of gain indices parameter is valid*/
    if (arraySize > ((gainIndexOffset - ADI_ADRV910X_MIN_RX_GAIN_TABLE_INDEX) + 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "gainTableRow arraySize exceeds the limit. Valid range 0 to (gainIndexOffset - ADI_ADRV910X_MIN_RX_GAIN_TABLE_INDEX +1)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_RANGE_CHECK(device, gainTableType, ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE, ADI_ADRV910X_RX_GAIN_COMPENSATION_TABLE);

    /*Check that Rx profile or ORx profile is valid*/
    if (((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_RX_PROFILE_VALID)) == 0) &&
        ((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_ORX_PROFILE_VALID)) == 0) &&
        ((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_TX_PROFILE_VALID)) == 0))
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

int32_t adi_adrv910x_Rx_GainTable_Write(adi_adrv910x_Device_t *device,
                                        adi_common_Port_e port,
                                        adi_common_ChannelNumber_e channel,
                                        uint8_t  gainIndexOffset,
                                        adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                        uint32_t arraySize,
                                        adi_adrv910x_RxGainTableType_e gainTableType)
{
    uint32_t baseIndex = 0;
    uint32_t baseAddress = 0;
    uint16_t numGainIndicesToWrite = 0;
    adi_adrv910x_RxGainTableRow_t *gainTablePtr = NULL;
    uint8_t minGainIndex = 0;

    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[((ADI_ADRV910X_MAX_GAIN_TABLE_INDEX - ADI_ADRV910X_MIN_GAIN_TABLE_INDEX) + 1) * ADI_ADRV910X_NUM_BYTES_PER_RX_GAIN_INDEX] = { 0 };
    static const uint8_t DEC_POWER_CONFIG1 = 0x09; /* dec_power_log_shift | dec_power_enable_meas */
    static const uint8_t DIGITAL_GAIN_CONFIG2 = 0x01; /* digital_gain_enable */
    static const uint8_t ABSOLUTE_MIN_INDEX = 187;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainTable_Write_Validate, device, port, channel, gainIndexOffset, gainTableRows, arraySize, gainTableType);

    /*Calculate base index for the config*/
    numGainIndicesToWrite = arraySize;
    gainTablePtr = gainTableRows;
    baseIndex = (gainIndexOffset - (numGainIndicesToWrite - 1));
    minGainIndex = (uint8_t)baseIndex;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adrv910x_RxGainTableFormat, device, gainTablePtr, &armDmaData[0], numGainIndicesToWrite);

    /*Resolve the RX Channel SRAM to program*/
    /*If Rx1 Channel Mask Set by user for this config, load Rx1 gain table*/
    if (ADI_CHANNEL_1 == channel)
    {
		/* Enable ARM clock to access Rx1/ORx1 gain table memory */
		ADI_EXPECT(adrv910x_NvsRegmapRx_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, true);
        ADI_EXPECT(adrv910x_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, true);
        ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, true);

        /*Resolve Rx1 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_1 + (baseIndex * ADI_ADRV910X_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        ADI_MSG_EXPECT("Error writing to ARM DMA while loading Rx Gain Table @ base address 0x73300000",
            adrv910x_DmaMemWrite,
            device,
            baseAddress,
            &armDmaData[0],
            (uint32_t)(numGainIndicesToWrite * ADI_ADRV910X_NUM_BYTES_PER_RX_GAIN_INDEX),
		    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N, ADI_PS1);

        ADRV910X_SPIWRITEBYTE(device,
		    "RX1_DEC_POWR_CONFIG_1",
		    ADRV910X_ADDR_RXB1_DEC_POWER_CONFIG1,
            DEC_POWER_CONFIG1);

        ADRV910X_SPIWRITEBYTE(device,
		    "RX1_DIGITAL_GAIN_CONFIG2",
            ADRV910X_ADDR_RX1_DIGITAL_GAIN_CONFIG2,
            DIGITAL_GAIN_CONFIG2);

		ADI_EXPECT(adrv910x_NvsRegmapRx_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, false);
        ADI_EXPECT(adrv910x_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, false);


	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Set, device, ADRV910X_BF_RXB1_CORE, minGainIndex);


    }
    else   /*If Rx2 Channel Mask Set by user for this config, load Rx2 gain table*/
    {
		/* Enable ARM clock to access Rx2/ORx2 gain table memory */
		ADI_EXPECT(adrv910x_NvsRegmapRxnb_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, true);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_ArmOrGroup1184ClkSel_Set, device, true);
        ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, true);

        /*Resolve Rx2 Gain Table SRAM load start address*/
        baseAddress = (uint32_t)ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_2 + (baseIndex * ADI_ADRV910X_NUM_BYTES_PER_RX_GAIN_INDEX);
        /*Write to the SRAM via ARM DMA*/
        ADI_MSG_EXPECT("Error writing to ARM DMA while loading Rx Gain Table @ base address 0x73400000",
            adrv910x_DmaMemWrite,
            device,
            baseAddress,
            &armDmaData[0],
            (uint32_t)(numGainIndicesToWrite * ADI_ADRV910X_NUM_BYTES_PER_RX_GAIN_INDEX),
		    ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N, ADI_PS1);

        ADRV910X_SPIWRITEBYTE(device,
            "RX2_DEC_POWR_CONFIG_1",
		    ADRV910X_ADDR_RXBNB_DEC_POWER_CONFIG1,
            DEC_POWER_CONFIG1);

        ADRV910X_SPIWRITEBYTE(device,
            "RX2_DIGITAL_GAIN_CONFIG2",
            ADRV910X_ADDR_RXNB_DIGITAL_GAIN_CONFIG2,
            DIGITAL_GAIN_CONFIG2);

		ADI_EXPECT(adrv910x_NvsRegmapRxnb_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, false);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_ArmOrGroup1184ClkSel_Set, device, false);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Set, device, ADRV910X_BF_RXB_NB_CORE, minGainIndex);

    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainTable_Read_Validate(adi_adrv910x_Device_t *device,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint8_t  gainIndexOffset,
                                                                    adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                                                    uint32_t arraySize,
                                                                    uint16_t *numGainIndicesRead)
{
    uint8_t minGainIndex = 0;

	adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;

    uint8_t instanceIdx = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);

    /*Check that the channel requested is valid*/
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (arraySize > ((ADI_ADRV910X_MAX_GAIN_TABLE_INDEX - ADI_ADRV910X_MIN_GAIN_TABLE_INDEX) + 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid arraySize parameter requested for Rx Channel Gain Table Read. Valid range is 0 to (ADI_ADRV910X_MAX_GAIN_TABLE_INDEX - ADI_ADRV910X_MIN_GAIN_TABLE_INDEX) + 1)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get, device, instance, &minGainIndex);

    ADI_RANGE_CHECK(device, gainIndexOffset, minGainIndex, ADI_ADRV910X_RX_GAIN_INDEX_MAX);

    if (((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_RX_PROFILE_VALID)) == 0) &&
        ((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_ORX_PROFILE_VALID)) == 0) &&
        ((ADRV910X_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV910X_TX_PROFILE_VALID)) == 0))
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

int32_t adi_adrv910x_Rx_GainTable_Read(adi_adrv910x_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                       uint8_t  gainIndexOffset,
                                       adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize,
                                       uint16_t *numGainIndicesRead)
{
    static const uint32_t NUM_BYTES_PER_GAIN_INDEX = 8;

    uint32_t baseAddress = 0;
    uint32_t baseIndex = 0;
    uint16_t maxGainIndices = 0;
    uint16_t numGainIndicesToRead = 0;
    uint8_t minGainIndex = 0;

	adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    /*Maximum Array Size = Max Gain Table Size x Bytes Per Gain Table Entry*/
    static uint8_t armDmaData[ADI_ADRV910X_GAIN_TABLE_ARRAY_SIZE] = { 0 };

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, gainTableRows, arraySize);
    /* numGainIndicesRead is the actual no.of gain indices read from SRAM(output).A NULL can be passed
    * if the value of no.of gain indices actually read is not required.
    */

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainTable_Read_Validate, device,
                           channel, gainIndexOffset, gainTableRows, arraySize, numGainIndicesRead);

    ADI_MUTEX_AQUIRE(device);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get, device, instance, &minGainIndex);

    maxGainIndices = (gainIndexOffset - minGainIndex) + 1;
    /*Calculate no. of indices to read and the base address for the config*/
    if (channel == ADI_CHANNEL_1)
    {
        baseAddress = (uint32_t)ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_1;
    }
    else if (channel == ADI_CHANNEL_2)
    {
        baseAddress = (uint32_t)ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_2;

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
    ADI_EXPECT(adrv910x_NvsRegmapRx_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, true);

    /* Enable ARM clock to access Rx2/ORx2 gain table memory */
    ADI_EXPECT(adrv910x_NvsRegmapRxnb_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapRxnb_ArmOrGroup1184ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, true);

    /*Read Gain Table Data for the requested channel via ARM DMA*/
    ADI_MSG_EXPECT("Error Reading Gain Table ARM DMA",
                   adrv910x_DmaMemRead,
                   device,
                   baseAddress,
                   &armDmaData[0],
                       (numGainIndicesToRead * NUM_BYTES_PER_GAIN_INDEX),
                       ADRV910X_ARM_MEM_READ_AUTOINCR,
                       ADI_PS1);

    /* Disable ARM clock to Rx1/ORx1 and Rx2/ORx2 gain table memory access */
    ADI_EXPECT(adrv910x_NvsRegmapRx_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, false);
    ADI_EXPECT(adrv910x_NvsRegmapRxnb_ForceArmClkOnArmOrGroup1184ClkSel_Set, device, false);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx1GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv910x_NvsRegmapRx_ArmOrGroup1184ClkSel_Set, device, false);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx2GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv910x_NvsRegmapRxnb_ArmOrGroup1184ClkSel_Set, device, false);

    /*Parse gain table data obtained in ARM DMA data format to an rx gain table row entry datastructure memory*/
    ADI_MSG_EXPECT("Error parsing gain table data",
                       adrv910x_RxGainTableParse,
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

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainControl_Mode_Set_Validate(adi_adrv910x_Device_t *device,
                                                                                            adi_common_ChannelNumber_e channel,
                                                                                            adi_adrv910x_RxGainControlMode_e gainCtrlMode)
{
    ADI_ENTRY_EXPECT(device);
    ADI_RANGE_CHECK(device, gainCtrlMode, ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI, ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_GainControl_Mode_Set(adi_adrv910x_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_RxGainControlMode_e gainCtrlMode)
{

	static const adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;

    uint8_t instanceIdx = 0;
    uint8_t bfVal = 0;
    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_Mode_Set_Validate, device, channel, gainCtrlMode);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI || gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_PIN)
    {
        bfVal = 0;
    }
    else
    {
        bfVal = 2;
    }

    ADI_MUTEX_AQUIRE(device);


	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcSetup_Set, device, instance, bfVal);

    if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, instance, true);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Set, device, instance, false);
        ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, instance, false);

    }
    else if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_PIN)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, instance, true);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Set, device, instance, true);
        ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, instance, false);
    }
    else if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, instance, false);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Set, device, instance, false);
        ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, instance, true);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainControl_Mode_Get_Validate(adi_adrv910x_Device_t *device,
                                                                                            adi_common_ChannelNumber_e channel,
                                                                                            adi_adrv910x_RxGainControlMode_e *gainCtrlMode)
{
    ADI_ENTRY_PTR_EXPECT(device, gainCtrlMode);

    /* Check for valid channel */
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_GainControl_Mode_Get(adi_adrv910x_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_RxGainControlMode_e *gainCtrlMode)
{

	static const adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
    adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;
    uint8_t agcSetup = 0;
    uint8_t pinControl = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_Mode_Get_Validate, device, channel, gainCtrlMode);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_MUTEX_AQUIRE(device);


	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcSetup_Get, device, instance, &agcSetup);
    if (ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO == agcSetup)
    {
        *gainCtrlMode = ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO;
    }
    else
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Get, device, instance, &pinControl);
        if (true == pinControl)
        {
            *gainCtrlMode = ADI_ADRV910X_RX_GAIN_CONTROL_MODE_PIN;
        }
        else
        {
            *gainCtrlMode = ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI;
        }
    }
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Set_Validate(adi_adrv910x_Device_t *device,
                                                                        adi_common_ChannelNumber_e channel,
                                                                        uint8_t minGainIndex,
                                                                        uint8_t maxGainIndex)
{
    uint8_t minIndex = 0;

	static const adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get, device, instance, &minIndex);

    ADI_RANGE_CHECK(device, minGainIndex, minIndex, (maxGainIndex - 1));
    ADI_RANGE_CHECK(device, maxGainIndex, (minGainIndex + 1), ADI_ADRV910X_RX_GAIN_INDEX_MAX);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Set(adi_adrv910x_Device_t *device,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t minGainIndex,
                                                        uint8_t maxGainIndex)
{

	static const adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Set_Validate, device, channel, minGainIndex, maxGainIndex);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_MUTEX_AQUIRE(device);


	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Set, device, instance, minGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Set, device, instance, maxGainIndex);
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Get_Validate(adi_adrv910x_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               uint8_t *minGainIndex,
                                               uint8_t *maxGainIndex)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, minGainIndex);
    ADI_NULL_PTR_RETURN(&device->common, maxGainIndex);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Get(adi_adrv910x_Device_t *device,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t *minGainIndex,
                                                        uint8_t *maxGainIndex)
{

	static const adrv910x_BfNvsRegmapRxbCommon_e instances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e instance = ADRV910X_BF_RXB1_CORE;
    uint8_t instanceIdx = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Get_Validate, device, channel, minGainIndex, maxGainIndex);

    adi_common_channel_to_index(channel, &instanceIdx);
    instance = instances[instanceIdx];

    ADI_MUTEX_AQUIRE(device);

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Get, device, instance, maxGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get, device, instance, minGainIndex);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainControl_Configure_Validate(adi_adrv910x_Device_t *device,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv910x_GainControlCfg_t *agcCfg)
{
    //TODO: verify max carries over into Nevis (0 and max are comparator excluded in range check)
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

    ADI_EXPECT(adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Set_Validate, device, channel, agcCfg->minGainIndex, agcCfg->maxGainIndex);
    ADI_RANGE_CHECK(device, agcCfg->peakWaitTime,           0, PEAK_WAIT_TIME_MAX);
    ADI_RANGE_CHECK(device, agcCfg->gainUpdateCounter,      0, GAIN_UPDATE_COUNTER_MAX);
    ADI_RANGE_CHECK(device, agcCfg->attackDelay_us,         0, ATTACK_DELAY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->slowLoopSettlingDelay,  0, SLOW_LOOP_SETTLING_DELAY_MAX);
    ADI_RANGE_CHECK(device, agcCfg->changeGainIfThreshHigh, 0, CHANGE_GAIN_IF_THRESH_HIGH_MAX);
    ADI_RANGE_CHECK(device, agcCfg->agcMode, ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK_AND_POWER, ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK);
    ADI_RANGE_CHECK(device, agcCfg->resetOnRxonGainIndex, agcCfg->minGainIndex, agcCfg->maxGainIndex);

    /* Mitigation Mode Enable Check */
    if (device->devStateInfo.rxNbAgcHighThresholdModeEnable == true &&
        device->devStateInfo.agcHighThresholdSettings.ApdPeakOverloadThresh == 0 && device->devStateInfo.agcHighThresholdSettings.ApdPeakUnderloadThresh  == 0 &&
        device->devStateInfo.agcHighThresholdSettings.Hb2PeakOverloadThresh == 0 && device->devStateInfo.agcHighThresholdSettings.Hb2PeakUnderloadThresh == 0 &&
        device->devStateInfo.agcHighThresholdSettings.Hb2SecondaryUpperThresh == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_INV_PARAM,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        arraySize,
                        "Invalid agcHighThresholdSettings requested when rxNbAgcHighThresholdModeEnable is enabled.");
        ADI_ERROR_RETURN(device->common.error.newAction);

    }
    if (device->devStateInfo.rxNbAgcHighThresholdModeEnable == false &&
        device->devStateInfo.agcHighThresholdSettings.ApdPeakOverloadThresh != 0 && device->devStateInfo.agcHighThresholdSettings.ApdPeakUnderloadThresh  != 0 &&
        device->devStateInfo.agcHighThresholdSettings.Hb2PeakOverloadThresh != 0 && device->devStateInfo.agcHighThresholdSettings.Hb2PeakUnderloadThresh != 0 &&
        device->devStateInfo.agcHighThresholdSettings.Hb2SecondaryUpperThresh != 0)
    {
        ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_INV_PARAM,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        arraySize,
                        "Invalid agcHighThresholdSettings requested when rxNbAgcHighThresholdModeEnable is disabled. agcHighThresholdSettings must be set to 0.");
        ADI_ERROR_RETURN(device->common.error.newAction);

    }


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
                    ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
        agcCfg->peak.feedback_apd_high_hb_high,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
                    agcCfg->power.feedback_inner_high_inner_low,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_15_14);
    ADI_RANGE_CHECK(device,
                    agcCfg->power.feedback_apd_high_apd_low,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
                    ADI_ADRV910X_GPIO_PIN_CRUMB_15_14);

    ADI_API_RETURN(device);
}
int32_t adi_adrv910x_Rx_GainControl_Configure(adi_adrv910x_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv910x_GainControlCfg_t *agcCfg)
{
    static const uint8_t HB_THRESH_CONFIG = 0x3;
    static const uint8_t POWER_INPUT_SELECT = 0x0;
    static const uint8_t POWER_LOG_SHIFT = 0x1;

	adrv910x_BfNvsRegmapRxbCommon_e nvsRegmapRxbCommonInstances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };
	adrv910x_BfNvsRegmapRxbCommon_e rxbCommonAddr = ADRV910X_BF_RXB1_CORE;
	adrv910x_BfNevisAnalogRxMemMap_e anaAddr = ADRV910X_BF_RX1_ANA;

    uint8_t instanceIdx = 0;
    uint8_t bfValue = 0;
    uint8_t regId = 0;
    uint32_t threshCalc = 0;

    /* apdHighThresh and apdLowThresh have to be written to 7 registers */
    static uint8_t REGISTER_ID = 7;

    uint8_t controlMuxAddress = 0;
    adi_adrv910x_RxGainControlMode_e gainCtrlMode = ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI;

    static const uint8_t ADI_ADRV910X_GAIN_PEAK_ADDRESS = 0x28;
    static const uint8_t ADI_ADRV910X_GAIN_PEAK_POWER_ADDRESS = 0x30;

    /* GPIO variables */
    uint8_t gpioCrumb1_0 = 0;
    uint8_t gpioCrumb3_2 = 0;
    uint8_t gpioSource1_0 = 0;
    uint8_t gpioSource3_2 = 0;
    uint32_t gpioOutEn = 0;
    static const uint8_t GPIO_CONTROL_SYSTEM_SEL = 0x0; /* Accessing SYS1 Registers */
    static const uint16_t GPIO_SOURCE_SEL_ADDR = 0x24a; /* SYS1 core gpio source sel base addr */
    static const uint8_t ADI_ADRV910X_GPIO_SOURCE_RX1_5_4 = 0x02; /* peak or peak&power */
    static const uint8_t ADI_ADRV910X_GPIO_SOURCE_RX1_7_6 = 0x03; /* peak or peak&power */
    static const uint8_t ADI_ADRV910X_GPIO_SOURCE_RX2_5_4 = 0x06; /* peak or peak&power */
    static const uint8_t ADI_ADRV910X_GPIO_SOURCE_RX2_7_6 = 0x07; /* peak or peak&power */

#ifdef __KERNEL__
    /* APD Low Frequency MITIGATION Mode Setup */
    static const uint8_t APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER = 3;
    uint8_t APD_LOW_FREQ_ERROR_MITIGATION_MODE = (uint8_t) device->devStateInfo.rxNbAgcHighThresholdModeEnable;

    static const uint32_t APD_LOW_FREQ_THRESH_SUBTRACTION_FACTOR = 33352;
    static const uint32_t APD_LOW_FREQ_THRESH_DIVISION_FACTOR = 133352;
    static const uint32_t APD_LOW_FREQ_THRESH_MULTIPLICATION_FACTOR = 100000;
#endif

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_Configure_Validate, device, channel, agcCfg);

    adi_common_channel_to_index(channel, &instanceIdx);
	rxbCommonAddr = nvsRegmapRxbCommonInstances[instanceIdx];
	anaAddr = nevisAnalogRxMemMapInstances[instanceIdx];

    ADI_MUTEX_AQUIRE(device);

#ifndef __KERNEL__
    /* APD Low Frequency MITIGATION Mode Setup */
    static const uint8_t APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER = 3;
    uint8_t APD_LOW_FREQ_ERROR_MITIGATION_MODE = (uint8_t)(device->devStateInfo.rxNbAgcHighThresholdModeEnable);

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

    if (APD_LOW_FREQ_ERROR_MITIGATION_MODE == 1) {
        agcCfg->peak.apdHighThresh = device->devStateInfo.agcHighThresholdSettings.ApdPeakOverloadThresh;
        agcCfg->peak.apdLowThresh = device->devStateInfo.agcHighThresholdSettings.ApdPeakUnderloadThresh;
        agcCfg->peak.hbHighThresh = device->devStateInfo.agcHighThresholdSettings.Hb2PeakOverloadThresh;
	    agcCfg->peak.hbUnderRangeHighThresh = device->devStateInfo.agcHighThresholdSettings.Hb2PeakUnderloadThresh;
        threshCalc = device->devStateInfo.agcHighThresholdSettings.Hb2SecondaryUpperThresh;
    }

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Set, device, rxbCommonAddr, agcCfg->maxGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcPeakWaitTime_Set, device, rxbCommonAddr, agcCfg->peakWaitTime);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Set, device, rxbCommonAddr, agcCfg->minGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcAttackDelay_Set, device, rxbCommonAddr, agcCfg->attackDelay_us);

	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcGainUpdateCounter_Set, device, agcCfg->gainUpdateCounter);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableFastRecoveryLoop_Set, device, agcCfg->enableFastRecoveryLoop);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLowThsPreventGainInc_Set, device, agcCfg->lowThreshPreventGainInc);

		bfValue = agcCfg->changeGainIfThreshHigh & 0x01;
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcChangeGainIfUlbthHigh_Set, device, bfValue);
		bfValue = (agcCfg->changeGainIfThreshHigh >> 1) & 0x01;
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcChangeGainIfAdcovrgHigh_Set, device, bfValue);

		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcPeakThresholdGainControlMode_Set, device, agcCfg->agcMode);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcResetOnRxon_Set, device, agcCfg->resetOnRxon);

		ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Set, device, rxbCommonAddr, agcCfg->resetOnRxonGainIndex);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableSyncPulseForGainCounter_Set, device, agcCfg->enableSyncPulseForGainCounter);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcSlowLoopSettlingDelay_Set, device, agcCfg->slowLoopSettlingDelay);

		/* Agc Peak */
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval0_Set, device, agcCfg->peak.agcUnderRangeLowInterval);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval1Mult_Set, device, agcCfg->peak.agcUnderRangeMidInterval);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval2Mult_Set, device, agcCfg->peak.agcUnderRangeHighInterval);
	}
	else /* channel 2 / NB */
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcGainUpdateCounter_Set, device, agcCfg->gainUpdateCounter);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableFastRecoveryLoop_Set, device, agcCfg->enableFastRecoveryLoop);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLowThsPreventGainInc_Set, device, agcCfg->lowThreshPreventGainInc);

		bfValue = agcCfg->changeGainIfThreshHigh & 0x01;
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcChangeGainIfUlbthHigh_Set, device, bfValue);
		bfValue = (agcCfg->changeGainIfThreshHigh >> 1) & 0x01;
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcChangeGainIfAdcovrgHigh_Set, device, bfValue);

		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcPeakThresholdGainControlMode_Set, device, agcCfg->agcMode);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcResetOnRxon_Set, device, agcCfg->resetOnRxon);

		ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Set, device, rxbCommonAddr, agcCfg->resetOnRxonGainIndex);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableSyncPulseForGainCounter_Set, device, agcCfg->enableSyncPulseForGainCounter);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcSlowLoopSettlingDelay_Set, device, agcCfg->slowLoopSettlingDelay);

		/* Agc Peak */
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval0_Set, device, agcCfg->peak.agcUnderRangeLowInterval);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval1Mult_Set, device, agcCfg->peak.agcUnderRangeMidInterval);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval2Mult_Set, device, agcCfg->peak.agcUnderRangeHighInterval);
	}


    for (regId = 0; regId < REGISTER_ID; regId++)
    {
        ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetUlbth_Set, device, anaAddr, regId, agcCfg->peak.apdHighThresh);
        ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetLlbth_Set, device, anaAddr, regId, agcCfg->peak.apdLowThresh);
    }

    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetDecay_Set,  device, anaAddr, 0);
    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxTiaForceUpdate_Set, device, anaAddr, 1);
    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxTiaForceUpdate_Set, device, anaAddr, 0);

	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlbThresholdExceededCounter_Set, device, agcCfg->peak.apdUpperThreshPeakExceededCount);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLlbThresholdExceededCounter_Set, device, agcCfg->peak.apdLowerThreshPeakExceededCount);
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlbThresholdExceededCounter_Set, device, agcCfg->peak.apdUpperThreshPeakExceededCount);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLlbThresholdExceededCounter_Set, device, agcCfg->peak.apdLowerThreshPeakExceededCount);
	}


	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUlbGainStep_Set, device, rxbCommonAddr, agcCfg->peak.apdGainStepAttack);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLlbGainStep_Set, device, rxbCommonAddr, agcCfg->peak.apdGainStepRecovery);

    /*HB Configuration register*/
    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxEnableDecOverload_Set,         device, agcCfg->peak.enableHbOverload);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadDurationCount_Set,  device, agcCfg->peak.hbOverloadDurationCount);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadThresholdCount_Set, device, agcCfg->peak.hbOverloadThreshCount);

	    /* HB */
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcHighOvrgExceededCounter_Set, device, agcCfg->peak.hbUpperThreshPeakExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcLowOvrgExceededCounter_Set, device, agcCfg->peak.hbUnderRangeHighThreshExceededCount);
    }
    else
    {
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxEnableDecOverload_Set,         device, agcCfg->peak.enableHbOverload);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadDurationCount_Set,  device, agcCfg->peak.hbOverloadDurationCount);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadThresholdCount_Set, device, agcCfg->peak.hbOverloadThreshCount);

	    /* HB */
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcHighOvrgExceededCounter_Set, device, agcCfg->peak.hbUpperThreshPeakExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcLowOvrgExceededCounter_Set, device, agcCfg->peak.hbUnderRangeHighThreshExceededCount);
    }

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgLowGainStep_Set, device, rxbCommonAddr, agcCfg->peak.hbGainStepHighRecovery);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgHighGainStep_Set, device, rxbCommonAddr, agcCfg->peak.hbGainStepAttack);

    if (ADI_CHANNEL_1 == channel)
    {
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcOvrgLowInt0GainStep_Set, device, agcCfg->peak.hbGainStepLowRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcOvrgLowInt1GainStep_Set, device, agcCfg->peak.hbGainStepMidRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcovrgLowInt1Counter_Set, device, agcCfg->peak.hbUnderRangeMidThreshExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcovrgLowInt0Counter_Set, device, agcCfg->peak.hbUnderRangeLowThreshExceededCount);

        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadUpperThreshold_Set,     device, agcCfg->peak.hbHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadInt0LowerThreshold_Set, device, agcCfg->peak.hbUnderRangeLowThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadInt1LowerThreshold_Set, device, agcCfg->peak.hbUnderRangeMidThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadLowerThreshold_Set,     device, agcCfg->peak.hbUnderRangeHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadPowerMode_Set,                    device, agcCfg->peak.hbOverloadPowerMode);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecThresholdConfig_Set,                      device, HB_THRESH_CONFIG);
    }
    else
    {
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcOvrgLowInt0GainStep_Set, device, agcCfg->peak.hbGainStepLowRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcOvrgLowInt1GainStep_Set, device, agcCfg->peak.hbGainStepMidRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcovrgLowInt1Counter_Set, device, agcCfg->peak.hbUnderRangeMidThreshExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcovrgLowInt0Counter_Set, device, agcCfg->peak.hbUnderRangeLowThreshExceededCount);

        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadUpperThreshold_Set,     device, agcCfg->peak.hbHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadInt0LowerThreshold_Set, device, agcCfg->peak.hbUnderRangeLowThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadInt1LowerThreshold_Set, device, agcCfg->peak.hbUnderRangeMidThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadLowerThreshold_Set,     device, agcCfg->peak.hbUnderRangeHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadPowerMode_Set,                    device, agcCfg->peak.hbOverloadPowerMode);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecThresholdConfig_Set,                      device, HB_THRESH_CONFIG);
    }



    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv910x_NvsRegmapRxb_DecimatedDataOverloadSecondaryUpperThreshold_Set, device, threshCalc);

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_Adcovrg2ndHighCounter_Set, device, APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcApdLowFreqErrorMitigationMode_Set, device, APD_LOW_FREQ_ERROR_MITIGATION_MODE);
    }
    else
    {
        ADI_EXPECT(adrv910x_NvsRegmapRxnb2_DecimatedDataOverloadSecondaryUpperThreshold_Set, device, threshCalc);

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_Adcovrg2ndHighCounter_Set, device, APD_LOW_FREQ_ADCOVRG_2ND_HIGH_COUNTER);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcApdLowFreqErrorMitigationMode_Set, device, APD_LOW_FREQ_ERROR_MITIGATION_MODE);
    }



    /* Power Configuration register */

	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnableMeas_Set, device, rxbCommonAddr, agcCfg->power.powerEnableMeasurement);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerInputSelect_Set, device, rxbCommonAddr, POWER_INPUT_SELECT);

	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower1Threshold_Set, device, agcCfg->power.underRangeLowPowerThresh);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower0Threshold_Set, device, agcCfg->power.underRangeHighPowerThresh);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower0ThresholdExceededGainStep_Set, device, agcCfg->power.underRangeHighPowerGainStepRecovery);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower1ThresholdExceededGainStep_Set, device, agcCfg->power.underRangeLowPowerGainStepRecovery);
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower1Threshold_Set, device, agcCfg->power.underRangeLowPowerThresh);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower0Threshold_Set, device, agcCfg->power.underRangeHighPowerThresh);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower0ThresholdExceededGainStep_Set, device, agcCfg->power.underRangeHighPowerGainStepRecovery);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower1ThresholdExceededGainStep_Set, device, agcCfg->power.underRangeLowPowerGainStepRecovery);
	}


	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerMeasurementDuration_Set, device, rxbCommonAddr, agcCfg->power.powerMeasurementDuration);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerWaitDelay_Set, device, rxbCommonAddr, agcCfg->power.powerMeasurementDelay);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLockLevel_Set, device, rxbCommonAddr, agcCfg->power.overRangeLowPowerThresh);

	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlSigPowerMeasDuration_Set, device, agcCfg->power.rxTddPowerMeasDuration);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlSigPowerMeasDelay_Set, device, agcCfg->power.rxTddPowerMeasDelay);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper1Threshold_Set, device, agcCfg->power.overRangeHighPowerThresh);

		ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerLogShift_Set, device, rxbCommonAddr, POWER_LOG_SHIFT);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper1ThresholdExceededGainStep_Set, device, agcCfg->power.overRangeHighPowerGainStepAttack);
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper0ThresholdExceededGainStep_Set, device, agcCfg->power.overRangeLowPowerGainStepAttack);

		/* External LNA */
		ADI_EXPECT(adrv910x_NvsRegmapRxb_ExtLnaSettlingDelay_Set, device, agcCfg->extLna.settlingDelay);
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlSigPowerMeasDuration_Set, device, agcCfg->power.rxTddPowerMeasDuration);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlSigPowerMeasDelay_Set, device, agcCfg->power.rxTddPowerMeasDelay);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper1Threshold_Set, device, agcCfg->power.overRangeHighPowerThresh);

		ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerLogShift_Set, device, rxbCommonAddr, POWER_LOG_SHIFT);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper1ThresholdExceededGainStep_Set, device, agcCfg->power.overRangeHighPowerGainStepAttack);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper0ThresholdExceededGainStep_Set, device, agcCfg->power.overRangeLowPowerGainStepAttack);

		/* External LNA */
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_ExtLnaSettlingDelay_Set, device, agcCfg->extLna.settlingDelay);
	}



    if (agcCfg->agcMode == ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK)
    {
        controlMuxAddress = ADI_ADRV910X_GAIN_PEAK_ADDRESS;
    }
    else
    {
        controlMuxAddress = ADI_ADRV910X_GAIN_PEAK_POWER_ADDRESS;
    }

    ADI_EXPECT(adi_adrv910x_Rx_GainControl_Mode_Get, device, channel, &gainCtrlMode);

    if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, rxbCommonAddr, true);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, rxbCommonAddr, false);
    }
    else if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_PIN)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, rxbCommonAddr, true);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, rxbCommonAddr, false);
    }
    else if (gainCtrlMode == ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO)
    {

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set, device, rxbCommonAddr, false);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set, device, rxbCommonAddr, true);
    }

    /* Determine crumbs and sources to set */
    if (agcCfg->agcMode == ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK)
    {
        gpioCrumb1_0 = (uint8_t)agcCfg->peak.feedback_apd_low_hb_low;
        gpioCrumb3_2 = (uint8_t)agcCfg->peak.feedback_apd_high_hb_high;
        if (ADI_CHANNEL_1 == channel)
        {
            gpioSource1_0 = ADI_ADRV910X_GPIO_SOURCE_RX1_5_4;
            gpioSource3_2 = ADI_ADRV910X_GPIO_SOURCE_RX1_7_6;
        }
        else
        {
            gpioSource1_0 = ADI_ADRV910X_GPIO_SOURCE_RX2_5_4;
            gpioSource3_2 = ADI_ADRV910X_GPIO_SOURCE_RX2_7_6;
        }
    }
    else
    {
        gpioCrumb1_0 = (uint8_t)agcCfg->power.feedback_inner_high_inner_low;
        gpioCrumb3_2 = (uint8_t)agcCfg->power.feedback_apd_high_apd_low;
        if (ADI_CHANNEL_1 == channel)
        {
            gpioSource1_0 = ADI_ADRV910X_GPIO_SOURCE_RX1_5_4;
            gpioSource3_2 = ADI_ADRV910X_GPIO_SOURCE_RX1_7_6;
        }
        else
        {
            gpioSource1_0 = ADI_ADRV910X_GPIO_SOURCE_RX2_5_4;
            gpioSource3_2 = ADI_ADRV910X_GPIO_SOURCE_RX2_7_6;
        }
    }

    /* Configure GPIO control system */
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Ps1DgpioAssign_Get, device, &gpioOutEn);
	gpioOutEn |= (1 << (gpioCrumb1_0 * 2 - 1)) | (1 << (gpioCrumb1_0 * 2 - 2));
	gpioOutEn |= (1 << (gpioCrumb3_2 * 2 - 1)) | (1 << (gpioCrumb3_2 * 2 - 2));
    ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Ps1DgpioAssign_Set, device, gpioOutEn);

    /* Configure pins as outputs */
    ADI_EXPECT(adrv910x_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn |= (1 << (gpioCrumb1_0 * 2 - 1)) | (1 << (gpioCrumb1_0 * 2 - 2));
    gpioOutEn |= (1 << (gpioCrumb3_2 * 2 - 1)) | (1 << (gpioCrumb3_2 * 2 - 2));
    ADI_EXPECT(adrv910x_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);

    /* Configure source */
    ADRV910X_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb1_0 - 1), gpioSource1_0);
    ADRV910X_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + gpioCrumb3_2 - 1), gpioSource3_2);

    /* Configure Control Out Mux */
    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv910x_NvsRegmapRx_ControlOutMuxSel_Set, device, controlMuxAddress);
    }
    else
    {
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_ControlOutMuxSel_Set, device, controlMuxAddress);
    }

	gpioOutEn = 0;
	ADI_EXPECT(adrv910x_NvsRegmapCore_Ps1DigitalSourceSel_Get, device, &gpioOutEn);
	gpioOutEn |= (1 << (gpioCrumb1_0 * 2 - 1)) | (1 << (gpioCrumb1_0 * 2 - 2));
	gpioOutEn |= (1 << (gpioCrumb3_2 * 2 - 1)) | (1 << (gpioCrumb3_2 * 2 - 2));
	ADI_EXPECT(adrv910x_NvsRegmapCore_Ps1DigitalSourceSel_Set, device, gpioOutEn);

    /* Enable/Disable RXQEC Freeze*/
	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx1Fic2CaptPedSoftOverrideEnable_Set, device, agcCfg->rxQecFreezeEnable);
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx2Fic2CaptPedSoftOverrideEnable_Set, device, agcCfg->rxQecFreezeEnable);
	}

    if (agcCfg->gpioFreezePin != ADI_ADRV910X_GPIO_UNASSIGNED)
	{
		/* Enable freeze mode */
		if (ADI_CHANNEL_1 == channel)
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableGainFreeze_Set, device, 0x1);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableGainFreeze_Set, device, 0x1);
		}

		/* Set up GPIO pins */
		ADI_EXPECT(adi_adrv910x_gpio_ManualInput_Configure, device, agcCfg->gpioFreezePin);
		if (ADI_CHANNEL_1 == channel)
		{
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioSelect_Set, device, (agcCfg->gpioFreezePin - 1));
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioMask_Set, device, 0);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioSelect_Set, device, (agcCfg->gpioFreezePin - 1));
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioMask_Set, device, 0);
		}
	}

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_GainControl_Inspect_Validate(adi_adrv910x_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv910x_GainControlCfg_t *agcCfg)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, agcCfg);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_GainControl_Inspect(adi_adrv910x_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_GainControlCfg_t *agcCfg)
{
	adrv910x_BfNvsRegmapRxbCommon_e rxbCommonAddr = ADRV910X_BF_RXB1_CORE;
    adrv910x_BfNevisAnalogRxMemMap_e anaAddr = ADRV910X_BF_RX1_ANA;
	adrv910x_BfNvsRegmapRxbCommon_e nvsRegmapRxbCommonInstances[] = { ADRV910X_BF_RXB1_CORE, ADRV910X_BF_RXB_NB_CORE };

    uint8_t instanceIdx = 0;
    uint8_t bfValue = 0;
    uint8_t i = 0;

    static const uint16_t GPIO_SOURCE_SEL_ADDR = 0x24a;
    enum
    {
        ADI_ADRV910X_GPIO_SOURCE_RX1_5_4 = 0x16, /* peak or peak&power */
        ADI_ADRV910X_GPIO_SOURCE_RX1_7_6 = 0x17, /* peak or peak&power */
        ADI_ADRV910X_GPIO_SOURCE_RX2_5_4 = 0x1a, /* peak */
        ADI_ADRV910X_GPIO_SOURCE_RX2_7_6 = 0x1b
    };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_GainControl_Inspect_Validate, device, channel, agcCfg);

    adi_common_channel_to_index(channel, &instanceIdx);
	rxbCommonAddr = nvsRegmapRxbCommonInstances[instanceIdx];
    anaAddr = nevisAnalogRxMemMapInstances[instanceIdx];
    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcPeakWaitTime_Get,      device, rxbCommonAddr, &agcCfg->peakWaitTime);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Get, device, rxbCommonAddr, &agcCfg->maxGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get, device, rxbCommonAddr, &agcCfg->minGainIndex);
	ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcAttackDelay_Get, device, rxbCommonAddr, &agcCfg->attackDelay_us);

    /*HB Configuration register*/
    if (ADI_CHANNEL_1 == channel)
    {
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcGainUpdateCounter_Get, device, &agcCfg->gainUpdateCounter);

	    /* AGC Control register - Slowloop_config*/
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableFastRecoveryLoop_Get, device, &bfValue);
	    agcCfg->enableFastRecoveryLoop = (bool)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLowThsPreventGainInc_Get, device, &bfValue);
	    agcCfg->lowThreshPreventGainInc = bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcChangeGainIfUlbthHigh_Get, device, &bfValue);
	    agcCfg->changeGainIfThreshHigh = (bfValue & 0x01);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcChangeGainIfAdcovrgHigh_Get, device, &bfValue);
	    agcCfg->changeGainIfThreshHigh |= ((bfValue << 1) & 0x02);

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcPeakThresholdGainControlMode_Get, device, &bfValue);
	    agcCfg->agcMode = (adi_adrv910x_RxGainControlDetectionMode_e)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcResetOnRxon_Get, device, &bfValue);
	    agcCfg->resetOnRxon = (bool)bfValue;


	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Get, device, rxbCommonAddr, &agcCfg->resetOnRxonGainIndex);

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableSyncPulseForGainCounter_Get, device, &bfValue);
	    agcCfg->enableSyncPulseForGainCounter = (bool)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcSlowLoopSettlingDelay_Get, device, &agcCfg->slowLoopSettlingDelay);

	    /* Agc Peak */
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval0_Get, device, &agcCfg->peak.agcUnderRangeLowInterval);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval1Mult_Get, device, &agcCfg->peak.agcUnderRangeMidInterval);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUrangeInterval2Mult_Get, device, &agcCfg->peak.agcUnderRangeHighInterval);
	    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetUlbth_Get, device, anaAddr, 0, &agcCfg->peak.apdHighThresh);
	    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetLlbth_Get, device, anaAddr, 0, &agcCfg->peak.apdLowThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlbThresholdExceededCounter_Get, device, &agcCfg->peak.apdUpperThreshPeakExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLlbThresholdExceededCounter_Get, device, &agcCfg->peak.apdLowerThreshPeakExceededCount);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUlbGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.apdGainStepAttack);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLlbGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.apdGainStepRecovery);

        ADI_EXPECT(adrv910x_NvsRegmapRx_RxEnableDecOverload_Get, device, &bfValue);
        agcCfg->peak.enableHbOverload = (bool)bfValue;

        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadDurationCount_Get,                device,  &agcCfg->peak.hbOverloadDurationCount);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadThresholdCount_Get,               device,  &agcCfg->peak.hbOverloadThreshCount);

        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadUpperThreshold_Get,     device,  &agcCfg->peak.hbHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadInt0LowerThreshold_Get, device,  &agcCfg->peak.hbUnderRangeLowThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadInt1LowerThreshold_Get, device,  &agcCfg->peak.hbUnderRangeMidThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecimatedDataOverloadLowerThreshold_Get,     device,  &agcCfg->peak.hbUnderRangeHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRx_RxDecOverloadPowerMode_Get,                   device,  &agcCfg->peak.hbOverloadPowerMode);

	    /* ---------------------------------------------------------------- */
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcHighOvrgExceededCounter_Get, device, &agcCfg->peak.hbUpperThreshPeakExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcLowOvrgExceededCounter_Get, device, &agcCfg->peak.hbUnderRangeHighThreshExceededCount);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgLowGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.hbGainStepHighRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgHighGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.hbGainStepAttack);

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcOvrgLowInt0GainStep_Get, device, &agcCfg->peak.hbGainStepLowRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcOvrgLowInt1GainStep_Get, device, &agcCfg->peak.hbGainStepMidRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcovrgLowInt1Counter_Get, device, &agcCfg->peak.hbUnderRangeMidThreshExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcAdcovrgLowInt0Counter_Get, device, &agcCfg->peak.hbUnderRangeLowThreshExceededCount);

	    /* Power Configuration register */

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnableMeas_Get, device, rxbCommonAddr, &bfValue);
	    agcCfg->power.powerEnableMeasurement = (bool)bfValue;
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower1Threshold_Get, device, &agcCfg->power.underRangeLowPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower0Threshold_Get, device, &agcCfg->power.underRangeHighPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower0ThresholdExceededGainStep_Get, device, &agcCfg->power.underRangeHighPowerGainStepRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcLower1ThresholdExceededGainStep_Get, device, &agcCfg->power.underRangeLowPowerGainStepRecovery);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerMeasurementDuration_Get, device, rxbCommonAddr, &agcCfg->power.powerMeasurementDuration);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerWaitDelay_Get, device, rxbCommonAddr, &agcCfg->power.powerMeasurementDelay);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLockLevel_Get, device, rxbCommonAddr, &agcCfg->power.overRangeLowPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlSigPowerMeasDuration_Get, device, &agcCfg->power.rxTddPowerMeasDuration);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUlSigPowerMeasDelay_Get, device, &agcCfg->power.rxTddPowerMeasDelay);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper1Threshold_Get, device, &agcCfg->power.overRangeHighPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper1ThresholdExceededGainStep_Get, device, &agcCfg->power.overRangeHighPowerGainStepAttack);
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcUpper0ThresholdExceededGainStep_Get, device, &agcCfg->power.overRangeLowPowerGainStepAttack);

	    /* External LNA */
	    ADI_EXPECT(adrv910x_NvsRegmapRxb_ExtLnaSettlingDelay_Get, device, &agcCfg->extLna.settlingDelay);
    }
    else
    {
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcGainUpdateCounter_Get, device, &agcCfg->gainUpdateCounter);

	    /* AGC Control register - Slowloop_config*/
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableFastRecoveryLoop_Get, device, &bfValue);
	    agcCfg->enableFastRecoveryLoop = (bool)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLowThsPreventGainInc_Get, device, &bfValue);
	    agcCfg->lowThreshPreventGainInc = bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcChangeGainIfUlbthHigh_Get, device, &bfValue);
	    agcCfg->changeGainIfThreshHigh = (bfValue & 0x01);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcChangeGainIfAdcovrgHigh_Get, device, &bfValue);
	    agcCfg->changeGainIfThreshHigh |= ((bfValue << 1) & 0x02);

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcPeakThresholdGainControlMode_Get, device, &bfValue);
	    agcCfg->agcMode = (adi_adrv910x_RxGainControlDetectionMode_e)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcResetOnRxon_Get, device, &bfValue);
	    agcCfg->resetOnRxon = (bool)bfValue;


	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Get, device, rxbCommonAddr, &agcCfg->resetOnRxonGainIndex);

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableSyncPulseForGainCounter_Get, device, &bfValue);
	    agcCfg->enableSyncPulseForGainCounter = (bool)bfValue;

	    ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcSlowLoopSettlingDelay_Get, device, &agcCfg->slowLoopSettlingDelay);

	    /* Agc Peak */
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval0_Get, device, &agcCfg->peak.agcUnderRangeLowInterval);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval1Mult_Get, device, &agcCfg->peak.agcUnderRangeMidInterval);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUrangeInterval2Mult_Get, device, &agcCfg->peak.agcUnderRangeHighInterval);
	    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetUlbth_Get, device, anaAddr, 0, &agcCfg->peak.apdHighThresh);
	    ADI_EXPECT(adrv910x_NevisAnalogRxMemMap_OrxBlockDetLlbth_Get, device, anaAddr, 0, &agcCfg->peak.apdLowThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlbThresholdExceededCounter_Get, device, &agcCfg->peak.apdUpperThreshPeakExceededCount);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLlbThresholdExceededCounter_Get, device, &agcCfg->peak.apdLowerThreshPeakExceededCount);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcUlbGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.apdGainStepAttack);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLlbGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.apdGainStepRecovery);

        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxEnableDecOverload_Get, device, &bfValue);
        agcCfg->peak.enableHbOverload = (bool)bfValue;

        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadDurationCount_Get,                device,  &agcCfg->peak.hbOverloadDurationCount);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadThresholdCount_Get,               device,  &agcCfg->peak.hbOverloadThreshCount);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadUpperThreshold_Get,     device,  &agcCfg->peak.hbHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadInt0LowerThreshold_Get, device,  &agcCfg->peak.hbUnderRangeLowThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadInt1LowerThreshold_Get, device,  &agcCfg->peak.hbUnderRangeMidThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecimatedDataOverloadLowerThreshold_Get,     device,  &agcCfg->peak.hbUnderRangeHighThresh);
        ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxDecOverloadPowerMode_Get,                   device,  &agcCfg->peak.hbOverloadPowerMode);

	    /*------------------------------------------------*/
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcHighOvrgExceededCounter_Get, device, &agcCfg->peak.hbUpperThreshPeakExceededCount);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcLowOvrgExceededCounter_Get, device, &agcCfg->peak.hbUnderRangeHighThreshExceededCount);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgLowGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.hbGainStepHighRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcOvrgHighGainStep_Get, device, rxbCommonAddr, &agcCfg->peak.hbGainStepAttack);

		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcOvrgLowInt0GainStep_Get, device, &agcCfg->peak.hbGainStepLowRecovery);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcOvrgLowInt1GainStep_Get, device, &agcCfg->peak.hbGainStepMidRecovery);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcovrgLowInt1Counter_Get, device, &agcCfg->peak.hbUnderRangeMidThreshExceededCount);
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcAdcovrgLowInt0Counter_Get, device, &agcCfg->peak.hbUnderRangeLowThreshExceededCount);

	    /* Power Configuration register */

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerEnableMeas_Get, device, rxbCommonAddr, &bfValue);
	    agcCfg->power.powerEnableMeasurement = (bool)bfValue;
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower1Threshold_Get, device, &agcCfg->power.underRangeLowPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower0Threshold_Get, device, &agcCfg->power.underRangeHighPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower0ThresholdExceededGainStep_Get, device, &agcCfg->power.underRangeHighPowerGainStepRecovery);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcLower1ThresholdExceededGainStep_Get, device, &agcCfg->power.underRangeLowPowerGainStepRecovery);

	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerMeasurementDuration_Get, device, rxbCommonAddr, &agcCfg->power.powerMeasurementDuration);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_DecPowerWaitDelay_Get, device, rxbCommonAddr, &agcCfg->power.powerMeasurementDelay);
	    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcLockLevel_Get, device, rxbCommonAddr, &agcCfg->power.overRangeLowPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlSigPowerMeasDuration_Get, device, &agcCfg->power.rxTddPowerMeasDuration);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUlSigPowerMeasDelay_Get, device, &agcCfg->power.rxTddPowerMeasDelay);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper1Threshold_Get, device, &agcCfg->power.overRangeHighPowerThresh);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper1ThresholdExceededGainStep_Get, device, &agcCfg->power.overRangeHighPowerGainStepAttack);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcUpper0ThresholdExceededGainStep_Get, device, &agcCfg->power.overRangeLowPowerGainStepAttack);

	    /* External LNA */
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb2_ExtLnaSettlingDelay_Get, device, &agcCfg->extLna.settlingDelay);
    }

    /* GPIO */
    agcCfg->peak.feedback_apd_high_hb_high = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->peak.feedback_apd_low_hb_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->power.feedback_apd_high_apd_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED;
    agcCfg->power.feedback_inner_high_inner_low = ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED;
    for (i = 0; i < ADI_ADRV910X_GPIO_PIN_CRUMB_15_14; i++)
    {
        ADRV910X_SPIREADBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + i), &bfValue);
        if (agcCfg->agcMode == (adi_adrv910x_RxGainControlDetectionMode_e)(0))
        {
            switch (bfValue)
            {
            case ADI_ADRV910X_GPIO_SOURCE_RX1_5_4:
                agcCfg->power.feedback_inner_high_inner_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX2_5_4:
                agcCfg->power.feedback_inner_high_inner_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX1_7_6:
                agcCfg->power.feedback_apd_high_apd_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX2_7_6:
                agcCfg->power.feedback_apd_high_apd_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (bfValue)
            {
            case ADI_ADRV910X_GPIO_SOURCE_RX1_5_4:
                agcCfg->peak.feedback_apd_low_hb_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX2_5_4:
                agcCfg->peak.feedback_apd_low_hb_low = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX1_7_6:
                agcCfg->peak.feedback_apd_high_hb_high = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            case ADI_ADRV910X_GPIO_SOURCE_RX2_7_6:
                agcCfg->peak.feedback_apd_high_hb_high = (adi_adrv910x_GpioPinCrumbSel_e)(i + 1);
                break;
            default:
                break;
            }
        }
    }

    /* Enable/Disable RXQEC Freeze flag*/
	if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx1Fic2CaptPedSoftOverrideEnable_Get, device, &bfValue);
    }
	else
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_Rx2Fic2CaptPedSoftOverrideEnable_Get, device, &bfValue);
    }
	agcCfg->rxQecFreezeEnable = (bool)bfValue;

    /* GPIO Freeze Pin*/
	if (ADI_CHANNEL_1 == channel)
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxb_AgcEnableGainFreeze_Get, device, &bfValue);
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapRxnb2_AgcEnableGainFreeze_Get, device, &bfValue);
	}
	if (bfValue == 0x1)
	{
		if (ADI_CHANNEL_1 == channel)
		{
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx1AgcSlowloopFreezeGpioSelect_Get, device, &bfValue);
		}
		else
		{
			ADI_EXPECT(adrv910x_NvsRegmapCore1_Rx2AgcSlowloopFreezeGpioSelect_Get, device, &bfValue);
		}
		agcCfg->gpioFreezePin = (adi_adrv910x_GpioPin_e)(bfValue + 1);
	}
	else
	{
		agcCfg->gpioFreezePin = ADI_ADRV910X_GPIO_UNASSIGNED;
	}
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_Gain_Set_Validate(adi_adrv910x_Device_t *device,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                uint8_t gainIndex,
                                                                                adi_adrv910x_RxGainControlMode_e *gainCtrlMode)
{
    static const uint32_t RX_CHANNELS[] = {ADI_ADRV910X_RX1, ADI_ADRV910X_RXNB};
    uint8_t chan_idx = 0;

    /* Check for valid channel */
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    adi_common_channel_to_index(channel, &chan_idx);

    /* Check that Rx profile is valid */
    if (0 == ADRV910X_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_idx]))
    {
        ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                channel,
                "Gain index set requested for an Rx channel but Rx profile is invalid in the device structure");
    }

    /* Check that requested gain index is within range */
    ADI_RANGE_CHECK(device, gainIndex, ADI_ADRV910X_RX_GAIN_INDEX_MIN, ADI_ADRV910X_RX_GAIN_INDEX_MAX);

    /* Confirm mode is set to Manual SPI Gain control */
    ADI_EXPECT(adi_adrv910x_Rx_GainControl_Mode_Get, device, channel, gainCtrlMode);
    if (ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI != *gainCtrlMode)
    {
       ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid Rx gain control mode to set Rx Gain");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Gain_Set(adi_adrv910x_Device_t *device,
                                adi_common_ChannelNumber_e channel,
                                uint8_t gainIndex)
{

	adrv910x_BfNvsRegmapRxbCommon_e baseAddr = ADRV910X_BF_RXB1_CORE;
    adi_adrv910x_RxGainControlMode_e gainCtrlMode = ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI;
    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Gain_Set_Validate, device, channel, gainIndex, &gainCtrlMode);

    if (ADI_CHANNEL_2 == channel)
    {
	    baseAddr = ADRV910X_BF_RXB_NB_CORE;

    }

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcManualGainIndex_Set, device, baseAddr, gainIndex);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_Gain_Get_Validate(adi_adrv910x_Device_t *device,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                uint8_t *gainIndex)
{
    ADI_NULL_PTR_RETURN(&device->common, gainIndex);

    /* Check for valid channel */
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /* Check that Rx profile is valid */
    if ((device->devStateInfo.profilesValid & ADI_ADRV910X_RX_PROFILE_VALID) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_INV_PARAM,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        channel,
                        "Rx Gain index read requested but Rx profile is invalid in device structure");
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Gain_Get(adi_adrv910x_Device_t *device,
                                adi_common_ChannelNumber_e channel,
                                uint8_t *gainIndex)
{

	adrv910x_BfNvsRegmapRxbCommon_e baseAddr = ADRV910X_BF_RXB1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Gain_Get_Validate, device, channel, gainIndex);

    if (ADI_CHANNEL_2 == channel)
    {
        baseAddr = ADRV910X_BF_RXB_NB_CORE;
    }
    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adrv910x_NvsRegmapRxbCommon_AgcGainIndex_Get, device, baseAddr, gainIndex);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Rx_InterfaceGain_Validate(adi_adrv910x_Device_t *device,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_adrv910x_RxGainTableType_e gainTableType,
                                                      adi_adrv910x_RxInterfaceGain_e gain)
{
    uint8_t chan_index = 0;
    static const uint32_t RX_OUTPUT_RATE_kHZ = 1000;

    adi_adrv910x_RxInterfaceGain_e rxInterfaceGainMin = ADI_ADRV910X_RX_INTERFACE_GAIN_18_DB;
    adi_adrv910x_RxInterfaceGain_e rxInterfaceGainMax = ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_36_DB;

    adi_common_channel_to_index(channel, &chan_index);

    if (device->devStateInfo.rxOutputRate_kHz[chan_index] < RX_OUTPUT_RATE_kHZ)
    {
        if (gainTableType == ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE)
        {
            rxInterfaceGainMin = ADI_ADRV910X_RX_INTERFACE_GAIN_18_DB;
            rxInterfaceGainMax = ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB;
        }
        else
        {
            rxInterfaceGainMin = ADI_ADRV910X_RX_INTERFACE_GAIN_18_DB;
            rxInterfaceGainMax = ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_36_DB;
        }
    }
    else
    {
        if (gainTableType == ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE)
        {
            rxInterfaceGainMin = ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB;
            rxInterfaceGainMax = ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB;
        }
        else
        {
            rxInterfaceGainMin = ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB;
            rxInterfaceGainMax = ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_36_DB;
        }

    }

    ADI_RANGE_CHECK(device, gain, rxInterfaceGainMin, rxInterfaceGainMax);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_InterfaceGain_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                               adi_common_ChannelNumber_e channel,
                                                                               adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, rxInterfaceGainCtrl);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->updateInstance,
                    ADI_ADRV910X_RX_INTERFACE_GAIN_UPDATE_TIMING_NEXT_FRAME,
                    ADI_ADRV910X_RX_INTERFACE_GAIN_UPDATE_TIMING_NOW);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->controlMode,
                    ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_AUTOMATIC,
                    ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_MANUAL);

    /* Perform Range check of allowed gain value */
    ADI_EXPECT(adi_adrv910x_Rx_InterfaceGain_Validate,
               device,
               channel,
               device->devStateInfo.gainTableType[channel - 1],
               rxInterfaceGainCtrl->gain);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->rssiDuration,
                    32,
                    255);

    ADI_RANGE_CHECK(device,
                    rxInterfaceGainCtrl->rssiMovingAverageDuration,
                    1,
                    50);
	ADI_RANGE_CHECK(device,
		            rxInterfaceGainCtrl->gainControlAutomaticThreshold_dBFS,
		            -30,
		            0);
	ADI_RANGE_CHECK(device,
		            rxInterfaceGainCtrl->signalPAR,
		            0,
		            30);

    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if (ADI_ADRV910X_CHANNEL_CALIBRATED != state)
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

int32_t adi_adrv910x_Rx_InterfaceGain_Configure(adi_adrv910x_Device_t *device,
                                                adi_common_ChannelNumber_e channel,
                                                adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl)
{
    uint8_t armData[12] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_InterfaceGain_Configure_Validate, device, channel, rxInterfaceGainCtrl);

    armData[0] = (uint8_t)(rxInterfaceGainCtrl->updateInstance);
    armData[1] = (uint8_t)(rxInterfaceGainCtrl->controlMode);
    armData[2] = (uint8_t)(device->devStateInfo.gainTableType[channel - 1]);
    armData[3] = (uint8_t)(rxInterfaceGainCtrl->gain);
    armData[4] = (uint8_t)(rxInterfaceGainCtrl->rssiDuration);
    armData[5] = (uint8_t)(rxInterfaceGainCtrl->rssiMovingAverageDuration);
	armData[6] = (uint8_t)(rxInterfaceGainCtrl->gainControlAutomaticThreshold_dBFS);
	armData[7] = (uint8_t)(rxInterfaceGainCtrl->signalPAR);
	armData[8] = (uint8_t)(rxInterfaceGainCtrl->enableFastAttack);

    ADI_MUTEX_AQUIRE(device);

    /* Write RX interface gain control parameters to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
	extData[1] = OBJID_GS_SLICER_CONTROL;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV910X_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_InterfaceGain_Set_Validate(adi_adrv910x_Device_t *device,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv910x_RxInterfaceGain_e gain)
{
    adi_adrv910x_RxInterfaceGainCtrl_t rxInterfaceGainCtrl = { 0 };
    adi_adrv910x_RxGainTableType_e gainTableType = ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE;
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_EXPECT(adi_adrv910x_Rx_InterfaceGain_Inspect, device, channel, &rxInterfaceGainCtrl, &gainTableType);

    /* adi_adrv910x_RxInterfaceGain_Set() is allowed only in Manual mode */
    if (rxInterfaceGainCtrl.controlMode != ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_MANUAL)
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
    ADI_EXPECT(adi_adrv910x_Rx_InterfaceGain_Validate, device, channel, gainTableType, gain);

    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, device, ADI_RX, channel, &state);
    if ((ADI_ADRV910X_CHANNEL_PRIMED != state)
     && (ADI_ADRV910X_CHANNEL_RF_ENABLED != state))
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

int32_t adi_adrv910x_Rx_InterfaceGain_Set(adi_adrv910x_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_RxInterfaceGain_e gain)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_InterfaceGain_Set_Validate, device, channel, gain);

    armData[0] = (uint8_t)gain;
    ADI_MUTEX_AQUIRE(device);
    /* Write RX interface gain control parameters to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain command */
	extData[1] = OBJID_GS_SLICER_GAIN;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV910X_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_InterfaceGain_Inspect_Validate(adi_adrv910x_Device_t *device,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl,
                                                                             adi_adrv910x_RxGainTableType_e *gainTableType)
{
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &currentState);

    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, rxInterfaceGainCtrl);
    ADI_ENTRY_PTR_EXPECT(device, gainTableType);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (ADI_ADRV910X_CHANNEL_STANDBY == currentState.channelStates[port_index][chan_index])
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

int32_t adi_adrv910x_Rx_InterfaceGain_Inspect(adi_adrv910x_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainCtrl,
                                              adi_adrv910x_RxGainTableType_e *gainTableType)
{
    uint8_t armReadBack[12] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_InterfaceGain_Inspect_Validate, device, channel, rxInterfaceGainCtrl, gainTableType);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
	extData[1] = OBJID_GS_SLICER_CONTROL;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               device,
               ADRV910X_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               ADRV910X_ARM_MEM_READ_AUTOINCR,
		       ADI_PS1)

    rxInterfaceGainCtrl->updateInstance                         = (adi_adrv910x_RxInterfaceGainUpdateTiming_e)armReadBack[0];
    rxInterfaceGainCtrl->controlMode                            = (adi_adrv910x_RxInterfaceGainCtrlMode_e)armReadBack[1];
    *gainTableType                                              = (adi_adrv910x_RxGainTableType_e)armReadBack[2];
    rxInterfaceGainCtrl->gain                                   = (adi_adrv910x_RxInterfaceGain_e)armReadBack[3];
    rxInterfaceGainCtrl->rssiDuration                           = armReadBack[4];
    rxInterfaceGainCtrl->rssiMovingAverageDuration              = armReadBack[5];
	rxInterfaceGainCtrl->gainControlAutomaticThreshold_dBFS     = armReadBack[6];
	rxInterfaceGainCtrl->signalPAR                              = armReadBack[7];
	rxInterfaceGainCtrl->enableFastAttack                       = armReadBack[8];

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_InterfaceGain_Get_Validate(adi_adrv910x_Device_t *device,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv910x_RxInterfaceGain_e *gain)
{
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &currentState);

    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);

    /* Check device pointer and gain pointer are not null */
    ADI_ENTRY_PTR_EXPECT(device, gain);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (ADI_ADRV910X_CHANNEL_STANDBY == currentState.channelStates[port_index][chan_index])
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

int32_t adi_adrv910x_Rx_InterfaceGain_Get(adi_adrv910x_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_RxInterfaceGain_e *gain)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_InterfaceGain_Get_Validate, device, channel, gain);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);

    /* Executing the SET RX interface gain control command */
	extData[1] = OBJID_GS_SLICER_GAIN;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    /* read the ARM memory to get Rx interface Gain */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               device,
               ADRV910X_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               ADRV910X_ARM_MEM_READ_AUTOINCR,
               ADI_PS1)

    *gain = (adi_adrv910x_RxInterfaceGain_e)armReadBack[0];

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_Rssi_Read_Validate(adi_adrv910x_Device_t *device,
                                                                 uint32_t *rxRssiPower_mdB,
                                                                 adi_common_ChannelNumber_e rxChannel)
{

    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, rxRssiPower_mdB);

    /* Check that the requested channel mask is valid */
    ADI_RANGE_CHECK(device, rxChannel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /* Validate state is RF ENABLED*/
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(rxChannel, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid channel state. Channel must be in RF ENABLED state");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Rssi_Read(adi_adrv910x_Device_t *device,
                                  adi_common_ChannelNumber_e channel,
                                  uint32_t *rxRssiPower_mdB)
{
    uint8_t armExtData[2] = { 0 };
    uint8_t armReadBack[6] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Rssi_Read_Validate, device, rxRssiPower_mdB, channel);

    armExtData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
    armExtData[1] = OBJID_GO_RSSI;

    /* send ARM GET opcode */
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_GET_OPCODE, &armExtData[0], sizeof(armExtData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV910X_ARM_GET_OPCODE,
                                        armExtData[1],
                                        ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
                                        ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               device,
               ADRV910X_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false, ADI_PS1)
    adrv910x_ParseFourBytes(&offset, armReadBack, rxRssiPower_mdB);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_Rx_Loid_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    adi_adrv910x_RxrfdcLoidCfg_t *loidConfig)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, loidConfig->loidDisableTrackingHighThreshold_negdBFS, 0, loidConfig->loidReenableTrackingLowThreshold_negdBFS);


    ADI_NULL_PTR_RETURN(&device->common, loidConfig);

    /* Validate state is STANDBY or CALIBRATED*/
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_STANDBY != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid channel state. Channel must be in CALIBRATED or STANDBY state");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Loid_Configure(adi_adrv910x_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                        adi_adrv910x_RxrfdcLoidCfg_t *loidConfig)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Loid_Configure_Validate, device, channel, loidConfig);

	armData[0] = loidConfig->loidEnable;
	armData[1] = 1;
	armData[2] = loidConfig->loidReenableTrackingLowThreshold_negdBFS;
    armData[3] = loidConfig->loidDisableTrackingHighThreshold_negdBFS;

    ADI_MUTEX_AQUIRE(device);

    /* Write LOID config to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = OBJID_GS_LOID;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_SET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_Loid_Inspect_Validate(adi_adrv910x_Device_t *device,
                              adi_common_ChannelNumber_e channel,
                              adi_adrv910x_RxrfdcLoidCfg_t *loidConfig)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, loidConfig);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Loid_Inspect(adi_adrv910x_Device_t *device,
                                     adi_common_ChannelNumber_e channel,
                                     adi_adrv910x_RxrfdcLoidCfg_t *loidConfig)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Loid_Inspect_Validate, device, channel, loidConfig);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = OBJID_GS_LOID;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get LOID config */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

	loidConfig->loidEnable = armReadBack[0];
	loidConfig->loidReenableTrackingLowThreshold_negdBFS = armReadBack[2];
    loidConfig->loidDisableTrackingHighThreshold_negdBFS = armReadBack[3];

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_Loid_Status_Get_Validate(adi_adrv910x_Device_t *device,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                uint8_t *rfdcResidual)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /* Validate state is CALIBRATED, PRIMED, or RF_ENABLED */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_PRIMED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid channel state. Channel must be in CALIBRATED, PRIMED, or RF_ENABLED state");
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_Loid_Status_Get(adi_adrv910x_Device_t *device,
                                     adi_common_ChannelNumber_e channel,
                                     uint8_t *rfdcResidual)
{
    uint8_t armReadBack[1] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_Loid_Status_Get_Validate, device, channel, rfdcResidual);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = OBJID_GO_RX_RFDC_READ;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get residual DC */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		false, ADI_PS1);
		
	*rfdcResidual = armReadBack[0];

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_HighBlockerMode_Configure_Validate(adi_adrv910x_Device_t *device, adi_common_ChannelNumber_e channel,
adi_adrv910x_HighBlockerModeCfg_t *highBlockerConfig)
{
   adi_adrv910x_RadioState_t state = { 0 };
   uint8_t port_index = 0;
   uint8_t chan_index = 0;

	if (channel != ADI_CHANNEL_2)
	{

		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			channel,
			"Invalid channel. Channel must RxNB to perform High Blocker Mode Configuration.");
	}

   ADI_NULL_PTR_RETURN(&device->common, highBlockerConfig);

   ADI_RANGE_CHECK(device, highBlockerConfig->enableHighBlockerMode, false, true);
   ADI_RANGE_CHECK(device, highBlockerConfig->highBlockerModeThresholdOne_negQuartdBFS, 0, 64);
   ADI_RANGE_CHECK(device, highBlockerConfig->highBlockerModeThresholdTwo_QuartdB, 0, 640);


   /* Validate state is STANDBY or CALIBRATED */
   ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
   adi_common_port_to_index(ADI_RX, &port_index);
   adi_common_channel_to_index(channel, &chan_index);
	if ((ADI_ADRV910X_CHANNEL_STANDBY != state.channelStates[port_index][chan_index]) && (ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]))
	{
       ADI_ERROR_REPORT(&device->common,
           ADI_COMMON_ERRSRC_API,
           ADI_COMMON_ERR_INV_PARAM,
           ADI_COMMON_ACT_ERR_CHECK_PARAM,
           channel,
			"Invalid channel state. Channel must be in STANDBY or CALIBRATED state");
	}

   ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_HighBlockerMode_Configure(adi_adrv910x_Device_t *device, adi_common_ChannelNumber_e channel,
adi_adrv910x_HighBlockerModeCfg_t *highBlockerConfig)
{
    uint8_t armData[9] = { 0 };
    uint8_t extData[3] = { 0 };
	uint32_t offset = 4;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_HighBlockerMode_Configure_Validate, device, channel, highBlockerConfig);

	armData[offset++] = highBlockerConfig->enableHighBlockerMode;
	adrv910x_LoadTwoBytes(&offset, armData, highBlockerConfig->highBlockerModeThresholdOne_negQuartdBFS);
	adrv910x_LoadTwoBytes(&offset, armData, highBlockerConfig->highBlockerModeThresholdTwo_QuartdB);
	
    ADI_MUTEX_AQUIRE(device);

    /* Write High blocker config to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
	extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_HIGH_BLOCKER_CONFIG;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_SET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);

}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Rx_HighBlockerMode_Inspect_Validate(adi_adrv910x_Device_t *device, adi_common_ChannelNumber_e channel,
adi_adrv910x_HighBlockerModeCfg_t *highBlockerConfig)
{
   adi_adrv910x_RadioState_t state = { 0 };
   uint8_t port_index = 0;
   uint8_t chan_index = 0;

	if (channel != ADI_CHANNEL_2)
	{

		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			channel,
			"Invalid channel. Channel must RxNB to inspect High Blocker Mode Configuration.");
	}

   ADI_NULL_PTR_RETURN(&device->common, highBlockerConfig);

   /* Validate state is STANDBY or CALIBRATED */
   ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
   adi_common_port_to_index(ADI_RX, &port_index);
   adi_common_channel_to_index(channel, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_STANDBY != state.channelStates[port_index][chan_index]) && (ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]))
   {
       ADI_ERROR_REPORT(&device->common,
           ADI_COMMON_ERRSRC_API,
           ADI_COMMON_ERR_INV_PARAM,
           ADI_COMMON_ACT_ERR_CHECK_PARAM,
           channel,
           "Invalid channel state. Channel must be in STANDBY or CALIBRATED state");
   }

   ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Rx_HighBlockerMode_Inspect(adi_adrv910x_Device_t *device, adi_common_ChannelNumber_e channel,
adi_adrv910x_HighBlockerModeCfg_t *highBlockerConfig)
{
    uint8_t armReadBack[8] = { 0 };
    uint8_t extData[3] = { 0 };
	uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Rx_HighBlockerMode_Inspect_Validate, device, channel, highBlockerConfig);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, channel);
	extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_HIGH_BLOCKER_CONFIG;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get High Blocker config */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

	highBlockerConfig->enableHighBlockerMode = (bool)armReadBack[offset++];
	adrv910x_ParseTwoBytes(&offset, armReadBack, &highBlockerConfig->highBlockerModeThresholdOne_negQuartdBFS);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &highBlockerConfig->highBlockerModeThresholdTwo_QuartdB);

    ADI_MUTEX_RELEASE(device);

    ADI_API_RETURN(device);
}