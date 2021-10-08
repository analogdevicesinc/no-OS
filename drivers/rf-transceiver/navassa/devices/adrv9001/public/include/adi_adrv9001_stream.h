/**
 * \file
 * \brief Contains ADRV9001 related function prototypes for adi_adrv9001_stream.c
 *
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_STREAM_H_
#define _ADI_ADRV9001_STREAM_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_stream_types.h"
#include "adi_adrv9001_arm_types.h"

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
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre This function is called after adi_adrv9001_Initialize, and before ARM is loaded.
 *
 * \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] byteOffset     Offset (starting from 0) of where to place the binary
 *                           array (if loaded in multiple function calls)
 * \param[in] binary         Byte array containing all valid ARM file data bytes
 * \param[in] byteCount      The number of bytes in the binary array file
 * \param[in] spiWriteMode   Preferred SPI write mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Stream_Image_Write(adi_adrv9001_Device_t *adrv9001, uint32_t byteOffset, uint8_t binary[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode);

 /**
 * \brief Reads back the version of the stream processor binary loaded in the ADRV9001 memory
 *
 * This function reads the ADRV9001 memory to read back the major.minor.maintenance.build
 * version for the stream processor binary loaded in ADRV9001 memory.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[out] streamVersion Stream processor version will be populated here, it is of struct type adi_adrv9001_StreamVersion_t.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Stream_Version(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_StreamVersion_t *streamVersion);

/**
* \brief Configure the DGPIO to debug stream processor operation status
*        Dedicated GPIO pins are used to route stream processor operation status:
*        Rx1 stream processor --> DGPIO0
*        Tx1 stream processor --> DGPIO1
*        Rx2 stream processor --> DGPIO2 
*        Tx2 stream processor --> DGPIO3
*
* \note Message type: \ref timing_mailbox "Mailbox command"
*
* \pre All channels state either in STANDBY or CALIBRATED
*
* \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device data structure
* \param[in]  streamToGpioDebug Flag to enable or disable stream to GPIO debug feature
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Stream_Gpio_Debug_Set(adi_adrv9001_Device_t *adrv9001, bool streamToGpioDebug);

/**
* \brief Get the current enabledness of stream to GPIO debug feature
*
* \note Message type: \ref timing_mailbox "Mailbox command"
*
* \pre All channels state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
*
* \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device data structure
* \param[out] streamToGpioDebug Current enabledness of stream to GPIO debug feature
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Stream_Gpio_Debug_Get(adi_adrv9001_Device_t *adrv9001, bool *streamToGpioDebug);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_STREAM_H_ */
