/* SPDX-License-Identifier: GPL-2.0 */
/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
# ifndef _ADRV_9025_CPU_H_
#define _ADRV_9025_CPU_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include "adi_adrv9025_types.h"
#include "adi_adrv9025_cpu_types.h"
#include "../../private/include/adrv9025_cpu_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Private Helper function to initialize cpu's data structure
*
*  This is a private function and is automatically called by the API.
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
*
* \retval Returns a character array with the error message specified by
*         errCode.
*/
void adrv9025_CpuInitialize(
    adi_adrv9025_Device_t* device);

/**
* \brief Private Helper function to get a CPU's key addresses
*
*  This is a private function and is automatically called by the API.
*
* \param cpu Structure pointer to the ADRV9025's CPU data structure containing settings
* \param cpuType type of cpu
* \param size size of memory to check starting from addr
*
* \retval Returns a character array with the error message specified by
*         errCode.
*/
adi_adrv9025_CpuAddr_t* adrv9025_CpuAddrGet(
    adi_adrv9025_Cpu_t*    cpu,
    adi_adrv9025_CpuType_e cpuType);

/**
* \brief Private Helper function to validate cpu memory address before read/write operation
*
*  This is a private function and is automatically called by the API.
*
* \param cpu Structure pointer to the ADRV9025's CPU data structure containing settings
* \param addr memory address to check
* \param size size of memory to check starting from addr
*
* \retval Returns a character array with the error message specified by
*         errCode.
*/
uint8_t adrv9025_CpuValidateMemoryAddr(
    adi_adrv9025_Cpu_t* cpu,
    uint32_t            addr,
    uint32_t            size);

/**
* \brief Macro to disable spi streaming in case of an error.
*
* \param device Device variable of which spi streaming bit to be restored
* \param recoveryAction Action to be tested against ADI_COMMON_ACT_NO_ACTION
*/
#define ADRV9025_SPI_STREAM_DISABLE_ON_ERROR(device, recoveryAction) \
    if(recoveryAction != ADI_COMMON_ACT_NO_ACTION) \
    { device->spiSettings.enSpiStreaming = 0; \
      adrv9025_SpiCfgSet(device, &device->spiSettings); \
      ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, device->common.error.newAction, NULL, "Failed to set SPI Config"); \
      ADI_ERROR_RETURN(device->common.error.newAction);}

/**
* \brief Macro to disable spi streaming.
*
* \param device Device variable of which spi streaming bit to be restored
*/
#define ADRV9025_SPI_STREAM_DISABLE(device) \
    device->spiSettings.enSpiStreaming = 0; \
    adrv9025_SpiCfgSet(device, &device->spiSettings); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, device->common.error.newAction, NULL, "Failed to set SPI Config"); \
    ADI_ERROR_RETURN(device->common.error.newAction);

/**
* \brief Macro to enable spi streaming.
* When spi streaming is enabled API will write full DMA words
* as A A D D D D packages
*
* \param device Device variable of which spi streaming bit to be restored
*/
#define ADRV9025_SPI_STREAM_ENABLE(device) \
    device->spiSettings.enSpiStreaming = 1; \
    device->spiSettings.autoIncAddrUp = 1; \
    adrv9025_SpiCfgSet(device, &device->spiSettings); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, device->common.error.newAction, NULL, "Failed to set SPI Config"); \
    ADI_ERROR_RETURN(device->common.error.newAction);

/**
* \brief Write to the ADRV9025 CPU program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param address The 32-bit CPU address to write
* \param data Byte array (uint8_t) containing data to be written to CPU memory
* \param byteCount Number of bytes in the data array to be written
* \param autoInc is boolean flag to enable or disable auto-increment of CPU register address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CpuDmaMemWrite(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    const uint8_t          data[],
    uint32_t               byteCount,
    uint8_t                autoIncrement);

/**
* \brief Read from the ADRV9025 CPU program or data memory
*
* The user must make sure the memory addresses are valid.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param address The 32bit CPU address to read from.
* \param returnData Byte(uint8_t) array containing the data read from the CPU memory.
* \param byteCount Number of bytes in the returnData array.
* \param autoIncrement is boolean flag to enable or disable auto-increment of CPU register address
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CpuDmaMemRead(
    adi_adrv9025_Device_t* device,
    uint32_t               address,
    uint8_t                returnData[],
    uint32_t               byteCount,
    uint8_t                autoIncrement);

/**
* \brief Private Helper function to return CPU error strings based on the error code.
*
*  This is a private function and is automatically called by the API.
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param errCode Error code for looking up CPU a specific error message from a table.
*
* \retval Returns a character array with the error message specified by
*         errCode.
*/
const char* adrv9025_CpuErrCodeToString(
    adi_adrv9025_Device_t* device,
    uint32_t               errCode);

/**
* \brief Private Helper function to process detected errors reported from the
*        cpu to determine recovery action.
*
*  This is a private function and is automatically called by the API.
*
* \param device Pointer to device data structure identifying desired device instance
* \param cpuType type of cpu to be checked
* \param errHdl Error Handler type
* \param detErr Error detected to be processed by handler (CPU opcode << 16 | CPU object ID <<8 | CPU cmdStatusByte)
* \param recoveryAction current Recovery Action,
* \param recAction new Recovery Action to be returned should error handler determine an error
*
* \retval uint32_t Value representing the latest recovery Action following processing of detected cpu error.
*/
int32_t adrv9025_CpuCmdErrorHandler(
    adi_adrv9025_Device_t* device,
    adi_adrv9025_CpuType_e cpuType,
    int32_t                errHdl,
    uint32_t               detErr,
    int32_t                recoveryAction);

/**
* \brief Writes the ADRV9025 CPU configuration settings
*
* \pre This function is called automatically during adi_adrv9025_CpuStart(), and
*      this function must be called after loading the CPU firmware.
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
int32_t adrv9025_CpuAdcProfilesWrite(
    adi_adrv9025_Device_t*            device,
    const adi_adrv9025_AdcProfiles_t* adcProfiles);

/**
* \brief Loads the ADC profile data into CPU memory
*
* This function writes ADC profile into the CPU data memory before
* loading the CPU firmware.  When the CPU firmware is loaded and begins
* to initialize, the CPU init sequence copies the ADC profiles from CPU
* memory and loads them into the CPU firmware internal data strucutures.
*
* \pre This function is called during device after downloading CPU code
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
int32_t adrv9025_CpuProfileWrite(
    adi_adrv9025_Device_t*     device,
    const adi_adrv9025_Init_t* init);

/**
* \brief Private helper function to return CPU System error code
*
*  This is a private function and is automatically called by the API.  BBIC
*  should use the adi_adrv9025_CpuSystemErrorGet function.
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param cpuType Type of cpu
* \param systemErrCode Return systemErrCode read from device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CpuSystemErrCodeGet(adi_adrv9025_Device_t* device,
                                     adi_adrv9025_CpuType_e cpuType,
                                     uint16_t*              systemErrCode);

/**
* \brief Private helper function to enter serdes test mode
*
*  This is a private function that should be used to put the arm into serdes test mode
*
* \param device Pointer to the ADRV9025 device settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CpuSerdesTestModeEnter(adi_adrv9025_Device_t *device);

/**
* \brief Private helper function to exit serdes test mode
*
*  This is a private function that should be used to exit serdes test mode
*
* \param device Pointer to the ADRV9025 device settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CpuSerdesTestModeExit(adi_adrv9025_Device_t *device);

/**
* \brief This function sets the Deserializer configuration.
* This function is used to load the correct setting for the desCfg configuration which is in the init structure depending on the device ID.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param init Pointer to init data structure
*
* \retval ADI_ADRV9025_RESET_LOG Recovery action for log reset
* \retval ADI_ADRV9025_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_JesdDesCfgSet(adi_adrv9025_Device_t *device,
                               adi_adrv9025_Init_t   *init);
#ifdef __cplusplus
}
#endif

#endif /* _ADRV_9025_CPU_H_ */
