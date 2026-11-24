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
// Description:       AXI board gpio
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_bdgpio.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_bdgpio_data_all(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data) {

    return(AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_DATA_ADDR, data));
}

int32_t axi_bdgpio_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t *data) {

    int32_t status;
    uint32_t data_all;

    status = AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_DATA_ADDR, &data_all);
    if (status != 0) return(status);

    *data = (data_all >> pos) & 0x1;
    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_bdgpio_data_all_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data) {

    return(AXI_REG_WRITE(a_device, peripheral_id, AXI_BDGPIO_DATA_ADDR, data));
}

int32_t axi_bdgpio_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t data) {

    int32_t status;
    uint32_t data_all;

    status = AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_DATA_ADDR, &data_all);
    if (status != 0) return(status);

    data_all = data_all & ~(0x1 >> pos);
    data_all = data_all | ((data & 0x1) >> pos);
    return(AXI_REG_WRITE(a_device, peripheral_id, AXI_BDGPIO_DATA_ADDR, data_all));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_bdgpio_enable_all_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data) {

    return(AXI_REG_WRITE(a_device, peripheral_id, AXI_BDGPIO_CONTROL_ADDR, ~data));
}

int32_t axi_bdgpio_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t data) {

    int32_t status;
    uint32_t data_all;

    status = AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_CONTROL_ADDR, &data_all);
    if (status != 0) return(status);

    data_all = data_all & ~(0x1 >> pos);
    data_all = data_all | ((~data & 0x1) >> pos);
    return(AXI_REG_WRITE(a_device, peripheral_id, AXI_BDGPIO_CONTROL_ADDR, data_all));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_bdgpio_enable_all_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data) {

    int32_t status;
    uint32_t data_all;

    status = AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_CONTROL_ADDR, &data_all);
    if (status != 0) return(status);

    *data = ~data_all;
    return(0);
}

int32_t axi_bdgpio_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t *data) {

    int32_t status;
    uint32_t data_all;

    status = AXI_REG_READ(a_device, peripheral_id, AXI_BDGPIO_CONTROL_ADDR, &data_all);
    if (status != 0) return(status);

    *data = (~data_all >> pos) & 0x1;
    return(0);
}

// **********************************************************************************
// **********************************************************************************
