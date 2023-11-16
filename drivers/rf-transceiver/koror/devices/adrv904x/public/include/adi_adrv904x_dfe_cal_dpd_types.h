/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_cal_dpd_types.h
 *
 * \brief Contains ADRV904X data types for on board dfe dpd feature
 *
 * ADRV904X API Version: 2.9.0.4
 */


#ifndef ADRV904X_DFE_CAL_DPD_TYPES_H__
#define ADRV904X_DFE_CAL_DPD_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_dfe_actuator_cmn_t.h"
#include "adi_adrv904x_dfe_svc_dfe_capture_cmn_t.h"
#include "adi_adrv904x_dfe_svc_dfe_adaptation_cmn_t.h"
#include "adi_adrv904x_cals_structs.h"


/**
 * \brief Maxinum number of feature filter coefficients
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS         (64U)

/**
 * \brief Maxinum number of features supported in a DPD model
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES             (255u)

/**
 * \brief Number of DPD GMP power models
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS             (4u)

/**
 * \brief Maximum number of DPD capture samples
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_CAPTURE_SAMPLES_NUM         (0x4000U)

/**
 * \brief Maximum number of DPD tdd lut switch capture batches
 */
#define  ADI_ADRV904X_DFE_APP_CAL_DPD_TDD_LUT_SWITCH_CAP_NUM      (4u)

/**
 * \brief Size of mask-able error array (uint32_t)
 */
#define ADI_ADRV904X_DFE_APP_CAL_DPD_NUM_ERROR_MASK               (12u)

/**
 * \brief Maximum number of customer provided peak rank ratios
 */
#define  ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM    (3u)


/**
 * \brief Maximum number of captures
 */

#define  EDPD_MAX_NUM_CAP_BATCHES    (8u)

/**
 * \brief DPD update modes
 */
typedef enum adi_adrv904x_DfeAppCalDpdUpdateMode
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_SU,       /*!< Simple Update */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_SUMO,     /*!< Simple Update Max Only */
    ADI_ADRV904X_DFE_APP_CAL_DPD_UPDATE_MODE_CMT,      /*!< Current Max Table Update - Model Switching between M and C Table on power */
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
    ADI_ADRV904X_DFE_APP_CAL_DPD_TYPE_CTC_2,           /*!< Charge trap correction - Not supported currently*/
} adi_adrv904x_DfeAppCalDpdType_e;

/**
 * \brief DPD types API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdType_t;

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

    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_0 = 4, /*!< CTC model 3 - Not supported currently*/
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_1 = 5, /*!< CTC model 4 - Not supported currently*/
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_2 = 6, /*!< CTC model 5 - Not supported currently*/
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_3 = 7, /*!< CTC model 6 - Not supported currently*/
    ADI_ADRV904X_DFE_APP_CAL_DPD_MODEL_TYPE_CTC_4 = 8, /*!< CTC model 7 - Not supported currently*/
} adi_adrv904x_DfeAppCalDpdModelType_e;

/**
 * \brief DPD models API size translation
 */
typedef uint8_t adi_adrv904x_DfeAppCalDpdModelType_t;

/**
 * \brief DPD Performance Metrics
 */
typedef enum adi_adrv904x_DfeAppCalDpdStability
{
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_TU_POWER = 0u,                            /*!< pre-DPD Tx power avergaed in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_TU_POWER,                                 /*!< pre-DPD Tx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_TX_POWER,                                 /*!< post-DPD Tx power avergaed in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_TX_POWER,                                 /*!< post-DPD Tx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_MEAN_ORX_POWER,                                /*!< ORx power avergaed in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_PEAK_ORX_POWER,                                /*!< ORx power peaked in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_LEN,                              /*!< Number of signal performance metric entries */
    ADI_ADRV904X_DFE_APP_CAL_DPD_DIRECT_EVM = ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_LEN, /*!< direct EVM in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_INDIRECT_EVM,                                  /*!< indirect EVM in one update duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_INDIRECT_ERROR,                                /*!< indirect error in one capture duration */
    ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN,                                 /*!< Number of performance metric entries */
} adi_adrv904x_DfeAppCalDpdStability_e;

#define ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_BITS    ((1u << ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_LEN) - 1u)
#define ADI_ADRV904X_DFE_APP_CAL_DPD_EVM_CHECK_BITS       (((1u << ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN) - 1u) - ADI_ADRV904X_DFE_APP_CAL_DPD_SIGNAL_CHECK_BITS)


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
    uint32_t perErrCntr0[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN];    /*!< Persistent error counter 0 */
    uint32_t perErrCntr1[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN];    /*!< Persistent error counter 1 */
} adi_adrv904x_DfeAppCalDpdStabilityCntr_t;

/**
 * \brief DPD Stability configuration
 */
typedef struct adi_adrv904x_DfeAppCalDpdStabilityCfg
{
    adi_adrv904x_DfeAppCalDpdStabilityBitCfg_t bit[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN];     /*!< see adi_adrv904x_DfeAppCalDpdStabilityBitCfg_t */
    adi_adrv904x_DfeAppCalDpdStabilityActCfg_t error[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];    /*!< see adi_adrv904x_DfeAppCalDpdStabilityActCfg_t */
} adi_adrv904x_DfeAppCalDpdStabilityCfg_t;

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
    adi_adrv904x_DfeAppCalDpdType_t partial;         /*!< DPD, CTC1, or CTC2 (note: no DPD+CTC1 combo) */
    uint8_t              updateOrder;     /*!< Update order (note: same order for (E)DPD High/Low power) */
    uint8_t              modelIndex;      /*!< 0/1/2: (E)DPD M/C/R models; 3 to 7: CTC models(Not supported currently) */
} adi_adrv904x_DfeAppCalDpdPartial_t;
ADI_ADRV904X_PACK_FINISH

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

/**
 * \brief Data structure to hold parameters of CTC rcdkmj table and coefficients
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtcAdpFeatureRow
{
    uint8_t                r;              /*!< CIC decimation ratio */
    int16_t                c;              /*!< c_est Laguerre filter coefficient of a time constant */
    int16_t                d;              /*!< d_est Laguerre filter coefficient of a time constant */
    uint8_t                k;              /*!< power row of |x| of a time constant */
    uint8_t                m;              /*!< IIR column */
    uint8_t                j;              /*!< polynomial term of  the IIR column output */
    adi_adrv904x_DfeAppCalDpdDblCpx_t a;              /*!< Complex coefficient of a row of features */
} adi_adrv904x_DfeAppCalCtcAdpFeatureRow_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configuration parameters of the peak detect capture
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg
{
    uint32_t windowLength;                 /*!< Peak search window length */
} adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of DPD model descriptor. It's used to
 *        describe the DPD model and retrieve the high/low power coefficients.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdModelDesc
{
    uint8_t                         features;                                     /*!< number of features defined in a model */
    adi_adrv904x_DfeAppCalDpdPartial_t         dpdPartial;                                   /*!< DPD partial */
    adi_adrv904x_DfeSvcDfeActuatorGmpDdrMode_t mode;                                         /*!< GMP+DDR mode */
    adi_adrv904x_DfeAppCalDpdActDepth_t        actDepth;                                     /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
    uint8_t                         linearTerm;                                   /*!< Linear term (off-diagonal << 4 | linear term (sample offset) value) */
    uint8_t                         lutIDelay;                                    /*!< LUT i delay (GMP1 LUT I << 4 | GMP0 starting LUT I) */
    uint8_t                         lutJDelay;                                    /*!< LUT j delay (GMP1 LUT J << 4 | GMP0 starting LUT J) */
    adi_adrv904x_DfeAppCalDpdAdpFeatureRow_t   feature[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES];    /*!< GMP+DDR mixed model description */
} adi_adrv904x_DfeAppCalDpdModelDesc_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold parameters of CTC model descriptor. It's used to
 *        describe the CTC model and retrieve the feature coefficients.
 * NOTE: This model isn't supported currently
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalCtcModelDesc
{
    uint8_t                       features;                                     /*!< number of features defined in a model */
    adi_adrv904x_DfeAppCalDpdPartial_t       ctcPartial;                                   /*!< CTC partial */
    float                         kLutAddrScale;                                /*!< kLut address scale */
    float                         fLutAddrScale;                                /*!< fLut address scale */
    adi_adrv904x_DfeAppCalCtcAdpFeatureRow_t feature[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_FEATURES];    /*!< CTC model description */
} adi_adrv904x_DfeAppCalCtcModelDesc_t;
ADI_ADRV904X_PACK_FINISH

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
    adi_adrv904x_DfeAppCalDpdCapturePeakDetCfg_t capPeakDetCfg;                            /*!< Capture: peak detector configuration */
} adi_adrv904x_DfeAppCalDpdCaptureCfg_t;
ADI_ADRV904X_PACK_FINISH

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


/**
 * \brief DPD tracking calibration configuration structure
 *
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdTrackCfg
{
    adi_adrv904x_DfeAppCalDpdAdpCfg_t     adapt;                                                          /*!< Adapt: adaptation engine configuration */
    uint8_t                    runClgc;                                                        /*!< Run (if 1) CLGC or not (if 0) before DPD */
    int16_t                    filterCoef[ADI_ADRV904X_DFE_APP_CAL_DPD_MAX_NUM_COEFFICIENTS];               /*!< Adapt: feature filter coefficient buffer (symmetric upto 32 complex pairs) */
    adi_adrv904x_DfeAppCalDpdUpdateMode_t updateMode;                                                     /*!< Dynamics: 0: SU, 1: SUMO, 2: CMT update mode*/
    int32_t                    mThresholdDB;                                                   /*!< Dynamics: SUMO/CMT M table threshold in dBFs * 100 (default -2100) */
    uint8_t                    forceDirect;                                                    /*!< Learning mode: force to use direct learning */
    uint8_t                    indirectRegValue[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS];             /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for indriect learning (M/C0/C1) */
    uint8_t                    directRegValue;                                                 /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for direct learning */
    uint8_t                    mu;                                                             /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                    lowPowerActionModel;                                            /*!< Robustness: on violation of tx/orx low power threshold, which model to select: 1, 2, or 4 (unity) */
    uint8_t                    lutResolution;                                                  /*!< Robustness: Three (outshift, lutscale) options 0: (9, 8192), 1: (10, 4096), 2: (11, 2048) */
    uint32_t                   numberOfMultiFrames;                                            /*!< This parameter configures the number of multiframes that Radio Sequencer capture pattern will run for. Length of each multiframe can be adjusted with 'ssb_period_in_ms' parameter in device profile.In RCI mode this parameter is a 'Don't Care' and needs to be set to 1.
                                                                                                *                                                                                       Default value used by FW is 1(default tracking cal configuration in DFE FW). Range [1-600] */
    uint32_t                   ctc1StatsSampleNum;                                             /*!< ctc1: Number of samples for ctc1 statistics */
    uint8_t                    ctc1ManualEnabled;                                              /*!< ctc1: To enable manual selection of peak rank ratios*/
    uint8_t                    ctc1DeltaPercentage;                                            /*!< ctc1: Delta percentage which specifies the range of peak power measurements acceptable for a capture (Manual Mode) */
    uint8_t                    ctc1PeakRankRatio[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM];    /*!< ctc1: Peak rank ratios (Manual mode) */
    uint8_t                    ctc1CaptureWeighting[ADI_ADRV904X_DFE_APP_CAL_CTC1_MAX_PEAK_RANK_RATIO_NUM]; /*!< ctc1: Capture weights for peak rank ratios (Manual mode) */
    uint8_t                    wbRegEnable;                                                    /*!< Enable wideband regularization for maintaining in-band gain flatness */
    uint8_t                    wbRegMode;                                                      /*!< 0: same wideband data for all channels, 1: Two set of wideband data (one set per chip side) */
    float                      wbRegAlpha;                                                     /*!< alpha, the scaling factor for features computed using captured samples, shall be within [0, 1] */
    float                      wbRegBeta;                                                      /*!< beta, the scaling factor for features computed using wideband training samples, shall be within [0, 1] */
    uint16_t                   estSizeOfCoefBias;                                              /*!< To estimate the bias of the delta coefficient of a DPD feature in direct learning mode */
    uint8_t                    useLegacyGainPhaseComp;                                         /*!< Fall back to legacy gain and phase compensation if it's 1 */
    adi_adrv904x_DfeAppCalDpdBwDetCfg_t   bwDetCfg;                                                       /*!< BW detctor configuration */
} adi_adrv904x_DfeAppCalDpdTrackCfg_t;
ADI_ADRV904X_PACK_FINISH

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

/**
 * \brief Data structure to hold parameters of DPD status
 *
 * This structure is only a part of complete DPD status.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeAppCalDpdStatus
{
    adi_adrv904x_CalStatus_t    hdr;                                                 /*!< cal status header */
    adi_adrv904x_DfeAppCalDpdPathDlyStatus_t   dpdPathDlyStatus;                                    /*!< pathdelay status */
    adi_adrv904x_DfeSvcDfeActuatorSaturation_t dfeActSatStatus;                                     /*!< actuator satuaration status */
    uint32_t                        powerM;                                              /*!< SUMO/CMT M table power */
    uint32_t                        txPower;                                             /*!< SUMO/CMT 10ms Tx power */
    uint32_t                        activeModel;                                         /*!< the active model table */
    uint32_t                        updatedModel;                                        /*!< the latest updated model */
    uint32_t                        powerR;                                              /*!< Stability R table power */
    float                           stability[ADI_ADRV904X_DFE_APP_CAL_DPD_STABILITY_LEN];            /*!< stats for stability improvement/enhancement */
    uint16_t                        errStatWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];           /*!< Error status word for DPD recovery action(s) */
    uint16_t                        actionWord[ADI_ADRV904X_DFE_APP_CAL_DPD_ERR_WORD_LEN];            /*!< Stability recovery action word */
    uint32_t                        capStartRetryCount;                                  /*!< capture retries due to start error */
    uint32_t                        capAbortRetryCount;                                  /*!< capture retries due to abort (e.g. mapping changes) */
    uint32_t                        capInvalidRetryCount;                                /*!< capture retries due to invalid/incomplete capture */
    uint32_t                        maxFracEstErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MAX_FRAC_EST_CNT_ERROR */
    uint32_t                        cholSolverErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CHOL_SOLVER_ERROR */
    uint32_t                        appExitRequestCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR */
    uint32_t                        lutEntrySatErrorCount;                               /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACT_LUT_ENTRY_SAT_ERROR */
    uint32_t                        stabilityErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STABILITY_ERROR */
    uint32_t                        lowPowerErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_LOW_ERROR */
    uint32_t                        highPowerErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_HIGH_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_HIGH_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_HIGH_ERROR */
    uint32_t                        saturationErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_SATURATING_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_SATURATING_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_SATURATING_ERROR */
    uint32_t                        lowPeakErrorCount;                                   /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_PEAK_TOO_LOW_ERROR, ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_PEAK_TOO_LOW_ERROR and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_PEAK_TOO_LOW_ERROR */
    uint32_t                        capAbortErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ABORTED_CAPTURE */
    uint32_t                        pathDelayErrorCount;                                 /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PATHDELAY_LAG_RANGE_ERROR */
    uint32_t                        capInvalidErrorCount;                                /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_BAD_CAPTURE_DATA and ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_INCOMPLETE_CAPTURE */
    uint64_t                        lutSaturationMask[ADI_ADRV904X_DFE_APP_CAL_DPD_GMP_POWER_MODELS]; /*!< bit mask for saturated luts. index[0]: DPD_MODEL_TYPE_DPD_0, index[1]: DPD_MODEL_TYPE_DPD_1, index[2]: DPD_MODEL_TYPE_DPD_2/reserved for future expansion, index[3]: DPD_MODEL_TYPE_DPD_3 */
    uint32_t                        noLutUpdateCount;                                    /*!< count for when no LUT update happens (because TX is too low, or below powerM in update mode 1) */
    uint32_t                        capStartErrorCount;                                  /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR */
    uint32_t                        periodEndedErrorCount;                               /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES */
    uint32_t                        lutUpdDmaNotTrigCount;                               /*!< count for ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_NOT_TRIGGERED */
    uint64_t                        ctc1UpThresVals[EDPD_MAX_NUM_CAP_BATCHES];           /*!< Upper-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                        ctc1LowThresVals[EDPD_MAX_NUM_CAP_BATCHES];          /*!< Lower-Thresholds for the Peak Value in Stops on pass captures (ctc1 mode) */
    uint64_t                        ctc1MaxPeakVal;                                      /*!< Maximum Peak Value after capturing signal statistics */
    uint64_t                        ctc1MinPeakVal;                                      /*!< Minimum Peak Value after capturing signal statistics */
    adi_adrv904x_DfeAppCalDpdBwDetStatus_t     bwDetStatus;                                         /*!< BW detector state and stats */
    uint8_t                         learningModeCurr;                                    /*!< 0: indirect learning update, 1: direct learning update */
} adi_adrv904x_DfeAppCalDpdStatus_t;
ADI_ADRV904X_PACK_FINISH

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


#endif /* ADRV904X_DFE_CAL_DPD_TYPES_H__ */


