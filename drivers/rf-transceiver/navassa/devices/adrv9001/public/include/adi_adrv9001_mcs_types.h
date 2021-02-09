/**
* \file
* \brief ADRV9001 Multi-Chip Synchronization (MCS) data types
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_MCS_TYPES_H_
#define _ADI_ADRV9001_MCS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

/**
 * \brief PLL Synchronization status general structure
 */
typedef struct adi_adrv9001_PllSyncStatus {
    bool jesdSyncComplete;                    /*!< JESD synchronization complete */
    bool digitalClocksSyncComplete;           /*!< digital clock synchronization complete */
    bool clockGenDividerSyncComplete;         /*!< clock generation divider synchronization complete */
    bool sdmClockDividerSyncComplete;         /*!< CLK PLL SDM clock divider synchronization complete */
    bool referenceClockDividerSyncComplete;   /*!< Reference clock divider synchronization complete */
} adi_adrv9001_PllSyncStatus_t;

/**
 * \brief Digital sub system Synchronization status general structure
 */
typedef struct adi_adrv9001_RxLvdsSyncStatus {
    bool lvdsFirstSyncComplete;   /*!< Rx LVDS divider first synchronization complete */
    bool lvdsSecondSyncComplete;  /*!< Rx LVDS divider second synchronization complete */
} adi_adrv9001_RxLvdsSyncStatus_t;

/**
 * \brief MCS analog and digital status
 */
typedef struct adi_adrv9001_McsStatus {
	adi_adrv9001_PllSyncStatus_t rf1PllSyncStatus;     /*!< RF1 PLL synchronization status */
	adi_adrv9001_PllSyncStatus_t rf2PllSyncStatus;     /*!< RF1 PLL synchronization status */
	adi_adrv9001_PllSyncStatus_t clkPllSyncStatus;     /*!< CLK PLL synchronization status */
	adi_adrv9001_PllSyncStatus_t clkPllLpSyncStatus;   /*!< LP CLK PLL synchronization status */

	adi_adrv9001_RxLvdsSyncStatus_t rx1LvdsSyncStatus; /*!< Rx1 digital LVDS synchronization status */
	adi_adrv9001_RxLvdsSyncStatus_t rx2LvdsSyncStatus; /*!< Rx2 digital LVDS synchronization status */

    bool firstDigitalSyncComplete;  /*!< Digital synchronization status */
    bool secondDigitalSyncComplete; /*!< Digital synchronization status */
} adi_adrv9001_McsStatus_t;

/**
 * \brief Enumerated list for SW MCS status
 */
typedef enum adi_adrv9001_McsSwStatus
{
    ADI_ADRV9001_MCSSWSTATUS_READY                       = 0,    /*!< Waiting for pulse 1 and 2 if MCS substate is MCS_READY */
    ADI_ADRV9001_MCSSWSTATUS_PULSE2_RECEIVED             = 1,    /*!< Pulse 2 received if MCS substate is MCS_TRANSITION */
    ADI_ADRV9001_MCSSWSTATUS_PULSE3_RECEIVED             = 2,    /*!< Pulse 3 received if MCS substate is MCS_TRANSITION */
    ADI_ADRV9001_MCSSWSTATUS_PULSE4_RECEIVED             = 3,    /*!< Pulse 4 received if MCS substate is MCS_TRANSITION */
    ADI_ADRV9001_MCSSWSTATUS_PULSE5_RECEIVED             = 4,    /*!< Pulse 5 received if MCS substate is MCS_TRANSITION */
    ADI_ADRV9001_MCSSWSTATUS_DEVICE_SWITCHED_TO_HSCLK    = 5,    /*!< Trancseiver has switched to high speed clock */
} adi_adrv9001_McsSwStatus_e;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_MCS_TYPES_H_ */
