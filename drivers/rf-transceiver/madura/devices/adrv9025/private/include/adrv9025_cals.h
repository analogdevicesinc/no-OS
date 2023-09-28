/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_cals.h
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv9025_cals.h
 * \brief Contains ADRV9025 calibration related private function prototypes for
 *        adrv9025_cals.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_CALS_H_
#define _ADRV9025_CALS_H_

#include "adi_adrv9025.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADRV9025_INITOVERRIDE 0xFFFFFFFF

/**
* \brief Private Helper function to return ARM Calibration error strings.
*
*  This is a private function and is automatically called by the API.
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param errCode Error code that allows looking up
*        a ARM specific error message.
*
* \retval Returns a character array with the error message specified by
*         errCode.
*/
const char* adrv9025_CalsErrMsgGet(adi_adrv9025_Device_t* device,
                                   uint32_t               errCode);

/**
* \brief Private Helper function to validate initcal structure.
*
* \param device Structure pointer to the ADRV9025 data structure containing settings
* \param initCals Structure pointer to ADRV9025 initcal settings.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_InitCalStructureCheck(adi_adrv9025_Device_t*   device,
                                       adi_adrv9025_InitCals_t* initCals);
#ifdef __cplusplus
}
#endif

#endif
