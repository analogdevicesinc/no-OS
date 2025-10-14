/**
 * \file
 * \brief Contains ARM features related function implementation defined in
 * adi_adrv910x_arm.h
 * 
 * Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */
#include "adrv910x_arm.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_user.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_arm_error_mapping.h"
#include "object_ids.h"

static uint32_t Crc32(const uint8_t buf[], uint32_t bufLen);

#ifndef NEVIS_PS2

int32_t adi_adrv910x_arm_AhbSpiBridge_Enable(adi_adrv910x_Device_t *device)
{
	uint8_t ahbSpiReg = 0;

	ADI_ENTRY_EXPECT(device);
	ADI_MUTEX_AQUIRE(device);
	ADRV910X_SPIREADBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_AHB_SPI_BRIDGE, &ahbSpiReg);

	ahbSpiReg = ahbSpiReg | ADRV910X_AHB_SPI_BRIDGE_ENABLE;
	ADRV910X_SPIWRITEBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_AHB_SPI_BRIDGE, ahbSpiReg);
	
	ADRV910X_SPIREADBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_SYS2_AHB_SPI_BRIDGE, &ahbSpiReg);

	ahbSpiReg = ahbSpiReg | ADRV910X_AHB_SPI_BRIDGE_ENABLE;
	ADRV910X_SPIWRITEBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_SYS2_AHB_SPI_BRIDGE, ahbSpiReg);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_AhbSpiBridge_Disable(adi_adrv910x_Device_t *device)
{
	uint8_t ahbSpiReg = 0;

	ADI_ENTRY_EXPECT(device);
	ADI_MUTEX_AQUIRE(device);
	ADRV910X_SPIREADBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_AHB_SPI_BRIDGE, &ahbSpiReg);

	ahbSpiReg = (ahbSpiReg & (~ADRV910X_AC1_ARM_M3_RUN));
	ADRV910X_SPIWRITEBYTE(device, "AHB_SPI_BRIDGE", ADRV910X_ADDR_AHB_SPI_BRIDGE, ahbSpiReg);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Image_Write(adi_adrv910x_Device_t *device, uint32_t byteOffset, const uint8_t binary[], uint32_t byteCount, adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode, adi_common_Processor_Subsystem_e PS)
{
	uint8_t stackPtr[4] = { 0 };
	uint8_t bootAddr[4] = { 0 };
	uint32_t i = 0;
	uint16_t stBase, entry;

	ADI_ENTRY_PTR_EXPECT(device, binary);
	ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1);

	if ((byteCount % 4) > 0)
	{
		if (PS == ADI_PS2)
		{
			/* add extra bytes for PS2 */
			/* large security issue here, the extra bytes will be random sutff past the end of the binary array :-( */
			byteCount += (4 - (byteCount % 4));
		}
		else
		{
			ADI_ERROR_REPORT(&device->common,
				ADI_COMMON_ERRSRC_API,
				ADI_COMMON_ERR_INV_PARAM,
				ADI_COMMON_ACT_ERR_CHECK_PARAM,
				byteCount,
				"Invalid byteCount. Must be multiple of 4.");
			ADI_ERROR_RETURN(device->common.error.newAction);
		}
	}

	if ((byteOffset % 4) > 0)
	{
		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			byteCount,
			"Invalid byteOffset. Must be multiple of 4.");
		ADI_ERROR_RETURN(device->common.error.newAction);
	}
	
	ADI_MUTEX_AQUIRE(device);
	if (byteOffset == 0)
	{
		if (byteCount < 8)
		{
			ADI_ERROR_REPORT(&device->common,
				ADI_COMMON_ERRSRC_API,
				ADI_COMMON_ERR_INV_PARAM,
				ADI_COMMON_ACT_ERR_CHECK_PARAM,
				byteCount,
				"Invalid byteCount. Must be larger than 8.");
			ADI_ERROR_RETURN(device->common.error.newAction);
		}

		/* extraction of stack pointer and boot address from top of array */
		stackPtr[0] = binary[0];
		stackPtr[1] = binary[1];
		stackPtr[2] = binary[2];
		stackPtr[3] = binary[3];

		bootAddr[0] = binary[4];
		bootAddr[1] = binary[5];
		bootAddr[2] = binary[6];
		bootAddr[3] = binary[7];

		if (PS == ADI_PS2)
		{
			entry = ADRV910X_ADDR_SYS2_ARM_BOOT_ADDR_BYTE0;
			stBase = ADRV910X_ADDR_SYS2_ARM_STACK_PTR_BYTE0;
		}
		else
		{
			entry = ADRV910X_ADDR_ARM_BOOT_ADDR_BYTE0;
			stBase = ADRV910X_ADDR_ARM_STACK_PTR_BYTE0;
		}
		/*     core.arm_boot_addr_byte0/1/2/3  */
		ADRV910X_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_0", entry, bootAddr[0]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_1", entry+1, bootAddr[1]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_2", entry+2, bootAddr[2]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_3", entry+3, bootAddr[3]);

		/* core.arm_stack_ptr_byte_0/1/2/3   -- */
		ADRV910X_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_0", stBase, stackPtr[0]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_1", stBase+1, stackPtr[1]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_2", stBase+2, stackPtr[2]);
		ADRV910X_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_3", stBase+3, stackPtr[3]);

	}
	
	if ((ADRV910X_ADDR_DEVICE_PROFILE_OFFSET >= byteOffset) &&
	    (ADRV910X_ADDR_DEVICE_PROFILE_OFFSET < (byteOffset + byteCount + 4)) &&
		PS != ADI_PS2)
	{
		/* extraction of profile address */
		i = ADRV910X_ADDR_DEVICE_PROFILE_OFFSET - byteOffset;
		device->devStateInfo.profileAddr = (((uint32_t)(binary[i + 3]) << 24) |
		    (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
	}

	if ((ADRV910X_ADDR_PFIR_PROFILE_BUFFER_OFFSET >= byteOffset) &&
	    (ADRV910X_ADDR_PFIR_PROFILE_BUFFER_OFFSET < (byteOffset + byteCount + 4)) &&
		PS != ADI_PS2)
	{
		/* extraction of PFIR buffer address */
		i = ADRV910X_ADDR_PFIR_PROFILE_BUFFER_OFFSET - byteOffset;
		device->devStateInfo.pfirProfileAddr = ((((uint32_t)binary[i + 3]) << 24) |
		    (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
	}

	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_START_PROG + byteOffset, &binary[0], byteCount, spiWriteMode, PS);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

#endif /* NEVIS_PS2 */

static __maybe_unused int32_t __maybe_unused adi_adrv910x_arm_Memory_ReadWrite_Validate(adi_adrv910x_Device_t *device,
                                                                         uint32_t address,
                                                                         uint8_t returnData[],
                                                                         uint32_t byteCount,
                                                                         adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode,
                                                                         uint8_t autoIncrement)
{
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    if (!(address >= ADRV910X_ADDR_ARM_START_PROG && address <= ADRV910X_ADDR_ARM_END_PROG) &&
        !(address >= ADRV910X_ADDR_ARM_START_DATA && address <= ADRV910X_ADDR_ARM_END_DATA))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         address,
                         "Invalid ARM Memory Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (!((address + byteCount - 1) >= ADRV910X_ADDR_ARM_START_PROG && (address + byteCount - 1) <= ADRV910X_ADDR_ARM_END_PROG) &&
        !((address + byteCount - 1) >= ADRV910X_ADDR_ARM_START_DATA && (address + byteCount - 1) <= ADRV910X_ADDR_ARM_END_DATA))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         address,
                         "Invalid ARM Memory Address + byteCount");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (autoIncrement && (byteCount % 4 != 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         byteCount,
                         "Invalid parameter value. byteCount must be a multiple of 4 when using autoIncrement");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    
    ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Memory_Read(adi_adrv910x_Device_t *device,
                                     uint32_t address,
                                     uint8_t returnData[],
                                     uint32_t byteCount,
                                     uint8_t autoIncrement,
	                                 adi_common_Processor_Subsystem_e PS)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_arm_Memory_ReadWrite_Validate, device, address, returnData, byteCount, 0, autoIncrement);
    ADI_MUTEX_AQUIRE(device);
    ADI_EXPECT(adrv910x_DmaMemRead, device, address, returnData, byteCount, autoIncrement, PS);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}
int32_t adi_adrv910x_arm_Memory_Write(adi_adrv910x_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode, adi_common_Processor_Subsystem_e PS)
{
#ifdef NEVIS_PS2
      if (PS != ADI_PS1)
      {
          ADI_ERROR_REPORT(&device->common,
                           ADI_COMMON_ERRSRC_API,
                           ADI_COMMON_ERR_INV_PARAM,
                           ADI_COMMON_ACT_ERR_CHECK_PARAM,
                           byteCount,
                           "PS2 API allow memory access to PS1 only");
          ADI_ERROR_RETURN(device->common.error.newAction);
      }
#endif
	ADI_PERFORM_VALIDATION(adi_adrv910x_arm_Memory_ReadWrite_Validate, device, address, (uint8_t *)data, byteCount, spiWriteMode, false);
    ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_DmaMemWrite, device, address, data, byteCount, spiWriteMode, PS);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

#ifndef NEVIS_PS2

int32_t adi_adrv910x_arm_StartStatus_Check(adi_adrv910x_Device_t *device, uint32_t timeout_us)
{
    int32_t halError = 0;
    uint32_t waitInterval_us = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t armDebugLoaded = ((device->devStateInfo.devState & ADI_ADRV910X_STATE_ARM_DEBUG_LOADED) == ADI_ADRV910X_STATE_ARM_DEBUG_LOADED) ? 1 : 0;
    uint8_t objId = 0;
    uint16_t errorCode = 0;
    ADI_ENTRY_EXPECT(device);

    /* Wait for ARM to exit BOOTUP state */
    waitInterval_us = (ADI_ADRV910X_GETARMBOOTUP_INTERVAL_US > timeout_us) ?
        timeout_us : ADI_ADRV910X_GETARMBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    ADI_MUTEX_AQUIRE(device);
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);

#if ADI_ADRV910X_SW_TEST > 0
        if (device->devStateInfo.swTest)
        {
            state.bootState = device->devStateInfo.swTest;
        }
#endif
        if ((state.bootState == ADI_ADRV910X_ARM_BOOT_POWERUP) ||
            ((armDebugLoaded == 1) && (state.bootState == ADI_ADRV910X_ARM_BOOT_JTAG_BUILD_STATUS_READY)))
        {
            /* Wait for ARM to boot */
            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working");
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if ((armDebugLoaded == 0) && (state.bootState == ADI_ADRV910X_ARM_BOOT_JTAG_BUILD_STATUS_READY))
        {
            device->devStateInfo.devState |= ADI_ADRV910X_STATE_ARM_DEBUG_LOADED;
            ADI_MUTEX_RELEASE(device);
            ADI_API_RETURN(device);
        }
        else if (state.bootState == ADI_ADRV910X_ARM_BOOT_READY)
        {
            break; /* Valid case - ARM booted successfully */
        }
        else if (state.bootState <= ADI_ADRV910X_ARM_BOOT_STATE_STREAM_RUNTIME_ERR)
        {
            ADI_EXPECT(adi_adrv910x_arm_SystemError_Get, device, &objId, (uint8_t *)(&errorCode));
            errorCode = ((uint16_t)objId << 8) | errorCode;
            if (errorCode == 0)
            {
                errorCode = state.bootState;
            }
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV910X_SRC_ARMFWSTATUS,
                             errorCode,
                             ADI_ADRV910X_ACT_ERR_RESET_ARM,
                             NULL,
                             adrv910x_error_table_ArmBootStatus[state.bootState]);
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV910X_SRC_ARMFWSTATUS,
                             state.bootState,
                             ADI_ADRV910X_ACT_ERR_RESET_ARM,
                             NULL,
                             "ARM Unknown error during bootup");
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (state.bootState != ADI_ADRV910X_ARM_BOOT_READY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV910X_SRC_ARMFWSTATUS,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV910X_ACT_ERR_RESET_ARM,
                         device,
                         "Timed out waiting for ARM bootup to happen");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

	ADI_MUTEX_RELEASE(device);
	
    device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_ARM_LOADED);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Start(adi_adrv910x_Device_t *device, adi_common_Processor_Subsystem_e PS)
{
    uint8_t armCtl1 = 0;
    uint8_t mailBox[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    
    ADI_ENTRY_EXPECT(device);
	ADI_MUTEX_AQUIRE(device);
	armCtl1 = ADRV910X_AC1_ARM_DEBUG_ENABLE | ADRV910X_AC1_ARM_MEM_HRESP_MASK | ADRV910X_AC1_ARM_M3_RUN;
	if (PS == ADI_PS1)
	{
		/* Set MailBox 0xFF */
		ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_GET, &mailBox[0], 4, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, PS);
		ADRV910X_SPIWRITEBYTE(device, "ARM_CTL_1", ADRV910X_ADDR_ARM_CTL_1, armCtl1);
	}
	else if (PS == ADI_PS2)
	{
		ADRV910X_SPIWRITEBYTE(device, "SYS2_ARM_CTL_1", ADRV910X_ADDR_SYS2_ARM_CTL_1, armCtl1);
	}
	else
	{
		ADI_ERROR_REPORT(&device->common,
			ADI_ADRV910X_SRC_ARMCMD,
			ADI_COMMON_ERR_API_FAIL,
			ADI_ADRV910X_ACT_ERR_RESET_ARM,
			device,
			"Inavlid ARM core selected");
	}

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

#endif /* NEVIS_PS2 */

int32_t adi_adrv910x_arm_SystemError_Get(adi_adrv910x_Device_t *device, uint8_t *objectId, uint8_t *errorCode)
{
	ADI_ENTRY_PTR_EXPECT(device, errorCode);
	ADI_NULL_PTR_RETURN(&device->common, objectId);
    ADI_MUTEX_AQUIRE(device);
	ADRV910X_SPIREADBYTE(device, "arm_cmd_status_12", ADRV910X_ADDR_ARM_CMD_STATUS_12, errorCode);
	ADRV910X_SPIREADBYTE(device, "arm_cmd_status_13", ADRV910X_ADDR_ARM_CMD_STATUS_13, objectId);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Profile_Write(adi_adrv910x_Device_t *device, const adi_adrv910x_Profiles_t *profile, adi_common_Processor_Subsystem_e PS)
{
	ADI_ENTRY_PTR_EXPECT(device, profile);

    ADI_MUTEX_AQUIRE(device);
	if (PS == ADI_PS1)
	{
		/* recompute CRC on data that is going to be written to device */
		deviceProfile_t *p = (deviceProfile_t *)&profile->profilePS1;
		p->checksum = Crc32((const uint8_t*)&profile->profilePS1, sizeof(deviceProfile_t) - 4);
		ADI_EXPECT(adrv910x_ArmProfileWrite, device, &profile->profilePS1, PS);
	}
	else
	{
		ADI_EXPECT(adrv910x_ArmProfileWrite, device, (const deviceProfile_t*)&profile->profilePS2, PS);
	}
	

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_PfirProfiles_Write(adi_adrv910x_Device_t *device, const pfirBuffer_t *init)
{
	ADI_ENTRY_PTR_EXPECT(device, init);

	/* update CRC on pfir buffer before sending */
	pfirBuffer_t *p = (pfirBuffer_t *)init;
	p->checksum = Crc32((const uint8_t*)p, sizeof(pfirBuffer_t) - 4);
    ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_PfirProfilesWrite, device, init);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_MailboxBusy_Get(adi_adrv910x_Device_t *device, bool *mailboxBusy)
{
	uint8_t bfVal = 0;

	/* Check device pointer is not null */
	ADI_ENTRY_PTR_EXPECT(device, mailboxBusy);
    ADI_MUTEX_AQUIRE(device);
	/* Read arm_command_busy bit in arm_command register*/
	ADI_EXPECT(adrv910x_NvsRegmapCore_ArmCommandBusy_Get, device, &bfVal);
	*mailboxBusy = (bool)bfVal;

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_CmdStatus_Get(adi_adrv910x_Device_t *device, uint16_t *errorWord, uint16_t *statusWord)
{
    uint8_t i = 0;
    uint8_t bytes[8] = { 0 };
    uint8_t status = 0;
    static const uint8_t ARM_ERR_MASK_LO = 0x0E;
    static const uint8_t ARM_ERR_MASK_HI = 0xE0;
    static const uint8_t ARM_PENDING_LO = 0x01;
    static const uint8_t ARM_PENDING_HI = 0x10;
    static const uint8_t ERROR_WORD_LO = 0x0001;
    static const uint8_t ERROR_WORD_HI = 0x0002;
    static const uint8_t STATUS_WORD_SHIFT_HI = 3;
    static const uint8_t CMD_STATUS_MAX = 8;


    ADI_ENTRY_PTR_EXPECT(device, errorWord);

    ADI_NULL_PTR_RETURN(&device->common, statusWord);

    /* ensuring the errorWord and statusWord are cleared */
    *errorWord = 0;
    *statusWord = 0;
    ADI_MUTEX_AQUIRE(device);
    /* read in the entire 64-bit status register into a byte array and parse one byte at a time */
    for (i = 0; i < CMD_STATUS_MAX; i++)
    {
        ADRV910X_SPIREADBYTE(device, "", ((uint16_t)ADRV910X_ADDR_ARM_CMD_STATUS_0 + i), &bytes[i]);

#if ADI_ADRV910X_SW_TEST > 0
        /* Test error */
        if (device->devStateInfo.swTest > 1)
        {
            bytes[i] = (uint8_t)device->devStateInfo.swTest;
        }
#endif

        /* assigning each pending bit from every opcode to a weighted position in statusWord */
        status = (bytes[i] & ARM_PENDING_HI) >> STATUS_WORD_SHIFT_HI;
        status |= (bytes[i] & ARM_PENDING_LO);
        *statusWord |= (uint16_t)(status << (i * 2));

        /* checking each 3-bit error field per nibble for > 0, then assigning to weighted bit in errorWord */
        if ((bytes[i] & ARM_ERR_MASK_LO) > 0)
        {
            *errorWord |= (ERROR_WORD_LO << (i * 2));
        }

        if ((bytes[i] & ARM_ERR_MASK_HI) > 0)
        {
            *errorWord |= (ERROR_WORD_HI << (i * 2));
        }
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_CmdStatusOpcode_Get(adi_adrv910x_Device_t *device, uint8_t opCode, uint8_t *cmdStatByte)
{
    uint8_t cmdByteIndex = 0;
    uint8_t cmdByte = 0;
    static const uint8_t ARM_STATUS_MASK = 0x0F;
    static const uint8_t ARM_STATUS_SHIFT_HI = 4;

    ADI_ENTRY_PTR_EXPECT(device, cmdStatByte);

    /* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
    if ((opCode != (uint8_t)ADRV910X_ARM_STREAM_TRIGGER_OPCODE) && ADRV910X_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Invalid ARM opcode given to ArmCmdStatusOpcodeGet()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* calculating command status register offset based on opcode status packing */
    cmdByteIndex = opCode >> 2;
    ADI_MUTEX_AQUIRE(device);
    /* reading the command status register for given opcode */
    ADRV910X_SPIREADBYTE(device, "", ((uint16_t)ADRV910X_ADDR_ARM_CMD_STATUS_0 + cmdByteIndex), &cmdByte);

    /* identifying nibble location in command register for given opcode */
    if (((opCode >> 1) & (uint8_t)0x01) == (uint8_t)0x01)
    {
        *cmdStatByte = ((cmdByte >> ARM_STATUS_SHIFT_HI) & ARM_STATUS_MASK);
    }
    else
    {
        *cmdStatByte = (cmdByte & ARM_STATUS_MASK);
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_CmdStatus_Wait(adi_adrv910x_Device_t *device,
                                        uint8_t opCode,
                                        uint8_t *cmdStatusByte,
                                        uint32_t timeout_us,
                                        uint32_t waitInterval_us)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t halError = 0;
    uint32_t eventCheck = 0;
    uint32_t numEventChecks = 0;
    static const uint8_t ARM_ERR_MASK = 0x0E;
    static const uint8_t ARM_PENDING = 0x01;

    ADI_ENTRY_PTR_EXPECT(device, cmdStatusByte);

    /* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
    if ((opCode != (uint8_t)ADRV910X_ARM_STREAM_TRIGGER_OPCODE) && ADRV910X_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Invalid ARM opcode given to ArmCmdStatusWait()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);
    ADI_MUTEX_AQUIRE(device);
    /* timeout event check loop */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        /* read status of opcode */
        recoveryAction = adi_adrv910x_arm_CmdStatusOpcode_Get(device, opCode, cmdStatusByte);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get ARM command status");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* If error code is non zero in [3:1], - return error */
        if ((*cmdStatusByte & ARM_ERR_MASK) > 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV910X_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV910X_ACT_ERR_RESET_ARM,
                device,
                "ArmCmdStatusWait() failed due to thrown ARM error. Is device in correct state for calling command?");
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);

        }

        /* if pending bit is set for opcode of interest and the number of events have not expired, perform wait */
        if (((*cmdStatusByte & ARM_PENDING) > 0) &&
            (eventCheck < numEventChecks))
        {
            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);

            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV910X_SRC_ARMCMD,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in ArmCmdStatusWait()");
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* if ARM Command did not complete within the timeout period */
    if ((*cmdStatusByte & ARM_PENDING) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV910X_SRC_ARMCMD,
            ADI_COMMON_ERR_API_FAIL,
            ADI_ADRV910X_ACT_ERR_RESET_ARM,
            device,
            "ArmCmdStatusWait() failed due to thrown ARM error. ARM time out ");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Cmd_Write(adi_adrv910x_Device_t *device, uint8_t opCode, const uint8_t extendedData[], uint32_t byteCount)
{
    int32_t halError = 0;
    bool armCommandBusy = false;
    uint8_t i = 0;
    uint32_t timeout_us = ADI_ADRV910X_SENDARMCMD_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV910X_SENDARMCMD_INTERVAL_US;
    uint32_t eventCheck = 0;
    uint32_t numEventChecks = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

#define EXT_CMD_BYTE_MAX 5

    ADI_ENTRY_EXPECT(device);

    if (byteCount > 0)
    {
        ADI_NULL_PTR_RETURN(&device->common, extendedData);
    }

    /* check for even-numbered opCodes only including opcode 0, but must not be greater than opCode 30 */
    if ((opCode != (uint8_t)ADRV910X_ARM_STREAM_TRIGGER_OPCODE) && ADRV910X_OPCODE_VALID(opCode))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Invalid ARM opcode given to ArmCmdWrite()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* the number of valid extended data bytes is from 0-4 */
    if (byteCount > EXT_CMD_BYTE_MAX)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Invalid number of extended data in ArmCmdWrite()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an arm mailbox command until mailbox is ready) */

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    ADI_MUTEX_AQUIRE(device);
    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adi_adrv910x_arm_MailboxBusy_Get(device, &armCommandBusy);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Invalid Get for armCommandBusy");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);

#if ADI_ADRV910X_SW_TEST > 0
        /* Test error */
        if (device->devStateInfo.swTest > 0)
        {
            armCommandBusy = true;
        }
#endif
        if (armCommandBusy == true)
        {
            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working in ArmCmdWrite()");
            ADI_MUTEX_ERROR_RELEASE(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* if busy bit remains set after timeout event loop function is exited, otherwise command is sent after extended bytes */
    if (armCommandBusy == true)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV910X_SRC_ARMCMD,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV910X_ACT_ERR_RESET_ARM,
                         device,
                         "ARM Mailbox Busy. Command not executed in ArmCmdWrite()");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        if (byteCount > 0)
        {
            for (i = 0; i < byteCount; i++)
            {
                ADRV910X_SPIWRITEBYTE(device, "ARM_EXT_CMD_BYTE_1+", ((uint16_t)ADRV910X_ADDR_ARM_EXT_CMD_BYTE_1 + i), extendedData[i]);
            }
        }

        ADRV910X_SPIWRITEBYTE(device, "ARM_COMMAND", ADRV910X_ADDR_ARM_COMMAND, opCode);
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_System_Program(adi_adrv910x_Device_t *device, uint8_t channelMask)
{
    uint8_t extData[2] = { 0 };
    uint8_t cmdStatusByte = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);

    /* Executing the SET System Config command */
    extData[0] = channelMask;
	extData[1] = OBJID_GS_SYSTEM_CONFIG;
    ADI_MUTEX_AQUIRE(device);
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv910x_arm_CmdStatus_Wait(device,
                                                     ADRV910X_ARM_SET_OPCODE,
                                                     &cmdStatusByte,
                                                     ADI_ADRV910X_SETARMSYSTEM_TIMEOUT_US,
                                                     ADI_ADRV910X_SETARMSYSTEM_INTERVAL_US);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /* ARM error handler to provide valid recovery action based on ARM error code */
        if ((cmdStatusByte >> 1) > 0)
        {
            ADI_EXPECT(adrv910x_ArmCmdErrorHandler,
                device,
                ADRV910X_ARMCMD_ERRCODE(ADRV910X_ARM_SET_OPCODE, 0, cmdStatusByte));
        }
    }
    // TODO: Valid for SDK2 state machine, move to initCalsRun in SDK3
	device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_INITCALS_RUN);
	
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_ConfigJtag_ps2(adi_adrv910x_Device_t *device)
{
	
	//#define JTAG_DAISY_CHAIN
	
	uint8_t SetJtagArmDaisyEn = 0;
	uint8_t SetJtagArmSel = 0;
	uint8_t SetJtagArm1TmsEn = 0;
	uint8_t SetJtagArm1TrstEn = 0;
	uint8_t SetJtagArm2TmsEn = 0;
	uint8_t SetJtagArm2TrstEn = 0;
	uint8_t SetJtagTestModeEn = 0;

#if !defined (JTAG_DAISY_CHAIN)
	/* Config JTAG Pins for PS2 */
	SetJtagArmDaisyEn = 0;
	SetJtagArmSel = 1;
	SetJtagArm1TmsEn = 0;
	SetJtagArm1TrstEn = 1;
	SetJtagArm2TmsEn = 1;
	SetJtagArm2TrstEn = 0;
	SetJtagTestModeEn = 1;
#endif
	
#if defined(JTAG_DAISY_CHAIN)
	/* Daisy Chain */
	SetJtagArmDaisyEn = 1;
	SetJtagArmSel = 1;
	SetJtagArm1TmsEn = 1;
	SetJtagArm1TrstEn = 0;
	SetJtagArm2TmsEn = 1;
	SetJtagArm2TrstEn = 0;
	SetJtagTestModeEn = 1;
#endif
	
	/* Check device pointer is not null */
	ADI_ENTRY_EXPECT(device);

	/* Config JtagArmDaisyEn */
	adrv910x_NevisMonitorRegmapCore_JtagArmDaisyEn_Set(device, SetJtagArmDaisyEn);
	/* Config JtagArmSel */
	adrv910x_NevisMonitorRegmapCore_JtagArmSel_Set(device, SetJtagArmSel);
	/* Config JtagArm1TmsEn */
	adrv910x_NevisMonitorRegmapCore_JtagArm1TmsEn_Set(device, SetJtagArm1TmsEn);
	/* Config JtagArm1TrstEn */
	adrv910x_NevisMonitorRegmapCore_JtagArm1TrstEn_Set(device, SetJtagArm1TrstEn);
	/* Config JtagArm2TmsEn */
	adrv910x_NevisMonitorRegmapCore_JtagArm2TmsEn_Set(device, SetJtagArm2TmsEn);
	/* Config JtagArm2TrstEn */
	adrv910x_NevisMonitorRegmapCore_JtagArm2TrstEn_Set(device, SetJtagArm2TrstEn);
	/* Config test mode */
	adrv910x_NevisMonitorRegmapCore_JtagTestmodeEn_Set(device, SetJtagTestModeEn);
	
	ADI_API_RETURN(device);
}

/*! ****************************************************************************
 * Function:    Crc32
 * 
 * @brief       Run Cyclic Redundancy Check on the specified block of memory
 *
 * @details     CRC32 algorithm, operating on 8-bit words
 *
 * Parameters: 
 * @param [in]  buf - array of bytes on which CRC is run
 * @param [in]  buf_len - length of the input array in bytes
 *
 * @return     32-bit checksum
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
*/
static uint32_t Crc32(const uint8_t buf[], uint32_t bufLen)
{
	static const uint32_t LUT_CRC32[16] = {
		0x00000000u, 0x04c11db7u,0x09823b6eu, 0x0d4326d9u,
		0x130476dcu, 0x17c56b6bu, 0x1a864db2u,0x1e475005u,
		0x2608edb8u, 0x22c9f00fu, 0x2f8ad6d6u, 0x2b4bcb61u,
		0x350c9b64u, 0x31cd86d3u, 0x3c8ea00au, 0x384fbdbdu
	};
	
	uint32_t i;
	uint32_t a, b, c, d;
  
	a = 0x00000000u;

	for (i = 0u; i < bufLen; i++) {
		b = (a >> 28u) & 0xfu;
		c = LUT_CRC32[b];
		d = buf[i];
		a = (a << 4u) | (d >> 4u);
		a = a ^ c;
		b = (a >> 28u) & 0xfu;
		c = LUT_CRC32[b];
		a = (a << 4u) | (d & 0xfu);
		a = a ^ c;
	}

	for (i = 0u; i < 4u; i++) {
		b = (a >> 28u) & 0xfu;
		c = LUT_CRC32[b];
		a = (a << 4u);
		a = a ^ c;
		b = (a >> 28u) & 0xfu;
		c = LUT_CRC32[b];
		a = (a << 4u);
		a = a ^ c;
	}

	return a;
}