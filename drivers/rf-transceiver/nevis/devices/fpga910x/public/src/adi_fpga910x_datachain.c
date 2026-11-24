/**
 * \file
 *
 * \brief Functions to configure the FPGA910X Data Chains
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
 * Copyright 2022 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include <stdlib.h>
#include <time.h>

#include "adi_adrv910x_user.h"
#include "adi_fpga910x_datachain.h"
#include "adi_fpga910x_gpio.h"
#include "adi_fpga910x_hal_wrapper.h"
#include "adi_fpga910x_ssi.h"
#include "axi_mspi.h"
#include "axi_dma.h"
#include "axi_adrv9102.h"
#include "axi_common.h"
#include "fpga910x_utilities.h"

int32_t adi_fpga910x_DataChain_Dma_Inspect(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId = 0;
    uint32_t value = 0;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, dmaCfg);

    dmaId = fpga910x_DMAIdGet(fpga910x, port, channel);
    axi_dma_top_length_get(fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaCfg->length);
    axi_dma_top_cyclic_get(fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &value);
    dmaCfg->continuous = (value == 1) ? true : false;
    axi_dma_top_reg_flags_get(fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &value);
    dmaCfg->dmaEnableSync = (value == 0x3) ? true : false;
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_DmaStatus_Get(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_fpga910x_DmaStatus_t *status)
{
    uint32_t dmaId;
    uint32_t value = 0;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, status);

    dmaId = fpga910x_DMAIdGet(fpga910x, port, channel);
    axi_dma_top_ovf_unf_get((void *)fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &value);
    status->overUnderflow = (value == 1) ? true : false;
    axi_dma_top_busy_get((void *)fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &value);
    status->active = (value == 1) ? true : false;
    status->complete = ~status->active;
    status->haltComplete = ~status->active;
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_PerformTransfer_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                              adi_common_Port_e port,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_fpga910x_DmaCfg_t *dmaCfg)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(fpga910x, dmaCfg->trigger, ADI_FPGA910X_DMA_TRIGGER_SMA_1, ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE);
    ADI_RANGE_CHECK(fpga910x, dmaCfg->captureType, ADI_FPGA910X_CAPTURE_TYPE_DISABLE, ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN);
    if ((dmaCfg->captureType == ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN) && (ADI_RX != port))
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            port,
            "Invalid port selected. Gain index capture can be performed only for Rx channels.");

    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_ConfigureTransfer(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint32_t dmaId,
                                                        adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t addr = 0;
    axi_dma_params_t dmaParams = { 0 };
    
    if (dmaCfg->trigger == ADI_FPGA910X_DMA_TRIGGER_SYNC)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_EDGE;
    }
    else if (dmaCfg->trigger != ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_RISING_EDGE;
    }
    else {
        dmaParams.triggers[(int) dmaCfg->trigger] = 0;		
    }

    if (ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN == dmaCfg->captureType)
    {
        if (ADI_CHANNEL_1 == channel)
        {
            ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, 
                       fpga910x,
                       ADI_FPGA910X_HAL_DMA_RX2,
                       &addr);
        }
        else
        {
            ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, 
                       fpga910x,
                       ADI_FPGA910X_HAL_DMA_RX1,
                       &addr);
        }
    }
    else
    {
        ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, 
                   fpga910x,
                   fpga910x_Dma_Get(fpga910x, port, channel),
                   &addr);
    }

    if (ADI_TX == port)
    {
        axi_dma_top_length_get((void *)fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaParams.length);
    }
    else
    {
        dmaParams.length = dmaCfg->length;
    }
    /* '(port == ADI_TX) ? 1 : 0;' moved to adi_fpga910x_DataChain_PerformSynchronousTransfer and 
     * adi_fpga910x_DataChain_PerformTransfer.
     */
    dmaParams.cyclic = (uint8_t)dmaCfg->continuous;
    dmaParams.qthreshold = 0x2;
    dmaParams.start_address = (uint64_t) addr;
    dmaParams.cyclic_sync = (dmaCfg->dmaEnableSync) ? 1 : 0;
    dmaParams.start_sync = (dmaCfg->dmaEnableSync) ? 1 : 0;

    ADI_EXPECT(axi_dma_config, fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaParams);
    ADI_EXPECT(axi_dma_begin, fpga910x, dmaId, AXI_DMA_TOP_OFFSET);

    ADI_API_RETURN(fpga910x);
}

/* most likely there will be a need for a dmaId version of this function in the future */
/* lazy approach here, ideally you want enumerated ids pushed upstream */

int32_t adi_fpga910x_DataChain_Dma_Configure(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId;

    dmaId = fpga910x_DMAIdGet(fpga910x, port, channel);
    return(adi_fpga910x_DataChain_ConfigureTransfer(fpga910x, port, channel, dmaId, dmaCfg));
}

static __maybe_unused int32_t adi_fpga910x_DataChain_CaptureWait(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  uint32_t dmaId,
                                                  adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t timeout_us = 0;

    timeout_us = KILO_TO_BASE_UNIT(dmaCfg->timeout_ms);
    ADI_EXPECT(axi_dma_finish, fpga910x, dmaId, AXI_DMA_TOP_OFFSET, timeout_us);
    ADI_API_RETURN(fpga910x); 
}  

int32_t adi_fpga910x_DataChain_Dma_Master_Set(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             bool enableMasterToEnable,
                                             bool enableMasterToTrigger)
{
    uint32_t ssiId0 = fpga910x_SsiIdGet(fpga910x, port, ADI_CHANNEL_1);
    uint32_t ssiId1 = fpga910x_SsiIdGet(fpga910x, port, ADI_CHANNEL_2);
    uint32_t dmaId0 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_1);
    uint32_t dmaId1 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_2);
    uint32_t dataEnable = (enableMasterToEnable == true) ? 0x1: 
                                                0x0;
    uint32_t dataTrigger = (enableMasterToTrigger == true) ? 0x1: 
                                                0x0;
    /* Enablenedness of Channel1 DMA_ENABLE as master for Channel2 */
    axi_adrv9102_ssi_master_enable_set(fpga910x, AXI_ADRV9102_ID, ssiId0, dataEnable);
    axi_adrv9102_ssi_master_enable_set(fpga910x, AXI_ADRV9102_ID, ssiId1, dataEnable);

    /* Enablenedness of Channel1 DMA_TRIGGER as master for Channel2 */
    axi_dma_top_master_trigger_set(fpga910x, dmaId0, AXI_DMA_TOP_OFFSET, dataTrigger);
    axi_dma_top_master_trigger_set(fpga910x, dmaId1, AXI_DMA_TOP_OFFSET, dataTrigger);

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Dma_Master_Get(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_Port_e port,
                                             bool *enableMasterToEnable,
                                             bool *enableMasterToTrigger)
{
    uint32_t ssiId0 = fpga910x_SsiIdGet(fpga910x, port, ADI_CHANNEL_1);
    uint32_t ssiId1 = fpga910x_SsiIdGet(fpga910x, port, ADI_CHANNEL_2);
    uint32_t dmaId0 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_1);
    uint32_t dmaId1 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_2);
    uint32_t dataEnable = 0;
    uint32_t dataTrigger = 0;
    /* Enablenedness of Channel1 DMA_ENABLE as master for Channel2 */
    axi_adrv9102_ssi_master_enable_get(fpga910x, AXI_ADRV9102_ID, ssiId0, &dataEnable);
    if (dataEnable == 1)
    {
        axi_adrv9102_ssi_master_enable_get(fpga910x, AXI_ADRV9102_ID, ssiId1, &dataEnable);
    }

    /* Enablenedness of Channel1 DMA_TRIGGER as master for Channel2 */
    axi_dma_top_master_trigger_get(fpga910x, dmaId0, AXI_DMA_TOP_OFFSET, &dataTrigger);
    if (dataTrigger == 1)
    {
        axi_dma_top_master_trigger_get(fpga910x, dmaId1, AXI_DMA_TOP_OFFSET, &dataTrigger);
    }
        
    *enableMasterToEnable = (dataEnable == 0) ? false :
                                                    true;
    *enableMasterToTrigger = (dataTrigger == 0) ? false :
                                                    true;
        
    ADI_API_RETURN(fpga910x);
}

/* same as the dma-configure above, but waits, no dmaId version. */

int32_t adi_fpga910x_DataChain_Dma_Wait(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_Port_e port,
                                        adi_common_ChannelNumber_e channel,
                                        adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId;

    dmaId = fpga910x_DMAIdGet(fpga910x, port, channel);
    return(adi_fpga910x_DataChain_CaptureWait(fpga910x, port, channel, dmaId, dmaCfg));
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Sync(adi_fpga910x_Device_t *fpga910x)
{
    uint32_t status = 0;

    /* Issue an MCS pulse to align two DMA channels */
    axi_adrv9102_top_mcs_trigger_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);
    axi_adrv9102_top_mcs_cdc_state_sync(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET);

    ADI_API_RETURN(fpga910x);
}
 
/* A synchronous transfer won't prevent the selection of any of the four trigger sources.
 * Unlike adi_fpga910x_DataChain_PerformTransfer, if the trigger is set to immediate
 * then FMC.SMA, rising edge, will be selected as the trigger source.
 */
int32_t adi_fpga910x_DataChain_PerformSynchronousTransfer(adi_fpga910x_Device_t *fpga910x,
                                                          adi_common_Port_e port,
                                                          adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaIdChannel1;
    uint32_t dmaIdChannel2;
    /* ADI_CHANNEL_1 is used for validation function. For synchronousTransfer we just want to validate port
     * and dmaCfg, since they are provided by the user. No need to verify the channel number.
     */
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_PerformTransfer_Validate, fpga910x, port, ADI_CHANNEL_1, dmaCfg);
    /* Verify that the capture type is not ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN */
    if (ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN == dmaCfg->captureType)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            dmaCfg->captureType,
            "Invalid capture type for synchronous transfer");
        ADI_ERROR_RETURN(fpga910x->common.error.newAction);
    }
    dmaIdChannel1 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_1);
    dmaIdChannel2 = fpga910x_DMAIdGet(fpga910x, port, ADI_CHANNEL_2);     
    /* Set trigger to SYNC (MCS signal as source) if the user has selected IMMEDIATE */
    if (ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE == dmaCfg->trigger)
    {
        dmaCfg->trigger = ADI_FPGA910X_DMA_TRIGGER_SYNC;
   }
    
    /* If the user has set the trigger mode to immediate, then */                                                     
    /* Setup DMA for channel 1 and 2 for specified port */
    ADI_EXPECT(adi_fpga910x_DataChain_ConfigureTransfer, fpga910x, port, ADI_CHANNEL_1, dmaIdChannel1, dmaCfg);
    ADI_EXPECT(adi_fpga910x_DataChain_ConfigureTransfer, fpga910x, port, ADI_CHANNEL_2, dmaIdChannel2, dmaCfg);

    /* Synchronize capture with MCS pulse */
    ADI_EXPECT(adi_fpga910x_DataChain_Sync, fpga910x);

    /* Poll status of DMA if the transfer type is not cyclic.
       Potentially only need to call captureWait once, however both are called in case there is a
       time out error with one of the DMAs.
     */
    if (false == dmaCfg->continuous)
    {
        ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, port, ADI_CHANNEL_1, dmaIdChannel1, dmaCfg);
        ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, port, ADI_CHANNEL_2, dmaIdChannel2, dmaCfg);
    }

    ADI_API_RETURN(fpga910x);
}

/* trigger select here is re-interpreted as follows. */
/* hardware supports four trigger sources. */
/* 1. CARRIER.SMA.0 (In ZC706, this is J67). */
/* 2. CARRIER.SMA.1 (In ZC706, this is J68). */
/* 3. FMC.SMA (In ADRV910X, this is MCS (this can also be sourced by fpga910x)). */
/* 4. fpga910x.GPIO (In ADRV910X, this is any one of the digital GPIO.) */
/* Each source can be either disabled (if all are disabled, it is immediate) */
/* Individually rising edge OR falling edge (combination is allowed). */
/* Immediate Trigger                : all trigger sources are disabled. */
/* ADI_FPGA910X_TDD_TRIGGER_SMA_1   : CARRIER.SMA.0 is selected, rising edge. */
/* ADI_FPGA910X_TDD_TRIGGER_SMA_2   : CARRIER.SMA.1 is selected, rising edge. */
/* ADI_FPGA910X_TDD_TRIGGER_MCS     : FMC.SMA is selected, rising edge */
/* ADI_FPGA910X_TDD_TRIGGER_GPIO    : fpga910x.GPIO is selected, rising edge. */
/* Also, setting a trigger source while DMA is in progress will result in UNKNOWN  behavior. */
/* It is certain that data integrity is lost, as data is disabled until trigger occurs. */
int32_t adi_fpga910x_DataChain_PerformTransfer(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_PerformTransfer_Validate, fpga910x, port, channel, dmaCfg);
    /* Determine DMA Id */
    dmaId = (ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN == dmaCfg->captureType) ? AXI_DMA_GAIN_ID :
                                                                         fpga910x_DMAIdGet(fpga910x, port, channel);
    /* Configure DMA */
    ADI_EXPECT(adi_fpga910x_DataChain_ConfigureTransfer, fpga910x, port, channel, dmaId, dmaCfg);
    /* Poll status of DMA if the transfer type is not cyclic */
    if (false == dmaCfg->continuous)
    {
        ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, port, channel, dmaId, dmaCfg);
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_PerformTransfer_Start(adi_fpga910x_Device_t *fpga910x,
                                                     adi_common_Port_e port,
                                                     adi_common_ChannelNumber_e channel,
                                                     adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_PerformTransfer_Validate, fpga910x, port, channel, dmaCfg);
    /* Determine DMA Id */
    dmaId = (ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN == dmaCfg->captureType) ? AXI_DMA_GAIN_ID :
                                                                         fpga910x_DMAIdGet(fpga910x, port, channel);
    /* Configure DMA */
    ADI_EXPECT(adi_fpga910x_DataChain_ConfigureTransfer, fpga910x, port, channel, dmaId, dmaCfg);

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_PerformTransfer_Finish(adi_fpga910x_Device_t *fpga910x,
                                                      adi_common_Port_e port,
                                                      adi_common_ChannelNumber_e channel,
                                                      adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_PerformTransfer_Validate, fpga910x, port, channel, dmaCfg);
    /* Determine DMA Id */
    dmaId = (ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN == dmaCfg->captureType) ? AXI_DMA_GAIN_ID :
                                                                         fpga910x_DMAIdGet(fpga910x, port, channel);

    /* Poll status of DMA if the transfer type is not cyclic */
    if (false == dmaCfg->continuous)
    {
        ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, port, channel, dmaId, dmaCfg);
    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_HopTable_Transfer_Validate(adi_fpga910x_Device_t *device,
                                                                  adi_fpga910x_DmaCfg_t *dmaCfg)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_RANGE_CHECK(device, dmaCfg->trigger, ADI_FPGA910X_DMA_TRIGGER_SMA_1, ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE);
    ADI_API_RETURN(device);
}

int32_t adi_fpga910x_DataChain_HopTable_Transfer_Start(adi_fpga910x_Device_t *fpga910x,
                                                       adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t dmaId = 0;
    uint32_t addr = 0;
    axi_dma_params_t dmaParams = { 0 };

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_HopTable_Transfer_Validate, fpga910x, dmaCfg);

    if (dmaCfg->trigger == ADI_FPGA910X_DMA_TRIGGER_SYNC)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_EDGE;
    }
    else if (dmaCfg->trigger != ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_RISING_EDGE;
    }
    else
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = 0;
    }
    
    dmaId = AXI_DMA_MOSI_ID;
    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, ADI_FPGA910X_HAL_DMA_SPI, &addr);
    dmaParams.start_address = (uint64_t) addr;
    dmaParams.length = dmaCfg->length;
    dmaParams.cyclic = dmaCfg->continuous;
    dmaParams.qthreshold = 0x0;
    dmaParams.cyclic_sync = 0x0;
    dmaParams.start_sync = 0x0;

    ADI_EXPECT(axi_dma_config, fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaParams);
    ADI_EXPECT(axi_dma_begin, fpga910x, dmaId, AXI_DMA_TOP_OFFSET);

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_HopTable_Transfer_Stop(adi_fpga910x_Device_t *fpga910x,
                                                      uint32_t timeout_ms)
{
    uint32_t timeout_us = 0;
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    timeout_us = KILO_TO_BASE_UNIT(timeout_ms);
    
    ADI_EXPECT(axi_dma_finish, fpga910x, AXI_DMA_MOSI_ID, AXI_DMA_TOP_OFFSET, timeout_us);
    ADI_EXPECT(axi_mspi_dma_finish, fpga910x, AXI_MSPI_ID, timeout_us);

    ADI_API_RETURN(fpga910x); 
}
static __maybe_unused int32_t adi_fpga910x_DataChain_Mspi_Dma_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                  adi_fpga910x_DmaTrigger_e trigger,
                                                                  uint32_t bytesPerTable,
                                                                  uint32_t framesPerTable)
{
    ADI_RANGE_CHECK(fpga910x, trigger, ADI_FPGA910X_DMA_TRIGGER_SMA_1, ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE);
    if (bytesPerTable <= 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            bytesPerTable,
            "bytesPerTable must be a positive integer.");
        ADI_ERROR_RETURN(fpga910x->common.error.newAction);
    }
    if (framesPerTable <= 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            framesPerTable,
            "framesPerTable must be a positive integer.");
        ADI_ERROR_RETURN(fpga910x->common.error.newAction);
    }
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Mspi_Dma_Configure(adi_fpga910x_Device_t *fpga910x,
                                                  adi_fpga910x_DmaTrigger_e trigger,
                                                  uint32_t bytesPerTable,
                                                  uint32_t framesPerTable)
{
    uint32_t mspiTriggerMode = 0;
    uint32_t timeout_us = 1000000 * 10;
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Mspi_Dma_Configure_Validate, fpga910x, trigger, bytesPerTable, framesPerTable);
    

    axi_mspi_top_reg_triggers_set(fpga910x, AXI_MSPI_ID, 0x0);
    axi_mspi_top_enable_set(fpga910x, AXI_MSPI_ID, 0x0);
    axi_mspi_finish(fpga910x, AXI_MSPI_ID, timeout_us);

    axi_mspi_slave_select_set(fpga910x, AXI_MSPI_ID, 0x1);
    if (trigger == ADI_FPGA910X_DMA_TRIGGER_SYNC) {
        axi_mspi_top_trigger_mode_set(fpga910x, AXI_MSPI_ID, trigger, AXI_MSPI_TOP_TRIGGER_EDGE);
    }
    else if (trigger != ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE) {
        axi_mspi_top_trigger_mode_set(fpga910x, AXI_MSPI_ID, trigger, AXI_MSPI_TOP_TRIGGER_RISING_EDGE);
    }
    axi_mspi_dma_config(fpga910x, AXI_MSPI_ID, (bytesPerTable - 1), (framesPerTable - 1));

    ADI_API_RETURN(fpga910x);

}

int32_t adi_fpga910x_DataChain_Mspi_Fifo_Configure(adi_fpga910x_Device_t *fpga910x)
{
    
    uint32_t timeout_us = 1000000 * 10;
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    axi_mspi_top_enable_set(fpga910x, AXI_MSPI_ID, 0x0);
    axi_mspi_finish(fpga910x, AXI_MSPI_ID, timeout_us);
    
    axi_mspi_top_reg_triggers_set(fpga910x, AXI_MSPI_ID, 0x0);
    axi_mspi_slave_select_set(fpga910x, AXI_MSPI_ID, 0x0);
    axi_mspi_fifo_config(fpga910x, AXI_MSPI_ID);
    
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_22I22Q_GainChange_GainIndexValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                   adi_common_Port_e port,
                                                                                   adi_common_ChannelNumber_e channel,
                                                                                   uint32_t iData[],
                                                                                   uint32_t qData[],
                                                                                   uint8_t gainChangeData[],
                                                                                   uint8_t gainIndexData[],
                                                                                   uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndexData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length * 2, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_22I22Q_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                    adi_common_Port_e port,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint32_t iData[],
                                                                    uint32_t qData[],
                                                                    uint8_t gainChangeData[],
                                                                    uint8_t gainIndexData[],
                                                                    uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES * 2] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_22I22Q_GainChange_GainIndexValidate, fpga910x, port, channel, iData, qData, gainChangeData, gainIndexData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length * 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length * 2);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    }
    
    for (i = 0; i < length; i++)
    {
        iData[i]          = (uint32_t)((rdBuff[2 * i + 1] & 0xFFFFFC00) >> 10); /* I =  rdBuff0[31:10] */
        qData[i]          = (uint32_t)((rdBuff[2 * i + 0] & 0xFFFFFC00) >> 10); /* Q =  rdBuff1[31:10] */
        gainChangeData[i] = (uint8_t) ((rdBuff[2 * i + 0] & 0x100)      >>  8); /* GD = rdBuff0[8] */
        gainIndexData[i]  = (uint8_t) ((rdBuff[2 * i + 0] & 0xFF)       >>  0); /* GI = rdBuff0[7:0] */
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_22I22QSigned_GainChange_GainIndexValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                         adi_common_Port_e port,
                                                                                         adi_common_ChannelNumber_e channel,
                                                                                         int32_t iData[],
                                                                                         int32_t qData[],
                                                                                         uint8_t gainChangeData[],
                                                                                         uint8_t gainIndexData[],
                                                                                         uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndexData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length * 2, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_22I22QSigned_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                          adi_common_Port_e port,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          int32_t iData[],
                                                                          int32_t qData[],
                                                                          uint8_t gainChangeData[],
                                                                          uint8_t gainIndexData[],
                                                                          uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t positiveFullScale = 1 << 21;
    int32_t fullScaleRange = 1 << 22;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES * 2] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_22I22QSigned_GainChange_GainIndexValidate, fpga910x, port, channel, iData, qData, gainChangeData, gainIndexData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length * 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length * 2);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    }
    
    for (i = 0; i < length; i++)
    {
        iData[i]          = (int32_t)((rdBuff[2 * i + 1] & 0xFFFFFC00) >> 10); /* I =  rdBuff0[31:10] */
        iData[i]          = iData[i] > positiveFullScale ? iData[i] - fullScaleRange : iData[i];
        qData[i]          = (int32_t)((rdBuff[2 * i + 0] & 0xFFFFFC00) >> 10); /* Q =  rdBuff1[31:10] */
        qData[i]          = qData[i] > positiveFullScale ? qData[i] - fullScaleRange : qData[i];
        gainChangeData[i] = (uint8_t)((rdBuff[2 * i + 0] & 0x100)      >>  8); /* GD = rdBuff0[8] */
        gainIndexData[i]  = (uint8_t)((rdBuff[2 * i + 0] & 0xFF)       >>  0); /* GI = rdBuff0[7:0] */
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleaved_GainChange_GainIndexValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                              adi_common_Port_e port,
                                                                                              adi_common_ChannelNumber_e channel,
                                                                                              uint32_t iqData[],
                                                                                              uint8_t gainChangeData[],
                                                                                              uint8_t gainIndexData[],
                                                                                              uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndexData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length * 2, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
    
}

int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleaved_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                               adi_common_Port_e port,
                                                                               adi_common_ChannelNumber_e channel,
                                                                               uint32_t iqData[],
                                                                               uint8_t gainChangeData[],
                                                                               uint8_t gainIndexData[],
                                                                               uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES * 2] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_22I22QInterleaved_GainChange_GainIndexValidate, fpga910x, port, channel, iqData, gainChangeData, gainIndexData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length * 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length * 2);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    }
    
    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i + 0]         = (uint32_t)((rdBuff[2 * i + 1] & 0xFFFFFC00) >> 10); /* I =  rdBuff0[31:10] */
        iqData[2 * i + 1]         = (uint32_t)((rdBuff[2 * i + 0] & 0xFFFFFC00) >> 10); /* Q =  rdBuff1[31:10] */
        gainChangeData[2 * i + 0] = (uint8_t) ((rdBuff[2 * i + 0] & 0x100)      >> 8); /* GD = rdBuff0[8] */
        gainIndexData[2 * i + 0]  = (uint8_t)((rdBuff[2 * i + 0] & 0xFF)        >> 0); /* GI = rdBuff0[7:0] */
        
        gainChangeData[2 * i + 1] = gainChangeData[2 * i + 0];
        gainIndexData[2 * i + 1]  = gainIndexData[2 * i + 0];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleavedSigned_GainChange_GainIndexValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                                    adi_common_Port_e port,
                                                                                                    adi_common_ChannelNumber_e channel,
                                                                                                    int32_t iqData[],
                                                                                                    uint8_t gainChangeData[],
                                                                                                    uint8_t gainIndexData[],
                                                                                                    uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainIndexData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length * 2, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
    
}

int32_t adi_fpga910x_DataChain_Data_Get_22I22QInterleavedSigned_GainChange_GainIndex(adi_fpga910x_Device_t *fpga910x,
                                                                                     adi_common_Port_e port,
                                                                                     adi_common_ChannelNumber_e channel,
                                                                                     int32_t iqData[],
                                                                                     uint8_t gainChangeData[],
                                                                                     uint8_t gainIndexData[],
                                                                                     uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int32_t positiveFullScale = 1 << 21;
    int32_t fullScaleRange = 1 << 22;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES * 2] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_22I22QInterleavedSigned_GainChange_GainIndexValidate, fpga910x, port, channel, iqData, gainChangeData, gainIndexData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length * 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length * 2);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    }
    
    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i + 0]         = (int32_t)((rdBuff[2 * i + 1] & 0xFFFFFC00) >> 10); /* I =  rdBuff0[31:10] */
        iqData[2 * i + 0]         = iqData[2 * i + 0] > positiveFullScale ? iqData[2 * i + 0] - fullScaleRange : iqData[2 * i + 0];
        iqData[2 * i + 1]         = (int32_t)((rdBuff[2 * i + 0] & 0xFFFFFC00) >> 10); /* Q =  rdBuff1[31:10] */
        iqData[2 * i + 1]         = iqData[2 * i + 1] > positiveFullScale ? iqData[2 * i + 1] - fullScaleRange : iqData[2 * i + 1];
        gainChangeData[2 * i + 0] = (uint8_t)((rdBuff[2 * i + 0] & 0x100)      >> 8); /* GD = rdBuff0[8] */
        gainIndexData[2 * i + 0]  = (uint8_t)((rdBuff[2 * i + 0] & 0xFF)        >> 0); /* GI = rdBuff0[7:0] */
        
        gainChangeData[2 * i + 1] = gainChangeData[2 * i + 0];
        gainIndexData[2 * i + 1]  = gainIndexData[2 * i + 0];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16I16QValidate(adi_fpga910x_Device_t *fpga910x,
                                                              adi_common_Port_e port,
                                                              adi_common_ChannelNumber_e channel,
                                                              uint16_t *iData,
                                                              uint16_t *qData,
                                                              uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16I16Q(adi_fpga910x_Device_t *fpga910x,
                                                           adi_common_Port_e port,
                                                           adi_common_ChannelNumber_e channel,
                                                           uint16_t *iData,
                                                           uint16_t *qData,
                                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16I16QValidate, fpga910x, port, channel, iData, qData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) -1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length; i++)
    {
        iData[i] = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        qData[i] = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16I16QSignedValidate(adi_fpga910x_Device_t *fpga910x,
                                                                    adi_common_Port_e port,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    int16_t *iData,
                                                                    int16_t *qData,
                                                                    uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16I16QSigned(adi_fpga910x_Device_t *fpga910x,
                                                     adi_common_Port_e port,
                                                     adi_common_ChannelNumber_e channel,
                                                     int16_t iData[],
                                                     int16_t qData[],
                                                     uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t positiveFullScale = 0x8000; // 2^15
    uint32_t fullScaleRange = 0x10000;   // 2^16
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16I16QSignedValidate, fpga910x, port, channel, iData, qData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length; i++)
    {
        iData[i] = (int16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        iData[i] = iData[i] > positiveFullScale ? iData[i] - fullScaleRange : iData[i];
        qData[i] = (int16_t)(rdBuff[i]  & 0x0000FFFF);
        qData[i] = qData[i] > positiveFullScale ? qData[i] - fullScaleRange : qData[i];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleavedValidate(adi_fpga910x_Device_t *fpga910x,
                                                                       adi_common_Port_e port,
                                                                       adi_common_ChannelNumber_e channel,
                                                                       uint16_t *iqData,
                                                                       uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleaved(adi_fpga910x_Device_t *fpga910x,
                                                                    adi_common_Port_e port,
                                                                    adi_common_ChannelNumber_e channel,
                                                                    uint16_t *iqData,
                                                                    uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16IQInterleavedValidate, fpga910x, port, channel, iqData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) -1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i]     = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        iqData[2 * i + 1] = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleavedSignedValidate(adi_fpga910x_Device_t *fpga910x,
                                                                             adi_common_Port_e port,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             int16_t *iqData,
                                                                             uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(device, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16IQInterleavedSigned(adi_fpga910x_Device_t *fpga910x,
                                                              adi_common_Port_e port,
                                                              adi_common_ChannelNumber_e channel,
                                                              int16_t *iqData,
                                                              uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    uint32_t positiveFullScale = 0x8000;  // 2^15
    uint32_t fullScaleRange = 0x10000;    // 2^16
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16IQInterleavedSignedValidate, fpga910x, port, channel, iqData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i]     = (int16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        iqData[2 * i]     = iqData[2 * i] > positiveFullScale ? iqData[2 * i] - fullScaleRange : iqData[2 * i];
        iqData[2 * i + 1] = (int16_t)(rdBuff[i]  & 0x0000FFFF);
        iqData[2 * i + 1] = iqData[2 * i + 1] > positiveFullScale ? iqData[2 * i + 1] - fullScaleRange : iqData[2 * i + 1];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16IValidate(adi_fpga910x_Device_t *fpga910x,
                                                           adi_common_Port_e port,
                                                           adi_common_ChannelNumber_e channel,
                                                           uint16_t *iData,
                                                           uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16I(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t *iData,
                                            uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16IValidate, fpga910x, port, channel, iData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iData[2 * i]     = (uint16_t)(rdBuff[i]  & 0x0000FFFF);
        iData[2 * i + 1] = (uint16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_16ISignedValidate(adi_fpga910x_Device_t *fpga910x,
                                                                 adi_common_Port_e port,
                                                                 adi_common_ChannelNumber_e channel,
                                                                 int16_t *iData,
                                                                 uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_16ISigned(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  int16_t *iData,
                                                  uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    uint32_t positiveFullScale = 0x8000;  // 2^15
    uint32_t fullScaleRange = 0x10000;    // 2^16
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16ISignedValidate, fpga910x, port, channel, iData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iData[2 * i]     = (int16_t)(rdBuff[i]  & 0x0000FFFF);
        iData[2 * i]     = iData[2 * i] > positiveFullScale ? iData[2 * i] - fullScaleRange : iData[2 * i];
        iData[2 * i + 1] = (int16_t)((rdBuff[i] & 0xFFFF0000) >> 16);
        iData[2 * i + 1] = iData[2 * i + 1] > positiveFullScale ? iData[2 * i + 1] - fullScaleRange : iData[2 * i + 1];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_15I15Q_GainChangeValidate(adi_fpga910x_Device_t *fpga910x,
                                                                         adi_common_Port_e port,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         uint16_t iData[],
                                                                         uint16_t qData[],
                                                                         uint8_t gainChangeData[],
                                                                         uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_15I15Q_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                          adi_common_Port_e port,
                                                          adi_common_ChannelNumber_e channel,
                                                          uint16_t iData[],
                                                          uint16_t qData[],
                                                          uint8_t gainChangeData[],
                                                          uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_15I15Q_GainChangeValidate, fpga910x, port, channel, iData, qData, gainChangeData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length; i++)
    {
        iData[i]          = (uint16_t)((rdBuff[i] & 0xFFFE0000) >> 16);
        qData[i]          = (uint16_t)((rdBuff[i] & 0x0000FFFE) >>  0);
        gainChangeData[i] = (uint16_t)((rdBuff[i] & 0x1)        >>  0);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_15I15QSigned_GainChangeValidate(adi_fpga910x_Device_t *fpga910x,
                                                                               adi_common_Port_e port,
                                                                               adi_common_ChannelNumber_e channel,
                                                                               int16_t iData[],
                                                                               int16_t qData[],
                                                                               uint8_t gainChangeData[],
                                                                               uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, qData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 4)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_15I15QSigned_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                adi_common_Port_e port,
                                                                adi_common_ChannelNumber_e channel,
                                                                int16_t iData[],
                                                                int16_t qData[],
                                                                uint8_t gainChangeData[],
                                                                uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t positiveFullScale = 0x8000;   // 2^15
    uint32_t fullScaleRange = 0x10000;     // 2^16
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY


    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_15I15QSigned_GainChangeValidate, fpga910x, port, channel, iData, qData, gainChangeData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    recoveryAction = adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length; i++)
    {
        iData[i]          = (int16_t)((rdBuff[i] & 0xFFFE0000) >> 16);
        iData[i]          = iData[i] > positiveFullScale ? iData[i] - fullScaleRange : iData[i];
        qData[i]          = (int16_t)((rdBuff[i] & 0x0000FFFE) >>  0);
        qData[i]          = qData[i] > positiveFullScale ? qData[i] - fullScaleRange : qData[i];
        gainChangeData[i] = (uint16_t)((rdBuff[i] & 0x1)        >>  0);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleaved_GainChangeValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                    adi_common_Port_e port,
                                                                                    adi_common_ChannelNumber_e channel,
                                                                                    uint16_t iqData[],
                                                                                    uint8_t gainChangeData[],
                                                                                    uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleaved_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                     adi_common_Port_e port,
                                                                     adi_common_ChannelNumber_e channel,
                                                                     uint16_t iqData[],
                                                                     uint8_t gainChangeData[],
                                                                     uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_15IQInterleaved_GainChangeValidate, fpga910x, port, channel, iqData, gainChangeData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i + 0]         = (uint16_t)((rdBuff[i] & 0xFFFE0000) >> 16);
        iqData[2 * i + 1]         = (uint16_t)((rdBuff[i] & 0x0000FFFE) >>  0);
        gainChangeData[2 * i + 0] = (uint16_t)((rdBuff[i] & 0x1)        >>  0);
        gainChangeData[2 * i + 1] = gainChangeData[2 * i + 0];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleavedSigned_GainChangeValidate(adi_fpga910x_Device_t *fpga910x,
                                                                                          adi_common_Port_e port,
                                                                                          adi_common_ChannelNumber_e channel,
                                                                                          int16_t iqData[],
                                                                                          uint8_t gainChangeData[],
                                                                                          uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, iqData);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeData);

#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES / 2)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_15IQInterleavedSigned_GainChange(adi_fpga910x_Device_t *fpga910x,
                                                                           adi_common_Port_e port,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           int16_t iqData[],
                                                                           uint8_t gainChangeData[],
                                                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    uint32_t positiveFullScale = 0x8000;    // 2^15
    uint32_t fullScaleRange = 0x10000;      // 2^16
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_15IQInterleavedSigned_GainChangeValidate, fpga910x, port, channel, iqData, gainChangeData, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 2, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 2) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        iqData[2 * i + 0]         = (int16_t)((rdBuff[i] & 0xFFFE0000) >> 16);
        iqData[2 * i + 0]         = iqData[2 * i + 0] > positiveFullScale ? iqData[2 * i + 0] - fullScaleRange : iqData[2 * i + 0];
        iqData[2 * i + 1]         = (int16_t)((rdBuff[i] & 0x0000FFFE) >>  0);
        iqData[2 * i + 1]         = iqData[2 * i + 1] > positiveFullScale ? iqData[2 * i + 1] - fullScaleRange : iqData[2 * i + 1];
        gainChangeData[2 * i + 0] = (uint16_t)((rdBuff[i] & 0x1)        >>  0);
        gainChangeData[2 * i + 1] = gainChangeData[2 * i + 0];
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataGet_SValidate(adi_fpga910x_Device_t *fpga910x,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              uint8_t *symbols,
                                              uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&fpga910x->common, symbols);
    
#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_GainIndexGet_SValidate(adi_fpga910x_Device_t *fpga910x,
                                                   adi_common_Port_e port,
                                                   adi_common_ChannelNumber_e channel,
                                                   uint8_t *gainChangeBit,
                                                   uint8_t *symbols,
                                                   uint32_t length)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, gainChangeBit);
    ADI_NULL_PTR_RETURN(&fpga910x->common, symbols);
    
#ifndef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_RANGE_CHECK(fpga910x, length, 0, ADI_FPGA910X_MAX_CAPTURE_BYTES)
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_GainIndex_Get_8S(adi_fpga910x_Device_t *fpga910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint8_t *gainChangeBit,
                                                uint8_t *symbols,
                                                uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_GainIndexGet_SValidate, fpga910x, ADI_RX, channel, gainChangeBit, symbols, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 4, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, ADI_RX, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 4) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 4; i++)
    {
        symbols[4 * i]       = (uint8_t)((rdBuff[i] & 0x000000FF) | 0x80);
        gainChangeBit[4 * i] = (uint8_t)((rdBuff[i] & 0x80) >> 7);

        symbols[4 * i + 1]       = (uint8_t)(((rdBuff[i] & 0x0000FF00) >> 8) | 0x80);
        gainChangeBit[4 * i + 1] = (uint8_t)((rdBuff[i] & 0x00008000) >> 15);

        symbols[4 * i + 2]       = (uint8_t)(((rdBuff[i] & 0x00FF0000) >> 16) | 0x80);
        gainChangeBit[4 * i + 2] = (uint8_t)((rdBuff[i] & 0x00800000) >> 23);

        symbols[4 * i + 3]       = (uint8_t)(((rdBuff[i] & 0xFF000000) >> 24) | 0x80);
        gainChangeBit[4 * i + 3] = (uint8_t)((rdBuff[i] & 0x80000000) >> 31);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_8S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataGet_SValidate, fpga910x, port, channel, symbols, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 4, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 4) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 4; i++)
    {
        symbols[4 * i]     = (uint8_t)(rdBuff[i]  & 0x000000FF);
        symbols[4 * i + 1] = (uint8_t)((rdBuff[i] & 0x0000FF00) >> 8);
        symbols[4 * i + 2] = (uint8_t)((rdBuff[i] & 0x00FF0000) >> 16);
        symbols[4 * i + 3] = (uint8_t)((rdBuff[i] & 0xFF000000) >> 24);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Get_2S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *rdBuff = NULL;
#else
    uint32_t rdBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataGet_SValidate, fpga910x, port, channel, symbols, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    rdBuff = calloc(length / 4, sizeof(uint32_t));
    if (rdBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    if (adi_fpga910x_hal_wrapper_Ram_Read(fpga910x, ramAddress, rdBuff, length / 4) != ADI_COMMON_ACT_NO_ACTION)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 4; i++)
    {
        symbols[4 * i]     = (uint8_t)(rdBuff[i]  & 0x00000003);
        symbols[4 * i + 1] = (uint8_t)((rdBuff[i] & 0x00000300) >> 8);
        symbols[4 * i + 2] = (uint8_t)((rdBuff[i] & 0x00030000) >> 16);
        symbols[4 * i + 3] = (uint8_t)((rdBuff[i] & 0x03000000) >> 24);
    }

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(rdBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_DmaLength_Set(adi_fpga910x_Device_t *fpga910x,
                                                    adi_common_Port_e port,
                                                    adi_common_ChannelNumber_e channel,
                                                    uint32_t numBytes)
{
    uint32_t dmaId;

    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ORX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    if (numBytes % 8 != 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM, numBytes,
            "Invalid parameter value, numBytes must be a multiple of 8.");
        ADI_ERROR_RETURN(fpga910x->common.error.newAction);
    }

    dmaId = fpga910x_DMAIdGet(fpga910x, port, channel);
    axi_dma_top_length_set(fpga910x, dmaId, AXI_DMA_TOP_OFFSET, numBytes);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16I16Q(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t *iData,
                                               uint16_t *qData,
                                               uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16I16QValidate, fpga910x, port, channel, iData, qData, length);

    ADI_EXPECT(adi_fpga910x_DataChain_DmaLength_Set, fpga910x, port, channel, length * 4);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    wrBuff = calloc(length, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length; i++)
    {
        wrBuff[i] = (uint32_t)(iData[i] << 16) | (uint32_t)qData[i];
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16I16QSigned(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               adi_common_ChannelNumber_e channel,
                                               int16_t *iData,
                                               int16_t *qData,
                                               uint32_t length)
{
    ADI_EXPECT(adi_fpga910x_DataChain_Data_Set_16I16Q, fpga910x, port, channel, (uint16_t*) iData, (uint16_t*) qData, length);
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16IQInterleaved(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint16_t *iqData,
                                                        uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16IQInterleavedValidate, fpga910x, port, channel, iqData, length);

    ADI_EXPECT(adi_fpga910x_DataChain_DmaLength_Set, fpga910x, port, channel, length * 2);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    wrBuff = calloc(length / 2, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        wrBuff[i] = (uint32_t)(iqData[2 * i] << 16) | (uint32_t)iqData[2 * i + 1];
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length / 2);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16IQInterleavedSigned(adi_fpga910x_Device_t *fpga910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        int16_t *iqData,
                                                        uint32_t length)
{
	ADI_EXPECT(adi_fpga910x_DataChain_Data_Set_16IQInterleaved, fpga910x, port, channel, (uint16_t*) iqData, length);
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16I(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint16_t *iData,
                                            uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_Data_Get_16IValidate, fpga910x, port, channel, iData, length);

    ADI_EXPECT(adi_fpga910x_DataChain_DmaLength_Set, fpga910x, port, channel, length * 2);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    wrBuff = calloc(length / 2, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 2; i++)
    {
        wrBuff[i] = (uint32_t)(iData[2 * i + 1] << 16) | (uint32_t)iData[2 * i];
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length / 2);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_16ISigned(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            int16_t *iData,
                                            uint32_t length)
{
    ADI_EXPECT(adi_fpga910x_DataChain_Data_Set_16I, fpga910x, port, channel, (uint16_t*) iData, length);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_8S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataGet_SValidate, fpga910x, port, channel, symbols, length);

    ADI_EXPECT(adi_fpga910x_DataChain_DmaLength_Set, fpga910x, port, channel, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    wrBuff = calloc(length / 4, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 4; i++)
    {
        wrBuff[i] = (uint32_t)(symbols[4 * i + 3] << 24) |
                    (uint32_t)(symbols[4 * i + 2] << 16) |
                    (uint32_t)(symbols[4 * i + 1] << 8)  |
                    (uint32_t)symbols[4 * i];
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length / 4);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_Data_Set_2S(adi_fpga910x_Device_t *fpga910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *symbols,
                                           uint32_t length)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataGet_SValidate, fpga910x, port, channel, symbols, length);

    ADI_EXPECT(adi_fpga910x_DataChain_DmaLength_Set, fpga910x, port, channel, length);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    wrBuff = calloc(length / 4, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, fpga910x_Dma_Get(fpga910x, port, channel), &ramAddress);
    if (ramAddress == (uint32_t) - 1)
    {
#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
        free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY
        ADI_API_RETURN(fpga910x);
    }

    for (i = 0; i < length / 4; i++)
    {
        wrBuff[i] = (uint32_t)((symbols[4 * i + 3] & 0x03) << 24) |
                    (uint32_t)((symbols[4 * i + 2] & 0x03) << 16) |
                    (uint32_t)((symbols[4 * i + 1] & 0x03) << 8) |
                    (uint32_t)(symbols[4 * i] & 0x03);
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length / 4);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_HopTable_Set_8S_Validate(adi_fpga910x_Device_t *fpga910x,
                                                               uint8_t spiPackedFhTable[],
                                                               uint32_t byteCount)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, spiPackedFhTable);
    ADI_ENTRY_PTR_ARRAY_EXPECT(fpga910x, spiPackedFhTable, byteCount);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_DataChain_HopTable_Set_8S(adi_fpga910x_Device_t *fpga910x,
                                               uint8_t spiPackedFhTable[],
                                               uint32_t byteCount)
{
    uint32_t i = 0;
    uint32_t ramAddress = 0;
    uint32_t length_32 = 0;
    //uint32_t data_32 = 0;

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    uint32_t *wrBuff = NULL;
#else
    uint32_t wrBuff[ADI_FPGA910X_MAX_CAPTURE_BYTES] = { 0 };
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_HopTable_Set_8S_Validate, fpga910x, spiPackedFhTable, byteCount);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    
    length_32 = (byteCount + 3) / 4;

    wrBuff = calloc(length_32, sizeof(uint32_t));
    if (wrBuff == NULL)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         NULL,
                         "Unable to allocate memory");
        ADI_API_RETURN(fpga910x);
    }
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x, ADI_FPGA910X_HAL_DMA_SPI, &ramAddress);
    
    for (i = 0; i < length_32; i++)
    {
        wrBuff[i] = (uint32_t)(spiPackedFhTable[4 * i + 3] << 24) |
                    (uint32_t)(spiPackedFhTable[4 * i + 2] << 16) |
                    (uint32_t)(spiPackedFhTable[4 * i + 1] << 8)  |
                    (uint32_t)spiPackedFhTable[4 * i];
    }

    adi_fpga910x_hal_wrapper_Ram_Write(fpga910x, ramAddress, wrBuff, length_32);

#ifdef ADI_FPGA910X_USE_DYNAMIC_MEMORY
    free(wrBuff);
#endif // ADI_FPGA910X_USE_DYNAMIC_MEMORY

    ADI_API_RETURN(fpga910x);
}

/*===========================================================================*/
/* GPIO-triggered SSI helpers (timing utilities + SsiSend / SsiReceive)       */
/*===========================================================================*/

static uint64_t adi_fpga910x_datachain_TimespecToNs(const struct timespec *ts)
{
    return ((uint64_t)ts->tv_sec * 1000000000ull) + (uint64_t)ts->tv_nsec;
}

static uint32_t adi_fpga910x_datachain_ElapsedMs(const struct timespec *start, const struct timespec *now)
{
    uint64_t startNs = adi_fpga910x_datachain_TimespecToNs(start);
    uint64_t nowNs = adi_fpga910x_datachain_TimespecToNs(now);
    return (uint32_t)((nowNs - startNs) / 1000000ull);
}

/*===========================================================================*/
/* adi_fpga910x_DataChain_SsiSend / SsiReceive                               */
/*===========================================================================*/

/* Hardcoded one-shot DMA configuration - not exposed to the caller. These values match
 * the configuration already exercised by the GS/Drone GPIO-triggered tests. */
#define ADI_FPGA910X_SSI_SEND_DMA_FINISH_TIMEOUT_MS    1000u
#define ADI_FPGA910X_SSI_RECEIVE_DMA_FINISH_TIMEOUT_MS 2000u

/* One-time-per-direction configuration stored by SsiSend_Configure/SsiReceive_Configure and
 * consumed by every subsequent SsiSend/SsiReceive call - channel/triggerPin/timeoutMs don't
 * change frame-to-frame, so there's no reason to re-validate or re-touch the GPIO pin's
 * hardware configuration on every single call. The Rx capture size is NOT stored here - it's
 * derived directly from SsiReceive's own per-call length parameter instead, since the only
 * valid captureWords would always be length/2 anyway; storing both would just be two copies
 * of the same number that could disagree. */
typedef struct
{
    adi_common_ChannelNumber_e channel;
    adi_fpga910x_GpioPin_e     triggerPin;
    uint32_t                   timeoutMs;
    bool                       configured;

    uint32_t                   dmaId; /* set once by *_Configure */

    /* triggerPin's level as of the end of the last WaitAndBegin call (or as sampled by
     * *_Configure for the first call) - carried across calls so a toggle that lands in the
     * gap between one call's detect-and-begin and the next call's poll is still seen as a
     * pending transition, instead of being silently absorbed as the "new resting level". */
    uint32_t                   lastKnownLevel;
} adi_fpga910x_datachain_SsiDirectionConfig_t;

static adi_fpga910x_datachain_SsiDirectionConfig_t g_ssiSendConfig    = { 0 };
static adi_fpga910x_datachain_SsiDirectionConfig_t g_ssiReceiveConfig = { 0 };

/* Shared one-time hardware setup for a GPIO trigger pin: configure it as a software input.
 * Called once from SsiSend_Configure/SsiReceive_Configure, not from every SsiSend/SsiReceive
 * call - idempotent if both directions happen to share the same physical pin. */
static int32_t adi_fpga910x_DataChain_GpioPin_ConfigureInput(adi_fpga910x_Device_t *fpga910x,
                                                              adi_fpga910x_GpioPin_e triggerPin)
{
    adi_fpga910x_GpioPinCfg_t triggerCfg;

    triggerCfg.mode = ADI_FPGA910X_GPIO_MODE_SOFTWARE;
    triggerCfg.direction = ADI_FPGA910X_GPIO_DIRECTION_INPUT;
    triggerCfg.dataOut = ADI_FPGA910X_GPIO_DATAOUT_CLEAR;
    triggerCfg.pin = triggerPin;
    triggerCfg.hwSourceMode = ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_TX1_ENABLE;
    ADI_EXPECT(adi_fpga910x_gpio_pin_Configure, fpga910x, &triggerCfg);

    ADI_API_RETURN(fpga910x);
}

/* Programs the DMA registers but deliberately omits axi_dma_begin -
 * adi_fpga910x_DataChain_SsiTrigger_WaitAndBegin issues that itself the instant it detects
 * the trigger pin's level change, so nothing but the begin is left on that path. Handles
 * only captureType DISABLE/RX_DATA, which is all SsiSend/SsiReceive use. */
static int32_t adi_fpga910x_DataChain_SsiTrigger_ConfigureDmaRegisters(adi_fpga910x_Device_t *fpga910x,
                                                                       adi_common_Port_e port,
                                                                       adi_common_ChannelNumber_e channel,
                                                                       uint32_t dmaId,
                                                                       adi_fpga910x_DmaCfg_t *dmaCfg)
{
    uint32_t addr = 0;
    axi_dma_params_t dmaParams = { 0 };

    if (dmaCfg->trigger == ADI_FPGA910X_DMA_TRIGGER_SYNC)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_EDGE;
    }
    else if (dmaCfg->trigger != ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE)
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = AXI_DMA_TOP_TRIGGER_RISING_EDGE;
    }
    else
    {
        dmaParams.triggers[(int) dmaCfg->trigger] = 0;
    }

    ADI_EXPECT(adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get, fpga910x,
              fpga910x_Dma_Get(fpga910x, port, channel), &addr);

    if (ADI_TX == port)
    {
        axi_dma_top_length_get((void *)fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaParams.length);
    }
    else
    {
        dmaParams.length = dmaCfg->length;
    }
    dmaParams.cyclic = (uint8_t)dmaCfg->continuous;
    dmaParams.qthreshold = 0x2;
    dmaParams.start_address = (uint64_t) addr;
    dmaParams.cyclic_sync = (dmaCfg->dmaEnableSync) ? 1 : 0;
    dmaParams.start_sync = (dmaCfg->dmaEnableSync) ? 1 : 0;

    ADI_EXPECT(axi_dma_config, fpga910x, dmaId, AXI_DMA_TOP_OFFSET, &dmaParams);

    ADI_API_RETURN(fpga910x);
}

/* Samples triggerPin's current level (0 or 1) from the GPIO group register. */
static uint32_t adi_fpga910x_DataChain_SsiTrigger_SampleLevel(adi_fpga910x_Device_t *fpga910x,
                                                               adi_fpga910x_GpioPin_e triggerPin)
{
    uint32_t triggerBit = (uint32_t)triggerPin - 1u;
    uint32_t data = 0u;

    axi_adrv9102_gpio_group_data((void *)fpga910x, AXI_ADRV9102_ID, ADI_FPGA910X_GPIO_GROUP_DGPIO, &data);
    return (data >> triggerBit) & 0x1u;
}

/* Polls triggerPin, blocking the caller until the pin's level changes or cfg->timeoutMs
 * elapses.
 *
 * Compares against cfg->lastKnownLevel (set by *_Configure for the first call, and updated
 * here on every detected change) rather than resampling a fresh baseline each call: a toggle
 * landing between one call's detect-and-begin and the next call's poll must still be seen as
 * a pending transition, not absorbed as the new "resting" level. */
static int32_t adi_fpga910x_DataChain_SsiTrigger_WaitAndBegin(adi_fpga910x_Device_t *fpga910x,
                                                               adi_fpga910x_datachain_SsiDirectionConfig_t *cfg)
{
    struct timespec startTs;
    struct timespec nowTs;
    uint32_t level;

    clock_gettime(CLOCK_MONOTONIC, &startTs);

    do
    {
        level = adi_fpga910x_DataChain_SsiTrigger_SampleLevel(fpga910x, cfg->triggerPin);

        if (level != cfg->lastKnownLevel)
        {
            cfg->lastKnownLevel = level;

            /* The DMA's address/length/trigger registers were already programmed by
             * SsiTrigger_ConfigureDmaRegisters, so the begin is the only thing between
             * detecting the level change and the transfer actually starting. */
            ADI_EXPECT(axi_dma_begin, fpga910x, cfg->dmaId, AXI_DMA_TOP_OFFSET);
            ADI_API_RETURN(fpga910x);
        }

        clock_gettime(CLOCK_MONOTONIC, &nowTs);
    } while (adi_fpga910x_datachain_ElapsedMs(&startTs, &nowTs) < cfg->timeoutMs);

    ADI_ERROR_REPORT(&fpga910x->common,
                     ADI_COMMON_ERRSRC_API,
                     ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL,
                     cfg->triggerPin,
                     "timed out waiting for GPIO trigger level change");

    ADI_API_RETURN(fpga910x);
}

/* Undoes SsiSend_Configure: marks SsiSend unconfigured, so a later SsiSend call fails until
 * SsiSend_Configure is called again. */
int32_t adi_fpga910x_DataChain_SsiSend_ConfigDeinit(adi_fpga910x_Device_t *fpga910x)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);

    g_ssiSendConfig.configured = false;

    ADI_API_RETURN(fpga910x);
}

/* Undoes SsiReceive_Configure: marks SsiReceive unconfigured, so a later SsiReceive call fails
 * until SsiReceive_Configure is called again. */
int32_t adi_fpga910x_DataChain_SsiReceive_ConfigDeinit(adi_fpga910x_Device_t *fpga910x)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);

    g_ssiReceiveConfig.configured = false;

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_SsiSend_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                                adi_common_ChannelNumber_e channel,
                                                                                adi_fpga910x_GpioPin_e triggerPin)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(fpga910x, triggerPin, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
    ADI_API_RETURN(fpga910x);
}

/* One-time setup for SsiSend: configures the trigger pin's GPIO hardware and remembers
 * channel/triggerPin/timeoutMs so every later SsiSend call is just "wait + transfer". */
int32_t adi_fpga910x_DataChain_SsiSend_Configure(adi_fpga910x_Device_t *fpga910x,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_fpga910x_GpioPin_e triggerPin,
                                                 uint32_t timeoutMs)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_SsiSend_Configure_Validate, fpga910x, channel, triggerPin);

    ADI_EXPECT(adi_fpga910x_DataChain_GpioPin_ConfigureInput, fpga910x, triggerPin);

    g_ssiSendConfig.channel = channel;
    g_ssiSendConfig.triggerPin = triggerPin;
    g_ssiSendConfig.timeoutMs = timeoutMs;
    g_ssiSendConfig.dmaId = fpga910x_DMAIdGet(fpga910x, ADI_TX, channel);
    g_ssiSendConfig.lastKnownLevel = adi_fpga910x_DataChain_SsiTrigger_SampleLevel(fpga910x, triggerPin);
    g_ssiSendConfig.configured = true;

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_SsiSend_Validate(adi_fpga910x_Device_t *fpga910x)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    if (!g_ssiSendConfig.configured)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         0,
                         "adi_fpga910x_DataChain_SsiSend_Configure must be called before SsiSend");
    }
    ADI_API_RETURN(fpga910x);
}

/* iqData is pushed before waiting for the trigger level change, so it's clear of the DMA
 * re-arm. */
int32_t adi_fpga910x_DataChain_SsiSend(adi_fpga910x_Device_t *fpga910x,
                                       uint16_t iqData[],
                                       uint32_t length)
{
    adi_fpga910x_DmaCfg_t dmaCfg;

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_SsiSend_Validate, fpga910x);

    /* length is ignored for Tx - the transfer length comes from Data_Set_16IQInterleaved. */
    dmaCfg.length = 0u;
    dmaCfg.continuous = false;
    dmaCfg.timeout_ms = ADI_FPGA910X_SSI_SEND_DMA_FINISH_TIMEOUT_MS;
    dmaCfg.trigger = ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE;
    dmaCfg.captureType = ADI_FPGA910X_CAPTURE_TYPE_DISABLE;
    dmaCfg.dmaEnableSync = false;

    /* Stage this call's payload before configuring the DMA - see function doc. */
    ADI_EXPECT(adi_fpga910x_DataChain_Data_Set_16IQInterleaved, fpga910x, ADI_TX, g_ssiSendConfig.channel, iqData, length);

    /* Program the DMA's address/length/trigger registers now, so WaitAndBegin only has to
     * call axi_dma_begin - the single fastest possible step - the instant it sees the level
     * change. */
    ADI_EXPECT(adi_fpga910x_DataChain_SsiTrigger_ConfigureDmaRegisters, fpga910x, ADI_TX, g_ssiSendConfig.channel, g_ssiSendConfig.dmaId, &dmaCfg);

    /* Blocks until the trigger pin's level changes, then begins the DMA transfer itself. */
    ADI_EXPECT(adi_fpga910x_DataChain_SsiTrigger_WaitAndBegin, fpga910x, &g_ssiSendConfig);

    /* The transfer has already begun - block until it finishes. */
    ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, ADI_TX, g_ssiSendConfig.channel, g_ssiSendConfig.dmaId, &dmaCfg);

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_SsiReceive_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                                   adi_common_ChannelNumber_e channel,
                                                                                   adi_fpga910x_GpioPin_e triggerPin)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(fpga910x, triggerPin, ADI_FPGA910X_GPIO_00, ADI_FPGA910X_GPIO_19);
    ADI_API_RETURN(fpga910x);
}

/* One-time setup for SsiReceive: configures the trigger pin's GPIO hardware and remembers
 * channel/triggerPin/timeoutMs so every later SsiReceive call is just "wait + transfer". The
 * capture size is NOT configured here - it comes from SsiReceive's own per-call length. */
int32_t adi_fpga910x_DataChain_SsiReceive_Configure(adi_fpga910x_Device_t *fpga910x,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_fpga910x_GpioPin_e triggerPin,
                                                    uint32_t timeoutMs)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_SsiReceive_Configure_Validate, fpga910x, channel, triggerPin);

    ADI_EXPECT(adi_fpga910x_DataChain_GpioPin_ConfigureInput, fpga910x, triggerPin);

    g_ssiReceiveConfig.channel = channel;
    g_ssiReceiveConfig.triggerPin = triggerPin;
    g_ssiReceiveConfig.timeoutMs = timeoutMs;
    g_ssiReceiveConfig.dmaId = fpga910x_DMAIdGet(fpga910x, ADI_RX, channel);
    g_ssiReceiveConfig.lastKnownLevel = adi_fpga910x_DataChain_SsiTrigger_SampleLevel(fpga910x, triggerPin);
    g_ssiReceiveConfig.configured = true;

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_DataChain_SsiReceive_Validate(adi_fpga910x_Device_t *fpga910x)
{
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    if (!g_ssiReceiveConfig.configured)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FULL,
                         0,
                         "adi_fpga910x_DataChain_SsiReceive_Configure must be called before SsiReceive");
    }
    ADI_API_RETURN(fpga910x);
}

/* iqData is drained after the transfer finishes, sized by length (length/2 32-bit words). */
int32_t adi_fpga910x_DataChain_SsiReceive(adi_fpga910x_Device_t *fpga910x,
                                          uint16_t iqData[],
                                          uint32_t length)
{
    adi_fpga910x_DmaCfg_t dmaCfg;

    ADI_PERFORM_VALIDATION(adi_fpga910x_DataChain_SsiReceive_Validate, fpga910x);

    /* length is in uint16_t elements, 2 per 32-bit captured word -> length * 2 bytes. */
    dmaCfg.length = length * 2u;
    dmaCfg.continuous = false;
    dmaCfg.timeout_ms = ADI_FPGA910X_SSI_RECEIVE_DMA_FINISH_TIMEOUT_MS;
    dmaCfg.trigger = ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE;
    dmaCfg.captureType = ADI_FPGA910X_CAPTURE_TYPE_RX_DATA;
    dmaCfg.dmaEnableSync = false;

    /* Program the DMA's address/length/trigger registers before waiting for the trigger pin,
     * same reasoning as SsiSend. */
    ADI_EXPECT(adi_fpga910x_DataChain_SsiTrigger_ConfigureDmaRegisters, fpga910x, ADI_RX, g_ssiReceiveConfig.channel, g_ssiReceiveConfig.dmaId, &dmaCfg);

    /* Blocks until the trigger pin's level changes, then begins the DMA transfer itself. */
    ADI_EXPECT(adi_fpga910x_DataChain_SsiTrigger_WaitAndBegin, fpga910x, &g_ssiReceiveConfig);

    /* The transfer has already begun - block until it finishes. */
    ADI_EXPECT(adi_fpga910x_DataChain_CaptureWait, fpga910x, ADI_RX, g_ssiReceiveConfig.channel, g_ssiReceiveConfig.dmaId, &dmaCfg);

    /* Drain this call's captured samples now that the transfer has finished. */
    ADI_EXPECT(adi_fpga910x_DataChain_Data_Get_16IQInterleaved, fpga910x, ADI_RX, g_ssiReceiveConfig.channel, iqData, length);

    ADI_API_RETURN(fpga910x);
}
