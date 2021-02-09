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
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] rxGainTablePath   Path to the Rx gain table in csv format to be programmed
 * \param[in] rxChannelMask     An OR'd combination of adi_common_ChannelNumber_e specifying which
 *                              Rx gain tables to load
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *adrv9001,
                                                const char *rxGainTablePath,
                                                uint32_t rxChannelMask);

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

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_UTILITIES_H_ */