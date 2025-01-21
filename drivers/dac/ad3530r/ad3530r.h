/***************************************************************************//**
 *   @file   ad3530r.c
 *   @author Sai Kiran Gudla (Saikiran.Gudla@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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

#ifndef _AD3530R_H_
#define _AD3530R_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_crc8.h"

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/


#define AD3530R_R1B          (1ul << 16)
#define AD3530R_R2B          (2ul << 16)
#define AD3530R_LEN(x)       (x >> 16)
#define AD3530R_ADDR(x)      (x & 0xFFFF)

#define AD3530R_MAX_REG_SIZE					2
#define AD3530R_MAX_CHANNEL_OP_MODE_0			4
#define AD3530R_MAX_SHORT_REG_ADDR				(AD3530R_R1B | 0x80)
#define AD3530R_READ_BIT						NO_OS_BIT(7)
#define AD3530R_ADDR_MASK						(~AD3530R_READ_BIT)
#define AD3530R_WRITE_BIT_LONG_INSTR			0x00
#define AD3530R_EXTERNAL_VREF_MASK				NO_OS_BIT(1)
#define AD3530R_DEFAULT_STATUS_REG_VAL			0x04
#define AD3530R_SCRATCH_PAD_TEST_VAL			0x34
#define AD3530R_CRC_POLY						0x07
#define AD3530R_CRC_SEED						0xA5
#define AD3530R_REG_ADDR_OPERATING_MODE_CHN(x) (AD3530R_R1B | (0x20 + x/4))

/* Register addresses */
/* Primary address space */
#define AD3530R_REG_ADDR_INTERFACE_CONFIG_A		(AD3530R_R1B | 0x00)
#define AD3530R_REG_ADDR_INTERFACE_CONFIG_B		(AD3530R_R1B | 0x01)
#define AD3530R_REG_ADDR_DEVICE_CONFIG			(AD3530R_R1B | 0x02)
#define AD3530R_REG_ADDR_CHIP_TYPE			    (AD3530R_R1B | 0x03)
#define AD3530R_REG_ADDR_PRODUCT_ID_L			(AD3530R_R1B | 0x04)
#define AD3530R_REG_ADDR_PRODUCT_ID_H			(AD3530R_R1B | 0x05)
#define AD3530R_REG_ADDR_CHIP_GRADE             (AD3530R_R1B | 0x06)
#define AD3530R_REG_ADDR_SCRATCH_PAD			(AD3530R_R1B | 0x0A)
#define AD3530R_REG_ADDR_SPI_REVISION			(AD3530R_R1B | 0x0B)
#define AD3530R_REG_ADDR_VENDOR_L               (AD3530R_R1B | 0x0C)
#define AD3530R_REG_ADDR_VENDOR_H               (AD3530R_R1B | 0x0D)
#define AD3530R_REG_ADDR_STREAM_MODE			(AD3530R_R1B | 0x0E)
#define AD3530R_REG_ADDR_TRANSFER_REGISTER		(AD3530R_R1B | 0x0F)
#define AD3530R_REG_ADDR_INTERFACE_CONFIG_C		(AD3530R_R1B | 0x10)
#define AD3530R_REG_ADDR_INTERFACE_STATUS_A		(AD3530R_R1B | 0x11)
#define AD3530R_REG_ADDR_OPERATING_MODE_0       (AD3530R_R1B | 0x20)
#define AD3530R_REG_ADDR_OPERATING_MODE_1       (AD3530R_R1B | 0x21)
#define AD3530R_REG_ADDR_OUTPUT_CONTROL_0       (AD3530R_R1B | 0x2A)
#define AD3530R_REG_ADDR_REF_CONTROL_0          (AD3530R_R1B | 0x3C)
#define AD3530R_REG_ADDR_MUX_OUT_SELECT         (AD3530R_R1B | 0x93)
#define AD3530R_REG_ADDR_STATUS_CONTROL         (AD3530R_R1B | 0xC2)

/* DAC configuration registers */
#define AD3530R_REG_ADDR_HW_LDAC_EN_0           (AD3530R_R1B | 0xD0)
#define AD3530R_REG_ADDR_SW_LDAC_EN_0           (AD3530R_R1B | 0xD1)
#define AD3530R_REG_ADDR_DAC_CHN(x)             (AD3530R_R2B | (0xD2 + (x * 2)))
#define AD3530R_REG_ADDR_MULTI_DAC_CH           (AD3530R_R2B | 0XE2)
#define AD3530R_REG_ADDR_MULTI_DAC_SEL_0        (AD3530R_R1B | 0XE4)
#define AD3530R_REG_ADDR_SW_LDAC_TRIG_A         (AD3530R_R1B | 0XE5)
#define AD3530R_REG_ADDR_MULTI_INPUT_CH         (AD3530R_R2B | 0XE6)
#define AD3530R_REG_ADDR_MULTI_INPUT_SEL_0      (AD3530R_R1B | 0XE8)
#define AD3530R_REG_ADDR_SW_LDAC_TRIG_B         (AD3530R_R1B | 0XE9)
#define AD3530R_REG_ADDR_INPUT_CHN(x)           (AD3530R_R2B | (0xEA + (x * 2)))

/* Register masks */
/* AD3530R_REG_ADDR_INTERFACE_CONFIG_A bit masks */
#define AD3530R_MASK_SOFTWARE_RESET             (NO_OS_BIT(7) | NO_OS_BIT(0))
#define AD3530R_MASK_ADDR_ASCENSION			    NO_OS_BIT(5)
#define AD3530R_MASK_SDO_ACTIVE			        NO_OS_BIT(4)

/* AD3530R_REG_ADDR_INTERFACE_CONFIG_B bit masks */
#define AD3530R_MASK_SINGLE_INST			    NO_OS_BIT(7)
#define AD3530R_MASK_SHORT_INSTRUCTION		    NO_OS_BIT(3)

/* AD3530R_REG_ADDR_DEVICE_CONFIG bit masks */
#define AD3530R_MASK_OPERATING_MODES			NO_OS_GENMASK(1, 0)

/* AD3530R_REG_ADDR_CHIP_GRADE bit masks */
#define AD3530R_MASK_GRADE				        NO_OS_GENMASK(7, 4)
#define AD3530R_MASK_DEVICE_REVISION			NO_OS_GENMASK(3, 0)

/* AD3530R_REG_ADDR_STREAM_MODE bit masks */
#define AD3530R_MASK_LENGTH				        0xFF

/* AD3530R_REG_ADDR_TRANSFER_REGISTER bit masks */
#define AD3530R_MASK_STREAM_LENGTH_KEEP_VALUE   NO_OS_BIT(2)

/* AD3530R_REG_ADDR_INTERFACE_CONFIG_C bit masks */
#define AD3530R_MASK_CRC_ENABLE			        (NO_OS_GENMASK(7, 6) | NO_OS_GENMASK(1, 0))
#define AD3530R_MASK_STRICT_REGISTER_ACCESS		NO_OS_BIT(5)
#define AD3530R_MASK_ACTIVE_INTERFACE_MODE		NO_OS_GENMASK(3, 2)

/* AD3530R_REG_ADDR_STREAM_MODE bit masks */
#define AD3530R_MASK_INTERFACE_NOT_READY		NO_OS_BIT(7)
#define AD3530R_MASK_CLOCK_COUNTING_ERROR		NO_OS_BIT(4)
#define AD3530R_MASK_INVALID_OR_NO_CRC          NO_OS_BIT(3)
#define AD3530R_MASK_PARTIAL_REGISTER_ACCESS    NO_OS_BIT(1)

/* AD3530R_REG_ADDR_OPERATING_MODE bit masks */
#define AD3530R_MASK_OPERATING_MODE(x)          0x03 << ((x % 4)*2)

/* AD3530R_REG_ADDR_OUTPUT_CONTROL_0 bit masks */
#define AD3530R_MASK_OUTPUT_RANGE               NO_OS_BIT(2)

/* AD3530R_REG_ADDR_REF_CONTROL_0 bit masks */
#define AD3530R_MASK_REERENCE_SELECT            NO_OS_BIT(0)

/* AD3530R_REG_ADDR_MUX_OUT_SELECT bit masks */
#define AD3530R_MASK_MUX_SELECT                 NO_OS_GENMASK(4, 0)

/* AD3530R_REG_ADDR_HW_LDAC_EN_0 bit masks */
#define AD3530R_MASK_HW_LDAC_EN_0(x)            NO_OS_BIT(x)

/* AD3530R_REG_ADDR_SW_LDAC_EN_0 bit masks */
#define AD3530R_MASK_SW_LDAC_EN_0(x)            NO_OS_BIT(x)

/* AD3530R_REG_ADDR_SW_LDAC_TRIG_B bit masks */
#define AD3530R_MASK_SW_LDAC_TRIG_B             NO_OS_BIT(7)

/* Useful defines */
#define AD3530R_REG_ADDR_MAX	                0xF9
#define AD3530R_NUM_CH					        8
#define AD3530R_MASK_CH(ch)				        NO_OS_BIT(ch)
#define AD3530R_LDAC_PULSE_US                   1
#define AD3530R_CH_DAC_DATA_LSB(x)				((x) & 0xFF)
#define AD3530R_CH_DAC_DATA_MSB(x)				((x) >> 8 & 0xFF)
#define AD3530R_CRC_ENABLE_VALUE			    (NO_OS_BIT(6) | NO_OS_BIT(1))
#define AD3530R_CRC_DISABLE_VALUE			    (NO_OS_BIT(1) | NO_OS_BIT(0))
#define AD3530R_NUM_MUX_OUT_SELECTS				27
#define AD3530R_NUM_REGS			45  // Number of valid registers (mb regs considered a single entity)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum ad3530r_id
 * @brief Device IDs
 */
enum ad3530r_id {
	AD3530R_ID
};

/**
 * @enum ad3530r_ch_vref_select
 * @brief Channel voltage reference options
 */
enum ad3530r_ch_vref_select {
	/* External source with Vref I/O as input */
	AD3530R_EXTERNAL_VREF_PIN_INPUT,
	/* Internal source with Vref I/O at 2.5V */
	AD3530R_INTERNAL_VREF_PIN_2P5V,
};

enum ad3530r_status {
	/* Status bits */
	AD3530R_DEVICE_NOT_READY = 0x0001,
	AD3530R_INTERFACE_NOT_READY = 0x0002,
	AD3530R_RESET_STATUS = 0x0004,
	AD3530R_DAC_UPDATE_STATUS = 0x0008,

	/* Errors */
	AD3530R_PARTIAL_REGISTER_ACCESS = 0x0002,
	AD3530R_INVALID_OR_NO_CRC = 0x0008,
	AD3530R_CLOCK_COUNTING_ERROR = 0x0010,
	AD3530R_DEVICE_NOT_READY_ERR = 0x0080,
};

/**
 * @enum ad3530r_ch_output_range
 * @brief Channel output range options
 */
enum ad3530r_ch_output_range {
	/* Range from 0 V to VREF */
	AD3530R_CH_OUTPUT_RANGE_0_VREF,
	/* Range from 0 V to 2*VREF */
	AD3530R_CH_OUTPUT_RANGE_0_2VREF,
};

/**
 * @enum ad3530r_operating_mode
 * @brief Operating modes
 */
enum ad3530r_operating_mode {
	AD3530R_CH_OPERATING_MODE_0,
	AD3530R_CH_OPERATING_MODE_1,
	AD3530R_CH_OPERATING_MODE_2,
	AD3530R_CH_OPERATING_MODE_3,
};

/**
 * @enum ad3530r_write_mode
 * @brief Write modes
 */
enum ad3530r_write_mode {
	/* Write to DAC registers. No need to trigger LDAC */
	AD3530R_WRITE_DAC_REGS,
	/* Write to input registers. User needs to trigger LDAC */
	AD3530R_WRITE_INPUT_REGS,
	/* Write to input registers. LDAC is triggered by the driver */
	AD3530R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC
};

/**
 * @enum ad3530r_mux_out_select
 * @brief Signals to monitor on MUX_OUT pin
 */
enum ad3530r_mux_out_select {
	POWERED_DOWN,
	VOUT0,
	IOUT0_SOURCE_MODE,
	IOUT0_SINK_MODE,
	VOUT1,
	IOUT1_SOURCE_MODE,
	IOUT1_SINK_MODE,
	VOUT2,
	IOUT2_SOURCE_MODE,
	IOUT2_SINK_MODE,
	VOUT3,
	IOUT3_SOURCE_MODE,
	IOUT3_SINK_MODE,
	VOUT4,
	IOUT4_SOURCE_MODE,
	IOUT4_SINK_MODE,
	VOUT5,
	IOUT5_SOURCE_MODE,
	IOUT5_SINK_MODE,
	VOUT6,
	IOUT6_SOURCE_MODE,
	IOUT6_SINK_MODE,
	VOUT7,
	IOUT7_SOURCE_MODE,
	IOUT7_SINK_MODE,
	DIE_TEMPERATURE,
	TIED_TO_AGND_INTERNALLY
};

/* By default all values are set to 0 */
struct ad3530r_transfer_config {
	/* Defines the length of the loop when streaming data */
	uint8_t		stream_mode_length;
	/* Determines Sequential Addressing Behavior */
	uint8_t		addr_asc;
	/* Select Streaming or Single Instruction Mode */
	uint8_t		single_instr;
	/* Determines the length of the address in the instruction phase */
	uint8_t		short_instr;
	/*
	 * Set this bit to prevent the STREAM_MODE LENGTH value from
	 * automatically resetting to zero
	 */
	uint8_t		stream_length_keep_value;
};

struct ad3530r_transfer_data {
	/* Starting address for transfer */
	uint16_t		addr;
	/* Data to transfer */
	uint8_t		*data;
	/* Size of data to transfer */
	uint32_t	len;
	/* Read transaction if true, write transfer otherwise */
	uint8_t		is_read;
	/* If NULL will be default or last configured will be used */
	struct ad3530r_transfer_config *spi_cfg;
};


struct ad3530r_desc {
	enum ad3530r_id	chip_id;
	struct ad3530r_transfer_config spi_cfg;
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *ldac;
	struct no_os_gpio_desc *reset;
	enum ad3530r_ch_vref_select vref_enable;
	enum ad3530r_operating_mode chn_op_mode[AD3530R_NUM_CH];
	enum ad3530r_ch_output_range range;
	uint16_t hw_ldac_mask;
	uint16_t sw_ldac_mask;
	uint8_t crc_en;
	uint8_t crc_table[NO_OS_CRC8_TABLE_SIZE];
	enum ad3530r_mux_out_select mux_out_sel;
};

struct ad3530r_init_param {
	enum ad3530r_id	chip_id;
	struct ad3530r_transfer_config spi_cfg;
	struct no_os_spi_init_param *spi_param;
	/* If set, reset is done with RESET pin, otherwise it will be soft */
	struct no_os_gpio_init_param *reset_gpio_param_optional;
	/* If set, input register are used and LDAC pulse is sent */
	struct no_os_gpio_init_param *ldac_gpio_param_optional;
	/* If set, uses internal reference and outputs internal Vref on Vref pin */
	enum ad3530r_ch_vref_select vref_enable;
	enum ad3530r_operating_mode chn_op_mode[AD3530R_NUM_CH];
	enum ad3530r_ch_output_range range;
	uint16_t hw_ldac_mask;
	uint16_t sw_ldac_mask;
	/* Set to enable CRC */
	uint8_t crc_en;
	enum ad3530r_mux_out_select mux_out_sel;
};

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/
int ad3530r_reg_write(struct ad3530r_desc *desc,
		      uint32_t reg_addr,
		      uint16_t reg_val);
int ad3530r_reg_read(struct ad3530r_desc *desc,
		     uint32_t reg_addr,
		     uint16_t *reg_val);
int ad3530r_spi_write_mask(struct ad3530r_desc *desc,
			   uint32_t addr, uint32_t mask, uint16_t val);
int ad3530r_update_interface_cfg(struct ad3530r_desc *desc,
				 struct ad3530r_transfer_config *cfg);
int ad3530r_multiple_reg_write(struct ad3530r_desc *desc,
			       uint32_t start_addr,
			       uint8_t buff_len,
			       uint8_t *buff);
int ad3530r_multiple_reg_read(struct ad3530r_desc *desc,
			      uint32_t addr,
			      uint8_t buff_len,
			      uint8_t *buff);
int ad3530r_set_reference(struct ad3530r_desc *desc,
			  enum ad3530r_ch_vref_select reference_selector);
int ad3530r_set_operating_mode(struct ad3530r_desc *desc,
			       uint8_t chn_num,
			       enum ad3530r_operating_mode chn_op_mode);
int ad3530r_set_output_range(struct ad3530r_desc *desc,
			     enum ad3530r_ch_output_range range_sel);
int ad3530r_set_crc_enable(struct ad3530r_desc *desc, bool en_di);
int ad3530r_set_mux_out_select(struct ad3530r_desc *desc,
			       enum ad3530r_mux_out_select mux_output_sel);
int ad3530r_set_hw_ldac(struct ad3530r_desc *desc, uint16_t mask_hw_ldac);
int ad3530r_set_sw_ldac(struct ad3530r_desc *desc, uint16_t mask_sw_ldac);
int ad3530r_set_dac_value(struct ad3530r_desc *desc,
			  uint16_t dac_value,
			  uint8_t dac_channel,
			  enum ad3530r_write_mode write_mode);
int ad3530r_set_multidac_value(struct ad3530r_desc *desc,
			       uint16_t dac_value,
			       uint16_t dac_chn_mask,
			       enum ad3530r_write_mode write_mode);
int ad3530r_sw_ldac_trigger(struct ad3530r_desc *desc);
int ad3530r_hw_ldac_trigger(struct ad3530r_desc *desc);
int ad3530r_reset(struct ad3530r_desc *desc);
int ad3530r_init(struct ad3530r_desc **desc,
		 struct ad3530r_init_param *init_param);
int ad3530r_remove(struct ad3530r_desc *desc);

#endif /* _AD3530R_H_ */