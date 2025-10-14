/**
 * \file
 * \brief Contains ADRV910X parameters validation related private function prototypes
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV910X_VALIDATORS_H_
#define _ADRV910X_VALIDATORS_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "adi_adrv910x_user.h"
#include "adi_adrv910x_types.h"
#include "adi_adrv910x_common_types.h"

/**
 * \brief Check for valid channels
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] channels      Set of port/channel combinations to prime
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Channel_Validate(adi_adrv910x_Device_t *adrv910x,
                                      adi_common_ChannelNumber_e channel);
/**
 * \brief Check for valid ports
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] ports         Set of port/channel combinations to prime
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Port_Validate(adi_adrv910x_Device_t *adrv910x,
                                   adi_common_Port_e port);

/**
 * \brief Check the following:
 *        1. whether ARM binary is loaded
 *        2. ports and channels are valid
 *
 * \brief Channel state is one of [STANDBY, CALIBRATED, PRIMED, RF ENABLED]
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] ports         Set of port/channel combinations to prime
 * \param[in] channels      Set of port/channel combinations to prime
 * \param[in] length        Length of ports and channels arrays - the number of port/channel combinations to prime; 
 *                          Each index i of ports and channels correspond to a single port/channel combination
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Channel_State_GenericValidate(adi_adrv910x_Device_t *adrv910x,
                                                   adi_common_Port_e ports[],
                                                   adi_common_ChannelNumber_e channels[],
                                                   uint8_t length);

#ifdef __cplusplus
}
#endif

#endif /* _ADRV910X_VALIDATORS_H_ */