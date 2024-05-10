/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_error_types.h
* \brief Contains common error data types for API Error messaging
*
* ADI common lib Version: 0.0.2.1
*/

#ifndef _ADI_COMMON_ERROR_TYPES_H_
#define _ADI_COMMON_ERROR_TYPES_H_

#include "adi_library.h"

/* adi_common_ErrFrameId_e Defines Stack Trace Size because of how error frames can be accessed */
#define ADI_COMMON_STACK_TRACE_SIZE     ADI_COMMON_ERRFRAME_MAX

/**
*  \brief   ADI Common Source Indexing
*/
#define ADI_COMMON_ERRSRC_NONE_IDX          0U
#define ADI_COMMON_ERRSRC_API_IDX           1U
#define ADI_COMMON_ERRSRC_HAL_IDX           2U
#define ADI_COMMON_ERRSRC_DEVICEHAL_IDX     3U
#define ADI_COMMON_ERRSRC_DEVICEBF_IDX      4U
#define ADI_COMMON_ERRSRC_APP_IDX           5U

#define ADI_COMMON_ERRSRC_RESERVED_IDX      99U

/**
*  \brief   ADI Common Error Codes
*/
#define ADI_COMMON_ERRCODE_NONE                     0UL     /* No Error */
#define ADI_COMMON_ERRCODE_INVALID_PARAM            1UL     /* Invalid Parameter */
#define ADI_COMMON_ERRCODE_NULL_PTR                 2UL     /* Null Pointer */
#define ADI_COMMON_ERRCODE_ARRAY_SIZE               4UL     /* Array Size not correct */
#define ADI_COMMON_ERRCODE_LOCK                     5UL     /* Lock Error */
#define ADI_COMMON_ERRCODE_API                      6UL     /* API Call Error */
#define ADI_COMMON_ERRCODE_FEATURE_NOT_IMPLEMENTED  7UL     /* ADI Feature not implemented */
#define ADI_COMMON_ERRCODE_TIMEOUT                  8UL     /* Timeout */
#define ADI_COMMON_ERRCODE_BOOT_TIMEOUT             9UL     /* Boot Timeout */

/**
*  \brief   ADI Common Sources
*/
typedef enum adi_common_ErrSources
{
    ADI_COMMON_ERRSRC_NONE      = ADI_COMMON_ERRSRC_NONE_IDX,           /* No Error/Source */
    ADI_COMMON_ERRSRC_API       = ADI_COMMON_ERRSRC_API_IDX,            /* API Error */
    ADI_COMMON_ERRSRC_HAL       = ADI_COMMON_ERRSRC_HAL_IDX,            /* HAL Error */
    ADI_COMMON_ERRSRC_DEVICEHAL = ADI_COMMON_ERRSRC_DEVICEHAL_IDX,      /* Device HAL Error */
    ADI_COMMON_ERRSRC_DEVICEBF  = ADI_COMMON_ERRSRC_DEVICEBF_IDX,       /* Device BF Error */
    ADI_COMMON_ERRSRC_APP       = ADI_COMMON_ERRSRC_APP_IDX             /* Application Error */
} adi_common_ErrSources_e;

/*
*  \brief ADI Common Error Frame Indexing
*/
typedef enum
{
    ADI_COMMON_ERRFRAME_IDX_0   = 0U,                           /* Error Frame Index 0 */
    ADI_COMMON_ERRFRAME_IDX_1   = 1U,                           /* Error Frame Index 1 */
    ADI_COMMON_ERRFRAME_IDX_2   = 2U,                           /* Error Frame Index 2 */
    ADI_COMMON_ERRFRAME_IDX_3   = 3U,                           /* Error Frame Index 3 */
    ADI_COMMON_ERRFRAME_IDX_4   = 4U,                           /* Error Frame Index 4 */
    ADI_COMMON_ERRFRAME_IDX_5   = 5U,                           /* Error Frame Index 5 */
    ADI_COMMON_ERRFRAME_IDX_6   = 6U,                           /* Error Frame Index 6 */
    ADI_COMMON_ERRFRAME_IDX_7   = 7U,                           /* Error Frame Index 7 */
    ADI_COMMON_ERRFRAME_IDX_8   = 8U,                           /* Error Frame Index 8 */
    ADI_COMMON_ERRFRAME_IDX_9   = 9U,                           /* Error Frame Index 9 */
    ADI_COMMON_ERRFRAME_EVENT   = ADI_COMMON_ERRFRAME_IDX_0,    /* Start of Array; Fixed */
    ADI_COMMON_ERRFRAME_END     = ADI_COMMON_ERRFRAME_IDX_9,    /* End of Array; Fixed */
    ADI_COMMON_ERRFRAME_MAX,                                    /* Stack Trace Size */
    ADI_COMMON_ERRFRAME_API     = ADI_COMMON_ERRFRAME_MAX       /* Last Entry to Stack Trace; Variable */
} adi_common_ErrFrameId_e;

/*
*  \brief ADI COMMON Recovery Actions
*/
typedef enum
{
    ADI_COMMON_ERR_ACT_RESET_DEVICE     = -500,     /* Device NOK: HW/SW Reset Required */

    /* All Reset Feature Unique Codes exist between -500 & -400 */
    ADI_COMMON_ERR_ACT_RESET_FEATURE    = -400,     /* API NOK: Feature Reset Required */

    /* All Reset Interface Unique Codes exist between -400 & -300 */
    ADI_COMMON_ERR_ACT_RESET_INTERFACE  = -300,     /* API NOK: Interface Reset Required */

    /* All Check Feature Unique Codes exist between -300 & -200 */
    ADI_COMMON_ERR_ACT_CHECK_FEATURE    = -200,     /* API OK; Feature is reporting an Error */

    /* All Check Interface Unique Codes exist between -200 & -100 */
    ADI_COMMON_ERR_ACT_CHECK_INTERFACE  = -100,     /* API OK; Interface is reporting an Error */

    /* All Development Unique Codes exist between -100 & -1 */
    ADI_COMMON_ERR_ACT_OPEN_DEVICE      = -10,      /* API OK; Device Not Open */
    ADI_COMMON_ERR_ACT_CHECK_PARAM      = -1,       /* API OK; Invalid Parameter passed to function */

    /* No Error/Recovery Action */
    ADI_COMMON_ERR_ACT_NONE             = 0         /* API OK; No Action Required */
} adi_common_ErrAction_e;

/*
*  \brief ADI Common Error Action Union
*/
typedef union
{
    adi_common_ErrAction_e  commonAction;   /* Common Error Action Type */
    int64_t                 rawValue;       /* Raw Value */
} adi_common_ErrAction_t;

/*
*  \brief ADI Error Event Information
*/
typedef struct
{
    int64_t         errCode;    /* Error Code */
    const char*     errMsg;     /* Error Message */
    uint32_t        errSrc;     /* Error Source */
} adi_common_ErrCodeInfo_t;

/*
*  \brief ADI Error File Information
*/
typedef struct
{
    uint32_t        line;       /* Source File Line Number;     Set via __LINE__ Std Macro */
    const char*     function;   /* Source File Function Name;   Set via __func__ Std Macro */
    uint32_t        file;       /* Source File Name ID;         Set via ADI_FILE */
} adi_common_ErrFileInfo_t;

/*
*  \brief ADI Error Variable Information
*/
typedef struct
{
    const char* varName;    /* Variable Name */
    int64_t     varData;    /* Variable Data */
} adi_common_ErrVarInfo_t;

/*
*  \brief ADI Common Error Frame Structure
*/
typedef struct
{
    adi_common_ErrAction_e      action;     /* Recovery Action */
    adi_common_ErrCodeInfo_t    errInfo;    /* Error Event Information */
    adi_common_ErrFileInfo_t    fileInfo;   /* File Information */
    adi_common_ErrVarInfo_t     varInfo;    /* Variable Information */
} adi_common_ErrFrame_t;

/*
*  \brief ADI Common Debug Information
*/
typedef struct
{
    adi_common_ErrAction_e      highestPriorityAction;  /* Highest Priority Recovery Action Captured */
    const char*                 errCause;               /* Error Cause Message */
    const char*                 actionMsg;              /* Recovery Action Message */
} adi_common_ErrDebugInfo_t;

/*
*  \brief ADI Common Error Device Information
*/
typedef struct
{
    uint32_t    type; /* Device Type */
    uint32_t    id;   /* Device Number */
    const char* name; /* Device Name */
} adi_common_ErrDeviceInfo_t;

/*
*  \brief ADI Common Error Data Information
*/
typedef struct
{
    adi_common_ErrDeviceInfo_t  errDeviceInfo;      /* Error Device Info */
    uint32_t                    stackIdx;           /* Internal Array Indexing */
} adi_common_ErrDataInfo_t;

/*
*  \brief ADI Common Error Data Structure
*/
typedef struct
{
    adi_common_ErrDataInfo_t    errDataInfo;                                /* Additional Error Data Information */
    adi_common_ErrFrame_t       stackTrace[ADI_COMMON_STACK_TRACE_SIZE];    /* Stack Trace from API from when Error Detected */
    adi_common_ErrDebugInfo_t   errDebugInfo;                               /* Additional Debug Information */
} adi_common_ErrData_t;

/*
*  \brief ADI Common Error Table Row
*/
typedef struct
{
    const int64_t                   errCode;
    const char* const               errMsg;
    const adi_common_ErrAction_e    action;
} adi_common_ErrTableRow_t;

/*
*  \brief ADI Common Device Specific Error Table Row
*/
typedef struct
{
    const int64_t                   errCode;
    const char* const               errMsg;
    const char* const               errCause;
    const adi_common_ErrAction_e    actionCode;
    const char* const               actionMsg;
} adi_common_DeviceErrTableRow_t;

/*
*  \brief ADI Common Error Information
*/
typedef struct
{
    int64_t                 errCode;
    const char*             errMsg;
    const char*             errCause;
    adi_common_ErrAction_e  actionCode;
    const char*             actionMsg;
} adi_common_ErrorInfo_t;

typedef adi_common_ErrTableRow_t adi_common_api_errors_t;
#endif /* _ADI_COMMON_ERROR_TYPES_H_ */

