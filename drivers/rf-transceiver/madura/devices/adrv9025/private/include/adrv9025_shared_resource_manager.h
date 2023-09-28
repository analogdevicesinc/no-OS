/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_shared_resource_manager.h
* \brief Contains ADRV9025 shared resource related private function prototypes for
*        adrv9025_shared_resource_manager.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_SHARED_RESOURCE_MANAGER_H_
#define _ADRV9025_SHARED_RESOURCE_MANAGER_H_

#include "../../private/include/adrv9025_shared_resource_manager_types.h"
#include "adi_adrv9025_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Resets the shared resource pool for a given adrv9025 device
*
*  Each shared resource is associated with a feature. On reset, all the
*  shared resources are assigned to 'UNUSED' and are ready for use by a given feature.
*
* \pre This function is required to be called during initialization
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceMgrReset(adi_adrv9025_Device_t* device);

/**
* \brief Returns if a requested shared resource is available for use
*
*  Each shared resource is associated with a feature. Returns a success if the shared resource
*  is unused and a failure if already in use.
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to test shared resource availability
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceID is the shared resource whose availability is being enquired
* \param sharedResourceAvailable result - ADI_TRUE or ADI_FALSE is returned depending
*                                         on availability of shared resource
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceAvailabilityCheck(adi_adrv9025_Device_t*      device,
                                                 adrv9025_SharedResourceID_e sharedResourceID,
                                                 uint8_t*                    sharedResourceAvailable);

/**
* \brief Returns the shared resource ID for a shared resource
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to test shared resource availability
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceType is the shared resource type(GPIO, MEMORY etc) for which ID is being requested
* \param sharedResource is typically the enum value of the shared resource for which ID is being requested
* \param sharedResourceID result shared resource ID is updated in this variable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceIdGet(adi_adrv9025_Device_t*        device,
                                     adrv9025_SharedResourceType_e sharedResourceType,
                                     int32_t                       sharedResource,
                                     adrv9025_SharedResourceID_e*  sharedResourceID);

/**
* \brief Returns the feature currently consuming the shared resource.
*
*  Each shared resource is associated with a feature in the resource pool.
*  This function returns the feature currently associated with the shared resource
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to enquire shared
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceID is the shared resource whose availability is under query
* \param featureID pointer to the feature associated with the requested shared resrouce ID
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceFeatureGet(adi_adrv9025_Device_t*      device,
                                          adrv9025_SharedResourceID_e sharedResourceID,
                                          adrv9025_FeatureID_e*       featureID);

/**
* \brief Returns the maximum semaphore count for a given feature.
*
*  Each shared resource is associated with a feature in the resource pool.
*  Some features share the shared resource with multiple consumers necessitating
*  a semaphore associated with each feature. This function can be used to enquire
*  the maximum semaphore count associated with the feature.
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to enquire the maximum semaphore count associated
*      with a feature
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param featureID The feature ID for which the maximum semaphore count is requested
* \param maxSemaphoreCount Pointer to the max semaphore count variable which will be updated
*                          with the maximum semaphore count for a given feature.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceFeatureMaxSemaphoreCntGet(adi_adrv9025_Device_t* device,
                                                         adrv9025_FeatureID_e   featureID,
                                                         uint8_t*               maxSemaphoreCount);

/**
* \brief Attempts to acquire the requested shared resource for a requested from the
*        adrv9025 device data structure shared resource pool
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to acquire a shared resource
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceID is the shared resource whose availability is under query
* \param featureID feature requesting the shared resource
* \param resourceAcquistionStatus stores ADI_SUCCESS if resrouce acquisition was successful,
*        ADI_FAILURE otherwise
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceAcquire(adi_adrv9025_Device_t*      device,
                                       adrv9025_SharedResourceID_e sharedResourceID,
                                       adrv9025_FeatureID_e        featureID,
                                       uint8_t*                    resourceAcquistionStatus);

/**
* \brief Attempts to acquire multiple shared resources for a requested feature from the
*        adrv9025 device data structure shared resource pool
*
*  This function returns a failure even if a single resource acquisition fails
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to acquire multiple shared resources for
*      the same feature
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceType is the enum of the shared resource type to be acquired
* \param sharedResourceArr is the array of shared resources to be acquired
* \param numSharedResources is the size of sharedResourceArr
* \param featureID feature requesting the shared resource
* \param resourceAcquistionStatus stores ADI_SUCCESS if resrouce acquisition was successful,
*        ADI_FAILURE otherwise
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourcesAcquire(adi_adrv9025_Device_t*        device,
                                        adrv9025_SharedResourceType_e sharedResourceType,
                                        int32_t                       sharedResourceArr[],
                                        uint32_t                      numSharedResources,
                                        adrv9025_FeatureID_e          featureID,
                                        uint8_t*                      resourceAcquistionStatus);

/**
* \brief Attempts to release the shared resource under consumption by the given feature
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to test shared resource availability
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceID is the shared resource whose availability is under query
* \param featureID feature requesting the shared resource
* \param resourceReleaseStatus stores ADI_SUCCESS if resrouce release was successful,
*        ADI_FAILURE otherwise
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourceRelease(adi_adrv9025_Device_t*      device,
                                       adrv9025_SharedResourceID_e sharedResourceID,
                                       adrv9025_FeatureID_e        featureID,
                                       uint8_t*                    resourceReleaseStatus);

/**
* \brief Attempts to release multiple shared resources for a requested feature from the
*        adrv9025 device data structure shared resource pool
*
*  This function returns a failure even if a single resource release fails
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to release multiple shared resources for
*      the same feature
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the adrv9025 device data structure
* \param sharedResourceType is the enum of the shared resource type to be released
* \param sharedResourceArr is the array of shared resources to be acquired
* \param numSharedResources is the size of sharedResourceArr
* \param featureID feature requesting the shared resource
* \param resourceReleaseStatus stores ADI_SUCCESS if resource release was successful,
*        ADI_FAILURE otherwise
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_SharedResourcesRelease(adi_adrv9025_Device_t*        device,
                                        adrv9025_SharedResourceType_e sharedResourceType,
                                        int32_t                       sharedResourceArr[],
                                        uint32_t                      numSharedResources,
                                        adrv9025_FeatureID_e          featureID,
                                        uint8_t*                      resourceReleaseStatus);

#ifdef __cplusplus
}
#endif

#endif /* ! _ADRV9025_SHARED_RESOURCE_MANAGER_H_ */
