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

#ifndef AXI_MSPI_TOP_H
#define AXI_MSPI_TOP_H


#include "axi_common.h"
#include "axi_utils.h"
#include "axi_mspi_top_regs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  register field read, axi_mspi_top_version 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_instance_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_mspi_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_slave_select_n 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_slave_select_n_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_slave_select_n 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_slave_select_n_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_mspi_top_reg_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_mspi_top_reg_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_auto 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_auto_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_auto 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_auto_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_busy 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_num_of_bytes 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_num_of_bytes_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_num_of_bytes 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_num_of_bytes_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_num_of_frames 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_num_of_frames_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_num_of_frames 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_num_of_frames_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_sclk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_sclk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_sclk_init 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_init_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_sclk_init 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_init_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_mosi_bidir_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_bidir_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_mosi_bidir_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_bidir_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_mosi_bidir_offset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_bidir_offset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_mosi_bidir_offset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_bidir_offset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_miso_fall1_rise0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_fall1_rise0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_miso_fall1_rise0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_fall1_rise0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_miso_select 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_miso_select 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_miso_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_miso_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_mspi_top_reg_triggers 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_mspi_top_reg_triggers 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_trigger_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t index_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_dma1_fifo0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_dma1_fifo0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_dma1_fifo0 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_dma1_fifo0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_mosi_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_mosi_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_miso_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_miso_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_mspi_top_reg_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_mosi_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_mosi_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_miso_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_miso_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_miso_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_byte_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_byte_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_ssn_state 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_ssn_state_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_ssn_sclk_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_ssn_sclk_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_mspi_top_dma_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_dma_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register read, axi_mspi_top_reg_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_mspi_top_reg_intr_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_mosi_full_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_mosi_full_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_mosi_full_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_mosi_full_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_mosi_empty_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_mosi_empty_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_mosi_empty_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_mosi_empty_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_miso_full_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_miso_full_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_miso_full_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_miso_full_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_miso_empty_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_miso_empty_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_miso_empty_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_miso_empty_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_dma_done_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_dma_done_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_dma_done_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_dma_done_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_intr_xfer_done_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_xfer_done_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_intr_xfer_done_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_intr_xfer_done_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_ssn_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_ssn_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_ssn_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_ssn_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_sclk_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_sclk_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_sclk_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_mspi_top_mosi_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_disable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_mspi_top_mosi_disable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_top_mosi_disable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


#ifdef __cplusplus
}
#endif


#endif

// **********************************************************************************
// **********************************************************************************

