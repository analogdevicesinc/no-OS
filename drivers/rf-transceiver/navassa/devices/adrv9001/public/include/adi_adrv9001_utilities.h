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

#include "adi_adrv9001_utilities_types.h"
#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This utility function reads and parses a JSON profile file, loading the contents into an init struct.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre The parameter init must have memory fully allocated.
 *
 * \param[in]  adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  profileFilename       String containing the absolute path of the profile file
 * \param[out] init                  is an init struct where the contents of the profile will be written
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_DeviceProfile_Load(adi_adrv9001_Device_t *adrv9001,
                                                  const char *profileFilename,
                                                  adi_adrv9001_Init_t *init);

/**
 * \brief This utility function parses the device profile available in JSON buffer, loading the contents into an init struct.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre The parameter init must have memory fully allocated.
 *
 * \param[in]  adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[out] init                  is an init struct where the contents of the profile will be written
 * \param[in]  jsonBuffer            Buffer from which the device profile is parsed
 * \param[in]  length                Length of the buffer
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_DeviceProfile_Parse(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_Init_t *init,
                                                   char *jsonBuffer,
                                                   uint32_t length);

/**
 * \brief This utility function loads ADRV9001 ARM binary image through ArmImageWrite() API
 *
 * This function reads the ARM binary file from a specified location
 * (typically in an SD card) and programs the ARM with the specified binary image
 *
 * Please note that a large chunk size defined by ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
 * in adi_adrv9001_user.h could potentially cause the stack to crash. Please optimize the chunk size
 * in accordance with the stack space available
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] armImagePath       Absolute path of the ARM image to be programmed
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_ArmImage_Load(adi_adrv9001_Device_t *adrv9001, const char *armImagePath);

/**
 * \brief This utility function loads ADRV9001 Stream binary image through StreamImageWrite() API
 *
 * This function reads the ADRV9001 stream binary file from a specified location
 * (typically in an SD card) and programs the stream processor with the specified binary image
 *
 * Please note that a large chunk size defined by ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
 * in adi_adrv9001_user.h could potentially cause the stack to crash. Please optimize the chunk size
 * in accordance with the stack space available.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] streamImagePath    Absolute path of the stream image to be programmed
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_StreamImage_Load(adi_adrv9001_Device_t *adrv9001, const char *streamImagePath);

#ifndef CLIENT_IGNORE
/**
* \brief This utility function programs ADRV9001 raw ARM binary image through StreamImageWrite() API
*
* This function takes the ADRV9001 stream image and programs the stream processor
*
* Please note that a large chunk size defined by ADI_ADRV9001_ARM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
* in adi_adrv9001_user.h could potentially cause the stack to crash. Please optimize the chunk size
* in accordance with the stack space available
*
* \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
* \param[in] streamImageBinary  Stream image binary to be programmed
* \param[in] arraySize          Stream iamge byte count.  Must be ADI_ADRV9001_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Utilities_StreamImageBinary_Program(adi_adrv9001_Device_t *device,
                                                         uint8_t (*streamImageBinary)[ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES]);
#endif // !CLIENT_IGNORE

/**
 * \brief This utility function load Arm files after Digital init sequence
 *
 * This function executes the following set of sub-functions required to be
 * complete after digital init sequence
 *
 * - Continue Digital Init (adi_adrv9001_InitDigital)
 * - Load ARM binary image from a .bin file
 * - Load Stream binary image from a .bin file
 * - Load Requested Rx gain tables from .csv files for initialized channels
 * - Load Reqeuested Tx atten tables from .csv files for initialized channels
 * - Load ARM profile
 * - ARM bootup
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] resourceCfg        Pointer to ADRV9001 resource config settings structure
 *
 * Rx Gain Table columns should be arranged in the following order
 *
 *  --------------------------------------------------------------------------------------------------------
 *  | Gain Index | FE Control Word | TIA Control | ADC Control | Ext Control | Phase Offset | Digital Gain |
 *  --------------------------------------------------------------------------------------------------------
 *
 * Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest gain index and
 * and progressing to highest gain Index
 *
 * Eg: If the gain table contains entries from index 192 to index 255, the table has to be arranged such that
 *     first line should contain row entries for index 192 and the last line should contain row entries to index 255
 *     with a total of 64 entries.
 *
 * Tx Atten Table columns should be arranged in the following order
 *
 *  ------------------------------------------------
 *  | Tx Atten Index | Tx Atten Hp | Tx Atten Mult |
 *  ------------------------------------------------
 *
 * Tx Atten Indices should be arranged in ascending order for row entries starting with the lowest tx atten index and
 * and progressing to highest tx atten Index (Eg: Starting from index 0 and progressing to index 1023)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_Resources_Load(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_ResourceCfg_t *resourceCfg);

/**
 * \brief This utility function loads ADRV9001 Rx Gain table file in csv format to ADRV9001 gain table SRAM
 *
 * This function reads the ADRV9001 Rx Gain Table file in csv format from a specified location
 * (typically in an SD card) and programs the ADRV9001 SRAM with the gain table for the requested channels
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] rxGainTablePath    Path to the Rx gain table in csv format to be programmed
 * \param[in] rxChannelMask      An OR'd combination of adi_common_ChannelNumber_e specifying which
                                Rx gain tables to load
 *
 * Rx Gain Table columns should be arranged in the following order
 *
 *  --------------------------------------------------------------------------------------------------------
 *  | Gain Index | FE Control Word | TIA Control | ADC Control | Ext Control | Phase Offset | Digital Gain |
 *  --------------------------------------------------------------------------------------------------------
 *
 * Rx Gain Indices should be arranged in ascending order for row entries starting with the lowest gain index and
 * and progressing to highest gain Index
 *
 * Eg: If the gain table contains entries from index 192 to index 255, the table has to be arranged such that
 *     first line should contain row entries for index 192 and the last line should contain row entries to index 255
 *     with a total of 64 entries.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_RxGainTable_Load(adi_adrv9001_Device_t *adrv9001,
                                                const char *rxGainTablePath,
                                                uint32_t rxChannelMask);

/**
 * \brief This utility function loads ADRV9001 Tx Attenuation table file in csv format to ADRV9001 Tx Atten table SRAM
 *
 * This function reads the ADRV9001 Tx Atten Table file in csv format from a specified location
 * (typically in an SD card) and programs the ADRV9001 SRAM with the atten table for the requested Tx channels
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] txAttenTablePath   Path to the Tx atten table in csv format to be programmed
 * \param[in] txChannelMask      An OR'd combination of adi_common_ChannelNumber_e specifying which
                                Tx attenuation tables to load
 *
 * Tx Atten Table columns should be arranged in the following order
 *
 *  ------------------------------------------------
 *  | Tx Atten Index | Tx Atten Hp | Tx Atten Mult |
 *  ------------------------------------------------
 *
 * Tx atten indices should be arranged in ascending order for row entries starting with the lowest tx atten index and
 * and progressing to highest tx atten Index (Eg: Starting from index 0 and progressing to index 1023)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_TxAttenTable_Load(adi_adrv9001_Device_t *adrv9001,
                                                 const char *txAttenTablePath,
                                                 uint32_t txChannelMask);

/**
 * \brief This utility function dumps the ADRV9001 ARM program and data memory through ArmMemRead() API
 *
 * This function reads the ARM Memory and writes the binary byte array directly to a binary file.
 * The first section is program memory followed by data memory.
 * The binaryFilename is opened before reading the ARM memory to verify that the filepath is valid and has write access.
 * A file IO error message will be thrown if write access is not valid for the binaryFilename path.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] binaryFilename     Absolute path of the ARM memory to be dumped
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_ArmMemory_Dump(adi_adrv9001_Device_t *adrv9001, const char *binaryFilename);

/**
 * \brief This utility function performs initialization of Radio Controls
 *
 * This function attempts to place Radio Controls into a required init state
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] initConfig         Pointer to ADRV9001 resource config settings structure
 * \param[in] channelMask        The mask of Tx/Rx channels
 * \parblock              Bit position |  channelMask
 *                      ---------------|----------------
 *                           bit 0     | RX1
 *                           bit 1     | RX2
 *                           bit 2     | TX1
 *                           bit 3     | TX2
 * \endparblock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Utilities_InitRadio_Load(adi_adrv9001_Device_t *adrv9001,
                                              adi_adrv9001_ResourceCfg_t *initConfig,
                                              uint8_t channelMask);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_UTILITIES_H_ */