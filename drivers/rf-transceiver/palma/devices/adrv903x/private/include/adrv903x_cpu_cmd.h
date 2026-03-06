/**
 * \file adrv903x_cpu_cmd.h
 * 
 * \brief   Contains device-specific command definitions
 *
 * \details Contains device-specific command definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_H__
#define __ADRV903X_CPU_CMD_H__

#include "adrv903x_cpu_cmd_intf.h"
#include "adrv903x_cpu_cmd_ping.h"
#include "adrv903x_cpu_cmd_run_init.h"
#include "adrv903x_cpu_cmd_tracking_cals.h"
#include "adrv903x_cpu_cmd_cal_status.h"
#include "adrv903x_cpu_cmd_getset_lofreq.h"
#include "adrv903x_cpu_cmd_getset_nco.h"
#include "adrv903x_cpu_cmd_mcs.h"
#include "adrv903x_cpu_cmd_devtemp.h"
#include "adrv903x_cpu_cmd_ram.h"
#include "adrv903x_cpu_cmd_force_exception.h"
#include "adrv903x_cpu_cmd_getset_config.h"
#include "adrv903x_cpu_cmd_enter_debug_mode.h"
#include "adrv903x_cpu_cmd_bkpt.h"
#include "adrv903x_cpu_cmd_ctrl.h"
#include "adrv903x_cpu_cmd_log.h"
#include "adrv903x_cpu_cmd_debug.h"
#include "adrv903x_cpu_cmd_sys_status.h"
#include "adi_adrv903x_cpu_cmd_dc_offset.h"
#include "adrv903x_cpu_cmd_getset_txatten_phase.h"
#include "adrv903x_cpu_cmd_run_serdes_eye_sweep.h"
#include "adrv903x_cpu_cmd_tx_to_orx_mapping.h"
#include "adrv903x_cpu_cmd_jesd_ser_lane_getset_cfg.h"
#include "adrv903x_cpu_cmd_gpio.h"
#include "adrv903x_cpu_cmd_t.h"
#include "adrv903x_cpu_cmd_efuse.h"
/**
 * \brief CPU command ID enumeration
 */
typedef enum adrv903x_CpuCmdId
{
    ADRV903X_CPU_CMD_ID_PING = 0x0u,                           /*!< PING: Ping the CPU */

    /* Initial calibration commands */
    ADRV903X_CPU_CMD_ID_RUN_INIT = 0x1u,                       /*!< RUN_INIT: Run initial calibrations */
    ADRV903X_CPU_CMD_ID_RUN_INIT_GET_COMPLETION_STATUS = 0x2u, /*!< RUN_INIT_GET_COMPLETION_STATUS: Get the completion status of initial calibrations */
    ADRV903X_CPU_CMD_ID_RUN_INIT_GET_DETAILED_STATUS = 0x3u,   /*!< RUN_INIT_GET_DETAILED_STATUS: Get detailed status information on initial calibrations */
    ADRV903X_CPU_CMD_ID_RUN_INIT_ABORT = 0x4u,                 /*!< RUN_INIT_ABORT: Abort any in progress initial calibrations */

    /* Tracking calibration commands */
    ADRV903X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS = 0x5u,      /*!< SET_ENABLED_TRACKING_CALS: Set the set of enabled tracking cals */
    ADRV903X_CPU_CMD_ID_GET_ENABLED_TRACKING_CALS = 0x6u,      /*!< GET_ENABLED_TRACKING_CALS: Get the set of enabled tracking cals */
    ADRV903X_CPU_CMD_ID_GET_TRACKING_CAL_STATE = 0x7u,         /*!< GET_TRACKING_CAL_STATE: Get detailed state information for all tracking cals */

    /* Calibration status commands */
    ADRV903X_CPU_CMD_ID_GET_CAL_STATUS = 0x8u,                 /*!< GET_CAL_STATUS: Get calibration status information */

    /* System status commands */
    ADRV903X_CPU_CMD_ID_GET_SYS_STATUS = 0x9u,                 /*!< GET_SYS_STATUS: Get system status information */

    /* LO frequency commands */
    ADRV903X_CPU_CMD_ID_GET_LO_FREQUENCY = 0xAu,               /*!< GET_LO_FREQUENCY: Get Lo Frequency value */
    ADRV903X_CPU_CMD_ID_SET_LO_FREQUENCY = 0xBu,               /*!< SET_LO_FREQUENCY: Set Lo Frequency value */

    /* LO loopfilter commands */
    ADRV903X_CPU_CMD_ID_GET_LOOPFILTER = 0xCu,                 /*!< GET_LOOPFILTER: Get LO Loopfilter value */
    ADRV903X_CPU_CMD_ID_SET_LOOPFILTER = 0xDu,                 /*!< SET_LOOPFILTER: Set LO Loopfilter value */   

    /* DC Offset commands */
    ADRV903X_CPU_CMD_ID_GET_DCOFFSET = 0xEu,                   /*!< GET_DCOFFSET: Get DC Offset parameters */
    ADRV903X_CPU_CMD_ID_SET_DCOFFSET = 0xFu,                   /*!< SET_DCOFFSET: Set DC Offset parameters */   

    /* Get Rx/TxLO  commands */
    ADRV903X_CPU_CMD_ID_GET_RXTXLOFREQ = 0x10u,                /*!< GET_RXTXLOFREQ: Get LO value for all of Rx/Tx channels */

    /* NCO commands */
    ADRV903X_CPU_CMD_ID_SET_RX_NCO = 0x11u,                    /*!< SET_RX_NCO: Set/Configure the Rx NCO */
    ADRV903X_CPU_CMD_ID_GET_RX_NCO = 0x12u,                    /*!< GET_RX_NCO: Get configuration from the Rx NCO */
    ADRV903X_CPU_CMD_ID_SET_ORX_NCO = 0x13u,                   /*!< SET_ORX_NCO: Set/Configure the Rx NCO */
    ADRV903X_CPU_CMD_ID_GET_ORX_NCO = 0x14u,                   /*!< GET_ORX_NCO: Get configuration from the ORx NCO */
    ADRV903X_CPU_CMD_ID_SET_TX_TEST_NCO = 0x15u,               /*!< SET_TX_TEST_NCO: Set/Configure one of two Tx Test NCOs */
    ADRV903X_CPU_CMD_ID_GET_TX_TEST_NCO = 0x16u,               /*!< GET_TX_TEST_NCO: Get configuration for one of the two Tx Test NCOs */
    ADRV903X_CPU_CMD_ID_SET_TX_MIX_NCO = 0x17u,                /*!< SET_TX_MIXER_NCO: Set/Configure the Tx Mixer NCO */
    ADRV903X_CPU_CMD_ID_GET_TX_MIX_NCO = 0x18u,                /*!< GET_TX_MIXER_NCO: Get configuration from the Tx Mixer NCO */

    /* MCS commands */
    ADRV903X_CPU_CMD_ID_START_MCS = 0x19u,                     /*!< START_MCS: Start Multichip Sync procedure */
    ADRV903X_CPU_CMD_ID_MCS_COMPLETE = 0x1Au,                  /*!< MCS_COMPLETE: Notification that MCS is complete */

    /* Temperature commands */
    ADRV903X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE = 0x1Bu,        /*!< GET_DEVICE_TEMPERATURE: Get device temperature information */
    ADRV903X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS = 0x1Cu,       /*!< GET_ENABLED_TEMPSENSORS Get enabled temp sensor */
    ADRV903X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS = 0x1Du,       /*!< SET_ENABLED_TEMPSENSORS Set enabled temp sensor */ 

    /* RAM capture commands */
    ADRV903X_CPU_CMD_ID_RAM_ACCESS_START = 0x1Eu,              /*!< RAM_ACCESS_START: Lock a capture RAM for exclusive access */
    ADRV903X_CPU_CMD_ID_RAM_ACCESS_STOP = 0x1Fu,               /*!< RAM_ACCESS_STOP: Unlock exclusive access to a capture RAM */

    /* Config commands */
    ADRV903X_CPU_CMD_ID_UNLOCK_CONFIG = 0x20u,                 /*!< UNLOCK_CONFIG: Unlock the configuration for changing */
    ADRV903X_CPU_CMD_ID_SET_CONFIG = 0x21u,                    /*!< SET_CONFIG: Set system or calibration configuration */
    ADRV903X_CPU_CMD_ID_GET_CONFIG = 0x22u,                    /*!< GET_CONFIG: Get system or calibration configuration */

    /* Ctrl command */
    ADRV903X_CPU_CMD_ID_SET_CTRL = 0x23u,                      /*!< SET_CTRL: Set system or calibration ctrl */

    /* Debug commands */
    ADRV903X_CPU_CMD_ID_ENTER_DEBUG_MODE = 0x24u,              /*!< ENTER_DEBUG_MODE: Enter debug mode */
    ADRV903X_CPU_CMD_ID_DEBUG = 0x25u,                         /*!< DEBUG: Generic debug command */

    /* CPU log commands */
    ADRV903X_CPU_CMD_ID_SET_LOG_FILTERS = 0x26u,               /*!< SET_LOG_FILTERS: Set CPU log filters */

    /* SW Breakpoint commands */
    ADRV903X_CPU_CMD_ID_RESUME_BKPT = 0x27u,                   /*!< RESUME_BKPT: Resume task(s) suspended due breakpoint */
    ADRV903X_CPU_CMD_ID_SET_BKPT_GPIO = 0x28u,                 /*!< SET_BKPT_GPIO: Set breakpoint GPIOs */

    /* Tx Atten Phase commands */
    ADRV903X_CPU_CMD_ID_GET_TX_ATTEN_PHASE = 0x29u,            /*!< GET_TX_ATTEN_PHASE: Get Tx Atten Phase array */
    ADRV903X_CPU_CMD_ID_SET_TX_ATTEN_PHASE = 0x2Au,            /*!< SET_TX_ATTEN_PHASE: Set Tx Atten Phase array */

    /* Tx to Orx Mapping Preset commands */
    ADRV903X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_ATTEN = 0x2Bu,    /*!< SET_TX_TO_ORX_PRESET_ATTEN: Set Tx to Orx Mapping Preset values for Orx Atten */
    ADRV903X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_NCO = 0x2Cu,      /*!< SET_TX_TO_ORX_PRESET_NCO: Set Tx to Orx Mapping Preset values for Orx NCO */

    /* Run SERDES Eye Sweep command */
    ADRV903X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP = 0x2Du,          /*!< RUN_SERDES_EYE_SWEEP: Run SERDES eye sweep */

    /* JESD Serializer Lane configuration commands */
    ADRV903X_CPU_CMD_ID_JESD_SER_LANE_GET_CFG = 0x2Eu,         /*!< JESD_SER_LANE_GET_CFG: Get Serializer Lane configuration */
    ADRV903X_CPU_CMD_ID_JESD_SER_LANE_SET_CFG = 0x2Fu,         /*!< JESD_SER_LANE_SET_CFG: Set Serializer Lane configuration */

     /* Run SERDES Eye Sweep command */
    ADRV903X_CPU_CMD_ID_RUN_SERDES_VERT_EYE_SWEEP = 0x30u,     /*!< RUN_SERDES_VERT_EYE_SWEEP: Run SERDES vertical eye sweep */

    /* GPIO Pin configuration commands */
    ADRV903X_CPU_CMD_ID_SET_GPIO = 0x31u,                      /*!< SET_GPIO: Set GPIO pin configuration */
    ADRV903X_CPU_CMD_ID_GET_GPIO = 0x32u,                      /*!< GET_GPIO: Get GPIO pin configuration */

    /* EFUSE command */
    ADRV903X_CPU_CMD_ID_EFUSE_GET = 0x33u,                     /*!< EFUSE_GET: Read EFUSE address */

    /* Tracking calibration V2 commands */
    ADRV903X_CPU_CMD_ID_SET_ENABLED_TRACKING_CALS_V2 = 0x34u,  /*!< SET_ENABLED_TRACKING_CALS_V2: Set the set of enabled tracking cals */

    /* JESD Serializer reset and cal commands */
    ADRV903X_CPU_CMD_ID_JESD_SER_RESET      = 0x35u,           /*!< JESD_SER_RESET: request pulsed srst reset */

    /* Set Cals in Fast attack mode */
    ADRV903X_CPU_CMD_ID_ENABLE_FAST_ATTACK = 0x36u,            /*!< ENABLE_FAST_ATTACK: Set Cals in Fast attack mode */

    /* RAM-ECC commands */
    ADRV903X_CPU_CMD_ID_UPDATE_ECC = 0x37u,                     /*!< Force an ECC update */

    /* Reprogram PLL */
    ADRV903X_CPU_CMD_ID_REPROGRAM_PLL = 0x38u,                 /*!< REPROGRAM_PLL: Run PLL Program Sequence */

    /* NCO commands pt. 2*/
    ADRV903X_CPU_CMD_ID_SET_ORX_NCO_V2 = 0x39u,                /*!< SET_RX_NCO: Set/Configure the Rx NCO pt. 2*/

    ADRV903X_CPU_CMD_ID_SET_ENABLE_ECC_SCRUB = 0x3Au,                /*!< SET_ENABLE_ECC; set enable for ECC scrubbing */
    ADRV903X_CPU_CMD_ID_GET_ENABLE_ECC_SCRUB = 0x3Bu,                /*!< GET_ENABLE_ECC; get enable state for ECC scrubbing */
    /* Get SERDES Metrics */
    ADRV903X_CPU_CMD_ID_GET_SERDES_FG_METRICS = 0x3Cu,         /*!< GET_SERDES_FG_METRICS: Fetch SERDES-in init cal state for debug purposes */
    ADRV903X_CPU_CMD_ID_GET_SERDES_BG_METRICS = 0x3Du,         /*!< GET_SERDES_BG_METRICS: Fetch SERDES-in tracking cal state for debug purposes */
    ADRV903X_CPU_CMD_ID_SET_CHAN_TO_PLLS = 0x3Eu,              /*!< SET_CHAN_TO_PLLS: Set LO assignemnts during runtime >  */
    
    ADRV903X_CPU_CMD_ID_JESD_TX_LANE_POWER = 0x3Fu,                /*!< Power up or down Serdes lanes */

    ADRV903X_CPU_CMD_ID_JESD_GET_RX_LANE_SINT_CODES = 0x40u,               /*!< Get Serdes lane SINT Codes */

    ADRV903X_CPU_CMD_RX_SPUR_BASE_BAND_FREQ_SET = 0x41u,      /*!< Rx SPUR Frequency Configuration TPGSWE-17029 */
    ADRV903X_CPU_CMD_RX_SPUR_BASE_BAND_FREQ_GET = 0x42u,      /*!< Rx SPUR Frequency Configuration TPGSWE-17029 */

    ADRV903X_CPU_CMD_ID_NUM_CMDS = 0x43u                      /*!< Number of command IDs. Must be last. */

} adrv903x_CpuCmdId_e;

/**
 * \brief CPU command payload union.
 *        Used to determine the maximum command payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrgven6_CpuCmdPayloadMaxSize
{
    adrv903x_CpuCmd_Ping_t                    pingCmd;
    adrv903x_CpuCmd_RunInit_t                 runInitCmd;
    adrv903x_CpuCmd_GetCalStatus_t            getCalStatusCmd;
    adrv903x_CpuCmd_GetSysStatus_t            getSysStatusCmd;
    adrv903x_CpuCmd_SetLoFreq_t               setLoFreqCmd;
    adrv903x_CpuCmd_GetLoFreq_t               getLoFreqCmd;
    adrv903x_CpuCmd_ChanCtrlToPlls_t          setchanCtrlToPllsCmd;
    adrv903x_CpuCmd_SetLoopfilter_t           setLooopfilterCmd;
    adrv903x_CpuCmd_GetLoopfilter_t           getLooopfilterCmd;
    adi_adrv903x_CpuCmd_SetDcOffset_t         setDcOffsetCmd;
    adi_adrv903x_CpuCmd_GetDcOffset_t         getDcOffsetCmd;
    adrv903x_CpuCmd_GetDevTemp_t              getDevTempCmd;
    adi_adrv903x_RxNcoConfig_t                setRxNcoCmd;
    adrv903x_RxNcoConfigReadback_t            getRxNcoCmd;
    adi_adrv903x_ORxNcoConfig_t               setOrxNcoCmd;
    adrv903x_ORxNcoConfigReadback_t           getOrxNcoCmd;
    adi_adrv903x_TxNcoMixConfig_t             setTxMixNcoCmd;
    adrv903x_TxNcoMixConfigReadback_t         getTxMixNcoCmd;
    adi_adrv903x_TxTestNcoConfig_t            setTxTestNcoCmd;
    adrv903x_TxTestNcoConfigReadback_t        getTxTestNcoCmd;
    adrv903x_CpuCmd_RamAccessStart_t          ramAccessStartCmd;
    adrv903x_CpuCmd_RamAccessStop_t           ramAccessStopCmd;
    adrv903x_CpuCmd_EnterDebugMode_t          enterDebugModeCmd;
    adrv903x_CpuCmd_UnlockConfig_t            unlockConfigCmd;
    adrv903x_CpuCmd_SetConfigMaxSize_t        setConfigMaxSize;
    adrv903x_CpuCmd_SetCtrlMaxSize_t          setCtrlMaxSize;
    adrv903x_CpuCmd_DebugMaxSize_t            debugMaxSize;
    adrv903x_CpuCmd_SetEnabledTrackingCals_t  setEnabledTrackingCalsCmd;
    adrv903x_CpuCmd_SetTxAttenPhase_t         getTxAttenPhase;
    adrv903x_CpuCmd_GetTxAttenPhase_t         setTxAttenPhase;
    adrv903x_CpuCmd_RunEyeSweep_t             runEyeSweepCmd;
    adrv903x_CpuCmd_GetSintCodes_t            getSintCodesCmd;
    adrv903x_CpuCmd_SetTxToOrxPresetAtten_t   setTxToOrxPresetAttenCmd;
    adrv903x_CpuCmd_SetTxToOrxPresetNco_t     setTxToOrxPresetNcoCmd;
    adrv903x_CpuCmd_GetJesdSerLaneCfg_t       getJesdSerLaneCfgCmd;
    adrv903x_CpuCmd_SetJesdSerLaneCfg_t       setJesdSerLaneCfgCmd;
    adrv903x_CpuCmd_SetGpio_t                 setGpioCmd;
    adrv903x_CpuCmd_GetGpio_t                 getGpioCmd;
    adrv903x_CpuCmd_EfuseGet_t                efuseGetCmd;
} adrgven6_CpuCmdPayloadMaxSize_t;

/**
 * \brief CPU command response payload union.
 *        Used to determine the maximum command response payload size.
 * \note Not instantiated. Only for size calculations.
 */
typedef union adrgven6_CpuCmdRespPayloadMaxSize
{
    adrv903x_CpuCmd_PingResp_t                       pingCmdResp;
    adrv903x_CpuCmd_RunInitResp_t                    runInitCmdResp;
    adrv903x_CpuCmd_RunInitGetCompletionStatusResp_t runInitGetCompletionStatusCmdResp;
    adrv903x_CpuCmd_RunInitGetDetailedStatusResp_t   runInitGetDetailedStatusCmdResp;
    adrv903x_CpuCmd_SetLoFreqResp_t                  setLoFreqCmdResp;
    adrv903x_CpuCmd_GetLoFreqResp_t                  getLoFreqCmdResp;
    adrv903x_CpuCmd_ChanCtrlToPllsResp_t             setChanCtrlToPllsResp;
    adrv903x_CpuCmd_GetRxTxLoFreqResp_t              getRxTxLoCmdResp;
    adrv903x_CpuCmd_SetLoopfilter_t                  setLoopfilterCmdResp;
    adrv903x_CpuCmd_GetLoopfilter_t                  getLoopfilterCmdResp;
    adrv903x_CpuCmd_GetDevTempResp_t                 getDevTempCmdResp;
    adrv903x_CpuCmd_GetCalStatusMaxSize_t            getCalStatusMaxSize;
    adrv903x_CpuCmd_GetSysStatusMaxSize_t            getSysStatusMaxSize;
    adrv903x_CpuCmd_StartMcsResp_t                   startMcsCmdResp;
    adrv903x_RxNcoConfigResp_t                       setRxNcoCmdResp;
    adi_adrv903x_RxNcoConfigReadbackResp_t           getRxNcoCmdResp;
    adrv903x_ORxNcoConfigResp_t                      setOrxNcoCmdResp;
    adi_adrv903x_ORxNcoConfigReadbackResp_t          getOrxNcoCmdResp;
    adrv903x_TxNcoMixConfigResp_t                    setTxMixNcoCmdResp;
    adi_adrv903x_TxNcoMixConfigReadbackResp_t        getTxMixNcoCmdResp;
    adrv903x_TxTestNcoConfigResp_t                   setTxTestNcoCmdResp;
    adi_adrv903x_TxTestNcoConfigReadbackResp_t       getTxTestNcoCmdResp;
    adrv903x_CpuCmd_RamAccessStartResp_t             ramAccessStartCmdResp;
    adrv903x_CpuCmd_RamAccessStopResp_t              ramAccessStopCmdResp;
    adrv903x_CpuCmd_EnterDebugModeResp_t             enterDebugModeResp;
    adrv903x_CpuCmd_UnlockConfigResp_t               unlockConfigResp;
    adrv903x_CpuCmd_GetConfigMaxSize_t               getConfigMaxSize;
    adrv903x_CpuCmd_SetCtrlRespMaxSize_t             setCtrlRespMaxSize;
    adrv903x_CpuCmd_DebugRespMaxSize_t               debugRespMaxSize;
    adrv903x_CpuCmd_SetEnabledTrackingCalsResp_t     setEnabledTrackingCalsCmdResp;
    adrv903x_CpuCmd_GetTxAttenPhaseResp_t            getTxAttenPhaseResp;
    adrv903x_CpuCmd_SetTxAttenPhaseResp_t            setTxAttenPhaseResp;
    adrv903x_CpuCmd_RunEyeSweepResp_t                runEyeSweepResp;
    adrv903x_CpuCmd_SetTxToOrxPresetAttenResp_t      setTxToOrxPresetAttenResp;
    adrv903x_CpuCmd_SetTxToOrxPresetNcoResp_t        setTxToOrxPresetNcoResp;
    adrv903x_CpuCmd_GetJesdSerLaneCfgResp_t          getJesdSerLaneCfgResp;
    adrv903x_CpuCmd_SetJesdSerLaneCfgResp_t          setJesdSerLaneCfgResp; 
    uint8_t                                          serdesPvtStatuBufSize[SERDES_PVT_BUFFER_SIZE];
    adrv903x_CpuCmd_SetGpioResp_t                    setGpioResp;
    adrv903x_CpuCmd_GetGpioResp_t                    getGpioResp;
    adrv903x_CpuCmd_EfuseGetResp_t                   efuseGetResp;
} adrgven6_CpuCmdRespPayloadMaxSize_t;

/**
 * Size of the largest CPU command, including header, in bytes.
 */
#define ADRV903X_CPU_CMD_MAX_SIZE_BYTES (sizeof(adrv903x_CpuCmd_t) + sizeof(adrgven6_CpuCmdPayloadMaxSize_t))

/**
 * Size of the largest CPU command response, including header, in bytes.
 */
#define ADRV903X_CPU_CMD_RESP_MAX_SIZE_BYTES (sizeof(adrv903x_CpuCmdResp_t) + sizeof(adrgven6_CpuCmdRespPayloadMaxSize_t))

#endif /* __ADRV903X_CPU_CMD_H__ */


