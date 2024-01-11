 /**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

 /**
 * \file adrv904x_cpu_error_codes_types.h
 *
 * \brief   Contains CPU Error code definitions
 *
 * \details Contains CPU Error code definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_ERROR_CODES_TYPES_H__
#define __ADRV904X_CPU_ERROR_CODES_TYPES_H__

#include "adrv904x_cpu_object_ids_types.h"
#include "adi_adrv904x_platform_pack.h"

#define ADRV904X_CPU_SYSTEM_ERROR_CODE_START 0xFF00u                                                       /*!< 0xFF00 - Starting System Related Error codes */

#define ADRV904X_CPU_RC_TUNER_CAL_CODE_ERROR_START         (ADRV904X_CPU_OBJID_IC_RC_TUNER << 8u)          /*!< 0x0000 - Starting RC Tuner Error code */
#define ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START     (ADRV904X_CPU_OBJID_IC_RX_DC_OFFSET << 8u)      /*!< 0x0100 - Starting DC Offset Rx Error code */
#define ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START           (ADRV904X_CPU_OBJID_IC_ADC_RX << 8u)            /*!< 0x0200 - Starting RX ADC Error code */
#define ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START          (ADRV904X_CPU_OBJID_IC_ADC_ORX << 8u)           /*!< 0x0300 - Starting ORX ADC Error code */
#define ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START         (ADRV904X_CPU_OBJID_IC_ADC_TXLB << 8u)          /*!< 0x0400 - Starting TxLB ADC Error code */
#define ADRV904X_CPU_TXDAC_CAL_ERROR_START                 (ADRV904X_CPU_OBJID_IC_TXDAC << 8u)             /*!< 0x0500 - Starting TxDAC Error code */
#define ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START           (ADRV904X_CPU_OBJID_IC_TXBBF << 8u)             /*!< 0x0600 - Starting BBF Tx Error code */
#define ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START         (ADRV904X_CPU_OBJID_IC_TXLB_PATH_DLY << 8u)     /*!< 0x0700 - Starting Pathdelay Error code */
#define ADRV904X_CPU_TX_ATTEN_CAL_CODE_ERROR_START         (ADRV904X_CPU_OBJID_IC_TX_ATTEN_CAL << 8u)      /*!< 0x0800 - Starting TxAtten Error code */
#define ADRV904X_CPU_HRM_CAL_CODE_ERROR_START              (ADRV904X_CPU_OBJID_IC_HRM << 8u)               /*!< 0x0900 - Starting HRM Error code */
#define ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START       (ADRV904X_CPU_OBJID_IC_TXLOL << 8u)             /*!< 0x0B00 - Starting TxLOL Error code */
#define ADRV904X_CPU_SERDES_CODE_ERROR_START               (ADRV904X_CPU_OBJID_IC_SERDES << 8u)            /*!< 0x0C00 - Starting SERDES Error code */
#define ADRV904X_CPU_TXLB_FILTER_CAL_CODE_ERROR_START      (ADRV904X_CPU_OBJID_IC_TXLB_FILTER << 8u)       /*!< 0x1000 - Starting TxLB Filter Error code */
#define ADRV904X_CPU_TXRX_PHASE_CAL_CODE_ERROR_START       (ADRV904X_CPU_OBJID_IC_TXRX_PHASE  << 8u)       /*!< 0x1100 - Starting Tx/Rx Phase Error code */
#define ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START            (ADRV904X_CPU_OBJID_TC_RXQEC << 8u)             /*!< 0x3000 - Starting RX QEC Error code */
#define ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START      (ADRV904X_CPU_OBJID_TC_TX_LOL << 8u)            /*!< 0x3100 - Starting TX LOL Error code */
#define ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START            (ADRV904X_CPU_OBJID_TC_TXQEC << 8u)             /*!< 0x3200 - Starting TX QEC Error code */

#define ADRV904X_CPU_CFG_DEVICE_PROFILE_ERROR_CODE_START   (ADRV904X_CPU_OBJID_CFG_DEVICE_PROFILE << 8u)   /*!< 0x8000 - Starting Device Profile Error code */
#define ADRV904X_CPU_CFG_PARITY_ERROR_CODE_START           (ADRV904X_CPU_OBJID_CFG_PARITY_ERROR_CHECK << 8u)/*!< 0x8500 - Starting Parity Error codes*/
#define ADRV904X_CPU_NCO_DRV_ERROR_CODE_START              (ADRV904X_CPU_OBJID_DRV_NCO << 8u)              /*!< 0xB000 - Starting NCO driver Error code */
#define ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START           (ADRV904X_CPU_OBJID_DRV_STREAM << 8u)           /*!< 0xB100 - Starting Stream driver Error codes */
#define ADRV904X_CPU_FSC_DRV_ERROR_CODE_START              (ADRV904X_CPU_OBJID_DRV_FSC << 8u)              /*!< 0xB200 - Starting FSC driver Error codes */
#define ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START      (ADRV904X_CPU_OBJID_DRV_MASTER_BIAS << 8u)      /*!< 0xB300 - Starting Master Bias driver Error codes */    
#define ADRV904X_CPU_LDO_DRV_ERROR_CODE_START              (ADRV904X_CPU_OBJID_DRV_LDO << 8u)              /*!< 0xB400 - Starting LDO driver Error code */
#define ADRV904X_CPU_DWT_DRV_ERROR_CODE_START              (ADRV904X_CPU_OBJID_DRV_DWT << 8u)              /*!< 0xB500 - Starting DWT driver Error code */
#define ADRV904X_CPU_TEMP_ERROR_CODE_START                 (ADRV904X_CPU_OBJID_DRV_TEMP << 8u)             /*!< 0xB600 - Starting Temperature driver Error code */
#define ADRV904X_CPU_PLL_ERROR_CODE_START                  (ADRV904X_CPU_OBJID_DRV_PLL << 8u)              /*!< 0xB700 - Starting PLL driver Error code */
#define ADRV904X_CPU_JESD_ERROR_CODE_START                 (ADRV904X_CPU_OBJID_DRV_JESD << 8u)             /*!< 0xB800 - Starting JESD driver Error code */
#define ADRV904X_CPU_UART_ERROR_CODE_START                 (ADRV904X_CPU_OBJID_DRV_UART << 8u)             /*!< 0xB900 - Starting UART driver Error code */
#define ADRV904X_CPU_TXATTEN_ERROR_CODE_START              (ADRV904X_CPU_OBJID_DRV_TXATTEN << 8u)          /*!< 0xBA00 - Starting TXatten driver Error code */
#define ADRV904X_CPU_TXLOL_DRV_ERROR_CODE_START            (ADRV904X_CPU_OBJID_DRV_TXLOL << 8u)            /*!< 0xBB00 - Starting TX LOL Accumulator driver Error codes */
#define ADRV904X_CPU_GPIO_DRV_ERROR_CODE_START             (ADRV904X_CPU_OBJID_DRV_GPIO << 8u)             /*!< 0xBD00 - Starting GPIO driver Error codes */

#define ADRV904X_CPU_DDCC_DRV_ERROR_CODE_START             (ADRV904X_CPU_OBJID_DRV_DDCC << 8u)             /*!< 0xF100 - Starting DDCC driver Error codes */

#define ADRV904X_CPU_RADIO_SEQUENCER_DRV_ERROR_CODE_START  (ADRV904X_CPU_OBJID_DRV_RADIO_SEQUENCER << 8u)  /*!< 0xBE00 - Starting Radio Sequencer driver error codes */
#define ADRV904X_CPU_CARRIER_RECONFIG_ERROR_CODE_START  (ADRV904X_CPU_OBJID_CFG_CARRIER_RECONFIG << 8u)  /*!< 0xBf00 - Starting Carrier Reconfigure error codes */

#ifndef CLIENT_IGNORE
typedef enum adrv904x_CpuErrorCode
{

    ADRV904X_CPU_NO_ERROR                            = 0UL,                                                         /*!<@errcode: 0x0000
                                                                                                                        @desc: No Error
                                                                                                                        @maincause: Operation Successful
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                        @mainrecovtext: No Action Required
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */

    /*!< ----- Object ID = ADRV904X_CPU_OBJID_IC_RC_TUNER Section Base Error Code = 0x0000 ------                       @errcode: 0x0000
                                                                                                                        @desc: RC Tuner Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_RC_TUNER_CAL_TIMEOUT_ERROR          = (ADRV904X_CPU_RC_TUNER_CAL_CODE_ERROR_START + 1u),           /*!<@errcode: 0x0001
                                                                                                                        @desc: RC Tuner: Calibration Timeout
                                                                                                                        @maincause: Timeout Waiting for RCAL and/or CCAL to Finish
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Re-program Device and re-run RC Tuner Calibration
                                                                                                                        @cause: Tuner clock not Enabled (RCAL and CCAL) and/or master bias is not up (CCAL)
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_RC_TUNER_CAL_SELFTEST_ERROR         = (ADRV904X_CPU_RC_TUNER_CAL_CODE_ERROR_START + 2u),           /*!<@errcode: 0x0002
                                                                                                                        @desc: RC Tuner: Self-Test Error
                                                                                                                        @maincause: RCAL/CCAL Results violate +/-25% limits (from Nominal)
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Re-program Device and re-run RC Tuner Calibration
                                                                                                                        @cause: Master bias output is incorrect (RCAL and CCAL) and/or tuner clock Frequency is incorrect (CCAL).
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext:
                                                                                                                    */

     /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_RX_DC_OFFSET Section Base Error Code = 0x0100 ------               @errcode: 0x0100
                                                                                                                        @desc: DC Offset Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_DC_OFFSET_CAL_RX_ABORT_ERROR         = (ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START + 1u),      /*!<@errcode: 0x0101
                                                                                                                        @desc: DC Offset: Calibration was aborted while collecting data
                                                                                                                        @maincause: Rx going low or higher priority Calibration aborting the current capture
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run the Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_DC_OFFSET_CAL_RX_TIMEOUT_ERROR       = (ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START + 2u),      /*!<@errcode: 0x0102
                                                                                                                        @desc: DC Offset: Calibration Timed out due to Error in Data Capture
                                                                                                                        @maincause: Hardware was unable to capture enough data within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run the Calibration 
                                                                                                                        @cause: Hardware was unable to capture enough data within time limit
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: Resetting Hardware to make Data Capture finish within time
                                                                                                                    */
    ADRV904X_CPU_DC_OFFSET_CAL_RX_FSC_DATACAP_ERROR   = (ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START + 3u),      /*!<@errcode: 0x0103
                                                                                                                        @desc: DC Offset: Internal Test to verify if Calibration was successful
                                                                                                                        @maincause: FSC was unable to capture data required to confirm Calibration completion
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Perform Soft Reset to complete Data Capture
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_DC_OFFSET_CAL_RX_UNDEF_API_FN_ERROR  = (ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START + 4u),      /*!<@errcode: 0x0104
                                                                                                                        @desc: DC Offset: Undefined control Command
                                                                                                                        @maincause: Invalid control Command used for tracking Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Valid
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_DC_OFFSET_CAL_RX_CONVERGENCE_ERROR   = (ADRV904X_CPU_DC_OFFSET_RX_CAL_CODE_ERROR_START + 5u),      /*!<@errcode: 0x0105
                                                                                                                        @desc: DC Offset: Calibration was unable to converge
                                                                                                                        @maincause: The residual DC Offset was higher than threshold
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_ADC_RX  Section Base Error Code = 0x0200 ------                     @errcode: 0x0200
                                                                                                                        @desc: ADC Rx Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_STAGE1_OBJ_ERROR                = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 1u),          /*!<@errcode: 0x0201
                                                                                                                        @desc: ADC Rx: Stage 1 Error
                                                                                                                        @maincause: Calibration may have saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_STAGEV_OBJ_ERROR               = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  2u),          /*!<@errcode: 0x0202
                                                                                                                        @desc: ADC Rx: Stage V Error
                                                                                                                        @maincause: Calibration may have saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_DRF_OBJ_ERROR                  = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  3u),          /*!<@errcode: 0x0203
                                                                                                                        @desc: ADC Rx: DRF Error
                                                                                                                        @maincause: Calibration may have saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_FSMCMD_ERROR                   = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  4u),          /*!<@errcode: 0x0204
                                                                                                                        @desc: ADC Rx: Invalid FSM Command
                                                                                                                        @maincause: Invalid FSM Command passed to ADC Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_TRACKING_NEEDS_INIT            = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  5u),          /*!<@errcode: 0x0205
                                                                                                                        @desc: ADC Rx: Run ADC Rx Initial Calibration before Tracking Calibration
                                                                                                                        @maincause: Initial ADC Rx Calibration has not completed successfully yet
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_CAL_TIMEOUT_ERROR              = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  6u),          /*!<@errcode: 0x0206
                                                                                                                        @desc: ADC Rx: Calibration Timeout
                                                                                                                        @maincause: Calibration did not finish in expected time
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_HW_STARTUP_ERROR               = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  7u),          /*!<@errcode: 0x0207
                                                                                                                        @desc: ADC Rx:HW start-up Error
                                                                                                                        @maincause: Unexpected HW behavior: start-up Error; report to ADI
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                        */
    ADRV904X_CPU_ADC_RX_CONFIG_INVALID_PARAM           = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  8u),          /*!<@errcode: 0x0208
                                                                                                                        @desc: ADC Rx: Invalid Configuration Parameter
                                                                                                                        @maincause: Invalid Configuration Parameter Passed to ADC Rx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_UNKNOWN_ERROR                  = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START +  9u),          /*!<@errcode: 0x0209
                                                                                                                        @desc: ADC Rx: Unknown Error
                                                                                                                        @maincause: Unspecified Error Detected in ADC Rx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_CTRL_FUNC_NOT_SUPPORTED        = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 11u),          /*!<@errcode: 0x020A
                                                                                                                        @desc: ADC Rx: Invalid Control Command
                                                                                                                        @maincause: Invalid Control Command provided to ADC Rx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_ALREADY_RUNNING_CAL            = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 11u),          /*!<@errcode: 0x020B
                                                                                                                        @desc: ADC Rx: Busy
                                                                                                                        @maincause: A Calibration is currently Running
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Wait for Current Calibration Running to Complete
                                                                                                                        @cause: 
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_CMD_NEEDS_DEBUG_MODE           = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 12u),          /*!<@errcode: 0x020C
                                                                                                                        @desc: ADC Rx: Control Command Requires Debug Mode
                                                                                                                        @maincause: Debug Mode is not Supported
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause: Debug Mode is not Enabled
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @recovtext: Check Control Command & Mode of Operation
                                                                                                                    */
    ADRV904X_CPU_ADC_RX_HW_REG_VERIFY                  = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 13u),          /*!<@errcode: 0x020D
                                                                                                                        @desc: ADC Rx: Register Map Verification Failed
                                                                                                                        @maincause: ADC Rx Module not Configured as Expected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */      
    ADRV904X_CPU_ADC_RX_TRACKING_CAL_TIMEOUT           = (ADRV904X_CPU_ADC_RX_CAL_CODE_ERROR_START + 14u),          /*!<@errcode: 0x020E
                                                                                                                        @desc: ADC Rx: Tracking Calibration Timeout
                                                                                                                        @maincause: Timeout due to Disabled Rx
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Enable Rx before Timeout
                                                                                                                        @cause: Internal Error Not Handled Correctly
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */     
        
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_ADC_ORX  Section Base Error Code = 0x0300 ------                    @errcode: 0x0300
                                                                                                                        @desc: ADC ORx Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SCAL_OBJ_ERROR                = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  1u),         /*!<@errcode: 0x0301
                                                                                                                        @desc: ADC ORx: Slice Calibration Object Error
                                                                                                                        @maincause: Calibration may have Saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SCAL_IRQ_STATUS_ERROR         = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  2u),         /*!<@errcode: 0x0302
                                                                                                                        @desc: ADC ORx: Slice Calibration Interrupt Status Error
                                                                                                                        @maincause: Interrupt Pin Caused Error, but Status was OK
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SCAL_IRQ_TIMEOUT_ERROR        = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  3u),         /*!<@errcode: 0x0303
                                                                                                                        @desc: ADC ORx: Slice Calibration Interrupt Timeout
                                                                                                                        @maincause: Interrupt Timer has Expired
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SCAL_FG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  4u),         /*!<@errcode: 0x0304
                                                                                                                        @desc: ADC ORx: Slice Initialization Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SCAL_BG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  5u),         /*!<@errcode: 0x0305
                                                                                                                        @desc: ADC ORx: Slice Tracking Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_FCAL_OBJ_ERROR                = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  6u),         /*!<@errcode: 0x0306
                                                                                                                        @desc: ADC ORx: Front-End Calibration Object Error
                                                                                                                        @maincause: Calibration may have Saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_FCAL_IRQ_STATUS_ERROR         = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  7u),         /*!<@errcode: 0x0307
                                                                                                                        @desc: ADC ORx: Front-End Calibration Interrupt Status Error
                                                                                                                        @maincause: Interrupt Pin caused Error, but status was OK
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_FCAL_IRQ_TIMEOUT_ERROR        = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  8u),         /*!<@errcode: 0x0308
                                                                                                                        @desc: ADC ORx: Front-End Calibration Interrupt Timeout Error
                                                                                                                        @maincause: Interrupt Timer has Expired
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_ORX_FCAL_FG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START +  9u),         /*!<@errcode: 0x0309
                                                                                                                        @desc: ADC ORx: Front-End Initialization Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_FCAL_BG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 10u),         /*!<@errcode: 0x030A
                                                                                                                        @desc: ADC ORx: Front-End Tracking Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ICAL_OBJ_ERROR                = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 11u),         /*!<@errcode: 0x030B
                                                                                                                        @desc: ADC ORx: Interleaving Timing Calibration Object Error
                                                                                                                        @maincause: Calibration may have Saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ICAL_IRQ_STATUS_ERROR         = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 12u),         /*!<@errcode: 0x030C
                                                                                                                        @desc: ADC ORx: Interleaving Timing Calibration Interrupt Status Error
                                                                                                                        @maincause: Interrupt Pin caused Error, but Status was OK
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ICAL_IRQ_TIMEOUT_ERROR        = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 13u),         /*!<@errcode: 0x030D
                                                                                                                        @desc: ADC ORx: Interleaving Timing Calibration Interrupt Timeout Error
                                                                                                                        @maincause: Interrupt Timer has Expired
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ICAL_FG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 14u),         /*!<@errcode: 0x030E
                                                                                                                        @desc: ADC ORx: Interleaving Timing Initialization Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ICAL_BG_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 15u),         /*!<@errcode: 0x030F
                                                                                                                        @desc: ADC ORx: Interleaving Timing Tracking Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_CMD_ERROR                     = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 16u),         /*!<@errcode: 0x0310
                                                                                                                        @desc: ADC ORx: Invalid Control Command
                                                                                                                        @maincause: Invalid Control Command passed to ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_QUEUE_ERROR                   = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 17u),         /*!<@errcode: 0x0311
                                                                                                                        @desc: ADC ORx: Control Command Queue Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_SLICE_INIT_ERROR              = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 18u),         /*!<@errcode: 0x0312
                                                                                                                        @desc: ADC ORx: Hardware Slice Initialization Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_TRACK_INIT_ERROR              = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 19u),         /*!<@errcode: 0x0313
                                                                                                                        @desc: ADC ORx: Hardware Track & Hold Initialization Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_HW_STARTUP_ERROR              = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 20u),         /*!<@errcode: 0x0314
                                                                                                                        @desc: ADC ORx: Hardware Start-Up Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_CAL_TIMEOUT_ERROR             = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 21u),         /*!<@errcode: 0x0315
                                                                                                                        @desc: ADC ORx: Calibration Timeout Error
                                                                                                                        @maincause: Calibration did not Finish in Expected Time
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_CONFIG_INVALID_PARAM          = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 22u),         /*!<@errcode: 0x0316
                                                                                                                        @desc: ADC ORx: Invalid Configuration Parameter
                                                                                                                        @maincause: Invalid Configuration Parameter Used in Command to ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_ORX_UNKNOWN_ERROR                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 23u),         /*!<@errcode: 0x0317
                                                                                                                        @desc: ADC ORx: Unknown Error
                                                                                                                        @maincause: Unspecified Error in ADC ORx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_CTRL_FUNC_NOT_SUPPORTED       = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 24u),         /*!<@errcode: 0x0318
                                                                                                                        @desc: ADC ORx: Control Command Not Supported
                                                                                                                        @maincause: Invalid Control Command 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_ALREADY_RUNNING_CAL           = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 25u),         /*!<@errcode: 0x0319
                                                                                                                        @desc: ADC ORx: Busy
                                                                                                                        @maincause: A Calibration is Currently Running
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Wait for Current Calibration Running to Complete
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_ORX_CMD_NEEDS_DEBUG_MODE          = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 26u),         /*!<@errcode: 0x031A
                                                                                                                        @desc: ADC ORx: Control Command Requires Debug Mode
                                                                                                                        @maincause: Debug Mode Not Enabled
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Check Command & Mode of Operation
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_TRACKING_NEEDS_INIT           = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 27u),         /*!<@errcode: 0x031B
                                                                                                                        @desc: ADC ORx: Run ADC ORx Initial Calibration before Tracking Calibration
                                                                                                                        @maincause: Initial ADC ORx Calibration has not completed successfully yet
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_INIT_DAC_NOT_RUNNING          = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 28u),         /*!<@errcode: 0x031C
                                                                                                                        @desc: ADC ORx: DAC Stream Write Error
                                                                                                                        @maincause: Stream DAC has stopped Writing
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_HW_REG_VERIFY                 = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 29u),         /*!<@errcode: 0x031D
                                                                                                                        @desc: ADC ORx: Register Map Verification Failed
                                                                                                                        @maincause: ADC ORx Module not Configured as Expected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_ORX_TRACKING_CAL_TIMEOUT          = (ADRV904X_CPU_ADC_ORX_CAL_CODE_ERROR_START + 30u),         /*!<@errcode: 0x031E
                                                                                                                        @desc: ADC ORx: Tracking Calibration Timeout
                                                                                                                        @maincause: Timeout Due to Disabled ORx
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Enable ORx before Timeout
                                                                                                                        @cause: Internal Error Not Handled Correctly
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */     
        
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_ADC_TXLB  Section Base Error Code = 0x0400 ------                   @errcode: 0x0400
                                                                                                                        @desc: ADC TxLb Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_ADC_TXLB_SCAL_OBJ_ERROR              = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  1u),         /*!<@errcode: 0x0401
                                                                                                                        @desc: ADC TxLb: Slice Calibration Object Error
                                                                                                                        @maincause: Calibration may have Saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_TXLB_SCAL_IRQ_STATUS_ERROR       = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  2u),         /*!<@errcode: 0x0402
                                                                                                                        @desc: ADC TxLb: Slice Calibration Interrupt Status Error
                                                                                                                        @maincause: Interrupt Pin caused Error, but Status was OK
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_SCAL_IRQ_TIMEOUT_ERROR      = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  3u),         /*!<@errcode: 0x0403
                                                                                                                        @desc: ADC TxLb: Slice Calibration Interrupt Timeout Error
                                                                                                                        @maincause: Interrupt Timer has Expired
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_SCAL_FG_ERROR               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  4u),         /*!<@errcode: 0x0404
                                                                                                                        @desc: ADC TxLb: Slice Initialization Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_SCAL_BG_ERROR               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  5u),         /*!<@errcode: 0x0405
                                                                                                                        @desc: ADC TxLb: Slice Tracking Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_FCAL_OBJ_ERROR              = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  6u),         /*!<@errcode: 0x0406
                                                                                                                        @desc: ADC TxLb: Front-End Calibration Object Error
                                                                                                                        @maincause: Calibration may have Saturated
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_FCAL_IRQ_STATUS_ERROR       = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  7u),         /*!<@errcode: 0x0407
                                                                                                                        @desc: ADC TxLb: Front-End Calibration Interrupt Status Error
                                                                                                                        @maincause: Interrupt Pin caused Error, but Status was OK
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_FCAL_IRQ_TIMEOUT_ERROR      = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  8u),         /*!<@errcode: 0x0408
                                                                                                                        @desc: ADC TxLb: Front-End Calibration Interrupt Timeout Error
                                                                                                                        @maincause: Interrupt Timer has Expired
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_FCAL_FG_ERROR               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START +  9u),         /*!<@errcode: 0x0409
                                                                                                                        @desc: ADC TxLb: Front-End Initialization Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_FCAL_BG_ERROR               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 10u),         /*!<@errcode: 0x040A
                                                                                                                        @desc: ADC TxLb: Front-End Tracking Calibration Interrupt Error
                                                                                                                        @maincause: Unexpected Interrupt Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_CMD_ERROR                   = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 11u),         /*!<@errcode: 0x040B
                                                                                                                        @desc: ADC TxLb: Invalid Control Command
                                                                                                                        @maincause: Invalid Control Command passed to ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_QUEUE_ERROR                 = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 12u),         /*!<@errcode: 0x040C
                                                                                                                        @desc: ADC TxLb: Control Command Queue Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC TxLb Module
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_SLICE_INIT_ERROR            = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 13u),         /*!<@errcode: 0x040D
                                                                                                                        @desc: ADC TxLb: Hardware Slice Initialization Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_HW_STARTUP_ERROR            = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 14u),         /*!<@errcode: 0x040E
                                                                                                                        @desc: ADC TxLb: Hardware Start-Up Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_TXLB_TRACK_INIT_ERROR            = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 15u),         /*!<@errcode: 0x040F
                                                                                                                        @desc: ADC TxLb: Hardware Track & Hold Initialization Error
                                                                                                                        @maincause: Unexpected Behaviour in ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_CAL_TIMEOUT_ERROR           = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 16u),         /*!<@errcode: 0x0410
                                                                                                                        @desc: ADC TxLb: Calibration Timeout
                                                                                                                        @maincause: Calibration did not Finish in Expected Time
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_CONFIG_INVALID_PARAM        = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 17u),         /*!<@errcode: 0x0411
                                                                                                                        @desc: ADC TxLb: Invalid Parameter
                                                                                                                        @maincause: Invalid Configuration Parameter passed to ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_UNKNOWN_ERROR               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 18u),         /*!<@errcode: 0x0412
                                                                                                                        @desc: ADC TxLb: Unknown Error
                                                                                                                        @maincause: Unspecified Error in ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_CTRL_FUNC_NOT_SUPPORTED     = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 19u),         /*!<@errcode: 0x0413
                                                                                                                        @desc: ADC TxLb: Control Command Not Supported
                                                                                                                        @maincause: Invalid Control Command passed to ADC TxLb Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_ALREADY_RUNNING_CAL         = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 20u),         /*!<@errcode: 0x0414
                                                                                                                        @desc: ADC Txlb: Busy
                                                                                                                        @maincause: A Calibration is Currently Running
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Wait Until Current Calibration Finishes
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_ADC_TXLB_CMD_NEEDS_DEBUG_MODE        = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 21u),         /*!<@errcode: 0x0415
                                                                                                                        @desc: ADC Txlb: Control Command Requires Debug Mode
                                                                                                                        @maincause: Debug Mode is not Supported
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause: Debug Mode is not Enabled
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @recovtext: Check Control Command & Mode of Operation
                                                                                                                        */     
    ADRV904X_CPU_ADC_TXLB_TRACKING_NEEDS_INIT         = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 22u),         /*!<@errcode: 0x0416
                                                                                                                        @desc: ADC Txlb: Run ADC ORx Initial Calibration before Tracking Calibration
                                                                                                                        @maincause: Initial ADC TxLb Calibration has not completed successfully yet
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_INIT_DAC_NOT_RUNNING        = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 23u),         /*!<@errcode: 0x0417
                                                                                                                        @desc: ADC Txlb: DAC Stream Write Error
                                                                                                                        @maincause: Stream DAC has stopped Writing
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_HW_REG_VERIFY               = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 24u),         /*!<@errcode: 0x0418
                                                                                                                        @desc: ADC Txlb: Register Map Verification Failed
                                                                                                                        @maincause: ADC TxLb Module not Configured as Expected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */     
    ADRV904X_CPU_ADC_TXLB_TRACKING_CAL_TIMEOUT        = (ADRV904X_CPU_ADC_TXLB_CAL_CODE_ERROR_START + 25u),         /*!<@errcode: 0x0419
                                                                                                                        @desc: ADC Txlb: Tracking Calibration Timeout
                                                                                                                        @maincause: Timeout due to Disabled Tx
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Enable ORx before Timeout
                                                                                                                        @cause: Internal Error Not Handled Correctly
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXDAC  Section Base Error Code = 0x0500 ------                      @errcode: 0x0500
                                                                                                                        @desc: Tx DAC Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */        
    /*  TODO: TPGSWE-2335,  API functionality to read power status and PLL lock bits in debug mode */       
    ADRV904X_CPU_TXDAC_CAL_CHAN_I_NOT_POWERED_UP_ERROR       = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 1u),           /*!<@errcode: 0x0501
                                                                                                                        @desc: Tx DAC: DAC Channel I not Powered Up
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Verify Supply Ready Flags and PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_Q_NOT_POWERED_UP_ERROR       = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 2u),           /*!<@errcode: 0x0502
                                                                                                                        @desc: Tx DAC: DAC Channel Q not Powered Up
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Verify Supply Ready Flags and PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_I_NOT_FINISHED_ERROR         = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 3u),           /*!<@errcode: 0x0503
                                                                                                                        @desc: Tx DAC: DAC Channel I Initial Calibration didn't finish
                                                                                                                        @maincause: DAC Clock not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Verify PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_Q_NOT_FINISHED_ERROR         = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 4u),           /*!<@errcode: 0x0504
                                                                                                                        @desc: Tx DAC: DAC Channel Q Initial Calibration didn't finish
                                                                                                                        @maincause: DAC Clock not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Verify PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC Clock has Incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify CLKDIV has Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_I_SATURATED_ERROR            = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 5u),           /*!<@errcode: 0x0505
                                                                                                                        @desc: Tx DAC: DAC Channel I Saturated during Initial Calibration
                                                                                                                        @maincause: Not Enough Range during TxDAC Initial Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Possibly not Enough Range, acquire a Memory Dump and contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_Q_SATURATED_ERROR            = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 6u),           /*!<@errcode: 0x0506
                                                                                                                        @desc: Tx DAC: DAC Channel Q Saturated during Initial Calibration
                                                                                                                        @maincause: Not Enough Range during TxDAC Initial Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Possibly not Enough Range, acquire a Memory Dump and contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_I_NOT_POWERED_DOWN_ERROR     = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 7u),           /*!<@errcode: 0x0507
                                                                                                                        @desc: Tx DAC: DAC Channel I not Powered Down
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Verify Supply Ready Flags and PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC clock has Incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_Q_NOT_POWERED_DOWN_ERROR     = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 8u),           /*!<@errcode: 0x0508
                                                                                                                        @desc: Tx DAC: DAC Channel Q not Powered Down
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: verify supply ready flags and PLL ready flags are set, if the problem persists acquire a Memory Dump and contact ADI                                                                                                                  @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_I_NOT_IN_STANDBY             = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 9u) ,          /*!<@errcode: 0x0509
                                                                                                                        @desc: Tx DAC: DAC Channel I not in Standby
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Verify Supply Ready Flags and PLL Ready Flags are Set, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                        @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_Q_NOT_IN_STANDBY             = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 10u),          /*!<@errcode: 0x050A
                                                                                                                        @desc: Tx DAC: DAC Channel Q not in Standby
                                                                                                                        @maincause: Supplies not Ready or Clock is not on
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: verify supply ready flags and PLL ready flags are set, if the problem persists acquire a Memory Dump and contact ADI                                                                                                                  @cause: TxDAC clock has incorrect Configuration
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have Correct Configuration
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_CHAN_INVALID_DIV                  = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 11u),          /*!<@errcode: 0x050B
                                                                                                                        @desc: Tx DAC: Invalid Clock Divider 
                                                                                                                        @maincause: Invalid Clock Divider Value
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Verify dacPowerDownDiv_i/q, dacPowerUpDiv_i/q, and txDacClkDiv have correct Configuration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */        
    ADRV904X_CPU_TXDAC_CAL_DAC_CLOCK_FREQ_OUT_OF_RANGE_ERROR    = (ADRV904X_CPU_TXDAC_CAL_ERROR_START + 12u),       /*!<@errcode: 0x050C
                                                                                                                        @desc: Tx DAC: DAC Sample Clock out of Operating Frequency Range
                                                                                                                        @maincause: Sample clock should be between 1.5 - 3GHz
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Sample Clock Rate
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
        
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXBBF Section Base Error Code = 0x0600------                        @errcode: 0x0600
                                                                                                                        @desc: BBF Tx Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_ABORT_ERROR               = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 1u),            /*!<@errcode: 0x0601
                                                                                                                        @desc: BBF Tx: Calibration Data Capture Aborted
                                                                                                                        @maincause: Tx Going Lower or Higher Priority
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_LPBK_DIS_ERROR            = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 2u),            /*!<@errcode: 0x0602
                                                                                                                        @desc: BBF Tx: Insufficient Power measured at Loopback path
                                                                                                                        @maincause: Insufficient power measured at Loopback path
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_FLATNESS_ERROR            = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 3u),            /*!<@errcode: 0x0603
                                                                                                                        @desc: BBF Tx: Base Band Filter was not Tuned to Achieve Flatness
                                                                                                                        @maincause: Algorithm unable to converge on Tuning Capacitor Values
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun the Initial Calibration again. Contact ADI if the problem persists
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_CAL_RESULTS_INVALID       = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 4u),            /*!<@errcode: 0x0604
                                                                                                                        @desc: BBF Tx: Invalid Data Captured by FSC Hardware
                                                                                                                        @maincause: FSC Hardware unable to Capture Correct Data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun the Initial Calibration again. Contact ADI if the problem persists
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_CTRL_CMD_NOT_SUPPORTED    = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 5u),            /*!<@errcode: 0x0605
                                                                                                                        @desc: BBF Tx: Control Command Not Supported
                                                                                                                        @maincause: Invalid Control Command passed to BBF Tx Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_CAP_ESTIMATE_HIGH         = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 6u),            /*!<@errcode: 0x0606
                                                                                                                        @desc: BBF Tx: Capacitor Estimate Higher than Threshold
                                                                                                                        @maincause: Calibration unable to Converge on Tuning Capacitor Values
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_CAP_ESTIMATE_LOW          = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 7u),            /*!<@errcode: 0x0607
                                                                                                                        @desc: BBF Tx: Capacitor Estimate Lower than Threshold
                                                                                                                        @maincause: Calibration unable to Converge on Tuning Capacitor Values
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           
    ADRV904X_CPU_BBF_CAL_TX_DATACAP_ERROR             = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 8u),            /*!<@errcode: 0x0608
                                                                                                                        @desc: BBF Tx: Hardware unable to Schedule/Complete Data Capture
                                                                                                                        @maincause: FSC Hardware unable to Capture Correct Data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_BBF_CAL_TX_RANGE_ERROR               = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 9u),            /*!<@errcode: 0x0609
                                                                                                                        @desc: BBF Tx: Test Mode Range Error
                                                                                                                        @maincause: Tx Input Range is Out of Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Sample Clock Rate
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */      
    ADRV904X_CPU_BBF_CAL_CPU_SYNC_ERROR                = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 10u),          /*!<@errcode: 0x060A
                                                                                                                        @desc: BBF Tx: CPU Synchronization Error
                                                                                                                        @maincause: Timeout Waiting for the Other CPU to Synchronize
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Whether Both Cores are Running
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */  
    ADRV904X_CPU_BBF_CAL_INVALID_FREQ_ADJUST_ERROR     = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 11u),          /*!<@errcode: 0x060B
                                                                                                                        @desc: BBF Tx: Error adjusting invalid frequency
                                                                                                                        @maincause: Unable to adjust LO or Fbb by adjust value
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Re-run the Initial Calibration. Contact ADI if the problem persists
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */  
    ADRV904X_CPU_BBF_CAL_TX_CAP_NOT_FOUND             = (ADRV904X_CPU_BBF_TX_CAL_CODE_ERROR_START + 12u),           /*!<@errcode: 0x060C
                                                                                                                        @desc: BBF Tx: Unable to find capacitor for proper attenuation
                                                                                                                        @maincause: Calibration unable to Converge on Tuning Capacitor Values
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */           

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXLB_PATH_DLY Section Base Error Code = 0x0700 ------               @errcode: 0x0700
                                                                                                                        @desc: Path Delay Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_PATH_DLY_CAL_USE_CASE_ERROR            = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 1u),        /*!<@errcode: 0x0701
                                                                                                                        @desc: Path Delay: Use case is not Currently Supported
                                                                                                                        @maincause: unsupported use case
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Check Use Case is Currently Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */       
    ADRV904X_CPU_PATH_DLY_CAL_SELFCHECK_ERROR           = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 2u),        /*!<@errcode: 0x0702
                                                                                                                        @desc: Path Delay: Self Check Failure
                                                                                                                        @maincause: Estimated Delay Exceeds Margin
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */       
    ADRV904X_CPU_PATH_DLY_CAL_LIMIT_OBS_ERROR           = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 3u),        /*!<@errcode: 0x0703
                                                                                                                        @desc: Path Delay:Number of observations more than the limit set
                                                                                                                        @maincause: Adjust algorithm Configuration Parameters to handle Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */       
    ADRV904X_CPU_PATH_DLY_CAL_XCORR_WAIT_TIMEOUT        = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 4u),        /*!<@errcode: 0x0704
                                                                                                                        @desc: Path Delay: Cross-Correlation Timeout
                                                                                                                        @maincause: FSC was unable to Capture Data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */       
    ADRV904X_CPU_PATH_DLY_CAL_XCORR_WAIT_ERROR          = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 5u),        /*!<@errcode: 0x0705
                                                                                                                        @desc: Path Delay: Cross-Correlation Error
                                                                                                                        @maincause: FSC was unable to capture data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */       
    ADRV904X_CPU_PATH_DLY_CAL_XCORR_GET_RESULTS_ERROR   = (ADRV904X_CPU_PATH_DLY_CAL_CODE_ERROR_START + 6u),        /*!<@errcode: 0x0706
                                                                                                                        @desc: Path Delay: Cross-Correlation Get Results Error
                                                                                                                        @maincause: Unable to Get FSC Capture Results
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TX_ATTEN_CAL  Section Base Error Code = 0x0800 ------               @errcode: 0x0800
                                                                                                                        @desc: TxAtten Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */           
            
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_HRM Section Base Error Code = 0x0900 ------                         @errcode: 0x0900
                                                                                                                        @desc: Tx HRM Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXHRM_CAL_NOT_RUN              = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 1u),                     /*!<@errcode: 0x0901
                                                                                                                        @desc: Tx HRM: Warm Boot Error
                                                                                                                        @maincause: Initial Calibration must run successfully at least once before Warm Boot
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: Unspecified HW Error occurred 
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */  
    ADRV904X_CPU_TXHRM_CAL_DATACAP_ERROR        = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 2u),                     /*!<@errcode: 0x0902
                                                                                                                        @desc: Tx HRM: Correlation Capture Error
                                                                                                                        @maincause: Timeout waiting for Data Capture to Complete
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: FSC is in a bad state
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_TXHRM_CAL_CORR_WAIT_TIMEOUT        = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 3u),                 /*!<@errcode: 0x0903
                                                                                                                        @desc: Tx HRM: Auto-Correlation Timeout
                                                                                                                        @maincause: FSC was unable to Capture Data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: FSC is in a bad state
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */       
    ADRV904X_CPU_TXHRM_CAL_CORR_GET_RESULTS_ERROR   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 4u),                 /*!<@errcode: 0x0904
                                                                                                                        @desc: Tx HRM: Auto-correlation Get Results
                                                                                                                        @maincause: Unable to Get FSC Capture Results
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: FSC is in a bad state
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */       
    ADRV904X_CPU_TXHRM_CAL_INVALID_SEARCH_PARAM_ERROR   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 5u),             /*!<@errcode: 0x0905
                                                                                                                        @desc: Tx HRM: Invalid Delay Parameter
                                                                                                                        @maincause: Delay Parameter being searched does not exist
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration. Contact ADI if the problem persists
                                                                                                                        @cause: Invalid Delay Control Parameter
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */       
    ADRV904X_CPU_TXHRM_CAL_SEARCH_PARAM_OUT_OF_RANGE_ERROR   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 6u),        /*!<@errcode: 0x0906
                                                                                                                        @desc: Tx HRM: Delay Parameter Value is Out of Range
                                                                                                                        @maincause: Delay Parameter being searched is outside Min/Max limits specified
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: Invalid Delay Control Data
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */       
    ADRV904X_CPU_TXHRM_CAL_SEARCH_ERROR   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 7u),                           /*!<@errcode: 0x0907
                                                                                                                        @desc: Tx HRM: Delay Parameter search Error
                                                                                                                        @maincause: Did not find optimum value for delay Parameter. Target Performance not Achieved                                                                                                                   
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: Invalid Delay Control Data
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */
    ADRV904X_CPU_TXHRM_CAL_POWER_MEAS_ERROR   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 8u),                       /*!<@errcode: 0x0908
                                                                                                                        @desc: Tx HRM: Fundamental Power Measurement Error
                                                                                                                        @maincause: Fundamental Power Measured is too Low                                                                                                                 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: Invalid Threshold Parameters
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */
   ADRV904X_CPU_TXHRM_CAL_ZEROING_NO_FDAC_DETECTED   = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 9u),               /*!<@errcode: 0x0909
                                                                                                                        @desc: Tx HRM: Zeroing Cal Error
                                                                                                                        @maincause: No optimum FDACP/FDACN detected                                                                                                                 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx HRM Initial Calibration
                                                                                                                        @cause: Invalid parameters
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */
   ADRV904X_CPU_TXHRM_CAL_BAD_CTRL_DATA_SZ       = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 10u),                  /*!<@errcode: 0x090A
                                                                                                                        @desc: Tx HRM: Incorrect Number of bytes supplied to HRM Control Function
                                                                                                                        @maincause: API function supplied incorrect number of bytes to HRM Control Function                                                                                                                 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check API call that supplied the parameters
                                                                                                                        @cause: Invalid parameters
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */
   ADRV904X_CPU_TXHRM_CAL_CHAN_ERR               = (ADRV904X_CPU_HRM_CAL_CODE_ERROR_START + 11u),                  /*!<@errcode: 0x090B
                                                                                                                        @desc: Tx HRM: Must choose only 1 channel for HRM Control Function
                                                                                                                        @maincause: API function supplied chnnel mask with more than one channel to HRM Control Function                                                                                                                 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check API call that supplied the parameters
                                                                                                                        @cause: Invalid parameters
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Parameters and try again. Contact ADI if the problem persists
                                                                                                                    */
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXLOL Section Base Error Code = 0x0B00 ------                       @errcode: 0x0B00
                                                                                                                        @desc: Tx LOL Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXLOL_INIT_CAL_FSC_DATACAP_ERROR = ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START + 1u,              /*!<@errcode: 0x0B01
                                                                                                                        @desc: TxLol:Hardware unable to schedule/complete Data Capture
                                                                                                                        @maincause: FSC was unable to capture data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Power measured at output is below threshold. Reset Device
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */                                                                                                                    
    ADRV904X_CPU_TXLOL_CORE_INVALID_OBS_MODE_ERROR = ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START + 4u,             /*!<errcode: 0x0B04
                                                                                                                        @desc: Tx Lol: Unsupported Observation Path
                                                                                                                        @maincause: Invalid Observation Mode Selected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx LOL Initial Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */          
    ADRV904X_CPU_TXLOL_CORE_CORRECTION_THRESHOLD_ERROR = ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START + 5u,         /*!<@errcode: 0x0B05
                                                                                                                        @desc: Tx Lol: Correction Predicted exceeds Threshold
                                                                                                                        @maincause: Predicted correction higher than Hardware threshold limits
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx LOL Initial Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:                                                                                                                        
                                                                                                                    */          
    ADRV904X_CPU_TXLOL_CORE_UPDATE_ERROR = ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START + 6u,                       /*!<@errcode: 0x0B06
                                                                                                                        @desc: TxLol: Internal Calibration Error 
                                                                                                                        @maincause: Invalid Observations Reported by Hardware
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Tx LOL Initial Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_TXLOL_EXT_DATACAP_INVALID_MAPPING_ERROR = ADRV904X_CPU_TXLOL_INIT_CAL_CODE_ERROR_START + 7u,       /*!<@errcode: 0x0B07
                                                                                                                        @desc: TxLol: Invalid Tx to ORx Mapping
                                                                                                                        @maincause: Invalid ORx Mapped to Current Tx Channel
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Tx to ORx Mapping Configuration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_SERDES Section Base Error Code = 0x0C00 ------                      @errcode: 0x0C00
                                                                                                                        @desc: SERDES Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_SERDES_CAL_LANE_POWERED_DOWN          = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 1u),               /*!<@errcode: 0x0C01
                                                                                                                        @desc: SERDES: Lane is Powered down or not Configured
                                                                                                                        @maincause: Requesting to calibrate a Lane that is Powered down or not Configured
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check provided Lane Number/Mask against SERDES Device Profile
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_SERDES_CAL_CTRL_CMD_NOT_SUPPORTED     = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 2u),               /*!<@errcode: 0x0C02
                                                                                                                        @desc: SERDES: Control Command not Supported
                                                                                                                        @maincause: Invalid Control Command passed to SERDES Module
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Command is Supported
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_CAL_INVALID_PARAM              = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 3u),               /*!<@errcode: 0x0C03
                                                                                                                        @desc: SERDES: Invalid Parameter Error
                                                                                                                        @maincause: Invalid Parameter in Control Command
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Parameters are supported for the Control Command
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_CAL_TEMPERATURE_INVALID        = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 4u),               /*!<@errcode: 0x0C04
                                                                                                                        @desc: SERDES: Temperature Error
                                                                                                                        @maincause: Unsupported Operating Temperature Detected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device & Wait for Normal Operating Temperature
                                                                                                                        @cause: Invalid Temperature Sensor Read Back
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: Reset the Device & Wait for Normal Operating Temperature
                                                                                                                    */              
    
    ADRV904X_CPU_SERDES_BG_LMS_NOT_CONVERGED  =          (ADRV904X_CPU_SERDES_CODE_ERROR_START + 5u),              /*!<@errcode: 0x0C05
                                                                                                                        @desc: SERDES: BG LMS can not converge
                                                                                                                        @maincause: Check user's Tx data - possible long stream of unscrambled 0 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                        @mainrecovtext: No Action Required if the JESD link is up
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    
    ADRV904X_CPU_SERDES_CAL_SEM_TAKE_FAILED            = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 6u),               /*!<@errcode: 0x0C06
                                                                                                                        @desc: SERDES: Semaphore Take Failed
                                                                                                                        @maincause: SERDES Internal OS Error failing to take the SERDES semaphore
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext:Reset the Device and re-initialize the SERDES, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_INIT_OFFSET_COMP_EXTEREME_VAL  = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 7u),               /*!<@errcode: 0x0C07
                                                                                                                        @desc: SERDES: One of the comparator Offset DACs reached its limits
                                                                                                                        @maincause: Exceeded operating temperature Range or unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device in a supported temperature region, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_INIT_OFFSET_GMSW_EXTEREME_VAL  = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 8u),               /*!<@errcode: 0x0C08
                                                                                                                        @desc: SERDES: One of the GmSw Offset DACs reached its limits
                                                                                                                        @maincause: Exceeded operating temperature Range or unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device in a supported temperature region, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_TRACK_OFFSET_ODAC_EXTEREME_VAL = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 9u),               /*!<@errcode: 0x0C09
                                                                                                                        @desc: SERDES: Offset DAC has reached its minimum or maximum value
                                                                                                                        @maincause: Exceeded operating temperature Range or unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device in a supported temperature region, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_INIT_SLEW_OR_REG_CAL_FAILED    = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 10u),              /*!<@errcode: 0x0C0A
                                                                                                                        @desc: SERDES: The regulator or slew-Rate Calibration didn't complete
                                                                                                                        @maincause: Unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset Device, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_TRACK_LMS_CAL_TIME_OUT         = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 11u),              /*!<@errcode: 0x0C0B
                                                                                                                        @desc: SERDES: Accumulation Timeout during BG Calibration
                                                                                                                        @maincause: No serial traffic present - Check serial Link integrity
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset JESD Link or Device, re-run Calibration, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */               
    ADRV904X_CPU_SERDES_TRACK_LMS_CAL_TIME_DELAY_ERROR = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 12u),              /*!<@errcode: 0x0C0C
                                                                                                                        @desc: SERDES: Applied Clock Delay is not Consistent with the Measured Delay
                                                                                                                        @maincause: Unexpected Hardware Behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset JESD Link or Device, re-run Calibration, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_INIT_CAL_TIMEOUT_ERROR         = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 13u),              /*!<@errcode: 0x0C0D
                                                                                                                        @desc: SERDES: Initialization Calibration didn't Complete in the Prescribed Amount of Time
                                                                                                                        @maincause: No serial traffic present - Check serial Link integrity
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset JESD Link or Device, re-run Calibration, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_TRACK_CAL_TIMEOUT_ERROR        = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 14u),              /*!<@errcode: 0x0C0E
                                                                                                                        @desc: SERDES: Tracking Calibration didn't complete in the prescribed amount of time
                                                                                                                        @maincause: No serial traffic present - Check serial Link integrity
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset JESD Link or Device, re-run Calibration, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_TRACK_NO_INIT_ERROR            = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 15u),              /*!<@errcode: 0x0C0F
                                                                                                                        @desc: SERDES: Attempting to run Tracking Calibrations before Initialization Calibrations
                                                                                                                        @maincause: Serdes tracking Calibration was initiated before SERDES Initial Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Run SERDES Initial Calibration before starting SERDES tracking Calibration
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_UNMAPPED_ERROR                 = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 16u),              /*!<@errcode: 0x0C10
                                                                                                                        @desc: SERDES: Unknown SERDES Error
                                                                                                                        @maincause: Unspecified SERDES Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset Device, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_SERDES_INIT_INVALID_LANE_COUNT        = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 17u),              /*!<@errcode: 0x0C11
                                                                                                                        @desc: SERDES: Single Lane Profile Unsupported for > 16.22Ghz
                                                                                                                        @maincause: Incorrect Profile Configuration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reconfigure Device to use at least 2 Lanes
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    ADRV904X_CPU_SERDES_FG_CTLE_CLOSED_EYE             = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 18u),              /*!<@errcode: 0x0C12
                                                                                                                        @desc: SERDES: Initialization Calibration didn't find a Suitable CTLE Setting with an open eye
                                                                                                                        @maincause: Check serial Link integrity - No serial traffic present
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset JESD Link or Device, re-run Calibration, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */
    ADRV904X_CPU_FG_OFF_CTLE_ODAC_EXTREME              = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 19u),              /*!<@errcode: 0x0C13
                                                                                                                        @desc: SERDES: CTLE Offset DACs reached its limits
                                                                                                                        @maincause: Exceeded operating temperature Range or unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device in a supported temperature region, if the problem persists contact ADI
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */

    ADRV904X_CPU_SERDES_CAL_SEM_CREATE_FAILED          = (ADRV904X_CPU_SERDES_CODE_ERROR_START + 20u),               /*!<@errcode: 0x0C14
                                                                                                                        @desc: SERDES: Semaphore Create Failed
                                                                                                                        @maincause: SERDES Internal OS Error failing to create the SERDES semaphore
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device and re-initialize the SERDES
                                                                                                                        @cause:
                                                                                                                        @recovenum:
                                                                                                                        @recovtext:
                                                                                                                    */              
    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXLB_FILTER  Section Base Error Code = 0x0E00 ------                @errcode: 0x0E00
                                                                                                                        @desc: TxLb Filter Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXLB_FILTER_CAL_RANGE_ERROR          = (ADRV904X_CPU_TXLB_FILTER_CAL_CODE_ERROR_START + 1u),       /*!<@errcode: 0x0E01
                                                                                                                        @desc: TxLb Filter: No Valid Capacitor Code Found
                                                                                                                        @maincause: Capacitor Code Range exceeded due to unexpected power measurement failures
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Reduce observed power measurement BW and re-run Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLB_FILTER_CAL_DATACAP_ERROR        = (ADRV904X_CPU_TXLB_FILTER_CAL_CODE_ERROR_START + 2u),       /*!<@errcode: 0x0E02
                                                                                                                        @desc: TxLb Filter: Unexpected Data Capture Error
                                                                                                                        @maincause: Timed out Waiting for Data Capture to complete
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run TxLb Filter Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLB_FILTER_CAL_NOT_RUN              = (ADRV904X_CPU_TXLB_FILTER_CAL_CODE_ERROR_START + 3u),       /*!<@errcode: 0x0E03
                                                                                                                        @desc: TxLb Filter: Calibration not completed for this Channel
                                                                                                                        @maincause: User did not run TxLb Filter Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Run TxLb Filter Calibration for the desired channels
                                                                                                                    */

    /*!< -------- Object ID = ADRV904X_CPU_OBJID_IC_TXRX_PHASE  Section Base Error Code = 0x1100 ------                 @errcode: 0x1100
                                                                                                                        @desc: Tx/Rx Phase Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXRX_PHASE_XCORR_ERROR_1          = (ADRV904X_CPU_TXRX_PHASE_CAL_CODE_ERROR_START + 1u),           /*!<@errcode: 0x1101
                                                                                                                        @desc: Tx/Rx Phase Calibration: Error performing phase measurement
                                                                                                                        @maincause: Unknown data capture error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                    */
    ADRV904X_CPU_TXRX_PHASE_XCORR_ERROR_2          = (ADRV904X_CPU_TXRX_PHASE_CAL_CODE_ERROR_START + 2u),           /*!<@errcode: 0x1102
                                                                                                                        @desc: Tx/Rx Phase Calibration: Error performing phase measurement
                                                                                                                        @maincause: Unknown data capture error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                    */
    ADRV904X_CPU_TXRX_PHASE_WRONG_LO_COMBINATION   = (ADRV904X_CPU_TXRX_PHASE_CAL_CODE_ERROR_START + 3u),           /*!<@errcode: 0x1103
                                                                                                                        @desc: Tx/Rx Phase Calibration: All the channels of the same CPU must either use the internal UHB LO or the external LO 
                                                                                                                        @maincause: Follow guidelines for selecting the LO source
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Use the same LO source (internal or external) for all the channels 
                                                                                                                    */
    /*!< ----- Object ID = ADRV904X_CPU_OBJID_TC_RXQEC Section Base Error Code = 0x3000 ----                            @errcode: 0x3000
                                                                                                                        @desc: Rx QEC Tracking Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_SEARCH_ERROR               = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  1u),            /*!<@errcode: 0x3001
                                                                                                                        @desc: Rx QEC: Error during Signal Search
                                                                                                                        @maincause: Persistent signal-search HW unexpected Error when completing search
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Issue a CTRL RESET Command and re-run the Calibration
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_DATACAP_ERROR              = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  2u),            /*!<@errcode: 0x3002
                                                                                                                        @desc: Rx QEC: Error during Data Capture
                                                                                                                        @maincause: Persistent data-capture HW unexpected Error when completing capture
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Issue a CTRL RESET Command and re-run the Calibration
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_UPDATE_ERROR               = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  3u),            /*!<@errcode: 0x3003
                                                                                                                        @desc: Rx QEC: Error during Core Update
                                                                                                                        @maincause: Persistent core computation unexpected Error when completing update
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Issue a CTRL RESET Command and re-run the Calibration
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_REFRESH_ERROR              = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  4u),            /*!<@errcode: 0x3004
                                                                                                                        @desc: Rx QEC:Error during QEC filter refresh
                                                                                                                        @maincause: Persistent filter computation unexpected Error when completing QEC refresh
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Issue a CTRL RESET Command and re-run the Calibration
                                                                                                                    */
                                                                                                                    
    ADRV904X_CPU_RXQEC_CAL_SEARCH_BW_ERROR            = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  5u),            /*!<@errcode: 0x3005
                                                                                                                        @desc: Rx QEC: Unsupported Use Case Error
                                                                                                                        @maincause: Number of Frequency bins per search band exceeds the maximum (32) supported by HW
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check the bandwidth Parameters in the use case or change the use case
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_GAIN_TABLE_ERROR           = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  6u),            /*!<@errcode: 0x3006
                                                                                                                        @desc: Rx QEC: Unsupported Use Case Error
                                                                                                                        @maincause: Unable to support the Rx Gain table that was loaded on boot
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Change the Rx Gain table or update the firmware
                                                                                                                    */
    ADRV904X_CPU_RXQEC_CAL_DCN_SDELAY_ERROR           = (ADRV904X_CPU_RXQEC_CAL_CODE_ERROR_START +  7u),            /*!<@errcode: 0x3007
                                                                                                                        @desc: Rx QEC: DC Notch Filter Start Delay too large
                                                                                                                        @maincause: Unsupported Configuration Parameters for DC notch filter
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check DC notch Configuration or update the firmware
                                                                                                                    */
    /*!< ----- Object ID = ADRV904X_CPU_OBJID_TC_TX_LOL Section Base Error Code = 0x3100 ----                           @errcode: 0x3100
                                                                                                                        @desc: Tx Lol Tracking Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_NO_INIT              = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  1u),      /*!<@errcode: 0x3101
                                                                                                                        @desc: Tx Lol: Cannot run Tracking Calibration without previously running Initialization Calibration
                                                                                                                        @maincause: Initialization Calibration was not run during initialization
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Run Initialization Calibration before attempting to run tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_RESOURCE_ERROR       = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  2u),      /*!<@errcode: 0x3102
                                                                                                                        @desc: Tx Lol: Error sharing resources with Tx QEC Calibration
                                                                                                                        @maincause: Unexpected Resource Sharing Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Initialization Calibration before attempting to run tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_FSC_DATACAP_ERROR    = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  3u),      /*!<@errcode: 0x3103
                                                                                                                        @desc: Tx Lol: Error capturing FSC data
                                                                                                                        @maincause: Unexpected Data Capture Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Initialization Calibration before attempting to run tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_DATACAP_ABORT_ERROR = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  4u),       /*!<@errcode: 0x3104
                                                                                                                        @desc: Tx Lol: Error capturing FSC data
                                                                                                                        @maincause: Data capture aborted due AGC over Range Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Initialization Calibration before attempting to run tracking Calibration
                                                                                                                    */    
    ADRV904X_CPU_TXLOL_TRACK_CAL_EXT_DATACAP_ERROR    = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  5u),      /*!<@errcode: 0x3105
                                                                                                                        @desc: Tx Lol: Error capturing data from External Tx LOL accumulator
                                                                                                                        @maincause: Unexpected Data Capture Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Initialization Calibration before attempting to run tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_EXT_PATHDELAY_ERROR  = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  6u),      /*!<@errcode: 0x3106
                                                                                                                        @desc: Tx Lol: Invalid Tx-to-ORx Path Delay passed to External Tx LOL accumulator
                                                                                                                        @maincause: Unsupported use case, Invalid Configurator output, or Invalid user Configuration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure combined Configurator and user-adjusted Path Delay is between 0 and 250
                                                                                                                    */
    ADRV904X_CPU_TXLOL_TRACK_CAL_EXT_NCO_ERROR        = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  7u),      /*!<@errcode: 0x3107
                                                                                                                        @desc: Tx Lol: ORx LOL NCO not configured properly to mix Tx LO to DC at accumulator Input
                                                                                                                        @maincause: LOL NCO not reconfigured properly when Tx LO or ORx NCO Frequency changed at run-time
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Disable LOL tracking Calibration, re-issue Tx LO and/or ORx NCO Frequency change Command, then re-enable LOL tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXLOL_CAL_INVALID_LO_FREQUENCY_ERROR = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  8u),      /*!<@errcode: 0x3108
                                                                                                                        @desc: Tx Lol: Lo Frequency violates Frequency planning conditions
                                                                                                                        @maincause: Lo Frequency coincides with Loopback/ORx Sampling frequencies
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Change Lo Frequency to avoid Overlap on Loopback/ORx bandwidth edge 
                                                                                                                    */
    ADRV904X_CPU_TXLOL_CORE_CONVERGENCE_ERROR    = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START +  9u),           /*!<@errcode: 0x3109
                                                                                                                        @desc: Tx Lol: Model does not agree with incoming observation data, indicating the Calibration has diverged
                                                                                                                        @maincause: Unexpected Calibration divergence can occur when observation data is corrupted (e.g. External Loopback disconnected)
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Initialization Calibration before attempting to run tracking Calibration 
                                                                                                                    */
    ADRV904X_CPU_TXLOL_CHANNEL_GAIN_ERROR       = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START + 10u),           /*!<@errcode: 0x310A
                                                                                                                        @desc: Tx Lol: Tx-to-ORx channel gain estimate falls outside expected range
                                                                                                                        @maincause: Typically a Tx-to-ORx mapping problem (not observing the expected Tx channel)
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Fix external observation path, issue LOL channel reset command, and re-enable LOL tracking cal
                                                                                                                    */
    ADRV904X_CPU_TXLOL_LEAKAGE_ERROR       = (ADRV904X_CPU_TXLOL_TRACK_CAL_CODE_ERROR_START + 11u),                /*!<@errcode: 0x310B
                                                                                                                        @desc: Tx Lol: estimate leakage is too large or NaN, or estimate leakage variance is too small or NaN 
                                                                                                                        @maincause: Typically an observation problem
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Fix external observation path, issue LOL channel reset command, and re-enable LOL tracking cal
                                                                                                                    */
        
    
    /*!< ----- Object ID = ADRV904X_CPU_OBJID_TC_TXQEC Section Base Error Code = 0x3200 ----                            @errcode: 0x3200
                                                                                                                        @desc: TxQEC Tracking Calibration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_DATACAP_ERROR              = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  1u),            /*!<@errcode: 0x3201
                                                                                                                        @desc: Tx QEC: Correlation Capture Error
                                                                                                                        @maincause: Timed out waiting for Data Capture to complete
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run Calibration
                                                                                                                        @cause: FSC is stuck
                                                                                                                        @recovenum:
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_NO_INIT_ERROR              = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  2u),            /*!<@errcode: 0x3202
                                                                                                                        @desc: Tx QEC: Initial Calibration not done
                                                                                                                        @maincause: At least one of the required Initial Calibrations was not run
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure that all required Initial Calibrations are run without Error
                                                                                                                        @cause: At least one of the Initial Calibrations was in Error
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Re-run the Initial Calibration in Error / Perform Warm Boot
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_SEARCH_ERROR               = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  3u),           /*!< @errcode: 0x3203
                                                                                                                        @desc: Tx QEC: Error Initializing Frequency mask for signal search
                                                                                                                        @maincause: Some other Calibration is conflicting with Tx QEC signal search
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Re-run TxQEC Tracking Calibration
                                                                                                                        @cause: Unspecified HW Error occurred during search
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @recovtext: Re-run TxQEC Tracking Calibration
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_SEARCH_BW_ERROR            = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  4u),           /*!< @errcode: 0x3204
                                                                                                                        @desc: Tx QEC: Error Initializing Frequency mask for signal search
                                                                                                                        @maincause: Signal search mask overflow due to unsupported Configurator Parameters
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Current firmware doesn't support this Profile/use case, contact ADI for FW update
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_TOO_MANY_BANDS_ERROR       = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  5u),           /*!< @errcode: 0x3205
                                                                                                                        @desc: Tx QEC: Error configuring Tx QEC to work over specified Tx bands
                                                                                                                        @maincause: Unsupported Configurator Parameters
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Current firmware doesn't support this Profile/use case, contact ADI for FW update
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_ACTUATOR_RANGE_ERROR       = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  6u),           /*!< @errcode: 0x3206
                                                                                                                        @desc: Tx QEC: Error configuring Tx QEC actuator, not enough correction Range
                                                                                                                        @maincause: The current firmware can not correct quadrature Error of this Device as it is different than expected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Contact ADI for a firmware update
                                                                                                                        @cause: Configurator output provides Invalid bandwidths, LO and Sampling Rate information
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @recovtext: Check Configurator output
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_DLY_CHECK__ERROR           = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  7u),           /*!< @errcode: 0x3207
                                                                                                                        @desc: Tx QEC: Self Check Error, new coefficients increases group delay
                                                                                                                        @maincause: Data capture returning unexpected results
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Re-run the algorithm
                                                                                                                        @cause: Configuration Parameter not set correctly
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Check Configuration Parameters
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_INSUFFICIENT_TONE_POWER    = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  8u),           /*!< @errcode: 0x3208
                                                                                                                        @desc: Low Tx tone power, possibly due to wrong tone Frequency or some other errors on Tx path
                                                                                                                        @maincause: TxQEC Initialization, insufficient tone power
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE
                                                                                                                        @mainrecovtext: TXQEC Initialization Re-Run, contact ADI for FW debug, if Error repeats
                                                                                                                    */
    ADRV904X_CPU_TXQEC_CAL_TOO_MANY_INTERFERERS_ERROR = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  9u),           /*!< @errcode: 0x3209
                                                                                                                        @desc: Detected too many potential interfering/aliasing signals in the LB path
                                                                                                                        @maincause: Number of significant interferences exceeds FW Setting. ADI anticipated this Error to not occur in field
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Contact ADI to disable reporting this Error, Calibration performance maybe reduced
                                                                                                                        @cause: This Error is Enabled from within FW
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE
                                                                                                                        @recovtext: ADI needs to change FW to increase Calibration performance, if customer is required to continue to operate under current carrier configurations
                                                                                                                    */                                                                                                              
    ADRV904X_CPU_TXQEC_CAL_INVALID_LO_FREQUENCY_ERROR = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START + 10u),            /*!<@errcode: 0x320A
                                                                                                                        @desc: Detected user programmed LO Frequency too close to one of interfering harmonic in the LB path
                                                                                                                        @maincause: User has changed the LO Frequency Setting at runtime. ADI anticipated this Error to occur less frequently
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Use formula provided by ADI to compute a valid LO Frequency, otherwise Calibration performance maybe severely reduced
                                                                                                                    */
   ADRV904X_CPU_TXQEC_CAL_LARGE_DC_NOTCH_SETTLE_TIME = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  11u),            /*!<@errcode: 0x320B
                                                                                                                        @desc: Tx QEC:DC notch settling time too large 
                                                                                                                        @maincause: Unsupported Configurator Parameters
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Current firmware doesn't support this Configuration, contact ADI for FW update
                                                                                                                    */
   ADRV904X_CPU_TXQEC_CAL_INNOV_CHECK_ERROR          = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  12u),            /*!<@errcode: 0x320C
                                                                                                                        @desc: Tx QEC:Calibration detected that the QEC model does not agree with the incoming observation data
                                                                                                                        @maincause: Calibration detected divergence due to a bad observation
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Perform QEC reset, or re-run Initialization Calibration, before re-enabling QEC tracking Calibration
                                                                                                                    */

   ADRV904X_CPU_TXQEC_CAL_NAN_ERROR                  = (ADRV904X_CPU_TXQEC_CAL_CODE_ERROR_START +  13u),            /*!<@errcode: 0x320D
                                                                                                                        @desc: Tx QEC:Calibration detected NaN in critical state data
                                                                                                                        @maincause: Unexpected NaN, possibly due to divide-by-zero
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Perform QEC reset, or re-run Initialization Calibration, before re-enabling QEC tracking Calibration
                                                                                                                    */
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_CFG_DEVICE_PROFILE Section Base Error Code = 0x8000 ----                   @errcode: 0x8000
                                                                                                                        @desc: Device Profile Configuration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_CFG_DEVICE_PROFILE_CRC_ERROR          = (ADRV904X_CPU_CFG_DEVICE_PROFILE_ERROR_CODE_START +    1u),/*!<@errcode: 0x8001
                                                                                                                        @desc: Device Profile: Failed CRC verification
                                                                                                                        @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device Profile Image
                                                                                                                        */
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_CFG_PARITY_ERROR_CHECK Section Base Error Code = 0x8500 ----               @errcode: 0x8500
                                                                                                                        @desc: Device Profile Configuration
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_CFG_PARITY_DOUBLE_BIT_ERROR           = (ADRV904X_CPU_CFG_PARITY_ERROR_CODE_START +    1u),        /*!<@errcode: 0x8501
                                                                                                                        @desc: Parity: Fatal 2bit Error
                                                                                                                        @maincause: Memory data is corrupted. Uncorrectable error.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: 
                                                                                                                    */
    
    /*!< ----- Object ID = ADRV904X_CPU_OBJID_DRV_NCO Section Base Error Code = 0xB000 ----                             @errcode: 0xB000
                                                                                                                        @desc: NCO
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_NCO_DRV_INVALID_FREQ                 = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 1u),               /*!<@errcode: 0xB001
                                                                                                                        @desc: NCO: Invalid Frequency Parameters
                                                                                                                        @maincause: Frequency not supported for Sampling Rate
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and NCO Frequency settings
                                                                                                                    */  
    ADRV904X_CPU_NCO_DRV_INVALID_PHASE                = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 2u),               /*!<@errcode: 0xB002
                                                                                                                        @desc: NCO: Invalid Phase Parameters
                                                                                                                        @maincause: Invalid Phase Offset Setting
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: NCO Phase Setting must be between 0 - 359
                                                                                                                    */  
    ADRV904X_CPU_NCO_DRV_INVALID_GAIN                 = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 3u),               /*!<@errcode: 0xB003
                                                                                                                        @desc: NCO: Invalid Gain Parameters
                                                                                                                        @maincause: Invalid Gain Setting
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: NCO I/Q Gain Setting is Invalid
                                                                                                                    */  
    ADRV904X_CPU_NCO_DRV_INVALID_SELECT               = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 4u),               /*!<@errcode: 0xB004
                                                                                                                        @desc: NCO: Band or type selection
                                                                                                                        @maincause: Invalid band or NCO selected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Valid Value must be used for DDC or Datapath NCO
                                                                                                                    */  
    ADRV904X_CPU_NCO_DRV_INVALID_DUAL_TONE            = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 5u),               /*!<@errcode: 0xB005
                                                                                                                        @desc: NCO: Dual tone frequencies are out-of-Range
                                                                                                                        @maincause: Frequencies not supported  for Sampling Rate
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and NCO Frequency settings
                                                                                                                    */  
    ADRV904X_CPU_NCO_DRV_INVALID_MIXER_NCOSET         = (ADRV904X_CPU_NCO_DRV_ERROR_CODE_START + 6u),               /*!<@errcode: 0xB006
                                                                                                                        @desc: NCO: Tx Mixer NCO Frequency can not be changed while TxTestTone Enabled
                                                                                                                        @maincause: Disable Test tone with TxTestToneSet() before calling TxNcoShifterSet()
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                        @mainrecovtext: Check API Call Sequence 
                                                                                                                    */  
        
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_STREAM Section Base Error Code = 0xB100 ---                            @errcode: 0xB100
                                                                                                                        @desc: Stream
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_STRM_DRV_INVALID_PARAM              = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 1u),             /*!<@errcode: 0xB101
                                                                                                                        @desc: Stream: Invalid Input Parameters
                                                                                                                        @maincause: One or more Input Parameters are not valid
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */
    ADRV904X_CPU_STRM_DRV_TIMEOUT                    = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 2u),             /*!<@errcode: 0xB102
                                                                                                                        @desc: Stream:Timer expired waiting for stream completion
                                                                                                                        @maincause: The stream Timeout can occur due to stream processor waiting for other streams to complete
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: The stream execution is timed out. Rerun the feature to recover from this Error.
                                                                                                                    */
    
    ADRV904X_CPU_STRM_DRV_GENERAL_ERROR              = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 3u),             /*!<@errcode: 0xB103
                                                                                                                        @desc: Stream: Triggered stream reported an Error
                                                                                                                        @maincause: The stream that is triggered either through firmware or API has generated an Error.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: The stream number being triggered should be checked.
                                                                                                                    */
                                                                                                            
    ADRV904X_CPU_STRM_DRV_CRC_ERROR                  = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 4u),             /*!<@errcode: 0xB104
                                                                                                                        @desc: Stream: Failed CRC verification
                                                                                                                        @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the stream image.
                                                                                                                        @cause: The failure could be due to bit errors in SPI interface.
                                                                                                                        @recovenum:
                                                                                                                    */
    ADRV904X_CPU_STRM_DRV_STM_REP_FIFO_ERROR         = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 5u),             /*!<@errcode: 0xB105
                                                                                                                        @desc: Stream: Stream Processor reported a FIFO Error
                                                                                                                        @maincause: The stream FIFO is full. This could happen due to back to back streams being triggered.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerunning the feature could solve the issue.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_EXT_TIMER_ERROR    = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 6u),             /*!<@errcode: 0xB106
                                                                                                                        @desc: Stream: Stream Processor reported an External Timer Error
                                                                                                                        @maincause: External Timer started by stream Failed. This could fail if no External timers available.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerunning the feature could solve the issue.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_INV_INSTR          = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 7u),             /*!<@errcode: 0xB107
                                                                                                                        @desc: Stream: Stream Processor reported an Invalid instruction
                                                                                                                        @maincause: This could happen if the stream is corrupted or stream is generated wrongly
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream. 
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_INV_AHB_ADDR       = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 8u),             /*!<@errcode: 0xB108
                                                                                                                        @desc: Stream: Stream Processor reported  an Invalid AHB address
                                                                                                                        @maincause: Wrong AHB address is accessed in the stream.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream. 
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_INV_STREAM_NUM     = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 9u),             /*!<@errcode: 0xB109
                                                                                                                        @desc: Stream: Stream Processor reported an Invalid stream number
                                                                                                                        @maincause: Invalid stream number called from stream or firmware
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream. 
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_STACK_OVERFLOW     = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 10u),            /*!<@errcode: 0xB10A
                                                                                                                        @desc: Stream: Stream Processor reported a stack overflow
                                                                                                                        @maincause: Too many nested calls in the stream can cause this
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_TIMEOUT            = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 11u),            /*!<@errcode: 0xB10B
                                                                                                                        @desc: Stream: Stream Processor reported a Timeout Error
                                                                                                                        @maincause: The stream did not complete within the set Timeout period
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream. 
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_CHECK_INSTR_ERROR  = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 12u),            /*!<@errcode: 0xB10C
                                                                                                                        @desc: Stream: Stream Processor reported a check instruction Error
                                                                                                                        @maincause: This could happen if a wrong instruction is used in the stream.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_STM_REP_INV_SPI_ADDR       = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 13u),            /*!<@errcode: 0xB10D
                                                                                                                        @desc: Stream: Stream Processor reported an Invalid SPI address
                                                                                                                        @maincause: Invalid SPI address accessed in the stream.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_PREV_STM_TIMEOUT           = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 14u),            /*!<@errcode: 0xB10E
                                                                                                                        @desc: Stream: Previous Stream didn't finish
                                                                                                                        @maincause: The previous stream started by the stream processor or firmware did not finish
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun the feature if the problem persists, contact ADI to fix the stream Timeout.
                                                                                                                    */      
    ADRV904X_CPU_STRM_DRV_FUNC_NOT_SUPPORTED         = (ADRV904X_CPU_STREAM_DRV_ERROR_CODE_START + 15u),            /*!<@errcode: 0xB10F
                                                                                                                        @desc: Stream: Function not supported
                                                                                                                        @maincause: The functionality is not yet supported.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Recreate the stream and reprogram the Device. If the problem persists contact ADI to fix the stream.
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_FSC Section Base Error Code = 0xB200 ---                               @errcode: 0xB200
                                                                                                                        @desc: Fsc
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_FSC_USE_CASE_DEC_ERROR              = (ADRV904X_CPU_FSC_DRV_ERROR_CODE_START + 1u),                /*!<@errcode: 0xB201
                                                                                                                        @desc: FSC: Cannot find FSC decimation rates to suit this use case
                                                                                                                        @maincause: Current Calibration Configuration does not support this use case
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check for valid/supported use case. Firmware update may be required.
                                                                                                                    */
    ADRV904X_CPU_FSC_USE_CASE_CLK_ERROR              = (ADRV904X_CPU_FSC_DRV_ERROR_CODE_START + 2u),                /*!<@errcode: 0xB202
                                                                                                                        @desc: FSC: Cannot find valid FSC clock Configuration to suit this use case
                                                                                                                        @maincause: This use case is not currently supported by the FSC and its Device driver
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check for valid/supported use case. Firmware update may be required.
                                                                                                                    */
    ADRV904X_CPU_FSC_NESTED_CAPTURE_ERROR            = (ADRV904X_CPU_FSC_DRV_ERROR_CODE_START + 3u),                /*!<@errcode: 0xB203
                                                                                                                        @desc: FSC: Attempted to start second FSC capture before the first had completed
                                                                                                                        @maincause: Firmware synchronization bug is causing a resource-sharing collision in the FSC
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @marinrecovtext: Attempt to restart the Calibration that reported this Error.
                                                                                                                    */
    ADRV904X_CPU_FSC_PHASE_COMP_CAL_ANALYSIS_ERROR   = (ADRV904X_CPU_FSC_DRV_ERROR_CODE_START + 4u),                /*!<@errcode: 0xB204
                                                                                                                        @desc: FSC: Cannot find valid Phase compensation factor for LB NCOs
                                                                                                                        @maincause: Loopback Phase Calibration has Failed due to unexpected or noisy observation data
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Attempt to re-run Path Delay Calibration
                                                                                                                    */
    ADRV904X_CPU_FSC_PHASE_COMP_CAL_CAPTURE_ERROR    = (ADRV904X_CPU_FSC_DRV_ERROR_CODE_START + 5u),                /*!<@errcode: 0xB205
                                                                                                                        @desc: FSC: Error capturing data for Phase Compensation Calibration
                                                                                                                        @maincause: Unexpected Data Capture failure
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Attempt to re-run Path Delay Calibration
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_MASTER_BIAS Section Base Error Code = 0xB300 ----                      @errcode: 0xB300
                                                                                                                        @desc: Master Bias
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_RB0_PTAT_CAL_FAILED     = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 1u),        /*!<@errcode: 0xB301
                                                                                                                        @desc: Master Bias: RB0 Ptat Calibration Failed
                                                                                                                        @maincause: Unexpected failure in resistor trim logic, or no clock provided to trim logic
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_RB1_PTAT_CAL_FAILED     = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 2u),        /*!<@errcode: 0xB302
                                                                                                                        @desc: Master Bias: RB1 Ptat Calibration Failed
                                                                                                                        @maincause: Unexpected failure in resistor trim logic, or no clock provided to trim logic
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_PTAT_R_TXBBF_INVALID    = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 3u),        /*!<@errcode: 0xB303
                                                                                                                        @desc: Master Bias: Invalid Ptat resistor value for Tx BBF
                                                                                                                        @maincause: Ptat trim Calibration generated an out-of-Range result
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_PTAT_R_UPC_INVALID      = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 4u),        /*!<@errcode: 0xB304
                                                                                                                        @desc: Master Bias: Invalid Ptat resistor value for UPC
                                                                                                                        @maincause: Ptat trim Calibration generated an out-of-Range result
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_PTAT_R_UPC_BIAS_INVALID = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 5u),        /*!<@errcode: 0xB305
                                                                                                                        @desc: Master Bias: Invalid Ptat resistor value for UPC BIAS
                                                                                                                        @maincause: Ptat trim Calibration generated an out-of-Range result
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_MASTER_BIAS_PTAT_R_VGA_INVALID      = (ADRV904X_CPU_MASTER_BIAS_DRV_ERROR_CODE_START + 6u),        /*!<@errcode: 0xB306
                                                                                                                        @desc: Master Bias: Invalid Ptat resistor value for VGA
                                                                                                                        @maincause: Ptat trim Calibration generated an out-of-Range result
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_LDO Section Base Error Code = 0xB400 ---                               @errcode: 0xB400
                                                                                                                        @desc: VCO LDO
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_VCO_LDO_LOWOUTPUT                   = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 1u),                /*!<@errcode: 0xB401
                                                                                                                        @desc: VCO LDO: LDO output voltage is below low side of target Range
                                                                                                                        @maincause: VCO LDO bypass capacitor is shorted to GND
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check Bypass Capacitor & Soldering for Damage and/or Short. Contact ADI if the problem persists.
                                                                                                                        @cause: Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: Reset Device and try again. Contact ADI if the problem persists
                                                                                                                    */
                
    ADRV904X_CPU_VCO_LDO_UVL                         = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 2u),                /*!<@errcode: 0xB402
                                                                                                                        @desc: VCO LDO: Input supply voltage is below low side of target Range
                                                                                                                        @maincause: Input supply voltage is below undervoltage-lockout threshold 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check the Input supply voltage and try again. Contact ADI if the problem persists.
                                                                                                                    */
                
    ADRV904X_CPU_VCO_LDO_NOREF                       = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 3u),                /*!<@errcode: 0xB403
                                                                                                                        @desc: VCO LDO: Reference Input voltage below low side of target Range
                                                                                                                        @maincause: Unexpected Hardware behavior, reference Input voltage below low side of target Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                
    ADRV904X_CPU_VCO_LDO_THERMSDN                    = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 4u),                /*!<@errcode: 0xB404
                                                                                                                        @desc: VCO LDO: Temperature is above high side of target Range
                                                                                                                        @maincause: Temperature is above high side of target Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check Heat Dissipation is at required level. Contact ADI if the problem persists.
                                                                                                                        @cause: Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                    
   ADRV904X_CPU_VCO_LDO_CURLIM                       = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 5u),                /*!<@errcode: 0xB405
                                                                                                                        @desc: VCO LDO: Load current is above high side of target Range dictated by VCO
                                                                                                                        @maincause: VCO LDO bypass capacitor is shorted to GND
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check Bypass capacitor & soldering for damage/short. Contact ADI if the problem persists.
                                                                                                                        @cause: Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                
    ADRV904X_CPU_VCO_LDO_OVERVOLT                    = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 6u),                /*!<@errcode: 0xB406
                                                                                                                        @desc: VCO LDO: LDO output voltage is above high side of target Range
                                                                                                                        @maincause: VCO LDO bypass capacitor is shorted to VDD
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check board layout. Contact ADI if the problem persists.
                                                                                                                        @cause:  Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */    
                    
    ADRV904X_CPU_SHUNT_LDO_SLDO1P0_UV                = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 7u),                /*!<@errcode: 0xB407
                                                                                                                        @desc: VCO LDO: 1.0V Shunt LDO: LDO output voltage is below low threshold
                                                                                                                        @maincause: PD bit = 1 or Rampup Bit = 0 or Master Bias/LCR is not up 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause: Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                
    ADRV904X_CPU_SHUNT_LDO_SLDO1P0_OV                = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 8u),                /*!<@errcode: 0xB408
                                                                                                                        @desc: VCO LDO: 1.0V Shunt LDO: LDO output voltage is above high threshold
                                                                                                                        @maincause: PD bit = 1 or Unexpected Hardware behavior
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                
                        
    ADRV904X_CPU_SHUNT_LDO_SLDO1P0_POWER_NOT_OK      = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 9u),                /*!<@errcode: 0xB409
                                                                                                                        @desc: VCO LDO: 1.0V Shunt LDO: LDO output voltage is out of regulation
                                                                                                                        @maincause: Unexpected Hardware behavior, under and over voltage status bits are set
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
                
                
    ADRV904X_CPU_SHUNT_LDO_SLDO0P8_POWER_NOT_OK      = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 10u),               /*!<@errcode: 0xB40A
                                                                                                                        @desc: VCO LDO: 0.8V Shunt LDO: LDO output voltage below the targeted threshold
                                                                                                                        @maincause: LDO output does not power up at all or it may need longer wait time to power up to the targeted threshold.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */    
                
    ADRV904X_CPU_VCO_LDO_BAD_STATE                   = (ADRV904X_CPU_LDO_DRV_ERROR_CODE_START + 11u),               /*!<@errcode: 0xB40B
                                                                                                                        @desc: VCO LDO: Unspecified VCO LDO Error
                                                                                                                        @maincause: Power Not Good & No Other LDO Status Flag is Indicating an Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        @cause:  Unexpected LDO Hardware behavior
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @recovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_DWT Section Base Error Code = 0xB500 ---                               @errcode: 0xB500
                                                                                                                        @desc: DWT
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_CPU_DWT_DRV_NOT_INITIALIZED             = (ADRV904X_CPU_DWT_DRV_ERROR_CODE_START + 1u),                /*!<@errcode: 0xB501
                                                                                                                        @desc: DWT: Data Watchpoint and Trace (DWT) driver not initialized
                                                                                                                        @maincause: HW doesn't support the DWT the driver is expecting
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device using a Valid Package and try again. Contact ADI if the problem persists.
                                                                                                                    */           
                
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_TEMP Section Base Error Code = 0xB600 ---                              @errcode: 0xB600
                                                                                                                        @desc: Temperature
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */          
    ADRV904X_CPU_DEVTEMP_INVALID_AVGMASK_ERROR       = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 1u),                   /*!<@errcode: 0xB601
                                                                                                                        @desc: Temperature:HAL Invalid Average Mask
                                                                                                                        @maincause: Parameter out of Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Parameter should be > 0 and < 256, see adi_adrv904x_DevTempSensor_e in the API
                                                                                                                    */          
                                                                                                                
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX0         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 2u),                   /*!<@errcode: 0xB602
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX0 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX1         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 3u),                   /*!<@errcode: 0xB603
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX1 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX2         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 4u),                   /*!<@errcode: 0xB604
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX2 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX3         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 5u),                   /*!<@errcode: 0xB605
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX3 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX4         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 6u),                   /*!<@errcode: 0xB606
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX4 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX5         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 7u),                   /*!<@errcode: 0xB607
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX5 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX6         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 8u),                   /*!<@errcode: 0xB608
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX6 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_TX7         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 9u),                   /*!<@errcode: 0xB609
                                                                                                                        @desc: Temperature: HAL Temp sensor for TX7 conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_CLKGEN      = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 10u),                  /*!<@errcode: 0xB60A
                                                                                                                        @desc: Temperature: HAL Temp sensor for CLK Gen PLL conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_RF0         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 11u),                  /*!<@errcode: 0xB60B
                                                                                                                        @desc: Temperature: HAL Temp sensor for RF0 PLL conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_RF1         = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 12u),                  /*!<@errcode: 0xB60C
                                                                                                                        @desc: Temperature: HAL Temp sensor for RF1 PLL conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_TIMEOUT_SERDES      = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 13u),                  /*!<@errcode: 0xB60D
                                                                                                                        @desc: Temperature: HAL Temp sensor for SERDES PLL conversion Timeout
                                                                                                                        @maincause: Hardware was unable to acquire temperature Sample within time limit
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device, if the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */          
    ADRV904X_CPU_DEV_TEMP_SENSOR_INVALID_EN_ERROR    = (ADRV904X_CPU_TEMP_ERROR_CODE_START + 14u),                  /*!<@errcode: 0xB60E
                                                                                                                        @desc: Temperature: HAL Invalid Temp sensor Enable bit
                                                                                                                        @maincause: Parameter out of Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Temp Sensor is not on the accessed core, see adi_adrv904x_DevTempSensor_e in the API
                                                                                                                    */
                 
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_PLL Section Base Error Code = 0xB700 ---                               @errcode: 0xB700
                                                                                                                        @desc: PLL
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                        */
    ADRV904X_CPU_PLL_SYNTH_LOCK_FAILED_ERROR        = (ADRV904X_CPU_PLL_ERROR_CODE_START + 1u),                     /*!<@errcode: 0xB701
                                                                                                                        @desc: PLL: Synthesizer Lock Failed
                                                                                                                        @maincause: Hardware Lock Detection Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check External Clock Sources and Power Supply
                                                                                                                    */              
    ADRV904X_CPU_PLL_RANGE_ERROR                    = (ADRV904X_CPU_PLL_ERROR_CODE_START + 2u),                     /*!<@errcode: 0xB702
                                                                                                                        @desc: PLL: User Input Frequency Out of Range
                                                                                                                        @maincause: Specified Frequency by the user was out of Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure requested Input Frequency is in Range
                                                                                                                    */              
    ADRV904X_CPU_PLL_INVALID_PLL_ERROR              = (ADRV904X_CPU_PLL_ERROR_CODE_START + 3u),                     /*!<@errcode: 0xB703
                                                                                                                        @desc: PLL: User specified Invalid PLL type
                                                                                                                        @maincause: Specified PLL name by the user was incorrect
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure the correct PLL is specified
                                                                                                                    */              
    ADRV904X_CPU_PLL_NOT_ENABLED_ERROR              = (ADRV904X_CPU_PLL_ERROR_CODE_START + 4u),                     /*!<@errcode: 0xB704
                                                                                                                        @desc: PLL: Selected PLL was Not Enabled
                                                                                                                        @maincause: Selected PLL not Enabled
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: This Error code is not used
                                                                                                                    */              
    ADRV904X_CPU_PLL_VCO_CAL_FAILED_ERROR           = (ADRV904X_CPU_PLL_ERROR_CODE_START + 5u),                     /*!<@errcode: 0xB705
                                                                                                                        @desc: PLL: VCO Calibration Failed
                                                                                                                        @maincause: Unexpected Hardware behavior, Hardware VCO Calibration Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_CPU_PLL_INVALID_LOOPFILTER_PARAM_ERROR = (ADRV904X_CPU_PLL_ERROR_CODE_START + 6u),                     /*!<@errcode: 0xB706
                                                                                                                        @desc: PLL: Invalid Loopfilter Parameters, Phase of BW out of Range.
                                                                                                                        @maincause: Specified Loopfilter Parameters out of Range
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check that the Input Parameters are in Range
                                                                                                                    */              
    ADRV904X_CPU_PLL_CP_CAL_FAILED_ERROR            = (ADRV904X_CPU_PLL_ERROR_CODE_START + 7u),                     /*!<@errcode: 0xB707
                                                                                                                        @desc: PLL: CP Calibration Failed
                                                                                                                        @maincause: Unexpected Hardware behavior, Hardware CP Calibration Timeout
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_CPU_PLL_BLEED_CAL_FAILED_ERROR         = (ADRV904X_CPU_PLL_ERROR_CODE_START + 8u),                     /*!<@errcode: 0xB708
                                                                                                                        @desc: PLL: Bleed ramp Calibration Failed
                                                                                                                        @maincause: Unexpected Hardware behavior, Hardware Bleed Calibration Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_CPU_PLL_INVALID_RX_LEAF_ERROR          = (ADRV904X_CPU_PLL_ERROR_CODE_START + 9u),                     /*!<@errcode: 0xB709
                                                                                                                        @desc: PLL: Invalid Rx Leaf Hardware Setting, the selected Frequency could not be realized.
                                                                                                                        @maincause: Specified RX Frequency could not be realized
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check that the Input Parameters are in Range
                                                                                                                    */              
    ADRV904X_CPU_PLL_INVALID_TX_LEAF_ERROR          = (ADRV904X_CPU_PLL_ERROR_CODE_START + 10u),                    /*!<@errcode: 0xB70A
                                                                                                                        @desc: PLL: Invalid Tx Leaf Hardware Setting, the selected Frequency could not be realized.
                                                                                                                        @maincause: Specified TX Frequency could not be realized
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: This Error code is not used
                                                                                                                    */              
    ADRV904X_PHASE_SYNC_INVALID_SETTING_ERROR       = (ADRV904X_CPU_PLL_ERROR_CODE_START + 11u),                    /*!<@errcode: 0xB70B
                                                                                                                        @desc: PLL: Phase Sync Invalid mode Setting
                                                                                                                        @maincause: Specified Phase Sync mode Invalid
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        */              
    ADRV904X_PHASE_SYNC_TIMEOUT_ERROR               = (ADRV904X_CPU_PLL_ERROR_CODE_START + 12u),                    /*!<@errcode: 0xB70C
                                                                                                                        @desc: PLL: Phase Sync Timeout
                                                                                                                        @maincause: Phase Sync Calibration Timeout
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_PHASE_MISMATCH_ERROR                  = (ADRV904X_CPU_PLL_ERROR_CODE_START + 13u),                     /*!<@errcode: 0xB70D
                                                                                                                        @desc: PLL:Phase Sync Phase Mismatch
                                                                                                                        @maincause: Phase Sync Golden Counter Mismatch
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_CPU_EXT_LO_IN_RANGE_ERROR              = (ADRV904X_CPU_PLL_ERROR_CODE_START + 14u),                    /*!<@errcode: 0xB70E
                                                                                                                        @desc: PLL: External LO Input Frequency out of Range
                                                                                                                        @maincause: Specified External LO Frequency out of Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */              
    ADRV904X_CPU_PLL_SWEEPFAIL_ARM_TOO_FAST         = (ADRV904X_CPU_PLL_ERROR_CODE_START + 15u),                    /*!<@errcode: 0xB70F
                                                                                                                        @desc: PLL: CLK Sweep Test Error
                                                                                                                        @maincause: CPU Clock is Too Fast
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */              
    ADRV904X_PLL_MUST_BE_IN_TEST_MODE               = (ADRV904X_CPU_PLL_ERROR_CODE_START + 17u),                    /*!<@errcode: 0xB711
                                                                                                                        @desc: PLL: Invalid Control Command
                                                                                                                        @maincause: PLL Module is not in Debug Mode
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Device is in correct state for Command
                                                                                                                    */              
    ADRV904X_PHASE_GCNT_SDM_MCS_TIMEOUT              = (ADRV904X_CPU_PLL_ERROR_CODE_START + 18u),                   /*!<@errcode: 0xB712
                                                                                                                        @desc: PLL: GCNT SDM MCS Timeout
                                                                                                                        @maincause: GCNT SDM MCS Hardware Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */                    
    ADRV904X_PHASE_GCNT_CLKGEN_MCS_TIMEOUT           = (ADRV904X_CPU_PLL_ERROR_CODE_START + 19u),                   /*!<@errcode: 0xB713
                                                                                                                        @desc: PLL: GCNT ClkGen MCS Timeout
                                                                                                                        @maincause: GCNT Clkgen MCS Hardware Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */                    
    ADRV904X_PLL_FORCED_ALC_TIMEOUT                  = (ADRV904X_CPU_PLL_ERROR_CODE_START + 20u),                   /*!<@errcode: 0xB714
                                                                                                                        @desc: PLL: Forced ALC Enable-Disable Timeout
                                                                                                                        @maincause: Forced ALC Enable-Disabled Hardware Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */    

    ADRV904X_CPU_PLL_SET_MODULUS_ERROR               = (ADRV904X_CPU_PLL_ERROR_CODE_START + 21u),                   /*!<@errcode: 0xB715
                                                                                                                        @desc: PLL: Unable to lookup and set the PLL modulus value
                                                                                                                        @maincause: Device clock must be divisable by 122.88 or 184.32 MHz
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check that device clock setting is supported
                                                                                                                    */
    ADRV904X_CPU_PLL_CLK_RATE_LIMIT_ERROR            = (ADRV904X_CPU_PLL_ERROR_CODE_START + 22u),                   /*!<@errcode: 0xB716
                                                                                                                        @desc: PLL: clock rate exceeded the design limit
                                                                                                                        @maincause: PLL clock rate must be < 250 MHz
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_JESD Section Base Error Code = 0xB800 ---                            !<@errcode: 0xB800
                                                                                                                        @desc: JESD
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_LANE_OVERLAP_ERROR        = (ADRV904X_CPU_JESD_ERROR_CODE_START + 1u),                    /*!<@errcode: 0xB801
                                                                                                                        @desc: JESD: Serializer Lane Overlap
                                                                                                                        @maincause: Incorrect Serializer Lane assignment 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_LANE_OVERLAP_ERROR       = (ADRV904X_CPU_JESD_ERROR_CODE_START + 2u),                    /*!<@errcode: 0xB802
                                                                                                                        @desc: JESD: Deserializer Lane Overlap
                                                                                                                        @maincause: Incorrect Deserializer Lane assignment
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_INVALID_SAMPRATE          = (ADRV904X_CPU_JESD_ERROR_CODE_START + 3u),                    /*!<@errcode: 0xB803
                                                                                                                        @desc: JESD: Framer Invalid Sample Rate
                                                                                                                        @maincause: Incorrect Framer Sample Rate in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_LKSH_INVALID_SAMPRATE         = (ADRV904X_CPU_JESD_ERROR_CODE_START + 4u),                    /*!<@errcode: 0xB804
                                                                                                                        @desc: JESD: Link sharing Invalid Sample Rate
                                                                                                                        @maincause: Incorrect Link sharing Sample Rate in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_INVALID_SAMPRATE         = (ADRV904X_CPU_JESD_ERROR_CODE_START + 5u),                    /*!<@errcode: 0xB805
                                                                                                                        @desc: JESD: Deframer Invalid Sample Rate
                                                                                                                        @maincause: Incorrect Deframer Sample Rate in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_SYNC_PAD_CFG_INVALID      = (ADRV904X_CPU_JESD_ERROR_CODE_START + 6u),                    /*!<@errcode: 0xB806
                                                                                                                        @desc: JESD: Invalid Framer Sync Pad Configuration
                                                                                                                        @maincause: Incorrect Framer Sync pad Configuration provided
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_SYNC_PAD_CFG_INVALID     = (ADRV904X_CPU_JESD_ERROR_CODE_START + 7u),                    /*!<@errcode: 0xB807
                                                                                                                        @desc: JESD:Invalid Deframer Sync Pad Configuration
                                                                                                                        @maincause: Incorrect Deframer Sync pad Configuration provided
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM0_NOT_ENABLED              = (ADRV904X_CPU_JESD_ERROR_CODE_START + 8u),                    /*!<@errcode: 0xB808
                                                                                                                        @desc: JESD: Framer 0 must be Enabled
                                                                                                                        @maincause: Framer 0 is not used in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_INVALID_SPLXBAR_ENTRY     = (ADRV904X_CPU_JESD_ERROR_CODE_START + 9u),                    /*!<@errcode: 0xB809
                                                                                                                        @desc: JESD: Invalid Framer Sample Crossbar entry
                                                                                                                        @maincause: Invalid Framer Sample Crossbar entry in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_INVALID_LINK_LAYER_MODE   = (ADRV904X_CPU_JESD_ERROR_CODE_START + 10u),                   /*!<@errcode: 0xB80A
                                                                                                                        @desc: JESD:Invalid Framer Link layer mode of Operation
                                                                                                                        @maincause: Invalid Framer mode Parameter in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_INVALID_SPLXBAR_ENTRY    = (ADRV904X_CPU_JESD_ERROR_CODE_START + 11u),                   /*!<@errcode: 0xB80B
                                                                                                                        @desc: JESD: Invalid Deframer Sample Crossbar entry
                                                                                                                        @maincause: Invalid Deframer Sample Crossbar entry in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_INVALID_LINK_LAYER_MODE  = (ADRV904X_CPU_JESD_ERROR_CODE_START + 12u),                   /*!<@errcode: 0xB80C
                                                                                                                        @desc: JESD: Invalid Deframer Link layer mode of Operation
                                                                                                                        @maincause: Invalid Deframer mode Parameter in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_S_NOT_POW2                = (ADRV904X_CPU_JESD_ERROR_CODE_START + 13u),                   /*!<@errcode: 0xB80D
                                                                                                                        @desc: JESD: Framer S Parameter is not a power of 2
                                                                                                                        @maincause: Invalid Framer S Parameter in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DFRM_S_NOT_POW2               = (ADRV904X_CPU_JESD_ERROR_CODE_START + 14u),                   /*!<@errcode: 0xB80E
                                                                                                                        @desc: JESD: Deframer S Parameter is not a power of 2
                                                                                                                        @maincause: Invalid Deframer S Parameter in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_INVALID_CONFIG_DET        = (ADRV904X_CPU_JESD_ERROR_CODE_START + 15u),                   /*!<@errcode: 0xB80F
                                                                                                                        @desc: JESD: Invalid Framer Configuration
                                                                                                                        @maincause: Invalid Framer Configuration detected by the JESD Hardware block
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_DRFM_INVALID_CONFIG_DET       = (ADRV904X_CPU_JESD_ERROR_CODE_START + 16u),                   /*!<@errcode: 0xB810
                                                                                                                        @desc: JESD: Invalid Deframer Configuration
                                                                                                                        @maincause: Invalid Deframer Configuration detected by the JESD Hardware block
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_LKSH_INVALID_SPLXBAR_ENTRY    = (ADRV904X_CPU_JESD_ERROR_CODE_START + 17u),                   /*!<@errcode: 0xB811
                                                                                                                        @desc: JESD: Invalid Sample Crossbar
                                                                                                                        @maincause: Invalid Link sharing Sample Crossbar entry in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_UNSUPPORTED_FREQ_DIV_RATIO    = (ADRV904X_CPU_JESD_ERROR_CODE_START + 18u),                   /*!<@errcode: 0xB812
                                                                                                                        @desc: JESD: Unsupported Frequency Divide Ratio
                                                                                                                        @maincause: Invalid Frequency Parameter in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_INVALID_FREQ_INPUT            = (ADRV904X_CPU_JESD_ERROR_CODE_START + 19u),                   /*!<@errcode: 0xB813
                                                                                                                        @desc: JESD: Invalid Frequency
                                                                                                                        @maincause: Invalid Frequency Parameter provided in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_FRM_NUM_OF_CONV_INVALID       = (ADRV904X_CPU_JESD_ERROR_CODE_START + 20u),                   /*!<@errcode: 0xB814
                                                                                                                        @desc: JESD: Invalid number of converters
                                                                                                                        @maincause: Invalid number of converters specified in the Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_SER_INVALID_LANE_SELECTED     = (ADRV904X_CPU_JESD_ERROR_CODE_START + 21u),                   /*!<@errcode: 0xB815
                                                                                                                        @desc: JESD: Invalid Serializer Lane selected
                                                                                                                        @maincause: Invalid Serializer Lane selected 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_SER_INVALID_LANE_PARAMETER    = (ADRV904X_CPU_JESD_ERROR_CODE_START + 22u),                   /*!<@errcode: 0xB816
                                                                                                                        @desc: JESD: Invalid Serializer Lane Parameter
                                                                                                                        @maincause: Invalid Serializer Lane Parameter
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_JESD_DRV_SER_CLK_OFFSET_CAL_FAILURE    = (ADRV904X_CPU_JESD_ERROR_CODE_START + 23u),                   /*!<@errcode: 0xB817
                                                                                                                        @desc: JESD: Serializer Clock offset calibration failed
                                                                                                                        @maincause: JESD: Serializer Clock offset calibration failed
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_UART Section Base Error Code = 0xB900 ---                              @errcode: 0xB900
                                                                                                                        @desc: UART
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_UART_DRV_INVALID_SCLK_FREQ            = (ADRV904X_CPU_UART_ERROR_CODE_START + 1u),                     /*!<@errcode: 0xB901
                                                                                                                        @desc: UART: Invalid SCLK Frequency
                                                                                                                        @maincause: SCLK is too low for configured UART bit Rate, check Device clock Settings
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Increase SCLK Frequency and try again.
                                                                                                                    */
                
                
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_TXATTEN Section Base Error Code = 0xBA00 ---                           @errcode: 0xBA00
                                                                                                                        @desc: Tx Attenuation Driver
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_TXATTEN_DRV_WRONG_SPI_MODE_ERROR      = (ADRV904X_CPU_TXATTEN_ERROR_CODE_START + 1u),                  /*!<@errcode: 0xBA01
                                                                                                                        @desc: TxAttenDrv: Must be in SPI mode to accesses this mode
                                                                                                                        @maincause: Invalid Tx attenuation mode selected
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Select SPI Mode before changing Tx Attenuation
                                                                                                                    */
    ADRV904X_TXATTEN_DRV_INVALID_HP_VALUE_ERROR    = (ADRV904X_CPU_TXATTEN_ERROR_CODE_START + 2u),                  /*!<@errcode: 0xBA02
                                                                                                                        @desc: TxAttenDrv: Invalid HP attenuation value, attenuation max exceeded
                                                                                                                        @maincause: Invalid HP attenuation value
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Select HP value within Range
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_TXLOL Section Base Error Code = 0xBB00 ---                             @errcode: 0xBB00
                                                                                                                        @desc: TxLolDrv
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
    ADRV904X_TXLOL_DRV_NESTED_ACCUM_ERROR          = (ADRV904X_CPU_TXLOL_DRV_ERROR_CODE_START + 1u),                /*!<@errcode: 0xBB01
                                                                                                                        @desc: TxLolDrv: Attempted second accumulation before the first completed
                                                                                                                        @maincause: Tx LOL Calibration state machine bug
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Stop and restart TxLOL tracking Calibration
                                                                                                                    */
    ADRV904X_TXLOL_DRV_HALT_ERROR                  = (ADRV904X_CPU_TXLOL_DRV_ERROR_CODE_START + 2u),                /*!<@errcode: 0xBB02
                                                                                                                        @desc: TxLolDrv: Attempted to read results before accumulator had halted
                                                                                                                        @maincause: Tx LOL Calibration firmware/Hardware handshake problem
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Stop and restart TxLOL tracking Calibration
                                                                                                                    */
    ADRV904X_TXLOL_DRV_CLK_CONFIG_NOT_SUPPORTED    = (ADRV904X_CPU_TXLOL_DRV_ERROR_CODE_START + 4u),                /*!<@errcode: 0xBB04
                                                                                                                        @desc: TxLolDrv: Cannot configure TX LOL clock faster than ORX LOL clock
                                                                                                                        @maincause: External Tx LOL observations not supported with the current Profile
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Device Profile and restart the Device
                                                                                                                    */
    ADRV904X_TXLOL_DRV_ORX_CHAN_ERROR              = (ADRV904X_CPU_TXLOL_DRV_ERROR_CODE_START + 5u),                /*!<@errcode: 0xBB05
                                                                                                                        @desc: TxLolDrv:Observation Channel is unsupported
                                                                                                                        @maincause: External Tx LOL observations do not support this ORx Channel
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check TX to ORx mapping
                                                                                                                    */
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_GPIO Section Base Error Code = 0xBD00 ---                              @errcode: 0xBD00
                                                                                                                        @desc: Gpio Driver
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */
                
    ADRV904X_GPIO_DRV_INVALID_SIGNAL_ID            = (ADRV904X_CPU_GPIO_DRV_ERROR_CODE_START + 1u),                 /*!<@errcode: 0xBD01
                                                                                                                        @desc: GpioDrv: Invalid signal ID provided
                                                                                                                        @maincause: Invalid signal ID provided
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */
    ADRV904X_GPIO_DRV_INVALID_PIN_ID               = (ADRV904X_CPU_GPIO_DRV_ERROR_CODE_START + 2u),                 /*!<@errcode: 0xBD02
                                                                                                                        @desc: GpioDrv: Invalid GPIO pin number provided
                                                                                                                        @maincause: Invalid GPIO pin number provided
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */

    /*!< ---- Object ID = ADRV904X_CPU_OBJID_DRV_RADIO_SEQUENCER Section Base Error Code = 0xBE00 ---                   @errcode: 0xBE00
                                                                                                                        @desc: RadioSequencerDrv
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */

    ADRV904X_CPU_RADIO_SEQUENCER_DRV_CRC_ERROR     = (ADRV904X_CPU_RADIO_SEQUENCER_DRV_ERROR_CODE_START + 1u),      /*!< @errcode: 0xBE01
                                                                                                                         @desc: RadioSequencerDrv: CRC verification failed
                                                                                                                         @maincause: Programmed image is corrupted. Original image may be invalid or may have been corrupted
                                                                                                                         @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                         @mainrecovtext: Reset device and try again. If problem persists contact ADI.
                                                                                                                    */
                                                                                                                    
    /*!< ---- Object ID = ADRV904X_CPU_OBJID_CFG_CARRIER_RECONFIG Section Base Error Code = 0xBF00 ---                  @errcode: 0xBF00
                                                                                                                        @desc: CarrierReconfigure
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                    */

    ADRV904X_CPU_CARRIER_RECONFIG_CHANNEL_FILTER_ERROR     = (ADRV904X_CPU_CARRIER_RECONFIG_ERROR_CODE_START + 1u),   /*!< @errcode: 0xBF01
                                                                                                                         @desc: CarrierReconfigure: Channel filter calculation error
                                                                                                                         @maincause: Invalid carrier configuration supplied
                                                                                                                         @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                         @mainrecovtext: Check supplied carrier configuration and channel filters
                                                                                                                      */

    ADRV904X_CPU_DDCC_DRV_INIT_ERR             = (ADRV904X_CPU_DDCC_DRV_ERROR_CODE_START + 1u),                     /*!<@errcode: 0xF101
                                                                                                                        @desc: DDCC: HW in unexpected state
                                                                                                                        @maincause: DDCC HW error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device using a Valid Package and try again. Contact ADI if the problem persists.
                                                                                                                      */
        
    ADRV904X_CPU_DDCC_DRV_SM_ERR             = (ADRV904X_CPU_DDCC_DRV_ERROR_CODE_START + 2u),                      /*!<@errcode: 0xF102
                                                                                                                        @desc: DDCC: HW SM in unexpected state
                                                                                                                        @maincause: DDCC HW State Machine error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device using a Valid Package and try again. Contact ADI if the problem persists.
                                                                                                                      */
    
    ADRV904X_CPU_DDCC_DRV_CM_ERR             = (ADRV904X_CPU_DDCC_DRV_ERROR_CODE_START + 3u),                      /*!<@errcode: 0xF103
                                                                                                                        @desc: DDCC: CAL Filter CM in unexpected state
                                                                                                                        @maincause: DDCC HW CAL Filter CM error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device using a Valid Package and try again. Contact ADI if the problem persists.
                                                                                                                      */        
        
    /*!< --- Object ID = ADRV904X_CPU_OBJID_SYSTEM_ERROR Section Base Error Code = 0xFF00 ---                           @errcode: 0xFF00
                                                                                                                        @desc: System
                                                                                                                        @maincause:
                                                                                                                        @mainrecovenum:
                                                                                                                        @separator: true
                                                                                                                        */
    ADRV904X_CPU_SYSTEM_GP_TIMER_ERROR                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 1u),               /*!<@errcode: 0xFF01
                                                                                                                        @desc: System: General-Purpose Timer Error
                                                                                                                        @maincause: Software Timer allocation failure.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_ERROR                  = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 2u),               /*!<@errcode: 0xFF02
                                                                                                                        @desc: System: Generic Mailbox Error
                                                                                                                        @maincause: Unspecified Mailbox Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_BUSY                   = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 3u),               /*!<@errcode: 0xFF03
                                                                                                                        @desc: System: Mailbox Busy Error
                                                                                                                        @maincause: The requested Mailbox Hardware is in use and cannot receive commands
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun feature. If the problem persists reset and try again. Contact ADI if the problem persists.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_INVALID_HANDLE         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 4u),               /*!<@errcode: 0xFF04
                                                                                                                        @desc: System: Invalid Mailbox Handle
                                                                                                                        @maincause: Mailbox Operation requested on an Invalid Mailbox
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_INVALID_LINK_NUMBER    = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 5u),               /*!<@errcode: 0xFF05
                                                                                                                        @desc: System: Invalid Mailbox Link Number
                                                                                                                        @maincause: Mailbox Operation requested on an Invalid Mailbox Link
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_LINK_BUSY              = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 6u),               /*!<@errcode: 0xFF06
                                                                                                                        @desc: System: Mailbox Link busy
                                                                                                                        @maincause: The requested Mailbox Link is in use and cannot receive commands
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun feature. If the problem persists reset and try again. Contact ADI if the problem persists.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_CMD_TOO_LARGE          = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 7u),               /*!<@errcode: 0xFF07
                                                                                                                        @desc: System: Mailbox Command too large for Link buffer
                                                                                                                        @maincause: Command payload is unexpectedly too large for Mailbox buffer
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_MAILBOX_CMD_BAD_TID            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 8u),               /*!<@errcode: 0xFF08
                                                                                                                        @desc: System: Mailbox Command Invalid transaction ID
                                                                                                                        @maincause: Mailbox received a Command with an Invalid transaction ID, most likely a duplicate Command.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun feature. If the problem persists reset and try again. Contact ADI if the problem persists.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CPU_ID                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 9u),               /*!<@errcode: 0xFF09
                                                                                                                        @desc: System: Invalid CPU ID specified
                                                                                                                        @maincause: Given CPU ID Parameter is Invalid. 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the CPU firmware image. If the problem persists contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_NULL_PTR                       = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 10u),              /*!<@errcode: 0xFF0A
                                                                                                                        @desc: System: Null Pointer Detected
                                                                                                                        @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the CPU firmware image. If the problem persists contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_TASK_CREATE_ERROR              = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 11u),              /*!<@errcode: 0xFF0B
                                                                                                                        @desc: System: O/S Task Create Error
                                                                                                                        @maincause: Invalid Parameters passed to the Task Create API
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CAPTURE_RAM_LOCK_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 12u),              /*!<@errcode: 0xFF0C
                                                                                                                        @desc: System: RAM Lock Error
                                                                                                                        @maincause: RAM Lock Procedure not followed for Data Capture
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CAPTURE_RAM_UNLOCK_ERROR       = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 13u),              /*!<@errcode: 0xFF0D
                                                                                                                        @desc: System: RAM Unlock Error
                                                                                                                        @maincause: RAM Unlock Procedure not followed for Data Capture
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_CTRL_CMD_ARG_INVALID                  = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 14u),              /*!<@errcode: 0xFF0E
                                                                                                                        @desc: System: Invalid Control Command Argument
                                                                                                                        @maincause: Invalid Control Command Argument
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Verify Command Argument is correct and try again.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_INIT_ERROR            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 15u),              /*!<@errcode: 0xFF0F
                                                                                                                        @desc: System: Initialization Calibration Startup Error
                                                                                                                        @maincause: Initialization Calibration Startup Error
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI 
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_ABORTED_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 20u),              /*!<@errcode: 0xFF14
                                                                                                                        @desc: System: Aborted Initialization Calibration 
                                                                                                                        @maincause: Initialization Calibration was Aborted by Command
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun the Initialization Calibrations
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_WARM_BOOT_CHKSUM_ERROR= (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 21u),              /*!<@errcode: 0xFF15
                                                                                                                        @desc: System: Initialization Calibration Warm Boot Checksum Error
                                                                                                                        @maincause: Initialization Calibration Warm Boot Data Image is corrupted
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun Initialization Calibration 
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_INV_CHAN_MASK_ERROR   = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 22u),              /*!<@errcode: 0xFF16
                                                                                                                        @desc: System: Initialization Calibration Invalid Channel mask Error
                                                                                                                        @maincause: Channel numbers that are not Enabled in the Profile are selected in the Channel mask
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the Initialization Calibration with correct Channel mask
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_INV_CAL_MASK_ERROR    = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 23u),              /*!<@errcode: 0xFF17
                                                                                                                        @desc: System: Initialization Calibration Invalid Calibration mask Error
                                                                                                                        @maincause: The Initialization Calibration mask might have unsupported Calibrations
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the Initialization Calibration with correct Calibration mask
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INIT_CAL_ALREADY_IN_PROGRESS   = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 24u),              /*!<@errcode: 0xFF18
                                                                                                                        @desc: System: Initialization Calibration Busy
                                                                                                                        @maincause: Initialization Calibration Command is sent again while one is in progress
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Make sure not to send the Initialization Calibration Command while one is in progress.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_EFUSE_PROFILE_ERROR            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 25u),              /*!<@errcode: 0xFF19
                                                                                                                        @desc: System: Profile Checksum Failed or bad Profile wrt to EFUSE
                                                                                                                        @maincause: The Device Profile image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reprogram the Device Profile image. If the problem persists contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_EFUSE_PRODUCT_ID_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 26u),              /*!<@errcode: 0xFF1A
                                                                                                                        @desc: System: Profile product ID doesn't match Profile product ID
                                                                                                                        @maincause: The Device Profile is not valid for this Product
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Reprogram the Device with a Valid Profile Image. If the problem persists contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_EVENT_CREATE_ERROR             = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 28u),              /*!<@errcode: 0xFF1C
                                                                                                                        @desc: System: Could not create the event
                                                                                                                        @maincause: Invalid Event Parameters are passed to the OS call
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_QUEUE_CREATE_ERROR             = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 29u),              /*!<@errcode: 0xFF1D
                                                                                                                        @desc: System: Could not create the Queue
                                                                                                                        @maincause: Invalid Queue Parameters are passed to the OS call
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_TIMER_CREATE_ERROR             = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 30u),              /*!<@errcode: 0xFF1E
                                                                                                                        @desc: System: Could not create the Timer
                                                                                                                        @maincause: Invalid Timer Parameters are passed to the OS call.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_QUEUE_POST_ERROR               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 31u),              /*!<@errcode: 0xFF1F
                                                                                                                        @desc: System: Failed to post the message
                                                                                                                        @maincause: Message Queue Full
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_QUEUE_RECV_ERROR               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 32u),              /*!<@errcode: 0xFF20
                                                                                                                        @desc: System: Failed to receive the message
                                                                                                                        @maincause: Invalid Queue Parameters are passed to the OS call
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_TASK_DOES_NOT_EXIST            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 36u),              /*!<@errcode: 0xFF24
                                                                                                                        @desc: System: Commands sent to task that does not exist
                                                                                                                        @maincause: This can happen if a Command is sent to a Channel that is not Enabled in the Profile or disabled.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the feature with the correct Channel mask.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CTRL_TASK_INVALID_MSG          = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 37u),              /*!<@errcode: 0xFF25
                                                                                                                        @desc: System: Invalid Control Command
                                                                                                                        @maincause: Invalid Control Command Received
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the feature with the correct Command.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INSUFFICIENT_MEMORY            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 38u),              /*!<@errcode: 0xFF26
                                                                                                                        @desc: System: Insufficient Memory available for the Requested Operation
                                                                                                                        @maincause: This can happen if an insufficient buffer size is sent for a Command. For example get Configuration or get Calibration status.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the Command with the correct buffer size allocated.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CTRL_FUNC_NOT_SUPPORTED        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 39u),              /*!<@errcode: 0xFF27
                                                                                                                        @desc: System: Invalid Calibration Control Command
                                                                                                                        @maincause: Invalid Control Command Sent to Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Rerun the Calibration with the Correct Control Command
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SET_CONFIG_FUNC_NOT_SUPPORTED  = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 40u),              /*!<@errcode: 0xFF28
                                                                                                                        @desc: System: Invalid Calibration Set Configuration
                                                                                                                        @maincause: Invalid Set Configuration sent to Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_GET_CONFIG_FUNC_NOT_SUPPORTED  = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 41u),              /*!<@errcode: 0xFF29
                                                                                                                        @desc: System: Invalid Calibration Get Configuration
                                                                                                                        @maincause: Invalid Get Configuration sent to Calibration
                                                                                                                        @mainrecovenum:ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_STATUS_FUNC_NOT_SUPPORTED      = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 42u),              /*!<@errcode: 0xFF2A
                                                                                                                        @desc: System: Invalid Calibration Get Status Command
                                                                                                                        @maincause: Invalid Get Status Command sent to Calibration
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CHANNEL_MASK           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 43u),              /*!<@errcode: 0xFF2B
                                                                                                                        @desc: System: The given Channel is not Enabled in the Profile
                                                                                                                        @maincause: This can happen if a Channel is not Enabled in the Profile is getting Enabled or disabled.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CONFIG_OBJECT          = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 44u),              /*!<@errcode: 0xFF2C
                                                                                                                        @desc: System: Invalid Configuration Object
                                                                                                                        @maincause: Invalid Set Configuration Command for Configuration Object ID
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CONFIG_SIZE            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 45u),              /*!<@errcode: 0xFF2D
                                                                                                                        @desc: System: Invalid Configuration Data Size
                                                                                                                        @maincause: Invalid Configuration Data Size for Module Configuration Buffer Size
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CONFIG_OFFSET          = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 46u),              /*!<@errcode: 0xFF2E
                                                                                                                        @desc: System: Invalid Configuration Offset
                                                                                                                        @maincause: Invalid Offset Sent with Configuration Command
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INVALID_CONFIG_STATE           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 47u),              /*!<@errcode: 0xFF2F
                                                                                                                        @desc: System: Invalid Configuration State
                                                                                                                        @maincause: Cannot Update Configuration while Initialization Calibrations are Running
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Rerun the Configuration Command after the Initialization Calibrations are complete.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_HAL_FUNC_NOT_IMPLEMENTED       = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 81u),              /*!<@errcode: 0xFF51
                                                                                                                        @desc: System: CPU HAL Feature not Implemented
                                                                                                                        @maincause: CPU HAL Feature not Implemented
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_HAL_INVALID_CHANNEL            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 82u),              /*!<@errcode: 0xFF52
                                                                                                                        @desc: System: HAL Invalid Channel
                                                                                                                        @maincause: Invalid Channel Configuration in Profile or an Invalid Channel requested
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Verify Channel Configuration and reprogram the Device Profile Image
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_HAL_INVALID_LO_ERROR           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 83u),              /*!<@errcode: 0xFF53
                                                                                                                        @desc: System: HAL Invalid LO
                                                                                                                        @maincause: Invalid PLL chosen for LO. This could be due to corrupted program memory
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_AHB_COMMON_INVALID_ERROR       = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 84u),              /*!<@errcode: 0xFF54
                                                                                                                        @desc: System: AHB (Common) Error detected
                                                                                                                        @maincause: Invalid SPI address access either an Invalid address or span of registers
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Verify address and Range are correct for the SPI region being accessed
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_AHB_CPU0_INVALID_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 85u),              /*!<@errcode: 0xFF55
                                                                                                                        @desc: System: AHB (CPU0) Error detected
                                                                                                                        @maincause: CPU0 accessing Invalid memory address or Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_AHB_CPU1_INVALID_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 86u),              /*!<@errcode: 0xFF56
                                                                                                                        @desc: System: AHB (CPU1) Error detected
                                                                                                                        @maincause: CPU1 accessing Invalid memory address or Range
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_INTERNAL_MCS_FAILED            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 87u),              /*!<@errcode: 0xFF57
                                                                                                                        @desc: System: Internal Multi-chip Sync (MCS) Failed to complete
                                                                                                                        @maincause: MCS Hardware did not complete synchronization in the allotted time
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SECONDARY_CPU_BOOT_FAILED      = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 88u),              /*!<@errcode: 0xFF58
                                                                                                                        @desc: System:Primary CPU detected secondary CPU(s) boot failure
                                                                                                                        @maincause: Secondary core not completing boot
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_UTIL_TIMER_ERROR               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 89u),              /*!<@errcode: 0xFF59
                                                                                                                        @desc: System: Utilities Timer Internal Error
                                                                                                                        @maincause: RTOS Task Scheduler not running for Timer request
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_LOGEN_INVALID_LO_CONFIG        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 90u),              /*!<@errcode: 0xFF5A
                                                                                                                        @desc: System: LOGEN Invalid LO Input/Output Configuration
                                                                                                                        @maincause: User selected an Invalid LOGEN Configuration, verify Device Profile/Configurator settings
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_FORCE_EXCEPTION_COMMAND        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 92u),              /*!<@errcode: 0xFF5C
                                                                                                                        @desc: System: Intentional crash, from force exception Command
                                                                                                                        @maincause: API Command sent to force an exception in the CPU
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: This is a deliberate forced Error
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CONFIG_LOCKED                  = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 93u),              /*!<@errcode: 0xFF5D
                                                                                                                        @desc: System: Configuration locked, for updating
                                                                                                                        @maincause: The Configuration is not unlocked before sending the Configuration Command
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Unlock the Configuration and resend the Configuration Command
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CONFIG_KEY_MISMATCH            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 94u),              /*!<@errcode: 0xFF5E
                                                                                                                        @desc: System: The given key to unlock Configuration did not match with the key in current package
                                                                                                                        @maincause: The Configuration key did not match required key to unlock
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Use the correct key given by ADI and try to unlock the Configuration again
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CONFIG_LIMIT_REACHED           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 95u),              /*!<@errcode: 0xFF5F
                                                                                                                        @desc: System: Number of Configuration update limit reached
                                                                                                                        @maincause: Reached the limit of the number Configuration commands allowed
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset the Device to send new set of Configuration Parameters
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_DBG_KEY_MISMATCH               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 96u),              /*!<@errcode: 0xFF60
                                                                                                                        @desc: System: The given key to enter debug mode did not match with the key to enter debug mode
                                                                                                                        @maincause: The debug key did not match required key to unlock.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Try to send the Command again with the right Debug key. Contact ADI to get the correct key if the problem persists.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_CTRL_CMD_LENGTH_INVALID        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 97u),              /*!<@errcode: 0xFF61
                                                                                                                        @desc: System: The Set Control Command length is less than expected or greater than max data size allowed
                                                                                                                        @maincause: The control Command buffer is greater than the supported size
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Change the size of the control Command payload or increase the control Command buffer size.
                                                                                                                        */
    ADRV904X_CPU_SYSTEM_WATCHDOG_EXPIRED               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 98u),              /*!<@errcode: 0xFF62
                                                                                                                        @desc: System: Watchdog Timer expired, system is unresponsive
                                                                                                                        @maincause: Not all firmware tasks completed in the expected time frame.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Collect memory dump to provide debug information and reset Device.
                                                                                                                        @cause: Not all firmware tasks completed in the expected time frame, possibly due to bad Parameters
                                                                                                                        @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @recovtext: Collect memory dump to provide debug information, verify that all Calibrations and commands have valid Parameters, and reset Device.
                                                                                                                        */
    ADRV904X_CPU_SYSTEM_SHARED_MEM_MUTEX_TIMEOUT       = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 99u),              /*!<@errcode: 0xFF63
                                                                                                                        @desc: System: Shared memory Mutex Timeout
                                                                                                                        @maincause: This could happen if the shared Mutex cannot be obtained within the Timeout period
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                        */
    ADRV904X_CPU_SYSTEM_SWBKPT_INVALID_TABLE_INDEX    = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 100u),              /*!<@errcode: 0xFF64
                                                                                                                        @desc: System: The given index to the SW Breakpoint Table is out of Range
                                                                                                                        @maincause: Unexpected SW Breakpoint Table Index
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SHARED_MEM_MUTEX_TAKEN_ERROR   = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 101u),             /*!<@errcode: 0xFF65
                                                                                                                        @desc: System: Shared memory Mutex already taken
                                                                                                                        @maincause: This could happen if trying to take the Mutex which is already taken
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */ 
    ADRV904X_CPU_SYSTEM_SHARED_MEM_MUTEX_RELEASE_ERROR = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 102u),             /*!<@errcode: 0xFF66
                                                                                                                        @desc: System: Shared memory Mutex released without owning it
                                                                                                                        @maincause: This could happen if trying to release the Mutex which is not owned
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */    
    ADRV904X_CPU_SYSTEM_UNSUPPORTED_CHANS_PER_CPU_ERROR = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 103u),            /*!< @errcode: 0xFF67
                                                                                                                        @desc: System: This system is being configured to support too many channels per CPU
                                                                                                                        @maincause: This is a Configurator Channel enable issue
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Check the Profile and the Configurator logic
                                                                                                                    */   
    ADRV904X_CPU_SYSTEM_INVALID_STATUS_SIZE           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 104u),              /*!<@errcode: 0xFF68
                                                                                                                        @desc: System: The given status size is Invalid
                                                                                                                        @maincause: The user requested the status size larger than supported size.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext:  Change the size of the status Command buffer size.
                                                                                                                    */ 
    ADRV904X_CPU_SYSTEM_STARTUP_TIMEOUT_ERROR         = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 105u),              /*!<@errcode: 0xFF69
                                                                                                                        @desc: System: Initialization and tracking start-up functions still running
                                                                                                                        @maincause: The Control task waits a predefined amount of time which is not long enough
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Increase the Timeout in the FW Control task.
                                                                                                                    */      
    ADRV904X_CPU_SYSTEM_DBG_CMD_NOT_SUPPORTED           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 106u),            /*!<@errcode: 0xFF6A
                                                                                                                        @desc: System: Invalid Debug Command
                                                                                                                        @maincause: Invalid Debug Command Provided 
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Debug Command is Supported. Contact ADI to get the correct Command if the problem persists.
                                                                                                                    */    
    ADRV904X_CPU_SYSTEM_DBG_CMD_PARAMS_INVALID          = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 107u),            /*!<@errcode: 0xFF6B
                                                                                                                        @desc: System: Invalid Debug Command Parameter
                                                                                                                        @maincause: Invalid Debug Command Parameter Provided
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Debug Command Parameter is Supported. Contact ADI to get the correct info if the problem persists.
                                                                                                                    */     
    ADRV904X_CPU_SYSTEM_HAL_TX_ATTEN_RESTORE_ERROR      = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 108u),            /*!<@errcode: 0xFF6C
                                                                                                                        @desc: System: HAL Tx attenuation Restore Failed
                                                                                                                        @maincause: Cannot restore Tx attenuation because the settings were not saved
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: Reset Feature and try again. If the problem persists acquire a Memory Dump and contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_DEBUG_FUNC_NOT_SUPPORTED        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 109u),            /*!<@errcode: 0xFF6D
                                                                                                                        @desc: System: Invalid Calibration Debug Control Command
                                                                                                                        @maincause: Invalid Debug Control Command sent to Calibration Feature
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Check Control Command is Supported
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_HEALTHMON_TOO_MANY_TASKS        = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 110u),            /*!<@errcode: 0xFF6E
                                                                                                                        @desc: System: The health monitor memory allocation is not large enough for the current number of tasks
                                                                                                                        @maincause: The number of RTOS tasks is greater than expected.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Allocate the required health monitor memory for the number of RTOS tasks
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SEM_CREATE_FAILED               = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 111u),            /*!<@errcode: 0xFF6F
                                                                                                                        @desc: System: Semaphore Create Failure
                                                                                                                        @maincause: System Failed to Create Semaphore
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SEM_TAKE_FAILED                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 112u),            /*!<@errcode: 0xFF70
                                                                                                                        @desc: System: Semaphore Take Failure
                                                                                                                        @maincause: System Failed to Take Semaphore
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SIMULATED_ERROR                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 113u),            /*!<@errcode: 0xFF71
                                                                                                                        @desc: System: Simulated Error
                                                                                                                        @maincause: Simulated System Error Condition for Internal Testing
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SI_REV_ID_ERROR                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 114u),            /*!<@errcode: 0xFF72
                                                                                                                        @desc: System: Unexpected Silicon Revision Detected
                                                                                                                        @maincause: Using Incompatible Hardware with Current Version of Software
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_RADIO_LOOPBACK_ERROR            = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 115u),            /*!<@errcode: 0xFF73
                                                                                                                        @desc: System: Radio Loopback requested but being used
                                                                                                                        @maincause: A Calibration is already using a radio Loopback
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Verify Calibration logic to find multiple request
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_TIMER_STOP_ERROR                = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 116u),            /*!<@errcode: 0xFF74
                                                                                                                        @desc: System: System Timer stop Error
                                                                                                                        @maincause: System Timer didn't stop within Timeout
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_TRACKING_CAL_SUSPEND_TIMEOUT    = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 117u),            /*!<@errcode: 0xFF75
                                                                                                                        @desc: System: Tracking Calibration did not suspend without Timeout
                                                                                                                        @maincause: The tracking Calibration Failed to suspend within the expected time.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_EFUSE_UNSUPPORTED_CPU           = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 118u),            /*!<@errcode: 0xFF76
                                                                                                                        @desc: System: Only primary CPU supports EFUSE commands
                                                                                                                        @maincause: Sent EFUSE Command to secondary CPU.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: API Error. Contact ADI.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_PROFILE_UNAVAILABLE             = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 119u),            /*!<@errcode: 0xFF77
                                                                                                                        @desc: System: The full Profile is only available during CPU0 boot-up
                                                                                                                        @maincause: Full Profile gets erased after CPU0 boot-up.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                        @mainrecovtext: Don't use full Profile after boot-up.
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_SEM_GIVE_FAILED                 = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 120u),            /*!<@errcode: 0xFF78
                                                                                                                        @desc: System: Semaphore give Operation failure
                                                                                                                        @maincause: This could happen because the system Failed in giving the semaphore.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                                                                                    */
    ADRV904X_CPU_SYSTEM_EFUSE_READ_TIMEOUT              = (ADRV904X_CPU_SYSTEM_ERROR_CODE_START + 121u),            /*!<@errcode: 0xFF79
                                                                                                                        @desc: System: Unable to read EFUSE
                                                                                                                        @maincause: The EFUSE read did not occur.
                                                                                                                        @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                        @mainrecovtext: Reset Device and try again. If the problem persists contact ADI
                                                                                                                    */

#ifndef ADI_ADRV904X_FW
    /* Pseudo Error code */
    ADRV904X_CPU_CAL_EXIT_OCCURRED_ERROR         = (int32_t)0x80000000      /* Remove UL for API to avoid ISO C Error. set to large number to force enum to be uint32_t */
#else
    /* Pseudo Error code */
    ADRV904X_CPU_CAL_EXIT_OCCURRED_ERROR         = 0x80000000UL             /* Add UL for FW to force it to be unsigned int. set to large number to force enum to be uint32_t */
#endif
} adrv904x_CpuErrorCode_e;

#endif

/* Type and enumeration for CPU boot status and Error codes */
typedef uint8_t adrv904x_CpuBootStatus_t;
typedef enum adrv904x_CpuBootStatus
{
    ADRV904X_CPU_BOOT_STATUS_POWER_UP    = 0u,          /*!<@errcode: 0x0000
                                                            @desc: Boot Status: CPU Powering Up
                                                            @maincause: CPU Still Booting Up
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                            @mainrecovtext: Wait for CPU to Boot
                                                        */
    ADRV904X_CPU_STATUS_READY            = 1u,          /*!<@errcode: 0x0001
                                                            @desc: Boot Status: CPU Ready
                                                            @maincause: CPU Booted Successfully
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                            @mainrecovtext: No Action Required
                                                        */
    ADRV904X_CPU_BOOT_WAIT_FOR_CPUS      = 2u,          /*!<@errcode: 0x0002
                                                            @desc: Boot Status: Primary CPU waiting for Secondary CPU(s)
                                                            @maincause: Secondary CPU(s) Failed to Boot
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                            @cause: Secondary CPU(s) Still Booting
                                                            @recovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                            @recovtext: Wait for CPU to Boot
                                                        */
    ADRV904X_CPU_BOOT_HANDSHAKE_ERR      = 3u,          /*!<@errcode: 0x0003
                                                            @desc: Boot Error: CPU-to-CPU Handshake
                                                            @maincause: Primary CPU did not get ACK from secondary CPU.
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_FW_CRC_ERR              = 4u,          /*!<@errcode: 0x0004
                                                            @desc: Boot Error: CPU Firmware Image CRC Failed
                                                            @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_STREAM_IMG_CRC_ERR      = 5u,          /*!<@errcode: 0x0005
                                                            @desc: Boot Error: Stream Image CRC Failed
                                                            @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_DEVPROF_CRC_ERR         = 6u,          /*!<@errcode: 0x0006
                                                            @desc: Boot Error: Device Profile Image CRC Failed
                                                            @maincause: Programmed image is corrupted. Original image may be Invalid or may have been corrupted during SPI transfer.
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_CLKGEN_ERR         = 7u,          /*!<@errcode: 0x0007
                                                            @desc: Boot Error: CLKGEN Setup Error
                                                            @maincause: System Clock Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_CLKJESD_ERR        = 8u,          /*!<@errcode: 0x0008
                                                            @desc: Boot Error: JESD Setup Error
                                                            @maincause: JESD Clock Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_PWRINIT_ERR        = 9u,          /*!<@errcode: 0x0009
                                                            @desc: Boot Error: Power Initialization Setup Error
                                                            @maincause:  Master Bias Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_JTAG_BUILD_STATUS_READY = 10u,         /*!<@errcode: 0x000A
                                                            @desc: Boot Status: JTAG Build Status Ready
                                                            @maincause: CPU Configured for JTAG Mode
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_CLKLOGEN_ERR       = 11u,         /*!<@errcode: 0x000B
                                                            @desc: Boot Error: Clock LOGEN
                                                            @maincause: Clock LOGEN Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_RXQECHW_ERR        = 12u,         /*!<@errcode: 0x000C
                                                            @desc: Boot Error: Rx QEC
                                                            @maincause: Rx QEC Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_ADC_RCAL_ERR            = 13u,         /*!<@errcode: 0x000D
                                                            @desc: Boot Error: ADC RCAL
                                                            @maincause: ADC RCalibration Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_STREAM_RUNTIME_ERR      = 14u,         /*!<@errcode: 0x000E
                                                            @desc: Boot Error: Main Stream Processor
                                                            @maincause: Main Stream Processor Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_INTERNAL_MCS_ERR        = 15u,         /*!<@errcode: 0x000F
                                                            @desc: Boot Error: Internal MCS
                                                            @maincause: MCS Hardware Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_HAL_DATACAP_INIT_ERR    = 16u,         /*!<@errcode: 0x0010
                                                            @desc: Boot Error: Data Capture
                                                            @maincause: Data Capture Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_FW_CLI_INIT_ERR         = 17u,         /*!<@errcode: 0x0011
                                                            @desc: Boot Error: Command Line Interface
                                                            @maincause: Command Line Interface (CLI) Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_TEMP_ERR           = 18u,         /*!<@errcode: 0x0012
                                                            @desc: Boot Error: Temperature Sensor
                                                            @maincause: Temperature Sensor Initialization Failed 
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_TXNCO_INIT_ERR     = 19u,         /*!<@errcode: 0x0013
                                                            @desc: Boot Error: TxTest/Mix NCO
                                                            @maincause: TX NCO Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_ORXNCO_INIT_ERR    = 20u,         /*!<@errcode: 0x0014
                                                            @desc: Boot Error: ORx Mix NCO
                                                            @maincause: ORX NCO Module Initialization Failed 
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_RXNCO_INIT_ERR     = 21u,         /*!<@errcode: 0x0015
                                                            @desc: Boot Error: Rx Mix NCO
                                                            @maincause: Rx NCO Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_BOOT_TXATTEN_INIT_ERR   = 22u,         /*!<@errcode: 0x0016
                                                            @desc: Boot Error: Tx Attenuator
                                                            @maincause: Tx Attenuator Module Initialization Failed 
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
    ADRV904X_CPU_PRS_MCS_ERR             = 23u,         /*!<@errcode: 0x0017
                                                            @desc: Boot Error: LO PLLs
                                                            @maincause: LOGEN Initialization (Pre MCS) Failed 
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
            
    ADRV904X_CPU_BOOT_UNEXPECTED_ERR     = 24u,         /*!<@errcode: 0x0018
                                                            @desc: Boot Error: Unspecified Error
                                                            @maincause: Unspecific Initialization Error Detected
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */
        
    ADRV904X_CPU_RADIO_RXTX_ERR          = 25u,         /*!<@errcode: 0x0019
                                                            @desc: Boot Error: Radio Rx/Tx 
                                                            @maincause: Radio Rx/Tx Module Initialization Failed
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */

    ADRV904X_CPU_BOOT_PID_PROFILE_MISMATCH_ERR = 26u,   /*!<@errcode: 0x001A
                                                            @desc: Boot Error: Product ID Mismatch
                                                            @maincause: Device Profile Incompatible with Product ID on Device
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                            @mainrecovtext: Use Compatible Device Profile. Contact ADI if the problem persists.
                                                        */
    ADRV904X_CPU_BOOT_TX_ORX_MAPPING_FREQ_ERR = 30u,    /*!<@errcode: 0x001E
                                                            @desc: Boot Error: Tx-to-ORx Mapping
                                                            @maincause: Unable to Calculate and Save Frequencies
                                                            @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                            @mainrecovtext: Check Device Profile, If the Problem Persists, Acquire a Memory Dump and Contact ADI
                                                        */

} adrv904x_CpuBootStatus_e;

/* Type used to store CPU Error status information */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuErrors
{
    adrv904x_CpuErrorCode_e lastCmd;
    adrv904x_CpuErrorCode_e system;
    adrv904x_CpuErrorCode_e trackCal;
} adrv904x_CpuErrors_t;)

#endif /* __ADRV904X_CPU_ERROR_CODES_TYPES_H__ */


