/**
 * \file
 * \brief Type definitions for ADRV9001 calibrations
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_CALS_TYPES_H_
#define _ADI_ADRV9001_CALS_TYPES_H_

#include "adi_adrv9001_defines.h"

/**
 *  \brief Enum to select desired InitCals select bits in the initCalMask.
 *  \note Maskable
 */
typedef enum adi_adrv9001_InitCalibrations
{
    ADI_ADRV9001_INIT_CAL_TX_QEC                = 0x00000001, //!< Tx Quadrature Error Correction
    ADI_ADRV9001_INIT_CAL_TX_LO_LEAKAGE         = 0x00000002, //!< Tx LO Leakage
    ADI_ADRV9001_INIT_CAL_TX_LB_PD              = 0x00000004, //!< Tx Loopback path delay
    ADI_ADRV9001_INIT_CAL_TX_DCC                = 0x00000008, //!< Tx Duty Cycle Correction
    ADI_ADRV9001_INIT_CAL_TX_BBAF               = 0x00000010, //!< Tx Baseband Analog Filter
    ADI_ADRV9001_INIT_CAL_TX_BBAF_GD            = 0x00000020, //!< Tx Baseband Analog Filter Group Delay
    ADI_ADRV9001_INIT_CAL_TX_ATTEN_DELAY        = 0x00000040, //!< Tx Attenuation Delay
    ADI_ADRV9001_INIT_CAL_TX_DAC                = 0x00000080, //!< Tx DAC
    ADI_ADRV9001_INIT_CAL_TX_PATH_DELAY         = 0x00000100, //!< Tx Path Delay

    ADI_ADRV9001_INIT_CAL_RX_HPADC_RC           = 0x00000200, //!< Rx HP ADC Resistance and Capacitance
    ADI_ADRV9001_INIT_CAL_RX_HPADC_FLASH        = 0x00000400, //!< Rx HP ADC Flash
    ADI_ADRV9001_INIT_CAL_RX_HPADC_DAC          = 0x00000800, //!< Rx HP ADC DAC
    ADI_ADRV9001_INIT_CAL_RX_DCC                = 0x00001000, //!< Rx Duty Cycle Correction
    ADI_ADRV9001_INIT_CAL_RX_LPADC              = 0x00002000, //!< Rx LP ADC
    ADI_ADRV9001_INIT_CAL_RX_TIA_CUTOFF         = 0x00004000, //!< Rx Trans-Impedance Amplifier Cutoff
    ADI_ADRV9001_INIT_CAL_RX_GROUP_DELAY        = 0x00008000, //!< Rx Trans-Impedance Amplifier Group Delay
    ADI_ADRV9001_INIT_CAL_RX_QEC_TCAL           = 0x00010000, //!< Rx QEC Tone Calibration
    ADI_ADRV9001_INIT_CAL_RX_QEC_FIC            = 0x00020000, //!< Rx QEC Frequency-Independent
    ADI_ADRV9001_INIT_CAL_RX_QEC_ILB_LO_DELAY   = 0x00040000, //!< Rx Internal Loopback LO Delay
    ADI_ADRV9001_INIT_CAL_RX_RF_DC_OFFSET       = 0x00080000, //!< Rx RF DC Offset
    ADI_ADRV9001_INIT_CAL_RX_GAIN_PATH_DELAY    = 0x00100000, //!< Rx Gain Path Delay

    ADI_ADRV9001_INIT_CAL_PLL                   = 0x00200000, //!< PLL
    ADI_ADRV9001_INIT_CAL_AUX_PLL               = 0x00400000, //!< AUX PLL

    ADI_ADRV9001_INIT_CAL_TX_ALL                = 0x000001FF, //!< Tx all Init Cals
    ADI_ADRV9001_INIT_CAL_RX_ALL                = 0x001FFE00, //!< Rx all Init Cals
    ADI_ADRV9001_INIT_CAL_RX_TX_ALL             = 0x001FFFFF, //!< Rx / Tx all Init Cals
    ADI_ADRV9001_INIT_CAL_SYSTEM_ALL            = 0x00600000, //!< All system Init Cals
}adi_adrv9001_InitCalibrations_e;

/**
 *  \brief Enum to select desired TrackingCals select bits in the trackingCalMask.
 *  \note Maskable
 */
typedef enum adi_adrv9001_TrackingCalibrations
{
    /* SW and HW tracking cals */
    ADI_ADRV9001_TRACKING_CAL_TX_QEC            = 0x00000001, //!< Tx Quadrature Error Correction
    ADI_ADRV9001_TRACKING_CAL_TX_LO_LEAKAGE     = 0x00000002, //!< Tx LO Leakage
    ADI_ADRV9001_TRACKING_CAL_TX_LB_PD          = 0x00000004, //!< Tx Loopback path delay
    ADI_ADRV9001_TRACKING_CAL_TX_PAC            = 0x00000008, //!< Tx Power Amplifier Correction
    ADI_ADRV9001_TRACKING_CAL_TX_DPD            = 0x00000010, //!< Tx Digital Pre Distortion
    ADI_ADRV9001_TRACKING_CAL_TX_CLGC           = 0x00000020, //!< Tx Close Loop Gain Control
    /* Bit 6-7: Not used (Reserved for future purpose) */
    ADI_ADRV9001_TRACKING_CAL_RX_HD2            = 0x00000100, //!< Rx Harmonic Distortion
    ADI_ADRV9001_TRACKING_CAL_RX_QEC_WBPOLY     = 0x00000200, //!< Rx Quadrature Error Correction Wideband Poly
    /* Bit 10-11: Not used (Reserved for future purpose) */
    ADI_ADRV9001_TRACKING_CAL_ORX_QEC_WBPOLY    = 0x00001000, //!< ORx Quadrature Error Correction Wideband Poly
    /* Bit 13-18:  Not used (Reserved for future purpose) */
    ADI_ADRV9001_TRACKING_CAL_RX_BBDC           = 0x00080000, //!< Rx Baseband DC rejection
    ADI_ADRV9001_TRACKING_CAL_RX_RFDC           = 0x00100000, //!< Rx RF DC
    ADI_ADRV9001_TRACKING_CAL_RX_QEC_FIC        = 0x00200000, //!< Rx Quadrature Error Correction FIC
    ADI_ADRV9001_TRACKING_CAL_RX_AGC            = 0x00400000, //!< Rx Automatic Gain Control
    ADI_ADRV9001_TRACKING_CAL_RX_RSSI           = 0x00800000  //!< Rx RSSI
    /* Bit 24-31: Not used */
}adi_adrv9001_TrackingCalibrations_e;

/**
 * \brief Enum to run desired InitCals algorithms.
 * 
 * When using the internal LO, initial calibrations can be run all at once using ADI_ADRV9001_INIT_CAL_MODE_ALL. When
 * using external LO, it may be necessary to run initial calibrations in two stages:
 *   - Run ADI_ADRV9001_INIT_CAL_MODE_SYSTEM_AND_RX calibrations
 *   - Change external LO frequency as necessary
 *   - Run ADI_ADRV9001_INIT_CAL_MODE_LOOPBACK_AND_TX calibrations
 */
typedef enum adi_adrv9001_InitCalMode
{
    ADI_ADRV9001_INIT_CAL_MODE_ALL,             //!< Run initial calibrations on all profiles
    ADI_ADRV9001_INIT_CAL_MODE_SYSTEM_AND_RX,   //!< Run initial calibrations for the system and on Rx profiles
    ADI_ADRV9001_INIT_CAL_MODE_LOOPBACK_AND_TX, //!< Run initial calibrations on Loopback (Internal & External) and Tx profiles
    ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY         //!< Run initial calibrations only on External Loop Back (ELB) profiles.
                                                //!< 'ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY' can be selected only when external path delay calibration is run
}adi_adrv9001_InitCalMode_e;

/**
* \brief Data structure to hold Cals Init structures
*/
typedef struct adi_adrv9001_InitCals
{
    adi_adrv9001_InitCalibrations_e sysInitCalMask;     //!< Calibration bit mask for non-channel related init cals
    /** Array containing calibration bit mask for channel related init cals.
        It contains two masks:
        1. chanInitCalMask[0]: CH_1 for masks on Rx1/Tx1 channels,
        2. chanInitCalMask[1]: CH_2 for masks on Rx2/Tx2 channels */
    adi_adrv9001_InitCalibrations_e chanInitCalMask[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_InitCalMode_e  calMode;                //!< Enum specifies the mode to run desired InitCals algorithms
} adi_adrv9001_InitCals_t;

/**
* \brief Data structure to hold tracking calibration mask for CH_1 (Rx1/Tx1) and CH_2 (Rx2/Tx2)
*/
typedef struct adi_adrv9001_TrackingCals
{
    /** Array containing calibration bit mask for channel related tracking cals.
        It contains two masks:
        1. chanTrackingCalMask[0]: CH_1 for masks on Rx1/Tx1 channels,
        2. chanTrackingCalMask[1]: CH_2 for masks on Rx2/Tx2 channels */
    adi_adrv9001_TrackingCalibrations_e chanTrackingCalMask[ADI_ADRV9001_MAX_RX_ONLY];
} adi_adrv9001_TrackingCals_t;

#endif /* _ADI_ADRV9001_CALS_TYPES_H_ */
