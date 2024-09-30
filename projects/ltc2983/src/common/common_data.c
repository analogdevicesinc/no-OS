/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc2983 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "common_data.h"
#include <stdbool.h>

struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

const struct no_os_spi_init_param ltc2983_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

const struct no_os_gpio_init_param ltc2983_gpio_rstn = {
	.port = GPIO_RSTN_PORT_NUM,
	.number = GPIO_RSTN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct ltc2983_init_param ltc2983_ip = {
	.spi_init = ltc2983_spi_ip,
	.gpio_rstn = ltc2983_gpio_rstn,
	.mux_delay_config_us = 1000,
	.filter_notch_freq = 0,
	.sensors[0] = NULL,
	.sensors[1] = &ltc2983_rsense_2.sensor,
	.sensors[2] = NULL,//&ltc2983_rsense_3.sensor,
	.sensors[3] = NULL,
	.sensors[4] = NULL,
	.sensors[5] = NULL,//&ltc2983_rtd_6.sensor,
	.sensors[6] = NULL,
	.sensors[7] = &ltc2983_thermistor_8.sensor,
	.sensors[8] = NULL,
	.sensors[9] = &ltc2983_thermistor_10.sensor,
	.sensors[10] = NULL,//&ltc2983_rtd_11.sensor,
	.sensors[11] = NULL,
	.sensors[12] = NULL,
	.sensors[13] = NULL,
	.sensors[14] = NULL,
	.sensors[15] = NULL,
	.sensors[16] = NULL,
	.sensors[17] = NULL,
	.sensors[18] = NULL,
	.sensors[19] = NULL,
	.custom_addr_ptr = LTC2983_CUST_SENS_TBL_START_REG,
	.dev_type = ID_LTC2983,
};

/******************************************************************************/
/****************************** DC2214A Sensors *******************************/
/******************************************************************************/

/**
 * Sense resistor connected to channels 1-2
 * Resistance: 2k ohms
 */
struct ltc2983_rsense ltc2983_rsense_2 = {
	.sensor = {
		.chan = 2,
		.type = LTC2983_RSENSE,
	},
	.r_sense_val = { // 10kohms
		.r_sense_val_int = 10000,
	}
};

/**
 * Thermistor simulator connected to channel 8
 * Sense Resistor: Channel 2
 */
struct ltc2983_thermistor ltc2983_thermistor_8 = {
	.sensor = {
		.chan = 8,
		.type = LTC2983_THERMISTOR_44008_44032,
	},
	.r_sense_chan = 2,
	.sensor_config = 0x2,
	.excitation_current = 0x3,
};

/**
 * Thermistor fixed simulator connected to channel 10
 * Sense Resistor: Channel 2
 * Expected: -30.59 C
 */
struct ltc2983_thermistor ltc2983_thermistor_10 = {
	.sensor = {
		.chan = 10,
		.type = LTC2983_THERMISTOR_44008_44032,
	},
	.r_sense_chan = 2,
	.sensor_config = 0x2,
	.excitation_current = 0x3,
};

/******************************************************************************/
/****************************** DC2213A Sensors *******************************/
/******************************************************************************/

/**
 * Sense resistor connected to channels 2-3
 * Resistance: 2k ohms
 */
struct ltc2983_rsense ltc2983_rsense_3 = {
	.sensor = {
		.chan = 3,
		.type = LTC2983_RSENSE,
	},
	.r_sense_val = { // 2kohms
		.r_sense_val_int = 2000,
	}
};

/**
 * RTD fixed simulator connected to channels 3-6
 * Type: PT-100
 * Sense resistor: channel 3
 */
struct ltc2983_rtd ltc2983_rtd_6 = {
	.sensor = {
		.chan = 6,
		.type = LTC2983_RTD_PT_100,
	},
	.r_sense_chan = 3,
	.sensor_config = 9,
	.excitation_current = 0x5,
	.rtd_curve = 0x1,
};

/**
 * RTD simulator connected to channels 10-11
 * Type: PT-1000
 * Sense resistor: channel 3
 */
struct ltc2983_rtd ltc2983_rtd_11 = {
	.sensor = {
		.chan = 11,
		.type = LTC2983_RTD_PT_1000,
	},
	.r_sense_chan = 3,
	.sensor_config = 1,
	.excitation_current = 0x2,
	.rtd_curve = 0x2,
};
