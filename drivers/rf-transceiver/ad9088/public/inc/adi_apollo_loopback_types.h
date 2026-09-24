/*!
 * \brief     LB BLOCK definition headers
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_LOOPBACK
 * @{
 */
#ifndef __ADI_APOLLO_LOOPBACK_TYPES_H__
#define __ADI_APOLLO_LOOPBACK_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
/*!
* \brief EnumeratesLB1
*/
typedef enum {
    ADI_APOLLO_LB1_BLEND_DISABLE    = 0x0,                   /*!< Disable blending*/
    ADI_APOLLO_LB1_BLEND_DIV_2      = 0x1,                   /*!< Enable blend with loopback1 data scaled by 2 */
    ADI_APOLLO_LB1_BLEND_DIV_4      = 0x3,                   /*!< Enable blend with loopback1 data scaled by 4 */
} adi_apollo_loopback_lb1_blend_mode_e;

#endif /* __ADI_APOLLO_LOOPBACK_TYPES_H__ */
/*! @} */
