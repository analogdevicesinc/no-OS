/*!
 * \brief     API private block select header file
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
#ifndef __ADI_APOLLO_PRIVATE_BLK_SEL_H__
#define __ADI_APOLLO_PRIVATE_BLK_SEL_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

/**
 * \brief  Sets the block sel mask for each feature set. Called at open, depends on 4t4r/8t8r.
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * 
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_private_blk_sel_mask_set(adi_apollo_device_t *device);

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_PRIVATE_BLK_SEL_H__ */
/*! @} */
