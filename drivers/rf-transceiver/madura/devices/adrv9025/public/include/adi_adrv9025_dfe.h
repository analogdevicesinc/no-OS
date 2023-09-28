/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_dfe.h
* \brief Contains ADRV9025 DFE related function prototypes for
*        adi_adrv9025_dfe.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_DFE_H_
#define _ADI_ADRV9025_DFE_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_dfe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
* DPD Ctrl Functions
****************************************************************************
*/
/**
* \brief Configures the base DPD model for the requested Tx channel
*
* This function configures a basis model for the DPD adaptation engine
* in the ADRV9025 device. The basis model configuration includes the feature
* set consisting of i(memory term),j(cross term),k(power term) and the complex coefficient values.
* A maximum of 190 features can be configured for the DPD adaptation engine.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and running
*      the dpd and path delay init cals but before enabling the DPD tracking calibration.
*
* \note Currently, the same DPD model config is applied to all the Tx channels.
*
* \param device Pointer to the device settings structure
* \param dpdModelConfig Pointer to the dpd model config structure consisting of the dpd model config settings.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdModelConfigSet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_DpdModelConfig_t* dpdModelConfig);

/**
* \brief Retrieves the DPD model feature set for the requested Tx channel from the device
*
* This function retrieves the DPD model config updated with the feature co-efficient values
* computed from the last DPD adaptation cycle.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \note Currently, the same DPD model config is applied to all the Tx channels.
*
* \param device Pointer to the device settings structure
* \param txChannel Reserved param
* \param dpdModelConfig Pointer to the dpd model config structure which will be updated with the updated DPD model config.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdModelConfigGet(adi_adrv9025_Device_t*         device,
                                       adi_adrv9025_TxChannels_e      txChannel,
                                       adi_adrv9025_DpdModelConfig_t* dpdModelConfig);

/**
* \brief Performs a full/partial reset of the DPD engine based on the reset mode selected
*
* This function performs a full DPD engine reset / LUT Restore / Coefficient Save depending on the mode selected.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the dpd init cal has run.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which DPD reset is required to be performed
* \param dpdResetMode Enum to select the DPD reset mode or restore an existing LUT
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdReset(adi_adrv9025_Device_t*      device,
                              adi_adrv9025_TxChannels_e   txChannel,
                              adi_adrv9025_DpdResetMode_e dpdResetMode);

/**
* \brief Reads the DPD status for the requested Tx channel
*
* This function retrieves the DPD tracking calibration status and DPD
* stability metrics/statistics. Typically the DPD status is used to monitor
* the DPD tracking calibration performance, stability metrics/statistics,
* and completion status once the DPD tracking calibration has been enabled.
* It is suggested that the timeout value for this operation shound be in the
* range of 0.5s and 1s as this operation can take a while to finish. The timeout
* value can be found in adi_adrv9025_user.h as ADI_ADRV9025_GETTXDPDSTATUS_TIMEOUT_US
*
* The user can also monitor the DPD status to retrieve errors encountered
* during DPD adapatation. The DPD status returns a specific error code
* pertaining to an error.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param txChannel Enum to select the target Tx channel whose DPD status is requested.
* \param dpdStatus Pointer to the dpd status structure which will be updated with the status retrieved from the device.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdStatusGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_DpdStatus_t* dpdStatus);

/**
* \brief Sets the DPD gain monitor configuration
*
* This function sets the DPD gain monitor configuration in the FW. Gain monitoring can be used to automatically switch to selected
* models when actuator gain overrange or underrange is seen. Gain violation thresholds are user configurable.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD Reset and before DPD tracking is enabled. Users shouldn't use LUT28 and LUT29
*       when gain monitoring is enabled and Model 3 is selected as either low gain or high gain model.
*
* \param device Pointer to the device settings structure
* \param dpdActGainMonitorConfig Pointer to the DPD gain monitor config structure which contains the configuration to be set in FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                  device,
                                                     adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig);

/**
* \brief Read the DPD gain monitor configuration
*
* This function reads the DPD gain monitor configuration in the FW. Gain monitoring can be used to automatically switch to selected
* models when actuator gain overrange or underrange is seen. Gain violation thresholds are user configurable.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD Reset and before DPD tracking is enabled. Users shouldn't use LUT28 and LUT29
*       when gain monitoring is enabled and Model 3 is selected as either low gain or high gain model.
*
* \param device Pointer to the device settings structure
* \param txChannel Tx channel selection to read gain monitor configuration from.
* \param dpdActGainMonitorConfig Pointer to the DPD gain monitor config structure which contains the configuration readback from FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdActuatorGainMonitorConfigGet(adi_adrv9025_Device_t*                  device,
                                                     adi_adrv9025_TxChannels_e               txChannel,
                                                     adi_adrv9025_DpdActGainMonitorConfig_t* dpdActGainMonitorConfig);

/**
* \brief Sets the DPD tracking configuration
*
* This function sets the DPD tracking configuration in the FW
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization when DPD tracking is not enabled
*
* \param device Pointer to the device settings structure
* \param dpdTrackingConfig Pointer to the DPD tracking config structure which contains the configuration to be set in FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdTrackingConfigSet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig);

/**
* \brief Retrieves the DPD tracking configuration
*
* This function gets the DPD tracking configuration from FW
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization when DPD tracking is not enabled
*
* \param device Pointer to the device settings structure
* \param txChannel Tx Channel for which DPD tracking config is to be obtained
* \param dpdTrackingConfig Pointer to the DPD tracking config structure which will be updated with the configuration obtained from the FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdTrackingConfigGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_DpdTrackingConfig_t* dpdTrackingConfig);
/**
* \brief Sets the Enhanced DPD tracking configuration
*
* This function sets the DPD tracking configuration in the FW
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization when DPD tracking is not enabled
*
* \param device Pointer to the device settings structure
* \param dpdTrackingConfig Pointer to the DPD tracking config structure which contains the configuration to be set in FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_EnhancedDpdTrackingConfigSet(adi_adrv9025_Device_t*            device,
                                                  adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig);
/**
* \brief Retrieves the Enhanced DPD tracking configuration
*
* This function gets the DPD tracking configuration from FW
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization when DPD tracking is not enabled
*
* \param device Pointer to the device settings structure
* \param txChannel Tx Channel for which DPD tracking config is to be obtained
* \param dpdTrackingConfig Pointer to the DPD tracking config structure which will be updated with the configuration obtained from the FW
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_EnhancedDpdTrackingConfigGet(adi_adrv9025_Device_t*                    device,
                                                  adi_adrv9025_TxChannels_e                 txChannel,
                                                  adi_adrv9025_EnhancedDpdTrackingConfig_t* dpdTrackingConfig);
/**
* \brief Configures criteria for DPD fault conditions
*
* This function configures the criteria for DPD fault conditions per Tx channel
* where each metric will have 2 different thresholds that will trigger 2 different
* fault conditions and 2 other fault conditions when the thresholds have been surpassed
* a certain number of times. The metrics values can be obtained using adi_adrv9025_DpdStatusGet
* in adi_adrv9025_DpdStatistics_t within adi_adrv9025_DpdStatus_t. When this function is called,
* the persistent error status will be cleared.
*
* +-----------------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------+
* |      adi_adrv9025_DpdErrorState_e       |                                                                    Condition                                                                           |
* +-----------------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------+
* | ADI_ADRV9025_DPD_ERR_STATE_0            | (ltGt == ADI_ADRV9025_DPD_COMPARATOR_LESS_THAN)? (metricVal < threshold0 && persistentCounter0++) : (metricVal > threshold0 && persistentCounter0++)   |
* | ADI_ADRV9025_DPD_ERR_STATE_1            | (ltGt == ADI_ADRV9025_DPD_COMPARATOR_LESS_THAN)? (metricVal < threshold1 && persistentCounter1++) : (metricVal > threshold1 && persistentCounter1++)   |
* | ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_0 | (persistentCounter0 > persistentCount)?                                                                                                                |
* | ADI_ADRV9025_DPD_ERR_STATE_PERSISTENT_1 | (persistentCounter1 > persistentCount)?                                                                                                                |
* +-----------------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------+
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD fault condition will be applied to
* \param dpdFaultCond array of fault conditions to be configured
* \param numOfCond number of fault conditions in the array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdFaultConditionSet(adi_adrv9025_Device_t*           device,
                                          uint32_t                         txChannelMask,
                                          adi_adrv9025_DpdFaultCondition_t dpdFaultCond[],
                                          uint8_t                          numOfCond);

/**
* \brief Retrieves configuration for DPD fault conditions criteria
*
* This function retrieves the configuration for DPD fault conditions criteria.
* txChannel and dpdMetric fields need to be filled in to retrieve the configuration
* for a particular Tx channel and DPD metric.
* See adi_adrv9025_DpdFaultConditionSet() for more details on DPD fault conditions.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannel Tx channel for which the fault condition is to be retrieved
* \param dpdMetric DPD metric for which the fault condition is to be retrieved
* \param dpdFaultCond Pointer to fault condition structure to be retrieved
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdFaultConditionGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_TxChannels_e         txChannel,
                                          adi_adrv9025_DpdMetric_e          dpdMetric,
                                          adi_adrv9025_DpdFaultCondition_t* dpdFaultCond);

/**
* \brief Configures recovery actions(s) to be taken for the configured fault condition(s)
*
* This function configures recovery action(s) to be taken automatically if there is any
* fault conditions raised within each error states defined in adi_adrv9025_DpdErrorState_e
* that matches any metrics configured within dpdRecoveryAction.dpdMetricsMask. There are
* 4 recovery actions that can be configured per Tx channel, 1 for each error state.
* List of possible recovery actions can be found in adi_adrv9025_DpdRecoveryAction_e.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function cannot be called when DPD tracking is enabled
*
* \param device Pointer to the device settings structure
* \param txChannelMask Mask consisting of 'OR'ed Tx channels for which the DPD recovery action will be applied to
* \param recoveryActionConfig array of recovery actions to be configured
* \param numOfConfig number of recovery actions in the array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_DpdRecoveryActionSet(adi_adrv9025_Device_t*                 device,
                                          uint32_t                               txChannelMask,
                                          adi_adrv9025_DpdRecoveryActionConfig_t recoveryActionConfig[],
                                          uint8_t                                numOfConfig);

/**
* \brief Retrieves configuration for DPD recovery actions
*
* This function retrieves the configuration for DPD recovery actions.
* txChannel and dpdErrorState fields need to be filled in to retrieve the configuration
* for a particular Tx channel and DPD error state.
* See adi_adrv9025_DpdRecoveryActionSet() for more details on DPD recovery actions.
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
int32_t adi_adrv9025_DpdRecoveryActionGet(adi_adrv9025_Device_t*                  device,
                                          adi_adrv9025_TxChannels_e               txChannel,
                                          adi_adrv9025_DpdErrorState_e            dpdErrorState,
                                          adi_adrv9025_DpdRecoveryActionConfig_t* recoveryActionConfig);

/****************************************************************************
* CFR Ctrl Functions
****************************************************************************
*/
/**
* \brief Sets up the core control config parameters for the CFR engine.
*
* This function sets up the cfr mode, peak threshold, interpolation factor and the delay.
* This API is required to be called to setup CFR control config before executing the CFR init cal.
* Currently, ADI_ADRV9025_CFR_MODE1 is the only mode of operation supported by the CFR engine.
* The user is expected to provide the final correction pulse to be applied to the CFR input in this mode.
* The threshold is calculated as cfrPeakThreshold = 10^(Target PAR_dB / 20) * RMS_Input.
* The user can setup an interpolation of 1x, 2x or 4x to applied to the Tx data before peak detection.
* The user can setup the CFR engine delay between 129 and 511 which translates to n+1 cycles per engine.
* The delay will be applied to all enabled CFR engines. CFR latency will be (txDelay+1)*numCfrEnginesEnabled + 3,
* where the additional 3 cycles comes from the hard clipper. The sample rate for the cycles here are at the Tx JESD rate..
*
* Each Tx channel CFR consists of 3 cascaded CFR engines followed by a hard clipper to
* clip the few peaks that are skipped by all 3 CFR Engines. The hard clipper can be optionally
* enabled via adi_adrv9025_CfrHardClipperConfigSet() API. The CFR control config is applied to
* all 3 CFR engines by this function. The threshold is adjusted internally by the ADRV9025 firmware
* before applying it to the 3 CFR engines.
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
* \param cfrCtrlConfig An array of CFR control config structures
* \param cfrCtrlCfgArraySize No. of configs contained in cfrCtrlConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrCtrlConfigSet(adi_adrv9025_Device_t*       device,
                                      adi_adrv9025_CfrCtrlConfig_t cfrCtrlConfig[],
                                      uint8_t                      cfrCtrlCfgArraySize);

/**
* \brief Retrieves the core control config parameters for the CFR engine
*
* This function retrieves the cfr mode, peak threshold, interpolation factor and the delay
* currently programmed in the device.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization and the ARM processor boot up is complete.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose CFR control config is required to be read back
* \param cfrCtrlConfig Pointer to CFR control config struct which will be updated with the CFR control settings from the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrCtrlConfigGet(adi_adrv9025_Device_t*        device,
                                      adi_adrv9025_TxChannels_e     txChannel,
                                      adi_adrv9025_CfrCtrlConfig_t* cfrCtrlConfig);

/**
* \brief This function Enables/Disables the CFR engine present before the DPD engine in the Tx datapath.
*
* Each Tx channel CFR consists of 3 cascaded CFR engines followed by a hard clipper to
* clip the few peaks that are skipped by all 3 CFR Engines. The hard clipper can be optionally
* enabled via adi_adrv9025_CfrHardClipperConfigSet() API. The CFR control config settings can
* be applied via adi_adrv9025_CfrCtrlConfigSet() API.
*
* To apply CFR correction to Tx data, the user can set adi_adrv9025_CfrEnable_t.cfrEngineXEnable to 1 and adi_adrv9025_CfrEnable_t.cfrEngineXBypassEnable to 0.
* To bypass CFR engine, the user can set adi_adrv9025_CfrEnable_t.cfrEngineXEnable to 1 and adi_adrv9025_CfrEnable_t.cfrEngineXBypassEnable to 1.
* To disable the CFR engine completely, the user can set adi_adrv9025_CfrEnable_t.cfrEngineXEnable to 0.
*
* Please note that this is an init time function and the enabling/disabling of CFR engine cannot be performed during runtime
*
* The recommended order of sequence to configure the CFR engine is
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
* \param cfrEnable An array of CFR enable control config structures
* \param cfrEnableArraySize No. of configs contained in cfrEnable array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrEnableSet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_CfrEnable_t cfrEnable[],
                                  uint8_t                  cfrEnableArraySize);

/**
* \brief Retrieves the current state of CFR engine enables.
*
* This function retrieves the the cfr mode, peak threshold, interpolation factor and the delay
* currently programmed in the device.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after device initialization and the ARM processor boot up is complete.
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose enable status is requested
* \param cfrEnable Pointer to a CFR enable structure which will be updated with the enable settings in the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrEnableGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_CfrEnable_t* cfrEnable);

/**
* \brief This function can be used to program the complex co-efficients of the CFR base pulse or the final CFR correction pulse.
*
* This function deprecates adi_adrv9025_CfrCorrectionPulseWrite() API.
* This function programs the final correction pulses in mode1(ADI_ADRV9025_CFR_MODE1) operation.
* This function expects the user to provide only the first half the correction pulse since it is assumed that the correction pulse is symmetric.
* This API supports the programming of the correction pulses in the following use cases :-
* 1) Final correction pulse of maximum length 1025(maximum half pulse length of 512) in Mode1 for a single carrier configuration.
*    Please note that run time carrier hot swapping is not supported if a pulse length of 1025(half pulse length of 512) is used.
* 2) Two final correction pulses of maximum length 513 (maximum half pulse length of 256) corresponding to two carrier configurations for on-the-fly
*    carrier configuration switching for Mode1 operation. Run time carrier switching can be executed via adi_adrv9025_CfrActiveCorrectionPulseSet() API
*    which will activate the correction pulse corresponding to the requested carrier config.
*
* \note Since all carriers are rectangular windows in the frequency domain, their respective base pulses will all be in the form of complex sinc function in the time domain.
*       This means that the real part of the correction pulse will be even symmetric and the imaginary part of the correction pulse will be odd symmetric.
*       Since the pulse is even/odd symmetric for real/imag components we know that the center tap of the pulse will always be (32767 + 0j), which is hard coded in the device.
*       The user is only expected to program the first half of the correction pulse excluding the center tap coefficient.
*
* \note Please note that currently mode1 operation is the only mode of operation supported and programming of base pulses for mode2 operation is not yet supported
*       and therefore, this API can be used to either program 1 final correction pulse of maximum length 1025(half pulse length of 512) or program
*       2 final correction pulses of maximum length 512(half pulse length 256) corresponding to 2 carrier configurations for on the fly runtime carrier switching.
*
* \note In case of runtime carrier config switching where 2 correction pulses are required to be programmed, the user has to ensure that the 2 correction
*       half pulses are of the same length. The parameter adi_adrv9025_CfrCorrectionPulse_t.numCoeffs specifies the half pulse length
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
* \param txChannelMask Tx channel selection mask to write cfrCorrectionPulses. Multiple channel selection is allowed.
* \param cfrCorrectionPulses An array consisting of the final correction pulse(s) in Mode1 operation. Please note that in case of
*                            programming two correction pulses for carrier hot swapping,cfrCorrectionPulses[0] corresponds to
*                            ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1  and cfrCorrectionPulses[1] corresponds to ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_2
* \param numCorrectionPulses No. of correction pulse coefficients to be programmed in the CFR engine. Please note that a maximum of two correction pulses
*                            can be programmed if the half pulse length of the 2 correction pulses are less than or equal to 256 and a maximum of 1 correction
*                            pulse can be programmed if the half pulse length of the correction pulse is greater than 256
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrCorrectionPulseWrite_v2(adi_adrv9025_Device_t *device,
                                                uint32_t txChannelMask,
                                                adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                                uint8_t numCorrectionPulses);

/**
* \brief This function can be used to read back the current complex co-efficients of the final CFR correction pulse programmed in the device.
*
* This function deprecates adi_adrv9025_CfrCorrectionPulseRead() API
* This function reads the final correction pulse to be used by the CFR engine to perform CFR correction when the CFR engine is configured
* to operate in (ADI_ADRV9025_CFR_MODE1). A maximum of 2 correction pulses of half pulse length 256 or a maximum of 1 correction pulse of
* half pulse length 512 can be read back from the device.
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
* \param maxCorrectionPulsesToRead Maximum no. of correction pulses to read which is also the size of cfrCorrectionPulses parameter.
*                                  The maximum no. of correction pulses supported for read back currently is 2.
* \param cfrCorrectionPulses An array of CFR correction pulses which will be updated with the CFR correction pulses retrieved from the device
* \param numCorrectionPulsesReturned No. of correction pulses actually read back from the CFR engine. The user can pass a NULL to this param
*                                    if this info is not required.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrCorrectionPulseRead_v2(adi_adrv9025_Device_t *device,
                                               adi_adrv9025_TxChannels_e txChannel,
                                               uint8_t maxCorrectionPulsesToRead,
                                               adi_adrv9025_CfrCorrectionPulse_t cfrCorrectionPulses[],
                                               uint8_t *numCorrectionPulsesReturned);

/**
* \brief This function updates the CFR detection, correction and hard clipper (if enabled) thresholds while
* CFR is running. It's used to update these thresholds without running CFR init calibration.
* Before calling this function CFR engine thresholds and hard clipper (if enabled) thresholds
* should be updated with adi_adrv9025_CfrCtrlConfigSet and adi_adrv9025_CfrHardClipperConfigSet functions
* respectively.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Target Tx channels to update CFR thresholds
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrThresholdsRunTimeUpdate(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask);

/**
* \brief This function updates the CFR pulse coefficients while CFR is running. It can be used to update
* pulse coefficients without running CFR init calibration. Before calling this function, CFR pulse must be
* written by using adi_adrv9025_CfrCorrectionPulseWrite_v2 function. This function will update either one full
* length pulse or two separate half length pulses. For details, please refer adi_adrv9025_CfrCorrectionPulseWrite_v2 function.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param txChannelMask Target Tx channels to update CFR thresholds
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrCoefficientsRunTimeUpdate(adi_adrv9025_Device_t *device, uint32_t txChannelMask);

/**
* \brief This function enables/disables the CFR hard clipper and also configures the threshold for the hard clipper
*
* The CFR hard clipper threshold is applied to an approximation of SQRT(I^2 + Q^2). The threshold
* is normalized to 1 and is relative to 0dBFS which means that a threshold of 1 corresponds to a
* threshold of 0dBFS.
*
* ADI recommends not setting the hard clipper threshold to a value less than -7 dBFS in order to ensure optimum performance.
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
* \pre This function may be called any time after device initialization and the ARM boot up is complete.
*      The hard clipper setting is currently an init time setting and has to be setup before running the
*      the CFR init cal. There is no support for dynamically changing the hard clipper threshold during runtime.
*
* \param device Pointer to the device settings structure
* \param cfrHardClipperConfig An array of CFR hard clipper config structures
* \param cfrHardClipperCfgArraySize No. of configs in cfrHardClipperConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrHardClipperConfigSet(adi_adrv9025_Device_t*              device,
                                             adi_adrv9025_CfrHardClipperConfig_t cfrHardClipperConfig[],
                                             uint8_t                             cfrHardClipperCfgArraySize);

/**
* \brief This function retrieves the CFR Hard clipper setting for the requested Tx channel
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the ARM boot up is complete
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the CFR Hard clipper status is requested
* \param cfrHardClipperConfig Pointer to the CFR hard clipper config which will be updated with the device
*                             hard clipper config settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrHardClipperConfigGet(adi_adrv9025_Device_t*               device,
                                             adi_adrv9025_TxChannels_e            txChannel,
                                             adi_adrv9025_CfrHardClipperConfig_t* cfrHardClipperConfig);

/**
* \brief Switches the final correction pulse to be applied in the CFR engine in mode1(ADI_ADRV9025_CFR_MODE1) operation
*
* This function can be used to activate one of the two final correction pulses
* corresponding to two carrier configurations when the active carrier configuration
* is changed during runtime (carrier config hot-swapping).
*
* This function can only be executed if 2 correction pulses of length 512(half pulse length of 256)
* or lesser are programmed into the device prior to calling this function. If a single correction
* pulse of length 512 or lesser (half pulse length of 256 or lesser)is programmed in the ADRV9025 device,
* this function does not have any effect. The correction pulses can be programmed via
* adi_adrv9025_CfrCorrectionPulseWrite_v2() API.
*
* \note Please note that this function does not change the CFR configuration including peak threshold,
*       interpolation factor and CFR engine enables when the active correction pulse is switched.
*
* \note By default, the device activates ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1 on reset
*
* \note Please note that it is necessary for the CFR engines to be enabled for the active correction pulse
*       switching to take place. Calling this function when CFR engine is disabled has no effect.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This runtime function may be called any time after device initialization and two correction pulses
*      of length 512 or lesser(half pulse length of 256 or lesser) are programmed via adi_adrv9025_CfrCorrectionPulseWrite_v2() API
*      and the CFR init cal has been executed. The CFR init cal can be executed via adi_adrv9025_InitCalsRun() API. It is also
*      necessary that the mode of operation is set to ADI_ADRV9025_CFR_MODE1 via adi_adrv9025_CfrCtrlConfigSet() API.
*
* \pre It is also necessary that at the time of calling this function, the CFR engine is required to be enabled failing which
*      the active correction pulse switching will not occur.
*
* \param device Pointer to the device settings structure
* \param txChannelMask A mask consisting of 'OR'ed tx channels for which the requested correction pulse is required to be activated.
* \param cfrCorrectionPulseSel Selection for the correction pulse to activate
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrActiveCorrectionPulseSet(adi_adrv9025_Device_t *device,
                                                 uint32_t txChannelMask,
                                                 adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e cfrCorrectionPulseSel);

/**
* \brief Returns the correction pulse currently activated in the CFR engine in mode1(ADI_ADRV9025_CFR_MODE1) operation
*
* This function can be used to retrieve the status of the correction pulse currently activated in the device
*
* \note By default, the device activates ADI_ADRV9025_CFR_CARRIER_HOT_SWAP_CORR_PULSE_1 on reset
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This runtime function may be called any time after device initialization and two correction pulses
*      of length 512 or lesser(half pulse length of 256 or lesser) are programmed via adi_adrv9025_CfrCorrectionPulseWrite_v2() API
*      and the CFR init cal has been executed. The CFR init cal can be executed via adi_adrv9025_InitCalsRun() API.
*
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the active correction pulse statis is requested.
* \param cfrCorrectionPulseSel Pointer to memory which will be updated with the currently active CFR correction pulse
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrActiveCorrectionPulseGet(adi_adrv9025_Device_t *device,
                                                 adi_adrv9025_TxChannels_e txChannel,
                                                 adi_adrv9025_CfrCarrierHotSwapCorrPulseSel_e *cfrCorrectionPulseSel);

/**
* \brief  Reads the CFR status for the requested Tx channel
*
* This function retrieves the CFR pipeline delay that user will experience each time Tx channel is enabled.
* FW calculates this delay by using the CFR engine configuration.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre FW calculates pipeline delay during CFR init cal.
*
* \param device Pointer to the device settings structure
* \param txChannel Enum to select the target Tx channel whose CFR status is requested.
* \param pipelineDelay Pointer to the pipeline delay readback variable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrPipelineDelayGet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint16_t *pipelineDelay);

/**
* \brief  Reads the CFR status for the requested Tx channel
*
* This function retrieves the CFR error code for the last CFR error occured
* alongwith the statistics information which includes the no. of peaks skipped,
* no. of peaks detected and the no. of peaks clipped by each CFR engine. The
* CFR statistis are updated once every 10 milliseconds.
*
* The user can also monitor the CFR status to retrieve errors encountered
* during CFR init cal execution.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param txChannel Enum to select the target Tx channel whose CFR status is requested.
* \param cfrStatus Pointer to the  status structure which will be updated with the status retrieved from the device.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrStatusGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_TxChannels_e txChannel,
                                  adi_adrv9025_CfrStatus_t* cfrStatus);

/**
* \brief Resets the CFR statistics
*
* The user accessible CFR statistics are updated once every 10 milliseconds.
*
* This function can be used to reset the CFR statistics. The CFR statistics are
* accumulated in 64 bit registers which can be retrieved via adi_adrv9025_CfrStatusGet() API.
* When the accumulated statistics are sufficiently high, the user can reset the statistics through
* this function to avoid overflow of the firmware accumulators.
*
* \note For the CFR statistics to be reset, it is necessary that the CFR engines are enabled
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and after running the CFR init cal at least once
*
* \param device Pointer to the device settings structure
* \param txChannelMask A mask consisting of 'OR'ed tx channels for which the CFR statistics are required to be reset.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_CfrStatisticsReset(adi_adrv9025_Device_t* device,
                                        uint32_t               txChannelMask);

/****************************************************************************
* CLGC Ctrl Functions
****************************************************************************
*/

/**
* \brief Configures the closed loop gain control(CLGC) DFE settings prior to enabling the CLGC tracking cal
*
* The CLGC is a tracking calibration that can be used to compensate for PA power fluctuations within a small range by controlling
* the Tx attenuation such that the overall Tx-ORx loop gain ratio is maintained.
*
* This function is intended to be called after enabling the CLGC tracking cal. On enabling the CLGC tracking cal or the DPD tracking cal
* the CLGC comes up with the following settings
*  -> Expected loop gain = 0dB
*  -> Tx signal qualifying threshold = -70dBFS
*  -> Orx signal qualifying threshold = -70dBFS
*  -> Minimum ORx signal to noise ratio = 28.6dB
*  -> Damping factor = 0.8
*
* The CLGC tracking cal can be enabled via adi_adrv9025_TrackingCalsEnableSet() API. Please refer to adi_adrv9025_TrackingCalibrations_e
* enum for CLGC tracking calibration masks. This function can be used to setup CLGC measurement and control loop configurations
* prior to enabling the CLGC tracking. Following conditions need to be fulfilled before enabling the CLGC tracking cal :-
*
* 1) External Tx to ORx mapping information must be conveyed to the ADRV9025 device via adi_adrv9025_TxToOrxMappingSet() API in command mode
*    OR through Tx-ORx mapping GPIOs in pin control mode.
* 2) Run the Tx to ORx external path delay init cal via adi_adrv9025_InitCalsRun() API with a mask value of 0x00200000 OR run a custom external
*    path delay estimation and configure the Tx-ORx external path delay values via adi_adrv9025_ExternalPathDelaySet() API.
*
* Since the CLGC is expected to be used in conjunction with DPD, it is expected that the Tx and ORx channels are running off the same LO frequencies.
*
* The CLGC tracking calibration can also be used to passively measure the Tx and ORx power levels alongwith the loop gain which can be retrieved via
* adi_adrv9025_ClgcStatusGet() API. To disable active loop gain control and enable power measurements only, the param adi_adrv9025_ClgcConfig_t.clgcEnableGainControl
* has to be set to 0.
*
* \note Please note that loop gain control is not enabled by default on enabling the CLGC tracking calibration. Enabling the CLGC
*       tracking calibration only enables passive measurement of Tx-ORx RMS power. The user is required to explicitly
*       enable loop gain control by calling this API with adi_adrv9025_ClgcConfig_t.clgcEnableGainControl set to 1.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the following conditions are fulfilled
*      1) Tx-ORx mapping information is conveyed to the ADRV9025 device firmware via adi_adrv9025_TxToOrxMappingSet() API or through GPIOs and the target ORx
*         channel for the Tx is disabled. Enabling the ORx channel pauses the CLGC tracking calibration.
*      2) The external Tx-ORx path delay estimation is complete either by executing the external path delay init cal (mask value ADI_ADRV9025_EXTERNAL_PATH_DELAY)
*         OR estimating the Tx-ORx path delay through a custom algorithm and programming the path delay via int32_t adi_adrv9025_ExternalPathDelaySet() API
*      3) For a standalone CLGC tracking The DPD init cal (mask value ADI_ADRV9025_DPD) has to be executed. The CLGC tracking calibration is implicitly
*         enabled if the DPD tracking cal is enabled (mask value ADI_ADRV9025_TRACK_TXn_DPD).
*      4) The CLGC tracking cal (mask value ADI_ADRV9025_TRACK_TXn_CLGC) or the DPD tracking cal(mask value ADI_ADRV9025_TRACK_TXn_DPD) has been enabled
*
* \param device Pointer to the device settings structure
* \param clgcConfig An array of CLGC control configurations to be applied to the requested Tx channels
* \param clgcConfigArraySize No. of configurations contained in clgcConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ClgcConfigSet(adi_adrv9025_Device_t*    device,
                                   adi_adrv9025_ClgcConfig_t clgcConfig[],
                                   uint8_t                   clgcConfigArraySize);

/**
* \brief Retrieves the active CLGC configuration in the device.
*
* This function can be used to read back the active CLGC control configuration in the device
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose CLGC configuration is requested
* \param clgcConfig Pointer to the memory which will be updated with the current CLGC configuration
*                   for the requested Tx channel
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ClgcConfigGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_ClgcConfig_t* clgcConfig);

/**
* \brief Retrieves the Tx-ORx loop power measurements and the tracking cal status
*
* This function can be used to read back tracking cal status and the Tx-ORx loop measurements including loop gain, rms power, tx atten index and orx gain index.
* The status of the CLGC tracking cal itself including error and completion status can be retrieved via adi_adrv9025_TrackingCalTxClgcStatusGet() API.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel whose CLGC status is requested
* \param clgcStatus Pointer to the memory which will be updated with the runtime CLGC status
*                   for the requested Tx channel
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ClgcStatusGet(adi_adrv9025_Device_t*     device,
                                   adi_adrv9025_TxChannels_e  txChannel,
                                   adi_adrv9025_ClgcStatus_t* clgcStatus);

/**
* \brief Config customer waveform data for enhanced DPD wideband regularization feature
*
* This function can be used to config waveform data I/Q into FW before enhanced DPD wideband regularization feature enable.
* This function should be called before adi_adrv9025_EnhancedDpdTrackingConfigSet() API. And then, could call adi_adrv9025_EnhancedDpdTrackingConfigSet()
* to enable wideband regularization feature.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param device Pointer to the device settings structure
* \param waveformDataReal Customer waveform data I part, Q14 format, bit15: sign flag, bit14: integer part, bit13 - bit0, fractional part
* \param waveformDataImag Customer waveform data Q part, the same with I part
* \param dataCount waveform data sample count, should be less than 2048
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_EnhancedDpdWidebandRegularizationConfigSet(adi_adrv9025_Device_t* device,
                                                                uint16_t               waveformDataReal[],
                                                                uint16_t               waveformDataImag[],
                                                                uint32_t               dataCount);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_DFE_H_ */
