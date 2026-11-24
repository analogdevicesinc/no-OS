/**
 * \file
 * \brief Types for FPGA910X Synchronous Serial Interface (SSI) configuration
 *
 * Copyright (c) 2018 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_SSI_TYPES_H_
#define _ADI_FPGA910X_SSI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_adrv910x_types.h"

#define ADI_FPGA910X_NUM_CHANNELS 2

/**
 * \brief Enumeration of SSI type
 */
typedef enum adi_fpga910x_SsiType
{
    ADI_FPGA910X_SSI_TYPE_DISABLE = 0,  /*!< Disable SSI Type */
    ADI_FPGA910X_SSI_TYPE_CMOS = 1,     /*!< CMOS SSI Type */
    ADI_FPGA910X_SSI_TYPE_LVDS = 2      /*!< LVDS SSI Type */
} adi_fpga910x_SsiType_e;

/**
 * \brief SSI formats
 * \note Copy of adi_adrv910x_SsiDataFormat_e
 */
typedef enum adi_fpga910x_SsiFormat
{
    ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA,                                     /*!< 2 bit symbol data (CMOS) */
    ADI_FPGA910X_SSI_FORMAT_8_BIT_SYMBOL_DATA,                                     /*!< 8 bit symbol data (CMOS) */
    ADI_FPGA910X_SSI_FORMAT_16_BIT_SYMBOL_DATA,                                    /*!< 16 bit symbol data (CMOS) */
    ADI_FPGA910X_SSI_FORMAT_12_BIT_I_Q_DATA,                                       /*!< 12 bit I/Q data (LVDS) */
    ADI_FPGA910X_SSI_FORMAT_16_BIT_I_Q_DATA,                                       /*!< 16 bit I/Q data (CMOS/LVDS) */
    ADI_FPGA910X_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE = 5,                 /*!< 15 bit I/Q data, 1 bit Rx Gain Change (CMOS/LVDS) */
    ADI_FPGA910X_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX = 6 /*!< 22 bit I/Q data, 1 bit Rx Gain Change, 8 bit Rx Gain Index (CMOS/LVDS) */
} adi_fpga910x_SsiFormat_e;

/**
 * \brief Type of data to transmit over SSI
 */
typedef enum adi_fpga910x_SsiTestModeData
{
    ADI_FPGA910X_SSI_TESTMODE_DATA_NORMAL = 0,  /*!< No check available for this data type */
    ADI_FPGA910X_SSI_TESTMODE_DATA_FIXED_PATTERN,
    ADI_FPGA910X_SSI_TESTMODE_DATA_RAMP_NIBBLE, /*!< CSSI-Only */
    ADI_FPGA910X_SSI_TESTMODE_DATA_RAMP_16_BIT, /*!< CSSI and LSSI */
    ADI_FPGA910X_SSI_TESTMODE_DATA_PRBS15,      /*!< LSSI-Only */
    ADI_FPGA910X_SSI_TESTMODE_DATA_PRBS7        /*!< LSSI-Only */
} adi_fpga910x_SsiTestModeData_e;

/**
 * \brief Enumeration of the SSI Strobe
 */
typedef enum adi_fpga910x_SsiStrobeType
{
    ADI_FPGA910X_SSI_SHORT_STROBE = 0,  /*!< Short SSI Strobe */
    ADI_FPGA910X_SSI_LONG_STROBE = 1    /*!< Long SSI Strobe */
} adi_fpga910x_SsiStrobeType_e;

/**
 * \brief Enumeration of the SSI number of lane
 */
typedef enum adi_fpga910x_SsiNumLane
{
    ADI_FPGA910X_SSI_1_LANE = 0,        /*!< 1 lane (CMOS/LVDS) */
    ADI_FPGA910X_SSI_2_LANE = 1,        /*!< 2 lane (LVDS) */
    ADI_FPGA910X_SSI_4_LANE = 2         /*!< 4 lane (CMOS) */
} adi_fpga910x_SsiNumLane_e;

/**
 * \brief Tx SSI reference clock pin option enumeration
 */
typedef enum adi_fpga910x_SsiTxRefClockPin
{
    ADI_FPGA910X_SSI_TX_REF_CLOCK_PIN_DISABLED = 0u,                /*!< Tx reference clock out pin disabled */
    ADI_FPGA910X_SSI_TX_REF_CLOCK_PIN_LVDS_DCLK_OUT_ENABLED = 1u,   /*!< Tx reference clock out pin TX_DCLK_OUT+/- enabled in LVDS mode */
    ADI_FPGA910X_SSI_TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_P_ENABLED = 1u, /*!< Tx reference clock out pin TX_DCLK_OUT+ enabled in CMOS mode */
    ADI_FPGA910X_SSI_TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_N_ENABLED = 2u, /*!< Tx reference clock out pin TX_DCLK_OUT- enabled in CMOS mode */
    ADI_FPGA910X_SSI_TX_REF_CLOCK_PIN_CMOS_STROBE_IN_N_ENABLED = 3u /*!< Tx reference clock out pin TX_STROBE_IN- enabled in CMOS mode */
} adi_fpga910x_SsiTxRefClockPin_e;

/**
 * \brief Data structure to hold FPGA910X SSI configuration.
 */
typedef struct adi_fpga910x_SsiConfig
{
    adi_fpga910x_SsiType_e ssiType;                /*!< SSI type */
    adi_fpga910x_SsiFormat_e ssiDataFormatSel;     /*!< SSI data format */
    adi_fpga910x_SsiNumLane_e numLaneSel;          /*!< SSI number of lanes */
    adi_fpga910x_SsiStrobeType_e strobeType;       /*!< SSI strobe type */
    uint8_t lsbFirst;                              /*!< SSI LSB first */
    uint8_t qFirst;                                /*!< SSI Q data first */
    adi_fpga910x_SsiTxRefClockPin_e txRefClockPin; /*!< Reference clock pin select */
    bool lvdsIBitInversion;                        /*!< LVDS SSI I bit inversion */
    bool lvdsQBitInversion;                        /*!< LVDS SSI Q bit inversion */
    bool lvdsStrobeBitInversion;                   /*!< LVDS SSI Strobe bit inversion */
    uint8_t lvdsUseLsbIn12bitMode;                 /*!< LVDS use LSB in 12 bit mode */
    bool lvdsRxClkInversionEn;                     /*!< LVDS Rx clock inversion enable */
    bool cmosDdrPosClkEn;                          /*!< CMOS DDR positive clock enable */
    bool cmosClkInversionEn;                       /*!< CMOS DDR clock inversion enable */
    bool ddrEn;                                    /*!< Double Data Rate (DDR) enable */
    bool rxMaskStrobeEn;                           /*!< Rx enable for strobe mask */
} adi_fpga910x_SsiConfig_t;

/**
 * \brief Data structure to transmit FPGA910X SSI test mode configuration
 */
typedef struct adi_fpga910x_TxSsiTestModeCfg
{
    adi_fpga910x_SsiTestModeData_e testData; /*!< Type of data to transmit over SSI */
    uint32_t fixedDataPatternToTransmit;     /*!< Value of Fixed pattern to transmit over interface */
} adi_fpga910x_TxSsiTestModeCfg_t;

/**
 * \brief Data structure to receive FPGA910X SSI test mode configuration
 */
typedef struct adi_fpga910x_RxSsiTestModeCfg
{
    adi_fpga910x_SsiTestModeData_e testData; /*!< Type of data to receive over SSI and check */
    uint32_t fixedDataPatternToCheck;        /*!< Value of Fixed pattern to check against pattern received over interface */
} adi_fpga910x_RxSsiTestModeCfg_t;

/**
 * \brief Data structure to hold FPGA910X CMOS delay calibration configuration for each channel.
 */
typedef struct adi_fpga910x_SsiCalibrationCfg
{
    uint8_t rxStrobeDelay[ADI_FPGA910X_NUM_CHANNELS]; /*!< Strobe delay for Rx1/Rx2 channels */
    uint8_t rxIDataDelay[ADI_FPGA910X_NUM_CHANNELS];  /*!< I data delay for Rx1/Rx2 channels */
    uint8_t rxQDataDelay[ADI_FPGA910X_NUM_CHANNELS];  /*!< Q data delay for Rx1/Rx2 channels */
    uint8_t txClkDelay;                               /*!< CLK delay for Tx channel */
    uint8_t txStrobeDelay;                            /*!< Strobe delay for Tx channel */
    uint8_t txIDataDelay;                             /*!< I data delay for Tx channel */
    uint8_t txQDataDelay;                             /*!< Q data delay for Tx channel */
    bool txClkInverted;                               /*!< Whether to invert the Tx SSI clock polarity */
} adi_fpga910x_SsiCalibrationCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_SSI_TYPES_H_ */
