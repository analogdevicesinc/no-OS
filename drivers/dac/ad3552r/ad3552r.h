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
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_crc8.h"

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/

/* Register addresses */
/* Primary address space */
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_A		0x00
#define   AD3552R_MASK_SOFTWARE_RESET			(NO_OS_BIT(7) | NO_OS_BIT(0))
#define   AD3552R_MASK_ADDR_ASCENSION			NO_OS_BIT(5)
#define   AD3552R_MASK_SDO_ACTIVE			NO_OS_BIT(4)
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_B		0x01
#define   AD3552R_MASK_SINGLE_INST			NO_OS_BIT(7)
#define   AD3552R_MASK_SHORT_INSTRUCTION		NO_OS_BIT(3)
#define AD3552R_REG_ADDR_DEVICE_CONFIG			0x02
#define   AD3552R_MASK_DEVICE_STATUS(n)			NO_OS_BIT(4 + (n))
#define   AD3552R_MASK_CUSTOM_MODES			(NO_OS_BIT(3) | NO_OS_BIT(2))
#define   AD3552R_MASK_OPERATING_MODES			NO_OS_GENMASK(1, 0)
#define AD3552R_REG_ADDR_CHIP_TYPE			0x03
#define   AD3552R_MASK_CLASS				NO_OS_GENMASK(7, 0)
#define AD3552R_REG_ADDR_PRODUCT_ID_L			0x04
#define AD3552R_REG_ADDR_PRODUCT_ID_H			0x05
#define AD3552R_REG_ADDR_CHIP_GRADE			0x06
#define   AD3552R_MASK_GRADE				NO_OS_GENMASK(7, 4)
#define   AD3552R_MASK_DEVICE_REVISION			NO_OS_GENMASK(3, 0)
#define AD3552R_REG_ADDR_SCRATCH_PAD			0x0A
#define AD3552R_REG_ADDR_SPI_REVISION			0x0B
#define AD3552R_REG_ADDR_VENDOR_L			0x0C
#define AD3552R_REG_ADDR_VENDOR_H			0x0D
#define AD3552R_REG_ADDR_STREAM_MODE			0x0E
#define   AD3552R_MASK_LENGTH				0xFF
#define AD3552R_REG_ADDR_TRANSFER_REGISTER		0x0F
#define   AD3552R_MASK_MULTI_IO_MODE			(NO_OS_BIT(7) | NO_OS_BIT(6))
#define   AD3552R_MASK_STREAM_LENGTH_KEEP_VALUE		NO_OS_BIT(2)
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_C		0x10
#define   AD3552R_MASK_CRC_ENABLE			(NO_OS_BIT(7) | NO_OS_BIT(6) | NO_OS_BIT(1) | NO_OS_BIT(0))
#define   AD3552R_MASK_STRICT_REGISTER_ACCESS		NO_OS_BIT(5)
#define AD3552R_REG_ADDR_INTERFACE_STATUS_A		0x11
#define   AD3552R_MASK_INTERFACE_NOT_READY		NO_OS_BIT(7)
#define   AD3552R_MASK_CLOCK_COUNTING_ERROR		NO_OS_BIT(5)
#define   AD3552R_MASK_INVALID_OR_NO_CRC		NO_OS_BIT(3)
#define   AD3552R_MASK_WRITE_TO_READ_ONLY_REGISTER	NO_OS_BIT(2)
#define   AD3552R_MASK_PARTIAL_REGISTER_ACCESS		NO_OS_BIT(1)
#define   AD3552R_MASK_REGISTER_ADDRESS_INVALID		NO_OS_BIT(0)
#define AD3552R_REG_ADDR_INTERFACE_CONFIG_D		0x14
#define   AD3552R_MASK_ALERT_ENABLE_PULLUP		NO_OS_BIT(6)
#define   AD3552R_MASK_MEM_CRC_EN			NO_OS_BIT(4)
#define   AD3552R_MASK_SDO_DRIVE_STRENGTH		(NO_OS_BIT(3) | NO_OS_BIT(2))
#define   AD3552R_MASK_DUAL_SPI_SYNCHROUNOUS_EN		NO_OS_BIT(1)
#define   AD3552R_MASK_SPI_CONFIG_DDR			NO_OS_BIT(0)
#define AD3552R_REG_ADDR_SH_REFERENCE_CONFIG		0x15
#define   AD3552R_MASK_IDUMP_FAST_MODE			NO_OS_BIT(6)
#define   AD3552R_MASK_SAMPLE_HOLD_DIFFERENTIAL_USER_EN	NO_OS_BIT(5)
#define   AD3552R_MASK_SAMPLE_HOLD_USER_TRIM		(NO_OS_BIT(4) | NO_OS_BIT(3))
#define   AD3552R_MASK_SAMPLE_HOLD_USER_ENABLE		NO_OS_BIT(2)
#define   AD3552R_MASK_REFERENCE_VOLTAGE_SEL		(NO_OS_BIT(1) | NO_OS_BIT(0))
#define AD3552R_REG_ADDR_ERR_ALARM_MASK			0x16
#define   AD3552R_MASK_REF_RANGE_ALARM			NO_OS_BIT(6)
#define   AD3552R_MASK_CLOCK_COUNT_ERR_ALARM		NO_OS_BIT(5)
#define   AD3552R_MASK_MEM_CRC_ERR_ALARM		NO_OS_BIT(4)
#define   AD3552R_MASK_SPI_CRC_ERR_ALARM		NO_OS_BIT(3)
#define   AD3552R_MASK_WRITE_TO_READ_ONLY_ALARM		NO_OS_BIT(2)
#define   AD3552R_MASK_PARTIAL_REGISTER_ACCESS_ALARM	NO_OS_BIT(1)
#define   AD3552R_MASK_REGISTER_ADDRESS_INVALID_ALARM	NO_OS_BIT(0)
#define AD3552R_REG_ADDR_ERR_STATUS			0x17
#define   AD3552R_MASK_REF_RANGE_ERR_STATUS			NO_OS_BIT(6)
#define   AD3552R_MASK_DUAL_SPI_STREAM_EXCEEDS_DAC_ERR_STATUS	NO_OS_BIT(5)
#define   AD3552R_MASK_MEM_CRC_ERR_STATUS			NO_OS_BIT(4)
#define   AD3552R_MASK_RESET_STATUS				NO_OS_BIT(0)
#define AD3552R_REG_ADDR_POWERDOWN_CONFIG		0x18
#define   AD3552R_MASK_CH_DAC_POWERDOWN(ch)		NO_OS_BIT(4 + (ch))
#define   AD3552R_MASK_CH_AMPLIFIER_POWERDOWN(ch)	NO_OS_BIT(ch)
#define AD3552R_REG_ADDR_CH0_CH1_OUTPUT_RANGE		0x19
#define   AD3552R_MASK_CH_OUTPUT_RANGE_SEL(ch)		((ch) ? 0xF0 : 0xF)
#define AD3552R_REG_ADDR_CH_OFFSET(ch)			(0x1B + (ch) * 2)
#define   AD3552R_MASK_CH_OFFSET_BITS_0_7		0xFF
#define AD3552R_REG_ADDR_CH_GAIN(ch)			(0x1C + (ch) * 2)
#define   AD3552R_MASK_CH_RANGE_OVERRIDE		NO_OS_BIT(7)
#define   AD3552R_MASK_CH_GAIN_SCALING_N		(NO_OS_BIT(6) | NO_OS_BIT(5))
#define   AD3552R_MASK_CH_GAIN_SCALING_P		(NO_OS_BIT(4) | NO_OS_BIT(3))
#define   AD3552R_MASK_CH_OFFSET_POLARITY		NO_OS_BIT(2)
#define   AD3552R_MASK_CH_OFFSET_BIT_8			NO_OS_BIT(0)

/*
 * Secondary region
 * For multibyte registers specify the highest address because the access is
 * done in descending order
 */
#define AD3552R_SECONDARY_REGION_START			0x28
#define AD3552R_REG_ADDR_HW_LDAC_16B			0x28
#define AD3552R_REG_ADDR_CH_DAC_16B(ch)			(0x2C - (1 - ch) * 2)
#define AD3552R_REG_ADDR_DAC_PAGE_MASK_16B		0x2E
#define AD3552R_REG_ADDR_CH_SELECT_16B			0x2F
#define AD3552R_REG_ADDR_INPUT_PAGE_MASK_16B		0x31
#define AD3552R_REG_ADDR_SW_LDAC_16B			0x32
#define AD3552R_REG_ADDR_CH_INPUT_16B(ch)		(0x36 - (1 - ch) * 2)
/* 3 bytes registers */
#define AD3552R_REG_START_24B				0x37
#define AD3552R_REG_ADDR_HW_LDAC_24B			0x37
#define AD3552R_REG_ADDR_CH_DAC_24B(ch)			(0x3D - (1 - ch) * 3)
#define AD3552R_REG_ADDR_DAC_PAGE_MASK_24B		0x40
#define AD3552R_REG_ADDR_CH_SELECT_24B			0x41
#define AD3552R_REG_ADDR_INPUT_PAGE_MASK_24B		0x44
#define AD3552R_REG_ADDR_SW_LDAC_24B			0x45
#define AD3552R_REG_ADDR_CH_INPUT_24B(ch)		(0x4B - (1 - ch) * 3)

#define AD3552R_REG_ADDR_MAX		0x4B

/* Useful defines */
#define AD3552R_MASK_CH(ch)				NO_OS_BIT(ch)
#define AD3552R_MASK_ALL_CH				(NO_OS_BIT(0) | NO_OS_BIT(1))
#define AD3552R_MASK_DAC_12B				0xFFF0
#define AD3552R_REAL_BITS_PREC_MODE			16
#define AD3552R_STORAGE_BITS_PREC_MODE			24
#define AD3552R_REAL_BITS_FAST_MODE			12
#define AD3552R_STORAGE_BITS_FAST_MODE			16
#define AD3552R_MAX_OFFSET				511
#define AD3552R_LDAC_PULSE_US				1
#define AD3552R_BOTH_CH_SELECT			(NO_OS_BIT(0) | NO_OS_BIT(1))
#define AD3552R_BOTH_CH_DESELECT		0x0

/* Maximum number of channels in this family of devices */
#define AD3552R_MAX_NUM_CH		2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ad3552r_id {
	AD3541R_ID,
	AD3542R_ID,
	AD3551R_ID,
	AD3552R_ID
};

enum ad3552r_ch_vref_select {
	/* Internal source with Vref I/O floating */
	AD3552R_INTERNAL_VREF_PIN_FLOATING,
	/* Internal source with Vref I/O at 2.5V */
	AD3552R_INTERNAL_VREF_PIN_2P5V,
	/* External source with Vref I/O as input */
	AD3552R_EXTERNAL_VREF_PIN_INPUT
};

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
	AD3552R_CH_OUTPUT_RANGE_0__2P5V,
	/* Range from 0 V to 5 V. Requires Rfb1x connection  */
	AD3552R_CH_OUTPUT_RANGE_0__5V,
	/* Range from 0 V to 10 V. Requires Rfb2x connection  */
	AD3552R_CH_OUTPUT_RANGE_0__10V,
	/* Range from -5 V to 5 V. Requires Rfb2x connection  */
	AD3552R_CH_OUTPUT_RANGE_NEG_5__5V,
	/* Range from -10 V to 10 V. Requires Rfb4x connection  */
	AD3552R_CH_OUTPUT_RANGE_NEG_10__10V,
};

enum ad3542r_ch_output_range {
	/* Range from 0 V to 2.5 V. Requires Rfb1x connection */
	AD3542R_CH_OUTPUT_RANGE_0__2P5V,
	/* Range from 0 V to 5 V. Requires Rfb1x connection  */
	AD3542R_CH_OUTPUT_RANGE_0__5V,
	/* Range from 0 V to 10 V. Requires Rfb2x connection  */
	AD3542R_CH_OUTPUT_RANGE_0__10V,
	/* Range from -5 V to 5 V. Requires Rfb2x connection  */
	AD3542R_CH_OUTPUT_RANGE_NEG_5__5V,
	/* Range from -2.5 V to 7.5 V. Requires Rfb2x connection  */
	AD3542R_CH_OUTPUT_RANGE_NEG_2P5__7P5V,
};

enum ad3552r_sdio_drive_strength {
	AD3552R_LOW_SDIO_DRIVE_STRENGTH,
	AD3552R_MEDIUM_LOW_SDIO_DRIVE_STRENGTH,
	AD3552R_MEDIUM_HIGH_SDIO_DRIVE_STRENGTH,
	AD3552R_HIGH_SDIO_DRIVE_STRENGTH
};

enum num_channels {
	AD3541R_NUM_CHANNELS=1,
	AD3542R_NUM_CHANNELS=2,
	AD3551R_NUM_CHANNELS=1,
	AD3552R_NUM_CHANNELS=2
};

#define AD3552R_CH_OUTPUT_RANGE_CUSTOM 100

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
#ifdef AD3552R_QSPI_IMPLEMENTED
	/* Spi mode: Strandard, Dual or Quad */
	AD3552R_SPI_MULTI_IO_MODE,
	/* Spi data rate: Single or dual */
	AD3552R_SPI_DATA_RATE,
	/* Dual spi synchronous mode */
	AD3552R_SPI_SYNCHRONOUS_ENABLE,
#endif
};

enum ad3552r_ch_attributes {
	/* DAC powerdown */
	AD3552R_CH_DAC_POWERDOWN,
	/* DAC amplifier powerdown */
	AD3552R_CH_AMPLIFIER_POWERDOWN,
	/* Select from enum ad3552r_ch_output_range or ad3542r_ch_output_range */
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
	/* Write to fast regs (only 16 bits of data) */
	AD3552R_CH_FAST_EN,
	/* Channel select. When set allow Input -> DAC and Mask -> DAC */
	AD3552R_CH_SELECT,
	/* Raw value to be set to dac */
	AD3552R_CH_CODE
};

enum ad3552r_write_mode {
	/* Write to DAC registers. No need to trigger LDAC */
	AD3552R_WRITE_DAC_REGS,
	/* Write to input registers. User needs to trigger LDAC */
	AD3552R_WRITE_INPUT_REGS,
	/* Write to input registers. LDAC is triggered by the driver */
	AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC
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
#ifdef AD3552R_QSPI_IMPLEMENTED
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
#endif
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

struct ad3552r_ch_data {
	int32_t scale_int;
	int32_t scale_dec;
	int32_t offset_int;
	int32_t offset_dec;
	int16_t gain_offset;
	uint16_t offset;
	uint8_t offset_polarity;
	uint16_t rfb;
	uint8_t n;
	uint8_t p;
	uint8_t range;
	uint8_t range_override;
	uint8_t fast_en;
};

struct ad3552r_desc {
	struct ad3552_transfer_config spi_cfg;
	struct no_os_spi_desc *spi;
	struct no_os_gpio_desc *ldac;
	struct no_os_gpio_desc *reset;
	struct ad3552r_ch_data ch_data[AD3552R_MAX_NUM_CH];
	uint8_t crc_table[NO_OS_CRC8_TABLE_SIZE];
	uint8_t chip_id;
	uint8_t crc_en : 1;
	uint8_t is_simultaneous : 1;
	uint8_t single_transfer : 1;
};

struct ad3552r_custom_output_range_cfg {
	int16_t gain_offset;
	/* GainP = 1 / ( 2 ^ gain_scaling_p_inv_log2)
	   From 0 to 3 */
	uint8_t gain_scaling_p_inv_log2;
	/* GainP = 1 / ( 2 ^ gain_scaling_n_inv_log2)
	   From 0 to 3 */
	uint8_t gain_scaling_n_inv_log2;
	/* RFB value */
	uint16_t rfb_ohms;
};

struct ad3552r_channel_init {
	bool en;
	/* Use only 12 bits precision instead of 16 for data. */
	bool fast_en;
	/*
	 * Use enum ad3552r_ch_ouput_range or ad3542r_ch_output_range
	 * (Depending on id), or AD3552R_CH_OUTPUT_RANGE_CUSTOM to configure
	 * using custom_output_range.
	 */
	uint8_t range;
	struct ad3552r_custom_output_range_cfg custom_range;
};

struct ad3552r_init_param {
	enum ad3552r_id	chip_id;
	struct no_os_spi_init_param spi_param;
	/* If set, reset is done with RESET pin, otherwise it will be soft */
	struct no_os_gpio_init_param	*reset_gpio_param_optional;
	/* If set, input register are used and LDAC pulse is sent */
	struct no_os_gpio_init_param	*ldac_gpio_param_optional;
	/* If set, use external Vref */
	bool use_external_vref;
	/* If set, output internal Vref on Vref pin */
	bool vref_out_enable;
	/* From 0 to 3 */
	uint8_t sdo_drive_strength;
	struct ad3552r_channel_init channels[AD3552R_MAX_NUM_CH];
	/* Set to enable CRC */
	bool crc_en;
	bool is_simultaneous;
	bool single_transfer;
};

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/

uint8_t ad3552r_reg_len(uint8_t addr);

uint8_t ad3552r_get_code_reg_addr(uint8_t ch, uint8_t is_dac, uint8_t is_fast);

int32_t ad3552r_init(struct ad3552r_desc **desc,
		     struct ad3552r_init_param *init_param);

int32_t ad3552r_remove(struct ad3552r_desc *desc);

int32_t ad3552r_reset(struct ad3552r_desc *desc);

/* Get status and error bits. If clear_errors is set, errors will be cleared */
int32_t ad3552r_get_status(struct ad3552r_desc *desc, uint32_t *status,
			   uint8_t clr_err);

int32_t ad3552r_transfer(struct ad3552r_desc *desc,
			 struct ad3552_transfer_data *data);

int32_t ad3552r_write_reg(struct ad3552r_desc *desc, uint8_t addr,
			  uint16_t val);

int32_t ad3552r_read_reg(struct ad3552r_desc *desc, uint8_t addr,
			 uint16_t *val);

int32_t ad3552r_get_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint16_t *val);

int32_t ad3552r_set_dev_value(struct ad3552r_desc *desc,
			      enum ad3552r_dev_attributes attr,
			      uint16_t val);

int32_t ad3552r_get_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t *val);

int32_t ad3552r_set_ch_value(struct ad3552r_desc *desc,
			     enum ad3552r_ch_attributes attr,
			     uint8_t ch,
			     uint16_t val);

int32_t ad3552r_get_scale(struct ad3552r_desc *desc, uint8_t ch,
			  int32_t *integer, int32_t *dec);

int32_t ad3552r_get_offset(struct ad3552r_desc *desc, uint8_t ch,
			   int32_t *integer, int32_t *dec);

int32_t ad3552r_ldac_trigger(struct ad3552r_desc *desc, uint16_t mask,
			     uint8_t is_fast);

int32_t ad3552r_set_asynchronous(struct ad3552r_desc *desc, uint8_t enable);

/* Send one sample at a time, one after an other or at a LDAC_period interval.
 * If LDAC pin set, send LDAC signal. Otherwise software LDAC is used. */
int32_t ad3552r_write_samples(struct ad3552r_desc *desc, uint16_t *data,
			      uint32_t samples, uint32_t ch_mask,
			      enum ad3552r_write_mode mode);

int32_t ad3552r_simulatneous_update_enable(struct ad3552r_desc *desc);
#endif /* _AD3552R_H_ */
