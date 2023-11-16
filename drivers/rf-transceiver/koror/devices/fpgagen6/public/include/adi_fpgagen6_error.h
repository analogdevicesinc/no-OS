/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_error.h
 *
 * \brief Device Specific Abstractions for Common Error Declarations
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_ERROR_H_
#define _ADI_FPGAGEN6_ERROR_H_

#include "adi_fpgagen6_error_types.h"
#include "adi_fpgagen6_types.h"

/*
* \brief   Device Specific Macro to check for Null Device & Error Pointer
* 
* \param    devicePtr Device Pointer
*/
#define ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(devicePtr)  \
if (devicePtr == NULL)                                  \
{                                                       \
    return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;            \
}

/*
* \brief    Device Specific Macro to check for Null Common Device & Error Pointer
* 
* \param    commonDev Common Device Pointer, which also contains the Error Pointer
*/
#define ADI_FPGAGEN6_NULL_COMMON_PTR_RETURN(commonDev)  \
if ((commonDev          == NULL) ||                     \
    (commonDev->errPtr  == NULL))                       \
{                                                       \
    return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;            \
}

/*
* \brief    Device Specific Macro to check for Null Pointer
* 
* \param    ptr Pointer to be checked
*/
#define ADI_FPGAGEN6_NULL_PTR_RETURN(ptr)       \
if (ptr == NULL)                                \
{                                               \
    return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;    \
}

/*
* \brief    Macro to perform the following:
*                       1) Null Pointer Check
*                       2) Report Null Pointer to Error Memory
*                       3) Return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM from API

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param ptr        Pointer to be checked
* 
*/
#define ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(commonDev, ptr)         \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_FPGAGEN6_ERRSRC_API,                \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM,       \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;                    \
    }                                                               \
}

/*
* \brief    Macro to perform the following:
*                       1) Null Pointer Check
*                       2) Report Null Pointer to Error Memory
*                       3) GOTO label specified

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param ptr        Pointer to be checked
* \param label      Label for GOTO
* 
*/
#define ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(commonDev, ptr, label)    \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_FPGAGEN6_ERRSRC_API,                \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM,       \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        goto label;                                                 \
    }                                                               \
}

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) Return Device Specific RecoveryAction (i.e. ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE)
*/
#define ADI_FPGAGEN6_API_NOT_IMPLEMENTED_REPORT_RETURN( commonDev)                      \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_FPGAGEN6_ERRSRC_API,                            \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE,                 \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                return ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE;                              \

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) GOTO label specified
*/
#define ADI_FPGAGEN6_API_NOT_IMPLEMENTED_REPORT_GOTO( commonDev, label)                 \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_FPGAGEN6_ERRSRC_API,                            \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE,                 \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                goto label;                                                             \

/*
* \brief FPGAGEN6 Error Return Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*               4) Return using Looked Up Action
*/
#define ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(errSrc, errCode, variable, recoveryAction)        \
{                                                                                                   \
    adi_fpgagen6_ErrorInfo_t error = { 0, NULL, NULL, ADI_FPGAGEN6_ERR_ACT_NONE, NULL };            \
                                                                                                    \
    if (ADI_FPGAGEN6_ERR_ACT_NONE != adi_fpgagen6_ErrInfoGet(   errSrc,                             \
                                                                errCode,                            \
                                                                &error.errMsg,                      \
                                                                &error.actionCode))                 \
    {                                                                                               \
        /* Report failure of FPGAGEN6 error lookup */                                               \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE,                                     \
                            variable,                                                               \
                            "FPGA Error Table Look up Issue");                                      \
        return ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE;                                                  \
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
* \brief FPGAGEN6 Error Goto Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*               4) Goto label with Looked Up Action
*/
#define ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(errSrc, errCode, variable, recoveryAction, label)   \
{                                                                                                   \
    adi_fpgagen6_ErrorInfo_t error = { 0, NULL, NULL, ADI_FPGAGEN6_ERR_ACT_NONE, NULL };            \
                                                                                                    \
    if (ADI_FPGAGEN6_ERR_ACT_NONE != adi_fpgagen6_ErrInfoGet(   errSrc,                             \
                                                                errCode,                            \
                                                                &error.errMsg,                      \
                                                                &error.actionCode))                 \
    {                                                                                               \
        /* Report failure of FPGAGEN6 error lookup */                                               \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE,                                     \
                            variable,                                                               \
                            "FPGA Error Table Look up Issue");                                      \
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
 * \retval ADI_FPGAGEN6_ERR_ACT_NONE if Lookup was Successful
 */
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_ErrInfoGet(   const adi_fpgagen6_ErrSource_e  errSrc,
                                                            const int64_t                   errCode,
                                                            const char** const              errMsgPtr,
                                                            adi_fpgagen6_ErrAction_e* const actionPtr);

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
* \retval ADI_FPGAGEN6_ERR_ACT_NONE if Lookup was Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_ErrDataGet(   const adi_fpgagen6_Device_t* const  device,
                                                            const adi_common_ErrFrameId_e       frameId,
                                                            adi_fpgagen6_ErrSource_e* const     errSrcPtr,
                                                            int64_t* const                      errCodePtr,
                                                            const char** const                  errMsgPtr,
                                                            adi_fpgagen6_ErrAction_e* const     actionPtr);


#endif /* _ADI_FPGA_ERROR_H_ */