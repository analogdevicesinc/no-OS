/**
 * \file adrv904x_cpu_device_profile_jesd_types.h
 *
 * \brief   Contains ADRV904X Device Profile JESD type definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_DEVICE_PROFILE_JESD_TYPES_H__
#define __ADRV904X_CPU_DEVICE_PROFILE_JESD_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"


#define ADRV904X_JESD_MAX_DESERIALIZER_LANES      (8u)
#define ADRV904X_JESD_MAX_SERIALIZER_LANES        (8u)
#define ADRV904X_JESD_MAX_FRM_SAMPLE_XBAR_IDX     (64u)
#define ADRV904X_JESD_MAX_DFRM_SAMPLE_XBAR_IDX    (32u)
#define ADRV904X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX    (32u)
/* ADRV904X_JESD_NUM_TXRX_CHAN == ADRV904X_NUM_TXRX_CHAN */
#define ADRV904X_JESD_NUM_TXRX_CHAN               (8u)

#define ADRV904X_JESD_MAX_CARRIER_SLOTS            (64u)  /*!< ADRV904X JESD Carrier slots         */
#define ADRV904X_JESD_MAX_FRM_CARRIER_XBAR_IDX     (192u) /*!< ADRV904X JESD Max FRM XBAR Carrier  */
#define ADRV904X_JESD_MAX_DFRM_CARRIER_XBAR_IDX    (192u) /*!< ADRV904X JESD Max DFRM XBAR Carrier */

typedef enum adrv904x_JesdVersion
{
    ADRV904X_JESD_204B            = 0u,
    ADRV904X_JESD_204C            = 1u,
    ADRV904X_JESD_LAST_VALID_MODE = ADRV904X_JESD_204C
} adrv904x_JesdVersion_e;

typedef uint8_t adrv904x_JesdVersion_t;

/*!< List of Framers */
typedef enum adrv904x_JesdFramer
{
    ADRV904X_JESD_FRAMER_0   = 0u,   /*!< Framer 0 selection */
    ADRV904X_JESD_FRAMER_1   = 1u,   /*!< Framer 1 selection */
    ADRV904X_JESD_FRAMER_2   = 2u,   /*!< Framer 2 selection */
    ADRV904X_JESD_FRAMER_NUM = 3u
} adrv904x_JesdFramer_e;

/*!< List of Deframers */
typedef enum adrv904x_JesdDeframer
{
    ADRV904X_JESD_DEFRAMER_0   = 0u,   /*!< Deframer 0 selection */
    ADRV904X_JESD_DEFRAMER_1   = 1u,   /*!< Deframer 1 selection */
    ADRV904X_JESD_DEFRAMER_NUM = 2u
} adrv904x_JesdDeframer_e;

/*!< List of Deframers */
typedef enum adrv904x_JesdLkShSwitcher
{
    ADRV904X_JESD_LKSH_SWITCHER_0   = 0u,   /*!< Link Sharing Switcher 0 selection */
    ADRV904X_JESD_LKSH_SWITCHER_1   = 1u,   /*!< Link Sharing Switcher 1 selection */
    ADRV904X_JESD_LKSH_SWITCHER_NUM = 2u
} adrv904x_JesdLkShSwitcher_e;

/*!< List of DAC Sample Xbar options */
typedef enum adrv904x_JesdDeframerSampleXbarSelect
{
    ADRV904X_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_I = 0u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX0_BAND_0_DATA_Q = 1u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_I = 2u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX1_BAND_0_DATA_Q = 3u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_I = 4u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX2_BAND_0_DATA_Q = 5u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_I = 6u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX3_BAND_0_DATA_Q = 7u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_I = 8u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX4_BAND_0_DATA_Q = 9u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_I = 10u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX5_BAND_0_DATA_Q = 11u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_I = 12u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX6_BAND_0_DATA_Q = 13u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_I = 14u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX7_BAND_0_DATA_Q = 15u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_I = 16u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX0_BAND_1_DATA_Q = 17u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_I = 18u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX1_BAND_1_DATA_Q = 19u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_I = 20u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX2_BAND_1_DATA_Q = 21u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_I = 22u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX3_BAND_1_DATA_Q = 23u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_I = 24u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX4_BAND_1_DATA_Q = 25u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_I = 26u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX5_BAND_1_DATA_Q = 27u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_I = 28u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX6_BAND_1_DATA_Q = 29u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_I = 30u,
    ADRV904X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_Q = 31u,
    ADRV904X_JESD_DFRM_SPLXBAR_LAST_VALID        = ADRV904X_JESD_DFRM_SPLXBAR_TX7_BAND_1_DATA_Q,
    ADRV904X_JESD_DFRM_SPLXBAR_INVALID           = 0x7Fu
} adrv904x_JesdDeframerSampleXbarSelect_e;

/*!< List of ADC Sample Xbar options */
typedef enum adrv904x_JesdFramerSampleXbarSelect
{
    ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I = 0u,
    ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q = 1u,
    ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_I = 2u,
    ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_Q = 3u,
    ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I = 4u,
    ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q = 5u,
    ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_I = 6u,
    ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_Q = 7u,
    ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I = 8u,
    ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q = 9u,
    ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_I = 10u,
    ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_Q = 11u,
    ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I = 12u,
    ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q = 13u,
    ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_I = 14u,
    ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_Q = 15u,

    ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I = 16u,
    ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q = 17u,
    ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_I = 18u,
    ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_Q = 19u,
    ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I = 20u,
    ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q = 21u,
    ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_I = 22u,
    ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_Q = 23u,
    ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I = 24u,
    ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q = 25u,
    ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_I = 26u,
    ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_Q = 27u,
    ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I = 28u,
    ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q = 29u,
    ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_I = 30u,
    ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q = 31u,
    ADRV904X_JESD_FRM_SPLXBAR_LAST_VALID_NO_INT = ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q, /* With no interleaving enabled */

    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_0     = 32u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_1     = 33u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_2     = 34u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_3     = 35u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_4     = 36u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_5     = 37u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_6     = 38u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_7     = 39u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_0     = 40u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_1     = 41u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_2     = 42u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_3     = 43u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_4     = 44u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_5     = 45u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_6     = 46u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_7     = 47u,

    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_0     = 48u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_1     = 49u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_2     = 50u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_3     = 51u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_4     = 52u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_5     = 53u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_6     = 54u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_7     = 55u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_0     = 56u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_1     = 57u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_2     = 58u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_3     = 59u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_4     = 60u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_5     = 61u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_6     = 62u,
    ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7     = 63u,
    ADRV904X_JESD_FRM_SPLXBAR_LAST_VALID        = ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7,
    ADRV904X_JESD_FRM_SPLXBAR_INVALID           = 0x7Fu
} adrv904x_JesdFramerSampleXbarSelect_e;

/*!< List of Jesd Frame Sync options */
typedef enum adrv904x_JesdFrmSyncbMode
{
    ADRV904X_JESD_FRM_SYNCB_PIN_MODE = 0u,
    ADRV904X_JESD_FRM_SYNCB_SPI_MODE = 1u
} adrv904x_JesdFrmSyncbMode_e;

/*!< List of Jesd Frame Pad options */
typedef enum adrv904x_JesdFrmSyncPadReq
{
    ADRV904X_JESD_FRM_PWR_ON_SYNC_PAD1      = 0u,
    ADRV904X_JESD_FRM_PWR_ON_SYNC_PAD2      = 1u,
    ADRV904X_JESD_FRM_PWR_ON_SYNC_PAD3      = 2u,
    ADRV904X_JESD_FRM_PWR_OFF_ALL_SYNC_PADS = 3u
} adrv904x_JesdFrmSyncPadReq_e;

typedef enum adrv904x_JesdFrmSyncbInSelect
{
    ADRV904X_JESD_FRM_SYNCB_CMOS                = 0u,
    ADRV904X_JESD_FRM_SYNCB_LVDS_WITH_INTL_TERM = 1u,
    ADRV904X_JESD_FRM_SYNCB_LVDS_NO_INTL_TERM   = 2u,
} adrv904x_JesdFrmSyncbInSelect_e;

typedef enum adrv904x_JesdDfrmSyncPadReq
{
    ADRV904X_JESD_DFRM_PWR_ON_SYNC_PAD1      = 0u,
    ADRV904X_JESD_DFRM_PWR_ON_SYNC_PAD2      = 1u,
    ADRV904X_JESD_DFRM_PWR_ON_ALL_SYNC_PADS  = 2u,
    ADRV904X_JESD_DFRM_PWR_OFF_ALL_SYNC_PADS = 3u
} adrv904x_JesdDfrmSyncPadReq_e;

typedef enum adrv904x_JesdJtxOutputDriveSwing
{
    ADRV904X_JESD_DRIVE_SWING_VTT_100    = 0u,
    ADRV904X_JESD_DRIVE_SWING_VTT_85     = 1u,
    ADRV904X_JESD_DRIVE_SWING_VTT_75     = 2u,
    ADRV904X_JESD_DRIVE_SWING_VTT_50     = 3u,
    ADRV904X_JESD_DRIVE_SWING_LAST_VALID = ADRV904X_JESD_DRIVE_SWING_VTT_50
} adrv904x_JesdJtxOutputDriveSwing_e;

typedef enum adrv904x_JesdJtxPreEmphasis
{
    ADRV904X_JESD_PRE_TAP_LEVEL_0_DB       = 0u,
    ADRV904X_JESD_PRE_TAP_LEVEL_3_DB       = 1u,
    ADRV904X_JESD_PRE_TAP_LEVEL_6_DB       = 2u,
    ADRV904X_JESD_PRE_TAP_LEVEL_LAST_VALID = ADRV904X_JESD_PRE_TAP_LEVEL_6_DB
} adrv904x_JesdJtxPreEmphasis_e;

typedef enum adrv904x_JesdJtxPostEmphasis
{
    ADRV904X_JESD_POST_TAP_LEVEL_0_DB       = 0u,
    ADRV904X_JESD_POST_TAP_LEVEL_3_DB       = 1u,
    ADRV904X_JESD_POST_TAP_LEVEL_6_DB       = 2u,
    ADRV904X_JESD_POST_TAP_LEVEL_9_DB       = 3u,
    ADRV904X_JESD_POST_TAP_LEVEL_12_DB      = 4u,
    ADRV904X_JESD_POST_TAP_LEVEL_LAST_VALID = ADRV904X_JESD_POST_TAP_LEVEL_12_DB
} adrv904x_JesdJtxPostEmphasis_e;

typedef enum adrv904x_JesdConvClockMode
{
    ADRV904X_JESD_NO_RESAMPLE_FIR = 0u,
    ADRV904X_JESD_2_3_RESAMPLE_FIR,
    ADRV904X_JESD_3_4_RESAMPLE_FIR
} adrv904x_JesdConvClockMode_e;

/* Bit macros for newSysref byte in Framer and Deframer structures */
#define ADRV904X_JESD_BITM_SYSREF_FOR_RELINK            (0x1U)
#define ADRV904X_JESD_BITP_SYSREF_FOR_RELINK            (0x0U)

#define ADRV904X_JESD_BITM_SYSREF_FOR_STARTUP           (0x2U)
#define ADRV904X_JESD_BITP_SYSREF_FOR_STARTUP           (0x1U)

#define ADRV904X_JESD_BITM_SYSREF_N_SHOT_COUNT          (0x3CU)
#define ADRV904X_JESD_BITP_SYSREF_N_SHOT_COUNT          (0x2U)

#define ADRV904X_JESD_BITM_SYSREF_N_SHOT_ENABLE         (0x40U)
#define ADRV904X_JESD_BITP_SYSREF_N_SHOT_ENABLE         (0x6U)

#define ADRV904X_JESD_BITM_SYSREF_IGNORE_WHEN_LINKED    (0x80U)
#define ADRV904X_JESD_BITP_SYSREF_IGNORE_WHEN_LINKED    (0x7U)

ADI_ADRV904X_PACK_START
/*!< adrv904x_JesdCarrierXBarSelect structure containing Jesd Carrier XBar Selection Parameters */
typedef struct adrv904x_JesdCarrierXBarSelect
{
    uint8_t chanSelect;                                /*!< Rx/Tx slice selection */
    uint8_t slotSelect;                                /*!< carrier slot selection */
} adrv904x_JesdCarrierXBarSelect_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeJesdFramerConfig structure containing Jesd Framer Configuration Parameters */
typedef struct adrv904x_DfeJesdFramerConfig
{
    adrv904x_JesdCarrierXBarSelect_t carrierXBar[ADRV904X_JESD_MAX_FRM_CARRIER_XBAR_IDX];           /*!< cDDC sample crossbar */
    uint8_t                          jesd204cInterleave;                                            /*!< 204c interleave (adrv904x_Jesd204cInterleaveMode_e) */

    uint8_t                          reserved[3];                                                   /*!< Reserved */
} adrv904x_DfeJesdFramerConfig_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeJesdDeframerConfig structure containing Jesd DeFramer Configuration Parameters */
typedef struct adrv904x_DfeJesdDeframerConfig
{
    adrv904x_JesdCarrierXBarSelect_t carrierXBar[ADRV904X_JESD_MAX_DFRM_CARRIER_XBAR_IDX];          /*!< cDUC sample crossbar */
    uint8_t                          jesd204cInterleave;                                            /*!< 204c interleave */

    uint8_t                          reserved[3];                                                   /*!< Reserved */
} adrv904x_DfeJesdDeframerConfig_t;

ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeJesdSettings structure containing Jesd Settings */
typedef struct adrv904x_DfeJesdSettings
{
    adrv904x_DfeJesdFramerConfig_t   framer[ADRV904X_JESD_FRAMER_NUM];                            /*!< Framer configuration structure */
    adrv904x_DfeJesdDeframerConfig_t deframer[ADRV904X_JESD_DEFRAMER_NUM];                        /*!< Deframer configuration structure */
    uint8_t                          txBand1Enable;                                               /*!< Tx Band1 data present in CDUC bypass operation. 1 bit per channel. */
    uint8_t                          txCducSyncSelect;                                            /*!< Selects the DFRM which generates the Tx Cduc sync source. 1 bit per channel. */
    uint8_t                          txClkEnable[ADRV904X_JESD_NUM_TXRX_CHAN];                    /*!< enable cDUC crossbar clock generation */
    uint8_t                          txCducSampleClkDiv[ADRV904X_JESD_NUM_TXRX_CHAN];             /*!< clock divider for CDUC deinterleaved samples */
    uint8_t                          txCducInterfaceClkDiv[ADRV904X_JESD_NUM_TXRX_CHAN];          /*!< clock divider for cDUC internal interface data (serialized/interleaved) */
    uint8_t                          txSampleSlotBypass;                                          /*!< bypass cDUC crossbar. 1 bit per deframer */
    uint8_t                          rxClkEnable[ADRV904X_JESD_NUM_TXRX_CHAN];                    /*!< enable cDDC crossbar clock generation */
    uint8_t                          rxCddcSampleClkDiv[ADRV904X_JESD_NUM_TXRX_CHAN];             /*!< clock divider for cDDC deinterleaved samples */
    uint8_t                          rxCddcInterfaceClkDiv[ADRV904X_JESD_NUM_TXRX_CHAN];          /*!< clock divider for cDDC internal interface data (serialized/interleaved) */
    uint8_t                          rxSampleSlotBypass;                                          /*!< bypass cDDC crossbar. 1 bit per framer[1:0] */

    uint8_t                          reserved[4];                                                 /*!< Reserved */
} adrv904x_DfeJesdSettings_t;

ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdFramerConfig
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
    uint8_t  laneXbar[ADRV904X_JESD_MAX_SERIALIZER_LANES];      /*!< Lane crossbar selection */
    uint8_t  sampleXBar[ADRV904X_JESD_MAX_FRM_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  reserved[2];                                       /*!< Reserved */
    uint32_t iqRate_kHz;                                        /*!< Framer I/Q rate */
    uint32_t laneRate_kHz;                                      /*!< Framer Lane rate */
    uint8_t  subclass;                                          /*!< Framer subclass setting */
    uint8_t  convClockMode;                                     /*!< Conv clock mode: 0 - No resampling fir used */
                                                                /*!<                  1 - 2/3 resampling fir used */
                                                                /*!<                  2 - 3/4 resampling fir used */

    uint8_t reserved1[5];                                       /*!< Reserved */
} adrv904x_JesdFramerConfig_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdDeframerConfig
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
    uint8_t  laneXbar[ADRV904X_JESD_MAX_DESERIALIZER_LANES];     /*!< Lane crossbar selection */
    uint8_t  sampleXBar[ADRV904X_JESD_MAX_DFRM_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  reserved[1];                                        /*!< Reserved */
    uint32_t iqRate_kHz;                                         /*!< Deframer I/Q rate */
    uint32_t laneRate_kHz;                                       /*!< Deframer Lane rate */
    uint8_t  subclass;                                           /*!< Deframer subclass setting */
    uint8_t  convClockMode;                                      /*!< Conv clock mode: 0 - No resampling fir used */
                                                                 /*!<                  1 - 2/3 resampling fir used */
                                                                 /*!<                  2 - 3/4 resampling fir used */

    uint8_t reserved1[2];                                        /*!< Reserved */
} adrv904x_JesdDeframerConfig_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdSerializerLane
{
    uint8_t serAmplitude;            /*!< Serializer lane amplitude */
    uint8_t serPreEmphasis;          /*!< Serializer lane pre-emphasis */
    uint8_t serPostEmphasis;         /*!< Serializer lane post-emphasis */
    uint8_t serInvertLanePolarity;   /*!< Serializer lane PN inversion select: 0 = do not invert. 1 = invert */
    uint8_t bitRepeatLog2;           /*!< Log2 of bit repeat ratio */
    uint8_t reserved[3];             /*!< Reserved bytes for alignment */
} adrv904x_JesdSerializerLane_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdDeserializerLane
{
    uint8_t  highBoost;                 /*!< High Boost enabled flag */
    uint8_t  desInvertLanePolarity;     /*!< Deserializer Lane PN inversion select: 0 = do not invert. 1 = invert */
    uint8_t  bitSplitLog2;              /*!< Log2 of bit split ratio */
    uint8_t  cpuId;                     /*!< Cpu Number to run on - 255 not used */
    uint32_t configOption[10];          /*!< For future use */
} adrv904x_JesdDeserializerLane_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdLinkSharingConfig
{
    uint8_t  linkSharingEnabled;                                 /*!< 0 = link sharing disabled, 1 = link sharing enabled */
    uint8_t  M;                                                  /*!< Number of ADCs */
    uint8_t  Kminus1;                                            /*!< Number of frames in a multiframe - 1 (0 - 255) */
    uint8_t  F;                                                  /*!< Number of bytes (octets) per frame. (1,2,4,8) */
    uint8_t  Np;                                                 /*!< Converter sample resolution in bits (12,16,24) */
    uint8_t  S;                                                  /*!< Number of samples per converter per frame */
    uint8_t  sampleXBar[ADRV904X_JESD_MAX_LKSH_SAMPLE_XBAR_IDX]; /*!< Converter sample crossbar selection */
    uint8_t  convClockMode;                                      /*!< Conv clock mode: 0 - No resampling fir used */
                                                                 /*!<                  1 - 2/3 resampling fir used */
                                                                 /*!<                  2 - 3/4 resampling fir used */
    uint32_t iqRate_kHz;                                         /*!< Deframer I/Q rate */
} adrv904x_JesdLinkSharingConfig_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_JesdSettings
{
    adrv904x_JesdFramerConfig_t      framer[ADRV904X_JESD_FRAMER_NUM];                        /*!< Framer configuration structure */
    adrv904x_JesdDeframerConfig_t    deframer[ADRV904X_JESD_DEFRAMER_NUM];                    /*!< Deframer configuration structure */
    adrv904x_JesdSerializerLane_t    serializerLane[ADRV904X_JESD_MAX_SERIALIZER_LANES];      /*!< Serializer configuration structure */
    adrv904x_JesdDeserializerLane_t  deserializerLane[ADRV904X_JESD_MAX_DESERIALIZER_LANES];  /*!< Deserializer configuration structure */
    adrv904x_JesdLinkSharingConfig_t linkSharingCfg[ADRV904X_JESD_LKSH_SWITCHER_NUM];         /*!< Link Sharing Switcher structure */
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
} adrv904x_JesdSettings_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_DEVICE_PROFILE_JESD_TYPES_H__ */


