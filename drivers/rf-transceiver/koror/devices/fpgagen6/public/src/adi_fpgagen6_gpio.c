/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file: adi_fpgagen6_gpio.c
 *
 * \brief Contains top level fpgagen6 gpio related functions
 *
 * FPGA API Version: 2.9.0.4
 */

#include "adi_fpgagen6_gpio.h"
#include "adi_fpgagen6_hal.h"

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_gpio.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_GPIO

/*
 * Private Helpers
 *
 */

static adi_fpgagen6_ErrAction_e fpgagen6_Spi2ModeSet(adi_fpgagen6_Device_t* const device, const uint8_t enableFlag);
static adi_fpgagen6_ErrAction_e fpgagen6_JtagModeSet(adi_fpgagen6_Device_t* const device, const uint8_t enableFlag);
static adi_fpgagen6_ErrAction_e fpgagen6_VerifyAvailableGpio(adi_fpgagen6_Device_t* const device, const uint32_t pinMask);

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioModeSet(  adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_GpioModes_e gpioMode)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (gpioMode == ADI_FPGAGEN6_GPIO_MODE_NONE)
    {
        recoveryAction = fpgagen6_Spi2ModeSet(device, ADI_DISABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = fpgagen6_JtagModeSet(device, ADI_DISABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else if (gpioMode == ADI_FPGAGEN6_GPIO_MODE_JTAG)
    {
        recoveryAction = fpgagen6_JtagModeSet(device, ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else if (gpioMode == ADI_FPGAGEN6_GPIO_MODE_SPI2)
    {
        recoveryAction = fpgagen6_Spi2ModeSet(device, ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                gpioMode,
                                "Unsupported mode for this function. Use adi_fpgagen6_TddGpioAssignSet/AllClear for TDD mode support");
        goto cleanup;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioModeGet(  adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_GpioPinSel_e gpioSel,
                                                            adi_fpgagen6_GpioModes_e* const gpioMode)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioMode, cleanup);

    *gpioMode = device->devStateInfo.gpioPinMode[gpioSel];

    recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioInputDirSet(  adi_fpgagen6_Device_t* const    device,
                                                                const uint32_t                  pinSelMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t orgPinSelMask = 0U;
    uint32_t newPinSelMask = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (pinSelMask > ADI_FPGAGEN6_GPIO_MASK_MAX_VALUE)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                pinSelMask,
                                "pinSelMask cannot exceed max value");
        goto cleanup;
    }

    recoveryAction = fpgagen6_VerifyAvailableGpio( device, pinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_INPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfGet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    &orgPinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_INPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    newPinSelMask = orgPinSelMask | pinSelMask;

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(   device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    newPinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_INPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioOutputDirSet( adi_fpgagen6_Device_t* const device,
                                                                const uint32_t pinSelMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t orgPinSelMask  = 0U;
    uint32_t newPinSelMask  = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (pinSelMask > ADI_FPGAGEN6_GPIO_MASK_MAX_VALUE)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                pinSelMask,
                                "pinSelMask cannot exceed max value");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_VerifyAvailableGpio(device, pinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfGet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                &orgPinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    newPinSelMask = (orgPinSelMask & ~pinSelMask);

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                newPinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDirectionGet( adi_fpgagen6_Device_t* const device,
                                                                uint32_t* const pinSelMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, pinSelMask, cleanup);

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfGet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                pinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioWrite(adi_fpgagen6_Device_t* const device,
                                                        const uint32_t wrData)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    /* Autogenerated Bitfield Function for GPIO Output Set is implemented as 3 discrete reg writes.
     * This causes intermediate gpio states between initial and final set states between each byte write.
     * Handle setting GPIO output level changes as quickly as possible, so use custom write to all bytes in
     * a single transaction.
     * 
     */
    fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr = FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO;
    recoveryAction = adi_fpgagen6_RegisterFieldWrite(device,
                                                     ((uint32_t) baseAddr + 0xE8),
                                                     wrData,
                                                     0xFFFFFFU,
                                                     0x0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_WRITE,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioRead(adi_fpgagen6_Device_t* const device,
                                                       uint32_t* const rdData)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rdData, cleanup);

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioInput_BfGet(device,
                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                        rdData);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_READ,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxSet(adi_fpgagen6_Device_t* const device,
                                                            const uint32_t tddGpioEnSel,
                                                            const adi_fpgagen6_GpioPinSel_e gpioSel)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t gpioEnTddSelReadBack = 0U;
    uint32_t gpioEnTddSel = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    if (gpioSel >= ADI_FPGAGEN6_GPIO_INVALID)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                gpioSel,
                                "Invalid GPIO selected");
        goto cleanup;
    }

    if (tddGpioEnSel >= ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                tddGpioEnSel,
                                "Invalid GPIO TDD Module selected");
        goto cleanup;
    }

    if (device->devStateInfo.gpioPinMode[(uint32_t)gpioSel] != ADI_FPGAGEN6_GPIO_MODE_NONE)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                gpioSel,
                                "GPIO selected is currently in use.");
        goto cleanup;
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelTddEnable_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    tddGpioEnSel,
                                                                    (uint8_t)gpioSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Append to Current GPIO Enable TDD Selection */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfGet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    &gpioEnTddSelReadBack);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    gpioEnTddSel = (uint32_t) (gpioEnTddSelReadBack | (uint32_t) (1U << (uint32_t) gpioSel));

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfSet(    device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                        gpioEnTddSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    device->devStateInfo.gpioPinMode[(uint32_t)gpioSel] = ADI_FPGAGEN6_GPIO_MODE_TDD;

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxGet(adi_fpgagen6_Device_t* const device,
                                                            const uint32_t tddGpioEnSel,
                                                            adi_fpgagen6_GpioPinSel_e* const gpioSel)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t gpioReadback = (uint8_t)ADI_FPGAGEN6_GPIO_INVALID;
    uint32_t gpioEnTddSelReadBack = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioSel, cleanup);

    if(tddGpioEnSel >= ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                tddGpioEnSel,
                                "Invalid GPIO TDD Module selected");
        goto cleanup;
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelTddEnable_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    tddGpioEnSel,
                                                                    &gpioReadback);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    if (gpioReadback >= ADI_FPGAGEN6_GPIO_INVALID)
    {
        recoveryAction = ADI__FPGAGEN6_ERR_ACT_RESET_FEATURE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                gpioReadback,
                                "Invalid GPIO Pin assigned to this TDD Module");
        goto cleanup;
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfGet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    &gpioEnTddSelReadBack);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    if (device->devStateInfo.gpioPinMode[(uint32_t)gpioReadback] != ADI_FPGAGEN6_GPIO_MODE_TDD)
    {
        *gpioSel = ADI_FPGAGEN6_GPIO_INVALID;
    }
    else if((gpioEnTddSelReadBack & gpioReadback) != 1U)    /* 0: GPIO Not Enabled; > 1 Invalid gpioReadback */
    {
        *gpioSel = ADI_FPGAGEN6_GPIO_INVALID;
    }
    else
    {
        *gpioSel = (adi_fpgagen6_GpioPinSel_e)gpioReadback;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxClear(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;
    uint32_t tddGpioModuleIdx = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    for (gpioIdx = 0; gpioIdx < ADI_FPGAGEN6_GPIO_COUNT; ++gpioIdx)
    {
        if (device->devStateInfo.gpioPinMode[gpioIdx] == ADI_FPGAGEN6_GPIO_MODE_TDD)
        {
            device->devStateInfo.gpioPinMode[gpioIdx] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        }
    }

    for (tddGpioModuleIdx = 0; tddGpioModuleIdx < ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO; tddGpioModuleIdx++)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelTddEnable_BfSet(  device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                        tddGpioModuleIdx,
                                                                        0U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_CLEAR,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfSet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_CLEAR,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

static adi_fpgagen6_ErrAction_e fpgagen6_Spi2ModeSet(adi_fpgagen6_Device_t* const device, const uint8_t enableFlag)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_fpgagen6_GpioPinSel_e spi2Pins[ADI_FPGAGEN6_NUM_SPI2_PINS] = {  ADI_FPGAGEN6_GPIO_00,
                                                                        ADI_FPGAGEN6_GPIO_01,
                                                                        ADI_FPGAGEN6_GPIO_02,
                                                                        ADI_FPGAGEN6_GPIO_03 };
    uint32_t gpioIdx = 0U;
    adi_fpgagen6_GpioModes_e assignedMode = ADI_FPGAGEN6_GPIO_MODE_NONE;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioEnSpi2_BfSet(device,
                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                            enableFlag);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_SPI2_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    if (enableFlag == ADI_ENABLE)
    {
        assignedMode = ADI_FPGAGEN6_GPIO_MODE_SPI2;
    }
    else
    {
        assignedMode = ADI_FPGAGEN6_GPIO_MODE_NONE;
    }

    for (gpioIdx = 0U; gpioIdx < ADI_FPGAGEN6_NUM_SPI2_PINS; ++gpioIdx)
    {
        device->devStateInfo.gpioPinMode[spi2Pins[gpioIdx]] = assignedMode;
    }

    return recoveryAction;
}

static adi_fpgagen6_ErrAction_e fpgagen6_JtagModeSet(adi_fpgagen6_Device_t* const device, uint8_t const enableFlag)
{
    adi_fpgagen6_ErrAction_e recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t pinSelMask  = 0U;
    uint32_t pinOutLevel = 0U;

    const uint32_t JTAG_TEST_CODE = 0x5U;

    uint32_t jtagCodePins =     (1U << ADI_FPGAGEN6_GPIO_00) |
                                (1U << ADI_FPGAGEN6_GPIO_01) |
                                (1U << ADI_FPGAGEN6_GPIO_02);

    uint32_t jtagInputPins =    (1U << ADI_FPGAGEN6_GPIO_03) |
                                (1U << ADI_FPGAGEN6_GPIO_04) |
                                (1U << ADI_FPGAGEN6_GPIO_05) |
                                (1U << ADI_FPGAGEN6_GPIO_06) |
                                (1U << ADI_FPGAGEN6_GPIO_07);

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfGet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                &pinSelMask);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioOutput_BfGet(device,
                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                            &pinOutLevel);

    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    if ((enableFlag & 0x1U) == ADI_ENABLE)
    {
        /* Set used to transmit the codes as output pins */
        pinSelMask &= ~jtagCodePins;
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    pinSelMask);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Output the code */
        pinOutLevel &= ~jtagCodePins;
        pinOutLevel |= JTAG_TEST_CODE;
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioOutput_BfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                pinOutLevel);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Set the JTAG pins as input */
        pinSelMask |= jtagInputPins;
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    pinSelMask);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    
        /* Clear the test pin value before setting it as output */
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_TestOutput_BfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                ADI_DISABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    
        /* Set test pin to output */
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_TestTristate_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    ADI_DISABLE);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    
        /* Drive the test pin high */
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_TestOutput_BfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_00] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_01] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_02] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_03] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_04] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_05] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_06] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_07] = ADI_FPGAGEN6_GPIO_MODE_JTAG;
    }
    else
    {
        uint32_t jtagPinsAll = (jtagCodePins | jtagInputPins);

        /* Clear the output value on the test pin */
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_TestOutput_BfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                ADI_DISABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Clear the output value on the code pins */
        pinOutLevel &= ~jtagCodePins;
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioOutput_BfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                pinOutLevel);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
        
        /* Force all JTAG GPIO pins to input */
        pinSelMask |= jtagPinsAll;
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    pinSelMask);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        /* Set test pin as input */
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_TestTristate_BfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                    ADI_ENABLE);

        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_GPIO,
                                                        ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_00] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_01] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_02] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_03] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_04] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_05] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_06] = ADI_FPGAGEN6_GPIO_MODE_NONE;
        device->devStateInfo.gpioPinMode[ADI_FPGAGEN6_GPIO_07] = ADI_FPGAGEN6_GPIO_MODE_NONE;
    }

    return recoveryAction;
}

static adi_fpgagen6_ErrAction_e fpgagen6_VerifyAvailableGpio(adi_fpgagen6_Device_t* const device, const uint32_t pinMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t gpioIdx = 0U;

    for (gpioIdx = 0U; gpioIdx < ADI_FPGAGEN6_GPIO_COUNT; ++gpioIdx)
    {
        if (((pinMask >> gpioIdx) & 0x1U) > 0U)
        {
            if (device->devStateInfo.gpioPinMode[gpioIdx] != ADI_FPGAGEN6_GPIO_MODE_NONE)
            {
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        pinMask,
                                        "Invalid pinmask; one or more GPIOs already in use");
                return recoveryAction;
            }
        }
    }

    if (gpioIdx == ADI_FPGAGEN6_GPIO_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCfgSet(  adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_GpIntCfg_t* const    config)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t tmpBit = 0U;
    uint8_t bfEdgeLevel = 0U;
    uint8_t bfPolarity = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, config, cleanup);
    
    /* Range check config parameters */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; idx++)
    {
        if ((config->gpIntPin[idx].detectionType != ADI_FPGAGEN6_GPINT_DETECT_TYPE_LEVEL) &&
            (config->gpIntPin[idx].detectionType != ADI_FPGAGEN6_GPINT_DETECT_TYPE_EDGE))
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpIntPin[idx].detectionType, "Invalid detection type selected in FPGA GP Interrupt configuration.");
            goto cleanup;
        }
        
        if ((config->gpIntPin[idx].detectionPolarity != ADI_FPGAGEN6_GPINT_DETECT_POLARITY_LOW) &&
            (config->gpIntPin[idx].detectionPolarity != ADI_FPGAGEN6_GPINT_DETECT_POLARITY_HIGH))
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpIntPin[idx].detectionType, "Invalid detection type selected in FPGA GP Interrupt configuration.");
            goto cleanup;
        }
    }
    
    /* Construct bitfield values from config struct */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; ++idx)
    {
        tmpBit = (uint8_t)config->gpIntPin[idx].detectionType;
        tmpBit <<= idx;
        bfEdgeLevel |= tmpBit;

        tmpBit = (uint8_t)config->gpIntPin[idx].detectionPolarity;
        tmpBit <<= idx;
        bfPolarity |= tmpBit;
    }

    /* Set configuration bitfields */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetEdgeLevel_BfSet( device,
                                                                          FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                          bfEdgeLevel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetPolarity_BfSet( device, 
                                                                         FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                         bfPolarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCfgGet(  adi_fpgagen6_Device_t* const        device,
                                                            adi_fpgagen6_GpIntCfg_t* const      config)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t tmpBit = 0U;
    uint8_t bfEdgeLevel = 0U;
    uint8_t bfPolarity = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, config, cleanup);

    /* Readback configuration bitfields */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetEdgeLevel_BfGet(  device,
                                                                           FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                          &bfEdgeLevel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetPolarity_BfGet(  device,
                                                                          FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                         &bfPolarity);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading GP Interrupt Detection Polarity bitfield");
        goto cleanup;
    }

    /* Store results in config struct */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; ++idx)
    {
        tmpBit = bfEdgeLevel;
        tmpBit >>= idx;
        tmpBit &= 0x1U;
        config->gpIntPin[idx].detectionType = (adi_fpgagen6_GpIntDetectionType_e)tmpBit;

        tmpBit = bfPolarity;
        tmpBit >>= idx;
        tmpBit &= 0x1U;
        config->gpIntPin[idx].detectionPolarity = (adi_fpgagen6_GpIntDetectionPolarity_e)tmpBit;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntStatusGet(   adi_fpgagen6_Device_t* const                device,
                                                                adi_fpgagen6_GpIntStatus_t* const           statusRead)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t tmpBit = 0U;
    uint8_t bfStatus = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, statusRead, cleanup);

    /* Readback status bitfield */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterrupt_BfGet(  device,
                                                               FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                              &bfStatus);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Store results in status struct */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; idx++)
    {
        tmpBit = bfStatus;
        tmpBit >>= idx;
        tmpBit &= 0x1U;
        statusRead->gpIntPin[idx] = tmpBit;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntStatusClear( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_GpIntStatus_t* const     statusClear)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t tmpBit = 0U;
    uint8_t bfClear = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, statusClear, cleanup);
    
    /* Range check statusClear parameters */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; idx++)
    {
        if ((statusClear->gpIntPin[idx] != ADI_FALSE) &&
            (statusClear->gpIntPin[idx] != ADI_TRUE))
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, statusClear->gpIntPin[idx], "Invalid status-clear selected for FPGA GP Interrupt. Must be 0 or 1.");
            goto cleanup;
        }
    }
    
    /* Construct bitfield value from statusClear struct */
    for (idx = 0U; idx < ADI_FPGAGEN6_NUM_GPINT_PINS; idx++)
    {
        tmpBit = statusClear->gpIntPin[idx];
        tmpBit <<= idx;
        bfClear |= tmpBit;
    }
    
    /* "Write 1 to Clear" (W1C) bitfield */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterrupt_BfSet( device,
                                                              FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                              bfClear);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_CLEAR,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCounterGet(  adi_fpgagen6_Device_t* const                device,
                                                                const uint8_t                               gpIntPinSelect,
                                                                uint32_t* const                             counter)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    ADI_FPGAGEN6_API_ENTRY(&device->common);
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, counter, cleanup);

    /* Range check gpIntPinSelect value */
    if (gpIntPinSelect >= ADI_FPGAGEN6_NUM_GPINT_PINS)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gpIntPinSelect, "Invalid gpIntPinSelect; Valid GP Interrupt pins are 0 or 1.");
        goto cleanup;
    }

    /* Readback the counter for the selected GP Interrupt pin */
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetCounter_BfGet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_GPIO,
                                                                        gpIntPinSelect, 
                                                                        counter);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_COUNTER_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerSet(adi_fpgagen6_Device_t* const   device,
                                                                   const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                   const adi_fpgagen6_GpioPinSel_e  gpioPin)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    {
        uint32_t pinSelMask = 1 << gpioPin;
        adi_fpgagen6_Ads10v1SmaPinCfg_t smaPinCfg; /* Initialization below */
    
        ADI_LIBRARY_MEMSET(&smaPinCfg, 0, sizeof(adi_fpgagen6_Ads10v1SmaPinCfg_t));
        
        /* Set GPIO pin to input direction. It also checking for GPIO pin availability */
        recoveryAction = adi_fpgagen6_GpioInputDirSet(device, pinSelMask);
        
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)   
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   pinSelMask,
                                   "Failed to set GPIO Input direction.");
            goto cleanup;       
        }
    
        device->devStateInfo.gpioPinMode[(uint32_t)gpioPin] = ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER;
    
        if (smaPin > ADI_FPGAGEN6_GPIO_SMA_PIN_3)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   smaPin,
                                   "Invalid GPIO SMA pin selected");
            goto cleanup;
        }
    
        if (gpioPin >= ADI_FPGAGEN6_GPIO_INVALID)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   gpioPin,
                                   "Invalid GPIO selected");
            goto cleanup;
        }
    
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelExtTrigger_BfSet(device,
                                                                          FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                          smaPin,
                                                                          gpioPin);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_SEL_EXT_TRIGGER_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    
        /* Setting SMA Pin to output direction */
        smaPinCfg.smaPin[smaPin] = ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_GPIO_SRC;
        recoveryAction = adi_fpgagen6_SmaPinCfgSet(device, &smaPinCfg);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(   &device->common,
                             ADI_FPGAGEN6_ERRSRC_GPIO,
                             ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT,
                             recoveryAction,
                             smaPinCfg.smaPin[smaPin],
                             "Error while setting SMA Pin configuration");
            goto cleanup;
        }
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}
 
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                   const adi_fpgagen6_GpioSmaPin_e    smaPin,
                                                                   adi_fpgagen6_GpioPinSel_e * const  gpioPin)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioPin, cleanup);
    

    
    {
        uint8_t bf_value = 0U;
        
        if (smaPin > ADI_FPGAGEN6_GPIO_SMA_PIN_3)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   smaPin,
                                   "Invalid GPIO SMA pin selected");
            goto cleanup;
        }
    
        recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelExtTrigger_BfGet(device,
                                                                          FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                          smaPin,
                                                                          &bf_value);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                    ADI_FPGAGEN6_ERRCODE_GPIO_SEL_EXT_TRIGGER_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }      
        
        *gpioPin = (adi_fpgagen6_GpioPinSel_e) bf_value;
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerClear(adi_fpgagen6_Device_t* const   device,
                                                                     const uint8_t                  smaPinMask)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    {
        adi_fpgagen6_GpioPinSel_e gpioPin = ADI_FPGAGEN6_GPIO_INVALID;
        uint32_t gpioPinSel = 0U;
        uint8_t smaPinIdx = 0U;
        uint8_t needToFreeSmaPin = ADI_FALSE;
        uint8_t SMA_VALID_PINS = (1U << ADI_FPGAGEN6_GPIO_SMA_PIN_0) |
                                 (1U << ADI_FPGAGEN6_GPIO_SMA_PIN_1) |
                                 (1U << ADI_FPGAGEN6_GPIO_SMA_PIN_2) |
                                 (1U << ADI_FPGAGEN6_GPIO_SMA_PIN_3);
        adi_fpgagen6_Ads10v1SmaPinCfg_t smaPinCfgToBeFreed; /* Initialization below */
        
        ADI_LIBRARY_MEMSET(&smaPinCfgToBeFreed, ADI_FPGAGEN6_AXI_ADS10V1_SMA_NO_SELECTION, sizeof(adi_fpgagen6_Ads10v1SmaPinCfg_t));
        
        if ((smaPinMask & ~SMA_VALID_PINS) != 0)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   smaPinMask,
                                   "Invalid SMA pin mask.");
            goto cleanup;             
        }
        
        for (smaPinIdx = 0; smaPinIdx <= ADI_FPGAGEN6_GPIO_SMA_PIN_3; smaPinIdx++)
        {
            if ((smaPinMask & (1U << smaPinIdx)) == 0U)
            {
                continue; /* skip unselected sma pin */
            }
                       
            /* Mark to release the SMA Pin */
            smaPinCfgToBeFreed.smaPin[smaPinIdx] = ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TDD_FSM_EXT;
            needToFreeSmaPin = ADI_TRUE;
            
            /* Retrieve GPIO Pin associated with the selected sma pin */
            recoveryAction = adi_fpgagen6_GpioSelExtTriggerGet(device,
                                                               (adi_fpgagen6_GpioSmaPin_e )smaPinIdx,
                                                               &gpioPin);
            if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)   
            {
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       gpioPin,
                                       "Failed to get GPIO Pin associated with the selected SMA pin.");
                goto cleanup;       
            }
            
            /* Free the GPIO Pin */
            if (device->devStateInfo.gpioPinMode[gpioPin] == ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER)
            {
                /* Mark the GPIO pin as unused */
                device->devStateInfo.gpioPinMode[gpioPin] = ADI_FPGAGEN6_GPIO_MODE_NONE;
    
                /* Set GPIO pin to input direction in case it was set to output */
                gpioPinSel = 1 << gpioPin;
                recoveryAction = adi_fpgagen6_GpioInputDirSet(device, gpioPinSel);     
                if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)   
                {
                    recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT(&device->common,
                                           recoveryAction,
                                           gpioPinSel,
                                           "Failed to set GPIO Input direction.");
                    goto cleanup;       
                }
            }
        }
        
        if (needToFreeSmaPin == ADI_TRUE)
        {
            /* Release those SMA Pins */
            recoveryAction = adi_fpgagen6_SmaPinCfgSet(device, &smaPinCfgToBeFreed);
            if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_FPGAGEN6_ERRSRC_GPIO,
                                 ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_INPUT,
                                 recoveryAction,
                                 needToFreeSmaPin,
                                 "Error while freeing SMA Pin ext trigger");
                goto cleanup;
            }   
        }
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelRxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                    const adi_fpgagen6_RxDatapath_e  rxDataPath,
                                                                    const adi_fpgagen6_GpioPinSel_e  gpioPin)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    {
        uint32_t pinSelMask = 1 << gpioPin;
    
        /* Set GPIO pin to input direction. It also checking for GPIO pin availability */
        recoveryAction = adi_fpgagen6_GpioInputDirSet(device, pinSelMask);
        
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)   
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   pinSelMask,
                                   "Failed to set GPIO Input direction.");
            goto cleanup;       
        }
    }
    
    device->devStateInfo.gpioPinMode[(uint32_t)gpioPin] = ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER;
    
    if (rxDataPath > ADI_FPGAGEN6_RX_DATAPATH_2)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxDataPath,
                               "Invalid Rx Datapath selected");
        goto cleanup;
    }
    
    if (gpioPin >= ADI_FPGAGEN6_GPIO_INVALID)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gpioPin,
                               "Invalid GPIO selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelRxdpTrigger_BfSet(device,
                                                                       FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                       rxDataPath,
                                                                       gpioPin);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_SEL_RXDP_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelRxdpTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                    const adi_fpgagen6_RxDatapath_e    rxDataPath,
                                                                    adi_fpgagen6_GpioPinSel_e * const  gpioPin)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioPin, cleanup);


    
    if (rxDataPath > ADI_FPGAGEN6_RX_DATAPATH_2)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxDataPath,
                               "Invalid Rx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelRxdpTrigger_BfGet(device,
                                                                       FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                       rxDataPath,
                                                                       (uint8_t *) gpioPin);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_SEL_RXDP_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelTxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                    const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                    const adi_fpgagen6_GpioPinSel_e  gpioPin)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);


    
    uint32_t pinSelMask = 1 << gpioPin;
    
    /* Set GPIO pin to input direction. It also checking for GPIO pin availability */
    recoveryAction = adi_fpgagen6_GpioInputDirSet(device, pinSelMask);
        
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)   
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               pinSelMask,
                               "Failed to set GPIO Input direction.");
        goto cleanup;       
    }
    
    device->devStateInfo.gpioPinMode[(uint32_t)gpioPin] = ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER;
    
    if (txDataPath > ADI_FPGAGEN6_TX_DATAPATH_1)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDataPath,
                               "Invalid Tx Datapath selected");
        goto cleanup;
    }
    
    if (gpioPin >= ADI_FPGAGEN6_GPIO_INVALID)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gpioPin,
                               "Invalid GPIO selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelTxdpTrigger_BfSet(device,
                                                                       FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                       txDataPath,
                                                                       gpioPin);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_SEL_TXDP_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelTxdpTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                    const adi_fpgagen6_TxDatapath_e    txDataPath,
                                                                    adi_fpgagen6_GpioPinSel_e * const  gpioPin)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, gpioPin, cleanup);
 

    
    if (txDataPath > ADI_FPGAGEN6_TX_DATAPATH_1)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDataPath,
                               "Invalid Tx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioSelTxdpTrigger_BfGet(device,
                                                                       FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                       txDataPath,
                                                                       (uint8_t *) gpioPin);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_SEL_TXDP_TRIGGER_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayExtTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                     const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                     const uint16_t                   delayExtTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    if (smaPin > ADI_FPGAGEN6_GPIO_SMA_PIN_3)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               smaPin,
                               "Invalid GPIO SMA pin selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayExtTrigger_BfSet(device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                        smaPin,
                                                                        delayExtTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_EXT_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayExtTriggerGet(adi_fpgagen6_Device_t* const   device,
                                                                     const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                     uint16_t * const  delayExtTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, delayExtTrigger, cleanup);

    
    if (smaPin > ADI_FPGAGEN6_GPIO_SMA_PIN_3)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               smaPin,
                               "Invalid GPIO SMA pin selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayExtTrigger_BfGet(device,
                                                                        FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                        smaPin,
                                                                        delayExtTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_EXT_TRIGGER_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayRxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_RxDatapath_e  rxDataPath,
                                                                      const uint16_t                   delayRxdpTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    if (rxDataPath > ADI_FPGAGEN6_RX_DATAPATH_2)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxDataPath,
                               "Invalid Rx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayRxdpTrigger_BfSet(device,
                                                                         FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                         rxDataPath,
                                                                         delayRxdpTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_RXDP_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayRxdpTriggerGet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_RxDatapath_e  rxDataPath,
                                                                      uint16_t * const                 delayRxdpTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, delayRxdpTrigger, cleanup);
   

    
    if (rxDataPath > ADI_FPGAGEN6_RX_DATAPATH_2)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxDataPath,
                               "Invalid Rx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayRxdpTrigger_BfGet(device,
                                                                         FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                         rxDataPath,
                                                                         delayRxdpTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_RXDP_TRIGGER_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayTxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                      const uint16_t                   delayTxdpTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
 
    ADI_FPGAGEN6_API_ENTRY(&device->common);

    
    if (txDataPath > ADI_FPGAGEN6_TX_DATAPATH_1)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDataPath,
                               "Invalid Tx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayTxdpTrigger_BfSet(device,
                                                                         FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                         txDataPath,
                                                                         delayTxdpTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_TXDP_TRIGGER_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayTxdpTriggerGet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                      uint16_t * const                 delayTxdpTrigger)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, delayTxdpTrigger, cleanup);

    
    if (txDataPath > ADI_FPGAGEN6_TX_DATAPATH_1)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDataPath,
                               "Invalid Tx Datapath selected");
        goto cleanup;
    }
    
    recoveryAction = fpgagen6_AxiAdrvgen6Gpio_GpioDelayTxdpTrigger_BfGet(device,
                                                                         FPGAGEN6_BF_ADRV904X_ADS10_KOROR_TOP_AXI_KOROR_0_AXI_KOROR_GPIO,
                                                                         txDataPath,
                                                                         delayTxdpTrigger);
    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_GPIO,
                                                ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_TXDP_TRIGGER_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


