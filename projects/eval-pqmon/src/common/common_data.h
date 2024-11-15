/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Common data header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "adi_pqlib.h"
#include "iio.h"
#include "iio_app.h"
#include "iio_pqm.h"
#include "iio_types.h"
#include "no_os_i2c.h"
#include "no_os_irq.h"
#include "no_os_spi.h"
#include "no_os_timer.h"
#include "no_os_uart.h"
#include "parameters.h"
#include "pqlib_example.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FW_VERSION 2.1

#define IIO_BUFF_TYPE int16_t
#define SAMPLES_PER_CHANNEL_PLATFORM 256
#define MAX_SIZE_BASE_ADDR (SAMPLES_PER_CHANNEL_PLATFORM * TOTAL_PQM_CHANNELS)
#define MAX_SIZE_BASE_ADDR_WITH_SIZE                                           \
  (MAX_SIZE_BASE_ADDR * sizeof(IIO_BUFF_TYPE))

#define TOTAL_PQM_CHANNELS 7
#define VOLTAGE_CH_NUMBER 3
#define MAX_CH_ATTRS 12
#define PQM_DEVICE_ATTR_NUMBER 35
#define WAVEFORM_BUFFER_LENGTH (256 * 7)

extern IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR];
extern struct no_os_uart_init_param iio_demo_uart_ip;
extern struct pqm_init_para pqm_ip;
extern struct no_os_spi_init_param spi_egy_ip;
extern struct no_os_i2c_init_param i2c_ip;
extern struct no_os_uart_init_param uart_ip_stdio;
extern struct no_os_timer_init_param timer_ip;
extern struct no_os_gpio_init_param reset_gpio_ip;
extern struct no_os_gpio_init_param intr_gpio_ip;
extern struct no_os_irq_init_param afe_callback_ctrl_ip;
extern struct no_os_callback_desc afe0_callback_desc;

static const char *const pqm_v_consel_available[] = {
	[VCONSEL_4W_WYE] = "4W_WYE",
	[VCONSEL_3W_DELTA_VB_VA_NEGVC] = "3W_DELTA_VA_VB_NEGVC",
	[VCONSEL_4W_WYE_VB_NEGVA_NEGVC] = "4W_WYE_VB_NEGVA_NEGVC",
	[VCONSEL_4W_DELTA_VB_NEGVA] = "4W_DELTA_VB_NEGVA",
	[VCONSEL_4W_DELTA_VA_VB_VC] = "4W_DELTA_VA_VB_VC",
};

static const char *const pqm_flicker_model_available[] = {
	[ADI_PQLIB_FLICKER_MODEL_230V_50HZ] = "230V_50HZ",
	[ADI_PQLIB_FLICKER_MODEL_120V_50HZ] = "120V_50HZ",
	[ADI_PQLIB_FLICKER_MODEL_230V_60HZ] = "230V_60HZ",
	[ADI_PQLIB_FLICKER_MODEL_120V_60HZ] = "120V_60HZ",
};

static const char *const pqm_nominal_frequency_available[] = {
	[ADI_PQLIB_NOMINAL_FREQUENCY_50HZ] = "50",
	[ADI_PQLIB_NOMINAL_FREQUENCY_60HZ] = "60",
};

struct pqm_desc {
	uint8_t reg[TOTAL_PQM_CHANNELS];
	float pqm_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t pqm_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t active_ch;
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

struct pqm_init_para {
	float dev_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t dev_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

#endif /* __COMMON_DATA_H__ */
