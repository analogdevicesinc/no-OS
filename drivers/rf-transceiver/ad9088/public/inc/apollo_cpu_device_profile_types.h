/**
 * \file apollo_cpu_device_profile_types.h
 * \brief Device profile for Apollo API
 * \copyright Analog Devices Inc. 2021. All rights reserved.
 * Released under the APOLLO API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef __APOLLO_CPU_DEVICE_PROFILE_TYPES_H__
#define __APOLLO_CPU_DEVICE_PROFILE_TYPES_H__

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdbool.h>
#include <stdint.h>
#endif
#include "adi_apollo_platform_pack.h"

#define ADI_APOLLO_PROFILE_VERSION_MAJOR         10            /*!< Major */
#define ADI_APOLLO_PROFILE_VERSION_MINOR         1             /*!< Minor */
#define ADI_APOLLO_PROFILE_VERSION_PATCH         2             /*!< Patch */

#define ADI_APOLLO_DACS_PER_SIDE                 4             /*!< #DACs per side. */
#define ADI_APOLLO_ADCS_PER_SIDE                 4             /*!< #Max ADCs per side. (8T8R) */
#define ADI_APOLLO_4T4R_ADCS_PER_SIDE            2             /*!< #ADCs per side. (4T4R) */
#define ADI_APOLLO_4T4R_ADCS_TOTAL               4             /*!< #ADCs total. (4T4R) */
#define ADI_APOLLO_NUM_NCOS_PER_DRC              2             /*!< #NCOs per DRC.  */
#define ADI_APOLLO_PFILT_COEFFS                 32             /*!< #coefficients for a programmable filter. */
#define ADI_APOLLO_CFIR_NUM_TAPS                16
#define ADI_APOLLO_CNCO_PROFILE_NUM             16             /*!< # of tuning profiles per CNCO */
#define ADI_APOLLO_FNCO_PROFILE_NUM             32             /*!< # of tuning profiles per FNCO */
#define ADI_APOLLO_DAC_DIG_SLICE_PER_SIDE        2
#define ADI_APOLLO_NUM_ADF4382_GPIOS             2             /*!< # of gpios to control ext ADF4382s */
#define ADI_APOLLO_MAX_NUM_ADF4382               2             /*!< Max # of ADF4382 that can be connected */
#define ADI_APOLLO_NUM_GPIOS                    51             /*!< # of GPIOs for Apollo */

#define ADI_APOLLO_PROFILE_TX_CHAN_MASK         (0x000000FFU)
#define ADI_APOLLO_PROFILE_TX_CHAN_POS          (0U)

#define ADI_APOLLO_PROFILE_RX_CHAN_MASK         (0x000000FFU)
#define ADI_APOLLO_PROFILE_RX_CHAN_POS          (0U)
#define ADI_APOLLO_NUM_TXRX_CHAN                8

#ifndef ADI_APOLLO_PACK_ENUM
#ifdef __GNUC__
#define ADI_APOLLO_PACK_ENUM __attribute__((packed))
#else
#define ADI_APOLLO_PACK_ENUM
#endif
#endif


/**
 * \brief Apollo Side Enumeration
 */
typedef enum adi_apollo_sides {
    ADI_APOLLO_SIDE_IDX_A   = 0u,   /*!< Apollo A Side */
    ADI_APOLLO_SIDE_IDX_B   = 1u,   /*!< Apollo B Side */
    ADI_APOLLO_NUM_SIDES    = 2u    /*!< Number of Apollo Sides */
} ADI_APOLLO_PACK_ENUM adi_apollo_sides_e;


/**
 * \brief Apollo Per Side Link Enumeration
 */
typedef enum adi_apollo_jesd_links {
    ADI_APOLLO_JESD_LINK_0  = 0u,       /*!< Apollo Link 0 */
    ADI_APOLLO_JESD_LINK_1  = 1u,       /*!< Apollo Link 1 */
    ADI_APOLLO_JESD_LINKS   = 2u        /*!< Number of Apollo Links Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_links_e;


/**
 * \brief RX Channel Enumeration
 */
typedef enum adi_apollo_rx_channel_num {
    ADI_APOLLO_RX_CH_A0     = 0u,           /*!< RX Channel A0 */
    ADI_APOLLO_RX_CH_A1     = 1u,           /*!< RX Channel A1 */
    ADI_APOLLO_RX_CH_A2     = 2u,           /*!< RX Channel A2 */
    ADI_APOLLO_RX_CH_A3     = 3u,           /*!< RX Channel A3 */
    ADI_APOLLO_RX_CH_B0     = 4u,           /*!< RX Channel B0 */
    ADI_APOLLO_RX_CH_B1     = 5u,           /*!< RX Channel B1 */
    ADI_APOLLO_RX_CH_B2     = 6u,           /*!< RX Channel B2 */
    ADI_APOLLO_RX_CH_B3     = 7u,           /*!< RX Channel B3 */
    ADI_APOLLO_RX_CH_MAX    = 7u,           /*!< RX Channel Max Count */
    ADI_APOLLO_RX_CH_LEN    = 8u            /*!< RX Channel Length for Arrays */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_channel_num_e;


/**
 * \brief TX Channel Enumeration
 */
typedef enum adi_apollo_tx_channel_num {
    ADI_APOLLO_TX_CH_A0     = 0u,           /*!< TX Channel A0 */
    ADI_APOLLO_TX_CH_A1     = 1u,           /*!< TX Channel A1 */
    ADI_APOLLO_TX_CH_A2     = 2u,           /*!< TX Channel A2 */
    ADI_APOLLO_TX_CH_A3     = 3u,           /*!< TX Channel A3 */
    ADI_APOLLO_TX_CH_B0     = 4u,           /*!< TX Channel B0 */
    ADI_APOLLO_TX_CH_B1     = 5u,           /*!< TX Channel B1 */
    ADI_APOLLO_TX_CH_B2     = 6u,           /*!< TX Channel B2 */
    ADI_APOLLO_TX_CH_B3     = 7u,           /*!< TX Channel B3 */
    ADI_APOLLO_TX_CH_MAX    = 7u,           /*!< TX Channel Max Count */
    ADI_APOLLO_TX_CH_LEN    = 8u            /*!< TX Channel Length for Arrays */
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_channel_num_e;


/**
 * \brief Coarse Digital Down Converter Index Enumeration
 */
typedef enum adi_apollo_cddc_idx {
    ADI_APOLLO_CDDC_IDX_0       = 0u,   /*!< CDDC Index 0 */
    ADI_APOLLO_CDDC_IDX_1       = 1u,   /*!< CDDC Index 1 */
    ADI_APOLLO_CDDCS_PER_SIDE   = 2u    /*!< CDDC Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_cddc_idx_e;


/**
 * \brief Coarse Digital Down Converter Path Index Enumeration
 */
typedef enum adi_apollo_cddc_path_idx {
    ADI_APOLLO_CDDC_PATH_IDX_0      = 0u,   /*!< CDDC Path Index 0 */
    ADI_APOLLO_CDDC_PATH_IDX_1      = 1u,   /*!< CDDC Path Index 1 */
    ADI_APOLLO_CDDC_PATH_IDX_2      = 2u,   /*!< CDDC Path Index 2 */
    ADI_APOLLO_CDDC_PATH_IDX_3      = 3u,   /*!< CDDC Path Index 3 */
    ADI_APOLLO_CDDC_PATHS_PER_SIDE  = 4u    /*!< CDDC Path Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_cddc_path_idx_e;


/**
 * \brief Fine Digital Down Converter Index Enumeration
 */
typedef enum adi_apollo_fddc_idx {
    ADI_APOLLO_FDDC_IDX_0       = 0u,   /*!< FDDC Index 0 */
    ADI_APOLLO_FDDC_IDX_1       = 1u,   /*!< FDDC Index 1 */
    ADI_APOLLO_FDDC_IDX_2       = 2u,   /*!< FDDC Index 2 */
    ADI_APOLLO_FDDC_IDX_3       = 3u,   /*!< FDDC Index 3 */
    ADI_APOLLO_FDDCS_PER_SIDE   = 4u    /*!< FDDC Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_idx_e;


/**
 * \brief Fine Digital Down Converter Path Index Enumeration
 */
typedef enum adi_apollo_fddc_path_idx {
    ADI_APOLLO_FDDC_PATH_IDX_0      = 0u,   /*!< FDDC Path Index 0 */
    ADI_APOLLO_FDDC_PATH_IDX_1      = 1u,   /*!< FDDC Path Index 1 */
    ADI_APOLLO_FDDC_PATH_IDX_2      = 2u,   /*!< FDDC Path Index 2 */
    ADI_APOLLO_FDDC_PATH_IDX_3      = 3u,   /*!< FDDC Path Index 3 */
    ADI_APOLLO_FDDC_PATH_IDX_4      = 4u,   /*!< FDDC Path Index 4 */
    ADI_APOLLO_FDDC_PATH_IDX_5      = 5u,   /*!< FDDC Path Index 5 */
    ADI_APOLLO_FDDC_PATH_IDX_6      = 6u,   /*!< FDDC Path Index 6 */
    ADI_APOLLO_FDDC_PATH_IDX_7      = 7u,   /*!< FDDC Path Index 7 */
    ADI_APOLLO_FDDC_PATHS_PER_SIDE  = 8u    /*!< FDDC Path Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_path_idx_e;


/**
 * \brief Buffer Memory Index Enumeration
 */
typedef enum adi_apollo_bmem_idx {
    ADI_APOLLO_BMEM_IDX_0       = 0u,   /*!< BMEM Index 0 */
    ADI_APOLLO_BMEM_IDX_1       = 1u,   /*!< BMEM Index 1 */
    ADI_APOLLO_BMEM_IDX_2       = 2u,   /*!< BMEM Index 2 */
    ADI_APOLLO_BMEM_IDX_3       = 3u,   /*!< BMEM Index 3 */
    ADI_APOLLO_BMEM_IDX_4       = 4u,   /*!< BMEM Index 4 */
    ADI_APOLLO_BMEM_IDX_5       = 5u,   /*!< BMEM Index 5 */
    ADI_APOLLO_BMEMS_PER_SIDE   = 6u    /*!< BMEM Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_bmem_idx_e;


/**
 * \brief Coarse Digital Up Converter Index Enumeration
 */
typedef enum adi_apollo_cduc_idx {
    ADI_APOLLO_CDUC_IDX_0       = 0u,   /*!< CDUC Index 0 */
    ADI_APOLLO_CDUC_IDX_1       = 1u,   /*!< CDUC Index 1 */
    ADI_APOLLO_CDUCS_PER_SIDE   = 2u    /*!< CDUC Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_cduc_idx_e;


/**
 * \brief Coarse Digital Up Converter Path Index Enumeration
 */
typedef enum adi_apollo_cduc_path_idx {
    ADI_APOLLO_CDUC_PATH_IDX_0      = 0u,   /*!< CDUC Path Index 0 */
    ADI_APOLLO_CDUC_PATH_IDX_1      = 1u,   /*!< CDUC Path Index 1 */
    ADI_APOLLO_CDUC_PATH_IDX_2      = 2u,   /*!< CDUC Path Index 2 */
    ADI_APOLLO_CDUC_PATH_IDX_3      = 3u,   /*!< CDUC Path Index 3 */
    ADI_APOLLO_CDUC_PATHS_PER_SIDE  = 4u    /*!< CDUC Path Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_cduc_path_idx_e;


/**
 * \brief Fine Digital Up Converter Index Enumeration
 */
typedef enum adi_apollo_fduc_idx {
    ADI_APOLLO_FDUC_IDX_0       = 0u,   /*!< FDUC Index 0 */
    ADI_APOLLO_FDUC_IDX_1       = 1u,   /*!< FDUC Index 1 */
    ADI_APOLLO_FDUC_IDX_2       = 2u,   /*!< FDUC Index 2 */
    ADI_APOLLO_FDUC_IDX_3       = 3u,   /*!< FDUC Index 3 */
    ADI_APOLLO_FDUCS_PER_SIDE   = 4u    /*!< FDUC Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_idx_e;


/**
 * \brief Fine Digital Up Converter Path Index Enumeration
 */
typedef enum adi_apollo_fduc_path_idx {
    ADI_APOLLO_FDUC_PATH_IDX_0      = 0u,   /*!< FDUC Path Index 0 */
    ADI_APOLLO_FDUC_PATH_IDX_1      = 1u,   /*!< FDUC Path Index 1 */
    ADI_APOLLO_FDUC_PATH_IDX_2      = 2u,   /*!< FDUC Path Index 2 */
    ADI_APOLLO_FDUC_PATH_IDX_3      = 3u,   /*!< FDUC Path Index 3 */
    ADI_APOLLO_FDUC_PATH_IDX_4      = 4u,   /*!< FDUC Path Index 4 */
    ADI_APOLLO_FDUC_PATH_IDX_5      = 5u,   /*!< FDUC Path Index 5 */
    ADI_APOLLO_FDUC_PATH_IDX_6      = 6u,   /*!< FDUC Path Index 6 */
    ADI_APOLLO_FDUC_PATH_IDX_7      = 7u,   /*!< FDUC Path Index 7 */
    ADI_APOLLO_FDUC_PATHS_PER_SIDE  = 8u    /*!< FDUC Path Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_path_idx_e;


/**
 * \brief Programmable Filter Index Enumeration
 */
typedef enum adi_apollo_pfilt_idx {
    ADI_APOLLO_PFILT_IDX_0      = 0u,   /*!< PFILT Index 0 */
    ADI_APOLLO_PFILTS_PER_SIDE  = 1u    /*!< PFILT Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_idx_e;


/**
 * \brief Complex Filter Index Enumeration
 */
typedef enum adi_apollo_cfir_idx {
    ADI_APOLLO_CFIR_IDX_0       = 0u,   /*!< CFIR Index 0 */
    ADI_APOLLO_CFIR_IDX_1       = 1u,   /*!< CFIR Index 1 */
    ADI_APOLLO_CFIRS_PER_SIDE   = 2u    /*!< CFIR Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_idx_e;


/**
 * \brief Signal Monitor Index Enumeration
 */
typedef enum adi_apollo_smon_idx {
    ADI_APOLLO_SMON_IDX_0       = 0u,   /*!< SMON Index 0 */
    ADI_APOLLO_SMON_IDX_1       = 1u,   /*!< SMON Index 1 */
    ADI_APOLLO_SMON_IDX_2       = 2u,   /*!< SMON Index 2 */
    ADI_APOLLO_SMON_IDX_3       = 3u,   /*!< SMON Index 3 */
    ADI_APOLLO_SMONS_PER_SIDE   = 4u    /*!< SMON Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_smon_idx_e;


/**
 * \brief RX/TX Enable Index Enumeration
 */
typedef enum adi_apollo_rxtx_enable_idx {
    ADI_APOLLO_RXTX_ENABLE_IDX_0        = 0u,   /*!< RXTX En Index 0 */
    ADI_APOLLO_RXTX_ENABLE_IDX_1        = 1u,   /*!< RXTX En Index 1 */
    ADI_APOLLO_RXTX_ENABLE_IDX_2        = 2u,   /*!< RXTX En Index 2 */
    ADI_APOLLO_RXTX_ENABLE_IDX_3        = 3u,   /*!< RXTX En Index 3 */
    ADI_APOLLO_RXTX_ENABLES_PER_SIDE    = 4u    /*!< RXTX En Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_rxtx_enable_idx_e;


/**
 * \brief RX/TX Power Enable Index Enumeration
 */
typedef enum adi_apollo_rxtx_enable_pwr_idx {
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_0    = 0u,   /*!< RXTX Power En Index 0 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_1    = 1u,   /*!< RXTX Power En Index 1 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_2    = 2u,   /*!< RXTX Power En Index 2 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_3    = 3u,   /*!< RXTX Power En Index 3 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_4    = 4u,   /*!< RXTX Power En Index 4 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_5    = 5u,   /*!< RXTX Power En Index 5 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_6    = 6u,   /*!< RXTX Power En Index 6 */
    ADI_APOLLO_RXTX_POWER_CTRL_IDX_7    = 7u,   /*!< RXTX Power En Index 7 */
    ADI_APOLLO_RXTX_POWER_CTRL_PER_SIDE = 8u    /*!< RXTX Power En Indices Per Side */
} ADI_APOLLO_PACK_ENUM adi_apollo_rxtx_enable_pwr_idx_e;


#define ADI_APOLLO_JESD_MAX_LANES_PER_SIDE      (12u)
#define ADI_APOLLO_JESD_MAX_SAMPLE_XBAR_IDXS    (96u)
#define ADI_APOLLO_JESD_MAX_CONV_XBAR_IDXS      (16u)


/**
 * \brief SerDes Lane Enumeration. Used for both JRx and JTx PHY Lanes.
 */
typedef enum adi_apollo_serdes_lane_num {
    ADI_APOLLO_SERDES_LANE_0    = 0u,       /*!< SerDes Lane #0 Index */
    ADI_APOLLO_SERDES_LANE_1    = 1u,       /*!< SerDes Lane #1 Index */
    ADI_APOLLO_SERDES_LANE_2    = 2u,       /*!< SerDes Lane #2 Index */
    ADI_APOLLO_SERDES_LANE_3    = 3u,       /*!< SerDes Lane #3 Index */
    ADI_APOLLO_SERDES_LANE_4    = 4u,       /*!< SerDes Lane #4 Index */
    ADI_APOLLO_SERDES_LANE_5    = 5u,       /*!< SerDes Lane #5 Index */
    ADI_APOLLO_SERDES_LANE_6    = 6u,       /*!< SerDes Lane #6 Index */
    ADI_APOLLO_SERDES_LANE_7    = 7u,       /*!< SerDes Lane #7 Index */
    ADI_APOLLO_SERDES_LANE_8    = 8u,       /*!< SerDes Lane #8 Index */
    ADI_APOLLO_SERDES_LANE_9    = 9u,       /*!< SerDes Lane #9 Index */
    ADI_APOLLO_SERDES_LANE_10   = 10u,      /*!< SerDes Lane #10 Index */
    ADI_APOLLO_SERDES_LANE_11   = 11u,      /*!< SerDes Lane #11 Index */
    ADI_APOLLO_SERDES_LANE_12   = 12u,      /*!< SerDes Lane #12 Index */
    ADI_APOLLO_SERDES_LANE_13   = 13u,      /*!< SerDes Lane #13 Index */
    ADI_APOLLO_SERDES_LANE_14   = 14u,      /*!< SerDes Lane #14 Index */
    ADI_APOLLO_SERDES_LANE_15   = 15u,      /*!< SerDes Lane #15 Index */
    ADI_APOLLO_SERDES_LANE_16   = 16u,      /*!< SerDes Lane #16 Index */
    ADI_APOLLO_SERDES_LANE_17   = 17u,      /*!< SerDes Lane #17 Index */
    ADI_APOLLO_SERDES_LANE_18   = 18u,      /*!< SerDes Lane #18 Index */
    ADI_APOLLO_SERDES_LANE_19   = 19u,      /*!< SerDes Lane #19 Index */
    ADI_APOLLO_SERDES_LANE_20   = 20u,      /*!< SerDes Lane #20 Index */
    ADI_APOLLO_SERDES_LANE_21   = 21u,      /*!< SerDes Lane #21 Index */
    ADI_APOLLO_SERDES_LANE_22   = 22u,      /*!< SerDes Lane #22 Index */
    ADI_APOLLO_SERDES_LANE_23   = 23u,      /*!< SerDes Lane #23 Index */
    ADI_APOLLO_SERDES_LANE_LEN  = 24u       /*!< Total Number of SerDes Lanes supported */
} ADI_APOLLO_PACK_ENUM adi_apollo_serdes_lane_num_e;


/**
 * \brief Serializer Framer Enumeration
 */
typedef enum adi_apollo_jesd_framer {
    ADI_APOLLO_JESD_FRAMER_0    = 0u,   /*!< Framer 0 */
    ADI_APOLLO_JESD_FRAMER_1    = 1u,   /*!< Framer 1 */
    ADI_APOLLO_JESD_FRAMER_NUM  = 2u    /*!< Total Number of Framers */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_framer_e;

/**
 * \brief Deserializer Deframer Enumeration
 */
typedef enum adi_apollo_jesd_deframer {
    ADI_APOLLO_JESD_DEFRAMER_0      = 0u,   /*!< Deframer 0 */
    ADI_APOLLO_JESD_DEFRAMER_1      = 1u,   /*!< Deframer 1 */
    ADI_APOLLO_JESD_DEFRAMER_NUM    = 2u    /*!< Total Number of Deframers */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_deframer_e;


/**
 * \brief SerDes Pack Index Enumeration. Used for both JRx and JTx.
 */
typedef enum adi_apollo_ser_des_pack {
    ADI_APOLLO_SERDES_PACK_IDX_0 = 0u,  /*!< Pack 0 */
    ADI_APOLLO_SERDES_PACK_IDX_1 = 1u,  /*!< Pack 1 */
    ADI_APOLLO_SERDES_NUM_PACKS  = 2u   /*!< Total Number of SerDes Packs */
} ADI_APOLLO_PACK_ENUM adi_apollo_ser_des_pack_e;


/**
 * \brief Serializer Quad-Byte Framer Enumeration
 */
typedef enum adi_apollo_jesd_qbf_txfe {
    ADI_APOLLO_JESD_QBF_TXFE_0      = 0u,   /*!< QBF 0 */
    ADI_APOLLO_JESD_QBF_TXFE_1      = 1u,   /*!< QBF 1 */
    ADI_APOLLO_JESD_QBF_TXFE_NUM    = 2u    /*!< Total Number of QBFs */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_qbf_txfe_e;


/**
 * \brief Serializer Framer Virtual Converter Enumeration
 */
typedef enum adi_apollo_jesd_frm_conv_xbar_select {
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_0    = 0u,   /*!< Virtual Converter 0 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_1    = 1u,   /*!< Virtual Converter 1 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_2    = 2u,   /*!< Virtual Converter 2 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_3    = 3u,   /*!< Virtual Converter 3 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_4    = 4u,   /*!< Virtual Converter 4 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_5    = 5u,   /*!< Virtual Converter 5 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_6    = 6u,   /*!< Virtual Converter 6 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_7    = 7u,   /*!< Virtual Converter 7 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_8    = 8u,   /*!< Virtual Converter 8 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_9    = 9u,   /*!< Virtual Converter 9 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_10   = 10u,  /*!< Virtual Converter 10 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_11   = 11u,  /*!< Virtual Converter 11 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_12   = 12u,  /*!< Virtual Converter 12 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_13   = 13u,  /*!< Virtual Converter 13 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_14   = 14u,  /*!< Virtual Converter 14 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_15   = 15u,  /*!< Virtual Converter 15 */
    ADI_APOLLO_JESD_FRM_CONVXBAR_VCONV_NUM  = 16u   /*!< Total Number of Virtual Converters */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_frm_conv_xbar_select_e;


/**
 * \brief Deserializer Deframer SYNC Pad Power Enumeration
 */
typedef enum adi_apollo_jesd_dfrm_sync_pad_req {
    ADI_APOLLO_JESD_DFRM_PWR_ON_SYNC_PAD1       = 0u,   /*!< Power On Sync Pad 1 */
    ADI_APOLLO_JESD_DFRM_PWR_ON_SYNC_PAD2       = 1u,   /*!< Power On Sync Pad 2 */
    ADI_APOLLO_JESD_DFRM_PWR_ON_ALL_SYNC_PADS   = 2u,   /*!< Power On All Sync Pads */
    ADI_APOLLO_JESD_DFRM_PWR_OFF_ALL_SYNC_PADS  = 3u    /*!< Power Off All Sync Pads */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_dfrm_sync_pad_req_e;


/**
 * \brief JESD Converter Clock Mode Enumeration
 */
typedef enum adi_apollo_jesd_conv_clock_mode {
    ADI_APOLLO_JESD_NO_RESAMPLE_FIR     = 0u,   /*!< No Resampling */
    ADI_APOLLO_JESD_2_3_RESAMPLE_FIR    = 1u,   /*!< 2/3 Resampling */
    ADI_APOLLO_JESD_3_4_RESAMPLE_FIR    = 2u    /*!< 3/4 Resampling */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_conv_clock_mode_e;


/**
 * \brief CPU ID Enumeration
 */
typedef enum adi_apollo_cpu_id {
    ADI_APOLLO_CPU_ID_0 = 0u,       /*!< CPU 0 */
    ADI_APOLLO_CPU_ID_1 = 1u        /*!< CPU 1 */
} ADI_APOLLO_PACK_ENUM adi_apollo_cpu_id_e;

#define ADI_APOLLO_RX_CHAN_LEN_CPU     (ADI_APOLLO_RX_CH_LEN  / 1u)
#define ADI_APOLLO_TX_CHAN_LEN_CPU     (ADI_APOLLO_TX_CH_LEN  / 1u)
#define ADI_APOLLO_SERDES_LANE_LEN_CPU (ADI_APOLLO_SERDES_LANE_LEN / 1u)
#define ADI_APOLLO_SERDES_PACK_LEN_CPU (ADI_APOLLO_SERDES_NUM_PACKS / 1u)


/**
 * \brief Profile Version Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_profile_version {
    uint8_t     major;                      /*!< Major component in profile version */
    uint16_t    minor;                      /*!< Minor component in profile version */
    uint8_t     patch;                      /*!< Patch component in profile version */
} adi_apollo_profile_version_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief DAC Configuration Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dac_cfg {
    uint64_t    dac_sampling_rate_Hz;                       /*!< DAC sampling rate in Hz. */
    bool        shuffle_en[ADI_APOLLO_DACS_PER_SIDE];       /*!< True to enable shuffle. */
    bool        scrambler_en[ADI_APOLLO_DACS_PER_SIDE];     /*!< True to enable scrambler */
    bool        data_invert_en[ADI_APOLLO_DACS_PER_SIDE];   /*!< True to enable data inversion */
} adi_apollo_dac_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief ADC FG Calibration Type Enumerations
 */
typedef enum adi_apollo_adc_init_cal_type {
    ADI_APOLLO_ADC_INIT_CAL_TYPE_EXT_INPUT = 0U,    /*!< ADC FG cal with external signal */
    ADI_APOLLO_ADC_INIT_CAL_TYPE_NO_INPUT = 1U,     /*!< ADC FG cal without external signal */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_init_cal_type_e;


/**
 * \brief ADC Slice Clocking Mode Enumerations
 */
typedef enum adi_apollo_adc_slc_mode {
    ADI_APOLLO_ADC_SLICE_PPRAND3P1  = 6u,   /*!< Ping-Pong Random 3+1 Mode */
    ADI_APOLLO_ADC_SLICE_PPSEQ4     = 7u,   /*!< Ping-Pong Sequential 4 Mode */
    ADI_APOLLO_ADC_SLICE_RAND3P1    = 10u,  /*!< Random 3+1 Mode */
    ADI_APOLLO_ADC_SLICE_SEQ4       = 11u   /*!< Sequential 4 Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_slc_mode_e;


/**
 * \brief ADC Track & Hold Mode Enumerations
 */
typedef enum adi_apollo_adc_trk_mode {
    ADI_APOLLO_ADC_TRACK_SEQ1   = 0u,   /*!< Single Track & Hold Mode */
    ADI_APOLLO_ADC_TRACK_SEQ2   = 1u    /*!< Sequential 2 Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_trk_mode_e;


/**
 * \brief ADC Full Scale Adjust Enumerations
 */
typedef enum adi_apollo_adc_full_scale_adj {
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M6P0DB    = 160U, /*!< -6.0 dB (0xA0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M5P0DB    = 176U, /*!< -5.0 dB (0xB0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M4P0DB    = 192U, /*!< -4.0 dB (0xC0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M3P0DB    = 208U, /*!< -3.0 dB (0xD0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M2P0DB    = 224U, /*!< -2.0 dB (0xE0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_M1P0DB    = 240U, /*!< -1.0 dB (0xF0) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_DEFAULT   = 0U,   /*!< 0.0 dB (0x00) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_P1P0DB    = 16U,  /*!< +1.0 dB (0x10) */
    ADI_APOLLO_ADC_FULL_SCALE_ADJ_P2P0DB    = 32U,  /*!< +2.0 dB (0x20) */
} ADI_APOLLO_PACK_ENUM  adi_apollo_adc_full_scale_adj_e;


/**
 * \brief ADC Input Coupling Enumerations
 */
typedef enum adi_apollo_adc_input_coupling {
    ADI_APOLLO_ADC_INPUT_COUPLING_AC    = 0u,   /*!< AC Coupled Input */
    ADI_APOLLO_ADC_INPUT_COUPLING_DC    = 1u    /*!< DC Coupled Input */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_coupling_e;


/**
 * \brief ADC Input Signal Type Enumerations
 */
typedef enum adi_apollo_adc_input_signal {
    ADI_APOLLO_ADC_INPUT_SIGNAL_DIFFERENTIAL    = 0u,   /*!< Differential Signal */
    ADI_APOLLO_ADC_INPUT_SIGNAL_SINGLE_ENDED    = 1u    /*!< Single-Ended Signal */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_signal_e;


/**
 * \brief ADC Input Polarity Enumerations
 */
typedef enum adi_apollo_adc_input_polarity {
    ADI_APOLLO_ADC_INPUT_POLARITY_NORMAL    = 0u,   /*!< Normal */
    ADI_APOLLO_ADC_INPUT_POLARITY_INVERT    = 1u    /*!< Inverted */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_polarity_e;


/**
 * \brief ADC Slice Mode Switch Configuration Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_adc_slice_mode_switch_cfg {
    uint8_t        switch_type;                         /*!< ADC Slice Mode Switch Type */
    uint8_t        trig_gpio_num;                       /*!< GPIO number used for triggering ADC slice mode switch */
} adi_apollo_adc_slice_mode_switch_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief ADC Configuration Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_adc_cfg {
    uint64_t                                adc_sampling_rate_Hz;                           /*!< ADC sampling rate in Hz. */
    adi_apollo_adc_init_cal_type_e          adc_init_cal_type;                              /*!< ADC initial calibration type. */
    uint8_t                                 adc_init_cal_index;                             /*!< ADC initial calibration index. */
    adi_apollo_adc_slc_mode_e               adc_slice_mode[ADI_APOLLO_ADCS_PER_SIDE];       /*!< Slice mode */
    adi_apollo_adc_trk_mode_e               adc_track_mode[ADI_APOLLO_ADCS_PER_SIDE];       /*!< Track mode */
    adi_apollo_adc_full_scale_adj_e         full_scale_adj[ADI_APOLLO_ADCS_PER_SIDE];       /*!< Full Scale Adjust */
    adi_apollo_adc_input_coupling_e         inp_coupling_type[ADI_APOLLO_ADCS_PER_SIDE];    /*!< Input Coupling Type */
    adi_apollo_adc_input_signal_e           inp_signal_type[ADI_APOLLO_ADCS_PER_SIDE];      /*!< Input Signal Type */
    adi_apollo_adc_input_polarity_e         inp_polarity[ADI_APOLLO_ADCS_PER_SIDE];         /*!< Input Polarity */
    adi_apollo_adc_slice_mode_switch_cfg_t  slice_mode_switch_cfg;                          /*!< Slice mode switch configuration */
    uint32_t                                adc_options[8];                                 /*!< ADC Options */
} adi_apollo_adc_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Coarse DDC (Digital Down Converter) Ratio/Decimation Enumeration
 */
typedef enum adi_apollo_cddc_ratio {
    ADI_APOLLO_CDDC_RATIO_1     = 0u,   /*!< Decimate by 1 */
    ADI_APOLLO_CDDC_RATIO_2     = 1u,   /*!< Decimate by 2 */
    ADI_APOLLO_CDDC_RATIO_3     = 2u,   /*!< Decimate by 3 */
    ADI_APOLLO_CDDC_RATIO_4     = 3u,   /*!< Decimate by 4 */
    ADI_APOLLO_CDDC_RATIO_6     = 4u,   /*!< Decimate by 6 */
    ADI_APOLLO_CDDC_RATIO_12    = 6u,   /*!< Decimate by 12 */
} ADI_APOLLO_PACK_ENUM adi_apollo_cddc_ratio_e;


/**
 * \brief Coarse DUC (Digital Up Converter) Ratio/Interpolation Enumeration
 */
typedef enum adi_apollo_cduc_ratio {
    ADI_APOLLO_CDUC_RATIO_1     = 1u,   /*!< Interpolate by 1 */
    ADI_APOLLO_CDUC_RATIO_2     = 2u,   /*!< Interpolate by 2 */
    ADI_APOLLO_CDUC_RATIO_3     = 3u,   /*!< Interpolate by 3 */
    ADI_APOLLO_CDUC_RATIO_4     = 4u,   /*!< Interpolate by 4 */
    ADI_APOLLO_CDUC_RATIO_6     = 6u,   /*!< Interpolate by 6 */
    ADI_APOLLO_CDUC_RATIO_8     = 8u,   /*!< Interpolate by 8 */
    ADI_APOLLO_CDUC_RATIO_12    = 12u,  /*!< Interpolate by 12 */
} ADI_APOLLO_PACK_ENUM adi_apollo_cduc_ratio_e;


/**
 * \brief Fine DDC (Digital Down Converter) Ratio/Decimation Enumeration
 */
typedef enum adi_apollo_fddc_ratio {
    ADI_APOLLO_FDDC_RATIO_1     = 0u,   /*!< Decimate by 1 */
    ADI_APOLLO_FDDC_RATIO_2     = 1u,   /*!< Decimate by 2 */
    ADI_APOLLO_FDDC_RATIO_4     = 2u,   /*!< Decimate by 4 */
    ADI_APOLLO_FDDC_RATIO_8     = 3u,   /*!< Decimate by 8 */
    ADI_APOLLO_FDDC_RATIO_16    = 4u,   /*!< Decimate by 16 */
    ADI_APOLLO_FDDC_RATIO_32    = 5u,   /*!< Decimate by 32 */
    ADI_APOLLO_FDDC_RATIO_64    = 6u    /*!< Decimate by 64 */
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_ratio_e;


/**
 * \brief Fine DUC (Digital Up Converter) Ratio/Interpolation Enumeration
 */
typedef enum adi_apollo_fduc_ratio {
    ADI_APOLLO_FDUC_RATIO_1     = 1u,   /*!< Interpolate by 1 */
    ADI_APOLLO_FDUC_RATIO_2     = 2u,   /*!< Interpolate by 2 */
    ADI_APOLLO_FDUC_RATIO_4     = 4u,   /*!< Interpolate by 4 */
    ADI_APOLLO_FDUC_RATIO_8     = 8u,   /*!< Interpolate by 8 */
    ADI_APOLLO_FDUC_RATIO_16    = 16u,  /*!< Interpolate by 16 */
    ADI_APOLLO_FDUC_RATIO_32    = 32u,  /*!< Interpolate by 32 */
    ADI_APOLLO_FDUC_RATIO_64    = 64u   /*!< Interpolate by 64 */
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_ratio_e;


/**
 * \brief NCO Mixer Modes Enumeration
 */
typedef enum adi_apollo_nco_mixer_mode {
    ADI_APOLLO_MXR_VAR_IF_MODE  = 0u,       /*!< Variable IF Mode */
    ADI_APOLLO_MXR_ZERO_IF_MODE = 1u,       /*!< Zero IF Mode */
    ADI_APOLLO_MXR_FS_BY_4_MODE = 2u,       /*!< Fs/4 Hz IF Mode */
    ADI_APOLLO_MXR_TEST_MODE    = 3u        /*!< Test Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_mixer_mode_e;


/**
 * \brief NCO Modes Enumeration
 */
typedef enum adi_apollo_nco_mode {
    ADI_APOLLO_NCO_MOD_INTEGER          = 0u,   /*!< Integer Mode */
    ADI_APOLLO_NCO_MOD_INT_PLUS_MODULUS = 1u    /*!< Modulus Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_mode_e;


/**
 * \brief NCO Profile Word Select Enumeration
 */
typedef enum adi_apollo_nco_profile_word_sel {
    ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT  = 0u,   /*!< Phase Increment Word */
    ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET     = 1u,   /*!< Phase Offset Word */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_profile_word_sel_e;


/**
 * \brief FNCO Profile Chan Selection Mode Enumeration
 */
typedef enum adi_apollo_nco_profile_sel_mode {
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_AUTO      = 0u,    /*!< Trigger based hopping. Auto Hopping Mode */
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_REGMAP    = 1u,    /*!< Trigger based hopping. Scheduled Regmap  */
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_GPIO      = 2u,    /*!< Trigger based hopping. Scheduled GPIO */
    ADI_APOLLO_NCO_CHAN_SEL_DIRECT_GPIO    = 3u,    /*!< Direct GPIO profile select. All params hop together */
    ADI_APOLLO_NCO_CHAN_SEL_DIRECT_REGMAP  = 4u     /*!< Direct spi/hsci nco profile select. All params hop together */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_profile_sel_mode_e;


/**
 * \brief FNCO Trigger Hop Select Enumeration
 */
typedef enum adi_apollo_fnco_trig_hop_sel {
    ADI_APOLLO_FNCO_TRIG_HOP_FREQ       = 1u,   /*!< Hop freq in trigger based hopping */
    ADI_APOLLO_FNCO_TRIG_HOP_PHASE      = 4u,   /*!< Hop phase in trigger based hopping */
    ADI_APOLLO_FNCO_TRIG_HOP_FREQ_PHASE = 5u    /*!< Hop freq and phase in trigger based hopping */
} ADI_APOLLO_PACK_ENUM adi_apollo_fnco_trig_hop_sel_e;


/**
 * \brief NCO Next Hop Select Mode Enumeration. Applicable for trigger based auto hop.
 */
typedef enum adi_apollo_nco_auto_hop_dir_sel {
    ADI_APOLLO_NCO_AUTO_HOP_DIR_DECR    = 0u,   /*!< Decrement profile num on trigger */
    ADI_APOLLO_NCO_AUTO_HOP_DIR_INCR    = 1u,   /*!< Increment profile num on trigger */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_auto_hop_dir_sel_e;


/**
 * \brief DRC Mixer Selection Enumeration
 */
typedef enum adi_apollo_drc_mixer_sel {
    ADI_APOLLO_DRC_MIXER_REAL       = 0u,   /*!< Real Mixing */
    ADI_APOLLO_DRC_MIXER_COMPLEX    = 1u    /*!< Complex Mixing */
} ADI_APOLLO_PACK_ENUM adi_apollo_drc_mixer_sel_e;


/**
 * \brief CDRC TB1 Filter Delay Enumeration
 */
typedef enum adi_apollo_cdrc_tb1_filt_dly {
    ADI_APOLLO_CDRC_TB1_FILT_DLY_NONE       = 0u,   /*!< No filter delay */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_1    = 1u,   /*!< Delay 1 filter stage */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_2    = 2u,   /*!< Delay 2 filter stages */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_3    = 3u    /*!< Delay 3 filter stages */
} ADI_APOLLO_PACK_ENUM adi_apollo_cdrc_tb1_filt_dly_e;


/**
 * \brief Complex Mixer Multiplication Scaling Enumeration
 */
typedef enum adi_apollo_nco_cmplx_mult_scale {
    ADI_APOLLO_CMPLX_MULT_SCALE_0p7 = 0u,       /*!< Scale by .7 */
    ADI_APOLLO_CMPLX_MULT_SCALE_1   = 1u        /*!< Scale by 1 */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_cmplx_mult_scale_e;


/**
 * \brief CNCO Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cnco_cfg {
    bool                              drc_en;                                               /*!< CDRC enable */
    adi_apollo_nco_mode_e             nco_mode;                                             /*!< NCO integer or integer plus modulus */

    /* Register name in Yoda: coarse_drc_mixer_ctrl */
    adi_apollo_nco_mixer_mode_e       nco_if_mode;                                          /*!< NCO mixer mode. var-if, zero-if, fs-by-4, dc-test */
    adi_apollo_drc_mixer_sel_e        drc_mxr_sel;                                          /*!< Real or complex mixing select */
    adi_apollo_nco_cmplx_mult_scale_e cmplx_mxr_mult_scale_en;                              /*!< NCO complex multiplication scale */
    uint16_t                          dc_testmode_value;                                    /*!< Mixer test mode value */

    uint32_t                          nco_phase_inc;                                        /*!< NCO phase increment word */
    uint32_t                          nco_phase_offset;                                     /*!< NCO phase offset word */
    uint32_t                          nco_phase_inc_frac_a;                                 /*!< Modulus phase increment numerator. */
    uint32_t                          nco_phase_inc_frac_b;                                 /*!< Modulus phase increment denominator. */
    uint32_t                          nco_phase_inc_words[ADI_APOLLO_CNCO_PROFILE_NUM];     /*!< NCO phase increment words */
    uint32_t                          nco_phase_offset_words[ADI_APOLLO_CNCO_PROFILE_NUM];  /*!< NCO phase offset words */

    bool                              amp_dither_en;                                        /*!< Amplitude Dither Enable, False: Disable, True: Enabled. */
    bool                              phase_dither_en;                                      /*!< Phase Dither Enable, False: Disable, True: Enabled. */

    /* Hopping and Trigger */
    adi_apollo_nco_profile_sel_mode_e nco_profile_sel_mode;                                 /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_hop_dir_sel_e nco_auto_inc_dec;                                     /*!< Increment or decrement profile selection for auto hop. */
    bool                              debug_cdrc_clkoff_n;                                  /*!< if TRUE (typical), enable nco clocks */

} adi_apollo_cnco_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Fine NCO Hop Mode Enumeration
 */
typedef enum adi_apollo_fnco_hop_mode {
    ADI_APOLLO_FNCO_HOP_MODE_STANDARD           = 0u,   /*!< Standard Hopping Mode */
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP              = 1u,   /*!< Frequency Ramp Hopping Mode */
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP_JMP          = 2u,   /*!< Frequency Ramp with Jump Hop Mode */
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP_JMP_ARAMP    = 3u    /*!< Frequency Ramp with Jump and Amplitude Ramping Hop Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_fnco_hop_mode_e;


/**
 * \brief FNCO Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fnco_cfg {
    bool                                drc_en;                                                 /*!< FDRC enable */
    adi_apollo_nco_mode_e               nco_mode;                                               /*!< NCO integer or integer plus modulus */

    /* Register name in Yoda: fine_mixer_ctrl */
    adi_apollo_nco_mixer_mode_e         nco_if_mode;                                            /*!< NCO mixer mode. var-if, zero-if, fs-by-4, dc-test */
    adi_apollo_drc_mixer_sel_e          drc_mxr_sel;                                            /*!< Real or complex mixing select */
    adi_apollo_nco_cmplx_mult_scale_e   cmplx_mxr_mult_scale_en;                                /*!< NCO complex multiplication scale */
    uint16_t                            dc_testmode_value;                                      /*!< Mixer test mode value */

    uint64_t                            nco_phase_inc;                                          /*!< NCO phase increment word */
    uint64_t                            nco_phase_offset;                                       /*!< NCO phase offset word */
    uint64_t                            nco_phase_inc_frac_a;                                   /*!< Modulus phase increment numerator. */
    uint64_t                            nco_phase_inc_frac_b;                                   /*!< Modulus phase increment denominator. */

    bool                                amp_dither_en;                                          /*!< Amplitude Dither Enable, False: Disable, True: Enabled. */
    bool                                phase_dither_en;                                        /*!< Phase Dither Enable, False: Disable, True: Enabled. */

    bool                                hop_mode_en;                                            /*!< If TRUE, enable freq hopping. */
    uint8_t                             hop_mode;                                               /*!< Fine NCO hop mode. */

    uint32_t                            nco_phase_inc_words[ADI_APOLLO_FNCO_PROFILE_NUM];       /*!< NCO phase increment words */
    uint32_t                            nco_phase_offset_words[ADI_APOLLO_FNCO_PROFILE_NUM];    /*!< NCO phase offset words */

    /* Hopping and Trigger */
    adi_apollo_nco_profile_sel_mode_e   nco_profile_sel_mode;                                   /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_hop_dir_sel_e   nco_auto_inc_dec_freq;                                  /*!< Increment or decrement freq profile selection for auto hop. */
    adi_apollo_nco_auto_hop_dir_sel_e   nco_auto_inc_dec_phase;                                 /*!< Increment or decrement phase profile selection for auto hop. */
    adi_apollo_fnco_trig_hop_sel_e      nco_trig_hop_sel;                                       /*!< Select params to change on hop. Freq, phase or both */
    bool                                debug_fdrc_clkoff_n;                                    /*!< if TRUE (typical), enable nco clocks */
} adi_apollo_fnco_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief TB1 Filter Delay Setting Enumeration
 */
typedef enum adi_apollo_tb1_filt_dly_cycles {
    ADI_APOLLO_TB1_FILT_DLY_DISABLED    = 0u,   /*!< TB1 Filter Delay Disabled */
    ADI_APOLLO_TB1_FILT_DLY_1CYCLE      = 1u,   /*!< TB1 Filter 1 Cycle Delay */
    ADI_APOLLO_TB1_FILT_DLY_2CYCLES     = 2u    /*!< TB1 Filter 2 Cycles Disabled */
} ADI_APOLLO_PACK_ENUM adi_apollo_tb1_filt_dly_cycles_e;


/**
 * \brief Coarse Digital Down Converter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cddc_cfg  {
    adi_apollo_cnco_cfg_t               nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to CDDC */
    adi_apollo_cddc_ratio_e             drc_ratio;                          /*!< Data Rate Conversion Ratio (Decimation) */
    bool                                fine_bypass;                        /*!< true = bypass the fine DDC. */
    uint8_t                             link_num;                           /*!< Jesd link num CDDC is connected to (when fddc is bypassed) */
    bool                                hb1_filt_dly_en;                    /*!< if TRUE, HB1 filter uses a delayed input */
    bool                                hb2_filt_dly_en;                    /*!< if TRUE, HB2 filter uses a delayed input */
    adi_apollo_tb1_filt_dly_cycles_e    tb1_filt_dly_cycles;                /*!< 0 = No Delay, 1 = 1 Cycle, 2 = 2 Cycles */
    bool                                hb1_gain_6db_en;                    /*!< if TRUE, the 6dB gain for HB1 filter is enabled */
    bool                                tb1_gain_6db_en;                    /*!< if TRUE, the 6dB gain for TB1 filter is enabled */

    bool                                trig_mst_en;                        /*!< true to enable the trigger master, false to disable */
    uint64_t                            trig_mst_period;                    /*!< Trigger master period, in units of Fs. Used w/ profile sel timer */
    uint64_t                            trig_mst_offset;                    /*!< Trigger master offset, in units of Fs. Used w/ profile sel timer */
    uint8_t                             debug_cddc_clkoff_n;                /*!< If this bit is zero the clocks are shutoff.bit 0 : filter input clock bit 1 : filter output clockbit 2:  hb1 clockbit 3: hb2 clockbit 4: reservedbit 5: reservedbit 6: tb1 clock */
    /* Trigger sel mux: external, internal or spi (see txrx_trigger_ts, reg: TRIG_SEL_MUX_CDRC0n amd TRIG_SEL_MUX_CDRC1n) */
} adi_apollo_cddc_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Coarse Digital Up Converter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cduc_cfg {
    adi_apollo_cnco_cfg_t   nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to CDUC */
    adi_apollo_cduc_ratio_e drc_ratio;                          /*!< Data Rate Conversion Ratio (Interpolation) */
} adi_apollo_cduc_cfg_t;
ADI_APOLLO_PACK_FINISH

/**
 * \brief Fine Digital Down Converter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fddc_cfg {
    adi_apollo_fnco_cfg_t   nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to FDDC */
    adi_apollo_fddc_ratio_e drc_ratio;                          /*!< Data Rate Conversion Ratio (Decimation) */

    uint8_t                 link_num;                           /*!< Jesd link num FDDC is connected */
    uint8_t                 debug_fddc_clkoff_n;                /*!< 0xff (typical) to enable ddc clks */
    bool                    hb1_filt_dly_en;                    /*!< if TRUE, HB1 filter uses a delayed input */
    bool                    hb2_filt_dly_en;                    /*!< if TRUE, HB2 filter uses a delayed input */
    bool                    hb3_filt_dly_en;                    /*!< if TRUE, HB3 filter uses a delayed input */
    bool                    hb4_filt_dly_en;                    /*!< if TRUE, HB4 filter uses a delayed input */
    bool                    hb5_filt_dly_en;                    /*!< if TRUE, HB5 filter uses a delayed input */
    bool                    hb6_filt_dly_en;                    /*!< if TRUE, HB6 filter uses a delayed input */
    bool                    hb1_gain_6db_en;                    /*!< if TRUE, the 6dB gain for HB1 filter is enabled */
} adi_apollo_fddc_cfg_t;
ADI_APOLLO_PACK_FINISH

/**
 * \brief Fine Digital Up Converter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fduc_cfg {
    adi_apollo_fnco_cfg_t   nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to FDUC */
    adi_apollo_fduc_ratio_e drc_ratio;                          /*!< Data Rate Conversion Ratio (Interpolation) */

    bool                    sub_dp_gain_en;                     /*!< FDUC input gain control enable */
    uint16_t                subdp_gain;                         /*!< FDUC sub data path gain */
} adi_apollo_fduc_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Programmable Filter (PFILT) Mode Enumeration
 */
typedef enum adi_apollo_pfilt_mode {
    ADI_APOLLO_PFILT_MODE_DISABLED          = 0u,   /*!< Disabled (filters bypassed) */
    ADI_APOLLO_PFILT_MODE_N_DIV_BY_4_REAL   = 1u,   /*!< Real N/4 Tap Filter for the I/Q channel */
    ADI_APOLLO_PFILT_MODE_N_DIV_BY_2_REAL   = 2u,   /*!< Real N/2 Tap Filter for the I/Q channel */
    ADI_APOLLO_PFILT_MODE_MATRIX            = 4u,   /*!< Real N/4 tap Matrix mode of operation (pfilt_q_mode/pfilt_i_mode must also be set to 100) */
    ADI_APOLLO_PFILT_MODE_HALF_COMPLEX      = 6u,   /*!< Half Complex Filter using N/2-Tap Filters for the Q/I channel + N/2 Tap Programmable Delay Line for the I/Q Channel */
    ADI_APOLLO_PFILT_MODE_N_REAL            = 7u    /*!< Real N Tap Filter for the I/Q (pfilt_q_mode/pfilt_i_mode must be set to 000 */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_e;


/**
 * \brief Programmable Filter Gain Enumeration
 */
typedef enum adi_apollo_pfilt_gain {
    ADI_APOLLO_PFILT_GAIN_ZERO_DB   = 0u,   /*!< No Gain (0 dB) */
    ADI_APOLLO_PFILT_GAIN_POS_6_DB  = 1u,   /*!< +6 dB Gain */
    ADI_APOLLO_PFILT_GAIN_POS_12_DB = 2u,   /*!< +12 dB Gain */
    ADI_APOLLO_PFILT_GAIN_POS_18_DB = 3u,   /*!< +18 dB Gain */
    ADI_APOLLO_PFILT_GAIN_POS_24_DB = 4u,   /*!< +24 dB Gain */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_gain_e;


/**
 * \brief Programmable Filter Coefficient Set Enumeration
 */
typedef enum adi_apollo_pfilt_coeffs_sets_per_prfl {
    ADI_APOLLO_PFILT_COEFF_SET_0    = 0u,               /*!< Coefficient Set 0 */
    ADI_APOLLO_PFILT_COEFF_SET_1    = 1u,               /*!< Coefficient Set 1 */
    ADI_APOLLO_PFILT_COEFF_SET_2    = 2u,               /*!< Coefficient Set 2 */
    ADI_APOLLO_PFILT_COEFF_SET_3    = 3u,               /*!< Coefficient Set 3 */
    ADI_APOLLO_PFILT_COEFF_SET_4    = 4u,               /*!< Coefficient Set 4 */
    ADI_APOLLO_PFILT_COEFF_SET_5    = 5u,               /*!< Coefficient Set 5 */
    ADI_APOLLO_PFILT_COEFF_SET_6    = 6u,               /*!< Coefficient Set 6 */
    ADI_APOLLO_PFILT_COEFF_SET_7    = 7u,               /*!< Coefficient Set 7 */
    ADI_APOLLO_PFILT_COEFF_SETS     = 8u                /*!< Total Number of Coefficient Sets */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_coeffs_sets_per_prfl_e;

/**
 * \brief Programmable Filter Control Set Enumeration
 */
typedef enum adi_apollo_pfilt_ctrl_sets {
    ADI_APOLLO_PFILT_CTRL_SET_0 = 0u,       /*!< Control Set 0 */
    ADI_APOLLO_PFILT_CTRL_SET_1 = 1u,       /*!< Control Set 1 */
    ADI_APOLLO_PFILT_CTRL_SETS  = 2u        /*!< Total Number of Control Sets */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_ctrl_sets_e;


/**
 * \brief Programmable Filter Mode Selection Enumeration
 */
typedef enum adi_apollo_pfilt_dq_mode {
    ADI_APOLLO_PFILT_DUAL_MODE  = 0u,   /*!< PFILT Dual Mode */
    ADI_APOLLO_PFILT_QUAD_MODE  = 1u,   /*!< PFILT Quad Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_dq_mode_e;


/**
 * \brief Programmable Filter Data Mode Selection Enumeration
 */
typedef enum adi_apollo_pfilt_data {
    ADI_APOLLO_PFILT_COMPLEX_DATA   = 0u,   /*!< PFILT Complex Data */
    ADI_APOLLO_PFILT_REAL_DATA      = 1u    /*!< PFILT Real Data */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_data_e;


/**
 * \brief Programmable Filter Switch Selection Enumeration
 */
typedef enum adi_apollo_pfilt_mode_sw_add_sub {
    ADI_APOLLO_PFILT_SUB_FOR_MOD_SW = 0u,       /*!< Select subtraction operation */
    ADI_APOLLO_PFILT_ADD_FOR_MOD_SW = 1u        /*!< Select addition operation */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_sw_add_sub_e;


/**
 * \brief Programmable Filter Switch 3dB Averaging Enable Enumeration
 */
typedef enum adi_apollo_pfilt_mode_sw_ave_en {
    ADI_APOLLO_PFILT_DISABLE_3DB_AVG_MOD_SW = 0u,   /*!< Disable Mode Switch (3dB Average). Only for Rx PFILT */
    ADI_APOLLO_PFILT_ENABLE_3DB_AVG_MOD_SW  = 1u    /*!< Enable Mode Switch (3dB Average). Only for Rx PFILT */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_sw_ave_en_e;


/**
 * \brief Programmable Filter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_pfilt_cfg {
    bool                                enable;                                                         /*!< Filter enable */

    int16_t                             coeffs[ADI_APOLLO_PFILT_COEFF_SETS][ADI_APOLLO_PFILT_COEFFS];   /*!< List of filter coefficients */

    adi_apollo_pfilt_mode_e             i_mode[ADI_APOLLO_PFILT_CTRL_SETS];                             /*!< Filter mode for i stream */
    adi_apollo_pfilt_mode_e             q_mode[ADI_APOLLO_PFILT_CTRL_SETS];                             /*!< Filter mode for q stream */

    bool                                output_dither_en[ADI_APOLLO_PFILT_CTRL_SETS];                   /*!< Enable Dither at Final Output Truncation (Rx Only) */

    adi_apollo_pfilt_data_e             real_data;                                                      /*!< Selects real or complex data streams */
    adi_apollo_pfilt_dq_mode_e          dq_mode;                                                        /*!< Dual(4t4r) or Quad(8t8r) filter mode */
    adi_apollo_pfilt_mode_sw_add_sub_e  add_sub_sel;                                                    /*!< Add/Sub for Mode Switch (3dB Average) */
    adi_apollo_pfilt_mode_sw_ave_en_e   mode_switch;                                                    /*!< Enable for 3dB Averaging (Mode Switch) for Rx PFILT */
    bool                                pfilt_trigger_en;                                               /*!< Enable for Trigger-based Profile Hopping */

    adi_apollo_pfilt_gain_e             pfir_ix_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain */
    adi_apollo_pfilt_gain_e             pfir_iy_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. Only used in matrix mode */
    adi_apollo_pfilt_gain_e             pfir_qx_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. */
    adi_apollo_pfilt_gain_e             pfir_qy_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. Only used in matrix mode */

    uint8_t                             pfir_ix_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain */
    uint8_t                             pfir_iy_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain. Only used in matrix mode */
    uint8_t                             pfir_qx_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain */
    uint8_t                             pfir_qy_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain. Only used in matrix mode */

    uint8_t                             hc_prog_delay[ADI_APOLLO_PFILT_COEFF_SETS];                     /*!<  Programmable delay line */
} adi_apollo_pfilt_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Complex Filter Profile Index Enumeration
 */
typedef enum adi_apollo_cfir_profiles {
    ADI_APOLLO_CFIR_PROFILE_IDX_0   = 0u,   /*!< CFIR Profile Index 0 */
    ADI_APOLLO_CFIR_PROFILE_IDX_1   = 1u,   /*!< CFIR Profile Index 1 */
    ADI_APOLLO_CFIR_NUM_PROFILES    = 2u    /*!< Number of CFIR Profile Indices */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_profiles_e;


/**
 * \brief Complex Filter Coefficient Set Enumeration
 */
typedef enum adi_apollo_cfir_coeffs_sets_per_prfl {
    ADI_APOLLO_CFIR_COEFF_SET_0 = 0,                /*!< CFIR Coefficient Set 0 */
    ADI_APOLLO_CFIR_COEFF_SET_1 = 1,                /*!< CFIR Coefficient Set 1 */
    ADI_APOLLO_CFIR_COEFF_SET_2 = 2,                /*!< CFIR Coefficient Set 2 */
    ADI_APOLLO_CFIR_COEFF_SET_3 = 3,                /*!< CFIR Coefficient Set 3 */
    ADI_APOLLO_CFIR_COEFF_SETS  = 4                 /*!< Number of CFIR Coefficient Sets */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_coeffs_sets_per_prfl_e;


/**
 * \brief Complex Filter Gain Enumeration
 */
typedef enum adi_apollo_cfir_gain {
    ADI_APOLLO_CFIR_GAIN_MINUS18_DB = 0,    /*!< -18 dB */
    ADI_APOLLO_CFIR_GAIN_MINUS12_DB = 1,    /*!< -12 dB */
    ADI_APOLLO_CFIR_GAIN_MINUS6_DB  = 2,    /*!< -6 dB */
    ADI_APOLLO_CFIR_GAIN_ZERO_DB    = 3,    /*!< No Gain (0 dB) */
    ADI_APOLLO_CFIR_GAIN_PLUS6_DB   = 4,    /*!< +6 dB */
    ADI_APOLLO_CFIR_GAIN_PLUS12_DB  = 5     /*!< +12 dB */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_gain_e;


/**
 * \brief Complex Filter Band Mode Enumeration
 */
typedef enum adi_apollo_cfir_sing_dual_mode {
    ADI_APOLLO_CFIR_SINGLE_BAND = 0,            /*!< CFIR Single Band Mode */
    ADI_APOLLO_CFIR_DUAL_BAND   = 1             /*!< CFIR Dual Band Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_sing_dual_mode_e;


/**
 * \brief Complex Filter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cfir_cfg {
    int16_t                             coeffs_i[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_NUM_TAPS];   /*!< CFIR I-data Filter Coefficients */
    int16_t                             coeffs_q[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_NUM_TAPS];   /*!< CFIR Q-data Filter Coefficients */
    adi_apollo_cfir_gain_e              cfir_gain_dB[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];                         /*!< CFIR Gain in dB Coefficients */
    uint16_t                            scalar_i[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];                             /*!< CFIR Scalar I Coefficients */
    uint16_t                            scalar_q[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];                             /*!< CFIR Scalar Q Coefficients */
    bool                                enable;                                                                                         /*!< High-level enable for complex filter */
    bool                                sparse_mode;                                                                                    /*!< CFIR Sparse Filter Mode */
    adi_apollo_cfir_sing_dual_mode_e    cfir_mode;                                                                                      /*!< CFIR Single/Dual Mode */
} adi_apollo_cfir_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Fractional Sampling Rate Converter Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fsrc_cfg {
    uint64_t    fsrc_rate_int;          /*!< Rate, integer part (48 bits). */
    uint64_t    fsrc_rate_frac_a;       /*!< Rate, numerator of fractional part (48 bits). */
    uint64_t    fsrc_rate_frac_b;       /*!< Rate, denominator of fractional part (48 bits). */
    uint16_t    fsrc_delay;             /*!< Sample fractional delay. */
    uint16_t    gain_reduction;         /*!< 12 bit value */
    uint8_t     ptr_syncrstval;         /*!< 6 bit value this is used for the FIFO pointer value when the internal fifo-sync is generated when ptr_overwrite is 1 */
    bool        ptr_overwrite;          /*!< 0 == FIFO reset ptr computed by design, 1 == Use pointer value from ptr_syncrstval  */
    bool        data_mult_dither_en;    /*!< 0 == Disable, 1 == Enable to use added dither in the fsrc data and delta multiplication calculation */
    bool        dither_en;              /*!< 0 == Disable, 1 == Enable to use added dither in the fsrc delta delay calculation */
    bool        split_4t4r;             /*!< 0 == Normal mode, 1 == Use the 4t4r split (4t4r two-stream cases) */
    bool        mode_1x;                /*!< 0 == Normal mode, 1 == 1x mode */
    bool        enable0;                /*!< 0 == Disable, 1 == Enable for fsrc0 */
    bool        enable1;                /*!< 0 == Disable, 1 == Enable for fsrc1 */
    bool        bypass;                 /*!< 0 == Not Bypassed, 1 == Bypass */
} adi_apollo_fsrc_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Dynamic Reconfig Configuration Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dyn_recfg_cfg {
    uint16_t    prefsrc_lcm;                /*!<  9 bit value  */
    uint16_t    postfsrc_lcm;               /*!<  16 bit value */
    bool        trig_recfg_mode;            /*!<  0 == Sync Mode, 1 == Trig Mode */
    bool        cnco_rst_en;                /*!<  0 == Disable, 1 == Enable */
    bool        fnco_rst_en;                /*!<  0 == Disable, 1 == Enable */
    bool        resync_en;                  /*!<  0 == Disable, 1 == Enable */
    bool        ts_rst_en;                  /*!<  0 == Disable, 1 == Enable */
    bool        tzero_coher_en;             /*!<  0 == Disable, 1 == Enable */
} adi_apollo_dyn_recfg_cfg_t;
ADI_APOLLO_PACK_FINISH


#define ADI_APOLLO_FDUC0_REPRESENTATION_IN_CDUC_SUMMER  0x01    /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC1_REPRESENTATION_IN_CDUC_SUMMER  0x02    /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC2_REPRESENTATION_IN_CDUC_SUMMER  0x04    /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC3_REPRESENTATION_IN_CDUC_SUMMER  0x08    /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC4_REPRESENTATION_IN_CDUC_SUMMER  0x10    /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC5_REPRESENTATION_IN_CDUC_SUMMER  0x20    /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC6_REPRESENTATION_IN_CDUC_SUMMER  0x40    /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC7_REPRESENTATION_IN_CDUC_SUMMER  0x80    /*!<  Only 8T8R */


/**
 * \brief Tx Mux1 (Mod Switch) Control Enumeration
 */
typedef enum adi_apollo_tx_mux1_sel {
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0_OUT1_FROM_I1        = 0u,   /*!< DAC0 = CNCO_[Side]0_I, DAC1 = CNCO_[Side]1_I */
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0I1_OUT1_FROM_Q0Q1    = 1u,   /*!< DAC0 = CNCO_[Side]0_I + CNCO_[Side]1_I, DAC1 = CNCO_[Side]0_Q + CNCO_[Side]1_Q */
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0_OUT1_FROM_Q0        = 2u,   /*!< DAC0 = CNCO_[Side]0_I, DAC1 = CNCO_[Side]0_Q */
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I1_OUT1_FROM_Q1        = 3u,   /*!< DAC0 = CNCO_[Side]1_I, DAC1 = CNCO_[Side]1_Q */

    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0_OUT1_FROM_I2        = 0u,   /*!< DAC0 = CNCO_[Side]0_I, DAC1 = CNCO_[Side]2_I */
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0I2_OUT1_FROM_Q0Q2    = 1u,   /*!< DAC0 = CNCO_[Side]0_I + CNCO_[Side]2_I, DAC1 = CNCO_[Side]0_Q + CNCO_[Side]2_Q */
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0_OUT1_FROM_Q0        = 2u,   /*!< DAC0 = CNCO_[Side]0_I, DAC1 = CNCO_[Side]0_Q */
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I2_OUT1_FROM_Q2        = 3u,   /*!< DAC0 = CNCO_[Side]2_I, DAC1 = CNCO_[Side]2_Q */

    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1_OUT3_FROM_I3        = 0u,   /*!< DAC2 = CNCO_[Side]1_I, DAC3 = CNCO_[Side]3_I */
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1I3_OUT3_FROM_Q1Q3    = 1u,   /*!< DAC2 = CNCO_[Side]1_I + CNCO_[Side]3_I, DAC3 = CNCO_[Side]1_Q + CNCO_[Side]3_Q */
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1_OUT3_FROM_Q1        = 2u,   /*!< DAC2 = CNCO_[Side]1_I, DAC3 = CNCO_[Side]1_Q */
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I3_OUT3_FROM_Q3        = 3u    /*!< DAC2 = CNCO_[Side]3_I, DAC3 = CNCO_[Side]3_Q */
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux1_sel_e;

/**
 * \brief Tx Mux1 (Mod Switch) Index Enumeration
 */
typedef enum adi_apollo_tx_mux1_idx {
    ADI_APOLLO_4T4R_MODSW0      = 0u,   /*!< 4t4r Mod Switch 0 Index */
    ADI_APOLLO_8T8R_MODSW0      = 0u,   /*!< 8t8r Mod Switch 0 Index */
    ADI_APOLLO_8T8R_MODSW1      = 1u,   /*!< 8t8r Mod Switch 1 Index */
    ADI_APOLLO_TX_MUX1_NUM_4T4R = 1u,   /*!< 4t4r has only 1 index */
    ADI_APOLLO_TX_MUX1_NUM_8T8R = 2u    /*!< Both modsw0 and modsw1 bit fields are in register dp_cfg */
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux1_idx_e;

/**
 * \brief High-speed cross bar control Mod Switch or ADC to DAC
 */
typedef enum adi_apollo_tx_mux0_sel {
    ADI_APOLLO_4T4R_DAC_FROM_MODSW0_OUT0    = 0u,
    ADI_APOLLO_4T4R_DAC_FROM_MODSW0_OUT1    = 1u,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW0_OUT0    = 0u,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW0_OUT1    = 1u,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW1_OUT0    = 2u,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW1_OUT1    = 3u,
    ADI_APOLLO_4T4R_LOOPBACK_DAC_FROM_ADC0  = 0u,
    ADI_APOLLO_4T4R_LOOPBACK_DAC_FROM_ADC1  = 1u,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC0  = 0u,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC1  = 1u,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC2  = 2u,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC3  = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux0_sel_e;


/**
 * \brief Tx Path Misc Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_txpath_misc {
    uint8_t                     cduc_dac_enables[ADI_APOLLO_CDUC_PATHS_PER_SIDE];   /*!< 1xNx XBar Control, onlt used when FDUCs are bypassed. */
    uint8_t                     fduc_cduc_summer[ADI_APOLLO_CDUC_PATHS_PER_SIDE];   /*!< FDUC to CDUC summer blocks. */
    /*!< If fduc_cduc_summer[0] = FDUC0_REPRESENTATION_IN_CDUC_SUMMER + FDUC1_REPRESENTATION_IN_CDUC_SUMMER,
         Then the CDUC 0 input will be the sum of FDUC 0 and FDUC 1. In Yoda, the registers are enables00, enables01, enables10, enables11 */

    adi_apollo_tx_mux1_sel_e    mux1_sel[ADI_APOLLO_TX_MUX1_NUM_8T8R];              /*!< Mod Switch controls */
    adi_apollo_tx_mux0_sel_e    mux0_sel[ADI_APOLLO_CDUC_PATHS_PER_SIDE];           /*!< Each of these occupy two bits in register hs_xbar_ctrl */
    bool                        low_samp_en;                                        /*!< Tx_Top.Tx_Misc.low_samp, TRUE: enable low sampling */
} adi_apollo_txpath_misc_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief PA Protect Power Detector Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_pwr_cfg {
    bool        pa_clk_en;                  /*!< True to enable when TxEN is low */
    bool        short_pac_en;               /*!< True to enable */
    bool        long_pac_en;                /*!< True to enable */
    uint16_t    pwr_threshold_long;         /*!< 13Bits for the long window average power threshold */
    uint16_t    pwr_threshold_short;        /*!< 13Bits for the short window average power threshold */
    uint8_t     avg_long_win;               /*!< 4Bits for 2 ** (9+avg_long_win) sample count */
    uint8_t     avg_short_win;              /*!< 2Bits for 2 ** (3+avg_short_win) sample count */
} adi_apollo_paprot_pwr_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief PA Protect Zero Flush from PA to FDUC Select
 */
typedef enum adi_apollo_paprot_zf_sel {
    ADI_APOLLO_PAPROT_ZF_SEL_F0_P0  = 0u,
    ADI_APOLLO_PAPROT_ZF_SEL_F2_P0  = 1u,
    ADI_APOLLO_PAPROT_ZF_SEL_F1_P0  = 2u,
    ADI_APOLLO_PAPROT_ZF_SEL_F3_P0  = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_paprot_zf_sel_e;


/**
 * \brief PA Protect JESD and Data Ready Select
 */
typedef enum adi_apollo_paprot_jesd_dr_sel {
    ADI_APOLLO_PAPROT_JESD_DF_SEL_L0    = 0u,
    ADI_APOLLO_PAPROT_JESD_DF_SEL_L1    = 1u,
    ADI_APOLLO_PAPROT_JESD_DF_SEL_BOTH  = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_paprot_jesd_dr_sel_e;


/**
 * \brief PA Protect State Machine Control Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_sm_cfg {
    bool                            enable_paprotsm;                    /*!< 1: Enable the PA protection state machine. The PA protection fsm is the main control of the PA protection algorithm. It monitors alarms and initiate ramp up/down when necessary. */
    bool                            rampdwn_to_papin_en;                /*!< 1: Will bring the RAMP down signal out on PA_EN pins. 0: The PA_EN signal will come out of txen power ctrl machine */
    bool                            srl_err_en;                         /*!< 1: Enable the slew rate errors to trigger an alarm when the measured slew rate is higher than a threshold. 0: will not trigger a ramp down. */
    bool                            avg_pwr_err_en;                     /*!< 1: Enable the average power to trigger an alarm when measured power is higher than a threshold. 0: will not trigger a ramp down. */
    bool    	                    auto_dyn_recfg_en;                  /*!< 1: dyn reconfig triggers ramps. 0: dyn reconfig would not trigger the ramps. */
    bool     	                    jesd_err_en;                        /*!< 1: Enable the JESD errors to trigger an alarm. */
    bool                            jesd_err_auto_cfg_en;               /*!< Allow the JESD errors to be automatically routed to different RAMPS based on JESD parameters. */
    bool                            data_ready_auto_cfg_en;             /*!< Allow the data_ready to be automatically routed to different RAMPS based on JESD parameters. */
    bool                            zero_flush_clock_stable_rise_en;    /*!< Enable Zero Flushing On Rising Edge of Clock Stable. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_dyn_recfg_ramp_up_en;    /*!< Enable Zero Flushing Before Ramp Up Via Dynamic Reconfiguration. PAPROT SM Must be Enabled. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_txen_fall_en;            /*!< Enable Zero Flushing on Falling Edge of TXEN. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_txen_rise_en;            /*!< Enable Zero Flushing on Rising Edge of TXEN. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_start_en;                /*!< Enable Zero Flushing After Initial Power Up and Clock Synchronization Sequence. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            drive_sm_by_dig_en;                 /*!< 1: dig_en from txen power control is driving the PA protect SM. 0: TXEN from pins will be driving the SM. */
    bool                            hold_sample_en;                     /*!< 1: Enable the function that will allow user to hold the last good sample. */
    bool                            skip_ramp_up;                       /*!< 1: The ramp up event in the SM will be disabled i.e. skip the ramp up state and jump to operation state. */
    bool                            skip_ramp_down;                     /*!< 1: The ramp down event in the SM will be disabled i.e. skip the ramp down state and jump to operation state. */
    bool                            ramp_up_timer_en;                   /*!< Enable the ramp up timer. */
    bool                            ramp_dwn_timer_en;                  /*!< Enable the ramp down timer. */
    uint16_t                        zero_flush_timer;                   /*!< 13Bits Zero Flush timer */
    uint16_t                        ramp_down_timer_val;                /*!< 16Bits Ramp Down timer */
    uint16_t                        ramp_up_timer_val;                  /*!< 16Bits Ramp Up timer */
    adi_apollo_paprot_zf_sel_e      dp_zero_flush_sel_0;                /*!< Select Zero Flush from PA to FDUC Mapping. */
    adi_apollo_paprot_zf_sel_e      dp_zero_flush_sel_1;                /*!< Select Zero Flush from PA to FDUC Mapping. */
    adi_apollo_paprot_jesd_dr_sel_e data_ready_sel_ramp;                /*!< The select bit for mapping data_ready[1:0] to the different RAMP paths. */
    adi_apollo_paprot_jesd_dr_sel_e jesd_err_sel_ramp;                  /*!< The select bit for mapping jesd_err[1:0] to the different RAMP paths. */
} adi_apollo_paprot_sm_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief PAProt Ramp Control Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_ramp_cfg {
    bool    gain_clk_en;                    /*!< True to enable when TxEN is low */
    bool    zero_detect_en;                 /*!< Enable the ramp up only when non-zero data is detected and this bit is enabled. */
    bool    force_ramps_dyn_recfg_en;       /*!< Enable the ramps from dynamic reconfig controller bypassing PA protect SM. */
    bool    force_ramp_up_en;               /*!< Enable the manual forcing of a ramp up. */
    bool    force_ramp_dwn_en;              /*!< Enable the manual forcing of a ramp down. */
    bool    enable_gain;                    /*!< True to enable the gain */
    bool    enable_ramp;                    /*!< Enable the ramping up and down of the gain steps. When this bit is set to 0 the multiplier gain value is fixed to the programmed GAIN_VALUE. When the ramp_en is asserted the gain block is automatically enabled. */
    bool    trig_ramp_together;             /*!< Enable simultaneous ramp up and ramp down of IQ pairs. */
    uint8_t gain_inc_step;                  /*!< 8Bits for the gain increase step for PA protection RAMP up process. */
    uint8_t gain_dec_step;                  /*!< 8Bits for the gain decrease step for PA protection RAMP down process. */
    uint8_t gain_max_val;                   /*!< 8Bits for the maximum gain value that data will be multiplied w/ after ramp up is completed ie: DAC = data*max_gain. */
} adi_apollo_paprot_ramp_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief PAProt Slew Rate Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_sr_cfg {
    bool        sr_clk_en;                  /*!< True to enable when TxEN is low */
    bool        sr_err_self_clr_en;         /*!< True to enable the slew rate detector error self clear */
    bool        sr_calculation_en;          /*!< True to enable slew rate calculations */
    bool        slew_rate_err_irq_en;       /*!< True to enable slew rate error IRQ */
    uint8_t     sr_path_sel;                /*!< 5Bits for the which of the 32 calculated values are read back */
    uint32_t    sr_threshold;               /*!< 17Bits for the slew rate threshold */
} adi_apollo_paprot_sr_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief PA Protect Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_cfg {
    bool                            pa_clkdiv_en;       /*!< True to enable */
    adi_apollo_paprot_pwr_cfg_t     paprot_pwr_cfg;     /*!< PAProt Power Detector Configuration */
    adi_apollo_paprot_sm_cfg_t      paprot_sm_cfg;      /*!< PAProt State Machine Configuration */
    adi_apollo_paprot_ramp_cfg_t    paprot_ramp_cfg;    /*!< PAProt Ramp Up/Down Configuration */
    adi_apollo_paprot_sr_cfg_t      paprot_sr_cfg;      /*!< PAProt Slew Rate Configuration */
} adi_apollo_paprot_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Rx/Tx Enable Sel Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxtxen_cfg {
    bool        enable_spien;           /*!< Enable SPI Control Bit for Respective Enable. 0 == Disable, 1 == Enable */
    bool        enable_spi;             /*!< SPI Control Bit Instead of GPIO for Respective Enable. This Value is Used Only If Respective ENABLE_SPIEN Bit is Set to 1. */
    bool        enable_pol;             /*!< Polarity Selection for Respective Enable. 0 == Active High, 1 == Active Low */
    uint8_t     slice_sel;              /*!< 4bits - Select for Slice by Respective Enable. Each Bit Corresponds to Separate Slice. */
    uint8_t     pfilt_sel;              /*!< 2bits - Select for PFILT Clock Gating by Respective Enable. Each Bit Corresponds to Separate PFILT. */
    uint8_t     cdrc_sel;               /*!< 4bits - Select for CDRC Clock Gating by Respective Enable. Each Bit Corresponds to Separate CDRC. */
    uint8_t     fdrc_sel;               /*!< 8bits - Select for FDRC Clock Gating by Respective Enable. Each Bit Corresponds to Separate FDRC. */
    uint8_t     cfir_sel;               /*!< 2bits - Select for CFIR Clock Gating by Respective Enable. Each Bit Corresponds to Separate CFIR. */
    uint8_t     fsrc_sel;               /*!< 2bits - Select for FSRC Clock Gating by Respective Enable. Each Bit Corresponds to Separate FSRC. */
    uint8_t     jesd_link_sel;          /*!< 2bits - Select for JESD Link Clock Gating by Respective Enable. Each Bit Corresponds to Separate JESD Link. */
    uint16_t    jesd_phy_sel;           /*!< 12bits - Select for JRX/JTX PHY Power Down by Respective Enable. Each Bit Corresponds to Separate JRX/JTX PHY. */
    bool        modsw_sel;              /*!< 0 == Not Selected, 1 == Selected */
    uint8_t     invsinc_sel;            /*!< 4bits - Select for INVSINC Clock Gating by Respective Enable. Each Bit Corresponds to Separate INVSINC. */
    uint8_t     gain_sel;               /*!< 4bits - Select for GAIN Control by Respective Enable. Each Bit Corresponds to Separate GAIN. */
    uint8_t     srd_sel;                /*!< 4bits - Select for Slew Rate Det by Respective Enable. Each Bit Corresponds to Separate SRD. */
} adi_apollo_rxtxen_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Rx/Tx Enable Power Sel Options
 */
typedef enum adi_apollo_rxtxen_pwr_sel {
    ADI_APOLLO_RXTXEN_PWR_SEL_0 = 0u,       /*!< rxtxen[0] to pin_rxtxen */
    ADI_APOLLO_RXTXEN_PWR_SEL_1 = 1u,       /*!< rxtxen[1] to pin_rxtxen */
    ADI_APOLLO_RXTXEN_PWR_SEL_2 = 2u,       /*!< rxtxen[2] to pin_rxtxen */
    ADI_APOLLO_RXTXEN_PWR_SEL_3 = 3u        /*!< rxtxen[3] to pin_rxtxen */
} ADI_APOLLO_PACK_ENUM adi_apollo_rxtxen_pwr_sel_e;


/**
 * \brief Rx/Tx Enable Power Rise Options
 */
typedef enum adi_apollo_rxtxen_pwr_rise {
    ADI_APOLLO_RXTXEN_PWR_RISE_E0   = 0u,   /*!< Signal to rise on E0 */
    ADI_APOLLO_RXTXEN_PWR_RISE_E1   = 1u,   /*!< Signal to rise on E1 */
    ADI_APOLLO_RXTXEN_PWR_RISE_E2   = 2u,   /*!< Signal to rise on E2 */
    ADI_APOLLO_RXTXEN_PWR_RISE_E3   = 3u    /*!< Signal to rise on E3 */
} ADI_APOLLO_PACK_ENUM adi_apollo_rxtxen_pwr_rise_e;


/**
 * \brief Rx/Tx Enable Power Fall Options
 */
typedef enum adi_apollo_rxtxen_pwr_fall {
    ADI_APOLLO_RXTXEN_PWR_FALL_E4   = 0u,   /*!< Signal to fall on E4 */
    ADI_APOLLO_RXTXEN_PWR_FALL_E5   = 1u,   /*!< Signal to fall on E5 */
    ADI_APOLLO_RXTXEN_PWR_FALL_E6   = 2u,   /*!< Signal to fall on E6 */
    ADI_APOLLO_RXTXEN_PWR_FALL_E7   = 3u    /*!< Signal to fall on E7 */
} ADI_APOLLO_PACK_ENUM adi_apollo_rxtxen_pwr_fall_e;


/**
 * \brief Rx/Tx Enable Power Control Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxtxen_pwr_cfg {
    adi_apollo_rxtxen_pwr_sel_e     rxtxen_sel;     /*!< These two bits will select which GPIO txen[3:0] bits is selected for the power control input signal.  */
    bool                            spi_rxtxen;     /*!< Setting for Rx/Tx Enable. Only in use if spi_rxtxen_ena == 1. 0 == Disable, 1 == Enable. */
    bool                            spi_rxtxen_ena; /*!< 0 == Use GPIO for Rx/Tx Enable. 1 == Use spi_rxtxen for Rx/Tx Enable. */
    bool                            sm_en;          /*!< 0 == State machine will be bypassed and dac_stanby=~Rx/TxEN, dig_en=Rx/TxEN and pa_en=Rx/TxEN. 1 == Rx/Tx en powerup control state machine is enabled. */
    bool                            sel_cnt_rate;   /*!< Select the txen clock rate to be used. This bit will add the option to run the TXEN power control at a slower rate. Slowing down the clock rate of the state machine counters will allow for a longer wait time between events from E0 to E7. This may be needed in case the datapath becomes longer. A setting of 1 will use the slower clock which is Fs/256 for 4T4R and Fs/128 for 8T8R, while the default of zero will select Fs/32 for 4T4R and Fs/16 for 8T8R. */
    adi_apollo_rxtxen_pwr_fall_e    conv_fall;      /*!< These bits define the falling edges of conv_en signal with respect to Rx/TxEN falling edges. Note conv_standby is inverted from conv_en. */
    adi_apollo_rxtxen_pwr_rise_e    conv_rise;      /*!< These bits define the rising edges of conv_en signal with respect to Rx/TxEN rising edges. Note conv_standby is inverted from conv_en. */
    adi_apollo_rxtxen_pwr_fall_e    dig_fall;       /*!< These bits define the falling edges of dig_en signal with respect to TxEN falling edges. */
    adi_apollo_rxtxen_pwr_rise_e    dig_rise;       /*!< These bits define the rising edges of dig_en signal with respect to TxEN rising edges. */
    adi_apollo_rxtxen_pwr_fall_e    pa_fall;        /*!< These bits define the falling edges of pa_en signal with respect to TxEN falling edges. */
    adi_apollo_rxtxen_pwr_rise_e    pa_rise;        /*!< These bits define the rising edges of pa_en signal with respect to TxEN rising edges. */
    uint8_t                         count_maxa;     /*!< 8bits - Maximum programmed counter value from E0 to E1. */
    uint8_t                         count_maxb;     /*!< 8bits - Maximum programmed counter value from E1 to E2. */
    uint8_t                         count_maxc;     /*!< 8bits - Maximum programmed counter value from E2 to E3. */
    uint8_t                         count_maxd;     /*!< 8bits - Maximum programmed counter value from E4 to E5. */
    uint8_t                         count_maxe;     /*!< 8bits - Maximum programmed counter value from E5 to E6. */
    uint8_t                         count_maxf;     /*!< 8bits - Maximum programmed counter value from E6 to E7. */
} adi_apollo_rxtxen_pwr_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief TX path configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_txpath {
    adi_apollo_cduc_cfg_t       tx_cduc[ADI_APOLLO_CDUCS_PER_SIDE];             /*!< Coarse digital up converter config */
    adi_apollo_fduc_cfg_t       tx_fduc[ADI_APOLLO_FDUCS_PER_SIDE];             /*!< Fine digital up converter config */
    adi_apollo_pfilt_cfg_t      tx_pfilt[ADI_APOLLO_PFILTS_PER_SIDE];           /*!< Programmable filters */
    adi_apollo_cfir_cfg_t       tx_cfir[ADI_APOLLO_CFIRS_PER_SIDE];             /*!< Complex FIRs */
    adi_apollo_fsrc_cfg_t       tx_fsrc;                                        /*!< Fractional sampling rate converters */
    adi_apollo_txpath_misc_t    tx_mux_summer_xbar;                             /*!< Summers. multiplexers, crossbar config */
    adi_apollo_dyn_recfg_cfg_t  tx_dyn_recfg;                                   /*!< Dynamic Reconfig Static settings */
    adi_apollo_paprot_cfg_t     tx_pa_prot[ADI_APOLLO_CDUC_PATHS_PER_SIDE];     /*!< PA Protect configuration */
    bool                        inv_sinc_en[ADI_APOLLO_CDUC_PATHS_PER_SIDE];    /*!< Inverse sinc enable */
    adi_apollo_rxtxen_cfg_t     tx_enable[ADI_APOLLO_RXTX_ENABLES_PER_SIDE];    /*!< Tx Enable Clock Gating config */
    adi_apollo_rxtxen_pwr_cfg_t tx_power[ADI_APOLLO_RXTX_POWER_CTRL_PER_SIDE];  /*!< Tx Enable Power config */
} adi_apollo_txpath_t;
ADI_APOLLO_PACK_FINISH

/*!
* Enumerates the values that two bit fields adc_0_mux_sel, bits 0 and 1 in
* register adc_mux_sel and adc_1_mux_sel, bits 2 and 3 in register adc_mux_sel
* can assume.
* For the case of 4T4R, adc_1_mux_sel does not have any impact, since there are
* only two ADCs on each side. For 8T8R, both of these fields are used and each
* of these fields controls one crossbar.
*/
typedef enum adi_apollo_rx_mux0_sel {
    ADI_APOLLO_4T4R_CB_OUT_0_FROM_ADC0  = 0u,   /*!< 4T4R's first (side A or B) adc_mux_sel=00000000  */
    ADI_APOLLO_4T4R_CB_OUT_0_FROM_ADC1  = 1u,   /*!< 4T4R's first (side A or B) adc_mux_sel=00000001  */
    ADI_APOLLO_4T4R_CB_OUT_1_FROM_ADC1  = 0u,   /*!< 4T4R's second (side A or B) adc_mux_sel=00000000  */
    ADI_APOLLO_4T4R_CB_OUT_1_FROM_ADC0  = 1u,   /*!< 4T4R's second (side A or B) adc_mux_sel=00000001  */
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC0    = 0u,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC1    = 2u,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC2    = 1u,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC3    = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux0_sel_e;

typedef enum adi_apollo_rx_mux0_idx {
    ADI_APOLLO_4T4R_CB_OUT0     = 0u,
    ADI_APOLLO_4T4R_CB_OUT1     = 1u,
    ADI_APOLLO_8T8R_CB_OUT0     = 0u,
    ADI_APOLLO_8T8R_CB_OUT2     = 1u,
    ADI_APOLLO_8T8R_CB_OUT1     = 2u,
    ADI_APOLLO_8T8R_CB_OUT3     = 3u,
    ADI_APOLLO_RX_MUX0_NUM_4T4R = 2u,
    ADI_APOLLO_RX_MUX0_NUM_8T8R = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux0_idx_e;


/**
 * \brief Rx Coarse fine crossbar (MUX2) configuration
 */
typedef enum adi_apollo_rx_mux2_sel {
    /* values for FDDC0 (and FDDC4 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F0         = 0u,   /*!< 4T4R: CDDC 0 goes to FDDC 0 */
    ADI_APOLLO_CB_4T4R_C1_TO_F0         = 4u,   /*!< 4T4R: CDDC 1 goes to FDDC 0 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F0_F4   = 0u,   /*!< 8T8R: CDDC 0 goes to FDDC 0 and CDDC 2 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F0_F4   = 1u,   /*!< 8T8R: CDDC 2 goes to FDDC 0 and CDDC 0 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C0_TO_F0_F4      = 2u,   /*!< 8T8R: CDDC 0 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C2_TO_F0_F4      = 3u,   /*!< 8T8R: CDDC 2 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F0_F4   = 4u,   /*!< 8T8R: CDDC 1 goes to FDDC 0 and CDDC 3 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F0_F4   = 5u,   /*!< 8T8R: CDDC 3 goes to FDDC 0 and CDDC 1 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C1_TO_F0_F4      = 6u,   /*!< 8T8R: CDDC 1 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C3_TO_F0_F4      = 7u,   /*!< 8T8R: CDDC 3 goes to both FDDC 0 and FDDC 4 */

    /* values for FDDC1 (and FDDC5 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F1         = 0u,   /*!< 4T4R: CDDC 0 goes to FDDC 1 */
    ADI_APOLLO_CB_4T4R_C1_TO_F1         = 4u,   /*!< 4T4R: CDDC 1 goes to FDDC 1 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F1_F5   = 0u,   /*!< 8T8R: CDDC 0 goes to FDDC 1 and CDDC 2 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F1_F5   = 1u,   /*!< 8T8R: CDDC 2 goes to FDDC 1 and CDDC 0 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C0_TO_F1_F5      = 2u,   /*!< 8T8R: CDDC 0 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C2_TO_F1_F5      = 3u,   /*!< 8T8R: CDDC 2 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F1_F5   = 4u,   /*!< 8T8R: CDDC 1 goes to FDDC 1 and CDDC 3 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F1_F5   = 5u,   /*!< 8T8R: CDDC 3 goes to FDDC 1 and CDDC 1 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C1_TO_F1_F5      = 6u,   /*!< 8T8R: CDDC 1 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C3_TO_F1_F5      = 7u,   /*!< 8T8R: CDDC 3 goes to both FDDC 1 and FDDC 5 */

    /* values for FDDC2 (and FDDC6 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F2         = 0u,   /*!< 4T4R: CDDC 0 goes to FDDC 2 */
    ADI_APOLLO_CB_4T4R_C1_TO_F2         = 4u,   /*!< 4T4R: CDDC 1 goes to FDDC 2 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F2_F6   = 0u,   /*!< 8T8R: CDDC 0 goes to FDDC 2 and CDDC 2 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F2_F6   = 1u,   /*!< 8T8R: CDDC 2 goes to FDDC 2 and CDDC 0 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C0_TO_F2_F6      = 2u,   /*!< 8T8R: CDDC 0 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C2_TO_F2_F6      = 3u,   /*!< 8T8R: CDDC 2 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F2_F6   = 4u,   /*!< 8T8R: CDDC 1 goes to FDDC 2 and CDDC 3 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F2_F6   = 5u,   /*!< 8T8R: CDDC 3 goes to FDDC 2 and CDDC 1 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C1_TO_F2_F6      = 6u,   /*!< 8T8R: CDDC 1 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C3_TO_F2_F6      = 7u,   /*!< 8T8R: CDDC 3 goes to both FDDC 2 and FDDC 6 */

    /* values for FDDC3 (and FDDC7 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F3         = 0u,   /*!< 4T4R: CDDC 0 goes to FDDC 3 */
    ADI_APOLLO_CB_4T4R_C1_TO_F3         = 4u,   /*!< 4T4R: CDDC 1 goes to FDDC 3 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F3_F7   = 0u,   /*!< 8T8R: CDDC 0 goes to FDDC 3 and CDDC 2 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F3_F7   = 1u,   /*!< 8T8R: CDDC 2 goes to FDDC 3 and CDDC 0 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C0_TO_F3_F7      = 2u,   /*!< 8T8R: CDDC 0 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C2_TO_F3_F7      = 3u,   /*!< 8T8R: CDDC 2 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F3_F7   = 4u,   /*!< 8T8R: CDDC 1 goes to FDDC 3 and CDDC 3 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F3_F7   = 5u,   /*!< 8T8R: CDDC 3 goes to FDDC 3 and CDDC 1 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C1_TO_F3_F7      = 6u,   /*!< 8T8R: CDDC 1 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C3_TO_F3_F7      = 7u    /*!< 8T8R: CDDC 3 goes to both FDDC 3 and FDDC 7 */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux2_sel_e;

typedef enum adi_apollo_rx_mux2_idx {
    ADI_APOLLO_4T4R_FDDC0   = 0u,
    ADI_APOLLO_4T4R_FDDC1   = 1u,
    ADI_APOLLO_4T4R_FDDC2   = 2u,
    ADI_APOLLO_4T4R_FDDC3   = 3u,
    ADI_APOLLO_8T8R_FDDC0_4 = 0u,
    ADI_APOLLO_8T8R_FDDC1_5 = 1u,
    ADI_APOLLO_8T8R_FDDC2_6 = 2u,
    ADI_APOLLO_8T8R_FDDC3_7 = 3u,
    ADI_APOLLO_RX_MUX2_NUM  = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux2_idx_e;


/**
 * \brief Rx Path Misc Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxpath_misc {
    adi_apollo_rx_mux0_sel_e    mux0_out_adc_sel[ADI_APOLLO_RX_MUX0_NUM_8T8R];  /*!< ADC to CB_out multiplexer (RX MUX0) select. Register name for programming is adc_mux_sel */
    adi_apollo_rx_mux2_sel_e    mux2_fddc_input_sel[ADI_APOLLO_RX_MUX2_NUM];    /*!< Coarse to fine crossbar (RX MUX2) config */
    bool                        low_samp_en;                                    /*!< Rx_Misc.low_samp, TRUE: enable low sampling */
} adi_apollo_rxpath_misc_t;
ADI_APOLLO_PACK_FINISH

typedef enum adi_apollo_rx_tmode_sel {
    /*
        dformat_tmode_sel_0[0] = 4T4R Converter	0 enable/8T8T Converter 0/1 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[1] = 4T4R Converter 1 enable/8T8T Converter 2/3 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[2] = 4T4R Converter 2 enable/8T8T Converter 4/5 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[3] = 4T4R Converter 3 enable/8T8T Converter 6/7 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[4] = 4T4R Converter 4 enable/8T8T Converter 8/9 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[5] = 4T4R Converter 5 enable/8T8T Converter 10/11 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[6] = 4T4R Converter 6 enable/8T8T Converter 12/13 enable:
            1'b0: disable
            1'b1 enable
        dformat_tmode_sel_0[7] = 4T4R Converter 7 enable/8T8T Converter 14/15 enable:
            1'b0: disable
            1'b1 enable
    */
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_DISABLE   = 0u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_0_ENABLE  = 1u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_1_ENABLE  = 2u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_2_ENABLE  = 4u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_3_ENABLE  = 8u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_4_ENABLE  = 16u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_5_ENABLE  = 32u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_6_ENABLE  = 64u,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_7_ENABLE  = 128u,

    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_DISABLE   = 0u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_01_ENABLE = 1u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_23_ENABLE = 2u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_45_ENABLE = 4u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_67_ENABLE = 8u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_89_ENABLE = 16u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_AB_ENABLE = 32u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_CD_ENABLE = 64u,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_EF_ENABLE = 128u,
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_sel_e;

/**
 * \brief Test Mode Type Selection Enumeration
 */
typedef enum adi_apollo_rx_tmode_type_sel {
    ADI_APOLLO_TMODE_TYPE_SEL_NORM      = 0u,   /*!< Normal Mode: TMode off */
    ADI_APOLLO_TMODE_TYPE_SEL_MIDSCALE  = 1u,   /*!< Midscale */
    ADI_APOLLO_TMODE_TYPE_SEL_POS_FS    = 2u,   /*!< Positive Fullscale */
    ADI_APOLLO_TMODE_TYPE_SEL_NEG_FS    = 3u,   /*!< Negative Fullscale */
    ADI_APOLLO_TMODE_TYPE_SEL_ACB       = 4u,   /*!< Alternating Checkerboard */
    ADI_APOLLO_TMODE_TYPE_SEL_PN23      = 5u,   /*!< Pseudo Random Number 23 */
    ADI_APOLLO_TMODE_TYPE_SEL_PN9       = 6u,   /*!< Pseudo Random Number 9 */
    ADI_APOLLO_TMODE_TYPE_SEL_WT        = 7u,   /*!< Word Toggle */
    ADI_APOLLO_TMODE_TYPE_SEL_USR       = 8u,   /*!< User Data Pattern */
    ADI_APOLLO_TMODE_TYPE_SEL_PN7       = 9u,   /*!< Pseudo Random Number 7 */
    ADI_APOLLO_TMODE_TYPE_SEL_PN15      = 10u,  /*!< Pseudo Random Number 15 */
    ADI_APOLLO_TMODE_TYPE_SEL_PN31      = 11u,  /*!< Pseudo Random Number 31 */
    ADI_APOLLO_TMODE_TYPE_SEL_RAMP      = 15u   /*!< Sample Ramp */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_type_sel_e;

/**
 * \brief Rx Test Mode Resolution Enumeration
 */
typedef enum adi_apollo_rx_tmode_res {
    ADI_APOLLO_RX_TMODE_RES_16B = 0u,   /*!< 16 Bit */
    ADI_APOLLO_RX_TMODE_RES_15B = 1u,   /*!< 15 Bit */
    ADI_APOLLO_RX_TMODE_RES_14B = 2u,   /*!< 14 Bit */
    ADI_APOLLO_RX_TMODE_RES_13B = 3u,   /*!< 13 Bit */
    ADI_APOLLO_RX_TMODE_RES_12B = 4u    /*!< 12 Bit */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_res_e;


/**
 * \brief Dformat Test Mode Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_tmode_cfg {
    adi_apollo_rx_tmode_sel_e       sel;            /*!< Tmode Data Routing Selection  */
    adi_apollo_rx_tmode_type_sel_e  type_sel;       /*!< Tmode Data Type Selection */
    bool                            pn_force_rst;   /*!< True to reset pn gen */
    bool                            flush;          /*!< True to enable sync reset of I path */
    bool                            usr_pat_sel;    /*!< Tmode Single or Continuous Pattern Selection */
    adi_apollo_rx_tmode_res_e       res;            /*!< Tmode Resolution */
} adi_apollo_tmode_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Rate Match Fifo Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rm_fifo_cfg {
    bool    invalid_en;                 /*!< True to enable invalid sample insertion */
    bool    sample_repeat_en;           /*!< True to enable sample repeat */
} adi_apollo_rm_fifo_cfg_t;
ADI_APOLLO_PACK_FINISH

/**
 * \brief RX Data Format Selection Enumeration
 */
typedef enum adi_apollo_rx_dformat_sel {
    ADI_APOLLO_RX_DFORMAT_SEL_2COMP     = 0u,   /*!< 2's Complement */
    ADI_APOLLO_RX_DFORMAT_SEL_OFF_BIN   = 1u,   /*!< Offset Binary */
    ADI_APOLLO_RX_DFORMAT_SEL_GRAY      = 2u    /*!< Gray Code */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_dformat_sel_e;

/**
 * \brief Rx Data Format Resolution Enumeration
 */
typedef enum adi_apollo_rx_dformat_res {
    ADI_APOLLO_RX_DFORMAT_RES_16B   = 0u,   /*!< 16 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_15B   = 1u,   /*!< 15 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_14B   = 2u,   /*!< 14 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_13B   = 3u,   /*!< 13 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_12B   = 4u,   /*!< 12 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_11B   = 5u,   /*!< 11 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_10B   = 6u,   /*!< 10 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_9B    = 7u,   /*!< 9 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_8B    = 8u,   /*!< 8 Bit */
    ADI_APOLLO_RX_DFORMAT_RES_7B    = 9u    /*!< 7 Bit */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_dformat_res_e;


/**
 * \brief Dformat Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dformat_cfg {
    bool                        inv;            /*!< True to invert ADC sample data */
    adi_apollo_rx_dformat_sel_e sel;            /*!< Dformat Output Data Selection */
    bool                        ddc_dither_en;  /*!< True to enable ddc dither */
    adi_apollo_rx_dformat_res_e res;            /*!< Dformat Resolution */
    adi_apollo_tmode_cfg_t      tmode;          /*!< Tmode config */
    adi_apollo_rm_fifo_cfg_t    rm_fifo;        /*!< Rate Match Fifo config */
} adi_apollo_dformat_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Valid SMON JESD Link Selections
 */
typedef enum adi_apollo_rx_smon_jlink_sel {
    ADI_APOLLO_RX_SMON_JLINK_SEL0   = 0,
    ADI_APOLLO_RX_SMON_JLINK_SEL1   = 1u
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_jlink_sel_e;


/**
 * \brief Valid SMON Framer Modes
 */
typedef enum adi_apollo_rx_smon_framer_mode {
    ADI_APOLLO_RX_SMON_FRAMER_MODE_10B  = 0u,
    ADI_APOLLO_RX_SMON_FRAMER_MODE_5B   = 1u,
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_framer_mode_e;


/**
 * \brief Valid SMON Sync Next Modes
 */
typedef enum adi_apollo_rx_smon_sync {
    ADI_APOLLO_RX_SMON_SYNC_CONT    = 0u,   /*!< Continuous Mode. */
    ADI_APOLLO_RX_SMON_SYNC_NEXT    = 1u    /*!< Next Synchroniztion Mode. */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_sync_e;


/**
 * \brief SMON Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_smon_cfg {
    uint32_t                            period;         /*!< 32Bits Signal Monitor Period. */
    bool                                gpio_en;        /*!< Signal Monitor GPIO Enable. If set, allows GPIO output of peak indication. Peak_en also has to be enabled for this feature. */
    bool                                peak_en;        /*!< Signal Monitor Peak Detector Enable. False: Peak Detector Disabled True: Peak Detector Enabled */
    adi_apollo_rx_smon_jlink_sel_e      jlink_sel;      /*!< SMON JESD Link Selection. */
    bool                                sframer_en;     /*!< Signal Monitor Serial Framer Enable. */
    adi_apollo_rx_smon_framer_mode_e    sframer_mode;   /*!< Signal Monitor Serial Framer Mode Selection. */
    bool                                sync_en;        /*!< SMON Synchronization Enable. */
    adi_apollo_rx_smon_sync_e           sync_next;      /*!< SMON Next Synchronization Mode. */
    uint16_t                            thresh_high;    /*!< 11Bits Signal Monitor GPIO Higher Threshold. */
    uint16_t                            thresh_low;     /*!< 11Bits Signal Monitor GPIO Lower Threshold. */
} adi_apollo_smon_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Buffer Memory Sample RAM Access Speed Mode Enumeration
 */
typedef enum adi_apollo_bmem_sram_speed_mode {
    ADI_APOLLO_BMEM_SRAM_SPEED_MODE_SLOW    = 0u,   /*!< Slow Mode for reading out. */
    ADI_APOLLO_BMEM_SRAM_SPEED_MODE_FAST    = 1u    /*!< Fast Mode for capturing and playing from/to datapath. */
} ADI_APOLLO_PACK_ENUM adi_apollo_bmem_sram_speed_mode_e;



/**
 * \brief Buffer Memory Mode of Operation Enumeration
 */
typedef enum adi_apollo_bmem_mode {
    ADI_APOLLO_BMEM_MODE_CAPTURE    = 0u,   /*!< Capture Mode. BMEM will capture data from the datapath. */
    ADI_APOLLO_BMEM_MODE_DELAY      = 1u,   /*!< Delay Mode. BMEM will act as a delay buffer for the datapath. */
    ADI_APOLLO_BMEM_MODE_AWG        = 2u    /*!< Arbitrary Waveform Generation Mode. BMEM will play the loaded vector to the datapath. */
} ADI_APOLLO_PACK_ENUM adi_apollo_bmem_mode_e;



/**
 * \brief Buffer Memory Hop Delay Selection Mode Enumeration
 */
typedef enum adi_apollo_bmem_hop_dly_sel_mode {
    ADI_APOLLO_BMEM_HOP_DLY_SEL_MODE_TRIG = 0u,         /*!< Hop Delay via Trigger. */
    ADI_APOLLO_BMEM_HOP_DLY_SEL_MODE_GPIO = 1u          /*!< Hop Delay via GPIO. */
} ADI_APOLLO_PACK_ENUM adi_apollo_bmem_hop_dly_sel_mode_e;



/**
 * \brief Buffer Memory 8t8r Capture Mask Mode Enumeration
 */
typedef enum adi_apollo_bmem_8t8r_cap_mask {
    ADI_APOLLO_BMEM_8T8R_CAP_MASK_ALL_UNMASKED  = 0u,   /*!< All Converters are Unmasked. */
    ADI_APOLLO_BMEM_8T8R_CAP_MASK_LOWER_MASKED  = 1u,   /*!< Lower Converters are Masked. */
    ADI_APOLLO_BMEM_8T8R_CAP_MASK_UPPER_MASKED  = 2u,   /*!< Upper Converters are Masked. */
    ADI_APOLLO_BMEM_8T8R_CAP_MASK_ALL_MASKED    = 3u    /*!< All Converters are Masked. */
} ADI_APOLLO_PACK_ENUM adi_apollo_bmem_8t8r_cap_mask_e;


/**
 * \brief BMEM Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_bmem_cfg {
    adi_apollo_bmem_sram_speed_mode_e   fast_nslow_mode;     /*!< SRAM Access Mode. */
    bool                                bmem_shut_down;	    /*!< True to set SRAM in Shut Down Mode. False to disable Shut Down Mode. */
    bool                                bmem_sleep;	        /*!< True to set SRAM in Sleep Mode. False to disable Sleep Mode. */
    adi_apollo_bmem_mode_e              bmem_mode;	        /*!< BMEM Mode of Operation. Capture, Delay, or AWG. */
    bool                                bmem_en;	            /*!< True to enable BMEM. */
    adi_apollo_bmem_hop_dly_sel_mode_e  hop_dly_sel_mode;	/*!< Hop Delay Selection Mode. */
    adi_apollo_bmem_8t8r_cap_mask_e     bmem_8t8r_cap_mask;  /*!< Disable Capture for none, some, or all converters in 8T8R Mode. */
    bool                                ramclk_ph_dis;	    /*!< True to Disable SRAM Clock Phasing */
    bool                                parity_check_en;	    /*!< True to Enable Parity Check on Data Read from SRAM. */
    bool                                trig_mode_en;        /*!< True to Enable Hopping via Trigger, False to use start signal. */
    bool                                trig_mode_sclr_en;	/*!< True to Self Clear TRIG_MODE Bit After a Trigger Event. */
    uint16_t                            sample_dly;          /*!< Sample Delay Value in Delay Mode. This Value is Used Only If TRIG_MODE Bit is Set to 0. */
    uint16_t                            hop_delay0;          /*!< Hop Delay Value 0 in Delay Mode.
                                                                This Value is Used Only If TRIG_MODE Bit or HOP_DLY_SEL_MODE Bit is Set to 1. If HOP_DLY_SEL_MODE Bit is 1, Hop Delay Value is Selected Based on I_HOP_DLY_SEL Inputs.
                                                                If TRIG_MODE Bit is 1, When Trigger Comes from Trigger Master, Hop Delay Value Switches from 0->1->2->3->0 */
    uint16_t                            hop_delay1;          /*!< Hop Delay Value 1 in Delay Mode. */
    uint16_t                            hop_delay2;          /*!< Hop Delay Value 2 in Delay Mode. */
    uint16_t                            hop_delay3;          /*!< Hop Delay Value 3 in Delay Mode. */
} adi_apollo_bmem_cfg_t;
ADI_APOLLO_PACK_FINISH



/**
 * \brief RX path configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxpath {
    adi_apollo_cddc_cfg_t       rx_cddc[ADI_APOLLO_CDDCS_PER_SIDE];             /*!< Coarse digital down converter config */
    adi_apollo_fddc_cfg_t       rx_fddc[ADI_APOLLO_FDDCS_PER_SIDE];             /*!< Fine digital down converter config */
    adi_apollo_pfilt_cfg_t      rx_pfilt[ADI_APOLLO_PFILTS_PER_SIDE];           /*!< Programmable filters */
    adi_apollo_cfir_cfg_t       rx_cfir[ADI_APOLLO_CFIRS_PER_SIDE];             /*!< Complex FIRs */
    adi_apollo_fsrc_cfg_t       rx_fsrc;                                        /*!< Fractional sampling rate converters */
    adi_apollo_rxpath_misc_t    rx_mux_splitter_xbar;                           /*!< Summers, multiplexers, crossbar config */
    adi_apollo_dyn_recfg_cfg_t  rx_dyn_recfg;                                   /*!< Dynamic Reconfig Static settings */
    adi_apollo_dformat_cfg_t    rx_dformat[ADI_APOLLO_JESD_LINKS];              /*!< Dformat, Tmode, and RMFifo config */
    adi_apollo_smon_cfg_t       rx_smon[ADI_APOLLO_SMONS_PER_SIDE];             /*!< SMON config */
    adi_apollo_rxtxen_cfg_t     rx_enable[ADI_APOLLO_RXTX_ENABLES_PER_SIDE];    /*!< Rx Enable Clock Gating config */
    adi_apollo_rxtxen_pwr_cfg_t rx_power[ADI_APOLLO_RXTX_POWER_CTRL_PER_SIDE];  /*!< Rx Enable Power config */
    adi_apollo_bmem_cfg_t       rx_bmem[ADI_APOLLO_BMEMS_PER_SIDE];             /*!< BMEM Config*/
} adi_apollo_rxpath_t;
ADI_APOLLO_PACK_FINISH



/**
 * \brief Clock PLL Config
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_clk_pll_cfg {
    uint32_t    loop_bandwidth;         /*!< PLL Loop bandwidth  */
    uint8_t     phase_margin;           /*!< PLL Phase Margin  */
    uint8_t     div_range;              /*!< Serdes only Div Range  */
    uint8_t     div2;                   /*!< Serdes only Div 2  */
    uint8_t     power;                  /*!< PLL Power setting  */
    uint8_t     ref_clk_div;            /*!< PLL ref clock divider  */
    uint8_t     i_bleed_en;             /*!< PLL bleed ramp enable  */
    uint32_t    feedback_int;           /*!< Integer portion of feedback factor */
    uint32_t    feedback_frac;          /*!< Fractional portion of feedback factor */
} adi_apollo_clk_pll_cfg_t;
ADI_APOLLO_PACK_FINISH



/**
 * \brief SERDES PLL Config
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_serdes_pll_cfg {
    uint32_t    loop_bandwidth;             /*!< PLL Loop bandwidth  */
    uint8_t     phase_margin;               /*!< PLL Phase Margin  */
    uint8_t     div_range;                  /*!< Serdes only Div Range  */
    uint8_t     div2;                       /*!< Serdes only Div 2  */
    uint8_t     power;                      /*!< PLL Power setting  */
    uint8_t     ref_clk_div;                /*!< PLL ref clock divider  */
    uint8_t     i_bleed_en;                 /*!< PLL bleed ramp enable  */
    uint8_t     serdes_pll_odiv;            /*!< Divider value (Yoda reg. name: register serdes_output_divider_ctl)  */
    uint32_t    feedback_int;               /*!< Integer portion of feedback factor */
    uint32_t    feedback_frac;              /*!< Fractional portion of feedback factor */
} adi_apollo_serdes_pll_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Valid Apollo Clocking Modes
 */
typedef enum adi_apollo_divg_mode {
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8              = 0u,   /*!< Single data rate, divide by 8 */
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_4              = 1u,   /*!< Single data rate, divide by 4 */
    ADI_APOLLO_CLOCKING_MODE_DDR_DIV_4              = 2u,   /*!< Double data rate, divide by 4 */
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_4_DDR_DIV_2    = 3u    /*!< Single data rate divide by 4 and double data rate divide by 2 */
} ADI_APOLLO_PACK_ENUM adi_apollo_divg_mode_e;


/**
 * \brief Valid ARM Clock Side Selection
 */
typedef enum adi_apollo_arm_clock_sel {
    ADI_APOLLO_ARM_CLOCK_SEL_A  = 0u,   /*!< A side */
    ADI_APOLLO_ARM_CLOCK_SEL_B  = 1u    /*!< B side */
} ADI_APOLLO_PACK_ENUM adi_apollo_arm_clock_sel_e;


/**
 * \brief Clock configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_clk_cfg {
    bool                        is_dual_clk;                                /*!< FALSE: Single (Center) clocking, TRUE: Dual clocking */
    bool                        clk_path_sel;                               /*!< FALSE: On-chip PLL, TRUE: External clock */
    uint32_t                    ref_clk_freq_kHz;                           /*!< Reference clock frequency. */
    adi_apollo_divg_mode_e      clocking_mode;                              /*!< DivG block in clock generator. */
    bool                        adc_divby2[ADI_APOLLO_NUM_SIDES];           /*!< Divider control for ADC clock path. TRUE: Divide by 2 */
    bool                        dac_divby2[ADI_APOLLO_NUM_SIDES];           /*!< Divider control for DAC clock path. TRUE: Divide by 2 */
    bool                        adc_inclk_invert0[ADI_APOLLO_NUM_SIDES];    /*!< Enable ADC 0 clock inversion. TRUE: invert clock */
    bool                        adc_inclk_invert1[ADI_APOLLO_NUM_SIDES];    /*!< Enable ADC 1 clock inversion. TRUE: invert clock */
    uint64_t                    dev_clk_freq_Hz;                            /*!< Device clock frequency in Hertz */
    adi_apollo_arm_clock_sel_e  arm_clock_sel;                              /*!< Selection of where the ARM/JESD clock originates when single_dual_clk_sel == True */
    uint8_t                     arm_clk_div;                                /*!< Arm clock divider. */
    uint8_t                     serdes_clk_div;                             /*!< SerDes Ref Clock Divider */
} adi_apollo_clk_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Sysref input configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_sysref_inp_cfg {
    uint64_t    ref_to_int_period_ratio;    /*!< Ratio of Reference Sysref to Internal Sysref */
    bool        sysref_present;             /*!< True if external sysref is provided to this instance of inp */
    bool        rx_term_en;                 /*!< True to enable on-chip termination, False to disable*/
    bool        cm_above_900mv;             /*!< True if wanted sysref common mode above 900mV */
} adi_apollo_sysref_inp_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief ADF4382 Delay Correction Mode. Bit 5 of ADF4382 register 0x032: DEL_MODE
 */
typedef enum adi_apollo_adf4382_corr {
    ADI_APOLLO_ADF4382_CORR_INT     = 0u,   /*!< Charge-Pump Bleed Mode */
    ADI_APOLLO_ADF4382_CORR_FRAC    = 1u,   /*!< Fractional Mode, using the ADF4382's sigma-delta modulator */
} ADI_APOLLO_PACK_ENUM adi_apollo_adf4382_corr_e;


/**
 * \brief ADF4382 Physical Instantiation Mapping for Dual-Clock
 */
typedef enum adi_apollo_adf4382_instance_side_cfg {
    ADI_APOLLO_ADF4382_INSTANCE_SIDE_CFG_NONE = 0u, /*!< No ADF4382 device is connected to this clocking side */
    ADI_APOLLO_ADF4382_INSTANCE_SIDE_CFG_1 = 1u,    /*!< Physical Instance 1 ADF4382 device is connected to this clocking side */
    ADI_APOLLO_ADF4382_INSTANCE_SIDE_CFG_2 = 2u,    /*!< Physical Instance 2 ADF4382 device is connected to this clocking side */
} ADI_APOLLO_PACK_ENUM adi_apollo_adf4382_instance_side_cfg_e;

/**
 * \brief ADF4382 configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_adf4382_cfg {
    adi_apollo_adf4382_instance_side_cfg_e  side_a_adf4382_instance_cfg;                            /*!< Defines which ADF4382 Instance is driving side A. Only relevant in Dual-Clock mode. */
    adi_apollo_adf4382_instance_side_cfg_e  side_b_adf4382_instance_cfg;                            /*!< Defines which ADF4382 Instance is driving side B. Only relevant in Dual-Clock mode. */

    adi_apollo_adf4382_corr_e       del_mode;                                                       /*!< Selects the clock phase correction approach for clock align for all adf4382 */

    /*!< ADF4382 Clock GPIO selection
    GPIOs that will be used to tell one or two ADF4382 to actuate one step of adjustment (either advance or delay clock based on the delay adjust variables)
    The first index corresponds to the ADF4382 instance '1' from ADF4382_clock_align_config
    */
    uint8_t                         clock_align_delay_adjust_gpio[ADI_APOLLO_NUM_ADF4382_GPIOS];    /*!< Clock Align Delay Adjust */
    uint8_t                         clock_align_delay_strobe_gpio[ADI_APOLLO_NUM_ADF4382_GPIOS];    /*!< Clock Align Delay Strobe */

    /*!< Settings per ADF4382 */
    bool                            track_polarity_select[ADI_APOLLO_MAX_NUM_ADF4382];              /*!< Variable used in case clock correction feedback(delay_adjust) to a particular ADF4382 is inverted. */
    uint8_t                         phase_adj[ADI_APOLLO_MAX_NUM_ADF4382];                          /*!< Phase adjustment steps of the ADF4382. Must match the PHASE_ADJUSTMENT of ADF4382. */
    uint32_t                        track_win[ADI_APOLLO_MAX_NUM_ADF4382];                          /*!< The amount of deviation the ADF4382's output clock can drift, in femtosecond [ fs ], relative to AD9084's External SysRef (BSYNC) before MCS Tracking attempts to correct it. This is the clock tracking window. */
} adi_apollo_adf4382_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief MCS Trigger configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_mcs_trigger_cfg {
    uint8_t trigger_pin_to_rx_datapath_a_mapping;   /*!< MCS Trigger for A Side Rx */
    uint8_t trigger_pin_to_rx_datapath_b_mapping;   /*!< MCS Trigger for B Side Rx */
    uint8_t trigger_pin_to_tx_datapath_a_mapping;   /*!< MCS Trigger for A Side Tx */
    uint8_t trigger_pin_to_tx_datapath_b_mapping;   /*!< MCS Trigger for B Side Tx */
} adi_apollo_mcs_trigger_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Multi-Chip Sync configuration
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_mcs_cfg {
    adi_apollo_sysref_inp_cfg_t     center_sysref;                              /*!< Center clocking region, sysref input pin configuration */
    adi_apollo_sysref_inp_cfg_t     side_a_sysref;                              /*!< Side A clocking region, sysref input pin configuration */
    adi_apollo_sysref_inp_cfg_t     side_b_sysref;                              /*!< Side B clocking region, sysref input pin configuration */

    bool                            use_gapped_sysref;                          /*!< if True, external sysref period (minimum) must match internal sysref period. If False, External Sysref period must be integer multiple of internal sysref */

    bool                            leave_sysref_rx_on;                         /*!< True to leave sysref rx on after sync, False to turn it off */

    uint16_t                        num_sysref_avg_mcs_fw;                      /*!< Number of sysref averages to use for mcs firmware */
    uint8_t                         sysref_lock_window_mcs_fw;                  /*!< Lock window for the sysref with mcs firmware */

    uint16_t                        internal_sysref_prd_digclk_cycles_center;   /*!< Sys Ref Period of Center MCS in number of master digital clock cycles */
    uint16_t                        internal_sysref_prd_digclk_cycles_side_a;   /*!< Sys Ref Period of side A MCS in number of master digital clock cycles */
    uint16_t                        internal_sysref_prd_digclk_cycles_side_b;   /*!< Sys Ref Period of side B MCS in number of master digital clock cycles */

    adi_apollo_adf4382_cfg_t        adf4382_cfg;
    adi_apollo_mcs_trigger_cfg_t    mcs_trigger_cfg;                            /*!< MCS Trigger Config */
} adi_apollo_mcs_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Valid JESD Sync Subclasses
 */
typedef enum adi_apollo_jesd_subclass {
    ADI_APOLLO_SUBCLASS_0   = 0u,
    ADI_APOLLO_SUBCLASS_1   = 1u,
    ADI_APOLLO_SUBCLASS_2   = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_subclass_e;


/**
 * \brief Valid JESD Logical Lanes
 */
typedef enum adi_apollo_jtx_lane_xbar_sel {
    ADI_APOLLO_XBAR_LOGIC_LANE_0    = 0u,
    ADI_APOLLO_XBAR_LOGIC_LANE_1    = 1u,
    ADI_APOLLO_XBAR_LOGIC_LANE_2    = 2u,
    ADI_APOLLO_XBAR_LOGIC_LANE_3    = 3u,
    ADI_APOLLO_XBAR_LOGIC_LANE_4    = 4u,
    ADI_APOLLO_XBAR_LOGIC_LANE_5    = 5u,
    ADI_APOLLO_XBAR_LOGIC_LANE_6    = 6u,
    ADI_APOLLO_XBAR_LOGIC_LANE_7    = 7u,
    ADI_APOLLO_XBAR_LOGIC_LANE_8    = 8u,
    ADI_APOLLO_XBAR_LOGIC_LANE_9    = 9u,
    ADI_APOLLO_XBAR_LOGIC_LANE_10   = 10u,
    ADI_APOLLO_XBAR_LOGIC_LANE_11   = 11u
} ADI_APOLLO_PACK_ENUM adi_apollo_jtx_lane_xbar_sel_e;


/**
 * \brief Valid JESD PHY Lanes
 */
typedef enum adi_apollo_jrx_lane_xbar_sel {
    ADI_APOLLO_XBAR_PHY_LANE_0  = 0u,
    ADI_APOLLO_XBAR_PHY_LANE_1  = 1u,
    ADI_APOLLO_XBAR_PHY_LANE_2  = 2u,
    ADI_APOLLO_XBAR_PHY_LANE_3  = 3u,
    ADI_APOLLO_XBAR_PHY_LANE_4  = 4u,
    ADI_APOLLO_XBAR_PHY_LANE_5  = 5u,
    ADI_APOLLO_XBAR_PHY_LANE_6  = 6u,
    ADI_APOLLO_XBAR_PHY_LANE_7  = 7u,
    ADI_APOLLO_XBAR_PHY_LANE_8  = 8u,
    ADI_APOLLO_XBAR_PHY_LANE_9  = 9u,
    ADI_APOLLO_XBAR_PHY_LANE_10 = 10u,
    ADI_APOLLO_XBAR_PHY_LANE_11 = 11u
} ADI_APOLLO_PACK_ENUM adi_apollo_jrx_lane_xbar_sel_e;


/**
 * \brief Valid JESD Versions
 */
typedef enum adi_apollo_jesd_version {
    ADI_APOLLO_JESD_204B    = 0u,
    ADI_APOLLO_JESD_204C    = 1u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_version_e;


/**
 * \brief Link independent configuration (applies to both links if dual_link is true)
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_common_cfg {
    adi_apollo_jesd_version_e   ver;            /*!< 0 = 204B, 1 = 204C */
    adi_apollo_jesd_subclass_e  subclass;       /*!< Subclass setting */
    bool                        dual_link;      /*!< TRUE: Dual link */
    uint16_t                    lane_enables;   /*!< Each bit for one lane (12 lanes) */
    uint32_t                    lane_rate_kHz;  /*!< Lane rate */
} adi_apollo_jesd_common_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Valid JESD Lanes Per Link Settings
 */
typedef enum adi_apollo_jesd_lanes_per_link {
    ADI_APOLLO_LANES_PER_LINK_1     = 0u,
    ADI_APOLLO_LANES_PER_LINK_2     = 1u,
    ADI_APOLLO_LANES_PER_LINK_3     = 2u,
    ADI_APOLLO_LANES_PER_LINK_4     = 3u,
    ADI_APOLLO_LANES_PER_LINK_6     = 5u,
    ADI_APOLLO_LANES_PER_LINK_8     = 7u,
    ADI_APOLLO_LANES_PER_LINK_12    = 11u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_lanes_per_link_e;


/**
 * \brief Valid JESD Octet Per Frame Settings
 */
typedef enum adi_apollo_jesd_octs_per_frm {
    ADI_APOLLO_OCT_PER_FRM_1    = 0u,
    ADI_APOLLO_OCT_PER_FRM_2    = 1u,
    ADI_APOLLO_OCT_PER_FRM_3    = 2u,
    ADI_APOLLO_OCT_PER_FRM_4    = 3u,
    ADI_APOLLO_OCT_PER_FRM_6    = 5u,
    ADI_APOLLO_OCT_PER_FRM_8    = 7u,
    ADI_APOLLO_OCT_PER_FRM_12   = 11u,
    ADI_APOLLO_OCT_PER_FRM_16   = 15u,
    ADI_APOLLO_OCT_PER_FRM_24   = 23u,
    ADI_APOLLO_OCT_PER_FRM_32   = 31u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_octs_per_frm_e;


/**
 * \brief Valid JESD Converter Per Link Settings
 */
typedef enum adi_apollo_jesd_convs_per_link {
    ADI_APOLLO_CONV_PER_LINK_1  = 0u,
    ADI_APOLLO_CONV_PER_LINK_2  = 1u,
    ADI_APOLLO_CONV_PER_LINK_3  = 2u,
    ADI_APOLLO_CONV_PER_LINK_4  = 3u,
    ADI_APOLLO_CONV_PER_LINK_6  = 5u,
    ADI_APOLLO_CONV_PER_LINK_8  = 7u,
    ADI_APOLLO_CONV_PER_LINK_12 = 11u,
    ADI_APOLLO_CONV_PER_LINK_16 = 15u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_convs_per_link_e;


/**
 * \brief Valid JESD Control Bits Per Sample Settings
 */
typedef enum adi_apollo_jesd_cont_bits_per_samp {
    ADI_APOLLO_CONT_BITS_PER_SAMP_0 = 0u,
    ADI_APOLLO_CONT_BITS_PER_SAMP_1 = 1u,
    ADI_APOLLO_CONT_BITS_PER_SAMP_2 = 2u,
    ADI_APOLLO_CONT_BITS_PER_SAMP_3 = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_cont_bits_per_samp_e;


/**
 * \brief Valid JESD Converter Resolution Settings
 */
typedef enum adi_apollo_jesd_conv_resol {
    ADI_APOLLO_CONV_RESOL_7     = 6u,
    ADI_APOLLO_CONV_RESOL_8     = 7u,
    ADI_APOLLO_CONV_RESOL_9     = 8u,
    ADI_APOLLO_CONV_RESOL_10    = 9u,
    ADI_APOLLO_CONV_RESOL_11    = 10u,
    ADI_APOLLO_CONV_RESOL_12    = 11u,
    ADI_APOLLO_CONV_RESOL_13    = 12u,
    ADI_APOLLO_CONV_RESOL_14    = 13u,
    ADI_APOLLO_CONV_RESOL_15    = 14u,
    ADI_APOLLO_CONV_RESOL_16    = 15u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_conv_resol_e;


/**
 * \brief Valid JESD Sample Per Conv Per Frame Settings
 */
typedef enum adi_apollo_jesd_samp_conv_frm {
    ADI_APOLLO_SAMP_CONV_FRM_1  = 0u,
    ADI_APOLLO_SAMP_CONV_FRM_2  = 1u,
    ADI_APOLLO_SAMP_CONV_FRM_3  = 2u,
    ADI_APOLLO_SAMP_CONV_FRM_4  = 3u,
    ADI_APOLLO_SAMP_CONV_FRM_6  = 5u,
    ADI_APOLLO_SAMP_CONV_FRM_8  = 7u,
    ADI_APOLLO_SAMP_CONV_FRM_12 = 11u,
    ADI_APOLLO_SAMP_CONV_FRM_16 = 15u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_samp_conv_frm_e;


/**
 * \brief Valid JESD Bits Per Sample Settings
 */
typedef enum adi_apollo_jesd_bits_per_samp {
    ADI_APOLLO_BITS_PER_SAMP_8  = 7u,
    ADI_APOLLO_BITS_PER_SAMP_12 = 11u,
    ADI_APOLLO_BITS_PER_SAMP_16 = 15u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_bits_per_samp_e;


/**
 * \brief Valid JESD Sample Per Conv Settings
 */
typedef enum adi_apollo_jesd_samp_per_conv {
    ADI_APOLLO_SAMP_PER_CONV_1  = 0u,
    ADI_APOLLO_SAMP_PER_CONV_2  = 1u,
    ADI_APOLLO_SAMP_PER_CONV_4  = 3u,
    ADI_APOLLO_SAMP_PER_CONV_8  = 7u,
    ADI_APOLLO_SAMP_PER_CONV_12 = 11u,
    ADI_APOLLO_SAMP_PER_CONV_16 = 15u,
    ADI_APOLLO_SAMP_PER_CONV_24 = 23u,
    ADI_APOLLO_SAMP_PER_CONV_32 = 31u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_samp_per_conv_e;


/**
 * \brief Valid JESD SYNCb Settings
 */
typedef enum adi_apollo_jesd_syncb_sel {
    ADI_APOLLO_JESD_FRM_SYNCB_CMOS                  = 0u,
    ADI_APOLLO_JESD_FRM_SYNCB_LVDS_WITH_INTL_TERM   = 1u,
    ADI_APOLLO_JESD_FRM_SYNCB_LVDS_NO_INTL_TERM     = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_syncb_sel_e;


/**
 * \brief Per Link JTx Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_tx_link_cfg {
    bool                                    link_in_use;                                        /*!< if TRUE, the link is in use */
    uint8_t                                 e_minus1;                                           /*!< Blocks in Multi-block minus 1 for 204C */
    uint8_t                                 bank_id;                                            /*!< Bank ID extension to Device ID (0-15)*/
    uint8_t                                 dev_id;                                             /*!< Device ID link identification number (0-255) */
    uint8_t                                 base_lane_id;                                       /*!< Starting Lane ID */
    adi_apollo_jesd_lanes_per_link_e        l_minus1;                                           /*!< Number of lanes minus 1 */
    adi_apollo_jesd_octs_per_frm_e          f_minus1;                                           /*!< Number of bytes (octets) per frame minus 1 */
    adi_apollo_jesd_convs_per_link_e        m_minus1;                                           /*!< Number of converters minus 1 */
    adi_apollo_jesd_samp_conv_frm_e         s_minus1;                                           /*!< Number of samples per converter per frame minus 1 */
    bool                                    high_dens;                                          /*!< TRUE: High-density */
    uint8_t                                 k_minus1;                                           /*!< Number of frames in a multiframe - 1 (0 - 255) */
    adi_apollo_jesd_conv_resol_e            n_minus1;                                           /*!< Link converter resolution minus 1 */
    adi_apollo_jesd_bits_per_samp_e         np_minus1;                                          /*!< Converter sample resolution in bits minus 1 */
    adi_apollo_jesd_samp_per_conv_e         ns_minus1;                                          /*!< Number of samples per converter in Conv_sample */
    adi_apollo_jesd_cont_bits_per_samp_e    cs;                                                 /*!< Number of control bits per sample*/
    bool                                    scr;                                                /*!< Scrambling enable */
    bool                                    quick_cfg_en;                                       /*!< Enable Quick configuration mode */
    uint8_t                                 quick_mode_id;                                      /*!< Quick configuration mode ID */
    uint8_t                                 syncb_in_sel;                                       /*!< Selects SYNCb input source. */
    adi_apollo_jesd_syncb_sel_e             syncb_lvds_mode;                                    /*!< SyncB LVDS Mode select */
    bool                                    fec_enable;                                         /*!< If TRUE, forward error correction is enabled */
    bool                                    async_mode;                                         /*!< IF TRUE, asynchronous mode is enabled */
    bool                                    metword_rev;                                        /*!< IF TRUE, bit ordering of CRC/FEC is reversed */
    uint16_t                                link_dp_ratio;                                      /*!< Link datapath ratio */
    uint16_t                                link_total_ratio;                                   /*!< Link total ratio */
    uint16_t                                phase_adjust;                                       /*!< LMFC offset for deterministic delay, Yoda register: sync_sysref_delay */
    adi_apollo_jesd_frm_conv_xbar_select_e  conv_xbar_sel[ADI_APOLLO_JESD_MAX_CONV_XBAR_IDXS];  /*!< Converter crossbar selection */
    adi_apollo_jtx_lane_xbar_sel_e          lane_xbar[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];      /*!< Lane crossbar selection */
} adi_apollo_jesd_tx_link_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief List of Serializer Output Swings
 */
typedef enum adi_apollo_jesd_jtx_output_drive_swing {
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_100     = 0u,       /*!< Output Swing at 100% Vtt */
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_85      = 1u,       /*!< Output Swing at 85% Vtt */
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_75      = 2u,       /*!< Output Swing at 75% Vtt */
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_50      = 3u,       /*!< Output Swing at 50% Vtt */
    ADI_APOLLO_JESD_DRIVE_SWING_LAST_VALID  = 3u        /*!< Last Valid Output Swing */
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_output_drive_swing_e;


/**
 * \brief List of Serializer Pre Tap Values
 */
typedef enum adi_apollo_jesd_jtx_pre_emphasis {
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_0_DB          = 0u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_3_DB          = 1u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_6_DB          = 2u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_LAST_VALID    = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_pre_emphasis_e;


/**
 * \brief List of Serializer Post Tap Values
 */
typedef enum adi_apollo_jesd_jtx_post_emphasis {
    ADI_APOLLO_JESD_POST_TAP_LEVEL_0_DB         = 0u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_3_DB         = 1u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_6_DB         = 2u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_9_DB         = 3u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_12_DB        = 4u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_LAST_VALID   = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_post_emphasis_e;


/**
 * \brief Valid SerDes Lanerate Adapt Ratios
 */
typedef enum adi_apollo_serdes_lr_adapt_ratio {
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_1  = 0u,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_2  = 1u,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_4  = 2u,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_8  = 3u,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_16 = 4u,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_32 = 5u
} ADI_APOLLO_PACK_ENUM adi_apollo_serdes_lr_adapt_ratio_e;



/**
 * \brief Per Lane Serializer Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_ser_lane {
    adi_apollo_jesd_jtx_output_drive_swing_e    ser_amplitude;              /*!< Serializer lane amplitude */
    adi_apollo_jesd_jtx_pre_emphasis_e          ser_pre_emphasis;           /*!< Serializer lane pre-emphasis */
    adi_apollo_jesd_jtx_post_emphasis_e         ser_post_emphasis;          /*!< Serializer lane post-emphasis */
    bool                                        ser_invert_lane_polarity;   /*!< Serializer lane PN inversion select: TRUE = invert */
    adi_apollo_serdes_lr_adapt_ratio_e          lanerate_adapt;             /*!< Lanerate Adapt ratio */
} adi_apollo_jesd_ser_lane_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief List of Rate Modes for Deserializer
 */
typedef enum adi_apollo_deser_mode {
    ADI_APOLLO_DESER_MODE_FULL_RATE       = 0u,     /*!< Full Rate Mode */
    ADI_APOLLO_DESER_MODE_HALF_RATE       = 1u,     /*!< Half Rate Mode */
    ADI_APOLLO_DESER_MODE_QUARTER_RATE    = 2u,     /*!< Quarter Rate Mode */
    ADI_APOLLO_DESER_MODE_VSR             = 3u,     /*!< Very Short Reach Mode */
    ADI_APOLLO_DESER_NUM_OF_MODES         = 4u,     /*!< Number of modes */
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_mode_e;


/**
 * \brief List of config options for deserializer
 */
typedef enum adi_apollo_deser_cfg_option_idx {
    ADI_APOLLO_DESER_CFG_OPTION_1   = 0u,
    ADI_APOLLO_DESER_CFG_OPTION_2   = 1u,
    ADI_APOLLO_DESER_CFG_OPTION_3   = 2u,
    ADI_APOLLO_DESER_CFG_OPTION_4   = 3u,
    ADI_APOLLO_DESER_CFG_OPTION_5   = 4u,
    ADI_APOLLO_DESER_CFG_OPTION_6   = 5u,
    ADI_APOLLO_DESER_CFG_OPTION_7   = 6u,
    ADI_APOLLO_DESER_CFG_OPTION_8   = 7u,
    ADI_APOLLO_DESER_CFG_OPTION_9   = 8u,
    ADI_APOLLO_DESER_CFG_OPTION_10  = 9u,
    ADI_APOLLO_DESER_NUM_CFGS       = 10u
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_cfg_option_idx_e;


/**
 * \brief List of Override options for deserializer
 */
typedef enum adi_apollo_deser_override_name {
    ADI_APOLLO_DESER_INVALID        = 0,        /*!< No further settings applied */
    ADI_APOLLO_DESER_JRX_CTLE_S1_GM = 1u,       /*!< AEQ stage 1 transconductance weight; 17 units with irregular coding - 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,31 are valid. */
    ADI_APOLLO_DESER_JRX_CTLE_S1_LD = 2u,       /*!< AEQ stage 1 load impedance weight; 15 units with irregular coding - 0,1,2,3,4,5,6,7,9,10,11,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_JRX_CTLE_S2_GM = 3u,       /*!< AEQ stage 2 load impedance weight; 12 units with irregular coding - 0,1,2,3,4,5,6,7,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_JRX_CTLE_S2_LD = 4u,       /*!< AEQ stage 2 transconductance weight; 13 units with irregular coding - 0,1,2,3,4,6,7,11,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_LAST_VALID     = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_override_name_e;



/**
 * \brief Per Lane Deserializer Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_deser_lane {
    uint8_t                               high_boost_options;                       /*!< Mask containing how the config options should be applied */
    bool                                  des_inv_lane_pol;                         /*!< Deserializer Lane PN inversion select: TRUE = invert */
    adi_apollo_serdes_lr_adapt_ratio_e    lanerate_adapt;                           /*!< Lanerate Adapt ratio */
    uint32_t                              cfg_options[ADI_APOLLO_DESER_NUM_CFGS];   /*!< Used for modifying the SerDes configuration at startup based off customers use case */
    uint16_t                              ctle_filter_mask;                         /*!< Used to modify Des PHY ctle filter before running cals.Multiple bits can be set for this field - Bit 0: low bandwidth, Bit 1: medium bandwidth, Bit 2: high bandwidth, Bits 3-8: For debugging purposes. */
} adi_apollo_jesd_deser_lane_t;
ADI_APOLLO_PACK_FINISH



/**
 * \brief Configuration for individual links within JRX
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_rx_link_cfg {
    bool                                    link_in_use;                                            /*!< if TRUE, the link is in use */
    uint8_t                                 e_minus1;                                               /*!< Blocks in Multi-block minus 1 for 204C */
    uint8_t                                 bank_id;                                                /*!< Bank ID extension to Device ID (0-15)*/
    uint8_t                                 dev_id;                                                 /*!< Device ID link identification number (0-255) */
    uint8_t                                 base_lane_id;                                           /*!< Starting Lane ID */
    adi_apollo_jesd_lanes_per_link_e        l_minus1;                                               /*!< Number of lanes minus 1 */
    adi_apollo_jesd_octs_per_frm_e          f_minus1;                                               /*!< Number of bytes (octets) per frame minus 1 */
    adi_apollo_jesd_convs_per_link_e        m_minus1;                                               /*!< Number of converters minus 1 */
    adi_apollo_jesd_samp_conv_frm_e         s_minus1;                                               /*!< Number of samples per converter per frame minus 1 */
    bool                                    high_dens;                                              /*!< High-density */
    uint8_t                                 k_minus1;                                               /*!< Number of frames in a multiframe minus 1 (0 - 255) */
    adi_apollo_jesd_conv_resol_e            n_minus1;                                               /*!< Link converter resolution minus 1 */
    adi_apollo_jesd_bits_per_samp_e         np_minus1;                                              /*!< Converter sample resolution in bits minus 1 */
    adi_apollo_jesd_samp_per_conv_e         ns_minus1;                                              /*!< Number of samples per converter in Conv_sample minus 1 */
    adi_apollo_jesd_cont_bits_per_samp_e    cs;                                                     /*!< Number of control bits per sample*/
    bool                                    scr;                                                    /*!< Scrambling enable */
    bool                                    quick_cfg_en;                                           /*!< Enable Quick configuration mode */
    uint8_t                                 quick_mode_id;                                          /*!< Quick configuration mode ID */
    adi_apollo_jesd_dfrm_sync_pad_req_e     syncb_out_sel;                                          /*!< Selects deframer SYNCBOUT pin. */
    adi_apollo_jesd_syncb_sel_e             syncb_lvds_mode;                                        /*!< SyncB LVDS Mode select */
    uint16_t                                link_dp_ratio;                                          /*!< Link datapath ratio */
    uint16_t                                link_total_ratio;                                       /*!< Link total ratio */
    bool                                    invalid_data_en;                                        /*!< True to enable invalid sample removal */
    uint8_t                                 invalid_sample_num;                                     /*!< The number of invalid samples each conv_clk. The value should be (JTX_NS - 1). */
    bool                                    sample_repeat_en;                                       /*!< True to enable sample repeat removal */
    uint16_t                                phase_adjust;                                           /*!< LMFC offset for deterministic delay, Yoda register: sync_sysref_delay */
    uint8_t                                 sample_xbar_sel[ADI_APOLLO_JESD_MAX_SAMPLE_XBAR_IDXS];  /*!< Converter sample crossbar selection */
    adi_apollo_jrx_lane_xbar_sel_e          lane_xbar[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];          /*!< Lane crossbar selection */
} adi_apollo_jesd_rx_link_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Per Side JTx Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_tx_cfg {
    adi_apollo_jesd_common_cfg_t    common_link_cfg;                                        /*!< Config for both links or one link if only one link */
    adi_apollo_jesd_tx_link_cfg_t   tx_link_cfg[ADI_APOLLO_JESD_LINKS];                     /*!< Config for each of the link(s) */
    adi_apollo_jesd_ser_lane_t      serializer_lane[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];    /*!< Per Serializer Lane Config */
} adi_apollo_jesd_tx_cfg_t;
ADI_APOLLO_PACK_FINISH

/**
 * \brief Possible number of bridging calibrations for SERDES Rx initial calibration
 */
typedef enum adi_apollo_deser_num_bridging_cals {
    ADI_APOLLO_NUM_BRIDGING_CALS_0 = 0u,
    ADI_APOLLO_NUM_BRIDGING_CALS_1 = 1u,
    ADI_APOLLO_NUM_BRIDGING_CALS_2 = 2u,
    ADI_APOLLO_NUM_BRIDGING_CALS_3 = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_num_bridging_cals_e;

ADI_APOLLO_PACK_START
/**
 * \brief JESD Deserializer Common Config Structure
 */
typedef struct adi_apollo_jesd_deser_common {
    adi_apollo_deser_mode_e                 rx_des_mode;            /*!< Deserializer mode settings */
    adi_apollo_deser_num_bridging_cals_e    num_bridging_cals;      /*!< Number of bridging calibrations */
    bool                                    sample_xbar_override;   /*!< 0 == Use design sample xbar ramp settings, 1 == Use values in sample_xbar_sel for both links. */
} adi_apollo_jesd_deser_common_t;
ADI_APOLLO_PACK_FINISH


ADI_APOLLO_PACK_START
/**
 * \brief Per Side JRx Config Structure
 */
typedef struct adi_apollo_jesd_rx_cfg {
    adi_apollo_jesd_common_cfg_t    common_link_cfg;                                        /*!< Common config for JRx link */
    adi_apollo_jesd_rx_link_cfg_t   rx_link_cfg[ADI_APOLLO_JESD_LINKS];                     /*!< Config for JRx link */
    adi_apollo_jesd_deser_common_t  deserializer_common;                                    /*!< Properties common to all Rx lanes in a pack */
    adi_apollo_jesd_deser_lane_t    deserializer_lane[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];  /*!< Per Deserializer Lane Config */
} adi_apollo_jesd_rx_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief GPIO Config Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_gpio_cfg {
    uint8_t     gpio_quick_config;                          /*!< GPIO quick configuration. See user guide for information on quick config use cases. */
    uint64_t    gpio_mode_en;                               /*!< Nth bit corresponds to Nth GPIO pin for enabling gpio_mode_dir for that pin. */
    uint64_t    gpio_mode_dir;                              /*!< This field has meaning only if corresponding gpio_mode_en is set to 1. if gpio_mode_dir = 1 than gpio will be configured as an input and if gpio_mode_dir=0 than gpio will be configured as output. */
    uint64_t    gpio_from_master;                           /*!< This field is used by masters (core0/core1/stream_proc) to write data to Digital GPIO pins. */
    uint64_t    gpio_from_set;                              /*!< Writing a '1' to a bit in this field sets the corresponding bit in the gpio_from_master register. Writing '0' has no effect. */
    uint64_t    gpio_from_clear;                            /*!<  Writing a '1' to a bit in this field clears the corresponding bit in the gpio_from_master register. Writing '0' has no effect. */
    uint8_t     gpio_source_control[ADI_APOLLO_NUM_GPIOS];  /*!< Select pin of GPIO functional pinmux. */
    uint64_t    gpio_stage_sel_lsb;                         /*!< 64Bits select the mux stage of GPIO pinmuxing. */
    uint64_t    gpio_stage_sel_msb;                         /*!< 38Bits select the mux stage of GPIO pinmuxing. */
} adi_apollo_gpio_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief SW Trim Level Enumeration
 */
typedef enum adi_apollo_cfg_sw_trim {
    ADI_APOLLO_SW_TRIM_NOT_SET = 0,     /*!< SW Trim is not set for this profile may not work with SW1/3 trim levels */
    ADI_APOLLO_SW_TRIM_1       = 1u,    /*!< This profile is suitable for SW1, SW3, SW5 trim levels. */
    ADI_APOLLO_SW_TRIM_3       = 3u,    /*!< This profile is suitable for SW3, SW5 trim levels. */
    ADI_APOLLO_SW_TRIM_5       = 5u     /*!< This profile is suitable only for SW5 trim levels. */
} ADI_APOLLO_PACK_ENUM adi_apollo_cfg_sw_trim_e;


/**
 * \brief Profile Configuration Structure - Contains the profile version and compatibilty information
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_profile_cfg {
    adi_apollo_profile_version_t    profile_version;    /*!< Profile version  */
    uint16_t                        cfg_version;        /*!< A specific profile can be valued several times, each with a new config_version */
    bool                            is_8t8r;            /*!< True for 8t8r, False for 4t4r  */
    adi_apollo_cfg_sw_trim_e        sw_trim;            /*!< The minimum SW Trim level this profile supports */
} adi_apollo_profile_cfg_t;
ADI_APOLLO_PACK_FINISH


/**
 * \brief Apollo Top Profile Structure
 */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_top {
    adi_apollo_profile_cfg_t    profile_cfg;                                                        /*!< Profile version and misc high level info */
    adi_apollo_clk_cfg_t        clk_cfg;                                                            /*!< Clock configuration */
    adi_apollo_clk_pll_cfg_t    clk_pll;                                                            /*!< Main PLL configuration */
    adi_apollo_mcs_cfg_t        mcs_cfg;                                                            /*!< MCS configuration */
    adi_apollo_gpio_cfg_t       gpio_cfg;                                                           /*!< GPIO configuration */
    adi_apollo_txpath_t         tx_path[ADI_APOLLO_NUM_SIDES];                                      /*!< Tx Path configuration */
    adi_apollo_rxpath_t         rx_path[ADI_APOLLO_NUM_SIDES];                                      /*!< Rx Path configuration */
    adi_apollo_jesd_tx_cfg_t    jtx[ADI_APOLLO_NUM_SIDES];                                          /*!< JTX configuration */
    adi_apollo_jesd_rx_cfg_t    jrx[ADI_APOLLO_NUM_SIDES];                                          /*!< JRX configuration */
    adi_apollo_dac_cfg_t        dac_cfg[ADI_APOLLO_NUM_SIDES];                                      /*!< DAC configuration */
    adi_apollo_adc_cfg_t        adc_cfg[ADI_APOLLO_NUM_SIDES];                                      /*!< ADC configuration */
    bool                        lb0_en[ADI_APOLLO_NUM_SIDES][ADI_APOLLO_DAC_DIG_SLICE_PER_SIDE];    /*!< Loopback for feeding the DAC from ADC */
    adi_apollo_serdes_pll_cfg_t serdes_pll;                                                         /*!< SERDES PLL structure */
    uint16_t                    adc_enable;                                                         /*!< ADC Enable. B0=ADC_A0, B1=ADC_A1, B2=ADC_A2, B3=ADC_A3, B4=ADC_B0, B5=ADC_B1, B6=ADC_B2, B7=ADC_B3  */
    uint16_t                    dac_enable;                                                         /*!< DAC Enable. B0=DAC_A0, B1=DAC_A1, B2=DAC_A2, B3=DAC_A3, B4=DAC_B0, B5=DAC_B1, B6=DAC_B2, B7=DAC_B3  */
    uint8_t                     reserved_cfg[64];                                                   /*!< Extra space for temporary feature implementation */
    uint32_t                    profile_checksum;                                                   /*!< Checksum of the entire profile using CRC32 */
} adi_apollo_top_t;
ADI_APOLLO_PACK_FINISH

#endif /* __APOLLO_CPU_DEVICE_PROFILE_TYPES_H__ */
