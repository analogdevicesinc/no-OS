// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI DMA Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_dma.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_reset(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    uint32_t data;
    int32_t status;

    data = 0x1;

    status = axi_dma_top_swreset_set(a_device, peripheral_id, module_id, data);
    if (status != 0) return(status);

    while (data == 1) {
        status = axi_dma_top_swreset_get(a_device, peripheral_id, module_id, &data);
        if (status != 0) return(status);
    }

    return(0);
}

int32_t axi_dma_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    int32_t status;

    status = axi_dma_top_start_set(a_device, peripheral_id, module_id, 0x1);
    if (status != 0) return(status);

    return(axi_dma_top_cdc_state_sync(a_device, peripheral_id, module_id));
}

int32_t axi_dma_stop(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    uint32_t data;
    uint32_t status;

    status = axi_dma_top_start_get(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);
    if (data == 0x0) return(0);

    status = axi_dma_top_stop_set(a_device, peripheral_id, module_id, 0x1);
    if (status != 0) return(status);

    return(axi_dma_top_cdc_state_sync(a_device, peripheral_id, module_id));
}

int32_t axi_dma_busy(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *data) {

    return(axi_dma_top_busy_get(a_device, peripheral_id, module_id, data));
}

int32_t axi_dma_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *data) {

    uint32_t value;
    int32_t status;

    status = axi_dma_top_reg_status_get(a_device, peripheral_id, module_id, &value);
    if (status != 0) return(status);

    *data = value ^ 0x1;
    return(0);
}

int32_t axi_dma_status_clear(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    return(axi_dma_top_reg_status_set(a_device, peripheral_id, module_id, (uint32_t) -1));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_start_address_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t data) {

    int32_t status;
    uint32_t addr[2];

    addr[0] = (uint32_t) (data & 0xffffffff);
    addr[1] = (uint32_t) ((data >> 32) & 0xffffffff);

    status = axi_dma_top_start_addr_low_set(a_device, peripheral_id, module_id, addr[0]);
    if (status != 0) return(status);

    status = axi_dma_top_start_addr_high_set(a_device, peripheral_id, module_id, addr[1]);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_dma_start_address_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t *data) {

    int32_t status;
    uint32_t addr[2];

    status = axi_dma_top_start_addr_low_get(a_device, peripheral_id, module_id, &addr[0]);
    if (status != 0) return(status);

    status = axi_dma_top_start_addr_high_get(a_device, peripheral_id, module_id, &addr[1]);
    if (status != 0) return(status);

    *data = addr[1];
    *data = (*data << 32) | addr[0];
    return(0);
}

int32_t axi_dma_base_address_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t data) {

    int32_t status;
    uint32_t addr[2];

    addr[0] = (uint32_t) (data & 0xffffffff);
    addr[1] = (uint32_t) ((data >> 32) & 0xffffffff);

    status = axi_dma_top_base_addr_low_set(a_device, peripheral_id, module_id, addr[0]);
    if (status != 0) return(status);

    status = axi_dma_top_base_addr_high_set(a_device, peripheral_id, module_id, addr[1]);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_dma_base_address_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t *data) {

    int32_t status;
    uint32_t addr[2];

    status = axi_dma_top_base_addr_low_get(a_device, peripheral_id, module_id, &addr[0]);
    if (status != 0) return(status);

    status = axi_dma_top_base_addr_high_get(a_device, peripheral_id, module_id, &addr[1]);
    if (status != 0) return(status);

    *data = addr[1];
    *data = (*data << 32) | addr[0];
    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, axi_dma_params_t *dma_param) {

    int32_t i;
    int32_t status;
    uint32_t data;

    if ((dma_param->length & 0xff800000) != 0x0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid dma length (%d), "
            "must be >= 1, and <= 8388607.\n", __func__, dma_param->length);
        return(-1);
    }

    status = axi_dma_busy(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);
    if (data != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_WARNING, "%s: dma is active, "
            "but overriding previous settings.\n", __func__);
    }

    status = axi_dma_status(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);
    if (data != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_WARNING, "%s: dma in error state, "
            "resetting data path..\n", __func__);
        status = axi_dma_reset(a_device, peripheral_id, module_id);
        if (status != 0) return(-1);
        status = axi_dma_top_cdc_state_sync(a_device, peripheral_id, module_id);
        if (status != 0) return(-1);
    }

    status = axi_dma_status_clear(a_device, peripheral_id, module_id);
    if (status != 0) return(status);

    for (i = 0; i < 6; i++) {
        status = axi_dma_top_trigger_mode_set(a_device, peripheral_id, module_id, i, dma_param->triggers[i]);
        if (status != 0) return(status);
    }

    status = axi_dma_top_start_sync_set(a_device, peripheral_id, module_id, dma_param->start_sync);
    if (status != 0) return(status);
    status = axi_dma_top_cyclic_sync_set(a_device, peripheral_id, module_id, dma_param->cyclic_sync);
    if (status != 0) return(status);
    status = axi_dma_start_address_set(a_device, peripheral_id, module_id, dma_param->start_address);
    if (status != 0) return(status);
    status = axi_dma_top_length_set(a_device, peripheral_id, module_id, dma_param->length);
    if (status != 0) return(status);
    status = axi_dma_top_num_of_buffers_set(a_device, peripheral_id, module_id, dma_param->no_of_buffers);
    if (status != 0) return(status);
    status = axi_dma_top_qthreshold_set(a_device, peripheral_id, module_id, dma_param->qthreshold);
    if (status != 0) return(status);
    status = axi_dma_top_cyclic_set(a_device, peripheral_id, module_id, dma_param->cyclic);
    if (status != 0) return(status);

    return(axi_dma_top_cdc_state_sync(a_device, peripheral_id, module_id));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_begin(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    return(axi_dma_start(a_device, peripheral_id, module_id));
}

int32_t axi_dma_report(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id) {

    uint32_t data;
    int32_t status;

    status = axi_dma_busy(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);
    if (data != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma transfer has "
            "timed out, low time out value or inactive interface.\n", __func__);
        return(-1);
    }

    status = axi_dma_status(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);
    if (data == 0) return(0);

    if ((data & AXI_DMA_TOP_XFER_OVF_UNF) != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma status, "
            "overflow or underflow errors.\n", __func__);
    }

    if ((data & AXI_DMA_TOP_XFER_LENGTH_ERROR) != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma status, "
            "length errors.\n", __func__);
    }

    if ((data & AXI_DMA_TOP_XFER_TLAST_ERROR) != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma status, "
            "stream TLAST errors.\n", __func__);
    }

    if ((data & AXI_DMA_TOP_XFER_ADDRDEC_ERROR) != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma status, "
            "invalid address errors.\n", __func__);
    }

    if ((data & AXI_DMA_TOP_XFER_SLAVE_ERROR) != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: dma status, "
            "slave errors.\n", __func__);
    }

    return(-1);
}

int32_t axi_dma_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t timeout_us) {

    uint32_t data;
    uint32_t timer;
    int32_t status;

    status = axi_dma_top_cyclic_get(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);

    if (data == 1) {
        status = axi_dma_stop(a_device, peripheral_id, module_id);
        if (status != 0) return(status);
    }

    data = 1;
    timer = timeout_us * 100;

    status = axi_dma_top_timer_set(a_device, peripheral_id, module_id, timer);
    if (status != 0) return(status);

    while ((timer > 0) && (data == 1)) {
        status = axi_dma_top_timer_get(a_device, peripheral_id, module_id, &timer);
        if (status != 0) return(status);
        status = axi_dma_busy(a_device, peripheral_id, module_id, &data);
        if (status != 0) return(status);
    }

    return(axi_dma_report(a_device, peripheral_id, module_id));
}

int32_t axi_dma_flush(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t timeout_us) {

    uint32_t data;
    int32_t status;

    status = axi_dma_top_cyclic_get(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);

    if (data == 1) {
        status = axi_dma_stop(a_device, peripheral_id, module_id);
        if (status != 0) return(status);
    }

    data = timeout_us;
    status = axi_dma_top_mmq_tlimit_set(a_device, peripheral_id, module_id, data);
    if (status != 0) return(status);

    while (data != 0) {
        status = axi_dma_top_mmq_status_get(a_device, peripheral_id, module_id, &data);
        if (status != 0) return(status);
    }

    return(axi_dma_report(a_device, peripheral_id, module_id));
}

int32_t axi_dma_run(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, axi_dma_params_t *dma_param, uint32_t timeout_us) {

    if (axi_dma_config(a_device, peripheral_id, module_id, dma_param) != 0) return(-1);
    if (axi_dma_begin(a_device, peripheral_id, module_id) != 0) return(-2);
    if (axi_dma_finish(a_device, peripheral_id, module_id, timeout_us) != 0) return(-3);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_dma_xfer_time_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *duration) {

    uint32_t data;
    int32_t status;

    status = axi_dma_top_data_xfer_count_get(a_device, peripheral_id, module_id, &data);
    if (status != 0) return(status);

    *duration = (uint32_t) ((data + 50) / 100);
    return(0);
}

// **********************************************************************************
// **********************************************************************************
