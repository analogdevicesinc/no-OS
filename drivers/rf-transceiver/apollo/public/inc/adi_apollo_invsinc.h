/*!
 * \brief     Inverse Sinc Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_INVSINC
 * @{
 */
#ifndef __ADI_APOLLO_INVSINC_H__
#define __ADI_APOLLO_INVSINC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_invsinc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Enable tx invsinc to compensate the DAC sinc roll off gain lost
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  invsincs         INVSINC channel selection. \ref adi_apollo_invsinc_select_e
 * \param[in]  enable           1: to enable invinc, 0: to disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_invsinc_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t invsincs, const uint8_t enable);

/**
 * \brief  Inspect Inverse Sinc param data from device
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   invsincs        A single INVSINC channel. \ref adi_apollo_invsinc_select_e
 * \param[out]  invsinc_inspect Pointer to invsinc inspect structure. \ref adi_apollo_invsinc_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_invsinc_inspect(adi_apollo_device_t *device, adi_apollo_blk_sel_t invsincs, adi_apollo_invsinc_inspect_t *invsinc_inspect);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_INVSINC_H__ */
/*! @} */
