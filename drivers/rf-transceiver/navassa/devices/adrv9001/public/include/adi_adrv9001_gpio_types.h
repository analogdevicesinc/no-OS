/**
 * \file
 * \brief Contains functions to allow control of the General Purpose IO functions on the ADRV9001 device
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_GPIO_TYPES_H_
#define _ADI_ADRV9001_GPIO_TYPES_H_

#include <adi_adrv9001_user.h>
#include "adi_adrv9001_types.h"

/**
 *  \brief Enumeration of GPIO pins
 */
typedef enum adi_adrv9001_GpioPin
{
    ADI_ADRV9001_GPIO_UNASSIGNED,       /*!< No GPIO pin selected */
    ADI_ADRV9001_GPIO_DIGITAL_00,       /*!< Digital GPIO Pin 0 */
    ADI_ADRV9001_GPIO_DIGITAL_01,       /*!< Digital GPIO Pin 1 */
    ADI_ADRV9001_GPIO_DIGITAL_02,       /*!< Digital GPIO Pin 2 */
    ADI_ADRV9001_GPIO_DIGITAL_03,       /*!< Digital GPIO Pin 3 */
    ADI_ADRV9001_GPIO_DIGITAL_04,       /*!< Digital GPIO Pin 4 */
    ADI_ADRV9001_GPIO_DIGITAL_05,       /*!< Digital GPIO Pin 5 */
    ADI_ADRV9001_GPIO_DIGITAL_06,       /*!< Digital GPIO Pin 6 */
    ADI_ADRV9001_GPIO_DIGITAL_07,       /*!< Digital GPIO Pin 7 */
    ADI_ADRV9001_GPIO_DIGITAL_08,       /*!< Digital GPIO Pin 8 */
    ADI_ADRV9001_GPIO_DIGITAL_09,       /*!< Digital GPIO Pin 9 */
    ADI_ADRV9001_GPIO_DIGITAL_10,       /*!< Digital GPIO Pin 10 */
    ADI_ADRV9001_GPIO_DIGITAL_11,       /*!< Digital GPIO Pin 11 */
    ADI_ADRV9001_GPIO_DIGITAL_12,       /*!< Digital GPIO Pin 12 */
    ADI_ADRV9001_GPIO_DIGITAL_13,       /*!< Digital GPIO Pin 13 */
    ADI_ADRV9001_GPIO_DIGITAL_14,       /*!< Digital GPIO Pin 14 */
    ADI_ADRV9001_GPIO_DIGITAL_15,       /*!< Digital GPIO Pin 15 */
    ADI_ADRV9001_GPIO_ANALOG_00,        /*!< Analog GPIO Pin 0 */
    ADI_ADRV9001_GPIO_ANALOG_01,        /*!< Analog GPIO Pin 1 */
    ADI_ADRV9001_GPIO_ANALOG_02,        /*!< Analog GPIO Pin 2 */
    ADI_ADRV9001_GPIO_ANALOG_03,        /*!< Analog GPIO Pin 3 */
    ADI_ADRV9001_GPIO_ANALOG_04,        /*!< Analog GPIO Pin 4 */
    ADI_ADRV9001_GPIO_ANALOG_05,        /*!< Analog GPIO Pin 5 */
    ADI_ADRV9001_GPIO_ANALOG_06,        /*!< Analog GPIO Pin 6 */
    ADI_ADRV9001_GPIO_ANALOG_07,        /*!< Analog GPIO Pin 7 */
    ADI_ADRV9001_GPIO_ANALOG_08,        /*!< Analog GPIO Pin 8 */
    ADI_ADRV9001_GPIO_ANALOG_09,        /*!< Analog GPIO Pin 9 */
    ADI_ADRV9001_GPIO_ANALOG_10,        /*!< Analog GPIO Pin 10 */
    ADI_ADRV9001_GPIO_ANALOG_11,        /*!< Analog GPIO Pin 11 */
} adi_adrv9001_GpioPin_e;
    
typedef enum adi_adrv9001_GpioPinCrumbSel
{
    ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
    ADI_ADRV9001_GPIO_PIN_CRUMB_01_00,
    ADI_ADRV9001_GPIO_PIN_CRUMB_03_02,
    ADI_ADRV9001_GPIO_PIN_CRUMB_05_04,
    ADI_ADRV9001_GPIO_PIN_CRUMB_07_06,
    ADI_ADRV9001_GPIO_PIN_CRUMB_09_08,
    ADI_ADRV9001_GPIO_PIN_CRUMB_11_10,
    ADI_ADRV9001_GPIO_PIN_CRUMB_13_12,
    ADI_ADRV9001_GPIO_PIN_CRUMB_15_14,
} adi_adrv9001_GpioPinCrumbSel_e;

/**
*  \brief Enum to select desired Tx1 SSI GPIO pins used by the API
*/
typedef enum adi_adrv9001_GpioTx1SsiPinSel
{
    ADI_ADRV9001_GPIO_TX1_SSI_00 = 0,  /*!< Select GPIO_Tx1_SSI_00*/
    ADI_ADRV9001_GPIO_TX1_SSI_01,      /*!< Select GPIO_Tx1_SSI_01*/
    ADI_ADRV9001_GPIO_TX1_SSI_02,      /*!< Select GPIO_Tx1_SSI_02*/
    ADI_ADRV9001_GPIO_TX1_SSI_03,      /*!< Select GPIO_Tx1_SSI_03*/
    ADI_ADRV9001_GPIO_TX1_SSI_04,      /*!< Select GPIO_Tx1_SSI_04*/
    ADI_ADRV9001_GPIO_TX1_SSI_05,      /*!< Select GPIO_Tx1_SSI_05*/
    ADI_ADRV9001_GPIO_TX1_SSI_06,      /*!< Select GPIO_Tx1_SSI_06*/
    ADI_ADRV9001_GPIO_TX1_SSI_07,      /*!< Select GPIO_Tx1_SSI_07*/    
    ADI_ADRV9001_GPIO_TX1_SSI_INVALID  /*!< Invalid Tx1 SSI GPIO*/
} adi_adrv9001_GpioTx1SsiPinSel_e;

/**
*  \brief Enum to select desired Tx2 SSI GPIO pins used by the API
*/
typedef enum adi_adrv9001_GpioTx2SsiPinSel
{
    ADI_ADRV9001_GPIO_TX2_SSI_00 = 0,  /*!< Select GPIO_Tx2_SSI_00*/
    ADI_ADRV9001_GPIO_TX2_SSI_01,      /*!< Select GPIO_Tx2_SSI_01*/
    ADI_ADRV9001_GPIO_TX2_SSI_02,      /*!< Select GPIO_Tx2_SSI_02*/
    ADI_ADRV9001_GPIO_TX2_SSI_03,      /*!< Select GPIO_Tx2_SSI_03*/
    ADI_ADRV9001_GPIO_TX2_SSI_04,      /*!< Select GPIO_Tx2_SSI_04*/
    ADI_ADRV9001_GPIO_TX2_SSI_05,      /*!< Select GPIO_Tx2_SSI_05*/
    ADI_ADRV9001_GPIO_TX2_SSI_06,      /*!< Select GPIO_Tx2_SSI_06*/
    ADI_ADRV9001_GPIO_TX2_SSI_07,      /*!< Select GPIO_Tx2_SSI_07*/    
    ADI_ADRV9001_GPIO_TX2_SSI_INVALID  /*!< Invalid TxX2 SSI GPIO*/
} adi_adrv9001_GpioTx2SsiPinSel_e;

/**
*  \brief Enum to select desired Rx1 SSI GPIO pins used by the API
*/
typedef enum adi_adrv9001_GpioRx1SsiPinSel
{
    ADI_ADRV9001_GPIO_RX1_SSI_00 = 0,  /*!< Select GPIO_Rx1_SSI_00*/
    ADI_ADRV9001_GPIO_RX1_SSI_01,      /*!< Select GPIO_Rx1_SSI_01*/
    ADI_ADRV9001_GPIO_RX1_SSI_02,      /*!< Select GPIO_Rx1_SSI_02*/
    ADI_ADRV9001_GPIO_RX1_SSI_03,      /*!< Select GPIO_Rx1_SSI_03*/
    ADI_ADRV9001_GPIO_RX1_SSI_04,      /*!< Select GPIO_Rx1_SSI_04*/
    ADI_ADRV9001_GPIO_RX1_SSI_05,      /*!< Select GPIO_Rx1_SSI_05*/
    ADI_ADRV9001_GPIO_RX1_SSI_06,      /*!< Select GPIO_Rx1_SSI_06*/
    ADI_ADRV9001_GPIO_RX1_SSI_07,      /*!< Select GPIO_Rx1_SSI_07*/    
    ADI_ADRV9001_GPIO_RX1_SSI_INVALID  /*!< Invalid Rx1 SSI GPIO*/
} adi_adrv9001_GpioRx1SsiPinSel_e;

/**
*  \brief Enum to select desired Rx2 SSI GPIO pins used by the API
*/
typedef enum adi_adrv9001_GpioRx2SsiPinSel
{
    ADI_ADRV9001_GPIO_RX2_SSI_00 = 0,  /*!< Select GPIO_Rx2_SSI_00*/
    ADI_ADRV9001_GPIO_RX2_SSI_01,      /*!< Select GPIO_Rx2_SSI_01*/
    ADI_ADRV9001_GPIO_RX2_SSI_02,      /*!< Select GPIO_Rx2_SSI_02*/
    ADI_ADRV9001_GPIO_RX2_SSI_03,      /*!< Select GPIO_Rx2_SSI_03*/
    ADI_ADRV9001_GPIO_RX2_SSI_04,      /*!< Select GPIO_Rx2_SSI_04*/
    ADI_ADRV9001_GPIO_RX2_SSI_05,      /*!< Select GPIO_Rx2_SSI_05*/
    ADI_ADRV9001_GPIO_RX2_SSI_06,      /*!< Select GPIO_Rx2_SSI_06*/
    ADI_ADRV9001_GPIO_RX2_SSI_07,      /*!< Select GPIO_Rx2_SSI_07*/    
    ADI_ADRV9001_GPIO_RX2_SSI_INVALID  /*!< Invalid Rx2 SSI GPIO*/
} adi_adrv9001_GpioRx2SsiPinSel_e;

typedef enum adi_adrv9001_GpioPinDirection
{
    ADI_ADRV9001_GPIO_PIN_DIRECTION_INPUT,
    ADI_ADRV9001_GPIO_PIN_DIRECTION_OUTPUT,
} adi_adrv9001_GpioPinDirection_e;

typedef enum adi_adrv9001_GpioPinLevel
{
    ADI_ADRV9001_GPIO_PIN_LEVEL_LOW,
    ADI_ADRV9001_GPIO_PIN_LEVEL_HIGH,
} adi_adrv9001_GpioPinLevel_e;

/**
 * \brief Enumeration of signals from the BBIC to ARM.
 *   These signals could be mapped to various GPIO pins. The GPIO_CTRL
 *    mailbox command is used to configure the GPIO pin to signal mapping.
 */
typedef enum adi_adrv9001_GpioSignal
{
    /* Digital GPIO Functions */
    ADI_ADRV9001_GPIO_SIGNAL_ORX_ENABLE_1,                  /*!< ORx Enable signal channel 1 */
    ADI_ADRV9001_GPIO_SIGNAL_ORX_ENABLE_2,                  /*!< ORx Enable signal channel 2 */
    ADI_ADRV9001_GPIO_SIGNAL_MON_ENABLE_SPS,                /*!< Monitor mode enable and System Power Saving request signal */
    ADI_ADRV9001_GPIO_SIGNAL_MON_BBIC_WAKEUP,               /*!< Monitor mode signal to wake up the BBIC */
    ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL1,         /*!< Power saving signal for channel 1 */
    ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL2,         /*!< Power saving signal for channel 2 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP,                        /*!< Frequency hopping hop request signal */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_0,       /*!< Frequency hopping Channel1 gain/atten select bit 0 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_1,       /*!< Frequency hopping Channel1 gain/atten select bit 1 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_2,       /*!< Frequency hopping Channel1 gain/atten select bit 2 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_0,       /*!< Frequency hopping Channel2 gain/atten select bit 0 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_1,       /*!< Frequency hopping Channel2 gain/atten select bit 1 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_2,       /*!< Frequency hopping Channel2 gain/atten select bit 2 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_0,              /*!< Frequency hopping frequency index select bit 0 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_1,              /*!< Frequency hopping frequency index select bit 1 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_2,              /*!< Frequency hopping frequency index select bit 2 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_3,              /*!< Frequency hopping frequency index select bit 3 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_4,              /*!< Frequency hopping frequency index select bit 4 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_TABLE_INDEX_5,              /*!< Frequency hopping frequency index select bit 5 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_TABLE_SELECT,           /*!< Frequency hopping Hop table select signal */
    ADI_ADRV9001_GPIO_SIGNAL_TX1_PA_RAMP_CTRL,              /*!< Tx1 Aux DAC ramp control request signal*/
    ADI_ADRV9001_GPIO_SIGNAL_TX2_PA_RAMP_CTRL,              /*!< Tx2 Aux DAC ramp control request signal*/

    ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL1,        /*!< ADC switching gpio based data capture for Channel 1 */
    ADI_ADRV9001_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL2,        /*!< ADC switching gpio based data capture for Channel 2 */

    /* Analog GPIO Functions */
    ADI_ADRV9001_GPIO_SIGNAL_TX1_EXT_FRONTEND_CONTROL,      /*!< Tx1 FrontEnd control signal */
    ADI_ADRV9001_GPIO_SIGNAL_TX2_EXT_FRONTEND_CONTROL,      /*!< Tx2 FrontEnd control signal */
    ADI_ADRV9001_GPIO_SIGNAL_RX1_EXT_FRONTEND_CONTROL,      /*!< Rx1 FrontEnd control signal */
    ADI_ADRV9001_GPIO_SIGNAL_RX2_EXT_FRONTEND_CONTROL,      /*!< Rx2 FrontEnd control signal */
    ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_1_LOCK,                /*!< External PLL 1 lock signal */
    ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_2_LOCK,                /*!< External PLL 2 lock signal */
    ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_1_CE,                  /*!< External PLL 1 Chip Enable signal */
    ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_2_CE,                  /*!< External PLL 2 Chip Enable signal */
    ADI_ADRV9001_GPIO_SIGNAL_RX_VCO_1_CE,                   /*!< Rx VCO 1 Chip Enable signal */
    ADI_ADRV9001_GPIO_SIGNAL_RX_VCO_2_CE,                   /*!< Rx VCO 2 Chip Enable signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_0,                     /*!< Aux DAC control 0 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_1,                     /*!< Aux DAC control 1 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_2,                     /*!< Aux DAC control 2 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_3,                     /*!< Aux DAC control 3 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_ADC_0,                     /*!< Aux ADC control 0 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_ADC_1,                     /*!< Aux ADC control 1 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_ADC_2,                     /*!< Aux ADC control 2 signal */
    ADI_ADRV9001_GPIO_SIGNAL_AUX_ADC_3,                     /*!< Aux ADC control 3 signal */

    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2,                      /*!< Frequency hopping hop request signal   */
    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP_2_TABLE_SELECT,         /*!< Frequency hopping table select for HOP 2 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP1_NCO_ASYNC_CHANGE,      /*!< Asynchronously change NCO for Hop1 */
    ADI_ADRV9001_GPIO_SIGNAL_FH_HOP2_NCO_ASYNC_CHANGE,      /*!< Asynchronously change NCO for Hop2 */
    ADI_ADRV9001_GPIO_SIGNAL_RX1_INTERFACEGAIN_SEED_SAVE = 52,   /*!< Seed (rising edge) or save (falling edge) the RX1 InterfaceGain */
    ADI_ADRV9001_GPIO_SIGNAL_RX2_INTERFACEGAIN_SEED_SAVE = 53,   /*!< Seed (rising edge) or save (falling edge) the RX2 InterfaceGain  */
	ADI_ADRV9001_GPIO_SIGNAL_RX1_AGC_FREEZE = 54,                /*!< Asynchronous update Rx1 AGC freeze  */
	ADI_ADRV9001_GPIO_SIGNAL_RX2_AGC_FREEZE = 55,                /*!< Asynchronous update Rx2 AGC freeze  */
	
    ADI_ADRV9001_GPIO_NUM_SIGNALS = 56,                     /*!< Total Number of signals from BBIC*/
} adi_adrv9001_GpioSignal_e;

/**
* \brief GP_INT status general structure
*/
typedef struct adi_adrv9001_gpIntStatus
{
    uint32_t gpIntStatus;
    uint32_t gpIntMask;
    uint32_t gpIntActiveSources;
    uint32_t gpIntSaveIrqMask;
} adi_adrv9001_gpIntStatus_t;

/**
 * \brief Enumeration of GPIO signal polarity 
 **/
typedef enum adi_adrv9001_GpioPolarity
{
    ADI_ADRV9001_GPIO_POLARITY_NORMAL = 0x00,      /*!< Normal polarity */
    ADI_ADRV9001_GPIO_POLARITY_INVERTED = 0x01,    /*!< Inverted polarity */
} adi_adrv9001_GpioPolarity_e;

typedef enum adi_adrv9001_GpioMaster
{
    ADI_ADRV9001_GPIO_MASTER_BBIC = 0x00,
    ADI_ADRV9001_GPIO_MASTER_ADRV9001 = 0x02
} adi_adrv9001_GpioMaster_e;

/**
 * \brief GPIO pin configuration
 */
typedef struct adi_adrv9001_GpioCfg
{
    adi_adrv9001_GpioPin_e pin;                 /*!< The GPIO pin */
    adi_adrv9001_GpioPolarity_e polarity;       /*!< Polarity of the GPIO pin (normal or inverted) */
    adi_adrv9001_GpioMaster_e master;           /*!< Whether BBIC or ADRV9001 controls this pin
    adi_adrv9001_GpioMaster_e master;            *   TODO: JS: Can't this be inferred from the signal and/or direction (in/out)? */
} adi_adrv9001_GpioCfg_t;

/**
 * \brief Initial GPIO configuration
 */
typedef struct adi_adrv9001_GpioCtrlInitCfg
{
    adi_adrv9001_GpioCfg_t tx_ext_frontend_ctrl[ADI_ADRV9001_NUM_PORTS];            /*!< (AGPIO) Tx FrontEnd controls */
    adi_adrv9001_GpioCfg_t rx_ext_frontend_ctrl[ADI_ADRV9001_NUM_PORTS];            /*!< (AGPIO) Rx FrontEnd controls */
    adi_adrv9001_GpioCfg_t ext_pll_chip_enable[ADI_ADRV9001_NUM_PORTS];             /*!< (AGPIO) External PLL Chip Enables */
    adi_adrv9001_GpioCfg_t vco_chip_enable[ADI_ADRV9001_NUM_PORTS];                 /*!< (AGPIO) Rx VCO Chip Enables */
    adi_adrv9001_GpioCfg_t ext_pll_lock[ADI_ADRV9001_NUM_PORTS];                    /*!< (AGPIO) External PLL locks */
    adi_adrv9001_GpioCfg_t channelPowerSaving[ADI_ADRV9001_NUM_CHANNELS];           /*!< (DGPIO) Channel Power Saving Enables */
    adi_adrv9001_GpioCfg_t systemPowerSavingAndMonitorEnable;                       /*!< (DGPIO) System Power Saving and Monitor Enable */
    adi_adrv9001_GpioCfg_t systemPowerSavingAndMonitorWakeUp;                       /*!< (DGPIO) Monitor WakeUp */
	adi_adrv9001_GpioCfg_t fh_update_rx_nco[ADI_ADRV9001_NUM_CHANNELS];             /*!< (DGPIO) Trigger Update of Rx NCO in Frequency Hop Frame */
    adi_adrv9001_GpioCfg_t rx_interfaceGain_seed_save[ADI_ADRV9001_NUM_CHANNELS];   /*!< (DGPIO) Trigger Seed (RisingEdge) or Save (FallingEdge) of interfaceGain */

} adi_adrv9001_GpioCtrlInitCfg_t;

#endif /* _ADI_ADRV9001_GPIO_TYPES_H_ */
