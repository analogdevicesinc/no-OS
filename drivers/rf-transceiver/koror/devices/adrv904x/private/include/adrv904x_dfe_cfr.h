/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_dfe_cfr.h
 * \brief Contains ADRV904X Tx related private function prototypes for
 *        adrv904x_dfe_cfr.c which helps adi_adrv904x_dfe_cfr.c
 *
 * ADRV904X API Version: 2.9.0.4
 */ 

#ifndef _ADRV904X_DFE_CFR_H_
#define _ADRV904X_DFE_CFR_H_


#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_error.h"

#include "../../private/bf/adrv904x_bf_tx_datapath.h"

#include "../../private/bf/adrv904x_bf_tx_cfr_reg.h"
/**
* \brief Look up the Tx cfr reg bitfield address given a Tx Channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Channel selection to read back gain index for (Valid Tx0-Tx7 only)
* \param[out] cfrChannelBitfieldAddr Tx cfr channel bitfield address which will be updated by this function
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrRegsBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                      const adi_adrv904x_TxChannels_e txChannel,
                                                                      adrv904x_BfTxCfrRegChanAddr_e* const cfrChannelBitfieldAddr);


/**
* \brief Calculate and write CFR Threshold Square value to selected engine's selected CFG's
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] cfrPeakThresholdScaler Scaler for peak threshold
* \param[in] cfrPeakThreshold Peak threshold value
* \param[in] engineBaseAddr Engine bf base address
* \param[in] configSelectMask Configuration select, CFG0/CFG1/Both
* \param[in] engineId Engine Id, 0:Engine 0 // 1:Engine 1 // 2:Engine 2
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWritePeakThreshSq(adi_adrv904x_Device_t* const device,
                                                               const uint32_t cfrPeakThresholdScaler,
                                                               const uint32_t cfrPeakThreshold,
                                                               const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                               const uint32_t configSelectMask,
                                                               const uint8_t engineId);

/**
* \brief Calculate and write CFR Threshold Sqrt value to selected engine's selected CFG's
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] cfrPeakThresholdScaler Scaler for peak threshold
* \param[in] cfrPeakThreshold Peak threshold value
* \param[in] engineBaseAddr Engine bf base address
* \param[in] configSelectMask Configuration select, CFG0/CFG1/Both
* \param[in] engineId Engine Id, 0:Engine 0 // 1:Engine 1 // 2:Engine 2
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWritePeakThreshSqrt(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t cfrPeakThresholdScaler,
                                                                 const uint32_t cfrPeakThreshold,
                                                                 const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                                 const uint32_t configSelectMask,
                                                                 const uint8_t engineId);

/**
* \brief Calculate and write CFR Correction Threshold value to selected engine's selected CFG's
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] cfrCorrectionThresholdScaler Correction scaler for peak threshold
* \param[in] cfrPeakThreshold Peak threshold value
* \param[in] engineBaseAddr Engine bf base address
* \param[in] configSelectMask Configuration select, CFG0/CFG1/Both
* \param[in] engineId Engine Id, 0:Engine 0 // 1:Engine 1 // 2:Engine 2
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrWriteCorrectionThresh(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t cfrCorrectionThresholdScaler,
                                                                   const uint32_t cfrPeakThreshold,
                                                                   const adrv904x_BfTxCfrRegChanAddr_e engineBaseAddr,
                                                                   const uint32_t configSelectMask,
                                                                   const uint8_t engineId);
/**
* \brief Write CFR Pulse private helper function
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] pulseSelect Pulse selection
* \param[in] txChanIdx Channel Id to write the pulse to
* \param[in] interpolationRate Interpolation rate configured for the selected pulse
* \param[in] cfrCorrectionPulse Pointer to data structure which contains the pulse data
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrCorrectionPulseWrite(adi_adrv904x_Device_t* const                      device,
                                                                  const adi_adrv904x_CfrPulseSel_e                  pulseSelect,
                                                                  const uint32_t                                    txChanIdx,
                                                                  const uint8_t                                     interpolationRate,
                                                                  const adi_adrv904x_CfrCorrectionPulse_t* const    cfrCorrectionPulse);
	
/**
* \brief Range check CFR pulse data
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] cfrPulseData Pulse data to range check
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrPulseWriteRangeCheck(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_CfrCorrectionPulse_t * const cfrPulseData);

/**
* \brief Returns data path base address for selected channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X data structure
* \param[in] txChannel Tx channel selection
* \param[out] txDatapathChannelBitfieldAddr Data path base address of selected channel
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CfrDatapathBitfieldAddressGet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                                        adrv904x_BfTxDatapathChanAddr_e* const txDatapathChannelBitfieldAddr);

#endif /* _ADRV904X_DFE_CFR_H_ */