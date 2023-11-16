/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_cals_types.h
 * 
 * \brief   Contains ADRV904X Calibration data types
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_CAL_TYPES_H__
#define __ADRV904X_CAL_TYPES_H__

#include "adi_adrv904x_cpu_error_codes_types.h"
#include "adi_adrv904x_cals_structs.h"

#ifndef ADI_ADRV904X_FW
#include "adi_adrv904x_platform_pack.h"
#else
#include "adi_adrv904x_platform_pack.h"
#include "adrv904x_cpu_object_ids_types.h"
#include "adrv904x_cpu_error_codes_types.h"
#endif

/* Number of channels that can be calibrated */
#define ADI_ADRV904X_CPU_NUM                    2U
#define ADI_ADRV904X_NUM_INIT_CAL_CHANNELS      8U
#define ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS  8U
#define ADI_ADRV904X_CHAR_ARRAY_MAX 256U

/* Size of struct that contains TxHRM data for one channel */
#define TX_HRM_CAL_DATA_PER_CHANNEL_SIZE 48U

/**
 * \brief Initial calibration mask
 */
typedef enum adi_adrv904x_InitCalibrations
{
    ADI_ADRV904X_IC_RX_DC_OFFSET         = 2,      /*!< RX DC Offet calibration         */
    ADI_ADRV904X_IC_ADC_RX               = 4,            /*!< ADC Orx calibration             */
    ADI_ADRV904X_IC_ADC_ORX              = 8,           /*!< ADC Orx calibration             */
    ADI_ADRV904X_IC_ADC_TXLB             = 16,          /*!< ADC Tx loopback calibration     */
    ADI_ADRV904X_IC_TXDAC                = 32,             /*!< Tx DAC calibration              */
    ADI_ADRV904X_IC_TXBBF                = 64,             /*!< Tx BB filter calibration        */
    ADI_ADRV904X_IC_TXLB_FILTER          = 16384,       /*!< Tx Loopback filter calibration  */
    ADI_ADRV904X_IC_TXLB_PATH_DLY        = 128,     /*!< Tx LB path delay calibration    */
    ADI_ADRV904X_IC_TX_ATTEN_CAL         = 256,      /*!< Tx atten phase/gain calibration (NOTE: not implemented) */
    ADI_ADRV904X_IC_HRM                  = 512,               /*!< HRM initial calibration         */
    ADI_ADRV904X_IC_TXQEC                = 1024,             /*!< TxQEC initial calibration       */
    ADI_ADRV904X_IC_TXLOL                = 2048,             /*!< TxLOL initial calibration       */
    ADI_ADRV904X_IC_SERDES               = 4096,            /*!< SerDes initial calibration      */
    ADI_ADRV904X_IC_TXRX_PHASE           = 131072,        /*!< TxRxPhase initial calibration   */
	
} adi_adrv904x_InitCalibrations_e;

/**
 * \brief JrxRepair Start up Mode
 */
typedef enum adi_adrv904x_JrxRepairInitMode
{
    ADI_ADRV904X_JRXREPAIR_INIT_NORMAL      = 0x0U, /*!< JRx Repair must be performed if init cal fails, at runtime or if history is present */
    ADI_ADRV904X_JRXREPAIR_INIT_NONSCREENED = 0x2U, /*!< JRx repair is applied automatically for all the transceivers that have not been screened */
    ADI_ADRV904X_JRXREPAIR_INIT_ALL         = 0x3U, /*!< JRx repair is applied automatically to all the transceivers */
} adi_adrv904x_JrxRepairInitMode_e;

typedef uint64_t adi_adrv904x_InitCalibrations_t;

#define ADI_ADRV904X_IC_ALL_CALS ((adi_adrv904x_InitCalibrations_t)(  ADI_ADRV904X_IC_RX_DC_OFFSET      \
                                                                    | ADI_ADRV904X_IC_ADC_RX            \
                                                                    | ADI_ADRV904X_IC_ADC_ORX           \
                                                                    | ADI_ADRV904X_IC_ADC_TXLB          \
                                                                    | ADI_ADRV904X_IC_TXDAC             \
                                                                    | ADI_ADRV904X_IC_TXBBF             \
                                                                    | ADI_ADRV904X_IC_TXLB_FILTER       \
                                                                    | ADI_ADRV904X_IC_TXLB_PATH_DLY     \
                                                                    | ADI_ADRV904X_IC_HRM               \
                                                                    | ADI_ADRV904X_IC_TXQEC             \
                                                                    | ADI_ADRV904X_IC_TXLOL             \
                                                                    | ADI_ADRV904X_IC_SERDES            \
                                                                    | ADI_ADRV904X_IC_TXRX_PHASE))

/**
* \brief Data structure to hold initial calibration information
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_InitCals
{
    adi_adrv904x_InitCalibrations_t calMask;  /*!< Mask of adi_adrv904x_InitCalibrations_e to run on init */
    uint32_t rxChannelMask;                   /*!< Mask to hold Rx channels to run on init */
    uint32_t txChannelMask;                   /*!< Mask to hold Tx channels to run on init */
    uint32_t orxChannelMask;                  /*!< Mask to hold ORx channels to run on init */
    uint8_t warmBoot;                         /*!< (not supported) */
} adi_adrv904x_InitCals_t;)

/**
* \brief Data structure to hold initial calibration information
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_ChannelTrackingCals
{
    uint32_t rxChannel;     /*!< RX Tracking calibration channel */
    uint32_t orxChannel;    /*!< ORX Tracking calibration channel */
    uint32_t txChannel;     /*!< TX Tracking calibration channel */
    uint32_t laneSerdes;   /*!< TX lane SERDES Tracking calibration channel */
} adi_adrv904x_ChannelTrackingCals_t;)
    
/**
 * \brief Data structure to hold the InitCalStatus information for readback
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_InitCalStatus
{
    adi_adrv904x_CpuErrorCode_t     initErrCodes[ADI_ADRV904X_NUM_INIT_CAL_CHANNELS];       /*!< Set of initial calibration error codes for Channels 1/2/3/4/5/6/7/8. */
    adi_adrv904x_InitCalibrations_t initErrCals[ADI_ADRV904X_NUM_INIT_CAL_CHANNELS];        /*!< Set of IDs of the specific initial calibration(s) reporting an error (adi_adrv904x_InitCalibrations_e) for Channels 1/2/3/4/5/6/7/8 */
    uint32_t                        calsDurationMsec;                                       /*!< Duration in msec of the last initial calibration run */
    adi_adrv904x_InitCalibrations_t calsSincePowerUp[ADI_ADRV904X_NUM_INIT_CAL_CHANNELS];   /*!< Bitmask (adi_adrv904x_InitCalibrations_e) indicating calibrations run since power up for Channels 1/2/3/4/5/6/7/8 */
    adi_adrv904x_InitCalibrations_t calsLastRun[ADI_ADRV904X_NUM_INIT_CAL_CHANNELS];        /*!< Bitmask (adi_adrv904x_InitCalibrations_e) indicating calibrations run in during the previous runInitCals() call for Channels 1/2/3/4/5/6/7/8 */
} adi_adrv904x_InitCalStatus_t;)

/**
* \brief Data structure to hold initial calibration error information
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_InitCalChannelErrData
{
    adi_adrv904x_InitCalibrations_t initErrCals;                                /*!< Error ID Mask for adi_adrv904x_InitCalibrations_e */
    adi_adrv904x_CpuErrorCode_t     errCode;                                    /*!< CPU Error Code */
    uint8_t                         errMsg[ADI_ADRV904X_CHAR_ARRAY_MAX];        /*!< Error Message */
    uint8_t                         errCause[ADI_ADRV904X_CHAR_ARRAY_MAX];      /*!< Error Cause */
    int64_t                         action;                                     /*!< Recovery Action */
    uint8_t                         actionMsg[ADI_ADRV904X_CHAR_ARRAY_MAX];     /*!< Recovery Action Message */
    adi_adrv904x_InitCalibrations_t calsSincePowerUp;                           /*!< Cals Run Since Power Up Bitmask */
    adi_adrv904x_InitCalibrations_t calsLastRun;                                /*!< Cals Since Last Run Bitmask */
} adi_adrv904x_InitCalChannelErrData_t;)

/**
* \brief Data structure to hold initial calibration error information
*/
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_InitCalErrData
{
    adi_adrv904x_InitCalChannelErrData_t    channel[ADI_ADRV904X_NUM_INIT_CAL_CHANNELS];    /*!< Init Cal Data for All Channels */
    uint32_t                                calsDurationMsec[ADI_ADRV904X_CPU_NUM];         /*!< Duration in msec of the last initial calibration run (Milliseconds) */
} adi_adrv904x_InitCalErrData_t;)

/**
 * \brief Tracking calibration ID
 */
typedef enum adi_adrv904x_TrackingCalibrationId
{
    ADI_ADRV904X_TC_RX_QEC     = 0,     /*!< RxQEC Tracking calibration       */
    ADI_ADRV904X_TC_TX_LOL     = 1,    /*!< TxLOL Tracking calibration       */
    ADI_ADRV904X_TC_TX_QEC     = 2,     /*!< TxQEC Tracking calibration       */
    ADI_ADRV904X_TC_TX_SERDES  = 3,    /*!< SERDES Tracking calibration      */
    ADI_ADRV904X_TC_RX_ADC     = 4,    /*!< Rx ADC Tracking calibration      */
    ADI_ADRV904X_TC_TX_LB_ADC  = 5,  /*!< TxLB ADC Tracking calibration    */
    ADI_ADRV904X_TC_ORX_ADC    = 6,   /*!< ORx ADC Tracking calibration     */
    ADI_ADRV904X_TC_NUM_CALS   = 6 + 1ul, /*!< Number of tracking calibrations  */
} adi_adrv904x_TrackingCalibrationId_e;

typedef uint32_t adi_adrv904x_TrackingCalibrationId_t;

/**
 * \brief Tracking calibration mask
 */
typedef enum adi_adrv904x_TrackingCalibrationMask
{
    ADI_ADRV904X_TC_RX_QEC_MASK     = 1,     /*!< RxQEC Tracking calibration       */
    ADI_ADRV904X_TC_TX_LOL_MASK     = 2,    /*!< TxLOL Tracking calibration       */
    ADI_ADRV904X_TC_TX_QEC_MASK     = 4,     /*!< TxQEC Tracking calibration       */
    ADI_ADRV904X_TC_TX_SERDES_MASK  = 8,    /*!< SERDES Tracking calibration      */
    ADI_ADRV904X_TC_RX_ADC_MASK     = 16,    /*!< Rx ADC Tracking calibration      */
    ADI_ADRV904X_TC_TX_LB_ADC_MASK  = 32,  /*!< TxLB ADC Tracking calibration    */
    ADI_ADRV904X_TC_ORX_ADC_MASK    = 64,   /*!< ORx ADC Tracking calibration     */
} adi_adrv904x_TrackingCalibrationMask_e;

typedef uint32_t adi_adrv904x_TrackingCalibrationMask_t;

#define ADI_ADRV904X_TC_ALL_CALS ((adi_adrv904x_TrackingCalibrationMask_t)( ADI_ADRV904X_TC_RX_QEC_MASK \
                                                                          | ADI_ADRV904X_TC_TX_LOL_MASK \
                                                                          | ADI_ADRV904X_TC_TX_QEC_MASK \
                                                                          | ADI_ADRV904X_TC_TX_SERDES_MASK \
                                                                          | ADI_ADRV904X_TC_RX_ADC_MASK \
                                                                          | ADI_ADRV904X_TC_TX_LB_ADC_MASK \
                                                                          | ADI_ADRV904X_TC_ORX_ADC_MASK))

/**
 * \brief Data structure to hold tracking calibration enable masks
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TrackingCalEnableMasks
{
    adi_adrv904x_TrackingCalibrationMask_t enableMask[ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS];
} adi_adrv904x_TrackingCalEnableMasks_t;)

/**
*  \brief Tracking calibration enable/disable flags
*/
typedef enum adi_adrv904x_TrackingCalEnableDisable
{
    ADI_ADRV904X_TRACKING_CAL_DISABLE = 0,  /*!< Disable tracking cals */
    ADI_ADRV904X_TRACKING_CAL_ENABLE  = 1   /*!< Enable tracking cals */
} adi_adrv904x_TrackingCalEnableDisable_e;

/**
*  \brief Tracking calibration states
*/
typedef enum adi_adrv904x_TrackingCalStateFlag
{
    ADI_ADRV904X_TC_STATE_SUSPENDED = 0x01,     /*!< Calibration is temporarily suspended */
    ADI_ADRV904X_TC_STATE_RESUMED   = 0x02,     /*!< Calibration is resumed for running */
    ADI_ADRV904X_TC_STATE_INACTIVE  = 0x04,     /*!< Calibration is not executing */
    ADI_ADRV904X_TC_STATE_RUNNING   = 0x08,     /*!< Calibration is executing */
    ADI_ADRV904X_TC_STATE_ENABLED   = 0x10,     /*!< Calibration is enabled*/
    ADI_ADRV904X_TC_STATE_DISABLED  = 0x20,     /*!< Calibration is disabled*/
    ADI_ADRV904X_TC_STATE_ERROR     = 0x40,     /*!< Calibration has errored. BBIC should re-run the cal. */
} adi_adrv904x_TrackingCalStateFlag_e;
typedef uint8_t adi_adrv904x_TrackingCalStateFlag_t;

/**
 * \brief Data structure to hold tracking calibration state information
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TrackingCalState
{
    adi_adrv904x_TrackingCalibrationMask_t calError[ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS];
    adi_adrv904x_TrackingCalStateFlag_t    calState[ADI_ADRV904X_NUM_TRACKING_CAL_CHANNELS][7];
} adi_adrv904x_TrackingCalState_t;)

/**
 * \brief Data structure to HRM data for one Tx channel
 */
typedef struct adi_adrv904x_TxHrmData
{
    uint8_t hrmCalData[TX_HRM_CAL_DATA_PER_CHANNEL_SIZE];
} adi_adrv904x_TxHrmData_t;

/**
 *  \brief Tx LOL Reset Type
 */
typedef enum adi_adrv904x_TxLolResetType
{
    ADI_ADRV904X_TX_LOL_SOFT_RESET = 0U,    /*!< Channel Reset, not including the Atten Table*/
    ADI_ADRV904X_TX_LOL_HARD_RESET = 1U     /*!< Reset LO Leakage, Tx-to-Lb Channel & LOL Actuator Data */   
} adi_adrv904x_TxLolResetType_e;

/**
 * \brief Data structure to reset Tx channel LOL
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TxLolReset
{
    uint32_t                       channelMask;  /*!< selected Tx channels to be reset */
    adi_adrv904x_TxLolResetType_e  resetType;    /*!< reset type */
} adi_adrv904x_TxLolReset_t;)

/**
 *  \brief Tx QEC Reset Type
 */
typedef enum adi_adrv904x_TxQecResetType
{
    ADI_ADRV904X_TX_QEC_TRACKING_HARD_RESET = 0U,   /*!< Combination of QEC & Channel Reset */
    ADI_ADRV904X_TX_QEC_TRACKING_QEC_RESET,         /*!< Reset Model of Quadrature Error Correction; Note QEC Performance will Degrade */
    ADI_ADRV904X_TX_QEC_TRACKING_CHANNEL_RESET,     /*!< Tx-to-Lb Channel Reset; Note QEC Performance is maintained */
} adi_adrv904x_TxQecResetType_e;

/**
 * \brief Data structure to reset Tx channel QEC
 */
ADI_ADRV904X_PACKED(
typedef struct adi_adrv904x_TxQecReset
{
    uint32_t                        channelMask;    /*!< selected Tx channels to be reset */
    adi_adrv904x_TxQecResetType_e   resetType;      /*!< reset type */
} adi_adrv904x_TxQecReset_t;)

#endif /* __ADRV904X_CAL_TYPES_H__ */


