/**
 * \file
 * \brief Contains top level TDD fpga910x related functions
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
 * Copyright 2022 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "fpga910x_utilities.h"
#include "adi_fpga910x_tdd.h"
#include "adi_fpga910x_gpio.h"

/* TDD address lookup table */

static const uint32_t adi_fpga910x_TddSelectLUT[] =
{
    AXI_ADRV9102_TDD_RX0_DEVICE,
    AXI_ADRV9102_TDD_TX0_DEVICE,
    AXI_ADRV9102_TDD_RX1_DEVICE,
    AXI_ADRV9102_TDD_ORX0_GPIO,
    AXI_ADRV9102_TDD_ORX1_GPIO,
    AXI_ADRV9102_TDD_RX0_DMA_DATA,
    AXI_ADRV9102_TDD_TX0_DMA_DATA,
    AXI_ADRV9102_TDD_RX1_DMA_DATA,
    AXI_ADRV9102_TDD_ORX0_DMA_DATA,
    AXI_ADRV9102_TDD_ORX1_DMA_DATA,
    AXI_ADRV9102_TDD_TX0_GPIO,
    AXI_ADRV9102_TDD_IFRM0CTL0_GPIO,
    AXI_ADRV9102_TDD_IFRM0CTL1_GPIO,
    AXI_ADRV9102_TDD_IFRM1CTL0_GPIO,
    AXI_ADRV9102_TDD_IFRM1CTL1_GPIO,
    AXI_ADRV9102_TDD_EXT0_TRIG,
    AXI_ADRV9102_TDD_EXT1_TRIG,
    AXI_ADRV9102_TDD_RX0_DMA_TRIG,
    AXI_ADRV9102_TDD_TX0_DMA_TRIG,
    AXI_ADRV9102_TDD_RX1_DMA_TRIG,
    AXI_ADRV9102_TDD_ORX0_DMA_TRIG,
    AXI_ADRV9102_TDD_ORX1_DMA_TRIG,
    AXI_ADRV9102_TDD_SPI_TRIG,
    AXI_ADRV9102_TDD_GPIO_TRIG,
};

static const adi_fpga910x_GpioHWSourceModes_e adi_fpga910x_TddHWSourceModeLUT[] = 
{
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_RX1_ENABLE,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_TX1_ENABLE,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_RX2_ENABLE,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_ORX1_ENABLE,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_ORX2_ENABLE,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_1,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_2,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_3,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_4,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_5,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_6,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED
};

static __maybe_unused int32_t adi_fpga910x_Tdd_Enable_Set_Validate(adi_fpga910x_Device_t *fpga910x,
                                                    adi_fpga910x_TddSelect_e tddSelect,
                                                    adi_fpga910x_TddMode_e tddMode)
{
    ADI_RANGE_CHECK(fpga910x, tddSelect, 0, ADI_FPGA910X_TDDSELECT_MAX_COUNT);
    ADI_RANGE_CHECK(fpga910x, tddMode, ADI_FPGA910X_TDD_ENABLE_LOW, ADI_FPGA910X_TDD_ENABLE_AUTO);
    ADI_API_RETURN(fpga910x);
}

/* Set TDD enable as requested by select */
int32_t adi_fpga910x_Tdd_Enable_Set(adi_fpga910x_Device_t *fpga910x,
                                    adi_fpga910x_TddSelect_e tddSelect,
                                    adi_fpga910x_TddMode_e tddMode)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_Tdd_Enable_Set_Validate, fpga910x, tddSelect, tddMode);
    axi_adrv9102_tdd_mode_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, adi_fpga910x_TddSelectLUT[tddSelect], tddMode);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_Tdd_Enable_Get_Validate(adi_fpga910x_Device_t *fpga910x,
                                                    adi_fpga910x_TddSelect_e tddSelect,
                                                    adi_fpga910x_TddMode_e *mode)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, mode);
    ADI_RANGE_CHECK(fpga910x, tddSelect, 0, ADI_FPGA910X_TDDSELECT_MAX_COUNT);
    ADI_API_RETURN(fpga910x);
}

/* Return TDD enable as requested by select */
int32_t adi_fpga910x_Tdd_Enable_Get(adi_fpga910x_Device_t *fpga910x,
                                    adi_fpga910x_TddSelect_e tddSelect,
                                    adi_fpga910x_TddMode_e *mode)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_Tdd_Enable_Get_Validate, fpga910x, tddSelect, mode);
    axi_adrv9102_tdd_mode_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, adi_fpga910x_TddSelectLUT[tddSelect], mode);
    ADI_API_RETURN(fpga910x);
}

/* Configure all TDD enables */
int32_t adi_fpga910x_Tdd_Configure(adi_fpga910x_Device_t *fpga910x,
                                   adi_fpga910x_TddConfig_t *tddConfig)
{
    int32_t i = 0;
    uint32_t tdd_mode = 0;
    uint32_t tdd_ratio = 0;
    uint32_t tdd_clk_freq = 0;
    axi_adrv9102_tdd_frame_params_t tdd_frame = { 0 };
    axi_adrv9102_tdd_enable_params_t tdd_enable = { 0 };

    /* get count from micro seconds, if so desired */
    axi_adrv9102_tdd_clk_mon_count_freq_hz((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, &tdd_clk_freq);
    tdd_clk_freq = tdd_clk_freq/1000000;

    for (i = 0; i < ADI_FPGA910X_TDDSELECT_MAX_COUNT; i++)
    {
        tdd_ratio = (tddConfig->enables[i].inMicroSeconds) ? tdd_clk_freq : 1;
        tdd_enable.primary_assert = tddConfig->enables[i].primaryAssert * tdd_ratio;
        tdd_enable.primary_deassert = tddConfig->enables[i].primaryDeassert * tdd_ratio;
        tdd_enable.secondary_assert = tddConfig->enables[i].secondaryAssert * tdd_ratio;
        tdd_enable.secondary_deassert = tddConfig->enables[i].secondaryDeassert * tdd_ratio;
        tdd_enable.primary_frame_assert = tddConfig->enables[i].primaryFrameAssert;
        tdd_enable.primary_frame_deassert = tddConfig->enables[i].primaryFrameDeassert;
        tdd_enable.secondary_frame_assert = tddConfig->enables[i].secondaryFrameAssert;
        tdd_enable.secondary_frame_deassert = tddConfig->enables[i].secondaryFrameDeassert;
        tdd_mode = (tddConfig->enables[i].enable) ? AXI_ADRV9102_TDD_ENABLE_AUTO : AXI_ADRV9102_TDD_ENABLE_LOW;
        axi_adrv9102_tdd_enable_set((void *)fpga910x, AXI_ADRV9102_ID, adi_fpga910x_TddSelectLUT[i], &tdd_enable);
        axi_adrv9102_tdd_mode_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, adi_fpga910x_TddSelectLUT[i], tdd_mode);
        axi_adrv9102_tdd_sync_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, adi_fpga910x_TddSelectLUT[i], tddConfig->enables[i].enableSyncToSample);
        if (tddConfig->enables[i].gpioPin != ADI_FPGA910X_GPIO_UNASSIGNED && adi_fpga910x_TddHWSourceModeLUT[i] != ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED)
        {
            adi_fpga910x_GpioPinCfg_t enablePin = { 
            .mode = ADI_FPGA910X_GPIO_MODE_HARDWARE, 
            .direction = ADI_FPGA910X_GPIO_DIRECTION_OUTPUT, 
            .dataOut = ADI_FPGA910X_GPIO_DATAOUT_CLEAR, 
            .pin = tddConfig->enables[i].gpioPin, 
            .hwSourceMode = adi_fpga910x_TddHWSourceModeLUT[i]
	        };
        
	        ADI_EXPECT(adi_fpga910x_gpio_pin_Configure, fpga910x, &enablePin);

        }
    }

    tdd_ratio = (tddConfig->frameTiming.inMicroSeconds) ? tdd_clk_freq : 1;
    tdd_frame.period = tddConfig->frameTiming.framePeriod * tdd_ratio;
    tdd_frame.num_of_frames = tddConfig->frameTiming.numberFrames;
    tdd_frame.continous = tddConfig->frameTiming.repeatFrameSequence;
    tdd_frame.frame_switch_period = tddConfig->frameTiming.frameSwitchTime * tdd_ratio;
    tdd_frame.frame_switch_num = tddConfig->frameTiming.frameSwitchNum;
    tdd_frame.frame_switch_enable = tddConfig->frameTiming.frameSwitchEnable;
    tdd_frame.triggers[(int) tddConfig->frameTiming.frameStartTrig] = AXI_ADRV9102_TDD_TRIGGER_RISING_EDGE;

    axi_adrv9102_tdd_frame_set((void *)fpga910x, AXI_ADRV9102_ID, &tdd_frame);
    
    ADI_API_RETURN(fpga910x);
}

/* Read all current TDD configurations */
int32_t adi_fpga910x_Tdd_Inspect(adi_fpga910x_Device_t *fpga910x,
                                 adi_fpga910x_TddConfig_t *tddConfig)
{
    int32_t i = 0;
    uint32_t tdd_mode = 0;
    uint32_t tdd_ratio = 0;
    uint32_t tdd_clk_freq = 0;
    axi_adrv9102_tdd_frame_params_t tdd_frame = { 0 };
    axi_adrv9102_tdd_enable_params_t tdd_enable = { 0 };

    /* get count from micro seconds, if so desired */
    axi_adrv9102_tdd_clk_mon_count_freq_hz((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, &tdd_clk_freq);
    tdd_clk_freq = (tdd_clk_freq == 0) ? 1 : tdd_clk_freq/1000000;

    for (i = 0; i < ADI_FPGA910X_TDDSELECT_MAX_COUNT; i++)
    {
        axi_adrv9102_tdd_enable_get((void *)fpga910x, AXI_ADRV9102_ID, adi_fpga910x_TddSelectLUT[i], &tdd_enable);
        tdd_ratio = (tddConfig->enables[i].inMicroSeconds) ? tdd_clk_freq : 1;
        tddConfig->enables[i].primaryAssert = tdd_enable.primary_assert/tdd_ratio;
        tddConfig->enables[i].primaryDeassert = tdd_enable.primary_deassert/tdd_ratio;
        tddConfig->enables[i].secondaryAssert = tdd_enable.secondary_assert/tdd_ratio;
        tddConfig->enables[i].secondaryDeassert = tdd_enable.secondary_deassert/tdd_ratio;
        tddConfig->enables[i].primaryFrameAssert = tdd_enable.primary_frame_assert;
        tddConfig->enables[i].primaryFrameDeassert = tdd_enable.primary_frame_deassert;
        tddConfig->enables[i].secondaryFrameAssert = tdd_enable.secondary_frame_assert;
        tddConfig->enables[i].secondaryFrameDeassert = tdd_enable.secondary_frame_deassert;
        axi_adrv9102_tdd_mode_get((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TDD_OFFSET, adi_fpga910x_TddSelectLUT[i], &tdd_mode);
        tddConfig->enables[i].enable = (tdd_mode == AXI_ADRV9102_TDD_ENABLE_AUTO) ? true : false;
    }

    axi_adrv9102_tdd_frame_get((void *)fpga910x, AXI_ADRV9102_ID, &tdd_frame);
    tdd_ratio = (tddConfig->frameTiming.inMicroSeconds) ? tdd_clk_freq : 1;
    tddConfig->frameTiming.framePeriod = tdd_frame.period / tdd_ratio;
    tddConfig->frameTiming.numberFrames = tdd_frame.num_of_frames;
    tddConfig->frameTiming.repeatFrameSequence = tdd_frame.continous;
    tddConfig->frameTiming.frameSwitchTime = tdd_frame.frame_switch_period / tdd_ratio;
    tddConfig->frameTiming.frameSwitchNum = tdd_frame.frame_switch_num;
    tddConfig->frameTiming.frameSwitchEnable = tdd_frame.frame_switch_enable;

    for (i = 0; i <= 3; i++) {
        if (tdd_frame.triggers[i] == AXI_ADRV9102_TDD_TRIGGER_DISABLED) continue;
        tddConfig->frameTiming.frameStartTrig = i;
        break;
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Tdd_Start(adi_fpga910x_Device_t *fpga910x)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    
    axi_adrv9102_tdd_start((void *)fpga910x, AXI_ADRV9102_ID);
    
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_Tdd_StartBit_Inspect(adi_fpga910x_Device_t *fpga910x)
{
    int32_t halError = 0;
    uint32_t timeout_us = 10000000;
    uint32_t interval_us = 50000;
    uint32_t numEventChecks = 0;
    uint32_t eventCheck = 0;
    uint32_t tddStatus = 0xFF;

    interval_us = (interval_us > timeout_us) ? timeout_us : interval_us;
    numEventChecks = (interval_us == 0) ? 1 : (timeout_us / interval_us);
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        axi_adrv9102_tdd_status((void *)fpga910x, AXI_ADRV9102_ID, &tddStatus);
        if (tddStatus == 0)
        {
            break;
        }
        else
        {
            halError = adi_common_hal_Wait_us(&fpga910x->common, interval_us);

            ADI_ERROR_REPORT(&fpga910x->common,
                ADI_COMMON_ERR_API_FAIL,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in ArmCmdStatusWait()");
            ADI_ERROR_RETURN(fpga910x->common.error.newAction);
        }
    }

    if (tddStatus != 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            tddStatus,
            "Unable to stop TDD. Function adi_fpga910x_Tdd_Stop() has failed");
        ADI_API_RETURN(fpga910x);
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Tdd_Stop(adi_fpga910x_Device_t *fpga910x)
{
	uint32_t tddStatus = 0;
   
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	axi_adrv9102_tdd_status(fpga910x, AXI_ADRV9102_ID, &tddStatus);

	if (tddStatus == 1)
	{
		axi_adrv9102_tdd_stop((void *)fpga910x, AXI_ADRV9102_ID);
		ADI_EXPECT(adi_fpga910x_Tdd_StartBit_Inspect, fpga910x);
	}
	ADI_API_RETURN(fpga910x);

}
