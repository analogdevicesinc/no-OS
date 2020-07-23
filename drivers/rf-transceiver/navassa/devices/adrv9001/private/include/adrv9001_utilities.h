/**
* \file
* \brief Contains Utility features related private function defenitions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADRV9001_UTILITIES_H_
#define _ADRV9001_UTILITIES_H_

#include "adi_adrv9001_utilities_types.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdio.h>
#endif

#define ADRV9001_MAX_FILE_BUFFER 128

#ifdef __cplusplus
extern "C" {
#endif
    
/**
* \brief This utility function safely reads a file and loads it to the buffer.
*
* This function performs the following
* - Opens the file and verify it is opened.
* - Determines the length (number of characters) and that the file is non-empty.
* - Verifies that the file pointer can be moved from beginning to end.
* - Allocates sufficient memory in the buffer and verifies.
* - Reads the file into the buffer.
* - Closes the file and reports the length.
*
* \param[in]  device Pointer to the ADRV9001 device data structure
* \param[in]  filename Full name and path of the file to be read
* \param[out] buffer Pointer to a buffer which will hold the file contents
* \param[out] fileLength Number of characters in the file buffer
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_SafeFileLoad(adi_adrv9001_Device_t *device, const char *filename, char **buffer, uint32_t *filelength);

/**
* \brief This utility function initializes radioctrl parameters
*
* This function performs the following
* - Sets up the ARM GPIO Pins
* - Sets up the Radio Ctrl Mode (Pin or SPI mode) for signal chain enable/disable
* - Initializes PLL frequencies
* - Enables/Disables Rx and Tx signal chains(only applicable for SPI mode)
*
* \param device Pointer to the ADRV9001 device data structure containing settings
* \param radioCtrlInit Pointer to ADRV9001 RadioCtrl init settings structure
* \param channelMask The mask of Tx/Rx channels
*
* \parblock              Bit position |  channelMask
*                      ---------------|----------------
*                           bit 0     | RX1
*                           bit 1     | RX2
*                           bit 2     | TX1
*                           bit 3     | TX2
* \endparblock
*
* \param ssiType LVDS or CMOS mode
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_RadioCtrlInit(adi_adrv9001_Device_t *device, 
                               adi_adrv9001_RadioCtrlInit_t *radioCtrlInit,
                               uint8_t channelMask,
                               adi_adrv9001_SsiType_e ssiType);


#ifdef __cplusplus
}
#endif

#endif
