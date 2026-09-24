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
#ifndef __ADI_APOLLO_INVSINC_TYPES_H__
#define __ADI_APOLLO_INVSINC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_NUM_INVSINC              8

/*!
 * \brief Enumerates TX INVSINC Select
 */
typedef enum {
    ADI_APOLLO_INVSINC_NONE = 0x0,                              /*!< No INVSINC */
    ADI_APOLLO_INVSINC_A0 = 0x1,                                /*!< INVSINC0 of SIDE A */
    ADI_APOLLO_INVSINC_A1 = 0x2,                                /*!< INVSINC1 of SIDE A */
    ADI_APOLLO_INVSINC_A2 = 0x4,                                /*!< INVSINC2 of SIDE A */
    ADI_APOLLO_INVSINC_A3 = 0x8,                                /*!< INVSINC3 of SIDE A */
    ADI_APOLLO_INVSINC_B0 = 0x10,                               /*!< INVSINC0 of SIDE B */
    ADI_APOLLO_INVSINC_B1 = 0x20,                               /*!< INVSINC1 of SIDE B */
    ADI_APOLLO_INVSINC_B2 = 0x40,                               /*!< INVSINC2 of SIDE B */
    ADI_APOLLO_INVSINC_B3 = 0x80,                               /*!< INVSINC3 of SIDE B */
    ADI_APOLLO_INVSINC_ALL = 0xFF,                              /*!< ALL INVSINCs */
    ADI_APOLLO_INVSINC_ALL_4T4R = 0x33                          /*!< ALL INVSINCs (4T4R) */

} adi_apollo_invsinc_select_e;

/**
 * \brief Inverse Sinc inspect params
 */
typedef struct
{
    uint8_t invsinc_en;                                         /*!< Inverse sinc enable status. 1 = enabled, 0 = disabled */
    uint8_t invsinc_clk_en;                                     /*!< Inverse sinc clock enable status. 1 = enabled, 0 = disabled */
} adi_apollo_invsinc_inspect_t;


#endif /* __ADI_APOLLO_INVSINC_TYPES_H__ */
/*! @} */
