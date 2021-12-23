/**
* \file
* \brief Contains prototype of ARM private features related function implemented in
*        adrv9001_arm.c
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADRV9001_ARM_H_
#define _ADRV9001_ARM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* ADI specific header files */
#include "adi_adrv9001.h"
#include "adi_adrv9001_fh_types.h"
#include "adi_adrv9001_arm_types.h"

/**
* \brief Read from the ADRV9001 ARM program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device        Structure pointer to the ADRV9001 data structure containing settings
* \param address       The 32bit ARM address to read from.
* \param returnData    Byte(uint8_t) array containing the data read from the ARM memory.
* \param byteCount     Number of bytes in the returnData array.
* \param autoIncrement Boolean flag to enable or disable auto-increment of ARM register address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv9001_DmaMemRead(adi_adrv9001_Device_t *device, uint32_t address, uint8_t returnData[], uint32_t byteCount, uint8_t autoIncrement);

/**
* \brief Write to the ADRV9001 ARM program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device    Structure pointer to the ADRV9001 data structure containing settings
* \param address   The 32-bit ARM address to write
* \param data Byte Array (uint8_t) containing data to be written to ARM memory
* \param byteCount Number of bytes in the data array to be written
* \param[in] spiWriteMode      Preferred SPI write mode
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv9001_DmaMemWrite(adi_adrv9001_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

/**
* \brief Write to the ADRV9001 ARM program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \param device                         Structure pointer to the ADRV9001 data structure containing settings
* \param[in] hopSignal                  Hop signal to configure appropriate tableId
* \param[in] tableId                    FH_HOP_TABLE_A or FH_HOP_TABLE_B. Used for ping-pong hop tables.
* \param hopTableAddress                The 32-bit ARM address to write
* \param numHopTableEntries             Byte Array (uint8_t) containing data to be written to ARM memory
* \param numHopTableEntriesByteCount    Number of bytes in the data array to be written
* \param hopTableBufferAddress          The 32-bit ARM address to write
* \param hopTableBufferData             Byte Array (uint8_t) containing data to be written to ARM memory
* \param hopTableBufferDataByteCount    Number of bytes in the data array to be written
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv9001_DmaMemWriteFH(adi_adrv9001_Device_t *device, 
		                            adi_adrv9001_FhHopSignal_e hopSignal,
		                            adi_adrv9001_FhHopTable_e tableId, 
		                            uint32_t hopTableAddress, 
		                            const uint8_t numHopTableEntries[], 
		                            uint32_t numHopTableEntriesByteCount, 
		                            uint32_t hopTableBufferAddress, 
		                            const uint8_t hopTableBufferData[], 
		                            uint32_t hopTableBufferDataByteCount);
/**
* \brief Write to the ADRV9001 Flex Stream Processor(0 - 3) memory 
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
int32_t adrv9001_FlexStreamProcessorMemWrite(adi_adrv9001_Device_t *device,
                                             uint32_t address,
                                             const uint8_t data[],
                                             uint32_t byteCount,
                                             uint8_t flexSpNumber,
                                             adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

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
int32_t adrv9001_FlexStreamProcessorMemRead(adi_adrv9001_Device_t *device,
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
int32_t adrv9001_ArmCmdErrorHandler(adi_adrv9001_Device_t *device, uint32_t detErr);

/**
* \brief Writes the ADRV9001 ARM configuration settings
*
* \param device Context variable - Pointer to the ADRV9001 device settings data structure
* \param init   Pointer to the ADRV9001 initialization settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv9001_ArmProfileWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init);

/**
* \brief Writes the ADRV9001 PFIR coefficients in ARM PFIR buffer
*
* \param device Context variable - Pointer to the ADRV9001 device settings data structure
* \param init   Pointer to the ADRV9001 initialization settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required
*/
int32_t adrv9001_PfirProfilesWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init);

/**
* \brief Extracts the dynamic components of an adi_adrv9001_Init_t and writes into ARM dynamic profile mailbox
*
* Does not notify the ARM. Just pushes the data into ARM memory.
*
* \pre Only encodes information about channels that are masked 'on' in txInitChannelMask/rxInitChannelMask.
* This field must be initialized by calling #adi_adrv9001_InitAnalog or #adi_adrv9001_Profiles_Verify.
*
* \param device Context variable - Pointer to the ADRV9001 device settings data structure
* \param[in]     dynamicProfile       ADRV9001 dynamic profile structure
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adrv9001_DynamicProfile_Write(adi_adrv9001_Device_t *device,
                                      const adi_adrv9000_DynamicProfile_t *dynamicProfile);

/**
* \brief Writes one PfirWbNbBuffer_t into the ARM PFIR buffer
*
* Does not notify the ARM. Just pushes the data into ARM memory.
*
* \param[in] device         Pointer to device data structure identifying desired device instance
* \param[in] pfirBufferAddr PFIR configuration to write
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adrv9001_PfirWbNbBuffer_Write(adi_adrv9001_Device_t *device,
                                      const adi_adrv9001_PfirWbNbBuffer_t *pfirBufferAddr);

void adrv9001_LoadTwoBytes(uint32_t *offset, uint8_t cfgData[], const uint16_t data);
void adrv9001_LoadFourBytes(uint32_t *offset, uint8_t cfgData[], const uint32_t data);
void adrv9001_LoadEightBytes(uint32_t *offset, uint8_t cfgData[], const uint64_t data);
    
void adrv9001_ParseTwoBytes(uint32_t *offset, uint8_t cfgData[], uint16_t *value);
void adrv9001_ParseFourBytes(uint32_t *offset, uint8_t cfgData[], uint32_t *value);
void adrv9001_ParseEightBytes(uint32_t *offset, uint8_t cfgData[], uint64_t *value);
    
#ifdef __cplusplus
}
#endif

#endif