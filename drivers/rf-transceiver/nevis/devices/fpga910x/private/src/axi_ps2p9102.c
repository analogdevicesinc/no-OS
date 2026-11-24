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
// Description:       AXI PS2P9102 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_ps2p9102.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_ps2p9102_iosel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t gpio[], uint32_t size) {

    int32_t status;

    uint32_t i2s_io_sclk_sel = 0;
    uint32_t i2s_io_ws_sel = 0;
    uint32_t i2s_io_msdi_sel = 0;
    uint32_t i2s_io_msdo_sel = 0;
    uint32_t qspi_io_sd3_sel = 0;
    uint32_t qspi_io_ss1_sel = 0;
    uint32_t uart_io_rx_sel = 0;
    uint32_t uart_io_tx_sel = 0;
    uint32_t sspi_io_ss0_sel = 0;
    uint32_t sspi_io_ss1_sel = 0;
    uint32_t sspi_io_ss2_sel = 0;
    uint32_t sspi_io_ss3_sel = 0;
    uint32_t sspi_io_sclk_sel = 0;
    uint32_t sspi_io_miso_sel = 0;
    uint32_t sspi_io_mosi_sel = 0;
    uint32_t i2c_io_scl_sel = 0;
    uint32_t i2c_io_sda_sel = 0;

    if (size != 20) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid gpio size (%d).\n", __func__, size);
        return(-100);
    }

    if (gpio[ 0] == AXI_PS2P9102_TOP_SSPI_IO_SS0) sspi_io_ss0_sel = AXI_PS2P9102_TOP_SSPI_IO_SS0_GPIO00;
    if (gpio[ 1] == AXI_PS2P9102_TOP_SSPI_IO_SS1) sspi_io_ss1_sel = AXI_PS2P9102_TOP_SSPI_IO_SS1_GPIO01;
    if (gpio[ 2] == AXI_PS2P9102_TOP_SSPI_IO_SS2) sspi_io_ss2_sel = AXI_PS2P9102_TOP_SSPI_IO_SS2_GPIO02;
    if (gpio[ 3] == AXI_PS2P9102_TOP_SSPI_IO_SS3) sspi_io_ss3_sel = AXI_PS2P9102_TOP_SSPI_IO_SS3_GPIO03;
    if (gpio[ 3] == AXI_PS2P9102_TOP_QSPI_IO_SD3) qspi_io_sd3_sel = AXI_PS2P9102_TOP_QSPI_IO_SD3_GPIO03;
    if (gpio[ 4] == AXI_PS2P9102_TOP_SSPI_IO_SS0) sspi_io_ss0_sel = AXI_PS2P9102_TOP_SSPI_IO_SS0_GPIO04;
    if (gpio[ 4] == AXI_PS2P9102_TOP_QSPI_IO_SS1) qspi_io_ss1_sel = AXI_PS2P9102_TOP_QSPI_IO_SS1_GPIO04;
    if (gpio[ 5] == AXI_PS2P9102_TOP_SSPI_IO_SS1) sspi_io_ss1_sel = AXI_PS2P9102_TOP_SSPI_IO_SS1_GPIO05;
    if (gpio[ 6] == AXI_PS2P9102_TOP_SSPI_IO_SS2) sspi_io_ss2_sel = AXI_PS2P9102_TOP_SSPI_IO_SS2_GPIO06;
    if (gpio[ 6] == AXI_PS2P9102_TOP_I2C_IO_SCL) i2c_io_scl_sel = AXI_PS2P9102_TOP_I2C_IO_SCL_GPIO06;
    if (gpio[ 7] == AXI_PS2P9102_TOP_SSPI_IO_SS3) sspi_io_ss3_sel = AXI_PS2P9102_TOP_SSPI_IO_SS3_GPIO07;
    if (gpio[ 7] == AXI_PS2P9102_TOP_I2C_IO_SDA) i2c_io_sda_sel = AXI_PS2P9102_TOP_I2C_IO_SDA_GPIO07;
    if (gpio[ 8] == AXI_PS2P9102_TOP_SSPI_IO_SS0) sspi_io_ss0_sel = AXI_PS2P9102_TOP_SSPI_IO_SS0_GPIO08;
    if (gpio[ 9] == AXI_PS2P9102_TOP_SSPI_IO_SCLK) sspi_io_sclk_sel = AXI_PS2P9102_TOP_SSPI_IO_SCLK_GPIO09;
    if (gpio[10] == AXI_PS2P9102_TOP_SSPI_IO_MISO) sspi_io_miso_sel = AXI_PS2P9102_TOP_SSPI_IO_MISO_GPIO10;
    if (gpio[11] == AXI_PS2P9102_TOP_SSPI_IO_MOSI) sspi_io_mosi_sel = AXI_PS2P9102_TOP_SSPI_IO_MOSI_GPIO11;
    if (gpio[12] == AXI_PS2P9102_TOP_SSPI_IO_SS0) sspi_io_ss0_sel = AXI_PS2P9102_TOP_SSPI_IO_SS0_GPIO12;
    if (gpio[12] == AXI_PS2P9102_TOP_I2S_IO_MSDO) i2s_io_msdo_sel = AXI_PS2P9102_TOP_I2S_IO_MSDO_GPIO12;
    if (gpio[13] == AXI_PS2P9102_TOP_SSPI_IO_SCLK) sspi_io_sclk_sel = AXI_PS2P9102_TOP_SSPI_IO_SCLK_GPIO13;
    if (gpio[13] == AXI_PS2P9102_TOP_I2S_IO_SCLK) i2s_io_sclk_sel = AXI_PS2P9102_TOP_I2S_IO_SCLK_GPIO13;
    if (gpio[14] == AXI_PS2P9102_TOP_SSPI_IO_MISO) sspi_io_miso_sel = AXI_PS2P9102_TOP_SSPI_IO_MISO_GPIO14;
    if (gpio[14] == AXI_PS2P9102_TOP_QSPI_IO_SD3) qspi_io_sd3_sel = AXI_PS2P9102_TOP_QSPI_IO_SD3_GPIO14;
    if (gpio[14] == AXI_PS2P9102_TOP_UART_IO_RXDATA) uart_io_rx_sel = AXI_PS2P9102_TOP_UART_IO_RX_GPIO14;
    if (gpio[14] == AXI_PS2P9102_TOP_I2S_IO_WS) i2s_io_ws_sel = AXI_PS2P9102_TOP_I2S_IO_WS_GPIO14;
    if (gpio[15] == AXI_PS2P9102_TOP_SSPI_IO_MOSI) sspi_io_mosi_sel = AXI_PS2P9102_TOP_SSPI_IO_MOSI_GPIO15;
    if (gpio[15] == AXI_PS2P9102_TOP_QSPI_IO_SS1) qspi_io_ss1_sel = AXI_PS2P9102_TOP_QSPI_IO_SS1_GPIO15;
    if (gpio[15] == AXI_PS2P9102_TOP_UART_IO_TXDATA) uart_io_tx_sel = AXI_PS2P9102_TOP_UART_IO_TX_GPIO15;
    if (gpio[15] == AXI_PS2P9102_TOP_I2S_IO_MSDI) i2s_io_msdi_sel = AXI_PS2P9102_TOP_I2S_IO_MSDI_GPIO15;
    if (gpio[16] == AXI_PS2P9102_TOP_SSPI_IO_SS1) sspi_io_ss1_sel = AXI_PS2P9102_TOP_SSPI_IO_SS1_GPIO16;
    if (gpio[16] == AXI_PS2P9102_TOP_QSPI_IO_SS1) qspi_io_ss1_sel = AXI_PS2P9102_TOP_QSPI_IO_SS1_GPIO16;
    if (gpio[16] == AXI_PS2P9102_TOP_I2S_IO_MSDO) i2s_io_msdo_sel = AXI_PS2P9102_TOP_I2S_IO_MSDO_GPIO16;
    if (gpio[17] == AXI_PS2P9102_TOP_SSPI_IO_SS2) sspi_io_ss2_sel = AXI_PS2P9102_TOP_SSPI_IO_SS2_GPIO17;
    if (gpio[17] == AXI_PS2P9102_TOP_I2S_IO_SCLK) i2s_io_sclk_sel = AXI_PS2P9102_TOP_I2S_IO_SCLK_GPIO17;
    if (gpio[18] == AXI_PS2P9102_TOP_SSPI_IO_SS3) sspi_io_ss3_sel = AXI_PS2P9102_TOP_SSPI_IO_SS3_GPIO18;
    if (gpio[18] == AXI_PS2P9102_TOP_UART_IO_RXDATA) uart_io_rx_sel = AXI_PS2P9102_TOP_UART_IO_RX_GPIO18;
    if (gpio[18] == AXI_PS2P9102_TOP_I2C_IO_SCL) i2c_io_scl_sel = AXI_PS2P9102_TOP_I2C_IO_SCL_GPIO18;
    if (gpio[18] == AXI_PS2P9102_TOP_I2S_IO_WS) i2s_io_ws_sel = AXI_PS2P9102_TOP_I2S_IO_WS_GPIO18;
    if (gpio[19] == AXI_PS2P9102_TOP_UART_IO_TXDATA) uart_io_tx_sel = AXI_PS2P9102_TOP_UART_IO_TX_GPIO19;
    if (gpio[19] == AXI_PS2P9102_TOP_I2C_IO_SDA) i2c_io_sda_sel = AXI_PS2P9102_TOP_I2C_IO_SDA_GPIO19;
    if (gpio[19] == AXI_PS2P9102_TOP_I2S_IO_MSDI) i2s_io_msdi_sel = AXI_PS2P9102_TOP_I2S_IO_MSDI_GPIO19;

    status = axi_ps2p9102_top_i2s_io_sclk_sel_set(a_device, peripheral_id, i2s_io_sclk_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_i2s_io_ws_sel_set(a_device, peripheral_id, i2s_io_ws_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_i2s_io_msdo_sel_set(a_device, peripheral_id, i2s_io_msdo_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_i2s_io_msdi_sel_set(a_device, peripheral_id, i2s_io_msdi_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_qspi_io_sd3_sel_set(a_device, peripheral_id, qspi_io_sd3_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_qspi_io_ss1_sel_set(a_device, peripheral_id, qspi_io_ss1_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_io_tx_sel_set(a_device, peripheral_id, uart_io_tx_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_uart_io_rx_sel_set(a_device, peripheral_id, uart_io_rx_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_ss0_sel_set(a_device, peripheral_id, sspi_io_ss0_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_ss1_sel_set(a_device, peripheral_id, sspi_io_ss1_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_ss2_sel_set(a_device, peripheral_id, sspi_io_ss2_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_ss3_sel_set(a_device, peripheral_id, sspi_io_ss3_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_sclk_sel_set(a_device, peripheral_id, sspi_io_sclk_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_mosi_sel_set(a_device, peripheral_id, sspi_io_mosi_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_sspi_io_miso_sel_set(a_device, peripheral_id, sspi_io_miso_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_i2c_io_scl_sel_set(a_device, peripheral_id, i2c_io_scl_sel);
    if (status != 0) return(status);
    status = axi_ps2p9102_top_i2c_io_sda_sel_set(a_device, peripheral_id, i2c_io_sda_sel);
    if (status != 0) return(status);

    return(0);
}

// **********************************************************************************
// **********************************************************************************
