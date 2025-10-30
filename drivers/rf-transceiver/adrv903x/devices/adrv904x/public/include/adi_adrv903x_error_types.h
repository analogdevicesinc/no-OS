/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv903x_error_types.h
 * 
 * \brief Device Specific Error Types
 *
 * \details
 *
 * ADRV903X API Version: 2.12.1.4 
 */

#ifndef _ADI_ADRV903X_ERROR_TYPES_H_
#define _ADI_ADRV903X_ERROR_TYPES_H_

#include "adi_library_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv903x_error_type_action.h"

/* Cal Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 100 */
#define ADI_ADRV903X_ERRCODE_CALS_INIT_ERROR_FLAG       100UL       /* Init Cals Run Error Flag */

/* CPU Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 200 */
#define ADI_ADRV903X_ERRCODE_CPU_CMD_ID                 200UL       /* Command ID Error */
#define ADI_ADRV903X_ERRCODE_CPU_CMD_RESPONSE           201UL       /* Command Response Error */
#define ADI_ADRV903X_ERRCODE_CPU_CMD_TIMEOUT            203UL       /* Command Timeout */
#define ADI_ADRV903X_ERRCODE_CPU_IMAGE_NOT_LOADED       204UL       /* CPU Image Not Loaded */
#define ADI_ADRV903X_ERRCODE_CPU_IMAGE_WRITE            205UL       /* CPU Image Write Error */
#define ADI_ADRV903X_ERRCODE_CPU_PROFILE_WRITE          206UL       /* CPU Profile Write Error */
#define ADI_ADRV903X_ERRCODE_CPU_MAILBOX_READ           207UL       /* CPU Mailbox Read Error */
#define ADI_ADRV903X_ERRCODE_CPU_RAM_ACCESS_START       208UL       /* CPU RAM Access Start Error */
#define ADI_ADRV903X_ERRCODE_CPU_RAM_LOCK               209UL       /* RAM Lock/Unlock Error Code */
#define ADI_ADRV903X_ERRCODE_CPU_RAM_ACCESS_STOP        210UL       /* CPU RAM Access Stop Error */
#define ADI_ADRV903X_ERRCODE_CPU_PING                   211UL       /* CPU Ping Error */
#define ADI_ADRV903X_ERRCODE_CPU_BOOT_TIMEOUT           212UL       /* CPU Boot Timeout */

/* Data Interface Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 300 */
#define ADI_ADRV903X_ERRCODE_DATAINTERFACE_TEST         300UL     /* Example */

/* GPIO Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 500 */
#define ADI_ADRV903X_ERRCODE_GPIO_TEST                  500UL     /* Example */

/* Device HAL Error Codes */

#define ADI_ADRV903X_ERRCODE_HAL_SPI_WRITE              600UL     /* SPI Write Error */
#define ADI_ADRV903X_ERRCODE_HAL_SPI_READ               601UL     /* SPI Read Error */
#define ADI_ADRV903X_ERRCODE_HAL_INVALID_DEVICE_STATE   602UL     /* Invalid device state */

/* JESD Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 700 */
#define ADI_ADRV903X_ERRCODE_JESD_TEST                  700UL     /* Example */

/* Radio Control Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 800 */
#define ADI_ADRV903X_ERRCODE_RADIOCTRL_LO_CFG           800UL     /* Lo Frequency Configuration Error */

/* RX Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 900 */
#define ADI_ADRV903X_ERRCODE_RX_TEST                    900UL     /* Example */

/* TX Error Codes */

    /* TODO: Implement High Level Error Codes; Reserved: 1000 */
#define ADI_ADRV903X_ERRCODE_TX_TEST                    1000UL     /* Example */

    /* TODO: Implement High Level Error Codes; Reserved: 1300 */
#define ADI_ADRV903X_ERRCODE_AGC_TEST                   1300UL     /* Example */

/*
* \brief   Device Specific Macro to check for Null Device & Error Pointer
* 
* \param    devicePtr Device Pointer
*/
#define ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(devicePtr)  \
if (devicePtr == NULL)                                  \
{                                                       \
    return ADI_ADRV903X_ERR_ACT_CHECK_PARAM;            \
}

/*
* \brief    Device Specific Macro to check for Null Common Device & Error Pointer
* 
* \param    commonDev Common Device Pointer, which also contains the Error Pointer
*/
#define ADI_ADRV903X_NULL_COMMON_PTR_RETURN(commonDev)  \
if ((commonDev          == NULL) ||                     \
    (commonDev->errPtr  == NULL))                       \
{                                                       \
    return ADI_ADRV903X_ERR_ACT_CHECK_PARAM;            \
}

/*
* \brief    Device Specific Macro to check for Null Pointer
* 
* \param    ptr Pointer to be checked
*/
#define ADI_ADRV903X_NULL_PTR_RETURN(ptr)       \
if (ptr == NULL)                                \
{                                               \
    return ADI_ADRV903X_ERR_ACT_CHECK_PARAM;    \
}

/*
* \brief    Macro to perform the following:
*                       1) Null Pointer Check
*                       2) Report Null Pointer to Error Memory
*                       3) Return ADI_COMMON_ERR_ACT_CHECK_PARAM from API

* \param commonDev  Pointer to the common Device Structure (contains a pointer to Error Memory)
* \param ptr        Pointer to be checked
* 
*/
#define ADI_ADRV903X_NULL_PTR_REPORT_RETURN(commonDev, ptr)         \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_ADRV903X_ERRSRC_API,                \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,       \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        return ADI_ADRV903X_ERR_ACT_CHECK_PARAM;                    \
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
#define ADI_ADRV903X_NULL_PTR_REPORT_GOTO(commonDev, ptr, label)    \
{                                                                   \
    if (ptr == NULL)                                                \
    {                                                               \
        ADI_ERROR_REPORT(   commonDev,                              \
                            ADI_ADRV903X_ERRSRC_API,                \
                            ADI_COMMON_ERRCODE_NULL_PTR,            \
                            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,       \
                            ptr,                                    \
                            "Null Pointer");                        \
                                                                    \
        goto label;                                                 \
    }                                                               \
}

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) Return Device Specific RecoveryAction (i.e. ADI_ADRV903X_ERR_ACT_CHECK_FEATURE)
*/
#define ADI_ADRV903X_API_NOT_IMPLEMENTED_REPORT_RETURN( commonDev)                      \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_ADRV903X_ERRSRC_API,                            \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_ADRV903X_ERR_ACT_CHECK_FEATURE,                 \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                return ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;                              \

/*
* \brief    Macro to perform the following:
*                       1) Report Feature Not Implemented
*                       2) GOTO label specified
*/
#define ADI_ADRV903X_API_NOT_IMPLEMENTED_REPORT_GOTO( commonDev, label)                 \
                                                                                        \
                ADI_ERROR_REPORT(   commonDev,                                          \
                                    ADI_ADRV903X_ERRSRC_API,                            \
                                    ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED,         \
                                    ADI_ADRV903X_ERR_ACT_CHECK_FEATURE,                 \
                                    ADI_NO_VARIABLE,                                    \
                                    "Feature not implemented");                         \
                goto label;                                                             \

/*
* \brief ADRV903X Error Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*/
#define ADI_ADRV903X_ERROR_INFO_GET(errSrc, errCode, variable, recoveryAction, error)               \
{                                                                                                   \
    if (ADI_ADRV903X_ERR_ACT_NONE != adi_adrv903x_ErrInfoGet(errSrc,                                \
                                                            errCode,                                \
                                                            &error.errMsg,                          \
                                                            &error.errCause,                        \
                                                            &error.actionCode,                      \
                                                            &error.actionMsg))                      \
    {                                                                                               \
        /* Report failure of ADRV903X error lookup */                                               \
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;                                        \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            recoveryAction,                                                         \
                            variable,                                                               \
                            "Raw Error Data; Cannot Find More Detailed Information");               \
    }                                                                                               \
}

/*
* \brief ADRV903X Error Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*/
#define ADI_ADRV903X_ERROR_INFO_GET_REPORT(errSrc, errCode, variable, recoveryAction)               \
{                                                                                                   \
    adi_adrv903x_ErrorInfo_t error = { 0, NULL, NULL, ADI_ADRV903X_ERR_ACT_NONE, NULL };            \
                                                                                                    \
    if (ADI_ADRV903X_ERR_ACT_NONE != adi_adrv903x_ErrInfoGet(errSrc,                                \
                                                            errCode,                                \
                                                            &error.errMsg,                          \
                                                            &error.errCause,                        \
                                                            &error.actionCode,                      \
                                                            &error.actionMsg))                      \
    {                                                                                               \
        /* Report failure of ADRV903X error lookup */                                               \
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_FEATURE;                                        \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            recoveryAction,                                                         \
                            variable,                                                               \
                            "Raw Error Data; Cannot Find More Detailed Information");               \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        /* Update Stack Trace */                                                                    \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            error.actionCode,                                                       \
                            variable,                                                               \
                            error.errMsg);                                                          \
        /* Update Additional Debug Information */                                                   \
        ADI_EXT_ERROR_REPORT(   &device->common,                                                    \
                                error.errCause,                                                     \
                                error.actionMsg);                                                   \
        recoveryAction = error.actionCode;                                                          \
    }                                                                                               \
}

/*
* \brief ADRV903X Error Goto Macro
*               1) Error Table Lookup (Report Lookup Error)
*               2) Report Error Information from Lookup
*               3) Update RecoveryAction
*               4) Goto label with Looked Up Action
*/
#define ADI_ADRV903X_ERROR_INFO_GET_REPORT_GOTO(errSrc, errCode, variable, recoveryAction, label)   \
{                                                                                                   \
    adi_adrv903x_ErrorInfo_t error = { 0, NULL, NULL, ADI_ADRV903X_ERR_ACT_NONE, NULL };            \
                                                                                                    \
    if(ADI_ADRV903X_ERR_ACT_NONE != adi_adrv903x_ErrInfoGet(errSrc,                                 \
                                                            errCode,                                \
                                                            &error.errMsg,                          \
                                                            &error.errCause,                        \
                                                            &error.actionCode,                      \
                                                            &error.actionMsg))                      \
    {                                                                                               \
        /* Report failure of ADRV903X error lookup */                                               \
        ADI_ERROR_REPORT(   &device->common,                                                        \
                            errSrc,                                                                 \
                            errCode,                                                                \
                            ADI_ADRV903X_ERR_ACT_CHECK_FEATURE,                                     \
                            variable,                                                               \
                            "Raw Error Data; Cannot Find More Detailed Information");               \
        goto label;                                                                                 \
    }                                                                                               \
    /* Update Stack Trace */                                                                        \
    ADI_ERROR_REPORT(   &device->common,                                                            \
                        errSrc,                                                                     \
                        errCode,                                                                    \
                        error.actionCode,                                                           \
                        variable,                                                                   \
                        error.errMsg);                                                              \
    /* Update Additional Debug Information */                                                       \
    ADI_EXT_ERROR_REPORT(   &device->common,                                                        \
                            error.errCause,                                                         \
                            error.actionMsg)                                                        \
    recoveryAction = error.actionCode;                                                              \
    goto label;                                                                                     \
}
/*
*  \brief   ADRV903X File Abstractions
*/
typedef enum adi_adrv903x_File
{
    ADI_ADRV903X_FILE_PUBLIC_AGC = 0x0U,
    ADI_ADRV903X_FILE_PRIVATE_AGC,
    ADI_ADRV903X_FILE_PUBLIC_CORE,
    ADI_ADRV903X_FILE_PUBLIC_CPU,
    ADI_ADRV903X_FILE_PRIVATE_CPU,
    ADI_ADRV903X_FILE_PUBLIC_CALS,
    ADI_ADRV903X_FILE_PRIVATE_CALS,
    ADI_ADRV903X_FILE_PUBLIC_DATAINTERFACE,
    ADI_ADRV903X_FILE_PRIVATE_DATAINTERFACE,
    ADI_ADRV903X_FILE_PUBLIC_DPD,
    ADI_ADRV903X_FILE_PRIVATE_DPD,
    ADI_ADRV903X_FILE_PUBLIC_ERROR,
    ADI_ADRV903X_FILE_PUBLIC_GPIO,
    ADI_ADRV903X_FILE_PRIVATE_GPIO,
    ADI_ADRV903X_FILE_PUBLIC_HAL,
    ADI_ADRV903X_FILE_PRIVATE_HAL,
    ADI_ADRV903X_FILE_PUBLIC_RADIOCTRL,
    ADI_ADRV903X_FILE_PRIVATE_RADIOCTRL,
    ADI_ADRV903X_FILE_PUBLIC_RX,
    ADI_ADRV903X_FILE_PRIVATE_RX,
    ADI_ADRV903X_FILE_PUBLIC_TX,
    ADI_ADRV903X_FILE_PRIVATE_TX,
    ADI_ADRV903X_FILE_PUBLIC_UTILITIES,
    ADI_ADRV903X_FILE_PUBLIC_VERSION,
    ADI_ADRV903X_FILE_PRIVATE_INIT,
    ADI_ADRV903X_FILE_PRIVATE_SHARED_RESOURCE_MANAGER,
    ADI_ADRV903X_FILE_PUBLIC_JESD,
    ADI_ADRV903X_FILE_PRIVATE_JESD,
    ADI_ADRV903X_FILE_PRIVATE_BINLOADER,
} adi_adrv903x_File_e;

/**
*  \brief   ADRV903X Error Sources - Needs to Map onto Common Sources
*/
typedef enum adi_adrv903x_ErrSource
{
    ADI_ADRV903X_ERRSRC_NONE                = ADI_COMMON_ERRSRC_NONE_IDX,       /*!< No Error/Source; Common Error Source Mapping */
    ADI_ADRV903X_ERRSRC_API                 = ADI_COMMON_ERRSRC_API_IDX,        /*!< API;             Common Error Source Mapping */
    ADI_ADRV903X_ERRSRC_HAL                 = ADI_COMMON_ERRSRC_HAL_IDX,        /*!< HAL;             Common Error Source Mapping */
    ADI_ADRV903X_ERRSRC_DEVICEBF            = ADI_COMMON_ERRSRC_DEVICEBF_IDX,   /*!< Device BF;       Common Error Source Mapping */
    ADI_ADRV903X_ERRSRC_AGC,                                                    /*!< AGC Error */
    ADI_ADRV903X_ERRSRC_CALS,                                                   /*!< Calibration Error */
    ADI_ADRV903X_ERRSRC_CPU_BOOT,                                               /*!< CPU Boot Error */
    ADI_ADRV903X_ERRSRC_CPU_RUNTIME,                                            /*!< CPU Runtime Error */
    ADI_ADRV903X_ERRSRC_CPU,                                                    /*!< CPU Error */
    ADI_ADRV903X_ERRSRC_DATAINTERFACE,                                          /*!< Data Interface Error */
    ADI_ADRV903X_ERRSRC_GPIO,                                                   /*!< GPIO Error */
    ADI_ADRV903X_ERRSRC_DEVICEHAL,                                              /*!< Device HAL Error */
    ADI_ADRV903X_ERRSRC_JESD,                                                   /*!< JESD Error */
    ADI_ADRV903X_ERRSRC_RADIOCTRL,                                              /*!< Radio Control Error */
    ADI_ADRV903X_ERRSRC_RX,                                                     /*!< RX Error */
    ADI_ADRV903X_ERRSRC_TX,                                                     /*!< TX Error */
    ADI_ADRV903X_ERRSRC_UNKNOWN                                                 /*!< Unknown */
} adi_adrv903x_ErrSource_e;

/**
*  \brief ADI ADRV903X Error Table Row
*/
typedef struct adi_adrv903x_ErrTableRow
{
    const int64_t                   errCode; /*!< Error code */
    const char* const               errMsg;  /*!< Error message */
    const adi_adrv903x_ErrAction_e  action;  /*!< Error action */
} adi_adrv903x_ErrTableRow_t;

/**
*  \brief ADI ADRV903X Device Error Table Row
*/
typedef struct adi_adrv903x_DeviceErrTableRow
{
    const int64_t                   errCode;    /*!< Error code */
    const char* const               errMsg;     /*!< Error message */
    const char* const               errCause;   /*!< Error cause */
    const adi_adrv903x_ErrAction_e  actionCode; /*!< Error action */
    const char* const               actionMsg;  /*!< Error msg */
} adi_adrv903x_DeviceErrTableRow_t;

/**
*  \brief ADI Common Error Information
*/
typedef struct adi_adrv903x_ErrorInfo
{
    int64_t                     errCode;        /*!< Error code */
    const char*                 errMsg;         /*!< Error msg */
    const char*                 errCause;       /*!< Error cause */
    adi_adrv903x_ErrAction_e    actionCode;     /*!< Error action */
    const char*                 actionMsg;      /*!< Error msg */
} adi_adrv903x_ErrorInfo_t;

/**
*  \brief ADI ADRV903X Error Action Union
*/
typedef union adi_adrv903x_ErrActionUnion
{
    adi_adrv903x_ErrAction_e    deviceAction;   /*!< ADRV903X Error Action Type */
    adi_common_ErrAction_t      common;         /*!< Common Error Action Union */
} adi_adrv903x_ErrActionUnion_t;

/* Type Definitions for Device Error Tables */
typedef adi_adrv903x_ErrTableRow_t          adrv903x_CalsErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_CpuErrCodes_t;

typedef adi_adrv903x_DeviceErrTableRow_t    adrv903x_CpuBootErrCodes_t;

typedef adi_adrv903x_DeviceErrTableRow_t    adrv903x_CpuRuntimeErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_DataInterfaceErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_AgcErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_GpioErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_HalErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_JesdErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_RadioCtrlErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_RxErrCodes_t;

typedef adi_adrv903x_ErrTableRow_t          adrv903x_TxErrCodes_t;
#endif /* _ADI_ADRV903X_ERROR_TYPES_H_ */
