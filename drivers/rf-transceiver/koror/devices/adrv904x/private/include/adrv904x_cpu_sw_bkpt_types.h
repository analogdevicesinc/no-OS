/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
/**
 * \file adrv904x_cpu_sw_bkpt_types.h
 * 
 * \brief   Contains the definitions for Software Breakpoints
 *
 * \details Contains the definitions for Software Breakpoints
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_SW_BKPT_TYPES_H__
#define __ADRV904X_CPU_SW_BKPT_TYPES_H__

#include <stdint.h>

/**
* \brief Software breakpoint table indexes. 
*/

typedef enum
{
    ADRV904X_SWBKPT_INDEX_INVALID,        /*!< Invalid index */
    ADRV904X_SWBKPT_INDEX_IC_TXBBF,       /*!< TXBBF Init Cal Index */
    ADRV904X_SWBKPT_INDEX_IC_TEST_0,      /*!< TEST_0 Init Cal Index */
    ADRV904X_SWBKPT_INDEX_IC_TEST_1,      /*!< TEST_1 Init Cal Index */
    ADRV904X_SWBKPT_INDEX_TC_RXQEC,       /*!< RxQEC Tracking Cal Index */
    ADRV904X_SWBKPT_INDEX_IC_TXLB_FILTER, /*!< TX LB Filter Init Cal Index */
    ADRV904X_SWBKPT_INDEX_IC_PATHDELAY,   /*!< Path Delay Cal Index */
    ADRV904X_SWBKPT_INDEX_IC_TXQEC,       /*!< TxQec Init Cal Index */
    ADRV904X_SWBKPT_INDEX_TC_TXQEC,       /*!< TxQec Track Cal Index */
    
    ADRV904X_CPU_BKPT_TABLE_SIZE          /*!< Table size */
} ADRV904X_SWBKPT_TABLE_INDEX;


#endif /* __ADRV904X_CPU_SW_BKPT_TYPES_H__ */


