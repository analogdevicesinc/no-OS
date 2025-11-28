/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_common_hal.h
 * \brief Contains ADI Hardware Abstraction layer function prototypes and type definitions for adi_common_hal.c
 *
 * ADI common lib Version: 0.0.2.1
 */ 

#ifndef _ADI_COMMON_HAL_H_
#define _ADI_COMMON_HAL_H_

#include "adi_common_types.h"
#include "adi_common_macros.h"
#include "adi_common_user.h"
#include "adi_common_error.h"
#include "adi_common_log.h"

/**
 * /brief Convenience macro for public API functions to call adi_common_hal_ApiEnter
 */
#define ADI_API_ENTER_RTN(commonDev)                                                                        \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiEnter((commonDev), __func__, ADI_TRUE);      \
                                                                                                            \
    if(ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                          \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Enter Issue");                          \
        return _recoveryAction;                                                                             \
    }                                                                                                       \
}

/**
 * /brief Convenience macro for public API functions to call adi_common_hal_ApiEnter
 *  and return. Always causes a return from the calling function.
 *
 *  If the supplied recovery action indicates an error then any failure in ApiExit is ignored.
 *  The recovery action from ApiExit is only used as the function's recovery action if the
 *  supplied recovery action indicates no error.
 *
 *  \param commonDev Pointer to the common structure of type adi_common_Device_t
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
#define ADI_API_EXIT(commonDev, recoveryAction)                                                             \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiExit((commonDev), __func__, ADI_TRUE); \
                                                                                                            \
    if (ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                         \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Exit Issue");                           \
    }                                                                                                       \
                                                                                                            \
    if(ADI_COMMON_ERR_ACT_NONE != recoveryAction)                                                           \
    {                                                                                                       \
        return recoveryAction;                                                                              \
    }                                                                                                       \
    return _recoveryAction;                                                                                 \
}


/**
* \brief Service to Convert HAL Error Code to Recovery Action
*
* \param[in] halCode Hal Error Code to be converted
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_ErrCodeConvert(const adi_hal_Err_e halCode);

/**
 * \brief Used to initialise the HAL hardware.
 *
 * This function must be called before any of the other adi_common_ functions are
 * called for the same \param commonDevice. This function must be called only
 * once for each value of \param commonDevice.
 *
 * Applications do not need to call this function directly as it is called by a
 * device-specific HwOpen call.
 *
 * Each ADI device contains a adi_common_Device_t. The device_specific HwOpen
 * function must be the first function to be called for any device. One of the
 * tasks carried out by the device-specific HwOpen is to call this function
 * to prepare the common-device layer part of the device.
 *
 * Unlike the other common device functions HwOpen and HwClose are not thread-safe.
 * This is because it is during these functions that the thread-safety mechanisms
 * relied on by the other functions are initialized and finalized. Therefore
 * the mechanisms are not available for use within HwOpen/Close to enforce
 * serialization. As device-specific HwOpen/Close functions use the common
 * HwOpen/Close the same restriction applies to the device-specific HwOpen/Close
 * functions. Applications therefore are responsible to ensure that there is no
 * contention between threads for device access when HwOpen/Close is called. For
 * instance an application could carry out all HwOpens in a single thread and
 * ensure all worker threads have stopped (or at least stopped using the API)
 * before calling HwClose on all devices from a single thread.
 *
 * \dep_begin
 * \dep{device->common}
 * \dep_end
 *
 * \param[in] commonDev Pointer to the common structure of type adi_common_Device_t
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
ADI_API adi_common_ErrAction_e adi_common_hal_HwOpen(adi_common_Device_t* const commonDev);

/**
* \brief Used to close the HAL hardware.
*
* Must be called when the \param commonDev is no longer required so that any resources in
* the common device layer can be released. Must only be called on a common device
* that has been previously opened. Once closed no other functions may be called
* on a common device and it cannot be re-opened.
*
* Each ADI device contains a adi_common_Device_t. The device_specific HwClose
* function must be the last function to be called for any device. One of the
* tasks carried out by the device-specific HwClose is to call this function
* to release the common-device layer part of the device.
*
* Applications do not need to call this function directly as it is called by a
* device-specific HwClose call.
*
* Applications must ensure that all threads have finished with the device before calling
* the device-specific HwClose. See HwOpen for further thread-safety considerations.
*
*
* \pre The device must have been previously opened using adi_common_hal_HwOpen.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param[in] commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_HwClose(adi_common_Device_t* const commonDev);

/**
* \brief    Service to control HwReset Signal via a Logic Level
*
*           Caller is responsible for handling triggering (i.e. Level or Edge)
*
* \pre The device must have been previously opened using adi_common_hal_HwOpen.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param[in] commonDev  Pointer to the common structure of type adi_common_Device_t
* \param[in] pinLevel   Pin level = 1 will hold the reset line high, 0 will hold the reset line low
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_HwReset(  const adi_common_Device_t* const    commonDev,
                                                        const uint8_t                       pinLevel);

/**
* \brief Used to sleep for a given number of microSeconds.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param[in] commonDev  Pointer to the common structure of type adi_common_Device_t
* \param[in] time_us    The number of micro seconds to sleep
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_Wait_us(  const adi_common_Device_t* const    commonDev,
                                                        const uint32_t                      time_us);

/**
* \brief Used to sleep for a given number of milliSeconds.
*
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
* \param time_ms The number of milli seconds to sleep.
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_Wait_ms(  const adi_common_Device_t* const    commonDev,
                                                        const uint32_t                      time_ms);

/**
* \brief Acquires the device so that it can only be used only in the context of the calling
* thread until such time as adi_common_hal_Unlock is called on the device.
*
* Blocks until the device is acquired. There is no timeout mechanism in the API although
* the underlying HAL may choose to timeout and return an error.
*
* Attempting to explicitly acquire a device that has already been acquired by the calling thread
* results in undefined behaviour.
*
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_Lock(adi_common_Device_t* const commonDev);

/**
* \brief Releases a device previously acquired by the calling thread.
*
* Only the thread that acquired the device can release the device. Attempting to release a device
* that has not been acquired by the calling thread results in undefined behaviour.
*
* Similar to public API calls on ADI devices the return value is a
* Recovery Action and is also written to commonDev->error.newAction.
*
* \dep_begin
* \dep{device->common}
* \dep_end
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_Unlock(adi_common_Device_t* const commonDev);

/**
 * /brief A convenience function that performs the tasks that must be preformed at the start
 *          of every public API function call.
 *
 *   - Log the API call
 *   - Lock the device
 *   - Assign the TLS err struct to the device for the duration of the call
 *   - Reset the err struct
 *   - Check device has been opened
 *
 *  Should only be called from a device's public API functions.
 *
 * \param commonDev Pointer to the common structure of type adi_common_Device_t
 * \param fnName The name of the calling function
 * \param doLocking If set to ADI_TRUE acquire the device lock otherwise do not.
 *
 * /post On successful completion the device is locked, the calling thread's error struct has been
 * assigned as the device's error struct and the struct has been set to indicate no error.
 * Otherwise the correct error structure has been assigned and has been set to indicate the error
 * that occurred.
 * The device is only locked on a successful return.
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
ADI_API adi_common_ErrAction_e adi_common_hal_ApiEnter( adi_common_Device_t* const  commonDev,
                                                        const char* const           fnName,
                                                        const uint32_t              doLocking);


/**
 * /brief A convenience function that performs the tasks that must be performed before returning
 *          from every public API function call.
 *
 *   - Log the API return
 *   - Unlock the device
 *
 *  Should only be called from a device's public API functions.
 *
 * \param commonDev Pointer to the common structure of type adi_common_Device_t
 * \param fnName The name of the calling function
 * \param doLocking If set to ADI_TRUE release the device lock otherwise do not.
 *
 * /post On successful completion the device is unlocked. Otherwise the device's error structure has been
 *  set to indicate the error that occurred.
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
ADI_API adi_common_ErrAction_e adi_common_hal_ApiExit(  adi_common_Device_t* const  commonDev,
                                                        const char* const           fnName,
                                                        const uint32_t              doLocking);

/**
 * /brief Allows greater control over the logging of entry to the API than adi_common_hal_ApiEnter.
 *
 * Refer to the documentation for the otherwise identical adi_common_hal_ApiEnter.
 *
 * \param commonDev Pointer to the common structure of type adi_common_Device_t
 * \param fnName The name of the calling function
 * \param doLocking If set to ADI_TRUE acquire the device lock otherwise do not.
 * \param logCtl If set to QUIET the logging of the entry is de-prioritized.
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
ADI_API adi_common_ErrAction_e adi_common_hal_ApiEnter_vLogCtl(adi_common_Device_t* const  commonDev,
                                                               const char* const           fnName,
                                                               const uint32_t              doLocking,
                                                               const adi_common_LogCtl_e   logCtl);

/**
 * /brief Allows greater control over the logging of exit from the API than adi_common_hal_ApiExit.
 *  
 * Refer to the documentation for the otherwise identical adi_common_hal_ApiExit.
 *
 * \param commonDev Pointer to the common structure of type adi_common_Device_t
 * \param fnName The name of the calling function
 * \param doLocking If set to ADI_TRUE acquire the device lock otherwise do not.
 * \param logCtl If set to QUIET the logging of the entry is de-prioritized.
 *
 * \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
 */
ADI_API adi_common_ErrAction_e adi_common_hal_ApiExit_vLogCtl(adi_common_Device_t* const  commonDev,
                                                               const char* const           fnName,
                                                               const uint32_t              doLocking,
                                                               const adi_common_LogCtl_e   logCtl);

/**
* /brief A convenience function that performs HAL Function Pointer Checks
*
*  Should only be called from a device's public API functions.
*
* \param commonDev Pointer to the common structure of type adi_common_Device_t
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_common_ErrAction_e adi_common_hal_PlatformFunctionCheck(adi_common_Device_t* const commonDev);

#endif
