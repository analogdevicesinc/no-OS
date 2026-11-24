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
// Description:       AXI AXI_ADRV9102_GPIO Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_ADRV9102_GPIO_H
#define AXI_ADRV9102_GPIO_H


#include "axi_common.h"
#include "axi_utils.h"
#include "axi_adrv9102_gpio_regs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  register field read, axi_adrv9102_gpio_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief timer delay (us) axi_adrv9102_gpio_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_data_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_data_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_data_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_buf_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_buf_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_buf_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register read, axi_adrv9102_gpio_reg_dgpio_pio_buf_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register read, axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register write, axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx0_pio_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx0_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx0_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_rx0_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx0_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_rx0_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx1_pio_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx1_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx1_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_rx1_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_rx1_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_rx1_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_tx0_pio_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_tx0_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field read, axi_adrv9102_gpio_tx0_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_tx0_pio_data_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_tx0_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_tx0_pio_data_out_enb 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_trig_src_group 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_trig_src_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_trig_src_group 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_trig_src_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_trig_src_pin 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_trig_src_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_trig_src_pin 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_trig_src_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_gainindex_src_group 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_gainindex_src_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_gainindex_src_group 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_gainindex_src_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_gainindex_src_pin 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_gainindex_src_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_adrv9102_gpio_gainindex_src_pin 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_gainindex_src_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_adrv9102_gpio_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


/**
 * \brief count frequency (Hz) axi_adrv9102_gpio_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data);


#ifdef __cplusplus
}
#endif


#endif

// **********************************************************************************
// **********************************************************************************

