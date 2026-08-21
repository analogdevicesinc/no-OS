/***************************************************************************//**
 *   @file   ad7191.h
 *   @brief  Header file of AD7191 driver.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#ifndef AD7191_H_
#define AD7191_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* 24-bit conversion result. */
#define AD7191_RESOLUTION		24
/* Offset-binary midscale (0 V differential input). */
#define AD7191_MIDSCALE			0x800000
/* Temperature sensor sensitivity, in codes per degree Celsius (see datasheet). */
#define AD7191_TEMP_CODES_PER_DEGREE	2815
/* Number of settling conversions discarded after a configuration change. */
#define AD7191_SETTLING_CONVERSIONS	1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum ad7191_channel
 * @brief AD7191 input channel selection.
 *
 * The AD7191 exposes two differential input pairs plus an internal temperature
 * sensor. The selection is driven on the CHAN and TEMP pins; TEMP overrides
 * CHAN. The values match the address encoding used by the register-less
 * sigma-delta core (bit 0 -> CHAN, bit 1 -> TEMP).
 */
enum ad7191_channel {
	/** AIN1(+)/AIN2(-) differential pair (CHAN low, TEMP low). */
	AD7191_CH_AIN1_AIN2,
	/** AIN3(+)/AIN4(-) differential pair (CHAN high, TEMP low). */
	AD7191_CH_AIN3_AIN4,
	/** Internal temperature sensor (TEMP high). */
	AD7191_CH_TEMP,
	AD7191_CH_NR,
};

/**
 * @enum ad7191_pga_gain
 * @brief AD7191 PGA gain selection, driven on the PGA1/PGA2 pins (Table 7).
 */
enum ad7191_pga_gain {
	/** Gain of 1. */
	AD7191_PGA_GAIN_1,
	/** Gain of 8. */
	AD7191_PGA_GAIN_8,
	/** Gain of 64. */
	AD7191_PGA_GAIN_64,
	/** Gain of 128. */
	AD7191_PGA_GAIN_128,
	AD7191_PGA_GAIN_NR,
};

/**
 * @enum ad7191_odr
 * @brief AD7191 output data rate selection, driven on the ODR1/ODR2 pins
 *        (Table 5).
 */
enum ad7191_odr {
	/** 120 Hz. */
	AD7191_ODR_120,
	/** 60 Hz. */
	AD7191_ODR_60,
	/** 50 Hz. */
	AD7191_ODR_50,
	/** 10 Hz. */
	AD7191_ODR_10,
	AD7191_ODR_NR,
};

/**
 * @enum ad7191_clock_sel
 * @brief AD7191 master clock source, driven on the CLKSEL pin.
 */
enum ad7191_clock_sel {
	/** External clock or crystal on MCLK1/MCLK2 (CLKSEL low). */
	AD7191_EXT_CLK,
	/** Internal 4.92 MHz clock (CLKSEL high). */
	AD7191_INT_CLK,
};

/**
 * @struct ad7191_init_param
 * @brief AD7191 device initialization parameters.
 */
struct ad7191_init_param {
	/** SPI initialization parameters. */
	struct no_os_spi_init_param spi_init;
	/** PDOWN pin: power-down / reset (high) or power-up / convert (low). */
	struct no_os_gpio_init_param *gpio_pdown;
	/** DOUT/RDY pin, wired to a second GPIO for data-ready polling. */
	struct no_os_gpio_init_param *gpio_rdy;
	/** CLKSEL pin. */
	struct no_os_gpio_init_param *gpio_clksel;
	/** TEMP pin. */
	struct no_os_gpio_init_param *gpio_temp;
	/** CHAN pin. */
	struct no_os_gpio_init_param *gpio_chan;
	/** PGA1 pin (gain LSB). */
	struct no_os_gpio_init_param *gpio_pga1;
	/** PGA2 pin (gain MSB). */
	struct no_os_gpio_init_param *gpio_pga2;
	/** ODR1 pin (data rate LSB). */
	struct no_os_gpio_init_param *gpio_odr1;
	/** ODR2 pin (data rate MSB). */
	struct no_os_gpio_init_param *gpio_odr2;
	/** Reference voltage, in millivolts. */
	uint16_t vref_mv;
	/** Initial clock source. */
	enum ad7191_clock_sel clock_sel;
	/** Initial input channel. */
	enum ad7191_channel channel;
	/** Initial PGA gain. */
	enum ad7191_pga_gain gain;
	/** Initial output data rate. */
	enum ad7191_odr odr;
};

/**
 * @struct ad7191_dev
 * @brief AD7191 device descriptor.
 */
struct ad7191_dev {
	/** SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/** PDOWN pin descriptor. */
	struct no_os_gpio_desc *gpio_pdown;
	/** DOUT/RDY pin descriptor (input, for data-ready polling). */
	struct no_os_gpio_desc *gpio_rdy;
	/** CLKSEL pin descriptor. */
	struct no_os_gpio_desc *gpio_clksel;
	/** TEMP pin descriptor. */
	struct no_os_gpio_desc *gpio_temp;
	/** CHAN pin descriptor. */
	struct no_os_gpio_desc *gpio_chan;
	/** PGA1 pin descriptor. */
	struct no_os_gpio_desc *gpio_pga1;
	/** PGA2 pin descriptor. */
	struct no_os_gpio_desc *gpio_pga2;
	/** ODR1 pin descriptor. */
	struct no_os_gpio_desc *gpio_odr1;
	/** ODR2 pin descriptor. */
	struct no_os_gpio_desc *gpio_odr2;
	/** Reference voltage, in millivolts. */
	uint16_t vref_mv;
	/** Current clock source. */
	enum ad7191_clock_sel clock_sel;
	/** Current input channel. */
	enum ad7191_channel channel;
	/** Current PGA gain. */
	enum ad7191_pga_gain gain;
	/** Current output data rate. */
	enum ad7191_odr odr;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** Output data rate value, in Hz, for each ad7191_odr setting. */
extern const uint32_t ad7191_odr_table[AD7191_ODR_NR];

/** PGA gain value for each ad7191_pga_gain setting. */
extern const uint32_t ad7191_gain_table[AD7191_PGA_GAIN_NR];

/** Initialize the AD7191 device. */
int ad7191_init(struct ad7191_dev **device,
		struct ad7191_init_param *init_param);

/** Free the resources allocated by ad7191_init(). */
int ad7191_remove(struct ad7191_dev *dev);

/** Select the clock source (CLKSEL pin). */
int ad7191_set_clock(struct ad7191_dev *dev, enum ad7191_clock_sel clock_sel);

/** Select the input channel (CHAN/TEMP pins). */
int ad7191_set_channel(struct ad7191_dev *dev, enum ad7191_channel channel);

/** Select the PGA gain (PGA1/PGA2 pins). */
int ad7191_set_gain(struct ad7191_dev *dev, enum ad7191_pga_gain gain);

/** Select the output data rate (ODR1/ODR2 pins). */
int ad7191_set_odr(struct ad7191_dev *dev, enum ad7191_odr odr);

/** Power up the device and start conversions (PDOWN low). */
int ad7191_power_up(struct ad7191_dev *dev);

/** Power down and reset the device (PDOWN high). */
int ad7191_power_down(struct ad7191_dev *dev);

/** Wait for a conversion to become ready (DOUT/RDY low). */
int ad7191_wait_ready(struct ad7191_dev *dev);

/** Read one 24-bit conversion result, assuming the device is powered up. */
int ad7191_read_data(struct ad7191_dev *dev, uint32_t *data);

/** Configure the channel, power up, discard settling data and read one sample. */
int ad7191_single_conversion(struct ad7191_dev *dev,
			     enum ad7191_channel channel, uint32_t *data);

#endif /* AD7191_H_ */
