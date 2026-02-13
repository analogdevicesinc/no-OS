/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv904x_dfe_cal_clgc_int_types.h
 *
 * \brief Contains DFE Application type definitions for CLGC, visible to BBIC API
 *        and customers. Structures here use integer types instead of floating point
 *
 * ADRV904X API Version: 2.15.0.4
 */


#ifndef ADRV904X_DFE_CAL_CLGC_INT_TYPES_H_
#define ADRV904X_DFE_CAL_CLGC_INT_TYPES_H_

#include "adi_adrv904x_dfe_cal_clgc_types.h"

/**
 * \brief Data structure to hold CLGC capture configuration parameters (without floating point numbers)
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcCaptureCfgInt
{
    adi_adrv904x_DfeAppCalClgcCaptureMode_e capMode; /*!< CLGC capture mode: TSSI or peak detection */
    uint32_t                     numCapBatches;      /*!< number of TSSI measurement batches in TSSI mode */
    uint32_t                     capDurationUs;      /*!< TSSI measurement time (usec) of one batch in TSSI mode */
    uint32_t                     capPeriodUs;        /*!< timeout (usec) for TSSI timer of one batch, or window length in peak det mode */
    int32_t                      minTxPowThres_e6;   /*!< minimum Tx power threshold needed to run CLGC tracking for each of batches (units of 1e-6) */
    int32_t                      minOrxPowThres_e6;  /*!< minimum ORx power threshold needed to run CLGC tracking for each of batches(units of 1e-6) */
} adi_adrv904x_DfeAppCalClgcCaptureCfgInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold CLGC tracking configuration parameters (without floating point numbers)
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcTrackCfgInt
{
    uint8_t enClgc;                   /*!< enable (1) or disable closed loop gain control */
    uint8_t enPaProtect;              /*!< enable (1) or disable PA protection assert check in CLGC */
    int32_t expLoopPowGain_e6;        /*!< expected Tx-ORx loop power gain for CLGC to track on (units of 1e-6) */
    int32_t maxLoopGainAdjustdB_e6;   /*!< maximum loop gain adjustment in dB for Tx atten (units of 1e-6) */
    int32_t minTxAttenLimitdB_e6;     /*!< minimum Tx attenuation limit in dB (units of 1e-6) */
    int32_t maxTxAttenLimitdB_e6;     /*!< maximum Tx attenuation limit in dB (units of 1e-6) */
} adi_adrv904x_DfeAppCalClgcTrackCfgInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of CLGC status (without floating point numbers)
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcStatusInt
{
    adi_adrv904x_CalStatus_t     hdr;                   /*!< cal status header */
    int32_t                      txPwr_e6;              /*!< Tx power measured (units of 1e-6) */
    int32_t                      orxPwr_e6;             /*!< ORx power measured (units of 1e-6) */
    int32_t                      loopPowGain_e6;        /*!< current loop gain (units of 1e-6) */
    int32_t                      txAttenAdjdB_e6;       /*!< current Tx atten adjustment (units of 1e-6) */
    uint32_t                     numCapBatchesRun;      /*!< number of measurement batches actually run */
    uint32_t                     actCapDurationUs;      /*!< actual measurement duration time in us */
    uint32_t                     capStartRetryCount;    /*!< capture retries due to start error */
    uint32_t                     capAbortRetryCount;    /*!< capture retries due to abort (e.g. mapping changes) */
    uint32_t                     capInvalidRetryCount;  /*!< capture retries due to invalid/incomplete capture */
    uint32_t                     lowSigRetryCount;      /*!< capture retries due to low signal power */
    uint32_t                     txAttenChangeCount;    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_CHANGED */
    uint32_t                     orxGainChangeCount;    /*!< count for ORx gain changes made by user */
    uint32_t                     txAttenLimitCount;     /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_BELOW_MIN_LIMIT and ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_ABOVE_MAX_LIMIT */
    uint32_t                     paProtectAssertCount;  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_PA_PROTECTION_ASSERTED */
    uint32_t                     lowSigErrorCount;      /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_SIGNAL_LOW_ERROR */
    uint32_t                     capAbortErrorCount;    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_ABORTED_CAPTURE */
    uint32_t                     capInvalidErrorCount;  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_INCOMPLETE_CAPTURE and ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_RESULT_GET_ERROR */
    uint32_t                     capStartErrorCount;    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR */
    uint32_t                     appExitRequestCount;   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR */
    uint32_t                     gainMonAssertedCount;  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_GAIN_MONITOR_ASSERTED */
    uint32_t                     periodEndedErrorCount; /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES*/
} adi_adrv904x_DfeAppCalClgcStatusInt_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADRV904X_DFE_CAL_CLGC_INT_TYPES_H_ */
