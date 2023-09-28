/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_arm.h
 * \brief Contains ADRV9025 ARM related function prototypes for adi_adrv9025_arm.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_ARM_H_
#define _ADI_ADRV9025_ARM_H_

#include "adi_adrv9025_arm_types.h"
#include "adi_adrv9025_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Start the ARM processor
 *
 * Sets ARM Run = 1. Then wait and check for ARM FW Status.
 *
 *
 * \pre This function is called after the device has been initialized and before multichip-sync
 * (MCS) has been completed
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{init-> (most members)}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device settings data structure
 * \param init Pointer to the ADRV9025 initialization setting data structure
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmStart(adi_adrv9025_Device_t*     device,
                              const adi_adrv9025_Init_t* init);

/**
 * \brief Check the ARM processor Firmware Status.
 *
 * After ARM Run = 1, then wait and check for ARM FW Status. This function use a SPI read,
 * and can not be use in a SPI write only (Broadcasting) mode.
 *
 * \pre This function is called after the device has been initialized and before multichip-sync
 * (MCS) has been completed
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device settings data structure
 * \param timeout_us Timeout to stop waiting for ARM to boot up.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmStartStatusCheck(adi_adrv9025_Device_t* device,
                                         uint32_t               timeout_us);

/**
 * \brief Writes the ADRV9025 ARM configuration settings
 *
 * \pre This function is called automatically during adi_adrv9025_ArmStart(), and
 *      this function must be called after loading the ARM firmware.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep{init-> (most members)}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device settings data structure
 * \param init Pointer to the ADRV9025 initialization settings data structure
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmProfileWrite(adi_adrv9025_Device_t*     device,
                                     const adi_adrv9025_Init_t* init);

/**
 * \brief Loads binary array into ARM program memory
 *
 * This function sets the ARM DMA control register bits for an ARM memory write, auto-incrementing
 * the address. Valid memory addresses are: Program Memory (0x01018000 - 0x0104FFFF)
 *
 * The top level application reads the binary file, parsing it into any array, starting at the first data byte
 * on the first line of the file. The array is passed to this function and writes it to ARM program memory. Any non-data
 * bytes should be excluded from the binary array when parsing.
 *
 * \pre This function is called after the device has been initialized, PLL lock status has been verified, and
 * the stream binary has been loaded
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device data structure containing settings
 * \param byteOffset Offset (starting from 0) of where to place the binary
 *                   array in ARM memory (if loaded in multiple function
                     calls)
 * \param binary Byte array containing all valid ARM file data bytes
 * \param byteCount The number of bytes in the binary array
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmImageWrite(adi_adrv9025_Device_t* device,
                                   uint32_t               byteOffset,
                                   const uint8_t          binary[],
                                   uint32_t               byteCount);

/**
 * \brief Loads the ADC profile data into ARM memory
 *
 * This function writes ADC profile into the ARM data memory before
 * loading the ARM firmware.  When the ARM firmware is loaded and begins
 * to initialize, the ARM init sequence copies the ADC profiles from ARM
 * memory and loads them into the ARM firmware internal data strucutures.
 *
 * \pre This function is called during device initialization after the ARM binary is loaded
 * \pre adcProfiles should be initialized using adi_adrv9025_AdcProfilesInit, prior to this function being invoked
 *
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the ADRV9025 device settings data structure
 * \param adcProfiles Pointer to the ADRV9025 ADC profile data structure
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_AdcProfilesWrite(adi_adrv9025_Device_t*            device,
                                      const adi_adrv9025_AdcProfiles_t* adcProfiles);

/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Read from the ADRV9025 ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01018000 - 0x0104FFFF),
 * Data Memory (0x20028000 - 0x2004FFFF).
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param address The 32bit ARM address to read from.
 * \param returnData Byte(uint8_t) array containing the data read from the ARM memory.
 * \param byteCount Number of bytes in the returnData array.
 * \param autoIncrement is boolean flag to enable or disable auto-increment of ARM register address
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmMemRead(adi_adrv9025_Device_t* device,
                                uint32_t               address,
                                uint8_t                returnData[],
                                uint32_t               byteCount,
                                uint8_t                autoIncrement);

/**
 * \brief Write to the ADRV9025 ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01018000 - 0x0104FFFF),
 * Data Memory (0x20028000 - 0x2004FFFF).
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param address The 32-bit ARM address to write
 * \param data Byte array (uint8_t) containing data to be written to ARM memory
 * \param byteCount Number of bytes in the data array to be written
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmMemWrite(adi_adrv9025_Device_t* device,
                                 uint32_t               address,
                                 const uint8_t          data[],
                                 uint32_t               byteCount);

/**
 * \brief Low level helper function used by ADRV9025 API to write the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV9025 API commands to write settings into the ARM memory.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param byteOffset Byte offset from the start of the objectId's memory location in ARM memory
 * \param data A byte array containing data to write to the ARM memory buffer.
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ArmConfigWrite(adi_adrv9025_Device_t* device,
                                    uint8_t                objectId,
                                    uint16_t               byteOffset,
                                    const uint8_t          data[],
                                    uint8_t                byteCount);

/**
 * \brief Low level helper function used by ADRV9025 API to read the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV9025 API commands to read settings from the ARM memory.
 *
 * \pre This function is private and is not called directly by the user
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param byteOffset Byte offset from the start of the objectId's memory location in ARM memory
 * \param returnData A byte array containing data read back from the ARM memory buffer
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmConfigRead(adi_adrv9025_Device_t* device,
                                   uint8_t                objectId,
                                   uint16_t               byteOffset,
                                   uint8_t                returnData[],
                                   uint8_t                byteCount);

/**
 * \brief Reads the ADRV9025 ARM 64-bit command status register and returns an error and status word
 *
 * A 64-bit status register consisting of a pending bit and three-bit error type is read one byte at
 * a time for the first 16 even-numbered opcodes. The function parses the pending bits and error bits into
 * two (2) separate 16-bit words. statusWord contains the status pending bits. errorWord contains
 * a single error bit if the error type > 0 for any three-bit code.
 * Each word is weighted according to the first 16 even-numbered opcodes where,
 * 0x0001 = opcode '0', 0x0002 = opcode '2', 0x0004 = opcode '4', 0x0008 = opcode '6' and so on.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param errorWord 16-bit error word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if any three-bit error type > 0, where '0' = OK
 * \param statusWord 16-bit pending bits word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if an opcode is pending, where '0' = no pending opcode
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Need to review number of error and status bits in SPI regs */
int32_t adi_adrv9025_ArmCmdStatusGet(adi_adrv9025_Device_t* device,
                                     uint16_t*              errorWord,
                                     uint16_t*              statusWord);

/**
 * \brief Isolated byte read of the ADRV9025 ARM 64-bit command status register based on the opcode
 *
 * A single byte read is performed on the 64-bit command status register according to
 * the opcode of interest. The pending bit and the error type are extracted from the status
 * register and returned as a single byte in the lower nibble.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param cmdStatByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9025_ArmCmdStatusOpcodeGet(adi_adrv9025_Device_t* device,
                                           uint8_t                opCode,
                                           uint8_t*               cmdStatByte);

/**
 * \brief ADRV9025 ARM command status wait function polls command status register until opcode of interest completes
 *
 * \pre ARM firmware load and initialization must take place first before attempting to use this function
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param cmdStatusByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 * \param timeout_us Command time-out period in microseconds
 * \param waitInterval_us Wait interval time to thread sleep between each check of the ARM command status to prevent SPI read thrashing
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 *
 * TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9025_ArmCmdStatusWait(adi_adrv9025_Device_t* device,
                                      uint8_t                opCode,
                                      uint8_t*               cmdStatusByte,
                                      uint32_t               timeout_us,
                                      uint32_t               waitInterval_us);

/**
 * \brief Sends a command to the ADRV9025 ARM processor interface
 *
 * \pre This function can be called after initializing the ARM processor
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the ADRV9025 data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param extendedData A byte array containing extended data to write to the ARM command interface
 * \param byteCount Number of bytes in the extendedData array (Valid size = 0-4 bytes)
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmCmdWrite(adi_adrv9025_Device_t* device,
                                 uint8_t                opCode,
                                 const uint8_t          extendedData[],
                                 uint8_t                byteCount);

/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
* \brief Reads back the version of the ARM binary loaded into the ADRV9025 ARM memory
*
* This function reads the ARM memory to read back the major.minor.releaseCandidate
* version for the ARM binary loaded into ARM memory.
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param armVersion Arm version will be populated here, it is of struct type adi_adrv9025_ArmVersion_t.
*
* \retval ADRV9025_ERR_OK Function completed successfully
* \retval ADRV9025_ERR_GETARMVER_NULL_PARM One of the function parameters has a NULL pointer
*/
int32_t adi_adrv9025_ArmVersionGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_ArmVersion_t* armVersion);

/**
 * \brief Verifies the ARM checksum value
 *
 * The checksum which is written into the binary file is verified with the calculated
 * checksum in the ADRV9025 ARM after the binary file has been loaded.  This function
 * will wait for a timeout period for the checksum calculation to occur.  The
 * user can adjust the timeout period and SPI read interval in adi_adrv9025_user.c by
 * adjusting the macros VERIFY_ARM_CHKSUM_TIMEOUTUS and
 * VERIFY_ARM_CHKSUM_INTERVALUS.
 *
 * \pre This function is called after the ARM binary file has been loaded to verify it's
 * checksum in ARM memory
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is structure pointer to the ADRV9025 data structure containing settings
 * \param checksum Pointer to adi_adrv9025_ChecksumTable_t data structure containing settings
 * \param checksumValid Indicate if a valid checksum
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmChecksumTableGet(adi_adrv9025_Device_t*        device,
                                         adi_adrv9025_ChecksumTable_t* checksum,
                                         uint8_t*                      checksumValid);

/**
 * \brief Reads Arm Mailbox Command Status
 *
 * This function reads Arm Mailbox Command Status from 'arm_command_busy' bit
 * located in 'arm_command' register (0x00c3)
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is structure pointer to the ADRV9025 data structure containing settings
 * \param mailboxBusy Pointer to byte containing mailbox busy status, 0:Arm is ready to accept a new command, 1:Arm is busy and cannot accept a new command
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ArmMailboxBusyGet(adi_adrv9025_Device_t* device,
                                       uint8_t*               mailboxBusy);

/**
* \brief Reads Arm System error message from ARM-C.
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param errorMessage Pointer to character array containing the error message
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ArmSystemErrorGet(adi_adrv9025_Device_t* device,
                                       const char**           errorMessage);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_ARM_H_ */
