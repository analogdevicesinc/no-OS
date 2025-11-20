/**
 * Copyright 2015 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_cal_clgc_types.h
 *
 * \brief Contains DFE Application type definitions for CLGC, visible to BBIC API
 *        and customers.
 *
 * ADRV904X API Version: 2.10.0.4
 */


#ifndef ADRV904X_DFE_CAL_CLGC_TYPES_H_
#define ADRV904X_DFE_CAL_CLGC_TYPES_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_dfe_capture_cmn_t.h"
#include "adi_adrv904x_cals_structs.h"

/**
 * \brief CLGC CTRL commands
 */
#define ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_FLAG    (0x80u)
#define ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_COMMAND_MASK    (0x0Fu)
#define ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_SET_FLAG        (0x10u)

/**
 * \brief CLGC capture modes
 */
typedef enum adi_adrv904x_DfeAppCalClgcCaptureMode
{
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CAPTURE_MODE_TSSI,         /*!< measurements based on TSSI power meters */
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CAPTURE_MODE_PEAK_DET,     /*!< measurements based on DPD peak detection captures */
} adi_adrv904x_DfeAppCalClgcCaptureMode_e;

/**
 * \brief Enumerated list of options of CLGC ctrl cmd
 */
typedef enum adi_adrv904x_DfeAppCalClgcCtrlCmd
{
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_CONFIG       = 0, /*!< config track config structure */
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_CONFIG     = 1, /*!< config capture config structure */
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_RUN          = 2, /*!< run CLGC track once */
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_TRACK_RESET        = 3, /*!< reset CLGC track */
    ADI_ADRV904X_DFE_APP_CAL_CLGC_CTRL_CAPTURE_BUF_ACCESS = 4, /*!< Capture buffer access command for debugging purposes */
} adi_adrv904x_DfeAppCalClgcCtrlCmd_e;

/**
 * \brief Data structure to hold CLGC capture configuration parameters
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcCaptureCfg
{
    adi_adrv904x_DfeAppCalClgcCaptureMode_e capMode;         /*!< CLGC capture mode: TSSI or peak detection */
    uint32_t                     numCapBatches;   /*!< number of TSSI measurement batches in TSSI mode */
    uint32_t                     capDurationUs;   /*!< TSSI measurement time (usec) of one batch in TSSI mode */
    uint32_t                     capPeriodUs;     /*!< timeout (usec) for TSSI timer of one batch, or window length in peak det mode */
    float                        minTxPowThres;   /*!< minimum Tx power threshold needed to run CLGC tracking for each of batches */
    float                        minOrxPowThres;  /*!< minimum ORx power threshold needed to run CLGC tracking for each of batches */
} adi_adrv904x_DfeAppCalClgcCaptureCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold CLGC tracking configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcTrackCfg
{
    uint8_t enClgc;                /*!< enable (1) or disable closed loop gain control */
    uint8_t enPaProtect;           /*!< enable (1) or disable PA protection assert check in CLGC */
    float   expLoopPowGain;        /*!< expected Tx-ORx loop power gain for CLGC to track on */
    float   maxLoopGainAdjustdB;   /*!< maximum loop gain adjustment in dB for Tx atten */
    float   minTxAttenLimitdB;     /*!< minimum Tx attenuation limit in dB */
    float   maxTxAttenLimitdB;     /*!< maximum Tx attenuation limit in dB */
} adi_adrv904x_DfeAppCalClgcTrackCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of CLGC status
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalClgcStatus
{
    adi_adrv904x_CalStatus_t hdr;                   /*!< cal status header */
    float                        txPwr;                 /*!< Tx power measured */
    float                        orxPwr;                /*!< ORx power measured */
    float                        loopPowGain;           /*!< current loop gain */
    float                        txAttenAdjdB;          /*!< current Tx atten adjustment */
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
} adi_adrv904x_DfeAppCalClgcStatus_t;
ADI_ADRV904X_PACK_FINISH


#endif /* ADRV904X_DFE_CAL_CLGC_TYPES_H_ */


