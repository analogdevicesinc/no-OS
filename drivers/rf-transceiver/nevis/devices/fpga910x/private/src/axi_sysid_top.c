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
// Description:       AXI AXI_SYSID_TOP Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_sysid_top_regs.h"
#include "axi_sysid_top.h"


/*  register field read, axi_sysid_top_version */

int32_t axi_sysid_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_instance_id */

int32_t axi_sysid_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_INSTANCE_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_scratch */

int32_t axi_sysid_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_scratch */

int32_t axi_sysid_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_SCRATCH_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_timer */

int32_t axi_sysid_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_timer */

int32_t axi_sysid_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_sysid_top_timer */

int32_t axi_sysid_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_SYSID_TOP_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_board_id */

int32_t axi_sysid_top_board_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_BOARD_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_gpio_in */

int32_t axi_sysid_top_gpio_in_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_GPIO_IN_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_gpio_out */

int32_t axi_sysid_top_gpio_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_GPIO_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_gpio_out */

int32_t axi_sysid_top_gpio_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_GPIO_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_status_state */

int32_t axi_sysid_top_status_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_STATUS_STATE_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_control_state */

int32_t axi_sysid_top_control_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_CONTROL_STATE_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_control_state */

int32_t axi_sysid_top_control_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_CONTROL_STATE_ADDR + addr_offset), data));
}


/*  register read, axi_sysid_top_reg_intr_enable */

int32_t axi_sysid_top_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_sysid_top_reg_intr_enable */

int32_t axi_sysid_top_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_intr_gpio_in_enable */

int32_t axi_sysid_top_intr_gpio_in_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_ADDR + addr_offset), AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_POS, AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_MASK, data));
}


/*  register field write, axi_sysid_top_intr_gpio_in_enable */

int32_t axi_sysid_top_intr_gpio_in_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_ADDR + addr_offset), AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_POS, AXI_SYSID_TOP_INTR_GPIO_IN_ENABLE_MASK, data));
}


/*  register field read, axi_sysid_top_intr_status_state_enable */

int32_t axi_sysid_top_intr_status_state_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_ADDR + addr_offset), AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_POS, AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_MASK, data));
}


/*  register field write, axi_sysid_top_intr_status_state_enable */

int32_t axi_sysid_top_intr_status_state_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_ADDR + addr_offset), AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_POS, AXI_SYSID_TOP_INTR_STATUS_STATE_ENABLE_MASK, data));
}


/*  register field read, axi_sysid_top_sys_version */

int32_t axi_sysid_top_sys_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_VERSION_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_params_0 */

int32_t axi_sysid_top_sys_params_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_PARAMS_0_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_params_1 */

int32_t axi_sysid_top_sys_params_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_PARAMS_1_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_build_0 */

int32_t axi_sysid_top_sys_build_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_BUILD_0_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_build_1 */

int32_t axi_sysid_top_sys_build_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_BUILD_1_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_0 */

int32_t axi_sysid_top_sys_info_0_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_0_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_1 */

int32_t axi_sysid_top_sys_info_0_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_1_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_2 */

int32_t axi_sysid_top_sys_info_0_2_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_2_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_3 */

int32_t axi_sysid_top_sys_info_0_3_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_3_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_4 */

int32_t axi_sysid_top_sys_info_0_4_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_4_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_5 */

int32_t axi_sysid_top_sys_info_0_5_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_5_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_6 */

int32_t axi_sysid_top_sys_info_0_6_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_6_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_0_7 */

int32_t axi_sysid_top_sys_info_0_7_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_0_7_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_0 */

int32_t axi_sysid_top_sys_info_1_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_0_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_1 */

int32_t axi_sysid_top_sys_info_1_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_1_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_2 */

int32_t axi_sysid_top_sys_info_1_2_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_2_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_3 */

int32_t axi_sysid_top_sys_info_1_3_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_3_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_4 */

int32_t axi_sysid_top_sys_info_1_4_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_4_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_5 */

int32_t axi_sysid_top_sys_info_1_5_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_5_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_6 */

int32_t axi_sysid_top_sys_info_1_6_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_6_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_info_1_7 */

int32_t axi_sysid_top_sys_info_1_7_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INFO_1_7_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_prescale_count */

int32_t axi_sysid_top_pwm_prescale_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_PRESCALE_COUNT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_prescale_count */

int32_t axi_sysid_top_pwm_prescale_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_PRESCALE_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_enable */

int32_t axi_sysid_top_pwm_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_enable */

int32_t axi_sysid_top_pwm_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_out */

int32_t axi_sysid_top_pwm_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_out */

int32_t axi_sysid_top_pwm_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_delay_count */

int32_t axi_sysid_top_pwm_delay_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_DELAY_COUNT_ADDR_OFFSET);

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_DELAY_COUNT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_delay_count */

int32_t axi_sysid_top_pwm_delay_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_DELAY_COUNT_ADDR_OFFSET);

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_DELAY_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_high_count */

int32_t axi_sysid_top_pwm_high_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_HIGH_COUNT_ADDR_OFFSET);

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_HIGH_COUNT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_high_count */

int32_t axi_sysid_top_pwm_high_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_HIGH_COUNT_ADDR_OFFSET);

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_HIGH_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_pwm_low_count */

int32_t axi_sysid_top_pwm_low_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_LOW_COUNT_ADDR_OFFSET);

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_LOW_COUNT_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_pwm_low_count */

int32_t axi_sysid_top_pwm_low_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_SYSID_TOP_PWM_LOW_COUNT_ADDR_OFFSET);

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_PWM_LOW_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_intr_status */

int32_t axi_sysid_top_sys_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INTR_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_intr_0_status */

int32_t axi_sysid_top_sys_intr_0_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INTR_0_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_sys_intr_0_enable */

int32_t axi_sysid_top_sys_intr_0_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INTR_0_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_sys_intr_0_enable */

int32_t axi_sysid_top_sys_intr_0_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_SYS_INTR_0_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_mem_id */

int32_t axi_sysid_top_mem_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_MEM_ID_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_mem_pg */

int32_t axi_sysid_top_mem_pg_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_MEM_PG_ADDR + addr_offset), data));
}


/*  register field read, axi_sysid_top_mem_pg_sel */

int32_t axi_sysid_top_mem_pg_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_SYSID_TOP_MEM_PG_SEL_ADDR + addr_offset), data));
}


/*  register field write, axi_sysid_top_mem_pg_sel */

int32_t axi_sysid_top_mem_pg_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_SYSID_TOP_MEM_PG_SEL_ADDR + addr_offset), data));
}

// **********************************************************************************
// **********************************************************************************

