/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADE7913 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ade7913.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// UART init params
extern struct no_os_uart_init_param uart_ip;
// GPIO LED init params
extern struct no_os_gpio_init_param gpio_led1_ip;
// SPI init params
extern struct no_os_spi_init_param ade7913_spi_ip0;
extern struct no_os_spi_init_param ade7913_spi_ip1;
extern struct no_os_spi_init_param ade7913_spi_ip2;
//GPIO RDY init params
extern struct no_os_gpio_init_param ade7913_gpio_rdy_ip;

/* Initial values for the rms filters */
#define INITIAL                     	1024
#define SAMPLES                     	1024

/* Scale for voltage divider */
#define V_MULT                      	(1000ul)
#define V_DIVIDER                   	(1001ul)

struct rms_adc_values {

	/* I rms value */
	int32_t				*i_rms;
	/* V1 rms value */
	int32_t				*v1_rms;
	/* V2 rms value */
	int32_t				*v2_rms;
	/* I ADC value */
	int32_t				*i_rms_adc;
	/* V1 ADC value */
	int32_t				*v1_rms_adc;
	/* V2 ADC value */
	int32_t				*v2_rms_adc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Toggle user LED */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

/* Saves the current and voltage values of device 1 in rms_adc structure */
int rms_adc_values_read(struct ade7913_dev *dev, struct rms_adc_values *rms);

/* Saves the current and voltage values of device 2 in rms_adc structure */
int rms_adc_values_read_2(struct ade7913_dev *dev, struct rms_adc_values *rms);

/* Saves the current and voltage values of device 3 in rms_adc structure */
int rms_adc_values_read_3(struct ade7913_dev *dev, struct rms_adc_values *rms);

/* Compute v1 rms value for device 1 */
int64_t rms_filter_v1(int32_t sample);

/* Compute v2 rms value for device 1 */
int64_t rms_filter_v2(int32_t sample);

/* Compute i rms value for device 1 */
int64_t rms_filter_i(int32_t sample);

/* Compute v1_2 rms value for device 2 */
int64_t rms_filter_v1_2(int32_t sample);

/* Compute v2_2 rms value for device 2 */
int64_t rms_filter_v2_2(int32_t sample);

/* Compute i_2 rms value for device 2 */
int64_t rms_filter_i_2(int32_t sample);

/* Compute v1_3 rms value for device 3 */
int64_t rms_filter_v1_3(int32_t sample);

/* Compute v2_3 rms value for device 3 */
int64_t rms_filter_v2_3(int32_t sample);

/* Compute i_3 rms value for device 3 */
int64_t rms_filter_i_3(int32_t sample);

/* Scale V value */
int64_t supply_scale_v(int32_t val);

#endif /* __COMMON_DATA_H__ */
