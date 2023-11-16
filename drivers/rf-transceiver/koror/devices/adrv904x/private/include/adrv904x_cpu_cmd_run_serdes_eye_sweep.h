/**
 * \file adrv904x_cpu_cmd_run_serdes_eye_sweep.h
 * 
 * \brief   Command definition for ADRV904X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP
 *
 * \details Command definition for ADRV904X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__
#define __ADRV904X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_cmd_run_serdes_eye_sweep.h"
#include "adrv904x_cpu_error_codes_types.h"

typedef uint8_t adrv904x_CpuCmd_DeserializerLane_t;
typedef uint8_t adrv904x_CpuCmd_PrbsPattern_t;

/**
 * \brief Serdes FSM State
 */
typedef enum adrv904x_Serdes_FsmState
{
    ADRV904X_SERDES_TEST_CMD_RUN     = 14u,  /*!< test cmd running.  */
    ADRV904X_SERDES_TEST_CMD_DONE    = 15u,  /*!< test cmd complete.  */
} adrv904x_Serdes_FsmState_e;

/**
 * \brief Serdes FSM Command Enumerations
 */
typedef enum adrv904x_Serdes_FsmCmd
{
    ADRV904X_SERDES_TEST_CMD                 = 7u, /*!< Serdes test commands */
                                                   /*!< Not used */
} adrv904x_Serdes_FsmCmd_e;

/**
 * \brief Serdes Calibration Command Enumerations
 */
typedef enum adrv904x_Serdes_CalCmd
{
    ADRV904X_SERDES_TEST_HORIZ_EYE_SWEEP     = 12u, /*!< Horizontal Eye Sweep */
    ADRV904X_SERDES_TEST_VERT_EYE_SWEEP      = 13u, /*!< Vertical Eye Sweep */
    ADRV904X_SERDES_TEST_SET_PHY_REG         = 24u, /*!< Set PHY Register */
    ADRV904X_SERDES_TEST_SET_VCM_SWC         = 27u, /*!< Set the VCM SwC config */
} adrv904x_Serdes_CalCmd_e;

/**
 * \brief Serdes Control Commands Enumerations
 */
typedef enum adrv904x_Serdes_CtrlCmd
{
    ADRV904X_SERDES_CTRL_CMD_INVALID   = 0u, /*!< Invalid Control Command */
    ADRV904X_SERDES_CTRL_CMD_OK        = 1u,
    ADRV904X_SERDES_CTRL_RUN           = 2u, /*!< Call phylib run  */
    ADRV904X_SERDES_CTRL_SET_FSM_CMD   = 3u, /*!< call phylib fsm  */
    ADRV904X_SERDES_CTRL_GET_FSM_STATE = 4u, /*!< call phylib fsm  */
} adrv904x_Serdes_CtrlCmd_e;

/**
 * Size of the Serdes Horizontal Eye Sweep Control Command
 */
#define ADRV904X_SERDES_HORIZ_EYE_SWEEP_CMD_SIZE_BYTES             16U
/**
 * Size of the Serdes Vertical Eye Sweep Control Command
 */
#define ADRV904X_SERDES_VERT_EYE_SWEEP_CMD_SIZE_BYTES               6U
/**
 * Size of the Serdes FSM response command header
 */
#define ADRV904X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES             4U

/**
 * \brief RUN_SERDES_EYE_SWEEP command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunEyeSweep
{
    adrv904x_CpuCmd_DeserializerLane_t lane;                 /*!< Deserializer lane number */
    adrv904x_CpuCmd_PrbsPattern_t      prbsPattern;          /*!< PRBS pattern */
    uint8_t                            forceUsingOuter;      /*!< Flag indicating 'outer' should be used for phase detection
                                                                  0 (default) - do not use outer for phase detection,
                                                                  1 -         - use outer for phase detection */
    uint32_t                           prbsCheckDuration_ms; /*!< Duration of PRBS check in [ms] */
} adrv904x_CpuCmd_RunEyeSweep_t;)

/**
 * \brief RUN_SERDES_EYE_SWEEP command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunEyeSweepResp
{
    adrv904x_CpuErrorCode_e     status;    /*!< CPU error status code */
    int8_t                      spoLeft;   /*!< SPO Left value */
    int8_t                      spoRight;  /*!< SPO Right value */
} adrv904x_CpuCmd_RunEyeSweepResp_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunVertEyeSweep
{
    adrv904x_CpuCmd_DeserializerLane_t lane;                 /*!< Deserializer lane number */
 } adrv904x_CpuCmd_RunVertEyeSweep_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunVertiEyeSweepResp
{
    adrv904x_CpuErrorCode_e status;     /*!< CPU error status code */
    int8_t    eyeHeightsAtSpo[512];     /* Store the upper and lower eye height for 16 SPOs. Only elements 0-65 are populated. */
} adrv904x_CpuCmd_RunVertEyeSweepResp_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_RunPerCompEyeSweep
{
    uint8_t        lane;
    uint8_t        slice;        /* the slice number of the comparator of which the eye is measured */
    uint8_t        flash;        /* the flash number of the comparator of which the eye is measured*/
    uint8_t        stopMeasEyeHeightThresh; /* 
                                            * When the measured eye height is 
                                            * below this value, we stop moving 
                                            * spo and measuring the eye height.
                                            */
 } adrv904x_CpuCmd_RunPerCompEyeSweep_t;)

/**
 * \brief RUN_SERDES_VERT_EYE_SWEEP command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_PerCompEyeSweepResp
{
    adrv904x_CpuErrorCode_e status;    /*!< CPU error status code */
    int8_t    perEyeHeightsAtSpo[512];   
} adrv904x_CpuCmd_PerCompEyeSweepResp_t;)

/**
 * \brief GET_SERDES_FG_METRICS, GET_SERDES_BG_METRICS command structure.
 * 
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SerdesCalStatusGet
{
    uint8_t        lane;                            /*!< Serdes lane id */
} adrv904x_CpuCmd_SerdesCalStatusGet_t;)

ADI_ADRV904X_PACKED(
typedef struct adrv904x_SerdesInitCalStatusCmdResp
{
    adi_adrv904x_SerdesInitCalStatus_t details;     /*!< Serdes init cal status details */
    uint32_t cpuErrorCode;                          /*!< Cmd success/fail */
} adrv904x_SerdesInitCalStatusCmdResp_t;)

ADI_ADRV904X_PACKED(
typedef struct adrv904x_SerdesTrackingCalStatusCmdResp
{
    adi_adrv904x_SerdesTrackingCalStatus_t details; /*!< Serdes tracking cal status details */
    uint32_t cpuErrorCode;                          /*!< Cmd success/fail */
} adrv904x_SerdesTrackingCalStatusCmdResp_t;)


#endif /* __ADRV904X_CPU_CMD_RUN_SERDES_EYE_SWEEP_H__ */


