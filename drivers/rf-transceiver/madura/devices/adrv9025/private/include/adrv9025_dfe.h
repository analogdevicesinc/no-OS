/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_dfe.h
* \brief Contains ADRV9025 DFE related function prototypes for
*        adrv9025_dfe.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_DFE_H_
#define _ADRV9025_DFE_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_dfe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Performs range check on the DPD model config params
*
* \pre This function is automatically called as part of adi_adrv9025_DpdModelConfigSet
*      if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdModelConfig Pointer to the dpd model config structure consisting of the dpd model config settings.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdModelConfigSetRangeCheck(adi_adrv9025_Device_t*         device,
                                             adi_adrv9025_DpdModelConfig_t* dpdModelConfig);

/**
* \brief Encodes the dpd model into config data format as expected by ADRV9025 firmware
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdModelConfig Pointer to the dpd model config structure consisting of the dpd model config settings.
* \param armConfigData Array which will be updated with the encoded dpd model config data
* \param arraySize Size of the armConfigData array in bytes
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdModelConfigEncode(adi_adrv9025_Device_t*         device,
                                      adi_adrv9025_DpdModelConfig_t* dpdModelConfig,
                                      uint8_t                        armConfigData[],
                                      uint16_t                       arraySize);

/**
* \brief Decodes the dpd model config data received from ADRV9025 firmware and updates
*        the dpd model config structure
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param armConfigData Array consisiting of the DPD model config data received from ADRV9025 firmware
* \param arraySize Size of the armConfigData array in bytes
* \param dpdModelConfig Pointer to the dpd model config structure which will be updated with
*        the DPD model config settings retrieved from the ADRV9025 device.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdModelConfigDecode(adi_adrv9025_Device_t*         device,
                                      uint8_t                        armConfigData[],
                                      uint16_t                       arraySize,
                                      adi_adrv9025_DpdModelConfig_t* dpdModelConfig);

/**
* \brief Deserialize raw data of DPD status into the structure
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdStatus Pointer to adi_adrv9025_DpdStatus_t where it will be populated with data deserialized from buffer
* \param buf Pointer to buffer containing the raw data
* \param size size of buffer
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdStatusDeserialize(adi_adrv9025_Device_t*    device,
                                      adi_adrv9025_DpdStatus_t* dpdStatus,
                                      uint8_t*                  buf,
                                      uint8_t                   bufSize);

/**
* \brief Performs range check on the DPD gain monitor config
*
* \pre This function is automatically called as part of adi_adrv9025_DpdActuatorGainMonitorConfigSet
*      if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdActGainMonitorConfig Pointer to the DPD gain monitor config structure consisting of the DPD gain monitor config settings.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdActuatorGainMonitorConfigRangeCheck(adi_adrv9025_Device_t*                  device,
                                                        adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig);

/**
* \brief Performs range check on the DPD tracking config
*
* \pre This function is automatically called as part of adi_adrv9025_DpdTrackingConfigSet
*      if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdTrackingConfig Pointer to the DPD tracking config structure consisting of the DPD tracking config settings.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdTrackingConfigRangeCheck(adi_adrv9025_Device_t*            device,
                                             adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig);

/**
* \brief Performs range check on the DPD fault conditions
*
* \pre This function is automatically called as part of adi_adrv9025_DpdFaultConditionSet
*       and adi_adrv9025_DpdFaultConditionSet if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD fault condition will be applied to
* \param dpdFaultCond Pointer to array of fault conditions to check
* \param numOfCond Number of fault conditions in the array
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpDFaultConditionRangeCheck(adi_adrv9025_Device_t*                  device,
                                             uint32_t                                txChannelMask,
                                             const adi_adrv9025_DpdFaultCondition_t* dpdFaultCond,
                                             const uint8_t                           numOfCond);

/**
* \brief Configures criteria for DPD fault condition
*
* Please see adi_adrv9025_DpdFaultConditionSet() for more details
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD fault condition will be applied to
* \param dpdFaultCond Pointer to fault condition to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdFaultConditionSet(adi_adrv9025_Device_t*                  device,
                                      uint32_t                                txChannelMask,
                                      const adi_adrv9025_DpdFaultCondition_t* dpdFaultCond);

/**
* \brief Retrieves configuration for DPD fault condition criteria
*
* Please see adi_adrv9025_DpdFaultConditionGet() for more details
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannel Tx channel for which the fault condition is to be retrieved
* \param dpdMetric dpd metric for which the fault condition is to be retrieved
* \param dpdFaultCond Pointer to fault condition structure to be retrieved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdFaultConditionGet(adi_adrv9025_Device_t*            device,
                                      adi_adrv9025_TxChannels_e         txChannel,
                                      adi_adrv9025_DpdMetric_e          dpdMetric,
                                      adi_adrv9025_DpdFaultCondition_t* dpdFaultCond);

/**
* \brief Performs range check on the DPD recovery action configs
*
* \pre This function is automatically called as part of adi_adrv9025_DpdRecoveryActionSet
*       and adi_adrv9025_DpdRecoveryActionGet if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD recovery action will be applied to
* \param recoveryActionConfig Pointer to array of recovery action configs to check
* \param numOfConfig Number of recovery action configs in the array
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdRecoveryActionConfigRangeCheck(adi_adrv9025_Device_t*                        device,
                                                   uint32_t                                      txChannelMask,
                                                   const adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig,
                                                   const uint8_t                                 numOfConfig);

/**
* \brief Configures recovery actions to be taken for the configured fault condition
*
* Please see adi_adrv9025_DpdRecoveryActionSet() for more details
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD recovery action will be applied to
* \param recoveryActionConfig Pointer to recovery action structure to be configured
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdRecoveryActionSet(adi_adrv9025_Device_t*                        device,
                                      uint32_t                                      txChannelMask,
                                      const adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig);

/**
* \brief Retrieves configuration for DPD recovery action
*
* Please see adi_adrv9025_DpdRecoveryActionGet() for more details
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannel Tx channel for which the recovery action is to be retrieved
* \param dpdErrorState DPD error state for which the recovery action config is to be retrieved
* \param recoveryActionConfig Pointer to recovery action config structure to be retrieved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_DpdRecoveryActionGet(adi_adrv9025_Device_t*                  device,
                                      adi_adrv9025_TxChannels_e               txChannel,
                                      adi_adrv9025_DpdErrorState_e            dpdErrorState,
                                      adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig);

/**
* \brief Performs range check on the CFR Hard clipper threshold
*
* \pre This function is automatically called as part of adi_adrv9025_CfrHardClipperConfigSet
*      if macro ADI_ADRV9025_CFR_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param cfrHardClipperConfig Pointer to the CFR hard clipper config structure consisting of the hard clipper config settings.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrHardClipperConfigSetRangeCheck(adi_adrv9025_Device_t*              device,
                                                   adi_adrv9025_CfrHardClipperConfig_t cfrHardClipperConfig[],
                                                   uint8_t                             cfrHardClipperCfgArraySize);

/**
* \brief Performs range check on the CFR correction pulse write parameters
*
* \pre This function is automatically called as part of adi_adrv9025_CfrCorrectionPulseWrite_v2
*      if macro ADI_ADRV9025_CFR_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param cfrCorrectionPulses An array consisting of the final correction pulse(s) in Mode1 operation
* \param numCorrectionPulses No. of correction pulse coefficients to be programmed in the CFR engine. Please note that a maximum of two correction pulses
*                            can be programmed if the half pulse length of the 2 correction pulses are less than or equal to 256 and a maximum of 1 correction
*                            pulse can be programmed if the half pulse length of the correction pulse is greater than 256.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCorrectionPulseWriteRangeCheck(adi_adrv9025_Device_t*            device,
                                                   adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                                   uint8_t                           numCorrectionPulses);

/**
* \brief Performs range check on the CFR core ctrl config params
*
* \pre This function is automatically called as part of adi_adrv9025_CfrCtrlConfigSet
*      if macro ADI_ADRV9025_CFR_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param cfrCtrlConfig Array of CFR core ctrl config structures
* \param cfrCtrlCfgArraySize No. of elements in the cfrCtrlConfig array
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCtrlConfigSetRangeCheck(adi_adrv9025_Device_t*       device,
                                            adi_adrv9025_CfrCtrlConfig_t cfrCtrlConfig[],
                                            uint8_t                      cfrCtrlCfgArraySize);

/**
* \brief Performs range check on the CFR core ctrl config params
*
* \pre This function is automatically called as part of adi_adrv9025_CfrActiveCorrectionPulseSet
*      if macro ADI_ADRV9025_CFR_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask A mask of 'OR'ed Tx channels for which the active correction pulse is required to be switched
* \param cfrCorrectionPulseSel The correction pulse to be activated for the requested Tx channels
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrActiveCorrectionPulseSetRangeCheck(adi_adrv9025_Device_t*                       device,
                                                       uint32_t                                     txChannelMask,
                                                       adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e cfrCorrectionPulseSel);

/**
* \brief Gets the CFR ctrl command configuration given the ctrl ID
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel channel for which the CFR core config will be applied
* \param subCmdId CFR Ctrl command ID to be executed
* \param armCtrlData Array consisting of CFR ctrl data to be written to ARM mailbox
* \param armCtrlDataSize No. of elements in armCtrlData array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCtrlCmdGet(adi_adrv9025_Device_t*    device,
                               adi_adrv9025_TxChannels_e txChannel,
                               uint8_t                   subCmdId,
                               uint8_t                   armCtrlData[],
                               uint8_t                   armCtrlDataSize);

/**
* \brief Executes the CFR ctrl command given the ctrl ID and the config data
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the CFR core config will be applied
* \param subCmdId CFR Ctrl command ID to be executed
* \param armCtrlData Array consisting of CFR ctrl data to be written to ARM mailbox
* \param armCtrlDataSize No. of elements in armCtrlData array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCtrlCmdExecute(adi_adrv9025_Device_t* device,
                                   uint8_t                txChannelMask,
                                   uint8_t                subCmdId,
                                   uint8_t                armCtrlData[],
                                   uint8_t                armCtrlDataSize);

/**
* \brief Reads a single CFR correction pulse coefficient via ARM Mailbox I/F
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose coefficient is required to be read
* \param pulseCoeffIndex Index of the coefficient to be retrieved
* \param pulseCoeffReal Real part of the CFR correction pulse coefficient retrieved from the device
* \param pulseCoeffImg Imaginary part of the CFR correction pulse coefficient retrieved from the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrPulseCoeffRead(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_TxChannels_e txChannel,
                                   uint16_t                  pulseCoeffIndex,
                                   int16_t*                  pulseCoeffReal,
                                   int16_t*                  pulseCoeffImg);

/**
* \brief Reads the CFR statistics for the requested Tx channel
*
* This function retrieves the no. of peaks detected and the no. of peaks corrected
* alongwith the no. of peaks clipped by the CFR engine.Please refer to adi_adrv9025_dfe_types.h for
* a detailed description of the CFR statistics data structure.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param txChannel Enum to select the target Tx channel whose CFR status is requested.
* \param cfrStatistics Pointer to the CFR statistics structure which will be updated with the statistics retrieved from the device.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrStatisticsGet(adi_adrv9025_Device_t*        device,
                                  adi_adrv9025_TxChannels_e     txChannel,
                                  adi_adrv9025_CfrStatistics_t* cfrStatistics);

/**
* \brief Concatenates a given array of CFR correction pulses into a single correction pulse
*
* This function can be used to either duplicate a single correction pulse of length less than 256 or
* or 2 different correction pulses of length less than 256 each. Concatenation of 2 correction pulses
* allows programming of 2 correction pulses via existing firmware.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param cfrCorrPulses An array of input correction pulses to concatenate
* \param numCorrectionPulses No. of correction pulses to be concatenated.
*                            A value of 1 duplicates the correction pulse and writes it back to cfrCorrPulseConcatenated
* \param cfrCorrPulseConcatenated Pointer to the correction pulse consisting of the concatenated correction pulses
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCorrectionPulsesConcatenate(adi_adrv9025_Device_t*             device,
                                                adi_adrv9025_CfrCorrectionPulse_t  cfrCorrPulses[],
                                                uint8_t                            numCorrectionPulses,
                                                adi_adrv9025_CfrCorrectionPulse_t* cfrCorrPulseConcatenated);

/**
* \brief Performs range check on the CLGC config parameters
*
* This function is automatically called as part of adi_adrv9025_ClgcConfigSet() API if ADI_ADRV9025_CLGC_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param clgcConfig An array of CLGC control configurations to be applied to the requested Tx channels
* \param clgcConfigArraySize No. of configurations contained in clgcConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ClgcConfigSetRangeCheck(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_ClgcConfig_t clgcConfig[],
                                         uint8_t                   clgcConfigArraySize);

/**
* \brief Configures the closed loop gain control(CLGC) DFE settings prior to enabling the CLGC tracking cal
*
* This function is automatically called as part of adi_adrv9025_ClgcConfigSet() API to write the CLGC configuration to the ARM mailbox
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param clgcConfig Pointer to the CLGC config to be applied to the requested Tx channels
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ClgcConfigSet(adi_adrv9025_Device_t*     device,
                               adi_adrv9025_ClgcConfig_t* clgcConfig);

/**
* \brief This function can be used to program the complex co-efficients of the final CFR correction pulse. This function is intended to be
*        used when the CFR engine is operating in ADI_ADRV9025_CFR_MODE1 mode.
*
* This function writes the final correction pulse to be used by the CFR engine to perform CFR correction when the CFR engine is configured
* to operate in (ADI_ADRV9025_CFR_MODE1). A maximum of 512 coefficients can be programmed for CFR correction. The user is expected to
* provide only half the correction pulse required for CFR correction since the pulse is symmetric.
*
* The recommended sequence to configure the CFR engine is
* 1) Program the CFR control config via adi_adrv9025_CfrControlConfigSet() API
* 2) Program the CFR correction pulse using adi_adrv9025_CfrCorrectionPulseWrite_v2() API
* 3) Enable the CFR engine via adi_adrv9025_CfrEnableSet() API
* 4) Optionally configure the hard clipper via adi_adrv9025_CfrHardClipperConfigSet() API
* 5) Execute the CFR init cal
* 6) Optionally set the active correction pulse to use in Mode 1 via adi_adrv9025_CfrActiveCorrectionPulseSet() API for
*    carrier config hot-swapping
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization and the ARM processor boot up is complete
*      but before the CFR init cal is executed.
*
* \param device Pointer to the device settings structure
* \param txChannelMask Reserved Param
* \param coeffReal An array consisting of the Real part of the complex CFR correction pulse coefficients
* \param coeffImaginary An array consisting of the imaginary part of the complex CFR correction pulse coefficients
* \param numCoeffs No. of correction pulse coefficients to be programmed in the CFR engine
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCorrectionPulseWrite(adi_adrv9025_Device_t* device,
                                         uint32_t               txChannelMask,
                                         int16_t                coeffReal[],
                                         int16_t                coeffImaginary[],
                                         uint16_t               numCoeffs);

/**
* \brief This function can be used to read back the current complex co-efficients of the final CFR correction pulse programmed in the device.
*
* This function reads the final correction pulse to be used by the CFR engine to perform CFR correction when the CFR engine is configured
* to operate in (ADI_ADRV9025_CFR_MODE1). A maximum of 512 coefficients can be read back from the device.
*
* \note Please note that this function can be called only when the Tx channel is off and the CFR engines are not active.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called only when the target Tx channel is off after device initialization and the ARM processor boot up is complete.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose correction pulse coefficients are requested
* \param coeffReal An array which will be updated with the Real part of the complex CFR correction pulse coefficients programmed in the device
* \param coeffImaginary An array which will be updated with the imaginary part of the complex CFR correction pulse coefficients programmed in the device
* \param numCoeffs No. of correction pulse coefficients to be read back from the CFR engine
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_CfrCorrectionPulseRead(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_TxChannels_e txChannel,
                                        int16_t                   coeffReal[],
                                        int16_t                   coeffImaginary[],
                                        uint16_t                  numCoeffs);

/**
* \brief This function is used to encapsulate the process of writing both the dpd and enhanced dpd tracking config structures to the fw
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param trackingConfig Target Tx channel whose correction pulse coefficients are requested
* \param enhancedTrackingConfig An array which will be updated with the Real part of the complex CFR correction pulse coefficients programmed in the device
* \param useEnhanced True or false flag indicating whether its enhanced dpd or not i.e. true for enhanced
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_dpdTrackingConfigSet(adi_adrv9025_Device_t*                    device,
                                      adi_adrv9025_DpdTrackingConfig_t*         trackingConfig,
                                      adi_adrv9025_EnhancedDpdTrackingConfig_t* enhancedTrackingConfig,
                                      uint8_t                                   useEnhanced);
/**
* \brief This function is used to encapsulate the process of read either the dpd or enhanced dpd tracking config structures from the fw
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannel tx channel to set configuration on
* \param trackingConfig Target Tx channel whose correction pulse coefficients are requested
* \param enhancedTrackingConfig An array which will be updated with the Real part of the complex CFR correction pulse coefficients programmed in the device
* \param useEnhanced True or false flag indicating whether its enhanced dpd or not i.e. true for enhanced
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_dpdTrackingConfigGet(adi_adrv9025_Device_t*                    device,
                                      adi_adrv9025_TxChannels_e                 txChannel,
                                      adi_adrv9025_DpdTrackingConfig_t*         trackingConfig,
                                      adi_adrv9025_EnhancedDpdTrackingConfig_t* enhancedTrackingConfig,
                                      uint8_t                                   useEnhanced);
/**
* \brief Performs range check on the DPD tracking config
*
* \pre This function is automatically called as part of adrv9025_DpdTrackingConfigSet
*      if macro ADI_ADRV9025_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param dpdTrackingConfig Pointer to the Enhanced DPD tracking config structure consisting of the DPD tracking config settings.
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_EnhancedDpdTrackingConfigRangeCheck(adi_adrv9025_Device_t*            device,
                                                     adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig);
#ifdef __cplusplus
}
#endif

#endif /* _ADRV9025_DFE_H_ */
