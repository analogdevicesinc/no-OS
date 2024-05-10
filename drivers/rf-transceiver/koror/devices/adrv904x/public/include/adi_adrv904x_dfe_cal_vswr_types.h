/**
 * Copyright 2015 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_cal_vswr_types.h
 *
 * \brief Contains ADRV904X data types for on board dfe vswr feature
 *
 * ADRV904X API Version: 2.10.0.4
 */


#ifndef ADRV904X_DFE_CAL_VSWR_TYPES_H__
#define ADRV904X_DFE_CAL_VSWR_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cals_structs.h"

/**
 * \brief VSWR Ctrl command mask
 */
#define ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_COMMAND_MASK    0x0Fu

/**
 * \brief Flag to distinguish between set and get Ctrl commands
 */
#define ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_SET_FLAG        0x10u

/**
 * \brief Number of reflections to report status on
 */
#define ADI_ADRV904X_DFE_APP_CAL_VSWR_NUM_REFLECTIONS      3u


/**
 * \brief Enumerated list of options of VSWR Ctrl command
 */
typedef enum adi_adrv904x_DfeAppCalVswrCtrlCmd
{
    ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_CONFIG, /*!< Set/get VSWR config */
    ADI_ADRV904X_DFE_APP_CAL_VSWR_CTRL_RESET,  /*!< Reset VSWR */
} adi_adrv904x_DfeAppCalVswrCtrlCmd_e;

/**
 * \brief Data structure to configure an alarm
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalVswrAlarmCfg
{
    int32_t thresholdmdB;       /*!< Return loss values above this are considered */
    uint8_t iterWindow;         /*!< This is the number of iterations from which the return loss will be considered (0 disables alarm, max is 64) */
    uint8_t iterThresholdCount; /*!< If the return loss is above the threshold for this many times in the window, it is an alarm condition */
} adi_adrv904x_DfeAppCalVswrAlarmCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold VSWR configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalVswrCfg
{
    uint32_t                  accumNumIter;         /*!< Number of accumulations in each capture */
    int32_t                   returnLossThreshmdB;  /*!< Upper threshold of reported reflections in milli dB */
    int32_t                   reversePathOffsetmdB; /*!< Offset applied to reverse path gain before calculating return loss */
    adi_adrv904x_DfeAppCalVswrAlarmCfg_t minorAlarm;           /*!< Minor alarm configuration - increments status counter */
    adi_adrv904x_DfeAppCalVswrAlarmCfg_t majorAlarm;           /*!< Major alarm configuration - increments status counter and raises GP Int. Cleared by resetting VSWR. */
} adi_adrv904x_DfeAppCalVswrCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold return loss info for a reflection. Only valid when delay values are non 0.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalVswrReflection
{
    int32_t  returnLossmdB;   /*!< Return loss in milli dB (positive value means observed gain is higher in forward dir) */
    uint16_t sampleDelay;     /*!< Reflection delay in ORx samples from VSWR playback signal injection point to ORx */
    uint8_t  fractionalDelay; /*!< Fractional (1/64) sample path delay to be added to above */
} adi_adrv904x_DfeAppCalVswrReflection_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold VSWR status values
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalVswrStatus
{
    adi_adrv904x_CalStatus_t hdr;                                          /*!< Common cal status header */

    adi_adrv904x_DfeAppCalVswrReflection_t  reflection[ADI_ADRV904X_DFE_APP_CAL_VSWR_NUM_REFLECTIONS]; /*!< Reflection coefficients */
    uint16_t                     fwdSampleDelay;                               /*!< Forward path delay in ORx samples from VSWR playback signal injection point to ORx */
    uint8_t                      fwdFractionalDelay;                           /*!< Fractional (1/64) sample path delay to be added to above */
    int32_t                      fwdGainmdB;                                   /*!< Forward path gain in milli dB */
    uint64_t                     fwdCaptureTimeStamp_us;                       /*!< Timestamp of forward capture */
    uint64_t                     revCaptureTimeStamp_us;                       /*!< Timestamp of reverse capture */
    int32_t                      fwdCapturePowermdB;                           /*!< Average power in forward accumulated capture (milli-dbFS) */
    int32_t                      revCapturePowermdB;                           /*!< Average power in reverse accumulated capture (milli-dbFS) */
    int32_t                      txPlaybackPowermdB;                           /*!< Average power in Tx playback waveform (milli-dbFS) */
    uint16_t                     captureAccumLength;                           /*!< Length in ORx samples of each accumulation */

    uint32_t                     calExitRequestCount;                          /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR */
    uint32_t                     capStartRetryCount;                           /*!< Capture retries due to start error */
    uint32_t                     capAbortRetryCount;                           /*!< Capture retries due to abort (e.g. mapping changes) */
    uint32_t                     capInvalidRetryCount;                         /*!< Capture retries due to invalid/incomplete capture */
    uint32_t                     capAbortErrorCount;                           /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_ABORTED_CAPTURE */
    uint32_t                     capInvalidErrorCount;                         /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_INCOMPLETE_CAPTURE */
    uint32_t                     capStartErrorCount;                           /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR */
    uint32_t                     periodEndedErrorCount;                        /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES */
    uint32_t                     txAttenChangeCount;                           /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_TX_ATTEN_CHANGED */
    uint32_t                     fwdPathUnstableCount;                         /*!< Count for ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_FWD_PATH_UNSTABLE */
    uint32_t                     minorAlarmCount;                              /*!< Count of minor alarms */
    uint32_t                     majorAlarmCount;                              /*!< Count of major alarms */
} adi_adrv904x_DfeAppCalVswrStatus_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADRV904X_DFE_CAL_VSWR_TYPES_H__ */


