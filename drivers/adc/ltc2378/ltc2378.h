/***************************************************************************//**
 *   @file   ltc2378.h
 *   @brief  Header file of the LTC2378 Driver
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
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

#ifndef __LTC2378_H__
#define __LTC2378_H__

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"

#define LTC2378_RESOLUTION_UNIPOLAR (1 << 20) /* 2^20 = 1048576 */
#define LTC2378_RESOLUTION_BIPOLAR  (1 << 19) /* 2^19 = 524288 */

/* Timing constants */
#define LTC2378_BUSY_TIMEOUT_US     1000      /* Timeout for BUSY signal in microseconds */
#define LTC2378_CNV_PULSE_WIDTH_US  1         /* CNV pulse width in microseconds */
#define LTC2378_CONVERSION_DELAY_US 1         /* Conversion delay when BUSY not available */
#define LTC2378_POWERDOWN_DELAY_US  2         /* Power-down setup time in microseconds */

/* IIO scale calculation constants */
#define LTC2378_IIO_SCALE_UNIPOLAR_SHIFT  20  /* 2^20 for unipolar mode */
#define LTC2378_IIO_SCALE_BIPOLAR_SHIFT   19  /* 2^19 for bipolar mode */

/* SPI data parsing constants */
#define LTC2378_DATA_MASK             0xFFFFF  /* 20-bit data mask */
#define LTC2378_DATA_SHIFT_BYTE0      12       /* Shift for byte 0 */
#define LTC2378_DATA_SHIFT_BYTE1      4        /* Shift for byte 1 */
#define LTC2378_DATA_SHIFT_BYTE2      4        /* Right shift for byte 2 */
#define LTC2378_SIGN_BIT_POSITION     19       /* Sign bit position for bipolar mode */

/**
 * @enum ltc2378_input_mode
 * @brief Input mode configuration for LTC2378-20
 */
enum ltc2378_input_mode {
	LTC2378_BIPOLAR,   /* Bipolar input: -Vref to +Vref */
	LTC2378_UNIPOLAR   /* Unipolar input: 0 to Vref */
};

/**
 * @struct ltc2378_dev
 * @brief Device descriptor for LTC2378-20
 */
struct ltc2378_dev {
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *gpio_cnv;
	struct no_os_gpio_desc *gpio_busy;
	uint32_t vref_uv;
	enum ltc2378_input_mode input_mode;
};

/**
 * @struct ltc2378_init_param
 * @brief Initialization parameters for LTC2378-20
 */
struct ltc2378_init_param {
	const struct no_os_spi_init_param *spi_init;
	const struct no_os_gpio_init_param *gpio_cnv_init;
	const struct no_os_gpio_init_param *gpio_busy_init;
	uint32_t vref_uv;
	enum ltc2378_input_mode input_mode;
};

/* Function declarations */

/* Initialize the LTC2378-20 device */
int ltc2378_init(struct ltc2378_dev **device,
		 struct ltc2378_init_param *init_param);

/* Remove the LTC2378-20 device and free allocated resources */
int ltc2378_remove(struct ltc2378_dev *dev);

/* Start a single ADC conversion */
int ltc2378_start_conversion(struct ltc2378_dev *dev);

/* Read a single raw ADC conversion result */
int ltc2378_read_raw(struct ltc2378_dev *dev, uint32_t *data);

/* Read and average multiple ADC conversion results */
int ltc2378_read_avg(struct ltc2378_dev *dev, uint32_t *avg_data,
		     uint16_t samples);

/* Convert raw ADC value to voltage in microvolts */
int ltc2378_raw_to_uv(struct ltc2378_dev *dev, uint32_t raw,
		      int32_t *voltage_uv);

/* Put the LTC2378-20 into power-down mode */
int ltc2378_power_down(struct ltc2378_dev *dev);

#endif /* __LTC2378_H__ */
