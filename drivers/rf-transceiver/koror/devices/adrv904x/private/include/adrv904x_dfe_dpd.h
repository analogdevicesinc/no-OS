/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_dfe_dpd.h
 * \brief Contains ADRV904X Tx related private function prototypes for
 *        adrv904x_dfe_dpd.c which helps adi_adrv904x_dfe_dpd.c
 *
 * ADRV904X API Version: 2.10.0.4
 */ 

#ifndef _ADRV904X_DFE_DPD_H_
#define _ADRV904X_DFE_DPD_H_

#include "adi_adrv904x_error.h"

/**
* \brief Performs range check on DPD Model configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] dpdModelType - selected DPD model type
* \param[in] modelDesc - pointer to DPD model parameter structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdModelConfigDpdSetRangeCheck(adi_adrv904x_Device_t* const                      device,
                                                                 const uint32_t                                    dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdModelType_e        dpdModelType,
                                                                 const adi_adrv904x_DfeAppCalDpdModelDesc_t* const modelDesc);

/**
* \brief Performs range check on DPD CTC Model configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] dpdModelType - selected DPD model type
* \param[in] modelDesc - pointer to CTC model parameter structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdModelConfigCtcSetRangeCheck(adi_adrv904x_Device_t* const                      device,
                                                                 const uint32_t                                    dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdModelType_e        dpdModelType,
                                                                 const adi_adrv904x_DfeAppCalCtcModelDesc_t* const modelDesc);

/**
* \brief Performs range check on DPD actuator gain monitor configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] dpdPowerMeterCfg - pointer to DPD power meter structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdActuatorGainMonitorConfigSetRangeCheck(adi_adrv904x_Device_t* const                          device,
                                                                            const uint32_t                                        dpdTxChannelMask,
                                                                            const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);

/**
* \brief Performs range check on DPD capture configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] dpdCaptureCfg - pointer to DPD capture structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdCaptureConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                const uint32_t                                     dpdTxChannelMask,
                                                                const adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const dpdCaptureCfg);

/**
* \brief Performs range check on DPD capture configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] dpdTrackCfg - pointer to DPD tracking structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdTrackingConfigSetRangeCheck(adi_adrv904x_Device_t* const                       device,
                                                                 const uint32_t                                     dpdTxChannelMask,
                                                                 const adi_adrv904x_DfeAppCalDpdTrackCfg_t* const   dpdTrackCfg);

/**
* \brief Performs range check on DPD TDD Lut Switch configuration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
* \param[in] dpdTxChannelMask - Tx channel selected
* \param[in] tddLutSwitchConfig - pointer to DPD TDD Lut Switch structure configured to be range checked
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
adi_adrv904x_ErrAction_e adrv904x_DpdTddLutSwitchCfgSetRangeCheck(adi_adrv904x_Device_t* const                           device,
                                                                  const uint32_t                                         dpdTxChannelMask,
                                                                  const adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const   tddLutSwitchConfig);

#endif /* _ADRV904X_DFE_DPD_H_ */