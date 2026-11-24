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
// Description:       AXI SSPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_ps2p9102_sspi.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_sspi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_ps2p9102_sspi_params_t *sspi_param) {

    int32_t status;

    status = axi_ps2p9102_top_sspi_bidir_count_set(a_device, peripheral_id, sspi_param->bidir_count);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_bidir_enable_set(a_device, peripheral_id, sspi_param->bidir_enable);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_miso_edge_sel_set(a_device, peripheral_id, sspi_param->miso_edge);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_mosi_edge_sel_set(a_device, peripheral_id, sspi_param->mosi_edge);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_ps2p9102_sspi_mosi_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t *data) {

    uint32_t value;
    int32_t status;

    status = axi_ps2p9102_top_sspi_mosi_data_get(a_device, peripheral_id, &value);
    if (status != 0) return(status);

    *data = (uint8_t) (value & 0xff);
    return(0);
}

int32_t axi_ps2p9102_sspi_miso_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t data) {

    return(axi_ps2p9102_top_sspi_miso_data_set(a_device, peripheral_id, (uint32_t) data));
}

int32_t axi_ps2p9102_sspi_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint8_t mosi_data[], const uint8_t miso_data[], uint32_t count) {

    uint8_t value;
    uint32_t data;
    int32_t status;
    uint32_t mosi_count;
    uint32_t miso_count;

    status = axi_ps2p9102_top_sspi_miso_empty_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);
    if (data == 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: miso fifo not empty, ",
            "pending or corrupted transfers.\n", __func__);
        return(-1);
    }

    status = axi_ps2p9102_top_sspi_mosi_empty_get(a_device, peripheral_id, &data);
    if (status != 0) return(status);
    if (data == 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: mosi fifo not empty, ",
            "pending or corrupted transfers.\n", __func__);
        return(-1);
    }

    miso_count = 0;
    mosi_count = 0;

    while ((mosi_count < count) || (miso_count < count)) {
        status = axi_ps2p9102_top_sspi_miso_full_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if ((data == 0) && (miso_count < count)) {
            status = axi_ps2p9102_sspi_miso_data(a_device, peripheral_id, miso_data[miso_count]);
            if (status != 0) return(status);
            miso_count = miso_count + 1;
        }
        status = axi_ps2p9102_top_sspi_mosi_empty_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if ((data == 0) && (mosi_count < count)) {
            status = axi_ps2p9102_sspi_mosi_data(a_device, peripheral_id, &value);
            if (mosi_data != NULL) mosi_data[mosi_count] = value;
            mosi_count = mosi_count + 1;
        }
    }

    return(0);
}
        
// **********************************************************************************
// **********************************************************************************
