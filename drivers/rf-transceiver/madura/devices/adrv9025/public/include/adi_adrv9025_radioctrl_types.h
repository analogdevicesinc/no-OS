/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_radioctrl_types.h
 * \brief Contains ADRV9025 API Radio Control data types
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_RADIOCTRL_TYPES_H_
#define _ADI_ADRV9025_RADIOCTRL_TYPES_H_

#include "adi_adrv9025_gpio_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief Data structure to hold ARM GPIO pin assignments, polarity, and pin enable for the ADRV9025 ARM
 */
typedef struct adi_adrv9025_ArmGpioPinCfg
{
    adi_adrv9025_GpioPinSel_e gpioPinSel; /*!< Select desired GPIO pin to input into ADRV9025 (valid 0-15) */
    uint8_t                   polarity;   /*!< Signal polarity (0 = Normal polarity, 1=ADRV9025 will invert the signal before using) */
    uint8_t                   enable;     /*!< 1 = Enable ADRV9025 ARM use of GPIO signal, 0 = ADRV9025 ARM uses ARM command to set this signal value */
} adi_adrv9025_ArmGpioPinCfg_t;

/**
 * \brief Data structure to hold ARM GPIO pin assignments for each ARM input/output pin.
 */
typedef struct adi_adrv9025_ArmGpioCfg
{
    /* ADRV9025 ARM input GPIO pins -- Only valid if orxPinMode = 1 */
    adi_adrv9025_ArmGpioPinCfg_t orx1TxSel0Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx1TxSel1Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx2TxSel0Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx2TxSel1Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx3TxSel0Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx3TxSel1Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx4TxSel0Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t orx4TxSel1Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t tx12CalEnPin;    /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t tx34CalEnPin;    /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t calUpdate0Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t calUpdate1Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t calUpdate2Pin;   /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
    adi_adrv9025_ArmGpioPinCfg_t rfPllFreqHopPin; /*!< Select desired GPIO pin as input to ADRV9025(valid 0-18), polarity, enable */
} adi_adrv9025_ArmGpioCfg_t;

/**
* \brief Data structure to hold Stream trigger GPIO pin assignments that can be used to trigger streams.
*/
typedef struct adi_adrv9025_StreamGpioPinCfg
{
    adi_adrv9025_GpioPinSel_e streamGpInput0;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 0 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput1;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 1 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput2;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 2 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput3;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 3 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput4;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 4 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput5;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 5 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput6;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 6 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput7;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 7 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput8;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 8 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput9;   /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 9 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput10;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 10 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput11;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 11 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput12;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 12 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput13;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 13 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput14;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 14 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
    adi_adrv9025_GpioPinSel_e streamGpInput15;  /*!< Select desired GPIO pin input to ADRV9025 stream processor GP input 15 (valid GPIO0-GPIO15). To disable select ADI_ADRV9025_GPIO_INVALID */
} adi_adrv9025_StreamGpioPinCfg_t;

/**
 * \brief Data structure to hold the radio state information.
 */
typedef enum adi_adrv9025_RadioState
{
    ADI_ADRV9025_RADIO_POWERUP      = 0,
    ADI_ADRV9025_RADIO_BOOTCOMPLETE,
    ADI_ADRV9025_BOOTERROR
} adi_adrv9025_RadioState_e;

/**
 * \brief Enumerated list of Tx to ORx1/2 Front End mapping selections
 */
typedef enum adi_adrv9025_TxToOrx12Mapping
{
    ADI_ADRV9025_MAP_TX1_ORX12      = 0,    /*!< Tx1 to ORx12 Front End mapping select */
    ADI_ADRV9025_MAP_TX2_ORX12      = 1     /*!< Tx2 to ORx12 Front End mapping select */
} adi_adrv9025_TxToOrx12Mapping_e;

/**
* \brief Enumerated list of Tx to ORx3/4 Front End mapping selections
*/
typedef enum adi_adrv9025_TxToOrx34Mapping
{
    ADI_ADRV9025_MAP_TX3_ORX34 = 0,     /*!< Tx3 to ORx3/4 Front End mapping select */
    ADI_ADRV9025_MAP_TX4_ORX34 = 1      /*!< Tx4 to ORx3/4 Front End mapping select */
} adi_adrv9025_TxToOrx34Mapping_e;

/**
 *  \brief Enum of PLL selections
 */
typedef enum adi_adrv9025_PllName
{
    ADI_ADRV9025_LO1_PLL = 0,   /*!< Selects PLL LO1 for Rx and Tx */
    ADI_ADRV9025_LO2_PLL,       /*!< Selects PLL LO2 for Rx and Tx */
    ADI_ADRV9025_AUX_PLL        /*!< Selects AUX PLL for Rx and tx*/
} adi_adrv9025_PllName_e;

/**
*  \brief Enum of PLL Mode selections
*/
typedef enum adi_adrv9025_PllModeSel
{
    ADI_ADRV9025_PLL_SLOW_MODE = 0x00,  /*!< Selects the PLL to be programmed to run in slow mode */
    ADI_ADRV9025_PLL_FAST_MODE = 0x00   /*!< Selects the PLL to be programmed to run in slow mode */
} adi_adrv9025_PllModeSel_e;

/**
*  \brief Enum of PLL Aux LO resync selections
*/
typedef enum adi_adrv9025_PllAuxLoResyncSel
{
    ADI_ADRV9025_PLL_AUX_LO_RESYNC_ENABLE   = 0x00,     /*!< Automatically resync AUX LO and TX LO (phase sync) */
    ADI_ADRV9025_PLL_AUX_LO_RESYNC_DISABLE  = 0x20      /*!< Don't automatically resync AUX LO and TX LO (phase sync) */
} adi_adrv9025_PllAuxLoResyncSel_e;

/**
*  \brief Enum of PLL Aux LO offset programming enable
*/
typedef enum adi_adrv9025_PllAuxLoOffsetProgSel
{
    ADI_ADRV9025_PLL_AUX_LO_OFFSET_PROG_ENABLE  = 0x00,     /*!< Automatically program AUXPLL with offset LO when setting TX LO frequency */
    ADI_ADRV9025_PLL_AUX_LO_OFFSET_PROG_DISABLE = 0x40      /*!< Don't automatically program AUXPLL with offset LO when setting TX LO frequency */
} adi_adrv9025_PllAuxLoOffsetProgSel_e;

/**
*  \brief Enum of Tx Enable Modes
*/
typedef enum adi_adrv9025_TxEnableMode
{
    ADI_ADRV9025_TX_EN_SPI_MODE = 0,    /*!< Selects SPI registers to control Tx signal path on/off */
    ADI_ADRV9025_TX_EN_PIN_MODE,        /*!< Selects Pin mode to control Tx signal path on/off. Tx signal path is controlled by dedicated input pins to ARM */
    ADI_ADRV9025_TX_EN_INVALID_MODE
} adi_adrv9025_TxEnableMode_e;

/**
*  \brief Enum of Rx Enable Modes
*/
typedef enum adi_adrv9025_RxEnableMode
{
    ADI_ADRV9025_RX_EN_SPI_MODE = 0,    /*!< Selects SPI registers to control Rx signal path on/off */
    ADI_ADRV9025_RX_EN_PIN_MODE,        /*!< Selects Pin mode to control Rx signal path on/off. Rx signal path is controlled by dedicated input pins to ARM */
    ADI_ADRV9025_RX_EN_INVALID_MODE
} adi_adrv9025_RxEnableMode_e;

/**
*  \brief Enum of ORx Enable Modes
*/
typedef enum adi_adrv9025_ORxEnableMode
{
    ADI_ADRV9025_ORX_EN_SPI_MODE = 0,           /*!< Selects SPI registers to control ORx signal path on/off */
    ADI_ADRV9025_ORX_EN_SINGLE_CH_3PIN_MODE,    /*!< Selects Pin mode to control ORx signal path on/off. ORx_ENABLE[0] pin is the enable/disable select. ORx_ENABLE[1], ORx_ENABLE[2] pins can be used to select ORx channel*/
    ADI_ADRV9025_ORX_EN_SINGLE_CH_2PIN_MODE,    /*!< Selects Pin mode to control ORx signal path on/off. ORx_ENABLE[0] pin is the enable/disable select. Channel select is accomplished by a combination of SPI reg and ORx_ENABLE[1] pin*/
    ADI_ADRV9025_ORX_EN_SINGLE_CH_1PIN_MODE,    /*!< Selects Pin mode to control ORx signal path on/off. ORx_ENABLE[0] pin is the enable/disable select. Channel select is accomplished by SPI registers*/
    ADI_ADRV9025_ORX_EN_DUAL_CH_4PIN_MODE,      /*!< Selects Pin mode to control ORx signal path on/off. ORx_ENABLE[0], ORx_ENABLE[1] pins are enable/disable switches. Channel select is accomplished by ORx_ENABLE[2] and ORx_ENABLE[3] pins*/
    ADI_ADRV9025_ORX_EN_DUAL_CH_2PIN_MODE,      /*!< Selects Pin mode to control ORx signal path on/off. ORx_ENABLE[0], ORx_ENABLE[1] pins are enable/disable switches. Channel select is accomplished by SPI Registers*/
    ADI_ADRV9025_ORX_EN_INVALID_MODE
} adi_adrv9025_ORxEnableMode_e;

/**
*  \brief Enum of Single Channel ORx Pin Mode ORx select
*/
typedef enum adi_adrv9025_SingleChannelPinModeOrxSel
{
    ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX1_FE = 0,    /*!< Selects ORx1 in single channel 2 pin / single channel 1 pin ORx pin mode */
    ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX2_FE,        /*!< Selects ORx2 in single channel 2 pin / single channel 1 pin ORx pin mode */
    ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX3_FE,        /*!< Selects ORx3 in single channel 2 pin / single channel 1 pin ORx pin mode */
    ADI_ADRV9025_SINGLE_CH_PIN_MODE_ORX4_FE,        /*!< Selects ORx4 in single channel 2 pin / single channel 1 pin ORx pin mode */
    ADI_ADRV9025_SINGLE_CH_PIN_MODE_INVALID_ORX_SEL
} adi_adrv9025_SingleChannelPinModeOrxSel_e;

/**
*  \brief Enum of Dual Channel ORx Pin Mode ORx select
*/
typedef enum adi_adrv9025_DualChannelPinModeOrxSel
{
    ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX1_ORX3_SEL = 0,    /*!< Selects ORx1 and ORx3 in dual channel 2 pin mode */
    ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX2_ORX3_SEL,        /*!< Selects ORx2 and ORx3 in dual channel 2 pin mode */
    ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX1_ORX4_SEL,        /*!< Selects ORx1 and ORx4 in dual channel 2 pin mode */
    ADI_ADRV9025_DUAL_CH_PIN_MODE_ORX2_ORX4_SEL,        /*!< Selects ORx2 and ORx4 in dual channel 2 pin mode */
    ADI_ADRV9025_DUAL_CH_PIN_MODE_INVALID_ORX_SEL
} adi_adrv9025_DualChannelPinModeOrxSel_e;

/**
* \brief Enumerated list of Tx to ORx1 mapping selections
*/
typedef enum adi_adrv9025_TxToOrx1Mapping
{
    ADI_ADRV9025_MAP_NONE_ORX1  = 0,     /*!< No Tx to ORx1 mapping select */
    ADI_ADRV9025_MAP_TX1_ORX1   = 2,     /*!< Tx1 to ORx1 mapping select */
    ADI_ADRV9025_MAP_TX2_ORX1   = 3      /*!< Tx2 to ORx1 mapping select */
} adi_adrv9025_TxToOrx1Mapping_e;

/**
* \brief Enumerated list of Tx to ORx2 mapping selections
*/
typedef enum adi_adrv9025_TxToOrx2Mapping
{
    ADI_ADRV9025_MAP_NONE_ORX2  = 0,    /*!< No Tx to ORx2 mapping select */
    ADI_ADRV9025_MAP_TX1_ORX2   = 2,    /*!< Tx1 to ORx2 mapping select */
    ADI_ADRV9025_MAP_TX2_ORX2   = 3     /*!< Tx2 to ORx2 mapping select */
} adi_adrv9025_TxToOrx2Mapping_e;

/**
* \brief Enumerated list of Tx to ORx3 mapping selections
*/
typedef enum adi_adrv9025_TxToOrx3Mapping
{
    ADI_ADRV9025_MAP_NONE_ORX3  = 0,    /*!< No Tx to ORx3 mapping select */
    ADI_ADRV9025_MAP_TX3_ORX3   = 2,    /*!< Tx3 to ORx3 mapping select */
    ADI_ADRV9025_MAP_TX4_ORX3   = 3     /*!< Tx4 to ORx3 mapping select */
} adi_adrv9025_TxToOrx3Mapping_e;

/**
* \brief Enumerated list of Tx to ORx4 mapping selections
*/
typedef enum adi_adrv9025_TxToOrx4Mapping
{
    ADI_ADRV9025_MAP_NONE_ORX4  = 0,    /*!< No Tx to ORx4 mapping select */
    ADI_ADRV9025_MAP_TX3_ORX4   = 2,    /*!< Tx3 to ORx4 mapping select */
    ADI_ADRV9025_MAP_TX4_ORX4   = 3     /*!< Tx4 to ORx4 mapping select */
} adi_adrv9025_TxToOrx4Mapping_e;

/**
*  \brief Enum to describe dual band status
*/
typedef enum adi_adrv9025_DualbandStatus
{
    ADI_ADRV9025_DUALBAND_SWITCHING_DISABLED    = 0x00,
    ADI_ADRV9025_DUALBAND_SWITCHING_ENABLED     = 0x01
} adi_adrv9025_DualbandStatus_e;

/**
* \brief Data structure to hold Tx Signal Path Control configurations
*/
typedef struct adi_adrv9025_TxRadioCtrlModeCfg
{
    adi_adrv9025_TxEnableMode_e txEnableMode;  /*!< Configuration for Tx Channel Enable mode */
    uint32_t                    txChannelMask; /*!< Tx channels for which configuration is valid */
} adi_adrv9025_TxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Rx Signal Path Control configurations
*/
typedef struct adi_adrv9025_RxRadioCtrlModeCfg
{
    adi_adrv9025_RxEnableMode_e rxEnableMode;  /*!< Configuration for Tx Channel Enable mode */
    uint32_t                    rxChannelMask; /*!< Tx channels for which configuration is valid */
} adi_adrv9025_RxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold ORx Signal Path Control configurations
*/
typedef struct adi_adrv9025_ORxRadioCtrlModeCfg
{
    adi_adrv9025_ORxEnableMode_e orxEnableMode;                                 /*!< Configuration for ORx Channel Enable mode */
    uint8_t                      orxPinSelectSettlingDelay_armClkCycles;        /*!< Amount of time for the firmware to wait before sampling ORx Enable Pins */
    /* uint8_t enableOrxLowToHighStreams; */                                    /*!< 1 - Forces use of a specified ORx High and ORx Low streams, 0 - default channel streams are used */
    adi_adrv9025_SingleChannelPinModeOrxSel_e singleChannel1PinModeOrxSel;      /*!< ORx channel select in single channel 1 pin ORx enable mode */
    adi_adrv9025_SingleChannelPinModeOrxSel_e singleChannel2PinModeLowOrxSel;   /*!< ORx channel select when ORx_ENABLE[1] pin is low in single channel 2 pin ORx enable mode */
    adi_adrv9025_SingleChannelPinModeOrxSel_e singleChannel2PinModeHighOrxSel;  /*!< ORx channel select when ORx_ENABLE[1] pin is high in single channel 2 pin ORx enable mode */
    adi_adrv9025_DualChannelPinModeOrxSel_e dualChannel2PinModeOrxSel;          /*!< ORx channel select in dual channel 2 pin ORx enable mode */
} adi_adrv9025_ORxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Signal Path Control configurations
*/
typedef struct adi_adrv9025_RadioCtrlModeCfg
{
    adi_adrv9025_TxRadioCtrlModeCfg_t  txRadioCtrlModeCfg;  /*!< Tx signal path enable mode configuration */
    adi_adrv9025_RxRadioCtrlModeCfg_t  rxRadioCtrlModeCfg;  /*!< Rx signal path enable mode configuration */
    adi_adrv9025_ORxRadioCtrlModeCfg_t orxRadioCtrlModeCfg; /*!< ORx signal path enable mode configuration */
} adi_adrv9025_RadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Tx to Orx mapping
*/
typedef struct adi_adrv9025_TxToOrxMappingConfig
{
    adi_adrv9025_TxToOrx1Mapping_e orx1Map; /*!< Selects Tx feedback to ORx1 input */
    adi_adrv9025_TxToOrx2Mapping_e orx2Map; /*!< Selects Tx feedback to ORx2 input */
    adi_adrv9025_TxToOrx3Mapping_e orx3Map; /*!< Selects Tx feedback to ORx3 input */
    adi_adrv9025_TxToOrx4Mapping_e orx4Map; /*!< Selects Tx feedback to ORx4 input */
} adi_adrv9025_TxToOrxMappingConfig_t;

/**
* \brief Data structure to hold the PLL settings
*/
typedef struct adi_adrv9025_PllConfig
{
    adi_adrv9025_PllName_e               pllName;               /*!< Select the target RF LO PLL source */
    adi_adrv9025_PllModeSel_e            pllModeSel;            /*!< PLL Mode select - Slow vs Fast mode */
    adi_adrv9025_PllAuxLoResyncSel_e     pllAuxLoResyncSel;     /* Aux LO Resync with Tx LO */
    adi_adrv9025_PllAuxLoOffsetProgSel_e pllAuxLoOffsetProgSel; /* Select if Aux LO is to be programmed with offset LO when programming Tx LO*/
    uint64_t                             pllLoFrequency_Hz;     /*!< Desired RF LO frequency in Hz */
} adi_adrv9025_PllConfig_t;

/**
* \brief Data structure to hold Synthesizer PLL Loop filter settings
*/
typedef struct adi_adrv9025_PllLoopFilterCfg
{
    uint16_t loopBandwidth_kHz;   /*!< Synthesizer PLL Loop filter bandwidth. Range TBD */
    uint8_t  phaseMargin_degrees; /*< Synthesizer PLL Loop filter phase margin in degrees. Range 40-85 */
    uint8_t  powerScale;          /*!< Synthesizer PLL Loop filter power scale. Range 0 - 10. Default is 10 */
} adi_adrv9025_PllLoopFilterCfg_t;

/**
*brief Data Structure to hold stream version settings
*/
typedef struct adi_adrv9025_StreamVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t mainVer;
    uint8_t buildVer;
} adi_adrv9025_StreamVersion_t;

#define ADRV9025_IMAGEINFOSIZE 4
#define ADRV9025_MAINSTREAMSIZEMAX 4*1024
#define ADRV9025_TXSTREAMSIZEMAX 1024
#define ADRV9025_RXSTREAMSIZEMAX 1024
#define ADRV9025_ORXSTREAMSIZEMAX 4*1024
#define ADRV9025_MAINNUM 1
#define ADRV9025_TXNUM 4
#define ADRV9025_RXNUM 4
#define ADRV9025_ORXNUM 2
#define ADRV9025_STREAM_MAX 11

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_RADIOCTRL_TYPES_H_ */
