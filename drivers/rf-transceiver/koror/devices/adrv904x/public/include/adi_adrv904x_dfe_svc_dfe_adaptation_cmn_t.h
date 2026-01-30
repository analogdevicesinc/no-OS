/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_adaptation_cmn_t.h
 * \brief DFE adaptation service module definitions and types
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_ADAPTATION_CMN_T_H_
#define ADRV904X_DFE_SVC_DFE_ADAPTATION_CMN_T_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/**
 * \brief Enumerated list of options of XACC operation mode
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_XACC_FINAL_DONE        = 0U,        /*!< Default mode: one final interrupt at the end of XACC */
    ADI_ADRV904X_DFE_SVC_DFE_XACC_INTERMEDIATE_DONE = 1U,        /*!< Legacy mode: generate interrupt for each XACC */
} adi_adrv904x_DfeSvcDfeXaccOpMode_e;

typedef uint8_t adi_adrv904x_DfeSvcDfeXaccOpMode_t;

/**
 * \brief Data structure to hold configuration parameters of hardware adaptation engines
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint16_t                xaccMaxIteration;        /*!< max number of XACC */
    uint16_t                xaccFCount;              /*!< feature count */
    uint8_t                 xaccLeak;                /*!< damping factor */
    uint8_t                 featureFilterEn;         /*!< enable/disable feature filtering */
    uint8_t                 numFilterCoefficients;   /*!< number of filter coefficients */
    uint8_t                 numFilterOutput;         /*!< number of filter output */
    adi_adrv904x_DfeSvcDfeXaccOpMode_t xaccLegacyMode;          /*!< operation mode */
} adi_adrv904x_DfeSvcDfeAdaptationCfg_t;
ADI_ADRV904X_PACK_FINISH

#endif  /* ADRV904X_DFE_SVC_DFE_ADAPTATION_CMN_T_H_ */

