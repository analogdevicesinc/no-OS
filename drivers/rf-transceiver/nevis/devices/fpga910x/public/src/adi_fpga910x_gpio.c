/**
 * \file:
 * \brief Functions to configure and control the FPGA910X GPIO pins
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
 * Copyright 2022 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_fpga910x_gpio.h"
#include "adi_fpga910x.h"
#include "fpga910x_utilities.h"

static __maybe_unused int32_t adi_fpga910x_gpio_OperationMode_Set_Validate(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_OperationModes_e operationMode)
{
	ADI_RANGE_CHECK(fpga910x, operationMode, ADI_FPGA910X_OPERATION_MODE_NORMAL, ADI_FPGA910X_OPERATION_MODE_JTAG);

	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_OperationMode_Set(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_OperationModes_e operationMode)
{
	ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_OperationMode_Set_Validate, fpga910x, operationMode);
	axi_adrv9102_top_mode_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, (uint32_t) operationMode);
	ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_OperationMode_Get_Validate(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_OperationModes_e *operationMode)
{
	ADI_NULL_PTR_RETURN(&fpga910x->common, operationMode);

	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_OperationMode_Get(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_OperationModes_e *operationMode)
{
	uint32_t mode = 0;
	ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_OperationMode_Get_Validate, fpga910x, operationMode);
    axi_adrv9102_top_mode_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &mode);
	*operationMode = (mode == 0) ? ADI_FPGA910X_OPERATION_MODE_NORMAL : ADI_FPGA910X_OPERATION_MODE_JTAG;
	ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_ReadValidate(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioGroup_e group, uint16_t *rdData)
{
	ADI_NULL_PTR_RETURN(&fpga910x->common, rdData);
	ADI_RANGE_CHECK(fpga910x, group, ADI_FPGA910X_GPIO_GROUP_DGPIO, ADI_FPGA910X_GPIO_GROUP_TX0);

	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_Read(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioGroup_e group, uint16_t *rdData)
{
	uint32_t data = 0;
	uint32_t gpioGroup = 0;

	ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_ReadValidate, fpga910x, group, rdData);

	axi_adrv9102_gpio_group_data((void *)fpga910x, AXI_ADRV9102_ID, group, &data);
	*rdData = (uint16_t) data;
	ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_pin_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                        adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, gpioPinConfig);

    ADI_RANGE_CHECK(fpga910x, gpioPinConfig->mode, ADI_FPGA910X_GPIO_MODE_SOFTWARE, ADI_FPGA910X_GPIO_MODE_HARDWARE);
    ADI_RANGE_CHECK(fpga910x, gpioPinConfig->direction, ADI_FPGA910X_GPIO_DIRECTION_INPUT, ADI_FPGA910X_GPIO_DIRECTION_OUTPUT);
    ADI_RANGE_CHECK(fpga910x, gpioPinConfig->dataOut, ADI_FPGA910X_GPIO_DATAOUT_CLEAR, ADI_FPGA910X_GPIO_DATAOUT_SET);
	ADI_RANGE_CHECK(fpga910x, gpioPinConfig->pin, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
    ADI_RANGE_CHECK(fpga910x, gpioPinConfig->hwSourceMode, ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_TX1_ENABLE, ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_6);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_pin_Configure(adi_fpga910x_Device_t *fpga910x,
                                        adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    uint32_t gpio_pin = (uint32_t)gpioPinConfig->pin-1;
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_pin_Configure_Validate, fpga910x, gpioPinConfig);
    
    axi_adrv9102_gpio_pin_mode_set((void *)fpga910x, AXI_ADRV9102_ID, gpio_pin, gpioPinConfig->mode);
    axi_adrv9102_gpio_pin_direction_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_DGPIO, gpio_pin, gpioPinConfig->direction);
    axi_adrv9102_gpio_pin_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_DGPIO, gpio_pin, (uint32_t)gpioPinConfig->dataOut);
    axi_adrv9102_gpio_pin_source_set((void *)fpga910x, AXI_ADRV9102_ID, gpio_pin, (uint32_t)gpioPinConfig->hwSourceMode);
    
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_pin_Inspect_Validate(adi_fpga910x_Device_t *fpga910x, 
                                                      adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, gpioPinConfig);
	ADI_RANGE_CHECK(fpga910x, gpioPinConfig->pin, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_pin_Inspect(adi_fpga910x_Device_t *fpga910x, 
                                      adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    uint32_t gpio_pin = (uint32_t)gpioPinConfig->pin-1;
    uint32_t gpio_mode;
    uint32_t gpio_direction;
    uint32_t gpio_data;
    uint32_t gpio_source;

    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_pin_Inspect_Validate, fpga910x, gpioPinConfig);

    axi_adrv9102_gpio_pin_mode_get((void *)fpga910x, AXI_ADRV9102_ID, gpio_pin, &gpio_mode);
    axi_adrv9102_gpio_pin_direction_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_DGPIO, gpio_pin, &gpio_direction);
    axi_adrv9102_gpio_pin_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_DGPIO, gpio_pin, &gpio_data);
    axi_adrv9102_gpio_pin_source_get((void *)fpga910x, AXI_ADRV9102_ID, gpio_pin, &gpio_source);

    gpioPinConfig->mode = (adi_fpga910x_GpioMode_e)gpio_mode;
    gpioPinConfig->direction = (adi_fpga910x_GpioDirection_e)gpio_direction;
    gpioPinConfig->dataOut = (adi_fpga910x_GpioDataOut_e)gpio_data;
    gpioPinConfig->hwSourceMode = (adi_fpga910x_GpioHWSourceModes_e)gpio_source;

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused uint8_t adi_fpga910x_gpio_pin_SourceMode_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                   adi_fpga910x_GpioPinCfg_t gpioPinConfig[],
                                                                   uint32_t length)
{
    uint8_t i = 0;

    ADI_NULL_PTR_RETURN(&fpga910x->common, gpioPinConfig);

    ADI_RANGE_CHECK(fpga910x, length, 1, ADI_FPGA910X_NUM_GPIO_PINS);

    for (i = 0; i < length; i++)
    {
        ADI_EXPECT(adi_fpga910x_gpio_pin_Configure_Validate, fpga910x, (gpioPinConfig + i));
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_pin_SourceMode_Configure(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioPinCfg_t gpioPinConfig[], uint32_t length)
{
    uint8_t i = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_pin_SourceMode_Configure_Validate, fpga910x, gpioPinConfig, length);

    for (i = 0; i < length; i++)
    {
        ADI_EXPECT(adi_fpga910x_gpio_pin_Configure, fpga910x, (gpioPinConfig + i));
    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_pin_SourceMode_Inspect_Validate(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, gpioPinConfig);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_pin_SourceMode_Inspect(adi_fpga910x_Device_t *fpga910x,
                                                 adi_fpga910x_GpioPinCfg_t *gpioPinConfig)
{
    uint8_t i = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_pin_SourceMode_Inspect_Validate, fpga910x, gpioPinConfig);

    for (i = 0; i < ADI_FPGA910X_NUM_GPIO_PINS; i++)
    {
        ADI_EXPECT(adi_fpga910x_gpio_pin_Inspect, fpga910x, (gpioPinConfig + i));
    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_GainIndex_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                              adi_fpga910x_GpioGainIndexCfg_t *gainIndexConfig)
{
    uint8_t i = 0;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndexConfig);
    ADI_RANGE_CHECK(fpga910x, gainIndexConfig->group, ADI_FPGA910X_GPIO_GROUP_DGPIO, ADI_FPGA910X_GPIO_GROUP_TX0);

    for (i = 0; i < 8; i++)
    {
        if (ADI_FPGA910X_GPIO_GROUP_DGPIO == gainIndexConfig->group)
        {
	        ADI_RANGE_CHECK(fpga910x, gainIndexConfig->readGainIndex[i], ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
        }
        else if ((ADI_FPGA910X_GPIO_GROUP_RX0 == gainIndexConfig->group) || (ADI_FPGA910X_GPIO_GROUP_RX1 == gainIndexConfig->group))
        {
            ADI_RANGE_CHECK(fpga910x, gainIndexConfig->readGainIndex[i], ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_07);
        }
        else
        {
            ADI_RANGE_CHECK(fpga910x, gainIndexConfig->readGainIndex[i], ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_09);
        }
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_GainIndex_Configure(adi_fpga910x_Device_t *fpga910x,
                                              adi_fpga910x_GpioGainIndexCfg_t *gainIndexConfig)
{
    uint32_t group;
    uint32_t pins[8];
    int32_t i;

    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_GainIndex_Configure_Validate, fpga910x, gainIndexConfig);

    for (i = 0; i < 8; i++)
    {
        pins[i] = (uint32_t)gainIndexConfig->readGainIndex[i];
    }

    if (ADI_FPGA910X_GPIO_GROUP_DGPIO == gainIndexConfig->group)
    {
        group = (uint32_t)(gainIndexConfig->group + 1);
    }
    else
    {
        group = (uint32_t)(gainIndexConfig->group + 3);
    }

    axi_adrv9102_gpio_gainindex_sel((void *)fpga910x, AXI_ADRV9102_ID, group, &pins[0], 8);
    axi_adrv9102_gpio_trig_sel((void *)fpga910x, AXI_ADRV9102_ID, group, pins[7]);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_GainIndex_Get_Validate(adi_fpga910x_Device_t *fpga910x,
                                                        uint32_t *gainIndex)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndex);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_GainIndex_Get(adi_fpga910x_Device_t *fpga910x,
                                        uint32_t *gainIndex)
{
    uint32_t gpio_data;
    uint32_t value = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_GainIndex_Get_Validate, fpga910x, gainIndex);

    gpio_data = 0;
    axi_adrv9102_gpio_group_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_TXSSI0, &value);
    gpio_data = (gpio_data << 8) | value;
    axi_adrv9102_gpio_group_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_RXSSI1, &value);
    gpio_data = (gpio_data << 8) | value;
    axi_adrv9102_gpio_group_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_GPIO_IO_GROUP_RXSSI0, &value);
    gpio_data = (gpio_data << 8) | value;

    *gainIndex = gpio_data;
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_TriggerSource_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                  adi_fpga910x_GpioTriggerCfg_t *triggerCfg)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(&fpga910x->common, triggerCfg);
    ADI_RANGE_CHECK(fpga910x, triggerCfg->group, ADI_FPGA910X_GPIO_GROUP_DGPIO, ADI_FPGA910X_GPIO_GROUP_TX0);

    if (ADI_FPGA910X_GPIO_GROUP_DGPIO == triggerCfg->group)
    {
	    ADI_RANGE_CHECK(fpga910x, triggerCfg->triggerSource, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
    }
    else if ((ADI_FPGA910X_GPIO_GROUP_RX0 == triggerCfg->group) || (ADI_FPGA910X_GPIO_GROUP_RX1 == triggerCfg->group))
    {
        ADI_RANGE_CHECK(fpga910x, triggerCfg->triggerSource, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_07);
    }
    else
    {
        ADI_RANGE_CHECK(fpga910x, triggerCfg->triggerSource, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_09);
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_TriggerSource_Configure(adi_fpga910x_Device_t *fpga910x,
                                                  adi_fpga910x_GpioTriggerCfg_t *triggerCfg)
{
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_TriggerSource_Configure_Validate, fpga910x, triggerCfg);

    axi_adrv9102_gpio_trig_sel((void *)fpga910x, AXI_ADRV9102_ID, (uint32_t)(triggerCfg->group), (uint32_t)(triggerCfg->triggerSource));
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_gpio_smapin_Validate(adi_fpga910x_Device_t *fpga910x,
                                                 adi_fpga910x_GpioSmaCfg_t *smaConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, smaConfig);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_smapin_Configure(adi_fpga910x_Device_t *fpga910x,
                                           adi_fpga910x_GpioSmaCfg_t *smaConfig)
{
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_smapin_Validate, fpga910x, smaConfig);

    axi_adrv9102_top_trig0_ext_oe_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, (uint32_t) (smaConfig->smaPin1));
    axi_adrv9102_top_trig1_ext_oe_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, (uint32_t) (smaConfig->smaPin2));

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_gpio_smapin_Inspect(adi_fpga910x_Device_t *fpga910x,
                                         adi_fpga910x_GpioSmaCfg_t *smaConfig)
{
    uint32_t pinConfig = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_gpio_smapin_Validate, fpga910x, smaConfig);

    axi_adrv9102_top_trig0_ext_oe_get(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &pinConfig);
    smaConfig->smaPin1  = (pinConfig == 1) ? true : false;
    axi_adrv9102_top_trig1_ext_oe_get(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &pinConfig);
    smaConfig->smaPin2  = (pinConfig == 1) ? true : false;
    ADI_API_RETURN(fpga910x);
}

