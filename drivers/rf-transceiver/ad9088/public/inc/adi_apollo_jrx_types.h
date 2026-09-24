/*!
 * \brief     JRX BLOCK definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_JRX
 * @{
 */
#ifndef __ADI_APOLLO_JRX_TYPES_H__
#define __ADI_APOLLO_JRX_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/

#define ADI_APOLLO_JRX_LANES_MAX            12
#define ADI_APOLLO_JRX_J204C_IRQ_NUM        10
#define ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT 2

/**
 * \brief Jrx link status mask from
 */
typedef enum {
    ADI_APOLLO_JRX_LINK_STAT_NOT_IN_TABLE = 0x01,
    ADI_APOLLO_JRX_LINK_STAT_SCALE_OVERFLOW = 0x02,
    ADI_APOLLO_JRX_LINK_STAT_USR_DAT_RDY = 0x20,
    ADI_APOLLO_JRX_LINK_STAT_SYSREF_RECV = 0x40,
    ADI_APOLLO_JRX_LINK_STAT_CFG_INVALID = 0x80
} adi_apollo_jrx_link_stat_e;

/**
 * \brief JRx 204C IRQs
 */
typedef enum {
    ADI_APOLLO_JRX_J204C_IRQ_CRC             = 0x01,
    ADI_APOLLO_JRX_J204C_IRQ_SH              = 0x02,
    ADI_APOLLO_JRX_J204C_IRQ_MB              = 0x04,
    ADI_APOLLO_JRX_J204C_IRQ_EMB             = 0x08,
    ADI_APOLLO_JRX_J204C_IRQ_DATA_RDY_LOST   = 0x10,
    ADI_APOLLO_JRX_J204C_IRQ_RM_FIFO_EMPTY   = 0x20,
    ADI_APOLLO_JRX_J204C_IRQ_RM_FIFO_FULL    = 0x40,
    ADI_APOLLO_JRX_J204C_IRQ_LANE_FIFO_EMPTY = 0x80,
    ADI_APOLLO_JRX_J204C_IRQ_LANE_FIFO_FULL  = 0x100,
    ADI_APOLLO_JRX_J204C_IRQ_INVALID_SAMPLE  = 0x200,
    ADI_APOLLO_JRX_J204C_IRQ_ALL             = 0x3FF,
 } adi_apollo_jrx_j204c_irq_e;

/**
 * \brief JESD Rx inspect params
 */
typedef struct adi_apollo_jesd_rx_inspect
{
    uint8_t link_en;                                           /*!< Link enable status. 1 = enabled, 0 = disabled */
    adi_apollo_jesd_lanes_per_link_e          l_minus1;        /*!< Number of lanes minus 1 */
    adi_apollo_jesd_octs_per_frm_e            f_minus1;        /*!< Number of bytes (octets) per frame minus 1 */
    adi_apollo_jesd_convs_per_link_e          m_minus1;        /*!< Number of converters minus 1 */
    adi_apollo_jesd_samp_conv_frm_e           s_minus1;        /*!< Number of samples per converter per frame minus 1 */
    adi_apollo_jesd_conv_resol_e              n_minus1;        /*!< Link converter resolution minus 1 */
    adi_apollo_jesd_bits_per_samp_e           np_minus1;       /*!< Converter sample resolution in bits minus 1 */
    adi_apollo_jesd_samp_per_conv_e           ns_minus1;       /*!< Number of samples per converter in Conv_sample */
    uint8_t                                   k_minus1;        /*!< Number of frames in a multiframe - 1 (0 - 255) */
    adi_apollo_jesd_cont_bits_per_samp_e      cs;              /*!< Number of control bits per sample */
    uint8_t                                   jesd_mode;       /*!< Jesd mode s*/
    adi_apollo_jesd_version_e                 ver;             /*!< 0 = 204B, 1 = 204C */
    adi_apollo_jesd_subclass_e                subclass;        /*!< Subclass setting */
} adi_apollo_jesd_rx_inspect_t;

#endif /* __ADI_APOLLO_JRX_TYPES_H__ */
/*! @} */
