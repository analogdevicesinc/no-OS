/**
 * Copyright 2020 - 2022 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_watchdog_t.h
 * \brief Contains DFE SDK data types.
 *
 * DFE SDK API Version: 2.10.0.4
 */
#ifndef ADRV904X_DFE_SVC_WATCHDOG_T_H_
#define ADRV904X_DFE_SVC_WATCHDOG_T_H_

/*============= I N C L U D E S =============*/

#include <stdint.h>

/**
 * \brief Watchdog instance enumerations
 *
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_WATCHDOG_SYSTEM_LOCKUP_INSTANCE = 0, /*!< Watchdog instance responsible for detecting firmware lockup */
    ADI_ADRV904X_DFE_SVC_WATCHDOG_SYSTEM_OVERLOAD_INSTANCE,   /*!< Watchdog instance responsible for detecting prolonged CPU overload */
    ADI_ADRV904X_DFE_SVC_WATCHDOG_INSTANCE_2,                 /*!< Enumeration for Watchdog instance 2 */
    ADI_ADRV904X_DFE_SVC_WATCHDOG_INSTANCE_3,                 /*!< Enumeration for Watchdog instance 3 */
    ADI_ADRV904X_DFE_SVC_WATCHDOG_NUM_WDT,                    /*!< Number of Watchdog instances */
} adi_adrv904x_DfeSvcWatchdogInstance_e;

typedef uint8_t adi_adrv904x_DfeSvcWatchdogInstance_t;

#endif /* ADRV904X_DFE_SVC_WATCHDOG_T_H_ */



