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
// Description:       AXI AXI_ADRV9102_TDD Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_adrv9102_tdd_regs.h"
#include "axi_adrv9102_tdd.h"


/*  register field read, axi_adrv9102_tdd_frame_period */

int32_t axi_adrv9102_tdd_frame_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_PERIOD_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_frame_period */

int32_t axi_adrv9102_tdd_frame_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_PERIOD_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_num_of_frames */

int32_t axi_adrv9102_tdd_num_of_frames_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_NUM_OF_FRAMES_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_num_of_frames */

int32_t axi_adrv9102_tdd_num_of_frames_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_NUM_OF_FRAMES_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_frame_switch_period */

int32_t axi_adrv9102_tdd_frame_switch_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_PERIOD_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_frame_switch_period */

int32_t axi_adrv9102_tdd_frame_switch_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_PERIOD_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_frame_switch_number */

int32_t axi_adrv9102_tdd_frame_switch_number_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_NUMBER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_frame_switch_number */

int32_t axi_adrv9102_tdd_frame_switch_number_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_NUMBER_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_tdd_reg_triggers */

int32_t axi_adrv9102_tdd_reg_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_tdd_reg_triggers */

int32_t axi_adrv9102_tdd_reg_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_REG_TRIGGERS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_trigger_mode */

int32_t axi_adrv9102_tdd_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_ADRV9102_TDD_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TDD_TRIGGER_MODE_ADDR + addr_offset), (AXI_ADRV9102_TDD_TRIGGER_MODE_POS + pos_offset), AXI_ADRV9102_TDD_TRIGGER_MODE_MASK, data));
}


/*  register field write, axi_adrv9102_tdd_trigger_mode */

int32_t axi_adrv9102_tdd_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_ADRV9102_TDD_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TDD_TRIGGER_MODE_ADDR + addr_offset), (AXI_ADRV9102_TDD_TRIGGER_MODE_POS + pos_offset), AXI_ADRV9102_TDD_TRIGGER_MODE_MASK, data));
}


/*  register read, axi_adrv9102_tdd_reg_sequence_control */

int32_t axi_adrv9102_tdd_reg_sequence_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_REG_SEQUENCE_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_tdd_reg_sequence_control */

int32_t axi_adrv9102_tdd_reg_sequence_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_REG_SEQUENCE_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_start */

int32_t axi_adrv9102_tdd_start_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TDD_START_ADDR + addr_offset), AXI_ADRV9102_TDD_START_POS, AXI_ADRV9102_TDD_START_MASK, data));
}


/*  register field write, axi_adrv9102_tdd_start */

int32_t axi_adrv9102_tdd_start_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TDD_START_ADDR + addr_offset), AXI_ADRV9102_TDD_START_POS, AXI_ADRV9102_TDD_START_MASK, data));
}


/*  register field read, axi_adrv9102_tdd_stop */

int32_t axi_adrv9102_tdd_stop_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TDD_STOP_ADDR + addr_offset), AXI_ADRV9102_TDD_STOP_POS, AXI_ADRV9102_TDD_STOP_MASK, data));
}


/*  register field write, axi_adrv9102_tdd_stop */

int32_t axi_adrv9102_tdd_stop_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TDD_STOP_ADDR + addr_offset), AXI_ADRV9102_TDD_STOP_POS, AXI_ADRV9102_TDD_STOP_MASK, data));
}


/*  register field read, axi_adrv9102_tdd_sequence_repeat */

int32_t axi_adrv9102_tdd_sequence_repeat_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TDD_SEQUENCE_REPEAT_ADDR + addr_offset), AXI_ADRV9102_TDD_SEQUENCE_REPEAT_POS, AXI_ADRV9102_TDD_SEQUENCE_REPEAT_MASK, data));
}


/*  register field write, axi_adrv9102_tdd_sequence_repeat */

int32_t axi_adrv9102_tdd_sequence_repeat_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TDD_SEQUENCE_REPEAT_ADDR + addr_offset), AXI_ADRV9102_TDD_SEQUENCE_REPEAT_POS, AXI_ADRV9102_TDD_SEQUENCE_REPEAT_MASK, data));
}


/*  register field read, axi_adrv9102_tdd_frame_switch_enable */

int32_t axi_adrv9102_tdd_frame_switch_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_ADDR + addr_offset), AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_POS, AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_tdd_frame_switch_enable */

int32_t axi_adrv9102_tdd_frame_switch_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_ADDR + addr_offset), AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_POS, AXI_ADRV9102_TDD_FRAME_SWITCH_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_tdd_swreset */

int32_t axi_adrv9102_tdd_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SWRESET_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_swreset */

int32_t axi_adrv9102_tdd_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SWRESET_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_timer */

int32_t axi_adrv9102_tdd_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_timer */

int32_t axi_adrv9102_tdd_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_adrv9102_tdd_timer */

int32_t axi_adrv9102_tdd_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_ADRV9102_TDD_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_clk_mon_count */

int32_t axi_adrv9102_tdd_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_adrv9102_tdd_clk_mon_count */

int32_t axi_adrv9102_tdd_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_ADRV9102_TDD_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_global_enb */

int32_t axi_adrv9102_tdd_global_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_GLOBAL_ENB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_global_enb */

int32_t axi_adrv9102_tdd_global_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_GLOBAL_ENB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_cdc_state */

int32_t axi_adrv9102_tdd_cdc_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_CDC_STATE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_cdc_state */

int32_t axi_adrv9102_tdd_cdc_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_CDC_STATE_ADDR + addr_offset), data));
}


/* clock domain transfer sync axi_adrv9102_tdd_cdc_state */

int32_t axi_adrv9102_tdd_cdc_state_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_cdc_sync(a_device, peripheral_id, (AXI_ADRV9102_TDD_CDC_STATE_ADDR + addr_offset), (AXI_ADRV9102_TDD_TIMER_ADDR + addr_offset)));
}


/*  register field read, axi_adrv9102_tdd_primary_assert */

int32_t axi_adrv9102_tdd_primary_assert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_ASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_primary_assert */

int32_t axi_adrv9102_tdd_primary_assert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_ASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_primary_deassert */

int32_t axi_adrv9102_tdd_primary_deassert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_DEASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_primary_deassert */

int32_t axi_adrv9102_tdd_primary_deassert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_DEASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_secondary_assert */

int32_t axi_adrv9102_tdd_secondary_assert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_ASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_secondary_assert */

int32_t axi_adrv9102_tdd_secondary_assert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_ASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_secondary_deassert */

int32_t axi_adrv9102_tdd_secondary_deassert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_DEASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_secondary_deassert */

int32_t axi_adrv9102_tdd_secondary_deassert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_DEASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_primary_frame_assert */

int32_t axi_adrv9102_tdd_primary_frame_assert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_FRAME_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_FRAME_ASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_primary_frame_assert */

int32_t axi_adrv9102_tdd_primary_frame_assert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_FRAME_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_FRAME_ASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_primary_frame_deassert */

int32_t axi_adrv9102_tdd_primary_frame_deassert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_FRAME_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_FRAME_DEASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_primary_frame_deassert */

int32_t axi_adrv9102_tdd_primary_frame_deassert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_PRIMARY_FRAME_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_PRIMARY_FRAME_DEASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_secondary_frame_assert */

int32_t axi_adrv9102_tdd_secondary_frame_assert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_FRAME_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_FRAME_ASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_secondary_frame_assert */

int32_t axi_adrv9102_tdd_secondary_frame_assert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_FRAME_ASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_FRAME_ASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_secondary_frame_deassert */

int32_t axi_adrv9102_tdd_secondary_frame_deassert_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_FRAME_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_FRAME_DEASSERT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_secondary_frame_deassert */

int32_t axi_adrv9102_tdd_secondary_frame_deassert_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SECONDARY_FRAME_DEASSERT_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SECONDARY_FRAME_DEASSERT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_mode */

int32_t axi_adrv9102_tdd_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_MODE_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_mode */

int32_t axi_adrv9102_tdd_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_MODE_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_MODE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_sync */

int32_t axi_adrv9102_tdd_sync_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SYNC_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_SYNC_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_tdd_sync */

int32_t axi_adrv9102_tdd_sync_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_SYNC_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_TDD_SYNC_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_tdd_clk_ratio */

int32_t axi_adrv9102_tdd_clk_ratio_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_TDD_CLK_RATIO_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_TDD_CLK_RATIO_ADDR + addr_offset), data));
}

// **********************************************************************************
// **********************************************************************************

