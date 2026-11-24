/**
* \file
* \brief FPGA910X Multi-Chip Synchronization (MCS) functions
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/
#include "adi_adrv910x_user.h"
#include "adi_fpga910x_mcs.h"
#include "adi_fpga910x_datachain.h"
#include "axi_adrv9102.h"
#include "adrv9102_zcu102.h"
#include "fpga910x_utilities.h"

static __maybe_unused int32_t adi_fpga910x_Mcs_Configure_Validate(adi_fpga910x_Device_t *fpga910x, 
                                                   adi_fpga910x_McsCfg_t *mcsCfg)
{
    ADI_RANGE_CHECK(fpga910x, mcsCfg->numberOfPulses, 1, 16);
    ADI_RANGE_CHECK(fpga910x, mcsCfg->mcsPulseWidth, 1, 16);
    ADI_RANGE_CHECK(fpga910x, mcsCfg->edge, ADI_FPGA910X_MCS_EDGE_RISING, ADI_FPGA910X_MCS_EDGE_FALLING);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Mcs_Configure(adi_fpga910x_Device_t *fpga910x, 
                                   adi_fpga910x_McsCfg_t *mcsCfg)
{
    uint32_t status = -1;
    axi_adrv9102_mcs_params_t mcs_params = { 0 };
    ADI_PERFORM_VALIDATION(adi_fpga910x_Mcs_Configure_Validate, fpga910x, mcsCfg);

    axi_adrv9102_mcs_status(fpga910x, AXI_ADRV9102_ID, &status);
    if (status != 0) 
    {
        ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
        ADI_COMMON_ACT_ERR_RESET_FEATURE, 0, "previous MCS sequence pending.");
        ADI_ERROR_RETURN(fpga910x->common.error.newAction);
    }

    mcs_params.period = mcsCfg->mcsPeriod - 1;
    mcs_params.width  = mcsCfg->mcsPulseWidth - 1;
    mcs_params.count  = mcsCfg->numberOfPulses - 1;
    mcs_params.edge = (uint32_t)mcsCfg->edge;
    mcs_params.select = AXI_ADRV9102_TOP_MCS_SEQUENCE;

    axi_adrv9102_mcs_config((void *)fpga910x, AXI_ADRV9102_ID, &mcs_params);

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Mcs_SsiConfigure(adi_fpga910x_Device_t *fpga910x)
{
   
    axi_adrv9102_top_reg_mcs_tx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x0);
    axi_adrv9102_top_reg_mcs_rx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x0);
    
    axi_adrv9102_top_mcs_timer_delay_us(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 100);
    axi_adrv9102_top_reg_mcs_tx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);
    axi_adrv9102_top_mcs_timer_delay_us(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 100);
    axi_adrv9102_top_reg_mcs_rx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Mcs_Start(adi_fpga910x_Device_t *fpga910x)
{
    uint32_t status = -1;
    uint32_t timeout_us = -1;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    axi_adrv9102_mcs_start(fpga910x, AXI_ADRV9102_ID);
    axi_adrv9102_top_mcs_timer_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, timeout_us);
    while (status != 0)
    {
        axi_adrv9102_mcs_status(fpga910x, AXI_ADRV9102_ID, &status);
        axi_adrv9102_top_mcs_timer_get(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, &timeout_us);
        if (timeout_us == 0)
        {
            ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_RESET_FEATURE, 0, "MCS sequence incomplete, timed-out.");
            ADI_ERROR_RETURN(fpga910x->common.error.newAction);
        }
    }
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Mcs_RxMcsToStrobeSampleLatency_Get (adi_fpga910x_Device_t *fpga910x, adi_common_ChannelNumber_e channel, uint32_t *latency)
{
    uint32_t ssiId = 0;
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(&fpga910x->common, latency);
    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_RX, channel);
    axi_adrv9102_ssi_mcs_latency_count_get(fpga910x, AXI_ADRV9102_ID, ssiId, latency);
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Mcs_Done_Get (adi_fpga910x_Device_t *fpga910x, bool *mcsDone)
{
    uint32_t status = -1;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(&fpga910x->common, mcsDone);
    axi_adrv9102_mcs_status(fpga910x, AXI_ADRV9102_ID, &status);
    *mcsDone = (status == 0);

    ADI_API_RETURN(fpga910x);
}
