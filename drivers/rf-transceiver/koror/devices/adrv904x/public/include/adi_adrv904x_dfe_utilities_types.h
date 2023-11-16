/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_utilities_types.h
* \brief Contains ADRV904X API Utilities data types
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_DFE_UTILITIES_TYPES_H_
#define _ADI_ADRV904X_DFE_UTILITIES_TYPES_H_

#include "adi_adrv904x_user.h"
#include "adi_library_types.h"
#include "adi_adrv904x_dfe_cfr.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_rx.h"

/**
 * \brief Maxinum logtrace buffer size 
 */
#define ADI_ADRV904X_MAX_LOGTRACE_SIZE        (1024U)

/**
 * \brief the logtrace wait writeflag timeout us 
 */
#define ADI_ADRV904X_LOGTRACE_TIMEOUT_US           (500U)

/**
 * \brief the logtrace wait writeflag interval us 
 */
#define ADI_ADRV904X_LOGTRACE_INTERVAL_US           (100U)

/**
*  \brief Data structure to logtrace Buffer
*/
typedef struct adi_adrv904x_DfeLogTraceBuffer
{
    uint8_t logTraceData[ADI_ADRV904X_MAX_LOGTRACE_SIZE]; /*!< DFE logtrace max data 1024 bytes*/
    uint32_t size; /*!< DFE logtrace data size */
    uint8_t overFlowFlag;/*!< DFE logtrace data overflow >*/
} adi_adrv904x_DfeLogTraceBuffer_t;

/**
*  \brief Enum for capture type, VSWR vs DPD
*/
typedef enum adi_adrv904x_DfeCaptureType
{
    ADI_ADRV904X_DFE_CAPTURE_TYPE_DPD = 0, /*!< DPD capture type */
    ADI_ADRV904X_DFE_CAPTURE_TYPE_VSWR = 1  /*!< VSWR capture type */
} adi_adrv904x_DfeCaptureType_e;

#endif /* _ADI_ADRV904X_DFE_UTILITIES_TYPES_H_ */
