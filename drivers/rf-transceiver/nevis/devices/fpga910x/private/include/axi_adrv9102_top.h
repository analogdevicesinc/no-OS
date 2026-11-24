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

#ifndef AXI_ADRV9102_TOP_H
#define AXI_ADRV9102_TOP_H


#include "axi_common.h"
#include "axi_utils.h"
#include "axi_adrv9102_top_regs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  register field read, axi_adrv9102_top_version 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_instance_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief timer delay (us) axi_adrv9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_resetb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_resetb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_resetb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_resetb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_gp_intr 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_gp_intr 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_gp_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_gp_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_gp_intr_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_gp_intr_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_usr_gpio_in 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_usr_gpio_in_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_usr_gpio_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_usr_gpio_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_usr_gpio_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_usr_gpio_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_request 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_request_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_request 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_request_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_mcs_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_top_reg_mcs_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_num 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_num_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_num 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_num_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_edge_fall1_rise0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_edge_fall1_rise0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_edge_fall1_rise0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_edge_fall1_rise0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_select 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_select 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_trigger 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_trigger 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_trigger_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_trigger_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_mcs_tx_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_tx_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_top_reg_mcs_tx_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_tx_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_tx_clk_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_clk_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_tx_clk_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_clk_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_tx_phy_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_phy_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_tx_phy_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_phy_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_tx_dp_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_dp_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_tx_dp_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_tx_dp_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_mcs_rx_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_rx_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_top_reg_mcs_rx_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mcs_rx_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_rx_clk_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_clk_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_rx_clk_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_clk_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_rx_phy_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_phy_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_rx_phy_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_phy_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_rx_dp_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_dp_resetn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_rx_dp_resetn 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_rx_dp_resetn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief count frequency (Hz) axi_adrv9102_top_mcs_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_cdc_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_cdc_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_cdc_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_cdc_state_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief clock domain transfer sync axi_adrv9102_top_mcs_cdc_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_cdc_state_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief timer delay (us) axi_adrv9102_top_mcs_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mcs_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mcs_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mcs_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_mmcm_access 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mmcm_access_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_top_reg_mmcm_access 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mmcm_access_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_request 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_request_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_request 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_request_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_rd1_wr0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_rd1_wr0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_rd1_wr0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_rd1_wr0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_addr 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_addr_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_addr 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_addr_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_wrdata 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_wrdata_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_wrdata 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_wrdata_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_rddata 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_rddata_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_reset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_reset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_reset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_reset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_mmcm_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_mmcm_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_busy 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_unlocked 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_unlocked_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_ref_clk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_ref_clk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_mmcm_ref_clk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_ref_clk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_mmcm_ref_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_ref_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief count frequency (Hz) axi_adrv9102_top_mmcm_ref_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_mmcm_ref_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register read, axi_adrv9102_top_reg_trig_ext_ctl 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_trig_ext_ctl_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_top_reg_trig_ext_ctl 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_trig_ext_ctl_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_trig0_ext_oe 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_trig0_ext_oe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_trig0_ext_oe 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_trig0_ext_oe_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_top_trig1_ext_oe 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_trig1_ext_oe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_top_trig1_ext_oe 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_trig1_ext_oe_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_top_reg_intr_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_reg_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_gp_intr_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_gp_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_rx0_intr_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_rx0_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_top_rx1_intr_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_top_rx1_intr_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


#ifdef __cplusplus
}
#endif


#endif

// **********************************************************************************
// **********************************************************************************

