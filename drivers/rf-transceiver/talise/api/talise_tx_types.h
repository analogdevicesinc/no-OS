// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_tx_types.h
 * \brief Contains Talise API Tx datapath data types
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_TX_TYPES_H_
#define TALISE_TX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Enum of possible Tx channel enables
 */
typedef enum {
	TAL_TXOFF = 0,      /*!< No Tx channels are enabled */
	TAL_TX1,            /*!< Tx1 channel enabled */
	TAL_TX2,            /*!< Tx2 channel enabled */
	TAL_TX1TX2          /*!< Tx1 + Tx2 channels enabled */
} taliseTxChannels_t;

/**
 *  \brief Enum of possible Tx DAC FS boost options
 */
typedef enum {
	TAL_DACFS_0DB	= 0x0,	/*!< No Full Scale Boost */
	TAL_DACFS_3DB	= 0x1F  /*!< Full scale boost = 3dB */
} taliseDacFullScale_t;

/**
 *  \brief Enum to set the Tx Atenuation step size
 */
typedef enum {
	TAL_TXATTEN_0P05_DB = 0,    /*!< Tx attenuation 0.05dB step size */
	TAL_TXATTEN_0P1_DB = 1,     /*!< Tx attenuation 0.1dB step size */
	TAL_TXATTEN_0P2_DB = 2,     /*!< Tx attenuation 0.2dB step size */
	TAL_TXATTEN_0P4_DB = 3      /*!< Tx attenuation 0.4dB step size */
} taliseTxAttenStepSize_t;

/**
 *  \brief Data structure to hold Talise Tx NCO test tone Configuration
 */
typedef struct {
	uint8_t enable;                 /*!< 0 = Disable Tx NCO, 1 = Enable Tx NCO on both transmitters */
	int32_t tx1ToneFreq_kHz;        /*!< Signed frequency in kHz of the desired Tx1 tone */
	int32_t tx2ToneFreq_kHz;        /*!< Signed frequency in kHz of the desired Tx2 tone */
} taliseTxNcoTestToneCfg_t;

/**
 *  \brief Enum to set the Tx Atenuation step size
 */
typedef enum {
	TAL_TXNCO_DISABLE = 0,        /*!< TX NCO Disable */
	TAL_TXNCO_TX1_ENABLE = 1,     /*!< TX NCO Tx1 Enable */
	TAL_TXNCO_TX2_ENABLE = 2,     /*!< TX NCO Tx2 Enable */
	TAL_TXNCO_ENABLE_ALL = 3      /*!< TX NCO Tx1 & Tx2 Enable */
} taliseTxNCOSelect_t;

/**
 *  \brief Data structure to hold Talise Tx NCO Configuration (tone configuration, enable/disable and tone phase configuration)
 */
typedef struct {
	taliseTxNCOSelect_t
	enableNCO; /* < 0 = Disable NCO, 1 = Enable Tx1 NCO, 2 = Enable Tx2 NCO, */
	int32_t tx1ToneFreq_kHz; /*!< Signed frequency in kHz of the desired Tx1 tone */
	int32_t tx2ToneFreq_kHz; /*!< Signed frequency in kHz of the desired Tx2 tone */
	int32_t tx1TonePhaseDeg;                         /*!< Tx1 Tone Phase degree */
	int32_t tx2TonePhaseDeg;                         /*!< Tx2 Tone Phase degree */
} taliseTxNcoShifterCfg_t;


/**
 *  \brief Data structure to hold Talise Tx Attenuation Control Pin Configuration
 */
typedef struct {
	uint8_t stepSize;                   /*!< The step that will increase or decrease the channel attenuation. This parameter sets the
                                                 change in Tx attenuation for each increment or decrement signal received in incr/decr mode.
                                                 Step of 1 changes attenuation by 0.05dB. Valid range is from 0 to 31 */
	taliseGpioPinSel_t txAttenIncPin;   /*!< GPIO used to increment Tx attenuation
                                                 Tx1 : TAL_GPIO_04 or TAL_GPIO_12
                                                 Tx2 : TAL_GPIO_06 or TAL_GPIO_14 */
	taliseGpioPinSel_t txAttenDecPin;    /*!< GPIO used to decrement Tx attenuation
                                                 Tx1 : TAL_GPIO_05 or TAL_GPIO_13
                                                 Tx2 : TAL_GPIO_07 or TAL_GPIO_15 */
	uint8_t enable;                     /*!< Enable (1) or disable (0) the attenuation pin control */
} taliseTxAttenCtrlPin_t;

/**
 *  \brief Data structure to hold Talise Tx PA Protection configuration settings
 */
typedef struct {
	uint8_t avgDuration;          /*!< Number of Tx samples (at JESD204 IQ sample rate) to average for the power measurement.
                                           samples = 2^(avgDuration + 5), 0 = 32 samples, max:14 = 524288 samples */
	uint8_t txAttenStep;          /*!< if PA protection threshold met, Tx Atten = TxAttenSetting + (txAttenStep * 0.4dB) */
	uint16_t tx1PowerThreshold;   /*!< tx1PowerThreashold = round(4096 * 10^(tx1PowerThreshold_dBFS / 10)) (valid 1-8191) */
	uint16_t tx2PowerThreshold;   /*!< tx2PowerThreashold = round(4096 * 10^(tx2PowerThreshold_dBFS / 10)) (valid 1-8191) */

	uint8_t peakCount;            /*!< 0=Peak Mode is disabled, if the Tx peak power threshold is exceeded more than peakCount times within
                                           one average duration, a PA error is flagged (Si Rev 0xB1: 0-30, Si Rev 0xC0: 0-31) */
	uint8_t tx1PeakThreshold;     /*!< 8-bit threshold for Tx1 peak detect. When instantaneous power exceeds this threshold, a peak is registered (valid 1-255)
                                           tx1PeakThreshold = round(128 * 10^(tx1PeakThreshold_dBFS / 10)) */
	uint8_t tx2PeakThreshold;     /*!< 8-bit threshold for Tx2 peak detect. When instantaneous power exceeds this threshold, a peak is registered (valid 1-255)
                                           tx2PeakThreashold = round(128 * 10^(tx2PeakThreshold_dBFS / 10))*/
} taliseTxPaProtectCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* TALISE_TX_TYPES_H_ */
