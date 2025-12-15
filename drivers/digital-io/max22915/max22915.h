/***************************************************************************//**
 *   @file   max22915.h
 *   @brief  Header file of MAX22915 Driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
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
#ifndef _MAX22915_H
#define _MAX22915_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX22915_MAX_CHANNELS		8
#define MAX22915_ADDR_MASK		NO_OS_GENMASK(5, 1)
#define MAX22915_RW_MASK		NO_OS_BIT(0)
#define MAX22915_CHIP_ADDR_MASK		NO_OS_GENMASK(7, 6)

#define MAX22915_REG_SET_OUTPUT	0x00
#define MAX22915_REG_SET_FLED	0x01
#define MAX22915_REG_SET_SLED	0x02
#define MAX22915_REG_INTERRUPT	0x03
#define MAX22915_REG_OVL_FLT	0x04
#define MAX22915_REG_CL_FLT	0x05
#define MAX22915_REG_OWOFF_FLT	0x06
#define MAX22915_REG_OWON_FLT	0x07
#define MAX22915_REG_SHTVDD_FLT	0x08
#define MAX22915_REG_GLOBAL_ERR	0x09
#define MAX22915_REG_OWOFF_ENABLE	0x0A
#define MAX22915_REG_OWON_ENABLE	0x0B
#define MAX22915_REG_SHTVDD_ENABLE	0x0C
#define MAX22915_REG_CONFIG1	0x0D
#define MAX22915_REG_CONFIG2	0x0E
#define MAX22915_REG_MASK	0x0F
#define MAX22915_REG_CONFIG3	0x10
#define MAX22915_REG_THERM_WARN_THR	0x11
#define MAX22915_REG_CURR_MEAS_ENABLE	0x12
#define MAX22915_REG_ADC_CONFIG1	0x13
#define MAX22915_REG_ADC_CONFIG2	0x14
#define MAX22915_REG_ADC_CONFIG3	0x15
#define MAX22915_REG_ADC_DATA	0x16
#define MAX22915_REG_OWON_FILTER	0x17

#define MAX22915_CH_MASK(channel)	NO_OS_BIT(channel)
#define MAX22915_SLED_SET_MASK		NO_OS_BIT(1)
#define MAX22915_FLED_SET_MASK		NO_OS_BIT(0)
#define MAX22915_CURR_AVG_MASK		NO_OS_GENMASK(6, 4)
#define MAX22915_CURR_SINGLE_MASK	NO_OS_BIT(3)
#define MAX22915_CURR_CONT_MASK		NO_OS_BIT(2)
#define MAX22915_TEMP_AVG_MASK		NO_OS_GENMASK(6, 4)
#define MAX22915_THERM_WARN_EN_MASK	NO_OS_BIT(3)
#define MAX22915_TEMP_CONT_MASK		NO_OS_BIT(2)
#define MAX22915_SPI_FRAME_SIZE		2

enum max22915_sw_state {
	SWITCH_OPEN,
	SWITCH_CLOSE,
};

enum max22915_led_state {
	LED_ON,
	LED_OFF,
};

struct max22915_current_config {
	uint8_t curr_average;
	bool curr_one_shot_en;
	bool curr_cont;
};

struct max22915_temp_config {
	uint8_t temp_average;
	bool therm_warn_en;
	bool temp_cont;
};

struct max22915_ow_config {
	bool owoff_enable;
	bool owon_enable;
};

struct max22915_desc {
	struct no_os_spi_desc *spi_desc;
	uint8_t chip_address;
	bool crc_en;
	enum max22915_sw_state sw_state[MAX22915_MAX_CHANNELS];
	bool shtvdd_enable[MAX22915_MAX_CHANNELS];
	struct max22915_ow_config ow_config[MAX22915_MAX_CHANNELS];
	uint8_t buff[MAX22915_SPI_FRAME_SIZE + 1];
};

struct max22915_init_param {
	struct no_os_spi_init_param *spi_init;
	uint8_t chip_address;
	enum max22915_sw_state sw_state[MAX22915_MAX_CHANNELS];
	struct max22915_ow_config ow_config[MAX22915_MAX_CHANNELS];
	bool shtvdd_enable[MAX22915_MAX_CHANNELS];
	struct max22915_current_config curr_config;
	struct max22915_temp_config temp_config;
	bool crc_en;
};

/** Initialize and configure the MAX22915 device */
int max22915_init(struct max22915_desc **desc,
		  struct max22915_init_param *init_param);

/** Write register data to MAX22915 */
int max22915_reg_write(struct max22915_desc *desc, uint8_t reg_addr,
		       uint8_t reg_data);

/** Read register data from MAX22915 */
int max22915_reg_read(struct max22915_desc *desc, uint8_t reg_addr,
		      uint8_t *reg_data);

/** Write register data to MAX22915 with bit mask */
int max22915_reg_write_mask(struct max22915_desc *desc,
			    uint8_t reg_addr,
			    uint8_t mask,
			    uint8_t reg_data);

/** Set the temperature measurement configuration */
int max22915_set_temperature_config(struct max22915_desc *dev,
				    struct max22915_temp_config *config);

/** Set the current measurement configuration */
int max22915_set_current_config(struct max22915_desc *dev,
				struct max22915_current_config *config);

/** Set the short to VDD detection configuration for a channel */
int max22915_set_shvdd_config(struct max22915_desc *dev,
			      uint8_t channel,
			      bool shtvdd_enable);

/** Set the open-wire off detection configuration for a channel */
int max22915_set_owoff_config(struct max22915_desc *dev,
			      uint8_t channel,
			      bool owoff_enable);

/** Set the open-wire on detection configuration for a channel */
int max22915_set_owon_config(struct max22915_desc *dev,
			     uint8_t channel,
			     bool owon_enable);

/** Set the switch state of a channel */
int max22915_set_sw_state(struct max22915_desc *dev,
			  uint8_t channel,
			  enum max22915_sw_state state);

/** Free the resources allocated during init */
int max22915_remove(struct max22915_desc *desc);

#endif /* _MAX22915_H */
