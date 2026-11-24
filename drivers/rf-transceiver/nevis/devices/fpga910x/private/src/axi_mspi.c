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
// Description:       AXI MSPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_mspi.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_mspi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_mspi_params_t *mspi_param) {

    uint32_t data;
    int32_t status;

    status = axi_mspi_top_busy_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);

    if (data != 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: spi transfer active, "
            "can not continue.\n", __func__);
        return(-1);
    }

    status = axi_mspi_top_sclk_period_set(a_device, peripheral_id, mspi_param->clk_period);
    if (status != 0) return(status);
    status = axi_mspi_top_sclk_init_set(a_device, peripheral_id, mspi_param->clk_init);
    if (status != 0) return(status);
    status = axi_mspi_top_mosi_bidir_mode_set(a_device, peripheral_id, mspi_param->bidir_enable);
    if (status != 0) return(status);
    status = axi_mspi_top_mosi_bidir_offset_set(a_device, peripheral_id, mspi_param->bidir_offset);
    if (status != 0) return(status);
    status = axi_mspi_top_miso_fall1_rise0_set(a_device, peripheral_id, mspi_param->miso_sample_edge);
    if (status != 0) return(status);
    status = axi_mspi_top_swreset_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_mspi_dma_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num_of_bytes, uint32_t num_of_frames) {

    int32_t status;

    status = axi_mspi_top_reg_enable_set(a_device, peripheral_id, AXI_MSPI_TOP_DISABLE);
    if (status != 0) return(status);
    status = axi_mspi_top_num_of_bytes_set(a_device, peripheral_id, num_of_bytes);
    if (status != 0) return(status);
    status = axi_mspi_top_num_of_frames_set(a_device, peripheral_id, num_of_frames);
    if (status != 0) return(status);
    status = axi_mspi_top_dma1_fifo0_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(status);
    status = axi_mspi_top_reg_enable_set(a_device, peripheral_id, AXI_MSPI_TOP_DMA_ENABLE);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_mspi_fifo_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    int32_t status;

    status = axi_mspi_top_reg_enable_set(a_device, peripheral_id, AXI_MSPI_TOP_DISABLE);
    if (status != 0) return(status);
    status = axi_mspi_top_num_of_bytes_set(a_device, peripheral_id, 0x0);
    if (status != 0) return(status);
    status = axi_mspi_top_num_of_frames_set(a_device, peripheral_id, 0x0);
    if (status != 0) return(status);
    status = axi_mspi_top_dma1_fifo0_set(a_device, peripheral_id, 0x0);
    if (status != 0) return(status);
    status = axi_mspi_top_reg_enable_set(a_device, peripheral_id, AXI_MSPI_TOP_FIFO_ENABLE);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_mspi_fifo_fast_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint8_t miso_data[], uint32_t max_16) {

    int32_t i;
    uint32_t data;
    int32_t status;

    status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);

    if (data != AXI_MSPI_TOP_XFER_EMPTY) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: fifos NOT empty, "
            "incomplete, or corrupted transfers (0x%x).\n", __func__, data);
        return(-1);
    }

    for (i = 0; i < max_16; i++) {
        data = (uint32_t) mosi_data[i];
        status = axi_mspi_top_mosi_data_set(a_device, peripheral_id, data);
        if (status != 0) return(status);
    }

    status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);

    if ((data & AXI_MSPI_TOP_XFER_MISO_MASK) == AXI_MSPI_TOP_XFER_MISO_EMPTY) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: miso fifo empty, "
            "incomplete, or corrupted transfers (0x%x).\n", __func__, data);
        return(-1);
    }

    for (i = 0; i < max_16; i++) {
        status = axi_mspi_top_miso_data_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        miso_data[i] = (uint8_t) data;
    }

    return(0);
}

int32_t axi_mspi_fifo_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint8_t miso_data[], uint32_t count) {

    int32_t i;
    uint32_t data;
    int32_t status;
    uint32_t mosi_count;
    uint32_t miso_count;

    status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);

    if (data != AXI_MSPI_TOP_XFER_EMPTY) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: fifos NOT empty, "
            "incomplete, or corrupted transfers (0x%x).\n", __func__, data);
        return(-1);
    }

    miso_count = 0;
    mosi_count = (count > 16) ? 16 : count;

    for (i = 0; i < mosi_count; i++) {
        data = (uint32_t) mosi_data[i];
        status = axi_mspi_top_mosi_data_set(a_device, peripheral_id, data);
        if (status != 0) return(status);
    }

    while ((mosi_count < count) || (miso_count < count)) {
        status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if (((data & AXI_MSPI_TOP_XFER_MOSI_MASK) != AXI_MSPI_TOP_XFER_MOSI_FULL) &&
            (mosi_count < count)) {
            data = (uint32_t) mosi_data[mosi_count];
            mosi_count = mosi_count + 1;
            status = axi_mspi_top_mosi_data_set(a_device, peripheral_id, data);
            if (status != 0) return(status);
        }
        if (((data & AXI_MSPI_TOP_XFER_MISO_MASK) != AXI_MSPI_TOP_XFER_MISO_EMPTY) &&
            (miso_count < count)) {
            status = axi_mspi_top_miso_data_get(a_device, peripheral_id, &data);
            if (status != 0) return(status);
            if (miso_data != NULL) miso_data[miso_count] = (uint8_t) data;
            miso_count = miso_count + 1;
        }
    }

    return(0);
}

int32_t axi_mspi_fifo_mosi_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint32_t count) {

    int32_t i;
    uint32_t len;
    uint32_t data;
    int32_t status;

    status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);

    if (data != AXI_MSPI_TOP_XFER_EMPTY) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: fifos NOT empty, "
            "incomplete, or corrupted transfers (0x%x).\n", __func__, data);
        return(-1);
    }

    status = axi_mspi_top_miso_enable_set(a_device, peripheral_id, 0x0);
    if (status != 0) return(status);

    len = (count > 16) ? 16 : count;

    for (i = 0; i < len; i++) {
        data = (uint32_t) mosi_data[i];
        status = axi_mspi_top_mosi_data_set(a_device, peripheral_id, data);
        if (status != 0) return(status);
    }

    for (i = len; i < count; i++) {
        status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
        if ((data & AXI_MSPI_TOP_XFER_MOSI_MASK) != AXI_MSPI_TOP_XFER_MOSI_FULL) {
            data = (uint32_t) mosi_data[i];
            status = axi_mspi_top_mosi_data_set(a_device, peripheral_id, data);
            if (status != 0) return(status);
        }
    }

    data = AXI_MSPI_TOP_XFER_MOSI_FULL;
    while ((data & AXI_MSPI_TOP_XFER_MOSI_MASK) != AXI_MSPI_TOP_XFER_MOSI_EMPTY) {
        status = axi_mspi_top_reg_status_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
    }

    status = axi_mspi_top_miso_enable_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_mspi_slave_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data) {

    uint32_t value;
    int32_t status;

    status = axi_mspi_top_slave_select_n_get(a_device, peripheral_id, &value);
    if (status != 0) return(status);

    *data = ~value;
    return(0);
}

int32_t axi_mspi_slave_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data) {

    return(axi_mspi_top_slave_select_n_set(a_device, peripheral_id, ~data));
}

int32_t axi_mspi_dma_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t timeout_us) {

    uint32_t data;
    uint32_t timer;
    int32_t status;

    data = 1;
    timer = timeout_us * 100;

    status = axi_mspi_top_timer_set(a_device, peripheral_id, timer);
    if (status != 0) return(status);

    while ((timer > 0) && (data == 1)) {
        status = axi_mspi_top_timer_get(a_device, peripheral_id, &timer);
        if (status != 0) return(status);
        status = axi_mspi_top_dma_status_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
    }

    if (data == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "dma transfer has timed out.\n", __func__);
        return(-1);
    }

    return(0);
}

int32_t axi_mspi_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t timeout_us) {

    uint32_t data;
    uint32_t timer;
    int32_t status;

    data = 1;
    timer = timeout_us * 100;

    status = axi_mspi_top_timer_set(a_device, peripheral_id, timer);
    if (status != 0) return(status);

    while ((timer > 0) && (data == 1)) {
        status = axi_mspi_top_timer_get(a_device, peripheral_id, &timer);
        if (status != 0) return(status);
        status = axi_mspi_top_busy_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
    }

    if (data == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "spi transfer has timed out.\n", __func__);
        return(-1);
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************

