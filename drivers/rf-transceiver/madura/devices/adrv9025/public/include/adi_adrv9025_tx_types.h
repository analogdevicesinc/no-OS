/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_tx_types.h
* \brief Contains ADRV9025 API Tx datapath data types
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_TX_TYPES_H_
#define _ADI_ADRV9025_TX_TYPES_H_

#include "adi_adrv9025_gpio_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ADRV9025_TX_ATTEN_TABLE_MAX     960
#define ADRV9025_TX_ATTEN_VALUE_MAX_MDB 41950    /*!< Maximum Value of TX Attenuation in mDb */

#define ADRV9025_ADDR_TX1_ATTEN_TABLE 0xA2000000
#define ADRV9025_ADDR_TX2_ATTEN_TABLE 0xAA000000
#define ADRV9025_ADDR_TX3_ATTEN_TABLE 0xB2000000
#define ADRV9025_ADDR_TX4_ATTEN_TABLE 0xBA000000

#define ADRV9025_TX_ATTEN_TABLE_HP_MASK     0x0003F000
#define ADRV9025_TX_ATTEN_TABLE_HP_SHIFT    12
#define ADRV9025_TX_ATTEN_TABLE_MULT_MASK   0x00000FFF
#define ADRV9025_TX_ATTEN_TABLE_MULT_SHIFT  0
#define ADRV9025_NUMBER_OF_TX_CHANNELS      4

/**
*  \brief Enum of possible Tx channel enables
*/
typedef enum adi_adrv9025_TxChannels
{
    ADI_ADRV9025_TXOFF = 0,
    /*!< No Tx channels are enabled */
    ADI_ADRV9025_TX1 = 1,
    /*!< Tx0 channel enabled */
    ADI_ADRV9025_TX2 = 2,
    /*!< Tx1 channel enabled */
    ADI_ADRV9025_TX3 = 4,
    /*!< Tx2 channel enabled */
    ADI_ADRV9025_TX4 = 8,
    /*!< Tx3 channel enabled */
    ADI_ADRV9025_TXALL = 0x0F /*!< All tx channel mask */
} adi_adrv9025_TxChannels_e;

/**
*  \brief Enum to set the Tx Atenuation step size
*/
typedef enum adi_adrv9025_TxAttenStepSize
{
    ADI_ADRV9025_TXATTEN_0P05_DB = 0,
    /*!< Tx attenuation 0.05dB step size */
    ADI_ADRV9025_TXATTEN_0P1_DB = 1,
    /*!< Tx attenuation 0.1dB step size */
    ADI_ADRV9025_TXATTEN_0P2_DB = 2,
    /*!< Tx attenuation 0.2dB step size */
    ADI_ADRV9025_TXATTEN_0P4_DB = 3 /*!< Tx attenuation 0.4dB step size */
} adi_adrv9025_TxAttenStepSize_e;

/**
*  \brief Enum for the different attenuation mode
*/
typedef enum adi_adrv9025_TxAttenMode
{
    ADI_ADRV9025_TXATTEN_BYPASS_MODE = 0,
    /*!< Tx attenuation mode Bypass: zero total attenuation */
    ADI_ADRV9025_TXATTEN_SPI_MODE = 1,
    /*!< Tx attenuation mode set by 10-bit attenuation index used to determine total attenuation */
    ADI_ADRV9025_TXATTEN_GPIO_MODE = 3,
    /*!< Tx attenuation is control with GPIO Incr/Decr: total attenuation is altered incrementally using pin control */
    ADI_ADRV9025_TXATTEN_SPI2_MODE = 4 /*!< Tx attenuation control is done over Spi2 interface */
} adi_adrv9025_TxAttenMode_e;

/**
 *  \brief Enum of possible Tx DAC FS boost options
 */
typedef enum adi_adrv9025_DacFullScale
{
    ADI_ADRV9025_TX_DACFS_0DB = 0x0,
    /*!< No Full Scale Boost */
    ADI_ADRV9025_TX_DACFS_3DB = 0x01 /*!< Full scale boost = 3dB */
} adi_adrv9025_DacFullScale_e;

/**
* \brief Enumerated list of options to disable Tx data when PLL unlocks
*/
typedef enum adi_adrv9025_TxDataIfUnlock
{
    ADI_ADRV9025_TXUNLOCK_TX_NOT_DISABLED = 0x0,
    /*!< Tx data is not disabled when unlock event */
    ADI_ADRV9025_TXUNLOCK_TX_RAMP_TO_MAX_ATTEN = 0x1 /*!< Tx data is ramped down to zero when unlock event */
} adi_adrv9025_TxDataIfUnlock_e;

/**
* \brief Enumerated list of test tone Gains
*/
typedef enum adi_adrv9025_TxNcoGain
{
    ADI_ADRV9025_TX_NCO_NEG18_DB = 0,
    /*!< Nco gain -18 dB */
    ADI_ADRV9025_TX_NCO_NEG12_DB = 1,
    /*!< Nco gain -12 dB */
    ADI_ADRV9025_TX_NCO_NEG6_DB = 2,
    /*!< Nco gain -6 dB */
    ADI_ADRV9025_TX_NCO_0_DB = 3 /*!< Nco gain 0 dB */
} adi_adrv9025_TxNcoGain_e;

/*
* \brief Enum to select Ramp Step Size and Ramp Step Duration
*
* tdd ramp Step/        |  Corresponding Step/
* Duration    [4 bits]  |  Duration in dB
* ----------------------|----------------------------
*            0          |        0.5
*            1          |        1.0
*            2          |        1.5
*            3          |        2.0
*            4          |        2.5
*            5          |        3.0
*            6          |        3.5
*            7          |        4.0
*            8          |        4.5
*            9          |        5.0
*            10         |        5.5
*            11         |        6.0
*
*/
typedef enum adi_adrv9025_TxRampStepSize
{
    ADI_ADRV9025_TXRAMP_0P5_DB = 0x00,
    ADI_ADRV9025_TXRAMP_1P0_DB,
    ADI_ADRV9025_TXRAMP_1P5_DB,
    ADI_ADRV9025_TXRAMP_2P0_DB,
    ADI_ADRV9025_TXRAMP_2P5_DB,
    ADI_ADRV9025_TXRAMP_3P0_DB,
    ADI_ADRV9025_TXRAMP_3P5_DB,
    ADI_ADRV9025_TXRAMP_4P0_DB,
    ADI_ADRV9025_TXRAMP_4P5_DB,
    ADI_ADRV9025_TXRAMP_5P0_DB,
    ADI_ADRV9025_TXRAMP_5P5_DB,
    ADI_ADRV9025_TXRAMP_6P0_DB
} adi_adrv9025_TxRamp_e;

/**
*  \brief Enum select Error flags to be cleared
*/
typedef enum adi_adrv9025_TxPaProtectErrFlags
{
    ADI_ADRV9025_TXPA_PROTECT_FLAGS_AVG_POWER_ERR = 1,
    ADI_ADRV9025_TXPA_PROTECT_FLAGS_PEAK_POWER_ERR = 2,
    ADI_ADRV9025_TXPA_PROTECT_FLAGS_ALL = 3
} adi_adrv9025_TxPaProtectErrFlags_e;

/**
*  \brief Enum to select event for Tx power ramp-down enable/disable feature
*/
typedef enum adi_adrv9025_PaPllDfrmRampDownEnSel
{
    ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ0 = 0x0001,
    ADI_ADRV9025_RAMP_DOWN_ON_DFRMIRQ1 = 0x0002,
    ADI_ADRV9025_RAMP_DOWN_ON_CLKPLLUNLOCK = 0x0004,
    ADI_ADRV9025_RAMP_DOWN_ON_AUXPLLUNLOCK = 0x0008,
    ADI_ADRV9025_RAMP_DOWN_ON_RF1PLLUNLOCK = 0x0010,
    ADI_ADRV9025_RAMP_DOWN_ON_RF2PLLUNLOCK = 0x0020,
    ADI_ADRV9025_RAMP_DOWN_ON_SERDESPLLUNLOCK = 0x0040,
    ADI_ADRV9025_RAMP_DOWN_ON_PA_PROT_ERROR = 0x0080
} adi_adrv9025_PaPllDfrmRampDownEnSel_e;

/**
*  \brief Enum to select event for Tx power ramp-down enable/disable feature
*/
typedef enum adi_adrv9025_PaPllDfrmRampDownErrSel
{
    ADI_ADRV9025_AVGPOWER_ERR = 0x0001,
    ADI_ADRV9025_PEAKPOWER_ERR = 0x0002,
    ADI_ADRV9025_PLLDFRM_ERR = 0x0004
} adi_adrv9025_PaPllDfrmRampDownErrSel_e;

/**
*  \brief Enum select for slew rate limiter operation mode
*/
typedef enum adi_adrv9025_SrlModeSel
{
    ADI_ADRV9025_SRL_DISABLED = 0x0000,
    /*!< Slew rate limiter is disabled */
    ADI_ADRV9025_SRL_BASIC = 0x0001,
    /*!< Slew rate limiter is enabled, but it just limits slew and doesn't do any ramping */
    ADI_ADRV9025_SRL_RAMP_DOWN = 0x0002,
    /*!< Slew rate limiter is enabled, it limits slew and ramps down Tx power when a violation is detected */
    ADI_ADRV9025_SRL_RAMP_DOWN_WITH_BBIC_INTERRUPT = 0x0003
    /*!< In addition to RAMP_DOWN mode, FW sends a BBIC interrupt. BBIC should manually ramp-up Tx power after recovery */
} adi_adrv9025_SrlModeSel_e;

/**
*  \brief Enum select for slew rate table
*/
typedef enum adi_adrv9025_SrlTableSel
{
    ADI_ADRV9025_SRL_TABLE0 = 0x0000,
    /*!< Slew limit is %10 of full scale */
    ADI_ADRV9025_SRL_TABLE1 = 0x0001,
    /*!< Slew limit is %20 of full scale */
    ADI_ADRV9025_SRL_TABLE2 = 0x0002,
    /*!< Slew limit is %30 of full scale */
    ADI_ADRV9025_SRL_TABLE3 = 0x0003 /*!< Slew limit is %50 of full scale */
} adi_adrv9025_SrlTableSel_e;

/**
*  \brief Enum select for Tx attenuation simultaneous update
*/
typedef enum adi_adrv9025_TxAttenSimultaneousUpdate
{
    ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_DISABLED = 0x0000,
    /*!< Simultaneous Update Disabled */
    ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_SPI = 0x0001,
    /*!< Simultaneous Update with SPI writes */
    ADI_ADRV9025_TXATTEN_SIMULTAN_UPDATE_GPIO = 0x0002 /*!< Simultaneous Update with GPIO pins */
} adi_adrv9025_TxAttenSimultaneousUpdate_e;

/**
*  \brief Enum select for pa protection input
*/
typedef enum adi_adrv9025_PaProtectionInputSel
{
    ADI_ADRV9025_COMPLEX_MULT_OUTPUT = 0x0000,
    /*!< Input data to Pa protection block is probed from complex mult output */
    ADI_ADRV9025_TXQEC_ACTUATOR_OUTPUT = 0x0001 /*!< Input data to Pa protection block is probed from tx qec actuator output */
} adi_adrv9025_PaProtectionInputSel_e;

/**
*  \brief Enum values representing bitmasks for Tx Ramp Down Events
*/
typedef enum adi_adrv9025_InitTxRampDownEvents
{
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ0 = 0x0001,
    /*!< During device initialization, mask(disable) Tx ramp down on Jesd deframer Irq 0 event */
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_DFRMIRQ1 = 0x0002,
    /*!< During device initialization, mask(disable) Tx ramp down on Jesd deframer Irq 1 event*/
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF1PLLUNLOCK = 0x0004,
    /*!< During device initialization, mask(disable) Tx ramp down on RF1 PLL Unlock */
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_RF2PLLUNLOCK = 0x0008,
    /*!< During device initialization, mask(disable) Tx ramp down on RF2 PLL Unlock */
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_SERDESPLLUNLOCK = 0x0010,
    /*!< During device initialization, mask(disable) Tx ramp down on Serdes PLL Unlock */
    ADI_ADRV9025_DISABLE_TX_RAMP_DOWN_ON_CLKPLLUNLOCK = 0x0020,
    /*!< During device initialization, mask(disable) Tx ramp down on Clk PLL Unlock */
    ADI_ADRV9025_TX_RAMP_DOWN_AUTOSELECT = 0x0080
    /*!< During device initialization, automatically decide the events for which Tx ramp down enabled. This bitmask will ignore
                                                                            the status of other bits. In auto mode, Tx ramp down will be enabled for active Jesd deframer/s and LO source/s (RF1/RF2 PLL) of Tx channels
                                                                            Tx ramp down on Serdes Pll(if its being used) and Clk Pll unlock events will be enabled */
} adi_adrv9025_InitTxRampDownEvents_e;

/**
*  \brief Enum values representing possible Tx NCO shifter operation modes
*/
typedef enum adi_adrv9025_TxNcoShifterMode
{
    ADI_ADRV9025_TXNCO_INJECT = 0x0000,
    /*!< Injects a single tone at the specified shift frequency at the specified gain */
    ADI_ADRV9025_TXNCO_MIX = 0x0001
    /*!< Mixes the data signal by the specified shift frequency up or down the band   */
} adi_adrv9025_TxNcoShifterMode_e;

/**
*  \brief Enum values representing possible Tx NCO shifter Tone Injection Gain
*/
typedef enum adi_adrv9025_TxNcoToneGain
{
    ADI_ADRV9025_TXNCO_GAIN_0dB = 3,
    /*!< Injects a single tone at the specified shift frequency at the 0dB gain */
    ADI_ADRV9025_TXNCO_GAIN_minus6dB = 2,
    /*!< Injects a single tone at the specified shift frequency at the -6dB gain */
    ADI_ADRV9025_TXNCO_GAIN_minus12dB = 1,
    /*!< Injects a single tone at the specified shift frequency at the -12dB gain */
    ADI_ADRV9025_TXNCO_GAIN_minus18dB = 0
    /*!< Injects a single tone at the specified shift frequency at the -18dB gain */
} adi_adrv9025_TxNcoToneGain_e;

/**
*  \brief Data structure to hold ADRV9025 Tx PA Protection configuration settings
*/
typedef struct adi_adrv9025_TxPaProtectCfg
{
    adi_adrv9025_TxChannels_e txChannel;    /*!< Tx Channel to be configered */
    uint8_t                   avgDuration;  /*!< PA Protection Average Power Measurement Duration. */
    uint8_t                   peakDuration; /*!< PA Protection Peak Power Measurement Duration */
    uint8_t                   txAttenStep;
    /*!< PA Protection Attenuation gain step. Gain step down is not allowed for Tokelau device. This field is not being used actively. */
    uint8_t gainStepDownEn;
    /*!< PA Protection Gain Step Down Enable. Gain step down is not allowed for Tokelau device. This field is not being used actively.*/
    uint16_t powerThreshold; /*!< PA Protection Average Power Threshold. */
    uint8_t  peakCount;      /*!< Peak Count Causing PA Error. */
    uint16_t peakThreshold;  /*!< PA Protection Peak Power Threshold. Max value for Silicon A: 255  Max Value for Silicon B: 8191 */
    uint8_t  rampStepDuration;
    /*!< PA Protection Ramp Step duration. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    uint8_t rampStepSize;
    /*!< PA Protection Ramp Step Size. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    uint8_t rampMaxAtten;
    /*!< PA Protection Ramp Max Attenuation. This field is not being used actively. It is set to a hardcoded value by PaPllDfrmEventRampDownEnableSet API */
    uint8_t avgPowerEnable;
    /*!< This enables average power measurement block. If enabled, PA error is flagged when average power measurement is above average power threshold */
    uint8_t peakPowerEnable;
    /*!< This enables peak power measurement block. If enabled, PA error is flagged when peak power count is above peak count threshold */
    adi_adrv9025_PaProtectionInputSel_e inputSel; /*!< This selects the source of input signal for Pa protection block */
    uint8_t                             avgPeakRatioEnable;
    /*!< This enables average to peak power ratio calculation block, both avgPower and peakPower calculations must be enabled before enabling ratio calculation */
} adi_adrv9025_TxPaProtectCfg_t;

/**
*  \brief Data structure to readback errors realated to power
*/
typedef struct adi_adrv9025_TxPaProtectionErr
{
    uint8_t  peakPowerErr; /*!< PA Protection Peak Power Error.
                                        Sticky. When set, disables power calculations unless "error_clear_required" bit is clear */
    uint8_t  avgPowerErr;  /*!< PA Protection Average Power Error.
                                        Sticky. When set, disables power calculations unless "error_clear_required" bit is clear.*/
    uint16_t powerErr;     /*!< PA Protection Error Power. Updated when average power error occurs. */
} adi_adrv9025_TxPaProtectionErr_t;

/**
*  \brief Data structure to hold ADRV9025 Tx PA Protection current status values
*/
typedef struct adi_adrv9025_TxPaProtectStatus
{
    uint16_t avgPower;      /*!< PA protection power for readback. */
    uint16_t avgPeakRatio;  /*!< Average to Peak power ratio for readback. */
    uint16_t avgErrorPower; /*!< Value of average power when average power Pa protection error is triggered */
} adi_adrv9025_TxPaProtectStatus_t;

/**
 * \brief Configuration structure for Tx power control
 */
typedef struct adi_adrv9025_TxAttenSimultUpdateCfg
{
    adi_adrv9025_GpioPinSel_e gpioUpdatePin;
    /*!< Gpio Pin selection to trigger Tx attenutaion simultaneous update, select INVALID when simultaneous update with GPIO is disabled */
    adi_adrv9025_TxAttenSimultaneousUpdate_e simultaneousUpdate; /*!< Tx attenuation simultaneous update mode selection */
} adi_adrv9025_TxAttenSimultUpdateCfg_t;

/**
*  \brief Data structure to hold ADRV9025 Tx Attenuation Pin Control Configuration
*/
typedef struct adi_adrv9025_TxAttenPinCfg
{
    uint32_t                  txChannelMask; /*!< One or multiple Tx channel selection. */
    uint8_t                   stepSize;      /*!< The step that will increase or decrease the channel attenuation. This parameter sets the
                                                   change in Tx attenuation for each increment or decrement signal received in incr/decr mode.
                                                   Step of 1 changes attenuation by 0.05dB. Valid range is from 0 to 31 */
    adi_adrv9025_GpioPinSel_e txAttenIncPin; /*!< GPIO used to increment Tx attenuation - GPIO00-GPIO15 */
    adi_adrv9025_GpioPinSel_e txAttenDecPin; /*!< GPIO used to decrement Tx attenuation - GPIO00-GPIO15 */
} adi_adrv9025_TxAttenPinCfg_t;

/**
* \brief Configuration structure for Tx power control
*/
typedef struct adi_adrv9025_TxAttenCfg
{
    uint32_t                       txChannelMask;
    adi_adrv9025_TxAttenStepSize_e txAttenStepSize; /*!< Tx Attenuation step size */
    adi_adrv9025_TxDataIfUnlock_e  disTxDataIfPllUnlock;
    /*!< Option to disable transmit data when a PLL unlocks. Note: This field is not being used actively.
                                                                         If user enables at least one pll unlock event with adi_adrv9025_PaPllDfrmEventRampDownEnableSet,
                                                                         gain ramp down on pll unlock is automatically enabled */
    adi_adrv9025_TxDataIfUnlock_e rampJesdDfrm;
    /*!< Ramp up attenuation when a deframer link unlocks. Note: This field is not being used actively.
                                                                 If user enables at least one dfrm event with adi_adrv9025_PaPllDfrmEventRampDownEnableSet,
                                                                 gain ramp down on dfrm event is automatically enabled */
    adi_adrv9025_TxAttenMode_e  attenMode; /*!<  */
    adi_adrv9025_DacFullScale_e dacFullScale;
    /*!< Tx Dac full scale Setting. WARNING: This field is not being used actively, left for backward compatibility*/
    adi_adrv9025_TxAttenSimultUpdateCfg_t simultaneousUpdateCfg;
    /*!< Tx Attenuation Simultaneous Update Configuration, it is only used when attenMode is selected as SPI */
    adi_adrv9025_TxAttenPinCfg_t gpioModePinCfg; /*!< Tx Attenuation GPIO mode pin configuration, it is only used when attenMode is selected as GPIO */
} adi_adrv9025_TxAttenCfg_t;

/**
* \brief Data structure to hold a adrv9025 device Tx attenuation calls
*/
typedef struct adi_adrv9025_TxAtten
{
    uint32_t txChannelMask;
    uint16_t txAttenuation_mdB;
} adi_adrv9025_TxAtten_t;

/**
 *  \brief Data structure to hold ADRV9025 Tx NCO test tone Configuration
 */
typedef struct adi_adrv9025_TxTestToneCfg
{
    uint8_t                  txChannelMask;
    uint8_t                  enable;        /*!< 0 = Disable Tx NCO, 1 = Enable Tx NCO on both transmitters */
    int32_t                  txToneFreq_Hz; /*!< Signed frequency in Hz of the desired Tx tone */
    adi_adrv9025_TxNcoGain_e txToneGain;    /*< NCO tone gain given by the enum adi_adrv9025_TxNcoGain_e */
} adi_adrv9025_TxTestToneCfg_t;

/**
* \brief Data structure to hold an adrv9025 device Tx attenuation Table Row
*        (Single row of a complete table)
*/
typedef struct adi_adrv9025_TxAttenTableRow
{
    uint16_t txAttenMult;
    uint8_t  txAttenHp;
    uint8_t  Reserve;
} adi_adrv9025_TxAttenTableRow_t;

/**
*  \brief Data structure to hold ADRV9025 Tx Attenuation Spi2 Pin Control Configuration
*/
typedef struct adi_adrv9025_TxAttenSpi2PinCfg
{
    uint32_t                          txChannelMask;
    adi_adrv9025_Spi2TxAttenGpioSel_e txAttenSpi2Pin;
}                                     adi_adrv9025_TxAttenSpi2PinCfg_t;

/**
*  \brief Data structure to hold ADRV9025 Tx Slew Rate Limiter Configuration
*/
typedef struct adi_adrv9025_SlewRateLimiterCfg_t
{
    uint32_t                   txChannelMask;
    adi_adrv9025_SrlModeSel_e  modeSel;  /*!< Slew rate limiter mode selection; DISABLED/BASIC/RAMPDOWN/RAMPDOWNWITHBBICINTERRUPT */
    adi_adrv9025_SrlTableSel_e tableSel; /*!< Table selection for slew rate limit; %10 / %20 / %30 / %50 of full scale */
    uint8_t                    srlOffset;
    /*!< Slew rate threshold to trigger limiter. (0 = straight table, 1 = 15/16 -- 1.0, 2 = 15/16 and 14/16 -- 1.0, etc.) */
} adi_adrv9025_SlewRateLimiterCfg_t;

/**
*  \brief Data structure to hold ADRV9025 Tx NCO Shifter Configuration
*/
typedef struct adi_adrv9025_TxNcoShifterCfg
{
    uint32_t                        txChannelMask;      /*!< Tx Channel to which the configuration is applied */
    adi_adrv9025_TxNcoShifterMode_e shifterMode;        /*!< Tx NCO operation mode 0 - inject tone, 1 - mix band frequency */
    int32_t                         shiftFrequency_kHz; /*!< Frequency at which to inject tone or frequency to shift signal up/down band */
    adi_adrv9025_TxNcoToneGain_e    shiftGain;          /*!< Gain to apply to injected tone */
    uint8_t                         enable;             /*!< flag indicating if the NCO is enabled = 1 or disabled = 0 */
} adi_adrv9025_TxNcoShifterCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_TX_TYPES_H_ */
