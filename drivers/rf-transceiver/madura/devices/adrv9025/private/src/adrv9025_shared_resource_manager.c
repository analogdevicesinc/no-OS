// SPDX-License-Identifier: GPL-2.0
/**
 * \file adrv9025_shared_resource_manager.c
 * \brief Contains ADRV9025 shared resource related private function implementations
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "adi_common_error_types.h"
#include "adi_adrv9025_gpio_types.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_hal.h"
#include "adi_common_macros.h"

/******************************************** User Functions ********************************************************************/

int32_t adrv9025_SharedResourceMgrReset(adi_adrv9025_Device_t* device)
{
    int32_t  recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint32_t sharedResourceIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    for (sharedResourceIndex = 0; sharedResourceIndex < ADRV9025_NUM_SHARED_RESOURCES; sharedResourceIndex++)
    {
        device->devStateInfo.sharedResourcePool[sharedResourceIndex].featureID      = ADRV9025_FEATURE_UNUSED;
        device->devStateInfo.sharedResourcePool[sharedResourceIndex].semaphoreCount = 0;
    }

    return recoveryAction;
}

int32_t adrv9025_SharedResourcesAcquire(adi_adrv9025_Device_t*        device,
                                        adrv9025_SharedResourceType_e sharedResourceType,
                                        int32_t                       sharedResourceArr[],
                                        uint32_t                      numSharedResources,
                                        adrv9025_FeatureID_e          featureID,
                                        uint8_t*                      resourceAcquistionStatus)
{
    uint32_t                    sharedResourceIndex = 0;
    adrv9025_SharedResourceID_e sharedResourceID    = ADRV9025_SHARED_RESOURCE_INVALID;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check resourceAcquistionStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        resourceAcquistionStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV9025_NUM_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         featureID,
                         "featureID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV9025_NUM_SHARED_RESOURCE_TYPES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceType,
                         "Invalid sharedResourceType encountered in adrv9025_SharedResourcesAcquire function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Loop through each shared resource.
      1) Get Shared Resource unique ID
      2) Acquire Shared Resource
      3) If the resource is in use by another feature than the one which requested return a failed status*/
    for (sharedResourceIndex = 0; sharedResourceIndex < numSharedResources; sharedResourceIndex++)
    {
        adrv9025_SharedResourceIdGet(device,
                                     sharedResourceType,
                                     sharedResourceArr[sharedResourceIndex],
                                     &sharedResourceID);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to resolve shared resource ID for the requested share resource");
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_SharedResourceAcquire(device,
                                       sharedResourceID,
                                       featureID,
                                       resourceAcquistionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to acquire requested share resource");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (*resourceAcquistionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquistionStatus,
                             "Error acquiring requested shared resource. Please check if the resource is already in use by another feature.");
            ADI_ERROR_RETURN(device->common.error.newAction);
            break;
        }
    }

    return device->common.error.newAction;
}

int32_t adrv9025_SharedResourcesRelease(adi_adrv9025_Device_t*        device,
                                        adrv9025_SharedResourceType_e sharedResourceType,
                                        int32_t                       sharedResourceArr[],
                                        uint32_t                      numSharedResources,
                                        adrv9025_FeatureID_e          featureID,
                                        uint8_t*                      resourceReleaseStatus)
{
    uint32_t                    sharedResourceIndex = 0;
    adrv9025_SharedResourceID_e sharedResourceID    = ADRV9025_SHARED_RESOURCE_INVALID;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check resourceReleaseStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        resourceReleaseStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV9025_NUM_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         featureID,
                         "featureID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV9025_NUM_SHARED_RESOURCE_TYPES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceType,
                         "Invalid sharedResourceType encountered in adrv9025_SharedResourcesAcquire function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Loop through each shared resource.
    1) Get Shared Resource unique ID
    2) Release Shared Resource
    3) If the resource is in use by another feature than the one which requested return a failed status*/
    for (sharedResourceIndex = 0; sharedResourceIndex < numSharedResources; sharedResourceIndex++)
    {
        adrv9025_SharedResourceIdGet(device,
                                     sharedResourceType,
                                     sharedResourceArr[sharedResourceIndex],
                                     &sharedResourceID);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to resolve shared resource ID for the requested share resource");
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_SharedResourceRelease(device,
                                       sharedResourceID,
                                       featureID,
                                       resourceReleaseStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while attempting to release requested share resource");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (*resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error releasing requested shared resource. Please check if the resource is already in use by another feature.");
            ADI_ERROR_RETURN(device->common.error.newAction);
            break;
        }
    }

    return device->common.error.newAction;
}

/******************************************** Helper/Debug Functions ********************************************************************/

int32_t adrv9025_SharedResourceAvailabilityCheck(adi_adrv9025_Device_t*      device,
                                                 adrv9025_SharedResourceID_e sharedResourceID,
                                                 uint8_t*                    sharedResourceAvailable)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV9025_NUM_SHARED_RESOURCES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceID,
                         "sharedResourceID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Return success if shared resource pool index of shared resource ID has a feature other than UNUSED associated with it*/
    if (device->devStateInfo.sharedResourcePool[sharedResourceID].featureID == ADRV9025_FEATURE_UNUSED)
    {
        *sharedResourceAvailable = (uint8_t)ADI_TRUE;
    }
    else
    {
        *sharedResourceAvailable = (uint8_t)ADI_FALSE;
    }

    return recoveryAction;
}

int32_t adrv9025_SharedResourceIdGet(adi_adrv9025_Device_t*        device,
                                     adrv9025_SharedResourceType_e sharedResourceType,
                                     int32_t                       sharedResource,
                                     adrv9025_SharedResourceID_e*  sharedResourceID)
{
    static const adrv9025_SharedResourceLut_t sharedResourceLut[] = {
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_00, ADRV9025_GPIO_00},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_01, ADRV9025_GPIO_01},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_02, ADRV9025_GPIO_02},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_03, ADRV9025_GPIO_03},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_04, ADRV9025_GPIO_04},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_05, ADRV9025_GPIO_05},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_06, ADRV9025_GPIO_06},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_07, ADRV9025_GPIO_07},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_08, ADRV9025_GPIO_08},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_09, ADRV9025_GPIO_09},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_10, ADRV9025_GPIO_10},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_11, ADRV9025_GPIO_11},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_12, ADRV9025_GPIO_12},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_13, ADRV9025_GPIO_13},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_14, ADRV9025_GPIO_14},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_15, ADRV9025_GPIO_15},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_16, ADRV9025_GPIO_16},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_17, ADRV9025_GPIO_17},
        {ADRV9025_SHARED_RESOURCE_GPIO, ADI_ADRV9025_GPIO_18, ADRV9025_GPIO_18},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_00, ADRV9025_GPIO_ANA_00},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_01, ADRV9025_GPIO_ANA_01},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_02, ADRV9025_GPIO_ANA_02},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_03, ADRV9025_GPIO_ANA_03},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_04, ADRV9025_GPIO_ANA_04},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_05, ADRV9025_GPIO_ANA_05},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_06, ADRV9025_GPIO_ANA_06},
        {ADRV9025_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV9025_GPIO_ANA_07, ADRV9025_GPIO_ANA_07}
    };
    /*****************Please add lut val for shared resources above this line*******************/

    int32_t  recoveryAction              = ADI_COMMON_ACT_NO_ACTION;
    uint32_t sharedResourceIndex         = 0;
    uint8_t  validSharedResourceTypeFlag = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        sharedResourceID);

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV9025_NUM_SHARED_RESOURCE_TYPES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceType,
                         "Invalid sharedResourceType encountered in adrv9025_SharedResourcesAcquire function");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (sharedResourceIndex = 0; sharedResourceIndex < ADRV9025_NUM_SHARED_RESOURCES; sharedResourceIndex++)
    {
        if (sharedResourceLut[sharedResourceIndex].sharedResourceType == sharedResourceType)
        {
            if (sharedResourceLut[sharedResourceIndex].sharedResource == sharedResource)
            {
                *sharedResourceID           = sharedResourceLut[sharedResourceIndex].sharedResourceId;
                validSharedResourceTypeFlag = 1;
                break;
            }
        }
    }

    /* Check not a valid shared resource ID found */
    if (validSharedResourceTypeFlag == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceType,
                         "Requested Shared Resource is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_SharedResourceFeatureGet(adi_adrv9025_Device_t*      device,
                                          adrv9025_SharedResourceID_e sharedResourceID,
                                          adrv9025_FeatureID_e*       featureID)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Range check that featureID is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        featureID);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV9025_NUM_SHARED_RESOURCES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceID,
                         "sharedResourceID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Return the feature associated with shared resource pool*/
    *featureID = device->devStateInfo.sharedResourcePool[sharedResourceID].featureID;

    return recoveryAction;
}

int32_t adrv9025_SharedResourceFeatureMaxSemaphoreCntGet(adi_adrv9025_Device_t* device,
                                                         adrv9025_FeatureID_e   featureID,
                                                         uint8_t*               maxSemaphoreCount)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /*Range check that the maximum semaphore count pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        maxSemaphoreCount);

    /*Range Check that feature ID is valid*/
    if (featureID >= ADRV9025_NUM_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         featureID,
                         "The feature ID requested for retrieving maximum semaphore count is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (featureID)
    {
    case(ADRV9025_FEATURE_RX_GAIN_CTRL_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_GAIN_CTRL_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_RX_PIN_CTRL_ATTN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_PIN_CTRL_ATTN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_GAIN_CTRL_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_RX_DUALBAND_LNA_CTRL_WORD_OUTPUT):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_SPI2_CTRL_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_SPI2_CTRL_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_ARM_GPIO_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_ARM_GPIO_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_RX_EXT_SLICER_CTRL):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_EXT_SLICER_CTRL_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_AUX_DAC_OUT):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_AUX_DAC_OUT_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_GPIO_STREAM_TRIGGER):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_GPIO_STREAM_TRIGGER_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_FOVR):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_FOVR_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_PCA):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_PCA_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_TX_ATTEN_UPDATE_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_TX_ATTEN_UPDATE_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    case(ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN):
    {
        *maxSemaphoreCount = (uint8_t)ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN_MAX_SEMAPHORE_COUNT;
        break;
    }
    default:
    {
        *maxSemaphoreCount = 1;
        break;
    }
    }

    return recoveryAction;
}

int32_t adrv9025_SharedResourceAcquire(adi_adrv9025_Device_t*      device,
                                       adrv9025_SharedResourceID_e sharedResourceID,
                                       adrv9025_FeatureID_e        featureID,
                                       uint8_t*                    resourceAcquistionStatus)
{
    uint8_t              sharedResourceAvailable = (uint8_t)ADI_FALSE;
    adrv9025_FeatureID_e currentFeatureID        = ADRV9025_FEATURE_UNUSED;
    uint8_t              maxSemaphoreCount       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check resourceAcquistionStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        resourceAcquistionStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV9025_NUM_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         featureID,
                         "featureID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range check that sharedResourceID is valid*/
    if (sharedResourceID >= ADRV9025_NUM_SHARED_RESOURCES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceID,
                         "sharedResourceID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Initialize resourceAcquistionStatus to false and set to true only if shared resource available*/
    *resourceAcquistionStatus = ADI_FAILURE;

    /*sharedResourceID check is done in Shared Resource Available function*/
    /*Acquire resource if it is not already acquired by another feature or already acquired by the requesting feature*/
    adrv9025_SharedResourceAvailabilityCheck(device,
                                             sharedResourceID,
                                             &sharedResourceAvailable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while checking for shared resource availability");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (sharedResourceAvailable == ADI_TRUE)
    {
        device->devStateInfo.sharedResourcePool[sharedResourceID].featureID = featureID;
        device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount++;
        *resourceAcquistionStatus = ADI_SUCCESS;
    }
    else
    {
        adrv9025_SharedResourceFeatureGet(device,
                                          sharedResourceID,
                                          &currentFeatureID);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while acquiring shared resource feature");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (currentFeatureID == featureID)
        {
            adrv9025_SharedResourceFeatureMaxSemaphoreCntGet(device,
                                                             featureID,
                                                             &maxSemaphoreCount);
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount < maxSemaphoreCount)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount++;
                *resourceAcquistionStatus = ADI_SUCCESS;
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adrv9025_SharedResourceRelease(adi_adrv9025_Device_t*      device,
                                       adrv9025_SharedResourceID_e sharedResourceID,
                                       adrv9025_FeatureID_e        featureID,
                                       uint8_t*                    resourceReleaseStatus)
{
    uint8_t              sharedResourceAvailable = ADI_FALSE;
    adrv9025_FeatureID_e currentFeatureID        = ADRV9025_FEATURE_UNUSED;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check resourceReleaseStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        resourceReleaseStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV9025_NUM_FEATURES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         featureID,
                         "featureID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range check that sharedResourceID is valid*/
    if (sharedResourceID >= ADRV9025_NUM_SHARED_RESOURCES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sharedResourceID,
                         "sharedResourceID is not valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Initialize resourceReleaseStatus to false and set to true only if shared resource is released successfully*/
    *resourceReleaseStatus = ADI_FAILURE;

    /*sharedResourceID check is done in Shared Resource Available function*/
    /*Release the shared resource only if the requested feature ID matches the current feature ID or is not in use currently*/
    adrv9025_SharedResourceAvailabilityCheck(device,
                                             sharedResourceID,
                                             &sharedResourceAvailable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                     NULL,
                     "Error while checking for shared resource availability");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (sharedResourceAvailable == ADI_TRUE)
    {
        *resourceReleaseStatus = ADI_SUCCESS;
    }
    else
    {
        adrv9025_SharedResourceFeatureGet(device,
                                          sharedResourceID,
                                          &currentFeatureID);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         NULL,
                         "Error while acquiring shared resource feature");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (currentFeatureID == featureID)
        {
            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount > 0)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount--;
            }

            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount == 0)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].featureID = ADRV9025_FEATURE_UNUSED;
            }

            *resourceReleaseStatus = ADI_SUCCESS;
        }
    }

    return device->common.error.newAction;
}
