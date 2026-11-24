/**
 * \file
 * \brief Contains ADRV910X API Radio Control data types
 *
 * Copyright (c) 2015 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_RADIO_TYPES_H_
#define _ADI_ADRV910X_RADIO_TYPES_H_

#include "adi_adrv910x_arm_types.h"
#include "adi_adrv910x_powersavingandmonitormode_types.h"
#include "adi_adrv910x_types.h"

/**
 * \brief Modes controlling how a channel is enabled
 */
typedef enum adi_adrv910x_ChannelEnableMode
{
    ADI_ADRV910X_SPI_MODE = 0,          /*!< Channel is enabled via SPI */
    ADI_ADRV910X_PIN_MODE               /*!< Channel is enabled via GPIO pin */
} adi_adrv910x_ChannelEnableMode_e;

typedef enum adi_adrv910x_ChannelState
{
    ADI_ADRV910X_CHANNEL_STANDBY,       /*!< Initial state for all channels once ARM completes boot sequences */
    ADI_ADRV910X_CHANNEL_CALIBRATED,    /*!< Minimum set of initial calibration done without errors; CLK PLL is on; data paths are off */
    ADI_ADRV910X_CHANNEL_PRIMED,        /*!< Data path and interface are on; tracking algorithms NOT scheduled, TRx NOT transmitting or receiving data */
    ADI_ADRV910X_CHANNEL_RF_ENABLED     /*!< Data path and interface are on; tracking algorithms are scheduled, TRx is transmitting or receiving data */
} adi_adrv910x_ChannelState_e;

/**
 * \brief Modes for LO Gen optimization
 */
typedef enum adi_adrv910x_LoGenOptimization
{
    ADI_ADRV910X_LO_GEN_OPTIMIZATION_PHASE_NOISE,      /*!< Reduce phase noise at the cost of increased power consumption */
    ADI_ADRV910X_LO_GEN_OPTIMIZATION_POWER_CONSUMPTION /*!< Reduce power consumption at the cost of increased phase noise */
} adi_adrv910x_LoGenOptimization_e;

/**
 * \brief Enumerated list of options for Rx pin selection
 */
typedef enum adi_adrv910x_RxRfInputSel
{
    ADI_ADRV910X_RX_A = 0,              /*!< Rx RF Input A */
    ADI_ADRV910X_RX_B = 1               /*!< Rx RF Input B */
} adi_adrv910x_RxRfInputSel_e;

/**
 * \brief Enum of PLL selections
 */
typedef enum adi_adrv910x_Pll
{
    ADI_ADRV910X_PLL_LO1 = 0,           /*!< Selects PLL LO1 for Rx and Tx */
    ADI_ADRV910X_PLL_LO2                /*!< Selects PLL LO2 for Rx and Tx */
} adi_adrv910x_Pll_e;

/**
 * \brief Enum of External PLL selections
 */
typedef enum adi_adrv910x_ExtPllSel
{
    ADI_ADRV910X_EXTPLL_VCO1 = 0,       /*!< Selects EXT PLL VCO1 for Rx and Tx */
    ADI_ADRV910X_EXTPLL_VCO2            /*!< Selects EXT PLL VCO2 for Rx and Tx */
} adi_adrv910x_ExtPllSel_e;

/**
 * \brief Enum of charge pump current settings for External VCO loop
 */
typedef enum adi_adrv910x_CpTest
{
    ADI_ADRV910X_CP_TEST_DISABLED = 0,         /*!< Selects disable cp test */
    ADI_ADRV910X_CP_TEST_CP_CURRENT_TO_GROUND, /*!< Selects force cp current to ground */
    ADI_ADRV910X_CP_TEST_CP_CURRENT_TO_VDD     /*!< Selects force cp current to vdd */
} adi_adrv910x_CpTest_e;

/**
 * \brief External VCO band enumeration
 */
typedef enum adi_adrv910x_ExtVCO_Band
{
    ADI_ADRV910X_EXTVCO_BAND1 = 0,      /*!< External VCO Band 1 */
    ADI_ADRV910X_EXTVCO_BAND2 = 1,      /*!< External VCO Band 2 */
    ADI_ADRV910X_EXTVCO_BAND3 = 2       /*!< External VCO Band 3 */
} adi_adrv910x_ExtVCO_Band_e;

/**
 * \brief Data structure to hold radio state information
 */
typedef struct adi_adrv910x_RadioState
{
    adi_adrv910x_ArmSystemStates_e systemState;                                                   /*!< ARM System State */
    adi_adrv910x_PowerSavingAndMonitorMode_ArmMonitorModeStates_e monitorModeState;               /*!< ARM Monitor Mode State (only valid when systemState is in Monitor Mode) */
    adi_adrv910x_ArmMcsStates_e mcsState;                                                         /*!< ARM MCS state (only valid when systemState is MCS) */
    adi_adrv910x_ArmBootStates_e bootState;                                                       /*!< ARM Boot State (only valid when systemState is in POWERUP state and not in MCS state) */
    adi_adrv910x_ChannelState_e channelStates[ADI_ADRV910X_NUM_PORTS][ADI_ADRV910X_NUM_CHANNELS]; /*!< State of each channel; Use adi_common_port_to_index and adi_common_channel_to_index to access conveniently e.g., RX1 is channelStates[adi_common_port_to_index(ADI_RX)][adi_common_channel_to_index(ADI_CHANNEL_1)] */
} adi_adrv910x_RadioState_t;

/**
 * \brief Options for configuring external VCO when using an external VCO for LO NOTE: Repack the byte orders, put extVcoPllRefClkDivider in the highest byte for SPI write
 */
typedef struct adi_adrv910x_ExtVcoConfig
{
    bool forceExtVcoPllRefClkDivider;          /*!< If set to true, forces override of reference clock divider using extVcoPllRefClkDivider */
    uint8_t extVcoPllRefClkDivider;            /*!< Reference clock divider setting when using external VCO on LOx
  Valid range: min 0 = div 1, max 4 = div 16, default 0 */
    uint8_t extVcoPllChargePumpCurrent;        /*!< Charge pump current setting when using external VCO on LOx
  Valid range: min 0 = 0.1mA, max 63 = 6.4mA, step 1 = 0.1mA, default: 15 = 1.5mA */
    uint8_t extVcoPllSettle;                   /*!< Time to wait for PLL to settle when using external VCO on LOx
  Valid range: min: 0 = 256 cycles, max: 10 = 1024*256 cycles, default: 6 = 64*256 cycles */
    bool extVcoPllSpeedUpMode;                 /*!< Set to true to enable speed up mode when using external VCO on LOx */
    uint16_t extVcoPllSpeedUpCounter;          /*!< ext PLL will use a larger charge pump current(extVcoPllSpeedUpChargePumpCurrent) to expedite fine tune phase until the charge pump speed up timer expires(extVcoPllSpeedUpCounter), then switch to the regular charge pump current(extVcoPllChargePumpCurrent) to lock the PLL
  Valid range: min 0, max 4095. the time duration for the speed up phase is calculated using the equasion extVcoPllSpeedUpCounter*512*T_devclk */
    uint8_t extVcoPllSpeedUpChargePumpCurrent; /*!< Charge pump current setting when using external VCO on LOx in speed up
  Valid range: min 0 = 0.1mA, max 63 = 6.4mA, step 1 = 0.1mA
  When extVcoPllSpeedUpMode is true, extVcoPllSpeedUpChargePumpCurrent must >= extVcoPllChargePumpCurrent */
    uint8_t extVcoPllChargePumpBleedCurrent;   /*!< Charge pump bleed current setting when using external VCO on LOx.
  Each LSB corresponds to 5uA: BleedCurrent = extVcoPllChargePumpBleedCurrent * 5uA.
  Valid range: min 0 = 0uA (bleed disabled), step 1 = 5uA.
  The programmed bleed current must not exceed one-tenth of extVcoPllChargePumpCurrent. */
} adi_adrv910x_ExtVcoConfig_t;

/**
 * \brief Carrier frequency configuration
 */
typedef struct adi_adrv910x_Carrier
{
    adi_adrv910x_LoGenOptimization_e loGenOptimization; /*!< LO Gen optimization mode. Note: When carrierFrequency_Hz > 1 GHz this is automatically selected by the ADRV910X regardless of what you set */
    uint64_t carrierFrequency_Hz;                       /*!< Carrier frequency, denoted in Hz. Valid range: 30MHz to 6 GHz */
    int32_t intermediateFrequency_Hz;                   /*!< Intermediate frequency, denoted in Hz.
  Valid range: between +/- min(20 MHz, rfChannelBandwidth_Hz /2)) */
    adi_adrv910x_ExtVcoConfig_t extVcoConfig;           /*!< Exteneded data for PLL configuration when using an external VCO */
    bool enableFrequencyChangeInPrimed;                 /*!< If set to true, allow Carrier frequency change in PRIMED radio state */
} adi_adrv910x_Carrier_t;

/**
 * \brief Data structure to hold Synthesizer PLL Loop filter settings
 */
typedef struct adi_adrv910x_PllLoopFilterCfg
{
    uint16_t effectiveLoopBandwidth_kHz; /*!< Synthesizer PLL effective Loop filter bandwidth. Range TBD. For read-only */
    uint16_t loopBandwidth_kHz;          /*!< Synthesizer PLL Loop filter bandwidth. Range 50-1500 */
    uint8_t phaseMargin_degrees;         /*!< < Synthesizer PLL Loop filter phase margin in degrees. Range 40-85 */
    uint8_t powerScale;                  /*!< Synthesizer PLL Loop filter power scale. Range 0 - 10. Default is 10 */
} adi_adrv910x_PllLoopFilterCfg_t;

/**
 * \brief Data structure to hold external PLL status settings
 */
typedef struct adi_adrv910x_ExtPll
{
    bool pllLocked;         /*!< Enable flag for external PLL Lock status */
    uint16_t coarseDacCode; /*!< Coarse tune DAC code for external VCO */
} adi_adrv910x_ExtPll_t;

/**
 * \brief Delay parameters from Tx_enable/Rx_enable rising/falling edge.
 * \note All fields have the valid range of 0 to 2^24 - 1 and are denoted in ARM clock cycles
 */
typedef struct adi_adrv910x_ChannelEnablementDelays
{
    uint32_t riseToOnDelay;       /*!< Delay from rising edge until LNA is powered up */
    uint32_t riseToAnalogOnDelay; /*!< Delay from rising edge until Tx/Rx analog power up procedure commences */
    uint32_t fallToOffDelay;      /*!< Delay from falling edge until LNA is powered down */
    uint32_t guardDelay;          /*!< Delay from rising edge until first valid data is received over the air */
    uint32_t holdDelay;           /*!< Delay from falling edge until the Tx/Rx interface is disabled */
} adi_adrv910x_ChannelEnablementDelays_t;

typedef struct adi_adrv910x_ExtPllOpenLoopTestConfig
{
    bool coarseLoopSwitchOn;       /*!< Enable coarse loop switch */
    adi_adrv910x_CpTest_e cpTest;  /*!< Configure charge pump current to GND or Vdd when open ext vco loop */
    bool forceUpdateCoarseDACCode; /*!< Force update CoarseTune DAC Code */
    uint16_t coarseDACCode;        /*!< Coarse tune DAC code for external VCO when open loop test */
} adi_adrv910x_ExtPllOpenLoopTestConfig_t;

/**
 * \brief External VCO power control configuration structure
 */
typedef struct adi_adrv910x_Radio_ExtVco_PowerControl
{
    adi_adrv910x_ExtVCO_Band_e extVcoBand; /*!< External VCO band selection */
    adi_adrv910x_ExtPllSel_e pll;          /*!< PLL selection (LO1 or LO2) */
} adi_adrv910x_Radio_ExtVco_PowerControl_t;

#endif /* _ADI_ADRV910X_RADIO_TYPES_H_ */
