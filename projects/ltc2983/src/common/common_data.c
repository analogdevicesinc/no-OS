/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc2983 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#if defined(ADT7604_BASIC_EXAMPLE) || defined(ADT7604_IIO_EXAMPLE)

struct ltc2983_init_param ltc2983_ip = {
	.spi_init = ltc2983_spi_ip,
	.gpio_rstn = ltc2983_gpio_rstn,
	.mux_delay_config_us = 1000,
	.filter_notch_freq = 0,
	.sensors[0]  = NULL,
	.sensors[1]  = &ltc2983_rsense_2.sensor,   /* CH2:  100Ω rsense (R4) for copper traces */
	.sensors[2]  = NULL,
	.sensors[3]  = &ltc2983_copper_4.sensor,   /* CH4:  sub-ohm copper trace */
	.sensors[4]  = NULL,
	.sensors[5]  = &ltc2983_copper_6.sensor,   /* CH6:  sub-ohm copper trace */
	.sensors[6]  = NULL,
	.sensors[7]  = &ltc2983_copper_8.sensor,   /* CH8:  sub-ohm copper trace */
	.sensors[8]  = NULL,
	.sensors[9]  = NULL,
	.sensors[10] = NULL,
	.sensors[11] = &ltc2983_rsense_12.sensor,  /* CH12: 1kΩ rsense (R6) for leak + PT100 */
	.sensors[12] = NULL,
	.sensors[13] = &ltc2983_leak_14.sensor,    /* CH14: leak detector */
	.sensors[14] = NULL,
	.sensors[15] = &ltc2983_leak_16.sensor,    /* CH16: leak detector */
	.sensors[16] = NULL,
	.sensors[17] = &ltc2983_rtd_18.sensor,     /* CH18: PT100 2-wire */
	.sensors[18] = NULL,
	.sensors[19] = NULL,
	.dev_type = ID_ADT7604,
};

/*
 * 100Ω precision resistor R4, sense resistor for copper trace channels 4/6/8.
 * Copper trace rsense must be entered in mΩ.
 */
struct ltc2983_rsense ltc2983_rsense_2 = {
	.sensor = {
		.chan = 2,
		.type = LTC2983_RSENSE,
	},
	.r_sense_val = {
		.r_sense_val_int = 100000, /* 100 Ω = 100,000 mΩ */
	},
};

/* 1kΩ precision resistor R6, sense resistor for leak detectors and PT100 */
struct ltc2983_rsense ltc2983_rsense_12 = {
	.sensor = {
		.chan = 12,
		.type = LTC2983_RSENSE,
	},
	.r_sense_val = {
		.r_sense_val_int = 1000,
	},
};

/* Copper trace CH3-CH4, sense resistor on CH2 */
struct ltc2983_rtd ltc2983_copper_4 = {
	.sensor = {
		.chan = 4,
		.type = LTC2983_RTD_CUSTOM,
	},
	.r_sense_chan = 2,
	.sub_ohm = true,
};

/* Copper trace CH5-CH6, sense resistor on CH2 */
struct ltc2983_rtd ltc2983_copper_6 = {
	.sensor = {
		.chan = 6,
		.type = LTC2983_RTD_CUSTOM,
	},
	.r_sense_chan = 2,
	.sub_ohm = true,
};

/* Copper trace CH7-CH8, sense resistor on CH2 */
struct ltc2983_rtd ltc2983_copper_8 = {
	.sensor = {
		.chan = 8,
		.type = LTC2983_RTD_CUSTOM,
	},
	.r_sense_chan = 2,
	.sub_ohm = true,
};

/*
 * Leak detector resistance-to-coverage lookup table (ADT7604 datasheet Table 41).
 * Each pair: (resistance_raw, coverage_raw)
 *   resistance_raw = resistance_Ohm * 16          (4 fractional bits)
 *   coverage_raw   = (coverage_percent + 273.15) * 1024  (10 fractional bits)
 * Table must be in ascending resistance order.
 */
static uint32_t ltc2983_leak_table[20] = {
	0, 382106,  /* P0:      0 Ohm -> 100% */
	3232, 381082,  /* P1:  202.02 Ohm ->  99% */
	4571, 351386,  /* P2:  285.71 Ohm ->  70% */
	5333, 341146,  /* P3:  333.33 Ohm ->  60% */
	6400, 330906,  /* P4:    400 Ohm ->  50% */
	8000, 320666,  /* P5:    500 Ohm ->  40% */
	10667, 310426,  /* P6:  666.67 Ohm ->  30% */
	16000, 300186,  /* P7:    1 kOhm ->  20% */
	32000, 289946,  /* P8:    2 kOhm ->  10% */
	16000000, 279706,  /* P9:    1 MOhm ->   0% */
};

struct ltc2983_custom_sensor ltc2983_leak_custom = {
	.table = ltc2983_leak_table,
	.len = 20,
	.is_steinhart = false,
};

/* Leak detector CH13-CH14, sense resistor on CH12 */
struct ltc2983_thermistor ltc2983_leak_14 = {
	.sensor = {
		.chan = 14,
		.type = LTC2983_THERMISTOR_CUSTOM,
	},
	.r_sense_chan = 12,
	.excitation_current = 3,
	.custom = &ltc2983_leak_custom,
};

/* Leak detector CH15-CH16, sense resistor on CH12 */
struct ltc2983_thermistor ltc2983_leak_16 = {
	.sensor = {
		.chan = 16,
		.type = LTC2983_THERMISTOR_CUSTOM,
	},
	.r_sense_chan = 12,
	.excitation_current = 3,
	.custom = &ltc2983_leak_custom,
};

/* PT100 on CH17-CH18, 2-wire, sense resistor on CH12 */
struct ltc2983_rtd ltc2983_rtd_18 = {
	.sensor = {
		.chan = 18,
		.type = LTC2983_RTD_PT_100,
	},
	.r_sense_chan = 12,
	.sensor_config = 1,
	.excitation_current = 0x7,
	.rtd_curve = 0,
};

#else

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
	.dev_type = ID_LTC2983,
};

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

#endif /* ADT7604_BASIC_EXAMPLE || ADT7604_IIO_EXAMPLE */
