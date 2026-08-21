/*!
 * \brief     Multi Chip Sync Calibration API definition header
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MCS_CAL
 * @{
 */
#ifndef __ADI_APOLLO_MCS_CAL_H__
#define __ADI_APOLLO_MCS_CAL_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_mcs_cal_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief   Load configuration data required for MCS Calibration.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[in]   cal_config      Pointer to MCS Cal Configuration Struct. \ref adi_apollo_mcs_cal_config_t.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully.
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_config_set(adi_apollo_device_t *device, adi_apollo_mcs_cal_config_t *cal_config);

/**
 * \brief   Execute an Initial MCS Calibration.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully.
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_init_run(adi_apollo_device_t *device);

/**
 * \brief   Get status of a MCS Initial Calibration run.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[out]  cal_status      Pointer to struct containing MCS init cal error and state info. \ref adi_apollo_mcs_cal_init_status_t.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully.
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_init_status_get(adi_apollo_device_t *device, adi_apollo_mcs_cal_init_status_t *cal_status);

/**
 * \brief   Set select parameters related to MCS Calibration.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[in]   mcs_parameter   Select MCS configuration parameter. \ref adi_apollo_mcs_parameter_e.
 * \param[in]   data            MCS Configuration Data corresponding to selected parameter.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_parameter_set(adi_apollo_device_t *device, adi_apollo_mcs_parameter_e mcs_parameter, uint64_t data);

/**
 * \brief   Set decimation for MCS Tracking Calibration to do TDC measurement.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[in]   decimation      TDC decimation value.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_tracking_decimation_set(adi_apollo_device_t *device, uint16_t decimation);

/**
 * \brief   Enable/Disable MCS Tracking Calibration.
 * \note    Tracking decimation needs to be set before calling this API.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[in]   enable          Enable(1) / Disable(0) MCS Tracking Calibration.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully.
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_tracking_enable(adi_apollo_device_t *device, uint8_t enable);

/**
 * \brief   Initialize MCS Tracking Calibration. This initializes mcs tracking fw with user-defined values in \ref adi_apollo_drv_adf4382_config_t.
 * \note    User needs to call this API if 'track_initialize' was not already set in \ref adi_apollo_drv_adf4382_config_t.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_tracking_initialize_set(adi_apollo_device_t *device);

/**
 * \brief   Execute MCS Foreground Tracking Calibration for fast SysRef alignment.
 * \note    MCS Foreground Tracking routine can send multiple phase adjust GPIO strobes, if needed, to ADF4382.
 *          This allows for faster SysRef clock calibration. It is recommended to run this AFTER \ref adi_apollo_mcs_cal_init_run().
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_fg_tracking_run(adi_apollo_device_t *device);

/**
 * \brief   Execute MCS Background Tracking Calibration for maintaining SysRef alignment.
 * \note    MCS Background Tracking routine runs periodically, in background, 
 *          measuring SysRef time difference using Apollo TDC and if required sends phase correction strobe to ADF4382.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_bg_tracking_run(adi_apollo_device_t *device);

/**
 * \brief   Halts(freezes) MCS Background Tracking Calibration. TDC measurement and phase correction routine will be halted.
 * \note    To be used before getting tracking cal status or if phase correction is not needed.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_bg_tracking_freeze(adi_apollo_device_t *device);

/**
 * \brief   Restarts(unfreezes) MCS Background Tracking Calibration. TDC measurement and phase correction routine will start again.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_bg_tracking_unfreeze(adi_apollo_device_t *device);

/**
 * \brief   Stops(aborts) MCS Background Tracking Calibration. TDC measurement and phase correction routine will stop.
 * \note    This will set all MCS configuration settings to 0 and disable TDC measurements. 
 *          To restart, user would have to re-configure MCS cal config and re-initialize it.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_bg_tracking_abort(adi_apollo_device_t *device);

/**
 * \brief   Force a MCS Foreground Tracking Calibration for fast SysRef alignment.
 * \note    This is only used if user doesn't want conventional MCS FG Tracking cal and want to send phase correction only when needed.
 *          It is recommended to run this AFTER \ref adi_apollo_mcs_cal_init_run().
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_force_fg_tracking_run(adi_apollo_device_t *device);

/**
 * \brief   Force a ONE TIME MCS Background Tracking Calibration for maintaining SysRef alignment.
 * \note    This is only used if user doesn't want conventional PERIODIC MCS BG Tracking cal and want to send phase correction only when needed.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_force_bg_tracking_run(adi_apollo_device_t *device);

/**
 * \brief   Performs a coarse bleed current adjustment on ADF4382.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_coarse_jump_set(adi_apollo_device_t *device);

/**
 * \brief   Get status of MCS Tracking Calibration.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure.
 * \param[out]  cal_status      Pointer to struct containing MCS tracking cal error and state info. \ref adi_apollo_mcs_cal_status_t.
 *
 * \return  API_CMS_ERROR_OK    API Completed Successfully.
 * \return  <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_mcs_cal_tracking_status_get(adi_apollo_device_t *device, adi_apollo_mcs_cal_status_t *cal_status);


#ifndef CLIENT_IGNORE


#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_MCS_CAL_H__ */
/*! @} */
