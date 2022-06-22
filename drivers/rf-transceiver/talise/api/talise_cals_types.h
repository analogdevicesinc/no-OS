// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_cals_types.h
 * \brief Contains Talise API Calibration data types
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_CALS_TYPES_H_
#define TALISE_CALS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	TAL_TX_BB_FILTER            = 0x00000001,   /*!< Tx BB filter calibration */
	TAL_ADC_TUNER               = 0x00000002,   /*!< ADC tuner calibration */
	TAL_TIA_3DB_CORNER          = 0x00000004,   /*!< TIA 3dB corner calibration */
	TAL_DC_OFFSET               = 0x00000008,   /*!< DC offset calibration */
	TAL_TX_ATTENUATION_DELAY    = 0x00000010,   /*!< Tx attenuation delay calibration */
	TAL_RX_GAIN_DELAY           = 0x00000020,   /*!< Rx gain delay calibration */
	TAL_FLASH_CAL               = 0x00000040,   /*!< Flash converter comparator calibration */
	TAL_PATH_DELAY              = 0x00000080,   /*!< Path delay equalization calibration */
	TAL_TX_LO_LEAKAGE_INTERNAL  = 0x00000100,   /*!< Internal Tx LO leakage calibration */
	TAL_TX_LO_LEAKAGE_EXTERNAL  = 0x00000200,   /*!< External Tx LO leakage calibration */
	TAL_TX_QEC_INIT             = 0x00000400,   /*!< Tx quadrature error correction calibration */
	TAL_LOOPBACK_RX_LO_DELAY    = 0x00000800,   /*!< Loopback Rx LO delay path calibration */
	TAL_LOOPBACK_RX_RX_QEC_INIT = 0x00001000,   /*!< Loopback Rx quadrature error correction calibration */
	TAL_RX_LO_DELAY             = 0x00002000,   /*!< Rx LO delay path calibration */
	TAL_RX_QEC_INIT             = 0x00004000,   /*!< Rx quadrature error correction calibration */
	TAL_RX_PHASE_CORRECTION     = 0x00008000,   /*!< Rx Phase correction calibration */
	TAL_ORX_LO_DELAY            = 0x00010000,   /*!< ORx LO delay path calibration */
	TAL_ORX_QEC_INIT            = 0x00020000,   /*!< ORx quadrature error correction calibration */
	TAL_TX_DAC                  = 0x00040000,   /*!< Tx DAC passband calibration */
	TAL_ADC_STITCHING           = 0x00080000,   /*!< ADC stitching calibration */
	TAL_FHM_CALS                = 0x00800000    /*!< FHM (Fast Frequency Hopping Mode) Calibrations */
} taliseInitCalibrations_t;

typedef enum {
	TAL_TRACK_NONE              = 0x00000000,   /*!< Disable all tracking calibrations */
	TAL_TRACK_RX1_QEC           = 0x00000001,   /*!< Rx1 quadrature error correction tracking calibration */
	TAL_TRACK_RX2_QEC           = 0x00000002,   /*!< Rx2 quadrature error correction tracking calibration */
	TAL_TRACK_ORX1_QEC          = 0x00000004,   /*!< ORx1 quadrature error correction tracking calibration */
	TAL_TRACK_ORX2_QEC          = 0x00000008,   /*!< ORx2 quadrature error correction tracking calibration */
	TAL_TRACK_TX1_LOL           = 0x00000010,   /*!< Tx1 LO leakage tracking calibration */
	TAL_TRACK_TX2_LOL           = 0x00000020,   /*!< Tx2 LO leakage tracking calibration */
	TAL_TRACK_TX1_QEC           = 0x00000040,   /*!< Tx1 quadrature error correction tracking calibration */
	TAL_TRACK_TX2_QEC           = 0x00000080,   /*!< Tx2 quadrature error correction tracking calibration */
	TAL_TRACK_RX1_HD2           = 0x00000100,   /*!< Rx1 HD2 error correction tracking calibration */
	TAL_TRACK_RX2_HD2           = 0x00000200,   /*!< Rx2 HD2 error correction tracking calibration */
	TAL_TRACK_ALL               = 0x000003FF    /*!< ENUM specifies all tracking cals */
} taliseTrackingCalibrations_t;

/**
 *  \brief Enum of possible wait events to use with TALISE_waitForEvent()
 */
typedef enum {
	TAL_CLKPLLCP = 0,               /*!< CLK PLL CP wait event */
	TAL_CLKPLL_LOCK,                /*!< CLK PLL lock wait event */
	TAL_RFPLLCP,                    /*!< RF PLL CP wait event */
	TAL_RFPLL_LOCK,                 /*!< RF PLL lock wait event */
	TAL_AUXPLLCP,                   /*!< AUX PLL CP wait event */
	TAL_AUXPLL_LOCK,                /*!< AUX PLL LOCK wait event */
	TAL_ARMBUSY                     /*!< ARM busy wait event */
} taliseWaitEvent_t;

/**
 *  \brief Enum of possible Rx Digital DC Offset channels
 */
typedef enum {
	TAL_DC_OFFSET_RX_CHN = 0,
	TAL_DC_OFFSET_ORX_CHN = 1
} taliseDcOffsetChannels_t;

/**
 *  \brief Enum of Rx/ORx channels mask  for configuring (Enable /disable) DC offsets.
 */
typedef enum {
	TAL_DC_OFFSET_ALL_OFF = 0x00,               /*!< Disable all the channels */
	TAL_DC_OFFSET_RX1 = 0x01,                   /*!< Enables Rx1  */
	TAL_DC_OFFSET_RX2 = 0x02,                   /*!< Enables Rx2  */
	TAL_DC_OFFSET_ORX1 = 0x04,                   /*!< Enables ORx1  */
	TAL_DC_OFFSET_ORX2 = 0x08,                   /*!< Enables ORx2  */
	TAL_DC_OFFSET_ALL_ON = 0x0F              /*!< Enables all the channels  */
} taliseRxDcOffsettEn_t;

/**
 *  \brief Enum of Tracking Calibration Batch Size in Micro Seconds
 */
typedef enum {
	TAL_TRACK_BATCH_SIZE_500_US = 0,
	TAL_TRACK_BATCH_SIZE_200_US = 1
} taliseTrackingCalBatchSize_t;

/**
 * \brief Data structure to hold Tx LOL Status
 */
typedef struct {
	uint32_t errorCode;         /*!< error code from Tx LOL */
	uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
	uint32_t varianceMetric;    /*!< metric of how well the tracking cal is performing */
	uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
	uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} taliseTxLolStatus_t;

/**
 * \brief Data structure to hold Tx QEC Status
 */
typedef struct {
	uint32_t errorCode;         /*!< error code from Tx QEC */
	uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
	uint32_t correctionMetric;  /*!< metric of how well the tracking cal is performing */
	uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
	uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} taliseTxQecStatus_t;

/**
 * \brief Data structure to hold Rx QEC Status
 */
typedef struct {
	uint32_t errorCode;         /*!< error code from Rx QEC */
	uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
	uint32_t selfcheckIrrDb;    /*!<  */
	uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
	uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} taliseRxQecStatus_t;

/**
 * \brief Data structure to hold Orx QEC Status
 */
typedef struct {
	uint32_t errorCode;         /*!< error code from Orx QEC */
	uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
	uint32_t selfcheckIrrDb;    /*!<  */
	uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
	uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} taliseOrxQecStatus_t;

/**
 * \brief Data structure to hold Rx HD2 Status
 */
typedef struct {
	uint32_t errorCode;         /*!< error code from HD2 */
	uint32_t percentComplete;   /*!< NOT APPLICABLE FOR HD2  Dynamic condition of this cal does not allow for convergence */
	uint32_t confidenceLevel;   /*!< Confidence level that cal has accurately indentified the coefficient required to cancel HD2  */
	/*!<     0 - No Observation, 1 - 7 indicates increasing confidence level */
	uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
	uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} taliseRxHd2Status_t;

/**
 * \brief Data structure to hold Rx HD2 calibration config settings
 *
 * Note: Set posSideBandSel to the side (negative - lower side band or
 * positive - upper side band) that has the desired
 * signal.  Correction will be correcting the 2nd harmonic distortion on the
 * same side of the spectrum.
 */
typedef struct {
	uint32_t posSideBandSel;     /*!< 1 = Correct HD2 of desired signal on the positive (upper) side of the complex FFT,
                                      0 = correct HD2 in the negative (lower)side of the complex FFT */
} taliseRxHd2Config_t;

#ifdef __cplusplus
}
#endif

#endif /* TALISE_CALS_TYPES_H_ */
