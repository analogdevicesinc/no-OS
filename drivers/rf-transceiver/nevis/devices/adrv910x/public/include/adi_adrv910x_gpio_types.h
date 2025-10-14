/**
* \file
* \brief Contains ADRV910X API GPIO data types
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2023 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV910X_GPIO_TYPES_H_
#define _ADI_ADRV910X_GPIO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "adi_adrv910x_types.h"

/**
 *  \brief Enumeration of GPIO pins
 */
typedef enum adi_adrv910x_GpioPin
{
    ADI_ADRV910X_GPIO_UNASSIGNED,       /*!< No GPIO pin selected */
    ADI_ADRV910X_GPIO_DIGITAL_00,       /*!< Digital GPIO Pin 0 */
    ADI_ADRV910X_GPIO_DIGITAL_01,       /*!< Digital GPIO Pin 1 */
    ADI_ADRV910X_GPIO_DIGITAL_02,       /*!< Digital GPIO Pin 2 */
    ADI_ADRV910X_GPIO_DIGITAL_03,       /*!< Digital GPIO Pin 3 */
    ADI_ADRV910X_GPIO_DIGITAL_04,       /*!< Digital GPIO Pin 4 */
    ADI_ADRV910X_GPIO_DIGITAL_05,       /*!< Digital GPIO Pin 5 */
    ADI_ADRV910X_GPIO_DIGITAL_06,       /*!< Digital GPIO Pin 6 */
    ADI_ADRV910X_GPIO_DIGITAL_07,       /*!< Digital GPIO Pin 7 */
    ADI_ADRV910X_GPIO_DIGITAL_08,       /*!< Digital GPIO Pin 8 */
    ADI_ADRV910X_GPIO_DIGITAL_09,       /*!< Digital GPIO Pin 9 */
    ADI_ADRV910X_GPIO_DIGITAL_10,       /*!< Digital GPIO Pin 10 */
    ADI_ADRV910X_GPIO_DIGITAL_11,       /*!< Digital GPIO Pin 11 */
    ADI_ADRV910X_GPIO_DIGITAL_12,       /*!< Digital GPIO Pin 12 */
    ADI_ADRV910X_GPIO_DIGITAL_13,       /*!< Digital GPIO Pin 13 */
    ADI_ADRV910X_GPIO_DIGITAL_14,       /*!< Digital GPIO Pin 14 */
    ADI_ADRV910X_GPIO_DIGITAL_15,       /*!< Digital GPIO Pin 15 */
    ADI_ADRV910X_GPIO_ANALOG_00,        /*!< Analog GPIO Pin 0 */
    ADI_ADRV910X_GPIO_ANALOG_01,        /*!< Analog GPIO Pin 1 */
    ADI_ADRV910X_GPIO_ANALOG_02,        /*!< Analog GPIO Pin 2 */
    ADI_ADRV910X_GPIO_ANALOG_03,        /*!< Analog GPIO Pin 3 */
    ADI_ADRV910X_GPIO_ANALOG_04,        /*!< Analog GPIO Pin 4 */
    ADI_ADRV910X_GPIO_ANALOG_05,        /*!< Analog GPIO Pin 5 */
    ADI_ADRV910X_GPIO_ANALOG_06,        /*!< Analog GPIO Pin 6 */
    ADI_ADRV910X_GPIO_ANALOG_07,        /*!< Analog GPIO Pin 7 */
    ADI_ADRV910X_GPIO_ANALOG_08,        /*!< Analog GPIO Pin 8 */
    ADI_ADRV910X_GPIO_ANALOG_09,        /*!< Analog GPIO Pin 9 */
    ADI_ADRV910X_GPIO_ANALOG_10,        /*!< Analog GPIO Pin 10 */
    ADI_ADRV910X_GPIO_ANALOG_11,        /*!< Analog GPIO Pin 11 */
} adi_adrv910x_GpioPin_e;

/**
 *  \brief Enumeration of GPIO pin crumb selections
 */    
typedef enum adi_adrv910x_GpioPinCrumbSel
{
    ADI_ADRV910X_GPIO_PIN_CRUMB_UNASSIGNED,
    ADI_ADRV910X_GPIO_PIN_CRUMB_01_00,
    ADI_ADRV910X_GPIO_PIN_CRUMB_03_02,
    ADI_ADRV910X_GPIO_PIN_CRUMB_05_04,
    ADI_ADRV910X_GPIO_PIN_CRUMB_07_06,
    ADI_ADRV910X_GPIO_PIN_CRUMB_09_08,
    ADI_ADRV910X_GPIO_PIN_CRUMB_11_10,
    ADI_ADRV910X_GPIO_PIN_CRUMB_13_12,
    ADI_ADRV910X_GPIO_PIN_CRUMB_15_14,
} adi_adrv910x_GpioPinCrumbSel_e;

/**
 * \brief Enumeration of signals from the BBIC to ARM.
 *   These signals could be mapped to various GPIO pins. The GPIO_CTRL
 *    mailbox command is used to configure the GPIO pin to signal mapping.
 */
typedef enum adi_adrv910x_GpioSignal
{
	/* Continuous sync detect indication */
	ADI_ADRV910X_GPIO_SIGNAL_CONTINUOUS_SYNC_DETECTION, /*!< Sync Detection output */
	/* Digital GPIO Functions */
	ADI_ADRV910X_GPIO_SIGNAL_ORX_ENABLE_1,			/*!< ORx Enable signal channel 1 */
	ADI_ADRV910X_GPIO_SIGNAL_ORX_ENABLE_2,			/*!< ORx Enable signal channel 2 */
	ADI_ADRV910X_GPIO_SIGNAL_MON_ENABLE_SPS,		/*!< Monitor mode enable and System Power Saving request signal */
	ADI_ADRV910X_GPIO_SIGNAL_MON_BBIC_WAKEUP,		/*!< Monitor mode signal to wake up the BBIC */
	ADI_ADRV910X_GPIO_SIGNAL_POWER_SAVING_CHANNEL1, /*!< Power saving signal for channel 1 */
	ADI_ADRV910X_GPIO_SIGNAL_POWER_SAVING_CHANNEL2, /*!< Power saving signal for channel 2 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP,
	/*!< Frequency hopping hop request signal */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_0, /*!< Frequency hopping Channel1 gain/atten select bit 0 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_1, /*!< Frequency hopping Channel1 gain/atten select bit 1 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH1_GAIN_ATTEN_SEL_2, /*!< Frequency hopping Channel1 gain/atten select bit 2 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_0, /*!< Frequency hopping Channel2 gain/atten select bit 0 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_1, /*!< Frequency hopping Channel2 gain/atten select bit 1 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_CH2_GAIN_ATTEN_SEL_2,
	/*!< Frequency hopping Channel2 gain/atten select bit 2 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_0, /*!< Frequency hopping frequency index select bit 0 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_1, /*!< Frequency hopping frequency index select bit 1 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_2, /*!< Frequency hopping frequency index select bit 2 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_3, /*!< Frequency hopping frequency index select bit 3 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_4, /*!< Frequency hopping frequency index select bit 4 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_TABLE_INDEX_5,
	/*!< Frequency hopping frequency index select bit 5 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP_TABLE_SELECT, /*!< Frequency hopping Hop table select signal */
	ADI_ADRV910X_GPIO_SIGNAL_TX1_PA_RAMP_CTRL,	  /*!< Tx1 Aux DAC ramp control request signal*/
	ADI_ADRV910X_GPIO_SIGNAL_TX2_PA_RAMP_CTRL,	  /*!< Tx2 Aux DAC ramp control request signal*/

	ADI_ADRV910X_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL1, /*!< ADC switching gpio based data capture for Channel 1 */
	ADI_ADRV910X_GPIO_SIGNAL_ADC_SWITCHING_CHANNEL2,
	/*!< ADC switching gpio based data capture for Channel 2 */

	/* Analog GPIO Functions */
	ADI_ADRV910X_GPIO_SIGNAL_TX1_EXT_FRONTEND_CONTROL,
	/*!< Tx1 FrontEnd control signal */
	ADI_ADRV910X_GPIO_SIGNAL_TX2_EXT_FRONTEND_CONTROL, /*!< Tx2 FrontEnd control signal */
	ADI_ADRV910X_GPIO_SIGNAL_RX1_EXT_FRONTEND_CONTROL, /*!< Rx1 FrontEnd control signal */
	ADI_ADRV910X_GPIO_SIGNAL_RX2_EXT_FRONTEND_CONTROL, /*!< Rx2 FrontEnd control signal */
	ADI_ADRV910X_GPIO_SIGNAL_EXT_PLL_1_LOCK,		   /*!< External PLL 1 lock signal */
	ADI_ADRV910X_GPIO_SIGNAL_EXT_PLL_2_LOCK,		   /*!< External PLL 2 lock signal */
	ADI_ADRV910X_GPIO_SIGNAL_EXT_PLL_1_CE,
	/*!< External PLL 1 Chip Enable signal */
	ADI_ADRV910X_GPIO_SIGNAL_EXT_PLL_2_CE, /*!< External PLL 2 Chip Enable signal */
	ADI_ADRV910X_GPIO_SIGNAL_RX_VCO_1_CE,  /*!< Rx VCO 1 Chip Enable signal */
	ADI_ADRV910X_GPIO_SIGNAL_RX_VCO_2_CE,  /*!< Rx VCO 2 Chip Enable signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_DAC_0,	   /*!< Aux DAC control 0 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_DAC_1,	   /*!< Aux DAC control 1 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_DAC_2,
	/*!< Aux DAC control 2 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_DAC_3, /*!< Aux DAC control 3 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_ADC_0, /*!< Aux ADC control 0 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_ADC_1, /*!< Aux ADC control 1 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_ADC_2, /*!< Aux ADC control 2 signal */
	ADI_ADRV910X_GPIO_SIGNAL_AUX_ADC_3, /*!< Aux ADC control 3 signal */

	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP_2,
	/*!< Frequency hopping hop request signal   */
	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP_2_TABLE_SELECT, /*!< Frequency hopping table select for HOP 2 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP1_NCO_ASYNC_CHANGE, /*!< Asynchronously change NCO for Hop1 */
	ADI_ADRV910X_GPIO_SIGNAL_FH_HOP2_NCO_ASYNC_CHANGE, /*!< Asynchronously change NCO for Hop2 */
	ADI_ADRV910X_GPIO_SIGNAL_RX1_INTERFACEGAIN_SEED_SAVE = 52, /*!< Seed (rising edge) or save (falling edge) the RX1 InterfaceGain */
	ADI_ADRV910X_GPIO_SIGNAL_RX2_INTERFACEGAIN_SEED_SAVE = 53, /*!< Seed (rising edge) or save (falling edge) the RX2 InterfaceGain  */
	
	ADI_ADRV910X_GPIO_NUM_SIGNALS = 54, /*!< Total Number of signals from BBIC*/
} adi_adrv910x_GpioSignal_e;

/**
 * \brief Enumeration of GPIO signal polarity 
 **/
typedef enum adi_adrv910x_GpioPolarity
{
    ADI_ADRV910X_GPIO_POLARITY_NORMAL = 0x00,      /*!< Normal polarity */
    ADI_ADRV910X_GPIO_POLARITY_INVERTED = 0x01,    /*!< Inverted polarity */
} adi_adrv910x_GpioPolarity_e;

typedef enum adi_adrv910x_GpioMaster
{
    ADI_ADRV910X_GPIO_MASTER_BBIC = 0x00,
    ADI_ADRV910X_GPIO_MASTER_ADRV910X = 0x02
} adi_adrv910x_GpioMaster_e;

/**
 * \brief GPIO pin configuration
 */
typedef struct adi_adrv910x_GpioCfg
{
    adi_adrv910x_GpioPin_e pin;                 /*!< The GPIO pin */
    adi_adrv910x_GpioPolarity_e polarity;       /*!< Polarity of the GPIO pin (normal or inverted) */
    adi_adrv910x_GpioMaster_e master;           /*!< Whether BBIC or ADRV910X controls this pin */
} adi_adrv910x_GpioCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_GPIO_TYPES_H_ */
