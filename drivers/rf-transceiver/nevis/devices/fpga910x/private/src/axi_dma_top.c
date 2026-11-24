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
// Description:       AXI AXI_DMA_TOP Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_dma_top_regs.h"
#include "axi_dma_top.h"


/*  register field read, axi_dma_top_version */

int32_t axi_dma_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_instance_id */

int32_t axi_dma_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_INSTANCE_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_scratch */

int32_t axi_dma_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_scratch */

int32_t axi_dma_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_timer */

int32_t axi_dma_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_timer */

int32_t axi_dma_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_dma_top_timer */

int32_t axi_dma_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_DMA_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_aximm_width */

int32_t axi_dma_top_aximm_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_AXIMM_WIDTH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_axis_width */

int32_t axi_dma_top_axis_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_AXIS_WIDTH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_dma_tx1_rx0 */

int32_t axi_dma_top_dma_tx1_rx0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_DMA_TX1_RX0_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_swreset */

int32_t axi_dma_top_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_swreset */

int32_t axi_dma_top_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_SWRESET_ADDR + addr_offset), data));
}


/*  register read, axi_dma_top_reg_control */

int32_t axi_dma_top_reg_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REG_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_dma_top_reg_control */

int32_t axi_dma_top_reg_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_REG_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_start */

int32_t axi_dma_top_start_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR + addr_offset), AXI_DMA_TOP_START_POS, AXI_DMA_TOP_START_MASK, data));
}


/*  register field write, axi_dma_top_start */

int32_t axi_dma_top_start_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR + addr_offset), AXI_DMA_TOP_START_POS, AXI_DMA_TOP_START_MASK, data));
}


/*  register field read, axi_dma_top_stop */

int32_t axi_dma_top_stop_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_STOP_ADDR + addr_offset), AXI_DMA_TOP_STOP_POS, AXI_DMA_TOP_STOP_MASK, data));
}


/*  register field write, axi_dma_top_stop */

int32_t axi_dma_top_stop_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_STOP_ADDR + addr_offset), AXI_DMA_TOP_STOP_POS, AXI_DMA_TOP_STOP_MASK, data));
}


/*  register field read, axi_dma_top_cyclic */

int32_t axi_dma_top_cyclic_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_CYCLIC_ADDR + addr_offset), AXI_DMA_TOP_CYCLIC_POS, AXI_DMA_TOP_CYCLIC_MASK, data));
}


/*  register field write, axi_dma_top_cyclic */

int32_t axi_dma_top_cyclic_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_CYCLIC_ADDR + addr_offset), AXI_DMA_TOP_CYCLIC_POS, AXI_DMA_TOP_CYCLIC_MASK, data));
}


/*  register field read, axi_dma_top_qthreshold */

int32_t axi_dma_top_qthreshold_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_QTHRESHOLD_ADDR + addr_offset), AXI_DMA_TOP_QTHRESHOLD_POS, AXI_DMA_TOP_QTHRESHOLD_MASK, data));
}


/*  register field write, axi_dma_top_qthreshold */

int32_t axi_dma_top_qthreshold_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_QTHRESHOLD_ADDR + addr_offset), AXI_DMA_TOP_QTHRESHOLD_POS, AXI_DMA_TOP_QTHRESHOLD_MASK, data));
}


/*  register field read, axi_dma_top_busy */

int32_t axi_dma_top_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_BUSY_ADDR + addr_offset), data));
}


/*  register read, axi_dma_top_reg_status */

int32_t axi_dma_top_reg_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REG_STATUS_ADDR + addr_offset), data));
}


/*  register write, axi_dma_top_reg_status */

int32_t axi_dma_top_reg_status_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_REG_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_status */

int32_t axi_dma_top_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_STATUS_ADDR + addr_offset), AXI_DMA_TOP_STATUS_POS, AXI_DMA_TOP_STATUS_MASK, data));
}


/*  register field read, axi_dma_top_ovf_unf */

int32_t axi_dma_top_ovf_unf_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_OVF_UNF_ADDR + addr_offset), AXI_DMA_TOP_OVF_UNF_POS, AXI_DMA_TOP_OVF_UNF_MASK, data));
}


/*  register field write, axi_dma_top_ovf_unf */

int32_t axi_dma_top_ovf_unf_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_OVF_UNF_ADDR + addr_offset), AXI_DMA_TOP_OVF_UNF_POS, AXI_DMA_TOP_OVF_UNF_MASK, data));
}


/*  register field read, axi_dma_top_length_error */

int32_t axi_dma_top_length_error_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_LENGTH_ERROR_ADDR + addr_offset), AXI_DMA_TOP_LENGTH_ERROR_POS, AXI_DMA_TOP_LENGTH_ERROR_MASK, data));
}


/*  register field write, axi_dma_top_length_error */

int32_t axi_dma_top_length_error_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_LENGTH_ERROR_ADDR + addr_offset), AXI_DMA_TOP_LENGTH_ERROR_POS, AXI_DMA_TOP_LENGTH_ERROR_MASK, data));
}


/*  register field read, axi_dma_top_tlast_error */

int32_t axi_dma_top_tlast_error_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_TLAST_ERROR_ADDR + addr_offset), AXI_DMA_TOP_TLAST_ERROR_POS, AXI_DMA_TOP_TLAST_ERROR_MASK, data));
}


/*  register field write, axi_dma_top_tlast_error */

int32_t axi_dma_top_tlast_error_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_TLAST_ERROR_ADDR + addr_offset), AXI_DMA_TOP_TLAST_ERROR_POS, AXI_DMA_TOP_TLAST_ERROR_MASK, data));
}


/*  register field read, axi_dma_top_addrdec_error */

int32_t axi_dma_top_addrdec_error_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_ADDRDEC_ERROR_ADDR + addr_offset), AXI_DMA_TOP_ADDRDEC_ERROR_POS, AXI_DMA_TOP_ADDRDEC_ERROR_MASK, data));
}


/*  register field write, axi_dma_top_addrdec_error */

int32_t axi_dma_top_addrdec_error_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_ADDRDEC_ERROR_ADDR + addr_offset), AXI_DMA_TOP_ADDRDEC_ERROR_POS, AXI_DMA_TOP_ADDRDEC_ERROR_MASK, data));
}


/*  register field read, axi_dma_top_slave_error */

int32_t axi_dma_top_slave_error_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_SLAVE_ERROR_ADDR + addr_offset), AXI_DMA_TOP_SLAVE_ERROR_POS, AXI_DMA_TOP_SLAVE_ERROR_MASK, data));
}


/*  register field write, axi_dma_top_slave_error */

int32_t axi_dma_top_slave_error_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_SLAVE_ERROR_ADDR + addr_offset), AXI_DMA_TOP_SLAVE_ERROR_POS, AXI_DMA_TOP_SLAVE_ERROR_MASK, data));
}


/*  register field read, axi_dma_top_req_buf_num */

int32_t axi_dma_top_req_buf_num_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REQ_BUF_NUM_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_ack_buf_num */

int32_t axi_dma_top_ack_buf_num_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_ACK_BUF_NUM_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_start_addr_low */

int32_t axi_dma_top_start_addr_low_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR_LOW_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_start_addr_low */

int32_t axi_dma_top_start_addr_low_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR_LOW_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_start_addr_high */

int32_t axi_dma_top_start_addr_high_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR_HIGH_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_start_addr_high */

int32_t axi_dma_top_start_addr_high_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_START_ADDR_HIGH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_base_addr_low */

int32_t axi_dma_top_base_addr_low_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_BASE_ADDR_LOW_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_base_addr_low */

int32_t axi_dma_top_base_addr_low_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_BASE_ADDR_LOW_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_base_addr_high */

int32_t axi_dma_top_base_addr_high_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_BASE_ADDR_HIGH_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_base_addr_high */

int32_t axi_dma_top_base_addr_high_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_BASE_ADDR_HIGH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_length */

int32_t axi_dma_top_length_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_LENGTH_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_length */

int32_t axi_dma_top_length_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_LENGTH_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_num_of_buffers */

int32_t axi_dma_top_num_of_buffers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_NUM_OF_BUFFERS_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_num_of_buffers */

int32_t axi_dma_top_num_of_buffers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_NUM_OF_BUFFERS_ADDR + addr_offset), data));
}


/*  register read, axi_dma_top_reg_triggers */

int32_t axi_dma_top_reg_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register write, axi_dma_top_reg_triggers */

int32_t axi_dma_top_reg_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_trigger_mode */

int32_t axi_dma_top_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_DMA_TOP_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_TRIGGER_MODE_ADDR + addr_offset), (AXI_DMA_TOP_TRIGGER_MODE_POS + pos_offset), AXI_DMA_TOP_TRIGGER_MODE_MASK, data));
}


/*  register field write, axi_dma_top_trigger_mode */

int32_t axi_dma_top_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_DMA_TOP_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_TRIGGER_MODE_ADDR + addr_offset), (AXI_DMA_TOP_TRIGGER_MODE_POS + pos_offset), AXI_DMA_TOP_TRIGGER_MODE_MASK, data));
}


/*  register read, axi_dma_top_reg_flags */

int32_t axi_dma_top_reg_flags_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REG_FLAGS_ADDR + addr_offset), data));
}


/*  register write, axi_dma_top_reg_flags */

int32_t axi_dma_top_reg_flags_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_REG_FLAGS_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_start_sync */

int32_t axi_dma_top_start_sync_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_START_SYNC_ADDR + addr_offset), AXI_DMA_TOP_START_SYNC_POS, AXI_DMA_TOP_START_SYNC_MASK, data));
}


/*  register field write, axi_dma_top_start_sync */

int32_t axi_dma_top_start_sync_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_START_SYNC_ADDR + addr_offset), AXI_DMA_TOP_START_SYNC_POS, AXI_DMA_TOP_START_SYNC_MASK, data));
}


/*  register field read, axi_dma_top_cyclic_sync */

int32_t axi_dma_top_cyclic_sync_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_CYCLIC_SYNC_ADDR + addr_offset), AXI_DMA_TOP_CYCLIC_SYNC_POS, AXI_DMA_TOP_CYCLIC_SYNC_MASK, data));
}


/*  register field write, axi_dma_top_cyclic_sync */

int32_t axi_dma_top_cyclic_sync_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_CYCLIC_SYNC_ADDR + addr_offset), AXI_DMA_TOP_CYCLIC_SYNC_POS, AXI_DMA_TOP_CYCLIC_SYNC_MASK, data));
}


/*  register field read, axi_dma_top_cdc_state */

int32_t axi_dma_top_cdc_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_CDC_STATE_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_cdc_state */

int32_t axi_dma_top_cdc_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_CDC_STATE_ADDR + addr_offset), data));
}


/* clock domain transfer sync axi_dma_top_cdc_state */

int32_t axi_dma_top_cdc_state_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_cdc_sync(a_device, peripheral_id, (AXI_DMA_TOP_CDC_STATE_ADDR + addr_offset), (AXI_DMA_TOP_TIMER_ADDR + addr_offset)));
}


/*  register field read, axi_dma_top_master_trigger */

int32_t axi_dma_top_master_trigger_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_MASTER_TRIGGER_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_master_trigger */

int32_t axi_dma_top_master_trigger_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_MASTER_TRIGGER_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_mmq_tlimit */

int32_t axi_dma_top_mmq_tlimit_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_MMQ_TLIMIT_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_mmq_tlimit */

int32_t axi_dma_top_mmq_tlimit_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_MMQ_TLIMIT_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_mmq_status */

int32_t axi_dma_top_mmq_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_MMQ_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_mmq_count */

int32_t axi_dma_top_mmq_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_MMQ_COUNT_ADDR + addr_offset), data));
}


/*  register read, axi_dma_top_reg_intr_enable */

int32_t axi_dma_top_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_dma_top_reg_intr_enable */

int32_t axi_dma_top_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_intr_done_enable */

int32_t axi_dma_top_intr_done_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_DONE_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_DONE_ENABLE_POS, AXI_DMA_TOP_INTR_DONE_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_done_enable */

int32_t axi_dma_top_intr_done_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_DONE_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_DONE_ENABLE_POS, AXI_DMA_TOP_INTR_DONE_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_intr_ovf_unf_enable */

int32_t axi_dma_top_intr_ovf_unf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_POS, AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_ovf_unf_enable */

int32_t axi_dma_top_intr_ovf_unf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_POS, AXI_DMA_TOP_INTR_OVF_UNF_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_intr_length_error_enable */

int32_t axi_dma_top_intr_length_error_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_length_error_enable */

int32_t axi_dma_top_intr_length_error_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_LENGTH_ERROR_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_intr_tlast_error_enable */

int32_t axi_dma_top_intr_tlast_error_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_tlast_error_enable */

int32_t axi_dma_top_intr_tlast_error_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_TLAST_ERROR_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_intr_addrdec_error_enable */

int32_t axi_dma_top_intr_addrdec_error_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_addrdec_error_enable */

int32_t axi_dma_top_intr_addrdec_error_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_ADDRDEC_ERROR_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_intr_slave_error_enable */

int32_t axi_dma_top_intr_slave_error_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_MASK, data));
}


/*  register field write, axi_dma_top_intr_slave_error_enable */

int32_t axi_dma_top_intr_slave_error_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_ADDR + addr_offset), AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_POS, AXI_DMA_TOP_INTR_SLAVE_ERROR_ENABLE_MASK, data));
}


/*  register field read, axi_dma_top_clk_mon_count */

int32_t axi_dma_top_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_dma_top_clk_mon_count */

int32_t axi_dma_top_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_DMA_TOP_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_data_xfer_count */

int32_t axi_dma_top_data_xfer_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_DATA_XFER_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_dma_top_intc_cntrl */

int32_t axi_dma_top_intc_cntrl_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_DMA_TOP_INTC_CNTRL_ADDR + addr_offset), data));
}


/*  register field write, axi_dma_top_intc_cntrl */

int32_t axi_dma_top_intc_cntrl_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_DMA_TOP_INTC_CNTRL_ADDR + addr_offset), data));
}

// **********************************************************************************
// **********************************************************************************

