/**
* \file
* \brief Contains Aux DAC features related function implementation defined in
* adi_adrv910x_auxdac.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv910x_auxdac.h"
#include "adi_adrv910x_gpio.h"
#include "adrv910x_bf.h"

static __maybe_unused int32_t adi_adrv910x_AuxDac_Configure_Validate(adi_adrv910x_Device_t *device, adi_adrv910x_AuxDac_e auxDac)
{
	ADI_RANGE_CHECK(device, auxDac, ADI_ADRV910X_AUXDAC0, ADI_ADRV910X_AUXDAC3);

	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_AuxDac_Configure(adi_adrv910x_Device_t *device, adi_adrv910x_AuxDac_e auxDac, bool enable)
{
	adi_adrv910x_GpioCfg_t gpio = {
		.pin = ADI_ADRV910X_GPIO_ANALOG_00,
		.polarity = ADI_ADRV910X_GPIO_POLARITY_NORMAL,
		.master = ADI_ADRV910X_GPIO_MASTER_ADRV910X,
	};

	static const uint8_t AUXDAC_DEFAULT_MODE = 0x01;
	static const uint8_t AUXDAC_MUX_SEL_VALUE = 0x00;

	ADI_PERFORM_VALIDATION(adi_adrv910x_AuxDac_Configure_Validate, device, auxDac);
	ADI_EXPECT(adrv910x_NvsRegmapCore2_AuxdacMuxsel_Set, device, AUXDAC_MUX_SEL_VALUE);

	switch (auxDac)
	{
	case ADI_ADRV910X_AUXDAC0:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac0Config_Set, device, AUXDAC_DEFAULT_MODE);
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac0Pd_Set, device, !enable);
		break;
	case ADI_ADRV910X_AUXDAC1:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac1Config_Set, device, AUXDAC_DEFAULT_MODE);
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac1Pd_Set, device, !enable);
		break;
	case ADI_ADRV910X_AUXDAC2:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac2Config_Set, device, AUXDAC_DEFAULT_MODE);
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac2Pd_Set, device, !enable);
		break;
	case ADI_ADRV910X_AUXDAC3:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac3Config_Set, device, AUXDAC_DEFAULT_MODE);
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac3Pd_Set, device, !enable);
		break;
	default:
		ADI_SHOULD_NOT_EXECUTE(device);
		break;
	}

	gpio.pin = ADI_ADRV910X_GPIO_ANALOG_00 + auxDac;
	ADI_EXPECT(adi_adrv910x_gpio_Ps1_Configure, device, (ADI_ADRV910X_GPIO_SIGNAL_AUX_DAC_0 + auxDac), &gpio);

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_AuxDac_Inspect_Validate(adi_adrv910x_Device_t *device,
	adi_adrv910x_AuxDac_e auxDac,
	bool *enabled)
{
	ADI_RANGE_CHECK(device, auxDac, ADI_ADRV910X_AUXDAC0, ADI_ADRV910X_AUXDAC3);
	ADI_NULL_PTR_RETURN(&device->common, enabled);
	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_AuxDac_Inspect(adi_adrv910x_Device_t *device, adi_adrv910x_AuxDac_e auxDac, bool *enabled)
{
	uint8_t bfEnable = 0;

	ADI_PERFORM_VALIDATION(adi_adrv910x_AuxDac_Inspect_Validate, device, auxDac, enabled);

	switch (auxDac)
	{
	case ADI_ADRV910X_AUXDAC0:
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac0Pd_Get, device, &bfEnable);
		break;
	case ADI_ADRV910X_AUXDAC1:
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac1Pd_Get, device, &bfEnable);
		break;
	case ADI_ADRV910X_AUXDAC2:
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac2Pd_Get, device, &bfEnable);
		break;
	case ADI_ADRV910X_AUXDAC3:
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Auxdac3Pd_Get, device, &bfEnable);
		break;
	default:
		ADI_SHOULD_NOT_EXECUTE(device);
		break;
	}
	*enabled = !(bool)bfEnable;

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_AuxDac_Code_Set_Validate(adi_adrv910x_Device_t *device,
	adi_adrv910x_AuxDac_e auxDac,
	uint16_t code)
{
	static const uint16_t AUX_DAC_VALUE_MIN = 0;
	static const uint16_t AUX_DAC_VALUE_MAX = 4095;

	ADI_RANGE_CHECK(device, auxDac, ADI_ADRV910X_AUXDAC0, ADI_ADRV910X_AUXDAC3);
	ADI_RANGE_CHECK(device, code, AUX_DAC_VALUE_MIN, AUX_DAC_VALUE_MAX);

	ADI_API_RETURN(device);
}


int32_t adi_adrv910x_AuxDac_Code_Set(adi_adrv910x_Device_t *device, adi_adrv910x_AuxDac_e auxDac, uint16_t code)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_AuxDac_Code_Set_Validate, device, auxDac, code);
	int32_t status = 0;
	switch (auxDac)
	{
	case ADI_ADRV910X_AUXDAC0:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac0_Set, device, code);
		break;
	case ADI_ADRV910X_AUXDAC1:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac1_Set, device, code);
		break;
	case ADI_ADRV910X_AUXDAC2:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac2_Set, device, code);
		break;
	case ADI_ADRV910X_AUXDAC3:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac3_Set, device, code);
		break;
	default:
		ADI_SHOULD_NOT_EXECUTE(device);
		break;
	}
	

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_AuxDac_Code_Get_Validate(adi_adrv910x_Device_t *device,
	adi_adrv910x_AuxDac_e auxDac,
	uint16_t *code)
{
	ADI_RANGE_CHECK(device, auxDac, ADI_ADRV910X_AUXDAC0, ADI_ADRV910X_AUXDAC3);
	ADI_NULL_PTR_RETURN(&device->common, code);
	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_AuxDac_Code_Get(adi_adrv910x_Device_t *device, adi_adrv910x_AuxDac_e auxDac, uint16_t *code)
{
	ADI_PERFORM_VALIDATION(adi_adrv910x_AuxDac_Code_Get_Validate, device, auxDac, code);

	switch (auxDac)
	{
	case ADI_ADRV910X_AUXDAC0:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac0_Get, device, code);
		break;
	case ADI_ADRV910X_AUXDAC1:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac1_Get, device, code);
		break;
	case ADI_ADRV910X_AUXDAC2:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac2_Get, device, code);
		break;
	case ADI_ADRV910X_AUXDAC3:
		ADI_EXPECT(adrv910x_NvsRegmapCore2_Auxdac3_Get, device, code);
		break;
	default:
		ADI_SHOULD_NOT_EXECUTE(device);
		break;
	}

	ADI_API_RETURN(device);
}