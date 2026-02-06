/**
 * Copyright 2023 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_ctc_actuator_cmn_t.h
 * \brief DFE CTC actuator service module definitions and types
 *
 * DFE SDK API Version: 2.15.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_
#define ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_

#include "adi_adrv904x_platform_pack.h"

#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW      (8u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_COL      (8u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC      (4u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_FLUTS    (32u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_FLUT_DEPTH       (32u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_TC       (4u)
/**
 * \brief Enumerated list of decimator selections
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_DECIMATOR_0,             /*!< CTC actuator first decimator */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_DECIMATOR_1,             /*!< CTC actuator second decimator */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_DECIMATOR_2,             /*!< CTC actuator third decimator */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_DECIMATOR_3,             /*!< CTC actuator fourth decimator */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_NUM_DECIMATORS           /*!< Number of decimators */
} adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e;

/**
 * \brief Enumerated list of input soruce selections
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_INPUT_CFR_IN,            /*!< CFR input as CTC input data */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_INPUT_CFR_ENG0_OUT,      /*!< CFR ENG0 output as CTC input data */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_INPUT_CFR_ENG1_OUT,      /*!< CFR ENG1 output as CTC input data */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_INPUT_CFR_ENG2_OUT,      /*!< CFR ENG2 output as CTC input data */
    ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_NUM_INPUTS               /*!< Number of inputs */
} adi_adrv904x_DfeSvcDfeCtcActuatorInput_e;

/**
 * \brief Enumerated list of CTC2 Model structure selections
 */
typedef enum
{
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_2R8C  = 0u,    /*!< 2 rows, 8 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_3R5C  = 1u,    /*!< 3 rows, 5 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_4R4C  = 2u,    /*!< 4 rows, 4 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_2R5C  = 3u,    /*!< 2 rows, 5 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_2R4C  = 4u,    /*!< 2 rows, 4 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_3R4C  = 5u,    /*!< 3 rows, 4 columns per tau */
    ADI_ADRV904X_DFE_APP_CAL_CTC2_MODEL_STRUCT_COUNT = 6u,    /*!< Number of CTC2 structures */
} adi_adrv904x_DfeSvcDfeCtc2ModelStructure_e;

#ifndef ADI_LIBRARY_RM_FLOATS

/**
 * \brief Data structure to hold the configurations per row
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t                           kPower;        /*!< power order of the sample magnitude */
    uint8_t                           numOfCols;     /*!< number of IIR columns in a row (The first column is the low-pass IIR filter and the subsequent columns are all-pass IIR filters) */
    double                            fLutAddrScale; /*!< ensure that all fLut entries are used for each row*/
    double                            tau;           /*!< L0B0, L0A1, L1B0, L1B1, and L1A1 can be derived from a particular tau [sec]*/
    adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e decSelect;     /*!< Decimator select for this row */
} adi_adrv904x_DfeSvcDfeCtcActuatorRowConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configurations per decimator
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    double  kLutAddrScale;        /*!< Klut address scaler for decimator. Used to utilize all entries of klut input */
    double  absScale;             /*!< Scaler for the klut values */
    double  absOffset;            /*!< Offset for the klut values */
    uint8_t decRatio;             /*!< CIC decimation ratio
                                   *   Max value: One decimator up to 255, one decimator up to 127, one decimator up to 63, one decimator up to 33
                                   *   Min value: 5 for all decimators
                                   *   Min value: Number of IIR blocks * cfrRate / hsdigRate */
} adi_adrv904x_DfeSvcDfeCtcActuatorDecConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold model configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t                           numOfCicDec;                                /*!< number of CIC decimatiors, upto ADI_ADRV904X_DFE_SVC_DFE_CTC_MAX_NUM_DEC */
    adi_adrv904x_DfeSvcDfeCtcActuatorDecConfig_t decConfig[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC]; /*!< configurations per decimator */
    adi_adrv904x_DfeSvcDfeCtcActuatorInput_e     ctcInputSel;                                /*!< select CTC input data source */
    uint8_t                           numOfRows;                                  /*!< number of rows, upto ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW */
    adi_adrv904x_DfeSvcDfeCtcActuatorRowConfig_t rowCfg[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];    /*!< Configurations per row  */
} adi_adrv904x_DfeSvcDfeCtcActuatorModelCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configurations per decimator V2
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    double  kLutAddrScale;                          /*!< Klut address scaler for decimator. Used to utilize all entries of klut input */
    uint8_t decRatio;                               /*!< CIC decimation ratio
                                                     *   Max value: One decimator up to 255, one decimator up to 127, one decimator up to 63, one decimator up to 33
                                                     *   Min value: 5 for all decimators
                                                     *   Min value: Number of IIR blocks * cfrRate / hsdigRate */
} adi_adrv904x_DfeSvcDfeCtcActuatorDecConfig_v2_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Data structure to hold the configurations per row V2
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t                           numOfCols;            /*!< number of IIR columns in a row (The first column is the low-pass IIR filter and the subsequent columns are all-pass IIR filters) */
    uint8_t                           kPower;               /*!< power order of the sample magnitude */
    double                            fLutAddrScale;        /*!< ensure that all fLut entries are used for each row*/
    double                            absScale;             /*!< Scaler for the klut values */
    double                            absOffset;            /*!< Offset for the klut values */
    double                            tau;                  /*!< L0B0, L0A1, L1B0, L1B1, and L1A1 can be derived from a particular tau [sec]*/
    adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e decSelect;            /*!< Decimator select for this row */
    uint8_t                           timeConstIdx;         /*!< timeConstant index for recovery of model structure for get function */
} adi_adrv904x_DfeSvcDfeCtcActuatorRowConfig_v2_t;
ADI_ADRV904X_PACK_FINISH


/**
 * \brief Data structure to hold model configuration parameters - Version 2
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t                              numOfCicDec;                                /*!< number of CIC decimatiors, upto ADI_ADRV904X_DFE_SVC_DFE_CTC_MAX_NUM_DEC */
    adi_adrv904x_DfeSvcDfeCtcActuatorDecConfig_v2_t decConfig[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC]; /*!< configurations per decimator */
    adi_adrv904x_DfeSvcDfeCtcActuatorInput_e        ctcInputSel;                                /*!< select CTC input data source */
    uint8_t                              numOfRows;                                  /*!< number of rows, upto ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW */
    adi_adrv904x_DfeSvcDfeCtcActuatorRowConfig_v2_t rowCfg[ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW];    /*!< Configurations per row  */
} adi_adrv904x_DfeSvcDfeCtcActuatorModelCfg_v2_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADI_LIBRARY_RM_FLOATS */

#endif /* ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_ */



