/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_err_codes_t.h
 *
 * \brief   Contains DFE error code definitions
 *
 * \details Contains DFE error code definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADI_ADRV904X_DFE_SVC_ERROR_CODES_T_H__
#define __ADI_ADRV904X_DFE_SVC_ERROR_CODES_T_H__

#include "adi_adrv904x_dfe_svc_err_object_ids_t.h"


#define ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START                     (ADI_ADRV904X_DFE_SVC_ERR_OBJID_COMMON << 8u)              /*!< 0x0000 - Starting common error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CMD << 8u)             /*!< 0x0100 - Starting Command handler service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START                   (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_OSAL << 8u)            /*!< 0x0200 - Starting OSAL, event error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_LOG_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_LOG << 8u)             /*!< 0x0300 - Starting Logging error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_DFE << 8u)             /*!< 0x0400 - Starting DFE error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_TIMER_ERROR_CODE_START                  (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_TIMER << 8u)           /*!< 0x0500 - Starting Timer error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START             (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_RADIO_CTRL << 8u)      /*!< 0x0600 - Starting Radio Control error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START                  (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_EVENT << 8u)           /*!< 0x0700 - Starting Event Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_CLI_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CLI << 8u)             /*!< 0x0800 - Starting CLI Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START                 (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_MALLOC << 8u)          /*!< 0x0900 - Starting Malloc Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_BBIC_BRIDGE_ERROR_CODE_START            (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BBIC_BRIDGE << 8u)     /*!< 0x0A00 - Starting BBIC Bridge Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START               (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BUF_MGMT << 8u)        /*!< 0x0B00 - Starting Memory Buffer Mgmt Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CFG << 8u)             /*!< 0x0C00 - Starting Configuration Adj Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_ECC_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_ECC << 8u)             /*!< 0x0F00 - Starting ECC Service error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START                (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_WATCHDOG << 8u)        /*!< 0x1000 - Starting Watchdog error code */
#define ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START                         (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_RADIO_CMD << 8u)       /*!< 0x1100 - Starting Radio Command Service */

#define ADI_ADRV904X_DFE_SVC_ERR_SVC_DBG_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_DBG << 8u)             /*!< 0x2000 - Starting Debug Mode Service error code */

#define ADI_ADRV904X_DFE_SVC_ERR_DRV_GPIO_ERROR_CODE_START                   (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPIO << 8u)            /*!< 0x3000 - Starting GPIO Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPT << 8u)             /*!< 0x3100 - Starting GPT Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_INTERRUPT << 8u)       /*!< 0x3200 - Starting Interrupt Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START                   (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_UART << 8u)            /*!< 0x3300 - Starting UART Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START                     (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_MB << 8u)              /*!< 0x3400 - Starting Mailbox error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_SYSCLK_ERROR_CODE_START                 (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_SYSCLK << 8u)          /*!< 0x3600 - Starting SYSCLOCK Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START                 (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_STREAM << 8u)          /*!< 0x3700 - Starting STREAM Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ERROR_CODE_START                (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_PWR_MTR << 8u)         /*!< 0x3800 - Starting Power Meter Driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DMA << 8u)             /*!< 0x3900 - Starting DMA driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_ERROR_CODE_START                (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_CAPTURE << 8u)         /*!< 0x3A00 - Starting Capture driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_HW_IF_ERROR_CODE_START                  (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_HW_IF << 8u)           /*!< 0x3B00 - Starting Hw If driver error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_ERROR_CODE_START                    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DFE << 8u)             /*!< 0x3C00 - Starting DFE driver error code */

#define ADI_ADRV904X_DFE_SVC_ERR_APP_ERROR_CODE_START                        (ADI_ADRV904X_DFE_SVC_ERR_OBJID_APP_ERROR << 8u)           /*!< 0xE000 - Starting Application error code */

#define ADI_ADRV904X_DFE_SVC_ERR_DFE_CFG_DEVICE_PROFILE_ERROR_CODE_START     (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CFG_DEVICE_PROFILE << 8u)  /*!< 0x8000 - Starting Device Profile error code */
#define ADI_ADRV904X_DFE_SVC_ERR_DFE_CFG_RADIO_SEQUENCER_ERROR_CODE_START    (ADI_ADRV904X_DFE_SVC_ERR_OBJID_CFG_RADIO_SEQUENCER << 8u) /*!< 0x8100 - Starting Radio Sequencer error code */


/**
 * \brief DFE firmware error codes
 */
typedef enum adi_adrv904x_DfeSvcErrCode
{
    ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR                      = 0UL,                                                          /*!< @errcode: 0x0000
                                                                                                                    * @desc: No Error
                                                                                                                    * @maincause: Operation was Successful
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_NONE
                                                                                                                    * @mainrecovtext: No Action Required
                                                                                                                    * @severity: INFO
                                                                                                                    */


    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_COMMON Section Base Error Code = 0x0000 ------                            @errcode: 0x0000
     *                                                                                                               @desc: Common Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_INVALID_PARAM                 = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 1u),                   /*!< @errcode: 0x0001
                                                                                                                    * @desc: Common Error: Parameter(s)
                                                                                                                    * @maincause: Invalid Parameter(s) Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Parameter(s) Provided
                                                                                                                    * @severity: CRITICAL, FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_NULL_PTR                      = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 2u),                   /*!< @errcode: 0x0002
                                                                                                                    * @desc: Common Error: Pointer(s)
                                                                                                                    * @maincause: Invalid Pointer(s) Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Pointer(s) Provided
                                                                                                                    * @severity: CRITICAL, FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_FEATURE_NOT_IMPLEMENTED       = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 3u),                   /*!< @errcode: 0x0003
                                                                                                                    * @desc: Common Error: Feature Implementation
                                                                                                                    * @maincause: Requested Feature has not been Implemented
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check that Feature has been Implemented
                                                                                                                    * @severity: INFO
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CRC_CHECK_FAILURE             = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 4u),                   /*!< @errcode: 0x0004
                                                                                                                    * @desc: Common Error: CRC Check
                                                                                                                    * @maincause: Computed CRC does not Match Provided CRC
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Validate Image being Checked & Reprogram Device
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INSUFFICIENT_MEMORY           = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 5u),                   /*!< @errcode: 0x0005
                                                                                                                    * @desc: Common Error: Insufficient Memory
                                                                                                                    * @maincause: Unable to Allocate requested Memory (i.e. Heap)
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Reprogram Device
                                                                                                                    * @severity: CRITICAL
                                                                                                                    */


    ADI_ADRV904X_DFE_SVC_ERR_CODE_STACK_OVERFLOW                = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 6u),                   /*!< @errcode: 0x0006
                                                                                                                    * @desc: Common Error: Stack Overflow
                                                                                                                    * @maincause: Allocated Stack Size has been Exhausted
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Reprogram Device
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_FATAL_EXCEPTION               = (ADI_ADRV904X_DFE_SVC_ERR_COMMON_ERROR_CODE_START + 7u),                   /*!< @errcode: 0x0007
                                                                                                                    * @desc: Common Error: Fatal Exception
                                                                                                                    * @maincause: Fatal Exception has been Triggered by CPU
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Reprogram Device
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CMD  Section Base Error Code = 0x0100 ------                          @errcode: 0x0100
     *                                                                                                               @desc: Command Interface Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_BAD_TID               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 1u),                  /*!< @errcode: 0x0101
                                                                                                                    * @desc: Command Error: Transaction ID
                                                                                                                    * @maincause: Invalid Transaction ID Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Transaction ID is Valid
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_LINK_BUSY             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 2u),                  /*!< @errcode: 0x0102
                                                                                                                    * @desc: Command Error: Link Busy
                                                                                                                    * @maincause: Link is Currently Processing a Command
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE
                                                                                                                    * @mainrecovtext: Wait for Link to Complete current Command
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_INVALID_LINK_ID       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 3u),                  /*!< @errcode: 0x0103
                                                                                                                    * @desc: Command Error: Link Number
                                                                                                                    * @maincause: Invalid Link Number Provided in Command
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Valid Link Number is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_INVALID_MAILBOX       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 4u),                  /*!< @errcode: 0x0104
                                                                                                                    * @desc: Command Error: Mailbox Number
                                                                                                                    * @maincause: Invalid Mailbox Number Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Mailbox Number is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_MSG_TOO_LARGE         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 5u),                  /*!< @errcode: 0x0105
                                                                                                                    * @desc: Command Error: Command Payload Size
                                                                                                                    * @maincause: Payload Size Exceeds Link Buffer Size
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Valid Payload Size is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_BAD_CMD_ID            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 6u),                  /*!< @errcode: 0x0106
                                                                                                                    * @desc: Command Error: Command ID
                                                                                                                    * @maincause: Invalid Command ID Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Valid Command ID is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_BAD_EVENT_TYPE        = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 7u),                  /*!< @errcode: 0x0107
                                                                                                                    * @desc: Command Error: Event Type
                                                                                                                    * @maincause: Invalid Event Type Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Event Type is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_BAD_PARAM             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 8u),                  /*!< @errcode: 0x0108
                                                                                                                    * @desc: Command Error: Parameter(s)
                                                                                                                    * @maincause: Invalid Parameter(s) Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Parameter(s) is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_BAD_LINK_BUFFER       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 9u),                  /*!< @errcode: 0x0109
                                                                                                                    * @desc: Command Error: link buffer is NULL
                                                                                                                    * @maincause: Invalid Parameter(s) Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Parameter(s) is Valid
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_CMD_TIMEOUT               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CMD_ERROR_CODE_START + 10u),                 /*!< @errcode: 0x010a
                                                                                                                    * @desc: Command Error: Mailbox command timed out
                                                                                                                    * @maincause: Potential timing issue with calibration threads
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Contact ADI with memdump
                                                                                                                    * @severity: FATAL
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START Section Base Error Code = 0x0200 ------               @errcode: 0x0200
     *                                                                                                               @desc: OSAL Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_DELETED                  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x01u),          /*!< @errcode: 0x0201
                                                                                                                * @desc: OSAL Service Error: Memory Pool
                                                                                                                * @maincause: Memory Pool was Deleted while Thread Suspended
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Feature is reporting an Error
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_POOL_ERROR               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x02u),          /*!< @errcode: 0x0202
                                                                                                                * @desc: OSAL Service Error: Memory Pool Pointer
                                                                                                                * @maincause: Invalid Memory Pool Pointer Provided by Thread
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                * @mainrecovtext: Reprogram Device
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_PTR_ERROR                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x03u),          /*!< @errcode: 0x0203
                                                                                                                * @desc: OSAL Service Error: Invalid Pointer
                                                                                                                * @maincause: NULL Pointer Detected
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Use Valid Pointer
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_WAIT_ERROR               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x04u),          /*!< @errcode: 0x0204
                                                                                                                * @desc: OSAL Service Error: Semaphore Wait
                                                                                                                * @maincause: OSAL Semaphore Wait Timeout
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check that Semaphore is not being requested from ISR
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_SIZE_ERROR               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x05u),          /*!< @errcode: 0x0205
                                                                                                                * @desc: OSAL Service Error: Memory Pool Size
                                                                                                                * @maincause: Invalid Memory Pool Size
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check the Memory Pool Size is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_GROUP_ERROR              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x06u),          /*!< @errcode: 0x0206
                                                                                                                * @desc: OSAL Service Error: Event Group Pointer
                                                                                                                * @maincause: Event Group Pointer is either NULL or Points to a Group already Created
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Event Group Pointer Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NO_EVENTS                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x07u),          /*!< @errcode: 0x0207
                                                                                                                * @desc: OSAL Service Error: No Event
                                                                                                                * @maincause: Service was unable to get the Specified Events
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Events were Created
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_OPTION_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x08u),          /*!< @errcode: 0x0208
                                                                                                                * @desc: OSAL Service Error: Option
                                                                                                                * @maincause: Invalid Get Option Specified
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Invalid Parameter Passed to Function
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_QUEUE_ERROR              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x09u),          /*!< @errcode: 0x0209
                                                                                                                * @desc: OSAL Service Error: Queue Pointer
                                                                                                                * @maincause: Queue Pointer was either NULL or Pointing to a Queue that was already Created
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Queue Pointer is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_QUEUE_EMPTY              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0au),          /*!< @errcode: 0x020a
                                                                                                                * @desc: OSAL Service Error: Queue Empty
                                                                                                                * @maincause: Message Queue is Empty
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Queue Management
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_QUEUE_FULL               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0bu),          /*!< @errcode: 0x020b
                                                                                                                * @desc: OSAL Service Error: Queue Full
                                                                                                                * @maincause: Message Queue is Full
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Queue Management
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_SEMAPHORE_ERROR          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0cu),          /*!< @errcode: 0x020c
                                                                                                                * @desc: OSAL Service Error: Semaphore Pointer
                                                                                                                * @maincause: Semaphore Pointer was either NULL or Pointing to a Semaphore that was already Created
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Semaphore being used is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NO_INSTANCE              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0du),          /*!< @errcode: 0x020d
                                                                                                                * @desc: OSAL Service Error: No Semaphore
                                                                                                                * @maincause: Semaphore Count is Zero
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Semaphore Management
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_THREAD_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0eu),          /*!< @errcode: 0x020e
                                                                                                                * @desc: OSAL Service Error: Thread Pointer
                                                                                                                * @maincause: Thread Pointer was either NULL or Points to a Thread that was already Created
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Pointer is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_PRIORITY_ERROR           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x0fu),          /*!< @errcode: 0x020f
                                                                                                                * @desc: OSAL Service Error: Thread Priority
                                                                                                                * @maincause: Invalid Thread Priority was Specified
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Use Valid Thread Priority
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NO_MEMORY                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x10u),          /*!< @errcode: 0x0210
                                                                                                                * @desc: OSAL Service Error: Memory Allocation
                                                                                                                * @maincause: Service was unable to Allocate a Block of Memory
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Memory Management
                                                                                                                * @severity:
                                                                                                                */


    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_DELETE_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x11u),          /*!< @errcode: 0x0211
                                                                                                                * @desc: OSAL Service Error: Thread Delete State
                                                                                                                * @maincause: Invalid Thread State for Deletion
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Thread are Correct State for Deletion
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_RESUME_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x12u),          /*!< @errcode: 0x0212
                                                                                                                * @desc: OSAL Service Error: Thread Resume State
                                                                                                                * @maincause: Thread cannot be Resumed because it is not Suspended
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Thread Management
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_CALLER_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x13u),          /*!< @errcode: 0x0213
                                                                                                                * @desc: OSAL Service Error: Non-Thread Calibrationl
                                                                                                                * @maincause: API function was Calibrationibrationled from a non-thread
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check how Function was Calibrationibrationled
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_SUSPEND_ERROR            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x14u),          /*!< @errcode: 0x0214
                                                                                                                * @desc: OSAL Service Error: Thread Suspend State
                                                                                                                * @maincause: Threads to be suspended must be in Terminated or Completed State
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Thread State
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_TIMER_ERROR              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x15u),          /*!< @errcode: 0x0215
                                                                                                                * @desc: OSAL Service Error: Timer Pointer
                                                                                                                * @maincause: Invalid Timer Pointer
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Pointer is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_TICK_ERROR               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x16u),          /*!< @errcode: 0x0216
                                                                                                                * @desc: OSAL Service Error: Timer Tick
                                                                                                                * @maincause: Initial Timer Tick Value Cannot be Zero
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Timer Tick is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_ACTIVATE_ERROR           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x17u),          /*!< @errcode: 0x0217
                                                                                                                * @desc: OSAL Service Error: Activation
                                                                                                                * @maincause: Invalid Activation was Specified
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Activation Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_THRESH_ERROR             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x18u),          /*!< @errcode: 0x0218
                                                                                                                * @desc: OSAL Service Error: Preemption Threshold
                                                                                                                * @maincause: Invalid Preemption Threshold was Specified
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Preemption Threshold is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_SUSPEND_LIFTED           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x19u),          /*!< @errcode: 0x0219
                                                                                                                * @desc: OSAL Service Error: Delayed Suspension
                                                                                                                * @maincause: Previously-set Delayed Suspension was Lifted
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Delayed Suspension
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_WAIT_ABORTED             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1au),          /*!< @errcode: 0x021a
                                                                                                                * @desc: OSAL Service Error: Wait Abort
                                                                                                                * @maincause: Wait was lifted by another thread, timer or ISR
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Event Behaviour
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_WAIT_ABORT_ERROR         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1bu),          /*!< @errcode: 0x021b
                                                                                                                * @desc: OSAL Service Error: Wait Abort Error
                                                                                                                * @maincause: Specified Thread is not in the Wait state
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Feature is reporting an Error
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_MUTEX_ERROR              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1cu),          /*!< @errcode: 0x021c
                                                                                                                * @desc: OSAL Service Error: Mutex Pointer
                                                                                                                * @maincause: Invalid Mutex pointer
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Mutex Pointer is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NOT_AVAILABLE            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1du),          /*!< @errcode: 0x021d
                                                                                                                * @desc: OSAL Service Error: Mutex UnAvailable
                                                                                                                * @maincause: Unable to obtain the Specified Mutex
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Mutex Availability
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NOT_OWNED                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1eu),          /*!< @errcode: 0x021e
                                                                                                                * @desc: OSAL Service Error: Mutex Ownership
                                                                                                                * @maincause: Specified Mutex is not Owned by the Calibrationler
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Ownership of Mutex
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_INHERIT_ERROR            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x1fu),          /*!< @errcode: 0x021f
                                                                                                                * @desc: OSAL Service Error: Priority Inheritance
                                                                                                                * @maincause: Invalid Priority Inheritance Parameter Specified
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Priority Inheritance Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_NOT_DONE                 = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x20u),          /*!< @errcode: 0x0220
                                                                                                                * @desc: OSAL Service Error: Operation Failed
                                                                                                                * @maincause: Specified Operation was not Completed
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Operation Process
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_CEILING_EXCEEDED         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x21u),          /*!< @errcode: 0x0221
                                                                                                                * @desc: OSAL Service Error: Semaphore Limit
                                                                                                                * @maincause: Semaphore upper limit has been exceeded
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check Usage of Semaphores
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_INVALID_CEILING          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x22u),          /*!< @errcode: 0x0222
                                                                                                                * @desc: OSAL Service Error: Invalid Semaphore Limit
                                                                                                                * @maincause: Specified Semaphore Ceiling is Invalid
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_LOG_LINK_SEARCH_FAILED   = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x23u),          /*!< @errcode: 0x0223
                                                                                                                * @desc: OSAL Service Error: Log Link Search Failed
                                                                                                                * @maincause: No Valid Node found in the OSAL Log Link
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_LOG_LINK_NODE_INVALID    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x24u),          /*!< @errcode: 0x0224
                                                                                                                * @desc: OSAL Service Error: Invalid Log Link Node
                                                                                                                * @maincause: OSAL Dynamic Link List Functions were Passed an Invalid Node Index as an Input Parameter
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_LOG_ITEM_INVALID         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x25u),          /*!< @errcode: 0x0225
                                                                                                                * @desc: OSAL Service Error: Invalid Log Item
                                                                                                                * @maincause: Invalid Input Log Item
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_LOG_INVALID_NULL_POINTER = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x26u),          /*!< @errcode: 0x0226
                                                                                                                * @desc: OSAL Service Error: Invalid Log Pointer
                                                                                                                * @maincause: Log Function was Passed an Invalid Pointer as a Parameter
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_INVALID_THREADNUMBER     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0x27u),          /*!< @errcode: 0x0227
                                                                                                                * @desc: the maximum number of init threads (64) is exceeded
                                                                                                                * @maincause: too many threads are being used for initialization
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    /* Reserve OSAL error codes for Samana platform */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_PLATFORM_START           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0xF0u),          /*!< @errcode: 0x02f0
                                                                                                                * @desc: OSAL Service Error: Osal Platform Start
                                                                                                                * @maincause: Osal Platform Start
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_PLATFORM_END             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0xFEu),          /*!< @errcode: 0x02fe
                                                                                                                * @desc: OSAL Service Error: Osal Platform End
                                                                                                                * @maincause: Osal Platform End
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_OSAL_FEATURE_NOT_ENABLED      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_OSAL_ERROR_CODE_START + 0xFFu),          /*!< @errcode: 0x02ff
                                                                                                                * @desc: OSAL Service Error: Disabled Feature
                                                                                                                * @maincause: Requested Feature is not Enabled
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Parameter is Valid
                                                                                                                * @severity:
                                                                                                                */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_SVC_LOG_ERROR Section Base Error Code = 0x0300 ------                           @errcode: 0x0300
     *                                                                                                               @desc: Log Trace Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_LOG_ARG_OUT_OF_RANGE               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_LOG_ERROR_CODE_START + 1u),             /*!< @errcode: 0x0301
                                                                                                                    * @desc: Log Trace Error: Parameter Out of Range
                                                                                                                    * @maincause: Invalid Log Argument Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Log Trace Argument is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_LOG_INVALID_COREID                 = (ADI_ADRV904X_DFE_SVC_ERR_SVC_LOG_ERROR_CODE_START + 2u),             /*!< @errcode: 0x0302
                                                                                                                    * @desc: Log Trace Error: Core ID
                                                                                                                    * @maincause: Invalid Core ID Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Valid Core ID is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_LOG_UART_TX_ERROR                  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_LOG_ERROR_CODE_START + 3u),             /*!< @errcode: 0x0303
                                                                                                                    * @desc: Log Trace Error: UART Transmit
                                                                                                                    * @maincause: UART Failed to Transmit
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE
                                                                                                                    * @mainrecovtext: Check that UART is Correctly Configured
                                                                                                                    * @severity:
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START Section Base Error Code = 0x0400 ------                @errcode: 0x0400
     *                                                                                                               @desc: DFE Event Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_QUEUE_FULL                     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 1u),             /*!< @errcode: 0x0401
                                                                                                                    * @desc: DFE Event Service Error: Capture Queue Full
                                                                                                                    * @maincause: Capture Queue is Full
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Wait until Queue has Capacity to process Requests
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_MALLOC_FAIL                    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 2u),             /*!< @errcode: 0x0402
                                                                                                                    * @desc: DFE Event Service Error: Memory Allocation
                                                                                                                    * @maincause: Capture Buffer could not be Allocated
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Wait until more Captures are processed before making a request this size
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_BAD_CAPTURE_TYPE               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 3u),             /*!< @errcode: 0x0403
                                                                                                                    * @desc: DFE Event Service Error: Capture Type
                                                                                                                    * @maincause: Invalid Capture Type Provided to Function
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Capture Type Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CAPTURE_NOT_CONFIGURED         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 4u),             /*!< @errcode: 0x0404
                                                                                                                    * @desc: DFE Event Service Error: Capture Type Request
                                                                                                                    * @maincause: Capture Type requested was not configured
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Request is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CAPTURE_PENDING                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 5u),             /*!< @errcode: 0x0405
                                                                                                                    * @desc: DFE Event Service Error: Capture Buffer Busy
                                                                                                                    * @maincause: Capture in Progress
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Wait until Current Capture has finished
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 6u),             /*!< @errcode: 0x0406
                                                                                                                    * @desc: DFE Event Service Error: Event Type
                                                                                                                    * @maincause: Invalid Event Type Requested
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Event Type Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_ADAPTATION_IN_PROGRESS         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 7u),             /*!< @errcode: 0x0407
                                                                                                                    * @desc: DFE Event Service Error: Adaptation Busy
                                                                                                                    * @maincause: Adaptation Currently in Progress
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Wait or Abort Current Request before Calibrationling Again
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CAPTURE_UNEXPECTED_EVENT       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 8u),             /*!< @errcode: 0x0408
                                                                                                                    * @desc: DFE Event Service Error: Unexpected Capture Event
                                                                                                                    * @maincause: Unknown Cause to Error Event
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Request is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_BAD_CAPTURE_CHANNEL            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 9u),             /*!< @errcode: 0x0409
                                                                                                                    * @desc: DFE Event Service Error: Capture Channel
                                                                                                                    * @maincause: Invalid Capture Channel Requested
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Capture Channel Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_INVALID_VSWR_DIR               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xAu),           /*!< @errcode: 0x040A
                                                                                                                    * @desc: DFE Event Service Error: VSWR Direction
                                                                                                                    * @maincause: Invalid VSWR Direction Parameter
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check VSWR Direction is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_INVALID_CAP_REQ                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xBu),           /*!< @errcode: 0x040B
                                                                                                                    * @desc: DFE Event Service Error: Invalid capture requested
                                                                                                                    * @maincause: Check if the capture request is supported for Radio mode (RS or RCI)
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the capture request is supported for Radio mode (RS or RCI)
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_PWR_MTR_ABORT_FAILED           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xCu),           /*!< @errcode: 0x040C
                                                                                                                    * @desc: DFE Capture Service Error: Power measurement abort has failed (RCI mode)
                                                                                                                    * @maincause: Unexpected error
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Contact ADI with memdump if the Problem Persists
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_INVALID_CAP_PER_REQ            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xDu),           /*!< @errcode: 0x040D
                                                                                                                    * @desc: DFE Event Service Error: Invalid capture period requested
                                                                                                                    * @maincause: Check if the capture period request is supported(for RS/RCI or DFE mapping mode)
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the capture period request is supported(for RS/RCI or DFE mapping mode)
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_DEC_RATE_CFG       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xEu),           /*!< @errcode: 0x040E
                                                                                                                    * @desc: DFE Event Service Error: Invalid CTC decimation rate configuration
                                                                                                                    * @maincause: CTC decimation rate does meet the min/max rate requirement
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the CTC decimation rate meets the min/max rate requirement
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_ENV_POWER_CFG      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0xFu),           /*!< @errcode: 0x040F
                                                                                                                    * @desc: DFE Event Service Error: Invalid CTC envelop power configuration
                                                                                                                    * @maincause: Invalid CTC envelop power is configured
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the CTC envelop power is configured properly
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_NUM_ROWS_CFG       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x10u),          /*!< @errcode: 0x0410
                                                                                                                    * @desc: DFE Event Service Error: Invalid CTC row configuration
                                                                                                                    * @maincause: Number of CTC rows exceed the maximum allowable rows in the HW
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the CTC rows are properly configured
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_NUM_COLS_CFG       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x11u),          /*!< @errcode: 0x0411
                                                                                                                    * @desc: DFE Event Service Error: Invalid CTC column configuration
                                                                                                                    * @maincause: Number of CTC columns exceed the maximum allowable columns in the HW
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the CTC columns are properly configured
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_DUPLICATE_ENV_POWER_CFG    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x12u),          /*!< @errcode: 0x0412
                                                                                                                    * @desc: DFE Event Service Error: Duplicate CTC envelop power configuration
                                                                                                                    * @maincause: Duplicate CTC envelop power is configured for the same time constant
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check if the duplicate CTC envelop power is configured for the same time constant
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_PATH_BALANCE_FAILED        = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x13u),          /*!< @errcode: 0x0413
                                                                                                                    * @desc: DFE Event Service Error: CTC path balance failed
                                                                                                                    * @maincause: Main datapath delay is less than CT datapath delay. Path balance can't be achieved.
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Need to reduce the decimator ratios or enable CFR with more CFR engines
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_PATH_FRAC_BALANCE_NEEDED   = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x14u),          /*!< @errcode: 0x0414
                                                                                                                    * @desc: DFE Event Service Error: CTC path fractional sample balance required
                                                                                                                    * @maincause: There is a fractional delay difference between main datapath and CT datapath
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Need to add one delay in both gmp_j_pgrm_delay and gmp_i_pgrm_delay of all GMP models
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_PATH_FRAC_BALANCE_FAILED   = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x15u),          /*!< @errcode: 0x0415
                                                                                                                    * @desc: DFE Event Service Error: CTC path exhibits uncorrectable fractional delay
                                                                                                                    * @maincause: Could be caused by bad configurator settings
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check configurator settings
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_CAPTURE_ALIGN_FAILED       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x16u),          /*!< @errcode: 0x0416
                                                                                                                    * @desc: DFE Event Service Error: Alignment of CTC pre-DPD capture and CTC envelop capture failed
                                                                                                                    * @maincause: Alignment of CTC pre-DPD capture and CTC envelop capture can not be achieved
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Modification of CTC model configuration is required
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_DEC_SELECTION      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x17u),          /*!< @errcode: 0x0417
                                                                                                                    * @desc: DFE Event Service Error: Invalid decimator selected for a row
                                                                                                                    * @maincause: Invalid decimator selected for a row
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Modification of CTC model configuration is required
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_CTC_INVALID_FBOX_SCALE         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DFE_ERROR_CODE_START + 0x18u),          /*!< @errcode: 0x0418
                                                                                                                    * @desc: DFE Event Service Error: Invalid fbox scaler selected
                                                                                                                    * @maincause: Fbox scaler value cannot be smaller than minimum scaler value detected based on max num of columns
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Increase fbox scaler value
                                                                                                                    * @severity:
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_RADIO_CTRL Section Base Error Code = 0x0600 ------                    @errcode: 0x0600
     *                                                                                                               @desc: Radio Control Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_RS_INVALID_PTRN_TBL     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 1u),  /*!< @errcode: 0x0601
                                                                                                                * @desc: Radio Control Service Error: Radio Sequencer Pattern Table
                                                                                                                * @maincause: Invalid Radio Sequencer Pattern Table Pointer
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                * @mainrecovtext: Check References to Pattern Tables
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_RS_LUT_CRC_FAILED       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 2u),  /*!< @errcode: 0x0602
                                                                                                                * @desc: Radio Control Service Errors: Radio Sequencer LUT CRC
                                                                                                                * @maincause: Computed LUT CRC does not Match the Provided CRC
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                * @mainrecovtext: Check Image is Valid & Reprogram Device
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_RS_NOT_INITIALIZED      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 3u),  /*!< @errcode: 0x0603
                                                                                                                * @desc: Radio Control Service Errors: Radio Sequencer Initialization
                                                                                                                * @maincause: CPU Profile doesn't Enable the Radio Sequencer
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                * @mainrecovtext: Review CPU Profile & Reprogram Device
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_TX_ATT_INVALID_STEPSIZE = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 4u),  /*!< @errcode: 0x0604
                                                                                                                * @desc: Radio Control Service Errors: Tx attenuation step size is invalid
                                                                                                                * @maincause: Tx attenuation step size is invalid
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                * @mainrecovtext: Review CPU Profile & Reprogram Device
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_ORX_ATT_INVALID         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 5u),  /*!< @errcode: 0x0605
                                                                                                                * @desc: Radio Control Service Errors: Orx attenuation value invalid
                                                                                                                * @maincause: Orx attenuation value is invalid
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                * @mainrecovtext: Use BBIC API to configure Orx attenuation
                                                                                                                * @severity:
                                                                                                                */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_RADIO_CTRL_TX_ORX_MAP_INVALID      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CTRL_ERROR_CODE_START + 6u),  /*!< @errcode: 0x0606
                                                                                                                * @desc: Radio Control Service Errors: Tx-Orx mapping invalid
                                                                                                                * @maincause: Tx to Orx mapping is not valid because the Orx is not a valid channel number, or the Orx is not connected to any Tx.
                                                                                                                * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                * @mainrecovtext: Check Tx and Orx channel numbers and mapping setting
                                                                                                                * @severity:
                                                                                                                */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_EVENT Section Base Error Code = 0x0700 ------                         @errcode: 0x0700
     *                                                                                                               @desc: Event Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_INVALID_TYPE       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 1u),                     /*!< @errcode: 0x0701
                                                                                                                    * @desc: Event Service Error: Event Type
                                                                                                                    * @maincause: Invalid Event Type Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Event Type Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_REGISTER_LIMIT     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 2u),                     /*!< @errcode: 0x0702
                                                                                                                    * @desc: Event Service Error: Event Register Limit
                                                                                                                    * @maincause: Thread Register Limit for same Event Type Reached
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check That All Threads are Required for that Event
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_ALREADY_REGISTERED = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 3u),                     /*!< @errcode: 0x0703
                                                                                                                    * @desc: Event Service Error: Duplicate Thread Registration
                                                                                                                    * @maincause: Thread already Registered for Event Type
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_NOT_REGISTERED     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 4u),                     /*!< @errcode: 0x0704
                                                                                                                    * @desc: Event Service Error: Event Not Registered
                                                                                                                    * @maincause: Thread not Registered to Receive Event Type
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check Thread can be registered for Event Type
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_QUEUE_FULL         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 5u),                     /*!< @errcode: 0x0705
                                                                                                                    * @desc: Event Service Error: Queue Full
                                                                                                                    * @maincause: Event could not be Notified because Receiving Thread's Queue was full
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check Capacity of Receiving Queue
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_EVENT_TIMEOUT            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_EVENT_ERROR_CODE_START + 6u),                     /*!< @errcode: 0x0706
                                                                                                                    * @desc: Event Service Error: Event Timeout
                                                                                                                    * @maincause: Event Timed Out
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check that Event was Correctly Registered
                                                                                                                    * @severity:
                                                                                                                    */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_MALLOC Section Base Error Code = 0x0900 ------                        @errcode: 0x0900
     *                                                                                                               @desc: Malloc Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_INIT_FAILED                = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x01u),       /*!< @errcode: 0x0901
                                                                                                                   * @desc: Malloc Service Error: Service Initialization Failed
                                                                                                                   * @maincause: Mutex / Byte Pool Create Failure, or Non-Contiguous Memory
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reprogram Device
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_ALLOC_FAILED               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x02u),       /*!< @errcode: 0x0902
                                                                                                                   * @desc: Malloc Service Error: Memory Pool Allocation Failed
                                                                                                                   * @maincause: Memory Pool too small or not enough Space Available
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_BLK_NOT_CONTIG             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x03u),       /*!< @errcode: 0x0903
                                                                                                                   * @desc: Malloc Service Error: Non-Contiguous Memory Region
                                                                                                                   * @maincause: Memory Block is not Contiguous
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reprogram Device
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_ENABLE_FAILED              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x04u),       /*!< @errcode: 0x0904
                                                                                                                   * @desc:Malloc Service Error: Memory Pool Create Failed
                                                                                                                   * @maincause: Could not Create Dynamic Memory Pools
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_BLK_UNALIGNED              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x05u),       /*!< @errcode: 0x0905
                                                                                                                   * @desc: Malloc Service Error: Memory Alignment Failure
                                                                                                                   * @maincause: Memory Region not aligned on unsigned long boundary
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_L4_BOUNDS_FAILURE          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x06u),       /*!< @errcode: 0x0906
                                                                                                                   * @desc: Malloc Service Error: L4 Boundary Limit
                                                                                                                   * @maincause: Requested Memory Region is outside of L4 Memory space
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_MEM_POOL_BYTES_AVAIL_ERROR = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x07u),       /*!< @errcode: 0x0907
                                                                                                                   * @desc: Malloc Service Error: Invalid Memory Pool
                                                                                                                   * @maincause: Number of Memory Pool bytes Available exceeds Memory Pool size
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_GET_INIT_HEAP_INFO_FAILED  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x08u),       /*!< @errcode: 0x0908
                                                                                                                   * @desc: Malloc Service Error: Invalid Heap Memory Pool
                                                                                                                   * @maincause: Initial Heap Memory Pool is Invalid or Returns Invalid Bytes Available
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_GET_FREE_SPACE_INFO_FAILED = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x09u),       /*!< @errcode: 0x0909
                                                                                                                   * @desc: Malloc Service Error: Free Space Information Get Failed
                                                                                                                   * @maincause: Free Space Memory Pool is Invalid or Returns Invalid Bytes Available
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MALLOC_GET_DYN_MEM_INFO_FAILED    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_MALLOC_ERROR_CODE_START + 0x0au),       /*!< @errcode: 0x090a
                                                                                                                   * @desc: Malloc Service Error: Dynamic Memory Information Get Failed
                                                                                                                   * @maincause: Initial Heap / Free Space Memory Pool Invalid or returns Invalid bytes Available
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                   * @severity:
                                                                                                                   */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BBIC_BRIDGE Section Base Error Code = 0x0A00 ------                   @errcode: 0x0A00
     *                                                                                                               @desc: BBIC Bridge Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_BBIC_BRIDGE_INIT_REGION_RELOCATE_FAILED = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BBIC_BRIDGE_ERROR_CODE_START + 1u), /*!< @errcode: 0x0A01
                                                                                                                     * @desc: BBIC Bridge Service Error: Firmware Image Initialization Region Size Limit
                                                                                                                     * @maincause: Firmware Image exceeds Initialization Region Limit
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Reduce Firmware Image Size and Reprogram Device. Contact ADI if the Problem Persists
                                                                                                                     * @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_BUF_MGMT Section Base Error Code = 0x0B00 ------                     @errcode: 0x0B00
     *                                                                                                              @desc: Memory Buffer Management Service
     *                                                                                                              @maincause:
     *                                                                                                              @mainrecovenum:
     *                                                                                                              @severity:
     *                                                                                                              @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INIT_FAILURE                 = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x01u),   /*!< @errcode: 0x0B01
                                                                                                                     * @desc: Memory Buffer Management Service Error: Initialization Failed
                                                                                                                     * @maincause: Memory Allocation Failed
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUF_ACQUIRE_FAILURE          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x02u),   /*!< @errcode: 0x0B02
                                                                                                                     * @desc: Memory Buffer Management Service Error: Capture Buffer Acquirement Failed
                                                                                                                     * @maincause: Lack of Memory Buffer Available for Required Capture
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUF_RELEASE_FAILURE          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x03u),   /*!< @errcode: 0x0B03
                                                                                                                     * @desc: Memory Buffer Management Service Error: Capture Mutex Release Failure
                                                                                                                     * @maincause: External DPD Capture Data Handle in Process
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Capture Status should be Released at this Point
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUFFER_INVALID               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x04u),   /*!< @errcode: 0x0B04
                                                                                                                     * @desc: Memory Buffer Management Service Error: Invalid Buffer Release
                                                                                                                     * @maincause: Attempt made to release an Invalid Memory Buffer
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check which Memory Buffers are Available for Release
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUF_NO_INSTANCE              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x05u),   /*!< @errcode: 0x0B05
                                                                                                                     * @desc: Memory Buffer Management Service Error: No Buffer Instance
                                                                                                                     * @maincause: Attempt made to acquire more buffers than Available
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Number of Available Buffers
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_ALLOCATE_FAILURE             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x06u),   /*!< @errcode: 0x0B06
                                                                                                                     * @desc: Memory Buffer Management Service Error: Buffer Allocation Failed
                                                                                                                     * @maincause: Unable to Allocate Memory for Buffer Management
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Memory Requirements & Usage
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUF_UNAVAILABLE              = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x07u),   /*!< @errcode: 0x0B07
                                                                                                                     * @desc: Memory Buffer Management Service Error: No Available Buffer
                                                                                                                     * @maincause: Buffer semaphore count is non-Zero, but no buffers marked Available
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Reprogram Device
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_BUF_NOT_ALLOCATED            = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x08u),   /*!< @errcode: 0x0B08
                                                                                                                     * @desc: Memory Buffer Management Service Error: UnAllocated Buffer
                                                                                                                     * @maincause: Buffer Type has Zero Allocated Instances
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Buffer Allocations for Buffer Type
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_CHANNELS         = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x09u),   /*!< @errcode: 0x0B09
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Samples
                                                                                                                     * @maincause: Invalid Number of Samples Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Samples Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_DPD_FEATURES     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Au),   /*!< @errcode: 0x0B0A
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of DPD Features
                                                                                                                     * @maincause: Invalid Number of DPD Features Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of DPD Features Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_FILT_COEFS       = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Bu),   /*!< @errcode: 0x0B0B
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Filter Coefficients
                                                                                                                     * @maincause: Invalid Number of Filter Coefficients Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Filter Coefficients Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_SAMPLES_2_WAY    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Cu),   /*!< @errcode: 0x0B0C
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of 2-Way Samples
                                                                                                                     * @maincause: Invalid Number of 2-Way Samples Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of 2-Way Samples Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_SAMPLES_3_WAY    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Du),   /*!< @errcode: 0x0B0D
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of 3-Way Samples
                                                                                                                     * @maincause: Invalid Number of 3-Way Samples Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of 3-Way Samples Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_ADAPT_BUFFERS    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Eu),   /*!< @errcode: 0x0B0E
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Adaptation Buffers
                                                                                                                     * @maincause: Invalid Number of Adaptation Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Adaptation Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_CAPTURE_WAYS     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x0Fu),   /*!< @errcode: 0x0B0F
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Capture Ways
                                                                                                                     * @maincause: Invalid Number of Capture Ways Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Capture Ways Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_CAPTURE_BUFFERS  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x10u),   /*!< @errcode: 0x0B10
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Capture Buffers
                                                                                                                     * @maincause: Invalid Number of Capture Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Capture Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_CAPTURE_PER_ADAPT    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x11u),   /*!< @errcode: 0x0B11
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of Captures per Adaptation
                                                                                                                     * @maincause: Invalid Number of Captures per Adaptation Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of Captures per Adaptation Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_VSWR_SAMPLES     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x12u),   /*!< @errcode: 0x0B12
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of VSWR Samples
                                                                                                                     * @maincause: Invalid Number of VSWR Samples Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of VSWR Samples Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_VSWR_BUFFERS     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x13u),   /*!< @errcode: 0x0B13
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of VSWR Buffers
                                                                                                                     * @maincause: Invalid Number of VSWR Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of VSWR Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_LUT_BUFFERS      = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x14u),   /*!< @errcode: 0x0B14
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of LUT Buffers
                                                                                                                     * @maincause: Invalid Number of LUT Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of VSWR Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_DPD_PROC_BUFFERS = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x15u),   /*!< @errcode: 0x0B15
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of DPD Processing Buffers
                                                                                                                     * @maincause: Invalid Number of DPD Processing Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of DPD Processing Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_BUF_MGMT_INVALID_NUM_CTC_FLUT_BUFFERS = (ADI_ADRV904X_DFE_SVC_ERR_SVC_BUF_MGMT_ERROR_CODE_START + 0x16u),   /*!< @errcode: 0x0B16
                                                                                                                     * @desc: Memory Buffer Management Service Error: Number of CTC FLUT Buffers
                                                                                                                     * @maincause: Invalid Number of CTC FLUT Buffers Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     * @mainrecovtext: Check Number of CTC FLUT Buffers Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_CFG Section Base Error Code = 0x0C00 ------                           @errcode: 0x0C00
     *                                                                                                               @desc: Configuration Adjustment Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_KEYMISMATCH             = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 1u),                   /*!< @errcode: 0x0C01
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Key MisMatch
                                                                                                                   * @maincause: Invalid Configuration Key Parameter
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Configuration Key Parameter is Valid
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_SYSTEM_CONFIG_LOCKED    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 2u),                   /*!< @errcode: 0x0C02
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Service is Locked
                                                                                                                   * @maincause: Configuration Service has not been Unlocked
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Unlock Configuration before trying again
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_GETCONFIG_NOT_SUPPORTED = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 3u),                   /*!< @errcode: 0x0C03
                                                                                                                   * @desc: Configuration Adjustment Service Error: Get Configuration Command
                                                                                                                   * @maincause: Get Configuration Command is not Supported for the Provided Object ID
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Get Configuration Command is Supported for the Provided Object ID
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_SETCONFIG_NOT_SUPPORTED = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 4u),                   /*!< @errcode: 0x0C04
                                                                                                                   * @desc: Configuration Adjustment Service Error: Set Configuration Command
                                                                                                                   * @maincause: Set Configuration Command is not Supported for the Provided ObjID
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Set Configuration Command is Supported for the Provided Object ID
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_INVALID_CONFIG_OBJECT   = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 5u),                   /*!< @errcode: 0x0C05
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Object ID
                                                                                                                   * @maincause: Invalid Configuration Object ID was received with the last Configuration Command
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Configuration Object ID is Valid
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_LIMIT_REACHED           = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 6u),                   /*!< @errcode: 0x0C06
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Limit
                                                                                                                   * @maincause: Configuration Buffer Limit Reached
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                   * @mainrecovtext: Check if all Configurations are Required
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_INVALID_CONFIG_SIZE     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 7u),                   /*!< @errcode: 0x0C07
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Data Size
                                                                                                                   * @maincause: Invalid Configuration Data Size Parameter
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Configuration Data Size Parameter is Valid
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_CFG_INVALID_CONFIG_OFFSET   = (ADI_ADRV904X_DFE_SVC_ERR_SVC_CFG_ERROR_CODE_START + 8u),                   /*!< @errcode: 0x0C08
                                                                                                                   * @desc: Configuration Adjustment Service Error: Configuration Offset
                                                                                                                   * @maincause: Invalid Configuration Offset Parameter
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check Configuration Offset Parameter is Valid
                                                                                                                   * @severity:
                                                                                                                   */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_ECC Section Base Error Code = 0x0F00 ------                           @errcode: 0x0F00
     *                                                                                                               @desc: ECC-Parity Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_ECC_ERROR        = (ADI_ADRV904X_DFE_SVC_ERR_SVC_ECC_ERROR_CODE_START + 1u),                              /*!< @errcode: 0x0F01
                                                                                                                   * @desc: ECC-Parity Error: Unrecoverable ECC Error
                                                                                                                   * @maincause: Memory Hardware Fault
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reset device and try again. Contact ADI if the Problem Persists
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_ECC_CONFIG_ERROR = (ADI_ADRV904X_DFE_SVC_ERR_SVC_ECC_ERROR_CODE_START + 2u),                              /*!< @errcode: 0x0F02
                                                                                                                   * @desc: ECC-Parity Error: ECC configuration
                                                                                                                   * @maincause: Incorrect Parameter Provided for ECC configuration
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                   * @severity:
                                                                                                                   */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_SVC_WATCHDOG Section Base Error Code = 0x1000 ------                      @errcode: 0x1000
     *                                                                                                               @desc: Watchdog Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_WDTIMER_INVALID_PARAMETER = (ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START + 1u),             /*!< @errcode: 0x1001
                                                                                                                   * @desc: Watchdog Error: Watchdog configuration
                                                                                                                   * @maincause: Incorrect Parameter Provided for Watchdog configuration
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Ensure Input Parameters are correct
                                                                                                                   * @severity: CRITICAL
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_WDTIMER_TIMER0_EXPIRED    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START + 2u),             /*!< @errcode: 0x1002
                                                                                                                   * @desc: Watchdog Error: Watchdog timer 0 expired
                                                                                                                   * @maincause: Possible system lockup detected
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reset device and try again. Contact ADI if the Problem Persists
                                                                                                                   * @severity: FATAL
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_WDTIMER_TIMER1_EXPIRED    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START + 3u),             /*!< @errcode: 0x1003
                                                                                                                   * @desc: Watchdog Error: Watchdog timer 1 expired
                                                                                                                   * @maincause: Possible system lockup detected
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reset device and try again. Contact ADI if the Problem Persists
                                                                                                                   * @severity: FATAL
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_WDTIMER_TIMER2_EXPIRED    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START + 4u),             /*!< @errcode: 0x1004
                                                                                                                   * @desc: Watchdog Error: Watchdog timer 2 expired
                                                                                                                   * @maincause: Possible system lockup detected
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reset device and try again. Contact ADI if the Problem Persists
                                                                                                                   * @severity: FATAL
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_WDTIMER_TIMER3_EXPIRED    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_WDTIMER_ERROR_CODE_START + 5u),             /*!< @errcode: 0x1005
                                                                                                                   * @desc: Watchdog Error: Watchdog timer 3 expired
                                                                                                                   * @maincause: Possible system lockup detected
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                   * @mainrecovtext: Reset device and try again. Contact ADI if the Problem Persists
                                                                                                                   * @severity: FATAL
                                                                                                                   */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START Section Base Error Code = 0x1100 ------                     @errcode: 0x1100
     *                                                                                                               @desc: Radio Command Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_BAD_TID          = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 1u),                      /*!< @errcode: 0x1101
                                                                                                                    * @desc: Radio Command Error: Transaction ID
                                                                                                                    * @maincause: Invalid Transaction ID Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Transaction ID is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_LINK_BUSY        = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 2u),                      /*!< @errcode: 0x1102
                                                                                                                    * @desc: Radio Command Error: Link Busy
                                                                                                                    * @maincause: Link is Currently Processing a Command
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE
                                                                                                                    * @mainrecovtext: Wait for Link to Complete current Command
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_INVALID_LINK_ID  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 3u),                      /*!< @errcode: 0x1103
                                                                                                                    * @desc: Radio Command Error: Link Number
                                                                                                                    * @maincause: Invalid Link Number Provided in Command
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Valid Link Number is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_INVALID_MAILBOX  = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 4u),                      /*!< @errcode: 0x1104
                                                                                                                    * @desc: Radio Command Error: Mailbox Number
                                                                                                                    * @maincause: Invalid Mailbox Number Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Mailbox Number is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_INVALID_MSG_SIZE = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 5u),                      /*!< @errcode: 0x1105
                                                                                                                    * @desc: Radio Command Error: Message size
                                                                                                                    * @maincause: Invalid Command or Response Size Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Command and Response Sizes are Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_BAD_RESPONSE     = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 6u),                      /*!< @errcode: 0x1106
                                                                                                                    * @desc: Radio Command Error: Message response content is bad
                                                                                                                    * @maincause: Response message contains unexpected data
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Command and Response values
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_SVC_RADIO_CMD_INVALID_PARAM    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_RADIO_CMD_START + 7u),                      /*!< @errcode: 0x1107
                                                                                                                    * @desc: Radio Command Error: Message parameter is invalid
                                                                                                                    * @maincause: Message contains an invalid parameter
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check parameters in message
                                                                                                                    * @severity:
                                                                                                                    */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPIO Section Base Error Code = 0x2000 ------                          @errcode: 0x2000
     *                                                                                                               @desc: Debug Service Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_DBG_KEY_MISMATCH                    = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DBG_ERROR_CODE_START + 1u),            /*!< @errcode: 0x2001
                                                                                                                    * @desc: Debug Service Error: Debug Key MisMatch
                                                                                                                    * @maincause: Invalid Debug Key Parameter
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Debug Key Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DBG_CMD_LEN_INVALID                 = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DBG_ERROR_CODE_START + 2u),            /*!< @errcode: 0x2002
                                                                                                                    * @desc: Debug Service Error: Command Length
                                                                                                                    * @maincause: Invalid Command Length Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Command Length is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DBG_CMD_NOT_SUPPORTED               = (ADI_ADRV904X_DFE_SVC_ERR_SVC_DBG_ERROR_CODE_START + 3u),            /*!< @errcode: 0x2003
                                                                                                                    * @desc: Debug Service Error: Debug Command
                                                                                                                    * @maincause: Invalid Debug Command Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Debug Command is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPIO Section Base Error Code = 0x3000 ------                          @errcode: 0x3000
     *                                                                                                               @desc: GPIO Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_GPIO_INVALID_SIGNAL_ID          = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPIO_ERROR_CODE_START + 1u),            /*!< @errcode: 0x3001
                                                                                                                     * @desc: GPIO Driver Error: Signal ID
                                                                                                                     * @maincause: Invalid Signal ID Provided
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check Signal ID Provided is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_GPIO_INVALID_PIN_ID             = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPIO_ERROR_CODE_START + 2u),            /*!< @errcode: 0x3002
                                                                                                                     * @desc: GPIO Driver Error: Pin ID
                                                                                                                     * @maincause: Invalid GPIO Pin ID Provided
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check Pin ID Provided is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_DRV_NO_GPIO_INPUT_ASSIGNED_TO_ALARM = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPIO_ERROR_CODE_START + 3u),            /*!< @errcode: 0x3003
                                                                                                                     * @desc: GPIO Driver Error: Alarm Assignment
                                                                                                                     * @maincause: Requested Alarm has no GPIO Input Assigned
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Verify Alarm Configuration
                                                                                                                     * @severity:
                                                                                                                     */


    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_GPT Section Base Error Code = 0x3100 ------                           @errcode: 0x3100
     *                                                                                                               @desc: General Purpose Timer Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_ALLOCATED                       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 1u),             /*!< @errcode: 0x3101
                                                                                                                     * @desc: GPT Driver Error: GPT Instance Allocated
                                                                                                                     * @maincause: Attempt to Allocate previously Allocated GPT instance
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check GPT Allocations
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_UNALLOCATED                     = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 2u),             /*!< @errcode: 0x3102
                                                                                                                     * @desc: GPT Driver Error: GPT Instance Not Allocated
                                                                                                                     * @maincause: Invalid GPT Instance Allocation State
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check GPT Instance Allocation Status
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_INIT_INVALID_CORE               = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 3u),             /*!< @errcode: 0x3103
                                                                                                                     * @desc: GPT Driver Error: Processor Core
                                                                                                                     * @maincause: Invalid Processor Core Parameter
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check Processor Core Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_INIT_INT_CFG_FAILED             = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 4u),             /*!< @errcode: 0x3104
                                                                                                                     * @desc: GPT Driver Error: Interrupt Configuration
                                                                                                                     * @maincause: Invalid processor core / failed to register IRQ
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check GPT Interrupt Parameters are Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_MAX_TIMEOUT_EXCEEDED            = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 5u),             /*!< @errcode: 0x3105
                                                                                                                     * @desc: GPT Driver Error: Timeout Limit
                                                                                                                     * @maincause: Invalid Timeout Limit Provided
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check Timeout Limit Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_INVALID_SCLK_FREQ               = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 6u),             /*!< @errcode: 0x3106
                                                                                                                     * @desc: GPT Driver Error: System Clock Get
                                                                                                                     * @maincause: Invalid System Clock Get
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check System Clock is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_GPT_MUTEX_FAILURE                   = (ADI_ADRV904X_DFE_SVC_ERR_DRV_GPT_ERROR_CODE_START + 7u),             /*!< @errcode: 0x3107
                                                                                                                     * @desc: GPT Driver Error: Mutex
                                                                                                                     * @maincause: OSAL Mutex Error
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check OSAL Mutex Status
                                                                                                                     * @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_INTERRUPT Section Base Error Code = 0x3200 ------                     @errcode: 0x3200
     *                                                                                                               @desc: Interrupt Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_INVALID_INTID       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 1u),                        /*!< @errcode: 0x3201
                                                                                                                    * @desc: Interrupt Driver Error: Interrupt ID
                                                                                                                    * @maincause: Invalid Interrupt ID Parameter Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check Interrupt ID Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_INVALID_CORE        = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 2u),                        /*!< @errcode: 0x3202
                                                                                                                    * @desc: Interrupt Driver Error: CPU Core ID
                                                                                                                    * @maincause: Invalid CPU Core ID Parameter Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                    * @mainrecovtext: Check CPU Core ID Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_INVALID_PRIORITY    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 3u),                        /*!< @errcode: 0x3203
                                                                                                                    * @desc: Interrupt Driver Error: Interrupt Priority
                                                                                                                    * @maincause: Invalid Interrupt Priority Parameter Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Interrupt Priority Parameter is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_INVALID_POINTER     = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 4u),                        /*!< @errcode: 0x3204
                                                                                                                    * @desc: Interrupt Driver Error: Invalid Interrupt Service Pointer
                                                                                                                    * @maincause: Invalid Interrupt Service Pointer Provided
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check Interrupt Service Pointer is Valid
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_UNEXPECTED          = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 5u),                        /*!< @errcode: 0x3205
                                                                                                                    * @desc: Interrupt Driver Error: Unexpected Interrupt
                                                                                                                    * @maincause: No handler installed for Interrupt
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check all Interrupts are correctly handled
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_FIQ_UNEXPECTED          = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 6u),                        /*!< @errcode: 0x3206
                                                                                                                    * @desc: Interrupt Driver Error: Unexpected Fast Interrupt
                                                                                                                    * @maincause: No handler installed for Fast Interrupt
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Check all Fast Interrupts are correctly handled
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_NOT_CONFIG          = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 7u),                        /*!< @errcode: 0x3207
                                                                                                                    * @desc: Interrupt Driver Error: Unconfigured Interrupt
                                                                                                                    * @maincause: No handler installed for Interrupt
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check Interrupt was configured correctly
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_ENABLED             = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 8u),                        /*!< @errcode: 0x3208
                                                                                                                    * @desc: Interrupt Driver Error: Active Interrupt
                                                                                                                    * @maincause: Interrupt not Disabled before subsequent Calibrationibrationl to Configure
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Disable Interrupt before reconfiguration
                                                                                                                    * @severity:
                                                                                                                    */

    /* Reserve Drv Int error codes for Samana platform */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_PLATFORM_START      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 0xF0u),                     /*!< @errcode: 0x32F0
                                                                                                                    * @desc: Interrupt Driver Error: Platform Interrupt Start
                                                                                                                    * @maincause: Platform Interrupt Start
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check Interrupt was configured correctly
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_INT_PLATFORM_END        = (ADI_ADRV904X_DFE_SVC_ERR_DRV_INT_ERROR_CODE_START + 0xFFu),                     /*!< @errcode: 0x32FF
                                                                                                                    * @desc: Interrupt Driver Error: Platform Interrupt End
                                                                                                                    * @maincause: Platform Interrupt End
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check Interrupt was configured correctly
                                                                                                                    * @severity:
                                                                                                                    */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_UART Section Base Error Code = 0x3300------                           @errcode: 0x3300
     *                                                                                                               @desc: UART Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_INVALID_INSTANCE   = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 1u),                        /*!< @errcode: 0x3301
                                                                                                                     * @desc: UART Driver Error: UART Driver ID
                                                                                                                     * @maincause: Invalid UART Driver ID Provided
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                     * @mainrecovtext: Check UART Driver ID Parameter is Valid
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_INVALID_SCLK_FREQ  = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 2u),                        /*!< @errcode: 0x3302
                                                                                                                     * @desc: UART Driver Error: System Clock Get
                                                                                                                     * @maincause: System Clock Data not Available for Calibrationculation
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check System Clock is Configured Correctly
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_SEM_CREATE_FAILURE = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 3u),                        /*!< @errcode: 0x3303
                                                                                                                     * @desc: UART Driver Error: Semaphore Create
                                                                                                                     * @maincause: OSAL unable to create Semaphore
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check OSAL Semaphores
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_MUTEX_FAILURE      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 4u),                        /*!< @errcode: 0x3304
                                                                                                                     * @desc: UART Driver Error: Mutex
                                                                                                                     * @maincause: OSAL unable to Process Mutex
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check OSAL Mutex
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_INT_INIT_FAILURE   = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 5u),                        /*!< @errcode: 0x3305
                                                                                                                     * @desc: UART Driver Error: Interrupt Initialization
                                                                                                                     * @maincause: Interrupt Driver did not Register Interrupt Handler
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Interrupt Handler
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_UNINITIALIZED      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 6u),                        /*!< @errcode: 0x3306
                                                                                                                     * @desc: UART Driver Error: Uninitialized
                                                                                                                     * @maincause: UART Driver Requested is not Initialized
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: UART Driver Requires Initialization before use
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_TX_FIFO_FULL       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 7u),                        /*!< @errcode: 0x3307
                                                                                                                     * @desc: UART Driver Error: Tx FIFO Full
                                                                                                                     * @maincause: Request made to Transmit with Tx FIFO Full
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Wait until UART Driver has transmitted existing Data
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_UART_TX_FAILURE         = (ADI_ADRV904X_DFE_SVC_ERR_DRV_UART_ERROR_CODE_START + 8u),                        /*!< @errcode: 0x3308
                                                                                                                     * @desc: UART Driver Error: Semaphore Wait
                                                                                                                     * @maincause: Unexpected Semaphore Error during UART Transmit
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check OSAL Semaphores
                                                                                                                     * @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_MB Section Base Error Code = 0x3400------                             @errcode: 0x3400
     *                                                                                                               @desc: Mailbox Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_BUSY                    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 1u),              /*!< @errcode: 0x3401
                                                                                                                    * @desc: Mailbox Driver Error: Busy
                                                                                                                    * @maincause: Mailbox is not Available to process new commands
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Poll Mailbox until Ready
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_INIT_FAILED             = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 2u),              /*!< @errcode: 0x3402
                                                                                                                    * @desc: Mailbox Driver Error: Initialization
                                                                                                                    * @maincause: Unable to create OSAL Mutex
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check OSAL Mutex for Mailbox
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_CLR_PENDING_FAILED      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 3u),              /*!< @errcode: 0x3403
                                                                                                                    * @desc: Mailbox Driver Error: Clear/Pending Update
                                                                                                                    * @maincause: OSAL Mutex Failed during Status Update
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check OSAL Mutex for Mailbox
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_REGISTER_HANDLER_FAILED = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 4u),              /*!< @errcode: 0x3404
                                                                                                                    * @desc: Mailbox Driver Error: Configuration Update
                                                                                                                    * @maincause: OSAL Mutex Failed during Configuration Update
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check OSAL Mutex for Mailbox
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_SEND_CMD_FAILED         = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 5u),              /*!< @errcode: 0x3405
                                                                                                                    * @desc: Mailbox Driver Error: Send Command
                                                                                                                    * @maincause: OSAL Mutex Failed during Send Command
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                    * @mainrecovtext: Check OSAL Mutex for Mailbox & if Mailbox was Ready to Accept Commands
                                                                                                                    * @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_MB_MAILBOX_FORCE_EXCEPTION_CMD     = (ADI_ADRV904X_DFE_SVC_ERR_DRV_MB_ERROR_CODE_START + 6u),              /*!< @errcode: 0x3406
                                                                                                                    * @desc: Mailbox Driver Error: Forced Exception
                                                                                                                    * @maincause: Forced Exception Command Received
                                                                                                                    * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    * @mainrecovtext: Reprogram Device
                                                                                                                    * @severity:
                                                                                                                    */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_SYSCLK Section Base Error Code = 0x3600------                         @errcode: 0x3600
     *                                                                                                               @desc: SYSCLK Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_SYSCLK_NOINIT_ERROR  = (ADI_ADRV904X_DFE_SVC_ERR_DRV_SYSCLK_ERROR_CODE_START + 1u),                          /*!< @errcode: 0x3601
                                                                                                                     * @desc: SYSCLK Driver Error: Initialization
                                                                                                                     * @maincause: Driver not Initialized
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Configure System Clock
                                                                                                                     * @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_SYSCLK_INVALID_PARAM = (ADI_ADRV904X_DFE_SVC_ERR_DRV_SYSCLK_ERROR_CODE_START + 2u),                          /*!< @errcode: 0x3602
                                                                                                                     * @desc: SYSCLK Driver Error: Invalid Frequency
                                                                                                                     * @maincause: System HS Digital Clock and/or Clock Dividers NOK
                                                                                                                     * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                     * @mainrecovtext: Check Profile Clock Settings
                                                                                                                     * @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_STREAM Section Base Error Code = 0x3700------                         @errcode: 0x3700
     *                                                                                                               @desc: STREAM Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_INVALID_PARAM = (ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START + 1u),                          /*!< @errcode: 0x3701
                                                                                                                     *   @desc: STREAM Driver Error: Generic Parameter
                                                                                                                     *   @maincause: One or more Input parameters are not Valid
                                                                                                                     *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     *   @mainrecovtext: Reset device, if Problem Persists contact ADI.
                                                                                                                     *   @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_TIMEOUT       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START + 2u),                          /*!< @errcode: 0x3702
                                                                                                                     *   @desc: STREAM Driver Error: Timeout
                                                                                                                     *   @maincause: Timeout can occur due to stream processor waiting for other streams to Complete
                                                                                                                     *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     *   @mainrecovtext: Reset the feature to recover from this error, if Problem Persists contact ADI.
                                                                                                                     *   @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_PREV_TIMEOUT  = (ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START + 3u),                          /*!< @errcode: 0x3703
                                                                                                                     *   @desc: STREAM Driver Error: Incomplete Task
                                                                                                                     *   @maincause: Previous stream started did not finish
                                                                                                                     *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     *   @mainrecovtext: Reset the feature to recover from this error. if Problem Persists contact ADI.
                                                                                                                     *   @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR         = (ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START + 4u),                          /*!< @errcode: 0x3704
                                                                                                                     *   @desc: STREAM Driver Error: Stream Error
                                                                                                                     *   @maincause: Stream resulted in an error
                                                                                                                     *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     *   @mainrecovtext: Reset the feature to recover from this error. if Problem Persists contact ADI.
                                                                                                                     *   @severity:
                                                                                                                     */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_PREV_ERROR    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_STREAM_ERROR_CODE_START + 5u),                          /*!< @errcode: 0x3705
                                                                                                                     *   @desc: STREAM Driver Error: Previous Stream Error
                                                                                                                     *   @maincause: Previous stream resulted in an error
                                                                                                                     *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                     *   @mainrecovtext: Reset the feature to recover from this error. if Problem Persists contact ADI.
                                                                                                                     *   @severity:
                                                                                                                     */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_PWR_MTR Section Base Error Code = 0x3800------                        @errcode: 0x3800
     *                                                                                                               @desc: Power Meter Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_INVALID_PARAM    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ERROR_CODE_START + 1u),                    /*!< @errcode: 0x3801
                                                                                                                    *   @desc: Power Meter Driver Error: Parameter
                                                                                                                    *   @maincause: Invalid Channel or Carrier Mask
                                                                                                                    *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    *   @mainrecovtext: Check Channel and/or Carrier Mask is Valid
                                                                                                                    *   @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_NOT_DISABLED     = (ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ERROR_CODE_START + 2u),                    /*!< @errcode: 0x3802
                                                                                                                    *   @desc: Power Meter Driver Error: Not Disabled
                                                                                                                    *   @maincause: Power Meter is not Disabled
                                                                                                                    *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    *   @mainrecovtext: Power Meter needs to be Disabled
                                                                                                                    *   @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_INVALID_CHANNEL  = (ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ERROR_CODE_START + 3u),                    /*!< @errcode: 0x3803
                                                                                                                    *   @desc: Channel number inValid
                                                                                                                    *   @maincause: Channel number is inValid for the meter type
                                                                                                                    *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    *   @mainrecovtext: Firmware programmatic error. disable power meter, if Problem Persists contact ADI
                                                                                                                    *   @severity:
                                                                                                                    */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ZERO_SAMPLE_RATE = (ADI_ADRV904X_DFE_SVC_ERR_DRV_PWR_MTR_ERROR_CODE_START + 4u),                    /*!< @errcode: 0x3804
                                                                                                                    *   @desc: Sample rate invalid
                                                                                                                    *   @maincause: Selected carrier for TSSI or RSSI may not be initialized
                                                                                                                    *   @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                    *   @mainrecovtext: Check if the carriers selected are enabled in the profile being used.
                                                                                                                    *   @severity:
                                                                                                                    */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DMA Section Base Error Code = 0x3900------                            @errcode: 0x3900
     *                                                                                                               @desc: DMA Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_CHANNEL_INVALID      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 1u),                           /*!< @errcode: 0x3901
                                                                                                                       * @desc: DMA Driver Error: DMA Channel Data
                                                                                                                       * @maincause: Invalid DMA Channel Data Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check DMA Channel Data is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_IN_USE               = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 2u),                           /*!< @errcode: 0x3902
                                                                                                                       * @desc: DMA Driver Error: DMA Stream Busy
                                                                                                                       * @maincause: Supplied DMA Channel/stream ID is already open and in use
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Use of Streams
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_NOT_OPEN             = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 3u),                           /*!< @errcode: 0x3903
                                                                                                                       * @desc: DMA Driver Error: Closed Stream
                                                                                                                       * @maincause: DMA Stream must be opened before it is used
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Use of Streams
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ALREADY_RUNNING      = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 4u),                           /*!< @errcode: 0x3904
                                                                                                                       * @desc: DMA Driver Error: DMA Busy
                                                                                                                       * @maincause: Function not permitted since DMA transfer is in progress
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Function invoked in incorrect sequence. Check DMA Sequence
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_STREAM_ID        = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 5u),                           /*!< @errcode: 0x3905
                                                                                                                       * @desc: DMA Driver Error: Stream ID
                                                                                                                       * @maincause: Invalid Stream ID Parameter Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                       * @mainrecovtext: Check Stream ID Parameter is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_PSIZE            = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 6u),                           /*!< @errcode: 0x3906
                                                                                                                       * @desc: DMA Driver Error: Transfer Size
                                                                                                                       * @maincause: Invalid Transfer Size Parameter Provided (i.e. only 4 or 8 bytes allowed)
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Psize Calibrationibrationculation is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_XCOUNT           = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 7u),                           /*!< @errcode: 0x3907
                                                                                                                       * @desc: DMA Driver Error: xCount
                                                                                                                       * @maincause: Invalid xCount Parameter Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check xCount Parameter is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_YCOUNT           = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0x8u),                         /*!< @errcode: 0x3908
                                                                                                                       * @desc: DMA Driver Error: yCount
                                                                                                                       * @maincause: Invalid yCount Parameter Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check yCount Parameter is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_START_ADDR       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0x9u),                         /*!< @errcode: 0x3909
                                                                                                                       * @desc: DMA Driver Error: Start Address
                                                                                                                       * @maincause: Invalid Start Address Parameter Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Start Address Parameter is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ADDR_ALIGNMENT       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0xAu),                         /*!< @errcode: 0x390A
                                                                                                                       * @desc: DMA Driver Error: Address Alignment
                                                                                                                       * @maincause: MSize Calibrationculation doesnt work with Start Address
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Start Address Parameter is Correctly Aligned
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_BAD_HANDLE           = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0xBu),                         /*!< @errcode: 0x390B
                                                                                                                       * @desc: DMA Driver Error: Handle Validation
                                                                                                                       * @maincause: DMA Stream Handle was outside Valid Range
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                       * @mainrecovtext: Check DMA Stream Handle
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_INVALID_NULL_POINTER = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0xCu),                         /*!< @errcode: 0x390C
                                                                                                                       * @desc: DMA Driver Error: NULL Pointer
                                                                                                                       * @maincause: Invalid Pointer Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_RESET_DEVICE
                                                                                                                       * @mainrecovtext: Check Pointers Passed as Parameters are Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_NO_BUFFER_TO_PROCESS = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DMA_ERROR_CODE_START + 0xDu),                         /*!< @errcode: 0x390D
                                                                                                                       * @desc: DMA Driver Error: Buffer(s)
                                                                                                                       * @maincause: Invalid Data buffer(s) Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                       * @mainrecovtext: Check Data buffer(s) Provided are Valid
                                                                                                                       * @severity:
                                                                                                                       */



    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_CAPTURE Section Base Error Code = 0x3A00------                        @errcode: 0x3A00
     *                                                                                                               @desc: Capture Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_VSWR_PLAYBACK_GENERATOR_ENABLED = (ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_ERROR_CODE_START + 1u),        /*!< @errcode: 0x3A01
                                                                                                                       * @desc: Capture Driver Error: VSWR Playback Generator Enabled
                                                                                                                       * @maincause: VSWR Playback Generator is running when trying to access RAM
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_FEATURE
                                                                                                                       * @mainrecovtext: Playback Generator must be Disabled while accessing VSWR Playback RAM
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_VSWR_PLAYBACK_LENGTH_INVALID    = (ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_ERROR_CODE_START + 2u),        /*!< @errcode: 0x3A02
                                                                                                                       * @desc: Capture Driver Error: VSWR Playback RAM Sample Count
                                                                                                                       * @maincause: Invalid Sample Count Parameter Provided
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                       * @mainrecovtext: Check Sample Count Parameter is Valid
                                                                                                                       * @severity:
                                                                                                                       */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_FRACT_CAP_DELAY_FOR_DEC_CAP     = (ADI_ADRV904X_DFE_SVC_ERR_DRV_CAPTURE_ERROR_CODE_START + 3u),        /*!< @errcode: 0x3A03
                                                                                                                       * @desc: Capture Driver Error: Fractional delay needed to align capture sets
                                                                                                                       * @maincause: Check CFR delay and CT DPD input selection. Fractional delay isn't allowed for capture buffer.
                                                                                                                       * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                       * @mainrecovtext: Check CFR and CT_DPD configuration
                                                                                                                       * @severity:
                                                                                                                       */


    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_HW_IF Section Base Error Code = 0x3B00------                          @errcode: 0x3B00
     *                                                                                                               @desc: Hw If Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    /* Reserve Drv Hw If error codes for Samana platform */
    ADI_ADRV904X_DFE_SVC_ERR_CODE_HW_IF_PLATFORM_START              = (ADI_ADRV904X_DFE_SVC_ERR_DRV_HW_IF_ERROR_CODE_START + 0xF0u),       /*!< @errcode: 0x3BF0
                                                                                                                  * @desc: Hw If Driver Error: Hw If Platform Start
                                                                                                                  * @maincause: Hw If Platform Start
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Parameter is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    ADI_ADRV904X_DFE_SVC_ERR_CODE_HW_IF_PLATFORM_END                = (ADI_ADRV904X_DFE_SVC_ERR_DRV_HW_IF_ERROR_CODE_START + 0xFFu),       /*!< @errcode: 0x3BFF
                                                                                                                  * @desc: Hw If Driver Error: Hw If Platform End
                                                                                                                  * @maincause: Hw If Platform End
                                                                                                                  * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                  * @mainrecovtext: Check Parameter is Valid
                                                                                                                  * @severity:
                                                                                                                  */

    /* ----- Object ID = ADI_ADRV904X_DFE_SVC_ERR_OBJID_DRV_DFE Section Base Error Code = 0x3C00------                            @errcode: 0x3C00
     *                                                                                                               @desc: DFE Driver Errors
     *                                                                                                               @maincause:
     *                                                                                                               @mainrecovenum:
     *                                                                                                               @severity:
     *                                                                                                               @separator: true
     */
    ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_INVALID_DECIMATION_RATIO       = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_ERROR_CODE_START + 1u),             /*!< @errcode: 0x3C01
                                                                                                                   * @desc: DFE Driver Error: Invalid decimation ratio for selected decimator
                                                                                                                   * @maincause: Check if CTC model is configured and decimators in use has non-zero decimation ratios
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check if CTC model is configured and decimators in use has non-zero decimation ratios
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_INVALID_FIRST_STAGE_INT_RATIO  = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_ERROR_CODE_START + 2u),             /*!< @errcode: 0x3C02
                                                                                                                   * @desc: DFE Driver Error: Invalid first stage interpolation ratio
                                                                                                                   * @maincause: Check if CTC model is configured and decimators in use has non-zero first stage interpolator
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check if CTC model is configured and decimators in use has non-zero first stage interpolator
                                                                                                                   * @severity:
                                                                                                                   */

    ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_INVALID_SECOND_STAGE_INT_RATIO = (ADI_ADRV904X_DFE_SVC_ERR_DRV_DFE_ERROR_CODE_START + 3u),             /*!< @errcode: 0x3C03
                                                                                                                   * @desc: DFE Driver Error: Invalid second stage interpolation ratio
                                                                                                                   * @maincause: Check if second stage interpolation ratio is non-zero
                                                                                                                   * @mainrecovenum: ADI_ADRV904X_ERR_ACT_CHECK_PARAM
                                                                                                                   * @mainrecovtext: Check if second stage interpolation ratio is non-zero
                                                                                                                   * @severity:
                                                                                                                   */
} adi_adrv904x_DfeSvcErrCode_e;

typedef uint32_t adi_adrv904x_DfeSvcErrCode_t;

#endif /* __ADI_ADRV904X_DFE_SVC_ERROR_CODES_T_H__ */


