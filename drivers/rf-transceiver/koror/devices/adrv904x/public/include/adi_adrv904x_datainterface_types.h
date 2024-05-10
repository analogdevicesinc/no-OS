 /**
 * Copyright 2015 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_datainterface_types.h
 * \brief Contains ADRV904X API data types related to the data interface
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_DATAINTERFACE_TYPES_H_
#define _ADI_ADRV904X_DATAINTERFACE_TYPES_H_

#include "adi_common_error.h"

/**
 * \brief Data structure defining the locations where the Rx Capture RAM can be used.
 */
typedef enum adi_adrv904x_RxOrxDataCaptureLocation
{
    ADI_ADRV904X_CAPTURE_LOC_DDC0          = 0,  /*!< DDC0 output          */
    ADI_ADRV904X_CAPTURE_LOC_DDC1          = 1,  /*!< DDC1 output          */
    ADI_ADRV904X_CAPTURE_LOC_DPD           = 2,  /*!< pre and post DPD     */
    ADI_ADRV904X_CAPTURE_LOC_DPD_PRE       = 3,  /*!< DPD input            */
    ADI_ADRV904X_CAPTURE_LOC_DPD_POST      = 4,  /*!< DPD output           */
    ADI_ADRV904X_CAPTURE_LOC_FSC           = 10, /*!< FSC                  */
    ADI_ADRV904X_CAPTURE_LOC_DATAPATH      = 11, /*!< ORx Formatter input  */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX0       = 13, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX1       = 14, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX2       = 15, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX3       = 16, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX4       = 17, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX5       = 18, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX6       = 19, /*!< Synchronized DPD/ORX */
    ADI_ADRV904X_CAPTURE_LOC_ORX_TX7       = 20, /*!< Synchronized DPD/ORX */
} adi_adrv904x_RxOrxDataCaptureLocation_e;

#define ADI_ADRV904X_MAX_DESERIALIZER_LANES                 8U
#define ADI_ADRV904X_MAX_SERIALIZER_LANES                   8U
#define ADI_ADRV904X_FRAMER_CONVERTERS                      24U
#define ADI_ADRV904X_NUM_DEFRAMERS_SAMPLE_XBAR              8U
#define ADI_ADRV904X_NUM_FRAMERS_SAMPLE_XBAR                64U
#define ADI_ADRV904X_NUM_LKSH_SAMPLE_XBAR                   32U
#define ADI_ADRV904X_MAX_CONVERTER                          64U
#define ADI_ADRV904X_MAX_CDDC_CONVERTER                     32U
#define ADI_ADRV904X_MAX_FRAMERS                            3U
#define ADI_ADRV904X_MAX_FRAMERS_LS                         2U
#define ADI_ADRV904X_MAX_DEFRAMERS                          2U


#define ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR                    32U

/**
*  \brief Enum to select desired framer
*/
typedef enum adi_adrv904x_FramerSel
{
    ADI_ADRV904X_FRAMER_0 = 0x01U, /*!< Framer 0 selected */
    ADI_ADRV904X_FRAMER_1 = 0x02U, /*!< Framer 1 selected */
    ADI_ADRV904X_FRAMER_2 = 0x04U, /*!< Framer 2 selected */
    ADI_ADRV904X_ALL_FRAMERS = (ADI_ADRV904X_FRAMER_0 | ADI_ADRV904X_FRAMER_1 | ADI_ADRV904X_FRAMER_2)       /*!< All Framers  selected */

} adi_adrv904x_FramerSel_e;

/**
*  \brief Enum to select desired deframer
*/
typedef enum adi_adrv904x_DeframerSel
{
    ADI_ADRV904X_DEFRAMER_0 = 0x01U, /*!< Deframer 0 selection */
    ADI_ADRV904X_DEFRAMER_1 = 0x02U, /*!< Deframer 1 selection */
    ADI_ADRV904X_ALL_DEFRAMER = (ADI_ADRV904X_DEFRAMER_0 | ADI_ADRV904X_DEFRAMER_1)   /*!< Used for cases where Tx1 uses one deframer, Tx2 uses the second deframer */
} adi_adrv904x_DeframerSel_e;

/**
* \brief Enumerated list of ADC Sample Xbar options
*/
typedef enum adi_adrv904x_AdcSampleXbarSel
{
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I = 0u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q = 1u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_I = 2u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_Q = 3u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I = 4u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q = 5u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_I = 6u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_Q = 7u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I = 8u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q = 9u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_I = 10u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_Q = 11u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I = 12u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q = 13u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_I = 14u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_Q = 15u,

    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I = 16u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q = 17u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_I = 18u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_Q = 19u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I = 20u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q = 21u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_I = 22u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_Q = 23u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I = 24u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q = 25u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_I = 26u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_Q = 27u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I = 28u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q = 29u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_I = 30u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q = 31u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_LAST_VALID_NO_INT = ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q,
    /* With no interleaving enabled */

    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_0 = 32u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_1 = 33u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_2 = 34u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_3 = 35u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_4 = 36u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_5 = 37u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_6 = 38u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_7 = 39u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_0 = 40u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_1 = 41u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_2 = 42u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_3 = 43u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_4 = 44u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_5 = 45u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_6 = 46u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_7 = 47u,

    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_0 = 48u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_1 = 49u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_2 = 50u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_3 = 51u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_4 = 52u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_5 = 53u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_6 = 54u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_7 = 55u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_0 = 56u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_1 = 57u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_2 = 58u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_3 = 59u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_4 = 60u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_5 = 61u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_6 = 62u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7 = 63u,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_LAST_VALID = ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7,
    ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID = 0x7Fu
} adi_adrv904x_AdcSampleXbarSel_e;

/**
* \brief Data structure to hold the DAC sample crossbar information
*/
typedef enum adi_adrv904x_DacSampleXbarSel
{
    ADI_ADRV904X_DEFRAMER_OUT0 = 0u,
    ADI_ADRV904X_DEFRAMER_OUT1 = 1u,
    ADI_ADRV904X_DEFRAMER_OUT2 = 2u,
    ADI_ADRV904X_DEFRAMER_OUT3 = 3u,
    ADI_ADRV904X_DEFRAMER_OUT4 = 4u,
    ADI_ADRV904X_DEFRAMER_OUT5 = 5u,
    ADI_ADRV904X_DEFRAMER_OUT6 = 6u,
    ADI_ADRV904X_DEFRAMER_OUT7 = 7u,
    ADI_ADRV904X_DEFRAMER_OUT8 = 8u,
    ADI_ADRV904X_DEFRAMER_OUT9 = 9u,
    ADI_ADRV904X_DEFRAMER_OUT10 = 10u,
    ADI_ADRV904X_DEFRAMER_OUT11 = 11u,
    ADI_ADRV904X_DEFRAMER_OUT12 = 12u,
    ADI_ADRV904X_DEFRAMER_OUT13 = 13u,
    ADI_ADRV904X_DEFRAMER_OUT14 = 14u,
    ADI_ADRV904X_DEFRAMER_OUT15 = 15u,
    ADI_ADRV904X_DEFRAMER_OUT16       = 16u,
    ADI_ADRV904X_DEFRAMER_OUT17       = 17u,
    ADI_ADRV904X_DEFRAMER_OUT18       = 18u,
    ADI_ADRV904X_DEFRAMER_OUT19       = 19u,
    ADI_ADRV904X_DEFRAMER_OUT20       = 20u,
    ADI_ADRV904X_DEFRAMER_OUT21       = 21u,
    ADI_ADRV904X_DEFRAMER_OUT22       = 22u,
    ADI_ADRV904X_DEFRAMER_OUT23       = 23u,
    ADI_ADRV904X_DEFRAMER_OUT24       = 24u,
    ADI_ADRV904X_DEFRAMER_OUT25       = 25u,
    ADI_ADRV904X_DEFRAMER_OUT26       = 26u,
    ADI_ADRV904X_DEFRAMER_OUT27       = 27u,
    ADI_ADRV904X_DEFRAMER_OUT28       = 28u,
    ADI_ADRV904X_DEFRAMER_OUT29       = 29u,
    ADI_ADRV904X_DEFRAMER_OUT30       = 30u,
    ADI_ADRV904X_DEFRAMER_OUT31       = 31u,

    ADI_ADRV904X_DEFRAMER_LAST_VALID  = 31u,

    ADI_ADRV904X_DEFRAMER_OUT_INVALID           = 0x7Fu
} adi_adrv904x_DacSampleXbarSel_e;

/**
* \brief Enum to select desired Serializer Lane Output Driver Swing
*/
typedef enum adi_adrv904x_SerLaneCfgOutputDriveSwing
{
    ADRV904X_DRIVE_SWING_VTT_100 = 0u, /*!< Output swing level: 1.00 * VTT */
    ADRV904X_DRIVE_SWING_VTT_85  = 1u, /*!< Output swing level: 0.85 * VTT */
    ADRV904X_DRIVE_SWING_VTT_75  = 2u, /*!< Output swing level: 0.75 * VTT */
    ADRV904X_DRIVE_SWING_VTT_50  = 3u  /*!< Output swing level: 0.50 * VTT */
} adi_adrv904x_SerLaneCfgOutputDriveSwing_e;

/**
* \brief Enum to select desired Serializer Lane Pre-Emphasis
*/
typedef enum adi_adrv904x_SerLaneCfgPreEmphasis
{
    ADRV904X_PRE_EMPHASIS_LEVEL_0_DB = 0u, /*!< Pre-emphasis level: 0dB */
    ADRV904X_PRE_EMPHASIS_LEVEL_3_DB = 1u, /*!< Pre-emphasis level: 3dB */
    ADRV904X_PRE_EMPHASIS_LEVEL_6_DB = 2u  /*!< Pre-emphasis level: 6dB */
} adi_adrv904x_SerLaneCfgPreEmphasis_e;

/**
* \brief Enum to select desired Serializer Lane Post-Emphasis
*/
typedef enum adi_adrv904x_SerLaneCfgPostEmphasis
{
    ADRV904X_POST_EMPHASIS_LEVEL_0_DB  = 0u, /*!< Post-emphasis level: 0dB */
    ADRV904X_POST_EMPHASIS_LEVEL_3_DB  = 1u, /*!< Post-emphasis level: 3dB */
    ADRV904X_POST_EMPHASIS_LEVEL_6_DB  = 2u, /*!< Post-emphasis level: 6dB */
    ADRV904X_POST_EMPHASIS_LEVEL_9_DB  = 3u, /*!< Post-emphasis level: 9dB */
    ADRV904X_POST_EMPHASIS_LEVEL_12_DB = 4u  /*!< Post-emphasis level: 12dB */
} adi_adrv904x_SerLaneCfgPostEmphasis_e;

/**
* \brief Data structure to hold the Serializer Lane settings
*/
typedef struct adi_adrv904x_SerLaneCfg
{
    adi_adrv904x_SerLaneCfgOutputDriveSwing_e outputDriveSwing; /*!< Serializer Lane output swing level */
    adi_adrv904x_SerLaneCfgPreEmphasis_e      preEmphasis;      /*!< Serializer Lane pre-emphasis */
    adi_adrv904x_SerLaneCfgPostEmphasis_e     postEmphasis;     /*!< Serializer Lane post-emphasis */
} adi_adrv904x_SerLaneCfg_t;

/**
* \brief Data structure to hold the Serializer Reset Settings
*/
typedef struct adi_adrv904x_CpuCmd_SerReset
{
    uint8_t  presetClockOffsets;      /*!< Preset the clock offsets and fifo start based on 204c/204b mode */
    uint32_t serResetParm;            /*!< Serializer reset parameter */
} adi_adrv904x_CpuCmd_SerReset_t;

/**
* \brief Data structure to hold the Serializer Reset Results
*/
typedef struct adi_adrv904x_CpuCmd_SerResetResp
{
    uint32_t serResetResults;            /*!< Ser Reset results CPU */
} adi_adrv904x_CpuCmd_SerResetResp_t;

/**
* \brief Data structure to hold the DAC sample crossbar settings
*/
typedef struct adi_adrv904x_DacSampleXbarCfg
{
    adi_adrv904x_DacSampleXbarSel_e txDacChan[ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR]; /*!< Sample Crossbar select for Tx I/Q channel data*/
} adi_adrv904x_DacSampleXbarCfg_t;

/**
* \brief Data structure to hold the ADC sample crossbar information
*/
typedef struct adi_adrv904x_AdcSampleXbarCfg
{
    adi_adrv904x_AdcSampleXbarSel_e AdcSampleXbar[ADI_ADRV904X_NUM_FRAMERS_SAMPLE_XBAR];
} adi_adrv904x_AdcSampleXbarCfg_t;

#define ADI_ADRV904X_NUM_ORX_ADC_SAMPLE_XBAR                32u
/**
 * \brief Enumerated list of ORX ADC Sample Xbar options
 */
typedef enum adi_adrv904x_OrxAdcSampleXbarSel
{
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_0 = 0u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_1 = 1u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_2 = 2u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_3 = 3u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_4 = 4u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_5 = 5u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_6 = 6u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_7 = 7u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_0 = 8u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_1 = 9u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_2 = 10u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_3 = 11u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_4 = 12u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_5 = 13u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_6 = 14u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_7 = 15u,

    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_0 = 16u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_1 = 17u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_2 = 18u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_3 = 19u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_4 = 20u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_5 = 21u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_6 = 22u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_7 = 23u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_0 = 24u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_1 = 25u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_2 = 26u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_3 = 27u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_4 = 28u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_5 = 29u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_6 = 30u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_7 = 31u,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_LAST_VALID    = ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_7,
    ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_INVALID       = 0x7Fu
} adi_adrv904x_OrxAdcSampleXbarSel_e;

/**
 * \brief Data structure to hold the ADC sample crossbar information
 */
typedef struct adi_adrv904x_OrxAdcSampleXbarCfg
{
    adi_adrv904x_OrxAdcSampleXbarSel_e OrxAdcSampleXbar[ADI_ADRV904X_NUM_ORX_ADC_SAMPLE_XBAR];
} adi_adrv904x_OrxAdcSampleXbarCfg_t;

#define ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX         192U /*!< Index for maximum FRM carrier Xbar  */
#define ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX        192U /*!< Index for maximum DFRM carrier Xbar */
/**
 * \brief structure for framer CDDC Sample crossbar
 */
typedef struct adi_adrv904x_CddcSampleXbarCfg
{
    uint8_t cddcSampleXbar[ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX]; /*!< CDDC Sample XBar */
} adi_adrv904x_CddcSampleXbarCfg_t;

/**
 * \brief structure for framer CDUC Sample crossbar
 */
typedef struct adi_adrv904x_CducSampleXbarCfg
{
    uint8_t cducSampleXbar[ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX]; /*!< CDUC Sample XBar */
} adi_adrv904x_CducSampleXbarCfg_t;

/**
* \brief Data structure to hold the Framer output lane crossbar information
*
* Note that not all framers have 8 outputs.  A physical lane can only be used
* on one framer at a time.
*/
typedef struct adi_adrv904x_SerLaneXbar
{
    uint8_t laneFramerOutSel[ADI_ADRV904X_MAX_SERIALIZER_LANES]; /*!< Framer output to route to Physical Lane (valid 0-7) */
} adi_adrv904x_SerLaneXbar_t;

/**
* \brief Data structure to hold the Deframer input lane crossbar information
*
* Note that not all deframers have 8 inputs.
*/
typedef struct adi_adrv904x_DeserLaneXbar
{
    uint8_t deframerInputLaneSel[ADI_ADRV904X_MAX_DESERIALIZER_LANES]; /*!< Physical lane select for deframer input (valid 0-7) */
} adi_adrv904x_DeserLaneXbar_t;

/**
*  \brief Data structure to hold ADRV904X JESD204B/C Framer configuration settings
*/
typedef struct adi_adrv904x_FramerCfg
{
    uint8_t enableJesd204C;         /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t bankId;                 /*!< JESD204B Configuration Bank ID extension to Device ID. Range is 0..15 */
    uint8_t deviceId;               /*!< JESD204B Configuration Device ID link identification number. Range is 0..255 */
    uint8_t lane0Id;                /*!< JESD204B Configuration starting Lane ID. If more than one lane is used, each lane will increment from the Lane0 ID. Range is 0..31 */
    uint8_t jesd204M;               /*!< Number of ADCs (0, 2, or 4) where 2 ADCs are required per receive chain (I and Q). */
    uint16_t jesd204K;              /*!< Number of frames in a multiframe. Default = 32, F*K must be modulo 4. Where, F=2*M/numberOfLanes  (Max 32 for JESD204B, Max 256 for JESD204C). */
    uint8_t jesd204F;               /*!< Number of bytes(octets) per frame (Valid 1, 2, 4, 8). */
    uint8_t jesd204Np;              /*!< converter sample resolution (12, 16, 24). */
    uint8_t jesd204E;               /*!< JESD204C E parameter. This is E -1 value. */
    uint8_t scramble;               /*!< Scrambling off if framerScramble = 0, if framerScramble > 0 scrambling is enabled */
    uint8_t serializerLanesEnabled; /*!< Serializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    uint16_t lmfcOffset;            /*!< LMFC offset value for deterministic latency setting. */
    uint8_t syncbInSelect;          /*!< Selects SYNCb input source. Where, 0 = use SYNCBIN0 for this framer, 1 = use SYNCBIN1 for this framer, 2 = use SYNCBIN2 */
    uint8_t overSample;             /*!< Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples) */
    uint8_t syncbInLvdsMode;        /*!< 1 - enable LVDS input pad with 100ohm internal termination, 0 - enable CMOS input pad */
    uint8_t syncbInLvdsPnInvert;    /*!< 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */
    adi_adrv904x_SerLaneXbar_t serializerLanePdCrossbar; /*!< Lane crossbar to map framer lane outputs to physical lanes */
    adi_adrv904x_AdcSampleXbarCfg_t adcCrossbar;         /*!< ADC converter to framer input mapping */
    adi_adrv904x_OrxAdcSampleXbarCfg_t orxAdcCrossbar; /*!< For ADRV904X: this contains ORX sample xbar for framer-2 */
    uint8_t newSysrefOnRelink;      /*!< Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set */
    uint8_t sysrefForStartup;       /*!< 1 = Framer: Require a SYSREF before CGS will be output from serializer, 0: Allow CGS to output before SYSREF occurs (recommended on framer to allow deframer CDR to lock and EQ to train)*/
    uint8_t sysrefNShotEnable;      /*!< 1 = Enable SYSREF NShot (ability to ignore first rising edge of SYSREF to ignore possible runt pulses.) */
    uint8_t sysrefNShotCount;       /*!< Count value of which SYSREF edge to use to reset LMFC phase, valid range is 0 to 15 */
    uint8_t sysrefIgnoreWhenLinked; /*!< When JESD204 link is up and valid, 1= ignore any sysref pulses */
    uint32_t  iqRate_kHz;           /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz;         /*!< Framer Lane rate */
    adi_adrv904x_CddcSampleXbarCfg_t cddcCrossbar; /*!< For ADRV904X: this contains CDDC sample xbar */
} adi_adrv904x_FramerCfg_t;

/**
* \brief Data structure to hold the settings for the deframer configuration
*/
typedef struct adi_adrv904x_DeframerCfg
{
    uint8_t enableJesd204C;           /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t bankId;                   /*!< Extension to Device ID. Range is 0..15 , bankId is not supported for Jrx, returns always 0*/
    uint8_t deviceId;                 /*!< Link identification number. Range is 0..255 */
    uint8_t laneId[ADI_ADRV904X_MAX_DESERIALIZER_LANES];
    uint8_t jesd204M;                 /*!< Number of DACs (0, 2, or 4) - 2 DACs per transmit chain (I and Q) */
    uint16_t jesd204K;                /*!< Number of frames in a multiframe. Default = 32, F*K = modulo 4. Where, F=2*M/numberOfLanes  (Max 32 for JESD204B, Max 256 for JESD204C) */
    uint8_t jesd204F;                 /*!< Number of bytes(octets) per frame . */
    uint8_t jesd204Np;                /*!< converter sample resolution (12, 16) */
    uint8_t jesd204E;                 /*!< JESD204C E parameter. This is E -1 value. */
    uint8_t decrambling;              /*!< decrambling off if decramble = 0, if decramble > 0 decrambling is enabled */
    uint8_t deserializerLanesEnabled; /*!< Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    uint16_t lmfcOffset;              /*!< LMFC offset value to adjust deterministic latency. */
    uint8_t syncbOutSelect;           /*!< Selects deframer SYNCBOUT pin (0 = SYNCBOUT0, 1 = SYNCBOUT1, 2 = output SYNCB to SYNCBOUT0 and SYNCBOUT1, 3 = No Pin Selected) */
    uint8_t syncbOutLvdsMode;         /*!< Ignored if syncbOutSelect = 3. Otherwise 1 - enable LVDS output pad, 0 - enable CMOS output pad */
    uint8_t syncbOutLvdsPnInvert    ; /*!< Ignored if syncbOutSelect = 3. Otherwise 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */
    uint8_t syncbOut0CmosDriveStrength; /*!< CMOS output drive strength. Max = 15 */
    uint8_t syncbOut1CmosDriveStrength; /*!< CMOS output drive strength. Max = 15 */
    adi_adrv904x_DeserLaneXbar_t deserializerLaneCrossbar; /*!< Lane crossbar to map physical lanes to deframer inputs */
    adi_adrv904x_DacSampleXbarCfg_t dacCrossbar;           /*!< Deframer output to DAC mapping */
    uint8_t newSysrefOnRelink;      /*!< Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set */
    uint8_t sysrefForStartup;       /*!< Suggested to enable for deframer so deframer will not assert SYNCB to init the link until SYSREF has occurred, which resets LMFC phase */
    uint8_t sysrefNShotEnable;      /*!< 1 = Enable SYSREF NShot (ability to ignore first rising edge of SYSREF to ignore possible runt pulses.) */
    uint8_t sysrefNShotCount;       /*!< Count value of which SYSREF edge to use to reset LMFC phase, valid range is 0 to 15 */
    uint8_t sysrefIgnoreWhenLinked; /*!< When JESD204 link is up and valid, 1= ignore any sysref pulses */
    uint32_t  iqRate_kHz;           /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz;         /*!< Framer Lane rate */
    adi_adrv904x_CducSampleXbarCfg_t cducCrossbar;         /*!< CDUC Xbar output */
} adi_adrv904x_DeframerCfg_t;

/**
* \brief Enum for JESD204B/C PRBS generated types
*/
typedef enum adi_adrv904x_DeframerPrbsOrder
{
    ADI_ADRV904X_PRBS_DISABLE = 0, /*!< Deframer PRBS pattern disable */
    ADI_ADRV904X_PRBS7,            /*!< Deframer PRBS7 pattern select */
    ADI_ADRV904X_PRBS9,            /*!< Deframer PRBS9 pattern select */
    ADI_ADRV904X_PRBS15,           /*!< Deframer PRBS15 pattern select */
    ADI_ADRV904X_PRBS31,           /*!< Deframer PRBS31 pattern select */
    ADI_ADRV904X_USERDATA          /*!< Deframer user supplied pattern select */
} adi_adrv904x_DeframerPrbsOrder_e;

/**
* \brief Enum for JESD204B deserializer / deframer PRBS selection
*/
typedef enum adi_adrv904x_DeframerPrbsCheckLoc
{
    ADI_ADRV904X_PRBSCHECK_LANEDATA  = 0U,   /*!< Check PRBS at deserializer lane output (does not require JESD204b link) */
    ADI_ADRV904X_PRBSCHECK_SAMPLEDATA       /*!< Check PRBS at output of deframer (JESD204b deframed sample) */
} adi_adrv904x_DeframerPrbsCheckLoc_e;

/**
*  \brief Data structure to hold ADRV904X JESD204b Deframer PRBS test configuration
*/
typedef struct adi_adrv904x_DfrmPrbsCfg
{
    adi_adrv904x_DeframerPrbsOrder_e polyOrder;           /*!< Deframer PRBS Order  */
    adi_adrv904x_DeframerPrbsCheckLoc_e checkerLocation;  /*!< Deframer PRBS Checker Location  */
} adi_adrv904x_DfrmPrbsCfg_t;

/**
* \brief Enum of possible Framer Test Data sources
*/
typedef enum adi_adrv904x_FramerDataSource
{
    ADI_ADRV904X_FTD_ADC_DATA = 0,        /*!< Framer test data ADC data source */
    ADI_ADRV904X_FTD_CHECKERBOARD,        /*!< Framer test data checkerboard data source */
    ADI_ADRV904X_FTD_TOGGLE0_1,           /*!< Framer test data toggle 0 to 1 data source */
    ADI_ADRV904X_FTD_PRBS31,              /*!< Framer test data PRBS31 data source */
    ADI_ADRV904X_FTD_PRBS23,              /*!< Framer test data PRBS23 data source */
    ADI_ADRV904X_FTD_PRBS15,              /*!< Framer test data PRBS15 data source */
    ADI_ADRV904X_FTD_PRBS9,               /*!< Framer test data PRBS9 data source */
    ADI_ADRV904X_FTD_PRBS7,               /*!< Framer test data PRBS7 data source */
    ADI_ADRV904X_FTD_RAMP,                /*!< Framer test data ramp data source */
    ADI_ADRV904X_FTD_PATTERN_REPEAT = 14, /*!< Framer test data 16-bit programmed pattern repeat source */
    ADI_ADRV904X_FTD_PATTERN_ONCE = 15    /*!< Framer test data 16-bit programmed pattern executed once source */
} adi_adrv904x_FramerDataSource_e;

/**
*  \brief Enum of Framer test data injection points
*/
typedef enum adi_adrv904x_FramerDataInjectPoint
{
    ADI_ADRV904X_FTD_FRAMERINPUT = 0, /*!< Framer test data injection point at framer input */
    ADI_ADRV904X_FTD_SERIALIZER,      /*!< Framer test data injection point at serializer input */
    ADI_ADRV904X_FTD_POST_LANEMAP     /*!< Framer test data injection point after lane mapping */
} adi_adrv904x_FramerDataInjectPoint_e;

/**
*  \brief Enum of Framer test data configuration
*/
typedef struct adi_adrv904x_FrmTestDataCfg
{
    uint8_t framerSelMask;                            /*!< Framer selected mask */
    adi_adrv904x_FramerDataSource_e testDataSource;   /*!< Framer test data source */
    adi_adrv904x_FramerDataInjectPoint_e injectPoint; /*!< Framer test data injection point */
} adi_adrv904x_FrmTestDataCfg_t;

/**
*  \brief Data structure to hold ADRV904X JESD204B/C Deframer PRBS Error Counter values on readback
*/
typedef struct adi_adrv904x_DfrmPrbsErrCounters
{
    adi_adrv904x_DeframerPrbsCheckLoc_e sampleSource;       /*!< Sample or Lane mode. Mode error count and status use zeroth index  for sample mode */
    uint8_t sampleErrors;                                   /*!<  0b: sample error count, 1b: invalid error flag, 2b: error flag */
    uint8_t errorStatus[ADI_ADRV904X_MAX_SERIALIZER_LANES]; /*!<  Array contains error status */
    uint32_t laneErrors[ADI_ADRV904X_MAX_SERIALIZER_LANES]; /*!< Lane 0 contains error counters if in sample mode, otherwise errors are maintained per lane */
} adi_adrv904x_DfrmPrbsErrCounters_t;

/**
* \brief Enum to select data capture length for Rx/Orx
*/
typedef enum adi_adrv904x_RxOrxDataCaptureLength
{
    ADI_ADRV904X_CAPTURE_SIZE_12K = 0x3000,
    ADI_ADRV904X_CAPTURE_SIZE_16K = 0x4000,
	ADI_ADRV904X_CAPTURE_SIZE_32K = 0x8000,
	ADI_ADRV904X_CAPTURE_SIZE_8K = 0x2000,
	ADI_ADRV904X_CAPTURE_SIZE_4K = 0x1000,
	ADI_ADRV904X_CAPTURE_SIZE_2K = 0x800,
	ADI_ADRV904X_CAPTURE_SIZE_1K = 0x400,
	ADI_ADRV904X_CAPTURE_SIZE_512 = 0x200,
	ADI_ADRV904X_CAPTURE_SIZE_256 = 0x100,
	ADI_ADRV904X_CAPTURE_SIZE_128 = 0x80,
	ADI_ADRV904X_CAPTURE_SIZE_64 = 0x40,
	ADI_ADRV904X_CAPTURE_SIZE_32 = 0x20

} adi_adrv904x_RxOrxDataCaptureLength_e;

/**
 * \brief Enum define ADRV904X JESD204B Deframer error counter reset option
 */
typedef enum adi_adrv904x_DfrmErrCounterReset
{
    ADI_ADRV904X_DFRM_BD_CLEAR = 0x01, /*!< BD Clear */
    ADI_ADRV904X_DFRM_INT_CLEAR = 0x2, /*!< Int Clear */
    ADI_ADRV904X_DFRM_UEK_CLEAR = 0x4  /*!< UEK Clear */
} adi_adrv904x_DfrmErrCounterReset_e;

/**
* \brief Enum to select GPINT enable/disable for deframer error counters
*/
typedef enum adi_adrv904x_DfrmErrCounterIrqSel
{
    ADI_ADRV904X_DFRM_ERR_COUNT_DISABLE_IRQ = 0,    /*!< Do not create GPINT when error counter overflows 255 */
    ADI_ADRV904X_DFRM_ERR_COUNT_ENABLE_IRQ          /*!< Create GPINT when error counter overflows 255 */
} adi_adrv904x_DfrmErrCounterIrqSel_e;

/**
* \brief Enum to select Serdes Error to be Cleared, Enabled or Disable
*/
typedef enum adi_adrv904x_SerdesErrAction
{
    ADI_ADRV904X_SERDES_PCLK_ERR_DISABLE   = 0x01U, /*!< Disable PCLK related errors */
    ADI_ADRV904X_SERDES_SYSREF_ERR_DISABLE = 0x02U, /*!< Disable SYSREF Phase error */
    ADI_ADRV904X_SERDES_ALL_ERR_DISABLE    = 0x03U, /*!< Disable both PCLK and SYSREF errors */
    ADI_ADRV904X_SERDES_PCLK_ERR_ENABLE    = 0x04U, /*!< Enable PCLK related errors */
    ADI_ADRV904X_SERDES_SYSREF_ERR_ENABLE  = 0x08U, /*!< Enable SYSREF Phase error */
    ADI_ADRV904X_SERDES_ALL_ERR_ENABLE     = 0x0CU, /*!< Enable both PCLK and SYSREF errors */
    ADI_ADRV904X_SERDES_PCLK_ERR_CLEAR     = 0x05U, /*!< Clear PCLK related errors */
    ADI_ADRV904X_SERDES_SYSREF_ERR_CLEAR   = 0x0AU, /*!< Clear SYSREF Phase error */
    ADI_ADRV904X_SERDES_ALL_ERR_CLEAR      = 0x0FU, /*!< Clear both PCLK and SYSREF errors */
} adi_adrv904x_SerdesErrAction_e;

/**
*  \brief Data structure to hold ADRV904X JESD204B/C Framer status
*/
typedef struct adi_adrv904x_FramerStatus
{
    uint8_t  status;              /*!< Framer status */
    uint8_t  framerSyncNeCount;   /*!< 204B: Count of SYNCB falling edges */
    uint8_t  qbfStateStatus;      /*!< 204B: QBF status */
    uint8_t  syncNSel;            /*!< 204B: Syncb crossbar select */
} adi_adrv904x_FramerStatus_t;

/**
*  \brief Data structure to hold ADRV904X JESD204B/C Deframer status information
*/
typedef struct adi_adrv904x_DeframerStatus
{
    uint8_t  status;     /*!<  Deframer status for both 204B/C */
    uint8_t  reserved;   /*!<  Contain additional status information */
} adi_adrv904x_DeframerStatus_t;

/**
*  \brief Data structure to hold ADRV904X JESD204B/C Deframer status information which include physical lanes.
*/
typedef struct adi_adrv904x_DeframerStatus_v2
{
    uint8_t  phyLaneMask;    /*!< Physical lane bit mask */
    uint8_t  laneStatus[8];  /*!< Deframer status for both 204B/C for physical lanes */
    uint8_t  linkState;      /*!< b0:204B/c, b1:link up/down */
} adi_adrv904x_DeframerStatus_v2_t;

/**
*  \brief Data structure to hold ADRV904X JESD204b Deframer Source register bits.
*/
typedef struct adi_adrv904x_DeframerIrqVector
{
    uint16_t lane[ADI_ADRV904X_MAX_DESERIALIZER_LANES];     /*!< IRQ vector data per physical lane */
    uint16_t deframer0;                                     /*!< Bitwise-OR of vectors for all lanes in use by Deframer0 */
    uint16_t deframer1;                                     /*!< Bitwise-OR of vectors for all lanes in use by Deframer1 */
} adi_adrv904x_DeframerIrqVector_t;

/*
 *  \brief Data structure to hold ADRV904X JESD204B Deframer error counter status
 */
typedef struct adi_adrv904x_DfrmErrCounterStatus
{
    uint8_t  laneStatus;    /*!< deframer status bits  */
    uint8_t  bdCntValue;    /*!< Bad-Disparity error counter current value */
    uint8_t  uekCntValue;   /*!< UnExpected-K error counter current value */
    uint8_t  nitCntValue;   /*!< Not-In-Table error counter current value */
} adi_adrv904x_DfrmErrCounterStatus_t;

/*
 *  \brief Data structure to hold ADRV904X JESD204C Deframer error counter status
 */
typedef struct adi_adrv904x_Dfrm204cErrCounterStatus
{
    uint8_t  shCntValue;         /*!< Count of block alignment errors */
    uint8_t  embCntValue;        /*!< Count of extended multiblock alignment errors */
    uint8_t  mbCntValue;         /*!< Count of multiblock alignment errors */
    uint8_t  crcCntValue;        /*!< Count of CRC parity errors */
} adi_adrv904x_Dfrm204cErrCounterStatus_t;

/**
*  \brief Data structure to hold ADRV904X JESD204b DeFramer ILAS configuration settings
*/
typedef struct adi_adrv904x_DfrmLane0Cfg
{
    uint8_t dfrmDID;   /*!< JESD204B Configuration Device ID for ILAS check */
    uint8_t dfrmBID;   /*!< JESD204B Configuration Bank ID for ILAS check */
    uint8_t dfrmLID0;  /*!< JESD204B Configuration starting Lane ID for ILAS check */
    uint8_t dfrmL;     /*!< JESD204B Configuration L = lanes per data converter for ILAS check */
    uint8_t dfrmSCR;   /*!< JESD204B Configuration scramble setting for ILAS check */
    uint8_t dfrmF;     /*!< JESD204B Configuration F = octets per frame for ILAS check */
    uint8_t dfrmK;     /*!< JESD204B Configuration K = frames per multiframe for ILAS check */
    uint8_t dfrmM;     /*!< JESD204B Configuration M = number of data converters for ILAS check */
    uint8_t dfrmN;     /*!< JESD204B Configuration N = data converter sample resolution for ILAS check */
    uint8_t dfrmCS;    /*!< JESD204B Configuration CS = number of control bits transferred per sample per frame for ILAS check */
    uint8_t dfrmNP;    /*!< JESD204B Configuration NP = JESD204B word size based on the highest resolution of the data converter for ILAS check */
    uint8_t dfrmS;     /*!< JESD204B Configuration S = number of samples/data converter/frame for ILAS check */
    uint8_t dfrmCF;    /*!< JESD204B Configuration CF = '0' = control bits appended to each sample, '1' = appended to end of frame for ILAS check */
    uint8_t dfrmHD;    /*!< JESD204B Configuration HD = high density bit - samples are contained within lane (0) or divided over more than one lane (1) for ILAS check */
    uint8_t dfrmFCHK0; /*!< JESD204B Configuration checksum for ILAS check lane0 */
} adi_adrv904x_DfrmLane0Cfg_t;

/**
*  \brief Data structure to hold ADRV904X JESD204b DeFramer ILAS configuration settings for each deframer
*/
typedef struct adi_adrv904x_DfrmCompareData
{
    uint32_t zeroCheckFlag;    /*!< Link Valid Data Flag - if returned all zero then link is not active */
    uint32_t ilasMismatchDfrm; /*!< ILAS mismatch Flag - all set bits indicated mismatch and bit position indicates incorrect value */
    adi_adrv904x_DfrmLane0Cfg_t dfrmIlasData; /*!< Captured ILAS data  */
    adi_adrv904x_DfrmLane0Cfg_t dfrmCfgData;  /*!< Configured ILAS data  */
} adi_adrv904x_DfrmCompareData_t;

/**
*  \brief Data structure to hold ADRV904X JESD204b DeFramer ILAS configuration settings
*/
typedef struct adi_adrv904x_DfrmLaneCfg_v2
{
    uint8_t jesdV;      /*!< JESD204B Configuration JESD version */
    uint8_t subclassV ; /*!< JESD204B Configuration Subclass version */
    uint8_t dfrmDID;    /*!< JESD204B Configuration Device ID for ILAS check */
    uint8_t dfrmBID;    /*!< JESD204B Configuration Bank ID for ILAS check */
    uint8_t dfrmLID;    /*!< JESD204B Configuration Lane ID for ILAS check */
    uint8_t dfrmL;      /*!< JESD204B Configuration L = lanes per data converter for ILAS check */
    uint8_t dfrmSCR;    /*!< JESD204B Configuration scramble setting for ILAS check */
    uint8_t dfrmF;      /*!< JESD204B Configuration F = octets per frame for ILAS check */
    uint8_t dfrmK;      /*!< JESD204B Configuration K = frames per multiframe for ILAS check */
    uint8_t dfrmM;      /*!< JESD204B Configuration M = number of data converters for ILAS check */
    uint8_t dfrmN;      /*!< JESD204B Configuration N = data converter sample resolution for ILAS check */
    uint8_t dfrmCS;     /*!< JESD204B Configuration CS = number of control bits transferred per sample per frame for ILAS check */
    uint8_t dfrmNP;     /*!< JESD204B Configuration NP = JESD204B word size based on the highest resolution of the data converter for ILAS check */
    uint8_t dfrmS;      /*!< JESD204B Configuration S = number of samples/data converter/frame for ILAS check */
    uint8_t dfrmCF;     /*!< JESD204B Configuration CF = '0' = control bits appended to each sample, '1' = appended to end of frame for ILAS check */
    uint8_t dfrmHD;     /*!< JESD204B Configuration HD = high density bit - samples are contained within lane (0) or divided over more than one lane (1) for ILAS check */
    uint8_t dfrmFCHK;   /*!< JESD204B Configuration checksum for ILAS check */
} adi_adrv904x_DfrmLaneCfg_v2_t;

/**
*  \brief Enum of ILAS deframer lane mismatch bits
*/
typedef enum adi_adrv904x_IlasMismatch
{
    ADI_ADRV904X_ILAS_DID       = 0x00000001,
    ADI_ADRV904X_ILAS_BID       = 0x00000002,
    ADI_ADRV904X_ILAS_LID       = 0x00000004,
    ADI_ADRV904X_ILAS_L         = 0x00000008,
    ADI_ADRV904X_ILAS_SCR       = 0x00000010,
    ADI_ADRV904X_ILAS_F         = 0x00000020,
    ADI_ADRV904X_ILAS_K         = 0x00000040,
    ADI_ADRV904X_ILAS_M         = 0x00000080,
    ADI_ADRV904X_ILAS_N         = 0x00000100,
    ADI_ADRV904X_ILAS_CS        = 0x00000200,
    ADI_ADRV904X_ILAS_NP        = 0x00000400,
    ADI_ADRV904X_ILAS_S         = 0x00000800,
    ADI_ADRV904X_ILAS_CF        = 0x00001000,
    ADI_ADRV904X_ILAS_HD        = 0x00002000,
    ADI_ADRV904X_ILAS_CKSM      = 0x00004000,
    ADI_ADRV904X_ILAS_JESDV     = 0x00008000,
    ADI_ADRV904X_ILAS_SUBCLASSV = 0x00010000
} adi_adrv904x_IlasMismatch_e;

/**
*  \brief Data structure to hold ADRV904X JESD204b DeFramer ILAS configuration settings for each deframer with all lanes
*/
typedef struct adi_adrv904x_DfrmCompareData_v2
{
    uint8_t  phyLaneEnMask;       /*!< Physical enabled lane bit mask for the selected deframer */
    uint8_t  laneMismatchMask;    /*!< lane mismatch bit mask for the selected deframer */
    uint32_t zeroCheckFlag[ADI_ADRV904X_MAX_DESERIALIZER_LANES];    /*!< Link Valid Data Flag - see adi_adrv904x_IlasMismatch_e for bit mask. */
    uint32_t ilasMismatchDfrm[ADI_ADRV904X_MAX_DESERIALIZER_LANES]; /*!< ILAS mismatch Flag - all set bits indicated mismatch and bit position indicates incorrect value */
    uint8_t  cfgDataChksum[ADI_ADRV904X_MAX_DESERIALIZER_LANES];    /*!< Configuration data checksum used to compare with each lane checksum */
    adi_adrv904x_DfrmLaneCfg_v2_t dfrmIlasData[ADI_ADRV904X_MAX_DESERIALIZER_LANES]; /*!< Captured ILAS data for all lanes */
    adi_adrv904x_DfrmLaneCfg_v2_t dfrmCfgData;     /*!< Configured ILAS data for selected deframer */
} adi_adrv904x_DfrmCompareData_v2_t;

/*
* \brief enum to set ADC sample crossbar loopback
*/
typedef enum adi_adrv904x_AdcSampleLoopbackXbar
{
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX0_DATA_I = 0U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX0_DATA_Q = 1U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX1_DATA_I = 4U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX1_DATA_Q = 5U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX2_DATA_I = 8U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX2_DATA_Q = 9U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX3_DATA_I = 12U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX3_DATA_Q = 13U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX4_DATA_I = 16U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX4_DATA_Q = 17U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX5_DATA_I = 20U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX5_DATA_Q = 21U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX6_DATA_I = 24U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX6_DATA_Q = 25U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX7_DATA_I = 28U,
    ADI_ADRV904X_JESD_FRM_LB_XBAR_TX7_DATA_Q = 29U,
} adi_adrv904x_AdcSampleLoopbackXbar_e;

/**
*  \brief Data structure to hold ADRV904X Deserializer lanes Common Mode Configuration
*/
typedef struct adi_adrv904x_DeserLanesVcmCfg
{
    uint8_t vcmDriverMask;    /*!< Lane Mask to configure the AMUX bus to be driven by the VCM node in the selected lane */
    uint8_t vcmReceiverMask;  /*!< Lane Mask to specify which lanes use the AMUx bus to set the JRx input common mode */
} adi_adrv904x_DeserLanesVcmCfg_t;

#define ADI_ADRV904X_MAX_SAMPLE_XBAR_INDEX_VALUE       32U /*!< Maximum samples Xbar index value    */
#define ADI_ADRV904X_XBAR_ENTRY_DISABLED             0x80U /*!< Disabled Xbar entry                 */
#define ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED        1U /*!< 1 is low                            */
#define ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED         0U /*!< 0 is active high                    */
#define ADI_ADRV904X_MAX_CHANNEL_XBAR_VALUE             7U /*!< Xbar value for max channel          */

/**
 * \brief enum for framer CDDC Mode
 */
typedef enum adi_adrv904x_CddcMode
{
    ADI_ADRV904X_CDDC_MODE = 0U, /*!< CDDC mode */
    ADI_ADRV904X_CDDC_BYPASS_MODE = 1U    /*!< CDDC bypass mode */
} adi_adrv904x_CddcMode_e;

/**
 * \brief enum for deframer CDUC Mode
 */
typedef enum adi_adrv904x_CducMode
{
    ADI_ADRV904X_CDUC_MODE = 0U, /*!< CDUC mode */
    ADI_ADRV904X_CDUC_BYPASS_MODE = 1U   /*!< CDUC bypass mode */
} adi_adrv904x_CducMode_e;

/**
 * \brief structure for framer CDDC RX Channel crossbar
 */
typedef struct adi_adrv904x_CddcChannelXbarCfg
{
    uint8_t  cddcChannelXbar[ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX]; /*!< CDDC RX Channel XBar */
} adi_adrv904x_CddcChannelXbarCfg_t;

/**
 * \brief structure for deframer CDUC TX Channel crossbar
 */
typedef struct adi_adrv904x_CducChannelXbarCfg
{
    uint8_t  cducChannelXbar[ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX]; /*!< CDUC TX Channel XBar */
} adi_adrv904x_CducChannelXbarCfg_t;
#endif  /* _ADI_ADRV904X_DATAINTERFACE_TYPES_H_ */
