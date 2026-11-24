/**
 * \file
 * \brief Contains top level fpga910x MMCM related functions
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
 * Copyright 2019 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "adi_fpga910x_clock.h"
#include "fpga910x_utilities.h"

static __maybe_unused int32_t adi_fpga910x_clock_Status_Get_Validate(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_ClockStatus_t *status)
{
    ADI_NULL_PTR_RETURN(fpga910x, status);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_clock_Status_Get(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_ClockStatus_t *status)
{
    uint32_t unlocked = 0;
    uint32_t value = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_clock_Status_Get_Validate, fpga910x, status);
    
    axi_adrv9102_top_mmcm_unlocked_get(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &unlocked);
    status->locked = !(bool)(unlocked & 0x1);
 
    axi_adrv9102_top_mcs_clk_mon_count_freq_hz(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &value);
    status->mcsClock_kHz = value / 1000;
    axi_adrv9102_gpio_clk_mon_count_freq_hz(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &value);
    status->observationGpiosClock_kHz = value / 1000;
    axi_adrv9102_tdd_clk_mon_count_freq_hz(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &value);
    status->tddClock_kHz = value / 1000;
    axi_adrv9102_top_mmcm_ref_clk_mon_count_freq_hz(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &value);
    status->refClock_kHz = value / 1000;
    
    ADI_API_RETURN(fpga910x);
}
