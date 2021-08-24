/**************************************************************************//**
*   @file   ad3552r.h
*   @brief  Header file of ad3552r Driver
*   @author Mihail Chindris (Mihail.Chindris@analog.com)
*
*******************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
******************************************************************************/

#ifndef _AD3552R_H_
#define _AD3552R_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
//#include "irq.h"
#include "gpio.h"
#include "crc8.h"

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/

#define AD3552R_NUM_CH					2

/* Register addresses */
/* Primary address space */
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_A		0x00
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_B		0x01
#define AD3552R_REG_ADDR_DEVICE_CONFIG			0x02
#define AD3552R_REG_ADDR_CHIP_TYPE			0x03
#define AD3552R_REG_ADDR_PRODUCT_ID_L			0x04
#define AD3552R_REG_ADDR_PRODUCT_ID_H			0x05
#define AD3552R_REG_ADDR_CHIP_GRADE			0x06
#define AD3552R_REG_ADDR_SCRATCH_PAD			0x0A
#define AD3552R_REG_ADDR_SPI_REVISION			0x0B
#define AD3552R_REG_ADDR_VENDOR_L			0x0C
#define AD3552R_REG_ADDR_VENDOR_H			0x0D
#define AD3552R_REG_ADDR_STREAM_MODE			0x0E
#define AD3552R_REG_ADDR_TRANSFER_REGISTER		0x0F
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_C		0x10
#define AD3552R_REG_ADDR_INTERFACE_STATUS_A		0x11
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_D		0x14
#define AD3552R_REG_ADDR_SH_REFERENCE_CONFIG		0x15
#define AD3552R_REG_ADDR_ERR_ALARM_MASK			0x16
#define AD3552R_REG_ADDR_ERR_STATUS			0x17
#define AD3552R_REG_ADDR_POWERDOWN_CONFIG		0x18
#define AD3552R_REG_ADDR_CH0_CH1_OUTPUT_RANGE		0x19
#define AD3552R_REG_ADDR_CH_OFFSET(ch)			(0x1B + (ch) * 2)
#define AD3552R_REG_ADDR_CH_GAIN(ch)			(0x1C + (ch) * 2)
/* Secondary region */
#define AD3552R_SECONDARY_REGION_START			0x28
#define AD3552R_REG_ADDR_HW_LDAC_16B			0x28
#define AD3552R_REG_ADDR_CH_DAC_16B(ch)			(0x29 + (ch) * 2)
#define AD3552R_REG_ADDR_DAC_PAGE_MASK_16B		0x2D
#define AD3552R_REG_ADDR_CH_SELECT_16B			0x2F
#define AD3552R_REG_ADDR_INPUT_PAGE_MASK_16B		0x30
#define AD3552R_REG_ADDR_SW_LDAC_16B			0x32
#define AD3552R_REG_ADDR_CH_INPUT_16B(ch)		(0x33 + (ch) * 2)
/* 3 bytes registers */
#define AD3552R_REG_START_24B				0x37
#define AD3552R_REG_ADDR_HW_LDAC_24B			0x37
#define AD3552R_REG_ADDR_CH_DAC_24B(ch)			(0x38 + (ch) * 3)
#define AD3552R_REG_ADDR_DAC_PAGE_MASK_24B		0x3E
#define AD3552R_REG_ADDR_CH_SELECT_24B			0x41
#define AD3552R_REG_ADDR_INPUT_PAGE_MASK_24B		0x42
#define AD3552R_REG_ADDR_SW_LDAC_24B			0x45
#define AD3552R_REG_ADDR_CH_INPUT_24B(ch)		(0x46 + (ch) * 3)

#define AD3552R_REG_ADDR_MAX				0x4B

/* AD3552R_REG_ADDR_INTERFACE_CONFIG_A */
#define AD3552R_MASK_SOFTWARE_RESET			(BIT(7) | BIT(0))
#define AD3552R_MASK_ADDR_ASCENSION			BIT(5)
#define AD3552R_MASK_SDO_ACTIVE				BIT(4)
/* AD3552R_REG_ADDR_INTERFACE_CONFIG_B */
#define AD3552R_MASK_SINGLE_INST			BIT(7)
#define AD3552R_MASK_SHORT_INSTRUCTION			BIT(3)
/* AD3552R_REG_ADDR_DEVICE_CONFIG */
#define AD3552R_MASK_DEVICE_STATUS(n)			BIT(4 + (n))
#define AD3552R_MASK_CUSTOM_MODES			(BIT(3) | BIT(2))
#define AD3552R_MASK_OPERATING_MODES			(BIT(1) | BIT(0))
/* AD3552R_REG_ADDR_CHIP_TYPE */
#define AD3552R_MASK_CLASS				0x0F
/* AD3552R_REG_ADDR_CHIP_GRADE */
#define AD3552R_MASK_GRADE				0xF0
#define AD3552R_MASK_DEVICE_REVISION			0x0F
/* AD3552R_REG_ADDR_STREAM_MODE */
#define AD3552R_MASK_LENGTH				0x0F
/* AD3552R_REG_ADDR_TRANSFER_REGISTER */
#define AD3552R_MASK_MULTI_IO_MODE			(BIT(7) | BIT(6))
#define AD3552R_MASK_STREAM_LENGTH_KEEP_VALUE		BIT(2)
/* AD3552R_REG_ADDR_INTERFACE_CONFIG_C */
#define AD3552R_MASK_CRC_ENABLE				(BIT(7) | BIT(6) | BIT(1) | BIT(0))
#define AD3552R_MASK_STRICT_REGISTER_ACCESS		BIT(5)
/* AD3552R_REG_ADDR_INTERFACE_STATUS_A */
#define AD3552R_MASK_INTERFACE_NOT_READY		BIT(7)
#define AD3552R_MASK_CLOCK_COUNTING_ERROR		BIT(5)
#define AD3552R_MASK_INVALID_OR_NO_CRC			BIT(3)
#define AD3552R_MASK_WRITE_TO_READ_ONLY_REGISTER	BIT(2)
#define AD3552R_MASK_PARTIAL_REGISTER_ACCESS		BIT(1)
#define AD3552R_MASK_REGISTER_ADDRESS_INVALID		BIT(0)
/* AD3552R_REG_ADDR_INTERFACE_CONFIG_D */
#define AD3552R_MASK_ALERT_ENABLE_PULLUP		BIT(6)
#define AD3552R_MASK_MEM_CRC_EN				BIT(4)
#define AD3552R_MASK_SDO_DRIVE_STRENGTH			(BIT(3) | BIT(2))
#define AD3552R_MASK_DUAL_SPI_SYNCHROUNOUS_EN		BIT(1)
#define AD3552R_MASK_SPI_CONFIG_DDR			BIT(0)
/* AD3552R_REG_ADDR_SH_REFERENCE_CONFIG */
#define AD3552R_MASK_IDUMP_FAST_MODE			BIT(6)
#define AD3552R_MASK_SAMPLE_HOLD_DIFFERENTIAL_USER_EN	BIT(5)
#define AD3552R_MASK_SAMPLE_HOLD_USER_TRIM		(BIT(4) | BIT(3))
#define AD3552R_MASK_SAMPLE_HOLD_USER_ENABLE		BIT(2)
#define AD3552R_MASK_REFERENCE_VOLTAGE_SEL		(BIT(1) | BIT(0))
/* AD3552R_REG_ADDR_ERR_ALARM_MASK */
#define AD3552R_MASK_REF_RANGE_ALARM			BIT(6)
#define AD3552R_MASK_CLOCK_COUNT_ERR_ALARM		BIT(5)
#define AD3552R_MASK_MEM_CRC_ERR_ALARM			BIT(4)
#define AD3552R_MASK_SPI_CRC_ERR_ALARM			BIT(3)
#define AD3552R_MASK_WRITE_TO_READ_ONLY_ALARM		BIT(2)
#define AD3552R_MASK_PARTIAL_REGISTER_ACCESS_ALARM	BIT(1)
#define AD3552R_MASK_REGISTER_ADDRESS_INVALID_ALARM	BIT(0)
/* AD3552R_REG_ADDR_ERR_STATUS */
#define AD3552R_MASK_REF_RANGE_ERR_STATUS			BIT(6)
#define AD3552R_MASK_DUAL_SPI_STREAM_EXCEEDS_DAC_ERR_STATUS	BIT(5)
#define AD3552R_MASK_MEM_CRC_ERR_STATUS				BIT(4)
#define AD3552R_MASK_RESET_STATUS				BIT(0)
/* AD3552R_REG_ADDR_POWERDOWN_CONFIG */
#define AD3552R_MASK_CH_DAC_POWERDOWN(ch)		BIT(4 + (ch))
#define AD3552R_MASK_CH_AMPLIFIER_POWERDOWN(ch)		BIT(ch)
/* AD3552R_REG_ADDR_CH0_CH1_OUTPUT_RANGE */
#define AD3552R_MASK_CH_OUTPUT_RANGE_SEL(ch)		((ch) ? 0xF0 : 0x0F)
/* AD3552R_REG_ADDR_CH_GAIN */
#define AD3552R_MASK_CH_RANGE_OVERRIDE			BIT(7)
#define AD3552R_MASK_CH_GAIN_SCALING_N			(BIT(6) | BIT(5))
#define AD3552R_MASK_CH_GAIN_SCALING_P			(BIT(4) | BIT(3))
#define AD3552R_MASK_CH_OFFSET_POLARITY			BIT(2)
#define AD3552R_MASK_CH_OFFSET				BIT(0)
/* Mask valid for multiple registers */
#define AD3552R_MASK_DAC_DATA24				0xFFFFC0
#define AD3552R_MASK_CH(ch)				BIT(ch)
#define AD3552R_MASK_ALL_CH				(BIT(0) | BIT(1))
#define AD3552R_REAL_BITS_PREC_MODE			16
#define AD3552R_STORAGE_BITS_PREC_MODE			24
#define AD3552R_REAL_BITS_FAST_MODE			12
#define AD3552R_STORAGE_BITS_FAST_MODE			16

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ad3552r_status {
	/* Status bits */
	AD3552R_RESET_STATUS				= 0x0001,
	AD3552R_INTERFACE_NOT_READY			= 0x0002,

	/* Errors */
	AD3552R_CLOCK_COUNTING_ERROR			= 0x0004,
	AD3552R_INVALID_OR_NO_CRC			= 0x0008,
	AD3552R_WRITE_TO_READ_ONLY_REGISTER		= 0x0010,
	AD3552R_PARTIAL_REGISTER_ACCESS			= 0x0020,
	AD3552R_REGISTER_ADDRESS_INVALID		= 0x0040,
	AD3552R_REF_RANGE_ERR_STATUS			= 0x0080,
	AD3552R_DUAL_SPI_STREAM_EXCEEDS_DAC_ERR_STATUS	= 0x0100,
	AD3552R_MEM_CRC_ERR_STATUS			= 0x0200
};

enum ad3552r_ch_output_range {
	/* Range from 0 V to 2.5 V. Requires Rfb1x connection */
	AD3552R_CH_OUTPUT_RANGE_0__2_5V,
	/* Range from 0 V to 5 V. Requires Rfb1x connection  */
	AD3552R_CH_OUTPUT_RANGE_0__5V,
	/* Range from 0 V to 10 V. Requires Rfb2x connection  */
	AD3552R_CH_OUTPUT_RANGE_0__10V,
	/* Range from -2.5 V to 7.5 V. Requires Rfb2x connection  */
	AD3552R_CH_OUTPUT_RANGE_NEG_5__5V,
	/* Range from -6.5 V to 3.5 V. Requires Rfb4x connection  */
	AD3552R_CH_OUTPUT_RANGE_NEG_10__10V,
};

enum ad3552r_ch_gain_scaling {
	/* Gain scaling of 1 */
	AD3552R_CH_GAIN_SCALING_1,
	/* Gain scaling of 0.5 */
	AD3552R_CH_GAIN_SCALING_0_5,
	/* Gain scaling of 0.25 */
	AD3552R_CH_GAIN_SCALING_0_25,
	/* Gain scaling of 0.125 */
	AD3552R_CH_GAIN_SCALING_0_125,
};

enum ad3552r_offset_polarity {
	/* Positive offset */
	AD3552R_OFFSET_POLARITY_POSITIVE,
	/* Negative offset */
	AD3552R_OFFSET_POLARITY_NEGATIVE,
};

/* Update modes for ad3552r_write_dev function */
enum ad3552r_update_mode {
	/* Update directly DAC register */
	AD3552R_UPDATE_DAC,
	/* Update DAC input registers */
	AD3552R_UPDATE_DAC_MASK,
	/* Update DAC input registers */
	AD3552R_UPDATE_INPUT,
	/* Update DAC input mask registers */
	AD3552R_UPDATE_INPUT_MASK,
};


/* Update modes for ad3552r_write_dev function */
enum ad3552r_input_trigger_mode {
	/* No trigger is set */
	AD3552R_TRIGGER_NONE,
	/* Use sw ldac trigger with software delay */
	AD3552R_TRIGGER_DELAY_DAC_UPDATE,
	/* Use sw ldac trigger with software delay */
	AD3552R_TRIGGER_SW_LDAC,
	/* Use hw LDAC with a signal generated by the MCU */
	AD3552R_TRIGGER_HW_LDAC_INTERNAL,
	/* Use hw LDAC with an external signal */
	AD3552R_TRIGGER_HW_LDAC_EXTERNAL
};

enum ad3552r_dev_attributes {
	/* Direct register values */
	/* From 0-3 */
	AD3552R_SDO_DRIVE_STRENGTH,
	/*
	 * 0 -> Internal Vref, vref_io pin floating (default)
	 * 1 -> Internal Vref, vref_io driven by internal vref
	 * 2 or 3 -> External Vref
	 */
	AD3552R_VREF_SELECT,
	/* Enable / Disable CRC */
	AD3552R_CRC_ENABLE,
	/* Spi mode: Strandard, Dual or Quad */
	AD3552R_SPI_MULTI_IO_MODE,
	/* Spi data rate: Single or dual */
	AD3552R_SPI_DATA_RATE,
	/* Dual spi synchronous mode */
	AD3552R_SPI_SYNCHRONOUS_ENABLE,

	/* Software implemented attributes */
	/* Update mode: Use values from enum ad3552r_update_mode. */
	AD3552R_UPDATE_MODE,
	/* Update mode: Use values from enum ad3552r_input_trigger_mode. */
	AD3552R_INPUT_TRIGGER_MODE,
	/* Trigger period or period between DAC writes */
	AD3552R_DAC_UPDATE_PERIOD_NS,
	/*
	 * Precision mode enabled.
	 * If set to 1, CHx_DAC/INPUT_24B registers will be used (16 bits used)
	 * If set to 0, CHx_DAC/INPUT_16B registers will be used (12 bits used)
	 */
	AD3552R_PRECISION_MODE_ENABLED
};

enum ad3552r_ch_attributes {
	/* DAC powerdown */
	AD3552R_CH_DAC_POWERDOWN,
	/* DAC amplifier powerdown */
	AD3552R_CH_AMPLIFIER_POWERDOWN,
	/* Select the output range. Select from enum ad3552r_ch_output_range */
	AD3552R_CH_OUTPUT_RANGE_SEL,
	/*
	 * Over-rider the range selector in order to manually set the output
	 * voltage range
	 */
	AD3552R_CH_RANGE_OVERRIDE,
	/* Manually set the offset voltage */
	AD3552R_CH_GAIN_OFFSET,
	/* Sets the polarity of the offset. */
	AD3552R_CH_GAIN_OFFSET_POLARITY,
	/* PDAC gain scaling */
	AD3552R_CH_GAIN_SCALING_P,
	/* NDAC gain scaling */
	AD3552R_CH_GAIN_SCALING_N,
	/* Trigger a software LDAC */
	AD3552R_CH_TRIGGER_SOFTWARE_LDAC,
	/* Hardware LDAC Mask */
	AD3552R_CH_HW_LDAC_MASK,
	/* Rfb value */
	AD3552R_CH_RFB,
	/* Code value when voltage is minimum. Used to calculate voltage */
	AD3552R_CH_CODE_AT_MIN,
	/* Code value when voltage is maximum. Used to calculate voltage */
	AD3552R_CH_CODE_AT_MAX,
	/* Select channel to be updated on page mask update */
	AD3552R_CH_SELECT,
	/* Raw value to be set to dac */
	AD3552R_CH_CODE,
	/* Enable/Disable channel */
	AD3552R_CH_ENABLE
};

/* By default all values are set to 0 */
struct ad3552_transfer_config {
	/* Defines the length of the loop when streaming data */
	uint8_t		stream_mode_length;
	/* Determines Sequential Addressing Behavior */
	uint8_t		addr_asc : 1;
	/* Select Streaming or Single Instruction Mode */
	uint8_t		single_instr: 1;
	/*
	 * Set this bit to prevent the STREAM_MODE LENGTH value from
	 * automatically resetting to zero
	 */
	uint8_t		stream_length_keep_value : 1;
	/* Controls the SPI. Single (0), Dual (1), Quad (2)*/
	uint8_t		multi_io_mode : 2;
	/*
	 * When this bIt is set, the DAC word is expected in
	 * Double Data Rate(DDR) configuration
	 */
	uint8_t		ddr : 1;
	/*
	 * When this bit is set the SPI interface is expected as a dual
	 * synchronous configuration
	 */
	uint8_t		synchronous : 1;
};

struct ad3552_transfer_data {
	/* Starting address for transfer */
	uint8_t		addr;
	/* Data to transfer */
	uint8_t		*data;
	/* Size of data to transfer */
	uint32_t	len;
	/* Read transaction if true, write transfer otherwise */
	uint8_t		is_read : 1;
	/* If NULL will be default or last configured will be used */
	struct ad3552_transfer_config *spi_cfg;
};

struct ad3552r_desc;

struct ad3552r_init_param {
	struct spi_init_param	spi_param;
	struct gpio_init_param	*ldac_param;
	//uint16_t		timer_id;
	//uint16_t		timer_intr_nb;
	uint8_t			(*is_hw_trigger_ready)(void *ctx);
	void			*hw_trigger_ctx;
	void			*tmr_extra;
	struct irq_ctrl_desc	*irq_crtl;
};

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/

int32_t ad3552r_init(struct ad3552r_desc **desc,
		     struct ad3552r_init_param *init_param);

int32_t ad3552r_remove(struct ad3552r_desc *desc);

int32_t ad3552r_software_reset(struct ad3552r_desc *desc);

/* Get status and error bits. If clear_errors is set, errors will be cleared */
int32_t ad3552r_get_status(struct ad3552r_desc *desc, uint32_t *status,
			   uint8_t clr_err);

int32_t ad3552r_transfer(struct ad3552r_desc *desc,
			     struct ad3552_transfer_data *data);

int32_t ad3552r_write_reg(struct ad3552r_desc *desc, uint8_t addr,
			  uint32_t val);

int32_t ad3552r_read_reg(struct ad3552r_desc *desc, uint8_t addr,
			 uint32_t *val);

int32_t ad3552r_get_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint32_t *val);

int32_t ad3552r_set_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint32_t val);

int32_t ad3552r_get_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t *val);

int32_t ad3552r_set_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t val);

int32_t ad3552r_set_ch_voltage(struct ad3552r_desc *desc, uint8_t ch,
			       const float val);

int32_t ad3552r_get_ch_voltage(struct ad3552r_desc *desc, uint8_t ch,
			       float *val);

int32_t ad3552r_prepare_write(struct ad3552r_desc *desc, uint32_t mask);

/**
 * Values from data will be updated accordingly to which AD3552R_UPDATE_MODE
 * was set.
 * If input registers or mask input register are updated, a software ldac
 * trigger will be generated with a period configured by AD3552R_LDAC_PERIOD_US
 */
int32_t ad3552r_write_dev(struct ad3552r_desc *desc, uint8_t *data,
			  uint32_t samples);

#ifdef DEBUG
int32_t read_regs(struct ad3552r_desc *dac);
#endif

#endif /* _AD3552R_H_ */
