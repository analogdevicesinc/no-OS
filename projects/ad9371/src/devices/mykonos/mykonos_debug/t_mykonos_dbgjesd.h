/*!
 * \file t_mykonos_dbgjesd.h
 *
 * \brief Contains definitions and structure declarations for mykonos_dbgjesd.c
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef T_MYKONOS_DBGJESD_H_
#define T_MYKONOS_DBGJESD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

/**
 *  \brief Enum of unique error codes from the Mykonos DBG API functions.
 * Each error condition in the library should get its own enum value to allow
 * easy debug of errors.
 */
typedef enum
{
    MYKONOS_ERR_DBG_OK = 0,
    MYKONOS_ERR_DBG_FAIL = 1,
    MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER,
    MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE,
    MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED,
    MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE,
    MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
    MYKONOS_ERR_DBG_NULL_PARM,
    MYKONOS_ERR_DBG_ERROR_SYNC_MASK,
    MYKONOS_ERR_DBG_ERROR_IRQ_MASK,
    MYKONOS_ERR_DBG_ERROR_THRESHOLD,

    MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN,
    MYKONOS_ERR_DBG_ILLEGAL_ENABLE,
    MYKONOS_ERR_DBG_PATTERN_GEN_NOT_ENABLED,
    MYKONOS_ERR_DBG_ILLEGAL_TOGGLE,
    MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN,
    MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE,
    MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE,

    MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE,
    MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL,

    MYKONOS_ERR_DBG_FRAMER_SEL_BASE_ADD_NULL,
    MYKONOS_ERR_DBG_FRAMER_ILLEGAL_JESD_CORE,

    MYKONOS_ERR_DBG_NUMBER_OF_ERRORS
} mykonosDbgErr_t;

/**
 *  \brief Enum to select Jesd core
 */
typedef enum
{
    MYK_FRAMER = 0,     /*!< Framer for the main receive channel */
    MYK_DEFRAMER = 1,   /*!< Deframer for Tx channel */
    MYK_OBS_FRAMER = 2  /*!< Framer for the observation channel */
} mykonos_jesdcore_t;

/**
 *  \brief Enum to set the Lane
 */
typedef enum
{
    MYK_LANE_0 = 0, /*!< Lane 0 */
    MYK_LANE_1 = 1, /*!< Lane 1 */
    MYK_LANE_2 = 2, /*!< Lane 2 */
    MYK_LANE_3 = 3  /*!< Lane 3 */
} mykonosLaneSel_t;

/**
 *  \brief Enum to set the Lane
 */
typedef enum
{
    MYK_BAD_DISP_CNTR = 0,  /*!< Bad disparity */
    MYK_NIT_CNTR = 1,       /*!< Not in table */
    MYK_UEKC_CNTR = 2       /*!< Unexpected K character */
} mykonosCtrSel_t;

/**
 *  \brief Enum for the IRQ mask
 */
typedef enum
{
    MYK_SYNC_BADDISP = 0x04,    /*!< Bad disparity mask enable */
    MYK_SYNC_NIT = 0x02,        /*!< Not in table */
    MYK_SYNC_UEKC = 0x01        /*!< Unexpected K character */
} mykonosSyncMasks_t;

/**
 *  \brief Enum for the IRQ mask
 */
typedef enum
{
    MYK_IRQ_CMM = 0x08,     /*!< Configuration mismatch mask enable */
    MYK_IRQ_BADDISP = 0x04, /*!< Bad disparity mask enable */
    MYK_IRQ_NIT = 0x02,     /*!< Not in table */
    MYK_IRQ_UEKC = 0x01     /*!< Unexpected K character */
} mykonosIrqMasks_t;

/**
 *  \brief Enum for the error handling type
 */
typedef enum
{
    MYK_CLEAR = 1,          /*!< Clear error handling type */
    MYK_RESET = 2,          /*!< Reset error handling type */
    MYK_CLEAR_RESET = 3     /*!< Clear and Reset error handling type */
} mykonosHandleType_t;

/**
 *  \brief Enum for the error type
 */
typedef enum
{
    MYK_CMM = 0x7B,     /*!< configuration mismatch */
    MYK_BADDISP = 0x6D, /*!< Bad disparity */
    MYK_NIT = 0x6E,     /*!< Not in table */
    MYK_UEKC = 0x6F     /*!< Unexpected K character */
} mykonosErrType_t;

/**
 *  \brief Data structure to hold the error counters per a given lane
 */
typedef struct
{
    uint8_t badDispCntr;    /*!< bad disparity counter can 0-255 */
    uint8_t nitCntr;        /*!< not in table counter can 0-255 */
    uint8_t uekcCntr;       /*!< unexpected K character counter can 0-255 */
} mykonosLaneErr_t;

/**
 *  \brief Data structure to hold the deframer status
 */
typedef struct
{
    uint8_t deframerStatus;         /*!< deframer status see deframer status function */
    uint8_t deframerStatus2;        /*!< deframer status2 see function MYKONOS_deframerRd2Stat(...)*/
    uint8_t fifoDepth;              /*!< fifo depth */
    uint8_t phaseOffsetLFMC_sysref; /*!< phase offset might not be needed*/
    uint8_t fifoFullEMpty;          /*!< fifo full/empty */
    mykonosLaneErr_t lane0;         /*!< Lane 0 errors */
    mykonosLaneErr_t lane1;         /*!< Lane 1 errors */
    mykonosLaneErr_t lane2;         /*!< Lane 2 errors */
    mykonosLaneErr_t lane3;         /*!< Lane 3 errors */
    uint8_t irqMask;                /*!< Mask for IRQ generation */
    uint8_t errCntrMax;             /*!< Max error counters */
    uint8_t enabledLanes;           /*!< Lanes that are enabled */
} mykonosDeframerStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* T_MYKONOS_DBGJESD_H_ */
