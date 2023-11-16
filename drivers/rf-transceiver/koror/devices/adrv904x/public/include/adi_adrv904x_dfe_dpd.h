/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_dpd.h
*
* \brief Contains ADRV904X DPD function prototypes for
*    adi_adrv904x_dfe_dpd.c
*
* ADRV904X API Version: 2.9.0.4
*/


#ifndef _ADI_ADRV904X_DFE_DPD_H_
#define _ADI_ADRV904X_DFE_DPD_H_

#include "adi_adrv904x_dfe_cal_dpd_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"
#include "adi_adrv904x_tx_types.h"

/** 
* \brief Configures the base DPD model for the requested Tx channel(s)
*
* This function configures a basis model for the DPD adaptation engine in the ADRV904x device. 
* The basis model configuration includes the feature set consisting of i(memory term),j(cross term),
* k(power term) and the complex coefficient values. A maximum of ADI_ADRV904X_DPD_MAX_NUM_FEATURES 
* features can be configured for the DPD adaptation engine.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel for which DPD model configuration is required
* \param[in] dpdModelType - Model type 
* \param[in] modelDesc - Pointer to the model config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdSet(adi_adrv904x_Device_t* const                         device,
                                                                   const uint32_t                                       dpdTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e           dpdModelType,
                                                                   const adi_adrv904x_DfeAppCalDpdModelDesc_t* const    modelDesc);

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
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD model configuration. Only one Tx channel should be selected
* \param[in] dpdModelType - Model type
* \param[out] modelDesc - Pointer to the model config structure which will be updated with the updated DPD model configuration.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigDpdGet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_TxChannels_e              dpdTxChannelSel,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e   dpdModelType,
                                                                   adi_adrv904x_DfeAppCalDpdModelDesc_t* const  modelDesc);

/** 
* \brief Configures the base CTC model for the requested Tx channel(s)
*
* WARNING : This API is not supported currently
* This function configures a basis model for the CTC adaptation engine in the ADRV904x device. 
* The basis model configuration includes the feature set consisting of r, c, d, k, m, j and the
* complex coefficient values. A maximum of ADI_ADRV904X_DPD_MAX_NUM_FEATURES features can be 
* configured for the CTC adaptation engine.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel for which CTC model configuration is required
* \param[in] dpdModelType - Model type 
* \param[in] modelDesc - Pointer to the model config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigCtcSet(adi_adrv904x_Device_t* const                         device,
                                                                   const uint32_t                                       dpdTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e           dpdModelType,
                                                                   const adi_adrv904x_DfeAppCalCtcModelDesc_t* const    modelDesc);

/** 
* \brief DPD reset for a requested Tx channel.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] txChannelMask - Tx channel mask for selected TX channels for DPD Reset.
* \param[in] dpdResetMode - DPD Reset mode defined in enum adi_adrv904x_DfeAppDpdResetMode_e 
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdReset(adi_adrv904x_Device_t* const            device,
                                                       const uint32_t                          txChannelMask,
                                                       const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode);

/**
* \brief Retrieves the CTC model feature set for the requested Tx channel from the device
*
* WARNING : This API is not supported currently
* This function retrieves the CTC model config updated with the feature co-efficient values
* computed from the last CTC adaptation cycle.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read CTC model configuration. Only one Tx channel should be selected
* \param[in] dpdModelType - Model type 
* \param[out] modelDesc - Pointer to the model config structure which will be updated with the updated CTC model configuration. 
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdModelConfigCtcGet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_TxChannels_e              dpdTxChannelSel,
                                                                   const adi_adrv904x_DfeAppCalDpdModelType_e   dpdModelType,
                                                                   adi_adrv904x_DfeAppCalCtcModelDesc_t* const   modelDesc);

/**
* \brief Sets the DPD gain monitor configuration
*
* WARNING : This API is not supported currently
* This function sets the DPD gain monitor/power meter configuration. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD Reset and before DPD tracking is enabled.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to apply configuration
* \param[in] dpdPowerMeterCfg - Pointer to the DPD gain monitor/power meter configuration structure  
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdActuatorGainMonitorConfigSet(adi_adrv904x_Device_t* const                          device,
                                                                              const uint32_t                                        dpdTxChannelMask,
                                                                              const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const dpdPowerMeterCfg);

/**
* \brief Read the DPD gain monitor configuration
*
* WARNING : This API is not supported currently
* This function reads the DPD gain monitor configuration/power meter configuration.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD gain monitor/power meter configuration. Only one Tx channel should be selected
* \param[out] dpdPowerMeterCfg - Pointer to the DPD gain monitor/power meter configuration structure 
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdActuatorGainMonitorConfigGet(adi_adrv904x_Device_t* const                      device,
                                                                              const adi_adrv904x_TxChannels_e                   dpdTxChannelSel,
                                                                              adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const   dpdPowerMeterCfg);

/**
* \brief Sets the DPD capture configuration
*
* This function sets the DPD capture configuration. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD Reset and before DPD tracking is enabled.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to capture configuration
* \param[in] dpdCaptureCfg - Pointer to the DPD capture config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureConfigSet(adi_adrv904x_Device_t* const                          device,
                                                                  const uint32_t                                        dpdTxChannelMask,
                                                                  const adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const    dpdCaptureCfg);

/**
* \brief Read the DPD capture configuration
*
* This function reads the DPD capture configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD capture configuration. Only one Tx channel should be selected
* \param[out] dpdCaptureCfg - Pointer to the DPD capture config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureConfigGet(adi_adrv904x_Device_t* const                  device,
                                                                  const adi_adrv904x_TxChannels_e               dpdTxChannelSel,
                                                                  adi_adrv904x_DfeAppCalDpdCaptureCfg_t* const  dpdCaptureCfg);

/**
* \brief Sets the DPD tracking calibration configuration
*
* This function sets the DPD tracking calibration configuration in the FW. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD Reset and before DPD tracking is enabled.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to apply DPD tracking calibration configuration
* \param[in] dpdTrackCfg - Pointer to the DPD tracking calibration config structure which contains the configuration to be set in FW
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigSet(adi_adrv904x_Device_t* const                     device,
                                                                   const uint32_t                                   dpdTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalDpdTrackCfg_t* const dpdTrackCfg);


/**
* \brief Read the DPD tracking calibration configuration
*
* This function reads the DPD tracking calibration configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD tracking calibration configuration. Only one Tx channel should be selected
* \param[out] dpdTrackCfg - Pointer to the DPD tracking calibration config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTrackingConfigGet(adi_adrv904x_Device_t* const                 device,
                                                                   const adi_adrv904x_TxChannels_e              dpdTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalDpdTrackCfg_t* const   dpdTrackCfg);

/**
* \brief Sets the DPD Power Meter Configuration
*
* This function sets the DPD Power Meter configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called after DPD tracking is enabled.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to apply Capture Buffers Access configuration
* \param[in] dpdPowerMeterCfg - Pointer to the DPD Power Meter config structure which contains the configuration to be set in FW
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPowerMeterConfigSet(adi_adrv904x_Device_t* const                           device,
                                                                     const uint32_t                                         dpdTxChannelMask,
                                                                     const adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const  dpdPowerMeterCfg);

/**
* \brief Read the DPD Power Meter configuration
*
* This function reads the DPD Power Meter configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after BBIC requests reading capture data by calling the function adi_adrv904x_DpdPowerMeterConfigSet().
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD Capture Buffers Access configuration. Only one Tx channel should be selected
* \param[out] dpdPowerMeterCfg - Pointer to the DPD Power Meter Configuration structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPowerMeterConfigGet(adi_adrv904x_Device_t* const                       device,
                                                                     const adi_adrv904x_TxChannels_e                    dpdTxChannelSel,
                                                                     adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t* const    dpdPowerMeterCfg);

/**
* \brief Sets the DPD Capture Buffers Access Configuration
*
* This function sets the DPD Capture Buffers Access configuration in the FW. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to apply Capture Buffers Access configuration
* \param[in] dpdCapBufAccess - Pointer to the DPD Capture Buffers Access config structure which contains the configuration to be set in FW
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                         const uint32_t                                                  dpdTxChannelMask,
                                                                         adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const dpdCapBufAccess);

/**
* \brief Read the DPD Capture Buffers Access configuration
*
* This function reads the DPD Capture Buffers Access state and capture buffer addresses
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after BBIC requests reading capture data by calling the function adi_adrv904x_DpdCaptureBuffersAccessSet().
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD Capture Buffers Access configuration. Only one Tx channel should be selected
* \param[out] dpdCapBufAccess - Pointer to the DPD Capture BUffers Access structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                         const adi_adrv904x_TxChannels_e                                 dpdTxChannelSel,
                                                                         adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const dpdCapBufAccess);
/**
 * \brief Returns the status of the actuator saturation status
 *
 * The function can be called to read back the status of the 
 * acturator saturation status including the saturation of cmul for dinsq calculation for DDR,
 * Magnitude saturation because of mag gain multiplication,GMP DDR Datapath Saturation
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channel Channel selection to read back cal status. Must be a single channel.
 * \param[out] calDpdActSatStatus Status of the actuator saturation status, as a structure of type adi_adrv904x_DfeSvcDfeActuatorSaturation_t is returned to this pointer address
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdActSatStatusGet(adi_adrv904x_Device_t* const                        device,
                                                                    const adi_adrv904x_Channels_e                       channel,
                                                                    adi_adrv904x_DfeSvcDfeActuatorSaturation_t* const   calDpdActSatStatus);

/**
 * \brief Returns the active model table
 *
 * The function can be called to read back active model table. returned value = 0 means M table, 1 for C table.
 * If DPD update mode is NOT CMT mode, M table is always in use;
 * othwerwise, M table is used for high power and C table is used for low power.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] channel Channel selection to read back cal status. Must be a single channel.
 * \param[out] calDpdActiveModelTable indicates which table is active.
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdActiveModelTableGet(adi_adrv904x_Device_t* const    device,
                                                                        const adi_adrv904x_Channels_e   channel,
                                                                        uint8_t* const                  calDpdActiveModelTable);
/**
 * \brief Read data directly from the capture buffer memory.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place. It reads data directly from capture buffer memory.
 * Intended to be used for debugging only and DPD tracking cal needs to be disabled before reading
 * back the data
 * Note: This API will NOT perform any scaling to compensate for resolution change in the FW for gain 
 *       expansion and it is up to the user to scale the data if required
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] orxChannelSel Channel selection to read back capture data. Must be a single ORx channel
 * \param[out] dpdCaptureData pointer to DPD capture data structure
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdCaptureDataFromBufMemGet(adi_adrv904x_Device_t*  const              device, 
                                                                             const adi_adrv904x_Channels_e              orxChannelSel,
                                                                             adi_adrv904x_DfeAppDpdCaptureData_t* const dpdCaptureData);

/**
* \brief Returns the status of the Dpd Cal Specific status
*
* The function can be called to read back the status of the 
* Dpd Cal Specific status in the adi_adrv904x_DfeAppCalDpdStatus_t
*
* \pre This function may be called any time after the device has been initialized, and
* initialization calibrations have taken place
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Context variable -Context variable -A pointer to the device settings structure
* \param[in] channel Channel selection to read back cal status. Must be a single channel.
* \param[out] calDpdSpecStatus Status of the Cal Specific, as a structure of type adi_adrv904x_DfeAppCalDpdStatus_t is returned to this pointer address
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeDpdCalSpecificStatusGet(adi_adrv904x_Device_t* const               device,
                                                                         const adi_adrv904x_Channels_e              channel,
                                                                         adi_adrv904x_DfeAppCalDpdStatus_t* const   calDpdSpecStatus);

/**
* \brief Sets the DPD Stability Configuration
*
* This function sets the DPD Stability configuration in the FW. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called before enabling Calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelMask - One or multiple Tx channel selection to apply DPD Stability configuration
* \param[in] dpdStabilityCfg - Pointer to the DPD Stability config structure which contains the configuration to be set in FW
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdStabilityCfgSet(adi_adrv904x_Device_t* const                   device,
                                                                 const uint32_t                                 dpdTxChannelMask,
                                                                 adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const dpdStabilityCfg);

/**
* \brief Read the DPD Stability configuration
*
* This function reads the DPD Stability configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after BBIC requests reading capture data by calling the function adi_adrv904x_DpdStabilityCfgSet().
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] dpdTxChannelSel - Tx channel selection to read DPD Stability configuration. Only one Tx channel should be selected
* \param[out] dpdStabilityCfg - Pointer to the DPD Stability config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdStabilityCfgGet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_TxChannels_e                 dpdTxChannelSel,
                                                                adi_adrv904x_DfeAppCalDpdStabilityCfg_t* const  dpdStabilityCfg);

/**
 * \brief TDD lut switching time config set for selected channel. It determins the time to switch model
 * after each tx raising edge
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] dfeTxChannelMask - channel mask to set TDD lut switching config
 * \param[in] tddLutSwitchConfig -pointer to internal dfe tdd lut switching time config
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTddLutSwitchCfgSet(adi_adrv904x_Device_t* const                         device,
                                                                    const uint32_t                                       dfeTxChannelMask,
                                                                    const adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const tddLutSwitchConfig);

/**
 * \brief TDD lut switching config get for selected channel.
 *
 * \pre This function may be called any time after the device has been initialized, and
 * initialization calibrations have taken place
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable -Context variable -A pointer to the device settings structure
 * \param[in] dpdTxChannelSel -Channel selection to get TDD lut switching config. Must be a single channel
 * \param[out] tddLutSwitchConfig -pointer to internal dfe tdd lut switching config
 *
 * \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdTddLutSwitchCfgGet(adi_adrv904x_Device_t* const                     device,
                                                                    const adi_adrv904x_TxChannels_e                  dpdTxChannelSel,
                                                                    adi_adrv904x_DfeAppDpdActTddLutSwitch_t* const   tddLutSwitchConfig);

/**
* \brief Read the WB Regularization structure that contains the buffer address and size
*
* This function reads the WB Regularization structure in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function should be called before BBIC transfers WB regularization data to get the address and length for that transfer
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in,out] DfeAppWbReg - Pointer to the WB Regularization structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/                                                                
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WbRegBufferAccess(adi_adrv904x_Device_t* const                   device,
                                                                adi_adrv904x_DfeAppwbRegularization_t* const   DfeAppWbReg);

/**
* \brief Write the WB Regularization data to the DFE
*
* This function writes the WB Regularization data to the DFE.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function is used to transfer WB regularization data from the BBIC to the DFE
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]     DfeAppWbRegData - Array of the WB Regularization data to write to the DFE
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_WbRegBufferWrite(adi_adrv904x_Device_t* const             device,
                                                               adi_adrv904x_DfeAppwbRegData_t* const    DfeAppWbRegData);

/**
* \brief Set the DPD pathdelay configuration
*
* This function sets the DPD pathdelay configuration.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function can be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]     dpdTxChannelMask - One or multiple Tx channel selection to capture configuration
* \param[in]     dpdPathdelayCfg - Pointer to the DPD pathdelay status structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelayCfg(adi_adrv904x_Device_t* const device,
                                                              const uint32_t dpdTxChannelMask,
                                                              const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg);

/**
* \brief DPD reset and pathdelay init
*
* This function resets DPD and sets the pathdelay configuration.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function can be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]     dpdTxChannelMask - One or multiple Tx channel selection to capture configuration
* \param[in]     dpdResetMode - DPD Reset mode defined in enum adi_adrv904x_DfeAppDpdResetMode_e 
* \param[in]     dpdPathdelayCfg - Pointer to the DPD pathdelay status structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdResetPathdelayInit(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t dpdTxChannelMask,
                                                                    const adi_adrv904x_DfeAppDpdResetMode_e dpdResetMode,
                                                                    const adi_adrv904x_DfeAppCalDpdPathDlyStatus_t* const dpdPathdelayCfg);

/**
* \brief Seed the DPD pathdelay
*
* This function seeds the DPD pathdelay and estimate the pathdelay during DPD tracking
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function can be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]     dpdTxChannelMask - One or multiple Tx channel selection to capture configuration
* \param[in]     dpdPathdelay - Pointer to the DPD pathdelay seed structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelaySeed(adi_adrv904x_Device_t* const                        device,
                                                               const uint32_t                                      dpdTxChannelMask,
                                                               const adi_adrv904x_DfeAppCalDpdPathDlySeed_t* const dpdPathdelay);

/**
* \brief Reset DPD pathdelay seed
*
* This function Reset the DPD pathdelay seed and seeded flag
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function can be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in]     dpdTxChannelMask - One or multiple Tx channel selection to capture configuration
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DpdPathdelayResetSeed(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t               dpdTxChannelMask);

#endif /* _ADI_ADRV904X_DFE_DPD_H_ */
