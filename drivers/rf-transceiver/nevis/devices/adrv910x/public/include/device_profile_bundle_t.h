/*! 
 * @brief   Device Profile external data declarations
 *
 * @details This file contains all the typedef device profile prototypes
 *           referenced globally.
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
********************************************************************************/

#ifndef __DEVICE_PROFILE_BUNDLE_T_HEADER__
#define __DEVICE_PROFILE_BUNDLE_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_top_t.h"
#include "device_profile_pfir_buffer_t.h"

/*!
 *  @addtogroup device_profile
 *  @ingroup drivers
 *  @{
 */


/****************************************************************************
* Profile bundle including the profile and the PFIR buffer values and settings.
* 
* This bundle may not be used in Firmware, but only in the Configurator and API.
****************************************************************************/
ADI_NEVIS_PACK_START
typedef struct
{
    deviceProfile_t profile;
	pfirBuffer_t    pfirBuffer;
} deviceProfileBundle_t;
ADI_NEVIS_PACK_FINISH

/*! @} */

#endif /*! __DEVICE_PROFILE_BUNDLE_T_HEADER__ */
