/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_eeprom_core.h
 * \brief Contains function declarations for EEPROM API
 */
#ifndef _ADI_EEPROM_H_
#define _ADI_EEPROM_H_

#include "adi_eeprom_types.h"

/**
* \brief Performs a Hardware Initialization for EEPROM Device
*
* \pre This function may be called after device->common.devHalInfo has been initialized with
* user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to EEPROM device data structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_eeprom_HwOpen(adi_eeprom_Device_t* const device);

/**
* \brief Performs a hardware shutdown for EEPROM Device
*
* \pre This function may be called after device->common.devHalInfo has been initialized with
* user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to EEPROM device data structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_eeprom_HwClose(adi_eeprom_Device_t* const device);

/**
* \brief Service to retrieve Board Information from EEPROM Device
*
* NOTE: Valid Board Information will also be stored in the device structure
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to EEPROM device data structure
* \param[out] boardInfo Board Information Structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_eeprom_BoardInfoRead(adi_eeprom_Device_t* const device, adi_eeprom_BoardInfo_t* const boardInfo);

#endif /* _ADI_EEPROM_TYPES_H_ */
