/**
 * Copyright 2023 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_ctc_actuator_cmn_t.h
 * \brief DFE CTC actuator service module definitions and types
 *
 * DFE SDK API Version: 2.9.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_
#define ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_ROW    (8u)
#define ADI_ADRV904X_DFE_SVC_DFE_CTC_ACT_MAX_NUM_DEC    (4u)

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
 * \brief Data structure to hold the configurations per row
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t                           kPower;        /*!< power order of the sample magnitude */
    uint8_t                           numOfCols;     /*!< number of IIR columns in a row (The first column is the low-pass IIR filter and the subsequent columns are all-pass IIR filters) */
    double                            fLutAddrScale; /*!< ensure that all fLut entries are used for each row*/
    double                            tau;           /*!< L0B0, L0A1, L1B0, L1B1, and L1A1 can be derived from a particular tau */
    adi_adrv904x_DfeSvcDfeCtcActuatorDecimator_e decSelect;     /*!< Decimator select for this row */
} adi_adrv904x_DfeSvcDfeCtcActuatorRowConfig_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold configurations per decimator
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    double  kLutAddrScale;                                                         /*!< ensure that all kLUT entries are used */
    double  absScale;                                                              /*!< scale the envelop, for better estimation (improving the condition number of the matrix) and actuation */
    double  absOffset;                                                             /*!< shifting the output of IIR filters so that the values are all positive and have proper addressing for the kLUT contents */
    uint8_t decRatio;                                                              /*!< CIC decimation ratio, Can be used to calculate gain compensation */
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
#endif  /* ADRV904X_DFE_SVC_DFE_CTC_ACTUATOR_CMN_T_H_ */



