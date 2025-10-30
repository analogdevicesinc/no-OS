/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_error.c
*
* \brief Device Specific Abstractions for Common Error Definitions
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_error.h"

#include "../../private/include/adrv903x_cpu_error_tables.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PUBLIC_ERROR


/* Error Code Tables */

/* API ADRV903X Module Specific Error Tables */

static const adrv903x_CalsErrCodes_t adrv903x_CalsErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_CALS_INIT_ERROR_FLAG,        ADI_STRING("Init Cals Error - Please Perform a InitCalsDetailedStatusGet for more Information"),    ADI_ADRV903X_ERR_ACT_CHECK_FEATURE }
};

static const adrv903x_CpuErrCodes_t adrv903x_CpuErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_CPU_CMD_ID,                  ADI_STRING("CPU Command ID Error"),                                 ADI_ADRV903X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE,            ADI_STRING("CPU Command Response Error"),                           ADI_ADRV903X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_CMD_TIMEOUT,             ADI_STRING("CPU Command Timeout"),                                  ADI_ADRV903X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_IMAGE_NOT_LOADED,        ADI_STRING("CPU Image Write Required before Boot Status Check"),    ADI_ADRV903X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV903X_ERRCODE_CPU_IMAGE_WRITE,             ADI_STRING("CPU Image Write Error"),                                ADI_ADRV903X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_PROFILE_WRITE,           ADI_STRING("CPU Profile Write Error"),                              ADI_ADRV903X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV903X_ERRCODE_CPU_MAILBOX_READ,            ADI_STRING("CPU Mailbox Error"),                                    ADI_ADRV903X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_RAM_ACCESS_START,        ADI_STRING("RAM Access Start Issue"),                               ADI_ADRV903X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV903X_ERRCODE_CPU_RAM_LOCK,                ADI_STRING("RAM Lock Issue"),                                       ADI_ADRV903X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_RAM_ACCESS_STOP,         ADI_STRING("RAM Access Stop Issue"),                                ADI_ADRV903X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV903X_ERRCODE_CPU_PING,                    ADI_STRING("CPU Ping Issue"),                                       ADI_ADRV903X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV903X_ERRCODE_CPU_BOOT_TIMEOUT,            ADI_STRING("CPU Boot Timeout"),                                     ADI_ADRV903X_ERR_ACT_RESET_DEVICE   }
};

/* CPU Boot & Runtime Errors are device error tables provided by firmware */

static const adrv903x_DataInterfaceErrCodes_t adrv903x_DataInterfaceErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_DATAINTERFACE_TEST,      ADI_STRING("Implement Data Interfaces Errors"),     ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_AgcErrCodes_t adrv903x_AgcErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_AGC_TEST, ADI_STRING("Implement AGC Errors"), ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_GpioErrCodes_t adrv903x_GpioErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_GPIO_TEST,               ADI_STRING("Implement Gpio Errors"),      ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_HalErrCodes_t adrv903x_HalErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_HAL_SPI_WRITE,           ADI_STRING("SPI Write Error"),                      ADI_ADRV903X_ERR_ACT_CHECK_INTERFACE },
    { ADI_ADRV903X_ERRCODE_HAL_SPI_READ,            ADI_STRING("SPI Read Error"),                       ADI_ADRV903X_ERR_ACT_CHECK_INTERFACE }
};

static const adrv903x_JesdErrCodes_t adrv903x_JesdErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_JESD_TEST,               ADI_STRING("Implement JESD Errors"),      ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_RadioCtrlErrCodes_t adrv903x_RadioCtrlErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_RADIOCTRL_LO_CFG,        ADI_STRING("Lo Frequency Configuration Error"),     ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_RxErrCodes_t adrv903x_RxErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_RX_TEST,                 ADI_STRING("Implement Rx Errors"),                  ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};

static const adrv903x_TxErrCodes_t adrv903x_TxErrTable[] =
{
    { ADI_ADRV903X_ERRCODE_TX_TEST,                 ADI_STRING("Implement Tx Errors"),                  ADI_ADRV903X_ERR_ACT_CHECK_PARAM }
};
/*****************************************************************************/
/***** Local functions' prototypes *******************************************/
/*****************************************************************************/

/**
* \brief    Service to Lookup a specified Error Table
*
* \param[in]    errTable        Error Table to be Looked Up
* \param[in]    errTableSize    Error Table Size
* \param[in]    errCode         Error Code to be Looked Up
* \param[out]   errMsgPtr       Associated Error Message from Look Up Table
* \param[out]   errActionPtr    Associated Error Action from Look Up Table
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
static adi_adrv903x_ErrAction_e adrv903x_ErrTableLookUp(const adi_adrv903x_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_adrv903x_ErrAction_e* const     errActionPtr);

/**
* \brief    Service to Lookup a specified Device Error Table
*
* \param[in]    errTable        Device Error Table to be Looked Up
* \param[in]    errTableSize    Error Table Size
* \param[in]    errCode         Error Code to be Looked Up
* \param[out]   errMsgPtr       Associated Error Message from Look Up Table
* \param[out]   errCausePtr     Associated Error Cause from Look Up Table
* \param[out]   actionCodePtr   Associated Error Action from Look Up Table
* \param[out]   actionMsgPtr    Associated Error Cause from Look Up Table
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
static adi_adrv903x_ErrAction_e adrv903x_DeviceErrTableLookUp(const adi_adrv903x_DeviceErrTableRow_t  errTable[],
                                                              const size_t                            errTableSize,
                                                              const uint32_t                          errCode,
                                                              const char** const                      errMsgPtr,
                                                              const char** const                      errCausePtr,
                                                              adi_adrv903x_ErrAction_e* const         actionCodePtr,
                                                              const char** const                      actionMsgPtr);


/*****************************************************************************/
/***** Local data types ******************************************************/
/*****************************************************************************/


/*****************************************************************************/
/***** Functions' definition *************************************************/
/*****************************************************************************/
static adi_adrv903x_ErrAction_e adrv903x_ErrTableLookUp(const adi_adrv903x_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_adrv903x_ErrAction_e* const     actionCodePtr)
{
        adi_adrv903x_ErrAction_e    recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t                    idx             = 0U;

    ADI_ADRV903X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionCodePtr);

    if (errTableSize >= sizeof(adi_adrv903x_ErrTableRow_t))
    {
        for (idx = 0U; (idx < (errTableSize / sizeof(adi_adrv903x_ErrTableRow_t))); ++idx)
        {
            if (errCode == errTable[idx].errCode)
            {
                *errMsgPtr      = errTable[idx].errMsg;
                *actionCodePtr  = errTable[idx].action;
                recoveryAction  = ADI_ADRV903X_ERR_ACT_NONE;
                break;
            }
        }
    }

    return recoveryAction;
}

static adi_adrv903x_ErrAction_e adrv903x_DeviceErrTableLookUp(const adi_adrv903x_DeviceErrTableRow_t  errTable[],
                                                              const size_t                            errTableSize,
                                                              const uint32_t                          errCode,
                                                              const char** const                      errMsgPtr,
                                                              const char** const                      errCausePtr,
                                                              adi_adrv903x_ErrAction_e* const         actionCodePtr,
                                                              const char** const                      actionMsgPtr)
{
        adi_adrv903x_ErrAction_e    recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t                    idx             = 0U;

    ADI_ADRV903X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV903X_NULL_PTR_RETURN(errCausePtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionCodePtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionMsgPtr);

    if (errTableSize >= sizeof(adi_adrv903x_DeviceErrTableRow_t))
    {
        for (idx = 0U; (idx < (errTableSize / sizeof(adi_adrv903x_DeviceErrTableRow_t))); ++idx)
        {
            if (errCode == errTable[idx].errCode)
            {
                *errMsgPtr      = errTable[idx].errMsg;
                *errCausePtr    = errTable[idx].errCause;
                *actionCodePtr  = errTable[idx].actionCode;
                *actionMsgPtr   = errTable[idx].actionMsg;
                recoveryAction  = ADI_ADRV903X_ERR_ACT_NONE;
                break;
            }
        }
    }

    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ErrInfoGet(const adi_adrv903x_ErrSource_e      errSrc,
                                                         const int64_t                       errCode,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv903x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Validate Arguments */
    ADI_ADRV903X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV903X_NULL_PTR_RETURN(errCausePtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionCodePtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionMsgPtr);

    /* Default Values */
    *errMsgPtr      = NULL;
    *errCausePtr    = NULL;
    *actionCodePtr  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    *actionMsgPtr   = NULL;

    switch (errSrc)
    {
        case ADI_ADRV903X_ERRSRC_API:
        case ADI_ADRV903X_ERRSRC_HAL:
        case ADI_ADRV903X_ERRSRC_DEVICEBF:
            /* Sources using Common Errors */
            recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_ErrorGet(errCode,
                                                                            errMsgPtr,
                                                                            (adi_common_ErrAction_e* const) actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_DEVICEHAL:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_HalErrTable,
                                                     sizeof(adrv903x_HalErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_CALS:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_CalsErrTable,
                                                     sizeof(adrv903x_CalsErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_CPU_BOOT:
            recoveryAction = adrv903x_DeviceErrTableLookUp( adrv903x_CpuBootErrTable,
                                                            sizeof(adrv903x_CpuBootErrTable),
                                                            errCode,
                                                            errMsgPtr,
                                                            errCausePtr,
                                                            actionCodePtr,
                                                            actionMsgPtr);
            break;

        case ADI_ADRV903X_ERRSRC_CPU_RUNTIME:
            recoveryAction = adrv903x_DeviceErrTableLookUp(adrv903x_CpuRunTimeErrTable,
                                                           sizeof(adrv903x_CpuRunTimeErrTable),
                                                           errCode,
                                                           errMsgPtr,
                                                           errCausePtr,
                                                           actionCodePtr,
                                                           actionMsgPtr);
            break;

        case ADI_ADRV903X_ERRSRC_CPU:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_CpuErrTable,
                                                     sizeof(adrv903x_CpuErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_DATAINTERFACE:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_DataInterfaceErrTable,
                                                     sizeof(adrv903x_DataInterfaceErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;
        
        case ADI_ADRV903X_ERRSRC_AGC:
                recoveryAction = adrv903x_ErrTableLookUp(adrv903x_AgcErrTable,
                                                    sizeof(adrv903x_AgcErrTable),
                                                    errCode,
                                                    errMsgPtr,
                                                    actionCodePtr);
            break;
        case ADI_ADRV903X_ERRSRC_GPIO:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_GpioErrTable,
                                                     sizeof(adrv903x_GpioErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_JESD:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_JesdErrTable,
                                                     sizeof(adrv903x_JesdErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_RADIOCTRL:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_RadioCtrlErrTable,
                                                     sizeof(adrv903x_RadioCtrlErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_RX:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_RxErrTable,
                                                     sizeof(adrv903x_RxErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV903X_ERRSRC_TX:
            recoveryAction = adrv903x_ErrTableLookUp(adrv903x_TxErrTable,
                                                     sizeof(adrv903x_TxErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;
        case ADI_ADRV903X_ERRSRC_NONE:
        case ADI_ADRV903X_ERRSRC_UNKNOWN:
            /* Fall Through */

        default:
            /* Invalid Error Source; No Lookup can be performed */
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;
            break;
    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_ErrDataGet(const adi_adrv903x_Device_t* const  device,
                                                         const adi_common_ErrFrameId_e       frameId,
                                                         adi_adrv903x_ErrSource_e* const     errSrcPtr,
                                                         int64_t* const                      errCodePtr,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv903x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr)
{
        adi_adrv903x_ErrAction_e    recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_common_ErrFrame_t       errFrame        = { ADI_COMMON_ERR_ACT_NONE, { 0, NULL, 0U }, { 0U, NULL, 0U }, { NULL, 0 } };


    /* Validate Arguments */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_NULL_PTR_RETURN(errSrcPtr);

    ADI_ADRV903X_NULL_PTR_RETURN(errCodePtr);

    ADI_ADRV903X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV903X_NULL_PTR_RETURN(actionCodePtr);

    /* Parse Error Frame */
    recoveryAction = (adi_adrv903x_ErrAction_e) ADI_ERROR_FRAME_GET(&device->common, frameId, &errFrame);
    if (ADI_ADRV903X_ERR_ACT_NONE == recoveryAction)
    {
        /* Convert to adrv903x Error Source */
        *errSrcPtr      = (adi_adrv903x_ErrSource_e) errFrame.errInfo.errSrc;
        *errCodePtr     = errFrame.errInfo.errCode;
        recoveryAction  = adi_adrv903x_ErrInfoGet(  *errSrcPtr,
                                                    *errCodePtr,
                                                    errMsgPtr,
                                                    errCausePtr,
                                                    actionCodePtr,
                                                    actionMsgPtr);
    }

    return recoveryAction;
}