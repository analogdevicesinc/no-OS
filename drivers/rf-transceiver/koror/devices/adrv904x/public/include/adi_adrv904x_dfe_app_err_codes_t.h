/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_app_err_codes_t.h
 *
 * \brief   Contains DFE App error code definitions
 *
 * \details Contains DFE App error code definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */
#ifndef __ADI_ADRV904X_DFE_APP_ERROR_CODES_T_H__
#define __ADI_ADRV904X_DFE_APP_ERROR_CODES_T_H__

#include "adi_adrv904x_dfe_svc_err_object_ids_t.h"



#define ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_APP_ERROR << 8u)                           /*!< 0xE000 - Starting DFE Application error code */
/**
 * \brief DFE firmware error codes
 */

typedef enum adi_adrv904x_DfeAppErrCode
{
    ADI_ADRV904X_DFE_APP_ERR_CODE_NO_ERROR                                    = 0UL,                                          /*!< @errcode: 0x0000
                                                                                                                  * @desc: No error
                                                                                                                  * @maincause: Operation was successful
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                  * @mainrecovtext: No action required
                                                                                                                  * @severity:INFO
                                                                                                                  */
/* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_APP_ERROR Section Base Error Code = 0xE000 ------                         @errcode: 0xE000
 *                                                                                                               @desc: APP Errors
 *                                                                                                               @maincause:
 *                                                                                                               @mainrecovenum:
 *                                                                                                               @severity:
 *                                                                                                               @separator: true
 */
    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CHOL_SOLVER_ERROR                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 1u),   /*!< @errcode: 0xE001
                                                                                                                  * @desc: APP Error: DPD Decomposition Calculation
                                                                                                                  * @maincause: Invalid Parameters Provided from XCORR
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check XCORR Data is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_DESCRIPTOR_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 2u),   /*!< @errcode: 0xE002
                                                                                                                  * @desc: APP Error: DPD Model Descriptor
                                                                                                                  * @maincause: Invalid Parameters Provided in Model Descriptor (i.e. cannot find LUT ID's or Number of LUT's)
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check Model Descriptor Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_DDR_DELAY_ERROR                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 3u),   /*!< @errcode: 0xE003
                                                                                                                  * @desc: APP Error: DPD DDR I and/or J
                                                                                                                  * @maincause: Invalid DDR I and/or J Parameter Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DDR I and/or J Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_POLY_TYPE_ERROR                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 4u),   /*!< @errcode: 0xE004
                                                                                                                  * @desc: APP Error: DPD DDR Mode Polynomial
                                                                                                                  * @maincause: Invalid Polynomial Parameter Provided for DDR Mode
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Polynomial Parameters are Valid for DDR Mode
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_DUPLICATE_LUT_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 5u),   /*!< @errcode: 0xE005
                                                                                                                  * @desc: APP Error: Duplicate DPD LUT
                                                                                                                  * @maincause: Same LUT is assigned multiple times
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check LUT Usage
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_FEATURE_COMPUTE_HOG                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 6u),   /*!< @errcode: 0xE006
                                                                                                                  * @desc: APP Error: DPD Queue Limit
                                                                                                                  * @maincause: Number of Compute Requests are exceeding the queue of pending request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check how Requests for the Feature are Managed
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MAX_FRAC_EST_CNT_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 7u),   /*!< @errcode: 0xE007
                                                                                                                  * @desc: APP Error: DPD Fractional Delay Estimation Count
                                                                                                                  * @maincause: Fractional Delay can't be Estimated with Bad Captures
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check that Captures will Provide Sufficient Data
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACT_LUT_ENTRY_SAT_ERROR                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 8u),   /*!< @errcode: 0xE008
                                                                                                                  * @desc: APP Error: DPD LUT Saturation
                                                                                                                  * @maincause: Underlying Datatype of LUT parameter is int16
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Value is in the range of int16
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_THREAD_GET_INVALID_CHANNELS             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 9u),   /*!< @errcode: 0xE009
                                                                                                                  * @desc: APP Error: DPD Channel Mask
                                                                                                                  * @maincause: Invalid Channel Mask Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check Channel Mask is Valid, (i.e. One Channel at a Time)
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STATUS_GET_FUNC_NOT_SUPPORTED           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 10u),  /*!< @errcode: 0xE00A
                                                                                                                  * @desc: APP Error: Unsupported DPD Status Get
                                                                                                                  * @maincause: Status Get function not Supported for Provided Tracking Calibration Status Type
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tracking Calibration Status Type has a Supported Status Get Feature
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STATUS_GET_INSUFFICIENT_MEMORY          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 11u),  /*!< @errcode: 0xE00B
                                                                                                                  * @desc: APP Error: DPD Status Buffer Size
                                                                                                                  * @maincause: Invalid Status Buffer Size for the Calibration Status Type Requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Status Size for the Calibration Status Type Requested
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_INVALID_CONFIG_SIZE                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 12u),  /*!< @errcode: 0xE00C
                                                                                                                  * @desc: APP Error: Configuration Structure Size
                                                                                                                  * @maincause: Invalid Configuration Size Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Configuration Read Size is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_INVALID_CONFIG_OFFSET                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 13u),  /*!< @errcode: 0xE00D
                                                                                                                  * @desc: APP Error: Configuration Structure Offset
                                                                                                                  * @maincause: Invalid Configuration Offset Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Configuration Offset is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CTRL_GET_INSUFFICIENT_MEMORY            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 14u),  /*!< @errcode: 0xE00E
                                                                                                                  * @desc: APP Error: DPD Control Command Buffer Size
                                                                                                                  * @maincause: Invalid Control Data Size Provided for Control Command Requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Control Data Size is Valid for Control Command
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_FUNC_NOT_SUPPORTED                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 15u),  /*!< @errcode: 0xE00F
                                                                                                                  * @desc: APP Error: DPD Control Command
                                                                                                                  * @maincause: Invalid Control Command Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Control Command is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CTRL_INVALID_PARAM                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 16u),  /*!< @errcode: 0xE010
                                                                                                                  * @desc: APP Error: DPD Control Command Parameter
                                                                                                                  * @maincause: Invalid Parameter Provided in Model Descriptor
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check Parameters Provided in Model Descriptor
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_TASK_DOES_NOT_EXIST           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 17u),  /*!< @errcode: 0xE011
                                                                                                                  * @desc: APP Error: Calibration Framework Command Not Executed
                                                                                                                  * @maincause: Command sent to a Channel that is not Enabled
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Command is Valid for Specified Channel
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_STATUS_FUNC_NOT_SUPPORTED     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 18u),  /*!< @errcode: 0xE012
                                                                                                                  * @desc: APP Error: Calibration Framework Status Get
                                                                                                                  * @maincause: Status Get Command sent to Calibration which does not support it
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Calibration has Status Get
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_INVALID_CONFIG_STATE          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 19u),  /*!< @errcode: 0xE013
                                                                                                                  * @desc: APP Error: Calibration Type
                                                                                                                  * @maincause: Invalid Calibration Type Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check Calibration Type is Valid
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_SET_CONFIG_FUNC_NOT_SUPPORTED = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 20u),  /*!< @errcode: 0xE014
                                                                                                                  * @desc: APP Error: Calibration Framework Configuration Set
                                                                                                                  * @maincause: Configuration Set Command is sent to Calibration which does not support it
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Configuration Set Command for Calibration is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_GET_CONFIG_FUNC_NOT_SUPPORTED = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 21u),  /*!< @errcode: 0xE015
                                                                                                                  * @desc: APP Error: Calibration Framework Configuration Get
                                                                                                                  * @maincause: Configuration Get Command is sent to Calibration which does not support it
                                                                                                                  * @mainrecovenum:ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Configuration Get Command for Calibration is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_HAL_INVALID_CHANNEL           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 22u),  /*!< @errcode: 0xE016
                                                                                                                  * @desc: APP Error: Calibration Framework HAL Channel
                                                                                                                  * @maincause: Invalid HAL Channel Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Verify Channel Configuration & Reprogram Device with Updated Profile Image
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_INVALID_CONFIG_OFFSET         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 23u),  /*!< @errcode: 0xE017
                                                                                                                  * @desc: APP Error: Calibration Framework Configuration Offset
                                                                                                                  * @maincause: Invalid Calibration Framework Configuration Offset
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Calibration Framework Configuration Offset is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_TASK_CREATE_ERROR             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 24u),  /*!< @errcode: 0xE018
                                                                                                                  * @desc: APP Error: Calibration Framework Task Thread Pointer
                                                                                                                  * @maincause: Invalid Thread Pointer Provided for Task by OSAL
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_EVENT_CREATE_ERROR            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 25u),  /*!< @errcode: 0xE019
                                                                                                                  * @desc: APP Error: Calibration Framework Task Event Pointer
                                                                                                                  * @maincause: Invalid Event Pointer Provided for Task by OSAL
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_TIMER_CREATE_ERROR            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 26u),  /*!< @errcode: 0xE01A
                                                                                                                  * @desc: APP Error: Calibration Framework Task Timer Pointer
                                                                                                                  * @maincause: Invalid Timer Pointer Provided for Task by OSAL
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAL_FRAMEWORK_TRACKING_CAL_SUSPEND_TIMEOUT  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 27u),  /*!< @errcode: 0xE01B
                                                                                                                  * @desc: APP Error: Calibration Framework Suspend Timeout
                                                                                                                  * @maincause: Tracking Calibration failed to Suspend within the expected time
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset Tracking Calibration and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_EVENT_TIMEOUT_ERROR                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 28u),  /*!< @errcode: 0xE01C
                                                                                                                  * @desc: APP Error: Calibration Framework Event Timeout
                                                                                                                  * @maincause: Tracking Calibration failed to Suspend
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Timeout is Valid for Tracking Calibration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PWR_METER_DPD_RATE_OVERFLOW             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 29u),  /*!< @errcode: 0xE01D
                                                                                                                  * @desc: APP Error: DPD Power Meter Rate Overflow
                                                                                                                  * @maincause: Invalid DPD Power Meter Rate
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Rate is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PWR_METER_CONFIG_FAIL                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 30u),  /*!< @errcode: 0xE01E
                                                                                                                  * @desc: APP Error: DPD Power Meter Configuration
                                                                                                                  * @maincause: Invalid DPD Power Meter Configuration Parameter(s)
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD Power Meter Configuration Parameter(s) are Valid
                                                                                                                  * @severity:
                                                                                                                  */
    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_LUT_COPY_FAILED                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 31u),  /*!< @errcode: 0xE01F
                                                                                                                  * @desc: APP Error: LUT Copy
                                                                                                                  * @maincause: Invalid LUT Update Parameter(s)
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check LUT Update Parameter(s) are Valid
                                                                                                                  * @severity: CRITICAL, FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_SEQ_MUTEX_REQ_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 32u),  /*!< @errcode: 0xE020
                                                                                                                  * @desc: APP Error: Capture Sequencer Mutex
                                                                                                                  * @maincause: Mutex Acquire/Release Failed for Capture Sequencer
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_SEQ_PERIOD_REQ_INVALID              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 33u),  /*!< @errcode: 0xE021
                                                                                                                  * @desc: APP Error: Capture Sequencer Period
                                                                                                                  * @maincause: Invalid Channel or Capture Period Type Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Channel and Capture Period Type are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_SEQ_CAPTURE_DONE_FAILED             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 34u),  /*!< @errcode: 0xE022
                                                                                                                  * @desc: APP Error: Capture Sequence Done Event
                                                                                                                  * @maincause: Invalid Manual Capture Done Event
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_INCOMPLETE_CAPTURE                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 35u),  /*!< @errcode: 0xE023
                                                                                                                  * @desc: APP Error: Capture Sequence Incomplete
                                                                                                                  * @maincause: Incomplete Capture Detected
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Capture Parameters are Valid
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_CONFIG_FAILED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 36u),  /*!< @errcode: 0xE024
                                                                                                                  * @desc: APP Error: DPD Model Configuration
                                                                                                                  * @maincause: Invalid Model Configuration Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Model Configuration Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_EVENT_RX_HANDLE_INIT_FAILED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 37u),  /*!< @errcode: 0xE025
                                                                                                                  * @desc: APP Error: Event Receive Handle Initialization
                                                                                                                  * @maincause: OSAL Failed to Initialize Event Receive Handle
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_EVENT_THREAD_CREATION_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 38u),  /*!< @errcode: 0xE026
                                                                                                                  * @desc: APP Error: Event Thread Create
                                                                                                                  * @maincause: OSAL Failed to Create Event Thread
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CAPTURE_REQUEST_FAILED                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 39u),  /*!< @errcode: 0xE027
                                                                                                                  * @desc: APP Error: DPD Capture Request
                                                                                                                  * @maincause: Invalid DPD Capture Request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD Capture Request Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_EVENT_HANDLE_GET_FAILED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 40u),  /*!< @errcode: 0xE028
                                                                                                                  * @desc: APP Error: Event Handle Get
                                                                                                                  * @maincause: OSAL failed to Get Event Handle
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_UNITY_MODEL_APPLIED                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 41u),  /*!< @errcode: 0xE029
                                                                                                                  * @desc: APP Error: Unity Model
                                                                                                                  * @maincause: Small Tx Signal Detected
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx Signal Configuration and/or Apply Unity Model
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ADAPTATION_CONFIG_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 42u),  /*!< @errcode: 0xE02A
                                                                                                                  * @desc: APP Error: DPD Adaptation Configuration
                                                                                                                  * @maincause: Invalid DPD Adaptation Configuration Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check DPD Adaptation Configuration Parameters are Valid
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACTUATOR_PASSTHROU_FAILED               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 43u),  /*!< @errcode: 0xE02B
                                                                                                                  * @desc: APP Error: DPD Actuator Passthrough Mode
                                                                                                                  * @maincause: Enabling/Disabling DPD actuator Passthrough mode Failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check Passthrough Parameters are Valid
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_LINEAR_TERM_CONFIG_FAILED         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 44u),  /*!< @errcode: 0xE02C
                                                                                                                  * @desc: APP Error: Linear Term Configuration
                                                                                                                  * @maincause: Linear Term Setting plus DDR j Delay exceeds J Delay Buffer Size
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Liner Term Setting and/or J Delay Buffer Size are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ABORTED_CAPTURE                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 45u),  /*!< @errcode: 0xE02D
                                                                                                                  * @desc: APP Error: Aborted Capture
                                                                                                                  * @maincause: Capture was Aborted
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Trigger new DPD Capture
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PATHDELAY_LAG_RANGE_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 46u),  /*!< @errcode: 0xE02E
                                                                                                                  * @desc: APP Error: DPD pathdelay lag range
                                                                                                                  * @maincause: Integer delay was drifted by more than 1 sample
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Enlarge xcorr lag range
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ACTUATOR_ENABLE_FAILED                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 47u),  /*!< @errcode: 0xE02F
                                                                                                                  * @desc: APP Error: DPD Actuator Enable Mode
                                                                                                                  * @maincause: Enabling/Disabling DPD Actuator Failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Check Enable Parameters are Valid
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POWER_METER_MUTEX_REQ_FAILED            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 48u),  /*!< @errcode: 0xE030
                                                                                                                  * @desc: APP Error: Power meter Sequencer Mutex req failed
                                                                                                                  * @maincause: Mutex Acquire/Release Failed for DPD power meter
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POWER_METER_INVALID_PARAM               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 49u),  /*!< @errcode: 0xE031
                                                                                                                  * @desc: APP Error: Power meter function is called with invalid parameter
                                                                                                                  * @maincause: Power meter function is called with invalid parameter
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_VBANK_NOT_POPULATED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 50u),  /*!< @errcode: 0xE032
                                                                                                                  * @desc: APP Error: DPD Model Vbank not populated
                                                                                                                  * @maincause: Selected Vbank to write LUTs isn't populated yet
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset the feature, contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_STABILITY_ERROR                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 51u),  /*!< @errcode: 0xE033
                                                                                                                  * @desc: APP Error: DPD stability check error
                                                                                                                  * @maincause: Selected performance metric(s) exceed the programmed threshold(s)
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Robustness framework takes recovery action(s)
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_NOT_SUPPORTED_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 52u),  /*!< @errcode: 0xE034
                                                                                                                  * @desc: APP Error: DPD supported model error
                                                                                                                  * @maincause: Configured model can not be supported with selected actuator depth
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Application Command Message is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CAPTURE_ABORT_FAILED                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 53u),  /*!< @errcode: 0xE035
                                                                                                                  * @desc: APP Error: Capture abort failure
                                                                                                                  * @maincause: Capture abort failure
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_LOW_ERROR                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 54u),  /*!< @errcode: 0xE036
                                                                                                                  * @desc: APP Error: ORx power is below the low power threshold
                                                                                                                  * @maincause: ORx signal power is too low and below the threshold
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust ORx gain to increase singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_LOW_ERROR                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 55u),  /*!< @errcode: 0xE037
                                                                                                                  * @desc: APP Error: Tx power is below the low power threshold
                                                                                                                  * @maincause: Tx signal power is too low and below the threshold
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backoff to increase singal power or check LUT entries
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_LOW_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 56u),  /*!< @errcode: 0xE038
                                                                                                                  * @desc: APP Error: Post-DPD Tx power is below the low power threshold
                                                                                                                  * @maincause: Post-DPD Tx signal power is too low and below the threshold
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backoff to increase singal power or check LUT entries
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_TOO_HIGH_ERROR                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 57u),  /*!< @errcode: 0xE039
                                                                                                                  * @desc: APP Error: ORx power is above the high power threshold
                                                                                                                  * @maincause: ORx signal power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust ORx gain to attenuate singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_TOO_HIGH_ERROR                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 58u),  /*!< @errcode: 0xE03A
                                                                                                                  * @desc: APP Error: Tx power is above the high power threshold
                                                                                                                  * @maincause: Tx signal power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backoff to decrease singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_TOO_HIGH_ERROR              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 59u),  /*!< @errcode: 0xE03B
                                                                                                                  * @desc: APP Error: Post-DPD Tx power is above the high power threshold
                                                                                                                  * @maincause: Post-DPD Tx signal power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backoff to decrease singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_PEAK_TOO_LOW_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 60u),  /*!< @errcode: 0xE03C
                                                                                                                  * @desc: APP Error: ORx peak power is below the peak power threshold
                                                                                                                  * @maincause: ORx signal peak power is too low
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust ORx atten to increase singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_PEAK_TOO_LOW_ERROR                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 61u),  /*!< @errcode: 0xE03D
                                                                                                                  * @desc: APP Error: Tx peak power is below the peak power threshold
                                                                                                                  * @maincause: Tx signal peak power is too low
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backtoff to increase singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_PEAK_TOO_LOW_ERROR          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 62u),  /*!< @errcode: 0xE03E
                                                                                                                  * @desc: APP Error: Post-DPD Tx peak power is below the peak power threshold
                                                                                                                  * @maincause: Post-DPD Tx signal peak power is too low
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backtoff to increase singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ORX_SATURATING_ERROR                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 63u),  /*!< @errcode: 0xE03F
                                                                                                                  * @desc: APP Error: ORx peak power is above the peak power threshold
                                                                                                                  * @maincause: ORx signal peak power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust ORx atten to reduce singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_TX_SATURATING_ERROR                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 64u),  /*!< @errcode: 0xE040
                                                                                                                  * @desc: APP Error: Tx peak power is above the peak power threshold
                                                                                                                  * @maincause: Tx signal peak power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backtoff to reduce singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_POST_DPD_TX_SATURATING_ERROR            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 65u),  /*!< @errcode: 0xE041
                                                                                                                  * @desc: APP Error: Post-DPD Tx peak power is above the peak power threshold
                                                                                                                  * @maincause: Post-DPD Tx signal peak power is too high
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: adjust tx digital backtoff to reduce singal power
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_EXCEED_MAX_FEATURE_INDEXES_ERROR        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 66u),  /*!< @errcode: 0xE042
                                                                                                                  * @desc: APP Error: Exceed maximum number of feature indexes per feature segment
                                                                                                                  * @maincause: DPD model might not be right
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: check DPD model or reach ADI for support
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_EXCEED_MAX_FEATURE_SEGMENTS_ERROR       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 67u),  /*!< @errcode: 0xE043
                                                                                                                  * @desc: APP Error: Exceed maximum number of feature segments
                                                                                                                  * @maincause: DPD model might not be right
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: check DPD model or reach ADI for support
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_EXCEED_PATHDELAY_RANGE_ERROR            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 68u),  /*!< @errcode: 0xE044
                                                                                                                  * @desc: APP Error: Difference between estiamted and seeded pathdelays exceeded the programmed range
                                                                                                                  * @maincause: This is for informational purposes only. There is insufficient information in the signal to estimate path delay
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Confirm the seed value and the programmed qualify range. No recovery action expected from the user if these values provided are accurate. The firmware will use the seeded value automatically, in case the path delay cannot be estimated
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_R_TABLE_NO_COEFFICIENTS                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 69u),  /*!< @errcode: 0xE045
                                                                                                                  * @desc: APP Error: SW_TO_R_TABLE recovery action used when no R table coefficients are configured
                                                                                                                  * @maincause: Model 2 (R table) coefficients not configured
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Ensure Model 2 (R table) has been configured, including after a DPD Reset, or disable use of SW_TO_R_TABLE recovery action
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_CAPTURE_RETRY_AFTER_ABORT              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 70u),  /*!< @errcode: 0xE046
                                                                                                                  * @desc: APP Error: VSWR Capture Retry after abort
                                                                                                                  * @maincause: Only happens in RCI mode. Mapping has changed when capture is in progress
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                  * @mainrecovtext: No action required. Cal will retry capture once the mapping is correct
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_TX_ATTEN_GET_FAILED                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 71u),  /*!< @errcode: 0xE047
                                                                                                                  * @desc: APP Error: VSWR unable to read back current Tx attenuation
                                                                                                                  * @maincause: Unexpected error returned when reading Tx attenuation from HW
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_TX_ATTEN_CHANGED                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 72u),  /*!< @errcode: 0xE048
                                                                                                                  * @desc: APP Error: Tx attenuation is changed while VSWR is capturing
                                                                                                                  * @maincause: User initiated change
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: It is mainly a warning that VSWR skips updating in the event
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_XCORR_FAIL                             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 73u),  /*!< @errcode: 0xE049
                                                                                                                  * @desc: APP Error: Failure to find forward gain or reflection using cross-correlation
                                                                                                                  * @maincause: Invalid playback signal that doesn't correlate with accumulated capture
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Reset VSWR after checking playback signal
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_FWD_PATH_UNSTABLE                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 74u),  /*!< @errcode: 0xE04A
                                                                                                                  * @desc: APP Error: Forward path delay found by VSWR has changed by unexpected amount
                                                                                                                  * @maincause: Forward path delay determined by cross correlation of accumulated capture has changed too much
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check playback signal, number of accumulation iterations, and forward path attenuation
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CMD_INVALID_OPCODE                          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 256u), /*!< @errcode: 0xE100
                                                                                                                  * @desc: APP Error: Command Opcode
                                                                                                                  * @maincause: Invalid Application Command Opcode Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Application Command Message is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CMD_INVALID                                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 257u), /*!< @errcode: 0xE101
                                                                                                                  * @desc: APP Error: Application Command
                                                                                                                  * @maincause: Invalid Application Command Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Application Command Message is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUFFER_LIMIT_EXCEEDED               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 258u), /*!< @errcode: 0xE102
                                                                                                                  * @desc: APP Error: Capture Buffer Storage
                                                                                                                  * @maincause: Capture Buffer Count Limit Reached
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Clear or Reduce Number of Stored Data Captures
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_FDD_LUT_COPY_FAILED                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 259u), /*!< @errcode: 0xE103
                                                                                                                  * @desc: APP Error: FDD LUT Copy
                                                                                                                  * @maincause: Invalid LUT Copy Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check LUT Copy Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_FAILED                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 260u), /*!< @errcode: 0xE104
                                                                                                                  * @desc: APP Error: TDD LUT Copy
                                                                                                                  * @maincause: Invalid LUT Copy Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check LUT Copy Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VBANK_BUF_NOT_ALLOCATED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 261u), /*!< @errcode: 0xE105
                                                                                                                  *   @desc: APP Error: vBank Buffers
                                                                                                                  *   @maincause: Invalid vBank Buffers Pointer(s)
                                                                                                                  *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  *   @mainrecovtext: Check vBank Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PWR_METER_READBACK_FAIL                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 262u), /*!< @errcode: 0xE106
                                                                                                                  * @desc: APP Error: DPD Power Meter Read
                                                                                                                  * @maincause: Invalid Tx Channel Parameter Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx Channel is Enabled
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NULL_PTR                                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 263u), /*!< @errcode: 0xE107
                                                                                                                  * @desc: APP Error: Capture Buffer Pointer
                                                                                                                  * @maincause: Invalid Capture Buffer Pointer
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Capture Buffer Pointer is Valid
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_ADAPTATION_BUFFER_ACQ_FAIL              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 264u), /*!< @errcode: 0xE108
                                                                                                                  * @desc: APP Error: DPD Adaptation Buffer
                                                                                                                  * @maincause: Failed to Acquire Adaptation Buffer
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check Capture Buffers are not still in use
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_CMD_NOT_SUPPORTED                      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 265u), /*!< @errcode: 0xE109
                                                                                                                  * @desc: APP Error: VSWR command not supported
                                                                                                                  * @maincause: VSWR received unsupported command
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check VSWR command data
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_INCOMPLETE_CAPTURE                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 266u), /*!< @errcode: 0xE10A
                                                                                                                  * @desc: APP Error: VSWR Capture Incomplete
                                                                                                                  * @maincause: VSWR Capture Reporting Incomplete
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check VSWR Calibration is Configured Correctly
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_CAPTURE_REQUEST_FAILED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 267u), /*!< @errcode: 0xE10B
                                                                                                                  * @desc: APP Error: VSWR Capture Request
                                                                                                                  * @maincause: Invalid VSWR Capture Request Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check VSWR Capture Request Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_EXT_CAPTURE_BUSY                            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 268u), /*!< @errcode: 0xE10C
                                                                                                                  * @desc: APP Error: Capture Buffer Busy
                                                                                                                  * @maincause: Capture Buffer is in use and cannot Respond to new Data Capture Request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Wait until Current Capture has Complete
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_ABORTED_CAPTURE                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 269u), /*!< @errcode: 0xE10D
                                                                                                                  * @desc: APP Error: VSWR Capture Abort
                                                                                                                  * @maincause: VSWR Capture Aborted
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Retry VSWR Capture
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_VBANK_BUFFER_ACQ_FAIL                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 270u), /*!< @errcode: 0xE10E
                                                                                                                  * @desc: APP Error: vBank Buffer(s) Acquire
                                                                                                                  * @maincause: VBank Buffer(s) not Available
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check that Buffer(s) are Available
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_FDD_LUT_COPY_START_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 271u), /*!< @errcode: 0xE10F
                                                                                                                  * @desc: FDD Lut Copy Start Failed
                                                                                                                  * @maincause: Unexpected LUT copy start failure
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_FDD_LUT_COPY_TIMEOUT                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 272u), /*!< @errcode: 0xE110
                                                                                                                  * @desc: FDD Lut Copy Timed Out
                                                                                                                  * @maincause: Timed out waiting for LUT copy to complete
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_FDD_LUT_COPY_EVENTGET_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 273u), /*!< @errcode: 0xE111
                                                                                                                  * @desc: FDD LUT Copy error while waiting for completion. Not timeout related.
                                                                                                                  * @maincause: Unknown error encountered while waiting for any LUT done or error event
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_FDD_LUT_COPY_SDK_DMA_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 274u), /*!< @errcode: 0xE112
                                                                                                                  * @desc: FDD LUT Copy error encountered during DMA transfer.
                                                                                                                  * @maincause: Unknown error encountered in DMA transfer.
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_START_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 275u), /*!< @errcode: 0xE113
                                                                                                                  * @desc: TDD Lut Copy Start Failed
                                                                                                                  * @maincause: Unexpected LUT copy start failure
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_TIMEOUT                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 276u), /*!< @errcode: 0xE114
                                                                                                                  * @desc: TDD Lut Copy Timed Out
                                                                                                                  * @maincause: Timed out waiting for LUT copy to complete
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_EVENTGET_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 277u), /*!< @errcode: 0xE115
                                                                                                                  * @desc: TDD LUT Copy error while waiting for completion. Not timeout related.
                                                                                                                  * @maincause: Unknown error encountered while waiting for any LUT done or error event
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 278u), /*!< @errcode: 0xE116
                                                                                                                  * @desc: TDD LUT Copy error encountered during DMA transfer.
                                                                                                                  * @maincause: Unknown error encountered in DMA transfer.
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_INPROG_TXON_ERROR      = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 279u), /*!< @errcode: 0xE117
                                                                                                                  * @desc: TDD LUT Copy was In Progress for a Tx channel during TX_ON.
                                                                                                                  * @maincause: TDD LUT Copy started too late after TX_OFF falling edge to be finished before TX_ON rising edge.
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset the feature to recover from this error. if problem persists contact ADI.
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TX_POWER_MEAS_REQUEST_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 280u), /*!< @errcode: 0xE118
                                                                                                                  * @desc: APP Error: Tx Power measurement Request
                                                                                                                  * @maincause: Invalid Tx Power measurement Request Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx Power Measurement Request Parameters are valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TX_POWER_MEAS_UNEXPECTED_CAP_TYPE           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 281u), /*!< @errcode: 0xE119
                                                                                                                  * @desc: APP Error: Unexpected capture type received in capture done event
                                                                                                                  * @maincause: Unexpected capture type received in capture done event
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_POWER_MEAS_ABORTED                          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 282u), /*!< @errcode: 0xE11A
                                                                                                                  * @desc: APP Error: Tx power measurement is aborted
                                                                                                                  * @maincause: Power measurement didn't complete within RS pattern
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Adjust pattern timing or power measurement duration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_RESULT_OVERWRITE                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 283u), /*!< @errcode: 0xE11B
                                                                                                                  * @desc: APP Error: Application consumed Capture Done Event Payload too late
                                                                                                                  * @maincause: SDK Cap Service overwrote Capture Done Event Payload before it was received and consumed by App
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Adjust configuration and/or timing in application to prevent this SDK overwrite.
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_LINEAR_TERM_NOT_MATCHED_M_TABLE   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 284u), /*!< @errcode: 0xE11C
                                                                                                                  * @desc: APP Error: LUT linear term of all tables must equal LUT linear term of Table 0
                                                                                                                  * @maincause: LUT linear term of table not equal to  LUT linear term of Table 0
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Change LUT linear term of table
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_SEM_WAIT_FAILED                             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 285u), /*!< @errcode: 0xE11D
                                                                                                                  * @desc: APP Error: Sempahore wait request failed
                                                                                                                  * @maincause: The call to sempahore wait API failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_SEM_POST_FAILED                             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 286u), /*!< @errcode: 0xE11E
                                                                                                                  * @desc: APP Error: Sempahore post request failed
                                                                                                                  * @maincause: The call to sempahore post API failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_POST_CAL_CLEANUP_FAILED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 287u), /*!< @errcode: 0xE11F
                                                                                                                  * @desc: APP Error: The post calibration cleanup failed
                                                                                                                  * @maincause: The call to perform post calibration cleanup failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_REQUEST_FAILED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 290u), /*!< @errcode: 0xE122
                                                                                                                  * @desc: APP Error: CLGC Capture Request
                                                                                                                  * @maincause: Invalid CLGC Capture Request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check CLGC capture configuration parameters
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_RESULT_GET_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 291u), /*!< @errcode: 0xE123
                                                                                                                  * @desc: APP Error: CLGC Capture Result Get
                                                                                                                  * @maincause: Service error when reading back power meter result
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */


    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_SIGNAL_LOW_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 292u), /*!< @errcode: 0xE124
                                                                                                                  * @desc: APP Error: Tx or ORx power is below threshold set
                                                                                                                  * @maincause: Power measurement cannot find healthy signal after max retries
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx or ORx signal level and capture thresholds
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_STATUS_GET_FUNC_NOT_SUPPORTED          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 293u), /*!< @errcode: 0xE125
                                                                                                                  * @desc: APP Error: Unsupported CLGC Status Get
                                                                                                                  * @maincause: Status Get function not Supported for Provided Tracking Calibration Status Type
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tracking Calibration Status Type has a Supported Status Get Feature
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_STATUS_GET_INSUFFICIENT_MEMORY         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 294u), /*!< @errcode: 0xE126
                                                                                                                  * @desc: APP Error: CLGC Status Buffer Size
                                                                                                                  * @maincause: Invalid Status Buffer Size for the Calibration Status Type Requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Status Size for the Calibration Status Type Requested
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CTRL_FUNC_NOT_SUPPORTED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 295u), /*!< @errcode: 0xE127
                                                                                                                  * @desc: APP Error: CLGC Control Command
                                                                                                                  * @maincause: Invalid Control Command Provided
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Control Command is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CTRL_GET_INSUFFICIENT_MEMORY           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 296u), /*!< @errcode: 0xE128
                                                                                                                  * @desc: APP Error: CLGC CTRL command Buffer Size
                                                                                                                  * @maincause: Invalid CTRL command Buffer Size for the Calibration CTRL Type Requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check CTRL command Size for the Calibration CTRL Type Requested
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_PA_PROTECTION_ASSERTED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 297u), /*!< @errcode: 0xE129
                                                                                                                  * @desc: APP Error: CLGC detects PA protection assertion
                                                                                                                  * @maincause: PA protection error asserted or Tx attenuation ramp-up/down in progress
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check PA protection thresholds or Tx signal level
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_ABOVE_MAX_LIMIT               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 298u), /*!< @errcode: 0xE12A
                                                                                                                  * @desc: APP Error: CLGC Tx Attenuation Above Maximum Limit
                                                                                                                  * @maincause: CLGC failed to ajust Tx attenuation because it is above maximum limit
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx atten limits or/and expected loop gain configured
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_BELOW_MIN_LIMIT               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 299u), /*!< @errcode: 0xE12B
                                                                                                                  * @desc: APP Error: CLGC Tx Attenuation Below Minimum Limit
                                                                                                                  * @maincause: CLGC failed to ajust Tx attenuation because it is below minimum limit
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Tx atten limits or/and expected loop gain configured
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_EVENT_UNEXPECTED               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 300u), /*!< @errcode: 0xE12C
                                                                                                                  * @desc: APP Error: Unexpected capture event received in capture state machine
                                                                                                                  * @maincause: Unexpected capture type received in capture state machine
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_SET_FAILED                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 301u), /*!< @errcode: 0xE12D
                                                                                                                  * @desc: APP Error: CLGC unable to set Tx attenuation
                                                                                                                  * @maincause: Unexpected error returned when writing Tx attenuation to HW
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_GET_FAILED                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 302u), /*!< @errcode: 0xE12E
                                                                                                                  * @desc: APP Error: CLGC unable to read back current Tx attenuation
                                                                                                                  * @maincause: Unexpected error returned when reading Tx attenuation from HW
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_ORX_ATTEN_GET_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 303u), /*!< @errcode: 0xE12F
                                                                                                                  * @desc: APP Error: CLGC unable to read back current ORx attenuation
                                                                                                                  * @maincause: Unexpected error returned when reading ORx attenuation from HW
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_ABORT_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 304u), /*!< @errcode: 0xE130
                                                                                                                  * @desc: APP Error: CLGC Capture Abort
                                                                                                                  * @maincause: Service error when aborting a CLGC capture
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_DISCARD_FAILED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 305u), /*!< @errcode: 0xE131
                                                                                                                  * @desc: APP Error: CLGC Capture Discard
                                                                                                                  * @maincause: Service error when discarding a CLGC capture
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_EVENT_REGISTER_FAILED                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 306u), /*!< @errcode: 0xE132
                                                                                                                  * @desc: APP Error: Capture Event Registration
                                                                                                                  * @maincause: OSAL failed to Register a Capture Event
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_START_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 307u), /*!< @errcode: 0xE133
                                                                                                                  * @desc: APP Error: Capture Start Event
                                                                                                                  * @maincause: Capture start error occured
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_ABORTED_CAPTURE                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 308u), /*!< @errcode: 0xE134
                                                                                                                  * @desc: APP Error: Capture Aborted
                                                                                                                  * @maincause: capture aborted due to EXIT request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_INCOMPLETE_CAPTURE                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 309u), /*!< @errcode: 0xE135
                                                                                                                  * @desc: APP Error: Incomplete Capture
                                                                                                                  * @maincause: DPD capture incomplete in CLGC peak detection mode
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ORX_MAP_INVALID                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 310u), /*!< @errcode: 0xE136
                                                                                                                  * @desc: APP Error: A Tx Channel Not Mapped to an ORx Channel
                                                                                                                  * @maincause: Tx-ORx map not set
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check and set Tx to ORx map
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_UNEXPECTED_CAPTURE_PERIOD_END_SIGNAL        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 311u), /*!< @errcode: 0xE137
                                                                                                                  * @desc: APP Error: Unexpected capture period end signal
                                                                                                                  * @maincause: Unexpected capture period end signal
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Contact ADI
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_RX_CAPTURE_REQUEST_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 312u), /*!< @errcode: 0xE138
                                                                                                                  * @desc: APP Error: Rx Capture Request failed
                                                                                                                  * @maincause: Invalid Rx Capture Request Parameters
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Rx Capture Request Parameters are Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_BAD_CAPTURE_DATA                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 313u), /*!< @errcode: 0xE139
                                                                                                                  * @desc: APP Error: bad data captured
                                                                                                                  * @maincause: Bad waveform or incompleted capture
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check waveform
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TX_ATTEN_CHANGED                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 314u), /*!< @errcode: 0xE13A
                                                                                                                  * @desc: APP Error: Tx attenuation is changed while CLGC is tracking
                                                                                                                  * @maincause: User initiated change
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: It is mainly a warning that CLGC skips updating in the event
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_ACCUM_CAPTURE_OVERFLOW                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 315u), /*!< @errcode: 0xE13B
                                                                                                                  * @desc: APP Error: VSWR Accumulating capture overflow
                                                                                                                  * @maincause: I or Q values in accumulated capture overflowed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check VSWR playback signal and capture configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CAPTURE_DISCARD_FAILED                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 316u), /*!< @errcode: 0xE13C
                                                                                                                  * @desc: APP Error: DPD Capture Discard
                                                                                                                  * @maincause: Service error when discarding a DPD capture
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CAPTURE_RETRY_AFTER_ABORT               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 317u), /*!< @errcode: 0xE13D
                                                                                                                  * @desc: APP Error: DPD Capture Retry after abort
                                                                                                                  * @maincause: Only happens in RCI mode. Mapping has changed when capture is in progress
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                  * @mainrecovtext: No action required. Cal will retry capture once the mapping is correct
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_CAPTURE_RETRY_AFTER_ABORT              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 318u), /*!< @errcode: 0xE13E
                                                                                                                  * @desc: APP Error: CLGC Capture Retry after abort
                                                                                                                  * @maincause: Only happens in RCI mode. Mapping has changed when capture is in progress
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                  * @mainrecovtext: No action required. Cal will retry capture once the mapping is correct
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DISABLE_ACTUATOR_PASSTHRU_FAILURE           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 319u), /*!< @errcode: 0xE13F
                                                                                                                  * @desc: APP Error: fail to disable actuator passthru
                                                                                                                  * @maincause: invalid param or fail to get dpd act driver mutex
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check parameter
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CFG_SYSTEM_CONFIG_LOCKED                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 320u), /*!< @errcode: 0xE140
                                                                                                                  * @desc: APP Error: App Configuration is Locked
                                                                                                                  * @maincause: Configuration has not been Unlocked
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Unlock Configuration before trying again
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_SWITCH_MUTEX_REQ_FAILED           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 321u), /*!< @errcode: 0xE141
                                                                                                                  * @desc: APP Error: Model switch mutex req failed
                                                                                                                  * @maincause: Mutex Acquire/Release Failed for DPD model switching
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */


    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_SWITCH_INVALID_PARAM              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 322u), /*!< @errcode: 0xE142
                                                                                                                  * @desc: APP Error: DPD model switch function is called with invalid parameter
                                                                                                                  * @maincause: DPD model switch function is called with invalid parameter
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_SWITCH_FAILURE                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 323u), /*!< @errcode: 0xE143
                                                                                                                  * @desc: APP Error: DPD model switch failed to switch model
                                                                                                                  * @maincause: DPD model has not been switching as requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: CRITICAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_MODEL_SWITCH_POWER_METER_FAILURE        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 324u), /*!< @errcode: 0xE144
                                                                                                                  * @desc: APP Error: DPD model switch power meter thread has failed
                                                                                                                  * @maincause: DPD models are not switching based on power meter measurements
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity: FATAL
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_GAIN_MONITOR_ASSERTED                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 325u), /*!< @errcode: 0xE145
                                                                                                                  * @desc: APP Error: CLGC update is skipped because DPD actuator gain monitor has asserted
                                                                                                                  * @maincause: DPD actuator gain outside set limits
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: It is mainly a warning that CLGC skips updating in the event
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_GAIN_MONITOR_FAILURE                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 326u), /*!< @errcode: 0xE146
                                                                                                                  * @desc: APP Error: CLGC update is skipped because DPD actuator gain monitor status could not be determined
                                                                                                                  * @maincause: DPD gain monitor status check has failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */


    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_SEQ_CAPTURE_REQ_PENDING             = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 327u), /*!< @errcode: 0xE147
                                                                                                                  * @desc: APP Error: There is already a pending capture period request
                                                                                                                  * @maincause: Cal thread is making a new capture period request before previous one has been processed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_PATH_DELAY_OVERRANGE                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 328u), /*!< @errcode: 0xE148
                                                                                                                  * @desc: APP Error: DPD integer Path Delay estimation is out of lag range (-255, 0)
                                                                                                                  * @maincause: DPD Path Delay Estimation Failure
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_GAIN_MONITOR_ASSERTED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 329u), /*!< @errcode: 0xE149
                                                                                                                  * @desc: APP Error: DPD update aborted because DPD actuator gain monitor has asserted
                                                                                                                  * @maincause: DPD actuator gain outside set limits
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TSSIORX_MEASURE_ERROR                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 330u), /*!< @errcode: 0xE14A
                                                                                                                  * @desc: APP Error: CLGC TSSI ORx meter failed to update
                                                                                                                  * @maincause: The call to perform TSSI ORx measurements failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TSSI1_MEASURE_ERROR                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 331u), /*!< @errcode: 0xE14B
                                                                                                                  * @desc: APP Error: CLGC TSSI1 meter failed to update
                                                                                                                  * @maincause: The call to perform TSSI1 measurements failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CLGC_TSSI2_MEASURE_ERROR                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 332u), /*!< @errcode: 0xE14C
                                                                                                                  * @desc: APP Error: CLGC TSSI2 meter failed to update
                                                                                                                  * @maincause: The call to perform TSSI2 measurements failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Reset DFE and try again. Contact ADI if the Problem Persists
                                                                                                                  * @severity:
                                                                                                                  */


    ADI_ADRV904X_DFE_APP_ERR_CODE_UNEXPECTED_CAPTURE_EVENT                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 333u), /*!< @errcode: 0xE14D
                                                                                                                  * @desc: APP Error: Unexpected event when performing captures
                                                                                                                  * @maincause: Unexpected event when performing captures
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_UNEXPECTED_CAPTURE_ID                       = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 334u), /*!< @errcode: 0xE14E
                                                                                                                  * @desc: APP Error: Unexpected capture id when performing captures
                                                                                                                  * @maincause: Unexpected capture id when performing captures
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_BUF_NO_INSTANCE_ERROR               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 335u), /*!< @errcode: 0xE14F
                                                                                                                  * @desc: APP Error: Cal thread couldn't acquire capture buffer for multiple times
                                                                                                                  * @maincause: Capture buffers are in use by some other calibration threads
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump if problem persists
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_INCOMPLETE_ERROR                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 336u), /*!< @errcode: 0xE150
                                                                                                                  * @desc: APP Error: Capture was incomplete
                                                                                                                  * @maincause: HW couldn't capture all samples or there were 0 samples in dataset
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check transmit data and TDD settings
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_ABORT_ERROR                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 337u), /*!< @errcode: 0xE151
                                                                                                                  * @desc: APP Error: Capture was aborted
                                                                                                                  * @maincause: Capture is aborted due to mapping change or user abort request
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check mapping settings (RCI mode) or capture period length(RS mode)
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_UNEXPECTED_CAPTURE_COUNT                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 338u), /*!< @errcode: 0xE152
                                                                                                                  * @desc: APP Error: Unexpected capture count in capture flow
                                                                                                                  * @maincause: Unexpected capture count in capture flow
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_UNEXPECTED_CAPTURE_PERIOD_END_EVENT         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 339u), /*!< @errcode: 0xE153
                                                                                                                  * @desc: APP Error: Unexpected capture period end event in capture flow
                                                                                                                  * @maincause: Unexpected capture period end event in capture flow
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_PERIOD_ENDED_BEFORE_CAPTURES        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 340u), /*!< @errcode: 0xE154
                                                                                                                  * @desc: APP Error: Capture period ended before all captures complete
                                                                                                                  * @maincause: Capture period might be too short or captures taking longer than expected
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check capture period, capture parameters and transmit waveform
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_MAX_NUMBER_OF_CAPTURES_ERROR                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 341u), /*!< @errcode: 0xE155
                                                                                                                  * @desc: APP Error: Application layer requested requested more than maximum number of captures
                                                                                                                  * @maincause: Application layer requested requested more than maximum number of captures
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_STORE_UNKNOWN_ERROR                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 342u), /*!< @errcode: 0xE156
                                                                                                                  * @desc: APP Error: Capture result reports an unexpected error
                                                                                                                  * @maincause: Capture result reports an unexpected error
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_INVALID_CAPTURE_TYPE                        = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 343u), /*!< @errcode: 0xE157
                                                                                                                  * @desc: APP Error: Invalid capture type selected by application thread
                                                                                                                  * @maincause: Invalid capture type is selected by application thread
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAPTURE_SW_TRIGGER_FAILED                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 344u), /*!< @errcode: 0xE158
                                                                                                                  * @desc: APP Error: Capture trigger in SW mode has failed
                                                                                                                  * @maincause: Capture trigger in SW mode has failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_NULL_PTR                                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 345u), /*!< @errcode: 0xE159
                                                                                                                  * @desc: APP Error: Null pointer
                                                                                                                  * @maincause: Invalid Pointer
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT64_FDD                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 346u), /*!< @errcode: 0xE15A
                                                                                                                  * @desc: APP Error: DPD not supported with LUT depth 64 in FDD mode
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT64_MODE2               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 347u), /*!< @errcode: 0xE15B
                                                                                                                  * @desc: APP Error: DPD update mode 2 not supported with LUT depth 64
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT64_R                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 348u), /*!< @errcode: 0xE15C
                                                                                                                  * @desc: APP Error: Only unity gain (Model 4) can be used as low-power/gain-mon recovery model with LUT depth 64
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_TDD_LUTSW                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 349u), /*!< @errcode: 0xE15D
                                                                                                                  * @desc: APP Error: TDD LUT switching is only supported at LUT depth 16
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT32_FDD_MODE2           = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 350u), /*!< @errcode: 0xE15E
                                                                                                                  * @desc: APP Error: DPD Update mode 2 not supported in FDD mode with LUT depth 32
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT32_FDD_R               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 351u), /*!< @errcode: 0xE15F
                                                                                                                  * @desc: APP Error: Only unity gain (Model 4) can be used as low-power/gain-mon recovery model with LUT depth 32 in FDD mode
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_LUT32_MODEL2              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 352u), /*!< @errcode: 0xE160
                                                                                                                  * @desc: APP Error: Model 2 cannot be used for low-power/gain-mon recovery with LUT depth 32
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_MODE2_MODEL1              = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 353u), /*!< @errcode: 0xE161
                                                                                                                  * @desc: APP Error: Model 1 cannot be used for low-power/gain-mon recovery with DPD update mode 2
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_NOT_SUPPORTED_FDD_LUTSW                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 354u), /*!< @errcode: 0xE162
                                                                                                                  * @desc: APP Error: TDD LUT switching is not supported in FDD mode
                                                                                                                  * @maincause: Invalid DPD configuration requested
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check DPD configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAP_SEQ_CFG_INVALID_PARAM                   = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 355u), /*!< @errcode: 0xE163
                                                                                                                  * @desc: APP Error: Cap sequencer config out of range
                                                                                                                  * @maincause: Invalid configuration
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check capture sequencer config parameter
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CAP_SEQ_CFG_INVALID_MODE                    = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 356u), /*!< @errcode: 0xE164
                                                                                                                  * @desc: APP Error: Cap sequencer command not supported in this mode
                                                                                                                  * @maincause: Invalid configuration
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check mapping operation mode, RCI/RS/Dfe mapping control
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_CMD_INSUFFICIENT_MEMORY                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 357u), /*!< @errcode: 0xE165
                                                                                                                  * @desc: APP Error: Not enough memory to respond to VSWR command
                                                                                                                  * @maincause: Invalid buffer size passed to VSWR command handler
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check VSWR command data
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_TX_WAVEFORM_READ_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 358u), /*!< @errcode: 0xE166
                                                                                                                  * @desc: APP Error: VSWR Waveform get failed
                                                                                                                  * @maincause: Vswr generator might be enabled or waveform length is invalid
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check VSWR waveform length
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_VSWR_TX_WAVEFORM_NOT_LOADED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 359u), /*!< @errcode: 0xE167
                                                                                                                  * @desc: APP Error: VSWR Waveform length is 0
                                                                                                                  * @maincause: Waveform is not loaded
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Load VSWR waveform before enabling cal
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_STOP_CRITERIA_NOT_MET_ERROR                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 360u), /*!< @errcode: 0xE168
                                                                                                                  * @desc: APP Error: Stop Criteria not met
                                                                                                                  * @maincause: Stop Criteria in Pass On Stop not met
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check if the stopping criteria is set properly
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_SIG_STATS_GEN_ERROR                         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 361u), /*!< @errcode: 0xE169
                                                                                                                  * @desc: APP Error: Signal Stats Generation Error
                                                                                                                  * @maincause: Capture Configuration
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check if capture engine is configured correctly
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TARGET_VALS_NOT_AVAIL_ERROR                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 362u), /*!< @errcode: 0xE16A
                                                                                                                  * @desc: APP Error: Target values were not populated
                                                                                                                  * @maincause: Capture configuration
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check if capture engine is configured correctly
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CTC1_CAPTURE_WEIGHT_NOT_VALID_ERROR         = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 363u), /*!< @errcode: 0xE16B
                                                                                                                  * @desc: APP Error: Capture weights in ctc1 mode are not valid
                                                                                                                  * @maincause: Capture configuration
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check if capture weights in ctc1 mode are configured correctly
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_DPD_CT_FLUT_BUFFER_ACQ_FAIL                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 364u), /*!< @errcode: 0xE16C
                                                                                                                  * @desc: APP Error: CT FLUT buffer acquire failed
                                                                                                                  * @maincause: CT FLUT Buffer not Available
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                  * @mainrecovtext: Check that if buffers are available
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_FLUT_COPY_FAILED                     = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 365u), /*!< @errcode: 0xE16D
                                                                                                                  * @desc: APP Error: CTC FLUT Copy
                                                                                                                  * @maincause: LUT copy timedout or DMA error reported
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_MODEL_CONFIG_FAILED                  = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 366u), /*!< @errcode: 0xE16E
                                                                                                                  * @desc: APP Error: CTC Model config failed
                                                                                                                  * @maincause: Invalid configuration or out of range parameter
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check CTC model configuration
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_ACT_DELAY_GET_FAILED                 = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 367u), /*!< @errcode: 0xE16F
                                                                                                                  * @desc: APP Error: CTC Act delay get failed
                                                                                                                  * @maincause: CTC driver mutex lock might have failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_ACT_FBOX_SCALE_GET_FAILED            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 368u), /*!< @errcode: 0xE170
                                                                                                                  * @desc: APP Error: CTC Act Fbox scaler get failed
                                                                                                                  * @maincause: CTC driver mutex lock might have failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_ACTUATOR_ENABLE_FAILED               = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 369u), /*!< @errcode: 0xE171
                                                                                                                  * @desc: APP Error: CTC Actuator enable failed
                                                                                                                  * @maincause: CTC driver mutex lock might have failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_ACTUATOR_RESET_FAILED                = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 370u), /*!< @errcode: 0xE172
                                                                                                                  * @desc: APP Error: CTC Actuator reset failed
                                                                                                                  * @maincause: CTC driver mutex lock might have failed
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_FEATURE
                                                                                                                  * @mainrecovtext: Contact ADI with memdump
                                                                                                                  * @severity:
                                                                                                                  */
    ADI_ADRV904X_DFE_APP_ERR_CODE_CT_DPD_ACT_FBOX_SCALE_SET_FAILED            = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 371u), /*!< @errcode: 0xE173
                                                                                                                  * @desc: APP Error: CTC Act Fbox scaler set failed
                                                                                                                  * @maincause: Fbox scaler is smaller than model config requires
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Increase fbox scaler value
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_APP_ERR_CODE_TDD_LUT_COPY_SDK_DMA_NOT_TRIGGERED          = (ADI_ADRV904X_DFE_APP_ERR_COMMON_ERROR_CODE_START + 372u), /*!< @errcode: 0xE174
                                                                                                                  * @desc: APP Error: Lut update DMA request cannot be implemented
                                                                                                                  * @maincause: Lack of TX falling edge
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Enable TX channel or shorten the TX on time
                                                                                                                  * @severity:
                                                                                                                  */


#ifndef ADI_ADRV904X_DFE_SVC_CMN_FW
    /* Pseudo error code */
    ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR = (int32_t) 0x80000000                                       /* Remove UL for API to avoid ISO C error. set to large number to force enum to be uint32_t */
#else
    /* Pseudo error code */
    ADI_ADRV904X_DFE_APP_ERR_CODE_CPU_CAL_EXIT_OCCURRED_ERROR = 0x80000000UL                                               /* Add UL for FW to force it to be unsigned int. set to large number to force enum to be uint32_t */
#endif
} adi_adrv904x_DfeAppErrCode_e;

#endif /* __ADI_ADRV904X_DFE_APP_ERROR_CODES_T_H__ */


