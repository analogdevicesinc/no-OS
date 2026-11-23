/**
 * \file
 * \brief ADRV910X core functionality
 * 
 * Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_ADRV910X_CORE_H_
#define _ADI_ADRV910X_CORE_H_


#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_arm_types.h"
#include "adi_common_types.h"
#include "adrv910x_arm.h"
#include "device_profile_top_t.h"
#include "device_profile_pfir_buffer_t.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NEVIS_PS2
/**
 * \brief Enable AHB SPI bridge
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_AhbSpiBridge_Enable(adi_adrv910x_Device_t *adrv910x);

/**
 * \brief Disable AHB Spi Bridge
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_AhbSpiBridge_Disable(adi_adrv910x_Device_t *adrv910x);

/**
* \brief Loads binary array into ARM program memory
*        'ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4' and 'ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4' 
*        are supported only if byteCount is a multiple of 4
*
* This function sets the ARM DMA control register bits for an ARM memory write, auto-incrementing
* the address. Valid memory addresses are: Program Memory (0x01000000 - 0x01037FFF)
*
* The top level application reads the binary file, parsing it into any array, starting at the first data byte
* on the first line of the file. The array is passed to this function and writes it to ARM program memory. Any non-data
* bytes should be excluded from the binary array when parsing.
*
* \pre This function is called after the device has been initialized, PLL lock status has been verified, and
* the stream binary has been loaded
*
* \param[in] adrv910x	    Context variable - Pointer to the ADRV910X device settings data structure
* \param[in] byteOffset		Offset (from 0) in ARM memory to start writing (if loaded in multiple function calls)
* \param[in] binary			Byte array containing all valid ARM file data bytes
* \param[in] byteCount		The number of bytes in the binary array
* \param[in] spiWriteMode   Preferred SPI write mode
* \param[in] PS             Processor system selection, ADI_PS1 or ADI_PS2
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv910x_arm_Image_Write(adi_adrv910x_Device_t *adrv910x,
                                     uint32_t byteOffset,
                                     const uint8_t binary[],
                                     uint32_t byteCount,
                                     adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode,
		                             adi_common_Processor_Subsystem_e PS);

#endif /* NEVIS_PS2 */
/**
 * \brief Read from the ADRV910X ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  address			The 32bit ARM address to read from.
 * \param[out] returnData		Byte array containing the data read from the ARM memory.
 * \param[in]  byteCount			Number of bytes in the returnData array.
 * \param[in]  autoIncrement		Boolean flag to enable or disable auto-increment of ARM register address
 * \param[in]  PS               Processor system selection, ADI_PS1 or ADI_PS2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_Memory_Read(adi_adrv910x_Device_t *adrv910x,
                                     uint32_t address,
                                     uint8_t returnData[],
                                     uint32_t byteCount,
                                     uint8_t autoIncrement,
		                             adi_common_Processor_Subsystem_e PS);	

/**
 * \brief Write to the ADRV910X ARM program or data memory
 *        'ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4' and 'ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4' 
 *        are supported only if byteCount is a multiple of 4
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] address			The 32-bit ARM address to write
 * \param[in] data				Byte array (uint8_t) containing data to be written to ARM memory
 * \param[in] byteCount			Number of bytes in the data array to be written
 * \param[in] spiWriteMode      Preferred SPI write mode
 * \param[in] PS                Processor system selection, ADI_PS1 or ADI_PS2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_Memory_Write(adi_adrv910x_Device_t *adrv910x,
                                      uint32_t address,
                                      const uint8_t data[],
                                      uint32_t byteCount,
                                      adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode,
		                              adi_common_Processor_Subsystem_e PS);
	
/**
 * \brief Check the ARM processor Firmware Status.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * 
 * After ARM Run = 1, then wait and check for ARM FW Status. This function use a SPI read,
 * and can not be use in a SPI write only (Broadcasting) mode.
 *
 * \pre This function is called after the device has been initialized and before multichip-sync
 * (MCS) has been completed
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] timeout_us		Timeout to stop waiting for ARM to boot up.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_StartStatus_Check(adi_adrv910x_Device_t *adrv910x, uint32_t timeout_us);
	
/** \brief Start the ARM processor
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x     Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in]  PS           Processor system selection, ADI_PS1 or ADI_PS2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
	int32_t adi_adrv910x_arm_Start(adi_adrv910x_Device_t *adrv910x, adi_common_Processor_Subsystem_e PS);
	
/**
 * \brief Reads the ADRV910X ARM System Error status
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] objectId     The object ID for which the error occurred
 * \param[out] errorCode    The error type within the object
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_SystemError_Get(adi_adrv910x_Device_t *adrv910x, uint8_t *objectId, uint8_t *errorCode);

#ifndef NEVIS_PS2
/**
 * \brief Writes the ADRV910X ARM configuration settings
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  profile		Pointer to the ADRV910X profile settings data union
 * \param[in]  PS           Processor subsystem to target, with ADI_PS1 or ADI_PS2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
	int32_t adi_adrv910x_arm_Profile_Write(adi_adrv910x_Device_t *adrv910x, const adi_adrv910x_Profiles_t *profile, adi_common_Processor_Subsystem_e PS);
	
/**
 * \brief Writes the ADRV910X PFIR coefficients in ARM memory
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] pfirBuffer		Pointer to the ADRV910X profile Pfir buffer struct
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_PfirProfiles_Write(adi_adrv910x_Device_t *adrv910x, const pfirBuffer_t *pfirBuffer);

#endif /* NEVIS_PS2 */
/**
 * \brief Reads Arm Mailbox Command Status
 *
 * This function reads Arm Mailbox Command Status from 'arm_command_busy' bit
 * located in 'arm_command' register (0x00c3)
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre ARM is initialized
 *
 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[out] mailboxBusy		Whether the mailbox is busy (true) or ready to accept new commands (false)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_MailboxBusy_Get(adi_adrv910x_Device_t *adrv910x, bool *mailboxBusy);
	
/**
 * \brief Reads the ADRV910X ARM 64-bit command status register and returns an error and status word
 *
 * A 64-bit status register consisting of a pending bit and three-bit error type is read one byte at
 * a time for the first 16 even-numbered opcodes. The function parses the pending bits and error bits into
 * two (2) separate 16-bit words. statusWord contains the status pending bits. errorWord contains
 * a single error bit if the error type > 0 for any three-bit code.
 * Each word is weighted according to the first 16 even-numbered opcodes where,
 * 0x0001 = opcode '0', 0x0002 = opcode '2', 0x0004 = opcode '4', 0x0008 = opcode '6' and so on.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] errorWord 16-bit error word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if any three-bit error type > 0, where '0' = OK
 * \param[out] statusWord 16-bit pending bits word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if an opcode is pending, where '0' = no pending opcode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of error and status bits in SPI regs */
int32_t adi_adrv910x_arm_CmdStatus_Get(adi_adrv910x_Device_t *adrv910x, uint16_t *errorWord, uint16_t *statusWord);

/**
 * \brief Isolated byte read of the ADRV910X ARM 64-bit command status register based on the opcode
 *
 * A single byte read is performed on the 64-bit command status register according to
 * the opcode of interest. The pending bit and the error type are extracted from the status
 * register and returned as a single byte in the lower nibble.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  opCode			Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[out] cmdStatByte		Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv910x_arm_CmdStatusOpcode_Get(adi_adrv910x_Device_t *adrv910x, uint8_t opCode, uint8_t *cmdStatByte);

#ifndef CLIENT_IGNORE
/**
 * \brief ADRV910X ARM command status wait function polls command status register until opcode of interest completes
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre ARM firmware load and initialization must take place first before attempting to use this function
 *
 * \param[in]  adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  opCode			Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[out] cmdStatusByte    Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 * \param[in]  timeout_us       Command time-out period in microseconds
 * \param[in]  waitInterval_us  Wait interval time to thread sleep between each check of the ARM command status to prevent SPI read thrashing
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv910x_arm_CmdStatus_Wait(adi_adrv910x_Device_t *adrv910x,
                                        uint8_t opCode,
                                        uint8_t *cmdStatusByte,
                                        uint32_t timeout_us,
                                        uint32_t waitInterval_us);
#endif

/**
 * \brief Sends a command to the ADRV910X ARM processor interface
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre ARM is initialized
 *
 * \param[in] adrv910x          Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] opCode            Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[in] extendedData      A byte array containing extended data to write to the ARM command interface
 * \param[in] byteCount         Number of bytes in the extendedData array (Valid size = 0-4 bytes)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_Cmd_Write(adi_adrv910x_Device_t *adrv910x, 
                                   uint8_t opCode,
                                   const uint8_t extendedData[],
                                   uint32_t byteCount);

/**
 * \brief Enable JTAG interface for PS2
 *
 * \param[in] adrv910x Context variable - Pointer to the ADRV910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_ConfigJtag_ps2(adi_adrv910x_Device_t *adrv910x);
	
/**
 * \brief Sends a command SET SYSTEM_CONFIG to the ADRV910X ARM processor interface
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] channelMask       The mask of Tx/Rx channels
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_System_Program(adi_adrv910x_Device_t *adrv910x, uint8_t channelMask);

	
#define ADRV910X_ARM_OPCODE_MASK      0xFFFF0000
#define ADRV910X_ARM_OPCODE_SHIFT     16
#define ADRV910X_ARM_OBJ_ID_MASK      0x0000FF00
#define ADRV910X_ARM_OBJ_ID_SHIFT     8
#define ADRV910X_ARM_ERROR_MASK       0x000000FF
#define ADRV910X_ARM_ERROR_SHIFT      0

#define ADRV910X_ARMCMD_ERRCODE(armOpCode, armObjId, armErrorFlag) ((armOpCode << ADRV910X_ARM_OPCODE_SHIFT) | (armObjId << ADRV910X_ARM_OBJ_ID_SHIFT) | armErrorFlag)

#define ADRV910X_OPCODE_VALID(a) (((a) != 0) && (((a) % 2) || ((a) > 30)))

#define ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(devicePtr, opcode, objid, time1, time2) \
{\
    int32_t _recoveryAction = ADI_COMMON_ACT_NO_ACTION; \
    uint8_t _cmdStatusByte = 0; \
    _recoveryAction = adi_adrv910x_arm_CmdStatus_Wait(devicePtr, \
                                                      (uint8_t)opcode, \
                                                      &_cmdStatusByte, \
                                                      (uint32_t)time1, \
                                                      (uint32_t)time2); \
    /* If cmdStatusByte is non-zero then flag an ARM error */ \
    if((_cmdStatusByte >> 1) > 0) \
    { \
        ADI_EXPECT(adrv910x_ArmCmdErrorHandler, devicePtr, ADRV910X_ARMCMD_ERRCODE(opcode, objid, _cmdStatusByte)); \
    } \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, devicePtr->common.error.errCode, _recoveryAction, NULL, devicePtr->common.error.errormessage); \
    ADI_MUTEX_ERROR_RELEASE(devicePtr) \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
}
	
#ifdef __cplusplus
}
#endif

#endif // !_ADI_ADRV910X_CORE_H_
