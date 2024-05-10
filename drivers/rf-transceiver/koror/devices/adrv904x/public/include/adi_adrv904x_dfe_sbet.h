/**
* Copyright 2023 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_sbet.h
* \brief Contains ADRV904X SBET function prototypes for
*    adi_adrv904x_dfe_sbet.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_DFE_SBET_H_
#define _ADI_ADRV904X_DFE_SBET_H_

#include "adi_adrv904x_dfe_sbet_types.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"

/**
* \brief Configures CFR thresholds for SBET feature.
*
* \pre This function is called after the device has been initialized. User can configure threshold
* for selected configuration set/s. It's users responsibility not to change the thresholds of active
* configuration set during runtime.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrThrConfig - Pointer to threshold configuration data structure to be applied selected Tx channels
*                               Same configuration will be applied to all 3 engines
* \param[in]     hardClipperThreshold - hard clipper thresldhold value used for sbet feature.
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply threshold configuration
* \param[in]     cfrPowerIndex - Cfr Power Index selection.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetCfrThresholdSet(adi_adrv904x_Device_t* const              device,
                                                                  const adi_adrv904x_CfrThrConfig_t* const  cfrThrConfig,
                                                                  const uint32_t                            hardClipperThreshold,
                                                                  const uint32_t                            cfrTxChannelMask,
                                                                  const uint32_t                            cfrPowerIndex);
/**
* \brief Configures the mapping of DPD to CFR model index for SBET feature.
*
* \pre This function is called after the device has been initialized. User can configure a different
* DPD to CFR model index mapping. It's users responsibility not to change the mapping during runtime.
* The following three configurations are supported: 
* config 0: dpd 0 --- cfr 0
*           dpd 1 --- cfr 1
*           dpd 2 --- cfr 1
*           dpd 3 --- cfr 1
*
* config 1: dpd 0 --- cfr 0
*           dpd 1 --- cfr 0
*           dpd 2 --- cfr 1
*           dpd 3 --- cfr 1
*
* config 2: dpd 0 --- cfr 0
*           dpd 1 --- cfr 0
*           dpd 2 --- cfr 0
*           dpd 3 --- cfr 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply mapping configuration
* \param[in]     mapping - SBET mapping of dpd model and cfr model.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetCfrMappingSet(adi_adrv904x_Device_t* const             device,
                                                                const uint32_t                           cfrTxChannelMask,
                                                                const uint32_t                           mapping);

/**
* \brief Configures Tx Attenuation for SBET feature.
*
* \pre This function is called after the device has been initialized. User can configure tx attenuation
* for selected tx channel/s. It will be applied when tx on is active and sbet feature enabled.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     configTx - Pointer to sbet tx attenuation configuration data structure to be applied to selected Tx channels
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetTxAttenSet(adi_adrv904x_Device_t* const device,
                                                             adi_adrv904x_SbetTxAtten_t * const configTx);
/**
* \brief retrieves Tx Attenuation for SBET feature.
*
* \pre This function is called after the device has been initialized. User can get tx attenuation
* for selected tx channel/s. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in,out] configTx - Pointer to sbet tx attenuation configuration data structure to be applied to selected Tx channels
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SbetTxAttenGet(adi_adrv904x_Device_t* const device,
                                                             adi_adrv904x_SbetTxAtten_t* const configTx);

#endif /* _ADI_ADRV904X_DFE_SBET_H_ */
