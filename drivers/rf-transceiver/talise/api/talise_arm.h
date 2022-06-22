// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_arm.h
 * \brief Contains Talise ARM related function prototypes for talise_arm.c
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_ARM_H_
#define TALISE_ARM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_types.h"
#include "talise_error_types.h"
#include "talise_arm_types.h"
/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */

/**
 * \brief Resets and performs initialization for the ARM processor
 *
 * Sets ARM Run = 0, Disables parity checks, sets ARM and SPI register clock selects,
 * resets ARM, and enables ARM SPI register access
 *
 * \pre This function is called after the device has been initialized and multichip-sync
 * (MCS) has been completed
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init-> (most members)}
 * \dep_end
 *
 * \param device Pointer to the Talise device settings data structure
 * \param init Pointer to the Talise initialization setting data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_initArm(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief Writes the Talise ARM configuration settings
 *
 * \pre This function is called automatically during TALISE_initArm(), and
 *      this function must be called before loading the ARM firmware.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep{init-> (most members)}
 * \dep_end
 *
 * \param device Pointer to the Talise device settings data structure
 * \param init Pointer to the Talise initialization settings data structure
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_writeArmProfile(taliseDevice_t *device, taliseInit_t *init);

/**
 * \brief Loads binary array into ARM program memory
 *
 * This function sets the ARM DMA control register bits for an ARM memory write in legacy mode, auto-incrementing
 * the address. Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000)
 *
 * The top level application reads the binary file, parsing it into any array, starting at the first data byte
 * on the first line of the file. The array is passed to this function and writes it to ARM program memory. Any non-data
 * bytes should be excluded from the binary array when parsing.
 *
 * \pre This function is called after the device has been initialized, PLL lock status has been verified, and
 * the stream binary has been loaded
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device data structure containing settings
 * \param binary Byte array containing all valid ARM file data bytes
 * \param count The number of bytes in the binary array
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_loadArmFromBinary(taliseDevice_t *device, uint8_t *binary,
				  uint32_t count);

/**
 * \brief Loads the ADC profile data into ARM memory
 *
 * This function writes four ADC profiles into the ARM data memory before
 * loading the ARM firmware.  When the ARM firmware is loaded and begins
 * to initialize, the ARM init sequence copies the ADC profiles from ARM
 * memory and loads them into the ARM firmware internal data strucutures.
 *
 * \pre This function is called automatically by the API during device
 *      initialization
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Pointer to the Talise device settings data structure
 * \param rxAdcProfile Pointer to the Talise Rx ADC profile data structure
 * \param orxLowPassAdcProfile Pointer to the Talise ORx low pass ADC profile data structure
 * \param orxBandPassAdcProfile Pointer to the Talise ORx band pass ADC profile data structure (ADC stitching only - ignored otherwise)
 * \param loopBackAdcProfile Pointer to the Talise Loopback Rx ADC profile data structure
 * \param orxMergeFilter Pointer to the Talise ORx merge filter data structure (ADC stitching only - ignored otherwise)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_loadAdcProfiles(taliseDevice_t *device, uint16_t *rxAdcProfile,
				uint16_t *orxLowPassAdcProfile,
				uint16_t *orxBandPassAdcProfile, uint16_t *loopBackAdcProfile,
				int16_t *orxMergeFilter);
/****************************************************************************
 * Runtime functions
 ****************************************************************************
 */

/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
 * \brief Read from the Talise ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param address The 32bit ARM address to read from.
 * \param returnData Byte(uint8_t) array containing the data read from the ARM memory.
 * \param bytesToRead Number of bytes in the returnData array.
 * \param autoIncrement is boolean flag to enable or disable auto-increment of ARM register address
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readArmMem(taliseDevice_t *device, uint32_t address,
			   uint8_t *returnData, uint32_t bytesToRead, uint8_t autoIncrement);

/**
 * \brief Write to the Talise ARM program or data memory
 *
 * Valid memory addresses are: Program Memory (0x01000000 - 0x0101C000),
 * Data Memory (0x20000000 - 0x20014000).
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param address The 32-bit ARM address to write
 * \param data Byte array (uint8_t) containing data to be written to ARM memory
 * \param byteCount Number of bytes in the data array to be written
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_writeArmMem(taliseDevice_t *device, uint32_t address,
			    uint8_t *data, uint32_t byteCount);

/**
 * \brief Low level helper function used by Talise API to write the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other Talise API commands to write settings into the ARM memory.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param offset Byte offset from the start of the objectId's memory location in ARM memory
 * \param data A byte array containing data to write to the ARM memory buffer.
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
*/
uint32_t TALISE_writeArmConfig(taliseDevice_t *device, uint8_t objectId,
			       uint16_t offset, uint8_t *data, uint8_t byteCount);

/**
 * \brief Low level helper function used by Talise API to read the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other Talise API commands to read settings from the ARM memory.
 *
 * \pre This function is private and is not called directly by the user
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param objectId ARM id of a particular structure or setting in ARM memory
 * \param offset Byte offset from the start of the objectId's memory location in ARM memory
 * \param data A byte array containing data to write to the ARM memory buffer
 * \param byteCount Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readArmConfig(taliseDevice_t *device, uint8_t objectId,
			      uint16_t offset, uint8_t *data, uint8_t byteCount);

/**
 * \brief Reads the Talise ARM 64-bit command status register and returns an error and status word
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
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param errorWord 16-bit error word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if any three-bit error type > 0, where '0' = OK
 * \param statusWord 16-bit pending bits word comprised of weighted bits according to first 16 even-numbered opcodes
 * The weighted bit is set if an opcode is pending, where '0' = no pending opcode
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readArmCmdStatus(taliseDevice_t *device, uint16_t *errorWord,
				 uint16_t *statusWord);

/**
 * \brief Isolated byte read of the Talise ARM 64-bit command status register based on the opcode
 *
 * A single byte read is performed on the 64-bit command status register according to
 * the opcode of interest. The pending bit and the error type are extracted from the status
 * register and returned as a single byte in the lower nibble.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param cmdStatByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_readArmCmdStatusByte(taliseDevice_t *device, uint8_t opCode,
				     uint8_t *cmdStatByte);

/**
 * \brief Talise ARM command status wait function polls command status register until opcode of interest completes
 *
 * This function polls the ARM status SPI register at the waitInterval_us rate,
 * blocking the function return until either the ARM command completes, the
 * ARM returns an error, or this function times out.
 *
 * In the event that this function returns an error, such as a timeout error
 * or ARM error, the cmdStatByte parameter is still returned with the ARM
 * command status, which can be used for debug to determine the lower
 * layer ARM error.
 *
 * \pre ARM firmware load and initialization must take place first before attempting to use this function
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param cmdStatByte Comprised of cmdStatByte[3:1] = error type, cmdStatByte[0] = pending flag for opCode of interest
 * \param timeout_us Command time-out period in microseconds
 * \param waitInterval_us Wait interval time to thread sleep between each check of the ARM command status to prevent SPI read thrashing
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_waitArmCmdStatus(taliseDevice_t *device, uint8_t opCode,
				 uint8_t *cmdStatByte, uint32_t timeout_us, uint32_t waitInterval_us);

/**
 * \brief Sends a command to the Talise ARM processor interface
 *
 * \pre This function can be called after initializing the ARM processor
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to the Talise data structure containing settings
 * \param opCode Opcode of interest where only the first 16 even-numbered integers are valid
 * \param extendedData A byte array containing extended data to write to the ARM command interface
 * \param extendedDataNumBytes Number of bytes in the extendedData array (Valid size = 0-4 bytes)
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_sendArmCommand(taliseDevice_t *device, uint8_t opCode,
			       const uint8_t *extendedData, uint8_t extendedDataNumBytes);


/****************************************************************************
 * Debug functions
 ****************************************************************************
 */

/**
* \brief Reads back the version of the ARM binary loaded into the Talise ARM memory
*
* This function reads the ARM memory to read back the major.minor.releaseCandidate
* version and the build type for the ARM binary loaded into ARM memory.
* Note: only 'TAL_ARM_BUILD_RELEASE' build type should be used with production code
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device is structure pointer to the Talise data structure containing settings
* \param talArmVersionInfo is a pointer to a structure containing the ARM binary version and
* build type data
*
* \retval TALACT_NO_ACTION Function completed successfully
* \retval TALACT_ERR_CHECK_PARAM One of the function parameters has a NULL pointer or ARM has not been loaded
*/
uint32_t TALISE_getArmVersion_v2(taliseDevice_t *device,
				 taliseArmVersionInfo_t *talArmVersionInfo);

/**
* \deprecated Please see TALISE_getArmVersion_v2 for current implementation
*
* \brief ***DEPRECATED*** Reads back the version of the ARM binary loaded into the Talise ARM memory
*
* This function reads the ARM memory to read back the major.minor.releaseCandidate
* version for the ARM binary loaded into ARM memory.
*
* <B>Dependencies</B>
* - device->spiSettings->chipSelectIndex
*
* \param device is structure pointer to the Talise data structure containing settings
* \param majorVer The Major version is returned in this parameter
* \param minorVer The Minor version is returned in this parameter
* \param rcVer The release candidate version (build number) is returned in this parameter
*
* \retval TALACT_NO_ACTION Function completed successfully
* \retval TALACT_ERR_CHECK_PARAM One of the function parameters has a NULL pointer
*/
uint32_t TALISE_getArmVersion(taliseDevice_t *device, uint8_t *majorVer,
			      uint8_t *minorVer, uint8_t *rcVer);

/**
 * \brief Verifies the ARM checksum value
 *
 * The checksum which is written into the binary file is verified with the calculated
 * checksum in the Talise ARM after the binary file has been loaded.  This function
 * will wait for a timeout period for the checksum calculation to occur.  The
 * user can adjust the timeout period and SPI read interval in talise_user.c by
 * adjusting the macros VERIFY_ARM_CHKSUM_TIMEOUTUS and
 * VERIFY_ARM_CHKSUM_INTERVALUS.
 *
 * \pre This function is called after the ARM binary file has been loaded to verify it's
 * checksum in ARM memory
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device is structure pointer to the Talise data structure containing settings
 *
 * \retval TALACT_WARN_RESET_LOG Recovery action for log reset
 * \retval TALACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_ERR_RESET_ARM Recovery action for ARM reset required
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
uint32_t TALISE_verifyArmChecksum(taliseDevice_t *device);

/**
 * \brief Private Helper function to return ARM error strings to the
 *        TALISE_getErrorMessage function.
 *
 *  This is a private function and is automatically called by the API.  BBIC
 *  should use the TALISE_getErrorMessage function.
 *
 * \param errSrc A value that represents the error source from the Talise API
 * \param errCode Error code that along with the error souce allows looking up
 *        a specific error message.
 *
 * \retval Returns a character array with the error message speficied by
 *         errSrc and errCode.
 */
const char* talGetArmErrorMessage(uint32_t errSrc, uint32_t errCode);

/**
 * \brief Private Helper function to process detected errors reported from the
 *        arm to determine recovery action.
 *
 *  This is a private function and is automatically called by the API.
 *
 * \param device Pointer to device data structure identifying desired device instance
 * \param errHdl Error Handler type
 * \param detErr Error detected to be processed by handler (ARM opcode << 16 | ARM object ID <<8 | ARM cmdStatusByte)
 * \param retVal current Recovery Action,
 * \param recAction new Recovery Action to be returned should error handler determine an error
  *
 * \retval uint32_t Value representing the latest recovery Action following processing of detected arm error.
*/
talRecoveryActions_t talArmCmdErrorHandler(taliseDevice_t *device,
		taliseErrHdls_t errHdl,
		uint32_t detErr, talRecoveryActions_t retVal, talRecoveryActions_t recAction);

#define ARMCMD_ERRCODE(armOpCode, armObjId, armErrorFlag) ((armOpCode << 16) | (armObjId << 8) | armErrorFlag)

#ifdef __cplusplus
}
#endif

#endif /* TALISE_ARM_H_ */
