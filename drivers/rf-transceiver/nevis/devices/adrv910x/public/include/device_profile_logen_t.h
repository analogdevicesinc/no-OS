/*! **************************************************************************** 
 *
 * @file:    device_profile_logen_t.h
 *
 * @brief:   Logen exported type definitions 
 *
 * @details: This file contains type definition for Logen driver 
 *
 * @version: $Revision:
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2014 Analog Devices, Inc. All Rights Reserved. This software is 
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using 
  this software you agree to the terms of the associated Analog Devices License 
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_LOGEN_T_HEADER__
#define __DEVICE_PROFILE_LOGEN_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_logen_spare_reg_t.h"
#include "device_profile_pll_t.h"
#include "device_profile_channels_t.h"

/*!< Local Oscillator Generation (LOGen) modes */
typedef enum {
    LOGEN_INTERNAL_IN     = 0x00u,     /*!< Set LOGen to internal mode */
    LOGEN_EXTERNAL_LO_IN  = 0x01u,     /*!< Set LOGen to external LO mode*/
    LOGEN_EXTERNAL_VCO_IN = 0x02u,     /*!< Set LOGen to external VCO mode  */
    LOGEN_INTERNAL_LO_OUT = 0x03u,     /*!< Set LOGen to internal LO out mode */
} ADI_NEVIS_PACK_ENUM loGenInMode_e;
     

typedef enum {
    LOGEN_NO_EXT_OUT = 0x00u,           /*!< No external Out signal*/
    LOGEN_EXTERNAL_OUT = 0x01u,         /*!< Set Logen to external OUT mode  */
} ADI_NEVIS_PACK_ENUM loGenOutMode_e;

typedef enum {
    LOGEN_EXTLO_DIFFERENTIAL,           /*!< External PLL input is differential */
    LOGEN_EXTLO_SINGLE_ENDED,           /*!< External PLL input is single ended */
    LOGEN_EXTLO_SINGLE_ENDED_IP,        /*!< External PLL input is single-ended using negative-polarity port */
    LOGEN_EXTLO_DIFFERENTIAL_180,       /*!< External PLL input is differential with 180-degree swap */
} ADI_NEVIS_PACK_ENUM loGenExtPllConnect_e;

typedef enum {
    ON_LO        = 0u,
    OFF_LO       = 1u,
    NUM_LO_TYPES = 2u,
    LO_TYPE_NA   = 0xFF,
} ADI_NEVIS_PACK_ENUM loType_e; //Initially TO BE REMOVED, but a value is used somewhere in device profile

ADI_NEVIS_PACK_START
typedef struct
{
    loGenInMode_e        extLoMode;       /*!< External LO mode */
    loGenExtPllConnect_e extLoPllConnect; /*!< External LO PLL connection */
    uint16_t             extLoDiv;        /*!< External LO in divider */
} loConfig_t;

ADI_NEVIS_PACK_FINISH

#endif /* __DEVICE_PROFILE_LOGEN_T_HEADER__ */
