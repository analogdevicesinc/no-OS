/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_agc.h
 * \brief Contains ADRV904X AGC related private function prototypes for
 *        adrv904x_agc.c which helps adi_adrv904x_agc.c
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADRV904X_AGC_H_
#define _ADRV904X_AGC_H_

#include "adi_adrv904x_error.h"
#include "adi_adrv904x_agc_types.h"

/**
* \brief Range check a given AGC configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcCfg Pointer to AGC configuration to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_AgcCfg_t * const agcCfg);

/**
* \brief Range check a given AGC gain range configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcGainRangeCfg Pointer to AGC gain range configuration to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcGainRangeCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_AgcGainRange_t * const agcGainRangeCfg);


/**
* \brief Range check a given dual band AGC configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] agcDualBandCfg Pointer to dual band AGC configuration to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_AgcDualBandCfgRangeCheck(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_AgcDualBandCfg_t * const agcDualBandCfg);
#endif
