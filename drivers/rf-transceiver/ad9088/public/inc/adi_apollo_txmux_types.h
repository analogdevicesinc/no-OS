/*!
 * \brief     TX MUX Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMUX
 * @{
 */
#ifndef __ADI_APOLLO_TXMUX_TYPES_H__
#define __ADI_APOLLO_TXMUX_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_TX_SUMMER_NUM            8
#define ADI_APOLLO_TX_SUMMER_PER_SIDE_NUM   4
#define ADI_APOLLO_NUM_DAC                  8     /* Num of DACs */
#define ADI_APOLLO_NUM_DAC_PER_SIDE         4     /* Num of DACs per side */
#define ADI_APOLLO_NUM_INVSINC              8

/**
 * \brief Enumerates Tx summer select
 */
typedef enum {
    ADI_APOLLO_TX_SUMMER_NONE = 0x00,                       /*!< No Tx summer */
    ADI_APOLLO_TX_SUMMER_A0 = 0x01,                         /*!< Summer 0 of SIDE A */
    ADI_APOLLO_TX_SUMMER_A1 = 0x02,                         /*!< Summer 1 of SIDE A */
    ADI_APOLLO_TX_SUMMER_A2 = 0x04,                         /*!< Summer 2 of SIDE A */
    ADI_APOLLO_TX_SUMMER_A3 = 0x08,                         /*!< Summer 3 of SIDE A */
    ADI_APOLLO_TX_SUMMER_B0 = 0x10,                         /*!< Summer 0 of SIDE B */
    ADI_APOLLO_TX_SUMMER_B1 = 0x20,                         /*!< Summer 1 of SIDE B */
    ADI_APOLLO_TX_SUMMER_B2 = 0x40,                         /*!< Summer 2 of SIDE B */
    ADI_APOLLO_TX_SUMMER_B3 = 0x80,                         /*!< Summer 3 of SIDE B */
    ADI_APOLLO_TX_SUMMER_ALL = 0xFF,                        /*!< All Tx Summers */
    ADI_APOLLO_TX_SUMMER_ALL_4T4R = 0x33                    /*!< All Tx Summers (4T4R) */
} adi_apollo_summer_select_e;

/**
 * \brief Enumerates the MODSW configuration options
 */
typedef enum {
    ADI_APOLLO_MODSW_0 = 0x00,              /*!< DAC0 = CDUC_A0 I, DAC1 = CDUC_A1 I */
    ADI_APOLLO_MODSW_1 = 0x01,              /*!< DAC0 = CDUC_A0 I + CDUC_A1 I, DAC1 = CDUC_A0 Q + CDUC_A1 Q */
    ADI_APOLLO_MODSW_2 = 0x02,              /*!< DAC0 = CDUC_A0 I, DAC1 = CDUC_A0 Q */
    ADI_APOLLO_MODSW_3 = 0x03               /*!< DAC0 = CDUC_A1 I, DAC1 = CDUC_A1 Q */
} adi_apollo_modsw_select_e;

#endif /* __ADI_APOLLO_TXMUX_TYPES_H__ */
/*! @} */
