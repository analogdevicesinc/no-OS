/*!
 * \brief     Tx Misc functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMISC
 * @{
 */
#ifndef __ADI_APOLLO_TXMISC_TYPES_H__
#define __ADI_APOLLO_TXMISC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_TX_MISC_REGMAP_NUM   2

/*!
* \brief tx path inspect param
*/
typedef struct {
    adi_apollo_txpath_misc_t    dp;                    /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
} adi_apollo_txmisc_inspect_t;


#endif /* __ADI_APOLLO_TXMISC_TYPES_H__ */
/*! @} */
