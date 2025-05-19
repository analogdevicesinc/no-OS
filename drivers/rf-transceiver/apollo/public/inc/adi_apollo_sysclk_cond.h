/*!
 * \brief:   Apollo system clock calibration functions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 *
 */

/*!
 * \addtogroup ADI_APOLLO_SYSCLK_COND
 * @{
 */
#ifndef __ADI_APOLLO_SYSCLK_COND_H__
#define __ADI_APOLLO_SYSCLK_COND_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_cfg_types.h"
#include "adi_apollo_mailbox.h"

/*============= E X P O R T S ==============*/

#define ADI_APOLLO_SYSCLK_COND_CENTER_MAX_TO 10

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Execute a foreground ADC clock conditioning calibration
 *
 * \param[in] device        	Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sysclk_cond_cal(adi_apollo_device_t *device);

/**
 * \brief Execute a background clock conditioning calibration
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sysclk_cond_bg_cal_start(adi_apollo_device_t* device);

/**
 * \brief Stop background clock conditioning calibration
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sysclk_cond_bg_cal_stop(adi_apollo_device_t* device);

/**
 * \brief Resume background clock conditioning calibration
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_sysclk_cond_bg_cal_resume(adi_apollo_device_t* device);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_SYSCLK_COND_H__ */
/*! @} */