/*!
 * \brief     Apollo API startup sequence API header
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_STARTUP
 * @{
 */
#ifndef __ADI_APOLLO_STARTUP_H__
#define __ADI_APOLLO_STARTUP_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"
#include "adi_apollo_device_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Perform Apollo startup sequence
 * 
 * \note All clocks must be configured for the device profile configuration prior to 
 * calling this function.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  dev_profile      Device profile
 * \param[in]  startup_type     Startup sequence run mask. May run sections \ref adi_apollo_startup_seq_type_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_startup_execute(adi_apollo_device_t *device, adi_apollo_top_t *dev_profile, uint32_t startup_type);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_STARTUP_H__ */
/*! @} */
