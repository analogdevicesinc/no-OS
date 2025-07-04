/*!
 * \brief     ADC definition headers
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_ADC
 * @{
 */
#ifndef __ADI_APOLLO_ADC_H__
#define __ADI_APOLLO_ADC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_adc_types.h"
#include "adi_apollo_cfg_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Execute an ADC init (foreground) calibration
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          Target ADC selector \ref adi_apollo_adc_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_cal(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs);

/**
 * \brief Execute an ADC init (foreground) calibration with config option
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in] init_cal_cfg  Initial cal configuration. Specify init defaults, NVM or user defined. \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_init_cal(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * @brief Start the ADC initialization calibration process without waiting for completion.
 *
 * This function initializes and starts the ADC foreground calibration for the specified ADC blocks.
 * It sets up the calibration configuration and issues the necessary mailbox commands to begin calibration.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in] init_cal_cfg  Initial cal configuration. Specify init defaults, NVM or user defined. \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_init_cal_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * @brief Wait for ADC initialization calibration to complete.
 *
 * It logs progress and detailed error information if the calibration fails or times out.
 *
 * \param[in] device        Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs          Target ADC selector \ref adi_apollo_adc_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_init_cal_complete(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs);

/**
 * \brief  Write adc tlines offset
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] offset            Offset for ADC Transmission Lines
 * \param[in] side_sel          side \ref adi_apollo_side_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_tlines_offset_set(adi_apollo_device_t *device, int8_t offset, adi_apollo_side_select_e side_sel);

/**
 * \brief  Set ADC Sync Path Delay
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] delay_type        Type of delay \ref adi_apollo_sync_path_delay_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_sync_path_delay_set(adi_apollo_device_t *device, adi_apollo_sync_path_delay_e delay_type);

/**
 * \brief  Set ADC Nyquist Zone
 *
 * \param[in] device            Context variable - Pointer to the APOLLO device data structure
 * \param[in] adcs              Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in] nyquist_zone      Nyquist zone: 1 = first Nyquist, 2 = second Nyquist
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_nyquist_zone_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t nyquist_zone);


/**
 * \brief   Get ADC Nyquist Zone
 * \note    Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  nyquist_zone        Pointer to Nyquist zone return val. 1 = first Nyquist, 2 = second Nyquist
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_nyquist_zone_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *nyquist_zone);

/**
 * \brief  Get ADC background calibration state
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out] state        Array of \ref adi_apollo_adc_bgcal_state_t structs containing per ADC bgcal error and state info
 *							Array/ADC indexes for 4T4R: 0: ADC A0, 1: ADC A1, 4: ADC B0, 5: ADC B1
 * \param[in]  len          Length of state array (should be 8)
 *
 * Common ADC bgcal states:
 *	0x1A = > APOLLO_CALFRMWRK_STATE_RESUMED   | APOLLO_CALFRMWRK_STATE_RUNNING  | APOLLO_CALFRMWRK_STATE_ENABLED
 *	0x25 = > APOLLO_CALFRMWRK_STATE_SUSPENDED | APOLLO_CALFRMWRK_STATE_INACTIVE | APOLLO_CALFRMWRK_STATE_DISABLED
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_bgcal_state_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_adc_bgcal_state_t state[], uint32_t len);

/**
 * \brief  Freeze the ADC background calibration
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 *
 * \note Use \ref adi_apollo_adc_bgcal_state_get to get background cal state info
*/
int32_t adi_apollo_adc_bgcal_freeze(adi_apollo_device_t* device, adi_apollo_blk_sel_t adcs);

/**
 * \brief  Unfreeze the ADC background calibration
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 *
 * \note Use \ref adi_apollo_adc_bgcal_state_get to get background cal state info
*/
int32_t adi_apollo_adc_bgcal_unfreeze(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs);

/**
 * \brief   Enable or Disable the ADC mode switching feature
 *
 * \note    To enable ADC mode switching, this API needs to be called BEFORE loading the device profile into DUTs memory
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  enable       1 enables the mode switching feature, 0 disables it
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_mode_switch_enable_set(adi_apollo_device_t *device, uint8_t enable);

/**
 * \brief  Perform setup sequence for the ADC Mode Switch for select ADCs.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_mode_switch_prepare(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs);

/**
 * \brief  Execute ADC Slice Mode Switch for select ADCs. For fast switch, this API is replaced by GPIO toggle with func mode ADI_APOLLO_FUNC_ADC_MODE_SWITCH
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]  method       Execute switch via fw or regmap. Parameter ADI_APOLLO_ADC_MODE_SWITCH_BY_GPIO is invalid \ref adi_apollo_adc_mode_switch_method_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_mode_switch_execute(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, adi_apollo_adc_mode_switch_method_e method);

/**
 * \brief  Perform restore/cleanup sequence for the ADC Mode Switch for select ADCs.
 *
 * \param[in]  device         Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs           Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]  enable_bgcal   1: Re-enable background cals 0: leave bgcal disabled
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_mode_switch_restore(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint8_t enable_bgcal);

/**
 * \brief  Program fast detect settings
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adcs         Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]  config       Fast detect settings \ref adi_apollo_adc_fast_detect_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_fast_detect_pgm(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, const adi_apollo_adc_fast_detect_pgm_t *config);

/**
 * \brief  Read fast detect status from fw
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  adc          Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out] fast_detect  1 if signal is detected, 0 otherwise
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_fast_detect_status_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint8_t *fast_detect);

/**
 * \brief  Get ADC status data
 *
 * \param[in]   device       Context variable - Pointer to the APOLLO device data structure
 * \param[out]  status       ADC status \ref adi_apollo_adc_status_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_adc_status_get(adi_apollo_device_t *device, adi_apollo_adc_status_t *status);

/**
 * \brief   Sets ADC cal gating on groups of calibrations preventing them from operating during background calibration.
 * \note    ADC BG Cal should be freezed (i.e paused) before calling this API.
 *          Changes made to the Calibration Gating configuration will take effect upon re-enabling bg calibration.
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs                Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]   cal_gates           ADC Background Calibration Gating Groups. \ref adi_apollo_adc_cal_group_gate_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_bg_cal_grp_gate_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t cal_gates);


/**
 * \brief   Gets ADC cal gating applied on groups of background calibration.
 * \note    ADC BG Cal should be freezed (i.e paused) before calling this API.
 *          Changes made to the Calibration Gating configuration will take effect upon re-enabling bg calibration.
 *          Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  cal_gates           ADC Background Calibration Gating Groups. \ref adi_apollo_adc_cal_group_gate_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_bg_cal_grp_gate_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *cal_gates);

/**
 * \brief   ADC DEBUG ONLY (deliberately vague)
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs                Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]   debug_val           ADC Debug Value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_debug0_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t debug_val);


/**
 * \brief   ADC DEBUG ONLY (deliberately vague)
 * \note    Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  debug_val           ADC Debug Value
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_debug0_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *debug_val);

/**
 * \brief   Gets ADC input protection status for monitored events like power, over-range detection and amplitude diversion.
 *          When a given status toggles high, it will remain high until this API is called to read it.
 *          Upon reading, the status will be cleared to 0.
 * \note    Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  input_status        Pointer to Struct with ADC input status. \ref adi_apollo_adc_input_status_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_input_status_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, adi_apollo_adc_input_status_t *input_status);

/**
 * \brief   Configures the ADC input level which will engage Over Range Protection.
 *          The threshold can be adjusted in 0.5 dB steps from -0.5 dBFS to -4 dBFS,
 *          relative to nominal (500 mVpp) full-scale. Default threshold is -1 dBFS.
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs                Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]   ovr_threshold       Over-Range protection threshold level. \ref adi_apollo_adc_ovr_threshold_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_ovr_threshold_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t ovr_threshold);

/**
 * \brief   Gets ADC's input level threshold which would engage Over-Range Protection.
 * \note    Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  ovr_threshold       Over-Range protection threshold level. \ref adi_apollo_adc_ovr_threshold_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_ovr_threshold_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *ovr_threshold);

/**
 * \brief   Configures the number of samples at or above the Over-Range Threshold required to
 *          engage Over-Range Protection.
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adcs                Target ADC selector \ref adi_apollo_adc_select_e
 * \param[in]   num_samples         Num of samples at or above threshold. Range 1 - 10000 Samples.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_ovr_samples_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t adcs, uint32_t num_samples);

/**
 * \brief   Gets number of samples at or above over-range threshold which would engage Over-Range Protection.
 * \note    Only one ADC select per call. Select one within ADI_APOLLO_ADC_A0 - ADI_APOLLO_ADC_B3.
 *
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc                 Target ADC selector \ref adi_apollo_adc_select_e
 * \param[out]  num_samples         Num of samples at or above threshold. Range 1 - 10000 Samples.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_adc_ovr_samples_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t adc, uint32_t *num_samples);

#ifdef __cplusplus
}
#endif

#endif /*__ADI_APOLLO_ADC_H__ */
/*! @} */
