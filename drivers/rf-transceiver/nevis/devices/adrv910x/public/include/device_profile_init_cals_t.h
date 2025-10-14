/*! ****************************************************************************
 *
 * @file:    device_profile_init_cals_t.h
 * *
 * @details: This file contains all the data type definitions for InitCalibrations
 *
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2022 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_INIT_CALS_T_HEADER__
#define __DEVICE_PROFILE_INIT_CALS_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_channels_t.h"

/**
 * \brief Enum to select desired InitCals select bits in the initCalMask.
 * \note Maskable
 */
typedef enum
{
    INIT_CAL_TX_QEC                = 0x00000001, //!< Tx Quadrature Error Correction
    INIT_CAL_TX_LO_LEAKAGE         = 0x00000002, //!< Tx LO Leakage
    INIT_CAL_TX_LB_PD              = 0x00000004, //!< Tx Loopback path delay
    INIT_CAL_TX_DCC                = 0x00000008, //!< Tx Duty Cycle Correction
    INIT_CAL_TX_BBAF               = 0x00000010, //!< Tx Baseband Analog Filter
    INIT_CAL_TX_BBAF_GD            = 0x00000020, //!< Tx Baseband Analog Filter Group Delay
    INIT_CAL_TX_ATTEN_DELAY        = 0x00000040, //!< Tx Attenuation Delay
    INIT_CAL_TX_DAC                = 0x00000080, //!< Tx DAC
    INIT_CAL_TX_PATH_DELAY         = 0x00000100, //!< Tx Path Delay

    INIT_CAL_RX_HPADC_RC           = 0x00000200, //!< Rx HP ADC Resistance and Capacitance
    INIT_CAL_RX_HPADC_FLASH        = 0x00000400, //!< Rx HP ADC Flash
    INIT_CAL_RX_HPADC_DAC          = 0x00000800, //!< Rx HP ADC DAC
    INIT_CAL_RX_DCC                = 0x00001000, //!< Rx Duty Cycle Correction
    INIT_CAL_RX_LPADC              = 0x00002000, //!< Rx LP ADC
    INIT_CAL_RX_TIA_CUTOFF         = 0x00004000, //!< Rx Trans-Impedance Amplifier Cutoff
    INIT_CAL_RX_GROUP_DELAY        = 0x00008000, //!< Rx Trans-Impedance Amplifier Group Delay
    INIT_CAL_RX_QEC_TCAL           = 0x00010000, //!< Rx QEC Tone Calibration
    INIT_CAL_RX_QEC_FIC            = 0x00020000, //!< Rx QEC Frequency-Independent
    INIT_CAL_RX_QEC_ILB_LO_DELAY   = 0x00040000, //!< Rx Internal Loopback LO Delay
    INIT_CAL_RX_RF_DC_OFFSET       = 0x00080000, //!< Rx RF DC Offset

	INIT_LO_RETUNE                 = 0x000B902B, //!< Minimium Subset of InitCals that must be run for LO Retune

    INIT_CAL_RX_GAIN_PATH_DELAY    = 0x00100000, //!< Rx Gain Path Delay
    INIT_CAL_RX_DMR_PATH_DELAY     = 0x00200000, //!< Rx DMR Path Delay

    INIT_CAL_PLL                   = 0x00400000, //!< PLL
    INIT_CAL_AUX_PLL               = 0x00800000, //!< AUX PLL

    INIT_CAL_TX_ALL                = 0x000001FF, //!< Tx all Init Cals
    INIT_CAL_RX_ALL                = 0x001FFE00, //!< Rx all Init Cals
    INIT_CAL_RX_TX_ALL             = 0x001FFFFF, //!< Rx / Tx all Init Cals
    INIT_CAL_SYSTEM_ALL            = 0x00C00000, //!< All system Init Cals
}ADI_NEVIS_PACK_ENUM initCalibrations_e;

/**
 * \brief Enum to select desired TrackingCals select bits in the trackingCalMask.
 * \note Maskable
 */
typedef enum
{
    /* SW and HW tracking cals */
    TRACKING_CAL_TX_QEC                                = 0x00000001, //!< Tx Quadrature Error Correction
    TRACKING_CAL_TX_LO_LEAKAGE                         = 0x00000002, //!< Tx LO Leakage
    TRACKING_CAL_TX_LB_PD                              = 0x00000004, //!< Tx Loopback path delay
    TRACKING_CAL_TX_PAC                                = 0x00000008, //!< Tx Power Amplifier Correction
    TRACKING_CAL_TX_DPD_CLGC                           = 0x00000010, //!< Tx Digital Pre Distortion and Close Loop Gain Control
    /* Bit 6-7: Not used (Reserved for future purpose) */
    TRACKING_CAL_RX_HD2                                = 0x00000100, //!< Rx Harmonic Distortion
    TRACKING_CAL_RX_QEC_WBPOLY                         = 0x00000200, //!< Rx Quadrature Error Correction Wideband Poly
    /* Bit 10-11: Not used (Reserved for future purpose) */
    TRACKING_CAL_ORX_QEC_WBPOLY                        = 0x00001000, //!< ORx Quadrature Error Correction Wideband Poly
    /* Bit 13-18:  Not used (Reserved for future purpose) */
    TRACKING_CAL_RX_BBDC                               = 0x00080000, //!< Rx Baseband DC rejection
    TRACKING_CAL_RX_RFDC                               = 0x00100000, //!< Rx RF DC
    TRACKING_CAL_RX_QEC_FIC                            = 0x00200000, //!< Rx Quadrature Error Correction FIC
    TRACKING_CAL_RX_GAIN_CONTROL_DETECTORS             = 0x00400000, //!< Rx Gain Control Detectors (Power, Analog Peak and Half Band)
    TRACKING_CAL_RX_RSSI                               = 0x00800000  //!< Rx RSSI
    /* Bit 24-31: Not used */
}ADI_NEVIS_PACK_ENUM trackingCalibrations_e;

/**
 * Enum to run desired InitCals algorithms.
 *
 * When using the internal LO, initial calibrations can be run all at once using INIT_CAL_MODE_ALL. When
 * using external LO, it may be necessary to run initial calibrations in two stages:
 *   - Run INIT_CAL_MODE_SYSTEM_AND_RX calibrations
 *   - Change external LO frequency as necessary
 *   - Run INIT_CAL_MODE_LOOPBACK_AND_TX calibrations
 */
typedef enum
{
    INIT_CAL_MODE_ALL,             //!< Run initial calibrations on all profiles
    INIT_CAL_MODE_SYSTEM_AND_RX,   //!< Run initial calibrations for the system and on Rx profiles
    INIT_CAL_MODE_LOOPBACK_AND_TX, //!< Run initial calibrations on Loopback (Internal & External) and Tx profiles
    INIT_CAL_MODE_ELB_ONLY         //!< Run initial calibrations only on External Loop Back (ELB) profiles.
                                                //!< 'INIT_CAL_MODE_ELB_ONLY' can be selected only when external path delay calibration is run
}ADI_NEVIS_PACK_ENUM initCalMode_e;

/**
* Data structure to hold Cals Init structures
*/
ADI_NEVIS_PACK_START
typedef struct
{
    initCalibrations_e sysInitCalMask;     //!< Calibration bit mask for non-channel related init cals
    /** Array containing calibration bit mask for channel related init cals.
        It contains two masks:
        1. chanInitCalMask[0]: CH_1 for masks on Rx1/Tx1 channels,
        2. chanInitCalMask[1]: CH_2 for masks on RxNB/TxNB channels */
	initCalibrations_e chanInitCalMask[NUM_RX_CHANNELS];
    initCalMode_e  calMode;                //!< Enum specifies the mode to run desired InitCals algorithms
    bool force;  //!< A value of true will force all enabled calibrations to re-run
} initCals_t;
ADI_NEVIS_PACK_FINISH

/**
* Data structure to hold tracking calibration mask for CH_1 (Rx1/Tx1) and CH_2 (RxNB/TxNB)
*/
ADI_NEVIS_PACK_START
typedef struct
{
    /** Array containing calibration bit mask for channel related tracking cals.
        It contains two masks:
        1. chanTrackingCalMask[0]: CH_1 for masks on Rx1/Tx1 channels,
        2. chanTrackingCalMask[1]: CH_2 for masks on RxNB/TxNB channels */
	trackingCalibrations_e chanTrackingCalMask[NUM_RX_CHANNELS];
} trackingCals_t;
ADI_NEVIS_PACK_FINISH

#endif /* __DEVICE_PROFILE_INIT_CALS_T_HEADER__ */
