/*!
 * \brief     API private device header file
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PRIVATE_DEVICE
 * @{
 */
#ifndef __ADI_APOLLO_PRIVATE_DEVICE_H__
#define __ADI_APOLLO_PRIVATE_DEVICE_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"
#include "adi_apollo_private_device_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

/**
 * \brief  Configure HSCI Linkup using Manual Mode
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_private_device_hsci_manual_linkup_configure(adi_apollo_device_t *device);

/**
 * \brief  Configure HSCI Linkup using Auto Mode
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_private_device_hsci_auto_linkup_configure(adi_apollo_device_t *device);

/**
 * \brief  Returns lockout state of a device feature (1 = feature is locked out by device, 0 = feature avail)
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal Target terminal \ref adi_apollo_terminal_e
 * \param[in]  feat     Feature to test for lockout
 * 
 *
 * \return Lockout state. 1: Feature is locked out by device variant. 0: Feature is available
 */
uint8_t adi_apollo_private_device_lockout_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_private_device_feat_lockout_e feat);

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_PRIVATE_DEVICE_H__ */
/*! @} */
