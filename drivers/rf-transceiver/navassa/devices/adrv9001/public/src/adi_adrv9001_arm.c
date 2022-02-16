/**
* \file
* \brief Contains ARM features related function implementation defined in
* adi_adrv9001_arm.h
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
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_fh.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_arm_error_mapping.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_validators.h"
#include "adrv9001_bf.h"

#include "object_ids.h"

/*********************************************************************************************************/
int32_t adi_adrv9001_arm_Enable(adi_adrv9001_Device_t *device)
{
    uint8_t armCtl1 = 0;

    ADI_ENTRY_EXPECT(device);

    ADI_EXPECT(adi_adrv9001_arm_AhbSpiBridge_Enable, device);

    ADRV9001_SPIREADBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, &armCtl1);

    armCtl1 = armCtl1 | ADRV9001_AC1_ARM_M3_RUN;
    ADRV9001_SPIWRITEBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, armCtl1);

    // Issue SW interrupt 4 to wake up ARM
    ADI_EXPECT(adi_adrv9001_arm_WakeupInterrupt_Set, device);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Disable(adi_adrv9001_Device_t *device)
{
    uint8_t armCtl1 = 0;

    ADI_ENTRY_EXPECT(device);

    /* TODO: Implement ISR to service ARM READY_FOR_MCS */

    ADRV9001_SPIREADBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, &armCtl1);

    armCtl1 = (armCtl1 & (~ADRV9001_AC1_ARM_M3_RUN));
    ADRV9001_SPIWRITEBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, armCtl1);

    ADI_EXPECT(adi_adrv9001_arm_AhbSpiBridge_Disable, device);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_AhbSpiBridge_Enable(adi_adrv9001_Device_t *device)
{
    uint8_t ahbSpiReg = 0;

    ADI_ENTRY_EXPECT(device);

    ADRV9001_SPIREADBYTE(device, "AHB_SPI_BRIDGE", ADRV9001_ADDR_AHB_SPI_BRIDGE, &ahbSpiReg);

    ahbSpiReg = ahbSpiReg | ADRV9001_AHB_SPI_BRIDGE_ENABLE;
    ADRV9001_SPIWRITEBYTE(device, "AHB_SPI_BRIDGE", ADRV9001_ADDR_AHB_SPI_BRIDGE, ahbSpiReg);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_AhbSpiBridge_Disable(adi_adrv9001_Device_t *device)
{
    uint8_t ahbSpiReg = 0;

    ADI_ENTRY_EXPECT(device);

    ADRV9001_SPIREADBYTE(device, "AHB_SPI_BRIDGE", ADRV9001_ADDR_AHB_SPI_BRIDGE, &ahbSpiReg);

    ahbSpiReg = (ahbSpiReg & (~ADRV9001_AC1_ARM_M3_RUN));
    ADRV9001_SPIWRITEBYTE(device, "AHB_SPI_BRIDGE", ADRV9001_ADDR_AHB_SPI_BRIDGE, ahbSpiReg);

    ADI_API_RETURN(device);
}


int32_t adi_adrv9001_arm_StartStatus_Check(adi_adrv9001_Device_t *device, uint32_t timeout_us)
{
    int32_t halError = 0;
    uint32_t waitInterval_us = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t armDebugLoaded = ((device->devStateInfo.devState & ADI_ADRV9001_STATE_ARM_DEBUG_LOADED) == ADI_ADRV9001_STATE_ARM_DEBUG_LOADED) ? 1 : 0;
    uint8_t objId = 0;
    uint16_t errorCode = 0;
    ADI_ENTRY_EXPECT(device);

    /* Wait for ARM to exit BOOTUP state */
    waitInterval_us = (ADI_ADRV9001_GETARMBOOTUP_INTERVAL_US > timeout_us) ?
        timeout_us : ADI_ADRV9001_GETARMBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);


    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        ADI_EXPECT(adi_adrv9001_Radio_State_Get, device, &state);

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest)
        {
            state.bootState = device->devStateInfo.swTest;
        }
#endif
        if ((state.bootState == ADI_ADRV9001_ARM_BOOT_POWERUP) ||
            ((armDebugLoaded == 1) && (state.bootState == ADI_ADRV9001_ARM_BOOT_JTAG_BUILD_STATUS_READY)))
        {
            /* Wait for ARM to boot */
            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else if ((armDebugLoaded == 0) && (state.bootState == ADI_ADRV9001_ARM_BOOT_JTAG_BUILD_STATUS_READY))
        {
            device->devStateInfo.devState |= ADI_ADRV9001_STATE_ARM_DEBUG_LOADED;
            ADI_API_RETURN(device);
        }
        else if (state.bootState == ADI_ADRV9001_ARM_BOOT_READY)
        {
            break; /* Valid case - ARM booted successfully */
        }
        else if (state.bootState <= ADI_ADRV9001_ARM_BOOT_STATE_STREAM_RUNTIME_ERR)
        {
            ADI_EXPECT(adi_adrv9001_arm_SystemError_Get, device, &objId, (uint8_t *)(&errorCode));
            errorCode = ((uint16_t)objId << 8) | errorCode;
            if (errorCode == 0)
            {
                errorCode = state.bootState;
            }
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9001_SRC_ARMFWSTATUS,
                             errorCode,
                             ADI_ADRV9001_ACT_ERR_RESET_ARM,
                             NULL,
                             adrv9001_error_table_ArmBootStatus[state.bootState]);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9001_SRC_ARMFWSTATUS,
                             state.bootState,
                             ADI_ADRV9001_ACT_ERR_RESET_ARM,
                             NULL,
                             "ARM Unknown error during bootup");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (state.bootState != ADI_ADRV9001_ARM_BOOT_READY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9001_SRC_ARMFWSTATUS,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9001_ACT_ERR_RESET_ARM,
                         device,
                         "Timed out waiting for ARM bootup to happen");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    device->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9001_STATE_ARM_LOADED);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Profile_Write(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init)
{
    ADI_ENTRY_PTR_EXPECT(device, init);

    /* FIXME: JS: Why does this function exist? */
    ADI_EXPECT(adrv9001_ArmProfileWrite, device, init);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_PfirProfiles_Write(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init)
{
    ADI_ENTRY_PTR_EXPECT(device, init);

    /* FIXME: JS: Why does this function exist? */
    ADI_EXPECT(adrv9001_PfirProfilesWrite, device, init);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Image_Write(adi_adrv9001_Device_t *device, uint32_t byteOffset, const uint8_t binary[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    uint8_t stackPtr[4] = { 0 };
    uint8_t bootAddr[4] = { 0 };
    uint32_t i = 0;

    ADI_ENTRY_PTR_EXPECT(device, binary);
    ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);

    if ((byteCount % 4) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            byteCount,
            "Invalid byteCount. Must be multiple of 4.");
        ADI_ERROR_RETURN(device->common.error.newAction);
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


        /*     core.arm_boot_addr_byte0/1/2/3  */
        ADRV9001_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_0", ADRV9001_ADDR_ARM_BOOT_ADDR_BYTE0, bootAddr[0]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_1", ADRV9001_ADDR_ARM_BOOT_ADDR_BYTE1, bootAddr[1]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_2", ADRV9001_ADDR_ARM_BOOT_ADDR_BYTE2, bootAddr[2]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_BOOT_ADDR_BYTE_3", ADRV9001_ADDR_ARM_BOOT_ADDR_BYTE3, bootAddr[3]);

        /* core.arm_stack_ptr_byte_0/1/2/3   -- */
        ADRV9001_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_0", ADRV9001_ADDR_ARM_STACK_PTR_BYTE0, stackPtr[0]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_1", ADRV9001_ADDR_ARM_STACK_PTR_BYTE1, stackPtr[1]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_2", ADRV9001_ADDR_ARM_STACK_PTR_BYTE2, stackPtr[2]);
        ADRV9001_SPIWRITEBYTE(device, "ARM_STACK_PTR_BYTE_3", ADRV9001_ADDR_ARM_STACK_PTR_BYTE3, stackPtr[3]);

    }

    if ((ADRV9001_ADDR_DEVICE_PROFILE_OFFSET >= byteOffset) &&
        (ADRV9001_ADDR_DEVICE_PROFILE_OFFSET < (byteOffset + byteCount + 4)))
    {
        /* extraction of profile address */
        i = ADRV9001_ADDR_DEVICE_PROFILE_OFFSET - byteOffset;
        device->devStateInfo.profileAddr = (((uint32_t)(binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    if ((ADRV9001_ADDR_PFIR_PROFILE_BUFFER_OFFSET >= byteOffset) &&
        (ADRV9001_ADDR_PFIR_PROFILE_BUFFER_OFFSET < (byteOffset + byteCount + 4)))
    {
        /* extraction of PFIR buffer address */
        i = ADRV9001_ADDR_PFIR_PROFILE_BUFFER_OFFSET - byteOffset;
        device->devStateInfo.pfirProfileAddr = ((((uint32_t)binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    if ((ADRV9001_ADDR_FH_HOP_TABLE_A_OFFSET >= byteOffset) &&
        (ADRV9001_ADDR_FH_HOP_TABLE_A_OFFSET < (byteOffset + byteCount + 4)))
    {
        i = ADRV9001_ADDR_FH_HOP_TABLE_A_OFFSET - byteOffset;
	    device->devStateInfo.fhHopTableA1Addr = ((((uint32_t)binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    if ((ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET >= byteOffset) &&
        (ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET < (byteOffset + byteCount + 4)))
    {
        i = ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET - byteOffset;
        device->devStateInfo.fhHopTableB1Addr = ((((uint32_t)binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_START_PROG + byteOffset, &binary[0], byteCount, spiWriteMode);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_arm_Memory_ReadWrite_Validate(adi_adrv9001_Device_t *device,
                                                                         uint32_t address,
                                                                         uint8_t returnData[],
                                                                         uint32_t byteCount,
                                                                         adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode,
                                                                         uint8_t autoIncrement)
{
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    if (!(address >= ADRV9001_ADDR_ARM_START_PROG && address <= ADRV9001_ADDR_ARM_END_PROG) &&
        !(address >= ADRV9001_ADDR_ARM_START_DATA && address <= ADRV9001_ADDR_ARM_END_DATA))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         address,
                         "Invalid ARM Memory Address");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (!((address + byteCount - 1) >= ADRV9001_ADDR_ARM_START_PROG && (address + byteCount - 1) <= ADRV9001_ADDR_ARM_END_PROG) &&
        !((address + byteCount - 1) >= ADRV9001_ADDR_ARM_START_DATA && (address + byteCount - 1) <= ADRV9001_ADDR_ARM_END_DATA))
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
    
    ADI_RANGE_CHECK(device, spiWriteMode, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Memory_Read(adi_adrv9001_Device_t *device,
                                     uint32_t address,
                                     uint8_t returnData[],
                                     uint32_t byteCount,
                                     uint8_t autoIncrement)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_Memory_ReadWrite_Validate, device, address, returnData, byteCount, 0, autoIncrement);

    ADI_EXPECT(adrv9001_DmaMemRead, device, address, returnData, byteCount, autoIncrement);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Memory_Read32(adi_adrv9001_Device_t *device,
	uint32_t address,
	uint32_t returnData[],
	uint32_t byteCount,
	uint8_t autoIncrement)
{
	//uint8_t data[] = { 0 };
	
	ADI_PERFORM_VALIDATION(adi_adrv9001_arm_Memory_ReadWrite_Validate, device, address, (uint8_t *)returnData, byteCount, 0, autoIncrement);

	ADI_EXPECT(adrv9001_DmaMemRead, device, address, (uint8_t *)returnData, byteCount, autoIncrement);

	ADI_API_RETURN(device);
}


int32_t adi_adrv9001_arm_Memory_Write(adi_adrv9001_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_Memory_ReadWrite_Validate, device, address, (uint8_t *)data, byteCount, spiWriteMode, false);

    ADI_EXPECT(adrv9001_DmaMemWrite, device, address, data, byteCount, spiWriteMode);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Memory_WriteFH(adi_adrv9001_Device_t *device,adi_adrv9001_FhHopSignal_e hopSignal, adi_adrv9001_FhHopTable_e tableId, uint32_t hopTableAddress, const uint8_t numHopTableEntries[], uint32_t numHopTableEntriesByteCount, uint32_t hopTableBufferAddress, const uint8_t hopTableBufferData[], uint32_t hopTableBufferDataByteCount)
{
	ADI_EXPECT(adrv9001_DmaMemWriteFH, device, hopSignal, tableId, hopTableAddress, numHopTableEntries, numHopTableEntriesByteCount, hopTableBufferAddress, hopTableBufferData, hopTableBufferDataByteCount);

	ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Config_Write(adi_adrv9001_Device_t *device, const uint8_t armData[], uint32_t armDataSize, const uint8_t mailboxCmd[], uint32_t mailboxCmdSize)
{
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, armData, armDataSize);
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, mailboxCmd, mailboxCmdSize);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET, &armData[0], armDataSize, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_SET_OPCODE, &mailboxCmd[0], mailboxCmdSize);

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_SET_OPCODE,
                                        mailboxCmd[1],
                                        ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_MailBox_Get(adi_adrv9001_Device_t *device, uint8_t objectId, uint8_t subObjectId, uint8_t channelMask, uint16_t byteOffset, uint8_t returnData[], uint32_t byteCount)
{
    uint8_t extendedData[5] = { 0 };

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    /* TODO: Validate objectId? */

    extendedData[0] = (uint8_t)((byteCount >> 0) & 0xFF);
    extendedData[1] = (uint8_t)((byteCount >> 8) & 0xFF);
    extendedData[2] = (uint8_t)((byteCount >> 16) & 0xFF);
    extendedData[3] = (uint8_t)((byteCount >> 24) & 0xFF);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_GET, &extendedData[0], 4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* ARM Object id, byte offset LSB, offset MSB = 0, byteCount will read that number of bytes */
    extendedData[0] = channelMask;
    extendedData[1] = objectId;
    extendedData[2] = subObjectId;
    extendedData[3] = (byteOffset & 0xFF);
    extendedData[4] = ((byteOffset >> 8) & 0xFF);

    /* send the read config ARM opcode */
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &extendedData[0], 5);

    /* check the command status for timeout */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_GET_OPCODE,
                                        objectId,
                                        ADI_ADRV9001_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV9001_READARMCFG_INTERVAL_US);
    if (byteCount % 4 != 0)
    {
        ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, ADRV9001_ADDR_ARM_MAILBOX_GET, returnData, byteCount, false);
    }
    else
    {
        ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, ADRV9001_ADDR_ARM_MAILBOX_GET, returnData, byteCount, ADRV9001_ARM_MEM_READ_AUTOINCR);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Config_Read(adi_adrv9001_Device_t *device, uint8_t objectId, uint8_t channelMask, uint16_t byteOffset, uint8_t returnData[], uint32_t byteCount)
{
    return adi_adrv9001_arm_MailBox_Get(device, OBJID_GS_CONFIG, objectId, channelMask, byteOffset, returnData, byteCount);
}

int32_t adi_adrv9001_arm_SystemError_Get(adi_adrv9001_Device_t *device, uint8_t *objectId, uint8_t *errorCode)
{
    ADI_ENTRY_PTR_EXPECT(device, errorCode);
    ADI_NULL_PTR_RETURN(&device->common, objectId);

    ADRV9001_SPIREADBYTE(device, "arm_cmd_status_12", ADRV9001_ADDR_ARM_CMD_STATUS_12, errorCode);
    ADRV9001_SPIREADBYTE(device, "arm_cmd_status_13", ADRV9001_ADDR_ARM_CMD_STATUS_13, objectId);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_CmdStatus_Get(adi_adrv9001_Device_t *device, uint16_t *errorWord, uint16_t *statusWord)
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

    /* read in the entire 64-bit status register into a byte array and parse one byte at a time */
    for (i = 0; i < CMD_STATUS_MAX; i++)
    {
        ADRV9001_SPIREADBYTE(device, "", ((uint16_t)ADRV9001_ADDR_ARM_CMD_STATUS_0 + i), &bytes[i]);

#if ADI_ADRV9001_SW_TEST > 0
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

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_CmdStatusOpcode_Get(adi_adrv9001_Device_t *device, uint8_t opCode, uint8_t *cmdStatByte)
{
    uint8_t cmdByteIndex = 0;
    uint8_t cmdByte = 0;
    static const uint8_t ARM_STATUS_MASK = 0x0F;
    static const uint8_t ARM_STATUS_SHIFT_HI = 4;

    ADI_ENTRY_PTR_EXPECT(device, cmdStatByte);

    /* check for even-numbered opCodes including opcode 0, but must not be greater than 30 */
    if ((opCode != (uint8_t)ADRV9001_ARM_STREAM_TRIGGER_OPCODE) && ADRV9001_OPCODE_VALID(opCode))
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

    /* reading the command status register for given opcode */
    ADRV9001_SPIREADBYTE(device, "", ((uint16_t)ADRV9001_ADDR_ARM_CMD_STATUS_0 + cmdByteIndex), &cmdByte);

    /* identifying nibble location in command register for given opcode */
    if (((opCode >> 1) & (uint8_t)0x01) == (uint8_t)0x01)
    {
        *cmdStatByte = ((cmdByte >> ARM_STATUS_SHIFT_HI) & ARM_STATUS_MASK);
    }
    else
    {
        *cmdStatByte = (cmdByte & ARM_STATUS_MASK);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_CmdStatus_Wait(adi_adrv9001_Device_t *device,
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
    if ((opCode != (uint8_t)ADRV9001_ARM_STREAM_TRIGGER_OPCODE) && ADRV9001_OPCODE_VALID(opCode))
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

    /* timeout event check loop */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        /* read status of opcode */
        recoveryAction = adi_adrv9001_arm_CmdStatusOpcode_Get(device, opCode, cmdStatusByte);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         NULL,
                         "Failed to get ARM command status");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* If error code is non zero in [3:1], - return error */
        if ((*cmdStatusByte & ARM_ERR_MASK) > 0)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                device,
                "ArmCmdStatusWait() failed due to thrown ARM error. Is device in correct state for calling command?");
            ADI_ERROR_RETURN(device->common.error.newAction);

        }

        /* if pending bit is set for opcode of interest and the number of events have not expired, perform wait */
        if (((*cmdStatusByte & ARM_PENDING) > 0) &&
            (eventCheck < numEventChecks))
        {
            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);

            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in ArmCmdStatusWait()");
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
            ADI_ADRV9001_SRC_ARMCMD,
            ADI_COMMON_ERR_API_FAIL,
            ADI_ADRV9001_ACT_ERR_RESET_ARM,
            device,
            "ArmCmdStatusWait() failed due to thrown ARM error. ARM time out ");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Cmd_Write(adi_adrv9001_Device_t *device, uint8_t opCode, const uint8_t extendedData[], uint32_t byteCount)
{
    int32_t halError = 0;
    bool armCommandBusy = false;
    uint8_t i = 0;
    uint32_t timeout_us = ADI_ADRV9001_SENDARMCMD_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV9001_SENDARMCMD_INTERVAL_US;
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
    if ((opCode != (uint8_t)ADRV9001_ARM_STREAM_TRIGGER_OPCODE) && ADRV9001_OPCODE_VALID(opCode))
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

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adi_adrv9001_arm_MailboxBusy_Get(device, &armCommandBusy);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Invalid Get for armCommandBusy");
        ADI_ERROR_RETURN(device->common.error.newAction);

#if ADI_ADRV9001_SW_TEST > 0
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
                         ADI_ADRV9001_SRC_ARMCMD,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_ADRV9001_ACT_ERR_RESET_ARM,
                         device,
                         "ARM Mailbox Busy. Command not executed in ArmCmdWrite()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        if (byteCount > 0)
        {
            for (i = 0; i < byteCount; i++)
            {
                ADRV9001_SPIWRITEBYTE(device, "ARM_EXT_CMD_BYTE_1+", ((uint16_t)ADRV9001_ADDR_ARM_EXT_CMD_BYTE_1 + i), extendedData[i]);
            }
        }

        ADRV9001_SPIWRITEBYTE(device, "ARM_COMMAND", ADRV9001_ADDR_ARM_COMMAND, opCode);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Version(adi_adrv9001_Device_t *device, adi_adrv9001_ArmVersion_t *armVersion)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t ver[8] = { 0 };
    uint32_t fullVersion = 0;
    static const uint8_t ARMBUILD_DEBUG = 0x01;
    static const uint8_t ARMBUILD_TESTOBJ = 0x04;
    static const uint8_t ARMBUILD_USES_FOUR_DIGIT_VERSION = 0x01;

    ADI_ENTRY_PTR_EXPECT(device, armVersion);

    if ((device->devStateInfo.devState & ADI_ADRV9001_STATE_ARM_LOADED) != ADI_ADRV9001_STATE_ARM_LOADED)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV9001_SRC_ARMCMD,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "ARM binary must be loaded before calling ArmVersionGet()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9001_arm_Memory_Read(device, ADRV9001_ADDR_ARM_VERSION, &ver[0], sizeof(ver), ADRV9001_ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Failed to read ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);
    fullVersion = (((uint32_t)ver[0]) | (((uint32_t)ver[1]) << 8) | (((uint32_t)ver[2]) << 16) | (((uint32_t)ver[3]) << 24));


#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest > 1)
    {
        fullVersion = device->devStateInfo.swTest;
    }
#endif

    if (ARMBUILD_USES_FOUR_DIGIT_VERSION)
    {
        armVersion->majorVer = (uint8_t)(fullVersion >> 28) & 0x0F;
        armVersion->minorVer = (uint8_t)(fullVersion >> 20) & 0xFF;
        armVersion->maintVer = (uint8_t)(fullVersion >> 12) & 0xFF;
        armVersion->rcVer = (uint16_t)(fullVersion & 0xFFF);
    }
    else
    {
        armVersion->rcVer = (uint8_t)(fullVersion % 100);
        armVersion->minorVer = (uint8_t)((fullVersion / 100) % 100);
        armVersion->majorVer = (uint8_t)(fullVersion / 10000);
        armVersion->maintVer = 0;
    }

    if (ver[4] & ARMBUILD_DEBUG)
    {
        armVersion->armBuildType = ADI_ADRV9001_ARMBUILD_DEBUG;
    }
    else if (ver[4] & ARMBUILD_TESTOBJ)
    {
        armVersion->armBuildType = ADI_ADRV9001_ARMBUILD_TESTOBJ;
    }
    else
    {
        armVersion->armBuildType = ADI_ADRV9001_ARMBUILD_RELEASE;
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_ChecksumTable_Get(adi_adrv9001_Device_t *device, adi_adrv9001_ChecksumTable_t *checksum, uint8_t *checksumValid)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t halError = 0;
    uint32_t buildTimeChecksum = 0x00000000;
    uint32_t calculatedChecksum = 0x00000000;
    uint8_t checkData[sizeof(adi_adrv9001_ChecksumTable_t)] = { 0 };
    uint32_t checkAddr = 0;
    uint32_t timeout_us = ADI_ADRV9001_VERIFY_ARM_CHKSUM_TIMEOUT_US;
    uint32_t waitInterval_us = ADI_ADRV9001_VERIFY_ARM_CHKSUM_INTERVAL_US;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    int i = 0;

    static const uint8_t CHECKSUM_BYTES = 0x4;
    static const uint8_t CHECKSUMENTRYSIZE = 8;
    static const uint32_t MIN_TIMEOUT_US = 10000;

    /* Arm stream checksum order: main, rx1/2, tx1/2, orx12 */
#if ADI_ADRV9001_STREAM_CHECKSUM_ENABLE > 0
    int j = 0;
    static const uint32_t streamChannel[] = {
        0xFFFFFFFF,
        ADI_ADRV9001_RX1,
        ADI_ADRV9001_RX2,
        ADI_ADRV9001_TX1,
        ADI_ADRV9001_TX2,
        (ADI_ADRV9001_ORX1 | ADI_ADRV9001_ORX2 |
                             ADI_ADRV9001_ILB1 |
                             ADI_ADRV9001_ILB2 |
                             ADI_ADRV9001_ELB1 |
                             ADI_ADRV9001_ELB2),
    };
#endif

    ADI_ENTRY_PTR_EXPECT(device, checksum);

    ADI_NULL_PTR_RETURN(&device->common, checksumValid);

    if (timeout_us < MIN_TIMEOUT_US)
    {
        /* Set minimum timeout of 10 ms. Prevent divide by zero error */
        timeout_us = MIN_TIMEOUT_US;
    }

    recoveryAction = adi_adrv9001_arm_Memory_Read(device, ADRV9001_ADDR_ARM_CALC_CHKSUM_PTR, checkData, CHECKSUM_BYTES, ADRV9001_ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Failed to read ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);
    checkAddr = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adi_adrv9001_arm_Memory_Read(device, checkAddr, &checkData[0], sizeof(checkData), ADRV9001_ARM_MEM_READ_AUTOINCR);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* ARM firmware checksum */
        buildTimeChecksum = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));
        calculatedChecksum = ((((uint32_t)checkData[7]) << 24) | (((uint32_t)checkData[6]) << 16) | (((uint32_t)checkData[5]) << 8) | ((uint32_t)checkData[4]));

        if ((calculatedChecksum == 0) && (eventCheck < numEventChecks))
        {
            /* wait */
            halError = adi_common_hal_Wait_us(&device->common, timeout_us);
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_ADI_HAL,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                eventCheck,
                "Timer not working");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* ARM completed calculating checksum */
    if ((calculatedChecksum > 0) && (buildTimeChecksum > 0))
    {
        *checksumValid = 1;
        checksum->fwCheckSums.buildChecksum = buildTimeChecksum;
        checksum->fwCheckSums.runChecksum = calculatedChecksum;

        /* performing checksum check, skip if checksum was not calculated (ARM DEBUG_MODE)*/
        if ((calculatedChecksum > 0) && (buildTimeChecksum != calculatedChecksum))
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARM,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                calculatedChecksum,
                "FW checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

/* Disabled Stream checksum verification as Firmware does not calculate checksum for Streams */
#if  ADI_ADRV9001_STREAM_CHECKSUM_ENABLE > 0
        for (j = 0; j < ADRV9001_MAX_NUM_STREAM; j++)
        {
            i = CHECKSUMENTRYSIZE * (j + 1);

            /* Streams checksum */
            buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
            calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

            checksum->streamsCheckSum[j].buildChecksum = buildTimeChecksum;
            checksum->streamsCheckSum[j].runChecksum = calculatedChecksum;

            /* Skip if channel not initialize */
            if ((device->devStateInfo.initializedChannels & streamChannel[j]) > 0)
            {
                /* performing checksum check */
                if (buildTimeChecksum != calculatedChecksum)
                {
                    ADI_ERROR_REPORT(&device->common,
                        ADI_ADRV9001_SRC_ARMCMD,
                        ADI_COMMON_ERR_API_FAIL,
                        ADI_ADRV9001_ACT_ERR_RESET_ARM,
                        calculatedChecksum,
                        "Streams checksum is invalid");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

        }
#endif
        i = CHECKSUMENTRYSIZE * (ADRV9001_MAX_NUM_STREAM + 1);

        /* Device profile checksum */
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

        checksum->deviceProfileCheckSum.buildChecksum = buildTimeChecksum;
        checksum->deviceProfileCheckSum.runChecksum = calculatedChecksum;

#if ADI_ADRV9001_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                calculatedChecksum,
                "Arm Profile checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif

        i = CHECKSUMENTRYSIZE * (ADRV9001_MAX_NUM_STREAM + 2);

        /* PFIR profile checksum */
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

        checksum->pfirProfileCheckSum.buildChecksum = buildTimeChecksum;
        checksum->pfirProfileCheckSum.runChecksum = calculatedChecksum;

#if ADI_ADRV9001_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                calculatedChecksum,
                "PFIR Profile checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_MailboxBusy_Get(adi_adrv9001_Device_t *device, bool *mailboxBusy)
{
    uint8_t bfVal = 0;

    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, mailboxBusy);

    /* Read arm_command_busy bit in arm_command(0x00c3) register*/
    ADI_EXPECT(adrv9001_NvsRegmapCore_ArmCommandBusy_Get, device, &bfVal);
    *mailboxBusy = (bool)bfVal;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_FwStatus_Check(adi_adrv9001_Device_t *device, uint32_t timeout_us,
    uint32_t fwCheckStatus)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t halError = 0;
    uint32_t fwStatus = 0;
    uint32_t waitInterval_us = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    uint8_t status = 0;

    static const uint32_t ADRV9001_ARM_FWSTATUS_ERROR = ADI_ADRV9001_SRC_ARMFWSTATUS << 16;

    ADI_ENTRY_EXPECT(device);

    /* Wait for ARM to exit BOOTUP state */
    waitInterval_us = (ADI_ADRV9001_GETARMBOOTUP_INTERVAL_US > timeout_us) ?
        timeout_us : ADI_ADRV9001_GETARMBOOTUP_INTERVAL_US;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);


    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        ADRV9001_SPIREADBYTE(device, "ARM_CMD_STATUS_8", ADRV9001_ADDR_ARM_CMD_STATUS_8, &status);

        fwStatus = (uint32_t)status;

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest)
        {
            ADRV9001_DEBUG_INFO("SwTest Mode: set fwStatus = device->devStateInfo.swTest");
            fwStatus = device->devStateInfo.swTest;
        }
#endif
        if (fwStatus == fwCheckStatus)
        {
            break;
        }
        else
        {
            /*Wait interval then check again */

            halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             halError,
                             ADI_COMMON_ACT_ERR_CHECK_TIMER,
                             device,
                             "Timer not working");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if (fwStatus != fwCheckStatus)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV9001_SRC_ARMFWSTATUS,
                         (ADRV9001_ARM_FWSTATUS_ERROR + fwStatus),
                         ADI_ADRV9001_ACT_ERR_RESET_ARM,
                         device,
                         "ArmBootStatus_e: Timed out waiting for ARM fwCheckStatus to happen");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "ArmFwStatusCheck Failed");
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Profile_Program(adi_adrv9001_Device_t *device)
{
    uint8_t extData[2] = { 0 };

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);

    /* Executing the SET Device Profile command */
    extData[0] = 0;
    extData[1] = OBJID_GS_DEVICE_PROFILE;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_SETARMPROFILE_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_SETARMPROFILE_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_System_Program(adi_adrv9001_Device_t *device, uint8_t channelMask)
{
    uint8_t extData[2] = { 0 };
    uint8_t cmdStatusByte = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	int orxFlgShift = 3;   //ORX flag shift from FW bit position, lsb bit 1, to API bit position ADRV9001_ORX1
	int ilbFlgShift = 4;   //ILB flag shift from FW bit position, lsb bit 2, to API bit position ADRV9001_ILB1
	int elbFlgShift = 5;   //ELB flag shift from FW bit position, lsb bit 3, to API bit position ADRV9001_ELB1
	int txFlgShiftUp = 3;   // TX enable flag, lsb bit 5
	int adcPortBFlg = 64;   // ADC port B enable flag, lsb bit 6

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);

    /* Executing the SET System Config command */
    extData[0] = channelMask;
    extData[1] = OBJID_GS_SYSTEM_CONFIG;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9001_arm_CmdStatus_Wait(device,
                                                     ADRV9001_ARM_SET_OPCODE,
                                                     &cmdStatusByte,
                                                     ADI_ADRV9001_SETARMSYSTEM_TIMEOUT_US,
                                                     ADI_ADRV9001_SETARMSYSTEM_INTERVAL_US);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /* ARM error handler to provide valid recovery action based on ARM error code */
        if ((cmdStatusByte >> 1) > 0)
        {
            ADI_EXPECT(adrv9001_ArmCmdErrorHandler,
                device,
                ADRV9001_ARMCMD_ERRCODE(ADRV9001_ARM_SET_OPCODE, 0, cmdStatusByte));
        }
    }

	adi_adrv9001_RxPortSwitchCfg_t portSwitchCfg = { 0 };
	ADI_EXPECT(adi_adrv9001_Rx_PortSwitch_Inspect, device, &portSwitchCfg);

	/*Mask for Channel 1 */
	device->devStateInfo.chProfEnMask[0] = ((device->devStateInfo.initializedChannels & ADI_ADRV9001_RX1) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ORX1) >> (orxFlgShift)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ILB1) >> (ilbFlgShift)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ELB1) >> (elbFlgShift)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_TX1) << (txFlgShiftUp)));
	
	/*Mask for Channel 2 */
	device->devStateInfo.chProfEnMask[1] = (((device->devStateInfo.initializedChannels & ADI_ADRV9001_RX2) >> 1) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ORX2) >> (orxFlgShift  + 1)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ILB2) >> (ilbFlgShift  + 1)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_ELB2) >> (elbFlgShift  + 1)) |
								   ((device->devStateInfo.initializedChannels & ADI_ADRV9001_TX2) << (txFlgShiftUp  - 1)));

	if (portSwitchCfg.enable == true)
	{
		device->devStateInfo.chProfEnMask[0] = device->devStateInfo.chProfEnMask[0] | adcPortBFlg;
		device->devStateInfo.chProfEnMask[1] = device->devStateInfo.chProfEnMask[1] | adcPortBFlg;
	}
	
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_WakeupInterrupt_Set(adi_adrv9001_Device_t *device)
{
    uint8_t sw4RegisterRead = 0;
    static const uint8_t SET_SW_INTERRUPT_4 = 0x01;

    ADI_ENTRY_EXPECT(device);

    /* Read SW interrupt 4 register */
    ADRV9001_SPIREADBYTE(device, "ADRV9001_ADDR_SW_INTERRUPT_4", ADRV9001_ADDR_SW_INTERRUPT_4, &sw4RegisterRead);

    /* Issue SW interrupt 4 to wake up ARM */
    ADRV9001_SPIWRITEBYTE(device,
                          "ADRV9001_ADDR_SW_INTERRUPT_4",
                          ADRV9001_ADDR_SW_INTERRUPT_4,
                          (sw4RegisterRead | SET_SW_INTERRUPT_4));

    /* Read SW interrupt 4 register */
    ADRV9001_SPIREADBYTE(device, "ADRV9001_ADDR_SW_INTERRUPT_4", ADRV9001_ADDR_SW_INTERRUPT_4, &sw4RegisterRead);

    /* Clear SW interrupt 4 to wake up ARM */
    ADRV9001_SPIWRITEBYTE(device,
                          "ADRV9001_ADDR_SW_INTERRUPT_4",
                          ADRV9001_ADDR_SW_INTERRUPT_4,
                          (sw4RegisterRead & (~SET_SW_INTERRUPT_4)));

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_arm_NextDynamicProfile_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                               const adi_adrv9000_DynamicProfile_t *dynamicProfile)
{
    /* Check input pointers are not null */
    ADI_ENTRY_PTR_EXPECT(adrv9001, dynamicProfile);
    ADI_RANGE_CHECK(adrv9001, dynamicProfile->dynamicProfileIndex, ADI_ADRV9001_DYNAMIC_PROFILE_INDEX0, ADI_ADRV9001_DYNAMIC_PROFILE_INDEX5)
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_NextDynamicProfile_Set(adi_adrv9001_Device_t *device,
                                                const adi_adrv9000_DynamicProfile_t *dynamicProfile)
{
    uint8_t extData[5] = { 0 };

    ADI_EXPECT(adi_adrv9001_arm_NextDynamicProfile_Set_Validate, device, dynamicProfile);
    /* Send the profile data */
    ADI_EXPECT(adrv9001_DynamicProfile_Write, device, dynamicProfile);

    /* Execute the SET:DYNAMIC_PROFILE command */
    extData[0] = 0xFF;
    extData[1] = OBJID_GS_DYNAMIC_PROFILE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_INTERVAL_US);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_arm_NextPfir_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                     uint8_t channelMask,
                                                                     const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff)
{
    /* Check input pointers are not null */
    ADI_ENTRY_PTR_EXPECT(adrv9001, pfirCoeff);
    /* Check the channelMask refers to valid channels (bits 0 to 3) and is not empty */
    ADI_RANGE_CHECK(adrv9001, channelMask, 1, 15)
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_NextPfir_Set(adi_adrv9001_Device_t *device,
                                      uint8_t channelMask,
                                      const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff)
{
    uint8_t extData[5] = { 0 };

    ADI_EXPECT(adi_adrv9001_arm_NextPfir_Set_Validate, device, channelMask, pfirCoeff);

    /* Send the PFIR data */
    ADI_EXPECT(adrv9001_PfirWbNbBuffer_Write, device, pfirCoeff);

    /* Execute the SET:DYNAMIC_PROFILE:channelMask command */
    extData[0] = channelMask;
    extData[1] = OBJID_GS_DYNAMIC_PROFILE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_NextRxChannelFilter_Set(adi_adrv9001_Device_t *device,
                                                 const adi_adrv9001_PfirWbNbBuffer_t *pfirRx1WbNbChFilterCoeff,
                                                 const adi_adrv9001_PfirWbNbBuffer_t *pfirRx2WbNbChFilterCoeff)
{
    ADI_ENTRY_EXPECT(device);

    if (pfirRx1WbNbChFilterCoeff != NULL)
    {
        /* Send Rx1 coeffs, which may be the same as Rx2 coeffs */
        uint8_t channelMask = ADI_ADRV9001_RX1 |
            ((pfirRx1WbNbChFilterCoeff == pfirRx2WbNbChFilterCoeff)? ADI_ADRV9001_RX2 : 0);
        ADI_EXPECT(adi_adrv9001_arm_NextPfir_Set, device, channelMask, pfirRx1WbNbChFilterCoeff);
    }
    else
    {
        /* Error if both Rx1 and Rx2 coeffs are NULL */
        ADI_NULL_PTR_RETURN(&device->common, pfirRx2WbNbChFilterCoeff);
    }

    if ((pfirRx2WbNbChFilterCoeff != NULL) && (pfirRx2WbNbChFilterCoeff != pfirRx1WbNbChFilterCoeff))
    {
        /* Send Rx2 coefs because different from Rx1 coeffs */
        ADI_EXPECT(adi_adrv9001_arm_NextPfir_Set, device, ADI_ADRV9001_RX2, pfirRx2WbNbChFilterCoeff);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_NextTxPulseShaper_Set(adi_adrv9001_Device_t *device,
                                               const adi_adrv9001_PfirWbNbBuffer_t *pfirTx1WbNbPulShpCoeff,
                                               const adi_adrv9001_PfirWbNbBuffer_t *pfirTx2WbNbPulShpCoeff)
{
    ADI_ENTRY_EXPECT(device);

    if (pfirTx1WbNbPulShpCoeff != NULL)
    {
        /* Send Tx1 coeffs, which may be the same as Tx2 coeffs */
        uint8_t channelMask = ADI_ADRV9001_TX1 |
            ((pfirTx1WbNbPulShpCoeff == pfirTx2WbNbPulShpCoeff)? ADI_ADRV9001_TX2 : 0);
        ADI_EXPECT(adi_adrv9001_arm_NextPfir_Set, device, channelMask, pfirTx1WbNbPulShpCoeff);
    }
    else
    {
        /* Error if both Tx1 and Tx2 coeffs are NULL */
        ADI_NULL_PTR_RETURN(&device->common, pfirTx2WbNbPulShpCoeff);
    }

    if ((pfirTx2WbNbPulShpCoeff != NULL) && (pfirTx2WbNbPulShpCoeff != pfirTx1WbNbPulShpCoeff))
    {
        /* Send Tx2 coefs because different from Tx1 coeffs */
        ADI_EXPECT(adi_adrv9001_arm_NextPfir_Set, device, ADI_ADRV9001_TX2, pfirTx2WbNbPulShpCoeff);
    }

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv9001_arm_Profile_Switch_Validate(adi_adrv9001_Device_t *adrv9001)
{
    uint8_t chan_index = 0;
    uint8_t port_index = 0;
    adi_adrv9001_RadioState_t currentState = { 0 };
    adi_common_Port_e port = ADI_RX;
    adi_common_ChannelNumber_e channel = ADI_CHANNEL_1;
    static const uint32_t CHANNELS[][2] = { {ADI_ADRV9001_RX1, ADI_ADRV9001_RX2},
                                            {ADI_ADRV9001_TX1, ADI_ADRV9001_TX2} };

    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    /* Validate current state */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);

    for (port = ADI_RX; port <= ADI_TX; port++)
    {
        for (channel = ADI_CHANNEL_1; channel <= ADI_CHANNEL_2; channel++)
        {
            adi_common_channel_to_index(channel, &chan_index);
            if (ADRV9001_BF_EQUAL(adrv9001->devStateInfo.initializedChannels, CHANNELS[port_index][chan_index]))
            {
                ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, port, channel, &state);
                //if ((ADI_ADRV9001_CHANNEL_STANDBY == state) || (ADI_ADRV9001_CHANNEL_RF_ENABLED == state))
                if(ADI_ADRV9001_CHANNEL_RF_ENABLED == state)
                {
                    adi_common_port_to_index(port, &port_index);
                    ADI_ERROR_REPORT(&adrv9001->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_API_FAIL,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        currentState.channelStates[port_index][chan_index],
                        "Error while attempting to send profile switch mailbox command to ARM firmware. All the channels must be in one of CALIBRATED or PRIMED state.");
                    ADI_API_RETURN(adrv9001)
                }
            }
        }
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_Profile_Switch(adi_adrv9001_Device_t *device)
{
    ADI_ENTRY_EXPECT(device);

    /* The SW_INTERRUPT bits are alone in their registers specifically so we don't need to read-modify-write */

    /* Issue SW interrupt 2 to trigger profile switch */
    ADRV9001_SPIWRITEBYTE(device, "ADRV9001_ADDR_SW_INTERRUPT_2", ADRV9001_ADDR_SW_INTERRUPT_2, 1);

    /* Clear SW interrupt 2 */
    ADRV9001_SPIWRITEBYTE(device, "ADRV9001_ADDR_SW_INTERRUPT_2", ADRV9001_ADDR_SW_INTERRUPT_2, 0);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Start(adi_adrv9001_Device_t *device)
{
    uint8_t armCtl1 = 0;
    uint8_t mailBox[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    
    ADI_ENTRY_EXPECT(device);

    /* Set MailBox 0xFF */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_GET, &mailBox[0], 4, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    
    armCtl1 = ADRV9001_AC1_ARM_DEBUG_ENABLE | ADRV9001_AC1_ARM_MEM_HRESP_MASK | ADRV9001_AC1_ARM_M3_RUN;
    ADRV9001_SPIWRITEBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, armCtl1);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_Stop(adi_adrv9001_Device_t *device)
{
    uint8_t armCtl1 = 0;

    ADI_ENTRY_EXPECT(device);

    armCtl1 = ADRV9001_AC1_ARM_DEBUG_ENABLE | ADRV9001_AC1_ARM_MEM_HRESP_MASK;
    ADRV9001_SPIWRITEBYTE(device, "ARM_CTL_1", ADRV9001_ADDR_ARM_CTL_1, armCtl1);

    ADI_API_RETURN(device);
}