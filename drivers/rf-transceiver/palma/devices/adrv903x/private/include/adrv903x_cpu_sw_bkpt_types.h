/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**
 * \file adrv903x_cpu_sw_bkpt_types.h
 * 
 * \brief   Contains the definitions for Software Breakpoints
 *
 * \details Contains the definitions for Software Breakpoints
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CPU_SW_BKPT_TYPES_H__
#define __ADRV903X_CPU_SW_BKPT_TYPES_H__


/**
* \brief Software breakpoint table indexes. 
*/

typedef enum
{
    ADRV903X_SWBKPT_INDEX_INVALID,        /*!< Invalid index */
    ADRV903X_SWBKPT_INDEX_IC_TXBBF,       /*!< TXBBF Init Cal Index */
    ADRV903X_SWBKPT_INDEX_IC_TEST_0,      /*!< TEST_0 Init Cal Index */
    ADRV903X_SWBKPT_INDEX_IC_TEST_1,      /*!< TEST_1 Init Cal Index */
    ADRV903X_SWBKPT_INDEX_TC_RXQEC,       /*!< RxQEC Tracking Cal Index */
    ADRV903X_SWBKPT_INDEX_IC_TXLB_FILTER, /*!< TX LB Filter Init Cal Index */
    ADRV903X_SWBKPT_INDEX_IC_PATHDELAY,   /*!< Path Delay Cal Index */
    ADRV903X_SWBKPT_INDEX_IC_TXQEC,       /*!< TxQec Init Cal Index */
    ADRV903X_SWBKPT_INDEX_TC_TXQEC,       /*!< TxQec Track Cal Index */
    
    ADRV903X_CPU_BKPT_TABLE_SIZE          /*!< Table size */
} ADRV903X_SWBKPT_TABLE_INDEX;


#endif /* __ADRV903X_CPU_SW_BKPT_TYPES_H__ */


