/**
 * \file
 * \brief Contains ADRV9001 ARM related function prototypes for adi_adrv9001_arm.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_ARM_H_
#define _ADI_ADRV9001_ARM_H_

#include "adi_adrv9001_arm_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_error.h"
#include "adrv9001_arm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enable the ARM processor
 *
 * Enables AHB SPI bridge
 * Sets ARM Run = 1 to enable ARM
 * Issue SW interrupt 4 to wake up ARM
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Enable(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Disable the ARM processor
 *
 * Sets ARM Run = 0.
 * Disables AHB SPI bridge
 * Polls ISR for ARM is READY_FOR_MCS
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Disable(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Enable AHB SPI bridge
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_AhbSpiBridge_Enable(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Disable AHB Spi Bridge
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_AhbSpiBridge_Disable(adi_adrv9001_Device_t *adrv9001);

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
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] timeout_us		Timeout to stop waiting for ARM to boot up.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_StartStatus_Check(adi_adrv9001_Device_t *adrv9001, uint32_t timeout_us);

/**
 * \brief Check the ARM processor Firmware Status.
 *
 * After ARM Run = 1, then wait and check for ARM FW Status. This function use a SPI read,
 * and can not be use in a SPI write only (Broadcasting) mode.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the device has been initialized and before multichip-sync
 * (MCS) has been completed
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] timeout_us		Timeout to stop waiting for ARM to boot up.
 * \param[in] fwCheckStatus	FW status to stop waiting for ARM to change to.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_FwStatus_Check(adi_adrv9001_Device_t *adrv9001, uint32_t timeout_us, uint32_t fwCheckStatus);

/**
 * \brief Writes the ADRV9001 ARM configuration settings
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] init			Pointer to the ADRV9001 initialization settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Profile_Write(adi_adrv9001_Device_t *adrv9001, const adi_adrv9001_Init_t *init);

/**
 * \brief Writes the ADRV9001 PFIR coefficients in ARM memory
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] init			Pointer to the ADRV9001 initialization settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_PfirProfiles_Write(adi_adrv9001_Device_t *adrv9001, const adi_adrv9001_Init_t *init);

/**
* \brief Loads binary array into ARM program memory
*        'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252' and 'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4' 
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
* \param[in] adrv9001	    Context variable - Pointer to the ADRV9001 device settings data structure
* \param[in] byteOffset		Offset (from 0) in ARM memory to start writing (if loaded in multiple function calls)
* \param[in] binary			Byte array containing all valid ARM file data bytes
* \param[in] byteCount		The number of bytes in the binary array
* \param[in] spiWriteMode   Preferred SPI write mode
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_arm_Image_Write(adi_adrv9001_Device_t *adrv9001,
                                     uint32_t byteOffset,
                                     const uint8_t binary[],
                                     uint32_t byteCount,
                                     adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

/**
 * \brief Sets bit0 of SW_Interrupt_4 register, which issues wake up interrupt to ARM
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_WakeupInterrupt_Set(adi_adrv9001_Device_t *adrv9001);


/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Read from the ADRV9001 ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  address			The 32bit ARM address to read from.
 * \param[out] returnData		Byte array containing the data read from the ARM memory.
 * \param[in]  byteCount			Number of bytes in the returnData array.
 * \param[in]  autoIncrement		Boolean flag to enable or disable auto-increment of ARM register address
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Memory_Read(adi_adrv9001_Device_t *adrv9001,
                                     uint32_t address,
                                     uint8_t returnData[],
                                     uint32_t byteCount,
                                     uint8_t autoIncrement);
/**
 * \brief Read from the ADRV9001 ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  address			The 32bit ARM address to read from.
 * \param[out] returnData		Byte array containing the data read from the ARM memory.
 * \param[in]  byteCount			Number of bytes in the returnData array.
 * \param[in]  autoIncrement		Boolean flag to enable or disable auto-increment of ARM register address
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Memory_Read32(adi_adrv9001_Device_t *adrv9001,
		uint32_t address,
		uint32_t returnData[],
		uint32_t byteCount,
		uint8_t autoIncrement);
/**
 * \brief Write to the ADRV9001 ARM program or data memory
 *        'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252' and 'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4' 
 *        are supported only if byteCount is a multiple of 4
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] address			The 32-bit ARM address to write
 * \param[in] data				Byte array (uint8_t) containing data to be written to ARM memory
 * \param[in] byteCount			Number of bytes in the data array to be written
 * \param[in] spiWriteMode      Preferred SPI write mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Memory_Write(adi_adrv9001_Device_t *adrv9001,
                                      uint32_t address,
                                      const uint8_t data[],
                                      uint32_t byteCount,
                                      adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

/**
* \brief Write to the ADRV9001 ARM program or data memory
*        'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252' and 'ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4' 
*        are supported only if byteCount is a multiple of 4
*
* Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
* Data Memory (0x20000000 - 0x20014000).
*
* \note Message type: \ref timing_direct "Direct register access"
*
* \pre This function is private and is not called directly by the user.
*
* \param[in] adrv9001			            Context variable - Pointer to the ADRV9001 device settings data structure
* \param[in] hopSignal                      Hop signal to configure appropriate tableId
* \param[in] tableId                        FH_HOP_TABLE_A or FH_HOP_TABLE_B. Used for ping-pong hop tables.
* \param[in] hopTableAddress                The 32-bit ARM address to write
* \param[in] numHopTableEntries             Byte Array (uint8_t) containing data to be written to ARM memory
* \param[in] numHopTableEntriesByteCount    Number of bytes in the data array to be written
* \param[in] hopTableBufferAddress          The 32-bit ARM address to write
* \param[in] hopTableBufferData             Byte Array (uint8_t) containing data to be written to ARM memory
* \param[in] hopTableBufferDataByteCount    Number of bytes in the data array to be written
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_arm_Memory_WriteFH(adi_adrv9001_Device_t *adrv9001, 
		                                    adi_adrv9001_FhHopSignal_e hopSignal,
		                                    adi_adrv9001_FhHopTable_e tableId, 
		                                    uint32_t hopTableAddress, 
		                                    const uint8_t numHopTableEntries[], 
		                                    uint32_t numHopTableEntriesByteCount, 
		                                    uint32_t hopTableBufferAddress, 
		                                    const uint8_t hopTableBufferData[], 
		                                    uint32_t hopTableBufferDataByteCount);

/**
 * \brief Low level helper function used by ADRV9001 API to write the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV9001 API commands to write settings into the ARM memory.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] armData			A byte array (uint8_t) containing data to be written to ARM memory
 * \param[in] armDataSize		Number of bytes in armData[] array to be written
 * \param[in] mailboxCmd		A byte array containing data to write to the ARM command interface
 * \param[in] mailboxCmdSize	Number of bytes to write to the ARM command interface
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Config_Write(adi_adrv9001_Device_t *adrv9001, 
                                      const uint8_t armData[],
                                      uint32_t armDataSize,
                                      const uint8_t mailboxCmd[],
                                      uint32_t mailboxCmdSize);

/**
 * \brief Low level helper function used by ADRV9001 API to read the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV9001 API commands to read settings from the ARM memory.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is private and is not called directly by the user
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  objectId			ARM id of a particular structure or setting in ARM memory
 * \param[in]  subObjectId      Additional ARM id of a particular structure or setting in ARM memory
 * \param[in]  channelMask      The mask of Tx/Rx channels
 * \param[in]  byteOffset		Byte offset from the start of the objectId's memory location in ARM memory
 * \param[out] returnData		A byte array containing data read back from the ARM memory buffer
 * \param[in]  byteCount		Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_MailBox_Get(adi_adrv9001_Device_t *adrv9001, 
                                     uint8_t objectId, 
                                     uint8_t subObjectId, 
                                     uint8_t channelMask, 
                                     uint16_t byteOffset, 
                                     uint8_t returnData[], 
                                     uint32_t byteCount);

/**
 * \brief Low level helper function used by ADRV9001 API to read the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV9001 API commands to read settings from the ARM memory.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is private and is not called directly by the user
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  objectId			ARM id of a particular structure or setting in ARM memory
 * \param[in]  channelMask      The mask of Tx/Rx channels
 * \param[in]  byteOffset		Byte offset from the start of the objectId's memory location in ARM memory
 * \param[out] returnData		A byte array containing data read back from the ARM memory buffer
 * \param[in]  byteCount		Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Config_Read(adi_adrv9001_Device_t *adrv9001, 
                                     uint8_t objectId,
                                     uint8_t channelMask,
                                     uint16_t byteOffset,
                                     uint8_t returnData[],
                                     uint32_t byteCount);

/**
 * \brief Reads the ADRV9001 ARM System Error status
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] objectId     The object ID for which the error occurred
 * \param[out] errorCode    The error type within the object
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_SystemError_Get(adi_adrv9001_Device_t *adrv9001, uint8_t *objectId, uint8_t *errorCode);

/**
 * \brief Reads the ADRV9001 ARM 64-bit command status register and returns an error and status word
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
 * \param[in]  adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] errorWord 16-bit error word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if any three-bit error type > 0, where '0' = OK
 * \param[out] statusWord 16-bit pending bits word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if an opcode is pending, where '0' = no pending opcode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of error and status bits in SPI regs */
int32_t adi_adrv9001_arm_CmdStatus_Get(adi_adrv9001_Device_t *adrv9001, uint16_t *errorWord, uint16_t *statusWord);

/**
 * \brief Isolated byte read of the ADRV9001 ARM 64-bit command status register based on the opcode
 *
 * A single byte read is performed on the 64-bit command status register according to
 * the opcode of interest. The pending bit and the error type are extracted from the status
 * register and returned as a single byte in the lower nibble.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  opCode			Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[out] cmdStatByte		Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9001_arm_CmdStatusOpcode_Get(adi_adrv9001_Device_t *adrv9001, uint8_t opCode, uint8_t *cmdStatByte);

#ifndef CLIENT_IGNORE
/**
 * \brief ADRV9001 ARM command status wait function polls command status register until opcode of interest completes
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre ARM firmware load and initialization must take place first before attempting to use this function
 *
 * \param[in]  adrv9001			Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  opCode			Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[out] cmdStatusByte    Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 * \param[in]  timeout_us       Command time-out period in microseconds
 * \param[in]  waitInterval_us  Wait interval time to thread sleep between each check of the ARM command status to prevent SPI read thrashing
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 /* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9001_arm_CmdStatus_Wait(adi_adrv9001_Device_t *adrv9001,
                                        uint8_t opCode,
                                        uint8_t *cmdStatusByte,
                                        uint32_t timeout_us,
                                        uint32_t waitInterval_us);
#endif

/**
 * \brief Sends a command to the ADRV9001 ARM processor interface
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre ARM is initialized
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] opCode            Opcode of interest where only the first 16 even-numbered integers are valid
 * \param[in] extendedData      A byte array containing extended data to write to the ARM command interface
 * \param[in] byteCount         Number of bytes in the extendedData array (Valid size = 0-4 bytes)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Cmd_Write(adi_adrv9001_Device_t *adrv9001, 
                                   uint8_t opCode,
                                   const uint8_t extendedData[],
                                   uint32_t byteCount);

/**
 * \brief Sends a command SET DEVICE_PROFILE to the ADRV9001 ARM processor interface
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001            Context variable - Pointer to the ADRV9001 device data structure containing settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Profile_Program(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Sends a command SET SYSTEM_CONFIG to the ADRV9001 ARM processor interface
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv9001			Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] channelMask       The mask of Tx/Rx channels
 * \parblock              Bit position |  channelMask
 *                      ---------------|----------------
 *                           bit 0     | RX1
 *                           bit 1     | RX2
 *                           bit 2     | TX1
 *                           bit 3     | TX2
 *                           bit 4     | ORX1
 *                           bit 5     | ORX2
 * \endparblock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_System_Program(adi_adrv9001_Device_t *adrv9001, uint8_t channelMask);

 /****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
 * \brief Reads back the version of the ARM binary loaded into the ADRV9001 ARM memory
 *
 * This function reads the ARM memory to read back the major.minor.releaseCandidate
 * version for the ARM binary loaded into ARM memory.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre ARM is initialized
 *
 * \param[in]  adrv9001           Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[out] armVersion		Arm version will be populated here, it is of struct type adi_adrv9001_ArmVersion_t.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Version(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_ArmVersion_t *armVersion);

 /**
 * \brief Verifies the ARM checksum value
 *
 * The checksum which is written into the binary file is verified with the calculated
 * checksum in the ADRV9001 ARM after the binary file has been loaded.  This function
 * will wait for a timeout period for the checksum calculation to occur.  The
 * user can adjust the timeout period and SPI read interval in adi_adrv9001_user.c by
 * adjusting the macros VERIFY_ARM_CHKSUM_TIMEOUT_US and
 * VERIFY_ARM_CHKSUM_INTERVAL_US.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function is called after the ARM binary file has been loaded to verify it's
 * checksum in ARM memory
 *
 * \param[in]  adrv9001           Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[out] checksum			Pointer to adi_adrv9001_ChecksumTable_t data structure containing settings
 * \param[out] checksumValid	Indicate if checksum is valid
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_ChecksumTable_Get(adi_adrv9001_Device_t *adrv9001, 
                                           adi_adrv9001_ChecksumTable_t *checksum,
                                           uint8_t *checksumValid);

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
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[out] mailboxBusy		Whether the mailbox is busy (true) or ready to accept new commands (false)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_MailboxBusy_Get(adi_adrv9001_Device_t *adrv9001, bool *mailboxBusy);

/** \brief Sends the dynamic components of an adi_adrv9001_Init_t to ARM and waits for it to process them
 *
 * \note The profile is not loaded into hardware until #adi_adrv9001_arm_Profile_Switch is called.
 *
 * \pre Only encodes information about channels that are masked 'on' in txInitChannelMask/rxInitChannelMask.
 * This field must be initialized by calling #adi_adrv9001_InitAnalog or #adi_adrv9001_Profiles_Verify.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED.
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] dynamicProfile        ADRV9001 dynamic profile structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_NextDynamicProfile_Set(adi_adrv9001_Device_t *adrv9001, 
                                                const adi_adrv9000_DynamicProfile_t *dynamicProfile);

/** \brief Sends one bank for PFIR coefficients to ARM and waits for it to process them
 *
 * \note ARM prepares the coefficients for all selected channels, but the old coefficients
 * remain in effect until #adi_adrv9001_arm_Profile_Switch is called.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001       Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in]  channelMask  The mask of Tx/Rx channels. For Rx this function updates the channel filters, and
 *                          for Tx it updates Pulse Shaping filters.
 * \param[in]  pfirCoeff    PFIR configuration to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_NextPfir_Set(adi_adrv9001_Device_t *adrv9001, 
                                      uint8_t channelMask, 
                                      const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff);

/** \brief Request ARM to action a pending PFIR change
 *
 * \note Message type: \ref timing_interrupt "Software interrupt"
 *
 * \pre Channel state must be CALIBRATED or PRIMED
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure containing settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Profile_Switch(adi_adrv9001_Device_t *adrv9001);

/** \brief Start the ARM processor
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure containing settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Start(adi_adrv9001_Device_t *adrv9001);

/** \brief Stop the ARM processor
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure containing settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_arm_Stop(adi_adrv9001_Device_t *adrv9001);


#define ADRV9001_ARM_OPCODE_MASK      0xFFFF0000
#define ADRV9001_ARM_OPCODE_SHIFT     16
#define ADRV9001_ARM_OBJ_ID_MASK      0x0000FF00
#define ADRV9001_ARM_OBJ_ID_SHIFT     8
#define ADRV9001_ARM_ERROR_MASK       0x000000FF
#define ADRV9001_ARM_ERROR_SHIFT      0

#define ADRV9001_ARMCMD_ERRCODE(armOpCode, armObjId, armErrorFlag) ((armOpCode << ADRV9001_ARM_OPCODE_SHIFT) | (armObjId << ADRV9001_ARM_OBJ_ID_SHIFT) | armErrorFlag)

#define ADRV9001_OPCODE_VALID(a) (((a) != 0) && (((a) % 2) || ((a) > 30)))

#define ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(devicePtr, opcode, objid, time1, time2) \
{\
    int32_t _recoveryAction = ADI_COMMON_ACT_NO_ACTION; \
    uint8_t _cmdStatusByte = 0; \
    _recoveryAction = adi_adrv9001_arm_CmdStatus_Wait(devicePtr, \
                                                      (uint8_t)opcode, \
                                                      &_cmdStatusByte, \
                                                      (uint32_t)time1, \
                                                      (uint32_t)time2); \
    /* If cmdStatusByte is non-zero then flag an ARM error */ \
    if((_cmdStatusByte >> 1) > 0) \
    { \
        ADI_EXPECT(adrv9001_ArmCmdErrorHandler, devicePtr, ADRV9001_ARMCMD_ERRCODE(opcode, objid, _cmdStatusByte)); \
    } \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, devicePtr->common.error.errCode, _recoveryAction, NULL, devicePtr->common.error.errormessage); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
}

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_ARM_H_ */
