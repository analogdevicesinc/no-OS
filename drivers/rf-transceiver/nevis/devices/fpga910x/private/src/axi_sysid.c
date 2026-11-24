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
// Description:       AXI SYSID Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_sysid.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_sysid_bulk_read(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t max_size, uint32_t data[], uint32_t size) {

    int32_t n;
    int32_t status;

    if (size > max_size) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: array size(%d) "
            "can not exceed %d.\n", __func__, size, max_size);
        return(-1);
    }

    for (n = 0; n < size; n++) {
        status = AXI_REG_READ(a_device, peripheral_id, (addr + (n * 4)), &data[n]);
        if (status != 0) return(status);
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_sysid_gpio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t *data) {

    return(axi_utils_field_read(a_device, peripheral_id,
        AXI_SYSID_TOP_GPIO_IN_ADDR, index, 0x1, data));
}

int32_t axi_sysid_gpio_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data) {

    return(axi_utils_field_write(a_device, peripheral_id,
        AXI_SYSID_TOP_GPIO_OUT_ADDR, index, 0x1, data));
}

int32_t axi_sysid_status_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t *data) {

    return(axi_utils_field_read(a_device, peripheral_id,
        AXI_SYSID_TOP_STATUS_STATE_ADDR, index, 0x1, data));
}

int32_t axi_sysid_control_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data) {

    return(axi_utils_field_write(a_device, peripheral_id,
        AXI_SYSID_TOP_CONTROL_STATE_ADDR, index, 0x1, data));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_sysid_pwm_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data) {

    uint32_t pwm;
    int32_t status;

    status = axi_sysid_top_pwm_enable_get(a_device, peripheral_id, &pwm);
    if (status != 0) return(status);

    pwm = pwm & ~(0x1 << index);

    status = axi_sysid_top_pwm_enable_set(a_device, peripheral_id, pwm);
    if (status != 0) return(status);

    status = axi_sysid_top_pwm_out_get(a_device, peripheral_id, &pwm);
    if (status != 0) return(status);

    pwm = pwm & ~(0x1 << index);
    pwm = pwm | (data << index);

    return(axi_sysid_top_pwm_out_set(a_device, peripheral_id, pwm));
}

int32_t axi_sysid_pwm_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t on_time_ms, uint32_t off_time_ms) {

    uint32_t mask;
    uint32_t enable;
    int32_t status;

    mask = 0x1 << index;

    if ((on_time_ms == 0) || (off_time_ms == 0)) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: on/off times "
            "can not be zero.\n", __func__);
        return(-1);
    }

    if (index >= 8) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid pwm "
            "index (%d).\n", __func__, index);
        return(-1);
    }

    status = axi_sysid_top_pwm_enable_get(a_device, peripheral_id, &enable);
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_enable_set(a_device, peripheral_id, (enable & ~mask));
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_prescale_count_set(a_device, peripheral_id, 100000);
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_delay_count_set(a_device, peripheral_id, index, 0x0);
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_high_count_set(a_device, peripheral_id, index, on_time_ms);
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_low_count_set(a_device, peripheral_id, index, off_time_ms);
    if (status != 0) return(status);
    status = axi_sysid_top_pwm_enable_set(a_device, peripheral_id, (enable | mask));
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_sysid_sys_params_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size) {

    return(axi_sysid_bulk_read(a_device, peripheral_id, AXI_SYSID_TOP_SYS_PARAMS_0_ADDR,
        AXI_SYSID_TOP_SYS_PARAMS_SIZE, data, size));
}

int32_t axi_sysid_sys_build_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size) {

    return(axi_sysid_bulk_read(a_device, peripheral_id, AXI_SYSID_TOP_SYS_BUILD_0_ADDR,
        AXI_SYSID_TOP_SYS_BUILD_SIZE, data, size));
}

int32_t axi_sysid_sys_info_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size) {

    return(axi_sysid_bulk_read(a_device, peripheral_id, AXI_SYSID_TOP_SYS_INFO_0_0_ADDR,
        AXI_SYSID_TOP_SYS_INFO_0_SIZE, data, size));
}

int32_t axi_sysid_sys_info_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size) {

    return(axi_sysid_bulk_read(a_device, peripheral_id, AXI_SYSID_TOP_SYS_INFO_1_0_ADDR,
        AXI_SYSID_TOP_SYS_INFO_1_SIZE, data, size));
}

// **********************************************************************************
// **********************************************************************************

