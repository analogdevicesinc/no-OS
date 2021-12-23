/**
 * \file
 * \brief Contains ADRV9001 API Radio Control data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_RADIO_TYPES_H_
#define _ADI_ADRV9001_RADIO_TYPES_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_gpio_types.h"
#include "adi_adrv9001_powersavingandmonitormode_types.h"

/**
 *  \brief Enum of PLL selections
 */
typedef enum adi_adrv9001_Pll
{
    ADI_ADRV9001_PLL_LO1 = 0, /*!< Selects PLL LO1 for Rx and Tx */
    ADI_ADRV9001_PLL_LO2,     /*!< Selects PLL LO2 for Rx and Tx */
    ADI_ADRV9001_PLL_AUX,     /*!< Selects AUX PLL for Rx and tx*/
    ADI_ADRV9001_PLL_CLK,
    ADI_ADRV9001_PLL_CLK_LP
} adi_adrv9001_Pll_e;

/**
 *  \brief Enum of PLL calibration mode
 */
typedef enum adi_adrv9001_PllCalibration
{
    ADI_ADRV9001_PLL_CALIBRATION_NORMAL,    /*!< PLL calibration setting in Normal mode */
    ADI_ADRV9001_PLL_CALIBRATION_FAST,      /*!< PLL calibration setting in Fast mode  */
    ADI_ADRV9001_PLL_CALIBRATION_RESERVED   /*!< RESERVED */
} adi_adrv9001_PllCalibration_e;

/**
 * \brief Modes for LO Gen optimization
 */
typedef enum adi_adrv9001_LoGenOptimization
{
    ADI_ADRV9001_LO_GEN_OPTIMIZATION_PHASE_NOISE,       // Reduce phase noise at the cost of increased power consumption
    ADI_ADRV9001_LO_GEN_OPTIMIZATION_POWER_CONSUMPTION  // Reduce power consumption at the cost of increased phase noise
} adi_adrv9001_LoGenOptimization_e;

/**
 * \brief PLL power levels
 */
typedef enum adi_adrv9001_PllPower
{
    ADI_ADRV9001_PLL_POWER_LOW,
    ADI_ADRV9001_PLL_POWER_MEDIUM,
    ADI_ADRV9001_PLL_POWER_HIGH
} adi_adrv9001_PllPower_e;

/**
 * \brief Carrier frequency configuration
 */
typedef struct adi_adrv9001_Carrier
{
    /** LO Gen optimization mode
     * \note When carrierFrequency_Hz > 1 GHz this is automatically selected by the ADRV9001 regardless of what you set
     */
    adi_adrv9001_LoGenOptimization_e loGenOptimization;
    uint64_t carrierFrequency_Hz;       /*!< Carrier frequency, denoted in Hz. Valid range: 30MHz to 6 GHz*/
    int32_t intermediateFrequency_Hz;   /*!< Intermediate frequency, denoted in Hz.
                                             Valid range: between +/- min(20 MHz, rfChannelBandwidth_Hz /2)) */
    adi_adrv9001_RxRfInputSel_e manualRxport; /*!< Port selection to manually switch Rx port */
} adi_adrv9001_Carrier_t;

/**
 * \brief pll configuration
 */
typedef struct adi_adrv9001_PllConfig
{
    adi_adrv9001_PllCalibration_e pllCalibration;
    adi_adrv9001_PllPower_e pllPower;

} adi_adrv9001_PllConfig_t;


/**
 * \brief Modes controlling how a channel is enabled
 */
typedef enum adi_adrv9001_ChannelEnableMode
{
    ADI_ADRV9001_SPI_MODE = 0,      // Channel is enabled via SPI
    ADI_ADRV9001_PIN_MODE           // Channel is enabled via GPIO pin
} adi_adrv9001_ChannelEnableMode_e;

typedef enum adi_adrv9001_ChannelState
{
    ADI_ADRV9001_CHANNEL_STANDBY,                /*!< Initial state for all channels once ARM completes boot sequences */
    ADI_ADRV9001_CHANNEL_CALIBRATED,             /*!< Minimum set of initial calibration done without errors; CLK PLL is on; data paths are off */
    ADI_ADRV9001_CHANNEL_PRIMED,                 /*!< Data path and interface are on; tracking algorithms NOT scheduled, TRx NOT transmitting or receiving data */
    ADI_ADRV9001_CHANNEL_RF_ENABLED,             /*!< Data path and interface are on; tracking algorithms are scheduled, TRx is transmitting or receiving data */
} adi_adrv9001_ChannelState_e;

/**
 *  \brief Data structure to hold radio state information
 */
typedef struct adi_adrv9001_RadioState
{
    adi_adrv9001_ArmSystemStates_e                                systemState;        /*!< ARM System State */
    adi_adrv9001_PowerSavingAndMonitorMode_ArmMonitorModeStates_e monitorModeState;   /*!< ARM Monitor Mode State (only valid when systemState is in Monitor Mode) */
    adi_adrv9001_ArmMcsStates_e                                   mcsState;           /*!< ARM MCS state (only valid when systemState is MCS) */
    adi_adrv9001_ArmBootStates_e                                  bootState;          /*!< ARM Boot State (only valid when systemState is in POWERUP state and not in MCS state) */
    /**
     * State of each channel; Use adi_common_port_to_index and adi_common_channel_to_index to access conveniently
     * e.g., RX1 is channelStates[adi_common_port_to_index(ADI_RX)][adi_common_channel_to_index(ADI_CHANNEL_1)]
     */
    adi_adrv9001_ChannelState_e channelStates[ADI_ADRV9001_NUM_PORTS][ADI_ADRV9001_NUM_CHANNELS];
} adi_adrv9001_RadioState_t;

/**
 * \brief Data structure to hold Synthesizer PLL Loop filter settings
 */
typedef struct adi_adrv9001_PllLoopFilterCfg
{
    uint16_t effectiveLoopBandwidth_kHz; /*!< Synthesizer PLL effective Loop filter bandwidth. Range TBD. For read-only */
    uint16_t loopBandwidth_kHz; /*!< Synthesizer PLL Loop filter bandwidth. Range 50-1500 */
    uint8_t  phaseMargin_degrees; /*< Synthesizer PLL Loop filter phase margin in degrees. Range 40-85 */
    uint8_t  powerScale; /*!< Synthesizer PLL Loop filter power scale. Range 0 - 10. Default is 10 */
} adi_adrv9001_PllLoopFilterCfg_t;

/**
 * \brief Delay parameters from Tx_enable/Rx_enable rising/falling edge.
 *
 * \note All fields have the valid range of 0 to 2^24 - 1 and are denoted in ARM clock cycles
 */
typedef struct adi_adrv9001_ChannelEnablementDelays
{
    uint32_t riseToOnDelay;         /*!< Delay from rising  edge until LNA is powered up */
    uint32_t riseToAnalogOnDelay;   /*!< Delay from rising  edge until Tx/Rx analog power up procedure commences */
    uint32_t fallToOffDelay;        /*!< Delay from falling edge until LNA is powered down */
    uint32_t guardDelay;            /*!< Delay from rising  edge until first valid data is received over the air */
    uint32_t holdDelay;             /*!< Delay from falling edge until the Tx/Rx interface is disabled */
} adi_adrv9001_ChannelEnablementDelays_t;

#endif /* _ADI_ADRV9001_RADIO_TYPES_H_ */
