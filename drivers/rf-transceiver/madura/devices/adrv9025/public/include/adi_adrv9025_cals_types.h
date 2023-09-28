/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_cals_types.h
 * \brief Contains ADRV9025 API Calibration data types
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_CALS_TYPES_H_
#define _ADI_ADRV9025_CALS_TYPES_H_

#define ARMINITCAL_ERRCODE(armCalId, armCalErrCode) ((armCalId << 8) | armCalErrCode)

#define ADI_ADRV9025_SWITCH_TIME_MIN 1000
#define ADI_ADRV9025_SWITCH_TIME_MAX 3000

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  \brief Enum to select desired InitCals select bits in the initCalMask.
 */
typedef enum adi_adrv9025_InitCalibrations
{
    ADI_ADRV9025_TX_BB_FILTER               = 0x00000001U,  /*!< Tx BB filter calibration */
    ADI_ADRV9025_ADC_TUNER                  = 0x00000002U,  /*!< ADC tuner calibration */
    ADI_ADRV9025_RX_TIA                     = 0x00000004U,  /*!< Rx TIA 3dB corner calibration */
    ADI_ADRV9025_ORX_TIA                    = 0x00000008U,  /*!< ORx TIA 3dB corner calibration */
    ADI_ADRV9025_LBRX_TIA                   = 0x00000010U,  /*!< Loopback Rx TIA 3dB corner calibration */
    ADI_ADRV9025_RX_DC_OFFSET               = 0x00000020U,  /*!< Rx DC offset calibration */
    ADI_ADRV9025_ORX_DC_OFFSET              = 0x00000040U,  /*!< ORx DC offset calibration */
    ADI_ADRV9025_LBRX_DC_OFFSET             = 0x00000080U,  /*!< LBRx DC offset calibration */
    ADI_ADRV9025_FLASH_CAL                  = 0x00000100U,  /*!< Flash converter comparator calibration */
    ADI_ADRV9025_INTERNAL_PATH_DELAY        = 0x00000200U,  /*!< Internal Path Delay calibration */
    ADI_ADRV9025_TX_LO_LEAKAGE_INTERNAL     = 0x00000400U,  /*!< Internal Tx LO leakage calibration */
    ADI_ADRV9025_TX_LO_LEAKAGE_EXTERNAL     = 0x00000800U,  /*!< External Tx LO leakage calibration */
    ADI_ADRV9025_TX_QEC_INIT                = 0x00001000U,  /*!< Tx quadrature error correction calibration */
    ADI_ADRV9025_LOOPBACK_RX_LO_DELAY       = 0x00002000U,  /*!< Loopback Rx LO delay path calibration */
    ADI_ADRV9025_LOOPBACK_RX_RX_QEC_INIT    = 0x00004000U,  /*!< Loopback Rx quadrature error correction calibration */
    ADI_ADRV9025_RX_LO_DELAY                = 0x00008000U,  /*!< Rx LO delay path calibration */
    ADI_ADRV9025_RX_QEC_INIT                = 0x00010000U,  /*!< Rx quadrature error correction calibration */
    ADI_ADRV9025_ORX_LO_DELAY               = 0x00020000U,  /*!< ORx LO delay path calibration */
    ADI_ADRV9025_ORX_QEC_INIT               = 0x00040000U,  /*!< ORx quadrature error correction calibration */
    ADI_ADRV9025_TX_DAC                     = 0x00080000U,  /*!< Tx DAC passband calibration */
    ADI_ADRV9025_DPD                        = 0x00100000U,  /*!< Digital Pre Distortion init calibration */
    ADI_ADRV9025_EXTERNAL_PATH_DELAY        = 0x00200000U,  /*!< External Tx-ORx Path delay equalization calibration */
    ADI_ADRV9025_VSWR                       = 0x00400000U,  /*!< Voltage Standing Wave Ratio calibration */
    ADI_ADRV9025_HD2                        = 0x00800000U,  /*!< Second Harmonic Distortion calibration */
    ADI_ADRV9025_TX_ATTENUATION_DELAY       = 0x01000000U,  /*!< Tx attenuation delay calibration */
    ADI_ADRV9025_TX_ATTEN_TABLE             = 0x02000000U,  /*!< Tx Attenuation Calibration */
    ADI_ADRV9025_RX_GAIN_DELAY              = 0x04000000U,  /*!< Rx gain delay calibration */
    ADI_ADRV9025_RX_GAIN_PHASE              = 0x08000000U,  /*!< Rx Gain and Phase calibration */
    ADI_ADRV9025_CFR_INIT                   = 0x20000000U,  /*!< Crest Factor Reduction init calibration */
    ADI_ADRV9025_SERDES_INIT                = 0x40000000U,  /*!< SERDES init cal */
    ADI_ADRV9025_MAX_INIT_CAL_MASK          = 0x6FFFFFFFU   /*!< Maximum Value of initcal mask */
} adi_adrv9025_InitCalibrations_e;

/**
 *  \brief Constants to select desired TrackingCals select bits in the trackingCalMask.
 */

#define ADI_ADRV9025_TRACK_NONE             0x0000000000000000     /*!< Disable all tracking calibrations */
#define ADI_ADRV9025_TRACK_RX1_QEC          0x0000000000000001     /*!< Rx1 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX2_QEC          0x0000000000000002     /*!< Rx2 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX3_QEC          0x0000000000000004     /*!< Rx3 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX4_QEC          0x0000000000000008     /*!< Rx4 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_ORX1_QEC         0x0000000000000010     /*!< ORx1 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_ORX2_QEC         0x0000000000000020     /*!< ORx2 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_ORX3_QEC         0x0000000000000040     /*!< ORx3 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_ORX4_QEC         0x0000000000000080     /*!< ORx4 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_TX1_LOL          0x0000000000000100     /*!< Tx1 LO leakage tracking calibration */
#define ADI_ADRV9025_TRACK_TX2_LOL          0x0000000000000200     /*!< Tx2 LO leakage tracking calibration */
#define ADI_ADRV9025_TRACK_TX3_LOL          0x0000000000000400     /*!< Tx3 LO leakage tracking calibration */
#define ADI_ADRV9025_TRACK_TX4_LOL          0x0000000000000800     /*!< Tx4 LO leakage tracking calibration */
#define ADI_ADRV9025_TRACK_TX1_QEC          0x0000000000001000     /*!< Tx1 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_TX2_QEC          0x0000000000002000     /*!< Tx2 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_TX3_QEC          0x0000000000004000     /*!< Tx3 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_TX4_QEC          0x0000000000008000     /*!< Tx4 quadrature error correction tracking calibration */
#define ADI_ADRV9025_TRACK_TX1_DPD          0x0000000000010000     /*!< Tx1 digital pre distortion tracking calibration */
#define ADI_ADRV9025_TRACK_TX2_DPD          0x0000000000020000     /*!< Tx2 digital pre distortion tracking calibration */
#define ADI_ADRV9025_TRACK_TX3_DPD          0x0000000000040000     /*!< Tx3 digital pre distortion tracking calibration */
#define ADI_ADRV9025_TRACK_TX4_DPD          0x0000000000080000     /*!< Tx4 digital pre distortion tracking calibration */
#define ADI_ADRV9025_TRACK_TX1_CLGC         0x0000000000100000     /*!< Tx1 closed loop gain control tracking calibration */
#define ADI_ADRV9025_TRACK_TX2_CLGC         0x0000000000200000     /*!< Tx2 closed loop gain control tracking calibration */
#define ADI_ADRV9025_TRACK_TX3_CLGC         0x0000000000400000     /*!< Tx3 closed loop gain control tracking calibration */
#define ADI_ADRV9025_TRACK_TX4_CLGC         0x0000000000800000     /*!< Tx4 closed loop gain control tracking calibration */
#define ADI_ADRV9025_TRACK_TX1_VSWR         0x0000000001000000     /*!< Tx1 voltage standing wave ratio tracking calibration */
#define ADI_ADRV9025_TRACK_TX2_VSWR         0x0000000002000000     /*!< Tx2 voltage standing wave ratio tracking calibration */
#define ADI_ADRV9025_TRACK_TX3_VSWR         0x0000000004000000     /*!< Tx3 voltage standing wave ratio tracking calibration */
#define ADI_ADRV9025_TRACK_TX4_VSWR         0x0000000008000000     /*!< Tx4 voltage standing wave ratio tracking calibration */
#define ADI_ADRV9025_TRACK_RX1_HD2          0x0000000010000000     /*!< Rx1 HD2 error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX2_HD2          0x0000000020000000     /*!< Rx2 HD2 error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX3_HD2          0x0000000040000000     /*!< Rx3 HD2 error correction tracking calibration */
#define ADI_ADRV9025_TRACK_RX4_HD2          0x0000000080000000     /*!< Rx4 HD2 error correction tracking calibration */
#define ADI_ADRV9025_TRACK_DESERIALIZER     0x0000000100000000     /*!< Deserializer tracking calibration */

/**
 *  \brief Enum of possible wait events
 */
typedef enum adi_adrv9025_WaitEvents
{
    ADI_ADRV9025_CLK_PLL_CP = 0,    /*!< CLK PLL CP wait event */
    ADI_ADRV9025_CLK_PLL_LOCK,      /*!< CLK PLL lock wait event */
    ADI_ADRV9025_LO1_PLL_CP,        /*!< RF PLL CP wait event */
    ADI_ADRV9025_LO1_PLL_LOCK,      /*!< RF PLL lock wait event */
    ADI_ADRV9025_LO2_PLL_CP,        /*!< RF PLL CP wait event */
    ADI_ADRV9025_LO2_PLL_LOCK,      /*!< RF PLL lock wait event */
    ADI_ADRV9025_AUX_PLL_CP,        /*!< AUX PLL CP wait event */
    ADI_ADRV9025_AUX_PLL_LOCK,      /*!< AUX PLL LOCK wait event */
    ADI_ADRV9025_ARMBUSY            /*!< ARM busy wait event */
} adi_adrv9025_WaitEvents_e;

/**
*  \brief Enum of Calibrations Tracking  Enable/ Disable flags
*/
typedef enum adi_adrv9025_TrackingCalEnableDisable
{
    ADI_ADRV9025_TRACKING_CAL_DISABLE   = 0,    /*!< Flag to indicate calibration is to disabled */
    ADI_ADRV9025_TRACKING_CAL_ENABLE    = 1     /*!< Flag to indicate calibration is to be enabled */
} adi_adrv9025_TrackingCalEnableDisable_e;

/**
 * \brief Enum of Calibration Errors
 */
typedef enum adi_adrv9025_TrackingCalError
{
    ADI_ADRV9025_TRACKINGCAL_RX1QEC_ERROR   = 0x01,         /*!< Error state of RX1 QEC   */
    ADI_ADRV9025_TRACKINGCAL_RX2QEC_ERROR   = 0x02,         /*!< Error state of RX2 QEC   */
    ADI_ADRV9025_TRACKINGCAL_RX3QEC_ERROR   = 0x04,         /*!< Error state of RX3 QEC   */
    ADI_ADRV9025_TRACKINGCAL_RX4QEC_ERROR   = 0x08,         /*!< Error state of RX4 QEC   */
    ADI_ADRV9025_TRACKINGCAL_ORX1QEC_ERROR  = 0x10,         /*!< Error state of ORX1 QEC  */
    ADI_ADRV9025_TRACKINGCAL_ORX2QEC_ERROR  = 0x20,         /*!< Error state of ORX2 QEC  */
    ADI_ADRV9025_TRACKINGCAL_ORX3QEC_ERROR  = 0x40,         /*!< Error state of ORX3 QEC  */
    ADI_ADRV9025_TRACKINGCAL_ORX4QEC_ERROR  = 0x80,         /*!< Error state of ORX4 QEC  */
    ADI_ADRV9025_TRACKINGCAL_TX1LOL_ERROR   = 0x100,        /*!< Error state of TX1 LOL   */
    ADI_ADRV9025_TRACKINGCAL_TX2LOL_ERROR   = 0x200,        /*!< Error state of TX2 LOL   */
    ADI_ADRV9025_TRACKINGCAL_TX3LOL_ERROR   = 0x400,        /*!< Error state of TX3 LOL   */
    ADI_ADRV9025_TRACKINGCAL_TX4LOL_ERROR   = 0x800,        /*!< Error state of TX4 LOL   */
    ADI_ADRV9025_TRACKINGCAL_TX1QEC_ERROR   = 0x1000,       /*!< Error state of TX1 QEC   */
    ADI_ADRV9025_TRACKINGCAL_TX2QEC_ERROR   = 0x2000,       /*!< Error state of TX2 QEC   */
    ADI_ADRV9025_TRACKINGCAL_TX3QEC_ERROR   = 0x4000,       /*!< Error state of TX3 QEC   */
    ADI_ADRV9025_TRACKINGCAL_TX4QEC_ERROR   = 0x8000,       /*!< Error state of TX4 QEC   */
    ADI_ADRV9025_TRACKINGCAL_TX1DPD_ERROR   = 0x10000,      /*!< Error state of TX1 DPD   */
    ADI_ADRV9025_TRACKINGCAL_TX2DPD_ERROR   = 0x20000,      /*!< Error state of TX2 DPD   */
    ADI_ADRV9025_TRACKINGCAL_TX3DPD_ERROR   = 0x40000,      /*!< Error state of TX3 DPD   */
    ADI_ADRV9025_TRACKINGCAL_TX4DPD_ERROR   = 0x80000,      /*!< Error state of TX4 DPD   */
    ADI_ADRV9025_TRACKINGCAL_TX1CLGC_ERROR  = 0x100000,     /*!< Error state of TX1 CLGC  */
    ADI_ADRV9025_TRACKINGCAL_TX2CLGC_ERROR  = 0x200000,     /*!< Error state of TX2 CLGC  */
    ADI_ADRV9025_TRACKINGCAL_TX3CLGC_ERROR  = 0x400000,     /*!< Error state of TX3 CLGC  */
    ADI_ADRV9025_TRACKINGCAL_TX4CLGC_ERROR  = 0x800000,     /*!< Error state of TX4 CLGC  */
    ADI_ADRV9025_TRACKINGCAL_TX1VSWR_ERROR  = 0x1000000,    /*!< Error state of TX1 VSWR  */
    ADI_ADRV9025_TRACKINGCAL_TX2VSWR_ERROR  = 0x2000000,    /*!< Error state of TX2 VSWR  */
    ADI_ADRV9025_TRACKINGCAL_TX3VSWR_ERROR  = 0x4000000,    /*!< Error state of TX3 VSWR  */
    ADI_ADRV9025_TRACKINGCAL_TX4VSWR_ERROR  = 0x8000000,    /*!< Error state of TX4 VSWR  */
    ADI_ADRV9025_TRACKINGCAL_RX1HD2_ERROR   = 0x10000000,   /*!< Error state of RX1 HD2   */
    ADI_ADRV9025_TRACKINGCAL_RX2HD2_ERROR   = 0x20000000,   /*!< Error state of RX2 HD2   */
    ADI_ADRV9025_TRACKINGCAL_RX3HD2_ERROR   = 0x40000000,   /*!< Error state of RX3 HD2   */
    ADI_ADRV9025_TRACKINGCAL_RX4HD2_ERROR   = 0x80000000    /*!< Error state of RX4 HD2   */
} adi_adrv9025_TrackingCalError_e;

/**
 * \brief Enum of Tracking Calibration States
 */
typedef enum adi_adrv9025_TrackingCalStateCode
{
    ADI_ADRV9025_TRACKINGCAL_SUSPENDED   = 0x01,    /*!< Cal is temporarily suspended by the framework.  */
    ADI_ADRV9025_TRACKINGCAL_RESUMED     = 0x02,    /*!< Cal is resumed for running.                     */
    ADI_ADRV9025_TRACKINGCAL_INACTIVE    = 0x04,    /*!< Cal's Main function is not executing            */
    ADI_ADRV9025_TRACKINGCAL_RUNNING     = 0x08,    /*!< Cal's Main function is executing                */
    ADI_ADRV9025_TRACKINGCAL_ENABLED     = 0x10,    /*!< Cal is enabled                                  */
    ADI_ADRV9025_TRACKINGCAL_DISABLED    = 0x20,    /*!< Cal is disabled                                 */
    ADI_ADRV9025_TRACKINGCAL_ERROR       = 0x40     /*!< Cal has generated error. It may be suspended
                                                       from running. BBIC should re-run the cal.       */
} adi_adrv9025_TrackingCalStateCode_e;

/**
* \brief Enumerated list of Clgc tracking cal errors
*/
typedef enum adi_adrv9025_ClgcTrackingCalError
{
    ADI_ADRV9025_CLGC_TRACKINGCAL_NO_ERROR                      = 0,
    ADI_ADRV9025_CLGC_TRACKINGCAL_DATACAP_ERROR                 = 0x3501,   /*!< Error code to convey that correlation capture error has occurred */
    ADI_ADRV9025_CLGC_TRACKINGCAL_DATACAP_ABORT_ERROR           = 0x3502,   /*!< Error code to convey that correlation capture abort error has occurred */
    ADI_ADRV9025_CLGC_TRACKINGCAL_NO_PATHDELAY_ERROR            = 0x3503,   /*!< Error code to convey that no path delay is found */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TRACK_APPLY_TX_ATTEN_ERROR    = 0x3504,   /*!< Error code to convey that Tx attenuation was failed to be applied */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TRACK_TX_ATTEN_LIMIT_ERROR    = 0x3505,   /*!< Error code to convey that Tx attenuation limit has been reached */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TRACK_CONVERG_LIMIT_ERROR     = 0x3506,   /*!< Error code to convey that convergence limit has been reached */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TRACK_PA_PROTECTION_ERROR     = 0x3507,   /*!< Error code to convey that PA protection error has occurred */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TRACK_DPD_SYNC_ERROR          = 0x3508,   /*!< Error code to convey that CLGC-DPD synchronization error has occurred */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TX_LEVEL_LOW_ERROR            = 0x3509,   /*!< Error code to convey that Tx power level is too low */
    ADI_ADRV9025_CLGC_TRACKINGCAL_ORX_LEVEL_LOW_ERROR           = 0x350A,   /*!< Error code to convey that ORx power level is too low */
    ADI_ADRV9025_CLGC_TRACKINGCAL_TX_MEAS_ERROR                 = 0x350B,   /*!< Error code to convey that there is a Tx power measurement error */
    ADI_ADRV9025_CLGC_TRACKINGCAL_ORX_MEAS_ERROR                = 0x350C,   /*!< Error code to convey that there is an ORx power measurement error */
    ADI_ADRV9025_CLGC_TRACKINGCAL_DATACAP_TIMEOUT_ERROR         = 0x350D    /*!< Error code to convey that data capture is timed out caused by invalid Tx to Orx mapping */
} adi_adrv9025_ClgcTrackingCalError_e;

/**
* \brief Enumerated list of CLGC and DPD synchronization status
*/
typedef enum adi_adrv9025_TrackingCalSyncStatus
{
    ADI_ADRV9025_TRACKINGCAL_SYNC_NA                      = 0,  /*!< Cal sync not available since startup */
    ADI_ADRV9025_TRACKINGCAL_SYNC_OK                      = 1,  /*!< Base and target cals are sync'ed */
    ADI_ADRV9025_TRACKINGCAL_SYNC_BASE_SYNC_NOT_ENABLED   = 2,  /*!< Sync not enabled in base cal task */
    ADI_ADRV9025_TRACKINGCAL_SYNC_TARG_SYNC_NOT_ENABLED   = 3,  /*!< Sync not enabled in target cal task */
    ADI_ADRV9025_TRACKINGCAL_SYNC_TARG_CAL_NOT_ENABLED    = 4,  /*!< Target cal task not enabled */
    ADI_ADRV9025_TRACKINGCAL_SYNC_SEM_CREATE_ERR          = 5,  /*!< Semaphore cannot be created */
    ADI_ADRV9025_TRACKINGCAL_SYNC_TARG_CAL_GET_STATUS_ERR = 6,  /*!< Cannot get target cal's enable status */
    ADI_ADRV9025_TRACKINGCAL_SYNC_TARG_CAL_GET_TIMER_ERR  = 7,  /*!< Cannot get target cal's timer period */
    ADI_ADRV9025_TRACKINGCAL_SYNC_SEM_TAKE_WARN           = 8,  /*!< Fail to take a semaphore before timeout */
    ADI_ADRV9025_TRACKINGCAL_SYNC_SEM_GIVE_ERR            = 9,  /*!< Fail to give a semaphore */
    ADI_ADRV9025_TRACKINGCAL_SYNC_BASE_CAL_ABORT          = 10  /*!< Base cal is aborted */
} adi_adrv9025_TrackingCalSyncStatus_e;

/**
* \brief Enum of Selecting the mShift channel
*/
typedef enum adi_adrv9025_DcOffsetChannels
{
    ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH0         = 0x0001,  /*!< MShift and DcOffset RX channel0 select           */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH1         = 0x0002,  /*!< MShift and DcOffset RX channel1 select           */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH2         = 0x0004,  /*!< MShift and DcOffset RX channel2 select           */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_RX_CH3         = 0x0008,  /*!< MShift and DcOffset RX channel3 select           */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH0        = 0x0010,  /*!< MShift and DcOffset ORX channel0 select          */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX_CH1        = 0x0020,  /*!< MShift and DcOffset ORX channel1 select          */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH0       = 0x0040,  /*!< MShift and DcOffset ORX2 mode channel0 select    */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORX2_CH1       = 0x0080,  /*!< MShift and DcOffset ORX2 mode channel1 select    */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH0 = 0x0100,  /*!< MShift and DcOffset ORX LpBack channel0 select   */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK1_CH1 = 0x0200,  /*!< MShift and DcOffset ORX LpBack channel1 select   */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH0 = 0x0400,  /*!< MShift and DcOffset ORX LpBack channel0 select   */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ORXLPBACK2_CH1 = 0x0800,  /*!< MShift and DcOffset ORX LpBack channel1 select   */
    ADI_ADRV9025_MSHIFT_DC_OFFSET_ALL_CHANNELS   = 0x0FFF   /*!< MShift and DcOffset All Channels selected        */
} adi_adrv9025_DcOffsetChannels_e;

/**
* \brief Enum of Warmboot status
*/
typedef enum adi_adrv9025_WarmbootStatus
{
    ADI_ADRV9025_WARMBOOT_OFF           = 0x00U,    /*!< Status indicating Warmboot off           */
    ADI_ADRV9025_RUN_INIT_WARMBOOT_IND  = 0x80U     /*!< Status indicating Warmboot on            */
} adi_adrv9025_WarmbootStatus_e;

/**
*  \brief Enum of possible Cal channel masks
*/
typedef enum adi_adrv9025_CalChannels
{
    ADI_ADRV9025_CAL_CH_1   = 0x1U, /*!< Channel 0 enabled */
    ADI_ADRV9025_CAL_CH_2   = 0x2U, /*!< Channel 1 enabled */
    ADI_ADRV9025_CAL_CH_3   = 0x4U, /*!< Channel 2 enabled */
    ADI_ADRV9025_CAL_CH_4   = 0x8U, /*!< Channel 3 enabled */
    ADI_ADRV9025_CAL_CH_ALL = 0x0F  /*!< All channel mask */
} adi_adrv9025_CalChannels_e;

/**
* \brief Data structure to hold Deserializer Tracking Status
*/
typedef struct adi_adrv9025_DesTrackingCalStatus
{
    uint32_t errorCode;       /*!< error code from Serdes tracking */
    uint32_t percentComplete; /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t varianceMetric;  /*!< NOT APPLICABLE FOR SERDES */
    uint32_t iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_DesTrackingCalStatus_t;

/**
 * \brief Data structure to hold Tx LOL Status
 */
typedef struct adi_adrv9025_TxLolStatus
{
    uint32_t errorCode;       /*!< error code from Tx LOL */
    uint32_t percentComplete; /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t varianceMetric;  /*!< metric of how well the tracking cal is performing */
    uint32_t iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_TxLolStatus_t;

/**
* \brief Data structure to hold Tx CLGC Status
*/
typedef struct adi_adrv9025_ClgcTrackingCalStatus
{
    adi_adrv9025_ClgcTrackingCalError_e errorCode;       /*!< error code from Tx CLGC */
    uint32_t                            percentComplete; /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t                            varianceMetric;  /*!< metric of how well the tracking cal is performing */
    uint32_t                            iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t                            updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_ClgcTrackingCalStatus_t;

/**
 * \brief Data structure to hold Tx QEC Status
 */
typedef struct adi_adrv9025_TxQecStatus
{
    uint32_t errorCode;        /*!< error code from Tx QEC */
    uint32_t percentComplete;  /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t correctionMetric; /*!< metric of how well the tracking cal is performing */
    uint32_t iterCount;        /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;      /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_TxQecStatus_t;

/**
 * \brief Data structure to hold Rx QEC Status
 */
typedef struct adi_adrv9025_RxQecStatus
{
    uint32_t errorCode;       /*!< error code from Rx QEC */
    uint32_t percentComplete; /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t selfcheckIrrDb;  /*!< Performance of the calibration */
    uint32_t iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_RxQecStatus_t;

/**
 * \brief Data structure to hold Rx HD2 Status
 */
typedef struct adi_adrv9025_RxHd2Status
{
    uint32_t errorCode;       /*!< error code from HD2 */
    uint32_t percentComplete; /*!< NOT APPLICABLE FOR HD2  Dynamic condition of this cal does not allow for convergence */
    uint32_t confidenceLevel; /*!< Confidence level that cal has accurately identified the coefficient required to cancel HD2
                                     0 - No Observation, 1 - 7 indicates increasing confidence level */
    uint32_t iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_RxHd2Status_t;

/**
* \brief Data structure to hold Orx QEC Status
*/
typedef struct adi_adrv9025_OrxQecStatus
{
    uint32_t errorCode;       /*!< error code from Orx QEC */
    uint32_t percentComplete; /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t selfcheckIrrDb;  /*!<  Performance of the calibration */
    uint32_t iterCount;       /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;     /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} adi_adrv9025_OrxQecStatus_t;

/**
* \brief Data structure to hold Rx HD2 calibration config settings
*
*        Note: Set posSideBandSel to the side (negative - lower side band or
*        positive - upper side band) that has the desired
*        signal.  Correction will be correcting the 2nd harmonic distortion on the
*        same side of the spectrum.
*
*        The stop/start frequency specify the harmonic correction range. If not specified, firmware chooses
*        a start frequency which is close to baseband DC (~0.12 x capture rate) and a stop frequency which
*        matches the edge of used BW specified by the profile for the device. Attempts to extend start/stop
*        outside the default range is clipped back to defaults.
*
*        It is important for HD2 cancellation to be disabled where signal isn't passed by the PFIR.
*
*        The control word is similar to specifying an HD2 control word using ARM control write, but being part of a
*        configuration write, it influences HD2 cancellation for all Rx channels.
*        Description for each bit of the control word is as follows:
*
*        BIT Number  |  Name                   |   Description
*        ------------------------------------------------------------------------------
*           0        |  clear_trust            |   If set to 1: Clear trust in learned correction information
*           1        |  clear_correction       |   If set to 1: Clears all learned correction information
*           2        |  suspend_correction     |   While set to 1: Disables w/o clearing learned correction corefficient
*           3        |  suspend_learning       |   While set to 1: Disables further learning of correction coefficient
*           4        |  suspend_degrading      |   While set to 1: Disables degrading of trust of corrrection coefficient
*           5        |  ignore_power           |   While set to 1: Ignore blocker power (merge all observations over power)
*           6        |  eval_enable            |   While set to 1: Calculate evaluation data when HFIR is updated
*           7        |  preserve_coef          |   While set to 1: Prevents stop/start from altering correction
*
*/
typedef struct adi_adrv9025_RxHd2Config
{
    uint32_t posSideBandSel;    /*!< 1 = Correct HD2 of desired signal on the positive (upper) side of the complex FFT,
                                    0 = correct HD2 in the negative (lower)side of the complex FFT */
    uint32_t control;           /*!< Applies ARM control word to all channels */
    uint32_t correctStart;      /*!< Adjusts start frequency of HD2 correction. Specifies harmonic correction range, not location of the blocker. Defaults to ~0.12 * capture rate. Can be specified anywhere between default range. LSB = 10kHz */
    uint32_t correctStop;       /*!< Adjusts stop frequency of HD2 correction. Specifies harmonic correction range, not location of the blocker. Defaults to the edge of the current BW. Can be specified anywhere between default range. LSB = 10kHz */
} adi_adrv9025_RxHd2Config_t;

/**
 * \brief Data structure to hold the InitCalStatus information for readback.
 */
typedef struct adi_adrv9025_InitCalStatus
{
    uint32_t initErrCode;         /*!< ObjID and Error code reported by the initialization calibration. ObjId Bits [15:8], Error Bits [7:0] */
    uint32_t initErrCal;          /*!< the object ID of the calibration reporting an error */
    uint32_t calsDurationUsec;    /*!< Duration time in uSec of the last initial calibration run */
    uint32_t calsSincePowerUp[4]; /*!< Bitmask indicating calibrations run since power up for Channel 1/2/3/4 */
    uint32_t calsLastRun[4];      /*!< Bitmask indicating calibrations run in during the previous runInitCals() call for Channel 1/2/3/4 */
} adi_adrv9025_InitCalStatus_t;

/**
* \brief Data structure to hold Cals Init structures
*
* \param calMask     Mask from adi_adrv9025_InitCalibrations_e to run on init.
* \param channelMask Mask to hold Channel to run on init
*                    Bit mask range, (valid 0x01-0x0F), 1 bit per channel.
*                    channelMask applies to the specific cals selected.
*                    If Tx cal, the channel mask refers to Tx channels,
*                    if Rx cals, channel mask refers to Rx channels,
*                    if ORx, channel mask refers to ORxchannels.
*                    If multiple selected, refers to all selected (Rx/Tx/ORx channels) for the cals selected.
* \param warmBoot    Not supported.
*/
typedef struct adi_adrv9025_InitCals
{
    uint32_t calMask;     /*!< Mask adi_adrv9025_InitCalibrations_e to run on init */
    uint8_t  channelMask; /*!< Mask to hold Channel to run on init */
    uint8_t  warmBoot;    /*!< (not supported) */
} adi_adrv9025_InitCals_t;

/**
* \brief Data structure to hold current state of all the calibrations.
*        Description for each bit of the returned state for every calibration is as follows
*
*        BIT Number  |  STATE       |   Description
*        ------------------------------------------------------------------------------
*           0        |  SUSPENDED   |   Cal is temporarily suspended by the framework.
*           1        |  RESUMED     |   Cal is resumed for running.
*           2        |  INACTIVE    |   Cal's Main function is not executing
*           3        |  RUNNING     |   Cal's Main function is executing
*           4        |  ENABLED     |   Cal is enabled
*           5        |  DISABLED    |   Cal is disabled
*           6        |  ERROR       |   Cal has generated error. It may be suspended from running.
*                    |              |   BBIC should re-run the cal.

* \param calError    4 bytes for Error codes Mask
*
*                    Bits        | Corresponding error
*                    [4 bytes]   |
*                   ----------------------------------------
*                     0          | Error state of RX1 QEC
*                     1          | Error state of RX2 QEC
*                     2          | Error state of RX3 QEC
*                     3          | Error state of RX4 QEC
*                     4          | Error state of ORX1 QEC
*                     5          | Error state of  ORX2 QEC
*                     6          | Error state of ORX3 QEC
*                     7          | Error state of ORX4 QEC
*                     8          | Error state of TX1 LOL
*                     9          | Error state of TX2 LOL
*                     10         | Error state of TX3 LOL
*                     11         | Error state of TX4 LOL
*                     12         | Error state of TX1 QEC
*                     13         | Error state of TX2 QEC
*                     14         | Error state of TX3 QEC
*                     15         | Error state of TX4 QEC
*                     16         | Error state of TX1 DPD
*                     17         | Error state of TX2 DPD
*                     18         | Error state of TX3 DPD
*                     19         | Error state of TX4 DPD
*                     20         | Error state of TX1 CLGC
*                     21         | Error state of TX2 CLGC
*                     22         | Error state of TX3 CLGC
*                     23         | Error state of TX4 CLGC
*                     24         | Error state of TX1 VSWR
*                     25         | Error state of TX2 VSWR
*                     26         | Error state of TX3 VSWR
*                     27         | Error state of TX4 VSWR
*                     28         | Error state of RX1 HD2
*                     29         | Error state of RX2 HD2
*                     30         | Error state of RX3 HD2
*                     31         | Error state of RX4 HD2
*
* \param rx1Qec       Rx Qec Tracking Cal Status for Channel 1.
* \param rx2Qec       Rx Qec Tracking Cal Status for Channel 2.
* \param rx3Qec       Rx Qec Tracking Cal Status for Channel 3.
* \param rx4Qec       Rx Qec Tracking Cal Status for Channel 4.
*
* \param orx1Qec      Orx Qec Tracking Cal Status for Channel 1.
* \param orx2Qec      Orx Qec Tracking Cal Status for Channel 2.
* \param orx3Qec      Orx Qec Tracking Cal Status for Channel 3.
* \param orx4Qec      Orx Qec Tracking Cal Status for Channel 4.
*
* \param tx1Lol       Tx Lol Tracking Cal Status for Channel 1.
* \param tx2Lol       Tx Lol Tracking Cal Status for Channel 2.
* \param tx3Lol       Tx Lol Tracking Cal Status for Channel 3.
* \param tx4Lol       Tx Lol Tracking Cal Status for Channel 4.
*
* \param tx1Qec       Tx Qec Tracking Cal Status for Channel 1.
* \param tx2Qec       Tx Qec Tracking Cal Status for Channel 2.
* \param tx3Qec       Tx Qec Tracking Cal Status for Channel 3.
* \param tx4Qec       Tx Qec Tracking Cal Status for Channel 4.
*
* \param tx1Dpd       Tx Dpd Tracking Cal Status for Channel 1.
* \param tx2Dpd       Tx Dpd Tracking Cal Status for Channel 2.
* \param tx3Dpd       Tx Dpd Tracking Cal Status for Channel 3.
* \param tx4Dpd       Tx Dpd Tracking Cal Status for Channel 4.
*
* \param tx1Clgc      Tx Clgc Tracking Cal Status for Channel 1.
* \param tx2Clgc      Tx Clgc Tracking Cal Status for Channel 2.
* \param tx3Clgc      Tx Clgc Tracking Cal Status for Channel 3.
* \param tx4Clgc      Tx Clgc Tracking Cal Status for Channel 4.
*
* \param tx1Vswr      Tx vswr Tracking Cal Status for Channel 1.
* \param tx2Vswr      Tx vswr Tracking Cal Status for Channel 2.
* \param tx3Vswr      Tx vswr Tracking Cal Status for Channel 3.
* \param tx4Vswr      Tx vswr Tracking Cal Status for Channel 4.
*
* \param rx1Hd2       Rx Hd2 Tracking Cal Status for Channel 1.
* \param rx2Hd2       Rx Hd2 Tracking Cal Status for Channel 2.
* \param rx3Hd2       Rx Hd2 Tracking Cal Status for Channel 3.
* \param rx4Hd2       Rx Hd2 Tracking Cal Status for Channel 4.
*
*/
typedef struct adi_adrv9025_TrackingCalState
{
    uint64_t calError; /*!< 8 bytes for Error codes Mask */

    uint8_t rx1Qec; /*!< Rx Qec Tracking Cal Status for Channel 1  */
    uint8_t rx2Qec; /*!< Rx Qec Tracking Cal Status for Channel 2  */
    uint8_t rx3Qec; /*!< Rx Qec Tracking Cal Status for Channel 3  */
    uint8_t rx4Qec; /*!< Rx Qec Tracking Cal Status for Channel 4  */

    uint8_t orx1Qec; /*!< Orx Qec Tracking Cal Status for Channel 1 */
    uint8_t orx2Qec; /*!< Orx Qec Tracking Cal Status for Channel 2 */
    uint8_t orx3Qec; /*!< Orx Qec Tracking Cal Status for Channel 3 */
    uint8_t orx4Qec; /*!< Orx Qec Tracking Cal Status for Channel 4 */

    uint8_t tx1Lol; /*!< Tx Lol Tracking Cal Status for Channel 1  */
    uint8_t tx2Lol; /*!< Tx Lol Tracking Cal Status for Channel 2  */
    uint8_t tx3Lol; /*!< Tx Lol Tracking Cal Status for Channel 3  */
    uint8_t tx4Lol; /*!< Tx Lol Tracking Cal Status for Channel 4  */

    uint8_t tx1Qec; /*!< Tx Qec Tracking Cal Status for Channel 1  */
    uint8_t tx2Qec; /*!< Tx Qec Tracking Cal Status for Channel 2  */
    uint8_t tx3Qec; /*!< Tx Qec Tracking Cal Status for Channel 3  */
    uint8_t tx4Qec; /*!< Tx Qec Tracking Cal Status for Channel 4  */

    uint8_t tx1Dpd; /*!< Tx Dpd Tracking Cal Status for Channel 1  */
    uint8_t tx2Dpd; /*!< Tx Dpd Tracking Cal Status for Channel 2  */
    uint8_t tx3Dpd; /*!< Tx Dpd Tracking Cal Status for Channel 3  */
    uint8_t tx4Dpd; /*!< Tx Dpd Tracking Cal Status for Channel 4  */

    uint8_t tx1Clgc; /*!< Tx Clgc Tracking Cal Status for Channel 1 */
    uint8_t tx2Clgc; /*!< Tx Clgc Tracking Cal Status for Channel 2 */
    uint8_t tx3Clgc; /*!< Tx Clgc Tracking Cal Status for Channel 3 */
    uint8_t tx4Clgc; /*!< Tx Clgc Tracking Cal Status for Channel 4 */

    uint8_t tx1Vswr; /*!< VSWR Tracking Cal Status for Channel 1  */
    uint8_t tx2Vswr; /*!< VSWR Tracking Cal Status for Channel 2  */
    uint8_t tx3Vswr; /*!< VSWR Tracking Cal Status for Channel 3  */
    uint8_t tx4Vswr; /*!< VSWR Tracking Cal Status for Channel 4  */

    uint8_t rx1Hd2; /*!< Rx Hd2 Tracking Cal Status for Channel 1  */
    uint8_t rx2Hd2; /*!< Rx Hd2 Tracking Cal Status for Channel 2  */
    uint8_t rx3Hd2; /*!< Rx Hd2 Tracking Cal Status for Channel 3  */
    uint8_t rx4Hd2; /*!< Rx Hd2 Tracking Cal Status for Channel 4  */

    uint8_t deserializer; /*!< Deserializer Tracking Cal Status */
} adi_adrv9025_TrackingCalState_t;

/**
* \brief Data structure to hold the external path delay settings
*/
typedef struct adi_adrv9025_ExternalPathDelay
{
    uint8_t fifoDelay;          /*!< External Tx-ORx data path delay for capturing samples */
    uint8_t interpolationIndex; /*!< Tx-ORx data path delay interpolation index aligned to 1/16th of a sample */
} adi_adrv9025_ExternalPathDelay_t;

/**
* \brief Data structure to hold the DC OffSet and MShift selections
*/
typedef struct adi_adrv9025_DcOffSet
{
    adi_adrv9025_DcOffsetChannels_e dcChannelSelect; /*!< enum that select the channel type to apply the offset and shift */
    uint8_t                         mShift;          /*!< mShift value between 0-31 */
} adi_adrv9025_DcOffSet_t;

/**
* \brief Data structure to hold the DC OffSet and MShift selections
*
* Both reset types resets the perturbation scaler, clears accumulated RX and TX values, sets fast attack=1 to force a fast attack
* and applies the measurements more strongly.
*
* Hard reset erases the TxLOL look-up table.  The look-up table keeps the correction values for all 64 attenuation values.
* Soft reset restarts the TxLOL estimation without erasing the TxLOL look-up table.
*/
typedef struct adi_adrv9025_ExtTxLolChannelReset_s
{
    adi_adrv9025_TxChannels_e channelSelect; /*!< enum that select the channels t0 reset */
    uint8_t                   resetType;     /*!< 0-Soft Reset,  1-Hard Reset */
} adi_adrv9025_ExtTxLolChannelReset_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_CALS_TYPES_H_ */
