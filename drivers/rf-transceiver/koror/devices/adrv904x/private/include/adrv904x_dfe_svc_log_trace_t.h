/**
 * Copyright 2020 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_dfe_svc_log_trace_t.h
 * \brief Contains data types for DFE SDK data information
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef __ADRV904X_DFE_SVC_LOG_TRACE_T_H__
#define __ADRV904X_DFE_SVC_LOG_TRACE_T_H__

#include "adi_adrv904x_dfe_svc_err_codes_t.h"
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_dfe_svc_log_trace_events_t.h"
#include <stdbool.h>

#define ADI_ADRV904X_DFE_SVC_LOGTRACE_RELEASE     (0)
#define ADI_ADRV904X_DFE_SVC_LOGTRACE_BUSY        (1)

#define ADI_ADRV904X_DFE_SVC_MAX_LOGTRACE_SIZE    (16384U)

#define ADI_ADRV904X_DFE_SVC_LOGTRACE_BUFFER_EMPTY(pBuff)    (((pBuff)->wrIndex == (pBuff)->rdIndex) && ((pBuff)->wrapFlag == false))
#define ADI_ADRV904X_DFE_SVC_LOGTRACE_BUFFER_FULL(pBuff)     (((pBuff)->wrIndex == (pBuff)->rdIndex) && ((pBuff)->wrapFlag == true))

/**
 * \brief LogTrace information structure for SDK
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint64_t pData;        /*!< Log Data Store Point*/
    uint32_t size;         /*!< LogTrace Buffer Size*/
    uint32_t wrIndex;      /*!< Log Data write index*/
    uint32_t rdIndex;      /*!< Log Data read index*/
    bool     wrapFlag;     /*!< Log Data wrap flag*/
    bool     overflowFlag; /*!< Memory Buffer overflow flag between api and sdk*/
    bool     writeFlag;    /*!< Memory Buffer write flag between api and sdk*/
    bool     syncMark;     /*!< Memory Buffer Synchronization flag between api and sdk*/
} adrv904x_DfeSvcLogTraceBuffer_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_DFE_SVC_LOG_TRACE_T_H__ */


