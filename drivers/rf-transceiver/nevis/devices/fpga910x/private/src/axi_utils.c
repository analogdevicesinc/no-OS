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
// Description:       AXI Internal Common Functions
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_utils.h"

// **********************************************************************************
// **********************************************************************************

/*  register field write */

int32_t axi_utils_field_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t pos, uint32_t mask, uint32_t data) {

    uint32_t value;
    int32_t status;

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = value & ~(mask << pos);
    value = value | ((data & mask) << pos);

    return(AXI_REG_WRITE(a_device, peripheral_id, addr, value));
}

/*  register field read */

int32_t axi_utils_field_read(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t pos, uint32_t mask, uint32_t *data) {

    uint32_t value;
    int32_t status;

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    *data = (value >> pos) & mask;

    return(0);
}

/*  timer delay microseconds */

int32_t axi_utils_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t data) {

    uint32_t value;
    int32_t status;

    if (data > (uint32_t) 42e6) return(-1);

    value = data * 100;
    status = AXI_REG_WRITE(a_device, peripheral_id, addr, value);
    if (status != 0) return(status);

    while (value > 0) {
        status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
        if (status != 0) return(status);
    }

    return(0);
}

/*  clock frequency 16.16 */

int32_t axi_utils_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t *data) {

    uint32_t value;
    int32_t status;

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if ((status == 0) && ((value >> 21) == 0)) {
        *data = (value * 1526);
        return(0);
    }

    *data = 0;
    return(-1);
}

/* cdc sync */

int32_t axi_utils_cdc_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t cdc_state_offset, uint32_t timer_offset) {

    uint32_t data;
    uint32_t timer;
    int32_t status;

    status = AXI_REG_READ(a_device, peripheral_id, cdc_state_offset, &data);
    if (status != 0) return(status);
    if (data == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: clock domain "
            "invalid state, clocks may not be active.\n", __func__);
        return(-1);
    }

    data = 0x1;
    timer = (uint32_t) (5e3 * 100);

    status = AXI_REG_WRITE(a_device, peripheral_id, cdc_state_offset, data);
    if (status != 0) return(status);
    status = AXI_REG_WRITE(a_device, peripheral_id, timer_offset, timer);
    if (status != 0) return(status);

    while (timer > 0) {
        status = AXI_REG_READ(a_device, peripheral_id, cdc_state_offset, &data);
        if (status != 0) return(status);
        if (data == 0) return(0);
        status = AXI_REG_READ(a_device, peripheral_id, timer_offset, &timer);
        if (status != 0) return(status);
    }

    AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: clock domain "
        "transfer has timed out.\n", __func__);
    return(-1);
}

// **********************************************************************************
// **********************************************************************************
