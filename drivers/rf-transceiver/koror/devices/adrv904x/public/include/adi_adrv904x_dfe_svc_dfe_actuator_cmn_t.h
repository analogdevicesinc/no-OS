/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_actuator_cmn_t.h
 * \brief DFE actuator service module definitions and types
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_ACTUATOR_CMN_T_H_
#define ADRV904X_DFE_SVC_DFE_ACTUATOR_CMN_T_H_

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/**
 * \brief Enumerated list of options of GMP and DDR mode
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE0,                   /*!< GMP0-50 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE1,                   /*!< GMP0-32 DDR7-6 DDR8-6 DDR9-6 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE2,                   /*!< GMP0-38 DDR7-6 DDR9-6 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE3,                   /*!< GMP0-38 DDR7-4 DDR8-4 DDR9-4 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE4,                   /*!< GMP0-44 DDR7-6 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE5,                   /*!< GMP0-44 DDR9-6 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE6,                   /*!< GMP0-44 DDR7-3 DDR9-3 */
    ADI_ADRV904X_DFE_SVC_DFE_ACT_GMP_DDR_MODE7,                   /*!< GMP0-32 GMP1-9 DDR7-3 DDR8-3 DDR9-3 */
} adi_adrv904x_DfeSvcDfeActuatorGmpDdrMode_e;

typedef uint8_t adi_adrv904x_DfeSvcDfeActuatorGmpDdrMode_t;

/**
 * \brief Data structure to hold actuator saturation info
 */
typedef struct
{
    uint32_t dinsqCalcCmulSat;         /*!< Indicates the saturation of cmul for dinsq calculation for DDR */
    uint32_t magnitudeGainSat;         /*!< Indicates Magnitude saturation because of mag gain multiplication */
    uint32_t gmpDdrDpSat;              /*!< Indicates GMP DDR Datapath Saturation */
} adi_adrv904x_DfeSvcDfeActuatorSaturation_t;

#endif  /* ADRV904X_DFE_SVC_DFE_ACTUATOR_CMN_T_H_ */

