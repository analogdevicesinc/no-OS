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
// Description:       AXI UART Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_ps2p9102_uart.h"

#define AXI_PS2P9102_UART_TIMEOUT_NS 1e9

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_uart_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_ps2p9102_uart_params_t *uart_param) {

    uint32_t diff;
    int32_t status;
    uint32_t clk_period;
    uint32_t clk_freq_hz;

    if (uart_param->clk_freq_hz == 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "uart clock frequency can not be 0!\n", __func__);
        return(-100);
    }

    if (uart_param->baud_rate == 0) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
            "baud rate (or divider) can not be 0!\n", __func__);
        return(-101);
    }

    clk_period = uart_param->baud_rate;

    if (uart_param->div_mode == 0) {

        status = axi_ps2p9102_top_uart_clk_mon_count_freq_hz(a_device, peripheral_id, &clk_freq_hz);
        if (status != 0) return(status);
        if (clk_freq_hz == 0) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: "
                "no uart clock detected!\n", __func__);
            return(-102);
        }
 
        diff = abs(uart_param->clk_freq_hz - clk_freq_hz);
        diff = (diff * 100) / uart_param->clk_freq_hz;
        if (diff != 0) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: mismatched hardware (%d Hz) "
                "and software (%d Hz) clock frequencies!\n", __func__, clk_freq_hz,
                uart_param->clk_freq_hz);
            return(-104);
        }
 
        clk_period = uart_param->clk_freq_hz / uart_param->baud_rate;
        diff = abs(uart_param->clk_freq_hz - (clk_period * uart_param->baud_rate));
        if (diff >= (uart_param->baud_rate / 2)) {
            clk_period = clk_period + 1;
        }

        if (clk_period == 0) {
            AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: requested buad rate (%d) "
                "is not supported!\n", __func__, uart_param->baud_rate);
            return(-105);
        }
    }

    status = axi_ps2p9102_top_uart_clk_period_set(a_device, peripheral_id, clk_period);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_bit_width_set(a_device, peripheral_id, uart_param->bit_width);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_stop_bits_set(a_device, peripheral_id, uart_param->stop_bits);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_parity_enable_set(a_device, peripheral_id, uart_param->parity_enable);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_parity_type_set(a_device, peripheral_id, uart_param->parity_type);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_rdy_polarity_set(a_device, peripheral_id, uart_param->ready_polarity);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_rdy_mode_set(a_device, peripheral_id, uart_param->ready_mode);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_loopback_set(a_device, peripheral_id, uart_param->loopback);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_swreset_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_enable_set(a_device, peripheral_id, 0x1);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_uart_tx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t data) {

    return(axi_ps2p9102_top_uart_tx_data_set(a_device, peripheral_id, (uint32_t) data));
}

int32_t axi_ps2p9102_uart_send(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t txdata[],
    uint32_t count, uint32_t *send_count) {

    int32_t i;
    uint32_t data;
    int32_t status;
    int32_t cur_size;
    uint32_t total_size;

    total_size = 0;
    *send_count = total_size;

    status = axi_ps2p9102_top_uart_timer_set(a_device, peripheral_id, (AXI_PS2P9102_UART_TIMEOUT_NS / 10));
    if (status != 0) return(status);
    while (total_size < count) {
        status = axi_ps2p9102_top_uart_timer_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if (data == 0) break;
        status = axi_ps2p9102_top_uart_tx_empty_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if (data == 0) continue;
        cur_size = count - total_size;
        if (cur_size < 0) break;
        if (cur_size > 16) cur_size = 16;
        for (i = 0; i < cur_size; i++) {
            status = axi_ps2p9102_uart_tx_data(a_device, peripheral_id, txdata[(total_size + i)]);
            if (status != 0) return(status);
        }
        total_size = total_size + cur_size;
        *send_count = total_size;
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_uart_rx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t *data) {

    uint32_t value;
    int32_t status;

    status = axi_ps2p9102_top_uart_rx_data_get(a_device, peripheral_id, &value);
    if (status != 0) return(status);

    *data = (uint8_t) (value & 0xff);
    return(0);
}

int32_t axi_ps2p9102_uart_recv(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t rxdata[],
    uint32_t count, uint32_t *recv_count, uint32_t *recv_status) {

    uint32_t data;
    uint32_t size;
    int32_t status;

    size = 0;
    *recv_status = 0;
    *recv_count = size;

    status = axi_ps2p9102_top_uart_timer_set(a_device, peripheral_id, (AXI_PS2P9102_UART_TIMEOUT_NS / 10));
    if (status != 0) return(status);
    while (size < count) {
        status = axi_ps2p9102_top_uart_timer_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if (data == 0) break;
        status = axi_ps2p9102_top_uart_rx_empty_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        if (data == 1) continue;
        status = axi_ps2p9102_uart_rx_data(a_device, peripheral_id, &rxdata[size]);
        if (status != 0) return(status);
        size = size + 1;
        *recv_count = size;
        status = axi_ps2p9102_top_reg_uart_rx_status_get(a_device, peripheral_id, &data);
        if (status != 0) return(status);
        *recv_status = *recv_status | data;
        status = axi_ps2p9102_top_reg_uart_rx_status_set(a_device, peripheral_id, 0xff);
        if (status != 0) return(status);
    }

    return(0);
}

// **********************************************************************************
// **********************************************************************************
