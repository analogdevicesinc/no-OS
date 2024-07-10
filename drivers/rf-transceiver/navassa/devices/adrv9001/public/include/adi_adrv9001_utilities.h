/**
 * \file
 * \brief Contains ADRV9001 utility functions to load ARM binaries
 *        load stream binaries, load Rx Gain Table, load Tx Atten Table.
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

/**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_UTILITIES_H_
#define _ADI_ADRV9001_UTILITIES_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_rx_types.h"
#include "adrv9001_arm_macros.h"
#include "adi_adrv9001_radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Load and write the ADRV9001 ARM binary image one page at a time
 * 
 * Loads each ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES sized page using adi_hal_ArmImagePageGet and writes
 * it using adi_adrv9001_arm_Image_Write.
 * 
 * \note A large ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES could cause the stack to crash. Please optimize 
 * the chunk size in accordance with the stack space available
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] armImagePath       Absolute path of the ARM image to be programmed
 * \param[in] spiWriteMode      Preferred SPI write mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_ArmImage_Load(adi_adrv9001_Device_t *adrv9001, const char *armImagePath, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

/**
 * \brief Load and write the ADRV9001 stream binary image one page at a time
 * 
 * Loads each ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES sized page using adi_hal_StreamImagePageGet and
 * writes it using adi_adrv9001_Stream_Image_Write.
 * 
 * \note A large ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES could cause the stack to crash. Please optimize
 * the chunk size in accordance with the stack space available.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] streamImagePath    Absolute path of the stream image to be programmed
 * \param[in] spiWriteMode       Preferred SPI write mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_StreamImage_Load(adi_adrv9001_Device_t *adrv9001, const char *streamImagePath, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

/**
 * \brief Load and write the ADRV9001 Rx gain table
 *
 * Loads each Rx gain table entry using adi_hal_RxGainTableEntryGet and writes the table to ADRV9001 SRAM using 
 * adi_adrv9001_Rx_GainTable_Write
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] port               The desired Rx or ORx port
 * \param[in] rxGainTablePath    Path to the Rx gain table in csv format to be programmed
 * \param[in] channel            The Rx Channel from which to write the gain table
 * \param[in] lnaConfig          The desired LNA configuration
 * \param[in] gainTableType      Gain table type loaded during initialization
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_Port_e port,
                                                const char *rxGainTablePath,
                                                adi_common_ChannelNumber_e channel,
                                                adi_adrv9001_RxLnaConfig_t *lnaConfig,
                                                adi_adrv9001_RxGainTableType_e gainTableType);

/**
 * \brief Load and write the ADRV9001 Tx attenuation table
 * 
 * Loads each Tx attenuation table entry using adi_hal_TxAttenTableEntryGet and writes the table to ADRV9001 SRAM using
 * adi_adrv9001_Tx_AttenuationTable_Write
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] txAttenTablePath  Path to the Tx atten table in csv format to be programmed
 * \param[in] txChannelMask     An OR'd combination of adi_common_ChannelNumber_e specifying which
 *                              Tx attenuation tables to load
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_TxAttenTable_Load(adi_adrv9001_Device_t *adrv9001,
                                                 const char *txAttenTablePath,
                                                 uint32_t txChannelMask);

/**
 * \brief Wrapper function for adi_common_hal_Wait_us
 *
 * User configurable blocking wait for the specified length of time
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] waitInterval_ms   User configurable time to wait, denoted in milliseconds (ms)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_WaitMs(adi_adrv9001_Device_t *adrv9001, uint32_t waitInterval_ms);
	
/**
 * \brief Function for In-System Debugging
 *
 * \pre This function can be called by the user anytime before initialization.
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] init              Pointer to the ADRV9001 initialization settings data structure
 * \param[in] armImagePath      Absolute path of the ARM image to be programmed
 * \param[in] streamImagePath   Absolute path of the stream image to be programmed
 * \param[in] armImageVer       ARM image version (from SDK : packages.adrv9001.config file)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_SystemDebugPreCalibrate(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_Init_t *init, const char *armImagePath, const char *streamImagePath, const char *armImageVer);

/**
 * \brief Function for In-System Debugging
 *
 * \pre All the initialized channels must be in CALIBRATED
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_SystemDebugPostCalibrate(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Read the InitCal cal coefficient table
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001         Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] calNo            InitCal number 
 * \param[in] maskChannel1     Calibration bit mask for channel 1
 * \param[in] maskChannel2     Calibration bit mask for channel 2
 * \param[out] addr            Address of InitCal
 * \param[out] size            Size of InitCal cal coefficients
 * \param[out] initMask        InitCal bit mask for channels
 * \param[out] profMask        InitCal profile mask for channels
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_VectTblChunkRead(adi_adrv9001_Device_t *adrv9001,
																		       uint16_t calNo,
																			   uint32_t maskChannel1,
																			   uint32_t maskChannel2,
																			   uint32_t *addr,
																			   uint32_t *size,
																			   uint32_t *initMask,
																			   uint32_t *profMask);

/**
 * \brief Read the InitCal coefficients needed for Warmboot in chunks
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] maskChannel1    Calibration bit mask for channel 1
 * \param[in] maskChannel2    Calibration bit mask for channel 2
 * \param[in] addr            Address of InitCal
 * \param[in] initMask        InitCal bit mask for channels
 * \param[in] profMask        InitCal profile mask for channels
 * \param[out] calVecTblData  Array of InitCal coefficients read
 * \param[in] size            Size of InitCal coefficient table
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_MaxArrayChunk_Get(adi_adrv9001_Device_t *adrv9001,
																				uint32_t maskChannel1,
																				uint32_t maskChannel2,
																				uint32_t addr,
																				uint32_t initMask,
																				uint32_t profMask,
																				uint8_t calVecTblData[],
																				uint32_t size);

/**
 * \brief Write the InitCal coefficients needed for Warmboot in chunks
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv9001        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] maskChannel1    Calibration bit mask for channel 1
 * \param[in] maskChannel2    Calibration bit mask for channel 2
 * \param[in] addr            Address of InitCal
 * \param[in] initMask        InitCal bit mask for channels
 * \param[in] profMask        InitCal profile mask for channels
 * \param[in] calVecTblData   Array of InitCal coefficients to write
 * \param[in] size            Size of Init cal vector table entry data
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_InitCals_WarmBoot_Coefficients_MaxArrayChunk_Set(adi_adrv9001_Device_t *adrv9001,
																				 uint32_t maskChannel1,
																				 uint32_t maskChannel2,
																				 uint32_t addr,
																				 uint32_t initMask,
																				 uint32_t profMask,
																				 uint8_t calVecTblData[],
																				 uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_UTILITIES_H_ */