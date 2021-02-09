/**
* \file
* \brief Contains ADRV9001 API Tx datapath data types
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the FPGA9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_TX_TYPES_H_
#define _ADI_ADRV9001_TX_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdbool.h>
#endif

#include "adi_adrv9001_auxdac_types.h"
#include "adi_adrv9001_gpio_types.h"

#define ADRV9001_TX_ATTEN_TABLE_MAX 960

#define ADI_ADRV9001_MAX_TXCHANNELS           2
#define ADI_ADRV9001_MAX_TX_CHANNEL_START     0
#define ADI_ADRV9001_MAX_TX_CHANNEL_END       1

#define ADRV9001_ADDR_TX1_ATTEN_TABLE 0x45300000
#define ADRV9001_ADDR_TX2_ATTEN_TABLE 0x45400000

#define ADRV9001_TX_PA_RAMP_LUT_SIZE        256

/**
*  \brief Enum to set the Tx Attenuation step size
*/
typedef enum adi_adrv9001_TxAttenStepSize
{
    ADI_ADRV9001_TXATTEN_0P05_DB = 0,    /*!< Tx attenuation 0.05dB step size */
    ADI_ADRV9001_TXATTEN_0P1_DB  = 1,    /*!< Tx attenuation 0.1dB step size */
    ADI_ADRV9001_TXATTEN_0P2_DB  = 2,    /*!< Tx attenuation 0.2dB step size */
    ADI_ADRV9001_TXATTEN_0P4_DB  = 3     /*!< Tx attenuation 0.4dB step size */
} adi_adrv9001_TxAttenStepSize_e;

/**
*  \brief Enum for the different attenuation mode
*/
typedef enum adi_adrv9001_TxAttenuationControlMode
{
    ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_BYPASS  = 0, /*!< Tx attenuation mode Bypass: zero total attenuation */
    ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI     = 1, /*!< Tx attenuation mode set by 10-bit attenuation index used to determine total attenuation */
    ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_PIN     = 3, /*!< Tx attenuation is control with GPIO Incr/Decr: total attenuation is altered incrementally using pin control */
} adi_adrv9001_TxAttenuationControlMode_e;

/**
 *  \brief Enum select for slew rate limiter statistics mode
 */
typedef enum adi_adrv9001_SrlStatisticsMode
{
    ADI_ADRV9001_SRL_STATISTICS_MIN_SLEW_FACTOR_OBSERVED = 0u,
    ADI_ADRV9001_SRL_STATISTICS_NUM_OF_SAMPLES_SLEW_RATE_LIMITED,
    ADI_ADRV9001_SRL_STATISTICS_CLEAR_TO_ZERO_10,     /* clear statistics to zero, same as b11 below */
    ADI_ADRV9001_SRL_STATISTICS_CLEAR_TO_ZERO_11      /* clear statistics to zero, same as b10 above */
} adi_adrv9001_SrlStatisticsMode_e;

/**
 *  \brief Enum select for slew rate table
 */
typedef enum adi_adrv9001_SrlTableSel
{
    ADI_ADRV9001_SRL_TABLE0 = 0x0000,        /*!< Slew limit is 10% of full scale */
    ADI_ADRV9001_SRL_TABLE1 = 0x0001,        /*!< Slew limit is 20% of full scale */
    ADI_ADRV9001_SRL_TABLE2 = 0x0002,        /*!< Slew limit is 30% of full scale */
    ADI_ADRV9001_SRL_TABLE3 = 0x0003         /*!< Slew limit is 50% of full scale */
} adi_adrv9001_SrlTableSel_e;

/**
*  \brief Enum select for PA protection input
*/
typedef enum adi_adrv9001_PaProtectionInputSel
{
    ADI_ADRV9001_COMPLEX_MULT_OUTPUT   = 0x0000, /*!< Input data to PA protection block is probed from complex mult output */
    ADI_ADRV9001_TXQEC_ACTUATOR_OUTPUT = 0x0001  /*!< Input data to PA protection block is probed from tx qec actuator output */
} adi_adrv9001_PaProtectionInputSel_e;

/**
 * \brief Configuration structure for Tx power control
 */
typedef struct adi_adrv9001_TxAttenuationConfig_t
{
    bool disableTxOnPllUnlock;                          /*!< If true, the PA will ramp down to the max attenuation if
                                                         *   an RF1 or RF2 PLL unlock occurs
                                                         *   NOTE: Currently read-only */
    adi_adrv9001_TxAttenStepSize_e txAttenStepSize;	    /*!< Tx Attenuation step size */
    adi_adrv9001_TxAttenuationControlMode_e attenMode;  /*!< The mode to control Tx attenuation */
} adi_adrv9001_TxAttenuationConfig_t;

/**
* \brief Data structure to hold an adrv9001 device Tx attenuation Table Row
*        (Single row of a complete table)
*/
typedef struct adi_adrv9001_TxAttenTableRow
{
    uint16_t txAttenMult;       /*!< Digital attenuation multiplier; Valid range: 0-4095 */
    uint8_t  txAttenHp;         /*!< Analog coarse attenuation gain; Valid range: 0-63 */
    uint8_t  Reserve;
} adi_adrv9001_TxAttenTableRow_t;

/**
*  \brief Data structure to hold ADRV9001 Tx PA Protection configuration settings
*/
typedef struct adi_adrv9001_TxPaProtectCfg
{
    uint8_t avgDuration;                /*!< PA Protection Average Power Measurement Duration. */
    uint8_t peakDuration;               /*!< PA Protection Peak Power Measurement Duration */
    uint8_t txAttenStep;                /*!< PA Protection Attenuation gain step. Gain step down is not allowed for Tokelau device. This field is not being used actively. */
    bool gainStepDownEn;                /*!< PA Protection Gain Step Down Enable. Gain step down is not allowed for Tokelau device. This field is not being used actively.*/
    uint16_t powerThreshold;            /*!< PA Protection Average Power Threshold. */
    uint16_t peakThreshold;             /*!< PA Protection Peak Power Threshold (Valid: 0 to 8191) */
    uint8_t peakCount;                  /*!< Peak Count Causing PA Error. */
    uint8_t rampStepDuration;           /*!< PA Protection Ramp Step duration. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    uint8_t rampStepSize;               /*!< PA Protection Ramp Step Size. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    uint8_t rampMaxAtten;               /*!< PA Protection Ramp Max Attenuation. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    bool avgPowerEnable;                /*!< This enables average power measurement block. If enabled, PA error is flagged when average power measurement is above average power threshold */
    bool peakPowerEnable;               /*!< This enables peak power measurement block. If enabled, PA error is flagged when peak power count is above peak count threshold */
    bool avgPeakRatioEnable; /*!< This enables average to peak power ratio calculation block, both avgPower and peakPower calculations must be enabled before enabling ratio calculation */
    adi_adrv9001_PaProtectionInputSel_e inputSel; /*!< This selects the source of input signal for Pa protection block */
} adi_adrv9001_TxPaProtectCfg_t;

/**
 *  \brief Data structure to hold ADRV9001 Tx Slew Rate Limiter Configuration
 */
typedef struct adi_adrv9001_SlewRateLimiterCfg
{
    bool srlEnable;                                         /*!< default = true */
    bool srlStatisticsEnable;                               /*!< default = true */
    adi_adrv9001_SrlTableSel_e srlTableSelect;              /*!< default = adi_adrv9001_srl_table3 */
    uint8_t srlSlewOffset;                                  /*!< 0 (default), 1, . . ., 15 */
    adi_adrv9001_SrlStatisticsMode_e srlStatisticsMode;     /*!< default = adi_adrv9001_srl_statistics_clear_to_zero */
} adi_adrv9001_SlewRateLimiterCfg_t;

/**
 *  \brief Data structure to hold ADRV9001 Tx PA ramp configuration
 */
typedef struct adi_adrv9001_PaRampCfg
{
    bool     enable;                                   /*!< PA Ramp enable; True = Enable PA ramp of Tx channel; False = Disable */
    bool     gpioTriggerSelect;                        /*!< Source of the enable signal. True: GPIO triggered; False: SPI triggered */
    uint16_t rampClock_kHz;                            /*!< Clock rate of Tx PA Ramp block */
    uint32_t triggerDelayRise;                         /*!< Programmable Delay Enable on the rising edge */
    uint32_t triggerDelayFall;                         /*!< Programmable Delay Enable on the falling edge */
    adi_adrv9001_GpioPin_e     gpioSource;             /*!< Desired GPIO pin to be used as source of trigger if gpioTriggered = True */
    uint8_t  upEndIndex;                               /*!< 8-bit look-up-table index. This index indicates the end of the ramp up waveform. */
    bool     asymmetricRamp;                           /*!< False = symmetric, True = Ramp-down waveform is asymmetric to the Ramp-up waveform */
    uint8_t  downEndIndex;                             /*!< 8-bit look-up-table index. This index indicates the start of the ramp down waveform.
                                                          Valid only when asymmetricRamp=1 */
    adi_adrv9001_AuxDac_e  auxDacChannelSelect;        /*!< Choose the AuxDacChannel [0, 1, 2, 3] to ouptut the Ramp or SPI signal */
    uint16_t paRampLUT[ADRV9001_TX_PA_RAMP_LUT_SIZE];  /*!< PA Ramp look-up-table. 256 depth Array of LUT elements */
} adi_adrv9001_PaRampCfg_t;

/**
 *  \brief Data structure to hold ADRV9001 Tx attenuation control configuration
 */
typedef struct adi_adrv9001_TxAttenuationPinControlCfg
{
    uint16_t stepSize_mdB;                  /*!< Step size of change in txAttenuation when a rising edge occurs on on either incrementPin or decrementPin. Range: 0 mdB to 1550 mdB, LSB =50 mdB */
    adi_adrv9001_GpioPin_e incrementPin;    /*!< When a rising edge occurs on this GPIO pin, txAttenuation will increment by stepSize_mdB .
                                                 Once txAttenuation has reached index 839(2094 mdB) subsequent rising edges on incrementPin will not change the txAttenuation */
    adi_adrv9001_GpioPin_e decrementPin;    /*!< When a rising edge occurs on this GPIO pin, txAttenuation will decrement by stepSize_mdB .
                                                 Once txAttenuation has reached index 0 subsequent rising edges on decrementPin will not change the txAttenuation */
} adi_adrv9001_TxAttenuationPinControlCfg_t;

/**
 * \brief Amplitude of the NCO tone
 */
typedef enum adi_adrv9001_TxInternalToneAmplitude
{
    ADI_ADRV9001_TXINTERNALTONEAMPLITUDE_0_DB,
    ADI_ADRV9001_TXINTERNALTONEAMPLITUDE_6_DB,
    ADI_ADRV9001_TXINTERNALTONEAMPLITUDE_12_DB
} adi_adrv9001_TxInternalToneAmplitude_e;

/**
 * \brief Internal tone generation (NCO) settings
 */
typedef struct adi_adrv9001_TxInternalToneGeneration
{
    bool enable;                                        /*!< Whether to enable NCO tone generation */
    adi_adrv9001_TxInternalToneAmplitude_e amplitude;   /*!< Amplitude of the tone */
    int32_t frequency_Hz;                               /*!< Frequency of the tone, in Hertz (valid: +/- tx_interface_rate) */
} adi_adrv9001_TxInternalToneGeneration_t;

#endif /* _ADI_ADRV9001_TX_TYPES_H_ */
