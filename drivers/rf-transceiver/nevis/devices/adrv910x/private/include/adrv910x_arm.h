/**
* \file
* \brief Contains prototype of ARM private features related function implemented in
*        adrv910x_arm.c
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADRV910X_ARM_H_
#define _ADRV910X_ARM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_arm_types.h"
#include "device_profile_top_t.h"
#include "device_profile_pfir_buffer_t.h"

/**
* \brief Read from the ADRV910X ARM program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device        Structure pointer to the ADRV910X data structure containing settings
* \param address       The 32bit ARM address to read from.
* \param returnData    Byte(uint8_t) array containing the data read from the ARM memory.
* \param byteCount     Number of bytes in the returnData array.
* \param autoIncrement Boolean flag to enable or disable auto-increment of ARM register address
* \param[in] PS        Processor system selection, ADI_PS1 or ADI_PS2
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
	int32_t adrv910x_DmaMemRead(adi_adrv910x_Device_t *device, uint32_t address, uint8_t returnData[], uint32_t byteCount, uint8_t autoIncrement, adi_common_Processor_Subsystem_e PS);

/**
* \brief Write to the ADRV910X ARM program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device    Structure pointer to the ADRV910X data structure containing settings
* \param address   The 32-bit ARM address to write
* \param data Byte Array (uint8_t) containing data to be written to ARM memory
* \param byteCount Number of bytes in the data array to be written
* \param[in] spiWriteMode      Preferred SPI write mode
* \param[in] PS    Processor system selection, ADI_PS1 or ADI_PS2
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
	int32_t adrv910x_DmaMemWrite(adi_adrv910x_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode, adi_common_Processor_Subsystem_e PS);
	
/**
* \brief Write to the ADRV910X Flex Stream Processor(0 - 3) memory 
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device             Structure pointer to the ADRV9001 data structure containing settings
* \param address            The 32-bit ARM address to write
* \param data               Byte Array (uint8_t) containing data to be written to the selected flex SP memory
* \param byteCount          Number of bytes in the data array to be written
* \param flexSpNumber       Flex Stream Procesor(SP) to select and write the data
* \param[in] spiWriteMode   Preferred SPI write mode
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adrv910x_FlexStreamProcessorMemWrite(adi_adrv910x_Device_t *device,
                                             uint32_t address,
                                             const uint8_t data[],
                                             uint32_t byteCount,
                                             uint8_t flexSpNumber,
                                             adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode);


/**
* \brief Read from the ADRV9001 Flex Stream Processor(0 - 3) memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device        Structure pointer to the ADRV9001 data structure containing settings
* \param address       The 32bit ARM address to read from.
* \param returnData    Byte(uint8_t) array containing the data read from the selected flex SP memory.
* \param byteCount     Number of bytes in the returnData array.
* \param autoIncrement Boolean flag to enable or disable auto-increment of ARM register address
* \param flexSpNumber  Flex Stream Procesor(SP) to select and read the data
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adrv910x_FlexStreamProcessorMemRead(adi_adrv910x_Device_t *device,
                                            uint32_t address,
                                            uint8_t returnData[],
                                            uint32_t byteCount,
                                            uint8_t autoIncrement,
                                            uint8_t flexSpNumber);
                                             /**
* \brief Private Helper function to process detected errors reported from the
*        arm to determine recovery action.
*
*  This is a private function and is automatically called by the API.
*
* \param device         Pointer to device data structure identifying desired device instance
* \param detErr         Error detected to be processed by handler:
*                       (ARM opcode << 16 | ARM object ID <<8 | ARM cmdStatusByte)
*
* \retval uint32_t Value representing the latest recovery Action following processing of detected arm error.
*/
int32_t adrv910x_ArmCmdErrorHandler(adi_adrv910x_Device_t *device, uint32_t detErr);
	
/**
* \brief Writes the ADRV910X ARM configuration settings
*
* \param device Context variable - Pointer to the ADRV910X device settings data structure
* \param profile   Pointer to the ADRV910X profile settings data structure
* \param PS Processor subsystem to target, with ADI_PS1 or ADI_PS2
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv910x_ArmProfileWrite(adi_adrv910x_Device_t *device, const deviceProfile_t *profile, adi_common_Processor_Subsystem_e PS);
	
/**
* \brief Writes the ADRV910X PFIR coefficients in ARM PFIR buffer
*
* \param device Context variable - Pointer to the ADRV910X device settings data structure
* \param pfirBuffer   Pointer to the ADRV910X profile Pfir buffer struct
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv910x_PfirProfilesWrite(adi_adrv910x_Device_t *device, const pfirBuffer_t *pfirBuffer);

/**
 * \brief Low level helper function used by ADRV910X API to read the ARM memory config structures for PS1
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV910X API commands to read settings from the ARM memory.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is private and is not called directly by the user
 *
 * \param  device			Context variable - Pointer to the ADRV910X device settings data structure
 * \param  objectId			ARM id of a particular structure or setting in ARM memory
 * \param  subObjectId      Additional ARM id of a particular structure or setting in ARM memory
 * \param  channelMask      The mask of Tx/Rx channels
 * \param  byteOffset		Byte offset from the start of the objectId's memory location in ARM memory
 * \param returnData		A byte array containing data read back from the ARM memory buffer
 * \param  byteCount		Number of bytes in the data array (Valid size = 1-255 bytes)
 *
 * \retval A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_MailBox_Get(adi_adrv910x_Device_t *device, 
                                     uint8_t objectId, 
                                     uint8_t subObjectId, 
                                     uint8_t channelMask, 
                                     uint16_t byteOffset, 
                                     uint8_t returnData[], 
                                     uint32_t byteCount);

/**
 * \brief Low level helper function used by ADRV910X API to write the ARM memory config structures
 *
 * Normally this function should not be required to be used directly by the BBIC.  This is a helper
 * function used by other ADRV910X API commands to write settings into the ARM memory.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \param[in] adrv910x			Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] armData			A byte array (uint8_t) containing data to be written to ARM memory
 * \param[in] armDataSize		Number of bytes in armData[] array to be written
 * \param[in] mailboxCmd		A byte array containing data to write to the ARM command interface
 * \param[in] mailboxCmdSize	Number of bytes to write to the ARM command interface
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_arm_Config_Write(adi_adrv910x_Device_t *adrv910x,
									  const uint8_t armData[],
									  uint32_t armDataSize,
									  const uint8_t mailboxCmd[],
									  uint32_t mailboxCmdSize);

void adrv910x_LoadTwoBytes(uint32_t *offset, uint8_t cfgData[], const uint16_t data);
void adrv910x_LoadFourBytes(uint32_t *offset, uint8_t cfgData[], const uint32_t data);
void adrv910x_LoadEightBytes(uint32_t *offset, uint8_t cfgData[], const uint64_t data);
    
void adrv910x_ParseTwoBytes(uint32_t *offset, uint8_t cfgData[], uint16_t *value);
void adrv910x_ParseFourBytes(uint32_t *offset, uint8_t cfgData[], uint32_t *value);
void adrv910x_ParseEightBytes(uint32_t *offset, uint8_t cfgData[], uint64_t *value);
    
#ifdef __cplusplus
}
#endif

#endif