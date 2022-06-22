// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_radioctrl_types.h
 * \brief Contains Talise API Radio Control data types
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_RADIOCTRL_TYPES_H_
#define TALISE_RADIOCTRL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Enum to select desired low voltage GPIO pin used by the API
 */
typedef enum {
	TAL_GPIO_00 = 0,
	TAL_GPIO_01,
	TAL_GPIO_02,
	TAL_GPIO_03,
	TAL_GPIO_04,
	TAL_GPIO_05,
	TAL_GPIO_06,
	TAL_GPIO_07,
	TAL_GPIO_08,
	TAL_GPIO_09,
	TAL_GPIO_10,
	TAL_GPIO_11,
	TAL_GPIO_12,
	TAL_GPIO_13,
	TAL_GPIO_14,
	TAL_GPIO_15,
	TAL_GPIO_16,
	TAL_GPIO_17,
	TAL_GPIO_18,
	TAL_GPIO_INVALID

} taliseGpioPinSel_t;

/**
 *  \brief Enum to select desired External LO output frequency = RFPLL VCO / 2 / ExtLoDiv
 *  Where ExtLoDiv is a power of 2 from (1, 2, 4, 8, 16, 32, 64)
 */
typedef enum {
	TAL_EXTLO_RFPLLVCO_DIV2 =  0,  /*!< External LO output in frequency range 3000 MHz    - 6000 MHz   */
	TAL_EXTLO_RFPLLVCO_DIV4 =  1,  /*!< External LO output in frequency range 1500 MHz    - 3000 MHz   */
	TAL_EXTLO_RFPLLVCO_DIV8 =  2,  /*!< External LO output in frequency range  750 MHz    - 1500 MHz   */
	TAL_EXTLO_RFPLLVCO_DIV16 = 3,  /*!< External LO output in frequency range  375 Mhz    -  750 MHz   */
	TAL_EXTLO_RFPLLVCO_DIV32 = 4,  /*!< External LO output in frequency range  187.5 MHz  -  375 MHz   */
	TAL_EXTLO_RFPLLVCO_DIV64 = 5   /*!< External LO output in frequency range   93.75 MHz -  187.5 MHz */
} taliseExtLoDiv_t;
/**
 * \brief Data structure to hold ARM GPIO pin assignments, polarity, and pin enable for the Talise ARM
 */
typedef struct {
	taliseGpioPinSel_t
	gpioPinSel; /*!< Select desired GPIO pin to input into Talise (valid 0-15) */
	uint8_t polarity;           /*!< Signal polarity (0 = Normal polarity, 1=Talise will invert the signal before using) */
	uint8_t enable;             /*!< 1 = Enable Talise ARM use of GPIO signal, 0 = Talise ARM uses ARM command to set this signal value */

} taliseArmGpioPinSettings_t;


/**
 * \brief Data structure to hold ARM GPIO pin assignments for each ARM input/output pin.
 */
typedef struct {
	/* Talise ARM input GPIO pins -- Only valid if orxPinMode = 1 */
	taliseArmGpioPinSettings_t
	orx1TxSel0Pin;      /*!< Select desired GPIO pin to input into Talise(valid 0-15), polarity, enable */
	taliseArmGpioPinSettings_t
	orx1TxSel1Pin;      /*!< Select desired GPIO pin to input into Talise(valid 0-15), polarity, enable */
	taliseArmGpioPinSettings_t
	orx2TxSel0Pin;      /*!< Select desired GPIO pin to input into Talise(valid 0-15), polarity, enable */
	taliseArmGpioPinSettings_t
	orx2TxSel1Pin;      /*!< Select desired GPIO pin to input into Talise(valid 0-15), polarity, enable */
	taliseArmGpioPinSettings_t
	enTxTrackingCals;   /*!< Select desired GPIO pin to input into Talise(valid 0-15), polarity, enable */

} taliseArmGpioConfig_t;

/**
 * \brief Enumerated list of Radio Control Config2 ORx1/ORx2 GPIO pin pair settings
 */
typedef enum {
	TAL_ORX1ORX2_PAIR_01_SEL = 0x00,   /*!< Radio Control Config 2 ORx1/ORx2 GPIO '0,1' pin pair select */
	TAL_ORX1ORX2_PAIR_45_SEL,          /*!< Radio Control Config 2 ORx1/ORx2 GPIO '4,5' pin pair select */
	TAL_ORX1ORX2_PAIR_89_SEL,          /*!< Radio Control Config 2 ORx1/ORx2 GPIO '8,9' pin pair select */
	TAL_ORX1ORX2_PAIR_NONE_SEL         /*!< Radio Control Config 2 ORx1/ORx2 GPIO 'none' pin pair select */

} taliseRadioCtlCfg2_t;

/**
 * \brief Enumerated list of Radio Control Config1 register bits
 */
typedef enum {
	TAL_TXRX_PIN_MODE = 0x01,       /*!< Radio Control Config 1 bit '0' mask */
	TAL_ORX_PIN_MODE = 0x02,        /*!< Radio Control Config 1 bit '1' mask */
	TAL_ORX_USES_RX_PINS = 0x04,    /*!< Radio Control Config 1 bit '2' mask */
	TAL_ORX_SEL = 0x10,             /*!< Radio Control Config 1 bit '4' mask */
	TAL_ORX_SINGLE_CHANNEL = 0x20,  /*!< Radio Control Config 1 bit '5' mask */
	TAL_ORX_ENAB_SEL_PIN = 0x40     /*!< Radio Control Config 1 bit '6' mask */
} taliseRadioCtlCfg1_t;

/**
 *  \brief Enum of possible Rx and ORx stream processor enables to be used with
 *         the TALISE_setRxTxEnable() function
 */
typedef enum {
	TAL_RXOFF_EN    = 0x00,  /*!< All Rx/ORx channels off */
	TAL_RX1_EN      = 0x01,  /*!< Rx1 channel enabled */
	TAL_RX2_EN      = 0x02,  /*!< Rx2 channel enabled */
	TAL_RX1RX2_EN   = 0x03,  /*!< Rx1 + Rx2 channels enabled */
	TAL_ORX1_EN     = 0x04,  /*!< ORx1 channel enabled */
	TAL_ORX2_EN     = 0x08,  /*!< ORx2 channel enabled */
	TAL_ORX1ORX2_EN = 0x0C	 /*!< ORx1 and ORx2 channels enabled - only allowed if ADC stitching is not enabled */

} taliseRxORxChannels_t;

/**
 * \brief Enumerated list of Tx to ORx mapping selections
 */
typedef enum {
	TAL_MAP_NONE = 0,           /*!< No Tx to ORx mapping select */
	TAL_MAP_TX1_ORX = 0x02,     /*!< Tx1 to ORx mapping select */
	TAL_MAP_TX2_ORX = 0x03      /*!< Tx2 to ORx mapping select */

} taliseTxToOrxMapping_t;

/**
 *  \brief Enum of PLL selections
 */
typedef enum {
	TAL_CLK_PLL = 0,                /*!< Selects CLK PLL for Rx and Tx */
	TAL_RF_PLL,                     /*!< Selects RF PLL for Rx and Tx */
	TAL_AUX_PLL                     /*!< Selects AUX PLL for Rx and tx*/

} taliseRfPllName_t;

/**
 * \brief Structure to setup/enable ORx LO Selection feature
 */
typedef struct {
	uint8_t disableAuxPllRelocking; /*!< Disables the ARM from automatically relocking the Aux PLL.
                                         Set to 1 when using AuxLO as ORx LO source, 0 = default when RFPLL used as ORx LO source */
	taliseGpioPinSel_t
	gpioSelect;  /*!< TAL_GPIO_INVALID = disable pin mode, GPIO0-15 valid */
} taliseOrxLoCfg_t;

/**
 *  \brief Enum of FHM Exit Mode Selections
 */
typedef enum {
	TAL_FHM_QUICK_EXIT = 0,  /*!< Selects quick exit mode on frequency hopping disable. In this case RF PLL bandwidth is left unchanded */
	TAL_FHM_FULL_EXIT        /*!< Selects full exit mode on frequency hopping disable. RF PLL Loop B/W is restored to narrowband.
                                  RF and Aux PLLs recalibrated and tracking cals resumed. */
} taliseFhmExitMode_t;

/**
 *  \brief Enum of FHM Trigger Mode Selections
 */
typedef enum {
	TAL_FHM_GPIO_MODE = 0,       /*!< Selects FHM trigger mode as GPIO. A low to high pulse triggers frequency hop */
	TAL_FHM_NON_GPIO_MODE,       /*!< Selects FHM trigger mode as non-GPIO. An ARM command triggers frequency hop */
	TAL_FHM_INVALID_TRIGGER_MODE
} taliseFhmTriggerMode_t;

/**
 * \brief Structure to setup talise frequency hopping config
 */
typedef struct {
	taliseGpioPinSel_t
	fhmGpioPin; /*!< Maps the Talise ARM GPIO pin(TAL_GPIO_0 - TAL_GPIO_15) for frequency hopping. A low to high pulse on this pin triggers freq hopping
                                        Setting fhmGpioPin = TAL_GPIO_INVALID will unassign ARM GPIO pin mapped to Rf Pll Frequency hopping*/
	uint32_t fhmMinFreq_MHz;       /*!< Sets frequency hopping range minimum frequency */
	uint32_t fhmMaxFreq_MHz;       /*!< Sets frequency hopping range maximum frequency */
} taliseFhmConfig_t;

/**
 * \brief Structure to setup talise frequency hopping mode settings
 */
typedef struct {
	uint8_t fhmEnable;                      /*!< 0 - Disables Frequency Hopping, 1 - Enables Frequency Hopping */
	uint8_t enableMcsSync;                  /*!< 0 - Disables MCS Synchronization on FHM enable, 1 - Enables MCS Synchronization on FHM enable. Ignored if fhmEnable = 0 */
	taliseFhmTriggerMode_t
	fhmTriggerMode;  /*!< TAL_FHM_GPIO_MODE - Frequency Hop triggered via GPIO low to high pulse
                                                 TAL_FHM_NON_GPIO_MODE - Frequency Hop triggered via ARM command*/
	taliseFhmExitMode_t
	fhmExitMode;        /*!< TAL_FHM_QUICK_EXIT = quick exit on frequency hopping disable,
                                                 TAL_FHM_FULL_EXIT = Full exit on frequency hopping disable. This is ignored if fhmEnable = 1*/
	uint64_t fhmInitFrequency_Hz;           /*!< First hop frequency that Rf Pll is configured to on enabling FHM */
} taliseFhmMode_t;

/**
 * \brief Structure to read talise frequency hopping mode status
 */
typedef struct {
	uint16_t currentFhmCmdErrorStatus;     /*!< Current FHM Enter Command Error Status. This is same as the mailbox command error status for FHM */
	uint16_t currentFhmHopErrorStatus;     /*!< Currently active FHM errors during frequency hopping */
	uint32_t numFhmHops;                   /*!< Total no. of Hops since entering FHM */
	uint32_t numFhmNoErrorEvents;          /*!< Total no. of NO FHM error events */
	uint64_t lastFhmNoErrorFreq_Hz;        /*!< Last frequency for which NO Error was encountered */
	uint32_t numFhmHopsOutsideScanRange;   /*!< Total no. of Hops outside FHM scan range */
	uint64_t lastFreqOutsideScanRange_Hz;  /*!< Last frequency which was outside FHM scan range*/
	uint32_t numInvalidFhmHopFrequencies;  /*!< Invalid Hop Freq */
	uint64_t lastInvalidHopFreq_Hz;        /*!< Last Invalid Hop Freq */
	uint32_t compPllError;                 /*!< PLL LO Computation Error */
	uint64_t compPllErrorFreq_Hz;          /*!< PLL LO Computation Error frequency */
	uint32_t rfPllLockFailed;              /*!< RF PLL Lock failed */
	uint64_t rfPllLockFailedFreq_Hz;       /*!< RF PLL Lock failed frequency*/
} taliseFhmStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* TALISE_RADIOCTRL_TYPES_H_ */
