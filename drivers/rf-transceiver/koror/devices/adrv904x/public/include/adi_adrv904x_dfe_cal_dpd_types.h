/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv904x_dfe_cal_dpd_types.h
 *
 * \brief Contains ADRV904X data types for on board dfe dpd feature
 *
 * ADRV904X API Version: 2.15.0.4
 */


#ifndef ADRV904X_DFE_CAL_DPD_TYPES_H__
#define ADRV904X_DFE_CAL_DPD_TYPES_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_dfe_actuator_cmn_t.h"
#include "adi_adrv904x_dfe_svc_dfe_capture_cmn_t.h"
#include "adi_adrv904x_dfe_svc_dfe_adaptation_cmn_t.h"
#include "adi_adrv904x_dfe_svc_dfe_ctc_actuator_cmn_t.h"
#include "adi_adrv904x_cals_structs.h"


/**
 * \brief Maximum number of feature filter coefficients
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS                (64U)

/**
 * \brief Maximum number of features supported in a DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES                    (510u)

/**
 * \brief Maximum number of features supported in a CTC2 DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES               (255u)

/**
 * \brief Maxinum number of features per decimator supported in a CTC2 DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES_PER_DEC       (255u)

/**
 * \brief Maxinum number of features per update(partials) supported in a CTC2 DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES_PER_UPDATE    (255u)

/**
 * \brief Maxinum number of timeconstants per CTC2 DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_TAU                    (4u)

/**
 * \brief Number of DPD GMP power models
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS                    (4u)

/**
 * \brief Number of CTC2 power models
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_CTC2_MODELS                         (1u)

/**
 * \brief Maximum number of DPD capture samples
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM                (0x4000U)

/**
 * \brief Maximum number of DPD tdd lut switch capture batches
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_TDD_LUT_SWITCH_CAP_NUM             (4u)

/**
 * \brief Size of mask-able error array (uint32_t)
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_ERROR_MASK                      (12u)

/**
 * \brief Maximum number of customer provided peak rank ratios
 */
#define  ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM           (3u)


/**
 * \brief Maximum number of captures
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES    (8u)

/**
 * \brief Maximum number of partial group for models
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP    (2u)

/**
 * \brief Number of power-based direct learning steps, used when updateMode=ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_PWR_BASED_MU       (12)

/**
 * \brief DPD update modes
 */
typedef enum adi_adrv904x_DfeAppCalDpdUpdateMode
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_SU,          /*!< Simple Update */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_SUMO,        /*!< Simple Update Max Only */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT,         /*!< Current Max Table Update - Model Switching between M and C Table on power */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL, /*!< Model Switching between M, C and R Table on power, with handling of uncertainty region.
                                              * In this mode, the 3 dynamic models must have the same feature configuration.
                                              */
} adi_adrv904x_DfeAppCalDpdUpdateMode_e;

/**
 * \brief DPD update modes API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdUpdateMode_t;

/**
 * \brief DPD types
 */
typedef enum adi_adrv904x_DfeAppCalDpdType
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_TYPE_DPD = 0u,        /*!< Regular DPD */
    ADI_ADRV904X_DFE_APP_CAL_DPD_TYPE_CTC_1,           /*!< CTC-Mode 1 (Enhanced DPD) */
} adi_adrv904x_DfeAppCalDpdType_e;

/**
 * \brief DPD types API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdType_t;

/**
 * \brief Enumerated list of different iteration types
 */
typedef enum
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_ITERATION,                                    /*!< GMP iteration where cal updates GMP models  */
    ADI_ADRV904X_DFE_APP_CAL_DPD_CTC2_ITERATION,                                   /*!< CTC2 iteration where cal updates CTC2 models */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ITER_TYPE_COUNT,                                  /*!< Number of different iteration types */
} adi_adrv904x_DfeAppCalDpdIterationType_e;

/**
 * \brief DPD GMP and DDR modes
 */
typedef enum adi_adrv904x_DfeAppCalDpdActGmpDdrMode
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE0,    /*!< GMP0-50 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE1,    /*!< GMP0-32 DDR7-6 DDR8-6 DDR9-6 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE2,    /*!< GMP0-38 DDR7-6 DDR9-6 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE3,    /*!< GMP0-38 DDR7-4 DDR8-4 DDR9-4 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE4,    /*!< GMP0-44 DDR7-6 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE5,    /*!< GMP0-44 DDR9-6 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE6,    /*!< GMP0-44 DDR7-3 DDR9-3 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_GMP_DDR_MODE7,    /*!< GMP0-32 GMP1-9 DDR7-3 DDR8-3 DDR9-3 */
} adi_adrv904x_DfeAppCalDpdActGmpDdrMode_e;

/**
 * \brief DPD GMP and DDR modes API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdActGmpDdrMode_t;

/**
 * \brief DPD model polynomial types
 */
typedef enum adi_adrv904x_DfeAppCalDpdActPolyType
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_GMP0 = 0u,    /*!< GMP polynomial */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_GMP1 = 1u,    /*!< GMP polynomial for startup model */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR7 = 7u,    /*!< DDR7 polynomial */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR8 = 8u,    /*!< DDR8 polynomial */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_POLY_DDR9 = 9u,    /*!< DDR9 polynomial */
} adi_adrv904x_DfeAppCalDpdActPolyType_e;

/**
 * \brief DPD model polynomial types API size translation
 */
typedef uint16_t adi_adrv904x_DfeAppCalDpdActPolyType_t;

/**
 * \brief DPD LUT depth
 */
typedef enum adi_adrv904x_DfeAppCalDpdActDepth
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_DEPTH_64 = 0u,    /*!< Current depth of LUT 0: 64 deep */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_DEPTH_32 = 1u,    /*!< Current depth of LUT 1: 32 deep */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ACT_DEPTH_16 = 2u,    /*!< Current depth of LUT 2: 16 deep */
} adi_adrv904x_DfeAppCalDpdActDepth_e;

/**
 * \brief DPD LUT depth API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdActDepth_t;

/**
 * \brief DPD models
 */
typedef enum adi_adrv904x_DfeAppCalDpdModelType
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_0 = 0, /*!< DPD model 0 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_1 = 1, /*!< DPD model 1 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_2 = 2, /*!< DPD model 2 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_DPD_3 = 3, /*!< DPD model 3 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_UNITY = 4, /*!< Unity model (may be reported in activeModel field in status) */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_NONE  = 6, /*!< May be reported in updateModel field in status if no model is updated */
} adi_adrv904x_DfeAppCalDpdModelType_e;


/**
 * \brief CTC2 Models
 */
typedef enum
{
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_TYPE_0         = 0u,     /*!< Model 0 */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_COUNT          = 1u,     /*!< Number of customer CTC2 actutator models types */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_TYPE_HARDCODED = 0xFFu,  /*!< Hardcoded CTCM2 model */
} adi_adrv904x_DfeAppCalCtc2ModelType_e;

/**
 * \brief DPD models API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdModelType_t;

/**
 * \brief DPD Performance Metrics
 */
typedef enum adi_adrv904x_DfeAppCalDpdStability
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_TU_POWER         = 0u,  /*!< pre-DPD Tx power averaged in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_TU_POWER         = 1u,  /*!< pre-DPD Tx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_TX_POWER         = 2u,  /*!< post-DPD Tx power averaged in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_TX_POWER         = 3u,  /*!< post-DPD Tx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_ORX_POWER        = 4u,  /*!< ORx power averaged in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_ORX_POWER        = 5u,  /*!< ORx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_LEN      = 6u,  /*!< Number of signal performance metric entries */

    ADI_ADRV904X_DFE_APP_CAL_DPD_DIRECT_EVM            = 6u,  /*!< direct EVM in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_INDIRECT_EVM          = 7u,  /*!< indirect EVM in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_INDIRECT_ERROR        = 8u,  /*!< indirect error in one capture duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN   = 9u,  /*!< Number of stability metric check entries */

    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_ENV_POWER        = 9u,  /*!< Envelope power averaged in one update duration, debug purpose */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_ENV_POWER        = 10u, /*!< Envelope power peaked in one update duration, debug purpose */
    ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_METRICS_LEN = 11u, /*!< Number of performance metric entries */
} adi_adrv904x_DfeAppCalDpdStability_e;

#define ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_BITS    ((1u << ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_LEN) - 1u)
#define ADI_ADRV904X_DFE_APP_CAL_DPD_EVM_CHECK_BITS       (((1u << ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN) - 1u) - ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_BITS)


/**
 * \brief DPD Stability words
 */
typedef enum adi_adrv904x_DfeAppCalDpdStabilityWord
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD0 = 0u,       /*!< Error status word 0 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PER_ERR_WORD0,        /*!< Persistent error status word 0 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD1,            /*!< Error status word 1 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PER_ERR_WORD1,        /*!< Persistent error status word 1 */
    ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN,         /*!< Number of error status words */
} adi_adrv904x_DfeAppCalDpdStabilityWord_e;

/**
 * \brief DPD Recovery actions
 */
typedef enum adi_adrv904x_DfeAppCalDpdRecoveryAction
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_SKIP_UPDATE = 0u,     /*!< Skip DPD update */
    ADI_ADRV904X_DFE_APP_CAL_DPD_RESET_ALL_LUTS,       /*!< DPD reset all LUTs */
    ADI_ADRV904X_DFE_APP_CAL_DPD_RESET_DYNAMIC_STATS,  /*!< DPD reset dynamic stats */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UNUSED0,              /*!< Unused */
    ADI_ADRV904X_DFE_APP_CAL_DPD_SW_TO_R_TABLE,        /*!< switch to R table (R is computed like M but with deltaP = 0) */
    ADI_ADRV904X_DFE_APP_CAL_DPD_RESET_FIRST_DPD,      /*!< reset first DPD flag for fast DPD convergence */
    ADI_ADRV904X_DFE_APP_CAL_DPD_SW_TO_MIN_DIRECT_EVM, /*!< switch to minimum direct EVM table (need different min direct EVM tables for high and low power models) */
    ADI_ADRV904X_DFE_APP_CAL_DPD_LOG_FATAL_ERROR,      /*!< Log fatal error for persistent error */
    ADI_ADRV904X_DFE_APP_CAL_DPD_RECOVERY_ACTION_LEN,  /*!< Number of recovery actions */
} adi_adrv904x_DfeAppCalDpdRecoveryAction_e;

/**
 * \brief BW detector state
 */
typedef enum
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_BW_DET_LOW = 0u,  /*!< state when BW metrics is greater than low BW threshold */
    ADI_ADRV904X_DFE_APP_CAL_DPD_BW_DET_INTER,     /*!< state when BW metrics is less than low BW threshold but greater than high BW threshold  */
    ADI_ADRV904X_DFE_APP_CAL_DPD_BW_DET_HIGH,      /*!< state when BW metrics is less than high BW threshold (reliable path delay is aquired) */
} adi_adrv904x_DfeAppCalDpdBwDetState_e;

/**
 * \brief DPD wide band regularization buffer segmentation
 */
typedef enum adi_adrv904x_DfeAppCalDpdWbRegBufSeg
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_WB_REG_BUF_SEG_1,        /*!< 1 set training data stored for all channels */
    ADI_ADRV904X_DFE_APP_CAL_DPD_WB_REG_BUF_SEG_2,        /*!< 2 set training data stored for all channels */
    ADI_ADRV904X_DFE_APP_CAL_DPD_WB_REG_BUF_SEG_4,        /*!< 4 set training data stored for all channels */
    ADI_ADRV904X_DFE_APP_CAL_DPD_WB_REG_BUF_SEG_8,        /*!< 8 set training data stored for all channels */
} adi_adrv904x_DfeAppCalDpdWbRegBufSeg_e;

/**
 * \brief DPD Stability bit configuration
 */
typedef struct adi_adrv904x_DfeAppCalDpdStabilityBitCfg
{
    uint16_t LTGT;                        /*!< LT (less than) or GT (greater than) */
    int16_t  threshold0;                  /*!< Threshold 0 */
    int16_t  threshold1;                  /*!< Threshold 1 */
    uint16_t persistentCnt;               /*!< Persistent error count limit */
} adi_adrv904x_DfeAppCalDpdStabilityBitCfg_t;

/**
 * \brief DPD Stability action configuration
 */
typedef struct adi_adrv904x_DfeAppCalDpdStabilityActCfg
{
    uint16_t mask;                        /*!< Error status word mask */
    uint16_t actionWord;                  /*!< DPD recovery action word */
} adi_adrv904x_DfeAppCalDpdStabilityActCfg_t;

/**
 * \brief DPD Stability counters
 */
typedef struct adi_adrv904x_DfeAppCalDpdStabilityCntr
{
    uint32_t perErrCntr0[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN];    /*!< Persistent error counter 0 */
    uint32_t perErrCntr1[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN];    /*!< Persistent error counter 1 */
} adi_adrv904x_DfeAppCalDpdStabilityCntr_t;

/**
 * \brief DPD Stability configuration
 */
typedef struct adi_adrv904x_DfeAppCalDpdStabilityCfg
{
    adi_adrv904x_DfeAppCalDpdStabilityBitCfg_t bit[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_CHECK_LEN]; /*!< see adi_adrv904x_DfeAppCalDpdStabilityBitCfg_t */
    adi_adrv904x_DfeAppCalDpdStabilityActCfg_t error[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];      /*!< see adi_adrv904x_DfeAppCalDpdStabilityActCfg_t */
} adi_adrv904x_DfeAppCalDpdStabilityCfg_t;

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Structure to represent double complex number
 *
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdDblCpx
{
    double real;                           /*!< Real part of complex number */
    double imag;                           /*!< Imaginary part of complex number */
} adi_adrv904x_DfeAppCalDpdDblCpx_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief DPD hardware adaptation engine configuration structure
 *
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdAdpCfg
{
    uint16_t numDpdSamples;                /*!< Number of DPD samples */
    uint8_t  featureFilterEn;              /*!< Enable or disable feature filtering */
} adi_adrv904x_DfeAppCalDpdAdpCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of DPD partial structure
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdPartial
{
    adi_adrv904x_DfeAppCalDpdType_t partial;         /*!< DPD or CTC1 */
    uint8_t              updateOrder;     /*!< Deprecated, not being used */
    uint8_t              modelIndex;      /*!< One of the GMP power models */
} adi_adrv904x_DfeAppCalDpdPartial_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to hold parameters of a row of DPD model features
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdAdpFeatureRow
{
    int8_t                      i;         /*!< i - amplitude term */
    int8_t                      j;         /*!< j - signal term */
    uint8_t                     k;         /*!< k - power term */
    adi_adrv904x_DfeAppCalDpdActPolyType_t poly;      /*!< polynomial selection */
    adi_adrv904x_DfeAppCalDpdDblCpx_t      a;         /*!< Complex coefficient of a row of features */
} adi_adrv904x_DfeAppCalDpdAdpFeatureRow_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Data structure to hold configuration parameters of the peak detect capture
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg
{
    uint32_t windowLength;                 /*!< Peak search window length */
} adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to hold parameters of DPD model descriptor. It's used to
 *        describe the DPD model and retrieve the high/low power coefficients.
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdModelDesc
{
    uint16_t                        features;                                     /*!< number of features defined in a model */
    adi_adrv904x_DfeAppCalDpdPartial_t         dpdPartial;                                   /*!< DPD partial */
    adi_adrv904x_DfeSvcDfeActuatorGmpDdrMode_t mode;                                         /*!< GMP+DDR mode */
    adi_adrv904x_DfeAppCalDpdActDepth_t        actDepth;                                     /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
    uint8_t                         linearTerm;                                   /*!< Linear term (off-diagonal << 4 | linear term (sample offset) value) */
    uint8_t                         lutIDelay;                                    /*!< LUT i delay (GMP1 LUT I << 4 | GMP0 starting LUT I) */
    uint8_t                         lutJDelay;                                    /*!< LUT j delay (GMP1 LUT J << 4 | GMP0 starting LUT J) */
    adi_adrv904x_DfeAppCalDpdAdpFeatureRow_t   feature[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES];    /*!< GMP+DDR mixed model description */
} adi_adrv904x_DfeAppCalDpdModelDesc_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Data structure to hold DPD power meter configuration parameters
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdPowerMeterCfg
{
    uint8_t  dpdInOutGainMonitorEn;      /*!< Gain Monitor is enabled */
    int32_t  dpdInOutQualLim;            /*!< Qualifying Threshold Limit(the resolution is 1/100 dB) */
    int32_t  dpdInOutMinGainLim;         /*!< Minimum Gain Threshold Limit(the resolution is 1/100 dB) */
    int32_t  dpdInOutMaxGainLim;         /*!< Maximum Gain Threshold Limit(the resolution is 1/100 dB) */
    uint8_t  modelTableForMinGainLim;    /*!< user specified model (1, 2 or 4=unity) for a gain under-range threshold violation */
    uint8_t  modelTableForMaxGainLim;    /*!< user specified model (1, 2 or 4=unity) for a gain over-range threshold violation */
    uint8_t  dpdInOutPwrMeasDuration;    /*!< Power measurement duration */
    uint16_t dpdInOutPwrMeasContDlyCntr; /*!< Wait before continuing with power measurement after txon */
    uint8_t  dpdInOutPwrMeasEn;          /*!< Enable Tx power measurement */
    uint8_t  dpdInOutPwrMeasPause;       /*!< Pause Tx power measurement */
    uint8_t  dpdInOutPwrMeasTDDModeEn;   /*!< run power measurement in TDD mode */
} adi_adrv904x_DfeAppCalDpdPowerMeterCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold DPD capture configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdCaptureCfg
{
    adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg_t capPeakDetCfg;                                       /*!< Capture: peak detector configuration */
    uint32_t                          ctc2FrameDelay[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES]; /*!< CTC2 frame delay for each capture batch, only for TDD profiles */
} adi_adrv904x_DfeAppCalDpdCaptureCfg_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to configure BW detector
 *
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t  bwDetEn;         /*!< enable (1) or disable (0) BW detector */
    uint16_t corrLagMax;      /*!< maximum lag evaluated by autocorrelations in BW detecor */
    float    loBwBeta2Thres;  /*!< threshold (<1) for narrowband signals not suitable for path delay estimate */
    float    hiBwBeta2Thres;  /*!< threshold (<1) for wideband signals suitable for path delay estimate */
    float    alphaTrack;      /*!< leaky filter alpha for tracking path delay estimate over long time */
} adi_adrv904x_DfeAppCalDpdBwDetCfg_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Enumerated list of capture batch screening modes
 */
typedef enum adi_adrv904x_DfeAppCalDpdScreenMode
{
    ADI_ADRV904X_DFE_APP_DPD_CAP_SCREEN_NONE,      /*!< No capture batch screening */
    ADI_ADRV904X_DFE_APP_DPD_CAP_SCREEN_POWER,     /*!< Average power of capture batch must match active model and be above min threshold */
    ADI_ADRV904X_DFE_APP_DPD_CAP_SCREEN_POWER_PAR, /*!< As above, plus PAR must be below per-model threshold */
} adi_adrv904x_DfeAppCalDpdScreenMode_e;

typedef uint8_t adi_adrv904x_DfeAppCalDpdScreenMode_t;

/**
 * \brief Data structure to configure capture screening
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdCapScreenCfg
{
    adi_adrv904x_DfeAppCalDpdScreenMode_t screenMode;                                    /*!< Capture screening mode (default = NONE) */
    uint8_t                    minNumBatches;                                 /*!< Minimum number of captures needed for adaptation (default = 1) */
    int16_t                    parThreshDB[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS]; /*!< PAR thresholds for each model (1/100 dB) */
} adi_adrv904x_DfeAppCalDpdCapScreenCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to configure power based mu values.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppMuPwrBasedCfg
{
    int32_t pwrRangeHighDB; /*!< Upper power limit of range covered bu mu values array (power values above this will use muPwrBased[0]). 1/100dBFS */
    int32_t pwrRangeLowDB;  /*!< Lower power limit of range covered bu mu values array (power values below this will use muPwrBased[ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_PWR_BASED_MU-1]). 1/100 dBFS */
    /*! Array of direct learning step sizes from 0 to 100 for DPD updateMode 3 operation, will be divided by 100.
     *  Power levels between pwrRangeHighDB and pwrRangeLowDB will be divided into ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_PWR_BASED_MU sections, and mu value from this array will be used
     */
    uint8_t muPwrBased[ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_PWR_BASED_MU];
} adi_adrv904x_DfeAppMuPwrBasedCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Configuration of DPD gain config
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdGainCfg
{
    uint8_t lutResolution[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS]; /*!< Outshift, lutscale config. 0: (9, 8192), 1: (10, 4096), 2: (11, 2048). */
    int32_t magGain_mdB[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];   /*!< Actuator magnitude gain (milli dB), range is -36dB to +12dB */
    int32_t postCfrGain_mdB;                                 /*!< Post CFR gain (milli dB), range is -36dB to +12dB */
    int32_t postDpdGain_mdB;                                 /*!< Post DPD gain (milli dB), range is -76dB to 0dB */
} adi_adrv904x_DfeAppCalDpdGainCfg_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief DPD tracking calibration configuration structure
 *
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdTrackCfg
{
    adi_adrv904x_DfeAppCalDpdAdpCfg_t       adapt;                                                          /*!< Adapt: adaptation engine configuration */
    uint8_t                      runClgc;                                                        /*!< Run (if 1) CLGC or not (if 0) before DPD */
    int16_t                      filterCoef[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS];               /*!< Adapt: feature filter coefficient buffer (symmetric upto 32 complex pairs) */
    adi_adrv904x_DfeAppCalDpdUpdateMode_t   updateMode;                                                     /*!< Dynamics: 0: SU, 1: SUMO, 2: CMT 2 model update mode, 3: CMT 3 model update mode*/
    int32_t                      mThresholdDB;                                                   /*!< Dynamics: SUMO/CMT M table threshold in dBFs * 100 (default -2100) */
    uint8_t                      forceDirect;                                                    /*!< Learning mode: force to use direct learning */
    uint8_t                      indirectRegValue[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];             /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for indriect learning (M/C0/C1) */
    uint8_t                      directRegValue;                                                 /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for direct learning */
    uint8_t                      ctc2RegValue;                                                   /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for CTC 2 direct learning */
    uint8_t                      mu;                                                             /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                      ctc2Mu;                                                         /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                      lowPowerActionModel;                                            /*!< Robustness: on violation of tx/orx low power threshold, which model to select: 1, 2, or 4 (unity) */
    uint8_t                      lutResolution;                                                  /*!< Deprecated - this field has no functionality! */
    uint32_t                     numberOfMultiFrames;                                            /*!< This parameter configures the number of multiframes that Radio Sequencer capture pattern will run for. Length of each multiframe can be adjusted with 'ssb_period_in_ms' parameter in device profile.In RCI mode this parameter is a 'Don't Care' and needs to be set to 1.
                                                                                                  *                                                                                       Default value used by FW is 1(default tracking cal configuration in DFE FW). Range [1-600] */
    uint32_t                     ctc1StatsSampleNum;                                             /*!< ctc1: Number of samples for ctc1 statistics */
    uint8_t                      ctc1ManualEnabled;                                              /*!< ctc1: To enable manual selection of peak rank ratios*/
    uint8_t                      ctc1DeltaPercentage;                                            /*!< ctc1: Delta percentage which specifies the range of peak power measurements acceptable for a capture (Manual Mode) */
    uint8_t                      ctc1PeakRankRatio[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM];    /*!< ctc1: Peak rank ratios (Manual mode) */
    uint8_t                      ctc1CaptureWeighting[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM]; /*!< ctc1: Capture weights for peak rank ratios (Manual mode) */
    uint8_t                      wbRegEnable;                                                    /*!< Enable wideband regularization for maintaining in-band gain flatness */
    uint8_t                      wbRegMode;                                                      /*!< Legacy variable */
    uint8_t                      wbRegBufSelect;                                                 /*!< wb buffer selection */
    float                        wbRegAlpha;                                                     /*!< alpha, the scaling factor for features computed using captured samples, shall be within [0, 1] */
    float                        wbRegBeta;                                                      /*!< beta, the scaling factor for features computed using wideband training samples, shall be within [0, 1] */
    uint16_t                     estSizeOfCoefBias;                                              /*!< To estimate the bias of the delta coefficient of a DPD feature in direct learning mode */
    uint16_t                     ctc2EstSizeOfCoefBias;                                          /*!< To estimate the bias of the delta coefficient of ctc2 */
    uint8_t                      useLegacyGainPhaseComp;                                         /*!< Fall back to legacy gain and phase compensation if it's 1 */
    adi_adrv904x_DfeAppCalDpdBwDetCfg_t     bwDetCfg;                                                       /*!< BW detctor configuration */
    int32_t                      cThresholdDB;                                                   /*!< Dynamics: SUMO/CMT C table threshold, the unit is dBFs * 100. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint16_t                     thresholdOverlapDB;                                             /*!< Dynamics: CMT: When power is within this value of a threshold, the adjacent model will be updated with same coefficients
                                                                                                  * (if it has not been adapted before), the unit is dBFs * 100. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL.
                                                                                                  */
    adi_adrv904x_DfeAppMuPwrBasedCfg_t      muPwrBasedCfg;                                                  /*!< Learning mode: direct learning step sizes DPD updateMode 3 operation */
    int8_t                       decayP;                                                         /*!< Dynamics: SUMO/CMT M table power decay. 1/10 dB units with positive value giving power decay ration less than 1 */
    adi_adrv904x_DfeAppCalDpdCapScreenCfg_t capScreenCfg;                                                   /*!< Capture screening config */
    int32_t                      txPfirFracDelAdjustThr;                                         /*!< This is the fractional delay threshold above which the fractional delay will be compensated through tx pfir when CTC2 mode is enabled. Range[-32, 32] mapping -0.5 - +0.5 sample frac delay at capture rate */
    uint8_t                      ctc2GmpIterCount;                                               /*!< CTC2 will be enabled after each GMP model completes this number of iterations */
    uint8_t                      modelFlushDisable;                                              /*!< Disable model flushing */
    uint32_t                     ctc2EnvPwrRatioThrDB;                                           /*!< Ctc2 max to min envelope power ratio threshold, the unit is 1/100dB, above this threshold CTC2 udpates will be skipped and ctc2EnvPwrRtViolCount will be incremented */
} adi_adrv904x_DfeAppCalDpdTrackCfg_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Enumerated list of options to reset Tx DPD module
 */
typedef enum adi_adrv904x_DfeAppDpdResetMode
{
    ADI_ADRV904X_DFE_APP_DPD_RESET_FULL = 1         /*!< Resets the DPD hardware and the firmware */
} adi_adrv904x_DfeAppDpdResetMode_e;

/**
 * \brief Data structure to hold parameters of DPD path delay status
 *
 * This structure is only a part of complete DPD path delay data.
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    int32_t intActDelay;    /*!< Integer Actuator Delay between post-DPD and pre-DPD at actuator rate */
    int32_t intExtDelay;    /*!< Integer External Delay between ORx and post-DPD at capture rate */
    int32_t fracExtDelay;   /*!< Fractional External Delay between ORx and post-DPD */
} adi_adrv904x_DfeAppCalDpdPathDlyStatus_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold seeds of DPD path delay
 *
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    adi_adrv904x_DfeAppCalDpdPathDlyStatus_t seed;            /*!< pathdelay seed */
    int32_t                       optSearchEnable; /*!< enable optimum searching in seeded mode */
    int32_t                       qualifyRange;    /*!< Integer range between seeded pathdelay and estimated pathdelay to qualify the estimated pathdelay */
} adi_adrv904x_DfeAppCalDpdPathDlySeed_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to configure BW detector
 *
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    float   beta2Curr;       /*!< BW metrics beta2 of current iteration */
    float   beta2Prev;       /*!< BW metrics beta2 of previous iteration */
    float   beta2Min;        /*!< minimum BW metrics beta2 since reset */
    float   beta2Max;        /*!< maximum BW metrics beta2 since reset */
    double  extDelayTrack;   /*!< total external path delay tracked in fractional unit */
    uint8_t bwDetStateCurr;  /*!< one of those in adi_adrv904x_DfeAppCalDpdBwDetState_e */
    uint8_t bwDetStateBest;  /*!< one of those in adi_adrv904x_DfeAppCalDpdBwDetState_e with widest BW detected */
} adi_adrv904x_DfeAppCalDpdBwDetStatus_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Enumerated list of capture batch screening modes
 */
typedef enum adi_adrv904x_DfeAppCalDpdCapBatchStatus
{
    ADI_ADRV904X_DFE_APP_DPD_CAP_BATCH_STATUS_NOT_DONE,      /*!< Capture batch was not done */
    ADI_ADRV904X_DFE_APP_DPD_CAP_BATCH_STATUS_VALID,         /*!< Capture batch was valid and used for adaptation */
    ADI_ADRV904X_DFE_APP_DPD_CAP_BATCH_STATUS_OTHER_MODEL,   /*!< Capture batch was valid but did not match the adapted model so was not used */
    ADI_ADRV904X_DFE_APP_DPD_CAP_BATCH_STATUS_INVALID_POWER, /*!< Capture batch was screened out because average power did not match actuator model */
    ADI_ADRV904X_DFE_APP_DPD_CAP_BATCH_STATUS_INVALID_PAR,   /*!< Capture batch was screened out because PAR was above threshold */
} adi_adrv904x_DfeAppCalDpdCapBatchStatus_e;

/**
 * \brief DPD screen modes API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdCapBatchStatus_t;

/**
 * \brief Data structure to report status of each capture
 *
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdCapBatchInfo
{
    int16_t                        avgPwrDB;      /*!< Average pre-DPD power (1/100 dB) */
    int16_t                        parDB;         /*!< Peak Avg Ratio of pre-DPD samples (average of top N peaks / avgTu). 1/100 dB */
    adi_adrv904x_DfeAppCalDpdModelType_t      selectedModel; /*!< Model active during capture batch */
    adi_adrv904x_DfeAppCalDpdCapBatchStatus_t status;        /*!< Status of capture */
} adi_adrv904x_DfeAppCalDpdCapBatchInfo_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to report maximum and minimum flut output value
 *
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2FlutOutputInfo
{
    int32_t flutMaxReal; /*!< maximum real flut output for a row */
    int32_t flutMaxImag; /*!< maximum imag flut output for a row */
    int32_t flutMinReal; /*!< minimum real flut output for a row */
    int32_t flutMinImag; /*!< minimum imag flut output for a row */
} adi_adrv904x_DfeAppCalCtc2FlutOutputInfo_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to report summation of flut output value for each row
 *
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2FlutRowOutputInfo
{
    int32_t rowMaxReal; /*!< summation of maximum real flut output for a row */
    int32_t rowMaxImag; /*!< summation of maximum imagflut output for a row */
    int32_t rowMinReal; /*!< summation of minimum real flut output for a row */
    int32_t rowMinImag; /*!< summation of minimum imag flut output for a row */
} adi_adrv904x_DfeAppCalCtc2FlutRowOutputInfo_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to hold parameters of DPD status
 *
 * This structure is only a part of complete DPD status.
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdStatus
{
    adi_adrv904x_CalStatus_t       hdr;                                                   /*!< cal status header */
    adi_adrv904x_DfeAppCalDpdPathDlyStatus_t      dpdPathDlyStatus;                                      /*!< pathdelay status */
    adi_adrv904x_DfeSvcDfeActuatorSaturation_t    dfeActSatStatus;                                       /*!< actuator satuaration status */
    uint32_t                           powerM[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP];           /*!< SUMO/CMT M table power */
    uint32_t                           txPower;                                               /*!< SUMO/CMT Capture Tx power */
    uint32_t                           activeModel;                                           /*!< the active model table (values from adi_adrv904x_DfeAppCalDpdModelType_e) */
    uint32_t                           updatedModel;                                          /*!< the latest updated GMP model (values from adi_adrv904x_DfeAppCalDpdModelType_e) */
    uint32_t                           powerR;                                                /*!< Stability R table power */
    float                              stability[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_METRICS_LEN];      /*!< stats for stability improvement/enhancement */
    uint16_t                           errStatWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];             /*!< Error status word for DPD recovery action(s) */
    uint16_t                           actionWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];              /*!< Stability recovery action word */
    uint32_t                           capStartRetryCount;                                    /*!< capture retries due to start error */
    uint32_t                           capAbortRetryCount;                                    /*!< capture retries due to abort (e.g. mapping changes) */
    uint32_t                           capInvalidRetryCount;                                  /*!< capture retries due to invalid/incomplete capture */
    uint32_t                           maxFracEstErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MAX_FRAC_EST_CNT_ERROR */
    uint32_t                           cholSolverErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CHOL_SOLVER_ERROR */
    uint32_t                           appExitRequestCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR */
    uint32_t                           lutEntrySatErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACT_LUT_ENTRY_SAT_ERROR */
    uint32_t                           flutEntrySatErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CTC2_FLUT_ENTRY_SAT_ERROR */
    uint32_t                           stabilityErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STABILITY_ERROR */
    uint32_t                           lowPowerErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_LOW_ERROR */
    uint32_t                           highPowerErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_HIGH_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_HIGH_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_HIGH_ERROR */
    uint32_t                           saturationErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_SATURATING_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_SATURATING_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_SATURATING_ERROR */
    uint32_t                           lowPeakErrorCount;                                     /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_PEAK_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_PEAK_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_PEAK_TOO_LOW_ERROR */
    uint32_t                           capAbortErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ABORTED_CAPTURE */
    uint32_t                           pathDelayErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PATHDELAY_LAG_RANGE_ERROR */
    uint32_t                           capInvalidErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_BAD_CAPTURE_DATA and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_INCOMPLETE_CAPTURE */
    uint64_t                           lutSaturationMask[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];   /*!< bit mask for saturated luts. index[0]: DPD_MODEL_TYPE_DPD_0, index[1]: DPD_MODEL_TYPE_DPD_1, index[2]: DPD_MODEL_TYPE_DPD_2, index[3]: DPD_MODEL_TYPE_DPD_3 */
    uint32_t                           noLutUpdateCount;                                      /*!< count for when no LUT update happens (because TX is too low, or below powerM in update mode 1) */
    uint32_t                           noFlutUpdateCount;                                     /*!< count for when CTC2 model isn't updated due to an error during adaptation */
    uint32_t                           capStartErrorCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR */
    uint32_t                           periodEndedErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES */
    uint32_t                           lutUpdDmaNotTrigCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_NOT_TRIGGERED */
    uint64_t                           ctc1UpThresVals[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES];  /*!< Upper-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                           ctc1LowThresVals[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES]; /*!< Lower-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                           ctc1MaxPeakVal;                                        /*!< Maximum Peak Value after capturing signal statistics */
    uint64_t                           ctc1MinPeakVal;                                        /*!< Minimum Peak Value after capturing signal statistics */
    adi_adrv904x_DfeAppCalDpdBwDetStatus_t        bwDetStatus;                                           /*!< BW detector state and stats */
    uint8_t                            learningModeCurr;                                      /*!< 0: indirect learning update, 1: direct learning update */
    uint32_t                           powerC[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_PARTIAL_GRP];           /*!< SUMO/CMT C table power. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint8_t                            copiedModelMask;                                       /*!< Mask of models that were copied if Tx power was near M or C threshold. Only used for ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT_3_MODEL. */
    uint8_t                            mu;                                                    /*!< direct learning step size from 0 to 100, will be divided by 100 */
    adi_adrv904x_DfeAppCalDpdCapBatchInfo_t       capBatchInfo[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_CAP_BATCHES];     /*!< Detailed info for each capture */
    uint32_t                           tooFewBatchesCount;                                    /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TOO_FEW_VALID_CAPTURES */
    uint64_t                           flutSaturationMask[ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_COUNT];      /*!< bit mask for saturated Fluts. */
    uint32_t                           lastIterationType;                                     /*!< Shows which iteration type has run last time (adi_adrv904x_DfeAppCalDpdIterationType_e) */
    uint32_t                           ctc2GainCalcErrorCount;                                /*!< count for CTC2 gain calculation error */
    uint32_t                           ctc2CaptureErrorCount;                                 /*!< count for CTC2 capture error */
    uint8_t                            ctc2CapDelayValid;                                     /*!< flag showing if CTC2 capture delay calculation is complete */
    float                              ctc2FracDelayNormPfir;                                 /*!< Accumulated fractional delay compensation value through Pfir */
    int32_t                            ctc2PreDpdToOrxPathDelay;                              /*!< CTC2 Pre Dpd to Orx path delay */
    int32_t                            maxIIROutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];         /*!< CTC2 maximum iir output */
    int32_t                            minIIROutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];         /*!< CTC2 minimum iir output */
    int32_t                            minIIRState[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];          /*!< CTC2 minimum iir state */
    uint32_t                           ctc2FlutSwapFailedCount;                               /*!< CTC2 count for CTC2 flut swap after flut update failure */
    adi_adrv904x_DfeAppCalCtc2FlutOutputInfo_t    ctc2FlutOutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];       /*!< CTC2 flut output: max value and min value of each row */
    adi_adrv904x_DfeAppCalCtc2FlutRowOutputInfo_t ctc2RowOutput[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];        /*!< CTC2 summation of flut output of each row */
    uint8_t                            ctc2FlutSatRowMap;                                     /*!< CTC2: flut saturation row map */
    uint32_t                           magGainSatErrorCount;                                  /*!< Mag gain feature calculation saturation error count */
    uint32_t                           gmpUpdateCount[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];      /*!< Number of times each model has been updated */
    uint32_t                           ctc2EnvPwrRtViolCount;                                 /*!< Ctc2 envelope power ratio violation count */
    uint32_t                           ctc2EnvPwrRatioDB;                                     /*!< Ctc2 envelope power ratio, the unit is 1/100dB */
    uint32_t                           ctc2EnvPwrAllZeroCount;                                /*!< Ctc2 envelope power all zero cross all batches count */
} adi_adrv904x_DfeAppCalDpdStatus_t;
ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

/**
 * \brief Data structure to hold DPD capture data for each channel.
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppDpdCaptureDataRow
{
    int16_t  dpdCaptureDataI[ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM];    /*!< Array consisting of I samples of DPD capture data */
    int16_t  dpdCaptureDataQ[ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM];    /*!< Array consisting of Q samples of DPD capture data */
    uint16_t dpdCaptureSampleArrSize;                                 /*!< Number of valid I/Q samples */
} adi_adrv904x_DfeAppDpdCaptureDataRow_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold DPD capture data for channel Tx, TxAlt and ORx.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppDpdCaptureData
{
    adi_adrv904x_DfeAppDpdCaptureDataRow_t txCaptureData;      /*!< Tx capture data for DPD adaptation. */
    adi_adrv904x_DfeAppDpdCaptureDataRow_t txAltCaptureData;   /*!< Tx alternate capture data for DPD adaptation. The data captured can be either pre-DPD/post-DPD actuator or pre CFR data */
    adi_adrv904x_DfeAppDpdCaptureDataRow_t orxCaptureData;     /*!< ORx capture data for DPD adaptation */
} adi_adrv904x_DfeAppDpdCaptureData_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold WB Regularization information
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppwbRegularization
{
    uint64_t wbRegStartAddress;     /*!< start address for wb regularization buffer */
    uint32_t wbRegBufferSize;       /*!< size of wb regularization buffer */
} adi_adrv904x_DfeAppwbRegularization_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters, which will be shared by api side and fw side.
 * this structure support actuator tdd lut switch
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppDpdActTddLutSwitch
{
    uint8_t  txOnMdl1SwitchEn;                                 /*!< enable automatic switching of dpd mode after txon */
    uint8_t  txOnMdl0SwitchEn;                                 /*!< enable automatic switching of dpd mode after txon */
    uint32_t txOnMdlSwitchDlyCnt;                              /*!< time duration for switching of dpd mode after tx on */
    uint32_t capDelay[ADI_ADRV904X_DFE_APP_CAL_DPD_TDD_LUT_SWITCH_CAP_NUM]; /*!< capture delay for model0 */
}adi_adrv904x_DfeAppDpdActTddLutSwitch_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold WB Regularization data array
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppwbRegData
{
    uint16_t wbRegData[16384];       /*!< wb regularization buffer/array */
} adi_adrv904x_DfeAppwbRegData_t;
ADI_ADRV904X_PACK_FINISH

#ifndef ADI_LIBRARY_RM_FLOATS
/**
 * \brief Data structure to hold info of CTC2 model rows
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2RowCfg
{
    adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e decSelect;     /*!< Decimator select for the row. */
    uint8_t                           kLut;          /*!< Envelope(|x|) power of the row. Max value: 8. If kLut=0, then row is disabled */
    double                            tau;           /*!< Time constant of the row[sec]*/
    double                            fLutAddrScale; /*!< Flut address scaler for the row. Used to utilize all entries of flut input */
} adi_adrv904x_DfeAppCalCtc2RowCfg_t;
ADI_ADRV904X_PACK_FINISH



/**
 * \brief Data structure to hold info of CTC2 model features
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2FeatureCfg
{
    uint8_t                rowSel; /*!< Row selection for the feature. 0:Row0, 1:Row1, ... 7:Row7 */
    uint8_t                colSel; /*!< IIR column index of the feature. 0:Col0, 1:Col1, ... 7:Col7 */
    uint8_t                kFlut;  /*!< Polynomial term of the IIR column output for the feature. Max value: 14 */
    adi_adrv904x_DfeAppCalDpdDblCpx_t a;      /*!< Complex coefficient for the feature */
} adi_adrv904x_DfeAppCalCtc2FeatureCfg_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Data structure to hold parameters of CTC2 model
 *
 * It is the interfacing structure for the user to describe the CTC2 model and retrieve the
 * the feature coefficients.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2ModelDesc
{
    adi_adrv904x_DfeSvcDfeCtcActuatorInput_e     ctcInputSel;                                    /*!< Select CTC input data source */
    adi_adrv904x_DfeAppCalCtc2ModelType_e        modelSel;                                       /*!< Model select of CTC2 model, Max value : 0 */
    uint8_t                           numOfFeats;                                     /*!< Number of features in the model. Max value 255 */
    adi_adrv904x_DfeSvcDfeCtcActuatorDecConfig_t decConfig[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC];     /*!< Dec configuration. Set decRatio=0 to disable a decimator*/
    adi_adrv904x_DfeAppCalCtc2RowCfg_t           rowCfg[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];        /*!< Row configuration. Set kLut=0 to disable a row */
    adi_adrv904x_DfeAppCalCtc2FeatureCfg_t       feature[ADI_ADRV904X_DFE_APP_CAL_CTC2_DPD_MAX_NUM_FEATURES]; /*!< CTC2 model feature configuration */
} adi_adrv904x_DfeAppCalCtc2ModelDesc_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configurations per decimator
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2DecConfig
{
    double  kLutAddrScale;        /*!< Klut address scaler for decimator. Used to utilize all entries of klut input */
    uint8_t decRatio;             /*!< CIC decimation ratio
                                   *   Max value: One decimator up to 255, one decimator up to 127, one decimator up to 63, one decimator up to 33
                                   *   Min value: 5 for all decimators
                                   *   Min value: Number of IIR blocks * cfrRate / hsdigRate */
} adi_adrv904x_DfeAppCalCtc2DecConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold info of CTC2 model rows
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2RowCfg_v2
{
    uint8_t kLut;                                    /*!< Envelope(|x|) power of the row. Max value: 8. If kLut=0, then row is disabled */
    double  fLutAddrScale;                           /*!< Flut address scaler for the row. Used to utilize all entries of flut input */
    double  absScale;                                /*!< Scaler for the klut values */
    double  absOffset;                               /*!< Offset for the klut values */
} adi_adrv904x_DfeAppCalCtc2RowCfg_v2_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold info of CTC2 model rows
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2TimeConstants
{
    double  tau;              /*!< Time constant of the row[sec] */
    uint8_t modelStructure;   /*!< CTC2 model structure - see adi_adrv904x_DfeSvcDfeCtc2ModelStructure_e for values */
    uint8_t decSelect;        /*!< Decimator select for the row. see adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e for values */
} adi_adrv904x_DfeAppCalCtc2TimeConstants_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of CTC2 model V2
 *
 * It is the interfacing structure for the user to describe the CTC2 model and retrieve the
 * the feature coefficients.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtc2ModelDesc_v2
{
    uint8_t                        ctcInputSel;                                            /*!< Select CTC input data source - see adi_adrv904x_DfeSvcDfeCtcActuatorInput_e for values */
    uint8_t                        modelSel;                                               /*!< Model select of CTC2 model, Max value : 0 see adi_adrv904x_DfeAppCalCtc2ModelType_e for values */
    adi_adrv904x_DfeAppCalCtc2DecConfig_t     decConfig[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC];             /*!< Dec configuration. Set decRatio=0 to disable a decimator*/
    adi_adrv904x_DfeAppCalCtc2RowCfg_v2_t     rowCfg[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];                /*!< Row configuration. Set kLut=0 to disable a row */
    adi_adrv904x_DfeAppCalCtc2TimeConstants_t timeConstants[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_TC];          /*!< CTC2 model time constants configuration */
} adi_adrv904x_DfeAppCalCtc2ModelDesc_v2_t;

ADI_ADRV904X_PACK_FINISH
#endif /* ADI_LIBRARY_RM_FLOATS */

#endif /* ADRV904X_DFE_CAL_DPD_TYPES_H__ */


