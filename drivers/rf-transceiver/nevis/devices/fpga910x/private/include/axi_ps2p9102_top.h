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

#ifndef AXI_PS2P9102_TOP_H
#define AXI_PS2P9102_TOP_H


#include "axi_common.h"
#include "axi_utils.h"
#include "axi_ps2p9102_top_regs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  register field read, axi_ps2p9102_top_version 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_version_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_instance_id 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_instance_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_i2s_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_i2s_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2s_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_i2s_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2s_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_io_sclk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_sclk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_io_sclk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_sclk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_io_ws_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_ws_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_io_ws_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_ws_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_io_msdo_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_msdo_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_io_msdo_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_msdo_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_io_msdi_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_msdi_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_io_msdi_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_io_msdi_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_i2s_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2s_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_i2s_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2s_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_master 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_master_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_master 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_master_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_no_of_bytes 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_no_of_bytes_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_no_of_bytes 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_no_of_bytes_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2s_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_i2s_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2s_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_tx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_tx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_rx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2s_rx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2s_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_qspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_qspi_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_qspi_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_io_sd3_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_sd3_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_io_sd3_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_sd3_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_io_ss1_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_ss1_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_io_ss1_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_io_ss1_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_ipsel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_ipsel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_ipsel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_ipsel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_qspi_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_qspi_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_master 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_master_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_master 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_master_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_tx_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_tx_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_rx_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_rx_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_qspi_counts 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_counts_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_qspi_counts 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_counts_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_bidir_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_bidir_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_bidir_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_bidir_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_tx_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_tx_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_rx_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_rx_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_qspi_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_qspi_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_tx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_tx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_rx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_rx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_ss 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_ss_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_qspi_ss_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_ss_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_qspi_ss_out 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_qspi_ss_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_uart_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_uart_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_uart_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_io_tx_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_tx_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_io_tx_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_tx_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_io_rx_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_rx_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_io_rx_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_io_rx_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_uart_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_uart_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_bit_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_bit_width_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_bit_width 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_bit_width_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_stop_bits 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_stop_bits_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_stop_bits 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_stop_bits_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_parity_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_parity_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_parity_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_parity_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_parity_type 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_parity_type_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_parity_type 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_parity_type_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rdy_polarity 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rdy_polarity_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rdy_polarity 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rdy_polarity_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rdy_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rdy_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rdy_mode 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rdy_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_loopback 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_loopback_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_loopback 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_loopback_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_clk_period_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_clk_period 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_clk_period_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_tx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_tx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_tx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_tx_rdy 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_tx_rdy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rx_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_rdy 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_rdy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rx_rdy 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_rdy_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_uart_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_tx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_tx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_tx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_tx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_uart_rx_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_rx_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_uart_rx_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_uart_rx_status_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_perror 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_perror_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rx_perror 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_perror_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_rx_ovf 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_ovf_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_uart_rx_ovf 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_rx_ovf_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_uart_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief count frequency (Hz) axi_ps2p9102_top_uart_clk_mon_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_uart_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_sspi_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_sspi_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_sspi_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_sspi_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_sspi_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_ss0_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss0_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_ss0_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss0_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_ss1_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss1_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_ss1_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss1_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_ss2_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss2_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_ss2_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss2_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_ss3_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss3_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_ss3_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_ss3_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_sclk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_sclk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_sclk_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_sclk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_mosi_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_mosi_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_mosi_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_mosi_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_io_miso_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_miso_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_io_miso_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_io_miso_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_swreset_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_swreset 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_swreset_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_sspi_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_sspi_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_sspi_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_sspi_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_bidir_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_bidir_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_bidir_count 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_bidir_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_bidir_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_bidir_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_bidir_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_bidir_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_miso_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_miso_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_mosi_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_edge_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_mosi_edge_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_edge_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_miso_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_miso_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_mosi_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_mosi_data 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_sspi_status 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_sspi_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_miso_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_miso_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_miso_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_mosi_full 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_mosi_empty 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_mosi_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field read, axi_ps2p9102_top_sspi_ss 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_ss_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_sspi_ss 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_sspi_ss_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2c_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2c_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief timer delay (us) axi_ps2p9102_top_i2c_timer 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2c_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_scratch_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2c_scratch 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_scratch_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register read, axi_ps2p9102_top_reg_i2c_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2c_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register write, axi_ps2p9102_top_reg_i2c_io_control 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_reg_i2c_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2c_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2c_io_enable 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2c_io_scl_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_scl_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2c_io_scl_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_scl_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


/**
 * \brief  register field read, axi_ps2p9102_top_i2c_io_sda_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_sda_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *data);


/**
 * \brief  register field write, axi_ps2p9102_top_i2c_io_sda_sel 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_top_i2c_io_sda_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t data);


#ifdef __cplusplus
}
#endif


#endif

// **********************************************************************************
// **********************************************************************************

