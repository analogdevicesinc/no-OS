/*!
 * \brief     Rx Crossbar Mux functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMUX
 * @{
 */
#ifndef __ADI_APOLLO_RXMUX_TYPES_H__
#define __ADI_APOLLO_RXMUX_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_ADC_MUX_NUM 8
#define ADI_APOLLO_ADC_MUX_PER_SIDE 4
#define ADI_APOLLO_ADC_PER_MUX 4


/*!
* \brief Enumerates Rx Sample Xbar select
*/
typedef enum {
    ADI_APOLLO_RXMUX_SAMPLE_FINE0_I =         0u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE0_Q =         1u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE1_I =         2u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE1_Q =         3u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE2_I =         4u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE2_Q =         5u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE3_I =         6u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE3_Q =         7u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE4_I =         8u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE4_Q =         9u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE5_I =         10u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE5_Q =         11u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE6_I =         12u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE6_Q =         13u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE7_I =         14u,
    ADI_APOLLO_RXMUX_SAMPLE_FINE7_Q =         15u,
} adi_apollo_rxmux_sample_sel_e;

#endif /* __ADI_APOLLO_RXMUX_TYPES_H__ */
/*! @} */