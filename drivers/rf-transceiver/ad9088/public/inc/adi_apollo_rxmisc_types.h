/*!
 * \brief     Rx Misc functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMISC
 * @{
 */
#ifndef __ADI_APOLLO_RXMISC_TYPES_H__
#define __ADI_APOLLO_RXMISC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/

/*!
* \brief Enumerates RX DATAPATH CLK selection
*/
typedef enum {
    ADI_APOLLO_DATAPATH_CLK_DISABLE =         0x0,                   /*!< Disable the datapath clocks */
    ADI_APOLLO_DATAPATH_CLK_ENABLE_INPHASE =  0x1,                   /*!< Enables the I datapath clocks */
    ADI_APOLLO_DATAPATH_CLK_ENABLE_QPHASE =   0x2,                   /*!< Enables the Q datapath clocks */
} adi_apollo_datapath_clk_e;

/*!
* \brief RX MISC programming data
*/
typedef struct {
    uint8_t datapath_clk;                            /*!< \ref adi_apollo_datapath_clk_e */
    uint8_t low_samp_en;                             /*!< 1: the lower sample case is enabled, 0: Disabled */
} adi_apollo_rx_misc_pgm_t;

#endif /* __ADI_APOLLO_RXMISC_TYPES_H__ */
/*! @} */
