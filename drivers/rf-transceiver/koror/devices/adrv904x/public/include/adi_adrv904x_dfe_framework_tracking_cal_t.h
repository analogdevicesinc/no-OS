/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_framework_tracking_cal_t.h
 *
 * \brief   Contains ADRV904X Tracking Cal Types Download File data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef ADRV904X_DFE_FRAMEWORK_TRACKING_CAL_T_H__
#define ADRV904X_DFE_FRAMEWORK_TRACKING_CAL_T_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_err_object_ids_t.h"


#define ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_NUM_OF_CHANNELS                      8U /*!< Number of DFE tracking cal channels */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_MAX_NUM_ERROR_CODES_TO_PAUSE_CAL     (4U)
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_MAX_NUM_DPD_CAPTURE_BUFS_TO_STORE    (8U)
/**
 * \brief DFE firmware calibration object IDs
 */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_VSWR                                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START)     /*!< Object ID for VSWR cal         */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_CFR                                     (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START + 1) /*!< Object ID for CFR  cal         */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_DPD                                     (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START + 2) /*!< Object ID for DPD  cal         */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_OBJID_CAL_EXT_DFE                                 (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START + 3) /*!< Object ID for external DFE cal */


/* Subtract the first tracking cal object ID to convert object ID to array index */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_OBJ_TO_IDX(x)    ((x) - ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START)  /*!< Macro for Object ID to index conversion */

/* Add the first tracking cal object ID to convert array index to object ID */
#define ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_IDX_TO_OBJ(x)    ((x) + ADI_ADRV904X_DFE_SVC_ERR_OBJID_CAL_START)  /*!< Macro for index to Object ID conversion */


/*! Calibration status type */
typedef enum adi_adrv904x_DfeAppFrameworkTrackingCalStatusType
{
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_COMMON,     /*!< Common calibration status */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_SPECIFIC,   /*!< Calibration-specific status */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_PRIVATE,    /*!< Private calibration-specific status */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_EXTENDED,   /*!< Calibration-extended status */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_STATUS_NUM_STATUS
} adi_adrv904x_DfeAppFrameworkTrackingCalStatusType_e;

/**
 * \brief DFE Tracking calibration types
 */
typedef enum adi_adrv904x_DfeAppFrameworkTrackingCalType
{
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_VSWR    = 0,                   /*!< VSWR tracking cal */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_CFR     = 1,                   /*!< CFR tracking cal */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_DPD     = 2,                   /*!< DPD tracking cal */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_EXT_DFE = 3,                   /*!< EXT DFE dumming tracking cal */
    NUM_TRACKCAL_TYPES                         = 4                    /*!< Number of Tracking Calibration Types */
} adi_adrv904x_DfeAppFrameworkTrackingCalType_e;

/**
 * \brief DFE Tracking calibration mask
 */
typedef enum adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask
{
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_VSWR_MASK    = 1 << ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_VSWR,    /*!< VSWR Tracking calibration       */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_CFR_MASK     = 1 << ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_CFR,     /*!< CFR Tracking calibration       */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_DPD_MASK     = 1 << ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_DPD,     /*!< DPD Tracking calibration       */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TX_EXT_DFE_MASK = 1 << ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_TYPE_EXT_DFE, /*!< EXT DFE Tracking calibration   */
} adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e;

/**
 * \brief DFE Tracking Calibration enable/disable types
 */
typedef enum adi_adrv904x_DfeAppTrackingCalEnableDisable
{
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_DISABLE = 0,  /*!< DFE Framework tracking cal disable  */
    ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKINGCAL_ENABLE  = 1   /*!< DFE Framework tracking cal enable  */
} adi_adrv904x_DfeAppTrackingCalEnableDisable_e;

typedef uint32_t   adi_adrv904x_DfeAppFrameworkTrackingCalType_t;          /*!< adi_adrv904x_DfeAppFrameworkTrackingCalType_e translation for API size         */
typedef uint32_t   adi_adrv904x_DfeAppTrackingCalEnableDisable_t;          /*!< adi_adrv904x_DfeAppTrackingCalEnableDisable_e translation for API size         */
typedef uint32_t   adi_adrv904x_DfeAppFrameworkTrackingCalStateFlag_t;     /*!< adi_adrv904x_DfeAppFrameworkTrackingCalStateFlag_e translation for API size    */
typedef uint32_t   adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_t;  /*!< adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_e translation for API size */

/**
 * \brief Data structure to hold tracking calibration enable masks
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks
{
    adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_t enableMask[ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_NUM_OF_CHANNELS];  /*!< enableMask; Array of tracking cal masks per channel  */
} adi_adrv904x_DfeAppFrameworkTrackingCalEnableMasks_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Data structure to hold tracking calibration state information
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppFrameworkTrackingCalState
{
    adi_adrv904x_DfeAppFrameworkTrackingCalibrationMask_t calError[ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_NUM_OF_CHANNELS];                     /*!< calError; Array of tracking cal errors per channel  */
    adi_adrv904x_DfeAppFrameworkTrackingCalStateFlag_t    calState[ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_NUM_OF_CHANNELS][NUM_TRACKCAL_TYPES]; /*!< calError; Array of tracking cal states per cal and channel  */
} adi_adrv904x_DfeAppFrameworkTrackingCalState_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to Set Capture Buffers Access parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet
{
    uint8_t  stopAfterCapDone;                                                          /*!< Skip the following captures and updates after captures are complete for that iteration */
    uint32_t errCodes[ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_MAX_NUM_ERROR_CODES_TO_PAUSE_CAL]; /*!< Array of error codes to pause the cal with capture data */
    uint8_t  releaseCapBuf;                                                             /*!< Release capture buffers and continue routine capture and updates, this will also clear capIsDone state */
} adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Data structure to Read Capture Buffer Access State and Buffers
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet
{
    uint8_t  capIsDone;                                                              /*!< FW will set this bit to indicate cal thread is paused and capture data is ready */
    uint8_t  num;                                                                    /*!< Number of valid captures in pMem array */
    uint64_t pMem[ADI_ADRV904X_DFE_APP_FRAMEWORK_TRACKING_CAL_MAX_NUM_DPD_CAPTURE_BUFS_TO_STORE]; /*!< Array of capture buffers */
} adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADRV904X_DFE_FRAMEWORK_TRACKING_CAL_T_H__ */

