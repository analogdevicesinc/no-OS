/*! 
 * @brief   Device Profile version external data declarations
 *
 * @details This file contains the structure definition for device profile version
 *           referenced globally.
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
********************************************************************************/

#ifndef __DEVICE_PROFILE_VERSION_T_HEADER__
#define __DEVICE_PROFILE_VERSION_T_HEADER__

#include "adi_device_profile_pack.h"

/*!
 *  @addtogroup device_profile
 *  @ingroup drivers
 *  @{
 */

#define NEVIS_PROFILE_VERSION_MAJOR         12             /*!< Major */
#define NEVIS_PROFILE_VERSION_MINOR         0             /*!< Minor */

#define  PROFILETYPES_VERSION "12.0.4"

/* Profile version structure */
ADI_NEVIS_PACK_START
typedef struct
{
    uint16_t    major;                /*!< Major component in profile version */
    uint16_t    minor;                /*!< Minor component in profile version */
} profileVersion_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif