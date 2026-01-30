/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv904x_dfe_cal_dpd_int_types.h
 *
 * \brief  Contains ADRV904X data types for on board dfe dpd feature, structures here
 *         use integer types instead of floating point numbers.
 *
 * ADRV904X API Version: 2.15.0.4
 */

#ifndef ADI_ADRV904X_DFE_CAL_DPD_INT_TYPES_H_
#define ADI_ADRV904X_DFE_CAL_DPD_INT_TYPES_H_

#include "adi_adrv904x_dfe_cal_dpd_types.h"

/**
 * \brief Structure to represent complex number, not using floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdDblCpxInt {
    int64_t real_e6; /*!< Real part of complex number, unit size of 1e-6 */
    int64_t imag_e6; /*!< Imaginary part of complex number, unit size of 1e-6 */
} adi_adrv904x_DfeAppCalDpdDblCpxInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of a row of DPD model features, without floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdAdpFeatureRowInt {
    int8_t                                 i;    /*!< i - amplitude term */
    int8_t                                 j;    /*!< j - signal term */
    uint8_t                                k;    /*!< k - power term */
    adi_adrv904x_DfeAppCalDpdActPolyType_t poly; /*!< polynomial selection */
    adi_adrv904x_DfeAppCalDpdDblCpxInt_t   a;    /*!< Complex coefficient of a row of features */
} adi_adrv904x_DfeAppCalDpdAdpFeatureRowInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of DPD model descriptor, without using floating point numbers.
 *        It's used to describe the DPD model and retrieve the high/low power coefficients.
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdModelDescInt
{
    uint16_t                                    features;                                               /*!< number of features defined in a model */
    adi_adrv904x_DfeAppCalDpdPartial_t          dpdPartial;                                             /*!< DPD partial */
    adi_adrv904x_DfeSvcDfeActuatorGmpDdrMode_t  mode;                                                   /*!< GMP+DDR mode */
    adi_adrv904x_DfeAppCalDpdActDepth_t         actDepth;                                               /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
    uint8_t                                     linearTerm;                                             /*!< Linear term (off-diagonal << 4 | linear term (sample offset) value) */
    uint8_t                                     lutIDelay;                                              /*!< LUT i delay (GMP1 LUT I << 4 | GMP0 starting LUT I) */
    uint8_t                                     lutJDelay;                                              /*!< LUT j delay (GMP1 LUT J << 4 | GMP0 starting LUT J) */
    adi_adrv904x_DfeAppCalDpdAdpFeatureRowInt_t feature[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES]; /*!< GMP+DDR mixed model description */
} adi_adrv904x_DfeAppCalDpdModelDescInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to configure BW detector, without floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdBwDetCfgInt
{
    uint8_t  bwDetEn;            /*!< enable (1) or disable (0) BW detector */
    uint16_t corrLagMax;         /*!< maximum lag evaluated by autocorrelations in BW detecor */
    int32_t  loBwBeta2Thres_e6;  /*!< threshold (<1) for narrowband signals not suitable for path delay estimate, unit size of 1e-6 */
    int32_t  hiBwBeta2Thres_e6;  /*!< threshold (<1) for wideband signals suitable for path delay estimate, unit size of 1e-6 */
    int32_t  alphaTrack_e6;      /*!< leaky filter alpha for tracking path delay estimate over long time, unit size of 1e-6 */
} adi_adrv904x_DfeAppCalDpdBwDetCfgInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief DPD tracking calibration configuration structure, without floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdTrackCfgInt {
    adi_adrv904x_DfeAppCalDpdAdpCfg_t       adapt;                  /*!< Adapt: adaptation engine configuration */
    uint8_t                                 runClgc;                /*!< Run (if 1) CLGC or not (if 0) before DPD */
    int16_t                                 filterCoef[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS]; /*!< Adapt: feature filter coefficient buffer (symmetric upto 32 complex pairs) */
    adi_adrv904x_DfeAppCalDpdUpdateMode_t   updateMode;             /*!< Dynamics: 0: SU, 1: SUMO, 2: CMT 2 model update mode, 3: CMT 3 model update mode*/
    int32_t                                 mThresholdDB;           /*!< Dynamics: SUMO/CMT M table threshold in dBFs * 100 (default -2100) */
    uint8_t                                 forceDirect;            /*!< Learning mode: force to use direct learning */
    uint8_t                                 indirectRegValue[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS]; /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for indriect learning (M/C0/C1) */
    uint8_t                                 directRegValue;         /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for direct learning */
    uint8_t                                 ctc2RegValue;           /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for CTC 2 direct learning */
    uint8_t                                 mu;                     /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                                 ctc2Mu;                 /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                                 lowPowerActionModel;    /*!< Robustness: on violation of tx/orx low power threshold, which model to select: 1, 2, or 4 (unity) */
    uint8_t                                 lutResolution;          /*!< Deprecated - this field has no functionality! */
    uint32_t                                numberOfMultiFrames;    /*!< This parameter configures the number of multiframes that Radio Sequencer capture pattern will run for. Length of each multiframe can be adjusted with 'ssb_period_in_ms' parameter in device profile.In RCI mode this parameter is a 'Don't Care' and needs to be set to 1.
                                                                     * Default value used by FW is 1(default tracking cal configuration in DFE FW). Range [1-600] */
    uint32_t                                ctc1StatsSampleNum;     /*!< ctc1: Number of samples for ctc1 statistics */
    uint8_t                                 ctc1ManualEnabled;      /*!< ctc1: To enable manual selection of peak rank ratios*/
    uint8_t                                 ctc1DeltaPercentage;    /*!< ctc1: Delta percentage which specifies the range of peak power measurements acceptable for a capture (Manual Mode) */
    uint8_t                                 ctc1PeakRankRatio[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM];    /*!< ctc1: Peak rank ratios (Manual mode) */
    uint8_t                                 ctc1CaptureWeighting[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM]; /*!< ctc1: Capture weights for peak rank ratios (Manual mode) */
    uint8_t                                 wbRegEnable;            /*!< Enable wideband regularization for maintaining in-band gain flatness */
    uint8_t                                 wbRegMode;              /*!< Legacy variable */
    uint8_t                                 wbRegBufSelect;         /*!< wb buffer selection */
    int32_t                                 wbRegAlpha_e6;          /*!< alpha, the scaling factor for features computed using captured samples, shall be within [0, 1], unit is 1e-6 */
    int32_t                                 wbRegBeta_e6;           /*!< beta, the scaling factor for features computed using wideband training samples, shall be within [0, 1], unit is 1e-6 */
    uint16_t                                estSizeOfCoefBias;      /*!< To estimate the bias of the delta coefficient of a DPD feature in direct learning mode */
    uint16_t                                ctc2EstSizeOfCoefBias;  /*!< To estimate the bias of the delta coefficient of ctc2 */
    uint8_t                                 useLegacyGainPhaseComp; /*!< Fall back to legacy gain and phase compensation if it's 1 */
    adi_adrv904x_DfeAppCalDpdBwDetCfgInt_t  bwDetCfg;               /*!< BW detector configuration, without floating point numbers */
    int32_t                                 cThresholdDB;           /*!< Dynamics: SUMO/CMT C table threshold, the unit is dBFs * 100. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint16_t                                thresholdOverlapDB;     /*!< Dynamics: CMT: When power is within this value of a threshold, the adjacent model will be updated with same coefficients
                                                                     * (if it has not been adapted before), the unit is dBFs * 100. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    adi_adrv904x_DfeAppMuPwrBasedCfg_t      muPwrBasedCfg;          /*!< Learning mode: direct learning step sizes DPD updateMode 3 operation */
    int8_t                                  decayP;                 /*!< Dynamics: SUMO/CMT M table power decay. 1/10 dB units with positive value giving power decay ration less than 1 */
    adi_adrv904x_DfeAppCalDpdCapScreenCfg_t capScreenCfg;           /*!< Capture screening config */
    int32_t                                 txPfirFracDelAdjustThr; /*!< This is the fractional delay threshold above which the fractional delay will be compensated through tx pfir when CTC2 mode is enabled. Range[-32, 32] mapping -0.5 - +0.5 sample frac delay at capture rate */
    uint8_t                                 ctc2GmpIterCount;       /*!< CTC2 will be enabled after each GMP model completes this number of iterations */
    uint8_t                                 modelFlushDisable;      /*!< Disable model flushing */
    uint32_t                                ctc2EnvPwrRatioThrDB;   /*!< Ctc2 max to min envelope power ratio threshold, the unit is 1/100dB, above this threshold CTC2 udpates will be skipped and ctc2EnvPwrRtViolCount will be incremented */
} adi_adrv904x_DfeAppCalDpdTrackCfgInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to configure BW detector, without floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdBwDetStatusInt
{
    int32_t beta2Curr_e6;     /*!< BW metrics beta2 of current iteration, unit size is 1e-6 */
    int32_t beta2Prev_e6;     /*!< BW metrics beta2 of previous iteration, unit size is 1e-6 */
    int32_t beta2Min_e6;      /*!< minimum BW metrics beta2 since reset, unit size is 1e-6 */
    int32_t beta2Max_e6;      /*!< maximum BW metrics beta2 since reset, unit size is 1e-6 */
    int64_t extDelayTrack_e6; /*!< total external path delay tracked in fractional unit, unit size is 1e-6 */
    uint8_t bwDetStateCurr;   /*!< one of those in adi_adrv904x_DfeAppCalDpdBwDetState_e */
    uint8_t bwDetStateBest;   /*!< one of those in adi_adrv904x_DfeAppCalDpdBwDetState_e with widest BW detected */
} adi_adrv904x_DfeAppCalDpdBwDetStatusInt_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of DPD status, without floating point numbers
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdStatusInt {
    adi_adrv904x_CalStatus_t                      hdr;                                                   /*!< cal status header */
    adi_adrv904x_DfeAppCalDpdPathDlyStatus_t      dpdPathDlyStatus;                                      /*!< pathdelay status */
    adi_adrv904x_DfeSvcDfeActuatorSaturation_t    dfeActSatStatus;                                       /*!< actuator satuaration status */
    uint32_t                                      powerM[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP];           /*!< SUMO/CMT M table power */
    uint32_t                                      txPower;                                               /*!< SUMO/CMT Capture Tx power */
    uint32_t                                      activeModel;                                           /*!< the active model table (values from adi_adrv904x_DfeAppCalDpdModelType_e) */
    uint32_t                                      updatedModel;                                          /*!< the latest updated GMP model (values from adi_adrv904x_DfeAppCalDpdModelType_e) */
    uint32_t                                      powerR;                                                /*!< Stability R table power */
    int32_t                                       stability_int[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_METRICS_LEN];  /*!< stats for stability improvement/enhancement */
    uint16_t                                      errStatWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];             /*!< Error status word for DPD recovery action(s) */
    uint16_t                                      actionWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];              /*!< Stability recovery action word */
    uint32_t                                      capStartRetryCount;                                    /*!< capture retries due to start error */
    uint32_t                                      capAbortRetryCount;                                    /*!< capture retries due to abort (e.g. mapping changes) */
    uint32_t                                      capInvalidRetryCount;                                  /*!< capture retries due to invalid/incomplete capture */
    uint32_t                                      maxFracEstErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MAX_FRAC_EST_CNT_ERROR */
    uint32_t                                      cholSolverErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CHOL_SOLVER_ERROR */
    uint32_t                                      appExitRequestCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR */
    uint32_t                                      lutEntrySatErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACT_LUT_ENTRY_SAT_ERROR */
    uint32_t                                      flutEntrySatErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CTC2_FLUT_ENTRY_SAT_ERROR */
    uint32_t                                      stabilityErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STABILITY_ERROR */
    uint32_t                                      lowPowerErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_LOW_ERROR */
    uint32_t                                      highPowerErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_HIGH_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_HIGH_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_HIGH_ERROR */
    uint32_t                                      saturationErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_SATURATING_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_SATURATING_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_SATURATING_ERROR */
    uint32_t                                      lowPeakErrorCount;                                     /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_PEAK_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_PEAK_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_PEAK_TOO_LOW_ERROR */
    uint32_t                                      capAbortErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ABORTED_CAPTURE */
    uint32_t                                      pathDelayErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PATHDELAY_LAG_RANGE_ERROR */
    uint32_t                                      capInvalidErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_BAD_CAPTURE_DATA and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_INCOMPLETE_CAPTURE */
    uint64_t                                      lutSaturationMask[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];   /*!< bit mask for saturated luts. index[0]: DPD_MODEL_TYPE_DPD_0, index[1]: DPD_MODEL_TYPE_DPD_1, index[2]: DPD_MODEL_TYPE_DPD_2, index[3]: DPD_MODEL_TYPE_DPD_3 */
    uint32_t                                      noLutUpdateCount;                                      /*!< count for when no LUT update happens (because TX is too low, or below powerM in update mode 1) */
    uint32_t                                      noFlutUpdateCount;                                     /*!< count for when CTC2 model isn't updated due to an error during adaptation */
    uint32_t                                      capStartErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR */
    uint32_t                                      periodEndedErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES */
    uint32_t                                      lutUpdDmaNotTrigCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_NOT_TRIGGERED */
    uint64_t                                      ctc1UpThresVals[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES];  /*!< Upper-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                                      ctc1LowThresVals[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES]; /*!< Lower-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                                      ctc1MaxPeakVal;                                        /*!< Maximum Peak Value after capturing signal statistics */
    uint64_t                                      ctc1MinPeakVal;                                        /*!< Minimum Peak Value after capturing signal statistics */
    adi_adrv904x_DfeAppCalDpdBwDetStatusInt_t     bwDetStatus;                                           /*!< BW detector state and stats, without floating point numbers */
    uint8_t                                       learningModeCurr;                                      /*!< 0: indirect learning update, 1: direct learning update */
    uint32_t                                      powerC[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP];           /*!< SUMO/CMT C table power. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint8_t                                       copiedModelMask;                                       /*!< Mask of models that were copied if Tx power was near M or C threshold. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint8_t                                       mu;                                                    /*!< direct learning step size from 0 to 100, will be divided by 100 */
    adi_adrv904x_DfeAppCalDpdCapBatchInfo_t       capBatchInfo[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES];     /*!< Detailed info for each capture */
    uint32_t                                      tooFewBatchesCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TOO_FEW_VALID_CAPTURES */
    uint64_t                                      flutSaturationMask[ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_COUNT];      /*!< bit mask for saturated Fluts. */
    uint32_t                                      lastIterationType;                                     /*!< Shows which iteration type has run last time (adi_adrv904x_DfeAppCalDpdIterationType_e) */
    uint32_t                                      ctc2GainCalcErrorCount;                                /*!< count for CTC2 gain calculation error */
    uint32_t                                      ctc2CaptureErrorCount;                                 /*!< count for CTC2 capture error */
    uint8_t                                       ctc2CapDelayValid;                                     /*!< flag showing if CTC2 capture delay calculation is complete */
    int32_t                                       ctc2FracDelayNormPfir_e6;                              /*!< Accumulated fractional delay compensation value through Pfir, unit size 1e-6 */
    int32_t                                       ctc2PreDpdToOrxPathDelay;                              /*!< CTC2 Pre Dpd to Orx path delay */
    int32_t                                       maxIIROutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];         /*!< CTC2 maximum iir output */
    int32_t                                       minIIROutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];         /*!< CTC2 minimum iir output */
    int32_t                                       minIIRState[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];          /*!< CTC2 minimum iir state */
    uint32_t                                      ctc2FlutSwapFailedCount;                               /*!< CTC2 count for CTC2 flut swap after flut update failure */
    adi_adrv904x_DfeAppCalCtc2FlutOutputInfo_t    ctc2FlutOutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];       /*!< CTC2 flut output: max value and min value of each row */
    adi_adrv904x_DfeAppCalCtc2FlutRowOutputInfo_t ctc2RowOutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];        /*!< CTC2 summation of flut output of each row */
    uint8_t                                       ctc2FlutSatRowMap;                                     /*!< CTC2: flut saturation row map */
    uint32_t                                      magGainSatErrorCount;                                  /*!< Mag gain feature calculation saturation error count */
    uint32_t                                      gmpUpdateCount[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];      /*!< Number of times each model has been updated */
    uint32_t                                      ctc2EnvPwrRtViolCount;                                 /*!< Ctc2 envelope power ratio violation count */
    uint32_t                                      ctc2EnvPwrRatioDB;                                     /*!< Ctc2 envelope power ratio, the unit is 1/100dB */
    uint32_t                                      ctc2EnvPwrAllZeroCount;                                /*!< Ctc2 envelope power all zero cross all batches count */
} adi_adrv904x_DfeAppCalDpdStatusInt_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADI_ADRV904X_DFE_CAL_DPD_INT_TYPES_H_ */
