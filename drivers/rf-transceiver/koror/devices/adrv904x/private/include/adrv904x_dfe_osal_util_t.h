/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_dfe_osal_util_t.h
 *
 * \brief Contains shared types used for OSAL data logging.
 * \details Contains enums and typedefs which are shared between the BBIC and DFE and used for logging OSAL information.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_DFE_OSAL_UTIL_T_H__
#define __ADRV904X_DFE_OSAL_UTIL_T_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/*
 *******************************************************************************
 *                              GLOBAL VARIABLES
 *******************************************************************************
 */

extern uint32_t g_dfeCrc;
extern uint32_t g_bbicCrc;

#define ADRV904X_DFE_OSAL_UTIL_INVALID_NODE_IDX    (0xFFFFFFFFU)

/*!< The link element can store either FOUR 8 bit word type data, or TWO 32 bit word type data, or ONE 64 bit type data */
typedef union adrv904x_DfeOsalUtilLinkElemType
{
    uint8_t  bData[4];
    uint32_t wData[2];
    uint64_t xData;
}adrv904x_DfeOsalUtilLinkElemType_t;

typedef enum adrv904x_DfeOsalUtilLogDataItem
{
    ADRV904X_DFE_OSAL_UTIL_LOG_THREAD = 0,
    ADRV904X_DFE_OSAL_UTIL_LOG_MUTEX,
    ADRV904X_DFE_OSAL_UTIL_LOG_EVENT_FLAG,
    ADRV904X_DFE_OSAL_UTIL_LOG_TIMER,
    ADRV904X_DFE_OSAL_UTIL_LOG_QUEUE,
    ADRV904X_DFE_OSAL_UTIL_LOG_SEM,
    ADRV904X_DFE_OSAL_UTIL_LOG_BLOCK_POOL,
    ADRV904X_DFE_OSAL_UTIL_LOG_CRC,
    ADRV904X_DFE_OSAL_UTIL_LOG_DFE_PROFILE,
    ADRV904X_DFE_OSAL_UTIL_LOG_CPU_LOADINFO,
    ADRV904X_DFE_OSAL_UTIL_LOG_MAX_ITEM,
}adrv904x_DfeOsalUtilLogDataItem_e;

typedef uint16_t adrv904x_DfeOsalUtilLogDataItem_t;

#define ADRV904X_DFE_OSAL_UTIL_LOG_METUX_NOT_NEEDED    (0U)
#define ADRV904X_DFE_OSAL_UTIL_LOG_METUX_NEEDED        (1U)

ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeOsalUtilLink
{
    adrv904x_DfeOsalUtilLinkElemType_t element;
    struct adrv904x_DfeOsalUtilLink    *next;
}adrv904x_DfeOsalUtilLinkNode_t;
ADI_ADRV904X_PACK_FINISH

typedef adrv904x_DfeOsalUtilLinkNode_t *adrv904x_DfeOsalUtilLogLink_t;

ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeOsalUtilLogDataUnit
{
    adrv904x_DfeOsalUtilLogLink_t logLink;            /*!< The pointer of osal log data */
    uint32_t                mtxFlag;            /*!< The Flag to indication whether mutex is used  */
    uint32_t                count;
}adrv904x_DfeOsalUtilLogDataUnit_t;
ADI_ADRV904X_PACK_FINISH

    ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeOsalUtilLogData
{
    adrv904x_DfeOsalUtilLogDataUnit_t osalLogData[ADRV904X_DFE_OSAL_UTIL_LOG_MAX_ITEM];
}adrv904x_DfeOsalUtilLogData_t;
ADI_ADRV904X_PACK_FINISH

#endif



