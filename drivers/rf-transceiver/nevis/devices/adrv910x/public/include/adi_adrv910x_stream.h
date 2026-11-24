/**
 * \file
 * \brief Contains ADRV910X related function prototypes for adi_adrv910x_stream.c
 *
 * Copyright (c) 2015 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_STREAM_H_
#define _ADI_ADRV910X_STREAM_H_

#include "adi_adrv910x_stream_types.h"
#include "adi_adrv910x_arm_types.h"
#include "adi_adrv910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Loads binary array into stream processor data memory
 *
 * FIXME PTN Navassa may not be 20K, check when into available
 *
 * A 20K element byte array is passed into this function.  The byte array is
 * obtained by reading the binary stream processor file provided by Analog
 * Devices.  The stream processor uses the information in the stream file to
 * properly power up and down the various signal chains.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre This function is called after adi_adrv910x_Initialize, and before ARM is loaded.
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in] byteOffset    Offset (starting from 0) of where to place the binary array (if loaded in multiple function calls)
 * \param[in] binary        Byte array containing all valid ARM file data bytes
 * \param[in] byteCount     The number of bytes in the binary array file
 * \param[in] spiWriteMode  Preferred SPI write mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Stream_Image_Write(adi_adrv910x_Device_t *adrv910x,
                                        uint32_t byteOffset,
                                        uint8_t binary[],
                                        uint32_t byteCount,
                                        adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode);

/**
 * \brief Reads back the version of the stream processor binary loaded in the ADRV910X memory
 *
 * This function reads the ADRV910X memory to read back the major.minor.maintenance.build
 * version for the stream processor binary loaded in ADRV910X memory.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x       Context variable - Pointer to the ADRV910X device data structure
 * \param[out] streamVersion  Stream processor version will be populated here, it is of struct type adi_adrv910x_StreamVersion_t.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Stream_Version(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_StreamVersion_t *streamVersion);

/**
 * \brief Configure the DGPIO to debug stream processor operation status Dedicated GPIO pins are used to route stream
 *        processor operation status:
 *        | Rx1 stream processor | DGPIO0 |
 *        | Tx1 stream processor | DGPIO1 |
 *        | Rx2 stream processor | DGPIO2 |
 *        | Tx2 stream processor | DGPIO3 |
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre All channels state either in #ADI_ADRV910X_CHANNEL_STANDBY or #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in] adrv910x           Context variable - Pointer to the ADRV910X device data structure
 * \param[in] streamToGpioDebug  Flag to enable or disable stream to GPIO debug feature
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Stream_Gpio_Debug_Set(adi_adrv910x_Device_t *adrv910x, bool streamToGpioDebug);

/**
 * \brief Get the current enabledness of stream to GPIO debug feature
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre All channels state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x           Context variable - Pointer to the ADRV910X device data structure
 * \param[out] streamToGpioDebug  Current enabledness of stream to GPIO debug feature
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Stream_Gpio_Debug_Get(adi_adrv910x_Device_t *adrv910x, bool *streamToGpioDebug);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_STREAM_H_ */
