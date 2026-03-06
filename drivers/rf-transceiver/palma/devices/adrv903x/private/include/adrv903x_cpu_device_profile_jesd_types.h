/**
 * \file adrv903x_cpu_device_profile_jesd_types.h
 *
 * \brief   Contains ADRV903X Device Profile JESD type definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_DEVICE_PROFILE_JESD_TYPES_H__
#define __ADRV903X_CPU_DEVICE_PROFILE_JESD_TYPES_H__

#include "adi_adrv903x_platform_pack.h"


#define ADRV903X_JESD_MAX_DESERIALIZER_LANES      (8u)
#define ADRV903X_JESD_MAX_SERIALIZER_LANES        (8u)
#define ADRV903X_JESD_MAX_FRM_SAMPLE_XBAR_IDX     (64u)
#define ADRV903X_JESD_MAX_DFRM_SAMPLE_XBAR_IDX    (32u)
#define ADRV903X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX    (32u)
/* ADRV903X_JESD_NUM_TXRX_CHAN == ADRV903X_NUM_TXRX_CHAN */
#define ADRV903X_JESD_NUM_TXRX_CHAN               (8u)

typedef enum adrv903x_JesdVersion
{
    ADRV903X_JESD_204B            = 0u,
    ADRV903X_JESD_204C            = 1u,
    ADRV903X_JESD_LAST_VALID_MODE = ADRV903X_JESD_204C
} adrv903x_JesdVersion_e;

typedef uint8_t adrv903x_JesdVersion_t;

/*!< List of Framers */
typedef enum adrv903x_JesdFramer
{
    ADRV903X_JESD_FRAMER_0   = 0u,   /*!< Framer 0 selection */
    ADRV903X_JESD_FRAMER_1   = 1u,   /*!< Framer 1 selection */
    ADRV903X_JESD_FRAMER_2   = 2u,   /*!< Framer 2 selection */
    ADRV903X_JESD_FRAMER_NUM = 3u
} adrv903x_JesdFramer_e;

/*!< List of Deframers */
typedef enum adrv903x_JesdDeframer
{
    ADRV903X_JESD_DEFRAMER_0   = 0u,   /*!< Deframer 0 selection */
    ADRV903X_JESD_DEFRAMER_1   = 1u,   /*!< Deframer 1 selection */
    ADRV903X_JESD_DEFRAMER_NUM = 2u
} adrv903x_JesdDeframer_e;

/*!< List of Deframers */
typedef enum adrv903x_JesdLkShSwitcher
{
    ADRV903X_JESD_LKSH_SWITCHER_0   = 0u,   /*!< Link Sharing Switcher 0 selection */
    ADRV903X_JESD_LKSH_SWITCHER_1   = 1u,   /*!< Link Sharing Switcher 1 selection */
    ADRV903X_JESD_LKSH_SWITCHER_NUM = 2u
} adrv903x_JesdLkShSwitcher_e;

/*!< List of DAC Sample Xbar options */
typedef enum adrv903x_JesdDeframerSampleXbarSelect
{
    ADRV903X_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_I = 0u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_Q = 1u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_I = 2u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_Q = 3u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_I = 4u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_Q = 5u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_I = 6u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_Q = 7u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_I = 8u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_Q = 9u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_I = 10u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_Q = 11u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_I = 12u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_Q = 13u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_I = 14u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_Q = 15u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_I = 16u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_Q = 17u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_I = 18u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_Q = 19u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_I = 20u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_Q = 21u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_I = 22u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_Q = 23u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_I = 24u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_Q = 25u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_I = 26u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_Q = 27u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_I = 28u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_Q = 29u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_I = 30u,
    ADRV903X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_Q = 31u,
    ADRV903X_JESD_DFRM_SPLXBAR_LAST_VALID        = ADRV903X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_Q,
    ADRV903X_JESD_DFRM_SPLXBAR_INVALID           = 0x7Fu
} adrv903x_JesdDeframerSampleXbarSelect_e;

/*!< List of ADC Sample Xbar options */
typedef enum adrv903x_JesdFramerSampleXbarSelect
{
    ADRV903X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I = 0u,
    ADRV903X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q = 1u,
    ADRV903X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_I = 2u,
    ADRV903X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_Q = 3u,
    ADRV903X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I = 4u,
    ADRV903X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q = 5u,
    ADRV903X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_I = 6u,
    ADRV903X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_Q = 7u,
    ADRV903X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I = 8u,
    ADRV903X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q = 9u,
    ADRV903X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_I = 10u,
    ADRV903X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_Q = 11u,
    ADRV903X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I = 12u,
    ADRV903X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q = 13u,
    ADRV903X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_I = 14u,
    ADRV903X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_Q = 15u,

    ADRV903X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I = 16u,
    ADRV903X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q = 17u,
    ADRV903X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_I = 18u,
    ADRV903X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_Q = 19u,
    ADRV903X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I = 20u,
    ADRV903X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q = 21u,
    ADRV903X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_I = 22u,
    ADRV903X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_Q = 23u,
    ADRV903X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I = 24u,
    ADRV903X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q = 25u,
    ADRV903X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_I = 26u,
    ADRV903X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_Q = 27u,
    ADRV903X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I = 28u,
    ADRV903X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q = 29u,
    ADRV903X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_I = 30u,
    ADRV903X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q = 31u,
    ADRV903X_JESD_FRM_SPLXBAR_LAST_VALID_NO_INT = ADRV903X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q, /* With no interleaving enabled */

    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_0     = 32u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_1     = 33u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_2     = 34u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_3     = 35u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_4     = 36u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_5     = 37u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_6     = 38u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_I_7     = 39u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_0     = 40u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_1     = 41u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_2     = 42u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_3     = 43u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_4     = 44u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_5     = 45u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_6     = 46u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_7     = 47u,

    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_0     = 48u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_1     = 49u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_2     = 50u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_3     = 51u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_4     = 52u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_5     = 53u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_6     = 54u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_I_7     = 55u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_0     = 56u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_1     = 57u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_2     = 58u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_3     = 59u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_4     = 60u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_5     = 61u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_6     = 62u,
    ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7     = 63u,
    ADRV903X_JESD_FRM_SPLXBAR_LAST_VALID        = ADRV903X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7,
    ADRV903X_JESD_FRM_SPLXBAR_INVALID           = 0x7Fu
} adrv903x_JesdFramerSampleXbarSelect_e;

/*!< List of Jesd Frame Sync options */
typedef enum adrv903x_JesdFrmSyncbMode
{
    ADRV903X_JESD_FRM_SYNCB_PIN_MODE = 0u,
    ADRV903X_JESD_FRM_SYNCB_SPI_MODE = 1u
} adrv903x_JesdFrmSyncbMode_e;

/*!< List of Jesd Frame Pad options */
typedef enum adrv903x_JesdFrmSyncPadReq
{
    ADRV903X_JESD_FRM_PWR_ON_SYNC_PAD1      = 0u,
    ADRV903X_JESD_FRM_PWR_ON_SYNC_PAD2      = 1u,
    ADRV903X_JESD_FRM_PWR_ON_SYNC_PAD3      = 2u,
    ADRV903X_JESD_FRM_PWR_OFF_ALL_SYNC_PADS = 3u
} adrv903x_JesdFrmSyncPadReq_e;

typedef enum adrv903x_JesdFrmSyncbInSelect
{
    ADRV903X_JESD_FRM_SYNCB_CMOS                = 0u,
    ADRV903X_JESD_FRM_SYNCB_LVDS_WITH_INTL_TERM = 1u,
    ADRV903X_JESD_FRM_SYNCB_LVDS_NO_INTL_TERM   = 2u,
} adrv903x_JesdFrmSyncbInSelect_e;

typedef enum adrv903x_JesdDfrmSyncPadReq
{
    ADRV903X_JESD_DFRM_PWR_ON_SYNC_PAD1      = 0u,
    ADRV903X_JESD_DFRM_PWR_ON_SYNC_PAD2      = 1u,
    ADRV903X_JESD_DFRM_PWR_ON_ALL_SYNC_PADS  = 2u,
    ADRV903X_JESD_DFRM_PWR_OFF_ALL_SYNC_PADS = 3u
} adrv903x_JesdDfrmSyncPadReq_e;

typedef enum adrv903x_JesdJtxOutputDriveSwing
{
    ADRV903X_JESD_DRIVE_SWING_VTT_100    = 0u,
    ADRV903X_JESD_DRIVE_SWING_VTT_85     = 1u,
    ADRV903X_JESD_DRIVE_SWING_VTT_75     = 2u,
    ADRV903X_JESD_DRIVE_SWING_VTT_50     = 3u,
    ADRV903X_JESD_DRIVE_SWING_LAST_VALID = ADRV903X_JESD_DRIVE_SWING_VTT_50
} adrv903x_JesdJtxOutputDriveSwing_e;

typedef enum adrv903x_JesdJtxPreEmphasis
{
    ADRV903X_JESD_PRE_TAP_LEVEL_0_DB       = 0u,
    ADRV903X_JESD_PRE_TAP_LEVEL_3_DB       = 1u,
    ADRV903X_JESD_PRE_TAP_LEVEL_6_DB       = 2u,
    ADRV903X_JESD_PRE_TAP_LEVEL_LAST_VALID = ADRV903X_JESD_PRE_TAP_LEVEL_6_DB
} adrv903x_JesdJtxPreEmphasis_e;

typedef enum adrv903x_JesdJtxPostEmphasis
{
    ADRV903X_JESD_POST_TAP_LEVEL_0_DB       = 0u,
    ADRV903X_JESD_POST_TAP_LEVEL_3_DB       = 1u,
    ADRV903X_JESD_POST_TAP_LEVEL_6_DB       = 2u,
    ADRV903X_JESD_POST_TAP_LEVEL_9_DB       = 3u,
    ADRV903X_JESD_POST_TAP_LEVEL_12_DB      = 4u,
    ADRV903X_JESD_POST_TAP_LEVEL_LAST_VALID = ADRV903X_JESD_POST_TAP_LEVEL_12_DB
} adrv903x_JesdJtxPostEmphasis_e;

typedef enum adrv903x_JesdConvClockMode
{
    ADRV903X_JESD_NO_RESAMPLE_FIR = 0u,
    ADRV903X_JESD_2_3_RESAMPLE_FIR,
    ADRV903X_JESD_3_4_RESAMPLE_FIR
} adrv903x_JesdConvClockMode_e;

/* Bit macros for newSysref byte in Framer and Deframer structures */
#define ADRV903X_JESD_BITM_SYSREF_FOR_RELINK            (0x1U)
#define ADRV903X_JESD_BITP_SYSREF_FOR_RELINK            (0x0U)

#define ADRV903X_JESD_BITM_SYSREF_FOR_STARTUP           (0x2U)
#define ADRV903X_JESD_BITP_SYSREF_FOR_STARTUP           (0x1U)

#define ADRV903X_JESD_BITM_SYSREF_N_SHOT_COUNT          (0x3CU)
#define ADRV903X_JESD_BITP_SYSREF_N_SHOT_COUNT          (0x2U)

#define ADRV903X_JESD_BITM_SYSREF_N_SHOT_ENABLE         (0x40U)
#define ADRV903X_JESD_BITP_SYSREF_N_SHOT_ENABLE         (0x6U)

#define ADRV903X_JESD_BITM_SYSREF_IGNORE_WHEN_LINKED    (0x80U)
#define ADRV903X_JESD_BITP_SYSREF_IGNORE_WHEN_LINKED    (0x7U)

ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdFramerConfig
{
    uint8_t  mode;                                              /*!< 0 = 204B, 1 = 204C */
    uint8_t  E;                                                 /*!< Blocks in Multi-block for 204C */
    uint8_t  bankId;                                            /*!< Bank ID extension to Device ID (0-15)*/
    uint8_t  deviceId;                                          /*!< Device ID link identification number (0-255) */
    uint8_t  lane0Id;                                           /*!< Starting Lane ID (0-31) */
    uint8_t  M;                                                 /*!< Number of ADCs/converters (0,2,4,8, 16) */
    uint8_t  Kminus1;                                           /*!< Number of frames in a multiframe - 1 (0 - 255) */
    uint8_t  F;                                                 /*!< Number of bytes (octets) per frame. (1,2,4,8) */
    uint8_t  Np;                                                /*!< Converter sample resolution in bits (12,16,24) */
    uint8_t  S;                                                 /*!< Number of samples per converter per frame */
    uint8_t  scramble;                                          /*!< Scrambling enable */
    uint8_t  serializerLanesEnabled;                            /*!< Serializer lane select bit field */
    uint8_t  lmfcOffset[2];                                     /*!< LMFC offset value for deterministic latency setting */
                                                                /*!<      lmfcOffset[0] - Bits  [7:0] of PhaseAdjust */
                                                                /*!<      lmfcOffset[1] - Bits [18:8] of PhaseAdjust */
    uint8_t  newSysref;                                         /*!< Flags for SYSREF control. */
    uint8_t  syncbInSPIMode;                                    /*!< SYNCb SPI/Pin Mode selection */
    uint8_t  syncbInSelect;                                     /*!< SYNCb input source */
    uint8_t  syncbInLvdsMode;                                   /*!< SYNCb LVDS mode enable: 0=CMOS mode,
                                                                 *                           1=LVDS mode,
                                                                 *                           2=LVDS mode with no internal termination */
    uint8_t  syncbInLvdsPnInvert;                               /*!< SYNCb LVDS PN inverted */
    uint8_t  laneXbar[ADRV903X_JESD_MAX_SERIALIZER_LANES];      /*!< Lane crossbar selection */
    uint8_t  sampleXBar[ADRV903X_JESD_MAX_FRM_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  reserved[2];                                       /*!< Reserved */
    uint32_t iqRate_kHz;                                        /*!< Framer I/Q rate */
    uint32_t laneRate_kHz;                                      /*!< Framer Lane rate */
    uint8_t  subclass;                                          /*!< Framer subclass setting */
    uint8_t  convClockMode;                                     /*!< Conv clock mode: 0 - No resampling fir used */
                                                                /*!<                  1 - 2/3 resampling fir used */
                                                                /*!<                  2 - 3/4 resampling fir used */

    uint8_t reserved1[5];                                       /*!< Reserved */
} adrv903x_JesdFramerConfig_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdDeframerConfig
{
    uint8_t  mode;                                               /*!< 0 = 204B, 1 = 204C  */
    uint8_t  E;                                                  /*!< Blocks in Multiblock for 204C */
    uint8_t  bankId;                                             /*!< Bank ID extension to Device ID */
    uint8_t  deviceId;                                           /*!< Device ID link identification number */
    uint8_t  lane0Id;                                            /*!< Starting Lane ID */
    uint8_t  M;                                                  /*!< Number of ADCs */
    uint8_t  Kminus1;                                            /*!< Number of frames in a multiframe - 1 (0 - 255) */
    uint8_t  F;                                                  /*!< Number of bytes (octets) per frame. (1,2,4,8) */
    uint8_t  Np;                                                 /*!< Converter sample resolution in bits (12,16,24) */
    uint8_t  S;                                                  /*!< Number of samples per converter per frame */
    uint8_t  scramble;                                           /*!< Scrambling enable */
    uint8_t  deserializerLanesEnabled;                           /*!< Deserializer lane select bit field */
    uint8_t  lmfcOffset[2];                                      /*!< LMFC offset value for deterministic latency setting. */
                                                                 /*!<      lmfcOffset[0] - Bits  [7:0] of PhaseAdjust */
                                                                 /*!<      lmfcOffset[1] - Bits [18:8] of PhaseAdjust */
    uint8_t  newSysref;                                          /*!< Flags for SYSREF control. */
    uint8_t  syncbOutSelect;                                     /*!< Selects deframer SYNCBOUT pin. */
    uint8_t  syncbOutLvdsMode;                                   /*!< SYNCb LVDS mode enable */
    uint8_t  syncbOutLvdsPnInvert;                               /*!< SYNCb LVDS PN Invert enable */
    uint8_t  syncbOutCmosDriveLevel;                             /*!< SYNCb CMOS drive_strength */
    uint8_t  laneXbar[ADRV903X_JESD_MAX_DESERIALIZER_LANES];     /*!< Lane crossbar selection */
    uint8_t  sampleXBar[ADRV903X_JESD_MAX_DFRM_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  reserved[1];                                        /*!< Reserved */
    uint32_t iqRate_kHz;                                         /*!< Deframer I/Q rate */
    uint32_t laneRate_kHz;                                       /*!< Deframer Lane rate */
    uint8_t  subclass;                                           /*!< Deframer subclass setting */
    uint8_t  convClockMode;                                      /*!< Conv clock mode: 0 - No resampling fir used */
                                                                 /*!<                  1 - 2/3 resampling fir used */
                                                                 /*!<                  2 - 3/4 resampling fir used */

    uint8_t interleavingEnabled;                                 /*!< Interleaving enabled for a deframer */
    uint8_t reserved1[1];                                        /*!< Reserved */
} adrv903x_JesdDeframerConfig_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdSerializerLane
{
    uint8_t serAmplitude;            /*!< Serializer lane amplitude */
    uint8_t serPreEmphasis;          /*!< Serializer lane pre-emphasis */
    uint8_t serPostEmphasis;         /*!< Serializer lane post-emphasis */
    uint8_t serInvertLanePolarity;   /*!< Serializer lane PN inversion select: 0 = do not invert. 1 = invert */
    uint8_t bitRepeatLog2;           /*!< Log2 of bit repeat ratio */
    uint8_t reserved[3];             /*!< Reserved bytes for alignment */
} adrv903x_JesdSerializerLane_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdDeserializerLane
{
    uint8_t  highBoost;                 /*!< High Boost enabled flag */
    uint8_t  desInvertLanePolarity;     /*!< Deserializer Lane PN inversion select: 0 = do not invert. 1 = invert */
    uint8_t  bitSplitLog2;              /*!< Log2 of bit split ratio */
    uint8_t  cpuId;                     /*!< Cpu Number to run on - 255 not used */
    uint32_t configOption[10];          /*!< For future use */
} adrv903x_JesdDeserializerLane_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdLinkSharingConfig
{
    uint8_t  linkSharingEnabled;                                 /*!< 0 = link sharing disabled, 1 = link sharing enabled */
    uint8_t  M;                                                  /*!< Number of ADCs */
    uint8_t  Kminus1;                                            /*!< Number of frames in a multiframe - 1 (0 - 255) */
    uint8_t  F;                                                  /*!< Number of bytes (octets) per frame. (1,2,4,8) */
    uint8_t  Np;                                                 /*!< Converter sample resolution in bits (12,16,24) */
    uint8_t  S;                                                  /*!< Number of samples per converter per frame */
    uint8_t  sampleXBar[ADRV903X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  convClockMode;                                      /*!< Conv clock mode: 0 - No resampling fir used */
                                                                 /*!<                  1 - 2/3 resampling fir used */
                                                                 /*!<                  2 - 3/4 resampling fir used */
    uint32_t iqRate_kHz;                                         /*!< Deframer I/Q rate */
} adrv903x_JesdLinkSharingConfig_t;
ADI_ADRV903X_PACK_FINISH

    ADI_ADRV903X_PACK_START
typedef struct adrv903x_JesdSettings
{
    adrv903x_JesdFramerConfig_t      framer[ADRV903X_JESD_FRAMER_NUM];                        /*!< Framer configuration structure */
    adrv903x_JesdDeframerConfig_t    deframer[ADRV903X_JESD_DEFRAMER_NUM];                    /*!< Deframer configuration structure */
    adrv903x_JesdSerializerLane_t    serializerLane[ADRV903X_JESD_MAX_SERIALIZER_LANES];      /*!< Serializer configuration structure */
    adrv903x_JesdDeserializerLane_t  deserializerLane[ADRV903X_JESD_MAX_DESERIALIZER_LANES];  /*!< Deserializer configuration structure */
    adrv903x_JesdLinkSharingConfig_t linkSharingCfg[ADRV903X_JESD_LKSH_SWITCHER_NUM];         /*!< Link Sharing Switcher structure */
    uint8_t                          jrxAuxSysrefMode;                                        /*!< JRX SYSREF mode {0|1} */
    uint8_t                          jrxAuxSysrefClkDivideRatio;                              /*!< JRX aux sysref clk divide ratio */
    uint16_t                         jrxAuxSysrefK;                                           /*!< JRX aux sysref K */
    uint8_t                          jrxAuxSysrefDevClkRatio;                                 /*!< JRX aux sysref dev clk ratio */
    uint8_t                          jtxAuxSysrefMode;                                        /*!< JTX SYSREF mode {0|1} */
    uint8_t                          jtxAuxSysrefClkDivideRatio;                              /*!< JTX aux sysref clk divide ratio */
    uint16_t                         jtxAuxSysrefK;                                           /*!< JTX aux sysref K */
    uint8_t                          jtxAuxSysrefDevClkRatio;                                 /*!< JTX aux sysref dev clk ratio */
    uint8_t                          rxdesQhfrate;                                            /*!< DESER mode settings */
    uint8_t                          txserTxPathClkDiv;                                       /*!< SER mode settings */
    uint16_t                         devClkKminus1;                                           /*!< Number of multiframe in terms of dev_clk clock period minus 1 */
    uint8_t                          reserved[2];                                             /*!< Reserved bytes for alignment */
} adrv903x_JesdSettings_t;
ADI_ADRV903X_PACK_FINISH

#endif /* __ADRV903X_CPU_DEVICE_PROFILE_JESD_TYPES_H__ */


