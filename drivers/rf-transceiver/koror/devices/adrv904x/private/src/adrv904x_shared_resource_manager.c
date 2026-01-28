/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_shared_resource_manager.c
 * \brief Contains ADRV904X shared resource related private function implementations
 *
 * ADRV904X API Version: 2.10.0.4
 */

#include "../../private/include/adrv904x_shared_resource_manager.h"

#include "adi_adrv904x_gpio_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_SHARED_RESOURCE_MANAGER

/******************************************** User Functions ********************************************************************/

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceMgrReset(adi_adrv904x_Device_t* const device)
{
    adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                    sharedResourceIndex = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    for (sharedResourceIndex = 0U; sharedResourceIndex < ADRV904X_NUM_SHARED_RESOURCES; sharedResourceIndex++)
    {
        device->devStateInfo.sharedResourcePool[sharedResourceIndex].featureID      = (uint32_t)ADRV904X_FEATURE_UNUSED;
        device->devStateInfo.sharedResourcePool[sharedResourceIndex].channelMask    = 0U;
        device->devStateInfo.sharedResourcePool[sharedResourceIndex].semaphoreCount = 0U;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourcesAcquire(adi_adrv904x_Device_t* const        device,
                                                                 const adrv904x_SharedResourceType_e sharedResourceType,
                                                                 int32_t                             sharedResourceArr[],
                                                                 const uint32_t                      numSharedResources,
                                                                 const adrv904x_FeatureID_e          featureID,
                                                                 uint8_t* const                      resourceAcquistionStatus)
{
    adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                    sharedResourceIndex = 0U;
    adrv904x_SharedResourceID_e sharedResourceID    = ADRV904X_SHARED_RESOURCE_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check resourceAcquistionStatus pointer is not null */
    ADI_ADRV904X_NULL_PTR_RETURN(resourceAcquistionStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV904X_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, featureID, "featureID is not valid");
        return recoveryAction;
    }

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV904X_NUM_SHARED_RESOURCE_TYPES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceType,
                               "Invalid sharedResourceType encountered in adrv904x_SharedResourcesAcquire function");
        return recoveryAction;
    }

    /*Loop through each shared resource.
      1) Get Shared Resource unique ID
      2) Acquire Shared Resource
      3) If the resource is in use by another feature than the one which requested return a failed status*/
    for (sharedResourceIndex = 0U; sharedResourceIndex < numSharedResources; sharedResourceIndex++)
    {
        recoveryAction = adrv904x_SharedResourceIdGet(device,
                                                      sharedResourceType,
                                                      sharedResourceArr[sharedResourceIndex],
                                                      &sharedResourceID);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   sharedResourceID,
                                   "Error while attempting to resolve shared resource ID for the requested share resource");
            return recoveryAction;
        }

        recoveryAction = adrv904x_SharedResourceAcquire(device,
                                                        sharedResourceID,
                                                        featureID,
                                                        resourceAcquistionStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   resourceAcquistionStatus,
                                   "Error while attempting to acquire requested share resource");
            return recoveryAction;
        }

        if (*resourceAcquistionStatus == ADI_FAILURE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   resourceAcquistionStatus,
                                   "Error acquiring requested shared resource. Please check if the resource is already in use by another feature.");
            return recoveryAction;
            break;
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourcesRelease(adi_adrv904x_Device_t*  const       device,
                                                                 const adrv904x_SharedResourceType_e sharedResourceType,
                                                                 int32_t                             sharedResourceArr[],
                                                                 const uint32_t                      numSharedResources,
                                                                 const adrv904x_FeatureID_e          featureID,
                                                                 uint8_t* const                      resourceReleaseStatus)
{
    adi_adrv904x_ErrAction_e        recoveryAction        = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                    sharedResourceIndex = 0U;
    adrv904x_SharedResourceID_e sharedResourceID    = ADRV904X_SHARED_RESOURCE_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check resourceReleaseStatus pointer is not null */
    ADI_ADRV904X_NULL_PTR_RETURN(resourceReleaseStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV904X_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               featureID,
                               "featureID is not valid");
        return recoveryAction;
    }

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV904X_NUM_SHARED_RESOURCE_TYPES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceType,
                               "Invalid sharedResourceType encountered in adrv904x_SharedResourcesAcquire function");
        return recoveryAction;
    }

    /*Loop through each shared resource.
    1) Get Shared Resource unique ID
    2) Release Shared Resource
    3) If the resource is in use by another feature than the one which requested return a failed status*/
    for (sharedResourceIndex = 0U; sharedResourceIndex < numSharedResources; sharedResourceIndex++)
    {
        recoveryAction = adrv904x_SharedResourceIdGet(device,
                                                      sharedResourceType,
                                                      sharedResourceArr[sharedResourceIndex],
                                                      &sharedResourceID);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   sharedResourceID,
                                   "Error while attempting to resolve shared resource ID for the requested share resource");
            return recoveryAction;
        }

        recoveryAction = adrv904x_SharedResourceRelease(device,
                                                        sharedResourceID,
                                                        featureID,
                                                        resourceReleaseStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   resourceReleaseStatus,
                                   "Error while attempting to release requested share resource");
            return recoveryAction;
        }

        if (*resourceReleaseStatus == ADI_FAILURE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   resourceReleaseStatus,
                                   "Error releasing requested shared resource. Please check if the resource is already in use by another feature.");
            return recoveryAction;
            break;
        }
    }

    return recoveryAction;
}

/******************************************** Helper/Debug Functions ********************************************************************/

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceAvailabilityCheck(adi_adrv904x_Device_t* const        device,
                                                                          const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                          uint8_t* const                      sharedResourceAvailable)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check sharedResourceAvailable pointer is not null */
    ADI_ADRV904X_NULL_PTR_RETURN(sharedResourceAvailable);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceID,
                               "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Return success if shared resource pool index of shared resource ID has a feature other than UNUSED associated with it*/
    if (device->devStateInfo.sharedResourcePool[sharedResourceID].featureID == (uint32_t)ADRV904X_FEATURE_UNUSED)
    {
        *sharedResourceAvailable = (uint8_t)ADI_TRUE;
    }
    else
    {
        *sharedResourceAvailable = (uint8_t)ADI_FALSE;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceIdGet(adi_adrv904x_Device_t* const        device,
                                                              const adrv904x_SharedResourceType_e sharedResourceType,
                                                              int32_t                                sharedResource,
                                                              adrv904x_SharedResourceID_e* const    sharedResourceID)
{
    static const adrv904x_SharedResourceLut_t sharedResourceLut[] = {
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_00, ADRV904X_GPIO_00 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_01, ADRV904X_GPIO_01 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_02, ADRV904X_GPIO_02 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_03, ADRV904X_GPIO_03 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_04, ADRV904X_GPIO_04 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_05, ADRV904X_GPIO_05 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_06, ADRV904X_GPIO_06 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_07, ADRV904X_GPIO_07 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_08, ADRV904X_GPIO_08 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_09, ADRV904X_GPIO_09 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_10, ADRV904X_GPIO_10 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_11, ADRV904X_GPIO_11 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_12, ADRV904X_GPIO_12 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_13, ADRV904X_GPIO_13 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_14, ADRV904X_GPIO_14 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_15, ADRV904X_GPIO_15 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_16, ADRV904X_GPIO_16 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_17, ADRV904X_GPIO_17 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_18, ADRV904X_GPIO_18 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_19, ADRV904X_GPIO_19 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_20, ADRV904X_GPIO_20 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_21, ADRV904X_GPIO_21 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_22, ADRV904X_GPIO_22 },
        { ADRV904X_SHARED_RESOURCE_GPIO, ADI_ADRV904X_GPIO_23, ADRV904X_GPIO_23 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_00, ADRV904X_GPIO_ANA_00 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_01, ADRV904X_GPIO_ANA_01 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_02, ADRV904X_GPIO_ANA_02 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_03, ADRV904X_GPIO_ANA_03 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_04, ADRV904X_GPIO_ANA_04 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_05, ADRV904X_GPIO_ANA_05 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_06, ADRV904X_GPIO_ANA_06 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_07, ADRV904X_GPIO_ANA_07 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_08, ADRV904X_GPIO_ANA_08 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_09, ADRV904X_GPIO_ANA_09 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_10, ADRV904X_GPIO_ANA_10 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_11, ADRV904X_GPIO_ANA_11 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_12, ADRV904X_GPIO_ANA_12 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_13, ADRV904X_GPIO_ANA_13 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_14, ADRV904X_GPIO_ANA_14 },
        { ADRV904X_SHARED_RESOURCE_GPIO_ANALOG, ADI_ADRV904X_GPIO_ANA_15, ADRV904X_GPIO_ANA_15 }
    };
    /*****************Please add lut val for shared resources above this line*******************/

    adi_adrv904x_ErrAction_e  recoveryAction              = ADI_ADRV904X_ERR_ACT_NONE;
    uint32_t                sharedResourceIndex         = 0U;
    uint8_t                    validSharedResourceTypeFlag = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,  ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_RETURN(sharedResourceID);

    /*Range Check that shared resource type is valid*/
    if (sharedResourceType >= ADRV904X_NUM_SHARED_RESOURCE_TYPES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceType,
                               "Invalid sharedResourceType encountered in adrv904x_SharedResourcesAcquire function");
        return recoveryAction;
    }

    for (sharedResourceIndex = 0U; sharedResourceIndex < ADRV904X_NUM_SHARED_RESOURCES; sharedResourceIndex++)
    {
        if (sharedResourceLut[sharedResourceIndex].sharedResourceType == sharedResourceType)
        {
            if (sharedResourceLut[sharedResourceIndex].sharedResource == sharedResource)
            {
                *sharedResourceID           = sharedResourceLut[sharedResourceIndex].sharedResourceId;
                validSharedResourceTypeFlag = 1U;
                break;
            }
        }
    }

    /* Check not a valid shared resource ID found */
    if (validSharedResourceTypeFlag == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               validSharedResourceTypeFlag,
                               "Requested Shared Resource is not valid");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceFeatureGet(adi_adrv904x_Device_t* const        device,
                                                                   const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                   adrv904x_FeatureID_e*  const        featureID)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Range check that featureID is not null*/
    ADI_ADRV904X_NULL_PTR_RETURN(featureID);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceID,
                               "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Return the feature associated with shared resource pool*/
    *featureID = (adrv904x_FeatureID_e)device->devStateInfo.sharedResourcePool[sharedResourceID].featureID;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceChannelMaskSet(adi_adrv904x_Device_t* const        device,
                                                                       const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                       const uint32_t                      channelMask)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            sharedResourceID,
            "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Set the channelMask associated with shared resource pool*/
    device->devStateInfo.sharedResourcePool[sharedResourceID].channelMask = channelMask;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceChannelMaskGet(adi_adrv904x_Device_t* const        device,
                                                                       const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                       uint32_t *  const        channelMask)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Range check that channelMask is not null*/
    ADI_ADRV904X_NULL_PTR_RETURN(channelMask);

    /*Range Check that shared resource ID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            sharedResourceID,
            "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Return the channelMask associated with shared resource pool*/
    *channelMask = device->devStateInfo.sharedResourcePool[sharedResourceID].channelMask;

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceFeatureMaxSemaphoreCntGet(adi_adrv904x_Device_t* const    device,
                                                                                  const adrv904x_FeatureID_e      featureID,
                                                                                  uint8_t*                        maxSemaphoreCount)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /*Range check that the maximum semaphore count pointer is not null*/
    ADI_ADRV904X_NULL_PTR_RETURN(maxSemaphoreCount);

    /*Range Check that feature ID is valid*/
    if (featureID >= ADRV904X_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               featureID,
                               "The feature ID requested for retrieving maximum semaphore count is not valid");
        return recoveryAction;
    }

    switch (featureID)
    {
    case ADRV904X_FEATURE_GPIO_ADC_TEST_GEN_ENABLE:        /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_AGC_GAIN_CHANGE:            /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_AGC_DEC_GAIN:               /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_AGC_INC_GAIN:               /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_AGC_SLOWLOOP_FREEZE_ENABLE: /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_AGC_MANUAL_GAIN_LOCK:       /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_SELECT_S1:                  /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_TX_ATTEN_UPD_GPIO:          /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_DTX_FORCE_PIN:              /* Fallthrough */
    case ADRV904X_FEATURE_GPIO_CDDC_RSSI_ENABLE:           /* Fallthrough */
        *maxSemaphoreCount = 8U;
        break;
    default:
        *maxSemaphoreCount = 1U;
        break;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceAcquire(adi_adrv904x_Device_t* const        device,
                                                                const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                const adrv904x_FeatureID_e          featureID,
                                                                uint8_t*                            resourceAcquistionStatus)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;
    uint8_t              sharedResourceAvailable = (uint8_t)ADI_FALSE;
    adrv904x_FeatureID_e currentFeatureID        = ADRV904X_FEATURE_UNUSED;
    uint8_t              maxSemaphoreCount       = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check resourceAcquistionStatus pointer is not null */
    ADI_ADRV904X_NULL_PTR_RETURN(resourceAcquistionStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV904X_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, featureID, "featureID is not valid");
        return recoveryAction;
    }

    /*Range check that sharedResourceID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceID,
                               "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Initialize resourceAcquistionStatus to false and set to true only if shared resource available*/
    *resourceAcquistionStatus = ADI_FAILURE;

    /*sharedResourceID check is done in Shared Resource Available function*/
    /*Acquire resource if it is not already acquired by another feature or already acquired by the requesting feature*/
    recoveryAction = adrv904x_SharedResourceAvailabilityCheck(device,
                                                              sharedResourceID,
                                                              &sharedResourceAvailable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceAvailable,
                               "Error while checking for shared resource availability");
        return recoveryAction;
    }

    if (sharedResourceAvailable == ADI_TRUE)
    {
        device->devStateInfo.sharedResourcePool[sharedResourceID].featureID = (uint32_t)featureID;
        device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount++;
        *resourceAcquistionStatus = ADI_SUCCESS;
    }
    else
    {
        recoveryAction = adrv904x_SharedResourceFeatureGet(device,
                                                           sharedResourceID,
                                                           &currentFeatureID);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   currentFeatureID,
                                   "Error while acquiring shared resource feature");
            return recoveryAction;
        }

        if (currentFeatureID == featureID)
        {
            recoveryAction = adrv904x_SharedResourceFeatureMaxSemaphoreCntGet(device,
                                                                              featureID,
                                                                              &maxSemaphoreCount);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                return recoveryAction;
            }

            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount < maxSemaphoreCount)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount++;
                *resourceAcquistionStatus = ADI_SUCCESS;
            }
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_SharedResourceRelease(adi_adrv904x_Device_t* const        device,
                                                                const adrv904x_SharedResourceID_e   sharedResourceID,
                                                                const adrv904x_FeatureID_e          featureID,
                                                                uint8_t* const                      resourceReleaseStatus)
{
    adi_adrv904x_ErrAction_e    recoveryAction    = ADI_ADRV904X_ERR_ACT_NONE;
    uint8_t                    sharedResourceAvailable = ADI_FALSE;
    adrv904x_FeatureID_e    currentFeatureID        = ADRV904X_FEATURE_UNUSED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check resourceReleaseStatus pointer is not null */
    ADI_ADRV904X_NULL_PTR_RETURN(resourceReleaseStatus);

    /*Range check that featureID is valid*/
    if (featureID >= ADRV904X_NUM_FEATURES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, featureID, "featureID is not valid");
        return recoveryAction;
    }

    /*Range check that sharedResourceID is valid*/
    if (sharedResourceID >= ADRV904X_NUM_SHARED_RESOURCES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceID,
                               "sharedResourceID is not valid");
        return recoveryAction;
    }

    /*Initialize resourceReleaseStatus to false and set to true only if shared resource is released successfully*/
    *resourceReleaseStatus = ADI_FAILURE;

    /*sharedResourceID check is done in Shared Resource Available function*/
    /*Release the shared resource only if the requested feature ID matches the current feature ID or is not in use currently*/
    recoveryAction = adrv904x_SharedResourceAvailabilityCheck(device,
                                                              sharedResourceID,
                                                              &sharedResourceAvailable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               sharedResourceAvailable,
                               "Error while checking for shared resource availability");
        return recoveryAction;
    }

    if (sharedResourceAvailable == ADI_TRUE)
    {
        *resourceReleaseStatus = ADI_SUCCESS;
    }
    else
    {
        recoveryAction = adrv904x_SharedResourceFeatureGet(device,
                                                           sharedResourceID,
                                                           &currentFeatureID);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   currentFeatureID,
                                   "Error while acquiring shared resource feature");
            return recoveryAction;
        }

        if (currentFeatureID == featureID)
        {
            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount > 0)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount--;
            }

            if (device->devStateInfo.sharedResourcePool[sharedResourceID].semaphoreCount == 0)
            {
                device->devStateInfo.sharedResourcePool[sharedResourceID].featureID = (uint32_t)ADRV904X_FEATURE_UNUSED;
            }

            *resourceReleaseStatus = ADI_SUCCESS;
        }
    }

    return recoveryAction;
}
