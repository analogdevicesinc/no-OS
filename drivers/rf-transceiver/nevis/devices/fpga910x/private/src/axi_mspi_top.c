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
// Description:       AXI AXI_MSPI_TOP Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_mspi_top_regs.h"
#include "axi_mspi_top.h"


/*  register field read, axi_mspi_top_version */

int32_t axi_mspi_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_instance_id */

int32_t axi_mspi_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_INSTANCE_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_scratch */

int32_t axi_mspi_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_scratch */

int32_t axi_mspi_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_timer */

int32_t axi_mspi_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_timer */

int32_t axi_mspi_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_mspi_top_timer */

int32_t axi_mspi_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_MSPI_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_slave_select_n */

int32_t axi_mspi_top_slave_select_n_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SLAVE_SELECT_N_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_slave_select_n */

int32_t axi_mspi_top_slave_select_n_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SLAVE_SELECT_N_ADDR + addr_offset), data));
}


/*  register read, axi_mspi_top_reg_enable */

int32_t axi_mspi_top_reg_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_REG_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_mspi_top_reg_enable */

int32_t axi_mspi_top_reg_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_REG_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_enable */

int32_t axi_mspi_top_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_ENABLE_POS, AXI_MSPI_TOP_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_enable */

int32_t axi_mspi_top_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_ENABLE_POS, AXI_MSPI_TOP_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_auto */

int32_t axi_mspi_top_auto_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_AUTO_ADDR + addr_offset), AXI_MSPI_TOP_AUTO_POS, AXI_MSPI_TOP_AUTO_MASK, data));
}


/*  register field write, axi_mspi_top_auto */

int32_t axi_mspi_top_auto_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_AUTO_ADDR + addr_offset), AXI_MSPI_TOP_AUTO_POS, AXI_MSPI_TOP_AUTO_MASK, data));
}


/*  register field read, axi_mspi_top_busy */

int32_t axi_mspi_top_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_BUSY_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_num_of_bytes */

int32_t axi_mspi_top_num_of_bytes_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_NUM_OF_BYTES_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_num_of_bytes */

int32_t axi_mspi_top_num_of_bytes_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_NUM_OF_BYTES_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_num_of_frames */

int32_t axi_mspi_top_num_of_frames_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_NUM_OF_FRAMES_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_num_of_frames */

int32_t axi_mspi_top_num_of_frames_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_NUM_OF_FRAMES_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_sclk_period */

int32_t axi_mspi_top_sclk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_PERIOD_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_sclk_period */

int32_t axi_mspi_top_sclk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_PERIOD_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_sclk_init */

int32_t axi_mspi_top_sclk_init_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_INIT_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_sclk_init */

int32_t axi_mspi_top_sclk_init_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_INIT_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_mosi_bidir_mode */

int32_t axi_mspi_top_mosi_bidir_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_BIDIR_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_mosi_bidir_mode */

int32_t axi_mspi_top_mosi_bidir_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_BIDIR_MODE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_mosi_bidir_offset */

int32_t axi_mspi_top_mosi_bidir_offset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_BIDIR_OFFSET_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_mosi_bidir_offset */

int32_t axi_mspi_top_mosi_bidir_offset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_BIDIR_OFFSET_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_miso_fall1_rise0 */

int32_t axi_mspi_top_miso_fall1_rise0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_FALL1_RISE0_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_miso_fall1_rise0 */

int32_t axi_mspi_top_miso_fall1_rise0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_FALL1_RISE0_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_miso_select */

int32_t axi_mspi_top_miso_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_SELECT_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_miso_select */

int32_t axi_mspi_top_miso_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_SELECT_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_miso_enable */

int32_t axi_mspi_top_miso_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_miso_enable */

int32_t axi_mspi_top_miso_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_ENABLE_ADDR + addr_offset), data));
}


/*  register read, axi_mspi_top_reg_triggers */

int32_t axi_mspi_top_reg_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register write, axi_mspi_top_reg_triggers */

int32_t axi_mspi_top_reg_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_trigger_mode */

int32_t axi_mspi_top_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;

    pos_offset = (index_id * AXI_MSPI_TOP_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_TRIGGER_MODE_ADDR + addr_offset), (AXI_MSPI_TOP_TRIGGER_MODE_POS + pos_offset), AXI_MSPI_TOP_TRIGGER_MODE_MASK, data));
}


/*  register field write, axi_mspi_top_trigger_mode */

int32_t axi_mspi_top_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;

    pos_offset = (index_id * AXI_MSPI_TOP_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_TRIGGER_MODE_ADDR + addr_offset), (AXI_MSPI_TOP_TRIGGER_MODE_POS + pos_offset), AXI_MSPI_TOP_TRIGGER_MODE_MASK, data));
}


/*  register field read, axi_mspi_top_dma1_fifo0 */

int32_t axi_mspi_top_dma1_fifo0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_DMA1_FIFO0_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_dma1_fifo0 */

int32_t axi_mspi_top_dma1_fifo0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_DMA1_FIFO0_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_swreset */

int32_t axi_mspi_top_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_swreset */

int32_t axi_mspi_top_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_mosi_data */

int32_t axi_mspi_top_mosi_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_mosi_data */

int32_t axi_mspi_top_mosi_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_miso_data */

int32_t axi_mspi_top_miso_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_miso_data */

int32_t axi_mspi_top_miso_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_DATA_ADDR + addr_offset), data));
}


/*  register read, axi_mspi_top_reg_status */

int32_t axi_mspi_top_reg_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_REG_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_mosi_full */

int32_t axi_mspi_top_mosi_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_FULL_ADDR + addr_offset), AXI_MSPI_TOP_MOSI_FULL_POS, AXI_MSPI_TOP_MOSI_FULL_MASK, data));
}


/*  register field read, axi_mspi_top_mosi_empty */

int32_t axi_mspi_top_mosi_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_EMPTY_ADDR + addr_offset), AXI_MSPI_TOP_MOSI_EMPTY_POS, AXI_MSPI_TOP_MOSI_EMPTY_MASK, data));
}


/*  register field read, axi_mspi_top_miso_full */

int32_t axi_mspi_top_miso_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_FULL_ADDR + addr_offset), AXI_MSPI_TOP_MISO_FULL_POS, AXI_MSPI_TOP_MISO_FULL_MASK, data));
}


/*  register field read, axi_mspi_top_miso_empty */

int32_t axi_mspi_top_miso_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_MISO_EMPTY_ADDR + addr_offset), AXI_MSPI_TOP_MISO_EMPTY_POS, AXI_MSPI_TOP_MISO_EMPTY_MASK, data));
}


/*  register field read, axi_mspi_top_byte_count */

int32_t axi_mspi_top_byte_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_BYTE_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_ssn_state */

int32_t axi_mspi_top_ssn_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SSN_STATE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_ssn_sclk_count */

int32_t axi_mspi_top_ssn_sclk_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SSN_SCLK_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_dma_status */

int32_t axi_mspi_top_dma_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_DMA_STATUS_ADDR + addr_offset), data));
}


/*  register read, axi_mspi_top_reg_intr_enable */

int32_t axi_mspi_top_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_mspi_top_reg_intr_enable */

int32_t axi_mspi_top_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_intr_mosi_full_enable */

int32_t axi_mspi_top_intr_mosi_full_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_POS, AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_mosi_full_enable */

int32_t axi_mspi_top_intr_mosi_full_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_POS, AXI_MSPI_TOP_INTR_MOSI_FULL_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_intr_mosi_empty_enable */

int32_t axi_mspi_top_intr_mosi_empty_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_POS, AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_mosi_empty_enable */

int32_t axi_mspi_top_intr_mosi_empty_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_POS, AXI_MSPI_TOP_INTR_MOSI_EMPTY_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_intr_miso_full_enable */

int32_t axi_mspi_top_intr_miso_full_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_POS, AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_miso_full_enable */

int32_t axi_mspi_top_intr_miso_full_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_POS, AXI_MSPI_TOP_INTR_MISO_FULL_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_intr_miso_empty_enable */

int32_t axi_mspi_top_intr_miso_empty_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_POS, AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_miso_empty_enable */

int32_t axi_mspi_top_intr_miso_empty_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_POS, AXI_MSPI_TOP_INTR_MISO_EMPTY_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_intr_dma_done_enable */

int32_t axi_mspi_top_intr_dma_done_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_POS, AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_dma_done_enable */

int32_t axi_mspi_top_intr_dma_done_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_POS, AXI_MSPI_TOP_INTR_DMA_DONE_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_intr_xfer_done_enable */

int32_t axi_mspi_top_intr_xfer_done_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_POS, AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_MASK, data));
}


/*  register field write, axi_mspi_top_intr_xfer_done_enable */

int32_t axi_mspi_top_intr_xfer_done_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_ADDR + addr_offset), AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_POS, AXI_MSPI_TOP_INTR_XFER_DONE_ENABLE_MASK, data));
}


/*  register field read, axi_mspi_top_ssn_disable */

int32_t axi_mspi_top_ssn_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SSN_DISABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_ssn_disable */

int32_t axi_mspi_top_ssn_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SSN_DISABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_sclk_disable */

int32_t axi_mspi_top_sclk_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_DISABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_sclk_disable */

int32_t axi_mspi_top_sclk_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_SCLK_DISABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_mspi_top_mosi_disable */

int32_t axi_mspi_top_mosi_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_DISABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_mspi_top_mosi_disable */

int32_t axi_mspi_top_mosi_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_MSPI_TOP_MOSI_DISABLE_ADDR + addr_offset), data));
}

// **********************************************************************************
// **********************************************************************************

