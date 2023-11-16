/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information.
 * see the "LICENSE.pdf" file in this zip file.
 */

 /**
* \file adi_fmc_eeprom.h
*
* \brief Declarations for ADI FMC EEPROM
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_FMC_EEPROM_H__
#define _ADI_FMC_EEPROM_H__

#include "adi_platform.h"

/**
* \brief Service to Read Board Information from FMC EEPROM Device
*
* \param[in] devHalCfg Pointer to device instance specific platform settings
* \param[out] boardInfo Board Information Structure
*
* \retval ADI_HAL_ERR_OK Function completed successfully
*/
ADI_API adi_hal_Err_e adi_platform_FmcEepromRead(void* const devHalCfg, adi_hal_BoardInfo_t* const boardInfo);

#endif

