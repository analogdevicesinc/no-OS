/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_cpu.h
* \brief Contains ADRV9025 processor function prototypes for
*    adi_adrv9025_cpu.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9025_CPU_H_
#define _ADI_ADRV9025_CPU_H_

#include "adi_adrv9025_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv9025_cpu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
* Initialization functions
****************************************************************************
*/

/**
 * \brief Start the CPU(s)
 *
 * Sets run bit to 1. Then wait and check for FW Status.
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
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_CpuStart(
    adi_adrv9025_Device_t* device);

/**
* \brief Loads binary array into CPU program memory
*
* This function sets the CPU DMA control register bits for an CPU memory write, auto-incrementing
* the address. Valid memory addresses are: Program Memory (0x01000000 - 0x0104FFFF)
*
* The top level application reads the binary file, parsing it into any array, starting at the first data byte
* on the first line of the file. The array is passed to this function and writes it to CPU program memory. Any non-data
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
* \param cpuType type of cpu
* \param byteOffset Offset (starting from 0) of where to place the binary
*                   array in CPU memory (if loaded in multiple function
*                   calls)
* \param binary Byte array containing all valid FW file data bytes
* \param byteCount The number of bytes in the binary array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuImageWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t               byteOffset,
    const uint8_t          binary[],
    uint32_t               byteCount);

/**
* \brief Writes the ADRV9025 CPU configuration settings
*
* \pre This function is called automatically during adi_adrv9025_CpuStart(), and
*      this function must be called after loading the firmware.
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
int32_t adi_adrv9025_CpuProfileWrite(
    adi_adrv9025_Device_t*     device,
    const adi_adrv9025_Init_t* init);

/**
* \brief Loads the ADC profile data into CPU memory
*
* This function writes ADC profile into the CPU data memory before
* loading the firmware.  When the firmware is loaded and begins
* to initialize, the CPU init sequence copies the ADC profiles from CPU
* memory and loads them into the firmware internal data strucutures.
*
* \pre This function is called during device after downloading FW code
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
int32_t adi_adrv9025_CpuAdcProfilesWrite(
    adi_adrv9025_Device_t*            device,
    const adi_adrv9025_AdcProfiles_t* adcProfiles);

/**
* \brief Check the Firmware Status.
*
* After setting run bit to 1, then wait and check for FW Status. This function does a SPI read,
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
* \param timeout_us Timeout to stop waiting for CPU to boot up.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuStartStatusCheck(
    adi_adrv9025_Device_t* device,
    uint32_t               timeout_us);

/****************************************************************************
* Runtime functions
****************************************************************************
*/

/****************************************************************************
* Helper functions
****************************************************************************
*/

/**
* \brief Write to the ADRV9025 CPU program or data memory
*
* Valid memory addresses are: Program Memory (0x01000000 - 0x0104FFFF),
* Data Memory (0x20000000 - 0x2004FFFF).
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param address The 32-bit CPU memory address to write
* \param data Byte array (uint8_t) containing data to be written to CPU memory
* \param byteCount Number of bytes in the data array to be written
* \param autoIncrement is boolean flag to enable or disable auto-increment of CPU register address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuMemWrite(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoIncrement);

/**
* \brief Read from the ADRV9025 CPU program or data memory
*
* Valid memory addresses are: Program Memory (0x01000000 - 0x0104FFFF),
* Data Memory (0x20000000 - 0x2004FFFF).
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param address The 32bit CPU memory address to read from.
* \param returnData Byte(uint8_t) array containing the data read from the CPU memory.
* \param byteCount Number of bytes in the returnData array.
* \param autoIncrement is boolean flag to enable or disable auto-increment of CPU register address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuMemRead(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement);

/**
* \brief Sends a command to the ADRV9025 CPU interface
*
* \pre This function can be called after initializing the CPU
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param cpuType type of cpu
* \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
* \param extendedData A byte array containing extended data to write to the CPU command interface
* \param byteCount Number of bytes in the extendedData array (Valid size = 0-4 bytes)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuCmdWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                opCode,
    const uint8_t          extendedData[],
    uint8_t                byteCount);

/**
* \brief Isolated byte read of the ADRV9025 CPU 64-bit command status register based on the opcode
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
* \param cpuType Type of cpu
* \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
* \param cmdStatByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9025_CpuCmdStatusOpcodeGet(adi_adrv9025_Device_t *device,
                                           adi_adrv9025_CpuType_e cpuType,
                                           uint8_t                opCode,
                                           uint8_t               *cmdStatByte);

/**
* \brief ADRV9025 CPU command status wait function polls command status register until opcode of interest completes
*
* \pre firmware load and initialization must take place first before attempting to use this function
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param cpuType Type of cpu
* \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
* \param cmdStatusByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
* \param timeout_us Command time-out period in microseconds
* \param waitInterval_us Wait interval time to thread sleep between each check of the CPU command status to prevent SPI read thrashing
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
* TODO: Need to review number of bits SPI regs per opcode */
int32_t adi_adrv9025_CpuCmdStatusWait(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                opCode,
    uint8_t*               cmdStatusByte,
    uint32_t               timeout_us,
    uint32_t               waitInterval_us);

/**
* \brief Sends a command to the ADRV9025 CPU interface and polls command status register until opcode of interest completes
*
* \pre This function can be called after initializing the CPU
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param cpuType type of cpu
* \param armOpCode Opcode of interest where only the first 16 even-numbered integers are valid
* \param extendedData A byte array containing extended data to write to the CPU command interface
* \param byteCount Number of bytes in the extendedData array (Valid size = 0-4 bytes)
* \param waitTimeoutUs Command time-out period in microseconds
* \param waitIntervalUs Wait interval time to thread sleep between each check of the CPU command status to prevent SPI read thrashing
* \param armObjId The interest object ID
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuCmdWriteWait(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                armOpCode,
    const uint8_t          extendedData[],
    uint8_t                byteCount,
    uint32_t               waitTimeoutUs,
    uint32_t               waitIntervalUs,
    uint8_t                armObjId);

/**
* \brief Low level helper function used by ADRV9025 API to read the CPU memory config structures
*
* Normally this function should not be required to be used directly by the BBIC.  This is a helper
* function used by other ADRV9025 API commands to read settings from the CPU memory.
*
* \pre This function is private and is not called directly by the user
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param cpuType Type of cpu
* \param objectId CPU id of a particular structure or setting in CPU memory
* \param byteOffset Byte offset from the start of the objectId's memory location in CPU memory
* \param returnData A byte array containing data read back from the CPU memory buffer
* \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuConfigRead(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                objectId,
    uint16_t               byteOffset,
    uint8_t                returnData[],
    uint8_t                byteCount);

/**
* \brief Low level helper function used by ADRV9025 API to write the CPU memory config structures
*
* Normally this function should not be required to be used directly by the BBIC.  This is a helper
* function used by other ADRV9025 API commands to write settings into the CPU memory.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param cpuType Type of cpu
* \param objectId CPU id of a particular structure or setting in CPU memory
* \param byteOffset Byte offset from the start of the objectId's memory location in CPU memory
* \param data A byte array containing data to write to the CPU memory buffer.
* \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuConfigWrite(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t                objectId,
    uint16_t               byteOffset,
    const uint8_t          data[],
    uint8_t                byteCount);

/**
* \brief Reads the ADRV9025 CPU 64-bit command status register and returns an error and status word
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
* \param cpuType Type of cpu
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
int32_t adi_adrv9025_CpuCmdStatusGet(adi_adrv9025_Device_t  *device,
                                     adi_adrv9025_CpuType_e  cpuType,
                                     uint16_t               *errorWord,
                                     uint16_t               *statusWord);

/****************************************************************************
* Debug functions
****************************************************************************
*/

/**
 * \brief Verifies the CPU checksum value
 *
 * The checksum which is written into the binary file is verified with the calculated
 * checksum in the ADRV9025 CPU after the binary file has been loaded.  This function
 * will wait for a timeout period for the checksum calculation to occur.  The
 * user can adjust the timeout period and SPI read interval in adi_adrv9025_user.c by
 * adjusting the macros VERIFY_CPU_CHKSUM_TIMEOUTUS and
 * VERIFY_CPU_CHKSUM_INTERVALS. This function is only supported for CPU-C
 *
 * \pre This function is called after the CPU FW binary file has been loaded to verify it's
 * checksum in CPU memory
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param device is structure pointer to the ADRV9025 data structure containing settings
 * \param cpuType Type of cpu - User can only select CPU-C.
 * \param checksum Pointer to adi_adrv9025_ChecksumTable_t data structure containing settings
 * \param checksumValid Indicate if a valid checksum
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for CPU reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_CpuChecksumTableGet(
    adi_adrv9025_Device_t*           device,
    adi_adrv9025_CpuType_e           cpuType,
    adi_adrv9025_CpuChecksumTable_t* checksum,
    uint8_t*                         checksumValid);

/**
* \brief Reads back the version of the CPU binary loaded into the ADRV9025 CPU memory
*
* This function reads the CPU memory to read back the major.minor.releaseCandidate
* version for the CPU binary loaded into CPU memory.
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param cpuType Type of cpu
* \param fwVersion FW version will be populated here, it is of struct type adi_adrv9025_CpuVersion_t.
*
* \retval ADRV9025_ERR_OK Function completed successfully
*/
int32_t adi_adrv9025_CpuFwVersionGet(
    adi_adrv9025_Device_t*       device,
    adi_adrv9025_CpuType_e       cpuType,
    adi_adrv9025_CpuFwVersion_t* fwVersion);

/**
* \brief Reads CPU Mailbox Command Status
*
* This function reads CPU Mailbox Command Status from 'arm_command_busy' bit
* located in 'arm_command' register (0x00c3)
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param mailboxBusy Pointer to byte containing mailbox busy status, 0:CPU is ready to accept a new command, 1:CPU is busy and cannot accept a new command
* \param cpuType Type of cpu
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuMailboxBusyGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint8_t*               mailboxBusy);

/**
 * \brief Reads CPU Exception Status
 *
 * This function reads Exception Status from arm memory
*
* \pre This function may be called any time after device initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device is structure pointer to the ADRV9025 data structure containing settings
* \param cpuType Type of cpu
* \param exceptionValue Pointer to byte containing ARM exception error code
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuExceptionGet(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    uint32_t*              exceptionValue);

/**
 * \brief Reads Arm System error message.
 *
 * \pre This function may be called any time after device initialization
 *
 * \dep_begin
 * \dep{ device->common.devHalInfo }
 * \dep_end
 *
 * \param device is structure pointer to the ADRV9025 data structure containing settings
 * \param cpuType Type of cpu
 * \param errorMessage Pointer to character array containing the error message
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CpuSystemErrorGet(adi_adrv9025_Device_t* device,
                                       adi_adrv9025_CpuType_e cpuType,
                                       const char**           errorMessage);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_CPU_H_ */
