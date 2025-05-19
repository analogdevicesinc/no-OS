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

#define ADI_APOLLO_PROFILE_VERSION_MAJOR         9             /*!< Major */
#define ADI_APOLLO_PROFILE_VERSION_MINOR         1             /*!< Minor */
#define ADI_APOLLO_PROFILE_VERSION_PATCH         0             /*!< Patch */

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

#ifdef __GNUC__
#define ADI_APOLLO_PACK_ENUM __attribute__((packed))
#else
#define ADI_APOLLO_PACK_ENUM
#endif

/*! Apollo A and B sides, historically named east and west sides */
typedef enum
{
    ADI_APOLLO_SIDE_IDX_A = 0,
    ADI_APOLLO_SIDE_IDX_B,
    ADI_APOLLO_NUM_SIDES
} ADI_APOLLO_PACK_ENUM adi_apollo_sides_e;

/*! Apollo Link Count Per Side */
typedef enum
{
    ADI_APOLLO_JESD_LINK_0 = 0,
    ADI_APOLLO_JESD_LINK_1,
    ADI_APOLLO_JESD_LINKS
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_links_e;

/* Enumeration for RX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adi_apollo_rx_channel_num
{
    ADI_APOLLO_RX_CH_A0 = 0u,
    ADI_APOLLO_RX_CH_A1 = 1u,
    ADI_APOLLO_RX_CH_A2 = 2u,
    ADI_APOLLO_RX_CH_A3 = 3u,
    ADI_APOLLO_RX_CH_B0 = 4u,
    ADI_APOLLO_RX_CH_B1 = 5u,
    ADI_APOLLO_RX_CH_B2 = 6u,
    ADI_APOLLO_RX_CH_B3 = 7u,
    ADI_APOLLO_RX_CH_MAX = 7u,
    ADI_APOLLO_RX_CH_LEN
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_channel_num_e;

/* Enumeration for TX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adi_apollo_tx_channel_num
{
    ADI_APOLLO_TX_CH_A0 = 0u,
    ADI_APOLLO_TX_CH_A1 = 1u,
    ADI_APOLLO_TX_CH_A2 = 2u,
    ADI_APOLLO_TX_CH_A3 = 3u,
    ADI_APOLLO_TX_CH_B0 = 4u,
    ADI_APOLLO_TX_CH_B1 = 5u,
    ADI_APOLLO_TX_CH_B2 = 6u,
    ADI_APOLLO_TX_CH_B3 = 7u,
    ADI_APOLLO_TX_CH_MAX = 7u,
    ADI_APOLLO_TX_CH_LEN
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_channel_num_e;

/*! Enumeration for coarse digital down converters */
typedef enum adi_apollo_cddc_idx
{
    ADI_APOLLO_CDDC_IDX_0 = 0,
    ADI_APOLLO_CDDC_IDX_1,
    ADI_APOLLO_CDDCS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_cddc_idx_e;

/*! Enumeration for coarse digital down converter paths */
typedef enum adi_apollo_cddc_path_idx
{
    ADI_APOLLO_CDDC_PATH_IDX_0 = 0,
    ADI_APOLLO_CDDC_PATH_IDX_1,
    ADI_APOLLO_CDDC_PATH_IDX_2,
    ADI_APOLLO_CDDC_PATH_IDX_3,
    ADI_APOLLO_CDDC_PATHS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_cddc_path_idx_e;

/*! Enumeration for fine digital down converters */
typedef enum adi_apollo_fddc_idx
{
    ADI_APOLLO_FDDC_IDX_0 = 0,
    ADI_APOLLO_FDDC_IDX_1,
    ADI_APOLLO_FDDC_IDX_2,
    ADI_APOLLO_FDDC_IDX_3,
    ADI_APOLLO_FDDCS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_idx_e;

/*! Enumeration for fine digital down converter paths */
typedef enum adi_apollo_fddc_path_idx
{
    ADI_APOLLO_FDDC_PATH_IDX_0 = 0,
    ADI_APOLLO_FDDC_PATH_IDX_1,
    ADI_APOLLO_FDDC_PATH_IDX_2,
    ADI_APOLLO_FDDC_PATH_IDX_3,
    ADI_APOLLO_FDDC_PATH_IDX_4,
    ADI_APOLLO_FDDC_PATH_IDX_5,
    ADI_APOLLO_FDDC_PATH_IDX_6,
    ADI_APOLLO_FDDC_PATH_IDX_7,
    ADI_APOLLO_FDDC_PATHS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_path_idx_e;

/*! Enumeration for coarse digital up converters */
typedef enum adi_apollo_cduc_idx
{
    ADI_APOLLO_CDUC_IDX_0 = 0,
    ADI_APOLLO_CDUC_IDX_1,
    ADI_APOLLO_CDUCS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_cduc_idx_e;

/*! Enumeration for coarse digital up converter paths */
typedef enum adi_apollo_cduc_path_idx
{
    ADI_APOLLO_CDUC_PATH_IDX_0 = 0,
    ADI_APOLLO_CDUC_PATH_IDX_1,
    ADI_APOLLO_CDUC_PATH_IDX_2,
    ADI_APOLLO_CDUC_PATH_IDX_3,
    ADI_APOLLO_CDUC_PATHS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_cduc_path_idx_e;

/*! Enumeration for fine digital up converters */
typedef enum adi_apollo_fduc_idx
{
    ADI_APOLLO_FDUC_IDX_0 = 0,
    ADI_APOLLO_FDUC_IDX_1,
    ADI_APOLLO_FDUC_IDX_2,
    ADI_APOLLO_FDUC_IDX_3,
    ADI_APOLLO_FDUCS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_idx_e;

/*! Enumeration for fine digital up converter paths */
typedef enum adi_apollo_fduc_path_idx
{
    ADI_APOLLO_FDUC_PATH_IDX_0 = 0,
    ADI_APOLLO_FDUC_PATH_IDX_1,
    ADI_APOLLO_FDUC_PATH_IDX_2,
    ADI_APOLLO_FDUC_PATH_IDX_3,
    ADI_APOLLO_FDUC_PATH_IDX_4,
    ADI_APOLLO_FDUC_PATH_IDX_5,
    ADI_APOLLO_FDUC_PATH_IDX_6,
    ADI_APOLLO_FDUC_PATH_IDX_7,
    ADI_APOLLO_FDUC_PATHS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_path_idx_e;

/*! Enumeration for programmable filters */
typedef enum adi_apollo_pfilt_idx
{
    ADI_APOLLO_PFILT_IDX_0 = 0,
    ADI_APOLLO_PFILTS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_idx_e;

/*! Enumeration for complex filters */
typedef enum adi_apollo_cfir_idx
{
    ADI_APOLLO_CFIR_IDX_0 = 0,
    ADI_APOLLO_CFIR_IDX_1,
    ADI_APOLLO_CFIRS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_idx_e;

/*! Enumeration for fractional sampling rate converters */
typedef enum adi_apollo_fsrc_idx
{
    ADI_APOLLO_FSRC_IDX_0 = 0,
    ADI_APOLLO_FSRC_IDX_1,
    ADI_APOLLO_FSRCS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_fsrc_idx_e;

/*! Enumeration for smon */
typedef enum adi_apollo_smon_idx
{
    ADI_APOLLO_SMON_IDX_0 = 0,
    ADI_APOLLO_SMON_IDX_1,
    ADI_APOLLO_SMON_IDX_2,
    ADI_APOLLO_SMON_IDX_3,
    ADI_APOLLO_SMONS_PER_SIDE
} ADI_APOLLO_PACK_ENUM adi_apollo_smon_idx_e;

#define ADI_APOLLO_JESD_MAX_LANES_PER_SIDE                (12u)
#define ADI_APOLLO_JESD_MAX_FRM_SAMPLE_XBAR_IDX           (64u)
#define ADI_APOLLO_JESD_MAX_LKSH_SAMPLE_XBAR_IDX          (32u)

/* Enumeration for serdes lane numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adi_apollo_serdes_lane_num
{
    ADI_APOLLO_SERDES_LANE_1 = 0u,         /*!< SERDES LANE # 1 index   */
    ADI_APOLLO_SERDES_LANE_2,              /*!< SERDES LANE # 2 index   */
    ADI_APOLLO_SERDES_LANE_3,              /*!< SERDES LANE # 3 index   */
    ADI_APOLLO_SERDES_LANE_4,              /*!< SERDES LANE # 4 index   */
    ADI_APOLLO_SERDES_LANE_5,              /*!< SERDES LANE # 5 index   */
    ADI_APOLLO_SERDES_LANE_6,              /*!< SERDES LANE # 6 index   */
    ADI_APOLLO_SERDES_LANE_7,              /*!< SERDES LANE # 7 index   */
    ADI_APOLLO_SERDES_LANE_8,              /*!< SERDES LANE # 8 index   */
    ADI_APOLLO_SERDES_LANE_9,              /*!< SERDES LANE # 9 index   */
    ADI_APOLLO_SERDES_LANE_10,             /*!< SERDES LANE # 10 index   */
    ADI_APOLLO_SERDES_LANE_11,             /*!< SERDES LANE # 11 index   */
    ADI_APOLLO_SERDES_LANE_12,             /*!< SERDES LANE # 12 index   */
    ADI_APOLLO_SERDES_LANE_13,             /*!< SERDES LANE # 13 index   */
    ADI_APOLLO_SERDES_LANE_14,             /*!< SERDES LANE # 14 index   */
    ADI_APOLLO_SERDES_LANE_15,             /*!< SERDES LANE # 15 index   */
    ADI_APOLLO_SERDES_LANE_16,             /*!< SERDES LANE # 16 index   */
    ADI_APOLLO_SERDES_LANE_17,             /*!< SERDES LANE # 17 index   */
    ADI_APOLLO_SERDES_LANE_18,             /*!< SERDES LANE # 18 index   */
    ADI_APOLLO_SERDES_LANE_19,             /*!< SERDES LANE # 19 index   */
    ADI_APOLLO_SERDES_LANE_20,             /*!< SERDES LANE # 20 index   */
    ADI_APOLLO_SERDES_LANE_21,             /*!< SERDES LANE # 21 index   */
    ADI_APOLLO_SERDES_LANE_22,             /*!< SERDES LANE # 22 index   */
    ADI_APOLLO_SERDES_LANE_23,             /*!< SERDES LANE # 23 index   */
    ADI_APOLLO_SERDES_LANE_24,             /*!< SERDES LANE # 24 index   */
    ADI_APOLLO_SERDES_LANE_LEN             /*!< Total number of SerDes Lanes supported  */
} ADI_APOLLO_PACK_ENUM adi_apollo_serdes_lane_num_e;

/*!< List of Framers */
typedef enum adi_apollo_jesd_framer
{
    ADI_APOLLO_JESD_FRAMER_0   = 0u,   /*!< Framer 0 selection */
    ADI_APOLLO_JESD_FRAMER_1   = 1u,   /*!< Framer 1 selection */
    ADI_APOLLO_JESD_FRAMER_NUM = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_framer_e;

/*!< List of Deframers */
typedef enum adi_apollo_jesd_deframer
{
    ADI_APOLLO_JESD_DEFRAMER_0   = 0u,   /*!< Deframer 0 selection */
    ADI_APOLLO_JESD_DEFRAMER_1   = 1u,   /*!< Deframer 1 selection */
    ADI_APOLLO_JESD_DEFRAMER_NUM = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_deframer_e;

/*!< List of SerDes PACKs */
typedef enum adi_apollo_ser_des_pack
{
    ADI_APOLLO_SERDES_PACK_IDX_0 = 0u,
    ADI_APOLLO_SERDES_PACK_IDX_1 = 1u,
    ADI_APOLLO_SERDES_NUM_PACKS  = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_ser_des_pack_e;

/*!< List of Deframers (??) */
//typedef enum adi_apollo_jesd_lk_sh_switcher
//{
//    ADI_APOLLO_JESD_LKSH_SWITCHER_0   = 0u,   /*!< Link Sharing Switcher 0 selection */
//    ADI_APOLLO_JESD_LKSH_SWITCHER_1   = 1u,   /*!< Link Sharing Switcher 1 selection */
//    ADI_APOLLO_JESD_LKSH_SWITCHER_NUM = 2u
//} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_lk_sh_switcher_e;

/*!< List of Qbf Txfe */
typedef enum adi_apollo_jesd_qbf_txfe
{
    ADI_APOLLO_JESD_QBF_TXFE_0      = 0u,
    ADI_APOLLO_JESD_QBF_TXFE_1      = 1u,
    ADI_APOLLO_JESD_QBF_TXFE_NUM    = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_qbf_txfe_e;

/*!< List of DAC Sample Xbar options */
typedef enum adi_apollo_jesd_dfrm_sample_xbar_select
{
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_I = 0u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_Q = 1u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_I = 2u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_Q = 3u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_I = 4u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_Q = 5u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_I = 6u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_Q = 7u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_I = 8u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_Q = 9u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_I = 10u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_Q = 11u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_I = 12u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_Q = 13u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_I = 14u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_Q = 15u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_I = 16u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_Q = 17u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_I = 18u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_Q = 19u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_I = 20u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_Q = 21u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_I = 22u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_Q = 23u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_I = 24u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_Q = 25u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_I = 26u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_Q = 27u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_I = 28u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_Q = 29u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_I = 30u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_Q = 31u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_LAST_VALID = 31u,
    ADI_APOLLO_JESD_DFRM_SPLXBAR_INVALID    = 0x7Fu
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_dfrm_sample_xbar_select_e;

/*!< List of ADC Sample Xbar options */
typedef enum adi_apollo_jesd_frm_sample_xbar_select
{
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I = 0u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q = 1u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_I = 2u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_Q = 3u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I = 4u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q = 5u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_I = 6u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_Q = 7u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I = 8u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q = 9u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_I = 10u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_Q = 11u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I = 12u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q = 13u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_I = 14u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_Q = 15u,

    ADI_APOLLO_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I = 16u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q = 17u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_I = 18u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_Q = 19u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I = 20u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q = 21u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_I = 22u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_Q = 23u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I = 24u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q = 25u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_I = 26u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_Q = 27u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I = 28u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q = 29u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_I = 30u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q = 31u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_LAST_VALID_NO_INT = 31u, /* With no interleaving enabled */

    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_0     = 32u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_1     = 33u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_2     = 34u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_3     = 35u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_4     = 36u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_5     = 37u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_6     = 38u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_I_7     = 39u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_0     = 40u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_1     = 41u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_2     = 42u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_3     = 43u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_4     = 44u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_5     = 45u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_6     = 46u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX0_DATA_Q_7     = 47u,

    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_0     = 48u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_1     = 49u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_2     = 50u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_3     = 51u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_4     = 52u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_5     = 53u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_6     = 54u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_I_7     = 55u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_0     = 56u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_1     = 57u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_2     = 58u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_3     = 59u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_4     = 60u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_5     = 61u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_6     = 62u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7     = 63u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_LAST_VALID        = 63u,
    ADI_APOLLO_JESD_FRM_SPLXBAR_INVALID           = 0x7Fu
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_frm_sample_xbar_select_e;

//typedef enum adi_apollo_jesd_frm_syncb_mode
//{
//    ADI_APOLLO_JESD_FRM_SYNCB_PIN_MODE        = 0u,
//    ADI_APOLLO_JESD_FRM_SYNCB_SPI_MODE        = 1u
//} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_frm_syncb_mode_e;

//typedef enum adi_apollo_jesd_frm_sync_pad_req
//{
//    ADI_APOLLO_JESD_FRM_PWR_OFF_ALL_SYNC_PADS = 0u,
//    ADI_APOLLO_JESD_FRM_PWR_ON_SYNC_PAD0      = 1u,
//    ADI_APOLLO_JESD_FRM_PWR_ON_SYNC_PAD1      = 2u,
//    ADI_APOLLO_JESD_FRM_PWR_ON_ALL_SYNC_PADS  = 3u
//} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_frm_sync_pad_req_e;

typedef enum adi_apollo_jesd_dfrm_sync_pad_req
{
    ADI_APOLLO_JESD_DFRM_PWR_ON_SYNC_PAD1      = 0u,
    ADI_APOLLO_JESD_DFRM_PWR_ON_SYNC_PAD2      = 1u,
    ADI_APOLLO_JESD_DFRM_PWR_ON_ALL_SYNC_PADS  = 2u,
    ADI_APOLLO_JESD_DFRM_PWR_OFF_ALL_SYNC_PADS = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_dfrm_sync_pad_req_e;

typedef enum adi_apollo_jesd_conv_clock_mode
{
    ADI_APOLLO_JESD_NO_RESAMPLE_FIR = 0u,
    ADI_APOLLO_JESD_2_3_RESAMPLE_FIR,
    ADI_APOLLO_JESD_3_4_RESAMPLE_FIR
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_conv_clock_mode_e;

typedef enum adi_apollo_cpu_id
{
    ADI_APOLLO_CPU_ID_0 = 0,
    ADI_APOLLO_CPU_ID_1
} ADI_APOLLO_PACK_ENUM adi_apollo_cpu_id_e;

/* Bit macros for newSysref byte in Framer and Deframer structures */
//#define ADI_APOLLO_JESD_BITM_SYSREF_FOR_RELINK          (0x1U)
//#define ADI_APOLLO_JESD_BITP_SYSREF_FOR_RELINK          (0x0U)
//
//#define ADI_APOLLO_JESD_BITM_SYSREF_FOR_STARTUP         (0x2U)
//#define ADI_APOLLO_JESD_BITP_SYSREF_FOR_STARTUP         (0x1U)
//
//#define ADI_APOLLO_JESD_BITM_SYSREF_N_SHOT_COUNT       (0x3CU)
//#define ADI_APOLLO_JESD_BITP_SYSREF_N_SHOT_COUNT        (0x2U)
//
//#define ADI_APOLLO_JESD_BITM_SYSREF_N_SHOT_ENABLE      (0x40U)
//#define ADI_APOLLO_JESD_BITP_SYSREF_N_SHOT_ENABLE       (0x6U)
//
//#define ADI_APOLLO_JESD_BITM_SYSREF_IGNORE_WHEN_LINKED (0x80U)
//#define ADI_APOLLO_JESD_BITP_SYSREF_IGNORE_WHEN_LINKED  (0x7U)

#define ADI_APOLLO_RX_CHAN_LEN_CPU     (ADI_APOLLO_RX_CH_LEN  / 1u)
#define ADI_APOLLO_TX_CHAN_LEN_CPU     (ADI_APOLLO_TX_CH_LEN  / 1u)
#define ADI_APOLLO_SERDES_LANE_LEN_CPU (ADI_APOLLO_SERDES_LANE_LEN / 1u)
#define ADI_APOLLO_SERDES_PACK_LEN_CPU (ADI_APOLLO_SERDES_NUM_PACKS / 1u)

/*! Profile version structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_profile_version
{
    uint8_t     major;                /*!< Major component in profile version */
    uint16_t    minor;                /*!< Minor component in profile version */
    uint8_t     patch;                /*!< Patch component in profile version */
} adi_apollo_profile_version_t;
ADI_APOLLO_PACK_FINISH

/*! DAC configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dac_cfg
{
    uint64_t    dac_sampling_rate_Hz;                    /*!<   DAC sampling rate in Hz. */
    bool        dynamic_dac_en;                          /*!<   DAC dynamic enable. */
    bool        shuffle_en[ADI_APOLLO_DACS_PER_SIDE];    /*!<   Shuffle enable. */
    bool        scrambler_en[ADI_APOLLO_DACS_PER_SIDE];  /*!<   Scrambler enable. */
} adi_apollo_dac_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! ADC Slice Clocking Mode Enumerations */
typedef enum
{
    ADI_APOLLO_ADC_SLICE_PPRAND3P1 = 6U,    /*!< Ping-Pong Random 3+1 Mode */
    ADI_APOLLO_ADC_SLICE_PPSEQ4 = 7U,       /*!< Ping-Pong Sequential 4 Mode */
    ADI_APOLLO_ADC_SLICE_RAND3P1 = 10U,     /*!< Random 3+1 Mode */
    ADI_APOLLO_ADC_SLICE_SEQ4 = 11U        /*!< Sequential 4 Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_slc_mode_e;

/*! ADC Track & Hold Mode Enumerations */
typedef enum
{
    ADI_APOLLO_ADC_TRACK_SEQ1 = 0U,         /*!< Single Track & Hold Mode */
    ADI_APOLLO_ADC_TRACK_SEQ2 = 1U,         /*!< Sequential 2 Mode */
    ADI_APOLLO_ADC_TRACK_SEQ3 = 2U,         /*!< Sequential 3 Mode */
    ADI_APOLLO_ADC_TRACK_RAND2P1 = 5U,      /*!< Random 2+1 Using LFSR */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_trk_mode_e;

/*! ADC Input Coupling Enumerations */
typedef enum
{
    ADI_APOLLO_ADC_INPUT_COUPLING_AC = 0U,  /*!< AC Coupled Input */
    ADI_APOLLO_ADC_INPUT_COUPLING_DC,       /*!< DC Coupled Input */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_coupling_e;

/*! ADC Input Signal Type Enumerations */
typedef enum
{
    ADI_APOLLO_ADC_INPUT_SIGNAL_DIFFERENTIAL = 0U,  /*!< Differential Signal */
    ADI_APOLLO_ADC_INPUT_SIGNAL_SINGLE_ENDED,       /*!< Single - Ended Signal */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_signal_e;

/*! ADC Input Polarity Enumerations */
typedef enum
{
    ADI_APOLLO_ADC_INPUT_POLARITY_NORMAL = 0U,  /*!< Normal */
    ADI_APOLLO_ADC_INPUT_POLARITY_INVERT,       /*!< Inverted */
} ADI_APOLLO_PACK_ENUM adi_apollo_adc_input_polarity_e;

/*! ADC configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_adc_cfg
{
    uint64_t                        adc_sampling_rate_Hz;                           /*!<   ADC sampling rate in Hz. */
    uint8_t                         adc_init_cal_index;                             /*!<   ADC initial calibration index. */
    adi_apollo_adc_slc_mode_e       adc_slice_mode[ADI_APOLLO_ADCS_PER_SIDE];       /*!<   Slice mode */
    adi_apollo_adc_trk_mode_e       adc_track_mode[ADI_APOLLO_ADCS_PER_SIDE];       /*!<   Track mode */
    uint8_t                         full_scale_adj[ADI_APOLLO_ADCS_PER_SIDE];       /*!<   Full Scale Adjust */
    adi_apollo_adc_input_coupling_e inp_coupling_type[ADI_APOLLO_ADCS_PER_SIDE];    /*!<   Input Coupling Type */
    adi_apollo_adc_input_signal_e   inp_signal_type[ADI_APOLLO_ADCS_PER_SIDE];      /*!<   Input Signal Type */
    adi_apollo_adc_input_polarity_e inp_polarity[ADI_APOLLO_ADCS_PER_SIDE];         /*!<   Input Polarity */
    uint32_t                         adc_options[8];                                /*!<   ADC Options */
} adi_apollo_adc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!
 * \brief Enumerates Coarse DDC decimation
 */
typedef enum {
    ADI_APOLLO_CDDC_DCM_1  = 0x0,               /*!< Decimate by 1 */
    ADI_APOLLO_CDDC_DCM_2  = 0x1,               /*!< Decimate by 2 */
    ADI_APOLLO_CDDC_DCM_3  = 0x2,               /*!< Decimate by 3 */
    ADI_APOLLO_CDDC_DCM_4  = 0x3,               /*!< Decimate by 4 */
    ADI_APOLLO_CDDC_DCM_6  = 0x4,               /*!< Decimate by 6 */
    ADI_APOLLO_CDDC_DCM_12 = 0x6,               /*!< Decimate by 12 */
} ADI_APOLLO_PACK_ENUM adi_apollo_coarse_ddc_dcm_e;

/*!
 * \brief Enumerates Coarse DUC interpolation
 */
typedef enum {
    ADI_APOLLO_CDUC_INTERP_1  = 0x1,            /*!< Interpolate by 1 */
    ADI_APOLLO_CDUC_INTERP_2  = 0x2,            /*!< Interpolate by 2 */
    ADI_APOLLO_CDUC_INTERP_3  = 0x3,            /*!< Interpolate by 3 */
    ADI_APOLLO_CDUC_INTERP_4  = 0x4,            /*!< Interpolate by 4 */
    ADI_APOLLO_CDUC_INTERP_6  = 0x6,            /*!< Interpolate by 6 */
    ADI_APOLLO_CDUC_INTERP_8  = 0x8,            /*!< Interpolate by 8 */
    ADI_APOLLO_CDUC_INTERP_12 = 0xC,            /*!< Interpolate by 12 */
} ADI_APOLLO_PACK_ENUM adi_apollo_coarse_duc_dcm_e;

/*!
 * \brief Enumerates Fine DDC (digital down convertor) ratio
 */
typedef enum
{
    ADI_APOLLO_FDDC_RATIO_1 = 0,
    ADI_APOLLO_FDDC_RATIO_2,
    ADI_APOLLO_FDDC_RATIO_4,
    ADI_APOLLO_FDDC_RATIO_8,
    ADI_APOLLO_FDDC_RATIO_16,
    ADI_APOLLO_FDDC_RATIO_32,
    ADI_APOLLO_FDDC_RATIO_64
} ADI_APOLLO_PACK_ENUM adi_apollo_fddc_ratio_e;

/*!
 * \brief Enumerates Fine DUC (digital up convertor) ratio
 */
typedef enum
{
    ADI_APOLLO_FDUC_RATIO_1  = 1,
    ADI_APOLLO_FDUC_RATIO_2  = 2,
    ADI_APOLLO_FDUC_RATIO_4  = 4,
    ADI_APOLLO_FDUC_RATIO_8  = 8,
    ADI_APOLLO_FDUC_RATIO_16 = 16,
    ADI_APOLLO_FDUC_RATIO_32 = 32,
    ADI_APOLLO_FDUC_RATIO_64 = 64
} ADI_APOLLO_PACK_ENUM adi_apollo_fduc_ratio_e;

/*!
 * \brief Enumerates nco mixer modes
 */
typedef enum
{
    ADI_APOLLO_MXR_VAR_IF_MODE = 0,        /*!< Variable IF Mode */
    ADI_APOLLO_MXR_ZERO_IF_MODE = 1,       /*!< Zero IF Mode */
    ADI_APOLLO_MXR_FS_BY_4_MODE = 2,       /*!< Fs/4 Hz IF Mode */
    ADI_APOLLO_MXR_TEST_MODE = 3           /*!< Test Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_mixer_mode_e;

/*!
 * \brief Enumerates integer and integer plus modulus modulus
 */
typedef enum
{
    ADI_APOLLO_NCO_MOD_INTEGER,
    ADI_APOLLO_NCO_MOD_INT_PLUS_MODULUS
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_mode_e;

/*!
 * \brief Select nco phase increment or phase offset word
 */
typedef enum {
    ADI_APOLLO_NCO_PROFILE_PHASE_INCREMENT = 0,                     /*!< Phase increment word */
    ADI_APOLLO_NCO_PROFILE_PHASE_OFFSET = 1,                        /*!< Phase offset word */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_profile_word_sel_e;

/*!
* \brief Enumerates fnco profile chan selection mode
*/
typedef enum {
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_AUTO      = 0x00,                 /*!< Trigger based hopping. Auto Hopping Mode */
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_REGMAP    = 0x01,                 /*!< Trigger based hopping. Scheduled Regmap  */
    ADI_APOLLO_NCO_CHAN_SEL_TRIG_GPIO      = 0x02,                 /*!< Trigger based hopping. Scheduled GPIO */
    ADI_APOLLO_NCO_CHAN_SEL_DIRECT_GPIO    = 0x03,                 /*!< Direct GPIO profile select. All params hop together */
    ADI_APOLLO_NCO_CHAN_SEL_DIRECT_REGMAP  = 0x04                  /*!< Direct spi/hsci nco profile select. All params hop together */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_profile_sel_mode_e;

/*!
* \brief Enumerates fnco trigger hop select
*/
typedef enum {
    ADI_APOLLO_FNCO_TRIG_HOP_FREQ       = 0x01,                     /*!< Hop freq in trigger based hopping */
    ADI_APOLLO_FNCO_TRIG_HOP_PHASE      = 0x04,                     /*!< Hop phase in trigger based hopping */
    ADI_APOLLO_FNCO_TRIG_HOP_FREQ_PHASE = 0x05,                     /*!< Hop freq and phase in trigger based hopping */
} ADI_APOLLO_PACK_ENUM adi_apollo_fnco_trig_hop_sel_e;

/*!
* \brief NCO next hop select mode. Applicable for trigger based auto hop.
*/
typedef enum {
    ADI_APOLLO_NCO_AUTO_HOP_DIR_DECR    = 0x0,       /*!< Decrement profile num on trigger */
    ADI_APOLLO_NCO_AUTO_HOP_DIR_INCR    = 0x1,       /*!< Increment profile num on trigger */
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_auto_hop_dir_sel_e;

/*!
* \brief Enumerates DRC mixer real or complex selection
*/
typedef enum {
    ADI_APOLLO_DRC_MIXER_REAL      = 0x0,       /*!< Real Mixing Enable */
    ADI_APOLLO_DRC_MIXER_COMPLEX   = 0x1,       /*!< Complex Mixing Enable */
} ADI_APOLLO_PACK_ENUM adi_apollo_drc_mixer_sel_e;

/*!
* \brief Enumerates CDRC TB1 filter delay
*/
typedef enum {
    ADI_APOLLO_CDRC_TB1_FILT_DLY_NONE       = 0,  /*!< No filter delay */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_1    = 1,  /*!< Delay 1 filter stage */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_2    = 2,  /*!< Delay 2 filter stages */
    ADI_APOLLO_CDRC_TB1_FILT_DLY_STAGE_3    = 3,  /*!< Delay 3 filter stages */
} ADI_APOLLO_PACK_ENUM adi_apollo_cdrc_tb1_filt_dly_e;

/*!
 * \brief Enumerates complex mixer multiplication scale
 */
typedef enum
{
    ADI_APOLLO_CMPLX_MULT_SCALE_0p7 = 0,
    ADI_APOLLO_CMPLX_MULT_SCALE_1
} ADI_APOLLO_PACK_ENUM adi_apollo_nco_cmplx_mult_scale_e;

/*!< CNCO Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cnco_cfg
{
    bool                              drc_en;                                 /*!< CDRC enable */
    adi_apollo_nco_mode_e             nco_mode;                               /*!< NCO integer or integer plus modulus */

    /* Register name in Yoda: coarse_drc_mixer_ctrl */
    adi_apollo_nco_mixer_mode_e       nco_if_mode;                            /*!< NCO mixer mode. var-if, zero-if, fs-by-4, dc-test */
    adi_apollo_drc_mixer_sel_e        drc_mxr_sel;                            /*!< Real or complex mixing select */
    adi_apollo_nco_cmplx_mult_scale_e cmplx_mxr_mult_scale_en;                /*!< NCO complex multiplication scale */
    uint16_t                          dc_testmode_value;                      /*!< Mixer test mode value */

    uint32_t                          nco_phase_inc;                          /*!< NCO phase increment word */
    uint32_t                          nco_phase_offset;                       /*!< NCO phase offset word */
    uint32_t                          nco_phase_inc_frac_a;                   /*!< Modulus phase increment numerator. */
    uint32_t                          nco_phase_inc_frac_b;                   /*!< Modulus phase increment denominator. */
    uint32_t                          nco_phase_inc_words[ADI_APOLLO_CNCO_PROFILE_NUM];     /*!< NCO phase increment words */
    uint32_t                          nco_phase_offset_words[ADI_APOLLO_CNCO_PROFILE_NUM];  /*!< NCO phase offset words */

    bool                              amp_dither_en;                         /*!< Amplitude Dither Enable, False: Disable, True: Enabled. */
    bool                              phase_dither_en;                       /*!< Phase Dither Enable, False: Disable, True: Enabled. */

    /* Hopping and Trigger */
    adi_apollo_nco_profile_sel_mode_e nco_profile_sel_mode;                   /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_hop_dir_sel_e nco_auto_inc_dec;                       /*!< Increment or decrement profile selection for auto hop. */
    bool                              debug_cdrc_clkoff_n;                    /*!< if TRUE (typical), enable nco clocks */   

} adi_apollo_cnco_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!
 * \brief Enumerates fine nco hop mode
 */
    typedef enum
{
    ADI_APOLLO_FNCO_HOP_MODE_STANDARD = 0,
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP,
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP_JMP,
    ADI_APOLLO_FNCO_HOP_MODE_FRAMP_JMP_ARAMP
} ADI_APOLLO_PACK_ENUM adi_apollo_fnco_hop_mode_e;

/*!< FNCO Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fnco_cfg
{
    bool                              drc_en;                   /*!< FDRC enable */
    adi_apollo_nco_mode_e             nco_mode;                 /*!< NCO integer or integer plus modulus */

    /* Register name in Yoda: fine_mixer_ctrl */
    adi_apollo_nco_mixer_mode_e       nco_if_mode;              /*!< NCO mixer mode. var-if, zero-if, fs-by-4, dc-test */
    adi_apollo_drc_mixer_sel_e        drc_mxr_sel;              /*!< Real or complex mixing select */
    adi_apollo_nco_cmplx_mult_scale_e cmplx_mxr_mult_scale_en;  /*!< NCO complex multiplication scale */
    uint16_t                          dc_testmode_value;        /*!< Mixer test mode value */

    uint64_t                          nco_phase_inc;            /*!< NCO phase increment word */
    uint64_t                          nco_phase_offset;         /*!< NCO phase offset word */
    uint64_t                          nco_phase_inc_frac_a;     /*!< Modulus phase increment numerator. */
    uint64_t                          nco_phase_inc_frac_b;     /*!< Modulus phase increment denominator. */

    bool                              amp_dither_en;            /*!< Amplitude Dither Enable, False: Disable, True: Enabled. */
    bool                              phase_dither_en;          /*!< Phase Dither Enable, False: Disable, True: Enabled. */

    bool                              hop_mode_en;              /*!< If TRUE, enable freq hopping. */
    uint8_t                           hop_mode;                 /*!< Fine NCO hop mode. */
    
    uint32_t                          nco_phase_inc_words[ADI_APOLLO_FNCO_PROFILE_NUM];       /*!< NCO phase increment words */
    uint32_t                          nco_phase_offset_words[ADI_APOLLO_FNCO_PROFILE_NUM];    /*!< NCO phase offset words */

    /* Hopping and Trigger */
    adi_apollo_nco_profile_sel_mode_e nco_profile_sel_mode;     /*!< Profile selection mode (e.g. direct or trigger based) */
    adi_apollo_nco_auto_hop_dir_sel_e nco_auto_inc_dec_freq;    /*!< Increment or decrement freq profile selection for auto hop. */
    adi_apollo_nco_auto_hop_dir_sel_e nco_auto_inc_dec_phase;   /*!< Increment or decrement phase profile selection for auto hop. */
    adi_apollo_fnco_trig_hop_sel_e    nco_trig_hop_sel;         /*!< Select params to change on hop. Freq, phase or both */
    bool                              debug_fdrc_clkoff_n;      /*!< if TRUE (typical), enable nco clocks */
} adi_apollo_fnco_cfg_t;
ADI_APOLLO_PACK_FINISH


/*!
 * \brief Enumerates tb1 filter delay setting
 */
    typedef enum
{
    ADI_APOLLO_TB1_FILT_DLY_DISABLED = 0,
    ADI_APOLLO_TB1_FILT_DLY_1CYCLE = 1,
    ADI_APOLLO_TB1_FILT_DLY_2CYCLES = 2
} ADI_APOLLO_PACK_ENUM adi_apollo_tb1_filt_dly_cycles_e;

/*!< CDDC Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cddc_cfg
{
    adi_apollo_cnco_cfg_t               nco[ADI_APOLLO_NUM_NCOS_PER_DRC];    /*!< NCO corresponding to CDDC */
    adi_apollo_coarse_ddc_dcm_e         drc_ratio;
    bool                                fine_bypass;                         /*!< true = bypass the fine DDC. */
    uint8_t                             link_num;                            /*!< Jesd link num CDDC is connected to (when fddc is bypassed) */
    bool                                hb1_filt_dly_en;                     /*!< if TRUE, HB1 filter uses a delayed input */
    bool                                hb2_filt_dly_en;                     /*!< if TRUE, HB2 filter uses a delayed input */
    adi_apollo_tb1_filt_dly_cycles_e    tb1_filt_dly_cycles;                 /*!< 0 = No Delay, 1 = 1 Cycle, 2 = 2 Cycles */
    bool                                hb1_gain_6db_en;                     /*!< if TRUE, the 6dB gain for HB1 filter is enabled */
    bool                                tb1_gain_6db_en;                     /*!< if TRUE, the 6dB gain for TB1 filter is enabled */

    bool                                trig_mst_en;                         /*!< true to enable the trigger master, false to disable */
    uint64_t                            trig_mst_period;                     /*!< Trigger master period, in units of Fs. Used w/ profile sel timer */
    uint64_t                            trig_mst_offset;                     /*!< Trigger master offset, in units of Fs. Used w/ profile sel timer */
    uint8_t                             debug_cddc_clkoff_n;                 /*!< If this bit is zero the clocks are shutoff.bit 0 : filter input clock bit 1 : filter output clockbit 2:  hb1 clockbit 3: hb2 clockbit 4: reservedbit 5: reservedbit 6: tb1 clock */
    // Trigger sel mux: external, internal or spi (see txrx_trigger_ts, reg: TRIG_SEL_MUX_CDRC0n amd TRIG_SEL_MUX_CDRC1n)

} adi_apollo_cddc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Apollo coarse duc config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cduc_cfg
{
    adi_apollo_cnco_cfg_t       nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to CDUC */
    adi_apollo_coarse_duc_dcm_e drc_ratio;
} adi_apollo_cduc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Apollo fine ddc config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fddc_cfg
{
    adi_apollo_fnco_cfg_t       nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to FDDC */
    adi_apollo_fddc_ratio_e     drc_ratio;

    uint8_t                     link_num;                           /*!< Jesd link num FDDC is connected */
    uint8_t                     debug_fddc_clkoff_n;                /*!< 0xff (typical) to enable ddc clks */
    bool                        hb1_filt_dly_en;                    /*!< if TRUE, HB1 filter uses a delayed input */
    bool                        hb2_filt_dly_en;                    /*!< if TRUE, HB2 filter uses a delayed input */
    bool                        hb3_filt_dly_en;                    /*!< if TRUE, HB3 filter uses a delayed input */
    bool                        hb4_filt_dly_en;                    /*!< if TRUE, HB4 filter uses a delayed input */
    bool                        hb5_filt_dly_en;                    /*!< if TRUE, HB5 filter uses a delayed input */
    bool                        hb6_filt_dly_en;                    /*!< if TRUE, HB6 filter uses a delayed input */
    bool                        hb1_gain_6db_en;                    /*!< if TRUE, the 6dB gain for HB1 filter is enabled */
} adi_apollo_fddc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Apollo fine duc config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fduc_cfg
{
    adi_apollo_fnco_cfg_t       nco[ADI_APOLLO_NUM_NCOS_PER_DRC];   /*!< NCO corresponding to FDUC */
    adi_apollo_fduc_ratio_e     drc_ratio;

    bool                        sub_dp_gain_en;                     /*!< FDUC input gain control enable */
    uint16_t                    subdp_gain;                         /*!< FDUC sub data path gain */
} adi_apollo_fduc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Programmable filter (PFLT) modes */
typedef enum
{
    ADI_APOLLO_PFILT_MODE_DISABLED = 0,             /*!< Disabled (filters bypassed) */
    ADI_APOLLO_PFILT_MODE_N_DIV_BY_4_REAL = 1,      /*!< Real N/4 Tap Filter for the I/Q channel */
    ADI_APOLLO_PFILT_MODE_N_DIV_BY_2_REAL = 2,      /*!< Real N/2 Tap Filter for the I/Q channel */
    ADI_APOLLO_PFILT_MODE_MATRIX = 4,               /*!< Real N/4 tap Matrix mode of operation (pfilt_q_mode/pfilt_i_mode must also be set to 100) */
    ADI_APOLLO_PFILT_MODE_HALF_COMPLEX = 6,         /*!< Half Complex Filter using N/2-Tap Filters for the Q/I channel + N/2 Tap Programmable Delay Line for the I/Q Channel */
    ADI_APOLLO_PFILT_MODE_N_REAL = 7,               /*!< Real N Tap Filter for the I/Q (pfilt_q_mode/pfilt_i_mode must be set to 000 */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_e;

/*! Programmable filter possible gain values */
typedef enum
{
    ADI_APOLLO_PFILT_GAIN_ZERO_DB = 0,
    ADI_APOLLO_PFILT_GAIN_POS_6_DB = 1,
    ADI_APOLLO_PFILT_GAIN_POS_12_DB = 2,
    ADI_APOLLO_PFILT_GAIN_POS_18_DB = 3,
    ADI_APOLLO_PFILT_GAIN_POS_24_DB = 4,
    ADI_APOLLO_PFILT_GAIN_NEG_24_DB = 12,
    ADI_APOLLO_PFILT_GAIN_NEG_18_DB = 13,
    ADI_APOLLO_PFILT_GAIN_NEG_12_DB = 14,
    ADI_APOLLO_PFILT_GAIN_NEG_6_DB = 15
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_gain_e;

/*! Programmable filter has four different sets of coefficients: */
typedef enum
{
    ADI_APOLLO_PFILT_COEFF_SET_0 = 0,
    ADI_APOLLO_PFILT_COEFF_SET_1,
    ADI_APOLLO_PFILT_COEFF_SET_2,
    ADI_APOLLO_PFILT_COEFF_SET_3,
    ADI_APOLLO_PFILT_COEFF_SETS
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_coeffs_sets_per_prfl_e;

/*!
* \brief Enumerates PFLIT mode selection
*/
typedef enum {
    ADI_APOLLO_PFILT_DUAL_MODE     = 0x0,               /*!< PFILT DUAL MODE */
    ADI_APOLLO_PFILT_QUAD_MODE     = 0x1,               /*!< PFILT QUAD MODE */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_dq_mode_e;

/*!
* \brief Enumerates PFLIT data selection
*/
typedef enum {
    ADI_APOLLO_PFILT_COMPLEX_DATA     = 0x0,            /*!< PFILT COMPLEX DATA */
    ADI_APOLLO_PFILT_REAL_DATA        = 0x1,            /*!< PFILT REAL DATA */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_data_e;

/*!
* \brief Enumerates PFLIT mode switch add/subract selection
*/
typedef enum {
    ADI_APOLLO_PFILT_SUB_FOR_MOD_SW     = 0x0,                 /*!< Select subtraction operation */
    ADI_APOLLO_PFILT_ADD_FOR_MOD_SW     = 0x1,                 /*!< Select addition operation */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_sw_add_sub_e;

/*!
* \brief Enumerates PFILT mode sw 3dB averaging enable/disable
*/
typedef enum {
    ADI_APOLLO_PFILT_DISABLE_3DB_AVG_MOD_SW     = 0,        /*!< Disable Mode Switch (3dB Average). Only for Rx PFILT */
    ADI_APOLLO_PFILT_ENABLE_3DB_AVG_MOD_SW      = 1,        /*!< Enable Mode Switch (3dB Average). Only for Rx PFILTd */
} ADI_APOLLO_PACK_ENUM adi_apollo_pfilt_mode_sw_ave_en_e;

/*! Apollo programmable filter config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_pfilt_cfg
{
    bool                    enable;                                                         /*!< Filter enable */

    uint16_t                coeffs[ADI_APOLLO_PFILT_COEFF_SETS][ADI_APOLLO_PFILT_COEFFS];

    adi_apollo_pfilt_mode_e i_mode;                                                         /*!< Filter mode for i stream */
    adi_apollo_pfilt_mode_e q_mode;                                                         /*!< Filter mode for q stream */

    adi_apollo_pfilt_data_e real_data;                                                      /*!< Selects real or complex data streams */
    adi_apollo_pfilt_dq_mode_e dq_mode;                                                     /*!< Dual(4t4r) or Quad(8t8r) filter mode */
    adi_apollo_pfilt_mode_sw_add_sub_e add_sub_sel;                                         /*!< Add/Sub for Mode Switch (3dB Average) */
    adi_apollo_pfilt_mode_sw_ave_en_e mode_switch;                                          /*!< Enable for 3dB Averaging (Mode Switch) for Rx PFILT */

    adi_apollo_pfilt_gain_e pfir_ix_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain */
    adi_apollo_pfilt_gain_e pfir_iy_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. Only used in matrix mode */
    adi_apollo_pfilt_gain_e pfir_qx_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. */
    adi_apollo_pfilt_gain_e pfir_qy_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];                   /*!< Shift gain. Only used in matrix mode */

    uint8_t                 pfir_ix_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain */
    uint8_t                 pfir_iy_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain. Only used in matrix mode */
    uint8_t                 pfir_qx_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain */
    uint8_t                 pfir_qy_scalar_gain_db[ADI_APOLLO_PFILT_COEFF_SETS];            /*!< Scalar gain. Only used in matrix mode */

    uint8_t                 hc_prog_delay[ADI_APOLLO_PFILT_COEFF_SETS];                     /*!<  Programmable delay line */
} adi_apollo_pfilt_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Complex filter profiles */
typedef enum
{
    ADI_APOLLO_CFIR_PROFILE_IDX_0 = 0,
    ADI_APOLLO_CFIR_PROFILE_IDX_1,
    ADI_APOLLO_CFIR_NUM_PROFILES
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_profiles_e;

/*! Each profile in complex filter has four sets of coefficients */
typedef enum
{
    ADI_APOLLO_CFIR_COEFF_SET_0 = 0,
    ADI_APOLLO_CFIR_COEFF_SET_1,
    ADI_APOLLO_CFIR_COEFF_SET_2,
    ADI_APOLLO_CFIR_COEFF_SET_3,
    ADI_APOLLO_CFIR_COEFF_SETS
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_coeffs_sets_per_prfl_e;

/*! Complex filter possible gain values */
typedef enum
{
    ADI_APOLLO_CFIR_GAIN_MINUS18_DB = 0,
    ADI_APOLLO_CFIR_GAIN_MINUS12_DB,
    ADI_APOLLO_CFIR_GAIN_MINUS6_DB,
    ADI_APOLLO_CFIR_GAIN_ZERO_DB,
    ADI_APOLLO_CFIR_GAIN_PLUS6_DB,
    ADI_APOLLO_CFIR_GAIN_PLUS12_DB,
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_gain_e;

/*! Single or dual band mode for complex filter */
typedef enum
{
    ADI_APOLLO_CFIR_SINGLE_BAND,
    ADI_APOLLO_CFIR_DUAL_BAND
} ADI_APOLLO_PACK_ENUM adi_apollo_cfir_sing_dual_mode_e;

/*! Apollo complex filter config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_cfir_cfg
{
    uint16_t                         coeffs_i[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_NUM_TAPS];
    uint16_t                         coeffs_q[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_NUM_TAPS];
    adi_apollo_cfir_gain_e           cfir_gain_dB[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];
    uint16_t                         scalar_i[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];
    uint16_t                         scalar_q[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS];
    bool                             enable;        /*!<  High-level enable for complex filter */
    bool                             sparse_mode;   /*!<  cfir_sparse_filt_en */
    adi_apollo_cfir_sing_dual_mode_e cfir_mode;     /*!<  CFIR single/dual mode */
} adi_apollo_cfir_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Fractional sampling rate converter configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_fsrc_cfg
{
    uint64_t   fsrc_rate_int;                          /*!<   Rate, integer part (48 bits). */
    uint64_t   fsrc_rate_frac_a;                       /*!<   Rate, numerator of fractional part (48 bits). */
    uint64_t   fsrc_rate_frac_b;                       /*!<   Rate, denominator of fractional part (48 bits). */
    uint16_t   fsrc_delay;                             /*!<   Sample fractional delay. */
    uint16_t   gain_reduction;                         /*!<   12 bit value */
    uint8_t    ptr_syncrstval;                         /*!<   6 bit value */
    bool       ptr_overwrite;
    bool       data_mult_dither_en;
    bool       dither_en;
    bool       split_4t4r;
    bool       mode_1x;
    bool       enable;
} adi_apollo_fsrc_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Dynamic Reconfig setup configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dyn_recfg_cfg
{
    uint16_t prefsrc_lcm;        /*!<  9 bit value  */
    uint16_t postfsrc_lcm;       /*!<  16 bit value */
    bool     trig_recfg_mode;    /*!<  0 == Sync Mode, 1 == Trig Mode */
    bool     cnco_rst_en;        /*!<  0 == Disable, 1 == Enable */
    bool     fnco_rst_en;        /*!<  0 == Disable, 1 == Enable */
    bool     resync_en;          /*!<  0 == Disable, 1 == Enable */
    bool     ts_rst_en;          /*!<  0 == Disable, 1 == Enable */
    bool     tzero_coher_en;     /*!<  0 == Disable, 1 == Enable */
} adi_apollo_dyn_recfg_cfg_t;
ADI_APOLLO_PACK_FINISH

#define ADI_APOLLO_FDUC0_REPRESENTATION_IN_CDUC_SUMMER       0x01        /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC1_REPRESENTATION_IN_CDUC_SUMMER       0x02        /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC2_REPRESENTATION_IN_CDUC_SUMMER       0x04        /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC3_REPRESENTATION_IN_CDUC_SUMMER       0x08        /*!<  Both 4T4R and 8T8R */
#define ADI_APOLLO_FDUC4_REPRESENTATION_IN_CDUC_SUMMER       0x10        /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC5_REPRESENTATION_IN_CDUC_SUMMER       0x20        /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC6_REPRESENTATION_IN_CDUC_SUMMER       0x40        /*!<  Only 8T8R */
#define ADI_APOLLO_FDUC7_REPRESENTATION_IN_CDUC_SUMMER       0x80        /*!<  Only 8T8R */

/*! Mode switch for CDUC to PFILT configuration values - 4T4R, Only modsw0 bitfield is used */
typedef enum
{
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0_OUT1_FROM_I1 = 0,
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0I1_OUT1_FROM_Q0Q1,
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I0_OUT1_FROM_Q0,
    ADI_APOLLO_4T4R_MODSW0_OUT0_FROM_I1_OUT1_FROM_Q1,

    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0_OUT1_FROM_I2 = 0,
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0I2_OUT1_FROM_Q0Q2,
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I0_OUT1_FROM_Q0,
    ADI_APOLLO_8T8R_MODSW0_OUT0_FROM_I1_OUT1_FROM_Q2,

    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1_OUT3_FROM_I3 = 0,
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1I3_OUT3_FROM_Q1Q3,
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I1_OUT3_FROM_Q1,
    ADI_APOLLO_8T8R_MODSW1_OUT2_FROM_I3_OUT3_FROM_Q3,
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux1_sel_e;

typedef enum
{
    ADI_APOLLO_4T4R_MODSW0 = 0,
    ADI_APOLLO_8T8R_MODSW0 = 0,
    ADI_APOLLO_8T8R_MODSW1 = 1,
    ADI_APOLLO_TX_MUX1_NUM_4T4R = 1,
    ADI_APOLLO_TX_MUX1_NUM_8T8R = 2 /* Both modsw0 and modsw1 bit fields are in register dp_cfg */
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux1_idx_e;

/*! High-speed cross bar control on Tx direction */
typedef enum
{
    ADI_APOLLO_4T4R_DAC_FROM_MODSW0_OUT0 = 0,
    ADI_APOLLO_4T4R_DAC_FROM_MODSW0_OUT1,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW0_OUT0 = 0,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW0_OUT1,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW1_OUT0,
    ADI_APOLLO_8T8R_DAC_FROM_MODSW1_OUT1,
    ADI_APOLLO_4T4R_LOOPBACK_DAC_FROM_ADC0 = 0,
    ADI_APOLLO_4T4R_LOOPBACK_DAC_FROM_ADC1,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC0 = 0,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC1,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC2,
    ADI_APOLLO_8T8R_LOOPBACK_DAC_FROM_ADC3,
} ADI_APOLLO_PACK_ENUM adi_apollo_tx_mux0_sel_e;

/*!< Tx Path Misc Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_txpath_misc
{
    uint8_t                   cduc_dac_enables[ADI_APOLLO_CDUC_PATHS_PER_SIDE];

    /*!<   FDUC to CDUC summer blocks.
           For instance, if fduc_cduc_summer[0] = FDUC0_REPRESENTATION_IN_CDUC_SUMMER + FDUC1_REPRESENTATION_IN_CDUC_SUMMER,
           Then the CDUC 0 input will be the sum of FDUC 0 and FDUC 1. In Yoda, the registers are enables00, enables01, enables10, enables11 */
    uint8_t                   fduc_cduc_summer[ADI_APOLLO_CDUC_PATHS_PER_SIDE];

    adi_apollo_tx_mux1_sel_e  mux1_sel[ADI_APOLLO_TX_MUX1_NUM_8T8R];

    adi_apollo_tx_mux0_sel_e  mux0_sel[ADI_APOLLO_CDUC_PATHS_PER_SIDE];    /*!< Each of these occupy two bits in register hs_xbar_ctrl */

    bool                      low_samp_en;      /*!<  Tx_Top.Tx_Misc.low_samp, TRUE: enable low sampling */
} adi_apollo_txpath_misc_t;
ADI_APOLLO_PACK_FINISH

/*!< PAProt Power Detector Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_pwr_cfg
{
    bool        pa_clk_en;              /*!<   True to enable when TxEN is low */
    bool        short_pac_en;           /*!<   True to enable */
    bool        long_pac_en;            /*!<   True to enable */
    uint16_t    pwr_threshold_long;     /*!<   13Bits for the long window average power threshold */
    uint16_t    pwr_threshold_short;    /*!<   13Bits for the short window average power threshold */
    uint8_t     avg_long_win;           /*!<   4Bits for 2 ** (9+avg_long_win) sample count */
    uint8_t     avg_short_win;          /*!<   2Bits for 2 ** (3+avg_short_win) sample count */
} adi_apollo_paprot_pwr_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! PA Protect Zero Flush from PA to FDUC Select */
typedef enum
{
    ADI_APOLLO_PAPROT_ZF_SEL_F0_P0 = 0,
    ADI_APOLLO_PAPROT_ZF_SEL_F2_P0,
    ADI_APOLLO_PAPROT_ZF_SEL_F1_P0,
    ADI_APOLLO_PAPROT_ZF_SEL_F3_P0,
} ADI_APOLLO_PACK_ENUM adi_apollo_paprot_zf_sel_e;

/*! PA Protect JESD and Data Ready Select */
typedef enum
{
    ADI_APOLLO_PAPROT_JESD_DF_SEL_L0 = 0,
    ADI_APOLLO_PAPROT_JESD_DF_SEL_L1,
    ADI_APOLLO_PAPROT_JESD_DF_SEL_BOTH,
} ADI_APOLLO_PACK_ENUM adi_apollo_paprot_jesd_dr_sel_e;

/*!< PAProt State Machine Control Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_sm_cfg
{
    bool                            enable_paprotsm;                    /*!<   1: Enable the PA protection state machine. The PA protection fsm is the main control of the PA protection algorithm. It monitors alarms and initiate ramp up/down when necessary. */
    bool                            rampdwn_to_papin_en;                /*!<   1: Will bring the RAMP down signal out on PA_EN pins. 0: The PA_EN signal will come out of txen power ctrl machine */
    bool                            srl_err_en;                         /*!<   1: Enable the slew rate errors to trigger an alarm when the measured slew rate is higher than a threshold. 0: will not trigger a ramp down. */
    bool                            avg_pwr_err_en;                     /*!<   1: Enable the average power to trigger an alarm when measured power is higher than a threshold. 0: will not trigger a ramp down. */
    bool    	                    auto_dyn_reconf_en;                 /*!<   1: dyn reconfig triggers ramps. 0: dyn reconfig would not trigger the ramps. */
    bool     	                    jesd_err_en;                        /*!<   1: Enable the JESD errors to trigger an alarm. */
    bool                            jesd_err_auto_config_en;            /*!<   Allow the JESD errors to be automatically routed to different RAMPS based on JESD parameters. */
    bool                            data_ready_auto_config_en;          /*!<   Allow the data_ready to be automatically routed to different RAMPS based on JESD parameters. */
    bool                            zero_flush_clock_stable_rise_en;    /*!<   Enable Zero Flushing On Rising Edge of Clock Stable. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_dyn_reconf_ramp_up_en;   /*!<   Enable Zero Flushing Before Ramp Up Via Dynamic Reconfiguration. PAPROT SM Must be Enabled. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_txen_fall_en;            /*!<   Enable Zero Flushing on Falling Edge of TXEN. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_txen_rise_en;            /*!<   Enable Zero Flushing on Rising Edge of TXEN. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            zero_flush_start_en;                /*!<   Enable Zero Flushing After Initial Power Up and Clock Synchronization Sequence. Zero Flushing Will Stop After ZERO_FLUSH_TIMER Clock Cycles. */
    bool                            drive_sm_by_dig_en;                 /*!<   1: dig_en from txen power control is driving the PA protect SM. 0: TXEN from pins will be driving the SM. */
    bool                            hold_sample_en;                     /*!<   1: Enable the function that will allow user to hold the last good sample. */
    bool                            skip_ramp_up;                       /*!<   1: The ramp up event in the SM will be disabled i.e. skip the ramp up state and jump to operation state. */
    bool                            skip_ramp_down;                     /*!<   1: The ramp down event in the SM will be disabled i.e. skip the ramp down state and jump to operation state. */
    bool                            ramp_up_timer_en;                   /*!<   Enable the ramp up timer. */
    bool                            ramp_dwn_timer_en;                  /*!<   Enable the ramp down timer. */
    uint16_t                        zero_flush_timer;                   /*!<   13Bits Zero Flush timer */
    uint16_t                        ramp_down_timer_val;                /*!<   16Bits Ramp Down timer */
    uint16_t                        ramp_up_timer_val;                  /*!<   16Bits Ramp Up timer */
    adi_apollo_paprot_zf_sel_e      dp_zero_flush_sel_0;                /*!<   Select Zero Flush from PA to FDUC Mapping. */
    adi_apollo_paprot_zf_sel_e      dp_zero_flush_sel_1;                /*!<   Select Zero Flush from PA to FDUC Mapping. */
    adi_apollo_paprot_jesd_dr_sel_e data_ready_sel_ramp;                /*!<   The select bit for mapping data_ready[1:0] to the different RAMP paths. */
    adi_apollo_paprot_jesd_dr_sel_e jesd_err_sel_ramp;                  /*!<   The select bit for mapping jesd_err[1:0] to the different RAMP paths. */
} adi_apollo_paprot_sm_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< PAProt Ramp Control Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_ramp_cfg
{
    bool        gain_clk_en;                /*!<   True to enable when TxEN is low */
    bool        zero_detect_en;             /*!<   Enable the ramp up only when non-zero data is detected and this bit is enabled. */
    bool        force_ramps_dyn_recfig_en;  /*!<   Enable the ramps from dynamic reconfig controller bypassing PA protect SM. */
    bool        force_ramp_up_en;           /*!<   Enable the manual forcing of a ramp up. */
    bool        force_ramp_dwn_en;          /*!<   Enable the manual forcing of a ramp down. */
    bool        enable_gain;                /*!<   True to enable the gain */
    bool        enable_ramp;                /*!<   Enable the ramping up and down of the gain steps. When this bit is set to 0 the multiplier gain value is fixed to the programmed GAIN_VALUE. When the ramp_en is asserted the gain block is automatically enabled. */
    bool        trig_ramp_together;         /*!<   Enable simultaneous ramp up and ramp down of IQ pairs. */
    uint8_t     gain_inc_step;              /*!<   8Bits for the gain increase step for PA protection RAMP up process. */
    uint8_t     gain_dec_step;              /*!<   8Bits for the gain decrease step for PA protection RAMP down process. */
    uint8_t     gain_max_val;               /*!<   8Bits for the maximum gain value that data will be multiplied w/ after ramp up is completed ie: DAC = data*max_gain. */
} adi_apollo_paprot_ramp_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< PAProt Slew Rate Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_sr_cfg
{
    bool        sr_clk_en;              /*!<   True to enable when TxEN is low */
    bool        sr_err_self_clr_en;     /*!<   True to enable the slew rate detector error self clear */
    bool        sr_calculation_en;      /*!<   True to enable slew rate calculations */
    bool        slew_rate_err_irq_en;   /*!<   True to enable slew rate error IRQ */
    uint8_t     sr_path_sel;            /*!<   5Bits for the which of the 32 calculated values are read back */
    uint32_t    sr_threshold;           /*!<   17Bits for the slew rate threshold */
} adi_apollo_paprot_sr_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< PA Protect Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_paprot_cfg
{
    bool                            pa_clkdiv_en;       /*!<   True to enable */
    adi_apollo_paprot_pwr_cfg_t     paprot_pwr_cfg;     /*!<   PAProt Power Detector Configuration */
    adi_apollo_paprot_sm_cfg_t      paprot_sm_cfg;      /*!<   PAProt State Machine Configuration */
    adi_apollo_paprot_ramp_cfg_t    paprot_ramp_cfg;    /*!<   PAProt Ramp Up/Down Configuration */
    adi_apollo_paprot_sr_cfg_t      paprot_sr_cfg;      /*!<   PAProt Slew Rate Configuration */
} adi_apollo_paprot_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! TX path configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_txpath
{
    adi_apollo_cduc_cfg_t        tx_cduc[ADI_APOLLO_CDUCS_PER_SIDE];            /*!<   Coarse digital up converter config */
    adi_apollo_fduc_cfg_t        tx_fduc[ADI_APOLLO_FDUCS_PER_SIDE];            /*!<   Fine digital up converter config */
    adi_apollo_pfilt_cfg_t       tx_pfilt[ADI_APOLLO_PFILTS_PER_SIDE];          /*!<   Programmable filters */
    adi_apollo_cfir_cfg_t        tx_cfir[ADI_APOLLO_CFIRS_PER_SIDE];            /*!<   Complex FIRs */
    adi_apollo_fsrc_cfg_t        tx_fsrc[ADI_APOLLO_FSRCS_PER_SIDE];            /*!<   Fractional sampling rate converters */
    adi_apollo_txpath_misc_t     tx_mux_summer_xbar;                            /*!<   Summers. multiplexers, crossbar config */
    adi_apollo_dyn_recfg_cfg_t   tx_dyn_recfg;                                  /*!<   Dynamic Reconfig Static settings */
    adi_apollo_paprot_cfg_t      tx_pa_prot[ADI_APOLLO_CDUC_PATHS_PER_SIDE];    /*!<   PA Protect configuration */
    bool                         inv_sinc_en[ADI_APOLLO_CDUC_PATHS_PER_SIDE];   /*!<   Inverse sinc enable */
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
typedef enum
{
    ADI_APOLLO_4T4R_CB_OUT_0_FROM_ADC0 = 0,                 /*!< 4T4R's first (side A or B) adc_mux_sel=00000000  */
    ADI_APOLLO_4T4R_CB_OUT_0_FROM_ADC1 = 1,                 /*!< 4T4R's first (side A or B) adc_mux_sel=00000001  */
    ADI_APOLLO_4T4R_CB_OUT_1_FROM_ADC1 = 0,                 /*!< 4T4R's second (side A or B) adc_mux_sel=00000000  */
    ADI_APOLLO_4T4R_CB_OUT_1_FROM_ADC0 = 1,                 /*!< 4T4R's second (side A or B) adc_mux_sel=00000001  */
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC0 = 0,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC1 = 2,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC2 = 1,
    ADI_APOLLO_8T8R_CB_OUT_FROM_ADC3 = 3
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux0_sel_e;

typedef enum
{
    ADI_APOLLO_4T4R_CB_OUT0 = 0,
    ADI_APOLLO_4T4R_CB_OUT1,
    ADI_APOLLO_8T8R_CB_OUT0 = 0,
    ADI_APOLLO_8T8R_CB_OUT2,
    ADI_APOLLO_8T8R_CB_OUT1,
    ADI_APOLLO_8T8R_CB_OUT3,
    ADI_APOLLO_RX_MUX0_NUM_4T4R = 2,
    ADI_APOLLO_RX_MUX0_NUM_8T8R = 4
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux0_idx_e;

/*! Rx Coarse fine crossbar (MUX2) configuration */ 
typedef enum
{
    /* values for FDDC0 (and FDDC4 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F0 = 0,          /*!< 4T4R: CDDC 0 goes to FDDC 0 */
    ADI_APOLLO_CB_4T4R_C1_TO_F0 = 4,          /*!< 4T4R: CDDC 1 goes to FDDC 0 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F0_F4 = 0,    /*!< 8T8R: CDDC 0 goes to FDDC 0 and CDDC 2 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F0_F4 = 1,    /*!< 8T8R: CDDC 2 goes to FDDC 0 and CDDC 0 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C0_TO_F0_F4 = 2,       /*!< 8T8R: CDDC 0 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C2_TO_F0_F4 = 3,       /*!< 8T8R: CDDC 2 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F0_F4 = 4,    /*!< 8T8R: CDDC 1 goes to FDDC 0 and CDDC 3 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F0_F4 = 5,    /*!< 8T8R: CDDC 3 goes to FDDC 0 and CDDC 1 goes to FDDC 4 */
    ADI_APOLLO_CB_8T8R_C1_TO_F0_F4 = 6,       /*!< 8T8R: CDDC 1 goes to both FDDC 0 and FDDC 4 */
    ADI_APOLLO_CB_8T8R_C3_TO_F0_F4 = 7,       /*!< 8T8R: CDDC 3 goes to both FDDC 0 and FDDC 4 */

    /* values for FDDC1 (and FDDC5 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F1 = 0,          /*!< 4T4R: CDDC 0 goes to FDDC 1 */
    ADI_APOLLO_CB_4T4R_C1_TO_F1 = 4,          /*!< 4T4R: CDDC 1 goes to FDDC 1 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F1_F5 = 0,    /*!< 8T8R: CDDC 0 goes to FDDC 1 and CDDC 2 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F1_F5 = 1,    /*!< 8T8R: CDDC 2 goes to FDDC 1 and CDDC 0 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C0_TO_F1_F5 = 2,       /*!< 8T8R: CDDC 0 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C2_TO_F1_F5 = 3,       /*!< 8T8R: CDDC 2 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F1_F5 = 4,    /*!< 8T8R: CDDC 1 goes to FDDC 1 and CDDC 3 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F1_F5 = 5,    /*!< 8T8R: CDDC 3 goes to FDDC 1 and CDDC 1 goes to FDDC 5 */
    ADI_APOLLO_CB_8T8R_C1_TO_F1_F5 = 6,       /*!< 8T8R: CDDC 1 goes to both FDDC 1 and FDDC 5 */
    ADI_APOLLO_CB_8T8R_C3_TO_F1_F5 = 7,       /*!< 8T8R: CDDC 3 goes to both FDDC 1 and FDDC 5 */

    /* values for FDDC2 (and FDDC6 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F2 = 0,          /*!< 4T4R: CDDC 0 goes to FDDC 2 */
    ADI_APOLLO_CB_4T4R_C1_TO_F2 = 4,          /*!< 4T4R: CDDC 1 goes to FDDC 2 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F2_F6 = 0,    /*!< 8T8R: CDDC 0 goes to FDDC 2 and CDDC 2 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F2_F6 = 1,    /*!< 8T8R: CDDC 2 goes to FDDC 2 and CDDC 0 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C0_TO_F2_F6 = 2,       /*!< 8T8R: CDDC 0 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C2_TO_F2_F6 = 3,       /*!< 8T8R: CDDC 2 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F2_F6 = 4,    /*!< 8T8R: CDDC 1 goes to FDDC 2 and CDDC 3 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F2_F6 = 5,    /*!< 8T8R: CDDC 3 goes to FDDC 2 and CDDC 1 goes to FDDC 6 */
    ADI_APOLLO_CB_8T8R_C1_TO_F2_F6 = 6,       /*!< 8T8R: CDDC 1 goes to both FDDC 2 and FDDC 6 */
    ADI_APOLLO_CB_8T8R_C3_TO_F2_F6 = 7,       /*!< 8T8R: CDDC 3 goes to both FDDC 2 and FDDC 6 */

    /* values for FDDC3 (and FDDC7 in 8T8R) */
    ADI_APOLLO_CB_4T4R_C0_TO_F3 = 0,          /*!< 4T4R: CDDC 0 goes to FDDC 3 */
    ADI_APOLLO_CB_4T4R_C1_TO_F3 = 4,          /*!< 4T4R: CDDC 1 goes to FDDC 3 */
    ADI_APOLLO_CB_8T8R_C0_C2_TO_F3_F7 = 0,    /*!< 8T8R: CDDC 0 goes to FDDC 3 and CDDC 2 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C2_C0_TO_F3_F7 = 1,    /*!< 8T8R: CDDC 2 goes to FDDC 3 and CDDC 0 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C0_TO_F3_F7 = 2,       /*!< 8T8R: CDDC 0 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C2_TO_F3_F7 = 3,       /*!< 8T8R: CDDC 2 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C1_C3_TO_F3_F7 = 4,    /*!< 8T8R: CDDC 1 goes to FDDC 3 and CDDC 3 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C3_C1_TO_F3_F7 = 5,    /*!< 8T8R: CDDC 3 goes to FDDC 3 and CDDC 1 goes to FDDC 7 */
    ADI_APOLLO_CB_8T8R_C1_TO_F3_F7 = 6,       /*!< 8T8R: CDDC 1 goes to both FDDC 3 and FDDC 7 */
    ADI_APOLLO_CB_8T8R_C3_TO_F3_F7 = 7,       /*!< 8T8R: CDDC 3 goes to both FDDC 3 and FDDC 7 */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux2_sel_e;

typedef enum
{
    ADI_APOLLO_4T4R_FDDC0 = 0,
    ADI_APOLLO_4T4R_FDDC1,
    ADI_APOLLO_4T4R_FDDC2,
    ADI_APOLLO_4T4R_FDDC3,
    ADI_APOLLO_8T8R_FDDC0_4 = 0,
    ADI_APOLLO_8T8R_FDDC1_5,
    ADI_APOLLO_8T8R_FDDC2_6,
    ADI_APOLLO_8T8R_FDDC3_7,
    ADI_APOLLO_RX_MUX2_NUM = 4
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_mux2_idx_e;

/*!< Rx Path Misc Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxpath_misc
{
    /*! ADC to CB_out multiplexer (RX MUX0) select. Register name for programming is adc_mux_sel */
    adi_apollo_rx_mux0_sel_e  mux0_out_adc_sel[ADI_APOLLO_RX_MUX0_NUM_8T8R];

    /*! Coarse to fine crossbar (RX MUX2) config */
    adi_apollo_rx_mux2_sel_e  mux2_fddc_input_sel[ADI_APOLLO_RX_MUX2_NUM];

    bool                      low_samp_en;      /*!<  Rx_Misc.low_samp, TRUE: enable low sampling */
} adi_apollo_rxpath_misc_t;
ADI_APOLLO_PACK_FINISH

typedef enum
{
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
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_DISABLE  = 0,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_0_ENABLE = 1,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_1_ENABLE = 2,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_2_ENABLE = 4,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_3_ENABLE = 8,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_4_ENABLE = 16,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_5_ENABLE = 32,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_6_ENABLE = 64,
    ADI_APOLLO_RX_4T4R_TMODE_SEL_CONV_7_ENABLE = 128,

    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_DISABLE   = 0,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_01_ENABLE = 1,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_23_ENABLE = 2,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_45_ENABLE = 4,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_67_ENABLE = 8,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_89_ENABLE = 16,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_AB_ENABLE = 32,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_CD_ENABLE = 64,
    ADI_APOLLO_RX_8T8R_TMODE_SEL_CONV_EF_ENABLE = 128,
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_sel_e;

typedef enum
{
    ADI_APOLLO_TMODE_TYPE_SEL_NORM = 0,
    ADI_APOLLO_TMODE_TYPE_SEL_MIDSCALE,
    ADI_APOLLO_TMODE_TYPE_SEL_POS_FS,
    ADI_APOLLO_TMODE_TYPE_SEL_NEG_FS,
    ADI_APOLLO_TMODE_TYPE_SEL_ACB,
    ADI_APOLLO_TMODE_TYPE_SEL_PN23,
    ADI_APOLLO_TMODE_TYPE_SEL_PN9,
    ADI_APOLLO_TMODE_TYPE_SEL_WT,
    ADI_APOLLO_TMODE_TYPE_SEL_USR,
    ADI_APOLLO_TMODE_TYPE_SEL_PN7,
    ADI_APOLLO_TMODE_TYPE_SEL_PN15,
    ADI_APOLLO_TMODE_TYPE_SEL_PN31,
    ADI_APOLLO_TMODE_TYPE_SEL_RAMP = 15,
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_type_sel_e;

typedef enum
{
    ADI_APOLLO_RX_TMODE_RES_16B = 0,
    ADI_APOLLO_RX_TMODE_RES_15B,
    ADI_APOLLO_RX_TMODE_RES_14B,
    ADI_APOLLO_RX_TMODE_RES_13B,
    ADI_APOLLO_RX_TMODE_RES_12B
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_tmode_res_e;

/*!< Dformat Test Mode Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_tmode_cfg
{
    adi_apollo_rx_tmode_sel_e      sel;            /*!< Tmode Data Routing Selection  */
    adi_apollo_rx_tmode_type_sel_e type_sel;       /*!< Tmode Data Type Selection */
    bool                           pn_force_rst;   /*!< True to reset pn gen */
    bool                           flush;          /*!< True to enable sync reset of I path */
    bool                           usr_pat_sel;    /*!< Tmode Single or Continuous Pattern Selection */
    adi_apollo_rx_tmode_res_e      res;
} adi_apollo_tmode_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< Rate Match Fifo Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rm_fifo_cfg
{
    bool invalid_en;        /*!< True to enable invalid sample insertion */
    bool sample_repeat_en;  /*!< True to enable sample repeat */
} adi_apollo_rm_fifo_cfg_t;
ADI_APOLLO_PACK_FINISH

typedef enum
{
    ADI_APOLLO_RX_DFORMAT_SEL_2COMP = 0,
    ADI_APOLLO_RX_DFORMAT_SEL_OFF_BIN,
    ADI_APOLLO_RX_DFORMAT_SEL_GRAY
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_dformat_sel_e;

typedef enum
{
    ADI_APOLLO_RX_DFORMAT_RES_16B = 0,
    ADI_APOLLO_RX_DFORMAT_RES_15B,
    ADI_APOLLO_RX_DFORMAT_RES_14B,
    ADI_APOLLO_RX_DFORMAT_RES_13B,
    ADI_APOLLO_RX_DFORMAT_RES_12B,
    ADI_APOLLO_RX_DFORMAT_RES_11B,
    ADI_APOLLO_RX_DFORMAT_RES_10B,
    ADI_APOLLO_RX_DFORMAT_RES_9B,
    ADI_APOLLO_RX_DFORMAT_RES_8B,
    ADI_APOLLO_RX_DFORMAT_RES_7B
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_dformat_res_e;

/*!< Dformat Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_dformat_cfg
{
    bool                        inv;               /*!< True to invert ADC sample data */
    adi_apollo_rx_dformat_sel_e sel;               /*!< Dformat Output Data Selection */
    bool                        ddc_dither_en;     /*!< True to enable ddc dither */
    adi_apollo_rx_dformat_res_e res;               /*!< Dformat Resolution */
    adi_apollo_tmode_cfg_t      tmode;             /*!< Tmode config */
    adi_apollo_rm_fifo_cfg_t    rm_fifo;           /*!< Rate Match Fifo config */
} adi_apollo_dformat_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< Valid SMON JESD Link Selections */
typedef enum
{
    ADI_APOLLO_RX_SMON_JLINK_SEL0 = 0,
    ADI_APOLLO_RX_SMON_JLINK_SEL1
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_jlink_sel_e;

/*!< Valid SMON Framer Modes */
typedef enum
{
    ADI_APOLLO_RX_SMON_FRAMER_MODE_10B = 0,
    ADI_APOLLO_RX_SMON_FRAMER_MODE_5B,
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_framer_mode_e;

/*!< Valid SMON Sync Next Modes */
typedef enum
{
    ADI_APOLLO_RX_SMON_SYNC_CONT = 0,   /*!< Continuous Mode. */
    ADI_APOLLO_RX_SMON_SYNC_NEXT        /*!< Next Synchroniztion Mode. */
} ADI_APOLLO_PACK_ENUM adi_apollo_rx_smon_sync_e;

/*!< SMON Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_smon_cfg
{
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

/*! RX path configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_rxpath
{
    adi_apollo_cddc_cfg_t       rx_cddc[ADI_APOLLO_CDDCS_PER_SIDE];     /*!< Coarse digital down converter config */
    adi_apollo_fddc_cfg_t       rx_fddc[ADI_APOLLO_FDDCS_PER_SIDE];     /*!< Fine digital down converter config */
    adi_apollo_pfilt_cfg_t      rx_pfilt[ADI_APOLLO_PFILTS_PER_SIDE];   /*!< Programmable filters */
    adi_apollo_cfir_cfg_t       rx_cfir[ADI_APOLLO_CFIRS_PER_SIDE];     /*!< Complex FIRs */
    adi_apollo_fsrc_cfg_t       rx_fsrc[ADI_APOLLO_FSRCS_PER_SIDE];     /*!< Fractional sampling rate converters */
    adi_apollo_rxpath_misc_t    rx_mux_splitter_xbar;                   /*!< Summers, multiplexers, crossbar config */
    adi_apollo_dyn_recfg_cfg_t  rx_dyn_recfg;                           /*!< Dynamic Reconfig Static settings */
    adi_apollo_dformat_cfg_t    rx_dformat[ADI_APOLLO_JESD_LINKS];      /*!< Dformat, Tmode, and RMFifo config */
    adi_apollo_smon_cfg_t       rx_smon[ADI_APOLLO_SMONS_PER_SIDE];     /*!< SMON config */
} adi_apollo_rxpath_t;
ADI_APOLLO_PACK_FINISH

/*! Clock PLL Config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_clk_pll_cfg
{
    uint32_t      loop_bandwidth;           /*!<  PLL Loop bandwidth  */
    uint8_t       phase_margin;             /*!<  PLL Phase Margin  */
    uint8_t       div_range;                /*!<  Serdes only Div Range  */
    uint8_t       div2;                     /*!<  Serdes only Div 2  */
    uint8_t       power;                    /*!<  PLL Power setting  */
    uint8_t       ref_clk_div;              /*!<  PLL ref clock divider  */
    uint8_t       i_bleed_en;               /*!<  PLL bleed ramp enable  */
    uint32_t      feedback_int;             /*!<  Integer portion of feedback factor */
    uint32_t      feedback_frac;            /*!<  Fractional portion of feedback factor */
} adi_apollo_clk_pll_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! SERDES PLL Config */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_serdes_pll_cfg
{
    uint32_t      loop_bandwidth;           /*!<  PLL Loop bandwidth  */
    uint8_t       phase_margin;             /*!<  PLL Phase Margin  */
    uint8_t       div_range;                /*!<  Serdes only Div Range  */
    uint8_t       div2;                     /*!<  Serdes only Div 2  */
    uint8_t       power;                    /*!<  PLL Power setting  */
    uint8_t       ref_clk_div;              /*!<  PLL ref clock divider  */
    uint8_t       i_bleed_en;               /*!<  PLL bleed ramp enable  */
    uint8_t       serdes_pll_odiv;          /*!<  Divider value (Yoda reg. name: register serdes_output_divider_ctl)  */
    uint32_t      feedback_int;             /*!<  Integer portion of feedback factor */
    uint32_t      feedback_frac;            /*!<  Fractional portion of feedback factor */
} adi_apollo_serdes_pll_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< Valid Apollo Clocking Modes */
typedef enum
{
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_8 = 0,         /*!<    Single data rate, divide by 8 */
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_4,             /*!<    Single data rate, divide by 4 */
    ADI_APOLLO_CLOCKING_MODE_DDR_DIV_4,             /*!<    Double data rate, divide by 4 */
    ADI_APOLLO_CLOCKING_MODE_SDR_DIV_4_DDR_DIV_2    /*!<    Single data rate divide by 4 and double data rate divide by 2 */
} ADI_APOLLO_PACK_ENUM adi_apollo_divg_mode_e;

/*!< Valid ARM Clock Side Selection */
typedef enum
{
    ADI_APOLLO_ARM_CLOCK_SEL_A = 0,         /*!<    A side */
    ADI_APOLLO_ARM_CLOCK_SEL_B              /*!<    B side */
} ADI_APOLLO_PACK_ENUM adi_apollo_arm_clock_sel_e;

/*! Clock configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_clk_cfg
{
    bool                        single_dual_clk_sel;                     /*!<   FALSE: Single (Center) clocking, TRUE: Dual clocking */
    bool                        clk_path_sel;                            /*!<   FALSE: On-chip PLL, TRUE: External clock */
    uint32_t                    ref_clk_freq_kHz;                        /*!<   Reference clock frequency. */
    adi_apollo_divg_mode_e      clocking_mode;                           /*!<   DivG block in clock generator. */
    bool                        adc_divby2[ADI_APOLLO_NUM_SIDES];        /*!<   Divider control for ADC clock path. TRUE: Divide by 2 */
    bool                        dac_divby2[ADI_APOLLO_NUM_SIDES];        /*!<   Divider control for DAC clock path. TRUE: Divide by 2 */
    bool                        adc_inclk_invert0[ADI_APOLLO_NUM_SIDES]; /*!<   Enable ADC 0 clock inversion. TRUE: invert clock */
    bool                        adc_inclk_invert1[ADI_APOLLO_NUM_SIDES]; /*!<   Enable ADC 1 clock inversion. TRUE: invert clock */
    uint32_t                    dev_clk_freq_kHz;                        /*!<   Device clock frequency */
    adi_apollo_arm_clock_sel_e  arm_clock_sel;                           /*!<   Selection of where the ARM/JESD clock originates when single_dual_clk_sel == True */
    uint8_t                     arm_clk_div;                             /*!<   Arm clock divider. */
    uint8_t                     serdes_clk_div;
} adi_apollo_clk_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Multi-Chip Sync modes */
typedef enum
{
    ADI_APOLLO_MCS_NO_CMD   = 0U,       /*!< No command, used for error checks only, user should not use */
    ADI_APOLLO_MCS_MODE0    = 1U,       /*!< Single clocking mode in subclass 1, foreground */
    ADI_APOLLO_MCS_MODE1    = 2U,       /*!< Dual clocking mode in subclass 1, foreground */
    ADI_APOLLO_MCS_MODE2    = 3U,       /*!< Dual clocking mode in subclass 0, foreground */
    ADI_APOLLO_MCS_MODE3    = 4U,       /*!< Dual clocking mode monitor, east taken as reference, foreground */
    ADI_APOLLO_MCS_MODE4    = 5U,       /*!< Dual clocking mode single sysref, foreground */
    ADI_APOLLO_MCS_MAX_MODE = 5U,       /*!< MCS mode should not go beyond this */
} ADI_APOLLO_PACK_ENUM adi_apollo_mcs_fsm_cmd_e;

/*! Sysref input configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_sysref_inp_cfg
{
    uint64_t    ref_to_int_period_ratio;    /*!< Ratio of Reference Sysref to Internal Sysref */
    bool        sysref_present;             /*!< True if external sysref is provided to this instance of inp */
    bool        rx_term_en;                 /*!< True to enable on-chip termination, False to disable*/
    bool        cm_above_900mv;             /*!< True if wanted sysref common mode above 900mV */
} adi_apollo_sysref_inp_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! ADF4382 Correction Mode */
typedef enum
{
    ADI_APOLLO_ADF4382_CORR_FRAC = 0U,      /*!< Fraction Mode */
    ADI_APOLLO_ADF4382_CORR_INT = 1U       /*!< Integer Mode */
} ADI_APOLLO_PACK_ENUM adi_apollo_adf4382_corr_e;

/*! ADF4382 configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_adf4382_cfg
{
    uint8_t                     center_adf4382_clock_align_config;         /*!< Defines the number of the ADF4382 instance driving the sysref */
    uint8_t                     aside_adf4382_clock_align_config;
    uint8_t                     bside_adf4382_clock_align_config;

    adi_apollo_adf4382_corr_e   correction_mode;        /*!< Selects the clock phase correction approach for clock align for all adf4382 */

    /*!< (ADF4382)Artemis Clock GPIO selection
    Up to two GPIOs(That is why it is an array).Example[0, 2](Different GPIOs) or [1, 1](Same GPIO to drive the chips)
    GPIOs that will be used to tell one or two Artemis to actuate one step of adjustment(either advance or delay clock based on the delay adjust variables)
    A value of 99 means that a GPIO has not been assigned.The first index corresponds to the ADF4382 instance '1' from ADF4382_clock_align_config
    */
    uint8_t                     clock_align_delay_adjust_gpio[ADI_APOLLO_NUM_ADF4382_GPIOS];
    uint8_t                     clock_align_delay_strobe_gpio[ADI_APOLLO_NUM_ADF4382_GPIOS];

    /*!< Settings per ADF4382 */
    uint8_t                     max_coarse_code[ADI_APOLLO_MAX_NUM_ADF4382];            /*!< This determines how many coarse segments in a particular ADF4382's correction DAC to be used, when the clock-to-AD9084's SysRef time error is larger than the tracking window. */
    uint16_t                    max_fine_code[ADI_APOLLO_MAX_NUM_ADF4382];              /*!< This determines how many fine segments in a particular ADF4382's correction DAC to be used, when the clock-to-AD9084's SysRef time error is smaller than the tracking window. */
    bool                        track_polarity_select[ADI_APOLLO_MAX_NUM_ADF4382];      /*!< Variable used in case clock correction feedback(delay_adjust) to a particular ADF4382 is inverted. */
    uint8_t                     phase_adj[ADI_APOLLO_MAX_NUM_ADF4382];                  /*!< Phase adjustment steps of the ADF4382. Must match the PHASE_ADJUSTMENT of ADF4382. */
    uint8_t                     track_win[ADI_APOLLO_MAX_NUM_ADF4382];                  /*!< This variable defines the amount of deviation a particular ADF4382's output clock can drift, in picosecond [ ps ], relative to AD9084's External SysRef(BSYNC) before we attempt to correct it.This is the clock tracking window. */
} adi_apollo_adf4382_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! MCS Trigger configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_mcs_trigger_cfg
{
    uint8_t trigger_pin_to_rx_datapath_a_mapping;   /*!< MCS Trigger for A Side Rx */
    uint8_t trigger_pin_to_rx_datapath_b_mapping;   /*!< MCS Trigger for B Side Rx */
    uint8_t trigger_pin_to_tx_datapath_a_mapping;   /*!< MCS Trigger for A Side Tx */
    uint8_t trigger_pin_to_tx_datapath_b_mapping;   /*!< MCS Trigger for B Side Tx */
} adi_apollo_mcs_trigger_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Multi-Chip Sync configuration */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_mcs_cfg
{
    adi_apollo_sysref_inp_cfg_t         center_sysref;
    adi_apollo_sysref_inp_cfg_t         aside_sysref;
    adi_apollo_sysref_inp_cfg_t         bside_sysref;

    bool                                use_gapped_sysref;                              /*!< if True, external sysref period (minimum) must match internal sysref period. If False, External Sysref period must be integer multiple of internal sysref */

    bool                                leave_sysref_rx_on;                             /*!< True to leave sysref rx on after sync, False to turn it off */

    uint16_t                            num_sysref_avg_mcs_fw;                          /*!< Number of sysref averages to use for mcs firmware */
    uint8_t                             sysref_lock_window_mcs_fw;                      /*!< Lock window for the sysref with mcs firmware */

    uint16_t                            internal_sysref_prd_digclk_cycles_center;       /*!< Sys Ref Period of Center MCS in number of master digital clock cycles */
    uint16_t                            internal_sysref_prd_digclk_cycles_side_a;       /*!< Sys Ref Period of side A MCS in number of master digital clock cycles */
    uint16_t                            internal_sysref_prd_digclk_cycles_side_b;       /*!< Sys Ref Period of side B MCS in number of master digital clock cycles */

    adi_apollo_adf4382_cfg_t            adf4382_cfg;
    adi_apollo_mcs_trigger_cfg_t        mcs_trigger_cfg;                                /*!< MCS Trigger Config */
} adi_apollo_mcs_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< Valid JESD Sync Subclasses */
typedef enum
{
    ADI_APOLLO_SUBCLASS_0 = 0,
    ADI_APOLLO_SUBCLASS_1,
    ADI_APOLLO_SUBCLASS_2
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_subclass_e;

/*!< Valid JESD Logical Lanes */
typedef enum
{
    ADI_APOLLO_XBAR_LOGIC_LANE_0 = 0,
    ADI_APOLLO_XBAR_LOGIC_LANE_1,
    ADI_APOLLO_XBAR_LOGIC_LANE_2,
    ADI_APOLLO_XBAR_LOGIC_LANE_3,
    ADI_APOLLO_XBAR_LOGIC_LANE_4,
    ADI_APOLLO_XBAR_LOGIC_LANE_5,
    ADI_APOLLO_XBAR_LOGIC_LANE_6,
    ADI_APOLLO_XBAR_LOGIC_LANE_7,
    ADI_APOLLO_XBAR_LOGIC_LANE_8,
    ADI_APOLLO_XBAR_LOGIC_LANE_9,
    ADI_APOLLO_XBAR_LOGIC_LANE_10,
    ADI_APOLLO_XBAR_LOGIC_LANE_11
} ADI_APOLLO_PACK_ENUM adi_apollo_jtx_lane_xbar_sel_e;

/*!< Valid JESD PHY Lanes */
typedef enum
{
    ADI_APOLLO_XBAR_PHY_LANE_0 = 0,
    ADI_APOLLO_XBAR_PHY_LANE_1,
    ADI_APOLLO_XBAR_PHY_LANE_2,
    ADI_APOLLO_XBAR_PHY_LANE_3,
    ADI_APOLLO_XBAR_PHY_LANE_4,
    ADI_APOLLO_XBAR_PHY_LANE_5,
    ADI_APOLLO_XBAR_PHY_LANE_6,
    ADI_APOLLO_XBAR_PHY_LANE_7,
    ADI_APOLLO_XBAR_PHY_LANE_8,
    ADI_APOLLO_XBAR_PHY_LANE_9,
    ADI_APOLLO_XBAR_PHY_LANE_10,
    ADI_APOLLO_XBAR_PHY_LANE_11
} ADI_APOLLO_PACK_ENUM adi_apollo_jrx_lane_xbar_sel_e;

/*!< Valid JESD Versions */
typedef enum adi_apollo_jesd_version
{
    ADI_APOLLO_JESD_204B = 0u,
    ADI_APOLLO_JESD_204C
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_version_e;

/*! Link independent configuration (applies to both links if dual_link is true) */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_common_cfg
{
    adi_apollo_jesd_version_e        ver;                                    /*!< 0 = 204B, 1 = 204C */
    adi_apollo_jesd_subclass_e       subclass;                               /*!< Subclass setting */
    bool                             dual_link;                              /*!< TRUE: Dual link */
    uint16_t                         lane_enables;                           /*!< Each bit for one lane (12 lanes) */
    uint32_t                         lane_rate_kHz;                          /*!< Lane rate */
} adi_apollo_jesd_common_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< Valid JESD Lanes Per Link Settings */
typedef enum
{
    ADI_APOLLO_LANES_PER_LINK_1 = 0,
    ADI_APOLLO_LANES_PER_LINK_2 = 1,
    ADI_APOLLO_LANES_PER_LINK_3 = 2,
    ADI_APOLLO_LANES_PER_LINK_4 = 3,
    ADI_APOLLO_LANES_PER_LINK_6 = 5,
    ADI_APOLLO_LANES_PER_LINK_8 = 7,
    ADI_APOLLO_LANES_PER_LINK_12 = 11
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_lanes_per_link_e;

/*!< Valid JESD Octet Per Frame Settings */
typedef enum
{
    ADI_APOLLO_OCT_PER_FRM_1 = 0,
    ADI_APOLLO_OCT_PER_FRM_2 = 1,
    ADI_APOLLO_OCT_PER_FRM_3 = 2,
    ADI_APOLLO_OCT_PER_FRM_4 = 3,
    ADI_APOLLO_OCT_PER_FRM_6 = 5,
    ADI_APOLLO_OCT_PER_FRM_8 = 7,
    ADI_APOLLO_OCT_PER_FRM_12 = 11,
    ADI_APOLLO_OCT_PER_FRM_16 = 15,
    ADI_APOLLO_OCT_PER_FRM_24 = 23,
    ADI_APOLLO_OCT_PER_FRM_32 = 31
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_octs_per_frm_e;

/*!< Valid JESD Converter Per Link Settings */
typedef enum
{
    ADI_APOLLO_CONV_PER_LINK_1 = 0,
    ADI_APOLLO_CONV_PER_LINK_2 = 1,
    ADI_APOLLO_CONV_PER_LINK_3 = 2,
    ADI_APOLLO_CONV_PER_LINK_4 = 3,
    ADI_APOLLO_CONV_PER_LINK_6 = 5,
    ADI_APOLLO_CONV_PER_LINK_8 = 7,
    ADI_APOLLO_CONV_PER_LINK_12 = 11,
    ADI_APOLLO_CONV_PER_LINK_16 = 15
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_convs_per_link_e;

/*!< Valid JESD Control Bits Per Sample Settings */
typedef enum
{
    ADI_APOLLO_CONT_BITS_PER_SAMP_0 = 0,
    ADI_APOLLO_CONT_BITS_PER_SAMP_1 = 1,
    ADI_APOLLO_CONT_BITS_PER_SAMP_2 = 2,
    ADI_APOLLO_CONT_BITS_PER_SAMP_3 = 3
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_cont_bits_per_samp_e;

/*!< Valid JESD Converter Resolution Settings */
typedef enum
{
    ADI_APOLLO_CONV_RESOL_7 = 6,
    ADI_APOLLO_CONV_RESOL_8 = 7,
    ADI_APOLLO_CONV_RESOL_9 = 8,
    ADI_APOLLO_CONV_RESOL_10 = 9,
    ADI_APOLLO_CONV_RESOL_11 = 10,
    ADI_APOLLO_CONV_RESOL_12 = 11,
    ADI_APOLLO_CONV_RESOL_13 = 12,
    ADI_APOLLO_CONV_RESOL_14 = 13,
    ADI_APOLLO_CONV_RESOL_15 = 14,
    ADI_APOLLO_CONV_RESOL_16 = 15,
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_conv_resol_e;

/*!< Valid JESD Sample Per Conv Per Frame Settings */
typedef enum
{
    ADI_APOLLO_SAMP_CONV_FRM_1 = 0,
    ADI_APOLLO_SAMP_CONV_FRM_2 = 1,
    ADI_APOLLO_SAMP_CONV_FRM_3 = 2,
    ADI_APOLLO_SAMP_CONV_FRM_4 = 3,
    ADI_APOLLO_SAMP_CONV_FRM_6 = 5,
    ADI_APOLLO_SAMP_CONV_FRM_8 = 7,
    ADI_APOLLO_SAMP_CONV_FRM_12 = 11,
    ADI_APOLLO_SAMP_CONV_FRM_16 = 15
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_samp_conv_frm_e;

/*!< Valid JESD Bits Per Sample Settings */
typedef enum
{
    ADI_APOLLO_BITS_PER_SAMP_8 = 7,
    ADI_APOLLO_BITS_PER_SAMP_12 = 11,
    ADI_APOLLO_BITS_PER_SAMP_16 = 15
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_bits_per_samp_e;

/*!< Valid JESD Sample Per Conv Settings */
typedef enum
{
    ADI_APOLLO_SAMP_PER_CONV_1 = 0,
    ADI_APOLLO_SAMP_PER_CONV_2 = 1,
    ADI_APOLLO_SAMP_PER_CONV_4 = 3,
    ADI_APOLLO_SAMP_PER_CONV_8 = 7,
    ADI_APOLLO_SAMP_PER_CONV_12 = 11,
    ADI_APOLLO_SAMP_PER_CONV_16 = 15,
    ADI_APOLLO_SAMP_PER_CONV_24 = 23,
    ADI_APOLLO_SAMP_PER_CONV_32 = 31
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_samp_per_conv_e;

/*!< Valid JESD SYNCb Settings */
typedef enum adi_apollo_jesd_syncb_sel
{
    ADI_APOLLO_JESD_FRM_SYNCB_CMOS                 = 0u,
    ADI_APOLLO_JESD_FRM_SYNCB_LVDS_WITH_INTL_TERM  = 1u,
    ADI_APOLLO_JESD_FRM_SYNCB_LVDS_NO_INTL_TERM    = 2u,
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_syncb_sel_e;

/*! Per Link JTx Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_tx_link_cfg
{
    bool                                      link_in_use;     /*!< if TRUE, the link is in use */
    uint8_t                                   e_minus1;        /*!< Blocks in Multi-block minus 1 for 204C */
    uint8_t                                   bank_id;         /*!< Bank ID extension to Device ID (0-15)*/
    uint8_t                                   dev_id;          /*!< Device ID link identification number (0-255) */
    uint8_t                                   base_lane_id;    /*!< Starting Lane ID */
    adi_apollo_jesd_lanes_per_link_e          l_minus1;        /*!< Number of lanes minus 1 */
    adi_apollo_jesd_octs_per_frm_e            f_minus1;        /*!< Number of bytes (octets) per frame minus 1 */
    adi_apollo_jesd_convs_per_link_e          m_minus1;        /*!< Number of converters minus 1 */
    adi_apollo_jesd_samp_conv_frm_e           s_minus1;        /*!< Number of samples per converter per frame minus 1 */
    bool                                      high_dens;       /*!< TRUE: High-density */
    uint8_t                                   k_minus1;        /*!< Number of frames in a multiframe - 1 (0 - 255) */
    adi_apollo_jesd_conv_resol_e              n_minus1;        /*!< Link converter resolution minus 1 */
    adi_apollo_jesd_bits_per_samp_e           np_minus1;       /*!< Converter sample resolution in bits minus 1 */
    adi_apollo_jesd_samp_per_conv_e           ns_minus1;       /*!< Number of samples per converter in Conv_sample */
    adi_apollo_jesd_cont_bits_per_samp_e      cs;              /*!< Number of control bits per sample*/
    bool                                      scr;             /*!< Scrambling enable */
    bool                                      quick_cfg_en;    /*!< Enable Quick configuration mode */
    uint8_t                                   quick_mode_id;   /*!< Quick configuration mode ID */
    uint8_t                                   syncb_in_sel;    /*!< Selects SYNCb input source. */
    adi_apollo_jesd_syncb_sel_e               syncb_lvds_mode;
    bool                                      fec_enable;      /*!< If TRUE, forward error correction is enabled */
    bool                                      async_mode;      /*!< IF TRUE, asynchronous mode is enabled */
    bool                                      metword_rev;     /*!< IF TRUE, bit ordering of CRC/FEC is reversed */
    uint16_t                                  link_dp_ratio;   /*!< Link datapath ratio */
    uint16_t                                  link_total_ratio;/*!< Link total ratio */
    uint16_t                                  phase_adjust;    /*!< LMFC offset for deterministic delay, Yoda register: sync_sysref_delay */
    adi_apollo_jesd_frm_sample_xbar_select_e  sample_xbar_sel[ADI_APOLLO_JESD_MAX_FRM_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    adi_apollo_jtx_lane_xbar_sel_e            lane_xbar[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];            /*!< Lane crossbar selection */
} adi_apollo_jesd_tx_link_cfg_t;
ADI_APOLLO_PACK_FINISH

/*!< List of Serializer Output Swings */
typedef enum adi_apollo_jesd_jtx_output_drive_swing
{
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_100 = 0u,
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_85  = 1u,
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_75  = 2u,
    ADI_APOLLO_JESD_DRIVE_SWING_VTT_50  = 3u,
    ADI_APOLLO_JESD_DRIVE_SWING_LAST_VALID = 3u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_output_drive_swing_e;

/*!< List of Serializer Pre Tap Values */
typedef enum adi_apollo_jesd_jtx_pre_emphasis
{
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_0_DB = 0u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_3_DB = 1u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_6_DB = 2u,
    ADI_APOLLO_JESD_PRE_TAP_LEVEL_LAST_VALID = 2u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_pre_emphasis_e;

/*!< List of Serializer Post Tap Values */
typedef enum adi_apollo_jesd_jtx_post_emphasis
{
    ADI_APOLLO_JESD_POST_TAP_LEVEL_0_DB  = 0u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_3_DB  = 1u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_6_DB  = 2u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_9_DB  = 3u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_12_DB = 4u,
    ADI_APOLLO_JESD_POST_TAP_LEVEL_LAST_VALID = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_jesd_jtx_post_emphasis_e;

/*!< Valid SerDes Lanerate Adapt Ratios */
typedef enum
{
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_1 = 0,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_2,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_4,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_8,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_16,
    ADI_APOLLO_SERDES_LR_ADAPT_RATIO_32,
} ADI_APOLLO_PACK_ENUM adi_apollo_serdes_lr_adapt_ratio_e;

/*!< Per Lane Serializer Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_ser_lane
{
    adi_apollo_jesd_jtx_output_drive_swing_e ser_amplitude;             /*!< Serializer lane amplitude */
    adi_apollo_jesd_jtx_pre_emphasis_e       ser_pre_emphasis;          /*!< Serializer lane pre-emphasis */
    adi_apollo_jesd_jtx_post_emphasis_e      ser_post_emphasis;         /*!< Serializer lane post-emphasis */
    bool                                     ser_invert_lane_polarity;  /*!< Serializer lane PN inversion select: TRUE = invert */
    adi_apollo_serdes_lr_adapt_ratio_e       lanerate_adapt;            /*!< Lanerate Adapt ratio */
} adi_apollo_jesd_ser_lane_t;
ADI_APOLLO_PACK_FINISH

/*!< List of Rate Modes for Deserializer */
typedef enum
{
    ADI_APOLLO_DESER_RATE_MODE_FULL_RATE = 0u,      /*!< Full Rate */
    ADI_APOLLO_DESER_RATE_MODE_HALF_RATE = 1u,      /*!< Half Rate */
    ADI_APOLLO_DESER_RATE_MODE_QUARTER_RATE = 2u,   /*!< Quarter Rate */
    ADI_APOLLO_DESER_RATE_MODE_VSR = 3u,            /*!< Very Short Reach */
    ADI_APOLLO_DESER_NUM_OF_RATE_MODES = 4u,        /*!< Number of modes */
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_rate_mode_e;

/*!< List of config options for deserializer */
typedef enum
{
    ADI_APOLLO_DESER_CONFIG_OPTION_1 = 0,
    ADI_APOLLO_DESER_CONFIG_OPTION_2,
    ADI_APOLLO_DESER_CONFIG_OPTION_3,
    ADI_APOLLO_DESER_CONFIG_OPTION_4,
    ADI_APOLLO_DESER_CONFIG_OPTION_5,
    ADI_APOLLO_DESER_CONFIG_OPTION_6,
    ADI_APOLLO_DESER_CONFIG_OPTION_7,
    ADI_APOLLO_DESER_CONFIG_OPTION_8,
    ADI_APOLLO_DESER_CONFIG_OPTION_9,
    ADI_APOLLO_DESER_CONFIG_OPTION_10,
    ADI_APOLLO_DESER_NUM_CONFIGS
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_config_option_idx_e;

/*!< List of Override options for deserializer */
typedef enum
{
    ADI_APOLLO_DESER_INVALID = 0, /*!< No further settings applied */
    ADI_APOLLO_DESER_JRX_CTLE_S1_GM = 1u, /*!< AEQ stage 1 transconductance weight; 17 units with irregular coding - 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,31 are valid. */
    ADI_APOLLO_DESER_JRX_CTLE_S1_LD = 2u, /*!< AEQ stage 1 load impedance weight; 15 units with irregular coding - 0,1,2,3,4,5,6,7,9,10,11,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_JRX_CTLE_S2_GM = 3u, /*!< AEQ stage 2 load impedance weight; 12 units with irregular coding - 0,1,2,3,4,5,6,7,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_JRX_CTLE_S2_LD = 4u, /*!< AEQ stage 2 transconductance weight; 13 units with irregular coding - 0,1,2,3,4,6,7,11,12,13,14,15 are valid.*/
    ADI_APOLLO_DESER_LAST_VALID = 4u
} ADI_APOLLO_PACK_ENUM adi_apollo_deser_override_name_e;

/*!< An override setting for SerDes */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_deser_override
{
    adi_apollo_deser_override_name_e names[ADI_APOLLO_DESER_NUM_CONFIGS];
    uint8_t                          values[ADI_APOLLO_DESER_NUM_CONFIGS];
} adi_apollo_jesd_deser_override_t;
ADI_APOLLO_PACK_FINISH

/*! Per Lane Deserializer Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_deser_lane
{
    uint8_t                             high_boost_options;                           /*!< Mask containing how the config options should be applied */
    bool                                des_inv_lane_pol;                             /*!< Deserializer Lane PN inversion select: TRUE = invert */
    adi_apollo_deser_rate_mode_e        rx_des_qhf_rate;                              /*!< Deserializer mode settings */
    adi_apollo_serdes_lr_adapt_ratio_e  lanerate_adapt;                               /*!< Lanerate Adapt ratio */
    uint32_t                            config_options[ADI_APOLLO_DESER_NUM_CONFIGS]; /*!< Used for modifying the SerDes configuration at startup based off customers use case */
    adi_apollo_jesd_deser_override_t    override_settings;                            /*!< Used to modify Serdes PHY bitfields before running cals, see serdes_rxdig_phy_regmap_core1p3 */
} adi_apollo_jesd_deser_lane_t;
ADI_APOLLO_PACK_FINISH

/*! Configuration for individual links within JRX */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_rx_link_cfg
{
    bool                                        link_in_use;                                                /*!< if TRUE, the link is in use */
    uint8_t                                     e_minus1;                                                   /*!< Blocks in Multi-block minus 1 for 204C */
    uint8_t                                     bank_id;                                                    /*!< Bank ID extension to Device ID (0-15)*/
    uint8_t                                     dev_id;                                                     /*!< Device ID link identification number (0-255) */
    uint8_t                                     base_lane_id;                                               /*!< Starting Lane ID */
    adi_apollo_jesd_lanes_per_link_e            l_minus1;                                                   /*!< Number of lanes minus 1 */
    adi_apollo_jesd_octs_per_frm_e              f_minus1;                                                   /*!< Number of bytes (octets) per frame minus 1 */
    adi_apollo_jesd_convs_per_link_e            m_minus1;                                                   /*!< Number of converters minus 1 */
    adi_apollo_jesd_samp_conv_frm_e             s_minus1;                                                   /*!< Number of samples per converter per frame minus 1 */
    bool                                        high_dens;                                                  /*!< High-density */
    uint8_t                                     k_minus1;                                                   /*!< Number of frames in a multiframe minus 1 (0 - 255) */
    adi_apollo_jesd_conv_resol_e                n_minus1;                                                   /*!< Link converter resolution minus 1 */
    adi_apollo_jesd_bits_per_samp_e             np_minus1;                                                  /*!< Converter sample resolution in bits minus 1 */
    adi_apollo_jesd_samp_per_conv_e             ns_minus1;                                                  /*!< Number of samples per converter in Conv_sample minus 1 */
    adi_apollo_jesd_cont_bits_per_samp_e        cs;                                                         /*!< Number of control bits per sample*/
    bool                                        scr;                                                        /*!< Scrambling enable */
    bool                                        quick_cfg_en;                                               /*!< Enable Quick configuration mode */
    uint8_t                                     quick_mode_id;                                              /*!< Quick configuration mode ID */
    adi_apollo_jesd_dfrm_sync_pad_req_e         syncb_out_sel;                                              /*!< Selects deframer SYNCBOUT pin. */
    adi_apollo_jesd_syncb_sel_e                 syncb_lvds_mode;
    uint16_t                                    link_dp_ratio;                                              /*!< Link datapath ratio */
    uint16_t                                    link_total_ratio;                                           /*!< Link total ratio */
    bool                                        invalid_data_en;                                            /*!<  True to enable invalid sample removal */
    uint8_t                                     invalid_sample_num;                                         /*!<  The number of invalid samples each conv_clk. The value should be (JTX_NS - 1). */
    bool                                        sample_repeat_en;                                           /*!<  True to enable sample repeat removal */
    uint16_t                                    phase_adjust;                                               /*!< LMFC offset for deterministic delay, Yoda register: sync_sysref_delay */
    adi_apollo_jesd_dfrm_sample_xbar_select_e   sample_xbar_sel[ADI_APOLLO_JESD_MAX_FRM_SAMPLE_XBAR_IDX];   /*!< Converter sample crossbar selection */
    adi_apollo_jrx_lane_xbar_sel_e              lane_xbar[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];              /*!< Lane crossbar selection */
} adi_apollo_jesd_rx_link_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Per Side JTx Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_tx_cfg
{
    adi_apollo_jesd_common_cfg_t    common_link_cfg;                        /*!<  Config for both links or one link if only one link */
    adi_apollo_jesd_tx_link_cfg_t   tx_link_cfg[ADI_APOLLO_JESD_LINKS];     /*!<  Config for each of the link(s) */
    adi_apollo_jesd_ser_lane_t      serializer_lane[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];
} adi_apollo_jesd_tx_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Per Side JRx Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_jesd_rx_cfg
{
    adi_apollo_jesd_common_cfg_t        common_link_cfg;                        /*!<  Common config for JRx link */
    adi_apollo_jesd_rx_link_cfg_t       rx_link_cfg[ADI_APOLLO_JESD_LINKS];     /*!<  Config for JRx link */
    adi_apollo_jesd_deser_lane_t        deserializer_lane[ADI_APOLLO_JESD_MAX_LANES_PER_SIDE];
} adi_apollo_jesd_rx_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! GPIO Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_gpio_cfg
{
    uint8_t  gpio_quick_config;                         /*!<  GPIO quick configuration. See user guide for information on quick config use cases. */
    uint64_t gpio_mode_en;                              /*!<  Nth bit corresponds to Nth GPIO pin for enabling gpio_mode_dir for that pin. */
    uint64_t gpio_mode_dir;                             /*!<  This field has meaning only if corresponding gpio_mode_en is set to 1. if gpio_mode_dir = 1 than gpio will be configured as an input and if gpio_mode_dir=0 than gpio will be configured as output. */
    uint64_t gpio_from_master;                          /*!<  This field is used by masters (core0/core1/stream_proc) to write data to Digital GPIO pins. */
    uint64_t gpio_from_set;                             /*!<  Writing a '1' to a bit in this field sets the corresponding bit in the gpio_from_master register. */
    uint8_t  gpio_source_control[ADI_APOLLO_NUM_GPIOS]; /*!<  Select pin of GPIO functional pinmux. */
    uint64_t gpio_stage_sel_lsb;                        /*!<  64Bits select the mux stage of GPIO pinmuxing. */ 
    uint64_t gpio_stage_sel_msb;                        /*!<  38Bits select the mux stage of GPIO pinmuxing. */
} adi_apollo_gpio_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! LinearX Config Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_linx_cfg
{
    uint8_t                     gpio_index;             /*!<  GPIO to be toggled during training process. */
    bool                        gpio_invert;            /*!<  True to invert GPIO signal. */
    adi_apollo_tx_channel_num_e signal_injection_dac;   /*!<  Index of the DAC used for Signal Injection. */
    uint32_t                    reserved;               /*!<  Reserved Configuration Fields for ADC. */
} adi_apollo_linx_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Profile Configuration Stucture */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_profile_cfg
{
    adi_apollo_profile_version_t  profile_version;  /*!<  Profile version  */
    uint16_t                      config_version;   /*!<  A specific profile can be valued several times, each with a new config_version */
    bool                          is_8t8r;          /*!<  True for 8t8r, False for 4t4r  */
} adi_apollo_profile_cfg_t;
ADI_APOLLO_PACK_FINISH

/*! Apollo Top Structure */
ADI_APOLLO_PACK_START
typedef struct adi_apollo_top
{
    adi_apollo_profile_cfg_t        profile_cfg;                                                        /*!<   Profile version and misc high level info */
    adi_apollo_clk_cfg_t            clk_cfg;                                                            /*!<   Clock configuration */
    adi_apollo_clk_pll_cfg_t        clk_pll;                                                            /*!<   Main PLL configuration */
    adi_apollo_mcs_cfg_t            mcs_cfg;                                                            /*!<   MCS configuration */
    adi_apollo_gpio_cfg_t           gpio_cfg;                                                           /*!<   GPIO configuration */
    adi_apollo_txpath_t             tx_path[ADI_APOLLO_NUM_SIDES];                                      /*!<   Tx Path configuration */
    adi_apollo_rxpath_t             rx_path[ADI_APOLLO_NUM_SIDES];                                      /*!<   Rx Path configuration */
    adi_apollo_jesd_tx_cfg_t        jtx[ADI_APOLLO_NUM_SIDES];                                          /*!<   JTX configuration */
    adi_apollo_jesd_rx_cfg_t        jrx[ADI_APOLLO_NUM_SIDES];                                          /*!<   JRX configuration */
    adi_apollo_dac_cfg_t            dac_config[ADI_APOLLO_NUM_SIDES];                                   /*!<   DAC configuration */
    adi_apollo_adc_cfg_t            adc_config[ADI_APOLLO_NUM_SIDES];                                   /*!<   ADC configuration */
    bool                            lb0_en[ADI_APOLLO_NUM_SIDES][ADI_APOLLO_DAC_DIG_SLICE_PER_SIDE];    /*!<   Loopback for feeding the DAC from ADC */
    adi_apollo_linx_cfg_t           linx_cfg[ADI_APOLLO_4T4R_ADCS_TOTAL];
    adi_apollo_serdes_pll_cfg_t     serdes_pll;                                                         /*!<   SERDES PLL structure */
    uint16_t                        adc_enable;                                                         /*!<   ADC Enable. B0=ADC_A0, B1=ADC_A1, B2=ADC_A2, B3=ADC_A3, B4=ADC_B0, B5=ADC_B1, B6=ADC_B2, B7=ADC_B3  */
    uint16_t                        dac_enable;                                                         /*!<   DAC Enable. B0=DAC_A0, B1=DAC_A1, B2=DAC_A2, B3=DAC_A3, B4=DAC_B0, B5=DAC_B1, B6=DAC_B2, B7=DAC_B3  */
    uint8_t                         reserved_cfg[64];                                                   /*!<   Extra space for temporary feature implementation */
    uint32_t                        profile_checksum;                                                   /*!<   Checksum of the entire profile using CRC32 */
} adi_apollo_top_t;
ADI_APOLLO_PACK_FINISH

#endif /* __APOLLO_CPU_DEVICE_PROFILE_TYPES_H__ */
