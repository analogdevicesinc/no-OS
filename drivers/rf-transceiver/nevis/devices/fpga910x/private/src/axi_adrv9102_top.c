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
// Description:       AXI AXI_ADRV9102_TOP Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_adrv9102_top_regs.h"
#include "axi_adrv9102_top.h"


/*  register field read, axi_adrv9102_top_version */

int32_t axi_adrv9102_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_instance_id */

int32_t axi_adrv9102_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_INSTANCE_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_scratch */

int32_t axi_adrv9102_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_scratch */

int32_t axi_adrv9102_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_timer */

int32_t axi_adrv9102_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_timer */

int32_t axi_adrv9102_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_adrv9102_top_timer */

int32_t axi_adrv9102_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_ADRV9102_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_resetb */

int32_t axi_adrv9102_top_resetb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_RESETB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_resetb */

int32_t axi_adrv9102_top_resetb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_RESETB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mode */

int32_t axi_adrv9102_top_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mode */

int32_t axi_adrv9102_top_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MODE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_gp_intr */

int32_t axi_adrv9102_top_gp_intr_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_gp_intr */

int32_t axi_adrv9102_top_gp_intr_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_gp_intr_enable */

int32_t axi_adrv9102_top_gp_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_gp_intr_enable */

int32_t axi_adrv9102_top_gp_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_gp_intr_mode */

int32_t axi_adrv9102_top_gp_intr_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_gp_intr_mode */

int32_t axi_adrv9102_top_gp_intr_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_MODE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_usr_gpio_in */

int32_t axi_adrv9102_top_usr_gpio_in_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_USR_GPIO_IN_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_usr_gpio_out */

int32_t axi_adrv9102_top_usr_gpio_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_USR_GPIO_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_usr_gpio_out */

int32_t axi_adrv9102_top_usr_gpio_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_USR_GPIO_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_request */

int32_t axi_adrv9102_top_mcs_request_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_REQUEST_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_request */

int32_t axi_adrv9102_top_mcs_request_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_REQUEST_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_top_reg_mcs_control */

int32_t axi_adrv9102_top_reg_mcs_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_top_reg_mcs_control */

int32_t axi_adrv9102_top_reg_mcs_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_width */

int32_t axi_adrv9102_top_mcs_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_WIDTH_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_WIDTH_POS, AXI_ADRV9102_TOP_MCS_WIDTH_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_width */

int32_t axi_adrv9102_top_mcs_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_WIDTH_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_WIDTH_POS, AXI_ADRV9102_TOP_MCS_WIDTH_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_num */

int32_t axi_adrv9102_top_mcs_num_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_NUM_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_NUM_POS, AXI_ADRV9102_TOP_MCS_NUM_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_num */

int32_t axi_adrv9102_top_mcs_num_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_NUM_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_NUM_POS, AXI_ADRV9102_TOP_MCS_NUM_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_edge_fall1_rise0 */

int32_t axi_adrv9102_top_mcs_edge_fall1_rise0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_POS, AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_edge_fall1_rise0 */

int32_t axi_adrv9102_top_mcs_edge_fall1_rise0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_POS, AXI_ADRV9102_TOP_MCS_EDGE_FALL1_RISE0_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_period */

int32_t axi_adrv9102_top_mcs_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_PERIOD_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_period */

int32_t axi_adrv9102_top_mcs_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_PERIOD_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_select */

int32_t axi_adrv9102_top_mcs_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_SELECT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_select */

int32_t axi_adrv9102_top_mcs_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_SELECT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_trigger */

int32_t axi_adrv9102_top_mcs_trigger_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_trigger */

int32_t axi_adrv9102_top_mcs_trigger_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_trigger_width */

int32_t axi_adrv9102_top_mcs_trigger_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_WIDTH_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_trigger_width */

int32_t axi_adrv9102_top_mcs_trigger_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_WIDTH_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_trigger_mode */

int32_t axi_adrv9102_top_mcs_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_trigger_mode */

int32_t axi_adrv9102_top_mcs_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TRIGGER_MODE_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_top_reg_mcs_tx_resetn */

int32_t axi_adrv9102_top_reg_mcs_tx_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_TX_RESETN_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_top_reg_mcs_tx_resetn */

int32_t axi_adrv9102_top_reg_mcs_tx_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_TX_RESETN_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_tx_clk_resetn */

int32_t axi_adrv9102_top_mcs_tx_clk_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_tx_clk_resetn */

int32_t axi_adrv9102_top_mcs_tx_clk_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_CLK_RESETN_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_tx_phy_resetn */

int32_t axi_adrv9102_top_mcs_tx_phy_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_tx_phy_resetn */

int32_t axi_adrv9102_top_mcs_tx_phy_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_PHY_RESETN_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_tx_dp_resetn */

int32_t axi_adrv9102_top_mcs_tx_dp_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_tx_dp_resetn */

int32_t axi_adrv9102_top_mcs_tx_dp_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_POS, AXI_ADRV9102_TOP_MCS_TX_DP_RESETN_MASK, data));
}


/*  register read, axi_adrv9102_top_reg_mcs_rx_resetn */

int32_t axi_adrv9102_top_reg_mcs_rx_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_RX_RESETN_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_top_reg_mcs_rx_resetn */

int32_t axi_adrv9102_top_reg_mcs_rx_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MCS_RX_RESETN_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_rx_clk_resetn */

int32_t axi_adrv9102_top_mcs_rx_clk_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_rx_clk_resetn */

int32_t axi_adrv9102_top_mcs_rx_clk_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_CLK_RESETN_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_rx_phy_resetn */

int32_t axi_adrv9102_top_mcs_rx_phy_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_rx_phy_resetn */

int32_t axi_adrv9102_top_mcs_rx_phy_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_PHY_RESETN_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_rx_dp_resetn */

int32_t axi_adrv9102_top_mcs_rx_dp_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_MASK, data));
}


/*  register field write, axi_adrv9102_top_mcs_rx_dp_resetn */

int32_t axi_adrv9102_top_mcs_rx_dp_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_ADDR + addr_offset), AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_POS, AXI_ADRV9102_TOP_MCS_RX_DP_RESETN_MASK, data));
}


/*  register field read, axi_adrv9102_top_mcs_clk_mon_count */

int32_t axi_adrv9102_top_mcs_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_adrv9102_top_mcs_clk_mon_count */

int32_t axi_adrv9102_top_mcs_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_cdc_state */

int32_t axi_adrv9102_top_mcs_cdc_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_CDC_STATE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_cdc_state */

int32_t axi_adrv9102_top_mcs_cdc_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_CDC_STATE_ADDR + addr_offset), data));
}


/* clock domain transfer sync axi_adrv9102_top_mcs_cdc_state */

int32_t axi_adrv9102_top_mcs_cdc_state_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_cdc_sync(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_CDC_STATE_ADDR + addr_offset), (AXI_ADRV9102_TOP_MCS_TIMER_ADDR + addr_offset)));
}


/*  register field read, axi_adrv9102_top_mcs_timer */

int32_t axi_adrv9102_top_mcs_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_timer */

int32_t axi_adrv9102_top_mcs_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_adrv9102_top_mcs_timer */

int32_t axi_adrv9102_top_mcs_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mcs_swreset */

int32_t axi_adrv9102_top_mcs_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mcs_swreset */

int32_t axi_adrv9102_top_mcs_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MCS_SWRESET_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_top_reg_mmcm_access */

int32_t axi_adrv9102_top_reg_mmcm_access_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MMCM_ACCESS_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_top_reg_mmcm_access */

int32_t axi_adrv9102_top_reg_mmcm_access_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MMCM_ACCESS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_request */

int32_t axi_adrv9102_top_mmcm_request_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REQUEST_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_REQUEST_POS, AXI_ADRV9102_TOP_MMCM_REQUEST_MASK, data));
}


/*  register field write, axi_adrv9102_top_mmcm_request */

int32_t axi_adrv9102_top_mmcm_request_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REQUEST_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_REQUEST_POS, AXI_ADRV9102_TOP_MMCM_REQUEST_MASK, data));
}


/*  register field read, axi_adrv9102_top_mmcm_rd1_wr0 */

int32_t axi_adrv9102_top_mmcm_rd1_wr0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_RD1_WR0_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_RD1_WR0_POS, AXI_ADRV9102_TOP_MMCM_RD1_WR0_MASK, data));
}


/*  register field write, axi_adrv9102_top_mmcm_rd1_wr0 */

int32_t axi_adrv9102_top_mmcm_rd1_wr0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_RD1_WR0_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_RD1_WR0_POS, AXI_ADRV9102_TOP_MMCM_RD1_WR0_MASK, data));
}


/*  register field read, axi_adrv9102_top_mmcm_addr */

int32_t axi_adrv9102_top_mmcm_addr_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_ADDR_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mmcm_addr */

int32_t axi_adrv9102_top_mmcm_addr_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_ADDR_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_wrdata */

int32_t axi_adrv9102_top_mmcm_wrdata_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_WRDATA_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mmcm_wrdata */

int32_t axi_adrv9102_top_mmcm_wrdata_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_WRDATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_rddata */

int32_t axi_adrv9102_top_mmcm_rddata_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_RDDATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_reset */

int32_t axi_adrv9102_top_mmcm_reset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_RESET_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mmcm_reset */

int32_t axi_adrv9102_top_mmcm_reset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_RESET_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_top_reg_mmcm_status */

int32_t axi_adrv9102_top_reg_mmcm_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_MMCM_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_busy */

int32_t axi_adrv9102_top_mmcm_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_BUSY_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_BUSY_POS, AXI_ADRV9102_TOP_MMCM_BUSY_MASK, data));
}


/*  register field read, axi_adrv9102_top_mmcm_unlocked */

int32_t axi_adrv9102_top_mmcm_unlocked_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_UNLOCKED_ADDR + addr_offset), AXI_ADRV9102_TOP_MMCM_UNLOCKED_POS, AXI_ADRV9102_TOP_MMCM_UNLOCKED_MASK, data));
}


/*  register field read, axi_adrv9102_top_mmcm_ref_clk_sel */

int32_t axi_adrv9102_top_mmcm_ref_clk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REF_CLK_SEL_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_top_mmcm_ref_clk_sel */

int32_t axi_adrv9102_top_mmcm_ref_clk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REF_CLK_SEL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_mmcm_ref_clk_mon_count */

int32_t axi_adrv9102_top_mmcm_ref_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REF_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_adrv9102_top_mmcm_ref_clk_mon_count */

int32_t axi_adrv9102_top_mmcm_ref_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_ADRV9102_TOP_MMCM_REF_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_top_reg_trig_ext_ctl */

int32_t axi_adrv9102_top_reg_trig_ext_ctl_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_TRIG_EXT_CTL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_top_reg_trig_ext_ctl */

int32_t axi_adrv9102_top_reg_trig_ext_ctl_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_TRIG_EXT_CTL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_trig0_ext_oe */

int32_t axi_adrv9102_top_trig0_ext_oe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_TRIG0_EXT_OE_ADDR + addr_offset), AXI_ADRV9102_TOP_TRIG0_EXT_OE_POS, AXI_ADRV9102_TOP_TRIG0_EXT_OE_MASK, data));
}


/*  register field write, axi_adrv9102_top_trig0_ext_oe */

int32_t axi_adrv9102_top_trig0_ext_oe_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_TRIG0_EXT_OE_ADDR + addr_offset), AXI_ADRV9102_TOP_TRIG0_EXT_OE_POS, AXI_ADRV9102_TOP_TRIG0_EXT_OE_MASK, data));
}


/*  register field read, axi_adrv9102_top_trig1_ext_oe */

int32_t axi_adrv9102_top_trig1_ext_oe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_TRIG1_EXT_OE_ADDR + addr_offset), AXI_ADRV9102_TOP_TRIG1_EXT_OE_POS, AXI_ADRV9102_TOP_TRIG1_EXT_OE_MASK, data));
}


/*  register field write, axi_adrv9102_top_trig1_ext_oe */

int32_t axi_adrv9102_top_trig1_ext_oe_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TOP_TRIG1_EXT_OE_ADDR + addr_offset), AXI_ADRV9102_TOP_TRIG1_EXT_OE_POS, AXI_ADRV9102_TOP_TRIG1_EXT_OE_MASK, data));
}


/*  register read, axi_adrv9102_top_reg_intr_status */

int32_t axi_adrv9102_top_reg_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TOP_REG_INTR_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_top_gp_intr_status */

int32_t axi_adrv9102_top_gp_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_GP_INTR_STATUS_ADDR + addr_offset), AXI_ADRV9102_TOP_GP_INTR_STATUS_POS, AXI_ADRV9102_TOP_GP_INTR_STATUS_MASK, data));
}


/*  register field read, axi_adrv9102_top_rx0_intr_status */

int32_t axi_adrv9102_top_rx0_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_RX0_INTR_STATUS_ADDR + addr_offset), AXI_ADRV9102_TOP_RX0_INTR_STATUS_POS, AXI_ADRV9102_TOP_RX0_INTR_STATUS_MASK, data));
}


/*  register field read, axi_adrv9102_top_rx1_intr_status */

int32_t axi_adrv9102_top_rx1_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TOP_RX1_INTR_STATUS_ADDR + addr_offset), AXI_ADRV9102_TOP_RX1_INTR_STATUS_POS, AXI_ADRV9102_TOP_RX1_INTR_STATUS_MASK, data));
}

// **********************************************************************************
// **********************************************************************************

