/**
* \file
* \brief Frequency Hopping functions
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
#include "adi_adrv9001_tx_types.h"
#include "adrv9001_bf.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_user.h"

#include "adrv9001_arm_macros.h"
#include "adrv9001_reg_addr_macros.h"
#include "adi_adrv9001_spi.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_fh_types.h"
#include "object_ids.h"

#ifdef __KERNEL__
#include <linux/string.h>
#endif

#define FREQ_HOPPING_SIZE_FIELD_NUM_BYTES            4u
#define FREQ_HOPPING_RX_GAIN_TABLE_NUM_BYTES         17u // 16 1 byte entries + 1 byte to indicate number of valid entries
#define FREQ_HOPPING_TX_ATTEN_TABLE_NUM_BYTES        35u // 16 2 byte entries + 1 byte to indicate number of valid entries
#define FREQ_HOPPING_CONFIGURATION_NUM_BYTES         40u
#define FREQ_HOPPING_CONFIGURATION_TOTAL_NUM_BYTES   (FREQ_HOPPING_SIZE_FIELD_NUM_BYTES     +\
                                                      FREQ_HOPPING_RX_GAIN_TABLE_NUM_BYTES  +\
                                                      FREQ_HOPPING_TX_ATTEN_TABLE_NUM_BYTES +\
                                                      FREQ_HOPPING_CONFIGURATION_NUM_BYTES)
#define FREQ_HOPPING_MAX_NUM_BYTES                   1536u
#define FREQ_HOPPING_HOP_TABLE_PARTITION_ADDR_OFFSET 16u

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
static __maybe_unused int32_t adi_adrv9001_fh_FrameInfo_Validate(adi_adrv9001_Device_t *adrv9001,
                                                     adi_adrv9001_FhHopFrame_t *hopFrame)
{
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);

    ADI_RANGE_CHECK_X(adrv9001, hopFrame->hopFrequencyHz,  
                     ADI_ADRV9001_FH_MIN_CARRIER_FREQUENCY_HZ, 
                     ADI_ADRV9001_FH_MAX_CARRIER_FREQUENCY_HZ, "%llu");
    ADI_API_RETURN(adrv9001);
}

static uint32_t adi_adrv9001_fh_GetHopTableBufferAddress(adi_adrv9001_Device_t *adrv9001,
                                                            uint32_t hopTableArmAddress)
{
    uint32_t hopTablePointerAddress = 0;
    uint32_t hopTableBufferAddress = 0;
    uint8_t hopTableBufferAddressBlock[4u] = { 0 };
    uint32_t offset = 0;

    /* Get ARM address for hop table buffer pointer */
    hopTablePointerAddress = hopTableArmAddress + sizeof(uint32_t);
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, hopTablePointerAddress, hopTableBufferAddressBlock, sizeof(hopTableBufferAddressBlock), false);
    adrv9001_ParseFourBytes(&offset, hopTableBufferAddressBlock, &hopTableBufferAddress);

    return hopTableBufferAddress;
}                                                         

static __maybe_unused int32_t adi_adrv9001_fh_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                 adi_adrv9001_FhCfg_t  *fhConfig)
{  
    uint32_t i;
	uint32_t j;
    uint8_t numHopSignals;
    uint8_t initializedChannelMask;
    uint8_t maxPossibleGainEntries;
	uint8_t channel1Mask  = (ADI_ADRV9001_RX1 | ADI_ADRV9001_TX1);
    uint8_t channel2Mask  = (ADI_ADRV9001_RX2 | ADI_ADRV9001_TX2);
    uint8_t rxChannelMask = (ADI_ADRV9001_RX1 | ADI_ADRV9001_RX2);
	static const uint32_t CHANNELS[][2] = {
		{ ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 },
		{ ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 }
	};
    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, fhConfig);
    initializedChannelMask = (uint8_t)(adrv9001->devStateInfo.initializedChannels & 0x0F);
    /* Check that DUAL hop is enabled for a device profile which has Channel 2 enabled */
    if ((fhConfig->mode == ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP)
      &&((initializedChannelMask & channel2Mask) == 0x00u))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         initializedChannelMask,
                         "Single channel device profile not valid for dual hop operation");
        ADI_API_RETURN(adrv9001);
    }

    /* Validate hop signal configurations */
    numHopSignals = (fhConfig->mode == ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP) ? 0x02 : 0x01;
    for (i = 0; i < numHopSignals; i++)
    {
        /* Can be unassigned but throw an error if it's set to analog*/
        ADI_RANGE_CHECK(adrv9001, fhConfig->hopSignalGpioConfig[i].pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);
    }
    /* Can be unassigned but throw an error if it's set to analog*/
    ADI_RANGE_CHECK(adrv9001, fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[0].pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);
    if ((fhConfig->hopTableSelectConfig.hopTableSelectMode == ADI_ADRV9001_FHHOPTABLESELECTMODE_INDEPENDENT) 
     && (fhConfig->mode == ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP))
    {
        /* Can be unassigned but throw an error if it's set to analog*/
        ADI_RANGE_CHECK(adrv9001, fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1].pin, ADI_ADRV9001_GPIO_UNASSIGNED, ADI_ADRV9001_GPIO_DIGITAL_15);
    }
    /* Only assign a second hop pin if in dual hop mode and independent select is enabled */
    if ((fhConfig->mode != ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP)
      &&(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1].pin != ADI_ADRV9001_GPIO_UNASSIGNED))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1].pin,
                "Second entry of hopTableSelectGpioConfig can only be assigned in dual hop mode");
            ADI_API_RETURN(adrv9001);
    }
    /* Check to see if second hop pin is assigned when hop table select mode is common */
    if ((fhConfig->hopTableSelectConfig.hopTableSelectMode == ADI_ADRV9001_FHHOPTABLESELECTMODE_COMMON)
      &&(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1].pin != ADI_ADRV9001_GPIO_UNASSIGNED))
    {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1].pin,
                "Second entry of hopTableSelectGpioConfig cannot be assigned if hopTableSlectMode is common");
            ADI_API_RETURN(adrv9001);
    }

    /* Check txAnalogPowerOnFrameDelay is within range */
    ADI_RANGE_CHECK(adrv9001, fhConfig->txAnalogPowerOnFrameDelay, 0, ADI_ADRV9001_FH_MAX_TX_FE_POWERON_FRAME_DELAY);
    /* Ensure tx analog power on delay is zero if Rx is enabled */
    if ((ADRV9001_BF_EQUAL(initializedChannelMask, rxChannelMask))
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
    ADI_RANGE_CHECK(adrv9001, fhConfig->mode, ADI_ADRV9001_FHMODE_LO_MUX_PREPROCESS, ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP);

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
    ADI_RANGE_CHECK(adrv9001, fhConfig->maxTxAtten_mdB, fhConfig->minTxAtten_mdB, ADRV9001_TX_MAX_ATTENUATION_MDB);

    if (fhConfig->minTxAtten_mdB % ADRV9001_TX_ATTENUATION_RESOLUTION_MDB != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->minTxAtten_mdB,
                         "Invalid attenuation_mdB value. The resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.05dB");
    }

    if (fhConfig->maxTxAtten_mdB % ADRV9001_TX_ATTENUATION_RESOLUTION_MDB != 0)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         fhConfig->maxTxAtten_mdB,
                         "Invalid attenuation_mdB value. The resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.05dB");
    }

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
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
		{
			if ((initializedChannelMask & (j == 0? channel1Mask:channel2Mask)) != 0x00u)
			{
				
				ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].numGainCtrlPins, 1u, ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS);
				for (i = 0; i < fhConfig->gainSetupByPinConfig[j].numGainCtrlPins; i++)
				{
					ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].gainSelectGpioConfig[i].pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
				}
				maxPossibleGainEntries = (1u << fhConfig->gainSetupByPinConfig[j].numGainCtrlPins);
			}

			
			if (ADRV9001_BF_EQUAL(adrv9001->devStateInfo.initializedChannels, CHANNELS[ADI_RX][j]))
			{
				/* Validate Rx gain table is within range specified by fhConfig */
				ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].numRxGainTableEntries, 1u, maxPossibleGainEntries);
				for (i = 0; i < fhConfig->gainSetupByPinConfig[j].numRxGainTableEntries; i++)
				{
					ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].rxGainTable[i], fhConfig->minRxGainIndex, fhConfig->maxRxGainIndex);
				}
			}
			if (ADRV9001_BF_EQUAL(adrv9001->devStateInfo.initializedChannels, CHANNELS[ADI_TX][j]))
			{
				/* Validate Tx atten table is within range specified by fhConfig */
				ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].numTxAttenTableEntries, 1u, maxPossibleGainEntries);
				for (i = 0; i < fhConfig->gainSetupByPinConfig[j].numTxAttenTableEntries; i++)
				{
					ADI_RANGE_CHECK(adrv9001, fhConfig->gainSetupByPinConfig[j].txAttenTable[i], fhConfig->minTxAtten_mdB, fhConfig->maxTxAtten_mdB);
				}
			}  
	    }
    }

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_fh_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                               adi_adrv9001_FhCfg_t  *fhConfig)
{  
    /* Check for NULL pointer */
    ADI_NULL_PTR_RETURN(&adrv9001->common, fhConfig);
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_fh_HopTable_Static_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                 adi_adrv9001_FhMode_e mode,
                                                                                 adi_adrv9001_FhHopSignal_e hopSignal,
                                                                                 adi_adrv9001_FhHopTable_e tableId, 
                                                                                 adi_adrv9001_FhHopFrame_t hopTable[],
                                                                                 uint32_t tableSize)
{
    uint32_t frequencyIndex = 0;
	uint32_t tempAddress = 0;
	if (tableId == ADI_ADRV9001_FHHOPTABLE_A)
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableA1Addr;
		}
		else
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableA2Addr;
		}
	}
	else
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableB1Addr;
		}
		else
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableB2Addr;
		}
	}
    uint8_t maxNumHopFrequencies = (mode != ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP) ? ADI_ADRV9001_FH_MAX_HOP_TABLE_SIZE :
                                                                                                      (ADI_ADRV9001_FH_MAX_HOP_TABLE_SIZE) / 2;                                                                          
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

    if ((hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_2) 
     && (mode != ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         hopSignal,
                         "Frequency hopping mode must be ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP to write HOP2 table");

        ADI_API_RETURN(adrv9001);
    }
    
    /* Check fhHopTable->numHopFrames are valid */
    ADI_RANGE_CHECK(adrv9001, tableSize, 1u, maxNumHopFrequencies);

    for (frequencyIndex = 0; frequencyIndex < tableSize; frequencyIndex++)
    {
        ADI_EXPECT(adi_adrv9001_fh_FrameInfo_Validate, adrv9001, &hopTable[frequencyIndex]);
    }

    ADI_API_RETURN(adrv9001);
}          

static __maybe_unused int32_t adi_adrv9001_fh_HopTable_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                        adi_adrv9001_FhHopSignal_e hopSignal,
                                                                        adi_adrv9001_FhHopTable_e tableId,
                                                                        adi_adrv9001_FhHopFrame_t hopTable[],
                                                                        uint32_t tableSize)
{
	uint32_t tempAddress = 0;
	if (tableId == ADI_ADRV9001_FHHOPTABLE_A)
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableA1Addr;
		}
		else
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableA2Addr;
		}
	}
	else
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableB1Addr;
		}
		else
		{
			tempAddress = adrv9001->devStateInfo.fhHopTableB2Addr;
		}
	}
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
    uint8_t hop1SignalsPortMask = 0;
    uint8_t hop2SignalsPortMask  = 0;
    uint32_t offset = 0;
    uint32_t tempOffset = 0;
    uint32_t i = 0;
	uint32_t j = 0;
    adi_adrv9001_GpioSignal_e frequencySelectPinSignal = 0;
    adi_adrv9001_GpioSignal_e gainSelectPinSignal      = 0;
    adi_adrv9001_MailboxChannel_e rxHopSignalPortMask[2] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    adi_adrv9001_MailboxChannel_e txHopSignalPortMask[2] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Configure_Validate, adrv9001, fhConfig);

    for (i = 0; i < ADI_ADRV9001_NUM_CHANNELS; i++)
    {
        if (ADI_ADRV9001_FH_HOP_SIGNAL_1 == fhConfig->rxPortHopSignals[i])
        {
            hop1SignalsPortMask |= rxHopSignalPortMask[i];
        }
        if (ADI_ADRV9001_FH_HOP_SIGNAL_1 == fhConfig->txPortHopSignals[i])
        {
            hop1SignalsPortMask |= txHopSignalPortMask[i];
        }
        if (ADI_ADRV9001_FH_HOP_SIGNAL_2 == fhConfig->rxPortHopSignals[i])
        {
            hop2SignalsPortMask |= rxHopSignalPortMask[i];
        }
        if (ADI_ADRV9001_FH_HOP_SIGNAL_2 == fhConfig->txPortHopSignals[i])
        {
            hop2SignalsPortMask |= txHopSignalPortMask[i];
        }
    }

    /* Size of configuration is first written to SET buffer, tells ARM how many bytes have been written */
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = fhConfig->mode;
    armData[offset++] = fhConfig->tableIndexCtrl;
    armData[offset++] = fhConfig->gainSetupByPin;
    armData[offset++] = fhConfig->hopTableSelectConfig.hopTableSelectMode;
    
    armData[offset++] = hop1SignalsPortMask;
    armData[offset++] = hop2SignalsPortMask;

    armData[offset++] = fhConfig->minRxGainIndex;
    armData[offset++] = fhConfig->maxRxGainIndex;
    adrv9001_LoadTwoBytes(&offset, armData, fhConfig->minTxAtten_mdB);
    adrv9001_LoadTwoBytes(&offset, armData, fhConfig->maxTxAtten_mdB);

    offset += 4u; /* padding */

    adrv9001_LoadEightBytes(&offset, armData, fhConfig->minOperatingFrequency_Hz);
    adrv9001_LoadEightBytes(&offset, armData, fhConfig->maxOperatingFrequency_Hz);

    adrv9001_LoadFourBytes(&offset, armData, fhConfig->minFrameDuration_us);
    armData[offset++] = fhConfig->txAnalogPowerOnFrameDelay;
    armData[offset++] = fhConfig->rxZeroIfEnable;
    offset += 2u; /* padding */
    /* If in gain select by pin mode, load Rx gain and Tx attenuation tables */
    if (fhConfig->gainSetupByPin == true) 
    {
        /* Load Rx gain and Tx atten table */
        armData[offset++] = fhConfig->gainSetupByPinConfig[0].numRxGainTableEntries;
	    armData[offset++] = fhConfig->gainSetupByPinConfig[1].numRxGainTableEntries;
        armData[offset++] = fhConfig->gainSetupByPinConfig[0].numTxAttenTableEntries;
	    armData[offset++] = fhConfig->gainSetupByPinConfig[1].numTxAttenTableEntries;
        /* Create a second offset variable to point to the Tx atten table location. 
           Rx gain index is 1 byte, so second offset is offset + (ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES * 1)
        */
        tempOffset = offset + 2 * ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES;
        /* Rx and Tx tables */
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
		    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
		    {
			    armData[offset++] = fhConfig->gainSetupByPinConfig[j].rxGainTable[i];
		    }
	    }
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
		    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
		    {
			    adrv9001_LoadTwoBytes(&tempOffset, armData, fhConfig->gainSetupByPinConfig[j].txAttenTable[i]);
		    }
	    }
    }
    /* Write to set buffer */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* Construct payload for set command and execute command */
    extData[0] =  0;   /* Channel Mask; unused for this command */
    extData[1] = OBJID_GS_FREQ_HOP_CONFIGURE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_SET_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_SET_OPCODE,
                                        OBJID_GS_FREQ_HOP_CONFIGURE,
                                        ADI_ADRV9001_WRITEARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_WRITEARMCFG_INTERVAL_US);

    /* After configuration, set GPIO's */
    /* Configure HOP1 pin if assigned */
    if (fhConfig->hopSignalGpioConfig[0u].pin != ADI_ADRV9001_GPIO_UNASSIGNED)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP, &(fhConfig->hopSignalGpioConfig[0u]));
    }

    /* Configure hop table select GPIO if assigned */
    if (fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[0u].pin != ADI_ADRV9001_GPIO_UNASSIGNED)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_TABLE_SELECT, &(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[0u]));
    }

    if (fhConfig->mode == ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP)
    {
        /* Configure HOP2 pin if assigned */
        if (fhConfig->hopSignalGpioConfig[1u].pin != ADI_ADRV9001_GPIO_UNASSIGNED)
        {
            ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2, &(fhConfig->hopSignalGpioConfig[1u]));
        }
        if (fhConfig->hopTableSelectConfig.hopTableSelectMode == ADI_ADRV9001_FHHOPTABLESELECTMODE_INDEPENDENT)
        {
            /* Configure hop table select GPIO if assigned */
            if (fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1u].pin != ADI_ADRV9001_GPIO_UNASSIGNED)
            {
                ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2_TABLE_SELECT, &(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1u]));
            }
        }
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
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
            gainSelectPinSignal = j == 0? ADI_ADRV9001_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_0 : ADI_ADRV9001_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_0;
		    for (i = 0; i < fhConfig->gainSetupByPinConfig[j].numGainCtrlPins; i++)
		    {
			    ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, (gainSelectPinSignal + i), &(fhConfig->gainSetupByPinConfig[j].gainSelectGpioConfig[i]));
		    }
	    }
    }
	adrv9001->devStateInfo.fhHopTableA2Addr = adrv9001->devStateInfo.fhHopTableA1Addr + FREQ_HOPPING_HOP_TABLE_PARTITION_ADDR_OFFSET;
	adrv9001->devStateInfo.fhHopTableB2Addr = adrv9001->devStateInfo.fhHopTableB1Addr + FREQ_HOPPING_HOP_TABLE_PARTITION_ADDR_OFFSET;
	adrv9001->devStateInfo.fhHopTableBufferA1Addr = adi_adrv9001_fh_GetHopTableBufferAddress(adrv9001, adrv9001->devStateInfo.fhHopTableA1Addr);
	adrv9001->devStateInfo.fhHopTableBufferB1Addr = adi_adrv9001_fh_GetHopTableBufferAddress(adrv9001, adrv9001->devStateInfo.fhHopTableB1Addr);
	adrv9001->devStateInfo.fhHopTableBufferA2Addr = adi_adrv9001_fh_GetHopTableBufferAddress(adrv9001, adrv9001->devStateInfo.fhHopTableA2Addr);
	adrv9001->devStateInfo.fhHopTableBufferB2Addr = adi_adrv9001_fh_GetHopTableBufferAddress(adrv9001, adrv9001->devStateInfo.fhHopTableB2Addr);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_Configuration_Inspect(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_FhCfg_t *fhConfig)
{
    /* Write the size to the GET buffer */
    uint8_t extData[5] = { 0 };
    uint8_t hop1SignalsPortMask = 0;
    uint8_t armData[FREQ_HOPPING_CONFIGURATION_TOTAL_NUM_BYTES]  = { 0 };
    uint32_t offset = 0;
    uint32_t tempOffset = 0;
    uint32_t i;
	uint32_t j;
    adi_adrv9001_GpioSignal_e frequencySelectPinSignal = 0;
    adi_adrv9001_GpioSignal_e gainSelectPinSignal      = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_Inspect_Validate, adrv9001, fhConfig);
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* Construct GET payload and send command */
    extData[0] =  0;   /* Channel Mask; unused for this command */
    extData[1] = OBJID_GS_FREQ_HOP_CONFIGURE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* check the command status for timeout */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        OBJID_GS_FREQ_HOP_CONFIGURE,
                                        ADI_ADRV9001_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_READARMCFG_INTERVAL_US);

    /* Parse get buffer */
    offset = 0;
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(armData), false);
    fhConfig->mode              = (adi_adrv9001_FhMode_e)armData[offset++];
    fhConfig->tableIndexCtrl    = armData[offset++];
    fhConfig->gainSetupByPin    = armData[offset++];
    fhConfig->hopTableSelectConfig.hopTableSelectMode = armData[offset++];
    
    hop1SignalsPortMask = armData[offset++];
    offset++;
    fhConfig->rxPortHopSignals[0] = ((hop1SignalsPortMask & 0x1) == 1) ? ADI_ADRV9001_FH_HOP_SIGNAL_1 : ADI_ADRV9001_FH_HOP_SIGNAL_2;
    fhConfig->rxPortHopSignals[1] = (((hop1SignalsPortMask >> 1) & 0x1 & 0x1) == 1) ? ADI_ADRV9001_FH_HOP_SIGNAL_1 : ADI_ADRV9001_FH_HOP_SIGNAL_2;
    fhConfig->txPortHopSignals[0] = (((hop1SignalsPortMask >> 2) & 0x1 & 0x1) == 1) ? ADI_ADRV9001_FH_HOP_SIGNAL_1 : ADI_ADRV9001_FH_HOP_SIGNAL_2;
    fhConfig->txPortHopSignals[1] = (((hop1SignalsPortMask >> 3) & 0x1 & 0x1) == 1) ? ADI_ADRV9001_FH_HOP_SIGNAL_1 : ADI_ADRV9001_FH_HOP_SIGNAL_2;

    fhConfig->minRxGainIndex    = armData[offset++];
    fhConfig->maxRxGainIndex    = armData[offset++];
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->minTxAtten_mdB);
    adrv9001_ParseTwoBytes(&offset,   armData, &fhConfig->maxTxAtten_mdB);
    offset += 4u;
    adrv9001_ParseEightBytes(&offset, armData, &fhConfig->minOperatingFrequency_Hz);
    adrv9001_ParseEightBytes(&offset, armData, &fhConfig->maxOperatingFrequency_Hz);
    adrv9001_ParseFourBytes(&offset,  armData, &fhConfig->minFrameDuration_us);
    fhConfig->txAnalogPowerOnFrameDelay = armData[offset++];
    fhConfig->rxZeroIfEnable = armData[offset++];
    offset += 2u;
    /* Get gain setup by pin information */
    if (fhConfig->gainSetupByPin)
    {
        for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
        {
            fhConfig->gainSetupByPinConfig[j].numRxGainTableEntries  = armData[offset++];
        }
        for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
        {
            fhConfig->gainSetupByPinConfig[j].numTxAttenTableEntries = armData[offset++];
        }
        
       tempOffset = offset + 2 * ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES;
        /* Rx and Tx tables */
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
		    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
		    {
			    fhConfig->gainSetupByPinConfig[j].rxGainTable[i] = armData[offset++];
		    }
	    }
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
		    for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES; i++)
		    {
			    adrv9001_ParseTwoBytes(&tempOffset, armData, &fhConfig->gainSetupByPinConfig[j].txAttenTable[i]);
		    }
	    }
        /* Inspect ADRV9001 GPIOs */
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
	    {
		    fhConfig->gainSetupByPinConfig[j].numGainCtrlPins = ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS;
	    }
	    for (j = 0; j < ADI_ADRV9001_NUM_CHANNELS; j++)
            for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS; i++)
            {
	            gainSelectPinSignal = j == 0 ? ADI_ADRV9001_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_0 : ADI_ADRV9001_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_0;;
                ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, (gainSelectPinSignal + i), &(fhConfig->gainSetupByPinConfig[j].gainSelectGpioConfig[i]));
                if (fhConfig->gainSetupByPinConfig[j].gainSelectGpioConfig[i].pin == ADI_ADRV9001_GPIO_UNASSIGNED)
                {
                    fhConfig->gainSetupByPinConfig[j].numGainCtrlPins = i;
                    break;
                }
            }
    }

    ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP, &(fhConfig->hopSignalGpioConfig[0]));
    ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_TABLE_SELECT, &(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[0]));
    
    if (fhConfig->mode == ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2, &(fhConfig->hopSignalGpioConfig[1]));
        ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2_TABLE_SELECT, &(fhConfig->hopTableSelectConfig.hopTableSelectGpioConfig[1]));
    }
    
    /* Inspect table index pins if selected */
    if (fhConfig->tableIndexCtrl == ADI_ADRV9001_TABLEINDEXCTRL_GPIO)
    {
        frequencySelectPinSignal = ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_0;
        fhConfig->numTableIndexPins = ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS;
        for (i = 0; i < ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS; i++)
        {
            ADI_EXPECT(adi_adrv9001_gpio_Inspect, adrv9001, (frequencySelectPinSignal + i), &(fhConfig->tableIndexGpioConfig[i]));
            if (fhConfig->tableIndexGpioConfig[i].pin == ADI_ADRV9001_GPIO_UNASSIGNED)
            {
                fhConfig->numTableIndexPins = i;
                break;
            }
        }
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Static_Configure(adi_adrv9001_Device_t *adrv9001,
                                                     adi_adrv9001_FhMode_e mode,
                                                     adi_adrv9001_FhHopSignal_e hopSignal,
                                                     adi_adrv9001_FhHopTable_e tableId, 
                                                     adi_adrv9001_FhHopFrame_t hopTable[],
                                                     uint32_t hopTableSize)
{
    /* Find address of hop table */
    uint32_t hopTableAddress = 0;
    uint32_t offset = 0;
    uint32_t frequencyIndex = 0;
    uint8_t numHopTableEntries[4u];
    uint32_t hopTableBufferAddress = 0;
	
    /* ARM Data is written directly to ARM memory because FREQ_HOPPING_NUM_BYTES is greater than set buffer size */
#ifndef __KERNEL__
    uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES] = { 0 };
#else
    /*
     * linux stack is not that big which means we need to be carefull. Some archs like arm set
     * Wframe-larger-than=1024
     */
    static uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES];

    memset(&armData, 0, sizeof(armData));
#endif
	if (tableId == ADI_ADRV9001_FHHOPTABLE_A)
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableA1Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferA1Addr;
		}
		else
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableA2Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferA2Addr;
		}
	}
	else
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableB1Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferB1Addr;
		}
		else
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableB2Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferB2Addr;
		}
	}                                                            
    
    adrv9001_LoadFourBytes(&offset, numHopTableEntries, hopTableSize);
    offset = 0;
    for (frequencyIndex = 0; frequencyIndex < hopTableSize; frequencyIndex++)
    {
        adrv9001_LoadFourBytes(&offset, armData, (hopTable[frequencyIndex].hopFrequencyHz & 0xFFFFFFFF));
        adrv9001_LoadFourBytes(&offset, armData, ((hopTable[frequencyIndex].hopFrequencyHz >> 32) & 0xFFFFFFFF));

        adrv9001_LoadFourBytes(&offset, armData, hopTable[frequencyIndex].rx1OffsetFrequencyHz);
        adrv9001_LoadFourBytes(&offset, armData, hopTable[frequencyIndex].rx2OffsetFrequencyHz);
        armData[offset++] = hopTable[frequencyIndex].rx1GainIndex;
	    armData[offset++] = hopTable[frequencyIndex].rx2GainIndex;
        armData[offset++] = hopTable[frequencyIndex].tx1Attenuation_fifthdB;
	    armData[offset++] = hopTable[frequencyIndex].tx2Attenuation_fifthdB;
    }
	
	/* Write the data directly to the ARM memory */
	/* 'offset' is used to represent the exact number of bytes to write to avoid writing over the entire table */
	ADI_EXPECT(adi_adrv9001_arm_Memory_WriteFH, adrv9001, hopSignal, tableId, hopTableAddress, numHopTableEntries, sizeof(numHopTableEntries), hopTableBufferAddress, armData, offset);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Inspect(adi_adrv9001_Device_t *adrv9001,
                                            adi_adrv9001_FhHopSignal_e hopSignal,
                                            adi_adrv9001_FhHopTable_e tableId, 
                                            adi_adrv9001_FhHopFrame_t hopTable[],
                                            uint32_t hopTableSize,
                                            uint32_t *numHopFramesRead)
{
    uint8_t  extData[5] = { 0 };
    uint32_t offset = 0;
    uint32_t frequencyIndex = 0;
    uint32_t hopTableAddress = 0;
    uint32_t numberOfBytesReadback = 0;
    uint8_t  numHopFrequenciesReadbackBlock[4u] = { 0 };
    uint32_t hopTableBufferAddress = 0;
    uint32_t numHopFrequenciesReadback = 0;
    uint32_t hopFrequencyHz_LSB = 0;
    uint32_t hopFrequencyHz_MSB = 0;

#ifndef __KERNEL__
    uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES] = { 0 };
#else
    /*
     * linux stack is not that big which means we need to be carefull. Some archs like arm set
     * Wframe-larger-than=1024
     */
    static uint8_t armData[FREQ_HOPPING_MAX_NUM_BYTES];

    memset(&armData, 0, sizeof(armData));
#endif
    
	if (tableId == ADI_ADRV9001_FHHOPTABLE_A)
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableA1Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferA1Addr;
		}
		else
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableA2Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferA2Addr;
		}
	}
	else
	{
		if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableB1Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferB1Addr;
		}
		else
		{
			hopTableAddress = adrv9001->devStateInfo.fhHopTableB2Addr;
			hopTableBufferAddress = adrv9001->devStateInfo.fhHopTableBufferB2Addr;
		}
	}             

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Inspect_Validate, adrv9001, hopSignal, tableId, hopTable, hopTableSize);
    /* Even though we are going to read directly from ARM memory, we can still use the GET protocol to 
       tell ARM how many bytes we will read. ARM will return an error if the read size is invalid
    */
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    /* Construct payload and send get command */
    extData[0] = 0; // Reset offset
    extData[1] = OBJID_GO_GET_FH_HOP_TABLE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        OBJID_GO_GET_FH_HOP_TABLE,
                                        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        ADI_ADRV9001_DEFAULT_INTERVAL_US);
    
    /* First read number of frequencies in hop table */
    offset = 0;
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, hopTableAddress, numHopFrequenciesReadbackBlock, sizeof(numHopFrequenciesReadbackBlock), false);
    adrv9001_ParseFourBytes(&offset, numHopFrequenciesReadbackBlock, &numHopFrequenciesReadback);
    if (numHopFramesRead != NULL)
    {
        *numHopFramesRead = numHopFrequenciesReadback;
    }
    /* Read back the hopping table from ARM memory based on number of HOP frequencies */
    numberOfBytesReadback = (numHopFrequenciesReadback * sizeof(adrv9001_FhHopFrame_t));
    if (numberOfBytesReadback > FREQ_HOPPING_MAX_NUM_BYTES)
    {
        numberOfBytesReadback = FREQ_HOPPING_MAX_NUM_BYTES;
    }
    if (numHopFrequenciesReadback > hopTableSize)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            hopTable,
            "Number of hop table entries readback exceeds size of input buffer");

        ADI_API_RETURN(adrv9001);
    }
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, hopTableBufferAddress, armData, numberOfBytesReadback, false);
    /* Parse hop table */
    offset = 0;
    for (frequencyIndex = 0; frequencyIndex < numHopFrequenciesReadback; frequencyIndex++)
    {
        adrv9001_ParseFourBytes(&offset, armData, &hopFrequencyHz_LSB);
        adrv9001_ParseFourBytes(&offset, armData, &hopFrequencyHz_MSB);
        hopTable[frequencyIndex].hopFrequencyHz = (((uint64_t)hopFrequencyHz_MSB << 32) | (uint64_t)hopFrequencyHz_LSB);

        adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopTable[frequencyIndex].rx1OffsetFrequencyHz));
        adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopTable[frequencyIndex].rx2OffsetFrequencyHz));
        hopTable[frequencyIndex].rx1GainIndex = armData[offset++];
	    hopTable[frequencyIndex].rx2GainIndex = armData[offset++];
	    hopTable[frequencyIndex].tx1Attenuation_fifthdB = armData[offset++];
	    hopTable[frequencyIndex].tx2Attenuation_fifthdB = armData[offset++];
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_fh_HopTable_Set(adi_adrv9001_Device_t *adrv9001,
                                        adi_adrv9001_FhHopSignal_e hopSignal,
                                        adi_adrv9001_FhHopTable_e tableId)
{
    /* Construct SET payload and send command */
    uint8_t extData[5] = { 0,
                           ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SELECT,
                           (uint8_t )tableId,
                           (uint8_t )hopSignal};

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
                                        adi_adrv9001_FhHopSignal_e hopSignal,
                                        adi_adrv9001_FhHopTable_e *tableId)
{
    uint8_t armData = sizeof(uint8_t);
    /* Construct SET payload and send command */
    uint8_t extData[5] = { 0,
                           OBJID_GO_GET_FH_HOP_TABLE_SELECT,
                           (uint8_t)hopSignal
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
                                        OBJID_GO_GET_FH_HOP_TABLE_SELECT,
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
    uint32_t hopFrequencyHz_LSB = 0;
    uint32_t hopFrequencyHz_MSB = 0;

    /* Check if FH is enabled in device profile */
    ADI_FH_CHECK_FH_ENABLED(adrv9001);
    ADI_NULL_PTR_RETURN(&adrv9001->common, hopFrame);
    ADI_RANGE_CHECK(adrv9001, frameIndex, ADI_ADRV9001_FHFRAMEINDEX_CURRENT_FRAME, ADI_ADRV9001_FHFRAMEINDEX_NEXT_FRAME);
    
    /* Write the size to the GET buffer */
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData));
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(uint32_t), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* Construct the GET payload and send the command */
    extData[0] =  0;   /* Channel Mask; unused for this command */
    extData[1] = OBJID_GO_GET_FREQ_HOP_FRAME_INFO;
    extData[2] = frameIndex;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_GET_OPCODE, extData, sizeof(extData));

    /* check the command status for timeout */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        ADRV9001_ARM_GET_OPCODE,
                                        OBJID_GO_GET_FREQ_HOP_FRAME_INFO,
                                        ADI_ADRV9001_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_READARMCFG_INTERVAL_US);

    /* Read config data from GET buffer */
    offset = 0; // Reset offset
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET, armData, sizeof(armData), false);
    adrv9001_ParseFourBytes(&offset, armData, &hopFrequencyHz_LSB);
    adrv9001_ParseFourBytes(&offset, armData, &hopFrequencyHz_MSB);
    hopFrame->hopFrequencyHz = (((uint64_t)hopFrequencyHz_MSB << 32) | (uint64_t)hopFrequencyHz_LSB);

    adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopFrame->rx1OffsetFrequencyHz));
    adrv9001_ParseFourBytes(&offset, armData, (uint32_t *)(&hopFrame->rx2OffsetFrequencyHz));
    hopFrame->rx1GainIndex = armData[offset++];
	hopFrame->rx2GainIndex = armData[offset++];
	hopFrame->tx1Attenuation_fifthdB = armData[offset++];
	hopFrame->tx2Attenuation_fifthdB = armData[offset++];
    ADI_API_RETURN(adrv9001);
}                                                              

int32_t adi_adrv9001_fh_Hop(adi_adrv9001_Device_t *adrv9001, 
                               adi_adrv9001_FhHopSignal_e hopSignal)
{
    /* Flip the hop signal */
    uint8_t BbicHop = 0u;
    if (hopSignal == ADI_ADRV9001_FH_HOP_SIGNAL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop1_Get, adrv9001, &BbicHop);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop1_Set, adrv9001, !BbicHop);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop2_Get, adrv9001, &BbicHop);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_BbicHop2_Set, adrv9001, !BbicHop);
    }

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_fh_NumberOfHops_Get(adi_adrv9001_Device_t *adrv9001,
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

static __maybe_unused int32_t adi_adrv9001_fh_HopTable_Dynamic_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                  adi_adrv9001_FhMode_e mode,
                                                                                  adi_adrv9001_FhHopSignal_e hopSignal,
                                                                                  adi_adrv9001_FhHopFrame_t hopTable[],
                                                                                  uint32_t tableSize,
                                                                                  adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                                                  uint8_t *spiPackedFhTable,
                                                                                  uint32_t length)
{
    uint32_t frequencyIndex = 0;
    uint32_t bytesPerTable = 0;
    uint32_t totalSpiPackedTableSize = 0;
    uint32_t totalHopTablePairs = 0;
    uint32_t numberOfHops = 0;

    ADI_RANGE_CHECK(adrv9001, mode, ADI_ADRV9001_FHMODE_LO_MUX_PREPROCESS, ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP);
    ADI_RANGE_CHECK(adrv9001, hopSignal, ADI_ADRV9001_FH_HOP_SIGNAL_1, ADI_ADRV9001_FH_HOP_SIGNAL_2);
    if (ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP != mode)
    {
        if (ADI_ADRV9001_FH_HOP_SIGNAL_2 == hopSignal)
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                mode,
                "FH mode must be dual hop for hopSignal to be HOP_2 ");
        }
        
    }

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

    totalHopTablePairs = tableSize / numberOfHops;
    ADI_EXPECT(adi_adrv9001_fh_HopTable_BytesPerTable_Get, adrv9001, numberHopsPerDynamicLoad, &bytesPerTable);
    totalSpiPackedTableSize = bytesPerTable * totalHopTablePairs;
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
    uint8_t hopTableSize[4] = { 0 };
    uint32_t offset = 0;
    uint32_t dataIdx = 0;
    uint32_t ADDR_ARM_DMA_ADDR[4] = { ADRV9001_ADDR_ARM_DMA_ADDR3, ADRV9001_ADDR_ARM_DMA_ADDR2, ADRV9001_ADDR_ARM_DMA_ADDR1, ADRV9001_ADDR_ARM_DMA_ADDR0 };
    uint32_t ADDR_ARM_DMA_DATA[4] = { ADRV9001_ADDR_ARM_DMA_DATA3, ADRV9001_ADDR_ARM_DMA_DATA2, ADRV9001_ADDR_ARM_DMA_DATA1, ADRV9001_ADDR_ARM_DMA_DATA0 };
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADRV9001_SPIREADBYTEDMA(adrv9001, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, &regVal);

    adrv9001_LoadFourBytes(&offset, hopTableSize, numberOfHops);

    /* Configure ADRV9001 DMA control register */
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADRV9001_ADDR_ARM_DMA_CTL, 0x4A, ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < 4; i++)
    {
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_ADDR[i], addrArray[i], ADRV9001_SPI_WRITE_POLARITY);
    }

    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[0], hopTableSize[3], ADRV9001_SPI_WRITE_POLARITY);
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[1], hopTableSize[2], ADRV9001_SPI_WRITE_POLARITY);
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[2], hopTableSize[1], ADRV9001_SPI_WRITE_POLARITY);
    adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_DATA[3], hopTableSize[0], ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < 4; i++)
    {
        adi_adrv9001_HopTable_Spi_DataPack(spiPackedFhTable, numWrBytes, ADDR_ARM_DMA_ADDR[i], addrArray[i + 4], ADRV9001_SPI_WRITE_POLARITY);
    }

    for (i = 0; i < numberOfHops; i++)
    {
        for (j = 0; j < sizeof(adrv9001_FhHopFrame_t); j += 4)
        {
            addrIdx = j % 4;
            dataIdx = (i * sizeof(adrv9001_FhHopFrame_t)) + j;
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
                                                      adi_adrv9001_FhMode_e mode,
                                                      adi_adrv9001_FhHopSignal_e hopSignal,
                                                      adi_adrv9001_FhHopFrame_t hopTable[],
                                                      uint32_t hopTableSize,
                                                      adi_adrv9001_FhPerDynamicLoad_e numberHopsPerDynamicLoad,
                                                      uint8_t spiPackedFhTable[],
                                                      uint32_t length)
{
    uint8_t addrArray_A[8] = { 0 };
    uint8_t addrArray_B[8] = { 0 };
    uint8_t bitmSwInt_A = 0;
    uint8_t bitmSwInt_B = 0;
    uint32_t armAddr_A = 0;
    uint32_t armAddr_B = 0;
    uint32_t hopTableBufferAddress_A = 0;
    uint32_t hopTableBufferAddress_B = 0;
    uint8_t fhTable_A[sizeof(adrv9001_FhHopFrame_t) * 8] = { 0 };
    uint8_t fhTable_B[sizeof(adrv9001_FhHopFrame_t) * 8] = { 0 };
    uint32_t numWrBytes = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t offset = 0;
    uint32_t numberOfHops = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_fh_HopTable_Dynamic_Configure_Validate, adrv9001, mode, hopSignal, hopTable, hopTableSize, numberHopsPerDynamicLoad, spiPackedFhTable, length);
    ADI_EXPECT(adi_adrv9001_fh_NumberOfHops_Get, adrv9001, numberHopsPerDynamicLoad, &numberOfHops);

    if (ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP == mode)
    {
        if (ADI_ADRV9001_FH_HOP_SIGNAL_1 == hopSignal)
        {
            bitmSwInt_A = 0x1;
	        hopTableBufferAddress_A = adrv9001->devStateInfo.fhHopTableBufferA1Addr; 
	        armAddr_A = adrv9001->devStateInfo.fhHopTableA1Addr;

            bitmSwInt_B = 0x80; 
	        hopTableBufferAddress_B = adrv9001->devStateInfo.fhHopTableBufferB1Addr;
	        armAddr_B = adrv9001->devStateInfo.fhHopTableB1Addr;
        }
        else //ADI_ADRV9001_FH_HOP_SIGNAL_2
        {
            bitmSwInt_A = 0x2;
	        hopTableBufferAddress_A = adrv9001->devStateInfo.fhHopTableBufferA2Addr;
	        armAddr_A = adrv9001->devStateInfo.fhHopTableA2Addr;

            bitmSwInt_B = 0x40;
	        hopTableBufferAddress_B = adrv9001->devStateInfo.fhHopTableBufferB2Addr;
	        armAddr_B = adrv9001->devStateInfo.fhHopTableB2Addr;
        }
    }
    else
    {
        bitmSwInt_A = 0x1; 
	    hopTableBufferAddress_A = adrv9001->devStateInfo.fhHopTableBufferA1Addr; 
	    armAddr_A = adrv9001->devStateInfo.fhHopTableA1Addr;

        bitmSwInt_B = 0x2; 
	    hopTableBufferAddress_B = adrv9001->devStateInfo.fhHopTableBufferB1Addr; 
	    armAddr_B = adrv9001->devStateInfo.fhHopTableB1Addr;
    }

    armAddr_A >>= 2;
    hopTableBufferAddress_A >>= 2;
    addrArray_A[0] = (uint8_t)((armAddr_A >> 24) & 0x000000FF);
    addrArray_A[1] = (uint8_t)((armAddr_A >> 16) & 0x000000FF);
    addrArray_A[2] = (uint8_t)((armAddr_A >> 8) & 0x000000FF);
    addrArray_A[3] = (uint8_t)(armAddr_A & 0x000000FF);
	addrArray_A[4] = (uint8_t)((hopTableBufferAddress_A >> 24) & 0x000000FF);
	addrArray_A[5] = (uint8_t)((hopTableBufferAddress_A >> 16) & 0x000000FF);
	addrArray_A[6] = (uint8_t)((hopTableBufferAddress_A >> 8) & 0x000000FF);
	addrArray_A[7] = (uint8_t)(hopTableBufferAddress_A & 0x000000FF);

    armAddr_B >>= 2;
    hopTableBufferAddress_B >>= 2;
    addrArray_B[0] = (uint8_t)((armAddr_B >> 24) & 0x000000FF);
    addrArray_B[1] = (uint8_t)((armAddr_B >> 16) & 0x000000FF);
    addrArray_B[2] = (uint8_t)((armAddr_B >> 8) & 0x000000FF);
    addrArray_B[3] = (uint8_t)(armAddr_B & 0x000000FF);
	addrArray_B[4] = (uint8_t)((hopTableBufferAddress_B >> 24) & 0x000000FF);
	addrArray_B[5] = (uint8_t)((hopTableBufferAddress_B >> 16) & 0x000000FF);
	addrArray_B[6] = (uint8_t)((hopTableBufferAddress_B >> 8) & 0x000000FF);
	addrArray_B[7] = (uint8_t)(hopTableBufferAddress_B & 0x000000FF);

    for (i = 0; i < hopTableSize; i += (2 * numberOfHops))
    {
        offset = 0;
        for (j = 0; j < numberOfHops; j++)
        {
            adrv9001_LoadFourBytes(&offset, fhTable_A, (hopTable[i + j].hopFrequencyHz & 0xFFFFFFFF));
            adrv9001_LoadFourBytes(&offset, fhTable_A, ((hopTable[i + j].hopFrequencyHz >> 32) & 0xFFFFFFFF));
            adrv9001_LoadFourBytes(&offset, fhTable_A, hopTable[i + j].rx1OffsetFrequencyHz);
            adrv9001_LoadFourBytes(&offset, fhTable_A, hopTable[i + j].rx2OffsetFrequencyHz);
            fhTable_A[offset++] = hopTable[i + j].rx1GainIndex;
	        fhTable_A[offset++] = hopTable[i + j].rx2GainIndex;
            adrv9001_LoadTwoBytes(&offset, fhTable_A, hopTable[i + j].tx1Attenuation_fifthdB);
	        adrv9001_LoadTwoBytes(&offset, fhTable_A, hopTable[i + j].tx2Attenuation_fifthdB);
        }
        ADI_EXPECT(adrv9001_HopTable_Spi_Pack, adrv9001, addrArray_A, fhTable_A, numberOfHops, &numWrBytes, bitmSwInt_A, spiPackedFhTable);
    
        offset = 0;
        for (j = numberOfHops; j < (2 * numberOfHops); j++)
        {
            adrv9001_LoadFourBytes(&offset, fhTable_B, (hopTable[i + j].hopFrequencyHz & 0xFFFFFFFF));
            adrv9001_LoadFourBytes(&offset, fhTable_B, ((hopTable[i + j].hopFrequencyHz >> 32) & 0xFFFFFFFF));
            adrv9001_LoadFourBytes(&offset, fhTable_B, hopTable[i + j].rx1OffsetFrequencyHz);
            adrv9001_LoadFourBytes(&offset, fhTable_B, hopTable[i + j].rx2OffsetFrequencyHz);
            fhTable_B[offset++] = hopTable[i + j].rx1GainIndex;
	        fhTable_B[offset++] = hopTable[i + j].rx2GainIndex;
            adrv9001_LoadTwoBytes(&offset, fhTable_B, hopTable[i + j].tx1Attenuation_fifthdB);
	        adrv9001_LoadTwoBytes(&offset, fhTable_B, hopTable[i + j].tx2Attenuation_fifthdB);
        }
        ADI_EXPECT(adrv9001_HopTable_Spi_Pack, adrv9001, addrArray_B, fhTable_B, numberOfHops, &numWrBytes, bitmSwInt_B, spiPackedFhTable);
    }

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_fh_HopTable_BytesPerTable_Get_Validate(adi_adrv9001_Device_t *adrv9001,
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
    
    payloadBytes = ((sizeof(adrv9001_FhHopFrame_t) * numberOfHops) + fhBytesLength) * 3;
    *bytesPerTable = payloadBytes + spiPackBytesPerTable;

    ADI_API_RETURN(adrv9001);
}
