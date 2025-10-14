/**
* \file
* \brief Contains gpio features related function implementation defined in
* adi_adrv910x_gpio.h
*
* Copyright 2023 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv910x_user.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"
#include "object_ids.h"

static __maybe_unused int32_t __maybe_unused adi_adrv910x_gpio_ManualInput_Configure_Validate(adi_adrv910x_Device_t *device,
                                           adi_adrv910x_GpioPin_e pin)
{
    ADI_RANGE_CHECK(device, pin, ADI_ADRV910X_GPIO_DIGITAL_00, ADI_ADRV910X_GPIO_DIGITAL_15);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_gpio_ManualInput_Configure(adi_adrv910x_Device_t *device, adi_adrv910x_GpioPin_e pin)
{
    uint32_t gpioOutEn = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_gpio_ManualInput_Configure_Validate, device, pin);

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adrv910x_NvsRegmapCore_NvsGpioDirectionControlOe_Get, device, &gpioOutEn);
    gpioOutEn &= ~(1 << (pin - 1));
    ADI_EXPECT(adrv910x_NvsRegmapCore_NvsGpioDirectionControlOe_Set, device, gpioOutEn);
	
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_gpio_Ps1_Configure_Validate(adi_adrv910x_Device_t *device,
	adi_adrv910x_GpioSignal_e signal,
	adi_adrv910x_GpioCfg_t *gpioConfig)
{
	/* Check device pointer is not null */
	ADI_ENTRY_PTR_EXPECT(device, gpioConfig);

	/*Check that GPIO signal is valid*/
	ADI_RANGE_CHECK(device, signal, 0x00, ADI_ADRV910X_GPIO_NUM_SIGNALS - 1);

	/*Check that GPIO pin is valid*/
	ADI_RANGE_CHECK(device, gpioConfig->pin, ADI_ADRV910X_GPIO_DIGITAL_00, ADI_ADRV910X_GPIO_ANALOG_11);

	/*Check that GPIO control is valid*/
	switch (gpioConfig->master)
	{
	case ADI_ADRV910X_GPIO_MASTER_BBIC:     /* Falls through */
	case ADI_ADRV910X_GPIO_MASTER_ADRV910X:
		break;
	default:
		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			armgpioConfig->control,
			"Invalid parameter value. armgpioConfig->control must be either ADI_ADRV910X_GPIO_MASTER_BBIC"
			" or ADI_ADRV910X_GPIO_MASTER_ADRV910X");
	}

	/*Check that GPIO polarity is valid*/
	ADI_RANGE_CHECK(device, gpioConfig->polarity, ADI_ADRV910X_GPIO_POLARITY_NORMAL, ADI_ADRV910X_GPIO_POLARITY_INVERTED);

	ADI_API_RETURN(device);
}

static const uint8_t MAILBOX_GPIO_UNASSIGNED = 28;

static uint8_t adrv910x_gpio_PinToMailbox_Convert(adi_adrv910x_GpioPin_e pin)
{
	if (ADI_ADRV910X_GPIO_UNASSIGNED == pin)
	{
		return MAILBOX_GPIO_UNASSIGNED;
	}
	return pin - 1;
}

static adi_adrv910x_GpioPin_e adrv910x_gpio_MailboxToPin_Convert(uint8_t mailboxPin)
{
	if (MAILBOX_GPIO_UNASSIGNED == mailboxPin)
	{
		return ADI_ADRV910X_GPIO_UNASSIGNED;
	}
	return (adi_adrv910x_GpioPin_e)(mailboxPin + 1);
}

int32_t adi_adrv910x_gpio_Ps1_Configure(adi_adrv910x_Device_t *device,
	adi_adrv910x_GpioSignal_e signal,
	adi_adrv910x_GpioCfg_t *gpioConfig)
{
	static const uint8_t GPIO_ENABLE = 0x04;

	uint8_t extData[5] = { 0 };
#if ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	uint8_t cmdStatusByte = 0;
#endif

	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

	ADI_PERFORM_VALIDATION(adi_adrv910x_gpio_Ps1_Configure_Validate, device, signal, gpioConfig);

	/* Command ARM to associate the currently assigned GPIO for the requested signal ID */
	extData[0] = 0;
	extData[1] = OBJID_GS_GPIO_CTRL;
	extData[2] = signal;
	extData[3] = adrv910x_gpio_PinToMailbox_Convert(gpioConfig->pin);
	extData[4] = GPIO_ENABLE | gpioConfig->master | gpioConfig->polarity;

	ADI_MUTEX_AQUIRE(device);

	recoveryAction = adi_adrv910x_arm_Cmd_Write(device, ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));
	ADI_ERROR_RETURN(device->common.error.newAction);
    
#if ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	/* Wait for command to finish executing */
	recoveryAction = adi_adrv910x_arm_CmdStatus_Wait(device,
		ADRV910X_ARM_SET_OPCODE,
		&cmdStatusByte,
		ADI_ADRV910X_SETARMGPIO_TIMEOUT_US,
		ADI_ADRV910X_SETARMGPIO_INTERVAL_US);
	if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
	{
		/* If cmdStatusByte is non-zero then flag an ARM error and release the acquired shared resource */
		if ((cmdStatusByte >> 1) > 0)
		{
			ADI_EXPECT(adrv910x_ArmCmdErrorHandler,
				device,
				ADRV910X_ARMCMD_ERRCODE(ADRV910X_ARM_SET_OPCODE, extData[0], cmdStatusByte));
		}
	}
#else
	recoveryAction = adi_common_hal_Wait_us(&device->common, ADI_ADRV910X_ARM_SET_OPCODE_WAIT_INTERVAL_US);
	if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
	{
		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_ADI_HAL,
			recoveryAction,
			ADI_COMMON_ACT_ERR_CHECK_TIMER,
			device,
			"Timer not working");
		ADI_ERROR_RETURN(device->common.error.newAction);
	}
#endif
	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_gpio_Ps1_Inspect_Validate(adi_adrv910x_Device_t *device,
	adi_adrv910x_GpioSignal_e gpioSignalSel,
	adi_adrv910x_GpioCfg_t *gpioConfig)
{
	ADI_RANGE_CHECK(device, gpioSignalSel, 0x00, ADI_ADRV910X_GPIO_NUM_SIGNALS - 1);
	ADI_NULL_PTR_RETURN(&device->common, gpioConfig);

	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_gpio_Ps1_Inspect(adi_adrv910x_Device_t *device,
	adi_adrv910x_GpioSignal_e signal,
	adi_adrv910x_GpioCfg_t *gpioConfig)
{
	static const uint8_t GPIO_CONTROL_SHIFT = 1;
	static const uint8_t GPIO_CONTROL_MASK  = 0x02;
	static const uint8_t GPIO_POLARITY_SHIFT    = 0;
	static const uint8_t GPIO_POLARITY_MASK = 0x01;

	uint8_t extData[3] = { 0 };
	uint8_t armData[2] = { 0 };

	/* Check device pointer is not null */
	ADI_PERFORM_VALIDATION(adi_adrv910x_gpio_Ps1_Inspect_Validate, device, signal, gpioConfig);

	/* Command ARM to return the currently assigned GPIO for the requested signal ID */
	extData[0] = 0;
	extData[1] = OBJID_GS_GPIO_CTRL;
	extData[2] = signal;

	ADI_MUTEX_AQUIRE(device);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		ADRV910X_ARM_GET_OPCODE,
		extData[1],
		ADI_ADRV910X_GETARMGPIO_TIMEOUT_US,
		ADI_ADRV910X_GETARMGPIO_INTERVAL_US);

	/* Read GPIO Pin sel from ARM mailbox */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		(uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armData[0],
		sizeof(armData),
		false, ADI_PS1);

	/* Return the GPIO data read back from ARM mailbox */
	gpioConfig->pin = adrv910x_gpio_MailboxToPin_Convert(armData[0]);
	gpioConfig->master = (adi_adrv910x_GpioMaster_e)((armData[1] & GPIO_CONTROL_MASK) >> GPIO_CONTROL_SHIFT);
	gpioConfig->polarity = (adi_adrv910x_GpioPolarity_e)((armData[1] & GPIO_POLARITY_MASK) >> GPIO_POLARITY_SHIFT);

	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}
