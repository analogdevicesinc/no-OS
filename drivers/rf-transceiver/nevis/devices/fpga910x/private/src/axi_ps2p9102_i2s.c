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
// Description:       AXI I2S Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_ps2p9102_i2s.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_i2s_tx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t data) {

    return(axi_ps2p9102_top_i2s_tx_data_set(a_device, peripheral_id, (uint32_t) data));
}

int32_t axi_ps2p9102_i2s_rx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t *data) {

    uint32_t value;
    int32_t status;

    status = axi_ps2p9102_top_i2s_rx_data_get(a_device, peripheral_id, &value);
    if (status != 0) return(status);

    *data = (uint8_t) (value & 0xff);
    return(0);
}

// **********************************************************************************
// **********************************************************************************
