// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_rx_types.h
 * \brief Contains Talise API Rx datapath data types
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_RX_TYPES_H_
#define TALISE_RX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "talise_radioctrl_types.h"

/**
 *  \brief Enum of possible Rx channel enables
 */
typedef enum {
	TAL_RXOFF = 0,      /*!< No Rx channels are enabled */
	TAL_RX1,            /*!< Rx1 channel enabled */
	TAL_RX2,            /*!< Rx2 channel enabled */
	TAL_RX1RX2          /*!< Rx1 + Rx2 channels enabled */
} taliseRxChannels_t;

/**
 *  \brief Enum of possible Observation Rx channels to enable and run calibrations for during init.
 *  Choose ENUM value that enables all channels that will be used in the system.  During system use,
 *  only one channel can be used at a time.  This is also used to alert the ARM processor
 *  which observation channels are valid for the current desired system setup.
 */
typedef enum {
	TAL_ORXOFF      = 0x00,
	TAL_ORX1        = 0x01,
	TAL_ORX2        = 0x02,
	TAL_ORX1ORX2    = 0x03
} taliseObsRxChannels_t;

/**
 *  \brief Enum to set the Rx Gain control mode
 */
typedef enum {
	TAL_MGC = 0,        /*!< Manual Gain Control */
	TAL_AGCFAST = 1,    /*!< 01 Fast Attack AGC Mode TDD mode */
	TAL_AGCSLOW = 2,    /*!< Slow Loop AGC FDD, TDD modes  */
	TAL_HYBRID = 3      /*!< Hybrid AGC Gain Control */
} taliseGainMode_t;

typedef struct {
	uint8_t rxFeGain;
	uint8_t extControl;
	uint8_t adcTiaGain;
	int16_t digGain;
	uint16_t phaseOffset;
} taliseRxGainTable_t;

typedef struct {
	uint8_t rxFeGain;
	uint8_t extControl;
	uint8_t adcTiaGain;
	int16_t digGain;
} taliseOrxGainTable_t;

/**
 *  \brief Enum to hold Talise Floating Point Formatter number of exponent bits
 */
typedef enum {
	TAL_2_EXPONENTBITS = 0,     /*!< Floating point values have 2 exponent bits, 13 significand bits, 1 sign bit */
	TAL_3_EXPONENTBITS = 1,     /*!< Floating point values have 3 exponent bits, 12 significand bits, 1 sign bit */
	TAL_4_EXPONENTBITS = 2,     /*!< Floating point values have 4 exponent bits, 11 significand bits, 1 sign bit */
	TAL_5_EXPONENTBITS = 3      /*!< Floating point values have 5 exponent bits, 10 significand bits, 1 sign bit  */
} taliseFpExponentModes_t;

/**
 *  \brief Enum to hold Talise Floating Point Formatter rounding modes for the Rx data path
 */
typedef enum {
	TAL_ROUND_TO_EVEN = 0,           /*!< Round floating point ties to an even value          */
	TAL_ROUNDTOWARDS_POSITIVE = 1,   /*!< Round floating point toward the positive direction  */
	TAL_ROUNDTOWARDS_NEGATIVE = 2,   /*!< Round floating point toward the negative direction  */
	TAL_ROUNDTOWARDS_ZERO = 3,       /*!< Round floating point toward the zero direction      */
	TAL_ROUND_FROM_EVEN = 4          /*!< Round floating point ties away from even value      */
} taliseFpRoundModes_t;

/**
 *  \brief Enum to hold Talise Rx attenuation values that are used when the
 *         floating point format is enabled
 */
typedef enum {
	TAL_FPATTEN_24DB = 4,      /*!< Set Rx attenuation to 24 dB when Rx Data format set to floating point mode  */
	TAL_FPATTEN_18DB = 5,      /*!< Set Rx attenuation to 18 dB when Rx Data format set to floating point mode  */
	TAL_FPATTEN_12DB = 6,      /*!< Set Rx attenuation to 12 dB when Rx Data format set to floating point mode */
	TAL_FPATTEN_6DB = 7,       /*!< Set Rx attenuation to 6 dB when Rx Data format set to floating point mode */
	TAL_FPATTEN_0DB = 0,            /*!< Set Rx attenuation to 0 dB when Rx Data format set to floating point mode */
	TAL_FPATTEN_MINUS6DB = 1,            /*!< Set Rx attenuation to -6 dB when Rx Data format set to floating point mode */
	TAL_FPATTEN_MINUS12DB = 2,           /*!< Set Rx attenuation to -12 dB when Rx Data format set to floating point mode */
	TAL_FPATTEN_MINUS18DB = 3            /*!< Set Rx attenuation to -18 dB when Rx Data format set to floating point mode */
} taliseFpAttenSteps_t;

/**
 *  \brief Enum to select the four Data Formatting options
 */
typedef enum {
	TAL_GAIN_COMPENSATION_DISABLED = 0,  /*!< Gain Compensation and Data Formatting are disabled */
	TAL_GAIN_WITH_FLOATING_POINT,        /*!< Gain Compensation enabled with floating point data formatting enabled and internal slicer enabled */
	TAL_GAIN_WITH_INTSLICER_NOGPIO,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with no GPIO Slicer Position output */
	TAL_GAIN_WITH_INTSLICER,             /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output */
	TAL_GAIN_WITH_EXTERNAL_SLICER        /*!< Gain Compensation enabled with integer data formatting and external slicer enabled */
} taliseDataFormattingModes_t;

/**
 *  \brief Enum to hold Talise Gain Slicer external pin gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum {
	TAL_EXTSLICER_STEPSIZE_1DB = 0,   /*!< Set Gain Slicer External gain step size to 1dB  */
	TAL_EXTSLICER_STEPSIZE_2DB,       /*!< Set Gain Slicer External gain step size to 2dB  */
	TAL_EXTSLICER_STEPSIZE_3DB,       /*!< Set Gain Slicer External gain step size to 3dB  */
	TAL_EXTSLICER_STEPSIZE_4DB        /*!< Set Gain Slicer External gain step size to 4dB  */
} taliseGainStepSize_t;

/**
 *  \brief Enum to hold Talise Rx1 Gain Slicer external pin GPIO selection options
 */
typedef enum {
	TAL_EXTSLICER_RX1_GPIO0_1_2 = 0,      /*!< Select Rx1 Gain Slicer External GPIO0, GPIO1, GPIO2 */
	TAL_EXTSLICER_RX1_GPIO5_6_7,          /*!< Select Rx1 Gain Slicer External GPIO5, GPIO6, GPIO7 */
	TAL_EXTSLICER_RX1_GPIO8_9_10,         /*!< Select Rx1 Gain Slicer External GPIO8, GPIO9, GPIO10 */
	TAL_EXTSLICER_RX1_GPIO_DISABLE        /*!< Select Rx1 Disable Gain Slicer External GPIO */
} taliseRx1ExtSlicerGpioSelect_t;

/**
*  \brief Enum to hold Talise Rx2 Gain Slicer external pin GPIO selection options
*/
typedef enum {
	TAL_EXTSLICER_RX2_GPIO11_12_13 = 0,   /*!< Select Rx2 Gain Slicer External GPIO11, GPIO12, GPIO13 */
	TAL_EXTSLICER_RX2_GPIO5_6_7 = 1,      /*!< Select Rx2 Gain Slicer External GPIO5, GPIO6, GPIO7 */
	TAL_EXTSLICER_RX2_GPIO_DISABLE = 3    /*!< Select Rx2 Disable Gain Slicer External GPIO */
} taliseRx2ExtSlicerGpioSelect_t;

/**
 *  \brief Enum to hold Talise integer mode number of embedded slicer bits and positions
 */
typedef enum {
	TAL_NO_EMBEDDED_SLICER_BITS = 0,       /*!< Disabled all embedded slicer bits  */
	TAL_EMBED_1_SLICERBIT_AT_MSB,          /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the MSB position in the data frame */
	TAL_EMBED_1_SLICERBIT_AT_LSB,          /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the LSB position in the data frame */
	TAL_EMBED_2_SLICERBITS_AT_MSB,         /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame */
	TAL_EMBED_2_SLICERBITS_AT_LSB          /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame */
} taliseEmbeddedBits_t;

/**
 *  \brief Enum to hold Talise integer sample resolution
 */
typedef enum {
	TAL_INTEGER_12BIT_2SCOMP = 0,    /*!< Selects integer sample 12 bit resolution with 2s compliment    */
	TAL_INTEGER_12BIT_SIGNED,        /*!< Selects integer sample 12 bit resolution with signed magnitude */
	TAL_INTEGER_16BIT_2SCOMP,        /*!< Selects integer sample 16 bit resolution with 2s compliment    */
	TAL_INTEGER_16BIT_SIGNED,        /*!< Selects integer sample 16 bit resolution with signed magnitude */
	TAL_INTEGER_24BIT_2SCOMP,        /*!< Selects integer sample 24 bit resolution with 2s compliment    */
	TAL_INTEGER_24BIT_SIGNED         /*!< Selects integer sample 24 bit resolution with signed magnitude */
} taliseIntSampleResolution_t;

/**
*  \brief Data structure to hold Talise Floating Point Formatter Configuration
*/
typedef struct {
	taliseDataFormattingModes_t formatSelect; /*!< Rx Channel format mode selects */

	/* Float Config Settings */
	taliseFpRoundModes_t
	fpRoundMode;         /*!< Rounding mode for floating point format (See enum values) */
	uint8_t fpDataFormat;                     /*!< If floating point format is enabled in formatSelect member, this sets the 16 bit output from MSB to LSB, 1 = {Sign, Significand, Exponent}, 0 = {Sign, Exponent, Significand} */
	uint8_t fpEncodeNan;                      /*!< 1 =  encodes the highest value of Exponent to mean NaN (Not a Number) to be compatible to IEEE754 specification (Valid: 0 or 1) */
	taliseFpExponentModes_t
	fpNumExpBits;     /*!< Indicates the number of exponent and significand bits in the floating point number */
	uint8_t fpHideLeadingOne;                 /*!< 1 =  Hides the leading one in significand to be compatible to the IEEE754 specification. 0 = a leading one exists at the MSB of the significand.  (Valid: 0, 1) */
	taliseFpAttenSteps_t
	fpRx1Atten;          /*!< Rx1 - Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */
	taliseFpAttenSteps_t
	fpRx2Atten;          /*!< Rx2 - Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */

	/* Integer Config Settings */
	taliseEmbeddedBits_t
	intEmbeddedBits;             /*!< Integer number of embedded bits and position */
	taliseIntSampleResolution_t
	intSampleResolution;  /*!< Integer sample resolution selecting either 12, 16, 14 bit modes with signed or 2s Complement */

	/* Slicer Config Settings */
	taliseGainStepSize_t
	extPinStepSize;          /*!< Enum selects the external pin gain step size */
	taliseRx1ExtSlicerGpioSelect_t
	rx1GpioSelect; /*!< Enum selects the Rx1 GPIO Configuration */
	taliseRx2ExtSlicerGpioSelect_t
	rx2GpioSelect; /*!< Enum selects the Rx2 GPIO Configuration */
	uint8_t externalLnaGain;                      /*!< Selects Slicer to compensate for external dualband LNA {0 - disabled, 1 - enabled */
	uint8_t tempCompensationEnable;               /*!< Selects Slicer to compensate for temperature variations {0 - disabled, 1 - enabled */
} taliseRxDataFormat_t;

/**
 *  \brief Data structure to hold Talise Rx Gain Control Pin Configuration
 */
typedef struct {
	uint8_t incStep;                /*!< Increment in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
	uint8_t decStep;                /*!< Decrement in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
	taliseGpioPinSel_t
	rxGainIncPin;/*!< GPIO used for the Increment gain input: Rx1 : TAL_GPIO_00 or TAL_GPIO_10, Rx2 : TAL_GPIO_03 or TAL_GPIO_13*/
	taliseGpioPinSel_t
	rxGainDecPin;/*!< GPIO used for the Decrement gain input: Rx1 : TAL_GPIO_01 or TAL_GPIO_11, Rx2 : TAL_GPIO_04 or TAL_GPIO_14*/
	uint8_t enable;                 /*!< Enable (1) or disable (0) the gain pin control*/
} taliseRxGainCtrlPin_t;

/**
 *  \brief Data structure to hold Talise Rx dualband LNA gain table entries
 */
typedef struct {
	uint8_t dualbandControl;    /*!< The external control value to be output on the 3.3V GPIO?s to control the LNA (values 0-3). */
	uint8_t dualbandGain;       /*!< The gain compensation value for the corresponding external control, used for RSSI and gain compensation.
                                     Range of 0 to 63 (0 to +31.5db in 0.5db steps). */
} taliseDualBandLnaGainTable_t;

typedef enum {
	TAL_RX1_NCO1A = 0,
	TAL_RX1_NCO1B,
	TAL_RX1_NCO2A,
	TAL_RX1_NCO2B,

	TAL_RX2_NCO1A,
	TAL_RX2_NCO1B,
	TAL_RX2_NCO2A,
	TAL_RX2_NCO2B
} taliseRxNcoChannel_t;

#ifdef __cplusplus
}
#endif

#endif /* TALISE_RX_TYPES_H_ */
