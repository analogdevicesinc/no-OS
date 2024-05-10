/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_cfr.h
* \brief Contains ADRV904X CFR function prototypes for
*    adi_adrv904x_dfe_cfr.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_DFE_CFR_H_
#define _ADI_ADRV904X_DFE_CFR_H_

#include "adi_adrv904x_dfe_cfr_types.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"



/**
* \brief Configures Cfr thresholds.
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
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply threshold configuration
* \param[in]     configSelectMask - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to create this mask
*                                   User can select multiple configurations
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrThresholdSet(adi_adrv904x_Device_t* const              device,
                                                              const adi_adrv904x_CfrThrConfig_t* const  cfrThrConfig,
                                                              const uint32_t                            cfrTxChannelMask,
                                                              const uint32_t                            configSelectMask);

/**
* \brief Read Cfr thresholds from device structure
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[out]    cfrThrConfig - Pointer to Threshold configuration data structure to read from selected Tx channel
* \param[in]     cfrTxChannelSel - Tx channel selection to read CFR thresholds. Only one Tx channel should be selected
* \param[in]     configSelect - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to select configuration.
*                                    Only one configuration should be selected.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrThresholdGet(adi_adrv904x_Device_t* const          device,
                                                              adi_adrv904x_CfrThrConfig_t* const    cfrThrConfig,
                                                              const uint32_t                        cfrTxChannelSel,
                                                              const uint32_t                        configSelect);


/**
* \brief Configures Cfr engine enable/bypass settings
* 
* \pre This function is called after the device has been initialized. User can configure engine enable/bypass
* for selected configuration set/s. It's users responsibility not to change the active
* configuration set during runtime.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrEngineEnable - Pointer to cfr engine enable configuration for each engine. 1:Enable//0:Disable
* \param[in]     cfrEngineBypass - Pointer to cfr bypass configuration for each engine. 1:Bypass//0:Don't bypass
* \param[in]     arraySize - Number of elements in cfrEngineEnable and cfrEngineBypass arrays. Typically needs to
*                be set to 3 to configure all engines
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply enable/bypass configuration
* \param[in]     configSelectMask - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to create this mask
*                                    User can select multiple configurations
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrEnableSet(adi_adrv904x_Device_t* const device,
                                                           const uint32_t               cfrEngineEnable[],
                                                           const uint32_t               cfrEngineBypass[],
                                                           const uint32_t               arraySize,
                                                           const uint32_t               cfrTxChannelMask,
                                                           const uint32_t               configSelectMask);

/**
* \brief Reads Cfr engine enable/bypass settings
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in, out]    cfrEngineEnable - Pointer to readback cfr engine enable configuration for each engine. 1:Enable//0:Disable
* \param[in, out]    cfrEngineBypass - Pointer to readback cfr bypass configuration for each engine. 1:Bypass//0:Don't bypass
* \param[in]     arraySize - Number of elements in cfrEngineEnable and cfrEngineBypass arrays. Typically needs to
*                be set to 3 to configure all engines
* \param[in]     cfrTxChannelSel - Tx channel selection to read CFR thresholds. Only one Tx channel should be selected
* \param[in]     configSelect - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to select configuration.
*                                    Only one configuration should be selected.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrEnableGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t                     cfrEngineEnable[],
                                                           uint32_t                     cfrEngineBypass[],
                                                           const uint32_t               arraySize,
                                                           const uint32_t               cfrTxChannelSel,
                                                           const uint32_t               configSelect);

/**
* \brief Configures Cfr configuration for selected channel/s and config/s
* 
* \pre This function is called after the device has been initialized. User can configure CFR settings
* for selected configuration set/s. It's users responsibility not to change the active
* configuration set during runtime.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrCtrlConfig - Pointer to cfr configuration struct to be applied
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply the configuration
* \param[in]     configSelectMask - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to create this mask
*                                    User can select multiple configurations
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrConfigSet(adi_adrv904x_Device_t* const             device,
                                                           const adi_adrv904x_CfrConfig_t* const    cfrCtrlConfig,
                                                           const uint32_t                           cfrTxChannelMask,
                                                           const uint32_t                           configSelectMask);

/**
* \brief Reads Cfr configuration from device
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[out]    cfrCtrlConfig - Pointer to readback CFR configuration from selected channel and config set
* \param[in]     cfrTxChannelSel - Tx channel selection to read CFR thresholds. Only one Tx channel should be selected
* \param[in]     configSelect - Config set 0/1 selection. adi_adrv904x_CfrConfig_e can be used to select configuration.
*                                    Only one configuration should be selected.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrConfigGet(adi_adrv904x_Device_t* const     device,
                                                           adi_adrv904x_CfrConfig_t* const  cfrCtrlConfig,
                                                           const uint32_t                   cfrTxChannelSel,
                                                           const uint32_t                   configSelect);

/**
* \brief Selects the active Cfr configuration set
* 
* \pre This function is called after the device has been initialized. CFR HW has 2 sets of configurations,
* which can be writting with adi_adrv904x_CfrConfigSet, adi_adrv904x_CfrEnableSet and adi_adrv904x_CfrThresholdSet
* APIs. User can switch between these 2 config sets by using this function. If immediate update option is selected,
* API switches to selected config set immediately. Otherwise this function triggers a stream which will switch to
* desired configuration on next carrier reconfiguration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply enable/bypass configuration
* \param[in]     configSelect     - Config set 0/1 selection.
* \param[in]     immediateUpdate  - 0:Change configuration on first carrier reconfig command, 1:Immediately, 2:Next Tx fall/raise edge
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActiveCfgSet(adi_adrv904x_Device_t* const      device,
                                                              const uint32_t                    cfrTxChannelMask,
                                                              const adi_adrv904x_CfrConfigSel_e configSelect,
                                                              const uint8_t                     immediateUpdate);

/**
* \brief Reads the active Cfr configuration set
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read active configuration
* \param[out]    configSelectRead - Pointer to readback active configuration set
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActiveCfgGet(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_TxChannels_e       txChannelSel,
                                                              adi_adrv904x_CfrConfigSel_e* const    configSelectRead);

/**
* \brief Selects the active Cfr configuration set
* 
* \pre This function is called after the device has been initialized. CFR HW can store 2 pulses,
* User can switch between these 2 pulses by using this function. If immediate update option is selected,
* API switches to selected pulse immediately. Otherwise this function triggers a stream which will switch to
* desired configuration on next carrier reconfiguration.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply enable/bypass configuration
* \param[in]     pulseSelect      - Pulse slection to be used by CFR engine
* \param[in]     immediateUpdate  - 1:Immediate Update, 0:Change pulse on first carrier reconfig command
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActivePulseSet(adi_adrv904x_Device_t* const        device,
                                                                const uint32_t                      cfrTxChannelMask,
                                                                const adi_adrv904x_CfrPulseSel_e    pulseSelect,
                                                                const uint8_t                       immediateUpdate);

/**
* \brief Reads the active Cfr pulse
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read active pulse
* \param[out]    pulseSelectRead - Pointer to readback pulse
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrActivePulseGet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_TxChannels_e     txChannelSel,
                                                                adi_adrv904x_CfrPulseSel_e* const   pulseSelectRead);

/**
* \brief Configures Cfr statistics configuration for selected channel/s
* 
* \pre This function is called after the device has been initialized. User can configure CFR statistics
* for multiple channels at the same time.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrStatisticsConfig - Pointer to cfr statistics struct to be applied
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to apply statistics config
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsCfgSet(adi_adrv904x_Device_t* const                  device,
                                                                  const adi_adrv904x_CfrStatisticsCfg_t* const  cfrStatisticsConfig,
                                                                  const uint32_t                                cfrTxChannelMask);


/**
* \brief Read Cfr statistics configuration of selected channel
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[out]    cfrStatisticsConfig - Pointer to cfr statistics cfg readback struct
* \param[in]     cfrTxChannelSel - Tx channel selection to read CFR statistics configuration
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsCfgGet(adi_adrv904x_Device_t* const              device,
                                                                  adi_adrv904x_CfrStatisticsCfg_t* const    cfrStatisticsConfig,
                                                                  const uint32_t                            cfrTxChannelSel);

/**
* \brief Read Cfr statistics of selected channel
* 
* \pre This function is called after the device has been initialized. When windowed statistics enabled,
* (i.e. adi_adrv904x_CfrStatisticsCfg_t->windowEn), user can select reaading either previous or current window
* to read latest statistics. In this mode, HW updates the previous window statistics when current window expires
* and clears the current window. When windowed statistics disabled, user should always read the current window
* statistics as HW doesn't update the previous window. If clearing statistics upon reading is set, the current window statistics
* (peak counters): 
*   - will be cleared immediately if CFR engines are enabled and not bypassed;
*   - will not be cleared if CFR engines are disabled and not bypassed;
*
* Note:
*   Bypassing CFR engines will disable CFR engines' clock. In this case, clearing statistics is not working !
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[out]    cfrStatistics - Pointer to cfr statistics readback
* \param[in]     cfrTxChannelSel - Tx channel selection to read CFR statistics
* \param[in]     prevWindow - 1:Read statistics of previous window // 0: Read statistics of current window
* \param[in]     clearStats - 1:Clear statistics upon reading(will clear both cur and prev windows) // 0: Don't clear
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrStatisticsGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CfrStatistics_t* const  cfrStatistics,
                                                               const adi_adrv904x_TxChannels_e      cfrTxChannelSel,
                                                               const uint8_t                        prevWindow,
                                                               const uint8_t                        clearStats);


/**
* \brief Writes CFR final correction pulse
* 
* \pre This function is called after the device has been initialized. User can select one or multiple
* Tx channels to write the pulse data. The real part of the correction pulse should be even symmetric
* and the imaginary part of the correction pulse should be odd symmetric. User should always write the
* half of the final pulse data skipping the center tap of the pulse which will always be (32767 + 0j).
* User can select one or multiple pulse selections to write the pulse data.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to write correction pulse
* \param[in]     cfrCorrectionPulses - Array of correction pulses to be written
* \param[in]     numCorrectionPulses - Number of correction pulses in cfrCorrectionPulses array
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrCorrectionPulseWrite(adi_adrv904x_Device_t* const              device,
                                                                      const uint32_t                            cfrTxChannelMask,
                                                                      const adi_adrv904x_CfrCorrectionPulse_t   cfrCorrectionPulses[],
                                                                      const uint32_t                            numCorrectionPulses);

/**
* \brief Reads CFR final correction pulse from selected Tx channel. This
* function reads pulse data from RAM0 since all RAM banks are expected to
* store the same pulse data
* 
* \pre This function is called after the device has been initialized. 
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]        cfrTxChannelSel - Tx channel selection to read pulse data
* \param[in, out]   cfrCorrectionPulse - Pointer to readback CFR correction pulse
* \param[in]        pulseSelect - Pulse selection to read
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrCorrectionPulseRead(adi_adrv904x_Device_t* const               device,
                                                                     const adi_adrv904x_TxChannels_e            cfrTxChannelSel,
                                                                     adi_adrv904x_CfrCorrectionPulse_t* const   cfrCorrectionPulse,
                                                                     const adi_adrv904x_CfrPulseSel_e           pulseSelect);

/**
* \brief Setting CFR Peak Duration for selected TX channels and CFR Engine.
* 
* \pre This is an input to the cfr peak detector. It indicates the maximum number of samples that can be part of a peak group, 
* before it is closed.
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - One or multiple Tx channel selection to write CFR Peak Duration
* \param[in]     configSelectMask - selected CFR CFG mask as defined in adi_adrv904x_CfrEngineSel_e. Multiple CFG selections are allowed.
* \param[in]     samplesCount - Number of samples range from 0 to 255.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlPeakDurationSet(adi_adrv904x_Device_t* const           device,
                                                                        const uint32_t                         cfrTxChannelMask,
                                                                        const uint32_t                         configSelectMask,
                                                                        const uint8_t                          samplesCount);

/**
* \brief Getting CFR Peak Duration for selected TX channel and CFR Engine.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]      txChannelSel - One Tx channel selection to read CFR Peak Duration
* \param[in]      configSelect - selected CFR CFG mask as defined in adi_adrv904x_CfrEngineSel_e
* \param[in,out]  samplesCount - Number of samples range from 0 to 255.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlPeakDurationGet(adi_adrv904x_Device_t* const        device,
                                                                        const adi_adrv904x_TxChannels_e     txChannelSel,
                                                                        const adi_adrv904x_CfrConfigSel_e   configSelect,
                                                                        uint8_t* const                      samplesCount);

/**
* \brief Set the CFR Delay values for the selected TX channel to a stream scratchpad register.
 *       The values will be written to both active and inactive CFR blocks when Tx is disabled.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     cfrTxChannelMask - Tx channel mask (multiples allowed)
* \param[in]     delay0 - delay for engine 0 (0 - 1023)
* \param[in]     delay1 - delay for engine 1 (0 - 1023)
* \param[in]     delay2 - delay for engine 2 (0 - 1023)
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlDelaySet(adi_adrv904x_Device_t* const device,
                                                                 const uint32_t               cfrTxChannelMask,
                                                                 const uint16_t               delay0,
                                                                 const uint16_t               delay1,
                                                                 const uint16_t               delay2);

/**
* \brief Get the CFR Delay values for the selected TX channel, always from the first CFR block.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - One Tx channel selection
* \param[in,out] delay0 - delay for engine 0 (0 - 1023)
* \param[in,out] delay1 - delay for engine 1 (0 - 1023)
* \param[in,out] delay2 - delay for engine 2 (0 - 1023)
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfrControlDelayGet(adi_adrv904x_Device_t* const       device,
                                                                 const adi_adrv904x_TxChannels_e    txChannelSel,
                                                                 uint16_t* const                    delay0,
                                                                 uint16_t* const                    delay1,
                                                                 uint16_t* const                    delay2);
#endif /* _ADI_ADRV904X_DFE_CFR_H_ */
