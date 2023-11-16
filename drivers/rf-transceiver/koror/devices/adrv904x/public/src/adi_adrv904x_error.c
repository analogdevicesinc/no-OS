/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_error.c
*
* \brief Device Specific Abstractions for Common Error Definitions
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_error.h"

#include "../../private/include/adrv904x_cpu_error_tables.h"


#include "../../private/include/adrv904x_dfe_app_error_tables.h"
#include "../../private/include/adrv904x_dfe_svc_error_tables.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_ERROR


/* Error Code Tables */

/* API ADRV904X Module Specific Error Tables */

static const adrv904x_CalsErrCodes_t adrv904x_CalsErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_CALS_INIT_ERROR_FLAG,        ADI_STRING("Init Cals Error - Please Perform a InitCalsDetailedStatusGet for more Information"),    ADI_ADRV904X_ERR_ACT_CHECK_FEATURE }
};

static const adrv904x_CpuErrCodes_t adrv904x_CpuErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_CPU_CMD_ID,                  ADI_STRING("CPU Command ID Error"),                                 ADI_ADRV904X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV904X_ERRCODE_CPU_CMD_RESPONSE,            ADI_STRING("CPU Command Response Error"),                           ADI_ADRV904X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_CMD_TIMEOUT,             ADI_STRING("CPU Command Timeout"),                                  ADI_ADRV904X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_IMAGE_NOT_LOADED,        ADI_STRING("CPU Image Write Required before Boot Status Check"),    ADI_ADRV904X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV904X_ERRCODE_CPU_IMAGE_WRITE,             ADI_STRING("CPU Image Write Error"),                                ADI_ADRV904X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_PROFILE_WRITE,           ADI_STRING("CPU Profile Write Error"),                              ADI_ADRV904X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV904X_ERRCODE_CPU_MAILBOX_READ,            ADI_STRING("CPU Mailbox Error"),                                    ADI_ADRV904X_ERR_ACT_CHECK_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_RAM_ACCESS_START,        ADI_STRING("RAM Access Start Issue"),                               ADI_ADRV904X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV904X_ERRCODE_CPU_RAM_LOCK,                ADI_STRING("RAM Lock Issue"),                                       ADI_ADRV904X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_RAM_ACCESS_STOP,         ADI_STRING("RAM Access Stop Issue"),                                ADI_ADRV904X_ERR_ACT_CHECK_PARAM    },
    { ADI_ADRV904X_ERRCODE_CPU_PING,                    ADI_STRING("CPU Ping Issue"),                                       ADI_ADRV904X_ERR_ACT_RESET_FEATURE  },
    { ADI_ADRV904X_ERRCODE_CPU_BOOT_TIMEOUT,            ADI_STRING("CPU Boot Timeout"),                                     ADI_ADRV904X_ERR_ACT_RESET_DEVICE   }
};

/* CPU Boot & Runtime Errors are device error tables provided by firmware */

static const adrv904x_DataInterfaceErrCodes_t adrv904x_DataInterfaceErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_DATAINTERFACE_TEST,      ADI_STRING("Implement Data Interfaces Errors"),     ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_AgcErrCodes_t adrv904x_AgcErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_AGC_TEST, ADI_STRING("Implement AGC Errors"), ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_GpioErrCodes_t adrv904x_GpioErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_GPIO_TEST,               ADI_STRING("Implement Gpio Errors"),      ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_HalErrCodes_t adrv904x_HalErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_HAL_SPI_WRITE,           ADI_STRING("SPI Write Error"),                      ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE },
    { ADI_ADRV904X_ERRCODE_HAL_SPI_READ,            ADI_STRING("SPI Read Error"),                       ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE }
};

static const adrv904x_JesdErrCodes_t adrv904x_JesdErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_JESD_TEST,               ADI_STRING("Implement JESD Errors"),      ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_RadioCtrlErrCodes_t adrv904x_RadioCtrlErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_RADIOCTRL_LO_CFG,        ADI_STRING("Lo Frequency Configuration Error"),     ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_RxErrCodes_t adrv904x_RxErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_RX_TEST,                 ADI_STRING("Implement Rx Errors"),                  ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};

static const adrv904x_TxErrCodes_t adrv904x_TxErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_TX_TEST,                 ADI_STRING("Implement Tx Errors"),                  ADI_ADRV904X_ERR_ACT_CHECK_PARAM }
};
static const adrv904x_DfeCpuErrCodes_t adrv904x_DfeCpuErrTable[] =
{
    { ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_ID,              ADI_STRING("DFE CPU Command ID Error"),                                 ADI_ADRV904X_ERR_ACT_CHECK_PARAM   },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_RESPONSE,        ADI_STRING("DFE CPU Command Response Error"),                           ADI_ADRV904X_ERR_ACT_RESET_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_CMD_TIMEOUT,         ADI_STRING("DFE CPU Command Timeout"),                                  ADI_ADRV904X_ERR_ACT_CHECK_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_IMAGE_NOT_LOADED,    ADI_STRING("DFE CPU Image Write Required before Boot Status Check"),    ADI_ADRV904X_ERR_ACT_CHECK_PARAM   },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_IMAGE_WRITE,         ADI_STRING("DFE CPU Image Write Error"),                                ADI_ADRV904X_ERR_ACT_CHECK_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_PROFILE_WRITE,       ADI_STRING("DFE CPU Profile Write Error"),                              ADI_ADRV904X_ERR_ACT_CHECK_PARAM   },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_MAILBOX_READ,        ADI_STRING("DFE CPU Mailbox Error"),                                    ADI_ADRV904X_ERR_ACT_CHECK_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_RAM_ACCESS_START,    ADI_STRING("DFE CPU RAM Access Start Issue"),                           ADI_ADRV904X_ERR_ACT_CHECK_PARAM   },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_RAM_LOCK,            ADI_STRING("DFE CPU RAM Lock Issue"),                                   ADI_ADRV904X_ERR_ACT_RESET_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_RAM_ACCESS_STOP,     ADI_STRING("DFE CPU RAM Access Stop Issue"),                            ADI_ADRV904X_ERR_ACT_CHECK_PARAM   },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_PING,                ADI_STRING("DFE CPU Ping Issue"),                                       ADI_ADRV904X_ERR_ACT_RESET_FEATURE },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_BOOT_TIMEOUT,        ADI_STRING("DFE CPU Boot Timeout"),                                     ADI_ADRV904X_ERR_ACT_RESET_DEVICE  },
    { ADI_ADRV904X_ERRCODE_DFE_CPU_BOOT_ERROR,          ADI_STRING("DFE CPU Boot error"),                                       ADI_ADRV904X_ERR_ACT_RESET_DEVICE  }
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
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
static adi_adrv904x_ErrAction_e adrv904x_ErrTableLookUp(const adi_adrv904x_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_adrv904x_ErrAction_e* const     errActionPtr);

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
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
static adi_adrv904x_ErrAction_e adrv904x_DeviceErrTableLookUp(const adi_adrv904x_DeviceErrTableRow_t  errTable[],
                                                              const size_t                            errTableSize,
                                                              const uint32_t                          errCode,
                                                              const char** const                      errMsgPtr,
                                                              const char** const                      errCausePtr,
                                                              adi_adrv904x_ErrAction_e* const         actionCodePtr,
                                                              const char** const                      actionMsgPtr);


/*****************************************************************************/
/***** Local data types ******************************************************/
/*****************************************************************************/


/*****************************************************************************/
/***** Functions' definition *************************************************/
/*****************************************************************************/
static adi_adrv904x_ErrAction_e adrv904x_ErrTableLookUp(const adi_adrv904x_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_adrv904x_ErrAction_e* const     actionCodePtr)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                    idx             = 0U;

    ADI_ADRV904X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionCodePtr);

    if (errTableSize >= sizeof(adi_adrv904x_ErrTableRow_t))
    {
        for (idx = 0U; (idx < (errTableSize / sizeof(adi_adrv904x_ErrTableRow_t))); ++idx)
        {
            if (errCode == errTable[idx].errCode)
            {
                *errMsgPtr      = errTable[idx].errMsg;
                *actionCodePtr  = errTable[idx].action;
                recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            }
        }
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_DeviceErrTableLookUp(const adi_adrv904x_DeviceErrTableRow_t  errTable[],
                                                              const size_t                            errTableSize,
                                                              const uint32_t                          errCode,
                                                              const char** const                      errMsgPtr,
                                                              const char** const                      errCausePtr,
                                                              adi_adrv904x_ErrAction_e* const         actionCodePtr,
                                                              const char** const                      actionMsgPtr)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                    idx             = 0U;

    ADI_ADRV904X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV904X_NULL_PTR_RETURN(errCausePtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionCodePtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionMsgPtr);

    if (errTableSize >= sizeof(adi_adrv904x_DeviceErrTableRow_t))
    {
        for (idx = 0U; (idx < (errTableSize / sizeof(adi_adrv904x_DeviceErrTableRow_t))); ++idx)
        {
            if (errCode == errTable[idx].errCode)
            {
                *errMsgPtr      = errTable[idx].errMsg;
                *errCausePtr    = errTable[idx].errCause;
                *actionCodePtr  = errTable[idx].actionCode;
                *actionMsgPtr   = errTable[idx].actionMsg;
                recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            }
        }
    }

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ErrInfoGet(const adi_adrv904x_ErrSource_e      errSrc,
                                                         const int64_t                       errCode,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv904x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Validate Arguments */
    ADI_ADRV904X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV904X_NULL_PTR_RETURN(errCausePtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionCodePtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionMsgPtr);

    /* Default Values */
    *errMsgPtr      = NULL;
    *errCausePtr    = NULL;
    *actionCodePtr  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    *actionMsgPtr   = NULL;

    switch (errSrc)
    {
        case ADI_ADRV904X_ERRSRC_API:
        case ADI_ADRV904X_ERRSRC_HAL:
        case ADI_ADRV904X_ERRSRC_DEVICEBF:
            /* Sources using Common Errors */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_ErrorGet(errCode,
                                                                            errMsgPtr,
                                                                            (adi_common_ErrAction_e* const) actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_DEVICEHAL:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_HalErrTable,
                                                     sizeof(adrv904x_HalErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_CALS:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_CalsErrTable,
                                                     sizeof(adrv904x_CalsErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_CPU_BOOT:
            recoveryAction = adrv904x_DeviceErrTableLookUp( adrv904x_CpuBootErrTable,
                                                            sizeof(adrv904x_CpuBootErrTable),
                                                            errCode,
                                                            errMsgPtr,
                                                            errCausePtr,
                                                            actionCodePtr,
                                                            actionMsgPtr);
            break;

        case ADI_ADRV904X_ERRSRC_CPU_RUNTIME:
            recoveryAction = adrv904x_DeviceErrTableLookUp(adrv904x_CpuRunTimeErrTable,
                                                           sizeof(adrv904x_CpuRunTimeErrTable),
                                                           errCode,
                                                           errMsgPtr,
                                                           errCausePtr,
                                                           actionCodePtr,
                                                           actionMsgPtr);
            break;

        case ADI_ADRV904X_ERRSRC_CPU:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_CpuErrTable,
                                                     sizeof(adrv904x_CpuErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_DATAINTERFACE:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_DataInterfaceErrTable,
                                                     sizeof(adrv904x_DataInterfaceErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;
        
        case ADI_ADRV904X_ERRSRC_AGC:
                recoveryAction = adrv904x_ErrTableLookUp(adrv904x_AgcErrTable,
                                                    sizeof(adrv904x_AgcErrTable),
                                                    errCode,
                                                    errMsgPtr,
                                                    actionCodePtr);
            break;
        case ADI_ADRV904X_ERRSRC_GPIO:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_GpioErrTable,
                                                     sizeof(adrv904x_GpioErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_JESD:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_JesdErrTable,
                                                     sizeof(adrv904x_JesdErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_RADIOCTRL:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_RadioCtrlErrTable,
                                                     sizeof(adrv904x_RadioCtrlErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_RX:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_RxErrTable,
                                                     sizeof(adrv904x_RxErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_TX:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_TxErrTable,
                                                     sizeof(adrv904x_TxErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;
        case ADI_ADRV904X_ERRSRC_DFE_CPU_BOOT:
            recoveryAction = adrv904x_DeviceErrTableLookUp(adrv904x_DfeCpuBootErrTable,
                                                           sizeof(adrv904x_DfeCpuBootErrTable),
                                                           errCode,
                                                           errMsgPtr,
                                                           errCausePtr,
                                                           actionCodePtr,
                                                           actionMsgPtr);
            break;

        case ADI_ADRV904X_ERRSRC_DFE_CPU:
            recoveryAction = adrv904x_ErrTableLookUp(adrv904x_DfeCpuErrTable,
                                                     sizeof(adrv904x_DfeCpuErrTable),
                                                     errCode,
                                                     errMsgPtr,
                                                     actionCodePtr);
            break;

        case ADI_ADRV904X_ERRSRC_DFE_APP_RUNTIME:
            recoveryAction = adrv904x_DeviceErrTableLookUp(adrv904x_DfeAppRunTimeErrTable,
                                                           sizeof(adrv904x_DfeAppRunTimeErrTable),
                                                           errCode,
                                                           errMsgPtr,
                                                           errCausePtr,
                                                           actionCodePtr,
                                                           actionMsgPtr);
            break;

        case ADI_ADRV904X_ERRSRC_DFE_SVC_RUNTIME:
            recoveryAction = adrv904x_DeviceErrTableLookUp(adrv904x_DfeSvcRunTimeErrTable,
                                                           sizeof(adrv904x_DfeSvcRunTimeErrTable),
                                                           errCode,
                                                           errMsgPtr,
                                                           errCausePtr,
                                                           actionCodePtr,
                                                           actionMsgPtr);
            break;
        case ADI_ADRV904X_ERRSRC_NONE:
        case ADI_ADRV904X_ERRSRC_UNKNOWN:
            /* Fall Through */

        default:
            /* Invalid Error Source; No Lookup can be performed */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            break;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ErrDataGet(const adi_adrv904x_Device_t* const  device,
                                                         const adi_common_ErrFrameId_e       frameId,
                                                         adi_adrv904x_ErrSource_e* const     errSrcPtr,
                                                         int64_t* const                      errCodePtr,
                                                         const char** const                  errMsgPtr,
                                                         const char** const                  errCausePtr,
                                                         adi_adrv904x_ErrAction_e* const     actionCodePtr,
                                                         const char** const                  actionMsgPtr)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_common_ErrFrame_t       errFrame        = { ADI_COMMON_ERR_ACT_NONE, { 0, NULL, 0U }, { 0U, NULL, 0U }, { NULL, 0 } };


    /* Validate Arguments */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_NULL_PTR_RETURN(errSrcPtr);

    ADI_ADRV904X_NULL_PTR_RETURN(errCodePtr);

    ADI_ADRV904X_NULL_PTR_RETURN(errMsgPtr);

    ADI_ADRV904X_NULL_PTR_RETURN(actionCodePtr);

    /* Parse Error Frame */
    recoveryAction = (adi_adrv904x_ErrAction_e) ADI_ERROR_FRAME_GET(&device->common, frameId, &errFrame);
    if (ADI_ADRV904X_ERR_ACT_NONE == recoveryAction)
    {
        /* Convert to adrv904x Error Source */
        *errSrcPtr      = (adi_adrv904x_ErrSource_e) errFrame.errInfo.errSrc;
        *errCodePtr     = errFrame.errInfo.errCode;
        recoveryAction  = adi_adrv904x_ErrInfoGet(  *errSrcPtr,
                                                    *errCodePtr,
                                                    errMsgPtr,
                                                    errCausePtr,
                                                    actionCodePtr,
                                                    actionMsgPtr);
    }

    return recoveryAction;
}