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
// Description:       AXI AXI_PS2P9102_TOP Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_ps2p9102_top_regs.h"
#include "axi_ps2p9102_top.h"


/*  register field read, axi_ps2p9102_top_version */

int32_t axi_ps2p9102_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_instance_id */

int32_t axi_ps2p9102_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_INSTANCE_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_scratch */

int32_t axi_ps2p9102_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_scratch */

int32_t axi_ps2p9102_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_timer */

int32_t axi_ps2p9102_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_timer */

int32_t axi_ps2p9102_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_timer */

int32_t axi_ps2p9102_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_timer */

int32_t axi_ps2p9102_top_i2s_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_timer */

int32_t axi_ps2p9102_top_i2s_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_i2s_timer */

int32_t axi_ps2p9102_top_i2s_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_scratch */

int32_t axi_ps2p9102_top_i2s_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_scratch */

int32_t axi_ps2p9102_top_i2s_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_SCRATCH_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_i2s_io_control */

int32_t axi_ps2p9102_top_reg_i2s_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2S_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_i2s_io_control */

int32_t axi_ps2p9102_top_reg_i2s_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2S_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_io_enable */

int32_t axi_ps2p9102_top_i2s_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_ENABLE_POS, AXI_PS2P9102_TOP_I2S_IO_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_io_enable */

int32_t axi_ps2p9102_top_i2s_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_ENABLE_POS, AXI_PS2P9102_TOP_I2S_IO_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_io_sclk_sel */

int32_t axi_ps2p9102_top_i2s_io_sclk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_io_sclk_sel */

int32_t axi_ps2p9102_top_i2s_io_sclk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_SCLK_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_io_ws_sel */

int32_t axi_ps2p9102_top_i2s_io_ws_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_WS_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_WS_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_WS_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_io_ws_sel */

int32_t axi_ps2p9102_top_i2s_io_ws_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_WS_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_WS_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_WS_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_io_msdo_sel */

int32_t axi_ps2p9102_top_i2s_io_msdo_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_io_msdo_sel */

int32_t axi_ps2p9102_top_i2s_io_msdo_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_MSDO_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_io_msdi_sel */

int32_t axi_ps2p9102_top_i2s_io_msdi_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_io_msdi_sel */

int32_t axi_ps2p9102_top_i2s_io_msdi_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_POS, AXI_PS2P9102_TOP_I2S_IO_MSDI_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_swreset */

int32_t axi_ps2p9102_top_i2s_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_swreset */

int32_t axi_ps2p9102_top_i2s_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_enable */

int32_t axi_ps2p9102_top_i2s_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_enable */

int32_t axi_ps2p9102_top_i2s_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_ENABLE_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_i2s_control */

int32_t axi_ps2p9102_top_reg_i2s_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2S_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_i2s_control */

int32_t axi_ps2p9102_top_reg_i2s_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2S_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_master */

int32_t axi_ps2p9102_top_i2s_master_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_MASTER_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_MASTER_POS, AXI_PS2P9102_TOP_I2S_MASTER_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_master */

int32_t axi_ps2p9102_top_i2s_master_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_MASTER_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_MASTER_POS, AXI_PS2P9102_TOP_I2S_MASTER_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_clk_period */

int32_t axi_ps2p9102_top_i2s_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_CLK_PERIOD_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_CLK_PERIOD_POS, AXI_PS2P9102_TOP_I2S_CLK_PERIOD_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_clk_period */

int32_t axi_ps2p9102_top_i2s_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_CLK_PERIOD_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_CLK_PERIOD_POS, AXI_PS2P9102_TOP_I2S_CLK_PERIOD_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_no_of_bytes */

int32_t axi_ps2p9102_top_i2s_no_of_bytes_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_POS, AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2s_no_of_bytes */

int32_t axi_ps2p9102_top_i2s_no_of_bytes_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_POS, AXI_PS2P9102_TOP_I2S_NO_OF_BYTES_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_tx_data */

int32_t axi_ps2p9102_top_i2s_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_tx_data */

int32_t axi_ps2p9102_top_i2s_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TX_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_rx_data */

int32_t axi_ps2p9102_top_i2s_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_RX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2s_rx_data */

int32_t axi_ps2p9102_top_i2s_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_RX_DATA_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_i2s_status */

int32_t axi_ps2p9102_top_reg_i2s_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2S_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2s_tx_full */

int32_t axi_ps2p9102_top_i2s_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_TX_FULL_POS, AXI_PS2P9102_TOP_I2S_TX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_tx_empty */

int32_t axi_ps2p9102_top_i2s_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_TX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_TX_EMPTY_POS, AXI_PS2P9102_TOP_I2S_TX_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_rx_full */

int32_t axi_ps2p9102_top_i2s_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_RX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_RX_FULL_POS, AXI_PS2P9102_TOP_I2S_RX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2s_rx_empty */

int32_t axi_ps2p9102_top_i2s_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2S_RX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_I2S_RX_EMPTY_POS, AXI_PS2P9102_TOP_I2S_RX_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_timer */

int32_t axi_ps2p9102_top_qspi_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_timer */

int32_t axi_ps2p9102_top_qspi_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_qspi_timer */

int32_t axi_ps2p9102_top_qspi_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_scratch */

int32_t axi_ps2p9102_top_qspi_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_scratch */

int32_t axi_ps2p9102_top_qspi_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SCRATCH_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_qspi_io_control */

int32_t axi_ps2p9102_top_reg_qspi_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_qspi_io_control */

int32_t axi_ps2p9102_top_reg_qspi_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_io_enable */

int32_t axi_ps2p9102_top_qspi_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_ENABLE_POS, AXI_PS2P9102_TOP_QSPI_IO_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_io_enable */

int32_t axi_ps2p9102_top_qspi_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_ENABLE_POS, AXI_PS2P9102_TOP_QSPI_IO_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_io_sd3_sel */

int32_t axi_ps2p9102_top_qspi_io_sd3_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_POS, AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_io_sd3_sel */

int32_t axi_ps2p9102_top_qspi_io_sd3_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_POS, AXI_PS2P9102_TOP_QSPI_IO_SD3_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_io_ss1_sel */

int32_t axi_ps2p9102_top_qspi_io_ss1_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_POS, AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_io_ss1_sel */

int32_t axi_ps2p9102_top_qspi_io_ss1_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_POS, AXI_PS2P9102_TOP_QSPI_IO_SS1_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_ipsel */

int32_t axi_ps2p9102_top_qspi_ipsel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IPSEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IPSEL_POS, AXI_PS2P9102_TOP_QSPI_IPSEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_ipsel */

int32_t axi_ps2p9102_top_qspi_ipsel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_IPSEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_IPSEL_POS, AXI_PS2P9102_TOP_QSPI_IPSEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_swreset */

int32_t axi_ps2p9102_top_qspi_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_swreset */

int32_t axi_ps2p9102_top_qspi_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_enable */

int32_t axi_ps2p9102_top_qspi_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_enable */

int32_t axi_ps2p9102_top_qspi_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_ENABLE_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_qspi_control */

int32_t axi_ps2p9102_top_reg_qspi_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_qspi_control */

int32_t axi_ps2p9102_top_reg_qspi_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_master */

int32_t axi_ps2p9102_top_qspi_master_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_MASTER_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_MASTER_POS, AXI_PS2P9102_TOP_QSPI_MASTER_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_master */

int32_t axi_ps2p9102_top_qspi_master_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_MASTER_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_MASTER_POS, AXI_PS2P9102_TOP_QSPI_MASTER_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_clk_period */

int32_t axi_ps2p9102_top_qspi_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_POS, AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_clk_period */

int32_t axi_ps2p9102_top_qspi_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_POS, AXI_PS2P9102_TOP_QSPI_CLK_PERIOD_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_tx_edge_sel */

int32_t axi_ps2p9102_top_qspi_tx_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_POS, AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_tx_edge_sel */

int32_t axi_ps2p9102_top_qspi_tx_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_POS, AXI_PS2P9102_TOP_QSPI_TX_EDGE_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_rx_edge_sel */

int32_t axi_ps2p9102_top_qspi_rx_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_POS, AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_rx_edge_sel */

int32_t axi_ps2p9102_top_qspi_rx_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_POS, AXI_PS2P9102_TOP_QSPI_RX_EDGE_SEL_MASK, data));
}


/*  register read, axi_ps2p9102_top_reg_qspi_counts */

int32_t axi_ps2p9102_top_reg_qspi_counts_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_COUNTS_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_qspi_counts */

int32_t axi_ps2p9102_top_reg_qspi_counts_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_COUNTS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_bidir_count */

int32_t axi_ps2p9102_top_qspi_bidir_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_POS, AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_bidir_count */

int32_t axi_ps2p9102_top_qspi_bidir_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_POS, AXI_PS2P9102_TOP_QSPI_BIDIR_COUNT_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_tx_count */

int32_t axi_ps2p9102_top_qspi_tx_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_COUNT_POS, AXI_PS2P9102_TOP_QSPI_TX_COUNT_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_tx_count */

int32_t axi_ps2p9102_top_qspi_tx_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_COUNT_POS, AXI_PS2P9102_TOP_QSPI_TX_COUNT_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_rx_count */

int32_t axi_ps2p9102_top_qspi_rx_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_COUNT_POS, AXI_PS2P9102_TOP_QSPI_RX_COUNT_MASK, data));
}


/*  register field write, axi_ps2p9102_top_qspi_rx_count */

int32_t axi_ps2p9102_top_qspi_rx_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_COUNT_POS, AXI_PS2P9102_TOP_QSPI_RX_COUNT_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_tx_data */

int32_t axi_ps2p9102_top_qspi_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_tx_data */

int32_t axi_ps2p9102_top_qspi_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_rx_data */

int32_t axi_ps2p9102_top_qspi_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_rx_data */

int32_t axi_ps2p9102_top_qspi_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_DATA_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_qspi_status */

int32_t axi_ps2p9102_top_reg_qspi_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_QSPI_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_tx_full */

int32_t axi_ps2p9102_top_qspi_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_FULL_POS, AXI_PS2P9102_TOP_QSPI_TX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_tx_empty */

int32_t axi_ps2p9102_top_qspi_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_TX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_TX_EMPTY_POS, AXI_PS2P9102_TOP_QSPI_TX_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_rx_full */

int32_t axi_ps2p9102_top_qspi_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_FULL_POS, AXI_PS2P9102_TOP_QSPI_RX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_rx_empty */

int32_t axi_ps2p9102_top_qspi_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_RX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_QSPI_RX_EMPTY_POS, AXI_PS2P9102_TOP_QSPI_RX_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_qspi_ss */

int32_t axi_ps2p9102_top_qspi_ss_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_qspi_ss_out */

int32_t axi_ps2p9102_top_qspi_ss_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SS_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_qspi_ss_out */

int32_t axi_ps2p9102_top_qspi_ss_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_QSPI_SS_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_timer */

int32_t axi_ps2p9102_top_uart_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_timer */

int32_t axi_ps2p9102_top_uart_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_uart_timer */

int32_t axi_ps2p9102_top_uart_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_scratch */

int32_t axi_ps2p9102_top_uart_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_scratch */

int32_t axi_ps2p9102_top_uart_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_SCRATCH_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_uart_io_control */

int32_t axi_ps2p9102_top_reg_uart_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_uart_io_control */

int32_t axi_ps2p9102_top_reg_uart_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_io_enable */

int32_t axi_ps2p9102_top_uart_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_ENABLE_POS, AXI_PS2P9102_TOP_UART_IO_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_io_enable */

int32_t axi_ps2p9102_top_uart_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_ENABLE_POS, AXI_PS2P9102_TOP_UART_IO_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_io_tx_sel */

int32_t axi_ps2p9102_top_uart_io_tx_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_TX_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_TX_SEL_POS, AXI_PS2P9102_TOP_UART_IO_TX_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_io_tx_sel */

int32_t axi_ps2p9102_top_uart_io_tx_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_TX_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_TX_SEL_POS, AXI_PS2P9102_TOP_UART_IO_TX_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_io_rx_sel */

int32_t axi_ps2p9102_top_uart_io_rx_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_RX_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_RX_SEL_POS, AXI_PS2P9102_TOP_UART_IO_RX_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_io_rx_sel */

int32_t axi_ps2p9102_top_uart_io_rx_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_IO_RX_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_IO_RX_SEL_POS, AXI_PS2P9102_TOP_UART_IO_RX_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_swreset */

int32_t axi_ps2p9102_top_uart_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_swreset */

int32_t axi_ps2p9102_top_uart_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_enable */

int32_t axi_ps2p9102_top_uart_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_enable */

int32_t axi_ps2p9102_top_uart_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_ENABLE_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_uart_control */

int32_t axi_ps2p9102_top_reg_uart_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_uart_control */

int32_t axi_ps2p9102_top_reg_uart_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_bit_width */

int32_t axi_ps2p9102_top_uart_bit_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_BIT_WIDTH_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_BIT_WIDTH_POS, AXI_PS2P9102_TOP_UART_BIT_WIDTH_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_bit_width */

int32_t axi_ps2p9102_top_uart_bit_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_BIT_WIDTH_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_BIT_WIDTH_POS, AXI_PS2P9102_TOP_UART_BIT_WIDTH_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_stop_bits */

int32_t axi_ps2p9102_top_uart_stop_bits_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_STOP_BITS_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_STOP_BITS_POS, AXI_PS2P9102_TOP_UART_STOP_BITS_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_stop_bits */

int32_t axi_ps2p9102_top_uart_stop_bits_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_STOP_BITS_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_STOP_BITS_POS, AXI_PS2P9102_TOP_UART_STOP_BITS_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_parity_enable */

int32_t axi_ps2p9102_top_uart_parity_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_PARITY_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_PARITY_ENABLE_POS, AXI_PS2P9102_TOP_UART_PARITY_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_parity_enable */

int32_t axi_ps2p9102_top_uart_parity_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_PARITY_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_PARITY_ENABLE_POS, AXI_PS2P9102_TOP_UART_PARITY_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_parity_type */

int32_t axi_ps2p9102_top_uart_parity_type_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_PARITY_TYPE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_PARITY_TYPE_POS, AXI_PS2P9102_TOP_UART_PARITY_TYPE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_parity_type */

int32_t axi_ps2p9102_top_uart_parity_type_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_PARITY_TYPE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_PARITY_TYPE_POS, AXI_PS2P9102_TOP_UART_PARITY_TYPE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_rdy_polarity */

int32_t axi_ps2p9102_top_uart_rdy_polarity_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RDY_POLARITY_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RDY_POLARITY_POS, AXI_PS2P9102_TOP_UART_RDY_POLARITY_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_rdy_polarity */

int32_t axi_ps2p9102_top_uart_rdy_polarity_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RDY_POLARITY_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RDY_POLARITY_POS, AXI_PS2P9102_TOP_UART_RDY_POLARITY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_rdy_mode */

int32_t axi_ps2p9102_top_uart_rdy_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RDY_MODE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RDY_MODE_POS, AXI_PS2P9102_TOP_UART_RDY_MODE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_rdy_mode */

int32_t axi_ps2p9102_top_uart_rdy_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RDY_MODE_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RDY_MODE_POS, AXI_PS2P9102_TOP_UART_RDY_MODE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_loopback */

int32_t axi_ps2p9102_top_uart_loopback_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_LOOPBACK_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_LOOPBACK_POS, AXI_PS2P9102_TOP_UART_LOOPBACK_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_loopback */

int32_t axi_ps2p9102_top_uart_loopback_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_LOOPBACK_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_LOOPBACK_POS, AXI_PS2P9102_TOP_UART_LOOPBACK_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_clk_period */

int32_t axi_ps2p9102_top_uart_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_CLK_PERIOD_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_clk_period */

int32_t axi_ps2p9102_top_uart_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_CLK_PERIOD_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_tx_data */

int32_t axi_ps2p9102_top_uart_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_tx_data */

int32_t axi_ps2p9102_top_uart_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TX_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_tx_rdy */

int32_t axi_ps2p9102_top_uart_tx_rdy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TX_RDY_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_data */

int32_t axi_ps2p9102_top_uart_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_rx_data */

int32_t axi_ps2p9102_top_uart_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_rdy */

int32_t axi_ps2p9102_top_uart_rx_rdy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_RDY_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_uart_rx_rdy */

int32_t axi_ps2p9102_top_uart_rx_rdy_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_RDY_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_uart_status */

int32_t axi_ps2p9102_top_reg_uart_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_tx_full */

int32_t axi_ps2p9102_top_uart_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_TX_FULL_POS, AXI_PS2P9102_TOP_UART_TX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_tx_empty */

int32_t axi_ps2p9102_top_uart_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_TX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_TX_EMPTY_POS, AXI_PS2P9102_TOP_UART_TX_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_full */

int32_t axi_ps2p9102_top_uart_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_FULL_POS, AXI_PS2P9102_TOP_UART_RX_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_empty */

int32_t axi_ps2p9102_top_uart_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_EMPTY_POS, AXI_PS2P9102_TOP_UART_RX_EMPTY_MASK, data));
}


/*  register read, axi_ps2p9102_top_reg_uart_rx_status */

int32_t axi_ps2p9102_top_reg_uart_rx_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_RX_STATUS_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_uart_rx_status */

int32_t axi_ps2p9102_top_reg_uart_rx_status_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_UART_RX_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_perror */

int32_t axi_ps2p9102_top_uart_rx_perror_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_PERROR_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_PERROR_POS, AXI_PS2P9102_TOP_UART_RX_PERROR_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_rx_perror */

int32_t axi_ps2p9102_top_uart_rx_perror_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_PERROR_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_PERROR_POS, AXI_PS2P9102_TOP_UART_RX_PERROR_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_rx_ovf */

int32_t axi_ps2p9102_top_uart_rx_ovf_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_OVF_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_OVF_POS, AXI_PS2P9102_TOP_UART_RX_OVF_MASK, data));
}


/*  register field write, axi_ps2p9102_top_uart_rx_ovf */

int32_t axi_ps2p9102_top_uart_rx_ovf_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_RX_OVF_ADDR + addr_offset), AXI_PS2P9102_TOP_UART_RX_OVF_POS, AXI_PS2P9102_TOP_UART_RX_OVF_MASK, data));
}


/*  register field read, axi_ps2p9102_top_uart_clk_mon_count */

int32_t axi_ps2p9102_top_uart_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_ps2p9102_top_uart_clk_mon_count */

int32_t axi_ps2p9102_top_uart_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_PS2P9102_TOP_UART_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_timer */

int32_t axi_ps2p9102_top_sspi_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_timer */

int32_t axi_ps2p9102_top_sspi_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_sspi_timer */

int32_t axi_ps2p9102_top_sspi_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_scratch */

int32_t axi_ps2p9102_top_sspi_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_scratch */

int32_t axi_ps2p9102_top_sspi_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SCRATCH_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_sspi_io_control */

int32_t axi_ps2p9102_top_reg_sspi_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_SSPI_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_sspi_io_control */

int32_t axi_ps2p9102_top_reg_sspi_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_SSPI_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_enable */

int32_t axi_ps2p9102_top_sspi_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_ENABLE_POS, AXI_PS2P9102_TOP_SSPI_IO_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_enable */

int32_t axi_ps2p9102_top_sspi_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_ENABLE_POS, AXI_PS2P9102_TOP_SSPI_IO_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_ss0_sel */

int32_t axi_ps2p9102_top_sspi_io_ss0_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_ss0_sel */

int32_t axi_ps2p9102_top_sspi_io_ss0_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS0_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_ss1_sel */

int32_t axi_ps2p9102_top_sspi_io_ss1_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_ss1_sel */

int32_t axi_ps2p9102_top_sspi_io_ss1_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS1_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_ss2_sel */

int32_t axi_ps2p9102_top_sspi_io_ss2_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_ss2_sel */

int32_t axi_ps2p9102_top_sspi_io_ss2_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS2_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_ss3_sel */

int32_t axi_ps2p9102_top_sspi_io_ss3_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_ss3_sel */

int32_t axi_ps2p9102_top_sspi_io_ss3_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SS3_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_sclk_sel */

int32_t axi_ps2p9102_top_sspi_io_sclk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_sclk_sel */

int32_t axi_ps2p9102_top_sspi_io_sclk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_SCLK_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_mosi_sel */

int32_t axi_ps2p9102_top_sspi_io_mosi_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_mosi_sel */

int32_t axi_ps2p9102_top_sspi_io_mosi_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_MOSI_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_io_miso_sel */

int32_t axi_ps2p9102_top_sspi_io_miso_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_io_miso_sel */

int32_t axi_ps2p9102_top_sspi_io_miso_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_POS, AXI_PS2P9102_TOP_SSPI_IO_MISO_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_swreset */

int32_t axi_ps2p9102_top_sspi_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_swreset */

int32_t axi_ps2p9102_top_sspi_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_enable */

int32_t axi_ps2p9102_top_sspi_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_enable */

int32_t axi_ps2p9102_top_sspi_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_ENABLE_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_sspi_control */

int32_t axi_ps2p9102_top_reg_sspi_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_SSPI_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_sspi_control */

int32_t axi_ps2p9102_top_reg_sspi_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_SSPI_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_bidir_count */

int32_t axi_ps2p9102_top_sspi_bidir_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_POS, AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_bidir_count */

int32_t axi_ps2p9102_top_sspi_bidir_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_POS, AXI_PS2P9102_TOP_SSPI_BIDIR_COUNT_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_bidir_enable */

int32_t axi_ps2p9102_top_sspi_bidir_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_POS, AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_bidir_enable */

int32_t axi_ps2p9102_top_sspi_bidir_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_POS, AXI_PS2P9102_TOP_SSPI_BIDIR_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_miso_edge_sel */

int32_t axi_ps2p9102_top_sspi_miso_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_POS, AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_miso_edge_sel */

int32_t axi_ps2p9102_top_sspi_miso_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_POS, AXI_PS2P9102_TOP_SSPI_MISO_EDGE_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_mosi_edge_sel */

int32_t axi_ps2p9102_top_sspi_mosi_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_POS, AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_sspi_mosi_edge_sel */

int32_t axi_ps2p9102_top_sspi_mosi_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_POS, AXI_PS2P9102_TOP_SSPI_MOSI_EDGE_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_miso_data */

int32_t axi_ps2p9102_top_sspi_miso_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_miso_data */

int32_t axi_ps2p9102_top_sspi_miso_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_mosi_data */

int32_t axi_ps2p9102_top_sspi_mosi_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_mosi_data */

int32_t axi_ps2p9102_top_sspi_mosi_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_DATA_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_sspi_status */

int32_t axi_ps2p9102_top_reg_sspi_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_SSPI_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_sspi_miso_full */

int32_t axi_ps2p9102_top_sspi_miso_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MISO_FULL_POS, AXI_PS2P9102_TOP_SSPI_MISO_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_miso_empty */

int32_t axi_ps2p9102_top_sspi_miso_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MISO_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MISO_EMPTY_POS, AXI_PS2P9102_TOP_SSPI_MISO_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_mosi_full */

int32_t axi_ps2p9102_top_sspi_mosi_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_FULL_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MOSI_FULL_POS, AXI_PS2P9102_TOP_SSPI_MOSI_FULL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_mosi_empty */

int32_t axi_ps2p9102_top_sspi_mosi_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_MOSI_EMPTY_ADDR + addr_offset), AXI_PS2P9102_TOP_SSPI_MOSI_EMPTY_POS, AXI_PS2P9102_TOP_SSPI_MOSI_EMPTY_MASK, data));
}


/*  register field read, axi_ps2p9102_top_sspi_ss */

int32_t axi_ps2p9102_top_sspi_ss_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SS_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_sspi_ss */

int32_t axi_ps2p9102_top_sspi_ss_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_SSPI_SS_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2c_timer */

int32_t axi_ps2p9102_top_i2c_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2c_timer */

int32_t axi_ps2p9102_top_i2c_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_ps2p9102_top_i2c_timer */

int32_t axi_ps2p9102_top_i2c_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2c_scratch */

int32_t axi_ps2p9102_top_i2c_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_ps2p9102_top_i2c_scratch */

int32_t axi_ps2p9102_top_i2c_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_SCRATCH_ADDR + addr_offset), data));
}


/*  register read, axi_ps2p9102_top_reg_i2c_io_control */

int32_t axi_ps2p9102_top_reg_i2c_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2C_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_ps2p9102_top_reg_i2c_io_control */

int32_t axi_ps2p9102_top_reg_i2c_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_PS2P9102_TOP_REG_I2C_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_ps2p9102_top_i2c_io_enable */

int32_t axi_ps2p9102_top_i2c_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_ENABLE_POS, AXI_PS2P9102_TOP_I2C_IO_ENABLE_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2c_io_enable */

int32_t axi_ps2p9102_top_i2c_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_ENABLE_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_ENABLE_POS, AXI_PS2P9102_TOP_I2C_IO_ENABLE_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2c_io_scl_sel */

int32_t axi_ps2p9102_top_i2c_io_scl_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_POS, AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2c_io_scl_sel */

int32_t axi_ps2p9102_top_i2c_io_scl_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_POS, AXI_PS2P9102_TOP_I2C_IO_SCL_SEL_MASK, data));
}


/*  register field read, axi_ps2p9102_top_i2c_io_sda_sel */

int32_t axi_ps2p9102_top_i2c_io_sda_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_POS, AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_MASK, data));
}


/*  register field write, axi_ps2p9102_top_i2c_io_sda_sel */

int32_t axi_ps2p9102_top_i2c_io_sda_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_ADDR + addr_offset), AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_POS, AXI_PS2P9102_TOP_I2C_IO_SDA_SEL_MASK, data));
}

// **********************************************************************************
// **********************************************************************************

