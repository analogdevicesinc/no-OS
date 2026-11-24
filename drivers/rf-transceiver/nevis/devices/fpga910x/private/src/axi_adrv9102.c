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
// Description:       AXI ADRV9102 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_adrv9102.h"

// **********************************************************************************
// **********************************************************************************

static const uint32_t axi_adrv9102_mmcm_filter_table[][2] = {

 {0x1900, 0x9990}, {0x1900, 0x9990}, {0x9800, 0x9990}, {0x9900, 0x9990}, {0x9900, 0x8990},
 {0x9900, 0x9190}, {0x9900, 0x0990}, {0x9800, 0x1190}, {0x9900, 0x8190}, {0x9900, 0x8190},
 {0x9800, 0x9890}, {0x9900, 0x9890}, {0x9900, 0x0190}, {0x9900, 0x0190}, {0x9900, 0x0190},
 {0x9800, 0x1890}, {0x9800, 0x1890}, {0x9900, 0x1890}, {0x9800, 0x8890}, {0x9800, 0x8890},
 {0x9900, 0x8890}, {0x9900, 0x8890}, {0x9900, 0x8890}, {0x9900, 0x8890}, {0x9900, 0x8890},
 {0x9100, 0x9090}, {0x9100, 0x9090}, {0x9100, 0x9090}, {0x9800, 0x9090}, {0x9800, 0x9090},
 {0x9800, 0x9090}, {0x9900, 0x9090}, {0x9900, 0x9090}, {0x9900, 0x9090}, {0x9900, 0x9090},
 {0x9900, 0x9090}, {0x9900, 0x9090}, {0x9800, 0x0890}, {0x9800, 0x0890}, {0x9800, 0x0890},
 {0x9800, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890},
 {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890},
 {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890},
 {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9900, 0x0890},
 {0x9900, 0x0890}, {0x9900, 0x0890}, {0x9000, 0x1090}, {0x9000, 0x1090}, {0x9000, 0x1090},
 {0x9000, 0x1090}, {0x9000, 0x1090}, {0x9000, 0x1090}, {0x9000, 0x1090}, {0x9000, 0x1090},
 {0x9100, 0x1090}, {0x9100, 0x1090}, {0x9100, 0x1090}, {0x9100, 0x1090}, {0x9100, 0x1090},
 {0x9100, 0x1090}, {0x9100, 0x1090}, {0x9800, 0x1090}, {0x9800, 0x1090}, {0x9800, 0x1090},
 {0x9800, 0x1090}, {0x9800, 0x1090}, {0x9800, 0x1090}, {0x9800, 0x1090}, {0x9800, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090},
 {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9900, 0x1090}, {0x9100, 0x8090},
 {0x9100, 0x8090}, {0x9100, 0x8090}, {0x9100, 0x8090}, {0x9100, 0x8090}, {0x9100, 0x8090},
 {0x9100, 0x8090}, {0x9100, 0x8090}, {0x9100, 0x8090}
};

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_mcs_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id, axi_adrv9102_mcs_params_t *mcs_param) {

    int32_t status;

    status = axi_adrv9102_top_mcs_width_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, mcs_param->width);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mcs_num_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, mcs_param->count);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mcs_edge_fall1_rise0_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, mcs_param->edge);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mcs_period_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, mcs_param->period);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mcs_select_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, mcs_param->select);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_mcs_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    return(axi_adrv9102_top_mcs_request_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x1));
}

int32_t axi_adrv9102_mcs_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *status) {

    return(axi_adrv9102_top_mcs_request_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, status));
}

int32_t axi_adrv9102_mcs_trigger(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    uint32_t data;
    int32_t status;

    data = 1;

    status = axi_adrv9102_top_mcs_trigger_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, data);
    if (status != 0) return(status);

    while (data == 1) {
        status = axi_adrv9102_top_mcs_trigger_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, &data);
        if (status != 0) return(status);
    }
 
    return(0);
}

int32_t axi_adrv9102_mcs_rx_info_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, axi_adrv9102_mcs_rx_info_t *mcs_rx_info) {

    int32_t data;
    int32_t status;
    int32_t timeout;

    timeout = 1e4;

    status = axi_adrv9102_ssi_timer_set(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, timeout);
    if (status != 0) return(status);

    while (timeout > 0) {

        status = axi_adrv9102_ssi_timer_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, &timeout);
        if (status != 0) return(status);
        status = axi_adrv9102_ssi_mcs_latency_busy_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, &data);
        if (status != 0) return(status);
        if (data == 1) continue;
        status = axi_adrv9102_ssi_mcs_latency_busy_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_1, &data);
        if (status != 0) return(status);
        if (data == 1) continue;
        break;
    }

    status = axi_adrv9102_ssi_mcs_latency_count_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, &mcs_rx_info->rx0_latency);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_mcs_strobe_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, &mcs_rx_info->rx0_strobe);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_mcs_data_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_0, &mcs_rx_info->rx0_data);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_mcs_latency_count_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_1, &mcs_rx_info->rx1_latency);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_mcs_strobe_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_1, &mcs_rx_info->rx1_strobe);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_mcs_data_get(a_device, peripheral_id, AXI_ADRV9102_SSI_RX_OFFSET_1, &mcs_rx_info->rx1_data);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_mmcm_busy(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *status) {

    return(axi_adrv9102_top_mmcm_busy_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, status));
}

int32_t axi_adrv9102_mmcm_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t data) {

    uint32_t value;
    int32_t status;

    value = 0x1;

    status = axi_adrv9102_top_mmcm_addr_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, addr);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_wrdata_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, data);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_rd1_wr0_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x0);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_request_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, value);
    if (status != 0) return(status);

    while (value == 1) {
        status = axi_adrv9102_mmcm_busy(a_device, peripheral_id, &value);
        if (status != 0) return(status);
    }

    return(0);
}

int32_t axi_adrv9102_mmcm_read(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t *data) {

    uint32_t value;
    int32_t status;

    value = 0x1;

    status = axi_adrv9102_top_mmcm_addr_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, addr);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_rd1_wr0_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x1);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_request_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, value);
    if (status != 0) return(status);

    while (value == 1) {
        status = axi_adrv9102_mmcm_busy(a_device, peripheral_id, &value);
        if (status != 0) return(status);
    }

    return(axi_adrv9102_top_mmcm_rddata_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, data));
}

int32_t axi_adrv9102_mmcm_read_modify_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t mask, uint32_t data) {

    uint32_t m_data;
    int32_t status;

    status = axi_adrv9102_mmcm_read(a_device, peripheral_id, addr, &m_data);
    if (status != 0) return(status);

    m_data = (m_data & mask) | data;

    status = axi_adrv9102_mmcm_write(a_device, peripheral_id, addr, m_data);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_mmcm_set_div(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t div) {

    uint32_t div_int;
    uint32_t div_frac;
    uint32_t frac;
    uint32_t frac_en;
    uint32_t delay_time;
    uint32_t high_time;
    uint32_t low_time;
    uint32_t odd;
    uint32_t edge;
    uint32_t no_count;
    uint32_t frac_wf_f;
    uint32_t frac_wf_r;
    uint32_t phase_mux;
    uint32_t phase_mux_f;
    uint32_t mx;
    int32_t  status;

    div_int = (uint32_t) (div/1000);
    div_frac = (uint32_t) (div % 1000);

    if ((addr != AXI_ADRV9102_TOP_MMCM_REF_MUL_ID) && (addr != AXI_ADRV9102_TOP_MMCM_OUT0_DIV_ID) &&
        (div_frac != 0)) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid divider %d; "
            "mmcm do not support fractional divide for this clock.\n", __func__, div);
    }

    if (div_frac == 0) {

        frac = 0;
        frac_en = 0;

        delay_time = 0;
        high_time = (uint32_t) (div_int/2);
        odd = 0;
        low_time = div_int - high_time;
        edge = low_time - high_time;
        no_count = 0;

        frac_wf_f = 0;
        frac_wf_r = 0;

        phase_mux = 0;
        phase_mux_f = 0;
        mx = 0;
 
    } else {

        frac = div_frac/125;
        frac_en = 1;

        delay_time = 0;
        high_time = div_int/2;
        odd = div_int - (2 * high_time);
        odd = (odd * 8) + frac;

        if ((div_int % 2) == 0) high_time = high_time - 1;
        low_time = div_int/2;
        if (((div_int % 2) == 0) || (frac == 1)) low_time = low_time - 1;
        edge = (div_int == 1) ? 0 : (div_int % 2);
        no_count = (div_int == 1) ? 1 : 0;

        frac_wf_f = 1;
        frac_wf_r = 1;
        if ((odd >= 2) && (odd <= 9)) frac_wf_f = 1;
        if ((odd >= 1) && (odd <= 8)) frac_wf_r = 1;
        if ((div_int == 2) && (frac == 1)) frac_wf_f = 1;

        phase_mux = 0;
        phase_mux_f = (frac/2);
        if ((div_int % 2) == 1) phase_mux_f = phase_mux_f + 4;
        mx = 0;
    }

    if ((addr == AXI_ADRV9102_TOP_MMCM_REF_MUL_ID) || (addr == AXI_ADRV9102_TOP_MMCM_OUT0_DIV_ID)) {
        status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id, (addr - 1),
            ((div_frac == 0) ? 0xffff : 0xc3ff), ((phase_mux_f << 11) |
            (frac_wf_f << 10)));
        if (status != 0) return(status);
    }

    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id, (addr + 0),
        0x1000, ((phase_mux << 13) | (high_time << 6) | low_time));
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id, (addr + 1),
        (((addr == AXI_ADRV9102_TOP_MMCM_REF_MUL_ID) || (addr == AXI_ADRV9102_TOP_MMCM_OUT0_DIV_ID)) ?
        0x8000 : 0xfc00), ((frac << 12) | (frac_en << 11) | (frac_wf_r << 10) | (mx << 8) |
        (edge << 7) | (no_count << 6) | delay_time));
    if (status != 0) return(status);
    return(0);
}

int32_t axi_adrv9102_mmcm_set_lock(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ref_mul) {

    uint32_t div;
    uint32_t unlock_cnt;
    uint32_t lock_high;
    uint32_t lock_cnt;
    uint32_t lock_delay;
    int32_t  status;

    unlock_cnt = 1;
    lock_cnt = 0x0fa;
    lock_high = 0x3e9;
    lock_delay = 0x1f;

    div = (uint32_t) (ref_mul/1000);
    div = (uint32_t) (div % 64);

    if (div >=  1) lock_cnt = 0x3e8;
    if (div >= 11) lock_cnt = 0x384;
    if (div >= 12) lock_cnt = 0x339;
    if (div >= 13) lock_cnt = 0x2ee;
    if (div >= 14) lock_cnt = 0x2bc;
    if (div >= 15) lock_cnt = 0x28a;
    if (div >= 16) lock_cnt = 0x271;
    if (div >= 17) lock_cnt = 0x23f;
    if (div >= 18) lock_cnt = 0x226;
    if (div >= 19) lock_cnt = 0x20d;
    if (div >= 20) lock_cnt = 0x1f4;
    if (div >= 21) lock_cnt = 0x1db;
    if (div >= 22) lock_cnt = 0x1c2;
    if (div >= 23) lock_cnt = 0x1a9;
    if (div >= 24) lock_cnt = 0x190;
    if (div >= 26) lock_cnt = 0x177;
    if (div >= 27) lock_cnt = 0x15e;
    if (div >= 29) lock_cnt = 0x145;
    if (div >= 31) lock_cnt = 0x12c;
    if (div >= 34) lock_cnt = 0x113;
    if (div >= 37) lock_cnt = 0x0fa;

    if (div >=  1) lock_delay = 0x06;
    if (div >=  3) lock_delay = 0x08;
    if (div >=  4) lock_delay = 0x0b;
    if (div >=  5) lock_delay = 0x0e;
    if (div >=  6) lock_delay = 0x11;
    if (div >=  7) lock_delay = 0x13;
    if (div >=  8) lock_delay = 0x16;
    if (div >=  9) lock_delay = 0x19;
    if (div >= 10) lock_delay = 0x1c;
    if (div >= 11) lock_delay = 0x1f;

    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        (AXI_ADRV9102_TOP_MMCM_LOCK_ID + 0), 0xfc00, lock_cnt);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        (AXI_ADRV9102_TOP_MMCM_LOCK_ID + 1), 0x8000, ((lock_delay << 10) | unlock_cnt));
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        (AXI_ADRV9102_TOP_MMCM_LOCK_ID + 2), 0x8000, ((lock_delay << 10) | lock_high));
    if (status != 0) return(status);
    return(0);
}

int32_t axi_adrv9102_mmcm_set_filter(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ref_mul) {

    uint32_t index;
    uint32_t filter_cp;
    uint32_t filter_res_lfhf;
    int32_t  status;

    index = (uint32_t) (ref_mul/1000);
    if ((index < 1) || (index > 128)) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid reference "
            "multiplier (%d); valid range is (1, 128000).\n", __func__, ref_mul);
        return(-100);
    }

    filter_cp = axi_adrv9102_mmcm_filter_table[(index-1)][0];
    filter_res_lfhf = axi_adrv9102_mmcm_filter_table[(index-1)][1];

    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        (AXI_ADRV9102_TOP_MMCM_FILTER_ID + 0), 0x66ff, filter_cp);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        (AXI_ADRV9102_TOP_MMCM_FILTER_ID + 1), 0x666f, filter_res_lfhf);
    if (status != 0) return(status);
    return(0);
}

int32_t axi_adrv9102_clk_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_clk_params_t *clk_param, uint32_t *locked) {

    uint32_t ref_clk_freq_hz;
    uint32_t dev_clk_freq_hz;
    uint32_t int_clk_freq_hz;
    uint32_t vco_clk_freq_hz;
    uint32_t uart_clk_freq_hz;
    uint32_t qspi_clk_freq_hz;

    uint32_t div;
    uint32_t mul;
    uint32_t diff;
    uint32_t ref_div;
    uint32_t ref_mul;
    uint32_t dev_div;
    uint32_t int_div;
    uint32_t uart_div;
    uint32_t qspi_div;
    uint32_t int_diff;

    uint32_t timer;
    uint32_t pll_ool;
    uint32_t ref_mul_min;
    uint32_t ref_mul_max;
    int32_t  status;

    // locked status

    *locked = 0;

    // reference clock range (maximum allowed is 800MHz)

    ref_div = 1;
    ref_clk_freq_hz = 0;

    if ((clk_param->ref1_clk_freq_hz >= 10000000) && (clk_param->ref1_clk_freq_hz <= 800000000)) {
        status = axi_adrv9102_top_mmcm_ref_clk_sel_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x1);
        if (status != 0) return(status);
        ref_clk_freq_hz = clk_param->ref1_clk_freq_hz;
    }

    if ((clk_param->ref0_clk_freq_hz >= 10000000) && (clk_param->ref0_clk_freq_hz <= 800000000)) {
        status = axi_adrv9102_top_mmcm_ref_clk_sel_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x0);
        if (status != 0) return(status);
        ref_clk_freq_hz = clk_param->ref0_clk_freq_hz;
    }

    if (ref_clk_freq_hz == 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid mmcm reference "
            "clocks (%dHz and %dHz), at least one of them must be >= 10MHz and <= 800MHz.\n",
            __func__, clk_param->ref0_clk_freq_hz, clk_param->ref1_clk_freq_hz);
        return(-1);
    }

    // vco reference clock range (maximum is pd limit, 450MHz)

    if (ref_clk_freq_hz > 450000000) {
        ref_div = 2;
        ref_clk_freq_hz = ref_clk_freq_hz/2;
    }

    // device clock range (low automatically takes care of 128 limit)
 
    dev_clk_freq_hz = ref_clk_freq_hz;

    if (dev_clk_freq_hz < 4.69e3) {
        div = (uint32_t) (4.69e3/dev_clk_freq_hz) + 1;
        dev_clk_freq_hz = div * dev_clk_freq_hz;
    }
 
    if (dev_clk_freq_hz > 200e6) {
        div = (uint32_t) (dev_clk_freq_hz/200e6);
        div = (div > 1) ? div : 1;
        dev_clk_freq_hz = (uint32_t) (dev_clk_freq_hz/div);
    }

    int_clk_freq_hz = 200e6;
    qspi_clk_freq_hz = int_clk_freq_hz / 4;
    uart_clk_freq_hz = dev_clk_freq_hz * 4;

    // simple vco calculation, a integer multiple of the a_device clock frequency

    ref_clk_freq_hz = (uint32_t) (ref_clk_freq_hz/1000);
    dev_clk_freq_hz = (uint32_t) (dev_clk_freq_hz/1000);
    uart_clk_freq_hz = (uint32_t) (uart_clk_freq_hz/1000);
    int_clk_freq_hz = (uint32_t) (int_clk_freq_hz/1000);
    qspi_clk_freq_hz = (uint32_t) (qspi_clk_freq_hz/1000);

    ref_mul_min = (uint32_t) ((600e6/ref_clk_freq_hz) + 500);
    ref_mul_max = (uint32_t) ((1440e6/ref_clk_freq_hz) - 500);
    ref_mul_min = (ref_mul_min/1000) * 1000;
    ref_mul_max = (ref_mul_max/1000) * 1000;

    int_diff = (uint32_t) -1;
    for (mul = ref_mul_min; mul <= ref_mul_max; mul = mul + 125) {
        vco_clk_freq_hz = mul * ref_clk_freq_hz;
        div = (uint32_t) (vco_clk_freq_hz / uart_clk_freq_hz);
        if ((div % 125) != 0) continue;
        if ((div * uart_clk_freq_hz) != vco_clk_freq_hz) continue;
        div = (uint32_t) (vco_clk_freq_hz / dev_clk_freq_hz);
        if ((div % 1000) != 0) continue;
        if ((div * dev_clk_freq_hz) != vco_clk_freq_hz) continue;
        div = (uint32_t) (vco_clk_freq_hz / int_clk_freq_hz);
        div = ((div + 500)/1000) * 1000;
        diff = abs(vco_clk_freq_hz - (div * int_clk_freq_hz));
        if (diff < int_diff) {
            ref_mul = mul;
            int_diff = diff;
        }
    }

    vco_clk_freq_hz = ref_mul * ref_clk_freq_hz;

    uart_div = (uint32_t) (vco_clk_freq_hz / uart_clk_freq_hz);
    dev_div = (uint32_t) (vco_clk_freq_hz / dev_clk_freq_hz);
    int_div = (uint32_t) (vco_clk_freq_hz / int_clk_freq_hz);
    int_div = ((int_div + 500)/1000) * 1000;

    qspi_div = int_div * 4;

    clk_param->gen_dev_clk_freq_hz = (uint32_t) (vco_clk_freq_hz / dev_div) * 1000;
    clk_param->gen_tdd_clk_freq_hz = (uint32_t) (vco_clk_freq_hz / int_div) * 1000;
    clk_param->gen_gpio_clk_freq_hz = (uint32_t) (vco_clk_freq_hz / int_div) * 1000;
    clk_param->gen_uart_clk_freq_hz = (uint32_t) (vco_clk_freq_hz / uart_div) * 1000;
    clk_param->gen_qspi_clk_freq_hz = (uint32_t) (vco_clk_freq_hz / qspi_div) * 1000;

    status = axi_adrv9102_top_mmcm_reset_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x1);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_POWER_ID, 0x0000, 0xffff);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_lock(a_device, peripheral_id, ref_mul);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_filter(a_device, peripheral_id, ref_mul);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_read_modify_write(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_REF_DIV_ID, 0xc000,
        ((ref_div == 1) ? 0x1041 : 0x0041));
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_REF_MUL_ID, ref_mul);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_OUT0_DIV_ID, uart_div);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_OUT1_DIV_ID, dev_div);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_OUT2_DIV_ID, int_div);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_OUT3_DIV_ID, int_div);
    if (status != 0) return(status);
    status = axi_adrv9102_mmcm_set_div(a_device, peripheral_id,
        AXI_ADRV9102_TOP_MMCM_OUT4_DIV_ID, qspi_div);
    if (status != 0) return(status);
    status = axi_adrv9102_top_mmcm_reset_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, 0x0);
    if (status != 0) return(status);

    *locked = 0;
    pll_ool = 1;
    timer = (uint32_t) -1;

    status = axi_adrv9102_top_timer_set(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, timer);
    if (status != 0) return(status);
    while (pll_ool == 1) {
        status = axi_adrv9102_top_mmcm_unlocked_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, &pll_ool);
        if (status != 0) return(status);
        status = axi_adrv9102_top_timer_get(a_device, peripheral_id, AXI_ADRV9102_TOP_OFFSET, &timer);
        if (status != 0) return(status);
        if ((pll_ool == 1) && (timer == 0)) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: mmcm is not locked, "
                "reference clocks may not be running.\n", __func__);
            return(-2);
        }
    }

    *locked = 1;
    return(0);
}
 
// **********************************************************************************
// **********************************************************************************

static const uint32_t axi_adrv9102_gpio_pio_addr[4][3] = {

    {AXI_ADRV9102_GPIO_REG_DGPIO_PIO_DATA_ADDR,
        AXI_ADRV9102_GPIO_REG_DGPIO_PIO_DATA_OUT_ADDR,
        AXI_ADRV9102_GPIO_REG_DGPIO_PIO_DATA_OUT_ENB_ADDR},
    {AXI_ADRV9102_GPIO_REG_RX0_PIO_DATA_ADDR,
        AXI_ADRV9102_GPIO_REG_RX0_PIO_DATA_OUT_ADDR,
        AXI_ADRV9102_GPIO_REG_RX0_PIO_DATA_OUT_ENB_ADDR},
    {AXI_ADRV9102_GPIO_REG_RX1_PIO_DATA_ADDR,
        AXI_ADRV9102_GPIO_REG_RX1_PIO_DATA_OUT_ADDR,
        AXI_ADRV9102_GPIO_REG_RX1_PIO_DATA_OUT_ENB_ADDR},
    {AXI_ADRV9102_GPIO_REG_TX0_PIO_DATA_ADDR,
        AXI_ADRV9102_GPIO_REG_TX0_PIO_DATA_OUT_ADDR,
        AXI_ADRV9102_GPIO_REG_TX0_PIO_DATA_OUT_ENB_ADDR}
};

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_gpio_pin_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data) {

    uint32_t value;
    int32_t status;
    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][0];

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = (value >> pin) & 0x1;
    *data = value;

    return(0);
}

int32_t axi_adrv9102_gpio_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t data) {

    uint32_t value;
    int32_t status;
    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][1];

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = value & ~(0x1 << pin);
    value = value | ((data & 0x1) << pin);

    status = AXI_REG_WRITE(a_device, peripheral_id, addr, value);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_gpio_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data) {

    uint32_t value;
    int32_t status;
    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][1];

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = (value >> pin) & 0x1;
    *data = value;

    return(0);
}

int32_t axi_adrv9102_gpio_pin_direction_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t data) {

    uint32_t value;
    int32_t status;
    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][2];

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = value & ~(0x1 << pin);
    value = value | ((data & 0x1) << pin);

    status = AXI_REG_WRITE(a_device, peripheral_id, addr, value);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_gpio_pin_direction_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data) {

    uint32_t value;
    int32_t status;
    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][2];

    status = AXI_REG_READ(a_device, peripheral_id, addr, &value);
    if (status != 0) return(status);

    value = (value >> pin) & 0x1;
    *data = value;

    return(0);
}

int32_t axi_adrv9102_gpio_pin_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t mode) {

    uint32_t data;
    uint32_t value;
    int32_t status;

    status = axi_adrv9102_gpio_dgpio_pio_data_mode_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, &data);
    if (status != 0) return(status);

    value = (mode == AXI_ADRV9102_GPIO_MODE_HW_TDD) ? 0x1 : 0x0;
    data = data & ~(0x1 << pin);
    data = data | ((value & 0x1) << pin);

    status = axi_adrv9102_gpio_dgpio_pio_data_mode_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, data);
    if (status != 0) return(status);

    status = axi_adrv9102_gpio_dgpio_pio_buf_mode_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, &data);
    if (status != 0) return(status);

    value = (mode == AXI_ADRV9102_GPIO_MODE_SW_BUF) ? 0x1 : 0x0;
    data = data & ~(0x1 << pin);
    data = data | ((value & 0x1) << pin);

    status = axi_adrv9102_gpio_dgpio_pio_buf_mode_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, data);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_gpio_pin_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t *mode) {

    uint32_t data;
    uint32_t value;
    int32_t status;

    status = axi_adrv9102_gpio_dgpio_pio_data_mode_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, &data);
    if (status != 0) return(status);

    data = (data >> pin) & 0x1;
    if (data == 1) {
        *mode = AXI_ADRV9102_GPIO_MODE_HW_TDD;
        return(0);
    }

    status = axi_adrv9102_gpio_dgpio_pio_buf_mode_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, &data);
    if (status != 0) return(status);

    data = (data >> pin) & 0x1;
    if (data == 1) {
        *mode = AXI_ADRV9102_GPIO_MODE_SW_BUF;
        return(0);
    }

    *mode = AXI_ADRV9102_GPIO_MODE_SW_REG;
    return(0);
}

int32_t axi_adrv9102_gpio_group_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data) {

    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][0];

    return(AXI_REG_READ(a_device, peripheral_id, addr, data));
}

int32_t axi_adrv9102_gpio_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t data) {

    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][1];

    return(AXI_REG_WRITE(a_device, peripheral_id, addr, data));
}

int32_t axi_adrv9102_gpio_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data) {

    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][1];

    return(AXI_REG_READ(a_device, peripheral_id, addr, data));
}

int32_t axi_adrv9102_gpio_group_direction_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t data) {

    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][2];

    return(AXI_REG_WRITE(a_device, peripheral_id, addr, data));
}

int32_t axi_adrv9102_gpio_group_direction_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data) {

    uint32_t addr;

    addr = AXI_ADRV9102_GPIO_OFFSET + axi_adrv9102_gpio_pio_addr[group][2];

    return(AXI_REG_READ(a_device, peripheral_id, addr, data));
}

int32_t axi_adrv9102_gpio_pin_source_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t data) {

    return(axi_adrv9102_gpio_dgpio_pio_sel_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, pin, data));
}

int32_t axi_adrv9102_gpio_pin_source_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t *data) {

    return(axi_adrv9102_gpio_dgpio_pio_sel_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, pin, data));
}

int32_t axi_adrv9102_gpio_buf_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num, uint32_t data) {

    return(axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, num, data));
}

int32_t axi_adrv9102_gpio_buf_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num, uint32_t *data) {

    return(axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_get(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, num, data));
}

int32_t axi_adrv9102_gpio_trig_sel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin) {

    int32_t status;

    status = axi_adrv9102_gpio_trig_src_group_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, group);
    if (status != 0) return(status);

    status = axi_adrv9102_gpio_trig_src_pin_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, pin);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_gpio_gainindex_sel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pins[], uint32_t count) {

    int32_t pin;
    int32_t status;

    if (count != 8) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: gain index "
            "array count(%d) must be 8.\n", __func__, count);
        return(-1);
    }

    status = axi_adrv9102_gpio_gainindex_src_group_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, group);
    if (status != 0) return(status);

    for (pin = 0; pin < 8; pin++) {
        status = axi_adrv9102_gpio_gainindex_src_pin_set(a_device, peripheral_id, AXI_ADRV9102_GPIO_OFFSET, pin, pins[pin]);
        if (status != 0) return(status);
    }

    return(0);
}


// **********************************************************************************
// **********************************************************************************


// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_ssi_init_param(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_ssi_params_t *ssi_param, uint32_t ssi_mode) {

    ssi_param->gpio = 0;
    ssi_param->data_rate = 0;
    ssi_param->bit_order = 0;
    ssi_param->iq_order = 0;
    ssi_param->sign_extension = 0;
    ssi_param->edge = 0;
    ssi_param->ss_ratio = 0;

    ssi_param->tx_clk_sel = 0;
    ssi_param->tx_clk_msb = 0;
    ssi_param->tx_buf_enable = 0x3f;
    ssi_param->tx_ss_mode = 0;

    ssi_param->orx_ss_ratio = 0;

    ssi_param->mode = (ssi_mode >> 14) & 0x1;
    ssi_param->num_of_lanes = (ssi_mode >> 0) & 0x3;
    ssi_param->strb_type = (ssi_mode >> 8) & 0x1;
    ssi_param->num_of_bits = (ssi_mode >> 4) & 0x7;

    return(0);
}

int32_t axi_adrv9102_ssi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, axi_adrv9102_ssi_params_t *ssi_param) {

    uint32_t data_0;
    uint32_t data_1;
    int32_t status;

    status = axi_adrv9102_ssi_cmos_lvds_id_get(a_device, peripheral_id, ssi_id, &data_0);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_cmos1_lvds0_get(a_device, peripheral_id, ssi_id, &data_1);
    if (status != 0) return(status);

    if ((data_0 == AXI_ADRV9102_SSI_TYPE_GPIO) || (ssi_param->gpio != 0) || (data_1 != ssi_param->mode)) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: ssi-%d, incompatible type, "
            "hardware reconfiguration is required.\n", __func__, ssi_id);
        return(-1);
    }

    status = axi_adrv9102_ssi_num_of_lanes_set(a_device, peripheral_id, ssi_id, ssi_param->num_of_lanes);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_num_of_bits_set(a_device, peripheral_id, ssi_id, ssi_param->num_of_bits);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_lstrb1_pstrb0_set(a_device, peripheral_id, ssi_id, ssi_param->strb_type);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_sdr1_ddr0_set(a_device, peripheral_id, ssi_id, ssi_param->data_rate);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_lsb1_msb0_set(a_device, peripheral_id, ssi_id, ssi_param->bit_order);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_q1_i0_set(a_device, peripheral_id, ssi_id, ssi_param->iq_order);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_fall1_rise0_set(a_device, peripheral_id, ssi_id, ssi_param->edge);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_sign_extn_set(a_device, peripheral_id, ssi_id, ssi_param->sign_extension);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_clk_sel_set(a_device, peripheral_id, ssi_id, ssi_param->tx_clk_sel);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_clk_msb_set(a_device, peripheral_id, ssi_id, ssi_param->tx_clk_msb);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_buf_enable_set(a_device, peripheral_id, ssi_id, ssi_param->tx_buf_enable);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_ss_ratio_set(a_device, peripheral_id, ssi_id, ssi_param->ss_ratio);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_ss_mode_set(a_device, peripheral_id, ssi_id, ssi_param->tx_ss_mode);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_obs_ss_ratio_set(a_device, peripheral_id, ssi_id, ssi_param->orx_ss_ratio);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_ssi_data_pattern_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t data) {

    int32_t status;
    uint32_t value[2];

    value[0] = (uint32_t) (data & 0xffffffff);
    value[1] = (uint32_t) ((data >> 32) & 0xffffffff);

    status = axi_adrv9102_ssi_data_pat_0_set(a_device, peripheral_id, ssi_id, value[0]);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_data_pat_1_set(a_device, peripheral_id, ssi_id, value[1]);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_ssi_data_pattern_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t *data) {

    int32_t status;
    uint32_t value[2];

    status = axi_adrv9102_ssi_data_pat_0_get(a_device, peripheral_id, ssi_id, &value[0]);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_data_pat_1_get(a_device, peripheral_id, ssi_id, &value[1]);
    if (status != 0) return(status);

    *data = value[1];
    *data = (*data << 32) | value[0];
    return(0);
}

int32_t axi_adrv9102_ssi_data_received_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t *data) {

    int32_t status;
    uint32_t value[2];

    status = axi_adrv9102_ssi_data_0_get(a_device, peripheral_id, ssi_id, &value[0]);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_data_1_get(a_device, peripheral_id, ssi_id, &value[1]);
    if (status != 0) return(status);

    *data = value[1];
    *data = (*data << 32) | value[0];
    return(0);
}

int32_t axi_adrv9102_ssi_data_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data) {

    int32_t pin;
    int32_t status;

    for (pin = 0; pin < 5; pin++) {
        status = axi_adrv9102_ssi_wrdelay_set(a_device, peripheral_id, ssi_id, pin, data);
        if (status != 0) return(status);
    }

    return(0);
}

int32_t axi_adrv9102_ssi_clk_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data) {

    return(axi_adrv9102_ssi_wrdelay_set(a_device, peripheral_id, ssi_id, 5, data));
}

int32_t axi_adrv9102_ssi_clk_int_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data) {

    return(axi_adrv9102_ssi_wrdelay_set(a_device, peripheral_id, ssi_id, 6, data));
}

int32_t axi_adrv9102_ssi_clk_ext_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data) {

    return(axi_adrv9102_ssi_wrdelay_set(a_device, peripheral_id, ssi_id, 7, data));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_ssi_pattern_latency_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t *data) {

    int32_t status;
    uint32_t value;

    status = axi_adrv9102_ssi_data_pat_latency_get(a_device, peripheral_id, ssi_id, &value);
    if (status != 0) return(status);

    *data = value * 4;
    return(0);
}

int32_t axi_adrv9102_ssi_pattern_run(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t *data) {

    int32_t status;
    uint32_t value;

    *data = (uint32_t) -1;

    value = 0x1;

    status = axi_adrv9102_ssi_data_pat_start_set(a_device, peripheral_id, ssi_id, value);
    if (status != 0) return(status);
    status = axi_adrv9102_ssi_timer_set(a_device, peripheral_id, ssi_id, (uint32_t) -1);
    if (status != 0) return(status);

    while (value == 0x1) {
        status = axi_adrv9102_ssi_timer_get(a_device, peripheral_id, ssi_id, &value);
        if (status != 0) return(status);
        if (value == 0) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: ssi-%d, pattern latency run "
                "has timed out.\n", __func__, ssi_id);
            return(-1);
        }
        status = axi_adrv9102_ssi_data_pat_start_get(a_device, peripheral_id, ssi_id, &value);
        if (status != 0) return(status);
        if (value == 0x0) break;
    }

    return(axi_adrv9102_ssi_pattern_latency_get(a_device, peripheral_id, ssi_id, data));
}

int32_t axi_adrv9102_ssi_delay_calibrate(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t verbose, uint32_t *delay) {

    int32_t cur_start_delay;
    int32_t cur_stop_delay;
    int32_t cur_delay;
    int32_t cur_range;
    int32_t status;

    uint32_t sample_cnt;
    uint32_t ssi_rx_id;
    uint32_t wait_cnt;
    uint32_t sweep;
    uint32_t data;

    *delay = (uint32_t) -1;
    cur_start_delay = -1;
    cur_stop_delay = 0;
    cur_range = -1;

    ssi_rx_id = ssi_id;
    sweep = 0x0;

    status = axi_adrv9102_ssi_clk_mon_count_get(a_device, peripheral_id, ssi_id, &sample_cnt);
    if (status != 0) return(status);
    if (sample_cnt > 0) sample_cnt = 65535/sample_cnt;
    if (sample_cnt == 0) sample_cnt = 1;

    if (verbose == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_INFO, "%s: ssi-%d, sample-count (%d).\n",
            __func__, ssi_id, sample_cnt);
    }

    if (ssi_id == AXI_ADRV9102_SSI_ID_2) {
        ssi_rx_id = AXI_ADRV9102_SSI_ID_0;
    }

    status = axi_adrv9102_ssi_delay_unlocked_get(a_device, peripheral_id, ssi_id, &data);
    if (status != 0) return(status);
    if (data == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: ssi-%d, delay controller "
            "is NOT locked, can not continue.\n", __func__, ssi_id);
        return(-1);
    }

    if (verbose == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_INFO, "%s: ssi-%d, delay scan:\n", __func__, ssi_id);
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "  delay ");
        for (cur_delay = 0; cur_delay < 0x20; cur_delay++) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "-%02d", cur_delay);
        }
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "\n");
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "  data  ");
    }

    for (cur_delay = 0; cur_delay < 0x20; cur_delay++) {

        if (ssi_rx_id == ssi_id) {
            status = axi_adrv9102_ssi_data_delay_set(a_device, peripheral_id, ssi_id, (uint32_t) cur_delay);
            if (status != 0) return(status);
        } else {
            status = axi_adrv9102_ssi_clk_ext_delay_set(a_device, peripheral_id, ssi_id, (uint32_t) cur_delay);
            if (status != 0) return(status);
            status = axi_adrv9102_ssi_clk_delay_set(a_device, peripheral_id, ssi_id, (uint32_t) cur_delay);
            if (status != 0) return(status);
        }

        status = axi_adrv9102_ssi_timer_delay_us(a_device, peripheral_id, ssi_rx_id, (sample_cnt * 8));
        if (status != 0) return(status);
        status = axi_adrv9102_ssi_reg_status_set(a_device, peripheral_id, ssi_rx_id, (uint32_t) -1);
        if (status != 0) return(status);
        status = axi_adrv9102_ssi_timer_set(a_device, peripheral_id, ssi_rx_id, (sample_cnt * 32));
        if (status != 0) return(status);

        data = 0;
        while (data == 0) {
            status = axi_adrv9102_ssi_reg_status_get(a_device, peripheral_id, ssi_rx_id, &data);
            if (status != 0) return(status);
            status = axi_adrv9102_ssi_timer_get(a_device, peripheral_id, ssi_rx_id, &wait_cnt);
            if (status != 0) return(status);
            if (wait_cnt == 0) break;
        }

        if (verbose == 1) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "--%x", data);
        }

        if (data == 0) sweep = sweep | (0x1 << cur_delay);

        if ((data != 0x0) || (cur_delay == 0x1f)) {
            if (cur_start_delay >= 0) {
                cur_stop_delay = cur_delay;
                if ((cur_stop_delay - cur_start_delay) > cur_range) {
                    *delay = (cur_stop_delay + cur_start_delay)/2;
                    cur_range = cur_stop_delay - cur_start_delay;
                }
                cur_start_delay = -1;
            }
        } else if (data == 0x0) {
            if (cur_start_delay < 0) {
                cur_start_delay = cur_delay;
                cur_stop_delay = cur_delay;
            }
        }
    }

    status = axi_adrv9102_ssi_delay_sweep_set(a_device, peripheral_id, ssi_id, ~sweep);
    if (status != 0) return(status);

    if (verbose == 1) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_NOTE, "\n");
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_INFO, "%s: ssi-%d, delay sweep (0x%08x).\n",
            __func__, ssi_id, ~sweep);
    }

    if (cur_range < 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: ssi-%d, calibration failed "
            "no suitable delay found.\n", __func__, ssi_id);
        return(-2);
    }

    if (ssi_rx_id == ssi_id) {
        status = axi_adrv9102_ssi_data_delay_set(a_device, peripheral_id, ssi_id, *delay);
        if (status != 0) return(status);
    } else {
        status = axi_adrv9102_ssi_clk_ext_delay_set(a_device, peripheral_id, ssi_id, *delay);
        if (status != 0) return(status);
        status = axi_adrv9102_ssi_clk_delay_set(a_device, peripheral_id, ssi_id, *delay);
        if (status != 0) return(status);
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_tdd_frame_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_tdd_frame_params_t *tdd_param) {

    int32_t i;
    int32_t status;

    status = axi_adrv9102_tdd_frame_period_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->period);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_num_of_frames_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->num_of_frames);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_period_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->frame_switch_period);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_number_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->frame_switch_num);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_sequence_repeat_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->continous);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_enable_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_param->frame_switch_enable);
    if (status != 0) return(status);

    for (i = 0; i < 4; i++) {
        status = axi_adrv9102_tdd_trigger_mode_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, i, tdd_param->triggers[i]);
        if (status != 0) return(status);
    }

    return(0);
}

int32_t axi_adrv9102_tdd_frame_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_tdd_frame_params_t *tdd_param) {

    int32_t i;
    int32_t status;

    status = axi_adrv9102_tdd_frame_period_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->period);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_num_of_frames_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->num_of_frames);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_period_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->frame_switch_period);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_number_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->frame_switch_num);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_sequence_repeat_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->continous);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_frame_switch_enable_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, &tdd_param->frame_switch_enable);
    if (status != 0) return(status);

    for (i = 0; i < 4; i++) {
        status = axi_adrv9102_tdd_trigger_mode_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, i, &tdd_param->triggers[i]);
        if (status != 0) return(status);
    }

    return(0);
}

int32_t axi_adrv9102_tdd_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    return(axi_adrv9102_tdd_start_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, 0x1));
}

int32_t axi_adrv9102_tdd_stop(AXI_DEVICE_T *a_device, uint32_t peripheral_id) {

    return(axi_adrv9102_tdd_stop_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, 0x1));
}

int32_t axi_adrv9102_tdd_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data) {

    return(axi_adrv9102_tdd_start_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, data));
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_adrv9102_tdd_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t tdd_id, axi_adrv9102_tdd_enable_params_t *tdd_param) {

    int32_t status;

    status = axi_adrv9102_tdd_primary_assert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->primary_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_deassert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->primary_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_assert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->secondary_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_deassert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->secondary_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_frame_assert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->primary_frame_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_frame_deassert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->primary_frame_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_frame_assert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->secondary_frame_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_frame_deassert_set(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, tdd_param->secondary_frame_deassert);
    if (status != 0) return(status);

    return(0);
}

int32_t axi_adrv9102_tdd_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t tdd_id, axi_adrv9102_tdd_enable_params_t *tdd_param) {

    int32_t status;

    status = axi_adrv9102_tdd_primary_assert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->primary_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_deassert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->primary_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_assert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->secondary_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_deassert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->secondary_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_frame_assert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->primary_frame_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_primary_frame_deassert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->primary_frame_deassert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_frame_assert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->secondary_frame_assert);
    if (status != 0) return(status);
    status = axi_adrv9102_tdd_secondary_frame_deassert_get(a_device, peripheral_id, AXI_ADRV9102_TDD_OFFSET, tdd_id, &tdd_param->secondary_frame_deassert);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************
