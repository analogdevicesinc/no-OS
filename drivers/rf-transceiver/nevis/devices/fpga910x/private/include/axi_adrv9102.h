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
// Description:       AXI ADRV9102 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_ADRV9102_H
#define AXI_ADRV9102_H

#include "axi_common.h"
#include "axi_adrv9102_top.h"
#include "axi_adrv9102_ssi.h"
#include "axi_adrv9102_tdd.h"
#include "axi_adrv9102_gpio.h"
#include "axi_adrv9102_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief mcs configuration, a valid configuration is required before start request.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] param desired configuration parameters
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mcs_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id, axi_adrv9102_mcs_params_t *mcs_param);

/**
 * \brief mcs start request
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mcs_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id);

/**
 * \brief mcs status querry
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] status of current mcs sequence
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mcs_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *status);

/**
 * \brief mcs trigger request for all internal synchronization
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mcs_trigger(AXI_DEVICE_T *a_device, uint32_t peripheral_id);

/**
 * \brief mcs synchronization report on receive interface
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] mcs_rx_info reporting
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mcs_rx_info_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, axi_adrv9102_mcs_rx_info_t *mcs_rx_info);

// **********************************************************************************
// **********************************************************************************

/**
 * \brief mmcm busy (indicates pending keyhole access)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] status pending indirect register request
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_busy(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t *status);

/**
 * \brief mmcm write (indirect access)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr of mmcm register (vendor specific)
 * \param[in] data of mmcm register (vendor specific)
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t data);

/**
 * \brief mmcm read (indirect access)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr of mmcm register (vendor specific)
 * \param[out] data of mmcm register (vendor specific)
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_read(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t *data);

/**
 * \brief mmcm read-modify-write == (read & mask) | write (indirect access)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr of mmcm register (vendor specific)
 * \param[in] mask of mmcm register  (vendor specific)
 * \param[in] data of mmcm register (vendor specific)
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_read_modify_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t mask, uint32_t data);

/**
 * \brief mmcm set clock divider (addressed by clock indices)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr of mmcm register (vendor specific)
 * \param[in] div of mmcm register  (vendor specific)
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_set_div(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t div);

/**
 * \brief mmcm set lock parameters.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ref_mul the vco reference-clock ratio
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_set_lock(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ref_mul);

/**
 * \brief mmcm set filter parameters.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ref_mul the vco reference-clock ratio
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_mmcm_set_filter(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ref_mul);

/**
 * \brief mmcm configuration, all inclusive (it resets the mmcm, sets the dividers,
 *  filters and other parameters, and checks if the mmcm is locked and outputs are active.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in,out] param desired parameters of mmcm/pll clocks and actual values
 * \param[out] locked the status of mmcm/pll
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_clk_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_clk_params_t *clk_param, uint32_t *locked);

// **********************************************************************************
// **********************************************************************************

/**
 * \brief gpio pin data (physical I/O pin value)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \param[out] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data);

/**
 * \brief gpio pin register set (out data, set direction to drive I/O)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t data);

/**
 * \brief gpio pin register get (out data, set direction to drive I/O)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \param[out] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data);

/**
 * \brief gpio pin direction set
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \param[in] data desired direction
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_direction_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t data);

/**
 * \brief gpio pin direction get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \param[out] data current direction
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_direction_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin, uint32_t *data);

/**
 * \brief gpio pin mode set (software, hardware modes)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pin desired pin
 * \param[in] mode desired mode
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t mode);

/**
 * \brief gpio pin mode get (software, hardware modes)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pin desired pin
 * \param[out] mode current mode
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t *mode);

/**
 * \brief gpio data, group
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_group_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data);

/**
 * \brief gpio data, group (software mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t data);

/**
 * \brief gpio data, group (software mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data);

/**
 * \brief gpio direction, group (software mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_group_direction_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t data);

/**
 * \brief gpio direction, group (software mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_group_direction_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t *data);

/**
 * \brief gpio data, group & pin (hardware mode, outputs only, tdd signals)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pin desired pin
 * \param[in] data desired source
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_source_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t data);

/**
 * \brief gpio data, group & pin (hardware mode, outputs only, tdd signals)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pin desired pin
 * \param[out] data current source
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_pin_source_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pin, uint32_t *data);

/**
 * \brief gpio buffer trigger set (per trigger index)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] num desired index
 * \param[in] data desired mode
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_buf_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num, uint32_t data);

/**
 * \brief gpio buffer trigger get (per trigger index)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] num desired index
 * \param[out] data current mode
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_buf_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num, uint32_t *data);

/**
 * \brief gpio trigger select
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pin desired pin
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_trig_sel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pin);

/**
 * \brief gpio gain-index select
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] group desired group
 * \param[in] pins desired pins
 * \param[in] count length of pins array (must be 8)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_gpio_gainindex_sel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t group, uint32_t pins[], uint32_t count);

// **********************************************************************************
// **********************************************************************************

/**
 * \brief initializes all the parameters of the ssi structure from a singluar mode
 *  specification, this is the simplest option to allow a quick setup.
 *  no register writes are performed, call config after initialization.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] ssi_param initialized to individual elements from mode
 * \param[in] ssi_mode desired ssi mode
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_init_param(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_ssi_params_t *ssi_param, uint32_t ssi_mode);

/**
 * \brief configure the ssi interface to the given parameters.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] ssi_param desired parameters
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, axi_adrv9102_ssi_params_t *ssi_param);

/**
 * \brief data pattern set (64bit maximum)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] data pattern set value array
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_data_pattern_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t data);

/**
 * \brief data pattern get (64bit maximum)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[out] data pattern get value array
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_data_pattern_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t *data);

/**
 * \brief data received get (64bit maximum), raw volatile data at the interface
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[out] data get value array
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_data_received_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint64_t *data);

/**
 * \brief data delay set (strobe + samples)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] data set delay
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_data_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data);

/**
 * \brief clock delay set (receive or transmit-clock-data-out)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] data set delay
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_clk_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data);

/**
 * \brief internal clock delay set (transmit clock tree delay)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] data set delay
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_clk_int_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data);

/**
 * \brief external clock delay set (transmit clock tree delay)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] data set delay
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_adrv9102_ssi_clk_ext_delay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t data);

/**
 * \brief ssi pulse pattern latency
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[out] data latency of pulse pattern loop back path
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_ssi_pattern_latency_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t *data);

/**
 * \brief ssi pulse pattern run, transmits, receives and reports latency
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[out] data latency of pulse pattern loop back path
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_ssi_pattern_run(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t *data);

/**
 * \brief ssi delay calibrate, call this function to calibrate and set the ssi delay
 *  for best clock to data timing.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] ssi_id of interface
 * \param[in] verbose set to output sweep and other information
 * \param[out] delay of ssi interface if successful
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_ssi_delay_calibrate(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t ssi_id, uint32_t verbose, uint32_t *delay);

// **********************************************************************************
// **********************************************************************************

/**
 * \brief tdd frame parameters set
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] param tdd frame set value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_frame_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_tdd_frame_params_t *tdd_param);

/**
 * \brief tdd frame parameters get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] param tdd frame get value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_frame_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_adrv9102_tdd_frame_params_t *tdd_param);

/**
 * \brief tdd start request
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id);

/**
 * \brief tdd stop request
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_stop(AXI_DEVICE_T *a_device, uint32_t peripheral_id);

/**
 * \brief tdd status
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data tdd status value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data);

/**
 * \brief tdd enable parameters set
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] tdd_id of the signal
 * \param[in] param tdd enable set value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t tdd_id, axi_adrv9102_tdd_enable_params_t *tdd_param);

/**
 * \brief tdd enable parameters get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] tdd_id of the signal
 * \param[out] param tdd enable get value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_adrv9102_tdd_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t tdd_id, axi_adrv9102_tdd_enable_params_t *tdd_param);

// **********************************************************************************
// **********************************************************************************

#ifdef __cplusplus
}
#endif

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************
