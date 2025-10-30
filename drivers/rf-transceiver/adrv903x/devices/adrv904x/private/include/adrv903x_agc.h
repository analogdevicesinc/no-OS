/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_agc.h
 * \brief Contains ADRV903X AGC related private function prototypes for
 *        adrv903x_agc.c which helps adi_adrv903x_agc.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_AGC_H_
#define _ADRV903X_AGC_H_

#include "adi_adrv903x_error.h"
#include "adi_adrv903x_agc_types.h"

/**
* \brief Range check a given AGC configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] agcCfg Pointer to AGC configuration to be range checked
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AgcCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                           const adi_adrv903x_AgcCfg_t * const agcCfg);

/**
* \brief Range check a given AGC gain range configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] agcGainRangeCfg Pointer to AGC gain range configuration to be range checked
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AgcGainRangeCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_AgcGainRange_t * const agcGainRangeCfg);


/**
* \brief Range check a given dual band AGC configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device's data structure.
* \param[in] agcDualBandCfg Pointer to dual band AGC configuration to be range checked
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_AgcDualBandCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                   const adi_adrv903x_AgcDualBandCfg_t * const agcDualBandCfg);
#endif
