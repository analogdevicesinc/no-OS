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
#include "adi_adrv9001_mcs.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_arm_error_mapping.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_validators.h"
#include "adrv9001_bf.h"

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
        else if (state.bootState <= ADI_ADRV9001_ARM_BOOT_CLKGEN_RCAL_ERR)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9001_SRC_ARMFWSTATUS,
                             state.bootState,
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

/* FIXME: Disabled Checksum verification as FW code disabled checksum calculation.
          It may have to enabled in future*/
#if 0
    adi_adrv9001_ChecksumTable_t checksum = { { 0 } };
    uint8_t checksumValid = 0;

    if (armDebugLoaded == 0)
    {
        recoveryAction = adi_adrv9001_arm_ChecksumTable_Get(device, &checksum, &checksumValid);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
#endif

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
        device->devStateInfo.fhHopTable1Addr = ((((uint32_t)binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    if ((ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET >= byteOffset) &&
        (ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET < (byteOffset + byteCount + 4)))
    {
        i = ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET - byteOffset;
        device->devStateInfo.fhHopTable2Addr = ((((uint32_t)binary[i + 3]) << 24) |
            (((uint32_t)binary[i + 2]) << 16) | (((uint32_t)binary[i + 1]) << 8) | ((uint32_t)binary[i]));
    }

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_START_PROG + byteOffset, &binary[0], byteCount, spiWriteMode);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_arm_Memory_ReadWrite_Validate(adi_adrv9001_Device_t *device,
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

int32_t adi_adrv9001_arm_Memory_Write(adi_adrv9001_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_Memory_ReadWrite_Validate, device, address, (uint8_t *)data, byteCount, spiWriteMode, false);

    ADI_EXPECT(adrv9001_DmaMemWrite, device, address, data, byteCount, spiWriteMode);

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

int32_t adi_adrv9001_arm_Config_Read(adi_adrv9001_Device_t *device, uint8_t objectId, uint8_t channelMask, uint16_t byteOffset, uint8_t returnData[], uint32_t byteCount)
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
    extendedData[1] = ADRV9001_ARM_OBJECTID_CONFIG;
    extendedData[2] = objectId;
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
        armVersion->minorVer = (uint8_t)(fullVersion >> 24) & 0x0F;
        armVersion->maintVer = (uint8_t)(fullVersion >> 16) & 0xFF;
        armVersion->rcVer = (uint16_t)(fullVersion & 0xFFFF);
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
    int j = 0;
    int i = 0;

    static const uint8_t CHECKSUM_BYTES = 0x4;
    static const uint8_t CHECKSUMENTRYSIZE = 8;
    static const uint32_t MIN_TIMEOUT_US = 10000;

    /* Arm stream checksum order: main, rx1/2, tx1/2, orx12 */
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


    ADI_ENTRY_PTR_EXPECT(device, checksum);

    ADI_NULL_PTR_RETURN(&device->common, checksumValid);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 1)
    {
        checksum->fwCheckSums.buildChecksum = 12345678;
        checksum->fwCheckSums.runChecksum = 12345678;
        *checksumValid = 1;
        return (int32_t)recoveryAction;
    }
#endif

    if (timeout_us < MIN_TIMEOUT_US)
    {
        /* Set minimum timeout of 10 ms. Prevent divide by zero error */
        timeout_us = MIN_TIMEOUT_US;
    }

    recoveryAction = adi_adrv9001_arm_Memory_Read(device, ADRV9001_ADDR_ARM_CALC_CHKSUM_PTR, checkData, CHECKSUM_BYTES, ADRV9001_ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Failed to read ARM memory");
    ADI_ERROR_RETURN(device->common.error.newAction);
    checkAddr = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest >= 2)
    {
        checkAddr = ADRV9001_ADDR_ARM_CALC_CHKSUM_PTR;
    }
#endif

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adi_adrv9001_arm_Memory_Read(device, checkAddr, &checkData[0], sizeof(checkData), ADRV9001_ARM_MEM_READ_AUTOINCR);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* ARM firmware checksum */
        buildTimeChecksum = ((((uint32_t)checkData[3]) << 24) | (((uint32_t)checkData[2]) << 16) | (((uint32_t)checkData[1]) << 8) | ((uint32_t)checkData[0]));
        calculatedChecksum = ((((uint32_t)checkData[7]) << 24) | (((uint32_t)checkData[6]) << 16) | (((uint32_t)checkData[5]) << 8) | ((uint32_t)checkData[4]));

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest == 3)
        {
            calculatedChecksum = 0;
            buildTimeChecksum = calculatedChecksum;
        }
#endif

        if ((calculatedChecksum == 0) && (eventCheck < numEventChecks))
        {
            /* wait */
            halError = adi_common_hal_Wait_us(&device->common, timeout_us);
#if ADI_ADRV9001_SW_TEST > 0
            if (device->devStateInfo.swTest == 3)
            {
                halError = 1;
            }
#endif
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

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest >= 5)
    {
        calculatedChecksum = 12345678;
        buildTimeChecksum = calculatedChecksum;
    }
    if (device->devStateInfo.swTest == 2)
    {
        calculatedChecksum = 0;
        buildTimeChecksum = calculatedChecksum;
    }
#endif

    /* ARM completed calculating checksum */
    if ((calculatedChecksum > 0) && (buildTimeChecksum > 0))
    {
        *checksumValid = 1;
        checksum->fwCheckSums.buildChecksum = buildTimeChecksum;
        checksum->fwCheckSums.runChecksum = calculatedChecksum;

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest == 5)
        {
            calculatedChecksum = 12345678;
            buildTimeChecksum = calculatedChecksum + 1;
        }
#endif

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

        for (j = 0; j < ADRV9001_MAX_NUM_STREAM; j++)
        {
            i = CHECKSUMENTRYSIZE * (j + 1);

            /* Streams checksum */
            buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
            calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

#if ADI_ADRV9001_SW_TEST > 0
            if (device->devStateInfo.swTest == 6)
            {
                calculatedChecksum = 12345678;
                buildTimeChecksum = calculatedChecksum + 1;
            }
#endif
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

        //i = offsetof(adi_adrv9001_checksumTable_t, deviceProfileCheckSum);
        i = CHECKSUMENTRYSIZE * (ADRV9001_MAX_NUM_STREAM + 1);

        /* Device profile checksum */
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest == 7)
        {
            calculatedChecksum = 12345678;
            buildTimeChecksum = calculatedChecksum + 1;
        }
#endif
        checksum->deviceProfileCheckSum.buildChecksum = buildTimeChecksum;
        checksum->deviceProfileCheckSum.runChecksum = calculatedChecksum;

#if ADI_ADRV9001_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                calculatedChecksum,
                "Arm Profile checksum is invalid");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
#endif

        i = CHECKSUMENTRYSIZE * (ADRV9001_MAX_NUM_STREAM + 2);

        /* ADC profile checksum */
        buildTimeChecksum = ((((uint32_t)checkData[i + 3]) << 24) | (((uint32_t)checkData[i + 2]) << 16) | (((uint32_t)checkData[i + 1]) << 8) | ((uint32_t)checkData[i + 0]));
        calculatedChecksum = ((((uint32_t)checkData[i + 7]) << 24) | (((uint32_t)checkData[i + 6]) << 16) | (((uint32_t)checkData[i + 5]) << 8) | ((uint32_t)checkData[i + 4]));

#if ADI_ADRV9001_SW_TEST > 0
        if (device->devStateInfo.swTest == 8)
        {
            calculatedChecksum = 12345678;
            buildTimeChecksum = calculatedChecksum + 1;
        }
#endif
        checksum->adcProfilefwCheckSum.buildChecksum = buildTimeChecksum;
        checksum->adcProfilefwCheckSum.runChecksum = calculatedChecksum;

#if ADI_ADRV9001_PROFILE_CHECKSUM_ENABLE > 0
        /* performing checksum check */
        if (buildTimeChecksum != calculatedChecksum)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                calculatedChecksum,
                "Adc Profile checksum is invalid");
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
    extData[1] = ADRV9001_ARM_OBJECTID_DEVICE_PROFILE;

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

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);

    /* Executing the SET System Config command */
    extData[0] = channelMask;
    extData[1] = ADRV9001_ARM_OBJECTID_SYSTEM_CONFIG;

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

int32_t adi_adrv9001_arm_NextDynamicProfile_Set(adi_adrv9001_Device_t *device,
                                                uint8_t dynamicProfileIndex,
                                                const adi_adrv9001_Init_t *init)
{
    uint8_t extData[5] = { 0 };

    /* Check input pointers are not null */
    ADI_ENTRY_PTR_EXPECT(device, init);

    /* Send the profile data */
    ADI_EXPECT(adrv9001_DynamicProfile_Write, device, dynamicProfileIndex, init);

    /* Execute the SET:DYNAMIC_PROFILE command */
    extData[0] = 0xFF;
    extData[1] = ADRV9001_ARM_OBJECTID_DYNAMIC_PROFILE;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_SETDYNAMICPROFILE_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_NextPfir_Set(adi_adrv9001_Device_t *device,
                                      uint8_t channelMask,
                                      const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff)
{
    uint8_t extData[5] = { 0 };

    /* Check input pointers are not null */
    ADI_ENTRY_PTR_EXPECT(device, pfirCoeff);

    /* Check the channelMask refers to valid channels (bits 0 to 3) and is not empty */
    ADI_RANGE_CHECK(device, channelMask, 1, 15)

    /* Send the PFIR data */
    ADI_EXPECT(adrv9001_PfirWbNbBuffer_Write, device, pfirCoeff);

    /* Execute the SET:DYNAMIC_PROFILE:channelMask command */
    extData[0] = channelMask;
    extData[1] = ADRV9001_ARM_OBJECTID_DYNAMIC_PROFILE;
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

static int32_t __maybe_unused adi_adrv9001_arm_ChannelPowerSaving_Configure_Validate(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    uint8_t chan_index = 0;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    adi_adrv9001_GpioCfg_t gpio = { 0 };
    adi_adrv9001_GpioSignal_e signal = ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL1;
    if (ADI_CHANNEL_2 == channel)
    {
        signal = ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL2;
    }

    ADI_NULL_PTR_RETURN(&device->common, powerSavingCfg);

    /* Check for valid channel */
    ADI_PERFORM_VALIDATION(adi_adrv9001_Channel_Validate, device, channel);

    /* 'channelDisabledPowerDownMode' and 'gpioPinPowerDownMode' are restricted between ADI_ADRV9001_POWERDOWNMODE_DISABLED and ADI_ADRV9001_POWERDOWNMODE_LDO */
    ADI_RANGE_CHECK(device,
                    powerSavingCfg->channelDisabledPowerDownMode,
                    ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_DISABLED,
                    ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_LDO);

    ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, signal, &gpio);

    if (powerSavingCfg->gpioPinPowerDownMode != ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_DISABLED)
    {
        if (ADI_ADRV9001_GPIO_UNASSIGNED == gpio.pin)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                powerSavingCfg->gpioPinPowerDownMode,
                "If gpioPinPowerDownMode is not POWERDOWN_DISABLED, then powerSavingGpio pin must be ASSIGNED.");
            ADI_API_RETURN(device)

        }
        /* gpioPinPowerDownMode must be greater than channelDisabledPowerDownMode */
        ADI_RANGE_CHECK(device,
                        powerSavingCfg->gpioPinPowerDownMode,
                        powerSavingCfg->channelDisabledPowerDownMode + 1,
                        ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_LDO);
    }

    adi_common_channel_to_index(channel, &chan_index);
    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to configure power saving. Rx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }


    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to configure power saving. Tx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_ChannelPowerSaving_Configure(adi_adrv9001_Device_t *device,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv9001_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    uint8_t armData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_ChannelPowerSaving_Configure_Validate, device, channel, powerSavingCfg);

    armData[0] = (uint8_t)channel;
    armData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_POWER_SAVING_CONFIG;
    armData[2] = (uint8_t)powerSavingCfg->channelDisabledPowerDownMode;
    armData[3] = (uint8_t)powerSavingCfg->gpioPinPowerDownMode;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, armData, sizeof(armData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        armData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_arm_ChannelPowerSaving_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                   adi_common_ChannelNumber_e channel,
                                                                                   adi_adrv9001_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    uint8_t chan_index = 0;

    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, powerSavingCfg);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to inspect power saving configuration. Rx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to inspect power saving configuration. Specified channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_ChannelPowerSaving_Inspect(adi_adrv9001_Device_t *device,
                                                     adi_common_ChannelNumber_e channel,
                                                     adi_adrv9001_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_ChannelPowerSaving_Inspect_Validate, device, channel, powerSavingCfg);

    extData[0] = (uint8_t)channel;
    extData[1] = ADRV9001_ARM_OBJECTID_GET_POWER_SAVING_CONFIG;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false)

    powerSavingCfg->channelDisabledPowerDownMode = (adi_adrv9001_ChannelPowerDownMode_e)armReadBack[0];
    powerSavingCfg->gpioPinPowerDownMode = (adi_adrv9001_ChannelPowerDownMode_e)armReadBack[1];

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused SystemPowerSavingAndMonitorMode_Configure_Validate(adi_adrv9001_Device_t *device,
    adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    adi_adrv9001_GpioCfg_t gpio = { 0 };

    /* Rx1 must be enabled to support Monitor Mode feature */
    if ((monitorModeCfg->detectionTime_us!= 0) && 
        (0 == ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_RX1)))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device->devStateInfo.initializedChannels,
            "Rx1 channel must be initialized to support Monitor Mode feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_NULL_PTR_RETURN(&device->common, monitorModeCfg);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->powerDownMode,
                    ADI_ADRV9001_SYSTEM_POWER_DOWN_MODE_CLKPLL,
                    ADI_ADRV9001_SYSTEM_POWER_DOWN_MODE_ARM);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->detectionMode,
                    ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI,
                    ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI_FFT);

    ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, ADI_ADRV9001_GPIO_SIGNAL_MON_ENABLE_SPS, &gpio);
    ADI_RANGE_CHECK(device, gpio.pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_SystemPowerSavingAndMonitorMode_Configure(adi_adrv9001_Device_t *device,
    adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(SystemPowerSavingAndMonitorMode_Configure_Validate, device, monitorModeCfg);

    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->initialBatterySaverDelay_us);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->detectionTime_us);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->sleepTime_us);
    armData[offset++] = (uint8_t)monitorModeCfg->detectionDataBufferEnable;
    armData[offset++] = (uint8_t)monitorModeCfg->externalPllEnable;

    /* Write monitor mode configuration parameters to ARM data memory */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_HIGHPRIORITY_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = 0;
    extData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_MONITOR_MODE_CONFIG;
    extData[2] = (uint8_t)(monitorModeCfg->powerDownMode);
    extData[3] = monitorModeCfg->detectionFirst;
    extData[4] = (uint8_t)(monitorModeCfg->detectionMode);

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_SystemPowerSavingAndMonitorMode_Inspect(adi_adrv9001_Device_t *device,
    adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armReadBack[17] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, monitorModeCfg);

    extData[0] = 0;
    extData[1] = ADRV9001_ARM_OBJECTID_GET_MONITOR_MODE_CONFIG;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false)

    monitorModeCfg->powerDownMode = (adi_adrv9001_SystemPowerDownMode_e)armReadBack[offset++];
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->initialBatterySaverDelay_us);
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->detectionTime_us);
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->sleepTime_us);
    monitorModeCfg->detectionFirst = armReadBack[offset++];
    monitorModeCfg->detectionMode = (adi_adrv9001_MonitorDetectionMode_e)armReadBack[offset++];
    monitorModeCfg->detectionDataBufferEnable = (armReadBack[offset++] == 1) ? true:false;
    monitorModeCfg->externalPllEnable = (armReadBack[offset++] == 1) ? true : false;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_SystemPowerSavingMode_Set(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_SystemPowerDownMode_e mode)
{
    adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t config = {
        .initialBatterySaverDelay_us = 0,
        .detectionTime_us = 0,
        .sleepTime_us = 0xFFFFFFFF,
        .detectionFirst = 0,
        .detectionMode = ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI,
        .detectionDataBufferEnable = 0
    };

    config.powerDownMode = mode;
    ADI_EXPECT(adi_adrv9001_arm_SystemPowerSavingAndMonitorMode_Configure, adrv9001, &config);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_SystemPowerSavingMode_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                          adi_adrv9001_SystemPowerDownMode_e *mode)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, mode);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_SystemPowerSavingMode_Get(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_SystemPowerDownMode_e *mode)
{
    adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t config = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_SystemPowerSavingMode_Get_Validate, adrv9001, mode);

    ADI_EXPECT(adi_adrv9001_arm_SystemPowerSavingAndMonitorMode_Inspect, adrv9001, &config);
    *mode = config.powerDownMode;

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_MonitorMode_Pattern_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                      adi_common_ChannelNumber_e channel,
                                                                                      adi_adrv9001_MonitorModePatternCfg_t *monitorModePattern)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModePattern);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv9001, monitorModePattern->patternLength, ADI_ADRV9001_MONITOR_MODE_PATTERN_LENGTH_240, ADI_ADRV9001_MONITOR_MODE_PATTERN_LENGTH_480)
    ADI_API_RETURN(adrv9001);
}

/* TODO: Change this function to be private when a function to measure
 * the value from CALIBRATED and program the value to the ADRV9001 is added */
int32_t adi_adrv9001_arm_MonitorMode_Pattern_Configure(adi_adrv9001_Device_t *adrv9001,
                                                       adi_common_ChannelNumber_e channel,
                                                       adi_adrv9001_MonitorModePatternCfg_t *monitorModePattern)
{
    uint16_t pattern = 0;
    uint16_t patternLength = 240;
    uint32_t readPattern = 0;
    adrv9001_BfNvsRegmapRx_e baseAddress = ADRV9001_BF_RX1_CORE;
    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_MonitorMode_Pattern_Configure_Validate, adrv9001, channel, monitorModePattern);
    if (ADI_CHANNEL_2 == channel)
    {
        baseAddress = ADRV9001_BF_RX2_CORE;
    }
    /* Select DpinFIFO test pattern; 1: select test pattern. 0: select datapath data */
    ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataSel_Set, adrv9001, baseAddress, 0x1);
    /* 1: enable DpinFIFO block 0: disable DPinFIFO */
    ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoEn_Set, adrv9001, baseAddress, 0x1);

    if (ADI_ADRV9001_MONITOR_MODE_PATTERN_LENGTH_480 == monitorModePattern->patternLength)
    {
        patternLength = 480;
    }

    for (pattern = 0; pattern < patternLength; pattern++)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataI_Set, adrv9001, baseAddress, monitorModePattern->patternI[pattern]);
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataI_Get, adrv9001, baseAddress, &readPattern);
        if (readPattern != monitorModePattern->patternI[pattern])
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern I is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);
        }
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataQ_Set, adrv9001, baseAddress, monitorModePattern->patternQ[pattern]);
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataQ_Get, adrv9001, baseAddress, &readPattern);
        if (readPattern != monitorModePattern->patternQ[pattern])
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern Q is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);
        }

        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestDataUpdate_Set, adrv9001, baseAddress, 0x1);
    }
    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_MonitorMode_Vector_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                     adi_common_ChannelNumber_e channel,
                                                                                     adi_adrv9001_MonitorModeVectorCfg_t *monitorModeVector)
{
    static uint16_t VECTOR_MASK_MAX = 0x3FFF;
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeVector);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv9001, monitorModeVector->vectorMask, 0x1, VECTOR_MASK_MAX);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_MonitorMode_Vector_Configure(adi_adrv9001_Device_t *adrv9001,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_adrv9001_MonitorModeVectorCfg_t *monitorModeVector)
{
    uint8_t mask = 0;
    uint8_t nibSel = 0;
    uint8_t vectorCrumb = 0;
    uint8_t vectorNibble = 0;
    uint16_t bfValue = 0;
    uint32_t vectorConverted = 0;
    static const uint8_t MONITOR_MODE_VECTOR_MAX = 14;
    static const uint8_t MAX_NIBBLE_PER_VECTOR = 12;
    adrv9001_BfNvsRegmapRx_e baseAddress = ADRV9001_BF_RX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_MonitorMode_Vector_Configure_Validate, adrv9001, channel, monitorModeVector);
    if (ADI_CHANNEL_2 == channel)
    {
        baseAddress = ADRV9001_BF_RX2_CORE;
    }

    /* Hard code D14 and D15 = 0x1 */
    bfValue = monitorModeVector->vectorMask | 0xC000;
    ADI_EXPECT(adrv9001_NvsRegmapRx_CorrCtrl_Set, adrv9001, baseAddress, bfValue);

    for (mask = 0; mask < MONITOR_MODE_VECTOR_MAX; mask++)
    {
        if (bfValue & (1 << mask))
        {
            for (nibSel = 0; nibSel < MAX_NIBBLE_PER_VECTOR; nibSel++)
            {
                vectorNibble = (monitorModeVector->vector[mask] >> (nibSel * 4)) & 0xF;
                /* bit3 and bit1 are extracted in each nibble and combined for conversion */
                /* 0x5: 0x00, 0x7:0x1, 0xD:0x2, 0xF:0x3 */
                vectorCrumb = ((vectorNibble & 0x8) >> 2) | ((vectorNibble & 0x2) >> 1);
                vectorConverted |= (vectorCrumb << (nibSel * 2));
            }
            ADI_EXPECT(adrv9001_NvsRegmapRx_Vcorrsig_Set, adrv9001, baseAddress, mask, vectorConverted);
            vectorConverted = 0;
        }
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_MonitorMode_Vector_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                   adi_common_ChannelNumber_e channel,
                                                                                   adi_adrv9001_MonitorModeVectorCfg_t *monitorModeVector)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeVector);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_MonitorMode_Vector_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_adrv9001_MonitorModeVectorCfg_t *monitorModeVector)
{
    uint8_t mask = 0;
    uint8_t crumbSel = 0;
    uint8_t vectorCrumb = 0;
    uint8_t vectorLut[4] = { 0x5, 0x7, 0xD, 0xF };
    uint16_t bfValue = 0;
    uint32_t readvector = 0;
    uint64_t vectorConverted = 0;
    uint64_t tempVector = 0;
    static const uint8_t MONITOR_MODE_VECTOR_MAX = 14;
    static const uint8_t MAX_CRUMB_PER_READVECTOR = 12;

    adrv9001_BfNvsRegmapRx_e baseAddress = ADRV9001_BF_RX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_MonitorMode_Vector_Inspect_Validate, adrv9001, channel, monitorModeVector);
    if (ADI_CHANNEL_2 == channel)
    {
        baseAddress = ADRV9001_BF_RX2_CORE;
    }

    ADI_EXPECT(adrv9001_NvsRegmapRx_CorrCtrl_Get, adrv9001, baseAddress, &bfValue);
    monitorModeVector->vectorMask = bfValue & 0x3FFF;

    for (mask = 0; mask < MONITOR_MODE_VECTOR_MAX; mask++)
    {
        if (bfValue & (1 << mask))
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_Vcorrsig_Get, adrv9001, baseAddress, mask, &readvector);

            for (crumbSel = 0; crumbSel < MAX_CRUMB_PER_READVECTOR; crumbSel++)
            {
                vectorCrumb = (readvector >> (crumbSel * 2)) & 0x3;
                tempVector = (uint64_t)vectorLut[vectorCrumb] << (crumbSel * 4);
                vectorConverted |= tempVector;
            }
            monitorModeVector->vector[mask] = vectorConverted;
            vectorConverted = 0;
        }
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_MonitorMode_Rssi_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                   adi_adrv9001_arm_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t port = 0;
    uint8_t channel = 0;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    uint32_t MEASUREMENT_DURATION_SAMPLES_MAX = 0x1FFFFF;
    int32_t  DETECTION_THRESHOLD_MIN = -140000;
    adi_common_Port_e ports[2] = { ADI_RX, ADI_TX };
    adi_common_ChannelNumber_e channels[2] = { ADI_CHANNEL_1, ADI_CHANNEL_2 };
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeRssiCfg);

    ADI_RANGE_CHECK(adrv9001, monitorModeRssiCfg->measurementDuration_samples, 0x2, MEASUREMENT_DURATION_SAMPLES_MAX);
    ADI_RANGE_CHECK(adrv9001, monitorModeRssiCfg->detectionThreshold_mdBFS, DETECTION_THRESHOLD_MIN, 0);

    if (0 == monitorModeRssiCfg->numberOfMeasurementsToAverage)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->numberOfMeasurementsToAverage,
            "'numberOfMeasurementsToAverage' cannot be 0.");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }

    /* TODO: Remove this check in future.
     * measurementsStartPeriod_ms = 0: Continuous RSSI measurements (currently not supported by FW) */
    if (0 == monitorModeRssiCfg->measurementsStartPeriod_ms)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->measurementsStartPeriod_ms,
            "'measurementsStartPeriod_ms = 0' is currently not supported by FW.");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }

    /* Validate state is STANDBY */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &state);
    for (port = 0; port < ADI_ARRAY_LEN(ports); port++)
    {
        for (channel = 0; channel < ADI_ARRAY_LEN(channels); channel++)
        {
            adi_common_port_to_index(ports[port], &port_index);
            adi_common_channel_to_index(channels[channel], &chan_index);
            if (ADI_ADRV9001_CHANNEL_STANDBY != state.channelStates[port_index][chan_index])
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channel,
                                 "Invalid channel state. Channel must be in STANDBY state");
                ADI_ERROR_RETURN(adrv9001->common.error.newAction);
            }
        }
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_arm_MonitorMode_Rssi_Configure(adi_adrv9001_Device_t *adrv9001,
                                                    adi_adrv9001_arm_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_MonitorMode_Rssi_Configure_Validate, adrv9001, monitorModeRssiCfg);

    offset += 4;
    armData[offset++] = monitorModeRssiCfg->numberOfMeasurementsToAverage;
    armData[offset++] = monitorModeRssiCfg->measurementsStartPeriod_ms;
    offset += 2;
    adrv9001_LoadFourBytes(&offset, armData, monitorModeRssiCfg->measurementDuration_samples);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeRssiCfg->detectionThreshold_mdBFS);

    extData[0] = 0;
    extData[1] = ADRV9001_ARM_OBJECTID_CONFIG;
    extData[2] = OBJID_CFG_MONITOR_MODE_RSSI;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_arm_MonitorMode_Rssi_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                 adi_adrv9001_arm_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    ADI_NULL_PTR_RETURN(&device->common, monitorModeRssiCfg);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_arm_MonitorMode_Rssi_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                  adi_adrv9001_arm_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armReadBack[12] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv9001_arm_MonitorMode_Rssi_Inspect_Validate, adrv9001, monitorModeRssiCfg);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_MONITOR_MODE_RSSI, channelMask, offset, armReadBack, sizeof(armReadBack))

    monitorModeRssiCfg->numberOfMeasurementsToAverage = armReadBack[offset++];
    monitorModeRssiCfg->measurementsStartPeriod_ms = armReadBack[offset++];
    offset += 2;
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeRssiCfg->measurementDuration_samples);
    adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t *)(&monitorModeRssiCfg->detectionThreshold_mdBFS));

    ADI_API_RETURN(adrv9001);
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