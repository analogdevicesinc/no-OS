/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_gpio.c
 * \brief Contains ADRV904X gpio helper-utility related private function implementations
 *
 * ADRV904X API Version: 2.9.0.4
 */

#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_shared_resource_manager.h"
#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/bf/adrv904x_bf_rx_dig.h"
#include "../../private/bf/adrv904x_bf_orx_dig.h"
#include "../../private/bf/adrv904x_bf_tx_dig.h"

 #include "../../private/bf/adrv904x_bf_sequencer.h"

#include "adi_adrv904x_gpio.h"


#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_GPIO

/****************************************************************************
 * High Level GPIO Utility Functions
 ****************************************************************************
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalGet(adi_adrv904x_Device_t* const        device,
                                                        const adi_adrv904x_GpioPinSel_e     gpio,
                                                        adi_adrv904x_GpioSignal_e* const    signal,
                                                        uint32_t* const                     channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
        
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, signal);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, channelMask);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get the current feature for this gpio from the SRM */
    sharedResourceID = (adrv904x_SharedResourceID_e)gpio;
    recoveryAction = adrv904x_SharedResourceFeatureGet(device, sharedResourceID, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retrieving feature for selected GPIO");
        return recoveryAction;
    }
    
    /* Convert SRM feature to the associated gpio signal */
    recoveryAction = adrv904x_FromFeatureToSignalGet(device, featureID, signal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retrieving signal for selected GPIO");
        return recoveryAction;
    }
    
    /* Get channelMask from SRM */
    recoveryAction  = adrv904x_SharedResourceChannelMaskGet(device, sharedResourceID, channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while getting channelMask");
        return recoveryAction;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalFind(adi_adrv904x_Device_t* const        device,
                                                         adi_adrv904x_GpioPinSel_e* const    gpio,
                                                         const adi_adrv904x_GpioSignal_e     signal,
                                                         const adi_adrv904x_Channels_e       channelSel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e readbackSignal = ADI_ADRV904X_GPIO_SIGNAL_INVALID;
    uint32_t readbackChannelMask = (uint32_t)ADI_ADRV904X_CHOFF;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpio);

    uint8_t gpioIterator = 0U;

    /* If this function returns INVALID gpio, it means selected signal isn't mapped to any GPIO */
    *gpio = ADI_ADRV904X_GPIO_INVALID;
    
    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    if ((channelSel != ADI_ADRV904X_CHOFF) &&
        (channelSel != ADI_ADRV904X_CH0) &&
        (channelSel != ADI_ADRV904X_CH1) &&
        (channelSel != ADI_ADRV904X_CH2) &&
        (channelSel != ADI_ADRV904X_CH3) &&
        (channelSel != ADI_ADRV904X_CH4) &&
        (channelSel != ADI_ADRV904X_CH5) &&
        (channelSel != ADI_ADRV904X_CH6) &&
        (channelSel != ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelSel,
                               "Invalid channel selected");
        return recoveryAction;
    }
    
    /* Check all GPIOs until we find the matching signal */
    for (gpioIterator = 0U; gpioIterator < (uint8_t)ADI_ADRV904X_GPIO_INVALID; gpioIterator++)
    {
        recoveryAction = adrv904x_GpioSignalGet(device,
                                                (adi_adrv904x_GpioPinSel_e)gpioIterator,
                                                &readbackSignal,
                                                &readbackChannelMask);
        
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read signal for selected GPIO");
            return recoveryAction;
        }

        if ((readbackSignal == signal) && 
            (((readbackChannelMask & (uint32_t)channelSel) != 0U) || (readbackChannelMask == (uint32_t)channelSel)))
        {
            *gpio = (adi_adrv904x_GpioPinSel_e)gpioIterator;
            break;
        }
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalRelease(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            const adi_adrv904x_GpioSignal_e     signal,
                                                            const uint32_t                      channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e initSignal = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t initChannelMask = 0U;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    uint8_t resourceStatus = ADI_FAILURE;
    uint8_t channelIdx = 0U;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    uint8_t isValid = ADI_FALSE;
    
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get signal info struct */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while obtaining signal info");
        return recoveryAction;
    }
    
    /* Check that this signal is a digital gpio signal domain */
    if ((info.domain != ADRV904X_GPIO_DOMAIN_DIGITAL) &&
        (info.domain != ADRV904X_GPIO_DOMAIN_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 info.domain,
                                 "Selected GPIO signal is not valid for digital GPIO pins");
        return recoveryAction;
    }

    /* Check channelMask validity for this signal's route type */
    recoveryAction = adrv904x_GpioRouteValidChannelMaskCheck( device, info.route, channelMask, &isValid );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while checking validity of channel Mask for GPIO signal");
        return recoveryAction;
    }
    if ( isValid == ADI_FALSE )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelMask,
                                 "Selected channelMask is not valid for selected GPIO signal");
        return recoveryAction;
    }

    /* Get the feature for the signal */
    recoveryAction = adrv904x_FromSignalToFeatureGet(device, signal, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while feature for selected signal");
        return recoveryAction;
    }
    
    /* Read the current signal for this gpio. Getter function will range check gpio */
    recoveryAction = adrv904x_GpioSignalGet(device, gpio, &initSignal, &initChannelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving current signal for the selected gpio");
        return recoveryAction;
    }
    
    /* If gpio is unused, then do nothing */
    if (initSignal == ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    /* If gpio is being used, then continue */
    else
    {
        /* signal is not channel instanced. Disconnect in hardware */
        if ( channelMask == 0U ) 
        {
            /* Disconnect signal from GPIO in hardware */
            recoveryAction = adrv904x_GpioConnect( device, ADI_DISABLE, gpio, &info, 0U );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while disconnecting GPIO from signal");
                return recoveryAction;
            }

            /* Release the shared resource for the feature */
            sharedResourceID = (adrv904x_SharedResourceID_e)gpio;
            recoveryAction = adrv904x_SharedResourceRelease(device, sharedResourceID, featureID, &resourceStatus);
            if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                (resourceStatus != ADI_SUCCESS))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while attempting to release shared resource");
                return recoveryAction;
            }
        }

        /* signal is channel-instanced, Disconnect selected channels in hardware */
        else
        {
            for ( channelIdx = 0U; channelIdx < 8U; channelIdx++ )
            {
                if ( ((channelMask >> channelIdx) & 1U) == 1U )
                {
                    /* Disconnect signal/chIdx from GPIO in hardware */
                    recoveryAction = adrv904x_GpioConnect( device, ADI_DISABLE, gpio, &info, channelIdx );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while disconnecting GPIO from signal");
                        return recoveryAction;
                    }

                    /* Release the shared resource for the feature */
                    sharedResourceID = (adrv904x_SharedResourceID_e)gpio;
                    recoveryAction = adrv904x_SharedResourceRelease(device, sharedResourceID, featureID, &resourceStatus);
                    if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                        (resourceStatus != ADI_SUCCESS))
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while attempting to release shared resource");
                        return recoveryAction;
                    }
                
                    /* Update channelMask by clearing bit for this channelIdx. Save channelMask */
                    initChannelMask &= ~(1U << channelIdx);
                    recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, initChannelMask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(  &device->common,
                                                recoveryAction,
                                                "Error while set channelMask");
                        return recoveryAction;
                    }
                } /* end of ifblock for channel included in channelMask */
                
            } /* end channel idx for loop */
        } /* end else block for channel-indexed signals */
    } /* end else block for a used GPIO */

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalSet(adi_adrv904x_Device_t* const        device,
                                                        const adi_adrv904x_GpioPinSel_e     gpio,
                                                        const adi_adrv904x_GpioSignal_e     signal,
                                                        const uint32_t                      channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t isValid = ADI_FALSE;
    uint32_t pinRequest = 1U;
    adi_adrv904x_GpioPinSel_e prevGpio = ADI_ADRV904X_GPIO_INVALID;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
    adi_adrv904x_GpioSignal_e initSig = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t initChannelMask = 0U;
    uint8_t channelIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    uint8_t resourceStatus = ADI_FAILURE;
     
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get signal info struct */
    /* Run checks against selected gpio channelMask */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while obtaining signal info");
        return recoveryAction;
    }
    
    /* Check that this signal is a digital gpio signal domain */
    if (info.domain != ADRV904X_GPIO_DOMAIN_DIGITAL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 info.domain,
                                 "Selected GPIO signal is not valid for digital GPIO pins");
        return recoveryAction;
    }
    
    /* Check that the user selected a gpio pin allowed by the select signal's pinMask */
    pinRequest = (1U << (uint32_t)(gpio));
    if ( (pinRequest & info.pinMask) != pinRequest)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Selected GPIO cannot be used to route the requested signal.");
        return recoveryAction;
    }
    
    /* Check channelMask validity for this signal's route type */
    recoveryAction = adrv904x_GpioRouteValidChannelMaskCheck( device, info.route, channelMask, &isValid );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while checking validity of channel Mask for GPIO signal");
        return recoveryAction;
    }
    if ( isValid == ADI_FALSE )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelMask,
                                 "Selected channelMask is not valid for selected GPIO signal");
        return recoveryAction;
    }

    /* Get SRM featureID that maps to the requested signal */
    recoveryAction = adrv904x_FromSignalToFeatureGet( device, signal, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while feature for selected signal");
        return recoveryAction;
    }
    
    /* If signal is not channel-instanced, release any other GPIO currently routing sig that are about to be set */
    if ( channelMask == 0U )
    {
        recoveryAction = adrv904x_GpioSignalFind( device, &prevGpio, signal, ADI_ADRV904X_CHOFF );
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   "Error while finding GPIO routing selected signal");
            return recoveryAction;
        }
        
        /* If prevGpio shows a pin other than the target is routing the signal, release it */
        if ( (prevGpio != ADI_ADRV904X_GPIO_INVALID) && (prevGpio != gpio) )
        {
            recoveryAction = adrv904x_GpioSignalRelease( device, prevGpio, signal, channelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while finding GPIO routing selected signal");
                return recoveryAction;
            }
        }
        
        /* if prevGpio was not the target, connect the signal to target */
        if (prevGpio != gpio)
        {
            
            /* Call GpioSignalGet() for target GPIO. If UNUSED, setup. Else failure. */
            recoveryAction = adrv904x_GpioSignalGet( device, gpio, &initSig, &initChannelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error retrieving current status of target GPIO");
                return recoveryAction;
            }
            if ( initSig != ADI_ADRV904X_GPIO_SIGNAL_UNUSED )
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         gpio,
                                         "Invalid GPIO selected. GPIO already in use by another signal");
                return recoveryAction;
            }
            
            /* Acquire Shared Resource */
            sharedResourceID = (adrv904x_SharedResourceID_e)gpio;
            resourceStatus = ADI_FAILURE;
            recoveryAction  = adrv904x_SharedResourceAcquire(device, sharedResourceID, featureID, &resourceStatus);
            if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                (resourceStatus != ADI_SUCCESS))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while acquiring shared resource");
                return recoveryAction;
            }

            /* Save channelMask */
            recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(  &device->common,
                                        recoveryAction,
                                        "Error while set channelMask");
                return recoveryAction;
            }
            
            /* Connect signal to GPIO in hardware */
            recoveryAction = adrv904x_GpioConnect( device, ADI_ENABLE, gpio, &info, 0U );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while connecting GPIO to signal");
                return recoveryAction;
            }
        }
    }
    
    /* If channel-instanced signal, release any other GPIO currently routing EACH sig/chan that are about to be set */
    else
    {
        for (channelIdx = 0U; channelIdx < 8U; channelIdx++)
        {
            chanSel = 1U << channelIdx;
            if ( ((channelMask >> channelIdx) & 1U) == 1U )
            {
                /* This channel is in the channelMask. Search for GPIO to release */
                recoveryAction = adrv904x_GpioSignalFind(device, &prevGpio, signal, (adi_adrv904x_Channels_e)(chanSel));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common,
                                           recoveryAction,
                                           "Error while finding GPIO routing selected signal");
                    return recoveryAction;
                }
                
                /* If prevGpio shows a pin other than the target is routing the signal/chIdx, release it */
                if ( (prevGpio != ADI_ADRV904X_GPIO_INVALID) && (prevGpio != gpio) )
                {
                    recoveryAction = adrv904x_GpioSignalRelease( device, prevGpio, signal, (adi_adrv904x_Channels_e)(1U << channelIdx) );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while finding GPIO routing selected signal");
                        return recoveryAction;
                    }
                }
                
                /* if prevGpio was not the target, connect the signal/chIdx to target */
                if (prevGpio != gpio)
                {
                    /* Call GpioSignalGet() for target GPIO. If UNUSED or already the target signal, setup. Else failure. */
                    recoveryAction = adrv904x_GpioSignalGet( device, gpio, &initSig, &initChannelMask );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error retrieving current status of target GPIO");
                        return recoveryAction;
                    }
                    if ( (initSig != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
                         (initSig != signal ) )
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT( &device->common,
                                                 recoveryAction,
                                                 gpio,
                                                 "Invalid GPIO selected. GPIO already in use by another signal");
                        return recoveryAction;
                    }
                    
                    /* Acquire Shared Resource */
                    sharedResourceID = (adrv904x_SharedResourceID_e)gpio;
                    resourceStatus = ADI_FAILURE;
                    recoveryAction  = adrv904x_SharedResourceAcquire(device, sharedResourceID, featureID, &resourceStatus);
                    if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                        (resourceStatus != ADI_SUCCESS))
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_API_ERROR_REPORT( &device->common,
                            recoveryAction,
                            "Error while acquiring shared resource");
                        return recoveryAction;
                    }

                    /* Update chanelMask by setting bit for this channelIdx. Save channelMask */
                    initChannelMask |= (1U << channelIdx);
                    recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, initChannelMask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(  &device->common,
                                                recoveryAction,
                                                "Error while set channelMask");
                        return recoveryAction;
                    }
                    
                    /* Connect signal to GPIO in hardware */
                    recoveryAction = adrv904x_GpioConnect( device, ADI_ENABLE, gpio, &info, channelIdx );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while finding connecting GPIO to signal");
                        return recoveryAction;
                    }
                }
                
            } /* end if block for channelMask including channelIdx */
        } /* end for loop over channelIdx */
    } /* end channel-instanced signal "else" block */
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalGet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              adi_adrv904x_GpioSignal_e* const    signal,
                                                              uint32_t* const                     channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    uint32_t sharedResourceIdVal = 0U;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
        
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, signal);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, channelMask);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_ANA_00) ||
        (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get the current feature for this gpio from the SRM */
    sharedResourceIdVal = (uint32_t)gpio + (uint32_t)ADRV904X_GPIO_ANA_00;
    sharedResourceID = (adrv904x_SharedResourceID_e)(sharedResourceIdVal);
    recoveryAction = adrv904x_SharedResourceFeatureGet(device, sharedResourceID, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving feature for selected GPIO");
        return recoveryAction;
    }
    
    /* Convert SRM feature to the associated gpio signal */
    recoveryAction = adrv904x_FromFeatureToSignalGet(device, featureID, signal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving signal for selected GPIO");
        return recoveryAction;
    }
    
    /* Get channelMask from SRM */
    recoveryAction  = adrv904x_SharedResourceChannelMaskGet(device, sharedResourceID, channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while get channelMask");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalFind(adi_adrv904x_Device_t* const           device,
                                                               adi_adrv904x_GpioAnaPinSel_e* const    gpio,
                                                               const adi_adrv904x_GpioSignal_e        signal,
                                                               const adi_adrv904x_Channels_e          channelSel)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e readbackSignal = ADI_ADRV904X_GPIO_SIGNAL_INVALID;
    uint32_t readbackChannelMask = (uint32_t)ADI_ADRV904X_CHOFF;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpio);

    uint8_t gpioIterator = 0U;

    /* If this function returns INVALID gpio, it means selected signal isn't mapped to any GPIO */
    *gpio = ADI_ADRV904X_GPIO_ANA_INVALID;

    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    if ((channelSel != ADI_ADRV904X_CHOFF) &&
        (channelSel != ADI_ADRV904X_CH0) &&
        (channelSel != ADI_ADRV904X_CH1) &&
        (channelSel != ADI_ADRV904X_CH2) &&
        (channelSel != ADI_ADRV904X_CH3) &&
        (channelSel != ADI_ADRV904X_CH4) &&
        (channelSel != ADI_ADRV904X_CH5) &&
        (channelSel != ADI_ADRV904X_CH6) &&
        (channelSel != ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            channelSel,
            "Invalid channel selected");
        return recoveryAction;
    }
    
    /* Check all GPIOs until we find the matching signal */
    for (gpioIterator = 0U; gpioIterator < (uint8_t)ADI_ADRV904X_GPIO_ANA_INVALID; gpioIterator++)
    {
        recoveryAction = adrv904x_GpioAnalogSignalGet(  device,
                                                        (adi_adrv904x_GpioAnaPinSel_e)gpioIterator,
                                                       &readbackSignal,
                                                       &readbackChannelMask);
        
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read signal for selected GPIO");
            return recoveryAction;
        }

        if ((readbackSignal == signal) && 
            (((readbackChannelMask & (uint32_t)channelSel) != 0U) || (readbackChannelMask == (uint32_t)channelSel)))
        {
            *gpio = (adi_adrv904x_GpioAnaPinSel_e)gpioIterator;
            break;
        }
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalRelease(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                                  const adi_adrv904x_GpioSignal_e     signal,
                                                                  const uint32_t                      channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e initSignal = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t initChannelMask = 0U;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    uint32_t sharedResourceIdVal = 0U;
    uint8_t resourceStatus = ADI_FAILURE;
    uint8_t channelIdx = 0U;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    uint8_t isValid = ADI_FALSE;
    
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_ANA_00) ||
        (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get signal info struct */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while obtaining signal info");
        return recoveryAction;
    }
    
    /* Check that this signal is an analog gpio signal domain */
    if ((info.domain != ADRV904X_GPIO_DOMAIN_ANALOG) &&
        (info.domain != ADRV904X_GPIO_DOMAIN_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            info.domain,
            "Selected GPIO signal is not valid for analog GPIO pins");
        return recoveryAction;
    }
    
    /* Check channelMask validity for this signal's route type */
    recoveryAction = adrv904x_GpioRouteValidChannelMaskCheck( device, info.route, channelMask, &isValid );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while checking validity of channel Mask for GPIO signal");
        return recoveryAction;
    }
    if ( isValid == ADI_FALSE )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelMask,
                                 "Selected channelMask is not valid for selected GPIO signal");
        return recoveryAction;
    }
    
    /* Get the feature for the signal */
    recoveryAction = adrv904x_FromSignalToFeatureGet(device, signal, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
            recoveryAction,
            "Error while feature for selected signal");
        return recoveryAction;
    }
    
    /* Read the current signal for this gpio. Getter function will range check gpio */
    recoveryAction = adrv904x_GpioAnalogSignalGet(device, gpio, &initSignal, &initChannelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
            recoveryAction,
            "Error while retreiving current signal for the selected gpio");
        return recoveryAction;
    }
    
    /* If gpio is unused, then do nothing */
    if (initSignal == ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    /* If gpio is being used, then continue */
    else
    {
        /* signal is not channel instanced. Disconnect in hardware */
        if ( channelMask == 0U ) 
        {
            /* Disconnect signal from GPIO in hardware */
            recoveryAction = adrv904x_GpioAnalogConnect( device, ADI_DISABLE, gpio, &info, 0U );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while disconnecting GPIO from signal");
                return recoveryAction;
            }

            /* Release the shared resource for the feature */
            sharedResourceIdVal = (uint32_t)gpio + (uint32_t)ADRV904X_GPIO_ANA_00;
            sharedResourceID = (adrv904x_SharedResourceID_e)(sharedResourceIdVal);
            recoveryAction = adrv904x_SharedResourceRelease(device, sharedResourceID, featureID, &resourceStatus);
            if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                (resourceStatus != ADI_SUCCESS))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while attempting to release shared resource");
                return recoveryAction;
            }
            
        }

        /* signal is channel-instanced, Disconnect selected channels in hardware */
        else
        {
            for ( channelIdx = 0U; channelIdx < 8U; channelIdx++ )
            {
                if ( ((channelMask >> channelIdx) & 1U) == 1U )
                {
                    /* Disconnect signal/chIdx from GPIO in hardware */
                    recoveryAction = adrv904x_GpioAnalogConnect( device, ADI_DISABLE, gpio, &info, channelIdx );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while disconnecting GPIO from signal");
                        return recoveryAction;
                    }

                    /* Release the shared resource for the feature */
                    sharedResourceIdVal = (uint32_t)gpio + (uint32_t)ADRV904X_GPIO_ANA_00;
                    sharedResourceID = (adrv904x_SharedResourceID_e)(sharedResourceIdVal);
                    recoveryAction = adrv904x_SharedResourceRelease(device, sharedResourceID, featureID, &resourceStatus);
                    if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                        (resourceStatus != ADI_SUCCESS))
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while attempting to release shared resource");
                        return recoveryAction;
                    }
                
                    /* Update channelMask by clearing bit for this channelIdx. Save channelMask */
                    initChannelMask &= ~(1U << channelIdx);
                    recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, initChannelMask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(  &device->common,
                                                recoveryAction,
                                                "Error while set channelMask");
                        return recoveryAction;
                    }
                } /* end of ifblock for channel included in channelMask */
                
            } /* end channel idx for loop */
        } /* end else block for channel-indexed signals */
    } /* end else block for a used GPIO */

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogSignalSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              const adi_adrv904x_GpioSignal_e     signal,
                                                              const uint32_t                      channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t isValid = ADI_FALSE;
    uint32_t pinRequest = 1U;
    adi_adrv904x_GpioAnaPinSel_e prevGpio = ADI_ADRV904X_GPIO_ANA_INVALID;
    adrv904x_SharedResourceID_e sharedResourceID = ADRV904X_SHARED_RESOURCE_INVALID;
    uint32_t sharedResourceIdVal = 0U;
    adrv904x_FeatureID_e featureID = ADRV904X_FEATURE_UNUSED;
    adi_adrv904x_GpioSignal_e initSig = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint32_t initChannelMask = 0U;
    uint8_t channelIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    uint8_t resourceStatus = ADI_FAILURE;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_ANA_00) ||
        (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Range check signal. */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get signal info struct */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while obtaining signal info");
        return recoveryAction;
    }
    
    /* Check that this signal is in analog gpio signal domain */
    if (info.domain != ADRV904X_GPIO_DOMAIN_ANALOG)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            info.domain,
            "Selected GPIO signal is not valid for analog GPIO pins");
        return recoveryAction;
    }
    
    /* Check that the user selected a gpio/signal combination with a valid route */
    pinRequest = (1U << (uint32_t)(gpio));
    if ((pinRequest & info.pinMask) != pinRequest)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Selected GPIO cannot be used to route the requested signal.");
        return recoveryAction;
    }
    
    /* Check channelMask validity for this signal's route type */
    recoveryAction = adrv904x_GpioRouteValidChannelMaskCheck( device, info.route, channelMask, &isValid );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while checking validity of channel Mask for GPIO signal");
        return recoveryAction;
    }
    if ( isValid == ADI_FALSE )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelMask,
                                 "Selected channelMask is not valid for selected GPIO signal");
        return recoveryAction;
    }
    
    /* Get SRM featureID that maps to the requested signal */
    recoveryAction = adrv904x_FromSignalToFeatureGet( device, signal, &featureID);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while feature for selected signal");
        return recoveryAction;
    }
    
    /* If signal is not channel-instanced, release any other GPIO currently routing sig that are about to be set */
    if ( channelMask == 0U )
    {
        recoveryAction  = adrv904x_GpioAnalogSignalFind(device, &prevGpio, signal, ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while finding GPIO routing selected signal");
            return recoveryAction;
        }
        
        /* If prevGpio shows a pin other than the target is routing the signal, release it */
        if ( (prevGpio != ADI_ADRV904X_GPIO_ANA_INVALID) && (prevGpio != gpio) )
        {
            recoveryAction = adrv904x_GpioAnalogSignalRelease( device, prevGpio, signal, channelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while finding GPIO routing selected signal");
                return recoveryAction;
            }
        }
        
        /* if prevGpio was not the target, connect the signal to target */
        if (prevGpio != gpio)
        {
            
            /* Call GpioAnalogSignalGet() for target GPIO. If UNUSED, setup. Else failure. */
            recoveryAction = adrv904x_GpioAnalogSignalGet( device, gpio, &initSig, &initChannelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error retrieving current status of target GPIO");
                return recoveryAction;
            }
            if ( initSig != ADI_ADRV904X_GPIO_SIGNAL_UNUSED )
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         gpio,
                                         "Invalid GPIO selected. GPIO already in use by another signal");
                return recoveryAction;
            }
            
            /* Acquire Shared Resource */
            sharedResourceIdVal = (uint32_t)gpio + (uint32_t)ADRV904X_GPIO_ANA_00;
            sharedResourceID = (adrv904x_SharedResourceID_e)(sharedResourceIdVal);
            resourceStatus = ADI_FAILURE;
            recoveryAction  = adrv904x_SharedResourceAcquire(device, sharedResourceID, featureID, &resourceStatus);
            if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                (resourceStatus != ADI_SUCCESS))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while acquiring shared resource");
                return recoveryAction;
            }

            /* Save channelMask */
            recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, 0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(  &device->common,
                                        recoveryAction,
                                        "Error while set channelMask");
                return recoveryAction;
            }
            
            /* Connect signal to GPIO in hardware */
            recoveryAction = adrv904x_GpioAnalogConnect( device, ADI_ENABLE, gpio, &info, 0U );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common,
                                       recoveryAction,
                                       "Error while connecting GPIO to signal");
                return recoveryAction;
            }
        }
    }
    
    /* If channel-instanced signal, release any other GPIO currently routing EACH sig/chan that are about to be set */
    else
    {
        for (channelIdx = 0U; channelIdx < 8U; channelIdx++)
        {
            if ( ((channelMask >> channelIdx) & 1U) == 1U )
            {
                chanSel = 1U << channelIdx;
                /* This channel is in the channelMask. Search for GPIO to release */
                recoveryAction = adrv904x_GpioAnalogSignalFind(device, &prevGpio, signal, (adi_adrv904x_Channels_e)(chanSel));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT( &device->common,
                                           recoveryAction,
                                           "Error while finding GPIO routing selected signal");
                    return recoveryAction;
                }
                
                /* If prevGpio shows a pin other than the target is routing the signal/chIdx, release it */
                if ( (prevGpio != ADI_ADRV904X_GPIO_ANA_INVALID) && (prevGpio != gpio) )
                {
                    recoveryAction = adrv904x_GpioAnalogSignalRelease( device, prevGpio, signal, (adi_adrv904x_Channels_e)(1U << channelIdx) );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while finding GPIO routing selected signal");
                        return recoveryAction;
                    }
                }
                
                /* if prevGpio was not the target, connect the signal/chIdx to target */
                if (prevGpio != gpio)
                {
                    /* Call GpioSignalGet() for target GPIO. If UNUSED or already the target signal, setup. Else failure. */
                    recoveryAction = adrv904x_GpioAnalogSignalGet( device, gpio, &initSig, &initChannelMask );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error retrieving current status of target GPIO");
                        return recoveryAction;
                    }
                    if ( (initSig != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
                         (initSig != signal ) )
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT( &device->common,
                                                 recoveryAction,
                                                 gpio,
                                                 "Invalid GPIO selected. GPIO already in use by another signal");
                        return recoveryAction;
                    }
                    
                    /* Acquire Shared Resource */
                    sharedResourceIdVal = (uint32_t)gpio + (uint32_t)ADRV904X_GPIO_ANA_00;
                    sharedResourceID = (adrv904x_SharedResourceID_e)(sharedResourceIdVal);
                    resourceStatus = ADI_FAILURE;
                    recoveryAction  = adrv904x_SharedResourceAcquire(device, sharedResourceID, featureID, &resourceStatus);
                    if ((recoveryAction != ADI_ADRV904X_ERR_ACT_NONE) ||
                        (resourceStatus != ADI_SUCCESS))
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while acquiring shared resource");
                        return recoveryAction;
                    }

                    /* Update chanelMask by setting bit for this channelIdx. Save channelMask */
                    initChannelMask |= (1U << channelIdx);
                    recoveryAction  = adrv904x_SharedResourceChannelMaskSet(device, sharedResourceID, initChannelMask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(  &device->common,
                                                recoveryAction,
                                                "Error while set channelMask");
                        return recoveryAction;
                    }
                    
                    /* Connect signal to GPIO in hardware */
                    recoveryAction = adrv904x_GpioAnalogConnect( device, ADI_ENABLE, gpio, &info, channelIdx );
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT( &device->common,
                                               recoveryAction,
                                               "Error while finding connecting GPIO to signal");
                        return recoveryAction;
                    }
                }
                
            } /* end if block for channelMask including channelIdx */
        } /* end for loop over channelIdx */
    } /* end channel-instanced signal "else" block */
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioMonitorOutSignalValidCheck(adi_adrv904x_Device_t* const       device,
                                                                         const adi_adrv904x_GpioSignal_e    signal,
                                                                         const uint8_t                      channel,
                                                                         uint8_t* const                     isValidFlag,
                                                                         uint32_t* const                    channelMask)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, isValidFlag);
    
    /* Range check signal */
    if ((signal <= ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, signal, "Invalid signal selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get signal info struct */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while obtaining signal info");
        return recoveryAction;
    }
    
    /* Check if signal/channel is a monitor output using the route */
    switch (info.route)
    {
    case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL:
        /* this route type is valid. channel is not used so ignore it */
        *isValidFlag = ADI_TRUE;
        if (channelMask != NULL)
        {
            *channelMask = 0U;
        }
        break;
    case ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX: /* FALLTHROUGH */
    case ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX:
        /* these route types are valid, but channel must be in range 0-7 */
        *isValidFlag = (channel < ADI_ADRV904X_MAX_TXCHANNELS) ? ADI_TRUE : ADI_FALSE;
        if(channelMask != NULL)
        {
            *channelMask = (1U << channel);
        }
        break;
    case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX:
        /* these route types are valid, but channel must be in range 0-1 */
        *isValidFlag = (channel < ADI_ADRV904X_MAX_ORX_ONLY) ? ADI_TRUE : ADI_FALSE;
        if (channelMask != NULL)
        {
            *channelMask = (1U << channel);
        }
        break;
    default:
        *isValidFlag = ADI_FALSE;
        if (channelMask != NULL)
        {
            *channelMask = 0U;
        }
        break;
    }
    
    return recoveryAction;
}


/****************************************************************************
 * Low Level GPIO Utility Helper Functions
 ****************************************************************************
 */
ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioSignalInfoGet(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioSignal_e     signal,
                                                            adrv904x_GpioSignalInfo_t* const    info)
{
    /* GPIO Signal Info LUT. Maps GPIO signal to feature, domain type, route type, pinMask, topSelect, and targetSelect */
    static const adrv904x_GpioSignalInfo_t gpioSignalLut[] = {
        { ADI_ADRV904X_GPIO_SIGNAL_UNUSED,                                  ADRV904X_GPIO_DOMAIN_NONE,       ADRV904X_GPIO_ROUTE_OFF,                    0x00FFFFFFU, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDIO,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002001U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDO,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004002U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CLK,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008004U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CSB,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010008U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_UART_PADRXSIN,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_UART_PADCTS,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_UART_PADRTSOUT,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_UART_PADTXSOUT,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 1U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_CLK_OUT,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_0,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_1,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_2,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_3,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_4,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_5,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_6,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_7,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 2U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_0,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_1,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_2,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_3,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_4,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_5,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_6,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_7,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_8,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_9,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_10,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_11,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_12,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_13,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_14,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_15,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_16,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_17,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_18,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_19,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_20,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_21,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_22,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_23,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_6,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_7,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_8,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_9,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_10,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_11,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_12,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_13,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_14,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_15,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_16,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_17,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_18,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_19,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_20,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_21,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_22,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_23,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_0,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_1,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_2,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_3,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_4,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_5,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_6,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_7,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_8,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_9,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_10,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_11,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_12,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_13,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_14,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_15,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_16,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_17,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_18,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_19,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_20,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_21,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_22,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_23,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_0,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_1,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_2,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_3,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_4,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_5,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_6,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_7,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_8,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_9,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_10,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_11,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_12,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_13,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_14,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_15,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_16,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_17,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_18,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_19,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_20,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_21,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_22,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_23,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 0 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 1 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_2,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 2 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_3,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_4,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_5,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 5 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 6 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 7 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 8 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 9 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 10 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 11 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_PLL_LOCKED,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 12 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_OVERRANGE_LOW_FLAG,            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 13 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_OVERRANGE_HIGH_FLAG,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 14 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_COMP_OUT,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 15 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_CAL_IN_PROGRESS,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 16 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_0,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 17 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_1,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 18 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_2,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 19 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_3,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 20 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_4,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 21 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_5,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 22 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_6,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 23 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_7,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 24 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_8,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 25 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_0,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 26 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_1,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 27 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_2,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 28 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_3,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 29 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_4,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 30 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_5,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 31 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_6,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 32 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_7,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 33 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_8,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 34 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_9,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 35 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_10,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 36 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 37 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 38 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 39 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 40 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 41 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 42 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_6,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 43 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_7,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 44 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_8,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 45 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_9,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 46 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_10,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 47 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_11,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 48 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_0,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 49 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_1,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 50 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_2,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 51 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_3,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 52 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_4,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 53 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_5,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 54 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_6,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 55 },
        { ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_7,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 56 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 57 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 58 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_2,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 59 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_3,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 60 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_4,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 61 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_5,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 62 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 63 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 64 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 65 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 66 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 67 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 68 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_PLL_LOCKED,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 69 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_OVERRANGE_LOW_FLAG,            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 70 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_OVERRANGE_HIGH_FLAG,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 71 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_COMP_OUT,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 72 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_CAL_IN_PROGRESS,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 73 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_0,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 74 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_1,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 75 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_2,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 76 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_3,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 77 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_4,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 78 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_5,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 79 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_6,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 80 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_7,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 81 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_8,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 82 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_0,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 83 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_1,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 84 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_2,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 85 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_3,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 86 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_4,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 87 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_5,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 88 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_6,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 89 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_7,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 90 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_8,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 91 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_9,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 92 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_10,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 93 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 94 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 95 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 96 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 97 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 98 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 99 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_6,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 100 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_7,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 101 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_8,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 102 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_9,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 103 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_10,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 104 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_11,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 105 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_0,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 106 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_1,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 107 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_2,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 108 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_3,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 109 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_4,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 110 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_5,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 111 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_6,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 112 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_7,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 113 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 114 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 115 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_2,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 116 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_3,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 117 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_4,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 118 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_5,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 119 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 120 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 121 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 122 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 123 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 124 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 125 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_PLL_LOCKED,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 126 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_OVERRANGE_LOW_FLAG,            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 127 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_OVERRANGE_HIGH_FLAG,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 128 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_COMP_OUT,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 129 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_CAL_IN_PROGRESS,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 130 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_0,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 131 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_1,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 132 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_2,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 133 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_3,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 134 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_4,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 135 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_5,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 136 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_6,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 137 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_7,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 138 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_8,       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 139 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_0,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 140 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_1,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 141 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_2,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 142 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_3,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 143 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_4,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 144 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_5,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 145 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_6,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 146 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_7,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 147 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_8,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 148 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_9,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 149 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_10,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 150 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 151 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 152 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 153 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_3,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 154 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_4,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 155 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_5,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 156 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_6,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 157 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_7,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 158 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_8,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 159 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_9,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 160 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_10,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 161 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_11,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 162 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_0,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 163 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_1,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 164 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_2,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 165 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_3,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 166 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_4,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 167 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_5,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 168 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_6,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 169 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_7,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 170 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_0,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 171 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_1,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 172 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_2,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 173 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_3,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 174 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_4,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 175 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_5,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 176 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_6,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 177 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_7,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 178 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_0,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 179 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_1,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 180 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_2,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 181 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_3,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 182 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_4,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 183 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_5,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 184 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_6,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 185 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_7,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 186 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_0,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 187 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_1,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 188 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_2,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 189 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_0,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 190 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_1,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 191 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_2,               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 192 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 193 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 194 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_2,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 195 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_3,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 196 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_4,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 197 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_5,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 198 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_6,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 199 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_7,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 200 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 201 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 202 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_2,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 203 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_3,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 204 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_4,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 205 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_5,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 206 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_6,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 207 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_7,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 208 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_0,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 209 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_1,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 210 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_2,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 211 },
        { ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_3,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 212 },
        { ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_CORE,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 213 },
        { ADI_ADRV904X_GPIO_SIGNAL_REF_CLK,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 214 },
        { ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_ARM0_ERROR,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 215 },
        { ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_ARM1_ERROR,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 216 },
        { ADI_ADRV904X_GPIO_SIGNAL_DIG_POWERGOOD,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 217 },
        { ADI_ADRV904X_GPIO_SIGNAL_MASTER_BIAS_CLK,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 218 },
        { ADI_ADRV904X_GPIO_SIGNAL_MBIAS_IGEN_PD_0,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 219 },
        { ADI_ADRV904X_GPIO_SIGNAL_MBIAS_IGEN_PD_1,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 220 },
        { ADI_ADRV904X_GPIO_SIGNAL_MBIAS_COMP_OUT_ANA_0,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 221 },
        { ADI_ADRV904X_GPIO_SIGNAL_MBIAS_COMP_OUT_ANA_1,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 222 },
        { ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_MBIAS_IGEN_PTATR_TRIM_DONE_0,    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 223 },
        { ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_MBIAS_IGEN_PTATR_TRIM_DONE_1,    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 224 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_0,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 225 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_1,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 226 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_2,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 227 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_3,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 228 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_4,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 229 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_5,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 230 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_6,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 231 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_7,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 232 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_8,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 233 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_9,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 234 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_10,                              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 235 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_11,                              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 236 },
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_12,                              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 237 },        
        { ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_13,                              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 260 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_0,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 238 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_1,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 239 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_2,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 240 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_3,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 241 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_4,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 242 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_5,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 243 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_6,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 244 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_7,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 245 },
        { ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_8,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 246 },
        { ADI_ADRV904X_GPIO_SIGNAL_NEW_PHASE_TOGGLE,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 247 },
        { ADI_ADRV904X_GPIO_SIGNAL_JESD_MCS_CLK_IND,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 248 },
        { ADI_ADRV904X_GPIO_SIGNAL_SERDES_PLL_PLL_LOCKED,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 249 },
        { ADI_ADRV904X_GPIO_SIGNAL_SERDES_PLL_VCO_COMP_OUT,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 250 },
        { ADI_ADRV904X_GPIO_SIGNAL_POWER_ON_RESET_N,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL,       0x00FFFFFFU, 6U, 251 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_0,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 1 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_1,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 2 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_2,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_3,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_4,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 5 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_5,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 6 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_6,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 7 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_7,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 8 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_8,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 9 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_9,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 10 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_10,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 11 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_11,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 12 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_12,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 13 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_13,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 14 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_14,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 15 },
        { ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_15,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 16 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_0,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 17 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_1,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 18 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_2,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 19 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_3,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 20 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_4,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 21 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_5,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 22 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_6,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 23 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_7,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 24 },
        { ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_0,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 41 },
        { ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_1,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 42 },
        { ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_2,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 43 },
        { ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_3,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 44 },
        { ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_RX,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 45 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_UPDATE,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 46 },
        { ADI_ADRV904X_GPIO_SIGNAL_SLICER_OVERFLOW,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 47 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_SEC_HIGH,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 48 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_SEC_HIGH_COUNTER_EXCEEDED,      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 49 },
        { ADI_ADRV904X_GPIO_SIGNAL_TIA_VALID,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 50 },
        { ADI_ADRV904X_GPIO_SIGNAL_RXFE_VALID,                              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 51 },
        { ADI_ADRV904X_GPIO_SIGNAL_RSSI_DECPWR_READY,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 53 },
        { ADI_ADRV904X_GPIO_SIGNAL_PEAK_COUNT_EXCEEDED_QEC_ANA,             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 54 },
        { ADI_ADRV904X_GPIO_SIGNAL_PEAK_COUNT_EXCEEDED_QEC_DIG,             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 55 },
        { ADI_ADRV904X_GPIO_SIGNAL_RSSI_SYMBOL_READY,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 56 },
        { ADI_ADRV904X_GPIO_SIGNAL_RXON,                                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 57 },
        { ADI_ADRV904X_GPIO_SIGNAL_ADC_OVERLOAD_RESET_ADC,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 58 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_0,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 59 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_1,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 60 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_2,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 61 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_3,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 62 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_UPDATE_COUNTER_EXPIRED,             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 63 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_MEASURE,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 64 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_CAL_DONE,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 65 },
        { ADI_ADRV904X_GPIO_SIGNAL_RFDC_MEASURE_COUNTER_EXPIRED,            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 66 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_INT1_COUNTER_EXCEEDED,      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 68 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_INT0_COUNTER_EXCEEDED,      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 69 },
        { ADI_ADRV904X_GPIO_SIGNAL_DIG_GAIN_SAT,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 70 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_INT1_LOW,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 71 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_INT0_LOW,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 72 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_STATE_0,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 73 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_STATE_1,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 74 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_GAINUPDATE_COUNTER_EXPIRED,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 75 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_LOW_TH_EXCEEDED,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 76 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_HIGH_TH_EXCEEDED,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 77 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LLB_COUNTER_EXCEEDED,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 78 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_LOW,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 81 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_HIGH,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 82 },
        { ADI_ADRV904X_GPIO_SIGNAL_GT_GAIN_CHANGE,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 83 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_ULB_COUNTER_EXCEEDED,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 84 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_COUNTER_EXCEEDED,           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 85 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_HIGH_COUNTER_EXCEEDED,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 86 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_LOW_0,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 87 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_LOW_1,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 88 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_HIGH_0,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 89 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_HIGH_1,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 90 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_TOGGLE_0,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 91 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_TOGGLE_1,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 92 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_0,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 93 },
        { ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 94 },
        { ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_TX,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_SRERROR,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 2 },
        { ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_PPERROR_OR_APERROR,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_ATTEN_RAMP_UP_PROT,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_ATTEN_RAMP_DOWN_PROT,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 5 },
        { ADI_ADRV904X_GPIO_SIGNAL_ATTEN_TDD_RAMP_DOWN,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 6 },
        { ADI_ADRV904X_GPIO_SIGNAL_ATTEN_TDD_RAMP_UP,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 7 },
        { ADI_ADRV904X_GPIO_SIGNAL_ATTEN_LATCH_EN,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 8 },
        { ADI_ADRV904X_GPIO_SIGNAL_DTX_POWER_UP_TRIGGER,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 9 },
        { ADI_ADRV904X_GPIO_SIGNAL_DTX_POWER_DOWN_TRIGGER,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 10 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_GAIN_CHANGE,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 11 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_POWER_READY,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 12 },
        { ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_SRD_IRQ,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 13 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_TSSI_DUC0_READY,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 14 },
        { ADI_ADRV904X_GPIO_SIGNAL_ADC_SAMPLE_OVR,                          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U, 15 },
        { ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_ORX,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX,         0x00FFFFFFU, 9U, 1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ORX_ON,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX,         0x00FFFFFFU, 9U, 2 },
        { ADI_ADRV904X_GPIO_SIGNAL_DECPWR_READY,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX,         0x00FFFFFFU, 9U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANY_ADC_SAMPLE_OVR,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX,         0x00FFFFFFU, 9U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000001U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000002U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000004U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000008U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000010U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000020U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000040U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000080U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000100U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000200U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000400U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00000800U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00001000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00002000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00004000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00008000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00010000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00020000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00040000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00080000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00100000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00200000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00400000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG,   0x00800000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_0,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_1,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_2,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_3,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_4,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_5,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_6,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_7,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_8,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_9,          ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_10,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_11,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_12,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_13,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_14,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_15,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_16,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_17,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_18,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_19,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_20,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_21,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_22,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_23,         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_0,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_1,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_2,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_3,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_4,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_5,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_6,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_7,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_8,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_9,           ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_10,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_11,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_12,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_13,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_14,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_15,          ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PEB,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000008U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_CLK,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000010U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_0,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000020U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_1,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000040U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_2,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000080U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_3,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000100U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_4,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000200U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_5,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000400U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_6,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00000800U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_7,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00001000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_8,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00002000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_9,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00004000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_10,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00008000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_11,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00010000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_12,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00020000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_13,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00040000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_14,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00080000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_15,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_PPI,           0x00100000U, 0U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ADC_TEST_GEN_ENABLE,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 0 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,                         ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 1 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_DEC_GAIN,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 2 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_INC_GAIN,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,              ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_AGC_MANUAL_GAIN_LOCK,                    ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX,            0x00FFFFFEU, 0U, 5 },
        { ADI_ADRV904X_GPIO_SIGNAL_SELECT_S1,                               ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_TX,            0x00FFFFFEU, 0U, 0 },
        { ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,                           ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_TX_DTX,        0x00FFFFFEU, 0U, 3 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_ATTEN_UPD_GPIO,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_TX,            0x00FFFFFEU, 0U, 4 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_0,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_1,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_2,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_3,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_4,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_5,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_6,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_7,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_8,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_9,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_10,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_11,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_12,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_13,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_14,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_15,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_0,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_1,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_2,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_3,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_4,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_5,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_6,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_7,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_8,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_9,              ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_10,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_11,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_12,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_13,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_14,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_15,             ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_0,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_1,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_2,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_3,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_4,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_5,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_6,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_7,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_8,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_9,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_10,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_11,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_12,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_13,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_14,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_15,                    ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_0,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_1,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_2,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_3,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_4,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_5,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_6,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_7,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_8,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_9,                      ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_10,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_11,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_12,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_13,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_14,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_15,                     ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_0,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_1,                       ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 5U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 6U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_0,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_1,            ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 7U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_SSB_SYNC,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RADIO_SEQ,     0x00FFFFFFU, 5U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_0,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_1,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_2,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_3,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_4,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_5,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_6,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_7,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_8,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_9,                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_10,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_11,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_12,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_13,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_14,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_15,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_16,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_17,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_18,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_19,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_20,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_21,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_22,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_23,                ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 13U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_0,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_1,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_2,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_3,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_4,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_5,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_6,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_7,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_8,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_9,                  ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_10,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_11,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_12,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_13,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_14,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_15,                 ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 8U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_DEST_RX_CDDC_RSSI,  0x00FFFFFEU, 0U,  -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_0,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U,  95 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_1,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U,  96 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_2,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U,  97 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_3,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U,  98 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_4,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U,  99 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_5,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 100 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_6,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 101 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_7,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX,          0x00FFFFFFU, 7U, 102 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_TSSI_DUC1_READY,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  16 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_0,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  17 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_1,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  18 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_2,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  19 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_3,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  20 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_4,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  21 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_5,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  22 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_6,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  23 },
        { ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_7,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  24 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_DPD_IN_POWER_READY,                   ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  25 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_DPD_OUT_POWER_READY,                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  26 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_DFE_TSSI_READY_1,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  43 },
        { ADI_ADRV904X_GPIO_SIGNAL_TX_DFE_TSSI_READY_2,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX,          0x00FFFFFFU, 8U,  44 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRXSIN,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 12U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADCTS,                        ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 12U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRTSOUT,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 12U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADTXSOUT,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 12U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TRST,                            ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 11U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDO_SWO_MUX,                     ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 11U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDI,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 11U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TMS,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 11U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TCK,                             ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 11U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_0,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_1,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_2,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_3,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_4,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_5,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_6,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_7,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_8,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_9,                                  ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_10,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_11,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_12,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_13,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_14,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_15,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_16,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_17,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_18,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_19,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_20,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_21,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_22,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_NOOP_23,                                 ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 15U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_0,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000001U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_1,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000002U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_2,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000004U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_3,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000008U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_4,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000010U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_5,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000020U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_6,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000040U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_7,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000080U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_8,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000100U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_9,                       ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000200U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_10,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000400U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_11,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00000800U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_12,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00001000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_13,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00002000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_14,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00004000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_15,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00008000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_16,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00010000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_17,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00020000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_18,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00040000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_19,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00080000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_20,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00100000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_21,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00200000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_22,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00400000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_23,                      ADRV904X_GPIO_DOMAIN_DIGITAL,    ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE,        0x00800000U, 4U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_0,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000001U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_1,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000002U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_2,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000004U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_3,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000008U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_4,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000010U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_5,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000020U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_6,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000040U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_7,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000080U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_8,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000100U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_9,                ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000200U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_10,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000400U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_11,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00000800U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_12,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00001000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_13,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00002000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_14,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00004000U, 3U, -1 },
        { ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_15,               ADRV904X_GPIO_DOMAIN_ANALOG,     ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE,        0x00008000U, 3U, -1 },
    };
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t numEntries = sizeof(gpioSignalLut) / sizeof(adrv904x_GpioSignalInfo_t);
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, info);
    
    /* Range Check that gpio signal is valid */
    if ((signal < ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS) ||
        (signal < 0) ||
        (signal >= numEntries))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid gpio signal encountered in adrv904x_SignalInfoGet function. Out of range.");
        return recoveryAction;
    }
    /* Check that LUT entry for index 'signal' has a signal field that matches exactly */
    else if ( gpioSignalLut[signal].signal != signal )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 signal,
                                 "Invalid gpio signal encountered in adrv904x_SignalInfoGet function. Lookup failure.");
        return recoveryAction;
    }
    /* Success. Get Signal Info */
    else
    {
        *info = gpioSignalLut[signal];
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_FromSignalToFeatureGet(adi_adrv904x_Device_t* const    device,
                                                                 const adi_adrv904x_GpioSignal_e signal,
                                                                 adrv904x_FeatureID_e* const     featureID)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_FeatureID_e tmpFeature = ADRV904X_NUM_FEATURES;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, featureID);
    
    /* Check that signal is valid */
    if (( signal <  ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        ( signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            signal,
            "Invalid signal encountered in adrv904x_FromSignalToFeatureGet function");
        return recoveryAction;
    }
    
    /* Range Check featureID when cast directly from signal. */
    /* Note: Though all signals have a 1-to-1 map to features, the converse is not true */
    tmpFeature = (adrv904x_FeatureID_e)((uint32_t) signal | GPIO_SIGNAL_FEATURE);
    if (( tmpFeature <  ADRV904X_FEATURE_UNUSED) ||
        ( tmpFeature >= ADRV904X_NUM_FEATURES))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            tmpFeature,
            "Invalid featureID encountered in adrv904x_FromSignalToFeatureGet function");
        return recoveryAction;
    }
    /* Success. Set featureID */
    else
    {
        *featureID = tmpFeature;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_FromFeatureToSignalGet(adi_adrv904x_Device_t* const        device,
                                                                 const adrv904x_FeatureID_e          featureID,
                                                                 adi_adrv904x_GpioSignal_e* const    signal)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e tmpSig = ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, signal);
    
    /* Check that featureID is valid */
    if (( featureID <  ADRV904X_FEATURE_UNUSED) ||
        ( featureID >= ADRV904X_NUM_FEATURES))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            featureID,
            "Invalid featureID encountered in adrv904x_FromFeatureToSignalGet function");
        return recoveryAction;
    }
    
    /* Range Check signal when cast directly from featureID. */
    /* Note: Though all signals have a 1-to-1 map to features, the converse is not true */
    tmpSig = (adi_adrv904x_GpioSignal_e)((uint32_t)featureID & ~GPIO_SIGNAL_FEATURE);
    if (( tmpSig <  ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        ( tmpSig >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            tmpSig,
            "Invalid signal encountered in adrv904x_FromFeatureToSignalGet function");
        return recoveryAction;
    }
    /* Success. Set signal */
    else
    {
        *signal = tmpSig;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg3Set(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            const uint8_t                       muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t) = {
        adrv904x_Core_GpioSourceControl0_BfSet,
        adrv904x_Core_GpioSourceControl1_BfSet,
        adrv904x_Core_GpioSourceControl2_BfSet,
        adrv904x_Core_GpioSourceControl3_BfSet,
        adrv904x_Core_GpioSourceControl4_BfSet,
        adrv904x_Core_GpioSourceControl5_BfSet,
        adrv904x_Core_GpioSourceControl6_BfSet,
        adrv904x_Core_GpioSourceControl7_BfSet,
        adrv904x_Core_GpioSourceControl8_BfSet,
        adrv904x_Core_GpioSourceControl9_BfSet,
        adrv904x_Core_GpioSourceControl10_BfSet,
        adrv904x_Core_GpioSourceControl11_BfSet,
        adrv904x_Core_GpioSourceControl12_BfSet,
        adrv904x_Core_GpioSourceControl13_BfSet,
        adrv904x_Core_GpioSourceControl14_BfSet,
        adrv904x_Core_GpioSourceControl15_BfSet,
        adrv904x_Core_GpioSourceControl16_BfSet,
        adrv904x_Core_GpioSourceControl17_BfSet,
        adrv904x_Core_GpioSourceControl18_BfSet,
        adrv904x_Core_GpioSourceControl19_BfSet,
        adrv904x_Core_GpioSourceControl20_BfSet,
        adrv904x_Core_GpioSourceControl21_BfSet,
        adrv904x_Core_GpioSourceControl22_BfSet,
        adrv904x_Core_GpioSourceControl23_BfSet
        };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if ( gpio >= ADI_ADRV904X_GPIO_INVALID )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE3_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg3 selection failure");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg3Get(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            uint8_t  * const                    muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfGetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t *) = {
        adrv904x_Core_GpioSourceControl0_BfGet,
        adrv904x_Core_GpioSourceControl1_BfGet,
        adrv904x_Core_GpioSourceControl2_BfGet,
        adrv904x_Core_GpioSourceControl3_BfGet,
        adrv904x_Core_GpioSourceControl4_BfGet,
        adrv904x_Core_GpioSourceControl5_BfGet,
        adrv904x_Core_GpioSourceControl6_BfGet,
        adrv904x_Core_GpioSourceControl7_BfGet,
        adrv904x_Core_GpioSourceControl8_BfGet,
        adrv904x_Core_GpioSourceControl9_BfGet,
        adrv904x_Core_GpioSourceControl10_BfGet,
        adrv904x_Core_GpioSourceControl11_BfGet,
        adrv904x_Core_GpioSourceControl12_BfGet,
        adrv904x_Core_GpioSourceControl13_BfGet,
        adrv904x_Core_GpioSourceControl14_BfGet,
        adrv904x_Core_GpioSourceControl15_BfGet,
        adrv904x_Core_GpioSourceControl16_BfGet,
        adrv904x_Core_GpioSourceControl17_BfGet,
        adrv904x_Core_GpioSourceControl18_BfGet,
        adrv904x_Core_GpioSourceControl19_BfGet,
        adrv904x_Core_GpioSourceControl20_BfGet,
        adrv904x_Core_GpioSourceControl21_BfGet,
        adrv904x_Core_GpioSourceControl22_BfGet,
        adrv904x_Core_GpioSourceControl23_BfGet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, muxSelect);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gpio,
                               "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfGetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Reading Pinmux Stg3 failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2RxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t) = {
        adrv904x_Core_GpioSourceRxControl0_BfSet,
        adrv904x_Core_GpioSourceRxControl1_BfSet,
        adrv904x_Core_GpioSourceRxControl2_BfSet,
        adrv904x_Core_GpioSourceRxControl3_BfSet,
        adrv904x_Core_GpioSourceRxControl4_BfSet,
        adrv904x_Core_GpioSourceRxControl5_BfSet,
        adrv904x_Core_GpioSourceRxControl6_BfSet,
        adrv904x_Core_GpioSourceRxControl7_BfSet,
        adrv904x_Core_GpioSourceRxControl8_BfSet,
        adrv904x_Core_GpioSourceRxControl9_BfSet,
        adrv904x_Core_GpioSourceRxControl10_BfSet,
        adrv904x_Core_GpioSourceRxControl11_BfSet,
        adrv904x_Core_GpioSourceRxControl12_BfSet,
        adrv904x_Core_GpioSourceRxControl13_BfSet,
        adrv904x_Core_GpioSourceRxControl14_BfSet,
        adrv904x_Core_GpioSourceRxControl15_BfSet,
        adrv904x_Core_GpioSourceRxControl16_BfSet,
        adrv904x_Core_GpioSourceRxControl17_BfSet,
        adrv904x_Core_GpioSourceRxControl18_BfSet,
        adrv904x_Core_GpioSourceRxControl19_BfSet,
        adrv904x_Core_GpioSourceRxControl20_BfSet,
        adrv904x_Core_GpioSourceRxControl21_BfSet,
        adrv904x_Core_GpioSourceRxControl22_BfSet,
        adrv904x_Core_GpioSourceRxControl23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE2_RX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg2 Rx selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2TxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t) = {
        adrv904x_Core_GpioSourceTxControl0_BfSet,
        adrv904x_Core_GpioSourceTxControl1_BfSet,
        adrv904x_Core_GpioSourceTxControl2_BfSet,
        adrv904x_Core_GpioSourceTxControl3_BfSet,
        adrv904x_Core_GpioSourceTxControl4_BfSet,
        adrv904x_Core_GpioSourceTxControl5_BfSet,
        adrv904x_Core_GpioSourceTxControl6_BfSet,
        adrv904x_Core_GpioSourceTxControl7_BfSet,
        adrv904x_Core_GpioSourceTxControl8_BfSet,
        adrv904x_Core_GpioSourceTxControl9_BfSet,
        adrv904x_Core_GpioSourceTxControl10_BfSet,
        adrv904x_Core_GpioSourceTxControl11_BfSet,
        adrv904x_Core_GpioSourceTxControl12_BfSet,
        adrv904x_Core_GpioSourceTxControl13_BfSet,
        adrv904x_Core_GpioSourceTxControl14_BfSet,
        adrv904x_Core_GpioSourceTxControl15_BfSet,
        adrv904x_Core_GpioSourceTxControl16_BfSet,
        adrv904x_Core_GpioSourceTxControl17_BfSet,
        adrv904x_Core_GpioSourceTxControl18_BfSet,
        adrv904x_Core_GpioSourceTxControl19_BfSet,
        adrv904x_Core_GpioSourceTxControl20_BfSet,
        adrv904x_Core_GpioSourceTxControl21_BfSet,
        adrv904x_Core_GpioSourceTxControl22_BfSet,
        adrv904x_Core_GpioSourceTxControl23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE2_TX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg2 Tx selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2OrxSet(adi_adrv904x_Device_t* const        device,
                                                               const adi_adrv904x_GpioPinSel_e     gpio,
                                                               const uint8_t                       muxSelect)
{   
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t) = {
        adrv904x_Core_GpioSourceOrxControl0_BfSet,
        adrv904x_Core_GpioSourceOrxControl1_BfSet,
        adrv904x_Core_GpioSourceOrxControl2_BfSet,
        adrv904x_Core_GpioSourceOrxControl3_BfSet,
        adrv904x_Core_GpioSourceOrxControl4_BfSet,
        adrv904x_Core_GpioSourceOrxControl5_BfSet,
        adrv904x_Core_GpioSourceOrxControl6_BfSet,
        adrv904x_Core_GpioSourceOrxControl7_BfSet,
        adrv904x_Core_GpioSourceOrxControl8_BfSet,
        adrv904x_Core_GpioSourceOrxControl9_BfSet,
        adrv904x_Core_GpioSourceOrxControl10_BfSet,
        adrv904x_Core_GpioSourceOrxControl11_BfSet,
        adrv904x_Core_GpioSourceOrxControl12_BfSet,
        adrv904x_Core_GpioSourceOrxControl13_BfSet,
        adrv904x_Core_GpioSourceOrxControl14_BfSet,
        adrv904x_Core_GpioSourceOrxControl15_BfSet,
        adrv904x_Core_GpioSourceOrxControl16_BfSet,
        adrv904x_Core_GpioSourceOrxControl17_BfSet,
        adrv904x_Core_GpioSourceOrxControl18_BfSet,
        adrv904x_Core_GpioSourceOrxControl19_BfSet,
        adrv904x_Core_GpioSourceOrxControl20_BfSet,
        adrv904x_Core_GpioSourceOrxControl21_BfSet,
        adrv904x_Core_GpioSourceOrxControl22_BfSet,
        adrv904x_Core_GpioSourceOrxControl23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE2_ORX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg2 ORx selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg2ActrlSet(adi_adrv904x_Device_t* const        device,
                                                                 const adi_adrv904x_GpioPinSel_e     gpio,
                                                                 const uint16_t                      muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint16_t) = {
        adrv904x_Core_DataFromControlOutSel0_BfSet,
        adrv904x_Core_DataFromControlOutSel1_BfSet,
        adrv904x_Core_DataFromControlOutSel2_BfSet,
        adrv904x_Core_DataFromControlOutSel3_BfSet,
        adrv904x_Core_DataFromControlOutSel4_BfSet,
        adrv904x_Core_DataFromControlOutSel5_BfSet,
        adrv904x_Core_DataFromControlOutSel6_BfSet,
        adrv904x_Core_DataFromControlOutSel7_BfSet,
        adrv904x_Core_DataFromControlOutSel8_BfSet,
        adrv904x_Core_DataFromControlOutSel9_BfSet,
        adrv904x_Core_DataFromControlOutSel10_BfSet,
        adrv904x_Core_DataFromControlOutSel11_BfSet,
        adrv904x_Core_DataFromControlOutSel12_BfSet,
        adrv904x_Core_DataFromControlOutSel13_BfSet,
        adrv904x_Core_DataFromControlOutSel14_BfSet,
        adrv904x_Core_DataFromControlOutSel15_BfSet,
        adrv904x_Core_DataFromControlOutSel16_BfSet,
        adrv904x_Core_DataFromControlOutSel17_BfSet,
        adrv904x_Core_DataFromControlOutSel18_BfSet,
        adrv904x_Core_DataFromControlOutSel19_BfSet,
        adrv904x_Core_DataFromControlOutSel20_BfSet,
        adrv904x_Core_DataFromControlOutSel21_BfSet,
        adrv904x_Core_DataFromControlOutSel22_BfSet,
        adrv904x_Core_DataFromControlOutSel23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE2_ACTRL_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device,
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg2 Actrl selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1RxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect,
                                                              const uint8_t                       channelIdx)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_RX_0__RX_DIG;
    uint8_t gpioIdx = 0U;
    uint32_t chanSel = 0U;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE1_RX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 muxSelect,
                                 "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get baseAddress for this channel */
    chanSel = 1U << channelIdx;
    recoveryAction = adrv904x_RxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxBitfieldAddressGet issue");
        return recoveryAction;
    }
    
    /* Set mux bitfield */
    gpioIdx = (uint8_t)(gpio);
    recoveryAction = adrv904x_RxDig_RxGpioSourceSelection_BfSet(device, NULL, baseAddr, gpioIdx, muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg1 Rx selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1TxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioPinSel_e     gpio,
                                                              const uint8_t                       muxSelect,
                                                              const uint8_t                       channelIdx)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_TX_0__TX_DIG;
    uint32_t gpioIdx = 0U;
    uint32_t chanSel = 0u;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfTxDigChanAddr_e, uint8_t) = {
        adrv904x_TxDig_TxGpioSourceSelection0_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection1_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection2_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection3_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection4_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection5_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection6_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection7_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection8_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection9_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection10_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection11_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection12_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection13_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection14_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection15_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection16_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection17_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection18_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection19_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection20_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection21_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection22_BfSet,
        adrv904x_TxDig_TxGpioSourceSelection23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE1_TX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 muxSelect,
                                 "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get baseAddress for this channel */
    chanSel = 1U << channelIdx;
    recoveryAction = adrv904x_TxBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxBitfieldAddressGet issue");
        return recoveryAction;
    }
    
    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx]( device, 
                                        NULL,
                                        baseAddr,
                                        muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg1 Tx selection failure");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioPinmuxStg1OrxSet(adi_adrv904x_Device_t* const        device,
                                                               const adi_adrv904x_GpioPinSel_e     gpio,
                                                               const uint8_t                       muxSelect,
                                                               const uint8_t                       channelIdx)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfOrxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_ORX_0__ORX_DIG;
    uint32_t gpioIdx = 0U;
    uint32_t chanSel = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfOrxDigChanAddr_e, uint8_t) = {
        adrv904x_OrxDig_OrxGpioSourceSelection0_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection1_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection2_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection3_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection4_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection5_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection6_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection7_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection8_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection9_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection10_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection11_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection12_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection13_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection14_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection15_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection16_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection17_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection18_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection19_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection20_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection21_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection22_BfSet,
        adrv904x_OrxDig_OrxGpioSourceSelection23_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_PINMUX_STAGE1_ORX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 muxSelect,
                                 "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }
    
    /* Get baseAddress for this channel */
    chanSel = 0x100U << channelIdx;
    recoveryAction = adrv904x_OrxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrBitfieldAddressGet issue");
        return recoveryAction;
    }
    
    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx]( device, 
                                        NULL,
                                        baseAddr,
                                        muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Pinmux Stg1 ORx selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioIeOverride(adi_adrv904x_Device_t* const        device,
                                                         const adi_adrv904x_GpioPinSel_e     gpio,
                                                         const uint8_t                       override)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCoreChanAddr_e baseAddr = (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR;
    uint32_t lclMask = 0U;
    uint32_t gpioIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check IE override bit is valid */
    if ((override != ADI_ENABLE) &&
        (override != ADI_DISABLE) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            override,
            "Invalid Input Enable override bit for a GPIO. Must be 0 or 1.");
        return recoveryAction;
    }
    
    /* Get current bitfield value */
    recoveryAction = adrv904x_Core_GpioSourceControlOverride_BfGet(  device, 
                                                                     NULL,
                                                                     baseAddr,
                                                                    &lclMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio source control override bf");
        return recoveryAction;
    }
                                                                                               
    /* Update mask bit for this gpio */
    gpioIdx = (uint32_t)(gpio);
    lclMask |= (override << gpioIdx );
    
    /* Update bitfield value */
    recoveryAction = adrv904x_Core_GpioSourceControlOverride_BfSet( device, 
                                                                    NULL,
                                                                    baseAddr,
                                                                    lclMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gpio source control override bf");
        return recoveryAction;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestRxSet(adi_adrv904x_Device_t* const        device,
                                                        const uint8_t                       destIdx,
                                                        const uint8_t                       channelIdx,
                                                        const uint8_t                       muxSelect)
{
    const uint8_t NUM_CHANS = 8U;
    const uint8_t NUM_MUX_SELECTIONS = 24U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfRxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_RX_0__RX_DIG;
    uint32_t chanSel = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[ADRV904X_GPIO_NUM_DESTINATIONS_RX])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfRxDigChanAddr_e, uint8_t) = {
        adrv904x_RxDig_TestGenEnGpioSelect_BfSet,
        adrv904x_RxDig_AgcGainChangeGpioSelect_BfSet,
        adrv904x_RxDig_AgcDecGainGpioSelect_BfSet,
        adrv904x_RxDig_AgcIncGainGpioSelect_BfSet,
        adrv904x_RxDig_AgcSlowloopFreezeEnableGpioSelect_BfSet,
        adrv904x_RxDig_AgcManualGainLockGpioSelect_BfSet,
        adrv904x_RxDig_RssiEnableGpioSelect_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check inputs */
    if (destIdx >= ADRV904X_GPIO_NUM_DESTINATIONS_RX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 destIdx,
                                 "Invalid destination index specified for setting Rx channel destination");
        return recoveryAction;
    }
    
    if (channelIdx >= NUM_CHANS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelIdx,
                                 "Invalid channel index specified for setting Rx channel destination");
        return recoveryAction;
    }
    
    if (muxSelect >= NUM_MUX_SELECTIONS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 muxSelect,
                                 "Invalid mux selection specified for setting Rx channel destination");
        return recoveryAction;
    }
    
    /* Get baseAddress for this channel */
    chanSel = 1U << channelIdx;
    recoveryAction = adrv904x_RxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(chanSel), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxBitfieldAddressGet issue");
        return recoveryAction;
    }
    
    /* Set bitfield */
    recoveryAction = bfSetter[destIdx]( device, 
                                        NULL,
                                        baseAddr,
                                        muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting Rx Destination mux");
        return recoveryAction;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestTxSet(adi_adrv904x_Device_t* const        device,
                                                        const uint8_t                       destIdx,
                                                        const uint8_t                       channelIdx,
                                                        const uint8_t                       muxSelect)
{
    const uint8_t NUM_CHANS = 8U;
    const uint8_t NUM_MUX_SELECTIONS = 24U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_TX_0__TX_DIG;
    uint32_t chanSel = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[ADRV904X_GPIO_NUM_DESTINATIONS_TX])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfTxDigChanAddr_e, uint8_t) = {
        adrv904x_TxDig_Spi2TxAttenGpioSelect_BfSet,
        adrv904x_TxDig_TpcIncrAttenGpioSelect_BfSet,
        adrv904x_TxDig_TpcDecrAttenGpioSelect_BfSet,
        adrv904x_TxDig_DtxForceGpioSelect_BfSet,
        adrv904x_TxDig_TxAttenUpdGpioSelect_BfSet,
        adrv904x_TxDig_TxDuc0TssiPinEnableSelect_BfSet
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check inputs */
    if (destIdx >= ADRV904X_GPIO_NUM_DESTINATIONS_TX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 destIdx,
                                 "Invalid destination index specified for setting Tx channel destination");
        return recoveryAction;
    }
    
    if (channelIdx >= NUM_CHANS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 channelIdx,
                                 "Invalid channel index specified for setting Tx channel destination");
        return recoveryAction;
    }
    
    if (muxSelect >= NUM_MUX_SELECTIONS)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 muxSelect,
                                 "Invalid mux selection specified for setting Tx channel destination");
        return recoveryAction;
    }
    
    /* Get baseAddress for this channel */
    chanSel = 1U << channelIdx;
    recoveryAction = adrv904x_TxBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxBitfieldAddressGet issue");
        return recoveryAction;
    }
    
    /* Set bitfield */
    recoveryAction = bfSetter[destIdx]( device, 
                                        NULL,
                                        baseAddr,
                                        muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting Tx Destination mux");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioDestStreamTrigSet(adi_adrv904x_Device_t* const       device,
                                                                const adi_adrv904x_GpioPinSel_e    gpio,
                                                                const uint8_t                      maskTrigger)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    uint32_t tmpMask = 0U;
    uint32_t streamPinmask = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if ( gpio >= ADI_ADRV904X_GPIO_INVALID )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check maskTrigger */
    if ((maskTrigger != ADI_ENABLE) &&
        (maskTrigger != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               maskTrigger,
                               "Invalid GPIO Stream maskTrigger Option. Must be 0 or 1.");
        return recoveryAction;
    }

    /* Get current stream gpio pin mask value */
    recoveryAction = adrv904x_Core_StreamProcGpioPinMask_BfGet(device,
                                                               NULL,
                                                               (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                               &streamPinmask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GPIO Stream Trigger mask set failure");
        return recoveryAction;
    }

    /* Update mask bit for selected gpio */
    gpioIdx = (uint32_t)(gpio);
    tmpMask = (1U << gpioIdx);
    /* if maskTrigger, bit needs to be set and stream WILL NOT be triggered*/
    if (maskTrigger == ADI_ENABLE)
    {
        streamPinmask |= tmpMask;
    }
    /* else, bit needs to be cleared and stream WILL be triggered */
    else
    {
        streamPinmask &= ~tmpMask;
    }
    
    /* Set bitfield */
    recoveryAction = adrv904x_Core_StreamProcGpioPinMask_BfSet(device,
                                                               NULL,
                                                               (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                               streamPinmask);
    if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GPIO Stream Trigger mask set failure");
        return recoveryAction;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioRouteValidChannelMaskCheck(adi_adrv904x_Device_t* const        device,
                                                                         const adrv904x_GpioRoute_e          route,
                                                                         const uint32_t                      channelMask,
                                                                         uint8_t* const                      isValid)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, isValid);
    
    switch(route)
        {
            /* NO channel can be selected for these route types */
            case ADRV904X_GPIO_ROUTE_OFF:
            case ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE:
            case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_PPI:
            case ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_RADIO_SEQ:
                *isValid = (channelMask == 0U) ? ADI_TRUE : ADI_FALSE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
                
            /* Exactly 1 channel of 8 must be selected for these route types */
            case ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX:
            case ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX:
                *isValid = ((channelMask == 0x01) ||
                            (channelMask == 0x02) ||
                            (channelMask == 0x04) ||
                            (channelMask == 0x08) ||
                            (channelMask == 0x10) ||
                            (channelMask == 0x20) ||
                            (channelMask == 0x40) ||
                            (channelMask == 0x80) ) 
                            ? ADI_TRUE
                            : ADI_FALSE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
                
            /* Only 1 channel of 2 can be selected for these route types */
            case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX:
                *isValid = ((channelMask == 0x01) ||
                            (channelMask == 0x02) ) 
                            ? ADI_TRUE
                            : ADI_FALSE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            
            /* At least 1 and at most 8 channels can be selected for these route types */
            case ADRV904X_GPIO_ROUTE_DIG_DEST_RX:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_TX:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_RX_CDDC_RSSI:
            case ADRV904X_GPIO_ROUTE_DIG_DEST_TX_DTX:
                *isValid = ((channelMask >= 0x01) &&
                            (channelMask <= 0xff) ) 
                            ? ADI_TRUE
                            : ADI_FALSE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
                break;
            
            /* These route types, or any other, are not currently supported. */
            case ADRV904X_GPIO_ROUTE_ANA_DEST_CORE:
            case ADRV904X_GPIO_ROUTE_ANA_DEST_RX:
            case ADRV904X_GPIO_ROUTE_ANA_DEST_TX:
            case ADRV904X_GPIO_ROUTE_ANA_DEST_ORX:
            default:
                *isValid = ADI_FALSE;
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                         recoveryAction,
                                         route,
                                         "Unsupported route for checking channelMask");
                return recoveryAction;
                break;
        }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioConnect(adi_adrv904x_Device_t* const            device,
                                                      const uint8_t                           connect,
                                                      const adi_adrv904x_GpioPinSel_e         gpio,
                                                      const adrv904x_GpioSignalInfo_t* const  sigInfo,
                                                      uint32_t const                          channelIdx)
{
    static const uint8_t DEFAULT_STG3_SELECT       = 0U;
    static const uint8_t DEFAULT_STG2_SELECT       = 0U;
    static const uint8_t DEFAULT_STG1_SELECT       = 0U;
    static const uint8_t DEFAULT_DESTRX_SELECT     = 0U;
    static const uint8_t DEFAULT_DESTTX_SELECT     = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check if connect is valid */
    if ((connect != ADI_ENABLE) &&
        (connect != ADI_DISABLE) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            connect,
            "Invalid GPIO Connect/Disconnet Option. Must be 0 or 1.");
        return recoveryAction;
    }
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Setup the Route for this GPIO/signal (and channel, if this signal is a channel-instantiate signal) */
    switch(sigInfo->route)
    {
        case ADRV904X_GPIO_ROUTE_DIG_PINMUX_CORE:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, 
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            break;
        
        case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ACTRL:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device,
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg2ActrlSet( device,
                                                              gpio,
                                                              (connect ? sigInfo->targetSelect : DEFAULT_STG2_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg2ActrlSet issue");
                return recoveryAction;
            }
            break;
        
        case ADRV904X_GPIO_ROUTE_DIG_PINMUX_RX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device,
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg2RxSet( device, 
                                                           gpio,
                                                           (connect ? channelIdx : DEFAULT_STG2_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg2RxSet issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg1RxSet( device,
                                                           gpio,
                                                           (connect ? sigInfo->targetSelect : DEFAULT_STG1_SELECT),
                                                           channelIdx );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg1RxSet issue");
                return recoveryAction;
            }
        
            break;
        
        case ADRV904X_GPIO_ROUTE_DIG_PINMUX_TX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device,
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg2TxSet( device, 
                                                           gpio,
                                                           (connect ? channelIdx : DEFAULT_STG2_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg2TxSet issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg1TxSet( device,
                                                           gpio, 
                                                           (connect ? sigInfo->targetSelect : DEFAULT_STG1_SELECT),
                                                           channelIdx );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg1TxSet issue");
                return recoveryAction;
            }
            break;
        
        case ADRV904X_GPIO_ROUTE_DIG_PINMUX_ORX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device,
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg2OrxSet( device,
                                                            gpio,
                                                            (connect ? channelIdx : DEFAULT_STG2_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg2OrxSet issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioPinmuxStg1OrxSet( device,
                                                            gpio, 
                                                            (connect ? sigInfo->targetSelect : DEFAULT_STG1_SELECT),
                                                            channelIdx );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg1OrxSet issue");
                return recoveryAction;
            }
            break;
        
        case ADRV904X_GPIO_ROUTE_DIG_DEST_RX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, gpio, DEFAULT_STG3_SELECT );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioDestRxSet( device,
                                                     sigInfo->targetSelect, 
                                                     channelIdx,
                                                     connect ? (uint8_t)gpio : DEFAULT_DESTRX_SELECT);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioDestRxSet issue");
                return recoveryAction;
            }
            
            break;
        case ADRV904X_GPIO_ROUTE_DIG_DEST_TX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, gpio, DEFAULT_STG3_SELECT );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioDestTxSet( device,
                                                     sigInfo->targetSelect,
                                                     channelIdx, 
                                                     connect ? (uint8_t)gpio : DEFAULT_DESTTX_SELECT);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioDestTxSet issue");
                return recoveryAction;
            }
            break;

        case ADRV904X_GPIO_ROUTE_DIG_DEST_STREAM_TRIG:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, 
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
        
        
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioDestStreamTrigSet( device,
                                                             gpio,
                                                             !connect);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adrv904x_GpioDestStreamTrigSet issue");
                return recoveryAction;
            }
            break;

        case ADRV904X_GPIO_ROUTE_DIG_DEST_RADIO_SEQ:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, 
                                                         gpio,
                                                         (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }

            recoveryAction = adrv904x_Sequencer_SsbSyncGpioSel_BfSet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS,
                                                                     (uint8_t)gpio);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting SSB SYNC gpio sel.");
                return recoveryAction;
            }
            break;

        case ADRV904X_GPIO_ROUTE_DIG_DEST_RX_CDDC_RSSI:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, 
                                                        gpio,
                                                        (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }

            {
                adrv904x_BfRxDigChanAddr_e baseAddr =  ADRV904X_BF_SLICE_RX_0__RX_DIG;
                /* Get baseAddress for this channel */
                recoveryAction = adrv904x_RxBitfieldAddressGet(device, (adi_adrv904x_RxChannels_e)(1U << channelIdx), &baseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxBitfieldAddressGet issue");
                    return recoveryAction;
                }
                recoveryAction =  adrv904x_RxDig_RssiEnableGpioSelect_BfSet(device,
                                                                            NULL,
                                                                            baseAddr,
                                                                            (uint8_t)gpio);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Rssi Enable gpio sel.");
                    return recoveryAction;
                }
            }
            break;

        case ADRV904X_GPIO_ROUTE_DIG_DEST_TX_DTX:
            recoveryAction = adrv904x_GpioPinmuxStg3Set( device, gpio, DEFAULT_STG3_SELECT );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioPinmuxStg3Set issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioDestTxSet( device,
                                                     sigInfo->targetSelect,
                                                     channelIdx, 
                                                     connect ? (uint8_t)gpio : DEFAULT_DESTTX_SELECT);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioDestTxSet issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_GpioDestStreamTrigSet( device,
                                                             gpio,
                                                             !connect);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adrv904x_GpioDestStreamTrigSet issue");
                return recoveryAction;
            }
            break;
        
        /* TODO: PPI Dest Core signals need some extra special handling. Add later when we start using them */
        case ADRV904X_GPIO_ROUTE_DIG_DEST_PPI:           /* Fallthrough */
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     sigInfo->route,
                                     "Unsupported route type for selected GPIO signal");
            return recoveryAction;
            break;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogIeOverride(adi_adrv904x_Device_t* const       device,
                                                               const adi_adrv904x_GpioAnaPinSel_e gpio,
                                                               const uint8_t                      override)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfCoreChanAddr_e baseAddr = (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR;
    uint16_t lclMask = 0U;
    uint32_t gpioIdx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check analog gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID ||
    	gpio < ADI_ADRV904X_GPIO_ANA_00)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            gpio,
            "Invalid analog GPIO selected. Out of range.");
        return recoveryAction;
    }

    /* Check IE override bit is valid */
    if ((override != ADI_ENABLE) &&
        (override != ADI_DISABLE) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            override,
            "Invalid Input Enable override bit for an analog GPIO. Must be 0 or 1.");
        return recoveryAction;
    }

    /* Get current bitfield value */
    recoveryAction = adrv904x_Core_GpioAnalogSourceControlOverride_BfGet(  	device,
                                                                     	 	NULL,
																			baseAddr,
																			&lclMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read analog gpio source control override bf");
        return recoveryAction;
    }

    /* Update mask bit for this gpio */
    gpioIdx = (uint32_t)(gpio);
    lclMask |= (override << gpioIdx );

    /* Update bitfield value */
    recoveryAction = adrv904x_Core_GpioAnalogSourceControlOverride_BfSet( 	device,
                                                                    		NULL,
																			baseAddr,
																			lclMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write analog gpio source control override bf");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogPinmuxSet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              const uint8_t                       muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfSetter[(uint32_t)(ADI_ADRV904X_GPIO_ANA_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t) = {
        adrv904x_Core_GpioAnalogSourceControl0_BfSet,
        adrv904x_Core_GpioAnalogSourceControl1_BfSet,
        adrv904x_Core_GpioAnalogSourceControl2_BfSet,
        adrv904x_Core_GpioAnalogSourceControl3_BfSet,
        adrv904x_Core_GpioAnalogSourceControl4_BfSet,
        adrv904x_Core_GpioAnalogSourceControl5_BfSet,
        adrv904x_Core_GpioAnalogSourceControl6_BfSet,
        adrv904x_Core_GpioAnalogSourceControl7_BfSet,
        adrv904x_Core_GpioAnalogSourceControl8_BfSet,
        adrv904x_Core_GpioAnalogSourceControl9_BfSet,
        adrv904x_Core_GpioAnalogSourceControl10_BfSet,
        adrv904x_Core_GpioAnalogSourceControl11_BfSet,
        adrv904x_Core_GpioAnalogSourceControl12_BfSet,
        adrv904x_Core_GpioAnalogSourceControl13_BfSet,
        adrv904x_Core_GpioAnalogSourceControl14_BfSet,
        adrv904x_Core_GpioAnalogSourceControl15_BfSet
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check gpio is valid */
    if ( gpio >= ADI_ADRV904X_GPIO_ANA_INVALID )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Check muxSelect */
    if (muxSelect > ADRV904X_GPIO_ANALOG_PINMUX_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               muxSelect,
                               "Invalid mux input selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfSetter[gpioIdx](device, 
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Analog Pinmux selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogPinmuxGet(adi_adrv904x_Device_t* const        device,
                                                              const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                              uint8_t  * const                    muxSelect)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_ErrAction_e(*bfGetter[(uint32_t)(ADI_ADRV904X_GPIO_ANA_INVALID)])(adi_adrv904x_Device_t*, adi_adrv904x_SpiCache_t*, adrv904x_BfCoreChanAddr_e, uint8_t *) = {
        adrv904x_Core_GpioAnalogSourceControl0_BfGet,
        adrv904x_Core_GpioAnalogSourceControl1_BfGet,
        adrv904x_Core_GpioAnalogSourceControl2_BfGet,
        adrv904x_Core_GpioAnalogSourceControl3_BfGet,
        adrv904x_Core_GpioAnalogSourceControl4_BfGet,
        adrv904x_Core_GpioAnalogSourceControl5_BfGet,
        adrv904x_Core_GpioAnalogSourceControl6_BfGet,
        adrv904x_Core_GpioAnalogSourceControl7_BfGet,
        adrv904x_Core_GpioAnalogSourceControl8_BfGet,
        adrv904x_Core_GpioAnalogSourceControl9_BfGet,
        adrv904x_Core_GpioAnalogSourceControl10_BfGet,
        adrv904x_Core_GpioAnalogSourceControl11_BfGet,
        adrv904x_Core_GpioAnalogSourceControl12_BfGet,
        adrv904x_Core_GpioAnalogSourceControl13_BfGet,
        adrv904x_Core_GpioAnalogSourceControl14_BfGet,
        adrv904x_Core_GpioAnalogSourceControl15_BfGet
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, muxSelect);
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gpio,
                               "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }

    /* Set mux bitfield */
    gpioIdx = (uint32_t)(gpio);
    recoveryAction = bfGetter[gpioIdx](device, 
                                       NULL,
                                       (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                       muxSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Reading Analog Pinmux selection failure");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpioAnalogConnect(adi_adrv904x_Device_t* const            device,
                                                            const uint8_t                           connect,
                                                            const adi_adrv904x_GpioAnaPinSel_e      gpio,
                                                            const adrv904x_GpioSignalInfo_t* const  sigInfo,
                                                            uint32_t const                          channelIdx)
{
    (void)channelIdx;
    static const uint8_t DEFAULT_STG3_SELECT       = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Check if connect is valid */
    if ((connect != ADI_ENABLE) &&
        (connect != ADI_DISABLE) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 connect,
                                 "Invalid GPIO Connect/Disconnet Option. Must be 0 or 1.");
        return recoveryAction;
    }
    
    /* Check gpio is valid */
    if (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 gpio,
                                 "Invalid GPIO selected. Out of range.");
        return recoveryAction;
    }
    
    /* Setup the Route for this GPIO/signal (and channel, if this signal is a channel-instantiate signal) */
    switch(sigInfo->route)
    {
        case ADRV904X_GPIO_ROUTE_ANA_PINMUX_CORE:
            recoveryAction = adrv904x_GpioAnalogPinmuxSet( device, 
                                                           gpio,
                                                           (connect ? sigInfo->topSelect : DEFAULT_STG3_SELECT) );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GpioAnalogPinmuxSet issue");
                return recoveryAction;
            }
            break;
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     sigInfo->route,
                                     "Unsupported route type for selected GPIO signal");
            return recoveryAction;
            break;
    }
    
    return recoveryAction;
}

/****************************************************************************
 * GP Int (General Purpose Interrupt) related functions
 ***************************************************************************
 */

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntTypeSet(adi_adrv904x_Device_t* const           device,
                                                       const adi_adrv904x_GpIntMask_t* const  gpIntType)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpIntType);

    recoveryAction = adrv904x_Core_GpInterruptsLevelPulseBLowerWord_BfSet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          gpIntType->lowerMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Type Lower Word.");
        return recoveryAction;
    }

    recoveryAction = adrv904x_Core_GpInterruptsLevelPulseBUpperWord_BfSet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          gpIntType->upperMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Type Upper Word.");
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntTypeGet(adi_adrv904x_Device_t* const     device,
                                                       adi_adrv904x_GpIntMask_t* const  gpIntType)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, gpIntType);

    recoveryAction = adrv904x_Core_GpInterruptsLevelPulseBLowerWord_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &gpIntType->lowerMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Type Lower Word.");
        return recoveryAction;
    }

    recoveryAction = adrv904x_Core_GpInterruptsLevelPulseBUpperWord_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &gpIntType->upperMask );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Type Upper Word.");
        return recoveryAction;
    }

    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntPreMcsInit( adi_adrv904x_Device_t* const                 device,
                                                           const adi_adrv904x_GpIntPinMaskCfg_t* const  pinMaskCfg)
{
    const adi_adrv904x_GpIntPinSelect_e pins = ADI_ADRV904X_GPINTALL;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const adi_adrv904x_GpIntMask_t initGpIntTypeMask = { ADRV904X_GP_INT_TYPE_DEFAULT_LOWER, ADRV904X_GP_INT_TYPE_DEFAULT_UPPER };
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pinMaskCfg);
    
    /* Setup proper GP Interrupt Type defaults (Level vs Edge Triggered) */
    recoveryAction = adrv904x_GpIntTypeSet(device, &initGpIntTypeMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing default GP Interrupt Types.");
        return recoveryAction;
    }
    
    /* Set GP Int Pin Masks to selected states */
    recoveryAction = adi_adrv904x_GpIntPinMaskCfgSet(device, pins, pinMaskCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Array.");
        return recoveryAction;
    }
    
    /* Clear all status bits */
    recoveryAction = adi_adrv904x_GpIntStatusClear(device, NULL);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing GP Interrupt Status bits.");
        return recoveryAction;
    }
    
    return recoveryAction;
}


ADI_API adi_adrv904x_ErrAction_e adrv904x_GpIntPostMcsInit( adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg)
{
    const adi_adrv904x_GpIntPinSelect_e pins = ADI_ADRV904X_GPINTALL;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, pinMaskCfg);
    
    /* Set GP Int Pin Masks to selected states */
    recoveryAction = adi_adrv904x_GpIntPinMaskCfgSet(device, pins, pinMaskCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Array.");
        return recoveryAction;
    }
    
    /* Clear all status bits */
    recoveryAction = adi_adrv904x_GpIntStatusClear(device, NULL);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing GP Interrupt Status bits.");
        return recoveryAction;
    }
    
    return recoveryAction;
}
