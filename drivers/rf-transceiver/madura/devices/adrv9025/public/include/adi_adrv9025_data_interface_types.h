/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_data_interface_types.h
* \brief Contains ADRV9025 API data interface types
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_DATA_INTERFACE_TYPES_H_
#define _ADI_ADRV9025_DATA_INTERFACE_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#define ADI_ADRV9025_NUM_LANES 4
#define ADI_ADRV9025_FRMR_NUM_OF_CONVERTERS 24
#define ADRV9025_QR_EYE_NUM_PHASES 36
/**
*  \brief Enum to select desired framer
*/
typedef enum adi_adrv9025_FramerSel
{
    ADI_ADRV9025_FRAMER_0 = 1,      /*!< Framer 0 selected */
    ADI_ADRV9025_FRAMER_1 = 2,      /*!< Framer 1 selected */
    ADI_ADRV9025_FRAMER_2 = 4,      /*!< Framer 2 selected */
    ADI_ADRV9025_ALL_FRAMERS = 7       /*!< All Framers  selected */

} adi_adrv9025_FramerSel_e;

/**
*  \brief Enum to select desired deframer
*/
typedef enum adi_adrv9025_DeframerSel
{
    ADI_ADRV9025_DEFRAMER_0 = 1,  /*!< Deframer A selection */
    ADI_ADRV9025_DEFRAMER_1 = 2,  /*!< Deframer B selection */
    ADI_ADRV9025_DEFRAMER_0_AND_1 = 3   /*!< Used for cases where Tx1 uses one deframer, Tx2 uses the second deframer */
} adi_adrv9025_DeframerSel_e;

/**
* \brief Enumerated list of DAC Sample Xbar options
*/
typedef enum adi_adrv9025_DacSampleXbarSel
{
    ADI_ADRV9025_DEFRAMER_OUT0 = 0x1,               /*!< Deframer out 0 */
    ADI_ADRV9025_DEFRAMER_OUT1 = 0x0,               /*!< Deframer out 1 */
    ADI_ADRV9025_DEFRAMER_OUT2 = 0x3,               /*!< Deframer out 2 */
    ADI_ADRV9025_DEFRAMER_OUT3 = 0x2,               /*!< Deframer out 3 */
    ADI_ADRV9025_DEFRAMER_OUT4 = 0x5,               /*!< Deframer out 4 */
    ADI_ADRV9025_DEFRAMER_OUT5 = 0x4,               /*!< Deframer out 5 */
    ADI_ADRV9025_DEFRAMER_OUT6 = 0x7,               /*!< Deframer out 6 */
    ADI_ADRV9025_DEFRAMER_OUT7 = 0x6,               /*!< Deframer out 7 */
    ADI_ADRV9025_DEFRAMER_OUT_DISABLE = 0x7F        /*!< Deframer out Disable Value */
} adi_adrv9025_DacSampleXbarSel_e;

/**
* \brief Enumerated list of ADC Sample Xbar options
*/
/* TODO: Verify enum values */
typedef enum adi_adrv9025_AdcSampleXbarSel
{
    ADI_ADRV9025_ADC_RX1_I = 0x1,                    /*!<  Rx1 I data */
    ADI_ADRV9025_ADC_RX1_Q = 0x0,                    /*!<  Rx1 Q data */
    ADI_ADRV9025_ADC_RX2_I = 0x3,                    /*!<  Rx2 I data */
    ADI_ADRV9025_ADC_RX2_Q = 0x2,                    /*!<  Rx2 Q data */
    ADI_ADRV9025_ADC_RX3_I = 0x5,                    /*!<  Rx3 I data */
    ADI_ADRV9025_ADC_RX3_Q = 0x4,                    /*!<  Rx3 Q data */
    ADI_ADRV9025_ADC_RX4_I = 0x7,                    /*!<  Rx4 I data */
    ADI_ADRV9025_ADC_RX4_Q = 0x6,                    /*!<  Rx4 Q data */

    ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_A_I = 0x1,    /*!<  Dualband Rx1 Band A I data */
    ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_A_Q = 0x0,    /*!<  Dualband Rx1 Band A Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_A_I = 0x3,    /*!<  Dualband Rx2 Band A I data */
    ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_A_Q = 0x2,    /*!<  Dualband Rx2 Band A Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_A_I = 0x5,    /*!<  Dualband Rx3 Band A I data */
    ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_A_Q = 0x4,    /*!<  Dualband Rx3 Band A Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_A_I = 0x7,    /*!<  Dualband Rx4 Band A I data */
    ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_A_Q = 0x6,    /*!<  Dualband Rx4 Band A Q data */

    ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_I = 0x9,    /*!<  Dualband Rx1 Band B I data */
    ADI_ADRV9025_ADC_DUALBAND_RX1_BAND_B_Q = 0x8,    /*!<  Dualband Rx1 Band B Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_I = 0xB,    /*!<  Dualband Rx2 Band B I data */
    ADI_ADRV9025_ADC_DUALBAND_RX2_BAND_B_Q = 0xA,    /*!<  Dualband Rx2 Band B Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_I = 0xD,    /*!<  Dualband Rx3 Band B I data */
    ADI_ADRV9025_ADC_DUALBAND_RX3_BAND_B_Q = 0xC,    /*!<  Dualband Rx3 Band B Q data */
    ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_I = 0xF,    /*!<  Dualband Rx4 Band B I data */
    ADI_ADRV9025_ADC_DUALBAND_RX4_BAND_B_Q = 0xE,    /*!<  Dualband Rx4 Band B Q data */

    ADI_ADRV9025_ADC_ORX1_I = 0x11,
    ADI_ADRV9025_ADC_ORX1_Q = 0x10,
    ADI_ADRV9025_ADC_ORX2_I = 0x13,
    ADI_ADRV9025_ADC_ORX2_Q = 0x12,

    ADI_ADRV9025_ADC_DISABLE = 0x7F

} adi_adrv9025_AdcSampleXbarSel_e;

/**
* \brief Enum of possible Framer Test Data sources
*/
typedef enum adi_adrv9025_FramerDataSource
{
    ADI_ADRV9025_FTD_ADC_DATA = 0,           /*!< Framer test data ADC data source */
    ADI_ADRV9025_FTD_CHECKERBOARD,           /*!< Framer test data checkerboard data source */
    ADI_ADRV9025_FTD_TOGGLE0_1,              /*!< Framer test data toggle 0 to 1 data source */
    ADI_ADRV9025_FTD_PRBS31,                 /*!< Framer test data PRBS31 data source */
    ADI_ADRV9025_FTD_PRBS23,                 /*!< Framer test data PRBS23 data source */
    ADI_ADRV9025_FTD_PRBS15,                 /*!< Framer test data PRBS15 data source */
    ADI_ADRV9025_FTD_PRBS9,                  /*!< Framer test data PRBS9 data source */
    ADI_ADRV9025_FTD_PRBS7,                  /*!< Framer test data PRBS7 data source */
    ADI_ADRV9025_FTD_RAMP,                   /*!< Framer test data ramp data source */
    ADI_ADRV9025_FTD_PATTERN_REPEAT = 14,    /*!< Framer test data 16-bit programmed pattern repeat source */
    ADI_ADRV9025_FTD_PATTERN_ONCE = 15       /*!< Framer test data 16-bit programmed pattern executed once source */

} adi_adrv9025_FramerDataSource_e;

/**
*  \brief Enum of Framer test data injection points
*/
typedef enum adi_adrv9025_FramerDataInjectPoint
{
    ADI_ADRV9025_FTD_FRAMERINPUT = 0,        /*!< Framer test data injection point at framer input */
    ADI_ADRV9025_FTD_SERIALIZER,             /*!< Framer test data injection point at serializer input */
    ADI_ADRV9025_FTD_POST_LANEMAP            /*!< Framer test data injection point after lane mapping */

} adi_adrv9025_FramerDataInjectPoint_e;

/**
* \brief Enum for JESD204B PRBS generated types
*/
typedef enum adi_adrv9025_DeframerPrbsOrder
{
    ADI_ADRV9025_PRBS_DISABLE = 0,  /*!< Deframer PRBS pattern disable */
    ADI_ADRV9025_PRBS7,             /*!< Deframer PRBS7 pattern select */
    ADI_ADRV9025_PRBS9,             /*!< Deframer PRBS9 pattern select */
    ADI_ADRV9025_PRBS15,            /*!< Deframer PRBS15 pattern select */
    ADI_ADRV9025_PRBS31,            /*!< Deframer PRBS31 pattern select */
    ADI_ADRV9025_USERDATA           /*!< Deframer user supplied pattern select */

} adi_adrv9025_DeframerPrbsOrder_e;

/**
* \brief Enum for JESD204B deserializer / deframer PRBS selection
*/
typedef enum adi_adrv9025_DeframerPrbsCheckLoc
{
    ADI_ADRV9025_PRBSCHECK_LANEDATA = 0, /*!< Check PRBS at deserializer lane output (does not require JESD204b link) */
    ADI_ADRV9025_PRBSCHECK_SAMPLEDATA    /*!< Check PRBS at output of deframer (JESD204b deframed sample) */

} adi_adrv9025_DeframerPrbsCheckLoc_e;

/**
* \brief Enum to select GPINT enable/disable for deframer error counters
*/
typedef enum adi_adrv9025_DfrmErrCounterIrqSel
{
    ADI_ADRV9025_DFRM_ERR_COUNT_DISABLE_IRQ = 0, /*!< Don't create GPINT when error counter overflows 255 */
    ADI_ADRV9025_DFRM_ERR_COUNT_ENABLE_IRQ       /*!< Create GPINT when error counter overflows 255 */
} adi_adrv9025_DfrmErrCounterIrqSel_e;

/**
* \brief Data structure to hold the DAC sample crossbar information
*/
typedef struct adi_adrv9025_DacSampleXbarCfg
{
    adi_adrv9025_DacSampleXbarSel_e tx1DacChanI;    /*!< Sample Crossbar select for Tx1 I channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx1DacChanQ;    /*!< Sample Crossbar select for Tx1 Q channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx2DacChanI;    /*!< Sample Crossbar select for Tx2 I channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx2DacChanQ;    /*!< Sample Crossbar select for Tx2 Q channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx3DacChanI;    /*!< Sample Crossbar select for Tx3 I channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx3DacChanQ;    /*!< Sample Crossbar select for Tx3 Q channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx4DacChanI;    /*!< Sample Crossbar select for Tx4 I channel data*/
    adi_adrv9025_DacSampleXbarSel_e tx4DacChanQ;    /*!< Sample Crossbar select for Tx4 Q channel data*/
} adi_adrv9025_DacSampleXbarCfg_t;

/**
* \brief Data structure to hold the ADC sample crossbar information
*/
typedef struct adi_adrv9025_AdcSampleXbarCfg
{
    adi_adrv9025_AdcSampleXbarSel_e conv0;      /*!< Sample Crossbar select for converter 0*/
    adi_adrv9025_AdcSampleXbarSel_e conv1;      /*!< Sample Crossbar select for converter 1*/
    adi_adrv9025_AdcSampleXbarSel_e conv2;      /*!< Sample Crossbar select for converter 2*/
    adi_adrv9025_AdcSampleXbarSel_e conv3;      /*!< Sample Crossbar select for converter 3*/
    adi_adrv9025_AdcSampleXbarSel_e conv4;      /*!< Sample Crossbar select for converter 4*/
    adi_adrv9025_AdcSampleXbarSel_e conv5;      /*!< Sample Crossbar select for converter 5*/
    adi_adrv9025_AdcSampleXbarSel_e conv6;      /*!< Sample Crossbar select for converter 6*/
    adi_adrv9025_AdcSampleXbarSel_e conv7;      /*!< Sample Crossbar select for converter 7*/
    adi_adrv9025_AdcSampleXbarSel_e conv8;      /*!< Sample Crossbar select for converter 8*/
    adi_adrv9025_AdcSampleXbarSel_e conv9;      /*!< Sample Crossbar select for converter 9*/
    adi_adrv9025_AdcSampleXbarSel_e conv10;     /*!< Sample Crossbar select for converter 10*/
    adi_adrv9025_AdcSampleXbarSel_e conv11;     /*!< Sample Crossbar select for converter 11*/
    adi_adrv9025_AdcSampleXbarSel_e conv12;     /*!< Sample Crossbar select for converter 12*/
    adi_adrv9025_AdcSampleXbarSel_e conv13;     /*!< Sample Crossbar select for converter 13*/
    adi_adrv9025_AdcSampleXbarSel_e conv14;     /*!< Sample Crossbar select for converter 14*/
    adi_adrv9025_AdcSampleXbarSel_e conv15;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv16;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv17;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv18;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv19;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv20;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv21;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv22;     /*!< Sample Crossbar select for converter 15*/
    adi_adrv9025_AdcSampleXbarSel_e conv23;     /*!< Sample Crossbar select for converter 15*/
} adi_adrv9025_AdcSampleXbarCfg_t;

/**
* \brief Data structure to hold the Framer output lane crossbar information
*
* Note that not all framers have 8 outputs.  A physical lane can only be used
* on one framer at a time.
*/
typedef struct adi_adrv9025_SerLaneXbar
{
    uint8_t lane0FramerOutSel; /*!< Framer output to route to Physical Lane 0 (valid 0-7) */
    uint8_t lane1FramerOutSel; /*!< Framer output to route to Physical Lane 1 (valid 0-7) */
    uint8_t lane2FramerOutSel; /*!< Framer output to route to Physical Lane 2 (valid 0-7) */
    uint8_t lane3FramerOutSel; /*!< Framer output to route to Physical Lane 3 (valid 0-7) */
} adi_adrv9025_SerLaneXbar_t;

/**
* \brief Data structure to hold the Deframer input lane crossbar information
*
* Note that not all deframers have 8 inputs.
*/
typedef struct adi_adrv9025_DeserLaneXbar
{
    uint8_t deframerInput0LaneSel; /*!< Physical lane select for deframer input 0  (valid 0-7) */
    uint8_t deframerInput1LaneSel; /*!< Physical lane select for deframer input 1 (valid 0-7) */
    uint8_t deframerInput2LaneSel; /*!< Physical lane select for deframer input 2 (valid 0-7) */
    uint8_t deframerInput3LaneSel; /*!< Physical lane select for deframer input 3 (valid 0-7) */
} adi_adrv9025_DeserLaneXbar_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b DeFramer ILAS configuration settings
*/
typedef struct adi_adrv9025_DfrmLane0Cfg
{
    uint8_t dfrmDID; /*!< JESD204B Configuration Device ID for ILAS check */
    uint8_t dfrmBID; /*!< JESD204B Configuration Bank ID for ILAS check */
    uint8_t dfrmLID0; /*!< JESD204B Configuration starting Lane ID for ILAS check */
    uint8_t dfrmL; /*!< JESD204B Configuration L = lanes per data converter for ILAS check */
    uint8_t dfrmSCR; /*!< JESD204B Configuration scramble setting for ILAS check */
    uint8_t dfrmF; /*!< JESD204B Configuration F = octets per frame for ILAS check */
    uint8_t dfrmK; /*!< JESD204B Configuration K = frames per multiframe for ILAS check */
    uint8_t dfrmM; /*!< JESD204B Configuration M = number of data converters for ILAS check */
    uint8_t dfrmN; /*!< JESD204B Configuration N = data converter sample resolution for ILAS check */
    uint8_t dfrmCS; /*!< JESD204B Configuration CS = number of control bits transferred per sample per frame for ILAS check */
    uint8_t dfrmNP; /*!< JESD204B Configuration NP = JESD204B word size based on the highest resolution of the data converter for ILAS check */
    uint8_t dfrmS; /*!< JESD204B Configuration S = number of samples/data converter/frame for ILAS check */
    uint8_t dfrmCF; /*!< JESD204B Configuration CF = '0' = control bits appended to each sample, '1' = appended to end of frame for ILAS check */
    uint8_t dfrmHD; /*!< JESD204B Configuration HD = high density bit - samples are contained within lane (0) or divided over more than one lane (1) for ILAS check */
    uint8_t dfrmFCHK0; /*!< JESD204B Configuration checksum for ILAS check lane0 */
} adi_adrv9025_DfrmLane0Cfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Framer configuration settings
*/
typedef struct adi_adrv9025_FrmCfg
{
    uint8_t enableJesd204C; /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t bankId; /*!< JESD204B Configuration Bank ID extension to Device ID. Range is 0..15 */
    uint8_t deviceId; /*!< JESD204B Configuration Device ID link identification number. Range is 0..255 */
    uint8_t lane0Id; /*!< JESD204B Configuration starting Lane ID. If more than one lane is used, each lane will increment from the Lane0 ID. Range is 0..31 */
    uint8_t jesd204M; /*!< Number of ADCs (0, 2, or 4) where 2 ADCs are required per receive chain (I and Q). */
    uint16_t jesd204K; /*!< Number of frames in a multiframe. Default = 32, F*K must be modulo 4. Where, F=2*M/numberOfLanes  (Max 32 for JESD204B, Max 256 for JESD204C). */
    uint8_t jesd204F; /*!< Number of bytes(octets) per frame (Valid 1, 2, 4, 8). */
    uint8_t jesd204Np; /*!< converter sample resolution (12, 16, 24). */
    uint8_t jesd204E; /*!< JESD204C E parameter */
    uint8_t scramble; /*!< Scrambling off if framerScramble = 0, if framerScramble > 0 scrambling is enabled */
    uint8_t serializerLanesEnabled; /*!< Serializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    uint16_t lmfcOffset; /*!< LMFC offset value for deterministic latency setting. */
    uint8_t syncbInSelect; /*!< Selects SYNCb input source. Where, 0 = use SYNCBIN0 for this framer, 1 = use SYNCBIN1 for this framer, 2 = use SYNCBIN2 */
    uint8_t overSample; /*!< Selects framer bit repeat or oversampling mode for lane rate matching. Where, 0 = bitRepeat mode (changes effective lanerate), 1 = overSample (maintains same lane rate between ObsRx framer and Rx framer and oversamples the ADC samples) */
    uint8_t syncbInLvdsMode; /*!< 1 - enable LVDS input pad with 100ohm internal termination, 0 - enable CMOS input pad */
    uint8_t syncbInLvdsPnInvert; /*!< 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */
    adi_adrv9025_SerLaneXbar_t serializerLaneCrossbar; /*!< Lane crossbar to map framer lane outputs to physical lanes */
    adi_adrv9025_AdcSampleXbarCfg_t adcCrossbar; /*!< ADC converter to framer input mapping */
    uint8_t newSysrefOnRelink; /*!< Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set */
    uint8_t sysrefForStartup; /*!< 1 = Framer: Require a SYSREF before CGS will be output from serializer, 0: Allow CGS to output before SYSREF occurs (recommended on framer to allow deframer CDR to lock and EQ to train)*/
    uint8_t sysrefNShotEnable; /*!< 1 = Enable SYSREF NShot (ability to ignore first rising edge of SYSREF to ignore possible runt pulses.) */
    uint8_t sysrefNShotCount; /*!< Count value of which SYSREF edge to use to reset LMFC phase, valid range is 0 to 15 */
    uint8_t sysrefIgnoreWhenLinked; /*!< When JESD204 link is up and valid, 1= ignore any sysref pulses */
} adi_adrv9025_FrmCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Link Sharing Framer configuration settings
*/
typedef struct adi_adrv9025_LinkSharingCfg
{
    uint8_t linkSharingEnabled;                                 /*!< 1 = link sharing enabled. This structure will hold ORx settings (typical); 0 = no link sharing. This structure unused */
    uint8_t linkSharingM;                                       /*!< Number of ADCs (0, 2, or 4) where 2 ADCs are required per receive chain (I and Q). */
    uint8_t linkSharingS; /*!< number of samples per frame */
    uint8_t linkSharingNp;                                      /*!< converter sample resolution (12, 16, 24). */
    adi_adrv9025_AdcSampleXbarCfg_t linkSharingAdcCrossbar;     /*!< ADC converter to framer input mapping */
} adi_adrv9025_LinkSharingCfg_t;

/**
* \brief Data structure to hold the settings for the deframer configuration
*/
typedef struct adi_adrv9025_DfrmCfg
{
    uint8_t enableJesd204C; /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t bankId; /*!< Extension to Device ID. Range is 0..15 , bankId is not supported for Jrx, returns always 0*/
    uint8_t deviceId; /*!< Link identification number. Range is 0..255 */
    uint8_t lane0Id; /*!< Lane0 ID. Range is 0..31 */
    uint8_t jesd204M; /*!< Number of DACs (0, 2, or 4) - 2 DACs per transmit chain (I and Q) */
    uint16_t jesd204K; /*!< Number of frames in a multiframe. Default = 32, F*K = modulo 4. Where, F=2*M/numberOfLanes  (Max 32 for JESD204B, Max 256 for JESD204C) */
    uint8_t jesd204F; /*!< Number of bytes(octets) per frame . */
    uint8_t jesd204Np; /*!< converter sample resolution (12, 16) */
    uint8_t jesd204E; /*!< JESD204C E parameter */
    uint8_t scramble; /*!< Scrambling off if scramble = 0, if framerScramble > 0 scrambling is enabled */
    uint8_t deserializerLanesEnabled; /*!< Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
    uint16_t lmfcOffset; /*!< LMFC offset value to adjust deterministic latency. */
    uint8_t syncbOutSelect; /*!< Selects deframer SYNCBOUT pin (0 = SYNCBOUT0, 1 = SYNCBOUT1, 2 = output SYNCB to SYNCBOUT0 and SYNCBOUT1) */
    uint8_t syncbOutLvdsMode; /*!< 1 - enable LVDS output pad, 0 - enable CMOS output pad  */
    uint8_t syncbOutLvdsPnInvert; /*!< 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */
    uint8_t syncbOutCmosSlewRate; /*!< 0 - fastest rise/fall times, 3 - slowest rise/fall times(not used, always returns 0) */
    uint8_t syncbOutCmosDriveLevel; /*!< 0 - normal cmos drive level, 1 - double cmos drive level */
    adi_adrv9025_DeserLaneXbar_t deserializerLaneCrossbar; /*!< Lane crossbar to map physical lanes to deframer inputs */
    adi_adrv9025_DacSampleXbarCfg_t dacCrossbar; /*!< Deframer output to DAC mapping */
    uint8_t newSysrefOnRelink; /*!< Flag for determining if SYSREF on relink should be set. Where, if > 0 = set, '0' = not set */
    uint8_t sysrefForStartup; /*!< Suggested to enable for deframer so deframer will not assert SYNCB to init the link until SYSREF has occurred, which resets LMFC phase */
    uint8_t sysrefNShotEnable; /*!< 1 = Enable SYSREF NShot (ability to ignore first rising edge of SYSREF to ignore possible runt pulses.) */
    uint8_t sysrefNShotCount; /*!< Count value of which SYSREF edge to use to reset LMFC phase, valid range is 0 to 15 */
    uint8_t sysrefIgnoreWhenLinked; /*!< When JESD204 link is up and valid, 1= ignore any sysref pulses */
} adi_adrv9025_DfrmCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Serializer configuration information
*/
typedef struct adi_adrv9025_SerCfg
{
    uint8_t serAmplitude;                       /*!< Serializer amplitude setting. Default = 1. (Range is 0..3) 0 = 1.0 * VTT, 1 = 0.85*VTT, 2 = 0.75*VTT, 3 = 0.5*VTT */
    uint8_t serPreEmphasis;                     /*!< Serializer pre-emphasis setting. Default = 0 (Range is 0..2) , 0=0dB, 1=3dB, 2=6dB*/
    uint8_t serPostEmphasis;                    /*!< Serializer Post-emphasis setting. (Valid 0,1,2,3,4) 0=0dB, 1=3dB, 2=6dB, 3=9dB, 4=12dB */
    uint8_t serInvertLanePolarity;              /*!< Serializer Lane PN inversion select. Default = 0. Where, bit[0] = 1 will invert lane [0], bit[1] = 1 will invert lane 1, etc. */
} adi_adrv9025_SerCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Deserializer configuration information
*/
typedef struct adi_adrv9025_DesCfg
{
    uint8_t desInvertLanePolarity;      /*!< Deserializer Lane PN inversion select.  1 = Invert PN, 0 = Do nothing */
    uint8_t highBoost;                  /*!< High Boost Lane enable select. 1 = High Boost enabled, 0 = High Boost disabled */
    uint32_t configOption1;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption2;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption3;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption4;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption5;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption6;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption7;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption8;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption9;             /*!< SERDES init & tracking calibration algorithm parameter */
    uint32_t configOption10;            /*!< SERDES init & tracking calibration algorithm parameter */
} adi_adrv9025_DesCfg_t;


/**
*  \brief Data structure to hold ADRV9025 Data Configuration controls
*/
typedef struct adi_adrv9025_JesdDataConfigCtrl
{
    uint8_t enable;             /*!< Enables data configuration feature */
    uint8_t configOption1;      /*!< Data configuration option 1 */
    uint8_t configOption2;      /*!< Data configuration option 2 */
} adi_adrv9025_JesdDataConfigCtrl_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Framer and Deframer configuration information
*/
typedef struct adi_adrv9025_DataInterfaceCfg
{
    adi_adrv9025_FrmCfg_t framer[3];                        /*!< Framer 0/1/2 configuration data structures */
    adi_adrv9025_DfrmCfg_t deframer[2];                     /*!< Deframer 0/1 configuration data structures */
    adi_adrv9025_SerCfg_t serCfg[ADI_ADRV9025_NUM_LANES];   /*!< Serializer PHY layer settings */
    adi_adrv9025_DesCfg_t desCfg[ADI_ADRV9025_NUM_LANES];   /*!< Deserializer PHY layer settings */
    adi_adrv9025_LinkSharingCfg_t linkSharingCfg;           /*!< Link Sharing configuration. Typically used to store ORx settings when Framer 0 stores Rx settings */
    adi_adrv9025_JesdDataConfigCtrl_t dataCfg;              /*!< Data configuration control structure */
    uint8_t channelSelect;                                  /*!< Allows use of control channel */
    uint8_t channelMode;                                    /*!< Allows the LSB of JESD data to be used as control bits when JESD compression is disabled */
} adi_adrv9025_DataInterfaceCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Framer Test data and debug configuration
*/
typedef struct adi_adrv9025_FrmTestDataCfg
{
    uint8_t framerSelMask;
    adi_adrv9025_FramerDataSource_e testDataSource;
    adi_adrv9025_FramerDataInjectPoint_e injectPoint;
} adi_adrv9025_FrmTestDataCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Deframer PRBS test configuration
*/
typedef struct adi_adrv9025_DfrmPrbsCfg
{
    adi_adrv9025_DeframerSel_e deframerSel;
    adi_adrv9025_DeframerPrbsOrder_e polyOrder;
    adi_adrv9025_DeframerPrbsCheckLoc_e checkerLocation;
} adi_adrv9025_DfrmPrbsCfg_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Deframer PRBS Error Counter values on readback
*/
typedef struct adi_adrv9025_DfrmPrbsErrCounters
{
    adi_adrv9025_DeframerPrbsCheckLoc_e sampleSource;    /*! Sample or Lane mode. Mode error count and status use zeroth index  for sample mode */
    uint8_t errorStatus[ADI_ADRV9025_NUM_LANES]; /*! Bit 0 = Lane inverted, bit 1 = invalid data flag, bit 2 = sample/lane error flag */
    uint32_t laneErrors[ADI_ADRV9025_NUM_LANES]; /*! Lane 0 contains error counters if in sample mode, otherwise errors are maintained per lane */
} adi_adrv9025_DfrmPrbsErrCounters_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Framer status information
*/
typedef struct adi_adrv9025_FramerStatus
{
    uint8_t  status;
    uint8_t  framerSyncNeCount;   /*! Count of SYNCB falling edges */
    uint8_t  qbfStateStatus;
    uint8_t  syncNSel;
} adi_adrv9025_FramerStatus_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Deframer status information
*/
typedef struct adi_adrv9025_DeframerStatus
{
    uint8_t  status;
    uint8_t  reserved;
} adi_adrv9025_DeframerStatus_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b Deframer Mask and Source register bits
*/
typedef struct adi_adrv9025_DeframerIrq_Mask_Vector
{
    uint16_t deframer0;
    uint16_t deframer1;
} adi_adrv9025_DeframerIrq_Mask_Vector_t;

/**
*  \brief Data structure to hold ADRV9025 JESD204b DeFramer ILAS configuration settings
*         for each deframer
*/
typedef struct adi_adrv9025_DfrmCompareData
{
    uint32_t zeroCheckFlag; /*!< Link Valid Data Flag - if returned all zero then link is not active */
    uint32_t ilasMismatchDfrm; /*!< ILAS mismatch Flag - all set bits indicated mismatch and bit position indicates incorrect value */
    adi_adrv9025_DfrmLane0Cfg_t dfrmIlasData; /*!< Captured ILAS data  */
    adi_adrv9025_DfrmLane0Cfg_t dfrmCfgData; /*!< Configured ILAS data  */
} adi_adrv9025_DfrmCompareData_t;
/**
*  \brief Data structure to hold ADRV9025 JESD204b Deframer error counter status
*/
typedef struct adi_adrv9025_DfrmErrCounterStatus
{
    uint8_t  laneStatus; /*!< deframer status bits  */
    uint8_t  bdCntValue; /*!< Bad-Disparity error counter current value */
    uint8_t  uekCntValue; /*!< UnExpected-K error counter current value */
    uint8_t  nitCntValue; /*!< Not-In-Table error counter current value */
} adi_adrv9025_DfrmErrCounterStatus_t;

typedef struct adi_adrv9025_GpInterruptSettings
{
    uint64_t gpIntMaskPin0; /*!< Default setting for GP interrupt Pin 0 mask  */
    uint64_t gpIntMaskPin1; /*!< Default setting for GP interrupt Pin 1 mask  */
} adi_adrv9025_GpInterruptSettings_t;

typedef struct adi_adrv9025_FramerProfileSetting
{
    uint8_t enableJesd204C;         /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t lane0Id;                /*!< JESD204B Configuration starting Lane ID. */
    uint8_t serializerLanesEnabled; /*!< Serializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
} adi_adrv9025_FramerProfileSetting_t;

typedef struct adi_adrv9025_DeframerProfileSetting
{
    uint8_t enableJesd204C;           /*!< 1= Enable JESD204C framer, 0 = use JESD204B framer */
    uint8_t lane0Id;                  /*!< JESD204B Configuration starting Lane ID. */
    uint8_t deserializerLanesEnabled; /*!< Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
} adi_adrv9025_DeframerProfileSetting_t;

typedef struct adi_adrv9025_JesdDataProfileCfg
{
    adi_adrv9025_FramerProfileSetting_t   framerCfg[3];     /*!< Framer 0/1/2 saved configuration */
    adi_adrv9025_DeframerProfileSetting_t deframerCfg[2];   /*!< Deframer 0/1 saved configuratio */
} adi_adrv9025_JesdDataProfileCfg_t;

/**
*  \brief Enum to select desired deframer lane
*/
typedef enum adi_adrv9025_LaneSel
{
    ADI_ADRV9025_LANE_0 = 0x01,  /*!< LANE 0 selection */
    ADI_ADRV9025_LANE_1 = 0X02,  /*!< LANE 1 selection */
    ADI_ADRV9025_LANE_2 = 0X04,  /*!< LANE 2 selection */
    ADI_ADRV9025_LANE_3 = 0X08,  /*!< LANE 3 selection */
    ADI_ADRV9025_ALL_LANES = 0x0F
} adi_adrv9025_LaneSel_e;

/**
*  \brief Structure to hold SPO settings
*/
typedef struct adi_adrv9025_DesEyeSweepResults
{
    uint8_t goodSpoLeft;
    uint8_t goodSpoRight;
    uint8_t goodSpoTotal;
} adi_adrv9025_DesEyeSweepResults_t;

typedef struct adi_adrv9025_EyeDiagramResultsQR
{
    uint8_t phase;         /* phase point in time that reading occured, this will be between -16 and +16 as Unit Interval for signal on QR mode consists of 32 phases */
    uint8_t b1Threshold;   /* Threshold of b1 value in DFE Core, this determines where a detection is approaching a 1 bit value or its inverse value approaching a 0 bit */
    int8_t  eyeStepUp;     /* position of eye top reading for a particular point in time */
    int8_t  eyeStepDown;   /* position of eye bottom reading for a particular point in time */

} adi_adrv9025_EyeDiagramResultsQR_t;

typedef struct adi_adrv9025_VerticalISIConfig
{
    uint32_t error_threshold;          /* Error Threshold */
    uint8_t start_eyemon_step;         /* Eye monitor step interval */
    uint8_t num_flashes;               /* Number of flashes to use */
    uint8_t c0_notc1;                  /* Flag indicating which comparitor to use (c0/c1) */
    uint8_t search_up_down;            /* Directional flag either search up or down */
} adi_adrv9025_VerticalISIConfig_t;

#ifdef __cplusplus
}


#endif

#endif /* _ADI_ADRV9025_DATA_INTERFACE_TYPES_H_ */
