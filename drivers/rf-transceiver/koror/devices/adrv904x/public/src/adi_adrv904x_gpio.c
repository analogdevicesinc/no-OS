/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_gpio.c
* \brief Contains GPIO feature related function implementation defined in
* adi_adrv904x_gpio.h
*
* ADRV904X API Version: 2.10.0.4
*/
#include "adi_adrv904x_gpio.h"

#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/bf/adrv904x_bf_core.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_GPIO

static const adi_adrv904x_GpioSignal_e manualModeInputSignal[ADI_ADRV904X_GPIO_COUNT] = { 
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_0,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_1,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_2,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_3,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_4,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_5,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_6,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_7,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_8,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_9,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_10,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_11,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_12,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_13,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_14,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_15,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_16,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_17,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_18,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_19,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_20,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_21,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_22,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_23
};

static const adi_adrv904x_GpioSignal_e manualModeOutputSignal[ADI_ADRV904X_GPIO_COUNT] = { 
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_0,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_1,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_2,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_3,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_4,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_5,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_6,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_7,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_8,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_9,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_10,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_11,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_12,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_13,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_14,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_15,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_16,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_17,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_18,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_19,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_20,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_21,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_22,
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_23
};

static const adi_adrv904x_GpioSignal_e analogManualModeInputSignal[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { 
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_0,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_1,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_2,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_3,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_4,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_5,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_6,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_7,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_8,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_9,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_10,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_11,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_12,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_13,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_14,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_15
};

static const adi_adrv904x_GpioSignal_e analogManualModeOutputSignal[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { 
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_0,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_1,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_2,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_3,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_4,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_5,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_6,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_7,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_8,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_9,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_10,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_11,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_12,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_13,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_14,
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_15
};

/****************************************************************************
 * GPIO related functions
 ****************************************************************************
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioForceHiZAllPins(adi_adrv904x_Device_t* const  	device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    uint8_t gpioPin;
    uint8_t override = 1;
    for(gpioPin = ADI_ADRV904X_GPIO_00; gpioPin <= ADI_ADRV904X_GPIO_23; gpioPin++)
    {
        recoveryAction = adrv904x_GpioIeOverride(device, (adi_adrv904x_GpioPinSel_e)gpioPin, override);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gpio source control override.");
            goto cleanup;
        }
    }

cleanup:
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioForceHiZ(adi_adrv904x_Device_t* const        device,
                                                             const adi_adrv904x_GpioPinSel_e     gpio,
                                                             const uint8_t                       override)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_GpioIeOverride(device, gpio, override);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gpio source control override.");
    }

	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogForceHiZAllPins(adi_adrv904x_Device_t* const 	device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    uint8_t gpioPin;
    uint8_t override = 1;
    for(gpioPin = ADI_ADRV904X_GPIO_ANA_00; gpioPin <= ADI_ADRV904X_GPIO_ANA_15; gpioPin++)
    {
        recoveryAction = adrv904x_GpioAnalogIeOverride(device, (adi_adrv904x_GpioAnaPinSel_e)gpioPin, override);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gpio source control override.");
            goto cleanup;
        }
    }

cleanup:
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogForceHiZ( adi_adrv904x_Device_t* const        device,
		                                                         const adi_adrv904x_GpioAnaPinSel_e  gpio,
									 const uint8_t                       override)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_GpioAnalogIeOverride(device, gpio, override);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write gpio source control override.");
    }

	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioStatusRead(   adi_adrv904x_Device_t* const        device,
                                                                adi_adrv904x_GpioStatus_t* const    status)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioStatus_t read;
    uint8_t i = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, status, cleanup);
    
    /* Clear out readback structure*/
    ADI_LIBRARY_MEMSET(&read, 0, sizeof(adi_adrv904x_GpioStatus_t));

    /* Readback Digital GPIO configs */
    recoveryAction = adi_adrv904x_GpioConfigAllGet(device, read.digSignal, read.digChMask, ADI_ADRV904X_GPIO_COUNT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading back the status of all Digital GPIOs");
        goto cleanup;
    }
    
    /* Readback Analog GPIO configs */
    recoveryAction = adi_adrv904x_GpioAnalogConfigAllGet(device, read.anaSignal, read.anaChMask, ADI_ADRV904X_GPIO_COUNT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading back the status of all Analog GPIOs");
        goto cleanup;
    }
    
    /* Build pinmask for allocated digital pins */
    read.digPinAllocated = 0U;
    for (i = 0U; i < ADI_ADRV904X_GPIO_COUNT; i++)
    {
        if (read.digSignal[i] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
        {
            read.digPinAllocated |= (1U << i);
        }
    }
    
    /* Build pinmask for allocated analog pins */
    read.anaPinAllocated = 0U;
    for (i = 0U; i < ADI_ADRV904X_GPIO_ANALOG_COUNT; i++)
    {
        if (read.anaSignal[i] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
        {
            read.anaPinAllocated |= (1U << i);
        }
    }
    
    /* Set status to readback structure */
    *status = read;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioConfigGet(adi_adrv904x_Device_t* const        device,
                                                            const adi_adrv904x_GpioPinSel_e     gpio,
                                                            adi_adrv904x_GpioSignal_e* const    signal,
                                                            uint32_t* const                     channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, signal, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, channelMask, cleanup);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpio, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }
    
    /* Call utility to retrieve current signal/channelMask for selected gpio */
    recoveryAction = adrv904x_GpioSignalGet( device, gpio, signal, channelMask );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while obtaining current GPIO configuration");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioConfigAllGet(adi_adrv904x_Device_t* const    device,
                                                               adi_adrv904x_GpioSignal_e       signalArray[],
                                                               uint32_t                        channelMaskArray[],
                                                               const uint32_t                  arraySize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_GpioPinSel_e tmpGpio = ADI_ADRV904X_GPIO_INVALID;
    adi_adrv904x_GpioSignal_e tmpSig = ADI_ADRV904X_GPIO_SIGNAL_INVALID;
    uint32_t tmpMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, signalArray, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, channelMaskArray, cleanup);
    
    /* Range check arraySize */
    if ( arraySize < ADI_ADRV904X_GPIO_INVALID )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, arraySize, "Insufficient arraySize provided by user to store GPIO signalArray and channelMaskArray. arraySize must be greater than or equal to 24.");
        goto cleanup;
    }
    
    /* Iterate through all gpios */
    for ( gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_INVALID; gpioIdx++ )
    {
        /* Call utility to retrieve  signal/channelMask for the gpio */
        tmpGpio = (adi_adrv904x_GpioPinSel_e)gpioIdx;
        recoveryAction = adrv904x_GpioSignalGet( device, tmpGpio, &tmpSig, &tmpMask );
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while obtaining current GPIO configuration");
            goto cleanup;
        }
        
        /* Store results into arrays */
        signalArray[gpioIdx] = tmpSig;
        channelMaskArray[gpioIdx] = tmpMask;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogConfigGet(adi_adrv904x_Device_t* const        device,
                                                                  const adi_adrv904x_GpioAnaPinSel_e  gpio,
                                                                  adi_adrv904x_GpioSignal_e* const    signal,
                                                                  uint32_t* const                     channelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, signal, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, channelMask, cleanup);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_ANA_00) ||
        (gpio >= ADI_ADRV904X_GPIO_ANA_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpio, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }
    
    /* Call utility to retrieve current signal/channelMask for selected gpio */
    recoveryAction = adrv904x_GpioAnalogSignalGet( device, gpio, signal, channelMask );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while obtaining current GPIO configuration");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogConfigAllGet(adi_adrv904x_Device_t* const    device,
                                                                     adi_adrv904x_GpioSignal_e       signalArray[],
                                                                     uint32_t                        channelMaskArray[],
                                                                     const uint32_t                  arraySize)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    adi_adrv904x_GpioAnaPinSel_e tmpGpio = ADI_ADRV904X_GPIO_ANA_INVALID;
    adi_adrv904x_GpioSignal_e tmpSig = ADI_ADRV904X_GPIO_SIGNAL_INVALID;
    uint32_t tmpMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, signalArray, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, channelMaskArray, cleanup);
    
    /* Range check arraySize */
    if ( arraySize < ADI_ADRV904X_GPIO_ANA_INVALID )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, arraySize, "Insufficient arraySize provided by user to store GPIO signalArray and channelMaskArray. arraySize must be greater than or equal to 24.");
        goto cleanup;
    }
    
    /* Iterate through all gpios */
    for ( gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_ANA_INVALID; gpioIdx++ )
    {
        /* Call utility to retrieve  signal/channelMask for the gpio */
        tmpGpio = (adi_adrv904x_GpioAnaPinSel_e)gpioIdx;
        recoveryAction = adrv904x_GpioAnalogSignalGet( device, tmpGpio, &tmpSig, &tmpMask );
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while obtaining current GPIO configuration");
            goto cleanup;
        }
        
        /* Store results into arrays */
        signalArray[gpioIdx] = tmpSig;
        channelMaskArray[gpioIdx] = tmpMask;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioMonitorOutSet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_GpioPinSel_e     gpio,
                                                                const adi_adrv904x_GpioSignal_e     signal,
                                                                const uint8_t                       channel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_GpioSignalInfo_t info = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED, ADRV904X_GPIO_DOMAIN_NONE, ADRV904X_GPIO_ROUTE_OFF, 0U, 0U, 0 };
    uint32_t channelMask = 0U;
    uint8_t isValidFlag = ADI_FALSE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpio, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }
    
    /* Range check signal */
    if ((signal < ADI_ADRV904X_GPIO_SIGNAL_UNUSED) ||
        (signal >= ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, signal, "Invalid signal selected. Out of range.");
        goto cleanup;
    }
    
    /* If signal is UNUSED, automatically try to release this pin from Monitor Output routing */
    if (signal == ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
    {
        /* Done whether successfully released or not */
        recoveryAction = adi_adrv904x_GpioMonitorOutRelease(device, gpio);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while releasing GPIO from a Monitor Output");
        }
        goto cleanup;
    }
    
    /* Get signal info struct */
    recoveryAction = adrv904x_GpioSignalInfoGet(device, signal, &info);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while obtaining signal info");
        goto cleanup;
    }
    
    /* Check if signal is a valid monitor output signal using the route */
    recoveryAction = adrv904x_GpioMonitorOutSignalValidCheck(device, signal, channel, &isValidFlag, &channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error checking Monitor Out signal validity");
        goto cleanup;
    }
    if (isValidFlag != ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, signal, "GPIO signal is not a valid GPIO Monitor Output.");
        goto cleanup;
    }
    
    /* Set the signal to the GPIO */
    recoveryAction = adrv904x_GpioSignalSet( device, gpio, signal, channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while routing selected monitor output signal to GPIO.");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioMonitorOutRelease(adi_adrv904x_Device_t* const        device,
                                                                    const adi_adrv904x_GpioPinSel_e     gpio)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e signal = ADI_ADRV904X_GPIO_SIGNAL_UNUSED;
    uint8_t isValidFlag = ADI_FALSE;
    uint32_t channelMask = 0U;
    uint8_t channel = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check gpio */
    if ((gpio < ADI_ADRV904X_GPIO_00) ||
        (gpio >= ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpio, "Invalid GPIO selected. Out of range.");
        goto cleanup;
    }
    
    /* Retrieve gpio's current signal */
    recoveryAction = adrv904x_GpioSignalGet(device, gpio, &signal, &channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retreiving currently routed GPIO signal.");
        goto cleanup;
    }
    
    /* Check for UNUSED. If UNUSED, nothing to do */
    if (signal == ADI_ADRV904X_GPIO_SIGNAL_UNUSED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }
    
    /* Convert channelMask to channel index */
    switch (channelMask)
    {
        case 0x01U:
            channel = 0U;
            break;
        case 0x02U:
            channel = 1U;
            break;
        case 0x04U:
            channel = 2U;
            break;
        case 0x08U:
            channel = 3U;
            break;
        case 0x10U:
            channel = 4U;
            break;
        case 0x20U:
            channel = 5U;
            break;
        case 0x40U:
            channel = 6U;
            break;
        case 0x80U:
            channel = 7U;
            break;
        default:
            channel = 0U;
            break;
    }
    
    /* Check for if the gpio is currently routing a valid Monitor Out Signal */
    recoveryAction = adrv904x_GpioMonitorOutSignalValidCheck(device, signal, channel, &isValidFlag, NULL);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error checking Monitor Out signal validity");
        goto cleanup;
    }
    if (isValidFlag != ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, signal, "GPIO signal is not a valid GPIO Monitor Output.");
        goto cleanup;
    }
    
    /* Release GPIO */
    recoveryAction = adrv904x_GpioSignalRelease(device, gpio, signal, channelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing GPIO from Monitor Out signal.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualInputDirSet(    adi_adrv904x_Device_t* const    device,
                                                                        const uint32_t                  gpioInputMask)
{
        static const uint32_t channelMask = 0U;
    static const uint32_t RANGE_CHECK_MASK = 0xFF000000U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e prevSignalArray[ADI_ADRV904X_GPIO_COUNT] = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED };
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_COUNT] = { 0U };
    uint32_t idx = 0U;
    uint32_t tmpMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check input */
    if ((gpioInputMask & RANGE_CHECK_MASK) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioInputMask, "Invalid GPIO Input Mask. Out of range.");
        goto cleanup;
    }
    
    /* Get current GPIO configs */
    recoveryAction = adi_adrv904x_GpioConfigAllGet( device, prevSignalArray, channelMaskArray, ADI_ADRV904X_GPIO_COUNT );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retreiving current GPIO configurations.");
        goto cleanup;
    }
    
    /* Check that all set-pins are unused or already allocated as Manual Inputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioInputMask & tmpMask) != 0U) &&
            (prevSignalArray[idx] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
            (prevSignalArray[idx] != manualModeInputSignal[idx]))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioInputMask, "Invalid GPIO Input Mask. A selected pin is already in use by another feature.");
            goto cleanup;
        }
    }
    
    /* Deallocate and disconnect any unset-pins that were previously allocated as Manual Inputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioInputMask & tmpMask) == 0U) &&
            (prevSignalArray[idx] == manualModeInputSignal[idx]))
        {
            recoveryAction = adrv904x_GpioSignalRelease( device, (adi_adrv904x_GpioPinSel_e)idx, prevSignalArray[idx], channelMaskArray[idx] );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while releasing a GPIO from use as a Manual Input.");
                goto cleanup;
            }
        }
    }
    
    /* Configure all set-pins */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if ((gpioInputMask & tmpMask) != 0U)
        {
            recoveryAction = adrv904x_GpioSignalSet( device, (adi_adrv904x_GpioPinSel_e)idx, manualModeInputSignal[idx], channelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while configuring a GPIO for use as a Manual Input.");
                goto cleanup;
            }
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputDirSet(   adi_adrv904x_Device_t* const    device,
                                                                        const uint32_t                  gpioOutputMask)
{
        static const uint32_t channelMask = 0U;
    static const uint32_t RANGE_CHECK_MASK = 0xFF000000U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e prevSignalArray[ADI_ADRV904X_GPIO_COUNT] = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED };
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_COUNT] = { 0U };
    uint32_t idx = 0U;
    uint32_t tmpMask = 0U;
    uint32_t clearDrivePins = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check input */
    if ((gpioOutputMask & RANGE_CHECK_MASK) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioOutputMask, "Invalid GPIO Output Mask. Out of range.");
        goto cleanup;
    }
    
    /* Get current GPIO configs */
    recoveryAction = adi_adrv904x_GpioConfigAllGet( device, prevSignalArray, channelMaskArray, ADI_ADRV904X_GPIO_COUNT );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retreiving current GPIO configurations.");
        goto cleanup;
    }
    
    /* Check that all set-pins are unused or already allocated as Manual Outputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioOutputMask & tmpMask) != 0U) &&
            (prevSignalArray[idx] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
            (prevSignalArray[idx] != manualModeOutputSignal[idx]))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioOutputMask, "Invalid GPIO Output Mask. A selected pin is already in use by another feature.");
            goto cleanup;
        }
    }
    
    /* Deallocate and disconnect any unset-pins that were previously allocated as Manual Outputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioOutputMask & tmpMask) == 0U) &&
            (prevSignalArray[idx] == manualModeOutputSignal[idx]))
        {
            recoveryAction = adrv904x_GpioSignalRelease( device, (adi_adrv904x_GpioPinSel_e)idx, prevSignalArray[idx], channelMaskArray[idx] );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while releasing a GPIO from use as a Manual Output.");
                goto cleanup;
            }
            
            clearDrivePins |= tmpMask;
        }
    }

    /* Clear the GpioFromMaster drive bits from any deallocated pins */
    recoveryAction = adrv904x_Core_GpioFromMasterClear_BfSet(device,
                                                             NULL,
                                                             (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                             clearDrivePins);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing drive levels for GPIOs previously allocated as Manual Mode Outputs.");
        goto cleanup;
    }

    /* Configure all set-pins */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if ((gpioOutputMask & tmpMask) != 0U)
        {
            recoveryAction = adrv904x_GpioSignalSet( device, (adi_adrv904x_GpioPinSel_e)idx, manualModeOutputSignal[idx], channelMask );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while configuring a GPIO for use as a Manual Output.");
                goto cleanup;
            }
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualInputPinLevelGet(   adi_adrv904x_Device_t* const    device,
                                                                            uint32_t * const                gpioInPinLevel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioInPinLevel, cleanup);

    /* Readback bitfield */
    /* Using Direct SPI instead of AHB */
    recoveryAction = adrv904x_Core_GpioSpiRead_BfGet(device,
                                                     NULL,
                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                     gpioInPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading back GPIO Manual Mode Input Word values.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputPinLevelGet(  adi_adrv904x_Device_t* const    device,
                                                                            uint32_t * const                gpioOutPinLevel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioOutPinLevel, cleanup);

    /* Readback bitfield */
    /* Using Direct SPI instead of AHB */
    recoveryAction = adrv904x_Core_GpioFromMaster_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        gpioOutPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading back GPIO Manual Mode Output Word values.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioManualOutputPinLevelSet(  adi_adrv904x_Device_t* const    device,
                                                                            const uint32_t                  gpioPinMask,
                                                                            const uint32_t                  gpioOutPinLevel)
{
        static const uint32_t VALID_RANGE_MASK = 0x00FFFFFFU;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_COUNT] = { 0U };
    uint32_t currentLevel = 0U;
    uint32_t toggleLevel = 0U;
    uint32_t idx = 0U;
    uint32_t tmpMask = 0U;
    uint32_t allocatedMask = 0U;
    adi_adrv904x_GpioSignal_e signalArray[ADI_ADRV904X_GPIO_COUNT] =
    { 
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED
    };
        
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check input */
    if ((gpioOutPinLevel & VALID_RANGE_MASK) != gpioOutPinLevel)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioOutPinLevel, "Invalid GPIO Output pin levels. Out of range.");
        goto cleanup;
    }
    
    /* Range check pinmask */
    if ((gpioPinMask & VALID_RANGE_MASK) != gpioPinMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioPinMask, "Invalid GPIO pinmask. Out of range.");
        goto cleanup;
    }
    
    /* Readback all GPIO allocations */
    recoveryAction = adi_adrv904x_GpioConfigAllGet(device, signalArray, channelMaskArray, ADI_ADRV904X_GPIO_COUNT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retreiving current GPIO configurations.");
        goto cleanup;
    }
    
    /* Build mask containing only/all pins allocated for this feature */
    for (idx = 0U; idx < ADI_ADRV904X_GPIO_COUNT; idx++)
    {
        tmpMask = 1U << idx;
        if (signalArray[idx] == manualModeOutputSignal[idx])
        {
            allocatedMask |= tmpMask;
        }
        else
        {
            tmpMask = ~tmpMask;
            allocatedMask &= tmpMask;
        }
    }
    
    /* Check that gpioPinMask contains only pins that are allocated for this feature */
    if ((gpioPinMask & allocatedMask) != gpioPinMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioPinMask, "Invalid GPIO pinmask. Includes pins that are not allocated as Manual Mode Outputs.");
        goto cleanup;
    }

    /* Readback current drive state of Digital GPIO pins */
    recoveryAction = adi_adrv904x_GpioManualOutputPinLevelGet(device, &currentLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retrieving current GPIO Manual Mode drive levels.");
        goto cleanup;
    }

    /* XOR with current levels with target levels to determine which pins need to toggle */
    toggleLevel = currentLevel ^ gpioOutPinLevel;

    /* Mask out pins that are not in the requested gpioPinMask. They should not be toggled. */
    toggleLevel &= gpioPinMask;

    /* Toggle appropriate pins to achieve target levels */
    recoveryAction = adrv904x_Core_GpioFromMasterToggle_BfSet(device,
                                                              NULL,
                                                              (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                              toggleLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting GPIO output levels.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualInputDirSet(adi_adrv904x_Device_t* const  device,
                                                                          const uint16_t                gpioAnalogInputMask)
{
        static const uint32_t channelMask = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e prevSignalArray[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED };
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { 0U };
    uint32_t idx = 0U;
    uint32_t tmpMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Get current GPIO configs */
    recoveryAction = adi_adrv904x_GpioAnalogConfigAllGet( device, prevSignalArray, channelMaskArray, ADI_ADRV904X_GPIO_ANALOG_COUNT );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retreiving current Analog GPIO configurations.");
        goto cleanup;
    }
    
    /* Check that all set-pins are unused or already allocated as Manual Inputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioAnalogInputMask & tmpMask) != 0U) &&
            (prevSignalArray[idx] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
            (prevSignalArray[idx] != analogManualModeInputSignal[idx]))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioAnalogInputMask, "Invalid Analog GPIO Input Mask. A selected pin is already in use by another feature.");
            goto cleanup;
        }
    }
    
    /* Deallocate and disconnect any unset-pins that were previously allocated as Manual Inputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioAnalogInputMask & tmpMask) == 0U) &&
            (prevSignalArray[idx] == analogManualModeInputSignal[idx]))
        {
            recoveryAction = adrv904x_GpioAnalogSignalRelease(device, (adi_adrv904x_GpioAnaPinSel_e)idx, prevSignalArray[idx], channelMaskArray[idx]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while releasing an Analog GPIO from use as a Manual Input.");
                goto cleanup;
            }
        }
    }
    
    /* Configure all set-pins */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if ((gpioAnalogInputMask & tmpMask) != 0U)
        {
            recoveryAction = adrv904x_GpioAnalogSignalSet(device, (adi_adrv904x_GpioAnaPinSel_e)idx, analogManualModeInputSignal[idx], channelMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while configuring an Analog GPIO for use as a Manual Input.");
                goto cleanup;
            }
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputDirSet( adi_adrv904x_Device_t* const    device,
                                                                            const uint16_t                  gpioAnalogOutputMask)
{
        static const uint32_t channelMask = 0U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_GpioSignal_e prevSignalArray[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { ADI_ADRV904X_GPIO_SIGNAL_UNUSED };
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { 0U };
    uint32_t idx = 0U;
    uint32_t tmpMask = 0U;
    uint32_t clearDrivePins = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Get current GPIO configs */
    recoveryAction = adi_adrv904x_GpioAnalogConfigAllGet( device, prevSignalArray, channelMaskArray, ADI_ADRV904X_GPIO_ANALOG_COUNT );
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while retreiving current Analog GPIO configurations.");
        goto cleanup;
    }
    
    /* Check that all set-pins are unused or already allocated as Manual Inputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioAnalogOutputMask & tmpMask) != 0U) &&
            (prevSignalArray[idx] != ADI_ADRV904X_GPIO_SIGNAL_UNUSED) &&
            (prevSignalArray[idx] != analogManualModeOutputSignal[idx]))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, gpioAnalogOutputMask, "Invalid Analog GPIO Output Mask. A selected pin is already in use by another feature.");
            goto cleanup;
        }
    }
    
    /* Deallocate and disconnect any unset-pins that were previously allocated as Analog Manual Outputs */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if (((gpioAnalogOutputMask & tmpMask) == 0U) &&
            (prevSignalArray[idx] == analogManualModeOutputSignal[idx]))
        {
            recoveryAction = adrv904x_GpioAnalogSignalRelease( device, (adi_adrv904x_GpioAnaPinSel_e)idx, prevSignalArray[idx], channelMaskArray[idx] );
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while releasing an Analog GPIO from use as a Manual Output.");
                goto cleanup;
            }

            clearDrivePins |= tmpMask;
        }
    }

    /* Clear the GpioFromMaster drive bits from any deallocated pins */
    recoveryAction = adrv904x_Core_GpioAnalogFromMasterClear_BfSet(device,
                                                                   NULL,
                                                                   (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                   clearDrivePins);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing drive levels for GPIOs previously allocated as Manual Mode Outputs.");
        goto cleanup;
    }

    /* Configure all set-pins */
    for ( idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++ )
    {
        tmpMask = 1U << idx;
        if ((gpioAnalogOutputMask & tmpMask) != 0U)
        {
            recoveryAction = adrv904x_GpioAnalogSignalSet(device, (adi_adrv904x_GpioAnaPinSel_e)idx, analogManualModeOutputSignal[idx], channelMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while configuring an Analog GPIO for use as a Manual Output.");
                goto cleanup;
            }
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualInputPinLevelGet( adi_adrv904x_Device_t* const    device,
                                                                                uint16_t * const                gpioAnalogInPinLevel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioAnalogInPinLevel, cleanup);

    /* Readback bitfield */
    /* Using Direct SPI instead of AHB */
    recoveryAction = adrv904x_Core_GpioAnalogSpiRead_BfGet(device,
                                                           NULL,
                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                           gpioAnalogInPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading back ANalog GPIO Manual Mode Input Word values.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputPinLevelGet(adi_adrv904x_Device_t* const  device,
                                                                                uint16_t * const              gpioAnalogOutPinLevel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioAnalogOutPinLevel, cleanup);

    /* Readback bitfield */
    /* Using Direct SPI instead of AHB */
    recoveryAction = adrv904x_Core_GpioAnalogFromMaster_BfGet(device,
                                                              NULL,
                                                              (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                              gpioAnalogOutPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common, recoveryAction, "Error while reading back Analog GPIO Manual Mode Output Word values.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioAnalogManualOutputPinLevelSet(adi_adrv904x_Device_t* const  device,
                                                                                const uint16_t                gpioAnalogPinMask,
                                                                                const uint16_t                gpioAnalogOutPinLevel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t channelMaskArray[ADI_ADRV904X_GPIO_COUNT] = { 0U };
    uint16_t currentLevel = 0U;
    uint32_t toggleLevel = 0U;
    uint32_t idx = 0U;
    uint16_t tmpMask = 0U;
    uint32_t allocatedMask = 0U;
    adi_adrv904x_GpioSignal_e signalArray[ADI_ADRV904X_GPIO_ANALOG_COUNT] =
    { 
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED,
        ADI_ADRV904X_GPIO_SIGNAL_UNUSED
    };
        
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Readback all Analog GPIO allocations */
    recoveryAction = adi_adrv904x_GpioAnalogConfigAllGet(device, signalArray, channelMaskArray, ADI_ADRV904X_GPIO_ANALOG_COUNT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retreiving current Analog GPIO configurations.");
        goto cleanup;
    }
    
    /* Build mask containing only/all pins allocated for this features */
    for (idx = 0U; idx < ADI_ADRV904X_GPIO_ANALOG_COUNT; idx++)
    {
        tmpMask = 1U << idx;
        if (signalArray[idx] == analogManualModeOutputSignal[idx])
        {
            allocatedMask |= tmpMask;
        }
        else
        {
            tmpMask = ~tmpMask;
            allocatedMask &= tmpMask;
        }
    }
    
    /* Check that gpioAnalogPinMask contains only pins that are allocated for this feature */
    if ((gpioAnalogPinMask & allocatedMask) != gpioAnalogPinMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpioAnalogPinMask, "Invalid Analog GPIO pinmask. Includes pins that are not allocated as Analog Manual Mode Outputs.");
        goto cleanup;
    }

    /* Readback current drive state of Digital GPIO pins */
    recoveryAction = adi_adrv904x_GpioAnalogManualOutputPinLevelGet(device, &currentLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while retrieving current Analog GPIO Manual Mode drive levels.");
        goto cleanup;
    }

    /* XOR with current levels with target levels to determine which pins need to toggle */
    toggleLevel = (uint32_t)(currentLevel ^ gpioAnalogOutPinLevel);

    /* Mask out pins that are not in the requested gpioAnalogPinMask. They should not be toggled. */
    toggleLevel &= gpioAnalogPinMask;

    /* Toggle appropriate pins to achieve target levels */
    recoveryAction = adrv904x_Core_GpioAnalogFromMasterToggle_BfSet(device,
                                                                    NULL,
                                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                    toggleLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Analog GPIO output levels.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

/****************************************************************************
 * GP Int (General Purpose Interrupt) related functions
 ***************************************************************************
 */

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntPinMaskCfgSet(   adi_adrv904x_Device_t* const                device,
                                                                    const adi_adrv904x_GpIntPinSelect_e         pinSelect,
                                                                    const adi_adrv904x_GpIntPinMaskCfg_t* const pinMaskCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pinMaskCfg, cleanup);
    
    if ((pinSelect != ADI_ADRV904X_GPINT0) &&
        (pinSelect != ADI_ADRV904X_GPINT1) &&
        (pinSelect != ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pinSelect, "Invalid GP Interrupt pin(s) selected. Out of range.");
        goto cleanup;
    }

    /* Set GPINT0 Mask */
    if ((pinSelect == ADI_ADRV904X_GPINT0) ||
        (pinSelect == ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = adrv904x_Core_GpInterruptsMaskLowerWordPin0_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           pinMaskCfg->gpInt0Mask.lowerMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Lower Word for GP Int0.");
            goto cleanup;
        }

        recoveryAction = adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           pinMaskCfg->gpInt0Mask.upperMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Upper Word for GP Int0.");
            goto cleanup;
        }
    }

    /* Set GPINT1 Mask */
    if ((pinSelect == ADI_ADRV904X_GPINT1) ||
        (pinSelect == ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = adrv904x_Core_GpInterruptsMaskLowerWordPin1_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           pinMaskCfg->gpInt1Mask.lowerMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Lower Word for GP Int1.");
            goto cleanup;
        }

        recoveryAction = adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           pinMaskCfg->gpInt1Mask.upperMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing GP Interrupt Mask Upper Word for GP Int1.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntPinMaskCfgGet(   adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpIntPinSelect_e     pinSelect,
                                                                    adi_adrv904x_GpIntPinMaskCfg_t* const   pinMaskCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pinMaskCfg, cleanup);
    
    if ((pinSelect != ADI_ADRV904X_GPINT0) &&
        (pinSelect != ADI_ADRV904X_GPINT1) &&
        (pinSelect != ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pinSelect, "Invalid GP Interrupt pin(s) selected. Out of range.");
        goto cleanup;
    }

    /* Get GPINT0 Mask */
    if ((pinSelect == ADI_ADRV904X_GPINT0) ||
        (pinSelect == ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = adrv904x_Core_GpInterruptsMaskLowerWordPin0_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           &pinMaskCfg->gpInt0Mask.lowerMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Mask Lower Word for GP Int0.");
            goto cleanup;
        }

        recoveryAction = adrv904x_Core_GpInterruptsMaskUpperWordPin0_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           &pinMaskCfg->gpInt0Mask.upperMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Mask Upper Word for GP Int0.");
            goto cleanup;
        }
    }

    /* Get GPINT1 Mask */
    if ((pinSelect == ADI_ADRV904X_GPINT1) ||
        (pinSelect == ADI_ADRV904X_GPINTALL))
    {
        recoveryAction = adrv904x_Core_GpInterruptsMaskLowerWordPin1_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           &pinMaskCfg->gpInt1Mask.lowerMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Mask Lower Word for GP Int1.");
            goto cleanup;
        }

        recoveryAction = adrv904x_Core_GpInterruptsMaskUpperWordPin1_BfGet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                           &pinMaskCfg->gpInt1Mask.upperMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Mask Upper Word for GP Int1.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStatusGet(   adi_adrv904x_Device_t* const        device,
                                                                adi_adrv904x_GpIntMask_t* const     gpIntStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpIntStatus, cleanup);

    /* Get GP interrupt status bits */
    recoveryAction = adrv904x_Core_GpInterruptsStatusLowerWord_BfGet(device, 
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                     &gpIntStatus->lowerMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Status Lower Word.");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_GpInterruptsStatusUpperWord_BfGet(device, 
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                     &gpIntStatus->upperMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading GP Interrupt Status Upper Word.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStatusClear( adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpIntMask_t* const   gpIntClear)
{
        static const uint64_t GP_INT_48BIT_ALL = 0x0000FFFFFFFFFFFFULL;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint64_t tmpLower = 0ULL;
    uint64_t tmpUpper = 0ULL;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* If gpIntClear is NULL ptr, simply clear all GP Int source status bits */
    tmpLower = (gpIntClear == NULL) ? GP_INT_48BIT_ALL : gpIntClear->lowerMask;
    tmpUpper = (gpIntClear == NULL) ? GP_INT_48BIT_ALL : gpIntClear->upperMask;

    /* Clear selected GP Interrupt status bits*/
    recoveryAction = adrv904x_Core_GpInterruptsStatusLowerWord_BfSet(device, 
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                     tmpLower);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing selected GP Interrupt Status Lower Word bits.");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_GpInterruptsStatusUpperWord_BfSet(device, 
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                     tmpUpper);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing selected GP Interrupt Status Upper Word bits.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStickyBitMaskSet(adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpIntMask_t* const   gpIntStickyBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpIntStickyBitMask, cleanup);

    /* Setup proper GP Interrupt Type defaults (Level vs Edge Triggered) */
    recoveryAction = adrv904x_GpIntTypeSet(device, gpIntStickyBitMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing default GP Interrupt Types.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpIntStickyBitMaskGet(adi_adrv904x_Device_t* const            device,
                                                                    adi_adrv904x_GpIntMask_t* const   gpIntStickyBitMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Validate inputs */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpIntStickyBitMask, cleanup);

    /* Setup proper GP Interrupt Type defaults (Level vs Edge Triggered) */
    recoveryAction = adrv904x_GpIntTypeGet(device, gpIntStickyBitMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing default GP Interrupt Types.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioHysteresisSet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                const uint32_t                          enable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    const uint8_t PAD_DS_GROUP      = (uint8_t) ((pinName & 0xF0U) >> 4U);
    const uint8_t PAD_DS_PIN_INDEX  = (uint8_t) (pinName & 0x0FU);

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (PAD_DS_GROUP > ADRV904X_DIGITAL_PIN_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_GROUP,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (PAD_DS_PIN_INDEX > ADRV904X_DIGITAL_PIN_PER_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_PIN_INDEX,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (enable > ADI_ENABLE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                enable,
                                "GPIO Hysterisis Enable Flag can only be 0 or 1");
        goto cleanup;
    }

    /* Read Modify Write */

    recoveryAction = adrv904x_Core_PadSt_BfGet( device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    if (enable == ADI_ENABLE)
    {
        bfValue = (bfValue | (1U << PAD_DS_PIN_INDEX));
    }
    else
    {
        bfValue = (bfValue & (0U << PAD_DS_PIN_INDEX));
    }

    recoveryAction = adrv904x_Core_PadSt_BfSet( device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioHysteresisGet(adi_adrv904x_Device_t* const            device,
                                                                const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                uint32_t* const                         enable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t bfValue = 0U;

    const uint8_t PAD_DS_GROUP      = (uint8_t) ((pinName & 0xF0U) >> 4U);
    const uint8_t PAD_DS_PIN_INDEX  = (uint8_t) (pinName & 0x0FU);

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    *enable = 0U;

    if (PAD_DS_GROUP > ADRV904X_DIGITAL_PIN_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_GROUP,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (PAD_DS_PIN_INDEX > ADRV904X_DIGITAL_PIN_PER_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_PIN_INDEX,
                                "Invalid Pin ID");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadSt_BfGet( device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    *enable = ((bfValue & (1U << PAD_DS_PIN_INDEX)) != 0U) ? ADI_ENABLE : ADI_DISABLE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioDriveStrengthSet( adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                    const adi_adrv904x_CmosPadDrvStr_e      driveStrength)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t dsRead[] = { 0U, 0U, 0U, 0U };
    uint8_t dsWrite[] = { 0U, 0U, 0U, 0U };
    uint8_t i = 0U;

    const uint8_t PAD_DS_GROUP      = (uint8_t) ((pinName & 0xF0U) >> 4U);
    const uint8_t PAD_DS_PIN_INDEX  = (uint8_t) (pinName & 0x0FU);

    static const uint8_t DS_ENUM_CONVERT[] = { 0U, 3U, 1U, 2U };

    const uint8_t IDX = DS_ENUM_CONVERT[driveStrength];

    static const uint8_t DS_VALUE[] = { 0x0U, 0x3U, 0x9U, 0xFU };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (PAD_DS_GROUP > ADRV904X_DIGITAL_PIN_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_GROUP,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (PAD_DS_PIN_INDEX > ADRV904X_DIGITAL_PIN_PER_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_PIN_INDEX,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (driveStrength > ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                driveStrength,
                                "Invalid GPIO Drive Strength Setting");
        goto cleanup;
    }

    /* Read Modify Write */

    /* 1) Read */
    recoveryAction = adrv904x_Core_PadDs0_BfGet(    device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    &dsRead[0]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs1_BfGet(    device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    &dsRead[1]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs2_BfGet(    device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    &dsRead[2]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs3_BfGet(    device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    &dsRead[3]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    /* 2) Modify */

    for (i = 0U; i < sizeof(dsWrite); ++i)
    {
        /* Checking DS Bit of the DS Value to be Used */
        if ((DS_VALUE[IDX] & (1U << i)) != 0U)
        {
            dsWrite[i] = (uint8_t)(dsRead[i] | (1U << PAD_DS_PIN_INDEX));
        }
        else
        {
            dsWrite[i] = (uint8_t)(dsRead[i] & (0U << PAD_DS_PIN_INDEX));
        }
    }

    /* 3) Write */

    if (dsWrite[0] != dsRead[0])
    {
        recoveryAction = adrv904x_Core_PadDs0_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    dsWrite[0]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }
    }

    if (dsWrite[1] != dsRead[1])
    {
        recoveryAction = adrv904x_Core_PadDs1_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    dsWrite[1]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }
    }

    if (dsWrite[2] != dsRead[2])
    {
        recoveryAction = adrv904x_Core_PadDs2_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    dsWrite[2]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }
    }

    if (dsWrite[3] != dsRead[3])
    {
        recoveryAction = adrv904x_Core_PadDs3_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    PAD_DS_GROUP,
                                                    dsWrite[3]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_GpioDriveStrengthGet( adi_adrv904x_Device_t* const            device,
                                                                    const adi_adrv904x_GpioDigitalPin_e     pinName,
                                                                    adi_adrv904x_CmosPadDrvStr_e* const     driveStrength)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t dsRead[] = { 0U, 0U, 0U, 0U };
    uint8_t i = 0U;

    uint8_t dsValue = 0U;

    const uint8_t PAD_DS_GROUP      = (uint8_t) ((pinName & 0xF0U) >> 4U);
    const uint8_t PAD_DS_PIN_INDEX  = (uint8_t) (pinName & 0x0FU);

    static const adi_adrv904x_CmosPadDrvStr_e ENUM_DS_CONVERT[] = { ADI_ADRV904X_CMOSPAD_DRV_WEAK, ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_1, ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_2, ADI_ADRV904X_CMOSPAD_DRV_STRONG };

    static const uint8_t DS_VALUE_IDX[]        = { 0x0U, 0x3U, 0x9U, 0xFU };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, driveStrength, cleanup);

    *driveStrength = (adi_adrv904x_CmosPadDrvStr_e) (0xFFU);

    if (PAD_DS_GROUP > ADRV904X_DIGITAL_PIN_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_GROUP,
                                "Invalid Pin ID");
        goto cleanup;
    }

    if (PAD_DS_PIN_INDEX > ADRV904X_DIGITAL_PIN_PER_GROUP_NUM)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                PAD_DS_PIN_INDEX,
                                "Invalid Pin ID");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs0_BfGet(device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &dsRead[0]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs1_BfGet(device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &dsRead[1]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs2_BfGet(device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &dsRead[2]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_PadDs3_BfGet(device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                PAD_DS_GROUP,
                                                &dsRead[3]);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    for (i = 0U; i < sizeof(dsRead); ++i)
    {
        if ((dsRead[i] & (1U << PAD_DS_PIN_INDEX)) != 0U)
        {
            dsValue |= (uint8_t) (1U << i);
        }
    }

    for (i = 0U; i < sizeof(DS_VALUE_IDX); ++i)
    {
        if (DS_VALUE_IDX[i] == dsValue)
        {
            *driveStrength = ENUM_DS_CONVERT[i];
        }
    }

    if (*driveStrength == (adi_adrv904x_CmosPadDrvStr_e)(0xFFU))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                dsValue,
                                "Invalid Drive Strength Value");

        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                *driveStrength,
                                "Invalid Drive Strength Value");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}