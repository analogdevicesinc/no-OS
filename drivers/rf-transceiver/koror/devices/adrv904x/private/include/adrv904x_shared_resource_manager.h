/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_shared_resource_manager.h
 * \brief Contains ADRV904X shared resource related private function prototypes for
 *        adrv904x_shared_resource_manager.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADRV904X_SHARED_RESOURCE_MANAGER_H_
#define _ADRV904X_SHARED_RESOURCE_MANAGER_H_

#include "adrv904x_shared_resource_manager_types.h"
#include "adi_adrv904x_error.h"


/**
* \brief Resets the shared resource pool for a given adrv904x device
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
* \param[in] device Pointer to the adrv904x device data structure
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceMgrReset(adi_adrv904x_Device_t* const device);

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
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is being inquired
* \param[out] sharedResourceAvailable result - ADI_TRUE or ADI_FALSE is returned depending 
*                                         on availability of shared resource
*                                         
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceAvailabilityCheck(adi_adrv904x_Device_t* const        device,
                                                                          const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                          uint8_t* const                      sharedResourceAvailable);

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
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceType is the shared resource type(GPIO, MEMORY etc) for which ID is being requested
* \param[in] sharedResource is typically the enum value of the shared resource for which ID is being requested
* \param[out] sharedResourceID result shared resource ID is updated in this variable
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceIdGet(adi_adrv904x_Device_t* const        device,
                                                              const adrv904x_SharedResourceType_e sharedResourceType,
                                                              int32_t                             sharedResource,
                                                              adrv904x_SharedResourceID_e* const  sharedResourceID);

/**
* \brief Returns the feature currently consuming the shared resource.
*
*  Each shared resource is associated with a feature in the resource pool. 
*  This function returns the feature currently associated with the shared resource
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to inquirer shared 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is under query
* \param[out] featureID pointer to the feature associated with the requested shared resource ID
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceFeatureGet(adi_adrv904x_Device_t* const        device,
                                                                   const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                   adrv904x_FeatureID_e*  const        featureID);

/**
* \brief Set the channelMask currently consuming the shared resource.
*
*  Each shared resource is associated with a feature in the resource pool. 
*  This function returns the channelMask currently associated with the shared resource
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to inquire shared 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is under query
* \param[in] channelMask the channelMask associated with the requested shared resource ID
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceChannelMaskSet(adi_adrv904x_Device_t* const        device,
                                                                       const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                       const uint32_t                      channelMask);

/**
* \brief Returns the channelMask currently consuming the shared resource.
*
*  Each shared resource is associated with a feature in the resource pool. 
*  This function returns the channelMask currently associated with the shared resource
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to inquire shared 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is under query
* \param[out] channelMask pointer to the channelMask associated with the requested shared resource ID
* 
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceChannelMaskGet(adi_adrv904x_Device_t* const        device,
                                                                       const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                       uint32_t *  const        channelMask);


/**
* \brief Returns the maximum semaphore count for a given feature.
*
*  Each shared resource is associated with a feature in the resource pool. 
*  Some features share the shared resource with multiple consumers necessitating
*  a semaphore associated with each feature. This function can be used to inquire
*  the maximum semaphore count associated with the feature.
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to inquire the maximum semaphore count associated
*      with a feature
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] featureID The feature ID for which the maximum semaphore count is requested
* \param[out] maxSemaphoreCount Pointer to the max semaphore count variable which will be updated
*                          with the maximum semaphore count for a given feature.
*                          
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceFeatureMaxSemaphoreCntGet(adi_adrv904x_Device_t* const    device,
                                                                                  const adrv904x_FeatureID_e      featureID,
                                                                                  uint8_t* const                  maxSemaphoreCount);

/**
* \brief Attempts to acquire the requested shared resource for a requested from the
*        adrv904x device data structure shared resource pool
*
* \pre This function is a private function used internally by the API and can be called
*      anytime after initialization by the API to acquire a shared resource
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is under query
* \param[in] featureID feature requesting the shared resource
* \param[out] resourceAcquistionStatus stores ADI_SUCCESS if resource acquisition was successful,
*        ADI_FAILURE otherwise
*        
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceAcquire(adi_adrv904x_Device_t* const        device,
                                                                const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                const adrv904x_FeatureID_e          featureID,
                                                                uint8_t* const                      resourceAcquistionStatus);

/**
* \brief Attempts to acquire multiple shared resources for a requested feature from the
*        adrv904x device data structure shared resource pool
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
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceType is the enum of the shared resource type to be acquired
* \param[in] sharedResourceArr is the array of shared resources to be acquired
* \param[in] numSharedResources is the size of sharedResourceArr
* \param[in] featureID feature requesting the shared resource
* \param[out] resourceAcquistionStatus stores ADI_SUCCESS if resource acquisition was successful,
*        ADI_FAILURE otherwise
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourcesAcquire(adi_adrv904x_Device_t* const        device,
                                                                 const adrv904x_SharedResourceType_e sharedResourceType,
                                                                 int32_t                             sharedResourceArr[],
                                                                 const uint32_t                      numSharedResources,
                                                                 const adrv904x_FeatureID_e          featureID,
                                                                 uint8_t* const                      resourceAcquistionStatus);

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
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceID is the shared resource whose availability is under query
* \param[in] featureID feature requesting the shared resource
* \param[out] resourceReleaseStatus stores ADI_SUCCESS if resource release was successful,
*        ADI_FAILURE otherwise
*        
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceRelease(adi_adrv904x_Device_t*  const       device,
                                                                const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                const adrv904x_FeatureID_e          featureID,
                                                                uint8_t* const                      resourceReleaseStatus);

/**
* \brief Attempts to release multiple shared resources for a requested feature from the
*        adrv904x device data structure shared resource pool
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
* \param[in] device Pointer to the adrv904x device data structure
* \param[in] sharedResourceType is the enum of the shared resource type to be released
* \param[in] sharedResourceArr is the array of shared resources to be acquired
* \param[in] numSharedResources is the size of sharedResourceArr
* \param[in] featureID feature requesting the shared resource
* \param[out] resourceReleaseStatus stores ADI_SUCCESS if resource release was successful,
*        ADI_FAILURE otherwise
*        
* \retval ADI_ADRV904X_ERR_ACT_NONE if Error Data is marked clear
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourcesRelease(adi_adrv904x_Device_t*  const       device,
                                                                 const adrv904x_SharedResourceType_e sharedResourceType,
                                                                 int32_t                             sharedResourceArr[],
                                                                 uint32_t                            numSharedResources,
                                                                 const adrv904x_FeatureID_e          featureID,
                                                                 uint8_t* const                      resourceReleaseStatus);

#endif // ! _ADRV904X_SHARED_RESOURCE_MANAGER_H_
