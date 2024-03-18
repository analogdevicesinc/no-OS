/***************************************************************************//**
 *   @file   ltc2672.h
 *   @brief  Header file of ltc2672 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifndef __LTC2672_H__
#define __LTC2672_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* LTC2672 Masks */
#define LTC2672_16_DONT_CARE 	0xFFF0
#define LTC2672_MUX_DONT_CARE 	0xFFFE0
#define LTC2672_DUMMY		 	0xFFFF
#define LTC2672_FAULT_REG_MASK	0xFF0000

/* LTC2672 Constants */
#define LTC2672_BASE_CURRENT 			3.125 // base current in mA
#define LTC2672_VMINUS_FIXED_CURRENT 	-80	// Fixed V- Current as per data sheet
#define LTC2672_OFF_CURRENT 			0
#define LTC2672_300MA_CURRENT 			300
#define LTC2672_16BIT_RESO   			65535
#define LTC2672_12BIT_RESO   			4095
#define LTC2672_TOTAL_CHANNELS 			5
#define LTC2672_MAX_CONFIG_MASK  		15
#define LTC2672_MAX_TOGGLE_MASK  		31
#define LTC2672_BIT_SHIFT_12BIT			4
#define LTC2672_NUM_MUX_SELECTS			22
#define LTC2672_NUM_CURRENT_SPANS		10
#define LTC2672_NUM_FAULTS				7

enum ltc2672_commands {
	LTC2672_CODE_TO_CHANNEL_X,
	LTC2672_PWRUP_UPD_CHANNEL_X,
	LTC2672_CODE_TO_CHANNEL_X_PWRUP_UPD_CHANNEL_ALL,
	LTC2672_CODE_PWRUP_UPD_CHANNEL_X,
	LTC2672_PWRDWN_CHANNEL_X,
	LTC2672_PWRDWN_DEV,
	LTC2672_SPAN_TO_CHANNEL_X,
	LTC2672_CNFG_CMD,
	LTC2672_CODE_TO_CHANNEL_ALL,
	LTC2672_PWRUP_UPD_CHANNEL_ALL,
	LTC2672_CODE_PWRUP_UPD_CHANNEL_ALL,
	LTC2672_MON_MUX,
	LTC2672_TOGGLE_SEL,
	LTC2672_TOGGLE_GLBL,
	LTC2672_SPAN_TO_CHANNEL_ALL,
	LTC2672_NO_OP
};

/* LTC2672 Command Generation */
#define LTC2672_COMMAND32_GENERATE(comm, add, dat) \
		(0xFF << 24) | (comm << 20) | (add << 16) | (dat)

#define LTC2672_COMMAND24_GENERATE(comm, add, dat) \
		(comm << 20) | (add << 16) | (dat)

#define LTC2672_SPAN_SET(span_code) 	 LTC2672_16_DONT_CARE | span_code

#define LTC2672_MUX_SET(mux_code) 	 	 LTC2672_MUX_DONT_CARE | mux_code

#define LTC2672_MUX32_GENERATE(comm, dat) \
		(0xFF << 24) | (comm << 20) | (dat)

#define LTC2672_MUX24_GENERATE(comm, dat) \
		(comm << 20) | (dat)

/* Device Family */
enum ltc2672_device_id {
	LTC2672_12,
	LTC2672_16
};

/* DAC Channels */
enum ltc2672_dac_ch {
	LTC2672_DAC0,
	LTC2672_DAC1,
	LTC2672_DAC2,
	LTC2672_DAC3,
	LTC2672_DAC4
};

/* Output Range */
enum ltc2672_out_range {
	LTC2672_OFF, // Off mode
	LTC2672_50VREF, // 3.125mA
	LTC2672_100VREF, // 6.25mA
	LTC2672_200VREF, //12.5mA
	LTC2672_400VREF, // 25mA
	LTC2672_800VREF, // 50mA
	LTC2672_1600VREF, // 100mA
	LTC2672_3200VREF,  // 200mA
	LTC2672_VMINUS_VREF, // Fixed, -80mA
	LTC2672_4800VREF = 0XF // 300mA
};

/* Multiplexer Command Codes */
enum ltc2672_mux_commands {
	LTC2672_MUX_DISABLED,
	LTC2672_MUX_IOUT0,
	LTC2672_MUX_IOUT1,
	LTC2672_MUX_IOUT2,
	LTC2672_MUX_IOUT3,
	LTC2672_MUX_IOUT4,
	LTC2672_MUC_VCC,
	LTC2672_MUX_VREF = 0x08,
	LTC2672_MUX_VREF_LO,
	LTC2672_MUX_DIE_TEMP,
	LTC2672_MUX_VDD0 = 0x10,
	LTC2672_MUX_VDD1,
	LTC2672_MUX_VDD2,
	LTC2672_MUX_VDD3,
	LTC2672_MUX_VDD4,
	LTC2672_MUX_VMINUS = 0X16,
	LTC2672_MUX_GND,
	LTC2672_MUX_VOUT0,
	LTC2672_MUX_VOUT1,
	LTC2672_MUX_VOUT2,
	LTC2672_MUX_VOUT3,
	LTC2672_MUX_VOUT4
};

/* Faults */
enum ltc2672_faults {
	LTC2672_OPEN_OUT0, // Open circuit CH0
	LTC2672_OPEN_OUT1, // Open circuit CH0
	LTC2672_OPEN_OUT2, // Open circuit CH0
	LTC2672_OPEN_OUT3, // Open circuit CH0
	LTC2672_OPEN_OUT4, // Open circuit CH0
	LTC2672_OVER_TEMP, // Over-temperature (T > 175 deg C)
	LTC2672_UNUSED, // Unused fault register bit
	LTC2672_INV_LENGTH, // Invalid SPI Length (len != 24 or 32 * n)
};

/**
 * @struct ltc2672_dev
 * @brief Structure holding ltc2672 descriptor.
 */
struct ltc2672_dev {
	/* SPI descriptor */
	struct no_os_spi_desc *comm_desc;
	/* Device Variant indicator */
	enum ltc2672_device_id id;
	/* DAC Channel Spans */
	enum ltc2672_out_range out_spans[LTC2672_TOTAL_CHANNELS];
	/* Maximum Current Per Channel in uA */
	uint32_t max_currents[LTC2672_TOTAL_CHANNELS];
	/* Previous command tracker */
	uint32_t prev_command;
	/* Global toggle bit flag */
	bool global_toggle;
};

/**
 * @struct ltc2672_init_param
 * @brief Structure holding the parameters for ltc2672 initialization.
 */
struct ltc2672_init_param {
	/* SPI descriptor */
	struct no_os_spi_init_param spi_init;
	/* Device Variant indicator */
	enum ltc2672_device_id id;
};

/** Device and communication init function */
int ltc2672_init(struct ltc2672_dev **, struct ltc2672_init_param *);

/** Free resources allocated by the init function */
int ltc2672_remove(struct ltc2672_dev *);

/** Configure LTC2672 and get response */
int ltc2672_transaction(struct ltc2672_dev *device, uint32_t, bool);

/** Convert current into dac code */
uint32_t ltc2672_current_to_code(struct ltc2672_dev *device, uint32_t,
				 enum ltc2672_dac_ch);

/** Set the dac code for a selected DAC channel */
int ltc2672_set_code_channel(struct ltc2672_dev *device, uint16_t code,
			     enum ltc2672_dac_ch out_ch);

/** Set the current for a selected DAC channel */
int ltc2672_set_current_channel(struct ltc2672_dev *, uint32_t,
				enum ltc2672_dac_ch);

/** Set the dac code for all DAC channels */
int ltc2672_set_code_all_channels(struct ltc2672_dev *device, uint16_t code);

/** Set the current for all DAC channels */
int ltc2672_set_current_all_channels(struct ltc2672_dev *, uint32_t);

/** Set the output span for a selected DAC channel */
int ltc2672_set_span_channel(struct ltc2672_dev *, enum ltc2672_out_range,
			     enum ltc2672_dac_ch);

/** Set the output span for all DAC channels */
int ltc2672_set_span_all_channels(struct ltc2672_dev *, enum ltc2672_out_range);

/** Power down the LTC2672-XX */
int ltc2672_chip_power_down(struct ltc2672_dev *);

/** Power down a selected DAC channel */
int ltc2672_power_down_channel(struct ltc2672_dev *, enum ltc2672_dac_ch);

/** Power down all DAC channels */
int ltc2672_power_down_all_channels(struct ltc2672_dev *);

/** Configure the LTC2672-XX MUX pin output for selected measurement */
int ltc2672_monitor_mux(struct ltc2672_dev *, enum ltc2672_mux_commands);

/** Setup selected channel's reg A and B */
int ltc2672_setup_toggle_channel(struct ltc2672_dev *, enum ltc2672_dac_ch,
				 uint32_t, uint32_t);

/** Enable channel/s to toggle */
int ltc2672_enable_toggle_channel(struct ltc2672_dev *, uint32_t);

/** Sets or resets the global toggle bit */
int ltc2672_global_toggle(struct ltc2672_dev *, bool);

/** Configures the fault/s to be detected */
int ltc2672_config_command(struct ltc2672_dev *, uint8_t);

#endif // __LTC2672_H__
