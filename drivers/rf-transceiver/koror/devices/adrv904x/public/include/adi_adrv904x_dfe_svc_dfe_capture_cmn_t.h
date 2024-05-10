/**
 * Copyright 2020 - 2023 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_capture_cmn_t.h
 * \brief DFE capture service module definitions and types
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_CAPTURE_CMN_T_H_
#define ADRV904X_DFE_SVC_DFE_CAPTURE_CMN_T_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/**
 * \brief Define maximum number of DPD capture buffers
 */
#define DPD_NUM_CAPTURE_BUFFER_MAX    (8u)

/**
 * \brief Enumerated list of options to select capture channel source
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CH_SRC_SEL_TX,                   /*!< Tx capture channel */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CH_SRC_SEL_ALT_TX,               /*!< Alt Tx capture channel */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CH_SRC_SEL_ORX,                  /*!< ORx capture channel */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_OF_CH_SRC_SEL                /*!< Num of capture channels */
} adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e;

/**
 * \brief Enumerated list of options to select Rx capture channel source
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_CH_SRC_SEL_RX,                   /*!< Rx capture channel */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_CH_SRC_SEL_ALT_RX,               /*!< Alt Rx capture channel */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_NUM_OF_CH_SRC_SEL                /*!< Num of capture channels */
} adi_adrv904x_DfeSvcDfeRxCaptureChSrcSel_e;

/**
 * \brief Defines the capture type being requested.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_IDLE,                                          /*!< Nothing to capture. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_FWD_VSWR,                                      /*!< Forward VSWR capture requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_REV_VSWR,                                      /*!< Reverse VSWR capture requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_CLGC,                                          /*!< CLGC capture type requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_DPD_CT,                                        /*!< DPD CT capture type requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_DPD_SIGSTAT,                                   /*!< DPD Sig Stat capture type requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_DPD_ONPASS,                                    /*!< DPD On Pass capture type requested. */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_DPD_REQUEST,                                   /*!< DPD capture type requested. This capture uses FWD path for Orx channel */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_DPD_REV_REQUEST,                               /*!< DPD capture type requested. This capture uses REV path for Orx channel */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PWR_MEAS_FWD,                                  /*!< Tx+ORx+Rx Power measurement with ORx FWD direction requested. This capture type should only be used in radio sequencer mode */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PWR_MEAS_REV,                                  /*!< Tx+ORx+Rx Power measurement with ORx REV direction requested. This capture type should only be used in radio sequencer mode */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_RX,                                            /*!< Rx capture requested. This capture type should only be used in radio sequencer mode */
    ADI_ADRV904X_DFE_SVC_DFE_NUM_OF_CAPTURE_TYPES                                   /*!< Number of capture types. */
} adi_adrv904x_DfeSvcDfeCaptureType_e;

/**
 * \brief Enumerated list of options to select capture trigger source
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TRIG_SRC_SW,                     /*!< SW trigger */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TRIG_SRC_RADIO,                  /*!< Radio sequencer trigger */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TRIG_SRC_GPIO,                   /*!< GPIO trigger */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TRIG_SRC_SLICE,                  /*!< Slice trigger */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_TRIG_TYPES                   /*!< Number of trigger source types */
} adi_adrv904x_DfeSvcDfeCaptureTrigSrc_e;

/**
 * \brief Enumerated list of options to select capture point
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PT_PRE_CFR,                      /*!< select pre CFR data */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PT_PRE_DPD,                      /*!< select pre DPD input */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PT_POST_DPD,                     /*!< select post DPD output */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PT_CT_DPD,                       /*!< CT_DPD capture mode */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_PT_NO_CAPTURE,                   /*!< no capture mode, zeros on bus to save power */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_NUM_PT_TYPES                     /*!< Number of capture point types */
} adi_adrv904x_DfeSvcDfeCapturePoint_e;

/**
 * \brief Enumerated list of options to select Rx capture point
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_PT_DC_OFFSET,                    /*!< select DC offset data */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_PT_HB2_INPUT,                    /*!< select HB2 input */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_PT_QFIR_OUTPUT,                  /*!< select QFIR output */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_PT_QFIR_INPUT,                   /*!< select QFIR input */
    ADI_ADRV904X_DFE_SVC_DFE_RX_CAPTURE_NUM_PT_TYPES                     /*!< Number of capture point types */
} adi_adrv904x_DfeSvcDfeRxCapturePoint_e;

/**
 * \brief Enumerated list for generic capture period types.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_POWER_MEAS_DIR_REV,                /*!< Orx Power measurement REV direction */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_POWER_MEAS_DIR_FWD,                /*!< Orx Power measurement FWD direction */
} adi_adrv904x_DfeSvcDfeCapturePowerMeasDir_e;

/**
 * \brief Enumerated list of capture target DPD model indexes.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TARGET_DPD_MODEL_INDEX_ANY,   /*!< Capture target DPD model index any */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TARGET_DPD_MODEL_INDEX_0,     /*!< Capture target DPD model index 0 */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TARGET_DPD_MODEL_INDEX_1,     /*!< Capture target DPD model index 1 */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TARGET_DPD_MODEL_INDEX_2,     /*!< Capture target DPD model index 2 */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_TARGET_DPD_MODEL_INDEX_3,     /*!< Capture target DPD model index 3 */
} adi_adrv904x_DfeSvcDfeCaptureTargetDpdModelIndex_e;

/**
 * \brief Enumerated list of SBET capture ingore count work type.
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_IGNORE_WITHOUT_CHECKING_MATCH_MODEL,     /*!< Ignore count without checking the match model */
    ADI_ADRV904X_DFE_SVC_DFE_CAPTURE_IGNORE_CHECKING_MATCH_MODEL,             /*!< Ignore count with checking the match model */
}adi_adrv904x_DfeSvcDfeCaptureIgnCntMode_e;

/**
 * \brief Data structure to hold sbet parameter.
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    adi_adrv904x_DfeSvcDfeCaptureTargetDpdModelIndex_e targetDpdModelIndex;      /*!< sbet target model */
    adi_adrv904x_DfeSvcDfeCaptureIgnCntMode_e          ignCntMode;               /*!< sbet ignore count mode: 0- without checking model 1- checking model */
    uint8_t                                 ignCnt;                   /*!< sbet ignore count 0 ~ 255 */
} adi_adrv904x_DfeSvcDfeCaptureSbetParameter_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the stop-on-pass capture
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint16_t peakPowerThreshHigh;                    /*!< peak power high threshold */
    uint16_t peakPowerThreshLow;                     /*!< peak power low threshold */
    uint32_t sumPowerThreshHigh;                     /*!< sum power high threshold */
    uint32_t sumPowerThreshLow;                      /*!< sum power low threshold */
    uint32_t instPowerThresh;                        /*!< instantaneous power power threshold */
    uint16_t maxNumSamplesAbvThresh;                 /*!< maximum number of samples above sample power threshold */
    uint16_t minNumSamplesAbvThresh;                 /*!< minimum number of samples above sample power threshold */
    uint16_t iirPwrThreshHigh;                       /*!< IIR power at the center high threshold */
    uint16_t iirPwrThreshLow;                        /*!< IIR power at the center low threshold */
    uint8_t  capStopOnPassCriteria;                  /*!< pass criteria bit 0-8 */
} adi_adrv904x_DfeSvcDfeCaptureStopOnPassCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold capture channel source selections
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e ch0;                   /*!< capture channel 0 source selection */
    adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e ch1;                   /*!< capture channel 1 source selection */
    adi_adrv904x_DfeSvcDfeCaptureChSrcSel_e ch2;                   /*!< capture channel 2 source selection */
} adi_adrv904x_DfeSvcDfeCaptureChSrcSel_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Rx capture channel source selections
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    adi_adrv904x_DfeSvcDfeRxCaptureChSrcSel_e ch0;                   /*!< capture channel 0 source selection */
    adi_adrv904x_DfeSvcDfeRxCaptureChSrcSel_e ch1;                   /*!< capture channel 1 source selection */
} adi_adrv904x_DfeSvcDfeRxCaptureChSrcSel_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold capture channel delay settings
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint16_t ch0;                                    /*!< capture channel 0 align cycle */
    uint16_t ch1;                                    /*!< capture channel 1 align cycle */
    uint16_t ch2;                                    /*!< capture channel 2 align cycle */
} adi_adrv904x_DfeSvcDfeCaptureChDelay_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold Rx capture channel delay settings
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint16_t ch0;                                    /*!< capture channel 0 align cycle */
    uint16_t ch1;                                    /*!< capture channel 1 align cycle */
} adi_adrv904x_DfeSvcDfeRxCaptureChDelay_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the frame delay capture
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t  frameDelayEn;                           /*!< frame delay capture enable (i.e. Tx on capture) */
    uint8_t  frameDelayRestartCtrl;                  /*!< frame delay restart control (0: on capture buffer trigger rising edge, 1: on capture buffer enable rising edge.) */
    uint32_t frameDelay;                             /*!< frame delay (i.e. delay from edge of the frame) */
    uint32_t frameRollover;                          /*!< period of frame counter 10-100ms */
} adi_adrv904x_DfeSvcDfeCaptureFrameDelayCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the peak detect capture
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint32_t windowLength;                           /*!< peak search window length */
    uint8_t  peakDetIIRDecay;                        /*!< peak detect IIR decay */
    uint8_t  peakDetIIRByp;                          /*!< peak detect IIR bypass */
    uint8_t  peakDetIIREn;                           /*!< peak detect IIR enable */
    uint8_t  peakDetEn;                              /*!< peak detect enable */
} adi_adrv904x_DfeSvcDfeCapturePeakDetCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the capture statistics control
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t statsIIRByp;                             /*!< stats IIR bypass */
    uint8_t statsIIRDecay;                           /*!< stats IIR decay */
} adi_adrv904x_DfeSvcDfeCaptureStatsCtrlCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the burst capture
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint16_t burstCount;                             /*!< capture burst count */
    uint32_t burstSpace;                             /*!< capture burst spacing */
} adi_adrv904x_DfeSvcDfeCaptureBurstCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the VSWR signal accumulation
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint32_t accumNumIter;                           /*!< number of accumulation iterations (Max value = 4194303) */
    uint16_t accumLen;                               /*!< accumulation length (Max value = 2046). When VSWR playback is being used, user should configure
                                                      *                                                           accumLen = Playback data length * Orx sample rate / Cfr output sample rate.
                                                      *                                                           When accumLen is configured to be an odd number capture buffer will output one extra
                                                      *                                                           I sample which needs to be skipped. For details, please check how VSWR cal thread handles
                                                      *                                                           capture buffer data*/
} adi_adrv904x_DfeSvcDfeCaptureVswrAccumCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold capture data status
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint32_t statsPeakPwr;                         /*!< peak power within a capture */
    uint32_t statsNumSampAbvThresh;                /*!< Number of samples above threshold */
    uint32_t statsIIRPwrCapMid;                    /*!< IIR leaky integrator output sampled at 1/2 of capture length */
    uint32_t statsMaxIIRPwr;                       /*!< Max of IIR leaky integrator output over the capture length */
    uint32_t statsMinIIRPwr;                       /*!< Min of IIR leaky integrator output over the capture length */
    uint32_t completedCount;                       /*!< completed capture count */
    uint32_t completedNumIter;                     /*!< VSWR completed number of accumulations */
    uint32_t errorStatus;                          /*!< error status, see ADI_ADRV904X_DFE_SVC_DFE_ERROR define above */
    uint32_t peakPos;                              /*!< peak position */
    uint64_t tssiTx1Power;                         /*!< TSSI 1 Tx power measurement */
    uint64_t tssiTx2Power;                         /*!< TSSI 2 Tx power measurement (Possible future) */
    uint64_t tssiOrxPower;                         /*!< TSSI Orx power measurement */
} adi_adrv904x_DfeSvcDfeCaptureStats_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold IIR states for the requested blocks
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t  iirStateValid;        /*!< 1: If IIR states are valid, 0:If IIR states are invalid */
    uint8_t  iirStateMask[8];      /*!< Shows which rows/columns used to read IIR state from.Byte0:Row0,.. Byte7:Row7
                                    *       For each byte, Bit0:Column0,... Bit7:Column7*/
    uint32_t iirState[8][8];       /*!< IIR states for each column/row. Format:3.18.. TODO: Use a macro instead of 8 */
} adi_adrv904x_DfeSvcDfeCaptureIIRStates_t;
ADI_ADRV904X_PACK_FINISH

#endif  /* ADRV904X_DFE_SVC_DFE_CAPTURE_CMN_T_H_ */

