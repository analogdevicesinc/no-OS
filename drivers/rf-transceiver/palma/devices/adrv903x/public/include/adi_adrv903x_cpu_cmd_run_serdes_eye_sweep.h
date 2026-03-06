/**
 * \file adi_adrv903x_cpu_cmd_run_serdes_eye_sweep.h
 * 
 * \brief   Command definition for ADI_ADRV903X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP
 *
 * \details Command definition for ADI_ADRV903X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADI_ADRV903X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__
#define __ADI_ADRV903X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

/**
 * \brief Deserializer lane enumeration
 */
typedef enum adi_adrv903x_CpuCmd_DeserializerLane
{
    ADI_ADRV903X_CPU_CMD_DESER_LINE0 = 0u,  /*!< Deserializer lane 0 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE1 = 1u,  /*!< Deserializer lane 1 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE2 = 2u,  /*!< Deserializer lane 2 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE3 = 3u,  /*!< Deserializer lane 3 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE4 = 4u,  /*!< Deserializer lane 4 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE5 = 5u,  /*!< Deserializer lane 5 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE6 = 6u,  /*!< Deserializer lane 6 */
    ADI_ADRV903X_CPU_CMD_DESER_LINE7 = 7u,  /*!< Deserializer lane 7 */
} adi_adrv903x_CpuCmd_DeserializerLane_e;

/**
 * \brief PRBS Pattern type enumeration
 */
typedef enum adi_adrv903x_CpuCmd_PrbsPattern
{
    ADI_ADRV903X_CPU_CMD_PRBS_PATTERN7  = 0u,  /*!< PRBS pattern type 7 */
    ADI_ADRV903X_CPU_CMD_PRBS_PATTERN9  = 1u,  /*!< PRBS pattern type 9 */
    ADI_ADRV903X_CPU_CMD_PRBS_PATTERN15 = 2u,  /*!< PRBS pattern type 15 */
    ADI_ADRV903X_CPU_CMD_PRBS_PATTERN31 = 3u,  /*!< PRBS pattern type 31 */
} adi_adrv903x_CpuCmd_PrbsPattern_e;

/**
 * \brief RUN_SERDES_EYE_SWEEP command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_RunEyeSweep
{
    adi_adrv903x_CpuCmd_DeserializerLane_e   lane;      /*!< Deserializer lane number */
    adi_adrv903x_CpuCmd_PrbsPattern_e prbsPattern;      /*!< PRBS pattern */
    uint8_t                       forceUsingOuter;      /*!< Flag indicating 'outer' should be used for phase detection
                                                             0 (default) - do not use outer for phase detection,
                                                             1 -         - use outer for phase detection */
    uint32_t                      prbsCheckDuration_ms; /*!< Duration of PRBS check in [ms] */
} adi_adrv903x_CpuCmd_RunEyeSweep_t;)

/**
 * \brief RUN_SERDES_EYE_SWEEP command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_RunEyeSweepResp
{
    int8_t  spoLeft;   /*!< SPO Left value */
    int8_t  spoRight;  /*!< SPO Right value */
} adi_adrv903x_CpuCmd_RunEyeSweepResp_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_RunVertEyeSweep
{
    adi_adrv903x_CpuCmd_DeserializerLane_e lane;   /*!< Deserializer lane number */
} adi_adrv903x_CpuCmd_RunVertEyeSweep_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_RunVertEyeSweepResp
{
    int8_t    eyeHeightsAtSpo[512];         /*!< store the upper and lower eye height at 16 SPOs*/
} adi_adrv903x_CpuCmd_RunVertEyeSweepResp_t;)


/**
 * \brief JESD_GET_RX_LANE_SINT_CODES command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_GetRxLaneSintCodes
{
    uint8_t    lane;                            /*!< Serdes lane id */
} adi_adrv903x_CpuCmd_adi_adrv903x_CpuCmd_GetRxLaneSintCodes_t;)

/**
 * \brief JESD_GET_RX_LANE_SINT_CODES command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_CpuCmd_GetRxLaneSintCodesResp
{
    uint8_t    sintCodes[8];            /*!< store the SINT Codes */
} adi_adrv903x_CpuCmd_GetRxLaneSintCodesResp_t;)

/**
 * \brief API stable equivalent of SerdesPhyLib_fgPubCalStatus_t
 */
ADI_ADRV903X_PACKED(
typedef struct adi_adrv903x_SerdesInitCalStatus
{
    int16_t  temperature;
    uint8_t  lpfIndex;
    uint8_t  ctleIndex;
    uint8_t  numEyes;
    uint16_t bsum;
    uint16_t bsum_dc;
    uint8_t  spoLeft;
    uint8_t  spoRight;
    uint16_t eom;
    uint16_t eomMax;
    int8_t   pd;
    int8_t   innerUp;
    int8_t   innerDown;
    int8_t   outerUp;
    int8_t   outerDown;
    int8_t   b[8U];
} adi_adrv903x_SerdesInitCalStatus_t;)

/**
 * \brief API stable equivalent of SerdesPhyLib_bgPubCalStatus_t
 */
ADI_ADRV903X_PACK_START
typedef struct adi_adrv903x_SerdesTrackingCalStatus
{
    int16_t  temperature;
    int8_t   pd[2U];
    int8_t   dllPeakPd[2U];
    int8_t   dllPeakPdDelta[2U];
    int8_t   innerUp;
    int8_t   innerDown;
    int8_t   outerUp;
    int8_t   outerDown;
    int8_t   b[8U];
#ifndef ADI_LIBRARY_RM_FLOATS
    float    ps[2U];
    float    yVector[16U];
#else
    uint32_t ps_float[2U];
    uint32_t yVector_float[16U];
#endif
} adi_adrv903x_SerdesTrackingCalStatus_t;
ADI_ADRV903X_PACK_FINISH

#endif /* __ADI_ADRV903X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__ */


