/**
 * \file
 * \brief Functions for working with ADRV9001 profiles
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_PROFILEUTIL_H_
#define _ADI_ADRV9001_PROFILEUTIL_H_

#include "adi_adrv9001_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This utility function parses the device profile available in JSON buffer, loading the contents into an init struct.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre The parameter init must have memory fully allocated.
 *
 * \param[in]  adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[out] init                  is an init struct where the contents of the profile will be written
 * \param[in]  jsonBuffer            Buffer from which the device profile is parsed
 * \param[in]  length                Length of the buffer
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_profileutil_Parse(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_Init_t *init,
                                       char *jsonBuffer,
                                       uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_PROFILEUTIL_H_ */
