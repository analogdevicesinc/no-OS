/**
* \file
* \brief Contains Sync Detection features related function implementation defined in
* adi_adrv910x_syncdetect.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2021 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "adi_adrv910x_user.h"
#include "adi_adrv910x_syncdetect.h"
#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_cals.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_arm_error_mapping.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_validators.h"
#include "adrv910x_bf.h"

#include "object_ids.h"

static __maybe_unused int32_t adi_adrv910x_syncDetect_Vector_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                           adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t *syncDetectVector)
{
    static uint16_t VECTOR_MASK_MAX = 0x3FFF;
    ADI_NULL_PTR_RETURN(&device->common, syncDetectVector);
    ADI_RANGE_CHECK(device, syncDetectVector->vectorMask, 0x1, VECTOR_MASK_MAX);
	ADI_RANGE_CHECK(device, syncDetectVector->qMask, 0x0, VECTOR_MASK_MAX);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_Vector_Configure(adi_adrv910x_Device_t *device,
                                                                            adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t *syncDetectVector)
{
	uint8_t armData[166] = { 0 };
	uint8_t extData[5] = { 0 };
	uint32_t offset = 4;
	uint32_t i = 0;
	uint32_t bit = 0;
	uint32_t nibSel = 0;
	uint32_t vectorConverted[SYNC_VECTOR_MAX] = { 0 };
	uint8_t vectorCrumb = 0;
	uint8_t vectorNibble = 0;
	static const uint8_t MAX_NIBBLE_PER_VECTOR = 12;

	ADI_PERFORM_VALIDATION(adi_adrv910x_syncDetect_Vector_Configure_Validate, device, syncDetectVector);

	extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
	extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_SYNC_DETECT_VECTOR;

	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		/* Convert dbits to symbols*/
		for (nibSel = 0; nibSel < MAX_NIBBLE_PER_VECTOR; nibSel++)
		{
			vectorNibble = (syncDetectVector->patternVect[i] >> (nibSel * 4)) & 0xF;
			/* bit3 and bit1 are extracted in each nibble and combined for conversion */
			/* 0x5: 0x00, 0x7:0x1, 0xD:0x2, 0xF:0x3 */
			vectorCrumb = ((vectorNibble & 0x8) >> 2) | ((vectorNibble & 0x2) >> 1);
			vectorConverted[i] |= (vectorCrumb << (nibSel * 2));
		}
		adrv910x_LoadFourBytes(&offset, armData, vectorConverted[i]);
	}
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		adrv910x_LoadFourBytes(&offset, armData, syncDetectVector->zeroMask[i]);
	}
	adrv910x_LoadTwoBytes(&offset, armData, syncDetectVector->vectorMask);
	adrv910x_LoadTwoBytes(&offset, armData, syncDetectVector->qMask);
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		/* Calculate patternSum : converting 0s to 1s and 1s to -1s in patternVect and summing*/
		for (bit = 0; bit < 24; bit++) 
		{
			if((syncDetectVector->zeroMask[i] >> bit) & 1u) 
			{
				continue; //skip the bit if zeroMask is set
			}
			armData[offset] += ((vectorConverted[i] >> bit) & 1u) ? -1 : 1;
		}
		offset++;
	}
	//offset += 2; //pad to 4 byte boundary
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		armData[offset++] = syncDetectVector->patternWeight[i];
	}
	//offset += 2; //pad to 4 byte boundary
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		/* Calculate zeroSum: summing the bits in zeroFlag */
		for (bit = 0; bit < 24; bit++) {
			armData[offset] += ((syncDetectVector->zeroMask[i] >> bit) & 1u) ? 1 : 0;
		}
		offset++;
	}
	//offset += 2; //pad to 4 byte boundary

	ADI_MUTEX_ACQUIRE(device);
	
	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, extData, sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		(uint8_t)ADRV910X_ARM_SET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_READARMCFG_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_READARMCFG_INTERVAL_US);
	
	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_syncDetect_Vector_Inspect_Validate(adi_adrv910x_Device_t *device,
																								adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t *syncDetectVector)
{
	ADI_NULL_PTR_RETURN(&device->common, syncDetectVector);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_Vector_Inspect(adi_adrv910x_Device_t *device,
                                                                          adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t *syncDetectVector)
{
	uint8_t armReadBack[166] = { 0 };
	uint8_t extData[5] = { 0 };
	uint32_t offset = 0;
	uint32_t i = 0;
	uint32_t packedDibits = 0;
	uint32_t nibSel = 0;
	static const uint8_t MAX_NIBBLE_PER_VECTOR = 12;
	
	ADI_PERFORM_VALIDATION(adi_adrv910x_syncDetect_Vector_Inspect_Validate, device, syncDetectVector);	

	extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
	extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_SYNC_DETECT_VECTOR;
    
	ADI_MUTEX_ACQUIRE(device);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		(uint8_t)ADRV910X_ARM_GET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

	/* read the ARM memory to get syncSearch status */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		armReadBack,
		sizeof(armReadBack),
		false,
		ADI_PS1)

	for(i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		adrv910x_ParseFourBytes(&offset, armReadBack, &packedDibits);
		syncDetectVector->patternVect[i] = 0ULL;

		if (packedDibits)
		{
			for (nibSel = 0; nibSel < MAX_NIBBLE_PER_VECTOR; ++nibSel) 
			{
				uint32_t dibit = (packedDibits >> (nibSel * 2U)) & 0x3U; // 2-bit value
				// Recreate the original symbol nibble in {0x5, 0x7, 0xD, 0xF}
				uint8_t symbolNibble = (uint8_t)(0x5U | ((dibit & 0x2U) << 2) | ((dibit & 0x1U) << 1));
				syncDetectVector->patternVect[i] |= ((uint64_t)symbolNibble) << (nibSel * 4U);
			}
		}
	}
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		adrv910x_ParseFourBytes(&offset, armReadBack, &syncDetectVector->zeroMask[i]);
	}
	adrv910x_ParseTwoBytes(&offset, armReadBack, &syncDetectVector->vectorMask);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &syncDetectVector->qMask);
	for(i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		/* skip the pattern sum*/
		offset += 1;
	}
	//offset += 2; //pad to 4 byte boundary
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		syncDetectVector->patternWeight[i] = armReadBack[offset++];
	}
	//offset += 2; //pad to 4 byte boundary
	for (i = 0; i < SYNC_VECTOR_MAX; i++)
	{
		/* skip the zero sum*/
		offset += 1;
	}
	//offset += 2; //pad to 4 byte boundary
	
	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_syncDetect_SyncSearch_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                              adi_adrv910x_SyncDetect_SyncSearchCfg_t *syncSearchCfg)
{
	static const int32_t MIN_POWER_THRESHOLD = (int32_t)(-256*(1<<23)); // -256 in S9.23
	static const uint32_t MAX_DETECT_MULTIPLIER = 0x7FFFFFFF;
	
	ADI_NULL_PTR_RETURN(&device->common, syncSearchCfg);
    ADI_RANGE_CHECK(device, syncSearchCfg->pathDelay, 0, 2047);
	ADI_RANGE_CHECK(device, syncSearchCfg->powerThreshold_dBm, MIN_POWER_THRESHOLD, 0);
	ADI_RANGE_CHECK(device, syncSearchCfg->detectMultiplier, 0, MAX_DETECT_MULTIPLIER);
	ADI_RANGE_CHECK(device, syncSearchCfg->preCount, 0, 65535);
	ADI_RANGE_CHECK(device, syncSearchCfg->postCount, 0, 65535);
    ADI_RANGE_CHECK(device, syncSearchCfg->algoMode, ADI_ADRV910X_SYNCDETECT_MODE_DMR, ADI_ADRV910X_SYNCDETECT_MODE_TETRA1);
	ADI_RANGE_CHECK(device, syncSearchCfg->dataPath, ADI_ADRV910X_SYNCDETECT_DP1, ADI_ADRV910X_SYNCDETECT_DP2);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_SyncSearch_Configure(adi_adrv910x_Device_t *device,
                                                                               adi_adrv910x_SyncDetect_SyncSearchCfg_t *syncSearchCfg)
{
    uint8_t armData[28] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 4;

    ADI_PERFORM_VALIDATION(adi_adrv910x_syncDetect_SyncSearch_Configure_Validate, device, syncSearchCfg);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
    extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_SYNC_DETECT;

    adrv910x_LoadFourBytes(&offset, armData, syncSearchCfg->pathDelay);
	adrv910x_LoadFourBytes(&offset, armData, syncSearchCfg->powerThreshold_dBm);
	adrv910x_LoadFourBytes(&offset, armData, syncSearchCfg->detectMultiplier);
    adrv910x_LoadTwoBytes(&offset, armData, syncSearchCfg->preCount);
    adrv910x_LoadTwoBytes(&offset, armData, syncSearchCfg->postCount);
    armData[offset++] = (uint8_t)syncSearchCfg->algoMode;
	offset += 3; //pad to 4 byte boundary
	armData[offset++] = (uint8_t)syncSearchCfg->dataPath;
	offset += 3; //pad to 4 byte boundary

	ADI_MUTEX_ACQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, extData, sizeof(extData));
	
	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		(uint8_t)ADRV910X_ARM_SET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_READARMCFG_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_READARMCFG_INTERVAL_US);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_SyncSearch_Inspect(adi_adrv910x_Device_t *device,
	adi_adrv910x_SyncDetect_SyncSearchCfg_t *syncSearchCfg)
{
	uint8_t armReadBack[28] = { 0 };
	uint8_t extData[5] = { 0 };
	uint32_t offset = 0;
	
	ADI_ENTRY_PTR_EXPECT(device, syncSearchCfg);	

	extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
	extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_SYNC_DETECT;
    
	ADI_MUTEX_ACQUIRE(device);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		(uint8_t)ADRV910X_ARM_GET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

	/* read the ARM memory to get syncSearch status */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		armReadBack,
		sizeof(armReadBack),
		false,
		ADI_PS1)

	adrv910x_ParseFourBytes(&offset, armReadBack, &syncSearchCfg->pathDelay);
	adrv910x_ParseFourBytes(&offset, armReadBack, &syncSearchCfg->powerThreshold_dBm);
	adrv910x_ParseFourBytes(&offset, armReadBack, &syncSearchCfg->detectMultiplier);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &syncSearchCfg->preCount);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &syncSearchCfg->postCount);
	syncSearchCfg->algoMode = (adi_adrv910x_SyncDetect_Mode_e)armReadBack[offset++];
	offset += 3; //pad to 4 byte boundary
	syncSearchCfg->dataPath = (syncDetectionDataPath_e)armReadBack[offset++];
	offset += 3; //pad to 4 byte boundary

	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_Set_Validate(adi_adrv910x_Device_t *device,
																																  bool syncDetectionEnable, adi_adrv910x_GpioPin_e detectedPin){
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_RANGE_CHECK(device, syncDetectionEnable, false, true);
	ADI_RANGE_CHECK(device, detectedPin, ADI_ADRV910X_GPIO_UNASSIGNED, ADI_ADRV910X_GPIO_DIGITAL_15);

    /* Validate state is CALIBRATED, PRIMED, or RF_ENABLED */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(ADI_CHANNEL_2, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_PRIMED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
		    ADI_CHANNEL_2,
            "RxNB must be in CALIBRATED, PRIMED, or RF_ENABLED state");
            ADI_API_RETURN(device);
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_Set(adi_adrv910x_Device_t *device,
																					bool syncDetectionEnable, adi_adrv910x_GpioPin_e detectedPin)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[2] = { 0 };

	ADI_PERFORM_VALIDATION(adi_adrv910x_syncDetect_ContinuousSyncDetect_Set_Validate, device, syncDetectionEnable, detectedPin);
    
    armData[0] = (uint8_t)syncDetectionEnable;  // 1: Enabled; 0: Disabled  
	armData[1] = (uint8_t)detectedPin - 1; //PintoMailbox Conversion due to enum

	if (detectedPin != ADI_ADRV910X_GPIO_UNASSIGNED)
	{
		adi_adrv910x_GpioCfg_t gpioCfg = {
			.pin = detectedPin,
			.polarity = ADI_ADRV910X_GPIO_POLARITY_NORMAL,
			.master = ADI_ADRV910X_GPIO_MASTER_ADRV910X
		};
		ADI_EXPECT(adi_adrv910x_gpio_Ps1_Configure, device, ADI_ADRV910X_GPIO_SIGNAL_CONTINUOUS_SYNC_DETECTION, &gpioCfg);
	}

	ADI_MUTEX_ACQUIRE(device);

    /* Write Continuous Sync Detection Enable to ARM mailbox*/
	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    
    extData[1] = OBJID_GS_CONTINUOUS_SYNC_DETECTION;
    
	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    
    /* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
    (uint8_t)ADRV910X_ARM_SET_OPCODE,
    extData[1],
		(uint32_t)ADI_ADRV910X_READARMCFG_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_READARMCFG_INTERVAL_US);
    
    ADI_MUTEX_RELEASE(device);

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_Get_Validate(adi_adrv910x_Device_t *device,
																																  bool *syncDetectionEnable, adi_adrv910x_GpioPin_e *detectedPin)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Validate state is CALIBRATED, PRIMED, or RF_ENABLED */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(ADI_CHANNEL_2, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_PRIMED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
		    ADI_CHANNEL_2,
		    "RxNB must be in CALIBRATED, PRIMED, or RF_ENABLED state");
            ADI_ERROR_RETURN(device->common.error.newAction);
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_Get(adi_adrv910x_Device_t *device,
																					bool *syncDetectionEnable, adi_adrv910x_GpioPin_e *detectedPin)
{
    uint8_t armReadBack[4] = { 0 };	
    uint8_t extData[2] = { 0 };

	ADI_PERFORM_VALIDATION(adi_adrv910x_syncDetect_ContinuousSyncDetect_Get_Validate, device, syncDetectionEnable, detectedPin);

    extData[1] = OBJID_GS_CONTINUOUS_SYNC_DETECTION;

    ADI_MUTEX_ACQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get continuous sync detect enable */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		false, ADI_PS1);
		
	*syncDetectionEnable = (bool)(armReadBack[0] & 0x1);
	if (detectedPin != NULL)
	{
		adi_adrv910x_GpioCfg_t gpioCfg = {0};

		ADI_EXPECT(adi_adrv910x_gpio_Ps1_Inspect, device, ADI_ADRV910X_GPIO_SIGNAL_CONTINUOUS_SYNC_DETECTION, &gpioCfg);
		*detectedPin = gpioCfg.pin;
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_Status_Get(adi_adrv910x_Device_t *adrv910x,
																						   adi_adrv910x_SyncDetect_SyncDetectionResult_t *syncDetectionResult)
{
	uint8_t extData[2] = {0};

	ADI_NULL_PTR_RETURN(&adrv910x->common, syncDetectionResult);

	extData[1] = OBJID_GO_SYNCDETECT_DATA_GET;

	ADI_MUTEX_ACQUIRE(adrv910x);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
										(uint8_t)ADRV910X_ARM_GET_OPCODE,
										extData[1],
										(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
										(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get continuous sync detect enable */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
			   adrv910x,
			   ADRV910X_ADDR_ARM_MAILBOX_GET,
			   (uint8_t*)syncDetectionResult,
			   sizeof(adi_adrv910x_SyncDetect_SyncDetectionResult_t),
			   false, ADI_PS1);
	
	/* send Ack if in sync */
	if (syncDetectionResult->syncDetected)
	{
		extData[1] = OBJID_GO_SYNCDETECT_DATA_READ_DONE;
		ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));
		
		ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
			(uint8_t)ADRV910X_ARM_GET_OPCODE,
			extData[1],
			(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
			(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);
	}

	ADI_MUTEX_RELEASE(adrv910x);
	ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_RxnbNco2Ftw_Set(adi_adrv910x_Device_t *adrv910x, uint32_t ncoControlWord)
{
	uint8_t nco70 = (ncoControlWord & 0xFF);
	uint8_t nco158 = ((ncoControlWord >> 8) & 0xFF);
	uint8_t nco2316 = ((ncoControlWord >> 16) & 0xFF);
	uint8_t nco3124 = ((ncoControlWord >> 24) & 0xFF);
	ADI_NULL_DEVICE_PTR_RETURN(adrv910x);
	ADI_MUTEX_ACQUIRE(adrv910x);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNbdemNco50Enable_Set, adrv910x, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNco2Ftw70_Set, adrv910x, nco70);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNco2Ftw158_Set, adrv910x, nco158);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNco2Ftw2316_Set, adrv910x, nco2316);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNco2Ftw3124_Set, adrv910x, nco3124);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxNco2FtwUpdate_Set, adrv910x, 0x1);
	ADI_MUTEX_RELEASE(adrv910x);
	ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_syncDetect_ContinuousSyncDetect_RxnbResampPhase_Set(adi_adrv910x_Device_t *adrv910x, int16_t resampleInputI, int16_t resampleInputQ)
{
	ADI_NULL_DEVICE_PTR_RETURN(adrv910x);
	ADI_MUTEX_ACQUIRE(adrv910x);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxnbResampEn_Set, adrv910x, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxnbResampPhaseI_Set, adrv910x, resampleInputI);
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_RxnbResampPhaseQ_Set, adrv910x, resampleInputQ);
	ADI_MUTEX_RELEASE(adrv910x);
	ADI_API_RETURN(adrv910x);
}