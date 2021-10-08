/**
 * \file
 * \brief Low Dropout Regulator (LDO) configuration
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV9001_POWERMANAGEMENT_H_
#define _ADRV9001_POWERMANAGEMENT_H_
    
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv9001_powermanagement_types.h"
#include "adi_adrv9001_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure ADRV9001 LDOs
 *
 * \pre All channels must be in STANDBY state
 *
 * \param[in] adrv9001		            Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] powerManagementSettings   Desired power management settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powermanagement_Configure(adi_adrv9001_Device_t *adrv9001, 
                                               adi_adrv9001_PowerManagementSettings_t *powerManagementSettings);

#ifdef __cplusplus
}
#endif

#endif