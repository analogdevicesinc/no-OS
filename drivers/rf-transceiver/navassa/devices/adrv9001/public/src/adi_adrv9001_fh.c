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

#include "adi_adrv9001_fh.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_rx_types.h"
#include "adrv9001_bf.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_user.h"

#include "adrv9001_arm_macros.h"
#include "adrv9001_reg_addr_macros.h"
#include "adi_adrv9001_spi.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9001_fh.c"
#endif

#define FREQ_HOPPING_SIZE_FIELD_NUM_BYTES          4u
#define FREQ_HOPPING_RX_GAIN_TABLE_NUM_BYTES       9u  // 8 1 byte entries + 1 byte to indicate number of valid entries
#define FREQ_HOPPING_TX_ATTEN_TABLE_NUM_BYTES      17u // 8 2 byte entries + 1 byte to indicate number of valid entries
#define FREQ_HOPPING_CONFIGURATION_NUM_BYTES       40u
#define FREQ_HOPPING_CONFIGURATION_TOTAL_NUM_BYTES (FREQ_HOPPING_SIZE_FIELD_NUM_BYTES     +\
                                                    FREQ_HOPPING_RX_GAIN_TABLE_NUM_BYTES  +\
                                                    FREQ_HOPPING_TX_ATTEN_TABLE_NUM_BYTES +\
                                                    FREQ_HOPPING_CONFIGURATION_NUM_BYTES)
#define FREQ_HOPPING_MAX_NUM_BYTES                 1032u
#define FREQ_HOPPING_MAX_TX_FE_POWERON_FRAME_DELAY 64u

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
    uint8_t initializedChannelMask;
    uint8_t maxPossibleGainEntries;
    uint8_t rxChannelMask = (ADI_ADRV9001_RX1 | \
                             ADI_ADRV9001_RX2);
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, fhConfig);
    /* Check channel mask */
    if ((!ADRV9001_BF_EQUAL(0x0F, fhConfig->hopSignalChannelMask))
      ||(fhConfig->hopSignalChannelMask == 0x00))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->hopSignalChannelMask,
                         "Invalid bits set in frequency hopping channel mask.");
        ADI_API_RETURN(adrv9001);
    }
    initializedChannelMask = (uint8_t)(adrv9001->devStateInfo.initializedChannels & 0x0F);
    /* Make sure channels in channel mask have been enabled in device profile. hopSignalChannelMask must be a subset of initializedChannels */
    if (!ADRV9001_BF_EQUAL(initializedChannelMask, fhConfig->hopSignalChannelMask))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->hopSignalChannelMask,
                         "Invalid frequency hopping channel mask for device profile");
        ADI_API_RETURN(adrv9001);
    }
    /* Check txAnalogPowerOnFrameDelay is within range */
    ADI_RANGE_CHECK(adrv9001, fhConfig->txAnalogPowerOnFrameDelay, 0, FREQ_HOPPING_MAX_TX_FE_POWERON_FRAME_DELAY);
    /* Ensure tx analog power on delay is zero if Rx is enabled */
    if ((ADRV9001_BF_EQUAL(fhConfig->hopSignalChannelMask, rxChannelMask))
      &&(fhConfig->txAnalogPowerOnFrameDelay > 0))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            fhConfig->txAnalogPowerOnFrameDelay,
            "Value must be non-zero in Tx only operation");
        ADI_API_RETURN(adrv9001);
    }
    
    /* Check mode*/
    ADI_RANGE_CHECK(adrv9001, fhConfig->mode, ADI_ADRV9001_FHMODE_LO_MUX_PREPROCESS, ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS);
    /* Check tableIndexCtrl_e */
    ADI_RANGE_CHECK(adrv9001, fhConfig->tableIndexCtrl, ADI_ADRV9001_TABLEINDEXCTRL_AUTO_LOOP, ADI_ADRV9001_TABLEINDEXCTRL_GPIO);
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
    ADI_RANGE_CHECK(adrv9001, fhConfig->minRxGainIndex, 
                    ADI_ADRV9001_RX_GAIN_INDEX_MIN, 
                    fhConfig->maxRxGainIndex);
    /* Max index can be equal to min and no greater than rx1MaxGainIndex */
    ADI_RANGE_CHECK(adrv9001, fhConfig->maxRxGainIndex, 
                    fhConfig->minRxGainIndex, 
                    ADI_ADRV9001_RX_GAIN_INDEX_MAX);
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
    if (ADI_ADRV9001_TABLEINDEXCTRL_GPIO == fhConfig->tableIndexCtrl) 
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
        ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.numGainCtrlPins, 1u, ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS);
        for (i = 0; i < fhConfig->gainSetupByPinConfig.numGainCtrlPins; i++)
        {
            ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.gainSelectGpioConfig[i].pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
        }
        maxPossibleGainEntries = (1u << fhConfig->gainSetupByPinConfig.numGainCtrlPins);

        /* Validate Rx gain table is within range specified by fhConfig */
        ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.numRxGainTableEntries, 1u, maxPossibleGainEntries);
        for (i = 0; i < fhConfig->gainSetupByPinConfig.numRxGainTableEntries; i++)
        {
	        ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.rxGainTable[i], fhConfig->minRxGainIndex, fhConfig->maxRxGainIndex);
        }

        /* Validate Tx atten table is within range specified by fhConfig */
	    ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.numTxAttenTableEntries, 1u, maxPossibleGainEntries);
        for (i = 0; i < fhConfig->gainSetupByPinConfig.numTxAttenTableEntries; i++)
        {
	        ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig.txAttenTable[i], fhConfig->minTxAtten_mdB, fhConfig->maxTxAtten_mdB);
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

static int32_t adi_adrv9001_fh_HopTable_Static_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
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
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, hopTable, tableSize);
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
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, hopTable, tableSize);
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
    uint8_t armData[FREQ_HOPPING_CONFIGURATION_TOTAL_NUM_BYTES]  = { 0 };
    uint32_t offset = 0;
    uint32_t tempOffset = 0;
    uint32_t i;
    adi_adrv9001_GpioSignal_e frequencySelectPinSignal = 0;
    adi_adrv9001_GpioSignal_e gainSelectPinSignal      = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Configure_Validate, adrv9001, fhConfig);

    /* Size of configuration is first written to SET buffer, tells ARM how many bytes have been written */
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = fhConfig->mode;
    armData[offset++] = fhConfig->hopSignalChannelMask;
    armData[offset++] = fhConfig->tableIndexCtrl;
    armData[offset++] = fhConfig->gainSetupByPin;
    armData[offset++] = fhConfig->minRxGainIndex;
    armData[offset++] = fhConfig->maxRxGainIndex;
    adrv9001_LoadTwoBytes(&offset, armData, fhConfig->minTxAtten_mdB);
    adrv9001_LoadTwoBytes(&offset, armData, fhConfig->maxTxAtten_mdB);
    offset += 6u; /* padding */
    adrv9001_LoadEightBytes(&offset, armData, fhConfig->minOperatingFrequency_Hz);
    adrv9001_LoadEightBytes(&offset, armData, fhConfig->maxOperatingFrequency_Hz);
    adrv9001_LoadFourBytes(&offset, armData, fhConfig->minFrameDuration_us);
    armData[offset++] = fhConfig->txAnalogPowerOnFrameDelay;
    offset += 3u; /* padding */
    /* If in gain select by pin mode, load Rx gain and Tx attenuation tables */
    if (fhConfig->gainSetupByPin == true) 
    {
        /* Load Rx gain and Tx atten table */
	    armData[offset++] = fhConfig->gainSetupByPinConfig.numRxGainTableEntries;
        armData[offset++] = fhConfig->gainSetupByPinConfig.numTxAttenTableEntries;
        /* Create a second offset variable to point to the Tx atten table location. 
           Rx gain index is 1 byte, so second offset is offset + (ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES * 1)
        */
        tempOffset = offset + ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES;
        /* Rx and Tx tables */
	    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
        {
	        armData[offset++] = fhConfig->gainSetupByPinConfig.rxGainTable[i];
            adrv9001_LoadTwoBytes(&tempOffset, armData, fhConfig->gainSetupByPinConfig.txAttenTable[i]);
        }
    }
    /* Write to set buffer */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

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
    if (fhConfig->tableIndexCtrl == ADI_ADRV9001_TABLEINDEXCTRL_GPIO)
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
        /* Configure ADRV9001 GPIOs */
        gainSelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_GAIN_SEL_0;
	    for (i = 0; i < fhConfig->gainSetupByPinConfig.numGainCtrlPins; i++)
        {
	        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, (gainSelectPinSignal + i), &(fhConfig->gainSetupByPinConfig.gainSelectGpioConfig[i]));
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
    uint8_t armData[FREQ_HOPPING_CONFIGURATION_TOTAL_NUM_BYTES]  = { 0 };
    uint32_t offset = 0;
    uint32_t tempOffset = 0;
    uint32_t i;
    adi_adrv9001_GpioSignal_e frequencySelectPinSignal = 0;
    adi_adrv9001_GpioSignal_e gainSelectPinSignal      = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Inspect_Validate, adrv9001, fhConfig);
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

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
    fhConfig->hopSignalChannelMask = armData[offset++];
    fhConfig->tableIndexCtrl    = armData[offset++];
    fhConfig->gainSetupByPin    = armData[offset++];
    fhConfig->minRxGainIndex    = armData[offset++];
    fhConfig->maxRxGainIndex    = armData[offset++];
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->minTxAtten_mdB);
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->maxTxAtten_mdB);
    offset += 6u;
    adrv9001_ParseEightBytes(&offset, armData, &fhConfig->minOperatingFrequency_Hz);
    adrv9001_ParseEightBytes(&offset, armData, &fhConfig->maxOperatingFrequency_Hz);
    adrv9001_ParseFourBytes(&offset,  armData, &fhConfig->minFrameDuration_us);
    fhConfig->txAnalogPowerOnFrameDelay = armData[offset++];
    offset += 3u;
    /* Get gain setup by pin information */
    if (fhConfig->gainSetupByPin)
    {
        fhConfig->gainSetupByPinConfig.numRxGainTableEntries  = armData[offset++];
        fhConfig->gainSetupByPinConfig.numTxAttenTableEntries = armData[offset++];
        tempOffset = offset + ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES;
        /* Rx and Tx tables */
	    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
        {
	        fhConfig->gainSetupByPinConfig.rxGainTable[i] = armData[offset++];
            adrv9001_ParseTwoBytes(&tempOffset, armData, &fhConfig->gainSetupByPinConfig.txAttenTable[i]);
        }
        /* Configure ADRV9001 GPIOs */
        gainSelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_GAIN_SEL_0;
	    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS; i++)
        {
	        ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, (gainSelectPinSignal + i), &(fhConfig->gainSetupByPinConfig.gainSelectGpioConfig[i]));
        }
    }
    /* Get GPIO configurations */
    ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP, &(fhConfig->hopSignalGpioConfig));
    /* Configure table index pins if selected */
    if (fhConfig->tableIndexCtrl == ADI_ADRV9001_TABLEINDEXCTRL_GPIO)
    {
        frequencySelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_0;
        for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS; i++)
        {
            ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, (frequencySelectPinSignal + i), &(fhConfig->tableIndexGpioConfig[i]));
        }
    }
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Static_Configure(adi_adrv9001_Device_t *adrv9001,
                                                  adi_adrv9001_FhHopTable_e tableId, 
                                                  adi_adrv9001_FhHopFrame_t hopTable[],
                                                  uint32_t hopTableSize)
{
    /* ARM Data is written directly to ARM memory because FREQ_HOPPING_NUM_BYTES is greater than set buffer size */
    uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES] = { 0 };
    uint32_t offset = 0;
    uint32_t frequencyIndex = 0;
    /* Find address of hop table */
    uint32_t _pFhHopTable = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? adrv9001->devStateInfo.fhHopTable1Addr : 
                                                                     adrv9001->devStateInfo.fhHopTable2Addr;

    /* Set bit 0 to SW_INTERRUPT_4 to trigger SWInt4 to load FH table A
    Set bit 1 to SW_INTERRUPT_4 to trigger SWInt11 to load FH table B */
    uint8_t bitmSwInt = (tableId == ADI_ADRV9001_FHHOPTABLE_A) ? 0x1 : 0x2;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Static_Configure_Validate, adrv9001, tableId, hopTable, hopTableSize);
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
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, _pFhHopTable, armData, sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* Issue SW interrupt 4 or 11 to load FH table A or B.  The SPI reg is self-cleared so there is no need to do read/mod/write. */
    ADRV9001_SPIWRITEBYTE(adrv9001, "ADRV9001_ADDR_SW_INTERRUPT_4", ADRV9001_ADDR_SW_INTERRUPT_4, bitmSwInt);

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
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
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
        hopTable[frequencyIndex].reserved = 0;
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
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, &armData, sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
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
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

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

static int32_t adi_adrv9001_fh_NumberOfHops_Get(adi_adrv9001_Device_t *adrv9001,
                                                adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                uint32_t *numberOfHops)
{
    switch (numberHopsPerDynamicLoad)
    {
    case ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_ONE:
        *numberOfHops = 1;
        break;
    case ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_FOUR:
        *numberOfHops = 4;
        break;
    case ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_EIGHT:
        *numberOfHops = 8;
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(adrv9001);
    }
    
    ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_HopTable_Dynamic_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                   adi_adrv9001_FhHopFrame_t hopTable[],
                                                                   uint32_t tableSize,
                                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                                   uint8_t *spiPackedFhTable,
                                                                   uint32_t length)
{
    uint32_t frequencyIndex = 0;
    uint32_t totalBytesPerHop = 0;
    uint32_t totalSpiPackedTableSize = 0;
    uint32_t totalHopTablePairs = 0;
    uint32_t numberOfHops = 0;
    static const uint32_t SPI_ADDRESS_BYTES = 18;
    static const uint32_t SPI_PACKED_DATA_BYTE = 3;

    ADI_ENTRY_PTR_EXPECT(adrv9001, spiPackedFhTable);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, hopTable, tableSize);
    ADI_RANGE_CHECK(adrv9001, numberHopsPerDynamicLoad, ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_ONE, ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_EIGHT);
    ADI_EXPECT(adi_adrv9001_fh_NumberOfHops_Get, adrv9001, numberHopsPerDynamicLoad, &numberOfHops);

    if ((tableSize % (2 * numberOfHops)) != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            tableSize,
            "tableSize must be a multiple of twice the size of numberHopsPerDynamicLoad ");
    }

    totalBytesPerHop = ((numberOfHops * sizeof(adi_adrv9001_FhHopFrame_t)) + 8) * SPI_PACKED_DATA_BYTE;
    totalHopTablePairs = tableSize / numberOfHops;
    totalSpiPackedTableSize = (SPI_ADDRESS_BYTES + totalBytesPerHop) * totalHopTablePairs;
    if (length < totalSpiPackedTableSize)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            tableSize,
            "spiPackedFhTable[] size is not sufficient ");
        
    }
    
    for (frequencyIndex = 0; frequencyIndex < tableSize; frequencyIndex++)
    {
        ADI_EXPECT(adi_adrv9001_fh_FrameInfo_Validate, adrv9001, &hopTable[frequencyIndex]);
    }

    ADI_API_RETURN(adrv9001);
}

static void adi_adrv9001_HopTable_Spi_DataPack(uint8_t *wrData,
                                               uint32_t *numWrBytes,
                                               uint16_t addr,
                                               uint8_t data,
                                               uint8_t writeFlag)
{
    wrData[(*numWrBytes)++] = (uint8_t)(((writeFlag & 0x01) << 7) | ((addr >> 8) & 0x7F));
    wrData[(*numWrBytes)++] = (uint8_t)(addr);
    /* coverty will trigger an error which is ok in this case. */
    wrData[(*numWrBytes)++] = (uint8_t)data;
}

static uint32_t adrv9001_HopTable_Spi_Pack(adi_adrv9001_Device_t *adrv9001,
                                           uint8_t *addrArray,
                                           uint8_t *hopTable,
                                           uint32_t numberOfHops,
                                           uint32_t *numWrBytes,
                                           uint8_t bitmSwInt,
                                           uint8_t spiPackedFhTable[])
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t addrIdx = 0;
    uint8_t regVal = 0;
    uint8_t hopTableSize[8] = { 0 };
    uint32_t offset = 0;
    uint32_t dataIdx = 0;
    uint32_t ADDR_ARM_DMA_ADDR[4] = { ADRV9001_ADDR_ARM_DMA_ADDR3, ADRV9001_ADDR_ARM_DMA_ADDR2, ADRV9001_ADDR_ARM_DMA_ADDR1, ADRV9001_ADDR_ARM_DMA_ADDR0 };
    uint32_t ADDR_ARM_DMA_DATA[4] = { ADRV9001_ADDR_ARM_DMA_DATA3, ADRV9001_ADDR_ARM_DMA_DATA2, ADRV9001_ADDR_ARM_DMA_DATA1, ADRV9001_ADDR_ARM_DMA_DATA0 };
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADRV9001_SPIREADBYTEDMA(adrv9001, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, &regVal);

    adrv9001_LoadEightBytes(&offset, hopTableSize, (uint64_t)numberOfHops);

    /* Configure ADRV9001 DMA control register */
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADRV9001_ADDR_ARM_DMA_CTL, 0x4A, ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < 4; i++)
    {
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_ADDR[i], addrArray[i], ADRV9001_SPI_WRITE_POLARITY);
    }

    for (i = 0; i < 8; i += 4)
    {
        addrIdx = i % 4;
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 0], hopTableSize[i + 3], ADRV9001_SPI_WRITE_POLARITY);
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 1], hopTableSize[i + 2], ADRV9001_SPI_WRITE_POLARITY);
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 2], hopTableSize[i + 1], ADRV9001_SPI_WRITE_POLARITY);
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 3], hopTableSize[i + 0], ADRV9001_SPI_WRITE_POLARITY);
    }

    for (i = 0; i < numberOfHops; i++)
    {
        for (j = 0; j < 16; j += 4)
        {
            addrIdx = j % 4;
            dataIdx = (i * 16) + j;
            adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 0], hopTable[dataIdx + 3], ADRV9001_SPI_WRITE_POLARITY);
            adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 1], hopTable[dataIdx + 2], ADRV9001_SPI_WRITE_POLARITY);
            adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 2], hopTable[dataIdx + 1], ADRV9001_SPI_WRITE_POLARITY);
            adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[addrIdx + 3], hopTable[dataIdx + 0], ADRV9001_SPI_WRITE_POLARITY);
        }
    }

    /* Issue SW interrupt 4 or 11 to load FH table A or B.  The SPI reg is self-cleared so there is no need to do read/mod/write. */
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADRV9001_ADDR_SW_INTERRUPT_4, bitmSwInt, ADRV9001_SPI_WRITE_POLARITY);
    
    /* Restore back the original values of ADRV9001 DMA control register */
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADRV9001_ADDR_ARM_DMA_CTL, regVal, ADRV9001_SPI_WRITE_POLARITY);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Dynamic_Configure(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_FhHopFrame_t hopTable[],
                                                   uint32_t hopTableSize,
                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                   uint8_t spiPackedFhTable[],
                                                   uint32_t length)
{
    uint8_t addrArray_A[4] = { 0 };
    uint8_t addrArray_B[4] = { 0 };
    uint8_t fhTable_A[160] = { 0 };
    uint8_t fhTable_B[160] = { 0 };
    uint8_t bitmSwInt = 0;
    uint32_t numWrBytes = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t armAddr = 0;
    uint32_t offset = 0;
    uint32_t numberOfHops = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Dynamic_Configure_Validate, adrv9001, hopTable, hopTableSize, numberHopsPerDynamicLoad, spiPackedFhTable, length);
    ADI_EXPECT(adi_adrv9001_fh_NumberOfHops_Get, adrv9001, numberHopsPerDynamicLoad, &numberOfHops);
    
    armAddr = adrv9001->devStateInfo.fhHopTable1Addr >> 2;
    addrArray_A[0] = (uint8_t)((armAddr >> 24) & 0x000000FF);
    addrArray_A[1] = (uint8_t)((armAddr >> 16) & 0x000000FF);
    addrArray_A[2] = (uint8_t)((armAddr >> 8) & 0x000000FF);
    addrArray_A[3] = (uint8_t)(armAddr & 0x000000FF);

    armAddr = adrv9001->devStateInfo.fhHopTable2Addr >> 2;
    addrArray_B[0] = (uint8_t)((armAddr >> 24) & 0x000000FF);
    addrArray_B[1] = (uint8_t)((armAddr >> 16) & 0x000000FF);
    addrArray_B[2] = (uint8_t)((armAddr >> 8) & 0x000000FF);
    addrArray_B[3] = (uint8_t)(armAddr & 0x000000FF);

    for (i = 0; i < hopTableSize; i += (2 * numberOfHops))
    {
        offset = 0;
        for (j = 0; j < numberOfHops; j++)
        {
            adrv9001_LoadEightBytes(&offset, fhTable_A, hopTable[i + j].hopFrequencyHz);
            adrv9001_LoadFourBytes(&offset, fhTable_A, hopTable[i + j].rxOffsetFrequencyHz);
            fhTable_A[offset++] = hopTable[i + j].rxGainIndex;
            offset++; /* Padding */
            adrv9001_LoadTwoBytes(&offset, fhTable_A, hopTable[i + j].txAttenuation_mdB);
        }

        /* Set bit 0x1 to SW_INTERRUPT_4 to trigger SWInt4 to load FH table A */
        bitmSwInt = 0x1;
        ADI_EXPECT(adrv9001_HopTable_Spi_Pack, adrv9001, addrArray_A, fhTable_A, numberOfHops, &numWrBytes, bitmSwInt, spiPackedFhTable);
    
        offset = 0;
        for (j = numberOfHops; j < (2 * numberOfHops); j++)
        {
            adrv9001_LoadEightBytes(&offset, fhTable_B, hopTable[i + j].hopFrequencyHz);
            adrv9001_LoadFourBytes(&offset, fhTable_B, hopTable[i + j].rxOffsetFrequencyHz);
            fhTable_B[offset++] = hopTable[i + j].rxGainIndex;
            offset++; /* Padding */
            adrv9001_LoadTwoBytes(&offset, fhTable_B, hopTable[i + j].txAttenuation_mdB);
        }

        /* Set bit 0x2 to SW_INTERRUPT_4 to trigger SWInt4 to load FH table B */
        bitmSwInt = 0x2;
        ADI_EXPECT(adrv9001_HopTable_Spi_Pack, adrv9001, addrArray_B, fhTable_B, numberOfHops, &numWrBytes, bitmSwInt, spiPackedFhTable);
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t adi_adrv9001_fh_HopTable_BytesPerTable_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                                   uint32_t *bytesPerTable)
{
    ADI_RANGE_CHECK(adrv9001, numberHopsPerDynamicLoad, ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_ONE, ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_EIGHT);
    ADI_NULL_PTR_RETURN(&adrv9001->common, bytesPerTable);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_BytesPerTable_Get(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                   uint32_t *bytesPerTable)
{
    uint32_t spiPackBytesPerTable = 0;
    uint32_t payloadBytes = 0;
    uint32_t numberOfHops = 0;

    /* Address length = 4 bytes; SPI packed bytes = 4 * 3 = 12 bytes */
    static uint32_t spiAddrPackLength = 12;
    static uint32_t spiConfigBytes = 6;
    static uint32_t spiInterruptBytes = 3;
    static uint32_t fhBytesLength = 8;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_BytesPerTable_Get_Validate, adrv9001, numberHopsPerDynamicLoad, bytesPerTable);
    ADI_EXPECT(adi_adrv9001_fh_NumberOfHops_Get, adrv9001, numberHopsPerDynamicLoad, &numberOfHops);

    spiPackBytesPerTable = spiConfigBytes + spiAddrPackLength + spiInterruptBytes;
    
    payloadBytes = ((sizeof(adi_adrv9001_FhHopFrame_t) * numberOfHops) + fhBytesLength) * 3;
    *bytesPerTable = payloadBytes + spiPackBytesPerTable;

    ADI_API_RETURN(adrv9001);
}
