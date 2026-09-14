/*!
 * \brief     Apollo DAC types header
 *
 * \copyright copyright(c) 2022 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DAC
 * @{
 */
#ifndef __ADI_APOLLO_DAC_TYPES_H__
#define __ADI_APOLLO_DAC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*!
* \brief DAC clock trim attributes
*/
typedef enum {
    ADI_APOLLO_DAC_CLK_TRIM_1   = 1,
    ADI_APOLLO_DAC_CLK_TRIM_2   = 2,
    ADI_APOLLO_DAC_CLK_TRIM_3   = 3
} adi_apollo_dac_clk_trim_e;

#endif /* __ADI_APOLLO_DAC_TYPES_H__ */
/*! @} */
