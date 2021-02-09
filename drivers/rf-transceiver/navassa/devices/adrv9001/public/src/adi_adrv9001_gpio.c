/**
* \file
* \brief Contains gpio features related function implementation defined in
* adi_adrv9001_gpio.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adi_adrv9001_gpio.h"

/* ADI specific header files */
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_spi.h"

#include "adrv9001_init.h"
#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_gpio.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf.h"

/* Header files related to libraries */

/* System header files */


/*********************************************************************************************************/
int32_t adi_adrv9001_gpio_GpIntHandler(adi_adrv9001_Device_t *device, adi_adrv9001_gpIntStatus_t *gpIntStatus)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    static const uint32_t GPINT_MASK_ALL_INTERRUPTS = 0xFFFFFFFF;

    ADI_ENTRY_PTR_EXPECT(device, gpIntStatus);

    /* retrieve the general purpose interrupt bitfield value */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfGet(device, &gpIntStatus->gpIntSaveIrqMask);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* mask all general purpose interrupt IRQs */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfSet(device, GPINT_MASK_ALL_INTERRUPTS);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve general purpose interrupt status word */
    recoveryAction = adrv9001_GpInterruptsStatusWordBfGet(device, &gpIntStatus->gpIntStatus);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve the general purpose interrupt IRQ mask */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfGet(device, &gpIntStatus->gpIntMask);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Mask for active sources */
    gpIntStatus->gpIntActiveSources = (gpIntStatus->gpIntStatus & ~gpIntStatus->gpIntMask);

    /* call the gp handler */
    recoveryAction = adrv9001_GpIntHandler(device, gpIntStatus);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Reload the stored gpInt IRQ mask to its original value when this API was entered */
    /* mask all general purpose interrupt IRQs */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfSet(device, gpIntStatus->gpIntSaveIrqMask);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, recoveryAction, gpIntStatus, "Failure setting gpIntStatus");

    return device->common.error.newAction;
}

int32_t adi_adrv9001_gpio_GpIntMask_Set(adi_adrv9001_Device_t *device, adi_adrv9001_gpMaskSelect_e maskSelect, adi_adrv9001_gpMaskArray_t *maskArray)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_ENTRY_PTR_EXPECT(device, maskArray);

    /* range check channel enum */
    if (maskSelect != ADI_ADRV9001_GPINT)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         maskSelect,
                         "Channel provided is out of range");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* retrieve the general purpose interrupt Pin IRQ mask */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfSet(device, maskArray->gpIntMask);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     maskArray,
                    "Error while trying to set adrv9001_GpInterruptsMaskPin_Set");

    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_GpIntMask_Get(adi_adrv9001_Device_t *device, adi_adrv9001_gpMaskSelect_e maskSelect, adi_adrv9001_gpMaskArray_t *maskArray)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_ENTRY_PTR_EXPECT(device, maskArray);

    /* range check channel enum */
    if (maskSelect != ADI_ADRV9001_GPINT)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         maskSelect,
                         "Channel provided is out of range");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* retrieve the general purpose interrupt Pin IRQ mask */
    recoveryAction = adrv9001_GpInterruptsMaskPinBfGet(device, &maskArray->gpIntMask);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     maskArray,
                     "Error while trying to get adrv9001_GpInterruptsMaskPin_Get");

    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_GpIntStatus_Get(adi_adrv9001_Device_t *device, uint32_t *gpIntStatus)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_ENTRY_PTR_EXPECT(device, gpIntStatus);

    /* retrieve the general purpose interrupt Pin IRQ mask */
    recoveryAction = adrv9001_GpInterruptsStatusWordBfGet(device, gpIntStatus);

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpIntStatus,
                     "Error while trying to get GP Int Status - Silicon A");

    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_OutputPinLevel_Set(adi_adrv9001_Device_t *device,
                                             adi_adrv9001_GpioPin_e pin,
                                             adi_adrv9001_GpioPinLevel_e level)
{
    uint16_t gpioLevels = 0;

    /* Check device pointer is not null */
    ADI_ENTRY_EXPECT(device);

    if (ADI_ADRV9001_GPIO_DIGITAL_00 <= pin && pin <= ADI_ADRV9001_GPIO_DIGITAL_15)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioSpiSource_Get, device, &gpioLevels);

        if (level == ADI_ADRV9001_GPIO_PIN_LEVEL_LOW)
        {
            gpioLevels &= ~(1 << (pin - 1));
        }
        else if (level == ADI_ADRV9001_GPIO_PIN_LEVEL_HIGH)
        {
            gpioLevels |= 1 << (pin - 1);
        }
        else
        {
            ADI_SHOULD_NOT_EXECUTE(device);
        }

        ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioSpiSource_Set, device, gpioLevels);
    }
    else if (ADI_ADRV9001_GPIO_ANALOG_00 <= pin && pin <= ADI_ADRV9001_GPIO_ANALOG_11)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogSpiSource_Get, device, &gpioLevels);

        if (level == ADI_ADRV9001_GPIO_PIN_LEVEL_LOW)
        {
            gpioLevels &= ~(1 << (pin - ADI_ADRV9001_GPIO_ANALOG_00));
        }
        else if (level == ADI_ADRV9001_GPIO_PIN_LEVEL_HIGH)
        {
            gpioLevels |= 1 << (pin - ADI_ADRV9001_GPIO_ANALOG_00);
        }
        else
        {
            ADI_SHOULD_NOT_EXECUTE(device);
        }

        ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogSpiSource_Set, device, gpioLevels);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_OutputPinLevel_Get(adi_adrv9001_Device_t *device,
                                             adi_adrv9001_GpioPin_e pin,
                                             adi_adrv9001_GpioPinLevel_e *gpioOutPinLevel)
{
    uint16_t pinLevels = 0;
    uint16_t pinLevelBitfield = 0;

    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, gpioOutPinLevel);

    if (ADI_ADRV9001_GPIO_DIGITAL_00 <= pin && pin <= ADI_ADRV9001_GPIO_DIGITAL_15)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioSpiSource_Get, device, &pinLevels);
        pinLevelBitfield = pinLevels & (1 << (pin - 1));
        *gpioOutPinLevel = pinLevelBitfield >> (pin - 1);
    }
    else if (ADI_ADRV9001_GPIO_ANALOG_00 <= pin && pin <= ADI_ADRV9001_GPIO_ANALOG_11)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogSpiSource_Get, device, &pinLevels);
        pinLevelBitfield = pinLevels & (1 << (pin - ADI_ADRV9001_GPIO_ANALOG_00));
        *gpioOutPinLevel = pinLevelBitfield >> (pin - ADI_ADRV9001_GPIO_ANALOG_00);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_InputPinLevel_Get(adi_adrv9001_Device_t *device,
                                            adi_adrv9001_GpioPin_e pin,
                                            adi_adrv9001_GpioPinLevel_e *gpioInPinLevel)
{
    uint16_t pinLevels = 0;

    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, gpioInPinLevel);


    if (ADI_ADRV9001_GPIO_DIGITAL_00 <= pin && pin <= ADI_ADRV9001_GPIO_DIGITAL_15)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioSpiRead_Get, device, &pinLevels);
        *gpioInPinLevel = (pinLevels & (1 << (pin - 1))) >> (pin - 1);
    }
    else if (ADI_ADRV9001_GPIO_ANALOG_00 <= pin && pin <= ADI_ADRV9001_GPIO_ANALOG_11)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogSpiRead_Get, device, &pinLevels);
        *gpioInPinLevel = (pinLevels & (1 << (pin - ADI_ADRV9001_GPIO_ANALOG_00))) >> (pin - ADI_ADRV9001_GPIO_ANALOG_00);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_gpio_ManualInput_Configure_Validate(adi_adrv9001_Device_t *device,
									       adi_adrv9001_GpioPin_e pin)
{
    ADI_RANGE_CHECK(device, pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_ManualInput_Configure(adi_adrv9001_Device_t *device, adi_adrv9001_GpioPin_e pin)
{
    uint16_t gpioOutEn = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_gpio_ManualInput_Configure_Validate, device, pin);

    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn &= ~(1 << (pin - 1));
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_gpio_ManualOutput_Configure_Validate(adi_adrv9001_Device_t *device,
										adi_adrv9001_GpioPinCrumbSel_e crumb)
{
    ADI_RANGE_CHECK(device, crumb, ADI_ADRV9001_GPIO_PIN_CRUMB_01_00, ADI_ADRV9001_GPIO_PIN_CRUMB_15_14);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_ManualOutput_Configure(adi_adrv9001_Device_t *device, adi_adrv9001_GpioPinCrumbSel_e crumb)
{
    uint16_t gpioOutEn = 0;
    static const uint8_t GPIO_SOURCE_SEL_ADDR = 0x56;
    static const uint8_t CRUMB_SOURCE_OFFSET = 7;   /* Map crumb enum value to SPI source crumb */

    ADI_PERFORM_VALIDATION(adi_adrv9001_gpio_ManualOutput_Configure_Validate, device, crumb);

    /* Configure pins as outputs */
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn |= (1 << (crumb * 2 - 1)) | (1 << (crumb * 2 - 2));
    ADI_EXPECT(adrv9001_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);

    /* Configure source */
    ADRV9001_SPIWRITEBYTE(device, "GPIO_SOURCE_SEL", (GPIO_SOURCE_SEL_ADDR + crumb - 1), crumb + CRUMB_SOURCE_OFFSET);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_gpio_ManualAnalogInput_Configure_Validate(adi_adrv9001_Device_t *device,
										     adi_adrv9001_GpioPin_e pin)
{
    ADI_RANGE_CHECK(device, pin, ADI_ADRV9001_GPIO_ANALOG_00, ADI_ADRV9001_GPIO_ANALOG_11);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_ManualAnalogInput_Configure(adi_adrv9001_Device_t *device,
                                                      adi_adrv9001_GpioPin_e pin)
{
    uint16_t gpioOutEn = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_gpio_ManualAnalogInput_Configure_Validate, device, pin);

    ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogDirectionControlOe_Get, device, &gpioOutEn);

    gpioOutEn &= ~(1 << (pin - ADI_ADRV9001_GPIO_ANALOG_00));

    ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogDirectionControlOe_Set, device, gpioOutEn);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_ManualAnalogOutput_Configure(adi_adrv9001_Device_t *device,
                                                       adi_adrv9001_GpioAnalogPinNibbleSel_e nibble)
{
    uint16_t gpioOutEn = 0;
    static const uint16_t ANALOG_GPIO_SOURCE_SEL_ADDR = 0x5C4;

    ADI_ENTRY_EXPECT(device);

    /* Configure pins as outputs */
    ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn |= (1 << (nibble * 4 - 1)) | (1 << (nibble * 4 - 2)) | (1 << (nibble * 4 - 3)) | (1 << (nibble * 4 - 4));
    ADI_EXPECT(adrv9001_NvsRegmapCore1_NvsGpioAnalogDirectionControlOe_Set, device, gpioOutEn);

    /* Configure source */
    ADRV9001_SPIWRITEBYTE(device, "ANALOG_GPIO_SOURCE_SEL", (ANALOG_GPIO_SOURCE_SEL_ADDR + nibble - 1), nibble - 1);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_ControlInit_Configure(adi_adrv9001_Device_t *adrv9001,
                                                adi_adrv9001_GpioCtrlInitCfg_t *initCfg)
{
    /* Check adrv9001 pointer is not null */
    ADI_ENTRY_EXPECT(adrv9001);

    /* FIXME: JS: Determine if mailbox allows unassigned, reorder pin enum, and remove these if's */
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->tx_ext_frontend_ctrl[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_TX1_EXT_FRONTEND_CONTROL, &initCfg->tx_ext_frontend_ctrl[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->tx_ext_frontend_ctrl[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_TX2_EXT_FRONTEND_CONTROL, &initCfg->tx_ext_frontend_ctrl[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->rx_ext_frontend_ctrl[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_RX1_EXT_FRONTEND_CONTROL, &initCfg->rx_ext_frontend_ctrl[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->rx_ext_frontend_ctrl[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_RX2_EXT_FRONTEND_CONTROL, &initCfg->rx_ext_frontend_ctrl[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->ext_pll_chip_enable[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_1_CE, &initCfg->ext_pll_chip_enable[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->ext_pll_chip_enable[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_2_CE, &initCfg->ext_pll_chip_enable[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->vco_chip_enable[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_RX_VCO_1_CE, &initCfg->vco_chip_enable[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->vco_chip_enable[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_RX_VCO_2_CE, &initCfg->vco_chip_enable[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->ext_pll_lock[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_1_LOCK, &initCfg->ext_pll_lock[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->ext_pll_lock[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_EXT_PLL_2_LOCK, &initCfg->ext_pll_lock[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->channelPowerSaving[0].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL1, &initCfg->channelPowerSaving[0]);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->channelPowerSaving[1].pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL2, &initCfg->channelPowerSaving[1]);
    }

    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->systemPowerSavingAndMonitorEnable.pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_MON_ENABLE_SPS, &initCfg->systemPowerSavingAndMonitorEnable);
    }
    if (ADI_ADRV9001_GPIO_UNASSIGNED != initCfg->systemPowerSavingAndMonitorWakeUp.pin)
    {
        ADI_EXPECT(adi_adrv9001_gpio_Configure, adrv9001, ADI_ADRV9001_GPIO_SIGNAL_MON_WAKEUP, &initCfg->systemPowerSavingAndMonitorWakeUp);
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_gpio_Configure_Validate(adi_adrv9001_Device_t *device,
								   adi_adrv9001_GpioSignal_e signal,
								   adi_adrv9001_GpioCfg_t *gpioConfig)
{
    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, gpioConfig);

    /*Check that GPIO signal is valid*/
    ADI_RANGE_CHECK(device, signal, 0x00, ADI_ADRV9001_GPIO_NUM_SIGNALS - 1);

    /*Check that GPIO pin is valid*/
    ADI_RANGE_CHECK(device, gpioConfig->pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_ANALOG_11);

    /*Check that GPIO control is valid*/
    switch (gpioConfig->master)
    {
    case ADI_ADRV9001_GPIO_MASTER_BBIC:     /* Falls through */
    case ADI_ADRV9001_GPIO_MASTER_ADRV9001:
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armgpioConfig->control,
                         "Invalid parameter value. armgpioConfig->control must be either ADI_ADRV9001_GPIO_MASTER_BBIC"
                         " or ADI_ADRV9001_GPIO_MASTER_ADRV9001");
    }

    /*Check that GPIO polarity is valid*/
    ADI_RANGE_CHECK(device, gpioConfig->polarity, ADI_ADRV9001_GPIO_POLARITY_NORMAL, ADI_ADRV9001_GPIO_POLARITY_INVERTED);

    ADI_API_RETURN(device);
}

static const uint8_t MAILBOX_GPIO_UNASSIGNED = 28;

static uint8_t adrv9001_gpio_PinToMailbox_Convert(adi_adrv9001_GpioPin_e pin)
{
    if (ADI_ADRV9001_GPIO_UNASSIGNED == pin)
    {
        return MAILBOX_GPIO_UNASSIGNED;
    }
    return pin - 1;
}

static adi_adrv9001_GpioPin_e adrv9001_gpio_MailboxToPin_Convert(uint8_t mailboxPin)
{
    if (MAILBOX_GPIO_UNASSIGNED == mailboxPin)
    {
        return ADI_ADRV9001_GPIO_UNASSIGNED;
    }
    return (adi_adrv9001_GpioPin_e)(mailboxPin + 1);
}

int32_t adi_adrv9001_gpio_Configure(adi_adrv9001_Device_t *device,
                                    adi_adrv9001_GpioSignal_e signal,
                                    adi_adrv9001_GpioCfg_t *gpioConfig)
{
    static const uint8_t GPIO_ENABLE = 0x04;

    uint8_t extData[5] = { 0 };
    uint8_t cmdStatusByte = 0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_PERFORM_VALIDATION(adi_adrv9001_gpio_Configure_Validate, device, signal, gpioConfig);

    /* Command ARM to associate the currently assigned GPIO for the requested signal ID */
    extData[0] = 0;
    extData[1] = ADRV9001_ARM_OBJECTID_GPIO_CTRL;
    extData[2] = signal;
    extData[3] = adrv9001_gpio_PinToMailbox_Convert(gpioConfig->pin);
    extData[4] = GPIO_ENABLE | gpioConfig->master | gpioConfig->polarity;

    recoveryAction = adi_adrv9001_arm_Cmd_Write(device, ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9001_arm_CmdStatus_Wait(device,
                                                     ADRV9001_ARM_SET_OPCODE,
                                                     &cmdStatusByte,
                                                     ADI_ADRV9001_SETARMGPIO_TIMEOUT_US,
                                                     ADI_ADRV9001_SETARMGPIO_INTERVAL_US);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        /* If cmdStatusByte is non-zero then flag an ARM error and release the acquired shared resource */
        if ((cmdStatusByte >> 1) > 0)
        {
            ADI_EXPECT(adrv9001_ArmCmdErrorHandler,
                        device,
                        ADRV9001_ARMCMD_ERRCODE(ADRV9001_ARM_SET_OPCODE, extData[0], cmdStatusByte));
        }
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_gpio_Inspect_Validate(adi_adrv9001_Device_t *device,
								 adi_adrv9001_GpioSignal_e gpioSignalSel,
								 adi_adrv9001_GpioCfg_t *gpioConfig)
{
    ADI_RANGE_CHECK(device, gpioSignalSel, 0x00, ADI_ADRV9001_GPIO_NUM_SIGNALS - 1);
    ADI_NULL_PTR_RETURN(&device->common, gpioConfig);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_gpio_Inspect(adi_adrv9001_Device_t *device,
                                  adi_adrv9001_GpioSignal_e signal,
                                  adi_adrv9001_GpioCfg_t *gpioConfig)
{
    static const uint8_t GPIO_CONTROL_SHIFT = 1;
    static const uint8_t GPIO_CONTROL_MASK  = 0x02;
    static const uint8_t GPIO_POLARITY_SHIFT    = 0;
    static const uint8_t GPIO_POLARITY_MASK = 0x01;

    uint8_t extData[3] = { 0 };
    uint8_t armData[2] = { 0 };

    /* Check device pointer is not null */
    ADI_PERFORM_VALIDATION(adi_adrv9001_gpio_Inspect_Validate, device, signal, gpioConfig);

    /* Command ARM to return the currently assigned GPIO for the requested signal ID */
    extData[0] = 0;
    extData[1] = ADRV9001_ARM_OBJECTID_GPIO_CTRL;
    extData[2] = signal;
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        ADI_ADRV9001_GETARMGPIO_TIMEOUT_US,
                                        ADI_ADRV9001_GETARMGPIO_INTERVAL_US);

    /* Read GPIO Pin sel from ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armData[0],
               sizeof(armData),
               false);

    /* Return the GPIO data read back from ARM mailbox */
    gpioConfig->pin = adrv9001_gpio_MailboxToPin_Convert(armData[0]);
    gpioConfig->master = (adi_adrv9001_GpioMaster_e)((armData[1] & GPIO_CONTROL_MASK) >> GPIO_CONTROL_SHIFT);
    gpioConfig->polarity = (adi_adrv9001_GpioPolarity_e)((armData[1] & GPIO_POLARITY_MASK) >> GPIO_POLARITY_SHIFT);

    ADI_API_RETURN(device);
}

