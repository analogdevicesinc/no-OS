/**
* \file
* \brief Contains Aux DAC features related function implementation defined in
* adi_adrv9001_auxdac.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_auxdac.h"

#include "adi_adrv9001_gpio.h"
#include "adrv9001_bf.h"

static int32_t __maybe_unused adi_adrv9001_AuxDac_Configure_Validate(adi_adrv9001_Device_t *device, adi_adrv9001_AuxDac_e auxDac)
{
    ADI_RANGE_CHECK(device, auxDac, ADI_ADRV9001_AUXDAC0, ADI_ADRV9001_AUXDAC3);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_AuxDac_Configure(adi_adrv9001_Device_t *device, adi_adrv9001_AuxDac_e auxDac, bool enable)
{
    adi_adrv9001_GpioCfg_t gpio = {
        .pin = ADI_ADRV9001_GPIO_ANALOG_00,
        .polarity = ADI_ADRV9001_GPIO_POLARITY_NORMAL,
        .master = ADI_ADRV9001_GPIO_MASTER_ADRV9001,
    };

    static const uint8_t AUXDAC_DEFAULT_MODE = 0x01;

    ADI_PERFORM_VALIDATION(adi_adrv9001_AuxDac_Configure_Validate, device, auxDac);

    switch (auxDac)
    {
    case ADI_ADRV9001_AUXDAC0:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac0Config_Set, device, AUXDAC_DEFAULT_MODE);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac0Pd_Set, device, !enable);
            break;
    case ADI_ADRV9001_AUXDAC1:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac1Config_Set, device, AUXDAC_DEFAULT_MODE);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac1Pd_Set, device, !enable);
        break;
    case ADI_ADRV9001_AUXDAC2:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac2Config_Set, device, AUXDAC_DEFAULT_MODE);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac2Pd_Set, device, !enable);
        break;
    case ADI_ADRV9001_AUXDAC3:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac3Config_Set, device, AUXDAC_DEFAULT_MODE);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac3Pd_Set, device, !enable);
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(device);
        break;
    }

    gpio.pin = ADI_ADRV9001_GPIO_ANALOG_00 + auxDac;
    ADI_EXPECT(adi_adrv9001_gpio_Configure, device, (ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_0 + auxDac), &gpio);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_AuxDac_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                   adi_adrv9001_AuxDac_e auxDac,
                                                                   bool *enabled)
{
    ADI_RANGE_CHECK(device, auxDac, ADI_ADRV9001_AUXDAC0, ADI_ADRV9001_AUXDAC3);
    ADI_NULL_PTR_RETURN(&device->common, enabled);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_AuxDac_Inspect(adi_adrv9001_Device_t *device, adi_adrv9001_AuxDac_e auxDac, bool *enabled)
{
    uint8_t bfEnable = 0;
    adi_adrv9001_GpioCfg_t gpio = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_AuxDac_Inspect_Validate, device, auxDac, enabled);

    switch (auxDac)
    {
    case ADI_ADRV9001_AUXDAC0:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac0Pd_Get, device, &bfEnable);
        break;
    case ADI_ADRV9001_AUXDAC1:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac1Pd_Get, device, &bfEnable);
        break;
    case ADI_ADRV9001_AUXDAC2:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac2Pd_Get, device, &bfEnable);
        break;
    case ADI_ADRV9001_AUXDAC3:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac3Pd_Get, device, &bfEnable);
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(device);
        break;
    }
    *enabled = !(bool)bfEnable;

    /* TODO: Remove */
    ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, (ADI_ADRV9001_GPIO_SIGNAL_AUX_DAC_0 + auxDac), &gpio);
    if (gpio.pin != ADI_ADRV9001_GPIO_ANALOG_00 + auxDac)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         pin,
                         "AuxDac read back unexpected pin assignment");
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_AuxDac_Code_Set_Validate(adi_adrv9001_Device_t *device,
                                                                    adi_adrv9001_AuxDac_e auxDac,
                                                                    uint16_t code)
{
    static const uint16_t AUX_DAC_VALUE_MIN = 0;
    static const uint16_t AUX_DAC_VALUE_MAX = 4095;

    ADI_RANGE_CHECK(device, auxDac, ADI_ADRV9001_AUXDAC0, ADI_ADRV9001_AUXDAC3);
    ADI_RANGE_CHECK(device, code, AUX_DAC_VALUE_MIN, AUX_DAC_VALUE_MAX);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_AuxDac_Code_Set(adi_adrv9001_Device_t *device, adi_adrv9001_AuxDac_e auxDac, uint16_t code)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_AuxDac_Code_Set_Validate, device, auxDac, code);

    switch (auxDac)
    {
    case ADI_ADRV9001_AUXDAC0:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac0_Set, device, code);
        break;
    case ADI_ADRV9001_AUXDAC1:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac1_Set, device, code);
        break;
    case ADI_ADRV9001_AUXDAC2:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac2_Set, device, code);
        break;
    case ADI_ADRV9001_AUXDAC3:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac3_Set, device, code);
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(device);
        break;
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_AuxDac_Code_Get_Validate(adi_adrv9001_Device_t *device,
                                                                    adi_adrv9001_AuxDac_e auxDac,
                                                                    uint16_t *code)
{
    ADI_RANGE_CHECK(device, auxDac, ADI_ADRV9001_AUXDAC0, ADI_ADRV9001_AUXDAC3);
    ADI_NULL_PTR_RETURN(&device->common, code);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_AuxDac_Code_Get(adi_adrv9001_Device_t *device, adi_adrv9001_AuxDac_e auxDac, uint16_t *code)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_AuxDac_Code_Get_Validate, device, auxDac, code);

    switch (auxDac)
    {
    case ADI_ADRV9001_AUXDAC0:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac0_Get, device, code);
        break;
    case ADI_ADRV9001_AUXDAC1:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac1_Get, device, code);
        break;
    case ADI_ADRV9001_AUXDAC2:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac2_Get, device, code);
        break;
    case ADI_ADRV9001_AUXDAC3:
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Auxdac3_Get, device, code);
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(device);
        break;
    }

    ADI_API_RETURN(device);
}
