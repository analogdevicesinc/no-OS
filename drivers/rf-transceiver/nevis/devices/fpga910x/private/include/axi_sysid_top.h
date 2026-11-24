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

#ifndef AXI_SYSID_TOP_H
#define AXI_SYSID_TOP_H


#include "axi_common.h"
#include "axi_utils.h"
#include "axi_sysid_top_regs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  register field read, axi_sysid_top_version 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_instance_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_sysid_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_board_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_board_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_gpio_in 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_gpio_in_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_gpio_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_gpio_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_gpio_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_gpio_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_status_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_status_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_control_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_control_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_control_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_control_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_sysid_top_reg_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_sysid_top_reg_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_intr_gpio_in_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_intr_gpio_in_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_intr_gpio_in_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_intr_gpio_in_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_intr_status_state_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_intr_status_state_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_intr_status_state_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_intr_status_state_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_sys_version 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_params_0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_params_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_params_1 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_params_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_build_0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_build_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_build_1 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_build_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_1 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_2 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_2_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_3 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_3_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_4 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_4_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_5 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_5_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_6 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_6_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_0_7 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_0_7_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_1 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_2 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_2_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_3 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_3_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_4 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_4_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_5 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_5_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_6 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_6_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_info_1_7 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_info_1_7_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_pwm_prescale_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_prescale_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_prescale_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_prescale_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_pwm_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_pwm_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_pwm_delay_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_delay_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_delay_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_delay_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_pwm_high_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_high_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_high_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_high_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_pwm_low_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_low_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_pwm_low_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_pwm_low_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_sys_intr_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_intr_0_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_intr_0_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_sys_intr_0_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_intr_0_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_sys_intr_0_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_sys_intr_0_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_sysid_top_mem_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_mem_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_mem_pg 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_mem_pg_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_sysid_top_mem_pg_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_mem_pg_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_sysid_top_mem_pg_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_top_mem_pg_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


#ifdef __cplusplus
}
#endif


#endif

// **********************************************************************************
// **********************************************************************************

