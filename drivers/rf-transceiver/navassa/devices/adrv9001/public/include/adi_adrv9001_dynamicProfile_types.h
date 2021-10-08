
/**
 * \file
 * \brief Type definitions for the ADRV9001 dynamic profile
 * \copyright Analog Devices Inc. 2021. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_DYNAMIC_PROFILE_TYPES_H_
#define _ADI_ADRV9001_DYNAMIC_PROFILE_TYPES_H_

#ifndef __KERNEL__
#include <stdint.h>
#endif

#include "adi_adrv9001_defines.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_txSettings_types.h"

/**
 *  \brief Rx Channel dynamic profile data structure
 */
typedef struct adi_adrv9000_RxDynamicProfile
{
    uint32_t rxOutputate_Hz;               /* Receiver sample rate in Hz. Note: in dynamic profile switching the interface rate is fixed. */
    uint32_t primaryBw_Hz;                 /* Rx RF bandwidth */
    adi_adrv9001_RxWbDecTop_t rxWbDecTop;  /* Rx wideband decimation top block */
} adi_adrv9000_RxDynamicProfile_t;

/**
 *  \brief Tx Channel dynamic profile data structure
 */
typedef struct adi_adrv9000_TxDynamicProfile
{
    uint32_t txInputRate_Hz;               /* Transmitter sample rate in Hz. Note: in dynamic profile switching the interface rate is fixed. */
    uint32_t primaryBw_Hz;                 /* Tx RF bandwidth */
    adi_adrv9001_TxWbIntTop_t txWbIntTop;  /* Tx wideband interpolation top block */
} adi_adrv9000_TxDynamicProfile_t;

/**
 * \brief Dynamic profile channel data structure
 */
typedef struct adi_adrv9000_DynamicProfileChannel
{
    adi_adrv9000_TxDynamicProfile_t txDynamicProfile;  /*!< Tx dynamic profile struct */
    adi_adrv9000_RxDynamicProfile_t rxDynamicProfile;  /*!< Rx dynamic profile struct */
    adi_adrv9000_RxDynamicProfile_t orxDynamicProfile; /*!< ORx dynamic profile struct */
    adi_adrv9000_RxDynamicProfile_t ilbDynamicProfile; /*!< ILB dynamic profile struct */
    adi_adrv9000_RxDynamicProfile_t elbDynamicProfile; /*!< ELB dynamic profile struct */
} adi_adrv9000_DynamicProfileChannel_t;

/**
 * \brief Enumerations of the dynamic profile index
 *
 *  Dynamic profile switching has the restriction that the ratio between the interface rate and
 *  sampling rate is always a power of 2.
 */
typedef enum adi_adrv9001_DynamicProfileIndex
{
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX0 = 0,  /*!< Selects the dynamic profile index = 0, slowest sampling rate  */
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX1 = 1,  /*!< Selects the dynamic profile index = 1 */
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX2 = 2,  /*!< Selects the dynamic profile index = 2 */
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX3 = 3,  /*!< Selects the dynamic profile index = 3 */
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX4 = 4,  /*!< Selects the dynamic profile index = 4 */
    ADI_ADRV9001_DYNAMIC_PROFILE_INDEX5 = 5   /*!< Selects the dynamic profile index = 5, fastest sampling rate */
} adi_adrv9001_DynamicProfileIndex_e;

/**
 * \brief Dynamic profile struct
 */
typedef struct adi_adrv9000_DynamicProfile
{
    adi_adrv9001_DynamicProfileIndex_e dynamicProfileIndex; /*!< Current dynamic profile index */
    adi_adrv9000_DynamicProfileChannel_t dynamicProfileChannels[ADI_ADRV9001_MAX_NUM_CHANNELS];  /*!< Dynamic profiles array */
} adi_adrv9000_DynamicProfile_t;

#endif /* _ADI_ADRV9001_DYNAMIC_PROFILE_TYPES_H_ */
