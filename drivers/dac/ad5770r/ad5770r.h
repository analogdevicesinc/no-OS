/***************************************************************************//**
 *   @file   ad5770r.h
 *   @brief  Header file for AD5770R Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
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

#ifndef AD5770R_H_
#define AD5770R_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifndef BIT
#define NO_OS_BIT(x)					(1UL << (x))
#endif
/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h.
 */
#ifdef GENMASK
#define NO_OS_GENMASK(h, l) \
		(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (31 - (h))))
#endif

/*SPI configuration registers*/
#define AD5770R_INTERFACE_CONFIG_A 	0x00
#define AD5770R_INTERFACE_CONFIG_B 	0x01
#define AD5770R_CHIP_TYPE		0x03
#define AD5770R_PRODUCT_ID_L		0x04
#define AD5770R_PRODUCT_ID_H		0x05
#define AD5770R_CHIP_GRADE		0x06
#define AD5770R_SCRATCH_PAD		0x0A
#define AD5770R_SPI_REVISION		0x0B
#define AD5770R_VENDOR_L		0x0C
#define AD5770R_VENDOR_H		0x0D
#define AD5770R_STREAM_MODE		0x0E
#define AD5770R_INTERFACE_CONFIG_C	0x10
#define AD5770R_INTERFACE_STATUS_A	0x11

/*AD5770R configuration registers*/
#define AD5770R_CHANNEL_CONFIG		0x14
#define AD5770R_OUTPUT_RANGE_CH0	0x15
#define AD5770R_OUTPUT_RANGE_CH1	0x16
#define AD5770R_OUTPUT_RANGE_CH2	0x17
#define AD5770R_OUTPUT_RANGE_CH3	0x18
#define AD5770R_OUTPUT_RANGE_CH4	0x19
#define AD5770R_OUTPUT_RANGE_CH5	0x1A
#define AD5770R_REFERENCE		0x1B
#define AD5770R_ALARM_CONFIG		0x1C
#define AD5770R_OUTPUT_FILTER_CH0	0x1D
#define AD5770R_OUTPUT_FILTER_CH1	0x1E
#define AD5770R_OUTPUT_FILTER_CH2	0x1F
#define AD5770R_OUTPUT_FILTER_CH3	0x20
#define AD5770R_OUTPUT_FILTER_CH4	0x21
#define AD5770R_OUTPUT_FILTER_CH5	0x22
#define AD5770R_MONITOR_SETUP		0x23
#define AD5770R_STATUS			0x24
#define AD5770R_HW_LDAC			0x25
#define AD5770R_CH0_DAC_LSB		0x26
#define AD5770R_CH0_DAC_MSB		0x27
#define AD5770R_CH1_DAC_LSB		0x28
#define AD5770R_CH1_DAC_MSB		0x29
#define AD5770R_CH2_DAC_LSB		0x2A
#define AD5770R_CH2_DAC_MSB		0x2B
#define AD5770R_CH3_DAC_LSB		0x2C
#define AD5770R_CH3_DAC_MSB		0x2D
#define AD5770R_CH4_DAC_LSB		0x2E
#define AD5770R_CH4_DAC_MSB		0x2F
#define AD5770R_CH5_DAC_LSB		0x30
#define AD5770R_CH5_DAC_MSB		0x31
#define AD5770R_DAC_PAGE_MASK_LSB	0x32
#define AD5770R_DAC_PAGE_MASK_MSB	0x33
#define AD5770R_CH_SELECT		0x34
#define AD5770R_INPUT_PAGE_MASK_LSB	0x35
#define AD5770R_INPUT_PAGE_MASK_MSB	0x36
#define AD5770R_SW_LDAC			0x37
#define AD5770R_CH0_INPUT_LSB		0x38
#define AD5770R_CH0_INPUT_MSB		0x39
#define AD5770R_CH1_INPUT_LSB		0x3A
#define AD5770R_CH1_INPUT_MSB		0x3B
#define AD5770R_CH2_INPUT_LSB		0x3C
#define AD5770R_CH2_INPUT_MSB		0x3D
#define AD5770R_CH3_INPUT_LSB		0x3E
#define AD5770R_CH3_INPUT_MSB		0x3F
#define AD5770R_CH4_INPUT_LSB		0x40
#define AD5770R_CH4_INPUT_MSB		0x41
#define AD5770R_CH5_INPUT_LSB		0x42
#define AD5770R_CH5_INPUT_MSB		0x43

/* AD5770R_INTERFACE_CONFIG_A */
#define AD5770R_INTERFACE_CONFIG_A_SW_RESET_MSK			NO_OS_BIT(7) | NO_OS_BIT(0)
#define AD5770R_INTERFACE_CONFIG_A_SW_RESET(x)			(((x) & 0x1) | 0x80)
#define AD5770R_INTERFACE_CONFIG_A_ADDR_ASCENSION_MSB_MSK	NO_OS_BIT(5)
#define AD5770R_INTERFACE_CONFIG_A_ADDR_ASCENSION_MSB(x)	(((x) & 0x1) << 5)
#define AD5770R_INTERFACE_CONFIG_A_SDO_ACTIVE_MSK		NO_OS_BIT(4) | NO_OS_BIT(3)

/* AD5770R_INTERFACE_CONFIG_B */
#define AD5770R_INTERFACE_CONFIG_B_SINGLE_INST_MSK		NO_OS_BIT(7)
#define AD5770R_INTERFACE_CONFIG_B_SINGLE_INST(x)		(((x) & 0x1) << 7)
#define AD5770R_INTERFACE_CONFIG_B_SHORT_INST_MSK		NO_OS_BIT(3)
#define AD5770R_INTERFACE_CONFIG_B_SHORT_INST(x)		(((x) & 0x1) << 3)

/* AD5770R_INTERFACE_CONFIG_C */
#define AD5770R_INTERFACE_CONFIG_C_STRUCT_REGISTER_ACCESS_MSK	NO_OS_BIT(5)
#define AD5770R_INTERFACE_CONFIG_C_STRUCT_REGISTER_ACCESS(x)	(((x) & 0x1) << 5)

/* AD5770R_CHANNEL_CONFIG */
#define AD5770R_CHANNEL_CONFIG_CH0_SINK_EN_MSK			NO_OS_BIT(7)
#define AD5770R_CHANNEL_CONFIG_CH0_SINK_EN(x)			(((x) & 0x1) << 7)
#define AD5770R_CHANNEL_CONFIG_CH5_SHUTDOWN_B_MSK		NO_OS_BIT(5)
#define AD5770R_CHANNEL_CONFIG_CH5_SHUTDOWN_B(x)		(((x) & 0x1) << 5)
#define AD5770R_CHANNEL_CONFIG_CH4_SHUTDOWN_B_MSK		NO_OS_BIT(4)
#define AD5770R_CHANNEL_CONFIG_CH4_SHUTDOWN_B(x)		(((x) & 0x1) << 4)
#define AD5770R_CHANNEL_CONFIG_CH3_SHUTDOWN_B_MSK		NO_OS_BIT(3)
#define AD5770R_CHANNEL_CONFIG_CH3_SHUTDOWN_B(x)		(((x) & 0x1) << 3)
#define AD5770R_CHANNEL_CONFIG_CH2_SHUTDOWN_B_MSK		NO_OS_BIT(2)
#define AD5770R_CHANNEL_CONFIG_CH2_SHUTDOWN_B(x)		(((x) & 0x1) << 2)
#define AD5770R_CHANNEL_CONFIG_CH1_SHUTDOWN_B_MSK		NO_OS_BIT(1)
#define AD5770R_CHANNEL_CONFIG_CH1_SHUTDOWN_B(x)		(((x) & 0x1) << 1)
#define AD5770R_CHANNEL_CONFIG_CH0_SHUTDOWN_B_MSK		NO_OS_BIT(0)
#define AD5770R_CHANNEL_CONFIG_CH0_SHUTDOWN_B(x)		(((x) & 0x1) << 0)

/* AD5770R_OUTPUT_RANGE */
#define AD5770R_OUTPUT_RANGE_OUTPUT_SCALING_MSK			NO_OS_GENMASK(7, 2)
#define AD5770R_OUTPUT_RANGE_OUTPUT_SCALING(x)			(((x) & 0x3F) << 2)
#define AD5770R_OUTPUT_RANGE_MODE_MSK				NO_OS_GENMASK(1, 0)
#define AD5770R_OUTPUT_RANGE_MODE(x)				((x) & 0x03)

/* AD5770R_REFERENCE */
#define AD5770R_REFERENCE_RESISTOR_SEL_MSK			NO_OS_BIT(2)
#define AD5770R_REFERENCE_RESISTOR_SEL(x)			(((x) & 0x1) << 2)
#define AD5770R_REFERENCE_VOLTATE_SEL_MSK			NO_OS_GENMASK(1, 0)
#define AD5770R_REFERENCE_VOLTATE_SEL(x)			(((x) & 0x3) << 0)

/* AD5770R_ALARM_CONFIG */
#define AD5770R_ALARM_CONFIG_BACKGROUND_CRC_ALARM_MASK(x)	(((x) & 0x1) << 7)
#define AD5770R_ALARM_CONFIG_IREF_FAULT_ALARM_MASK(x)		(((x) & 0x1) << 6)
#define AD5770R_ALARM_CONFIG_NEGATIVE_CHANNEL0_ALARM_MASK(x)	(((x) & 0x1) << 5)
#define AD5770R_ALARM_CONFIG_OVER_TEMP_ALARM_MASK(x)		(((x) & 0x1) << 4)
#define AD5770R_ALARM_CONFIG_TEMP_WARNING_ALARM_MASK(x)		(((x) & 0x1) << 3)
#define AD5770R_ALARM_CONFIG_BACKGROUND_CRC_EN(x)		(((x) & 0x1) << 2)
#define AD5770R_ALARM_CONFIG_THERMAL_SHUTDOWN_EN(x)		(((x) & 0x1) << 1)
#define AD5770R_ALARM_CONFIG_OPEN_DRAIN_EN(x)			(((x) & 0x1) << 0)

/* AD5770R_OUTPUT_FILTER_CH */
#define AD5770R_OUTPUT_FILTER_CH_MSK				NO_OS_GENMASK(3, 0)
#define AD5770R_OUTPUT_FILTER_CH(x)				(((x) & 0xF) << 0)

/* AD5770R_MONITOR_SETUP */
#define AD5770R_MONITOR_SETUP_MON_FUNCTION_MSK			NO_OS_GENMASK(7, 6)
#define AD5770R_MONITOR_SETUP_MON_FUNCTION(x)			(((x) & 0x3) << 6)
#define AD5770R_MONITOR_SETUP_MUX_BUFFER_MSK			NO_OS_BIT(5)
#define AD5770R_MONITOR_SETUP_MUX_BUFFER(x)			(((x) & 0x1) << 5)
#define AD5770R_MONITOR_SETUP_IB_EXT_EN_MSK			NO_OS_BIT(4)
#define AD5770R_MONITOR_SETUP_IB_EXT_EN(x)			(((x) & 0x1) << 4)
#define AD5770R_MONITOR_SETUP_MON_CH_MSK			NO_OS_GENMASK(3, 0)
#define AD5770R_MONITOR_SETUP_MON_CH(x)				(((x) & 0x7) << 0)

/* AD5770R_STATUS */
#define AD5770R_STATUS_BACKGROUND_CRC_STATUS_MSK		NO_OS_BIT(7)
#define AD5770R_STATUS_IREF_FAULT_MSK				NO_OS_BIT(3)
#define AD5770R_STATUS_NEGATIVE_CHANNEL0_MSK			NO_OS_BIT(2)
#define AD5770R_STATUS_OVER_TEMP_MSK				NO_OS_BIT(1)
#define AD5770R_STATUS_TEMP_WARNING_MSK				NO_OS_BIT(0)

/* AD5770R_HW_LDAC */
#define AD5770R_HW_LDAC_MASK_CH(x, channel)			(((x) & 0x1) << (channel))

/* AD5770R_CH_DAC */
#define AD5770R_CH_DAC_DATA_LSB(x)				(((x) & 0x3F) << 2)
#define AD5770R_CH_DAC_DATA_MSB(x)				(((x) & 0xFF) << 0)

/* AD5770R_CH_SELECT */
#define AD5770R_CH_SELECT_SEL_CH(x, channel)			(((x) & 0x1) << (channel))

/* AD5770R_CH_INPUT */
#define AD5770R_CH_DAC_INPUT_DATA_LSB(x)			(((x) & 0x3F) << 2)
#define AD5770R_CH_DAC_INPUT_DATA_MSB(x)			(((x) & 0xFF) << 0)

/* AD5770R_SW_LDAC */
#define AD5770R_SW_LDAC_CH(x, channel)				(((x) & 0x1) << (channel))


#define AD5770R_REG_READ(x)					(((x) & 0x7F) | 0x80)
#define AD5770R_REG_WRITE(x)					((x) & 0x7F)

enum ad5770r_output_filter_resistor {
	AD5770R_OUTPUT_FILTER_RESISTOR_60_OHM = 0x0,
	AD5770R_OUTPUT_FILTER_RESISTOR_5_6_KOHM = 0x5,
	AD5770R_OUTPUT_FILTER_RESISTOR_11_2_KOHM,
	AD5770R_OUTPUT_FILTER_RESISTOR_22_2_KOHM,
	AD5770R_OUTPUT_FILTER_RESISTOR_44_4_KOHM,
	AD5770R_OUTPUT_FILTER_RESISTOR_104_KOHM,
};

enum ad5770r_monitor_function {
	AD5770R_DISABLE = 0,
	AD5770R_VOLTAGE_MONITORING,
	AD5770R_CURRENT_MONITORING,
	AD5770R_TEMPERATURE_MONITORING
};

enum ad5770r_channels {
	AD5770R_CH0 = 0,
	AD5770R_CH1,
	AD5770R_CH2,
	AD5770R_CH3,
	AD5770R_CH4,
	AD5770R_CH5
};

enum ad5770r_reference_voltage {
	AD5770R_EXT_REF_2_5_V = 0,
	AD5770R_INT_REF_1_25_V_OUT_ON,
	AD5770R_EXT_REF_1_25_V,
	AD5770R_INT_REF_1_25_V_OUT_OFF
};

struct ad5770r_monitor_setup {
	enum ad5770r_monitor_function		monitor_function;
	bool 					mux_buffer;
	bool					ib_ext_en;
	enum ad5770r_channels			monitor_channel;
};

struct ad5770r_dac_page_mask {
	uint16_t 		dac_data_page_mask;
	uint16_t		input_page_mask;
};

struct ad5770r_output_range {
	uint8_t		output_scale;
	uint8_t		output_range_mode;
};

struct ad5770r_device_spi_settings {
	bool			addr_ascension;
	bool			single_instruction; // for multibyte read/write
	uint8_t		stream_mode_length;
};

struct ad5770r_channel_switches {
	bool en0, en1, en2, en3, en4, en5, sink0;
};

struct ad5770r_alarm_cfg {
	bool open_drain_en;
	bool thermal_shutdown_en;
	bool background_crc_en;
	bool temp_warning_msk;
	bool over_temp_msk;
	bool neg_ch0_msk;
	bool iref_fault_msk;
	bool background_crc_msk;
};

struct ad5770r_dev {
	/* SPI */
	struct no_os_spi_desc			*spi_desc;
	/* GPIO */
	/** note: the GPIOs are optional */
	struct no_os_gpio_desc			*gpio_alarm_n;
	struct no_os_gpio_desc			*gpio_reset_n;
	struct no_os_gpio_desc			*gpio_ldac_n;
	/* Device SPI Settings */
	struct ad5770r_device_spi_settings	dev_spi_settings;
	/* Device Settings */
	struct ad5770r_channel_switches		channel_config;
	struct ad5770r_output_range		output_mode[6];
	bool					external_reference;
	enum ad5770r_reference_voltage		reference_selector;
	struct ad5770r_alarm_cfg		alarm_config;
	enum ad5770r_output_filter_resistor	output_filter[6];
	struct ad5770r_monitor_setup		mon_setup;
	struct ad5770r_channel_switches		mask_hw_ldac;
	uint16_t				dac_value[6];
	struct ad5770r_dac_page_mask		page_mask;
	struct ad5770r_channel_switches		mask_channel_sel;
	struct ad5770r_channel_switches		sw_ldac;
	uint16_t				input_value[6];
};

struct ad5770r_init_param {
	/* SPI */
	struct no_os_spi_init_param			spi_init;
	/* GPIO */
	struct no_os_gpio_init_param			*gpio_alarm_n;
	struct no_os_gpio_init_param			*gpio_reset_n;
	struct no_os_gpio_init_param			*gpio_ldac_n;
	/* Device SPI Settings */
	struct ad5770r_device_spi_settings	dev_spi_settings;
	/* Device Settings */
	struct ad5770r_channel_switches		channel_config;
	struct ad5770r_output_range		output_mode[6];
	bool					external_reference;
	enum ad5770r_reference_voltage		reference_selector;
	struct ad5770r_alarm_cfg		alarm_config;
	enum ad5770r_output_filter_resistor	output_filter[6];
	struct ad5770r_monitor_setup		mon_setup;
	struct ad5770r_channel_switches		mask_hw_ldac;
	uint16_t				dac_value[6];
	struct ad5770r_dac_page_mask		page_mask;
	struct ad5770r_channel_switches		mask_channel_sel;
	struct ad5770r_channel_switches		sw_ldac;
	uint16_t				input_value[6];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad5770r_spi_reg_read(struct ad5770r_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t ad5770r_spi_reg_read_multiple(struct ad5770r_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
int32_t ad5770r_spi_reg_write(struct ad5770r_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t ad5770r_spi_reg_write_multiple(struct ad5770r_dev *dev,
				       uint8_t reg_addr,
				       uint8_t *reg_data,
				       uint16_t count);
int32_t ad5770r_spi_write_mask(struct ad5770r_dev *dev,
			       uint8_t reg_addr,
			       uint32_t mask,
			       uint8_t data);
int32_t ad5770r_set_device_spi(struct ad5770r_dev *dev,
			       const struct ad5770r_device_spi_settings *spi_settings);
int32_t ad5770r_channel_config(struct ad5770r_dev *dev,
			       const struct ad5770r_channel_switches *channel_config);
int32_t ad5770r_set_output_mode(struct ad5770r_dev *dev,
				const struct ad5770r_output_range *output_mode,
				enum ad5770r_channels channel);
int32_t ad5770r_set_reference(struct ad5770r_dev *dev,
			      bool external_reference,
			      enum ad5770r_reference_voltage reference_selector);
int32_t ad5770r_set_alarm(struct ad5770r_dev *dev,
			  const struct ad5770r_alarm_cfg *const alarm_config);
int32_t ad5770r_set_output_filter(struct ad5770r_dev *dev,
				  enum ad5770r_output_filter_resistor output_filter,
				  enum ad5770r_channels channel);
int32_t ad5770r_set_hw_ldac(struct ad5770r_dev *dev,
			    const struct ad5770r_channel_switches *mask_hw_ldac);
int32_t ad5770r_set_dac_value(struct ad5770r_dev *dev,
			      uint16_t dac_value, enum ad5770r_channels channel);
int32_t ad5770r_set_dac_input(struct ad5770r_dev *dev,
			      uint16_t dac_input, enum ad5770r_channels channel);
int32_t ad5770r_set_page_mask(struct ad5770r_dev *dev,
			      const struct ad5770r_dac_page_mask *page_mask);
int32_t ad5770r_set_mask_channel(struct ad5770r_dev *dev,
				 const struct ad5770r_channel_switches *mask_channel_sel);
int32_t ad5770r_set_sw_ldac(struct ad5770r_dev *dev,
			    const struct ad5770r_channel_switches *sw_ldac);
int32_t ad5770r_get_status(struct ad5770r_dev *dev,
			   uint8_t *status);
int32_t ad5770r_get_interface_status(struct ad5770r_dev *dev,
				     uint8_t *status);
int32_t ad5770r_set_monitor_setup(struct ad5770r_dev *dev,
				  const struct ad5770r_monitor_setup *mon_setup);
int32_t ad5770r_init(struct ad5770r_dev **device,
		     const struct ad5770r_init_param *init_param);
int32_t ad5770r_remove(struct ad5770r_dev *dev);

#endif /* AD5770R_H_ */
