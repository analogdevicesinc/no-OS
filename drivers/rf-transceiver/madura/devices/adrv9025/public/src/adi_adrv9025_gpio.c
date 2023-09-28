// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_gpio.c
* \brief Contains gpio features related function implementation defined in
* adi_adrv9025_gpio.h
*
* ADRV9025 API Version: 6.4.0.14
*/

/*
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "../../private/include/adrv9025_gpio.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_bf_tx.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "../../private/include/adrv9025_bf_analog_tx_mem_map.h"
#include "../../private/include/adrv9025_bf_orx.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_gpio.c"
#endif

int32_t adi_adrv9025_GpioOeGet(adi_adrv9025_Device_t* device,
                               uint32_t*              gpioOutEn)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check GPIO out en write back pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        gpioOutEn);

    recoveryAction = adrv9025_CoreGpioDirectionControlOeBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              gpioOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read GPIO Direction Control bitfields");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioInputDirSet(adi_adrv9025_Device_t* device,
                                     uint32_t               gpioInputMask)
{
    static const uint32_t GPIO_OE_MASK = 0x7FFFF;

    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t gpioOutEn      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that GPIO Input Mask is within range */
    if (gpioInputMask > GPIO_OE_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         gpioInputMask,
                         "GPIO input mask is out of range. Valid range 0x00000-0x7FFFF(GPIO0-GPIO18)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read the current status of GPIO direction control of ADRV9025 */
    recoveryAction = adi_adrv9025_GpioOeGet(device,
                                            &gpioOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error reading current ADRV9025 GPIO OE while attempting to set GPIO input mask");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* To set the GPIOs as input, 0 has to be written to the corresponding GPIO direction control bitfield */
    gpioOutEn &= GPIO_OE_MASK;
    gpioOutEn &= ~gpioInputMask;
    recoveryAction = adrv9025_CoreGpioDirectionControlOeBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              gpioOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpioInputMask,
                     "Error writing ADRV9025 GPIO OE while attempting to set GPIO input direction control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioAnalogInputDirSet(adi_adrv9025_Device_t* device,
                                           uint8_t                gpioAnalogInputMask)
{
    int32_t recoveryAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t gpioAnalogOutEn = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Read the current status of GPIO direction control of ADRV9025 */
    recoveryAction = adrv9025_CoreGpioAnalogDirectionControlOeBfGet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    &gpioAnalogOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error reading current ADRV9025 Analog GPIO OE while attempting to set GPIO_ANA input direction control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* To set the GPIOs as input, 0 has to be written to the corresponding GPIO direction control bitfield */
    gpioAnalogOutEn &= ~gpioAnalogInputMask;
    recoveryAction = adrv9025_CoreGpioAnalogDirectionControlOeBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    gpioAnalogOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpioAnalogInputMask,
                     "Error writing ADRV9025 Analog GPIO OE while attempting to set GPIO_ANA input direction control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioAnalogOutputDirSet(adi_adrv9025_Device_t* device,
                                            uint8_t                gpioAnalogOutputMask)
{
    int32_t recoveryAction  = ADI_COMMON_ACT_NO_ACTION;
    uint8_t gpioAnalogOutEn = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Read the current status of GPIO direction control of ADRV9025 */
    recoveryAction = adrv9025_CoreGpioAnalogDirectionControlOeBfGet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    &gpioAnalogOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error reading current ADRV9025 Analog GPIO OE while attempting to set GPIO_ANA output mask");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* To set the GPIOs as output, 1 has to be written to the corresponding GPIO direction control bitfield */
    gpioAnalogOutEn |= gpioAnalogOutputMask;
    recoveryAction = adrv9025_CoreGpioAnalogDirectionControlOeBfSet(device,
                                                                    ADRV9025_BF_CORE,
                                                                    gpioAnalogOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpio3v3OutputMask,
                     "Error writing ADRV9025 Analog GPIO OE while attempting to set GPIO_ANA output direction control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioOutputDirSet(adi_adrv9025_Device_t* device,
                                      uint32_t               gpioOutputMask)

{
    static const uint32_t GPIO_OE_MASK = 0x7FFFF;

    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t gpioOutEn      = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that GPIO Input Mask is within range */
    if (gpioOutputMask > GPIO_OE_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         gpioOutputMask,
                         "GPIO output mask is out of range. Valid range 0x00000-0x7FFFF(GPIO0-GPIO18)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read the current status of GPIO direction control of ADRV9025 */
    recoveryAction = adi_adrv9025_GpioOeGet(device,
                                            &gpioOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "Error reading current ADRV9025 GPIO OE while attempting to set GPIO input mask");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* To set the GPIOs as output, 1 has to be written to the corresponding GPIO direction control bitfield */
    gpioOutEn |= gpioOutputMask;

    recoveryAction = adrv9025_CoreGpioDirectionControlOeBfSet(device,
                                                              ADRV9025_BF_CORE,
                                                              gpioOutEn);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     recoveryAction,
                     recoveryAction,
                     gpioOutputMask,
                     "Error writing ADRV9025 GPIO OE while attempting to set GPIO output direction control");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_GpioOeSet(adi_adrv9025_Device_t* device,
                               uint32_t               gpioOutEn,
                               uint32_t               gpioUsedMask)
{
	  /* Check device pointer is not null */
	  ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);
    UNUSED_PARA(device);
    UNUSED_PARA(gpioOutEn);
    UNUSED_PARA(gpioUsedMask);
    return 0;
}

int32_t adi_adrv9025_GpioOutSourceCtrlSet(adi_adrv9025_Device_t* device,
                                          uint32_t               gpioSrcCtrl)
{
    static const uint32_t GPIO_SRC_MASK = 0xFFFFF;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t regData        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that GPIO Source Control is within range */
    if (gpioSrcCtrl > GPIO_SRC_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         gpioInputMask,
                         "GPIO output Source Control is out of range. Valid range 0x00000-0x7FFFF(GPIO0-GPIO18)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    regData        = (uint8_t)((gpioSrcCtrl >> 0) & 0x0000000F);
    recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfSet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (uint8_t)((gpioSrcCtrl >> 4) & 0x0000000F);
    recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (uint8_t)((gpioSrcCtrl >> 8) & 0x0000000F);
    recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (uint8_t)((gpioSrcCtrl >> 12) & 0x0000000F);
    recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (uint8_t)((gpioSrcCtrl >> 16) & 0x0000000F);
    recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfSet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  regData);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error from BF CODE");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpioOutSourceCtrlGet(adi_adrv9025_Device_t* device,
                                          uint32_t*              gpioSrcCtrl)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t regData[5]     = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpioSrcCtrl);

    recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfGet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             &regData[0]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             &regData[1]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             &regData[2]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                             ADRV9025_BF_CORE,
                                                                             &regData[3]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfGet(device,
                                                                  ADRV9025_BF_CORE,
                                                                  &regData[4]);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error from BF CODE");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* reconstructing byte reads into gpioSrcCtrl word */
    *gpioSrcCtrl = ((uint32_t)(regData[4] & 0x0F) << 16) | ((uint32_t)(regData[3] & 0x0F) << 12) |
    ((uint32_t)(regData[2] & 0x0F) << 8) | ((uint32_t)(regData[1] & 0x0F) << 4) | ((uint32_t)(regData[0] & 0x0F));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpioOutPinLevelSet(adi_adrv9025_Device_t* device,
                                        uint32_t               gpioOutPinLevel)
{
    static const uint32_t GPIO_PIN_MASK = 0x7FFFF;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that GPIO Output Mask is within range */
    if (gpioOutPinLevel > GPIO_PIN_MASK)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         gpioInputMask,
                         "GPIO output Pin Level is out of range. Valid range 0x00000-0x7FFFF(GPIO0-GPIO18)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_CoreGpioSpiSourceBfSet(device,
                                                     ADRV9025_BF_CORE,
                                                     gpioOutPinLevel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpioInPinLevel,
                     "Error writing ADRV9025 GPIO Output Pin Level");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (recoveryAction);
}

int32_t adi_adrv9025_GpioInputPinLevelGet(adi_adrv9025_Device_t* device,
                                          uint32_t*              gpioInPinLevel)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpioInPinLevel);

    recoveryAction = adrv9025_CoreGpioSpiReadBfGet(device,
                                                   ADRV9025_BF_CORE,
                                                   gpioInPinLevel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpioInPinLevel,
                     "Error reading ADRV9025 GPIO Input Pin Level");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (recoveryAction);
}

int32_t adi_adrv9025_GpioOutPinLevelGet(adi_adrv9025_Device_t* device,
                                        uint32_t*              gpioOutPinLevel)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpioOutPinLevel);

    recoveryAction = adrv9025_CoreGpioSpiSourceBfGet(device,
                                                     ADRV9025_BF_CORE,
                                                     gpioOutPinLevel);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpioOutPinLevel,
                     "Error reading GPIO Output Pin Level");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (recoveryAction);
}

int32_t adi_adrv9025_GpioMonitorOutSrcSet(adi_adrv9025_Device_t* device,
                                          uint8_t                monitorIndex,
                                          uint8_t                monitorMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t regData        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_CoreMonitorOutBfSet(device,
                                                  ADRV9025_BF_CORE,
                                                  monitorIndex);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error writing GPIO Monitor Index");
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask & 0x01);
    recoveryAction = adrv9025_CoreEnCtrl0BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 1) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl1BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 2) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl2BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 3) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl3BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 4) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl4BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 5) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl5BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 6) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl6BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    regData        = (monitorMask >> 7) & 0x01;
    recoveryAction = adrv9025_CoreEnCtrl7BfSet(device,
                                               ADRV9025_BF_CORE,
                                               regData);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (recoveryAction);
}

int32_t adi_adrv9025_GpioMonitorOutSrcGet(adi_adrv9025_Device_t* device,
                                          uint8_t*               monitorIndex,
                                          uint8_t*               monitorMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t regData[8]     = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        monitorIndex);

    ADI_NULL_PTR_RETURN(&device->common,
                        monitorMask);

    recoveryAction = adrv9025_CoreMonitorOutBfGet(device,
                                                  ADRV9025_BF_CORE,
                                                  monitorIndex);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error reading GPIO Monitor Index");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl0BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[0]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl1BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[1]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl2BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[2]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl3BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[3]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl4BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[4]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl5BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[5]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl6BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[6]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_CoreEnCtrl7BfGet(device,
                                               ADRV9025_BF_CORE,
                                               &regData[7]);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* reconstructing bit reads into monitorMask */
    *monitorMask = (((regData[7] & 0x01) << 7) | ((regData[6] & 0x01) << 6) |
        ((regData[5] & 0x01) << 5) | ((regData[4] & 0x01) << 4) |
        ((regData[3] & 0x01) << 3) | ((regData[2] & 0x01) << 2) |
        ((regData[1] & 0x01) << 1) | (regData[0] & 0x01));

    return (recoveryAction);
}

int32_t adi_adrv9025_GPIntClearStatusRegister(adi_adrv9025_Device_t* device)
{
    static const uint64_t STATUS_CLEAR_ALL = 0xFFFFFFFFFFFFFFFF;
    int32_t               recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint64_t              status           = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Clearing status register in hardware requires all sticky bits set to 1 to clear their corresponding status bit */
    /* First save current state of sticky mask reg */
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 &status);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set all bits to one to clear corresponding status register bit */
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 STATUS_CLEAR_ALL);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore sticky mask reg to what it was before clearing. */
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 status);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_GpInt0Handler(adi_adrv9025_Device_t*      device,
                                   adi_adrv9025_gpIntStatus_t* gpInt0Status)
{
    int32_t               recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    static const uint64_t GPINT_CLEAR_INTERRUPTS = 0x0000000000000000;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpInt0Status);

    /* block all gpInt1 (lower Priority) Interrupts during gpInt0 processing */
    /* retrieve the general purpose interrupt Pin0 IRQ mask */
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInt0Status->gp_Int_Save_Irq_Mask0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve the general purpose interrupt Pin1 IRQ mask */
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInt0Status->gp_Int_Save_Irq_Mask1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve status register sticky mask */
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 &gpInt0Status->gp_Int_StickyBit_Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve general purpose interrupt status word */
    recoveryAction = adrv9025_CoreGpInterruptsStatusWordBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              &gpInt0Status->gp_Interrupt_Status);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Clear all events as we process the status register (this also clears the relevant bits of the status register)*/
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 GPINT_CLEAR_INTERRUPTS);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Save current GP Int pin0 mask to the mask variable */
    gpInt0Status->gp_Int_Irq_Mask = gpInt0Status->gp_Int_Save_Irq_Mask0;

    /* Mask for active sources */
    gpInt0Status->gp_Int_Active_Sources = (gpInt0Status->gp_Interrupt_Status & ~gpInt0Status->gp_Int_Irq_Mask);

    /* call the gp handler */
    adrv9025_GpIntHandler(device,
                          gpInt0Status);

    return recoveryAction;
}

int32_t adi_adrv9025_GpInt1Handler(adi_adrv9025_Device_t*      device,
                                   adi_adrv9025_gpIntStatus_t* gpInt1Status)
{
    int32_t               recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    static const uint64_t GPINT_CLEAR_INTERRUPTS = 0x0000000000000000;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpInt1Status);

    /* block all gpInt0 (lower Priority) Interrupts during gpInt1 processing */
    /* retrieve the general purpose interrupt Pin0 IRQ mask */
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInt1Status->gp_Int_Save_Irq_Mask0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve the general purpose interrupt Pin1 IRQ mask */
    recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &gpInt1Status->gp_Int_Save_Irq_Mask1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve status register sticky mask */
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 &gpInt1Status->gp_Int_StickyBit_Mask);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* retrieve general purpose interrupt status word */
    recoveryAction = adrv9025_CoreGpInterruptsStatusWordBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              &gpInt1Status->gp_Interrupt_Status);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Clear all events as we process the status register (this also clears the relevant bits of the status register)*/
    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 GPINT_CLEAR_INTERRUPTS);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Save current GP Int pin1 mask to the mask variable */
    gpInt1Status->gp_Int_Irq_Mask = gpInt1Status->gp_Int_Save_Irq_Mask1;

    /* Mask for active sources */
    gpInt1Status->gp_Int_Active_Sources = (gpInt1Status->gp_Interrupt_Status & ~gpInt1Status->gp_Int_Irq_Mask);

    /* call the gp handler */
    adrv9025_GpIntHandler(device,
                          gpInt1Status);

    return recoveryAction;
}

int32_t adi_adrv9025_GpIntMaskSet(adi_adrv9025_Device_t*       device,
                                  adi_adrv9025_gpMaskSelect_e  maskSelect,
                                  adi_adrv9025_gp_MaskArray_t* maskArray)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        maskArray);

    /* range check channel enum */
    if ((maskSelect != ADI_ADRV9025_GPINT0) && (maskSelect != ADI_ADRV9025_GPINT1) && (maskSelect != ADI_ADRV9025_GPINTALL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         maskSelect,
                         "Channel provided is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((maskSelect == ADI_ADRV9025_GPINT0) || (maskSelect == ADI_ADRV9025_GPINTALL))
    {
        /* retrieve the general purpose interrupt Pin0 IRQ mask */
        recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                maskArray->gpInt0Mask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         maskArray,
                         "Error while trying to set CoreGpInterruptsMaskPin0 - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((maskSelect == ADI_ADRV9025_GPINT1) || (maskSelect == ADI_ADRV9025_GPINTALL))
    {
        /* retrieve the general purpose interrupt Pin1 IRQ mask */
        recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfSet(device,
                                                                ADRV9025_BF_CORE,
                                                                maskArray->gpInt1Mask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         maskArray,
                         "Error while trying to set CoreGpInterruptsMaskPin1 - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpIntMaskGet(adi_adrv9025_Device_t*       device,
                                  adi_adrv9025_gpMaskSelect_e  maskSelect,
                                  adi_adrv9025_gp_MaskArray_t* maskArray)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        maskArray);

    /* range check channel enum */
    if ((maskSelect != ADI_ADRV9025_GPINT0) && (maskSelect != ADI_ADRV9025_GPINT1) && (maskSelect != ADI_ADRV9025_GPINTALL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         maskSelect,
                         "Channel provided is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((maskSelect == ADI_ADRV9025_GPINT0) || (maskSelect == ADI_ADRV9025_GPINTALL))
    {
        /* retrieve the general purpose interrupt Pin0 IRQ mask */
        recoveryAction = adrv9025_CoreGpInterruptsMaskPin0BfGet(device,
                                                                ADRV9025_BF_CORE,
                                                                &maskArray->gpInt0Mask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         maskArray,
                         "Error while trying to get CoreGpInterruptsMaskPin0 - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((maskSelect == ADI_ADRV9025_GPINT1) || (maskSelect == ADI_ADRV9025_GPINTALL))
    {
        /* retrieve the general purpose interrupt Pin1 IRQ mask */
        recoveryAction = adrv9025_CoreGpInterruptsMaskPin1BfGet(device,
                                                                ADRV9025_BF_CORE,
                                                                &maskArray->gpInt1Mask);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         maskArray,
                         "Error while trying to get CoreGpInterruptsMaskPin1 - Silicon B");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpIntStatusGet(adi_adrv9025_Device_t* device,
                                    uint64_t*              gpIntStatus)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        gpIntStatus);

    /* retrieve the general purpose interrupt Pin0 IRQ mask */
    recoveryAction = adrv9025_CoreGpInterruptsStatusWordBfGet(device,
                                                              ADRV9025_BF_CORE,
                                                              gpIntStatus);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     gpIntStatus,
                     "Error while trying to get GP Int Status - Silicon B");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_GpIntStickyBitMaskSet(adi_adrv9025_Device_t* device,
                                           uint64_t               eventMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfSet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 eventMask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     eventMask,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_GpIntStickyBitMaskGet(adi_adrv9025_Device_t* device,
                                           uint64_t*              eventMask)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        eventMask);

    recoveryAction = adrv9025_CoreGpInterruptsStickyBitMaskBfGet(device,
                                                                 ADRV9025_BF_CORE,
                                                                 eventMask);
    ADI_ERROR_REPORT(&device->common,
                     device->common.error.errSource,
                     device->common.error.errCode,
                     device->common.error.newAction,
                     eventMask,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_TemperatureGet(adi_adrv9025_Device_t* device,
                                    int16_t*               temperatureDegC)
{
    static const uint8_t ARM_MEM_READ_AUTOINCR = 1;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armExtData[1]  = {ADRV9025_CPU_OBJECTID_TEMP_SENSOR};
    uint8_t cmdStatusByte  = 0;
    uint8_t armReadBack[2] = {0};
    int32_t errHdl         = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        temperatureDegC);

    /* Execute ARM command to retrieve temperature */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              ADRV9025_CPU_GET_OPCODE,
                                              &armExtData[0],
                                              sizeof(armExtData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_GETTEMPERATURE_TIMEOUT_US,
                                                   ADI_ADRV9025_GETTEMPERATURE_INTERVAL_US);

    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     errHdl,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              armExtData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read Temperature from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armReadBack[0],
                                             sizeof(armReadBack),
                                             ARM_MEM_READ_AUTOINCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *temperatureDegC = (int16_t)((uint16_t)armReadBack[0] | ((uint16_t)armReadBack[1] << 8));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_Spi2CfgSet(adi_adrv9025_Device_t* device,
                                uint8_t                spi2Enable)
{
    int32_t recoveryAction            = ADI_COMMON_ACT_NO_ACTION;
    uint8_t regData                   = 0;
    uint8_t currentSpi2En             = 0;
    uint8_t resourceAcquisitionStatus = ADI_FAILURE;
    uint8_t resourceReleaseStatus     = ADI_FAILURE;
#define NUM_GPIOS_IN_SPI2_CTRL_PIN_FEATURE 4U
    int32_t  sharedResourceArr[NUM_GPIOS_IN_SPI2_CTRL_PIN_FEATURE] = {0};
    uint32_t spi2GpioPinMask                                       = 0;
    uint8_t  sharedResourceCounter                                 = 0;

    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_SPI2_CTRL_PIN;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    adi_adrv9025_Spi2CfgGet(device,
                            &currentSpi2En);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Free current resource */
    if (currentSpi2En > 0)
    {
        /* Disable SPI2 */
        regData        = 0;
        recoveryAction = adrv9025_CoreSpi2EnBfSet(device,
                                                  ADRV9025_BF_CORE,
                                                  regData);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* GPIO 0/1/2/3 for SPI2 */
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_00;
        /* Release GPIO[1] if it is 4-wire mode */
        if (device->spiSettings.fourWireMode == 1)
        {
            sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_01;
        }
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_02;
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_03;

        recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         sharedResourceCounter,
                                                         THIS_FEATURE_ID,
                                                         &resourceReleaseStatus);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceReleaseStatus,
                             "Error while attempting to release GPIO shared resouce for GPIO SPI2 ctrl Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /*Set released GPIO pins as inputs since unused pins are supposed to be inputs */
        spi2GpioPinMask = 0x0000000F;
        recoveryAction  = adi_adrv9025_GpioInputDirSet(device,
                                                       spi2GpioPinMask);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (spi2Enable > 0)
    {
        sharedResourceCounter = 0;

        /* GPIO 0/1/2/3 for SPI2 */
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_00;
        if (device->spiSettings.fourWireMode == 1)
        {
            sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_01;
        }
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_02;
        sharedResourceArr[sharedResourceCounter++] = (uint32_t)ADI_ADRV9025_GPIO_03;

        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourceArr[0],
                                                         sharedResourceCounter,
                                                         THIS_FEATURE_ID,
                                                         &resourceAcquisitionStatus);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while attempting to acquire GPIO shared resouce for GPIO SPI2 Pins");
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (resourceAcquisitionStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             resourceAcquisitionStatus,
                             "Error while attempting to acquire GPIO shared resouce for GPIO SPI2 Pins");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Enable SPI2 */
        regData        = 1;
        recoveryAction = adrv9025_CoreSpi2EnBfSet(device,
                                                  ADRV9025_BF_CORE,
                                                  regData);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_Spi2CfgGet(adi_adrv9025_Device_t* device,
                                uint8_t*               spi2Enable)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        spi2Enable);

    /* Get current value of SPI2 */
    recoveryAction = adrv9025_CoreSpi2EnBfGet(device,
                                              ADRV9025_BF_CORE,
                                              spi2Enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_INV_PARAM,
                     recoveryAction,
                     NULL,
                     "Error from BF CODE");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_AuxDacCfgSet(adi_adrv9025_Device_t*   device,
                                  adi_adrv9025_AuxDacCfg_t auxDacConfig[],
                                  uint8_t                  numberOfCfg)
{
    static const adrv9025_FeatureID_e THIS_FEATURE_ID = ADRV9025_FEATURE_AUX_DAC_OUT;

    /* Variables to iterate over configurations to be set */
    uint8_t                i         = 0;
    uint8_t                k         = 0;
    adi_adrv9025_AuxDacs_e auxDacSel = ADI_ADRV9025_AUXDAC0;

    /* Variable to hold enable/disable configuration */
    uint8_t bfEnable = 0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Variables to handle shared GPIO sources */
    int32_t sharedResourceArr[ADI_ADRV9025_MAX_AUXDACS] = {0};
    uint8_t numSharedResources                          = 0;
    uint8_t resourceAcquisitionStatus                   = ADI_FAILURE;
    uint8_t resourceReleaseStatus                       = ADI_FAILURE;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacConfig);

    /* check that if number of configs is > 0 and <= 8 */
    if (numberOfCfg == 0 || numberOfCfg > 8)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numberOfCfg,
                         "Invalid number of configurations");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < numberOfCfg; i++)
    {
        /* Reset shared resource variable */
        numSharedResources = 0;

        /* Range check for all the values */
        if (adrv9025_AuxDacCfgRangeCheck(device,
                                         &auxDacConfig[i]) != ADI_COMMON_ACT_NO_ACTION)
        {
            /* Configuration is not valid */
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             auxDacConfig,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            for (k = 0; k < ADI_ADRV9025_MAX_AUXDACS; k++)
            {
                if (ADRV9025_BF_EQUAL(auxDacConfig[i].auxDacMask,
                                      ((uint32_t)1 << (uint32_t)k)))
                {
                    /* Add GPIO pin to shared resources array */
                    sharedResourceArr[numSharedResources++] = (int32_t)k;

                    /* Set AuxDAC configuration */
                    auxDacSel      = (adi_adrv9025_AuxDacs_e)((uint8_t)(1 << k));
                    recoveryAction = adrv9025_AuxDacCfgSet(device,
                                                           auxDacSel);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     device->common.error.errCode,
                                     recoveryAction,
                                     NULL,
                                     "Unable to set AuxDAC configuration");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            /* Read AuxDAC enable status for read/modify/write */
            recoveryAction = adrv9025_CorePdAuxdacBfGet(device,
                                                        ADRV9025_BF_CORE,
                                                        &bfEnable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while reading AuxDACs enabled bits");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Power up/down selected AuxDACs at this channel */
            bfEnable = bfEnable | auxDacConfig[i].auxDacMask;

            /*Try to Release Shared GPIOs */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                                             &sharedResourceArr[0],
                                                             numSharedResources,
                                                             THIS_FEATURE_ID,
                                                             &resourceReleaseStatus);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while releasing shared GPIO pins while disabling AuxDACs");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (resourceReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                                 ADI_ADRV9025_ACT_ERR_RESET_GPIO,
                                 resourceReleaseStatus,
                                 "Unable to release GPIOs for AuxDACs while disabling AuxDACs ");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Configure GPIO pin as input */
            recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                          auxDacConfig[i].auxDacMask);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Unable to set GPIO direction as input ");
            ADI_ERROR_RETURN(device->common.error.newAction);

            if (auxDacConfig[i].enable > 0)
            {
                bfEnable = bfEnable & (~auxDacConfig[i].auxDacMask);

                /* Try to acquire Shared Analog GPIOs */
                recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                                 ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                                                 &sharedResourceArr[0],
                                                                 numSharedResources,
                                                                 THIS_FEATURE_ID,
                                                                 &resourceAcquisitionStatus);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Error while acquiring shared GPIO pins while enabling AuxDACs");
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (resourceAcquisitionStatus == ADI_FAILURE)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_ADRV9025_ERR_SHARED_RESOURCE_ACQUIRE,
                                     ADI_ADRV9025_ACT_ERR_RESET_GPIO,
                                     resourceAcquisitionStatus,
                                     "Unable to acquire shared Analog GPIOs while enabling AuxDACs");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                /* Set GPIO analog pins as output */
                if ((recoveryAction = adi_adrv9025_GpioOutputDirSet(device,
                                                                    auxDacConfig[i].auxDacMask)) != ADI_COMMON_ACT_NO_ACTION)
                {
                    /*Clear the error so that clean up functions execute */
                    recoveryAction = adrv9025_ErrorClear(&device->common);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     device->common.error.errCode,
                                     recoveryAction,
                                     NULL,
                                     "Error while clearing error ");
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    /*Release Shared GPIOs if setting of GPIO Input Dir is unsuccessful */
                    recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                                     ADRV9025_SHARED_RESOURCE_GPIO_ANALOG,
                                                                     &sharedResourceArr[0],
                                                                     numSharedResources,
                                                                     THIS_FEATURE_ID,
                                                                     &resourceReleaseStatus);
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     device->common.error.errCode,
                                     recoveryAction,
                                     NULL,
                                     "Error while releasing shared GPIO pins ");
                    ADI_ERROR_RETURN(device->common.error.newAction);

                    if (resourceReleaseStatus == ADI_FAILURE)
                    {
                        ADI_ERROR_REPORT(&device->common,
                                         ADI_COMMON_ERRSRC_API,
                                         ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE,
                                         ADI_ADRV9025_ACT_ERR_RESET_GPIO,
                                         resourceReleaseStatus,
                                         "Unable to release GPIOs for AuxDACs while attempting to exit due to GPIO dir set error");
                        ADI_ERROR_RETURN(device->common.error.newAction);
                    }

                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_API_FAIL,
                                     ADI_ADRV9025_ACT_ERR_RESET_SPI,
                                     gpioAnalogInputOutputMask,
                                     "Unable to set GPIO output direction for AuxDACs");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }

            recoveryAction = adrv9025_CorePdAuxdacBfSet(device,
                                                        ADRV9025_BF_CORE,
                                                        bfEnable);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             NULL,
                             "Error while setting enable/disable for AuxDACs");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxDacCfgGet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_AuxDacs_e    auxDacMask,
                                  adi_adrv9025_AuxDacCfg_t* auxDacConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adi_adrv9025_AuxDacCfg_t auxDacConfigTmp = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacConfig);

    /* Check whether selected AuxDAC index is valid */
    if ((auxDacMask != ADI_ADRV9025_AUXDAC0) && (auxDacMask != ADI_ADRV9025_AUXDAC1) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC2) && (auxDacMask != ADI_ADRV9025_AUXDAC3) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC4) && (auxDacMask != ADI_ADRV9025_AUXDAC5) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC6) && (auxDacMask != ADI_ADRV9025_AUXDAC7))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxDacMask,
                         "Invalid AuxDAC index is selected for AuxDAC. Valid AuxDAC indexes are AuxDAC0-7.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Assign auxDacMask to auxDacMask input */
    auxDacConfigTmp.auxDacMask = (uint32_t)auxDacMask;

    /* Read enable bit for selected Tx Channel and AuxDAC */
    recoveryAction = adrv9025_CorePdAuxdacBfGet(device,
                                                ADRV9025_BF_CORE,
                                                &auxDacConfigTmp.enable);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while reading AuxDACs enabled bits");
    ADI_ERROR_RETURN(device->common.error.newAction);

    auxDacConfigTmp.enable = (~auxDacConfigTmp.enable) & (uint8_t)auxDacMask;
    auxDacConfigTmp.enable = (auxDacConfigTmp.enable > 0 ? 1 : 0);

    /* Assign temporary struct content to readback pointer */
    *auxDacConfig = auxDacConfigTmp;
    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxDacValueSet(adi_adrv9025_Device_t*     device,
                                    adi_adrv9025_AuxDacValue_t auxDacValues[],
                                    uint8_t                    numberOfCfg)
{
    static const uint32_t ALL_AUXDAC_MASK = (uint32_t)ADI_ADRV9025_AUXDAC0 | (uint32_t)ADI_ADRV9025_AUXDAC1 |
    (uint32_t)ADI_ADRV9025_AUXDAC2 | (uint32_t)ADI_ADRV9025_AUXDAC3 |
    (uint32_t)ADI_ADRV9025_AUXDAC4 | (uint32_t)ADI_ADRV9025_AUXDAC5 |
    (uint32_t)ADI_ADRV9025_AUXDAC6 | (uint32_t)ADI_ADRV9025_AUXDAC7;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adi_adrv9025_AuxDacs_e auxDacSel = ADI_ADRV9025_AUXDAC0;

    /* Variables to iterate over DAC words to be set */
    uint8_t i = 0;
    uint8_t k = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacValues);

    /* Check that if number of configs is > 0 */
    if (numberOfCfg == 0)
    {
        /* no valid configs */
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         numberOfCfg,
                         "Invalid number of configurations");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < numberOfCfg; i++)
    {
        /* Check whether selected AuxDAC index is valid */
        if (((auxDacValues[i].auxDacMask & (~ALL_AUXDAC_MASK)) != 0) || auxDacValues[i].auxDacMask == 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             auxDacMask,
                             "Invalid AuxDAC index is selected for AuxDAC. Valid values are between 1-255.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        for (k = 0; k < ADI_ADRV9025_MAX_AUXDACS; k++)
        {
            if (ADRV9025_BF_EQUAL(auxDacValues[i].auxDacMask,
                                  ((uint32_t)1 << (uint32_t)k)))
            {
                auxDacSel = (adi_adrv9025_AuxDacs_e)((uint8_t)(1 << k));

                /* Set 12 bit AuxDAC word */
                recoveryAction = adrv9025_AuxDacValueSet(device,
                                                         auxDacSel,
                                                         auxDacValues[i].dacWord);
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 device->common.error.errCode,
                                 recoveryAction,
                                 NULL,
                                 "Unable to set 12 bit AuxDAC word");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxDacValueGet(adi_adrv9025_Device_t*      device,
                                    adi_adrv9025_AuxDacs_e      auxDacMask,
                                    adi_adrv9025_AuxDacValue_t* auxDacValue)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacValue);

    /* Check whether selected AuxDAC index is valid */
    if ((auxDacMask != ADI_ADRV9025_AUXDAC0) && (auxDacMask != ADI_ADRV9025_AUXDAC1) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC2) && (auxDacMask != ADI_ADRV9025_AUXDAC3) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC4) && (auxDacMask != ADI_ADRV9025_AUXDAC5) &&
        (auxDacMask != ADI_ADRV9025_AUXDAC6) && (auxDacMask != ADI_ADRV9025_AUXDAC7))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxDacMask,
                         "Invalid AuxDAC index is selected for AuxDAC. Valid AuxDAC indexes are AuxDAC0-7.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Assign auxDacMask field from function input */
    auxDacValue->auxDacMask = auxDacMask;

    /* Get 12 bit DAC word from selected AuxDAC */
    if (auxDacMask == ADI_ADRV9025_AUXDAC0)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac0BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH1,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC1)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac1BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH1,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC2)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac2BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH1,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC2");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC3)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac3BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH1,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC4)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac0BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH2,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC5)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac1BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH2,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC5");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (auxDacMask == ADI_ADRV9025_AUXDAC6)
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac2BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH2,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC6");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac3BfGet(device,
                                                             ADRV9025_BF_ANALOG_TX_CH2,
                                                             &auxDacValue->dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         "Error while reading DAC word for AuxDAC7");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxAdcCfgSet(adi_adrv9025_Device_t*    device,
                                  adi_adrv9025_AuxAdcCfg_t* auxAdcConfig,
                                  uint8_t                   arraySize)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t i              = 0; /* iteration variable */

    /* variables used for configuration checking */
    uint32_t auxAdcSelectMask = 0;
    uint32_t auxAdcSelection  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcConfig);

    if ((arraySize > 0u) && (arraySize <= ADI_ADRV9025_AUXADC_NUM))
    {
        /* Ensure that no configurations are targeting the same ADCs */
        /* No need to check if arraySize == 1 */
        for (i = 0; i < arraySize; i++)
        {
#if ADI_ADRV9025_GPIO_RANGE_CHECK > 0
            /* use one hot encoding to check if multiple configuration structs
             * are targeting the same ADC */
            auxAdcSelection = 1u << auxAdcConfig[i].auxAdcSelect;
            if ((auxAdcSelection & auxAdcSelectMask) > 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 auxAdcConfig[i].auxAdcSelect,
                                 "More than one ADC configuration is targeting a common ADC");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            else
            {
                auxAdcSelectMask |= auxAdcSelection;
            }

            /* Range Check the supplied configuration */
            recoveryAction = adrv9025_AuxAdcCfgRangeCheck(device,
                                                          &auxAdcConfig[i]);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             auxAdcConfig[i],
                             "Error while range checking supplied configuration");
            ADI_ERROR_RETURN(device->common.error.newAction);

#endif /* #if ADI_ADRV9025_GPIO_RANGE_CHECK > 0 */

            /* Configuration is fine, configure ADC */
            recoveryAction = adrv9025_AuxAdcCfgWrite(device,
                                                     &auxAdcConfig[i]);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             recoveryAction,
                             auxAdcConfig[i],
                             "Error while writing supplied configuration");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "arraySize is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxAdcCfgGet(adi_adrv9025_Device_t*      device,
                                  adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                  adi_adrv9025_AuxAdcCfg_t*   auxAdcConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcConfig);

    recoveryAction = adrv9025_AuxAdcCfgRead(device,
                                            auxAdcSelect,
                                            auxAdcConfig);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC configuration");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxAdcValueGet(adi_adrv9025_Device_t*      device,
                                    adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                    adi_adrv9025_AuxAdcValue_t* auxAdcValue)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcValue);

    recoveryAction = adrv9025_AuxAdcValueGet(device,
                                             auxAdcSelect,
                                             auxAdcValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to obtain ADC sample");

    return device->common.error.newAction;
}

int32_t adi_adrv9025_AuxAdcMeasurementStart(adi_adrv9025_Device_t*      device,
                                            adi_adrv9025_AuxAdcSelect_e auxAdcSelect)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    recoveryAction = adrv9025_AuxAdcMeasurementStart(device,
                                                     auxAdcSelect);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while starting Aux Adc Measurement");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}
