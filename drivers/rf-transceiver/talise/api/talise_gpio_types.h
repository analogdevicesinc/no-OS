// SPDX-License-Identifier: GPL-2.0
/**
 * \file talise_gpio_types.h
 * \brief Contains functions to allow control of the General Purpose IO functions on the Talise device
 *
 * Talise API version: 3.6.2.1
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef TALISE_GPIO_TYPES_H_
#define TALISE_GPIO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Enum to set the low voltage GPIO mode
 */
typedef enum {
	TAL_GPIO_MONITOR_MODE = 0,      /*!< Allows a choice of debug signals to output from Talise to monitor the state of the device */
	TAL_GPIO_BITBANG_MODE = 3,      /*!< Manual mode, API function sets output pin levels and reads input pin levels */
	TAL_GPIO_ARM_OUT_MODE = 9,      /*!< Allows internal ARM processor to output on GPIO pins */
	TAL_GPIO_SLICER_OUT_MODE = 10   /*!< Allows Slicer active configuration to the GPIO output  pins */
} taliseGpioMode_t;

/**
 *  \brief Enum to set the GPIO3v3 mode
 */
typedef enum {
	TAL_GPIO3V3_LEVELTRANSLATE_MODE     = 1,    /*!< Level translate mode, signal level on low voltage GPIO output on GPIO3v3 pins */
	TAL_GPIO3V3_INVLEVELTRANSLATE_MODE  = 2,    /*!< Inverted Level translate mode, inverse of signal level on low voltage GPIO output on GPIO3v3 pins */
	TAL_GPIO3V3_BITBANG_MODE            = 3,    /*!< Manual mode, API function sets output pin levels and reads input pin levels */
	TAL_GPIO3V3_EXTATTEN_LUT_MODE       = 4     /*!< GPIO3v3 output level follows Rx1/Rx2 gain table external control 6bit field. */
} taliseGpio3v3Mode_t;

/**
 *  \brief Enum of unique error codes for the Talise GPIO API functions.
 * Each error condition in the library should get its own enum value to allow
 * easy debug of errors.
 */
typedef enum {
	TALISE_ERR_GPIO_OK = 0,
	TALISE_ERR_MONITOR_OUT_INDEX_RANGE,
	TALISE_ERR_GETGPIOMON_INDEX_NULL_PARM,
	TALISE_ERR_GETGPIOMON_MONITORMASK_NULL_PARM,
	TALISE_ERR_GETGPIO_OE_NULL_PARM,
	TALISE_ERR_GPIO_OE_INV_PARM,
	TALISE_ERR_GPIO_SRC_INV_PARM,
	TALISE_ERR_GETGPIO_SRC_NULL_PARM,
	TALISE_ERR_GPIO_LEVEL_INV_PARM,
	TALISE_ERR_GETGPIO_LEVEL_NULL_PARM,
	TALISE_ERR_GETGPIO_SETLEVEL_NULL_PARM,
	TALISE_ERR_SETUPAUXDAC_NULL_PARM,
	TALISE_ERR_SETUPAUXDAC_INV_10BIT_AUXDACCODE,
	TALISE_ERR_SETUPAUXDAC_INV_12BIT_AUXDACCODE,
	TALISE_ERR_WRITEAUXDAC_INV_10BIT_AUXDACCODE,
	TALISE_ERR_WRITEAUXDAC_INV_12BIT_AUXDACCODE,
	TALISE_ERR_WRITEAUXDAC_INV_AUXDACINDEX,
	TALISE_ERR_SETUPAUXDAC_INV_RESOLUTION,
	TALISE_ERR_GPIO3V3_OE_INV_PARM,
	TALISE_ERR_GETGPIO3V3_OE_NULL_PARM,
	TALISE_ERR_GPIO3V3_SRC_INV_PARM,
	TALISE_ERR_GETGPIO3V3_SRC_NULL_PARM,
	TALISE_ERR_GPIO3V3_LEVEL_INV_PARM,
	TALISE_ERR_GETGPIO3V3_LEVEL_NULL_PARM,
	TALISE_ERR_GETGPIO3V3_SETLEVEL_NULL_PARM,
	TALISE_ERR_GPINT_OK,
	TALISE_ERR_GPINT_STATUS_NULL_PARM,
	TALISE_ERR_GPINT_GPINTDIAG_NULL_PARM,
	TALISE_ERR_GPINT_NO_SOURCE_FOUND,
	TALISE_ERR_GPINT_SOURCE_NOT_IMPLEMENTED,
	TALISE_ERR_GPINT_CLKPLL_UNLOCKED,
	TALISE_ERR_GPINT_RFPLL_UNLOCKED,
	TALISE_ERR_GPINT_AUXPLL_UNLOCKED,
	TALISE_ERR_GPINT_ARM_WATCHDOG_TIMEOUT,
	TALISE_ERR_GPINT_ARM_FORCE_GPINT,
	TALISE_ERR_GPINT_ARM_SYSTEM_ERROR,
	TALISE_ERR_GPINT_ARM_DATA_PARITY_ERROR,
	TALISE_ERR_GPINT_ARM_PROG_PARITY_ERROR,
	TALISE_ERR_GPINT_ARM_CALIBRATION_ERROR,
	TALISE_ERR_GPINT_FRAMERA,
	TALISE_ERR_GPINT_DEFRAMERA,
	TALISE_ERR_GPINT_FRAMERB,
	TALISE_ERR_GPINT_DEFRAMERB,
	TALISE_ERR_GPINT_PA_PROTECT_CH1,
	TALISE_ERR_GPINT_PA_PROTECT_CH2,
	TALISE_ERR_GPINT_STREAM_ERROR,
	TALISE_ERR_SETAUXADCPINMODEGPIO_INV_GPIO,
	TALISE_ERR_SETAUXADCPINMODEGPIO_GPIO_IN_USE,
	TALISE_ERR_STARTAUXADC_INV_CHANNEL,
	TALISE_ERR_STARTAUXADC_INV_MODE,
	TALISE_ERR_STARTAUXADC_INV_NUM_SAMPLES,
	TALISE_ERR_STARTAUXADC_INV_SAMPLING_PERIOD,
	TALISE_ERR_STARTAUXADC_NULL_PARAM,
	TALISE_ERR_READAUXADC_NULL_PARAM,
	TAL_ERR_GPIO_NUMBER_OF_ERRORS /* Keep this ENUM last as a reference to the total number of error enum values */
} taliseGpioErr_t;

/**
 * \brief Enumeration for 10bit AuxDAC voltage for center DAC code (code 512).
 */
typedef enum {
	TAL_AUXDACVREF_1V = 0, /*!< AuxDAC reference at 1V */
	TAL_AUXDACVREF_1P5V = 1, /*!< AuxDAC reference at 1.5V */
	TAL_AUXDACVREF_2V = 2, /*!< AuxDAC reference at 2V */
	TAL_AUXDACVREF_2P5V = 3 /*!< AuxDAC reference at 2.5V */
} taliseAuxDacVref_t;

/**
 * \brief Enumeration for AuxDAC resolution modes.
 */
typedef enum {
	TAL_AUXDACRES_12BIT = 0, /*!< 12bit DAC resolution for a subset of the output voltage range centered around VREF */
	TAL_AUXDACRES_11BIT = 1, /*!< 11bit DAC resolution for a subset of the output voltage range centered around VREF */
	TAL_AUXDACRES_10BIT = 2  /*!< 10bit DAC resolution for 100mv to 3v range */
} taliseAuxDacResolution_t;

/**
 * \brief Data structure to hold the auxiliary DAC settings
 */
typedef struct {
	uint16_t
	auxDacEnables;         /*!< Aux DAC enable bit for each DAC, where the first ten bits correspond to the 10-bit DACs, and the next consecutive two bits enable the 12-bit DACs */
	taliseAuxDacVref_t
	auxDacVref[10];        /*!< Aux DAC voltage reference value for each of the 10-bit DACs */
	taliseAuxDacResolution_t
	auxDacResolution[10];  /*!< Aux DAC slope (resolution of voltage change per AuxDAC code) - only applies to 10bit DACs (0-9) */
	uint16_t
	auxDacValues[12];      /*!< Aux DAC values for each 10-bit DAC correspond to the first 10 array elements, the next consecutive array elements correspond to the two 12-bit DAC values */
} taliseAuxDac_t;

/**
 * \brief General Purpose interrupt mask types
 */
typedef enum {
	TAL_GP_MASK_STREAM_ERROR            = 0x1000,   /*!< Stream processor error GP Interrupt mask bit */
	TAL_GP_MASK_ARM_CALIBRATION_ERROR   = 0x0800,   /*!< ARM calibration error GP Interrupt mask bit */
	TAL_GP_MASK_ARM_SYSTEM_ERROR        = 0x0400,   /*!< ARM System error GP Interrupt mask bit */
	TAL_GP_MASK_ARM_FORCE_INTERRUPT     = 0x0200,   /*!< ARM force GP Interrupt mask bit */
	TAL_GP_MASK_WATCHDOG_TIMEOUT        = 0x0100,   /*!< Watchdog GP Interrupt mask bit */
	TAL_GP_MASK_PA_PROTECTION_TX2_ERROR = 0x0080,   /*!< Tx2 PA protection error GP Interrupt mask bit */
	TAL_GP_MASK_PA_PROTECTION_TX1_ERROR = 0x0040,   /*!< Tx1 PA protection error GP Interrupt mask bit */
	TAL_GP_MASK_JESD_DEFRMER_IRQ        = 0x0020,   /*!< JESD204B Deframer IRQ error GP Interrupt mask bit */
	TAL_GP_MASK_JESD_FRAMER_IRQ         = 0x0010,   /*!< JESD204B Framer IRQ error GP Interrupt mask bit */
	TAL_GP_MASK_CLK_SYNTH_UNLOCK        = 0x0008,   /*!< Device clock PLL non-lock error GP Interrupt mask bit */
	TAL_GP_MASK_AUX_SYNTH_UNLOCK        = 0x0004,   /*!< Auxiliary PLL non-lock error GP Interrupt mask bit */
	TAL_GP_MASK_RF_SYNTH_UNLOCK         = 0x0002    /*!< RF PLL non-lock error GP Interrupt mask bit */
} taliseGpIntMask_t;

#define TAL_GPMASK_MSB (uint16_t)(TAL_GP_MASK_STREAM_ERROR | \
                                  TAL_GP_MASK_ARM_CALIBRATION_ERROR | \
                                  TAL_GP_MASK_ARM_SYSTEM_ERROR | \
                                  TAL_GP_MASK_ARM_FORCE_INTERRUPT | \
                                  TAL_GP_MASK_WATCHDOG_TIMEOUT)

#define TAL_GPMASK_LSB (uint16_t)(TAL_GP_MASK_PA_PROTECTION_TX1_ERROR | \
                                  TAL_GP_MASK_PA_PROTECTION_TX2_ERROR | \
                                  TAL_GP_MASK_JESD_DEFRMER_IRQ | \
                                  TAL_GP_MASK_JESD_FRAMER_IRQ | \
                                  TAL_GP_MASK_CLK_SYNTH_UNLOCK | \
                                  TAL_GP_MASK_AUX_SYNTH_UNLOCK | \
                                  TAL_GP_MASK_RF_SYNTH_UNLOCK)
/**
 * \brief GPIO settings for SPI2 TxAtten select
 */
typedef enum {
	TAL_SPI2_TXATTEN_GPIO4   = 0x00,    /*!< Select GPIO4 for SPI2 Tx Attenuation select */
	TAL_SPI2_TXATTEN_GPIO8   = 0x01,    /*!< Select GPIO8 for SPI2 Tx Attenuation select */
	TAL_SPI2_TXATTEN_GPIO14  = 0x02,    /*!< Select GPIO14 for SPI2 Tx Attenuation select */
	TAL_SPI2_TXATTEN_DISABLE = 0x03     /*!< Disable GPIO for SPI2 Tx Attenuation select */
} taliseSpi2TxAttenGpioSel_t;


/**
 * \brief gpIntHandler diagnostic structure
 */
typedef struct {
	uint8_t data[9];                /*!< All GP_INT sources */
	taliseFramerSel_t
	framer;       /*!< Interrupting framer, valid only for framer sources */
	taliseDeframerSel_t
	deframer;   /*!< Interrupting deframer, valid only for deframer sources */
	int32_t deframerInputsMask;     /*!< Interrupting deframer input mask (bit per deframer input), valid only for deframer sources (valid 0x0-0xF)
                                         deframerInputsMask is the deframer lane after the deframer lane crossbar swapping (lane input of the deframer) */
} taliseGpIntInformation_t;

/**
 * \brief Talise Aux ADC Channel types
 */
typedef enum {
	TAL_AUXADC_CH0 = 0,  /*!< Select Aux ADC Channel 0 for sampling and conversion*/
	TAL_AUXADC_CH1 = 1,  /*!< Select Aux ADC Channel 1 for sampling and conversion*/
	TAL_AUXADC_CH2 = 2,  /*!< Select Aux ADC Channel 2 for sampling and conversion*/
	TAL_AUXADC_CH3 = 3   /*!< Select Aux ADC Channel 3 for sampling and conversion*/
} taliseAuxAdcChannels_t;

/**
 * \brief Talise Aux ADC modes for sampling
 */
typedef enum {
	TAL_AUXADC_NONPIN_MODE = 0,  /*!< Select Aux ADC sampling and conversion in Non-Pin mode (ARM Internal timer is used for sampling and conversion)*/
	TAL_AUXADC_PIN_MODE    = 1   /*!< Select Aux ADC sampling and conversion in Pin mode (Pulses on ARM GPIO Input pins are used to schedule sampling and conversion)*/
} taliseAuxAdcModes_t;

/**
 * \brief Talise Aux ADC configuration structure
 */
typedef struct {
	taliseAuxAdcChannels_t
	auxAdcChannelSel;  /*!< Selects the channel which is supposed to sample AuxADC input for A/D conversion */
	taliseAuxAdcModes_t
	auxAdcMode;        /*!< Selects mode to latch and store conversion results */
	uint16_t
	numSamples;        /*!< No. of A/D conversions to be performed in range 1 - 1000 */
	uint16_t
	samplingPeriod_us; /*!< Sampling interval time in microseconds (Minimum 15us) NOTE: Valid only for non pin mode. Ignored for pin mode. */
} taliseAuxAdcConfig_t;

/**
 * \brief Talise Aux ADC conversion result structure
 */
typedef struct {
	uint16_t auxAdcCodeAvg;     /*!< 12-bit Average of AuxADC A/D conversion samples */
	uint16_t numSamples;        /*!< No. of samples averaged in AuxAdcCodeAvg */
	uint8_t  completeIndicator; /*!< Flag to indicate if a scheduled AuxADC conversion completed. 1 - AuxADC Conversion Complete, 0 - AuxADC Conversion Incomplete */
} taliseAuxAdcResult_t;


#ifdef __cplusplus
}
#endif

#endif /* TALISE_GPIO_TYPES_H_ */
