/**
* \file
* \brief Contains Frequency Hopping functions implementation
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001_fh.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_gpio.h"
#include "adrv9001_bf.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_reg_addr_macros.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9001_fh.c"
#endif

#define FREQ_HOPPING_CONFIGURATION_NUM_BYTES 30u
#define FREQ_HOPPING_MAX_NUM_BYTES           1032u

#define ADI_FH_CHECK_FH_ENABLED(device) \
if (device->devStateInfo.frequencyHoppingEnabled == 0) \
{\
    ADI_ERROR_REPORT(&device->common, \
                     ADI_COMMON_ERRSRC_API, \
                     ADI_COMMON_ERR_API_FAIL, \
                     ADI_COMMON_ACT_ERR_CHECK_PARAM, \
                     devStateInfo->frequencyHoppingEnabled, \
                     "Frequency hopping not enabled in device profile"); \
    ADI_API_RETURN(device); \
}

/* TODO JP: Consider making the definitions for attenuation settings a #def in tx_types.h */
static const uint16_t NORMAL_MAX_ATTENUATION_MDB        = 41950;
static const uint16_t NORMAL_ATTENUATION_RESOLUTION_MDB = 50;

/*  TODO JP: Determine if we need to validate the whole table.
    It's difficult to validate the hop table due to the following:
	    1) At the time of receiving the HOP table, we don't know the channel sequence. So a tx or rx gain index
	       might be valid, or it might be a 'don't care' value, programmed by the user. We could potentially enforce the
		   user to set 0 to get around this.
        2) There doesn't seem a nice way to validate the rx gain in API, since there is no state
           of the FH operating gain range. The best we could do would be to ensure its within the
           absolute min/max. However, this could be effected by point 1.
        3) Validating the whole table before its written to ARM might be valuable, however, it might
           also waste time. Especially considering we can't validate every field in the table.
           It might be better for ARM to validate it once received, or on the fly.
*/
static int32_t adi_adrv9001_fh_FrameInfo_Validate(adi_adrv9001_Device_t *adrv9001,
                                                  adi_adrv9001_FhHopFrame_t *hopFrame)
{
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);

    ADI_RANGE_CHECK_X(adrv9001, hopFrame->hopFrequencyHz,
                     ADI_ADRV9001_FH_MIN_CARRIER_FREQUENCY_HZ,
                     ADI_ADRV9001_FH_MAX_CARRIER_FREQUENCY_HZ, "%llu");

#if 0
    ADI_RANGE_CHECK(adrv9001, hopFrame->txAttenuation_mdB, 0, NORMAL_MAX_ATTENUATION_MDB);

    if (hopFrame->txAttenuation_mdB % NORMAL_ATTENUATION_RESOLUTION_MDB != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         hopFrame->txAttenuation_mdB,
                         "Invalid attenuation_mdB value. The resolution is only 0.05dB");
    }

    ADI_RANGE_CHECK(adrv9001, hopFrame->rxGainIndex, MIN_RX_GAIN_INDEX, MAX_RX_GAIN_INDEX)
#endif
	/* TODO JP: Validate Offset frequency. Not sure about ranges */
	ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                  adi_adrv9001_FhCfg_t  *fhConfig)
{
    uint32_t i;
    uint8_t initialisedChannelMask;
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    /* Check for NULL pointer */
	ADI_NULL_PTR_RETURN(&adrv9001->common, fhConfig);
    /* Check channel mask */
	if ((!ADRV9001_BF_EQUAL(0x0F, fhConfig->channelMaskHopPin))
      ||(fhConfig->channelMaskHopPin == 0x00))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->channelMaskHopPin,
                         "Invalid bits set in frequency hopping channel mask.");
        ADI_API_RETURN(adrv9001);
    }
    initialisedChannelMask = (uint8_t)(adrv9001->devStateInfo.initializedChannels & 0x0F);
    /* Mask sure channels in channel mask have been enabled in device profile. channelMaskHopPin must be a subset of initializedChannels */
	if (!ADRV9001_BF_EQUAL(initialisedChannelMask, fhConfig->channelMaskHopPin))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adrv9001->devStateInfo.initializedChannels,
                         "Invalid frequency hopping channel mask for device profile");
        ADI_API_RETURN(adrv9001);
    }
    /* Check mode*/
    ADI_RANGE_CHECK(adrv9001, fhConfig->mode, ADI_ADRV9001_FHMODE_VERY_FAST, ADI_ADRV9001_FHMODE_SLOW);
    /* Check operating frequency range */
	ADI_RANGE_CHECK_X(adrv9001, fhConfig->minOperatingFrequency_Hz,
                      ADI_ADRV9001_FH_MIN_CARRIER_FREQUENCY_HZ,
                      ADI_ADRV9001_FH_MAX_CARRIER_FREQUENCY_HZ, "%llu");
	ADI_RANGE_CHECK_X(adrv9001, fhConfig->maxOperatingFrequency_Hz,
                      ADI_ADRV9001_FH_MIN_CARRIER_FREQUENCY_HZ,
                      ADI_ADRV9001_FH_MAX_CARRIER_FREQUENCY_HZ, "%llu");

    if (fhConfig->minOperatingFrequency_Hz >= fhConfig->maxOperatingFrequency_Hz)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->operatingFrequencyRangeHz,
                         "Minimum frequency in range is greater than maximum");
        ADI_API_RETURN(adrv9001);
    }
    /* Check RX gain ranges */
    if (ADRV9001_BF_EQUAL(fhConfig->channelMaskHopPin, ADI_CHANNEL_1))
    {
        ADI_RANGE_CHECK(adrv9001, fhConfig->minRxGainIndex,
                        adrv9001->devStateInfo.gainIndexes.rx1MinGainIndex,
                        fhConfig->maxRxGainIndex);
        /* Max index can be equal to min and no greater than rx1MaxGainIndex */
        ADI_RANGE_CHECK(adrv9001, fhConfig->maxRxGainIndex,
                        fhConfig->minRxGainIndex,
                        adrv9001->devStateInfo.gainIndexes.rx1MaxGainIndex);
    }
    /* TODO JP: Investigate gain requirements for diversity mode. Will min/max RX gain be
             the same for both channels in diversity mode?
    */
    if (ADRV9001_BF_EQUAL(fhConfig->channelMaskHopPin, ADI_CHANNEL_2))
    {
        ADI_RANGE_CHECK(adrv9001, fhConfig->minRxGainIndex,
                        adrv9001->devStateInfo.gainIndexes.rx2MinGainIndex,
                        fhConfig->maxRxGainIndex);
        /* Max index can be equal to min and no greater than rx2MaxGainIndex */
        ADI_RANGE_CHECK(adrv9001, fhConfig->maxRxGainIndex,
                        fhConfig->minRxGainIndex,
                        adrv9001->devStateInfo.gainIndexes.rx2MaxGainIndex);
    }
    /* TODO JP: Investigate requirements for TX attenuation in diversity mode.
             Will min/max be the same, or will we need a seperate field?
    */
    ADI_RANGE_CHECK(adrv9001, fhConfig->minTxAtten_mdB, 0, fhConfig->maxTxAtten_mdB);
    ADI_RANGE_CHECK(adrv9001, fhConfig->maxTxAtten_mdB, fhConfig->minTxAtten_mdB, NORMAL_MAX_ATTENUATION_MDB);

    if (fhConfig->minTxAtten_mdB % NORMAL_ATTENUATION_RESOLUTION_MDB != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->minTxAtten_mdB,
                         "Invalid attenuation_mdB value. The resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.05dB");
    }

    if (fhConfig->maxTxAtten_mdB % NORMAL_ATTENUATION_RESOLUTION_MDB != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->maxTxAtten_mdB,
                         "Invalid attenuation_mdB value. The resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.05dB");
    }

    /* Can be unassigned but throw an error if it's set to analog*/
    ADI_RANGE_CHECK(adrv9001, fhConfig->hopSignalGpioConfig.pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);

    /* Check frequency select pins */
	if (true == fhConfig->tableIndexByPin)
    {
	    ADI_RANGE_CHECK(adrv9001, fhConfig->numTableIndexPins, 1u, ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS);
        for (i = 0; i < fhConfig->numTableIndexPins; i++)
        {
	        ADI_RANGE_CHECK(adrv9001, fhConfig->tableIndexGpioConfig[i].pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
        }
    }
    /* Configure gain select pins */
    if (true == fhConfig->gainSetupByPin)
    {
	    ADI_RANGE_CHECK(adrv9001, fhConfig->numGainCtrlPins, 1u, ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS);
        for (i = 0; i < fhConfig->numGainCtrlPins; i++)
        {
            ADI_RANGE_CHECK(adrv9001, fhConfig->tableIndexGpioConfig[i].pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
        }
    }
	/* Can be unassigned but throw an error if it's set to analog*/
	ADI_RANGE_CHECK(adrv9001, fhConfig->hopTableSwitchGpioConfig.pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);

	ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                adi_adrv9001_FhCfg_t  *fhConfig)
{
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, fhConfig);
    ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_HopTable_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                           adi_adrv9001_FhHopTable_e tableId,
                                                           adi_adrv9001_FhHopFrame_t hopTable[],
                                                           uint32_t tableSize)
{
    uint32_t frequencyIndex = 0;
    uint32_t tempAddress = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? adrv9001->devStateInfo.fhHopTable1Addr :
                                                                      adrv9001->devStateInfo.fhHopTable2Addr;
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, hopTable);
    ADI_API_ENTRY_PTR_ARRAY_EXPECT(adrv9001, hopTable, tableSize);
    ADI_RANGE_CHECK(adrv9001, tableId, ADI_ADRV9001_FHHOPTABLE_A, ADI_ADRV9001_FHHOPTABLE_B);

    if ((tempAddress < ADRV9001_ADDR_ARM_START_DATA) || (tempAddress > ADRV9001_ADDR_ARM_END_DATA))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            adrv9001,
            "Invalid FH Hop Table Address");

        ADI_API_RETURN(adrv9001);
    }

    /* Check fhHopTable->numHopFrames are valid */
	ADI_RANGE_CHECK(adrv9001, tableSize, 1u, ADI_ADRV9001_FH_MAX_HOP_TABLE_SIZE);

    for (frequencyIndex = 0; frequencyIndex < tableSize; frequencyIndex++)
    {
	    ADI_EXPECT(adi_adrv9001_fh_FrameInfo_Validate, adrv9001, &hopTable[frequencyIndex]);
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_HopTable_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                         adi_adrv9001_FhHopTable_e tableId,
                                                         adi_adrv9001_FhHopFrame_t hopTable[],
                                                         uint32_t tableSize)
{
    uint32_t tempAddress = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? adrv9001->devStateInfo.fhHopTable1Addr :
                                                                    adrv9001->devStateInfo.fhHopTable2Addr;

    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, hopTable);
    ADI_API_ENTRY_PTR_ARRAY_EXPECT(adrv9001, hopTable, tableSize);
    /* Check table Id */
    ADI_RANGE_CHECK(adrv9001, tableId, ADI_ADRV9001_FHHOPTABLE_A, ADI_ADRV9001_FHHOPTABLE_B);

    if ((tempAddress < ADRV9001_ADDR_ARM_START_DATA) || (tempAddress > ADRV9001_ADDR_ARM_END_DATA))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            adrv9001,
            "Invalid FH Hop Table Address");

        ADI_API_RETURN(adrv9001);
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_Configure(adi_adrv9001_Device_t *adrv9001,
                                  adi_adrv9001_FhCfg_t  *fhConfig)
{
    /* Write the config data to the SET buffer */
    uint8_t extData[5] = { 0 };
    uint8_t armData[FREQ_HOPPING_CONFIGURATION_NUM_BYTES]  = { 0 };
    uint32_t offset = 0;
    uint32_t i;
    adi_adrv9001_GpioSignal_e frequencySelectPinSignal = 0;
    adi_adrv9001_GpioSignal_e gainSelectPinSignal      = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Configure_Validate, adrv9001, fhConfig);

    /* Size of configuration is first written to SET buffer, tells ARM how many bytes have been written */
	adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
	armData[offset++] = fhConfig->mode;
	armData[offset++] = fhConfig->channelMaskHopPin;
	armData[offset++] = fhConfig->tableIndexByPin;
	armData[offset++] = fhConfig->gainSetupByPin;
	armData[offset++] = fhConfig->minRxGainIndex;
	armData[offset++] = fhConfig->maxRxGainIndex;
	adrv9001_LoadTwoBytes(&offset, armData, fhConfig->minTxAtten_mdB);
	adrv9001_LoadTwoBytes(&offset, armData, fhConfig->maxTxAtten_mdB);
	adrv9001_LoadEightBytes(&offset, armData, fhConfig->minOperatingFrequency_Hz);
	adrv9001_LoadEightBytes(&offset, armData, fhConfig->maxOperatingFrequency_Hz);
    /* Write to set buffer */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData));

    /* Construct payload for set command and execute command */
	extData[0] =  0;   /* Channel Mask; unused for this command */
	extData[1] = ADRV9001_ARM_OBJECTID_FREQ_HOP_CONFIGURE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_SET_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_SET_OPCODE,
                                        ADRV9001_ARM_OBJECTID_FREQ_HOP_CONFIGURE,
                                        ADI_ADRV9001_WRITEARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_WRITEARMCFG_INTERVAL_US);

    /* After configuration, set GPIO's */
    /* Configure HOP1 pin if assigned */
	if (fhConfig->hopSignalGpioConfig.pin != ADI_ADRV9001_GPIO_UNASSIGNED)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP, &(fhConfig->hopSignalGpioConfig));
    }
    /* Configure table index pins if selected */
	if (fhConfig->tableIndexByPin == true)
    {
        frequencySelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_0;
        for (i = 0; i < fhConfig->numTableIndexPins; i++)
        {
            ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, (frequencySelectPinSignal + i), &(fhConfig->tableIndexGpioConfig[i]));
        }
    }
    /* Configure gain index pins if selected */
    if (fhConfig->gainSetupByPin == true)
    {
        gainSelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_GAIN_SEL_0;
        for (i = 0; i < fhConfig->numGainCtrlPins; i++)
        {
            ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, (gainSelectPinSignal + i), &(fhConfig->gainSelectGpioConfig[i]));
        }
    }
    /* Configure hop table switch GPIO if assigned */
    if (fhConfig->hopTableSwitchGpioConfig.pin != ADI_ADRV9001_GPIO_UNASSIGNED)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_TABLE_SWITCH, &(fhConfig->hopTableSwitchGpioConfig));
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_Configuration_Inspect(adi_adrv9001_Device_t *adrv9001,
                                              adi_adrv9001_FhCfg_t *fhConfig)
{
    /* Write the size to the GET buffer */
    uint8_t extData[5] = { 0 };
    uint8_t armData[FREQ_HOPPING_CONFIGURATION_NUM_BYTES]  = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Inspect_Validate, adrv9001, fhConfig);

	adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t));

    /* Construct GET payload and send command */
    extData[0] =  0;   /* Channel Mask; unused for this command */
	extData[1] = ADRV9001_ARM_OBJECTID_FREQ_HOP_CONFIGURE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* check the command status for timeout */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        ADRV9001_ARM_OBJECTID_FREQ_HOP_CONFIGURE,
                                        ADI_ADRV9001_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_READARMCFG_INTERVAL_US);

    /* Parse get buffer */
    offset = 0;
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(armData), false);
    fhConfig->mode              = (adi_adrv9001_FhMode_e)armData[offset++];
	fhConfig->channelMaskHopPin = armData[offset++];
	fhConfig->tableIndexByPin   = armData[offset++];
	fhConfig->gainSetupByPin    = armData[offset++];
	fhConfig->minRxGainIndex    = armData[offset++];
	fhConfig->maxRxGainIndex    = armData[offset++];
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->minTxAtten_mdB);
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->maxTxAtten_mdB);
	adrv9001_ParseEightBytes(&offset, armData, &fhConfig->minOperatingFrequency_Hz);
	adrv9001_ParseEightBytes(&offset, armData, &fhConfig->maxOperatingFrequency_Hz);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Configure(adi_adrv9001_Device_t *adrv9001,
                                           adi_adrv9001_FhHopTable_e tableId,
                                           adi_adrv9001_FhHopFrame_t hopTable[],
										   uint32_t hopTableSize)
{
    uint8_t extData[5]   = { 0 };
    /* ARM Data is written directly to ARM memory because FREQ_HOPPING_NUM_BYTES is greater than set buffer size */
    uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES] = { 0 };
    uint32_t offset = 0;
	uint32_t frequencyIndex = 0;
    /* Find address of hop table */
	uint32_t _pFhHopTable = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? adrv9001->devStateInfo.fhHopTable1Addr :
	                                                                 adrv9001->devStateInfo.fhHopTable2Addr;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Configure_Validate, adrv9001, tableId, hopTable, hopTableSize);
    /* Fill memory block */
	armData[offset++] = hopTableSize;
    offset  += 7u; /* Padding */
	for (frequencyIndex = 0; frequencyIndex < hopTableSize; frequencyIndex++)
    {
	    adrv9001_LoadEightBytes(&offset, armData, hopTable[frequencyIndex].hopFrequencyHz);
	    adrv9001_LoadFourBytes(&offset, armData, hopTable[frequencyIndex].rxOffsetFrequencyHz);
	    armData[offset++] = hopTable[frequencyIndex].rxGainIndex;
        offset ++; /* Padding */
	    adrv9001_LoadTwoBytes(&offset, armData, hopTable[frequencyIndex].txAttenuation_mdB);
    }

    /* Write the data directly to the ARM memory */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, _pFhHopTable, armData, sizeof(armData));

    /* Construct SET payload and send the command */
	extData[0] = 0;
	extData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SET;
	extData[2] = tableId;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
										ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SET,
                                        ADI_ADRV9001_SETFH_TIMEOUT_US,
                                        ADI_ADRV9001_SETFH_INTERVAL_US);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Inspect(adi_adrv9001_Device_t *adrv9001,
                                         adi_adrv9001_FhHopTable_e tableId,
										 adi_adrv9001_FhHopFrame_t hopTable[],
		                                 uint32_t hopTableSize,
		                                 uint32_t *numHopFramesRead)
{
    uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
	uint32_t frequencyIndex = 0;
    uint32_t numHopFrequenciesReadback = 0;
    /* Table must be read directly from ARM memory, instead of GET buffer. Get pointer to hop table */
    uint32_t _pFhHopTable = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? adrv9001->devStateInfo.fhHopTable1Addr :
	                                                                 adrv9001->devStateInfo.fhHopTable2Addr;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Inspect_Validate, adrv9001, tableId, hopTable, hopTableSize);

    /* Even though we are going to read directly from ARM memory, we can still use the GET protocol to
       tell ARM how many bytes we will read. ARM will return an error if the read size is invalid
    */
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t));
    /* Construct payload and send get command */
    extData[0] = 0; // Reset offset
    extData[1] = ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE,
                                        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* Read hop table from ARM memory */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, _pFhHopTable, armData, sizeof(armData), false);

    /* Parse memory block */
	offset = 0;
    /* First get fhHopTable->numHopFrames read back */
	numHopFrequenciesReadback = armData[offset++];
    if (numHopFramesRead != NULL)
    {
	    *numHopFramesRead = numHopFrequenciesReadback;
    }
    offset += 7u; /* Padding */
    /* Parse hop table */
	for (frequencyIndex = 0; frequencyIndex < numHopFrequenciesReadback; frequencyIndex++)
    {
	    adrv9001_ParseEightBytes(&offset, armData, &hopTable[frequencyIndex].hopFrequencyHz);
	    adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopTable[frequencyIndex].rxOffsetFrequencyHz));
	    hopTable[frequencyIndex].rxGainIndex = armData[offset++];
        offset ++; /* Padding */
	    adrv9001_ParseTwoBytes(&offset, armData, &hopTable[frequencyIndex].txAttenuation_mdB);
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Set(adi_adrv9001_Device_t *adrv9001,
                                     adi_adrv9001_FhHopTable_e tableId)
{
    /* Construct SET payload and send command */
    uint8_t extData[5] = { 0,
                           ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SELECT,
                           (uint8_t )tableId };

    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    ADI_RANGE_CHECK(adrv9001, tableId, ADI_ADRV9001_FHHOPTABLE_A, ADI_ADRV9001_FHHOPTABLE_B);
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SELECT,
                                        ADI_ADRV9001_SETFH_TIMEOUT_US,
                                        ADI_ADRV9001_SETFH_INTERVAL_US);

    ADI_API_RETURN(adrv9001);
}


int32_t adi_adrv9001_fh_HopTable_Get(adi_adrv9001_Device_t *adrv9001,
                                     adi_adrv9001_FhHopTable_e *tableId)
{
    uint8_t armData = sizeof(uint8_t);
     /* Construct SET payload and send command */
    uint8_t extData[5] = { 0,
                           ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE_SELECT
                         };

    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    ADI_NULL_PTR_RETURN(&adrv9001->common, tableId);
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, &armData, sizeof(armData));
    armData = 0;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
										ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE_SELECT,
                                        ADI_ADRV9001_SETFH_TIMEOUT_US,
                                        ADI_ADRV9001_SETFH_INTERVAL_US);
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, &armData, sizeof(armData), false);
    *tableId = (adi_adrv9001_FhHopTable_e)armData;

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_FrameInfo_Inspect(adi_adrv9001_Device_t *adrv9001,
                                          adi_adrv9001_FhFrameIndex_e frameIndex,
                                          adi_adrv9001_FhHopFrame_t *hopFrame)
{
    uint8_t armData[16u]  = { 0 };
    uint8_t extData[5u]   = { 0 };
    uint32_t offset = 0;

    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    ADI_NULL_PTR_RETURN(&adrv9001->common, hopFrame);
	ADI_RANGE_CHECK(adrv9001, frameIndex, ADI_ADRV9001_FHFRAMEINDEX_CURRENT_FRAME, ADI_ADRV9001_FHFRAMEINDEX_NEXT_FRAME);

    /* Write the size to the GET buffer */
	adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t));

    /* Construct the GET payload and send the command */
    extData[0] =  0;   /* Channel Mask; unused for this command */
	extData[1] = ADRV9001_ARM_OBJECTID_GET_FH_FRAME_INFO;
    extData[2] = frameIndex;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* check the command status for timeout */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        ADRV9001_ARM_OBJECTID_GET_FH_FRAME_INFO,
                                        ADI_ADRV9001_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_READARMCFG_INTERVAL_US);

    /* Read config data from GET buffer */
    offset = 0; // Reset offset
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(armData), false);
    adrv9001_ParseEightBytes(&offset, armData, &hopFrame->hopFrequencyHz);
    adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopFrame->rxOffsetFrequencyHz));
    hopFrame->rxGainIndex = armData[offset++];
    adrv9001_ParseTwoBytes(&offset, armData, &hopFrame->txAttenuation_mdB);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_Hop(adi_adrv9001_Device_t *adrv9001)
{
    /* Flip the hop signal */
    uint8_t BbicHop = 0u;
    ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop1_Get, adrv9001, &BbicHop);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop1_Set, adrv9001, !BbicHop);

    ADI_API_RETURN(adrv9001);
}