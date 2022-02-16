/**
* \file
* \brief Contains ADRV9001 API Rx datapath data types
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_RX_TYPES_H_
#define _ADI_ADRV9001_RX_TYPES_H_

#include "adi_adrv9001_gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#define ADI_ADRV9001_RX_GAIN_INDEX_MIN 187
#define ADI_ADRV9001_RX_GAIN_INDEX_MAX 255

/**
*  \brief Rx gain table SRAM base addresses
*/
typedef enum adi_adrv9001_RxGainTableBaseAddr_e
{
    ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_1 = 0x73300000,  /*!< Select gain table SRAM base address for Rx1 Channel */
    ADI_ADRV9001_RX_GAIN_TABLE_BASE_ADDR_2 = 0x73400000,  /*!< Select gain table SRAM base address for Rx2 Channel */
} adi_adrv9001_RxGainTableBaseAddr_e;

/**
 *  \brief Supported gain control modes
 */
typedef enum adi_adrv9001_RxGainControlMode
{
    ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI,   /*!< Explicitly set the gain index via SPI */
    ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN,   /*!< Increment and decrement the gain index by pulsing GPIO pins */
    ADI_ADRV9001_RX_GAIN_CONTROL_MODE_AUTO   /*!< ADRV9001 Gain Control block sets the gain index based on thresholds */
} adi_adrv9001_RxGainControlMode_e;

/**
 * \brief Data structure to hold ADRV9001 Rx gain table row entry
 */
typedef struct adi_adrv9001_RxGainTableRow
{
    uint8_t rxFeGain;     /*!< Rx Front End gain for a given gain index */
    uint8_t extControl;   /*!< External LNA control word */
    uint8_t adcTiaGain;   /*!< ADC and TIA control for a given gain index */
    int16_t digGain;      /*!< Digital gain ranging from -18dB to 50dB (68dB total range) */
    uint16_t phaseOffset; /*!< 16 bit phase offset from 0 - 2pi in resolution of 0.005 degrees */
} adi_adrv9001_RxGainTableRow_t;

/**
*  \brief Enum to determine Rx interface gain control type
*/
typedef enum adi_adrv9001_RxInterfaceGainCtrlMode
{
    ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_AUTOMATIC = 0x00,
    /*!< Use internal Rx interface gain value. */
    ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_MANUAL = 0x01,
    /*!< Use external Rx interface gain value. Gain value has to be provided in this case. */
} adi_adrv9001_RxInterfaceGainCtrlMode_e;

/**
*  \brief Enum to determine Rx interface gain value
*/
typedef enum adi_adrv9001_RxInterfaceGain
{
    ADI_ADRV9001_RX_INTERFACE_GAIN_18_DB = 0,       /*!<  18 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_12_DB,           /*!<  12 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_6_DB,            /*!<   6 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_0_DB,            /*!<   0 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_6_DB,   /*!<  -6 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_12_DB,  /*!< -12 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_18_DB,  /*!< -18 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_24_DB,  /*!< -24 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_30_DB,  /*!< -30 dB */
    ADI_ADRV9001_RX_INTERFACE_GAIN_NEGATIVE_36_DB,  /*!< -36 dB */
} adi_adrv9001_RxInterfaceGain_e;

/**
*  \brief Enum to select desired manual gain peak signals for rxGainCtrlFeedbackMask in adrv9001_RxGainCtrlFeedbackCfg_t
*/
typedef enum adi_adrv9001_RxManualGainPeakSignalSel
{
    ADI_ADRV9001_RX_GAINCONTROL_OVRG_LOW_COUNTER_EXCEEDED    = 0x0001,
    ADI_ADRV9001_RX_GAINCONTROL_OVRG_LLB_COUNTER_EXCEEDED    = 0x0002,
    ADI_ADRV9001_RX_GAINCONTROL_OVRG_HIGH_COUNTER_EXCEEDED   = 0x0004,
    ADI_ADRV9001_RX_GAINCONTROL_OVRG_ULB_COUNTER_EXCEEDED_0  = 0x0008,
} adi_adrv9001_RxManualGainPeakSignalSel_e;

/**
*  \brief Enum to select desired manual gain peak power signals for rxGainCtrlFeedbackMask in adrv9001_RxGainCtrlFeedbackCfg_t
*/
typedef enum adi_adrv9001_rxManualGainPeakPowerSignalSel
{
    ADI_ADRV9001_RX_GT_GAIN_CHANGE_0                 = 0x0001,
    ADI_ADRV9001_RX_GAINCONTROL_SL_LOW_TH_EXCEEDED           = 0x0002,
    ADI_ADRV9001_RX_GAINCONTROL_SL_HIGH_TH_EXCEEDED          = 0x0004,
    ADI_ADRV9001_RX_GAINCONTROL_OVRG_ULB_COUNTER_EXCEEDED_1  = 0x0008,
} adi_adrv9001_RxManualGainPeakPowerSignalSel_e;

/**
*  \brief Enum of Rx interface gain control update instance
*/
    typedef enum adi_adrv9001_RxInterfaceGainUpdateTiming
{
    ADI_ADRV9001_RX_INTERFACE_GAIN_UPDATE_TIMING_NEXT_FRAME = 0x00,
    /*!< Update Rx interface gain control at start of next frame */
    ADI_ADRV9001_RX_INTERFACE_GAIN_UPDATE_TIMING_NOW = 0x01,
    /*!< Update Rx interface gain control immediately  */
} adi_adrv9001_RxInterfaceGainUpdateTiming_e;

/**
* \brief Data structure to set/get the Rx interface gain control parameters for the given Rx channel.
*/
typedef struct adi_adrv9001_RxInterfaceGainCtrl
{
    adi_adrv9001_RxInterfaceGainUpdateTiming_e  updateInstance;
    adi_adrv9001_RxInterfaceGainCtrlMode_e      controlMode;
    adi_adrv9001_RxInterfaceGain_e              gain;
    uint8_t rssiDuration;                                           /* Duration of RSSI measurement (unit = 1ms/255 ) */
    uint8_t rssiMovingAverageDuration;                              /* Number of measurements in RSSI Moving-Average window */
    uint8_t reserved1;
    uint8_t reserved2;

} adi_adrv9001_RxInterfaceGainCtrl_t;

typedef enum adi_adrv9001_AdcTypeSwitchMode
{
    ADI_ADRV9001_ADC_SWITCH_IMMEDIATE,
    ADI_ADRV9001_ADC_SWITCH_PINCONTROL_ENABLED,
    ADI_ADRV9001_ADC_SWITCH_PINCONTROL_DISABLED
} adi_adrv9001_AdcTypeSwitchMode_e;

/**
 * \brief Structure which holds the ADC switch configuration
 */
typedef struct adi_adrv9001_AdcSwitchCfg
{
    adi_adrv9001_AdcTypeSwitchMode_e adcSwitchMode;
    adi_adrv9001_GpioCfg_t adcSwitchGpio;
} adi_adrv9001_AdcSwitchCfg_t;

/**
 * \brief Structure which holds the DGPIO pin selection to route gain index
 *        4 pairs of DGPIO pins - ADI_ADRV9001_GPIO_PIN_CRUMB_XX_YY to route 8 bits of gain index
 */
typedef struct adi_adrv9001_GainIndexPinCfg
{
    adi_adrv9001_GpioPinCrumbSel_e gainIndex_01_00;
    adi_adrv9001_GpioPinCrumbSel_e gainIndex_03_02;
    adi_adrv9001_GpioPinCrumbSel_e gainIndex_05_04;
    adi_adrv9001_GpioPinCrumbSel_e gainIndex_07_06;
} adi_adrv9001_GainIndexPinCfg_t;

/**
 * \brief Structure which holds the ADC switch configuration
 */
typedef struct adi_adrv9001_RxPortSwitchCfg
{
    uint64_t  minFreqPortA_Hz;
    uint64_t  maxFreqPortA_Hz;
    uint64_t  minFreqPortB_Hz;
    uint64_t  maxFreqPortB_Hz;
    bool      enable;
    bool      manualRxPortSwitch;
} adi_adrv9001_RxPortSwitchCfg_t;
    
/**
 * \brief Structure which holds the LOID configuration parameters
 */
typedef struct adi_adrv9001_RxrfdcLoidCfg
{
	bool loidEnable;								/* LOID enable flag for RX1 and RX2 */
	uint8_t loidThreshold_negdBFS;				    /* Threshold for LO detection (in -dBFS) */  
} adi_adrv9001_RxrfdcLoidCfg_t ;	
    
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_RX_TYPES_H_ */
