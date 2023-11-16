/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_common_types.h
* \brief Contains ADI common types.
*
* ADI common lib Version: 0.0.2.1
*/

#ifndef _ADI_COMMON_TYPES_H_
#define _ADI_COMMON_TYPES_H_

#include "adi_common_error_types.h"
#include "adi_platform_types.h"

#define ADI_COMMON_DEVICE_BF_ENCODE(value, mask, shift) (((value) << (shift)) & (mask))
#define ADI_COMMON_DEVICE_BF_DECODE(value, mask, shift) (((value) & (mask)) >> (shift))
#define ADI_COMMON_DEVICE_BF_CLEAR(result, mask)  (result = ((result) & ~(mask)))
#define ADI_COMMON_DEVICE_BF_SET(result, mask) (result = (result) | (mask))
#define ADI_COMMON_DEVICE_BF_UPDATE(result, value, mask, shift)  (result = ((result) & ~(mask)) | ( ((value) << (shift)) & (mask)))
#define ADI_COMMON_DEVICE_BF_EQUAL(value, mask) ((mask) == ((value) & (mask)))

/*
*  \brief   Common File Abstractions
*/
typedef enum
{
    ADI_COMMON_FILE_HAL            = 0x0U,
    ADI_COMMON_FILE_LOGGING
} adi_common_File_e;

/*
*  \brief ADI Common Device Information
*/
typedef struct
{
    uint32_t    type;   /* Device Type */
    uint32_t    id;     /* Device Number */
    const char* name;   /* Device Name */
} adi_common_DeviceInfo_t;

/**
 * \brief Mask values to be used with adi_common_Device.state.
 *
 * Members are intended to populate a uint8_t bitfield so must not exceed 1 << 7.
 */
typedef enum
{
    ADI_COMMON_DEVICE_STATE_OPEN     = 1U << 0U,         /*!< Device has been opened successfully with HwOpen. */
    /** Device is in a time-critical state. May be used to suspend some logging etc. */
    ADI_COMMON_DEVICE_STATE_TC       = 1U << 1U
} adi_common_DeviceState_e;

/**
 * \brief Values to be used with adi_common_hal_ApiEnter_vLogCtl
 */
typedef enum
{
    ADI_COMMON_DEVICE_LOGCTL_NORMAL   = 1U << 0U,  /*!< Log API entry/exit as normal */
    ADI_COMMON_DEVICE_LOGCTL_QUIET    = 1U << 1U   /*!< De-prioritize the logging of this entry/exit */
}adi_common_LogCtl_e;

/**
*  \brief ADI common device structure
*/
typedef struct adi_common_Device
{
    adi_common_DeviceInfo_t     deviceInfo;     /* Device Information */
    void*                       devHalInfo;     /* Hardware Abstraction Layer Settings */
    adi_common_ErrData_t*       errPtr;         /* Error Memory Pointer */
    uint8_t                     wrOnly;         /* !0 => device is in a SPI write-only state, 0 device is in normal read-write state */
    adi_hal_mutex_t             mutex;          /* Used to serialize device access from multi-threaded client applications */
    uint16_t                    lockCnt;        /* Counts number of times lock acquired using adi_common_Lock() */
    const char*                 type;           /*!< Human-readable string indicating the device type */
    uint16_t                    id;             /*!< Identifies an instance of a particular device type */
    uint8_t                     state;          /*!< Bitfield as per adi_common_DeviceState_e */
    uint16_t                    publicCnt;      /*!< Public API Count in the Call Stack */
} adi_common_Device_t;

/**
 *  \brief Evaluates to non-zero if a device's IS_OPEN state is set. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t 
 */
#define ADI_COMMON_DEVICE_STATE_IS_OPEN(commonDev)   ADI_COMMON_DEVICE_BF_EQUAL((commonDev).state, ADI_COMMON_DEVICE_STATE_OPEN)

/**
 *  \brief Sets a device's IS_OPEN state. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t
 */
#define ADI_COMMON_DEVICE_STATE_OPEN_SET(commonDev)  ADI_COMMON_DEVICE_BF_SET((commonDev).state, ADI_COMMON_DEVICE_STATE_OPEN)

/**
 *  \brief Clears a device's IS_OPEN state. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t
 */
#define ADI_COMMON_DEVICE_STATE_OPEN_CLR(commonDev)  ADI_COMMON_DEVICE_BF_CLEAR((commonDev).state, ADI_COMMON_DEVICE_STATE_OPEN)

/**
 *  \brief Evaluates to non-zero if a device's TC (time-critical) state is set. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t
 */
#define ADI_COMMON_DEVICE_STATE_IS_TC(commonDev)  ADI_COMMON_DEVICE_BF_EQUAL((commonDev).state, ADI_COMMON_DEVICE_STATE_TC)
/**
 *  \brief Sets a device's TC (time-critical) state. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t
 */
#define ADI_COMMON_DEVICE_STATE_TC_SET(commonDev) ADI_COMMON_DEVICE_BF_SET((commonDev).state, ADI_COMMON_DEVICE_STATE_TC)
/**
 *  \brief Clears a device's TC (time-critical) state. See adi_common_DeviceState_e.
 *  \param commonDev - adi_common_Device_t
 */
#define ADI_COMMON_DEVICE_STATE_TC_CLR(commonDev) ADI_COMMON_DEVICE_BF_CLEAR((commonDev).state, ADI_COMMON_DEVICE_STATE_TC)

#endif  /* _ADI_COMMON_TYPES_H_ */