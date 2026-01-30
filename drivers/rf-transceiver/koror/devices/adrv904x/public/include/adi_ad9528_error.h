/**
* Copyright 2020 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_ad9528_error.h
*
* \brief Device Specific Abstractions for Common Error Declarations
*
*/

#ifndef _ADI_AD9528_ERROR_H_
#define _ADI_AD9528_ERROR_H_

#include "adi_ad9528_types.h"
#include "adi_ad9528_error_types.h"

/*
* \brief AD9528 Error Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*               4) Return using Looked Up Action
*/
#define ADI_AD9528_ERROR_INFO_GET_REPORT_RETURN(errSrc, errCode, variable, recoveryAction)          \
{                                                                                                   \
    adi_common_ErrorInfo_t error = { 0, NULL, NULL, ADI_COMMON_ERR_ACT_NONE, NULL };                \
                                                                                                    \
    if(ADI_COMMON_ERR_ACT_NONE != adi_ad9528_ErrInfoGet(errSrc,                                     \
                                                        errCode,                                    \
                                                        &error.errMsg,                              \
                                                        &error.actionCode))                         \
    {                                                                                               \
        /* Report failure of AD9528 error lookup */                                                 \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            ADI_COMMON_ERR_ACT_CHECK_FEATURE,                                       \
                            variable,                                                               \
                            "AD9528 Error Table Look up Issue");                                    \
        return ADI_COMMON_ERR_ACT_CHECK_FEATURE;                                                    \
    }                                                                                               \
    /* Update Stack Trace */                                                                        \
    ADI_ERROR_REPORT(   &device->common,                                                            \
                        errSrc,                                                                     \
                        errCode,                                                                    \
                        error.actionCode,                                                           \
                        variable,                                                                   \
                        error.errMsg);                                                              \
    recoveryAction = error.actionCode;                                                              \
    return recoveryAction;                                                                          \
}

/*
* \brief AD9528 Error Goto Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*               4) Goto label with Looked Up Action
*/
#define ADI_AD9528_ERROR_INFO_GET_REPORT_GOTO(errSrc, errCode, variable, recoveryAction, label)     \
{                                                                                                   \
    adi_common_ErrorInfo_t error = { 0, NULL, NULL, ADI_COMMON_ERR_ACT_NONE, NULL };                \
                                                                                                    \
    if(ADI_COMMON_ERR_ACT_NONE != adi_ad9528_ErrInfoGet(errSrc,                                     \
                                                        errCode,                                    \
                                                        &error.errMsg,                              \
                                                        &error.actionCode))                         \
    {                                                                                               \
        /* Report failure of AD9528 error lookup */                                                 \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            ADI_COMMON_ERR_ACT_CHECK_FEATURE,                                       \
                            variable,                                                               \
                            "AD9528 Error Table Look up Issue");                                    \
        goto label;                                                                                 \
    }                                                                                               \
    /* Update Stack Trace */                                                                        \
    ADI_ERROR_REPORT(   &device->common,                                                            \
                        errSrc,                                                                     \
                        errCode,                                                                    \
                        error.actionCode,                                                           \
                        variable,                                                                   \
                        error.errMsg);                                                              \
    recoveryAction = error.actionCode;                                                              \
    goto label;                                                                                     \
}

/**
 * \brief Service to Lookup the Error Message and Recovery Action for an Error Code
 * 
 * \param[in]   errSrc      Error Source for the Device Type
 * \param[in]   errCode     Error Code to be Looked Up
 * \param[out]  errMsgPtr   Associated Error Message
 * \param[out]  actionPtr   Associated Recovery Action for the Error
 * 
 * \retval ADI_COMMON_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_common_ErrAction_e adi_ad9528_ErrInfoGet(   const adi_ad9528_ErrSource_e        errSrc,
                                                        const int64_t                       errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_common_ErrAction_e* const       actionPtr);

/**
 * \brief   Service to Parse Error Data from the Error Structure
 * 
 * \param[in]   device      Pointer to the device
 * \param[in]   frameId     Stack Trace Frame ID
 * \param[out]  errSrcPtr   Error Source for the Device Type
 * \param[out]  errCodePtr  Error Code
 * \param[out]  errMsgPtr   Associated Error Message
 * \param[out]  actionPtr   Associated Recovery Action for the Error
 * 
 * \retval ADI_COMMON_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_common_ErrAction_e adi_ad9528_ErrDataGet(   const adi_ad9528_Device_t* const    device,
                                                        const adi_common_ErrFrameId_e       frameId,
                                                        adi_ad9528_ErrSource_e* const       errSrcPtr,
                                                        int64_t* const                      errCodePtr,
                                                        const char** const                  errMsgPtr,
                                                        adi_common_ErrAction_e* const       actionPtr);



#endif /* _ADI_AD9528_ERROR_H_ */
