/*! ****************************************************************************
 *
 * @file:    device_profile_ssi_t.h
 *
 * @brief:   SSI external data declarations
 *
 * @details: This file contains all the typedef SSI prototypes
 *           referenced globally.
 *
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_SSI_T_HEADER__
#define __DEVICE_PROFILE_SSI_T_HEADER__

#include "adi_device_profile_pack.h"

/**
  @addtogroup ssi
  @ingroup drivers

  Provides driver functions for SSI.
  @{
*/

/*! Rx SSI input interface enumeration */
typedef enum
{
    SSI_RX1_DP_RXNB_DP1 = 0,               /*!< RX1 DP/RXNB DP1 data path */
    SSI_RXNB_DP_DP2 = 1,                  /*!< RX NB data path/ RXNB DP2 data path */
    SSI_FIX_PTTN = 2,                      /*!< Fixed pattern */
    SSI_TX_LPBK = 3,                      /*!< TX loopback */
    SSI_SYS2_MEM2FIFO = 4,                /*!< SYS2 RX Memory to FIFO*/
    SSI_SYS1_EVNT_LOG = 5,                 /*!< SYS1 event logger */ 
    SSI_SYS2_EVNT_LOG = 6,                 /*!< SYS2 event logger */ 
    SSI_RX1_DP = 7,                       /*!< only for RX NB to select RX1 DP*/
    SSI_RXNB_DP12_IONLY = 8,              /*!< only for RX NB: Idata = DP1 Idata, Qata = DP2 Idata*/
} ADI_NEVIS_PACK_ENUM rxSsiDataSrc_e;

/*! Tx data input source enum*/
typedef enum
{
    TX_SSI = 0,               /*!< TX SSI */
    TX_SYS2 = 1,              /*!< TX data from PS2 memory*/
} ADI_NEVIS_PACK_ENUM txDataSrc_e;

/*! SSI type enumeration */
typedef enum
{
    SSI_TYPE_DISABLE,                        /*!< Disable */
    SSI_TYPE_CMOS,                           /*!< CMOS */
    SSI_TYPE_LVDS                            /*!< LVDS */
} ADI_NEVIS_PACK_ENUM ssiType_e;

/*! SSI data format enumeration */
typedef enum
{
    SSI_FORMAT_2_BIT_SYMBOL_DATA,           /*!< 2  bit symbol data (CMOS) */
    SSI_FORMAT_8_BIT_SYMBOL_DATA,           /*!< 8  bit symbol data (CMOS) */
    SSI_FORMAT_16_BIT_SYMBOL_DATA,          /*!< 16 bit symbol data (CMOS) */
    SSI_FORMAT_12_BIT_I_Q_DATA,             /*!< 12 bit I/Q data (LVDS) */
    SSI_FORMAT_16_BIT_I_Q_DATA,             /*!< 16 bit I/Q data (CMOS/LVDS) */
    SSI_FORMAT_16_BIT_I_Q_DATA_15_BIT_GAIN_CHANGE_ONLY,         /*!< 15 bit I/Q data, 1 bit gain change for RX SSI */
    SSI_FORMAT_32_BIT_I_Q_DATA_22_BIT_GAIN_INDEX_AND_CHANGE,    /*!< 22 bit I/Q data, 1 bit gain change, 5 bit gain for RX SSI */
} ADI_NEVIS_PACK_ENUM ssiDataFormat_e;

/*! SSI number of lane enumeration */
typedef enum
{
    SSI_1_LANE,                             /*!< 1 lane (CMOS/LVDS) */
    SSI_2_LANE,                             /*!< 2 lane (LVDS) */
    SSI_4_LANE                              /*!< 4 lane (CMOS) */
} ADI_NEVIS_PACK_ENUM ssiNumLane_e;

/*! SSI type enumeration */
typedef enum
{
    SSI_SHORT_STROBE,
    SSI_LONG_STROBE
} ADI_NEVIS_PACK_ENUM ssiStrobeType_e;

/*! Tx SSI reference clock pin option enumeration */
typedef enum
{
    TX_REF_CLOCK_PIN_DISABLED                   = 0u,   /*!< Tx reference clock out pin disabled */
    TX_REF_CLOCK_PIN_LVDS_DCLK_OUT_ENABLED      = 1u,   /*!< Tx reference clock out pin TX_DCLK_OUT+/- enabled in LVDS mode */
    TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_P_ENABLED    = 1u,   /*!< Tx reference clock out pin TX_DCLK_OUT+   enabled in CMOS mode */
    TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_N_ENABLED    = 2u,   /*!< Tx reference clock out pin TX_DCLK_OUT-   enabled in CMOS mode */
    TX_REF_CLOCK_PIN_CMOS_STROBE_IN_N_ENABLED   = 3u,   /*!< Tx reference clock out pin TX_STROBE_IN-  enabled in CMOS mode */
} ADI_NEVIS_PACK_ENUM ssiTxRefClockPin_e;


/****************************************************************************
* SSI configuration data structure.
****************************************************************************/
ADI_NEVIS_PACK_START
typedef struct {
    ssiType_e           ssiType;                /*!< SSI type */
    ssiDataFormat_e     ssiDataFormatSel;       /*!< SSI data format */
    ssiNumLane_e        numLaneSel;             /*!< SSI number of lanes */
    ssiStrobeType_e     strobeType;             /*!< SSI strobe type */
    uint8_t             lsbFirst;               /*!< SSI LSB first */
    uint8_t             qFirst;                 /*!< SSI Q data first */
    ssiTxRefClockPin_e  txRefClockPin;          /*!< SSI TX reference clock pin option */
    uint8_t             lvdsBitInversion;       /*!< LVDS SSI I/Q/Strobe bit inversions. I bit inversion. Rx: Inverts I and Q. Tx: Inverts I only; Q bit inversion. Rx: Has no effect. Tx: Inverts Q only */
    uint8_t             lvdsUseLsbIn12bitMode;  /*!< LVDS use LSB in 12 bit mode */
    rxSsiDataSrc_e      rxSsiDataSrc;           /*!< RX/RXNB data source  */
    txDataSrc_e         txDataSrc;
    uint8_t             lvdsRxClkInversionEn;   /*!< LVDS Rx clock inversion enable */
    uint8_t             cmosTxDdrNegStrobeEn;
    uint8_t             cmosDdrPosClkEn;        /*!< CMOS DDR positive clock enable */
    uint8_t             cmosClkInversionEn;     /*!< CMOS DDR/SDR clock inversion enable */
    uint8_t             ddrEn;                  /*!< DDR enable */
    uint8_t             rxMaskStrobeEn;         /*!< RX mask strobe enable (for SSI interpolation mode only) */
    uint8_t             padding[3];
} ssiConfig_t;
ADI_NEVIS_PACK_FINISH


/** @} */

#endif /* __DEVICE_PROFILE_SSI_T_HEADER__ */
