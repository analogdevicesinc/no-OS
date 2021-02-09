/**
* \file
* \brief Contains ADI common types.
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_TYPES_H_
#define _ADI_COMMON_TYPES_H_

#include "adi_common_error_types.h"

/**
 * \brief Enumeration of channel numbers
 *
 * \note These enums may be masked (OR'd) to specify multiple channels at once
 */
typedef enum adi_common_ChannelNumber
{
    ADI_CHANNEL_1 = 0x1,    /* In hex to indicate maskable value */
    ADI_CHANNEL_2 = 0x2
} adi_common_ChannelNumber_e;

/**
 * \brief Enumeration of port types
 *
 * \note These enums may NOT be masked (OR'd) to specify multiple ports at once
 */
typedef enum adi_common_Port
{
    ADI_RX  = 0,    /* In decimal to indicate NON-maskable value */
    ADI_TX  = 1,
    ADI_ORX = 2,
    ADI_ILB = 3,
    ADI_ELB = 4
} adi_common_Port_e;

#ifndef CLIENT_IGNORE
/**
 *  \brief ADI common device structure
 */
typedef struct adi_common_Device
{
    void                     *devHalInfo; /*!< ADI_HAL Hardware layer settings pointer specific to this device instance */
    adi_common_ErrStruct_t   error;
    /* function pointer for datapacking or device specific hal */
} adi_common_Device_t;
#endif // !CLIENT_IGNORE

/**
 * \brief Data structure to hold API version information
 */
typedef struct adi_common_ApiVersion
{
    uint32_t major; /*!< API Major Version number */
    uint32_t minor; /*!< API Minor Version number */
    uint32_t patch; /*!< API Patch Version number */
} adi_common_ApiVersion_t;

#endif  /* _ADI_COMMON_TYPES_H_ */

