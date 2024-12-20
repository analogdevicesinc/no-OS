/***************************************************************************//**
 *   @file   ad8460.h
 *   @brief  Implementation of AD8460 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
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

#ifndef __AD8460_H__
#define __AD8460_H__

#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define AD8460_HVDAC_DATA_WORD(x)		(0x60 + (2 * (x)))

#define AD8460_HV_RESET_MSK			NO_OS_BIT(7)
#define AD8460_HV_SLEEP_MSK			NO_OS_BIT(4)
#define AD8460_WAVE_GEN_MODE_MSK		NO_OS_BIT(0)

#define AD8460_HVDAC_SLEEP_MSK			NO_OS_BIT(3)

#define AD8460_FAULT_ARM_MSK			NO_OS_BIT(7)
#define AD8460_FAULT_LIMIT_MSK			NO_OS_GENMASK(6, 0)

#define AD8460_APG_MODE_ENABLE_MSK		NO_OS_BIT(5)
#define AD8460_PATTERN_DEPTH_MSK		NO_OS_GENMASK(3, 0)

#define AD8460_QUIESCENT_CURRENT_MSK		NO_OS_GENMASK(7, 0)

#define AD8460_SHUTDOWN_FLAG_MSK		NO_OS_BIT(7)

#define AD8460_DATA_BYTE_LOW_MSK		NO_OS_GENMASK(7, 0)
#define AD8460_DATA_BYTE_HIGH_MSK		NO_OS_GENMASK(5, 0)
#define AD8460_DATA_BYTE_FULL_MSK		NO_OS_GENMASK(13, 0)

#define AD8460_DEFAULT_FAULT_PROTECT		0x00
#define AD8460_DATA_BYTE_WORD_LENGTH		2
#define AD8460_NUM_DATA_WORDS			16
#define AD8460_NOMINAL_VOLTAGE_SPAN		80
#define AD8460_MIN_EXT_RESISTOR_OHMS		2000
#define AD8460_MAX_EXT_RESISTOR_OHMS		20000
#define AD8460_MIN_VREFIO_UV			120000
#define AD8460_MAX_VREFIO_UV			1200000
#define AD8460_ABS_MAX_OVERVOLTAGE_UV		55000000
#define AD8460_ABS_MAX_OVERCURRENT_UA		1000000
#define AD8460_MAX_OVERTEMPERATURE_MC		150000
#define AD8460_MIN_OVERTEMPERATURE_MC		20000
#define AD8460_CURRENT_LIMIT_CONV(x)		((x) / 15625)
#define AD8460_VOLTAGE_LIMIT_CONV(x)		((x) / 1953000)
#define AD8460_TEMP_LIMIT_CONV(x)		(((x) + 266640) / 6510)

/**
 * @brief AD8460 descriptor
 */
struct ad8460_device {
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
	/** Reset GPIO descriptor */
	struct no_os_gpio_desc *gpio_rstn;
	/** REFIO drive voltage in millivolts */
	int refio_1p2v_mv;
	/** External resistor connected to FS_ADJ in ohms */
	uint32_t ext_resistor_ohms;
};

/**
 * @brief AD8460 init param
 */
struct ad8460_init_param {
	/** Host processor SPI configuration */
	struct no_os_spi_init_param spi_init_param;
	/** Reset GPIO configuration */
	struct no_os_gpio_init_param gpio_rstn;
	/** REFIO drive voltage in millivolts */
	int refio_1p2v_mv;
	/** External resistor connected to FS_ADJ in ohms */
	uint32_t ext_resistor_ohms;
};

/** Read a register value */
int ad8460_reg_read(struct ad8460_device *dev, uint8_t addr, uint8_t *val);

/** Write a register value */
int ad8460_reg_write(struct ad8460_device *dev, uint8_t addr, uint8_t val);

/** Read-modify-write operation */
int ad8460_reg_update_bits(struct ad8460_device *dev, uint8_t addr,
			   uint8_t mask, uint8_t val);

/** Device and comm init function */
int ad8460_init(struct ad8460_device **dev,
		struct ad8460_init_param *init_param);

/** Free resources allocated by the init function */
int ad8460_remove(struct ad8460_device *dev);

/** Reset the AD8460 device */
int ad8460_reset(struct ad8460_device *dev);

/** Get the value of a high voltage DAC word */
int ad8460_get_hvdac_word(struct ad8460_device *dev, int8_t index,
			  uint16_t *val);

/** Set the value of a high voltage DAC word */
int ad8460_set_hvdac_word(struct ad8460_device *dev, int8_t index,
			  uint16_t val);

/** Enable or disable the APG mode */
int ad8460_enable_apg_mode(struct ad8460_device *dev, int val);

/** Read the shutdown flag */
int ad8460_read_shutdown_flag(struct ad8460_device *dev, uint8_t *flag);

/** Reset the high voltage driver */
int ad8460_hv_reset(struct ad8460_device *dev);

/** Set the sample value */
int ad8460_set_sample(struct ad8460_device *dev, uint16_t val);

#endif	/* __AD8460_H__ */
