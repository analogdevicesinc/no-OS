/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Common data header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#if defined(PQM_CONN_T1L)
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "adin1110.h"
#endif

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

#if defined(PQM_CONN_USB)
extern struct no_os_uart_init_param iio_demo_usb_ip;
#elif defined(PQM_CONN_SERIAL)
extern struct no_os_uart_init_param iio_demo_serial_ip;
#elif defined(PQM_CONN_T1L)
extern struct no_os_uart_init_param iio_demo_serial_ip;
extern const struct no_os_gpio_init_param adin1110_int_ip;
extern const struct no_os_gpio_init_param adin1110_rst_gpio_ip;
extern const struct no_os_gpio_init_param adin1110_swpd_ip;
extern const struct no_os_gpio_init_param adin1110_tx2p4_ip;
extern const struct no_os_gpio_init_param adin1110_mssel_ip;
extern const struct no_os_gpio_init_param adin1110_cfg0_ip;
extern const struct no_os_gpio_init_param adin1110_cfg1_ip;
extern const struct no_os_spi_init_param adin1110_spi_ip;

extern struct adin1110_init_param adin1110_ip;
extern struct lwip_network_param lwip_ip;
#endif

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
