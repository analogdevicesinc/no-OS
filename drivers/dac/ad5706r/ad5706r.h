/***************************************************************************//**
 *   @file   ad5706r.h
 *   @brief  Header file of AD5706R Driver.
 *   @author NAlteza (nathaniel.alteza@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
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
#ifndef AD5706R_H_
#define AD5706R_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_crc8.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD5706R_R1B		(1ul << 16)  	/* 1-byte register address */
#define AD5706R_R2B		(2ul << 16)  	/* 2-byte register address */
#define AD5706R_LEN(x)		(x >> 16)	/* Get register byte length */
#define AD5706R_ADDR(x)		(x & 0xFFFF)
#define AD5706R_ADDR_MD(x)	(x & 0x0FFF)	/* Reg Addr exclude MD Addr */
#define AD5706R_MD_ADDR_MASK		NO_OS_GENMASK(1, 0)
#define AD5706R_MD_ADDR(x)		((x & AD5706R_MD_ADDR_MASK) << 4)

#define AD5706R_READ_BIT			NO_OS_BIT(7)
#define AD5706R_ADDR_MASK			(~AD5706R_READ_BIT)
#define AD5706R_WRITE_BIT_LONG_INSTR		0x00
#define AD5706R_CRC_POLY			0x07
#define AD5706R_CRC_SEED			0xA5
#define AD5706R_SCRATCH_PAD_TEST_VAL		0x6A

#define AD5706R_MULTIBYTE_REG_START		0x14
#define AD5706R_MULTIBYTE_REG_END		0x71

/*
 * AD5706R Registers Addresses
 */
#define AD5706R_REG_INTERFACE_CONFIG_A		(AD5706R_R1B | 0x00)
#define AD5706R_REG_INTERFACE_CONFIG_B		(AD5706R_R1B | 0x01)
#define AD5706R_REG_DEVICE_CONFIG		(AD5706R_R1B | 0x02)
#define AD5706R_REG_CHIP_TYPE			(AD5706R_R1B | 0x03)
#define AD5706R_REG_PRODUCT_ID_L		(AD5706R_R1B | 0x04)
#define AD5706R_REG_PRODUCT_ID_H		(AD5706R_R1B | 0x05)
#define AD5706R_REG_CHIP_GRADE			(AD5706R_R1B | 0x06)
#define AD5706R_REG_DEVICE_INDEX		(AD5706R_R1B | 0x09)
#define AD5706R_REG_SCRATCH_PAD			(AD5706R_R1B | 0x0A)
#define AD5706R_REG_SPI_REVISION		(AD5706R_R1B | 0x0B)
#define AD5706R_REG_VENDOR_L			(AD5706R_R1B | 0x0C)
#define AD5706R_REG_VENDOR_H			(AD5706R_R1B | 0x0D)
#define AD5706R_REG_STREAM_MODE			(AD5706R_R1B | 0x0E)
#define AD5706R_REG_TRANSFER_CONFIG		(AD5706R_R1B | 0x0F)
#define AD5706R_REG_INTERFACE_CONFIG_C		(AD5706R_R1B | 0x10)
#define AD5706R_REG_INTERFACE_STATUS_A		(AD5706R_R1B | 0x11)

/*
 * AD5706R DAC Configuration Registers
 */
#define AD5706R_REG_MULTI_DAC_CH_SEL		(AD5706R_R2B | 0x14)
#define AD5706R_REG_LDAC_SYNC_ASYNC		(AD5706R_R2B | 0x16)
#define AD5706R_REG_LDAC_HW_SW			(AD5706R_R2B | 0x18)
#define AD5706R_REG_LDAC_EDGE_SEL_CH(x)		(AD5706R_R2B | (0x1A + (x * 2)))
#define AD5706R_REG_OUT_OPERATING_MODE		(AD5706R_R2B | 0x22)
#define AD5706R_REG_OUT_SWITCH_EN		(AD5706R_R2B | 0x24)
#define AD5706R_REG_HW_SHUTDOWN_EN		(AD5706R_R2B | 0x26)
#define AD5706R_REG_OUT_RANGE_CH(x)		(AD5706R_R2B | (0x28 + (x * 2)))
#define AD5706R_REG_FUNC_EN			(AD5706R_R2B | 0x30)
#define AD5706R_REG_FUNC_MODE_SEL_CH(x)		(AD5706R_R2B | (0x32 + (x * 2)))
#define AD5706R_REG_FUNC_DAC_INPUT_B_CH(x)	(AD5706R_R2B | (0x3A + (x * 2)))
#define AD5706R_REG_FUNC_DITHER_PERIOD_CH(x)	(AD5706R_R2B | (0x42 + (x * 2)))
#define AD5706R_REG_FUNC_DITHER_PHASE_CH(x)	(AD5706R_R2B | (0x4A + (x * 2)))
#define AD5706R_REG_MUX_OUT_SEL			(AD5706R_R2B | 0x54)
#define AD5706R_REG_MUX_OUT_CONTROL		(AD5706R_R2B | 0x56)
#define AD5706R_REG_TEMP_WARN_INT_EN		(AD5706R_R2B | 0x58)
#define AD5706R_REG_MULTI_DAC_SW_LDAC		(AD5706R_R2B | 0x5A)
#define AD5706R_REG_MULTI_DAC_INPUT_A		(AD5706R_R2B | 0x5C)
#define AD5706R_REG_DAC_SW_LDAC			(AD5706R_R2B | 0x5E)
#define AD5706R_REG_DAC_INPUT_A_CH(x)		(AD5706R_R2B | (0x60 + (x * 2)))
#define AD5706R_REG_DAC_DATA_READBACK_CH(x)	(AD5706R_R2B | (0x68 + (x * 2)))
#define AD5706R_REG_TEMP_WARN_STAT		(AD5706R_R2B | 0x70)

#define AD5706R_REG_DIGITAL_STATUS		(AD5706R_R1B | 0x72)
#define AD5706R_REG_BANDGAP_CONTROL		(AD5706R_R1B | 0x73)
#define AD5706R_REG_USER_SPARE_0		(AD5706R_R1B | 0x74)
#define AD5706R_REG_USER_SPARE_1		(AD5706R_R1B | 0x75)
#define AD5706R_REG_USER_SPARE_2		(AD5706R_R1B | 0x76)
#define AD5706R_REG_USER_SPARE_3		(AD5706R_R1B | 0x77)

/*
 * AD5706R_REG_INTERFACE_CONFIG_A
 */
#define AD5706R_INT_CONFIG_A_SW_RESET_MASK	(NO_OS_BIT(7) | NO_OS_BIT(0))
#define AD5706R_INT_CONFIG_A_ADDR_ASC_MASK	NO_OS_BIT(5)
#define AD5706R_INT_CONFIG_A_SDO_EN_MASK	NO_OS_BIT(4)

/*
 * AD5706R_REG_INTERFACE_CONFIG_B
 */
#define AD5706R_INT_CONFIG_B_SINGLE_INSTR_MASK	NO_OS_BIT(7)
#define AD5706R_INT_CONFIG_B_SHORT_INSTR_MASK	NO_OS_BIT(3)

/*
 * AD5706R_REG_DEVICE_CONFIG
 */
#define AD5706R_DEVICE_CONFIG_OPERATING_MODE	NO_OS_GENMASK(1, 0)

/*
 * AD5706R_REG_CHIP_TYPE
 */
#define AD5706R_CHIP_TYPE_MASK			NO_OS_GENMASK(3, 0)
#define AD5706R_CHIP_TYPE			0x4

/*
 * AD5706R_REG_PRODUCT_ID
 */
#define AD5706R_PRODUCT_ID_L			0x30
#define AD5706R_PRODUCT_ID_H			0x41

/*
 * AD5706R_REG_CHIP_GRADE
 */
#define AD5706R_CHIP_GRADE_MASK			NO_OS_GENMASK(7, 4)
#define AD5706R_CHIP_GRADE			0x0
#define AD5706R_DEVICE_REV_MASK			NO_OS_GENMASK(3, 0)
#define AD5706R_DEVICE_REV			0x0

/*
 * AD5706R_REG_SPI_REVISION
 */
#define AD5706R_SPI_TYPE_MASK			NO_OS_GENMASK(7, 6)
#define AD5706R_SPI_TYPE			0x2
#define AD5706R_SPI_VER_MASK			NO_OS_GENMASK(5, 0)
#define AD5706R_SPI_VER				0x4

/*
 * AD5706R_REG_VENDOR_ID
 */
#define AD5706R_VENDOR_ID_L			0x56
#define AD5706R_VENDOR_ID_H			0x04

/*
 * AD5706R_REG_TRANSFER_CONFIG
 */
#define AD5706R_TRANSFER_CONFIG_MASK		NO_OS_BIT(2)

/*
 * AD5706R_REG_INTERFACE_CONFIG_C
 */
#define AD5706R_CRC_ENABLE_MASK			(NO_OS_GENMASK(7, 6) | NO_OS_GENMASK(1, 0))
#define AD5706R_INT_CONFIG_C_STRICT_INSTR_MASK	NO_OS_BIT(3)

/*
 * AD5706R_REG_INTERFACE_STATUS_A
 */
#define AD5706R_NOT_READY_ERR_MASK		NO_OS_BIT(7)
#define AD5706R_CLK_COUNT_ERR_MASK		NO_OS_BIT(4)
#define AD5706R_CRC_ERR_MASK			NO_OS_BIT(3)
#define AD5706R_WRITE_TO_READ_ONLY_REG_ERR_MASK	NO_OS_BIT(2)
#define AD5706R_REG_PARTIAL_ACC_ERR_MASK	NO_OS_BIT(1)
#define AD5706R_ADDR_INV_ERR_MASK		NO_OS_BIT(0)

/*
 * AD5706R_REG_MULTI_DAC_CH_SEL
 */
#define AD5706R_MULTI_DAC_CH_SEL_MASK		NO_OS_GENMASK(3, 0)
#define AD5706R_MULTI_DAC_CH_SEL_MODE(x, ch)	(((x) & 0x1) << (ch))

/*
 * AD5706R_REG_LDAC_SYNC_ASYNC
 */
#define AD5706R_REG_LDAC_SYNC_ASYNC_MASK	NO_OS_GENMASK(3, 0)

/*
 * AD5706R_REG_LDAC_HW_SW
 */
#define AD5706R_LDAC_HW_SW_MASK			NO_OS_GENMASK(3, 0)

/*
 * AD5706R_REG_LDAC_EDGE_SEL_CHn
 */
#define AD5706R_LDAC_EDGE_SEL_CH_MASK		NO_OS_GENMASK(1, 0)

/*
 * AD5706R_REG_OUT_OPERATING_MODE
 */
#define AD5706R_OUT_OPERATING_MASK		NO_OS_GENMASK(3, 0)
#define AD5706R_OUT_OPERATING_MODE(x, ch)	(((x) & 0x1) << (ch))

/*
 * AD5706R_REG_OUT_SWITCH_EN
 */
#define AD5706R_OUT_SWITCH_EN_MASK		NO_OS_GENMASK(3, 0)
#define AD5706R_OUT_SWITCH_EN_MODE(x, ch)	(((x) & 0x1) << (ch))

/*
 * AD5706R_REG_HW_SHUTDOWN_EN
 */
#define AD5706R_HW_SHUTDOWN_EN_MASK		NO_OS_GENMASK(3, 0)
#define AD5706R_HW_SHUTDOWN_EN_MODE(x, ch)	(((x) & 0x1) << (ch))

/*
 * AD5706R_REG_OUT_RANGE_CHn
 */
#define AD5706R_OUT_RANGE_CHn_MASK		NO_OS_GENMASK(1, 0)

/*
 * AD5706R_REG_FUNC_EN
 */
#define AD5706R_FUNC_EN_MASK			NO_OS_GENMASK(3, 0)

/*
 * AD5706R_REG_FUNC_MODE_SEL_CHn
 */
#define AD5706R_FUNC_MODE_SEL_CHn		NO_OS_BIT(0)

/*
 * AD5706R_REG_FUNC_DITHER_PERIOD_CHn
 */
#define AD5706R_FUNC_DITHER_PERIOD_CHn_MASK	NO_OS_GENMASK(2, 0)

/*
 * AD5706R_REG_FUNC_DITHER_PHASE_CHn
 */
#define AD5706R_FUNC_DITHER_PHASE_CHn_MASK	NO_OS_GENMASK(1, 0)

/*
 * AD5706R_REG_MUX_OUT_SEL
 */
#define AD5706R_MUX_OUT_EN			NO_OS_BIT(7)
#define AD5706R_MUX_PARAM_SEL_MASK		NO_OS_GENMASK(4, 0)

/*
 * AD5706R_REG_MUX_OUT_CONTROL
 */
#define AD5706R_MUX_OUT_BUFFER_EN		NO_OS_BIT(0)

/*
 * AD5706R_REG_TEMP_WARN_INT_EN
 */
#define AD5706R_TEMP_WARN_INT_CHn_EN_MASK	NO_OS_GENMASK(3, 0)
#define AD5706R_TEMP_WARN_INT_CHn_EN(x, ch)	(((x) & 0x1) << (ch))

/*
 * AD5706R_REG_MULTI_DAC_SW_LDAC
 */
#define AD5706R_MULTI_DAC_SW_LDAC_TRIG		NO_OS_BIT(0)

/*
 * AD5706R_REG_DAC_SW_LDAC
 */
#define AD5706R_DAC_SW_LDAC_MASK		NO_OS_GENMASK(3, 0)

/*
 * AD5706R_REG_DIGITAL_STATUS
 */
#define AD5706R_INTERFACE_ERR			NO_OS_BIT(2)
#define AD5706R_POWERUP_COMPLETED		NO_OS_BIT(1)
#define AD5706R_RESET_OCCURRED			NO_OS_BIT(0)

/*
 * AD5706R_REG_BANDGAP_CONTROL
 */
#define AD5706R_BANDGAP_CONTROL_EN		NO_OS_BIT(0)

/* Useful defines */
#define AD5706R_CRC_ENABLE_VALUE		(NO_OS_BIT(6) | NO_OS_BIT(1))
#define AD5706R_CRC_DISABLE_VALUE		NO_OS_GENMASK(1, 0)
#define AD5706R_NUM_CH				4
#define AD5706R_MAX_MD_ADDR 			0x03
#define AD5706R_REG_ADDR_MAX	                0x77
#define AD5706R_LDAC_PULSE_US                   1
#define AD5706R_CHANNEL_SEL(ch)			NO_OS_BIT(ch)
#define AD5706R_INTF_CFG_A_DEFAULT		0x10
#define AD5706R_PRODUCT_ID			((AD5706R_PRODUCT_ID_H << 8) | (AD5706R_PRODUCT_ID_L))

/******************************************************************************/
/*************************** Types Declarations *******************************/

/**
 * @enum ad5706r_channels
 * @brief AD5706R list of channels
 */
enum ad5706r_channels {
	/** Channel 0 */
	AD5706R_CH0,
	/** Channel 1 */
	AD5706R_CH1,
	/** Channel 2 */
	AD5706R_CH2,
	/** Channel 3 */
	AD5706R_CH3,
	/** Max number of channels */
	AD5706R_CH_MAX
};

/**
 * @enum ad5706r_dac_resolution
 * @brief AD5706R list of supported data resolution
 */
enum ad5706r_dac_resolution {
	AD5706R_16_BIT_RESOLUTION,
	AD5706R_12_BIT_RESOLUTION,
	AD5706R_10_BIT_RESOLUTION
};

/**
 * @enum ad5706r_vref_select
 * @brief Voltage reference options
 */
enum ad5706r_vref_select {
	/* External source with Vref I/O as input */
	AD5706R_EXTERNAL_VREF_PIN_INPUT,
	/* Internal source with Vref I/O at 2.5V */
	AD5706R_INTERNAL_VREF_PIN_2P5V,
};

/**
 * @enum ad5706r_ldac_edge_trig
 * @brief AD5706R hardware LDAC edge select options
 */
enum ad5706r_ldac_edge_trig {
	/** Trigger at Rising Edge. */
	AD5706R_RISING_EDGE_TRIG,
	/** Trigger at Falling Edge. */
	AD5706R_FALLING_EDGE_TRIG,
	/** Trigger at Any Edge. */
	AD5706R_ANY_EDGE_TRIG
};

/**
 * @enum ad5706r_output_op_mode
 * @brief AD5706R hardware-software LDAC select options
 */
enum ad5706r_output_op_mode {
	AD5706R_SHUTDOWN_SW,
	AD5706R_SHUTDOWN_SW_W_GND,
	AD5706R_NORMAL_SW,
	AD5706R_SHUTDOWN_HW,
	AD5706R_SHUTDOWN_HW_W_GND,
	AD5706R_NORMAL_HW
};

/**
 * @enum ad5706r_mux_out_select
 * @brief Signals to monitor on MUX_OUT pin
 */
enum ad5706r_mux_out_select {
	AD5706R_AGND,
	AD5706R_AVDD,
	AD5706R_VERVREF,
	AD5706R_AGND_SPARE,
	AD5706R_VOUT0,
	AD5706R_VOUT1,
	AD5706R_VOUT2,
	AD5706R_VOUT3,
	AD5706R_IOUT0,
	AD5706R_IOUT1,
	AD5706R_IOUT2,
	AD5706R_IOUT3,
	AD5706R_PVDD0,
	AD5706R_PVDD1,
	AD5706R_PVDD2,
	AD5706R_PVDD3,
	AD5706R_TDIODE0,
	AD5706R_TDIODE1,
	AD5706R_TDIODE2,
	AD5706R_TDIODE3,
	AD5706R_MUXIN0,
	AD5706R_MUXIN1,
	AD5706R_MUXIN2,
	AD5706R_MUXIN3
};

/**
 * @enum ad5706r_dac_mode
 * @brief DAC modes
 */
enum ad5706r_dac_mode {
	AD5706R_DIRECT_WRITE_REG,
	AD5706R_SW_LDAC,
	AD5706R_HW_LDAC,
	AD5706R_SW_TOGGLE,
	AD5706R_HW_TOGGLE,
	AD5706R_SW_DITHER,
	AD5706R_HW_DITHER
};

/**
 * @enum ad5706r_output_range
 * @brief AD5706R list of output range
 */
enum ad5706r_output_range {
	/** 50mA output range */
	AD5706R_50mA,
	/** 150mA output range */
	AD5706R_150mA,
	/** 200mA output range */
	AD5706R_200mA,
	/** 300mA output range */
	AD5706R_300mA
};

/**
 * @enum ad5706r_hw_sw_trig
 * @brief AD5706R HW or SW trigger mode
 */
enum ad5706r_hw_sw_trig {
	/** HW trigger */
	AD5706R_HW_TRIG,
	/** SW trigger */
	AD5706R_SW_TRIG
};

/**
 * @enum ad5706r_function_mode
 * @brief AD5706R list of function modes
 */
enum ad5706r_function_mode {
	/** Toggle Function */
	AD5706R_TOGGLE,
	/** Dither Function */
	AD5706R_DITHER
};

/**
 * @enum ad5706r_dither_period
 * @brief AD5706R list of dither function period settings
 */
enum ad5706r_dither_period {
	/** 4 sample period setting*/
	AD5706R_SAMPLES_4,
	/** 8 sample period setting*/
	AD5706R_SAMPLES_8,
	/** 16 sample period setting*/
	AD5706R_SAMPLES_16,
	/** 32 sample period setting*/
	AD5706R_SAMPLES_32,
	/** 64 sample period setting*/
	AD5706R_SAMPLES_64
};

/**
 * @enum ad5706r_dither_phase
 * @brief AD5706R list of dither function phase settings
 */
enum ad5706r_dither_phase {
	/** 0 Degrees setting*/
	AD5706R_DEGREES_0,
	/** 90 Degrees setting*/
	AD5706R_DEGREES_90,
	/** 180 Degrees setting*/
	AD5706R_DEGREES_180,
	/** 270 Degrees setting*/
	AD5706R_DEGREES_270
};

/**
 * @enum ad5706r_mux_out_state
 * @brief AD5706R list of MUX Out states
 */
enum ad5706r_mux_out_state {
	/** High Impedance*/
	AD5706R_HIGH_Z,
	/** Bypass*/
	AD5706R_BYPASS,
	/** Buffer*/
	AD5706R_BUFFER
};

struct ad5706r_device_spi_settings {
	bool		addr_asc;
	bool		short_instr; /* for 7-bit addressing */
	bool		single_instr; /*  for multibyte read/write */
	bool		stream_length_keep_value; /* to keep stream length */
	uint8_t		stream_mode_length;
};

struct ad5706r_ldac_settings {
	/* Enable function generator mask. */
	uint16_t			func_en_mask;
	/* Function generator mode select. */
	enum ad5706r_function_mode	func_mode[AD5706R_NUM_CH];
	/* LDAC sync/async mask. */
	uint16_t			ldac_sync_async_mask;
	/* LDAC HW/SW mask. */
	uint16_t 			ldac_hw_sw_mask;
	/* LDAC/TGP/DCLK edge trig */
	enum ad5706r_ldac_edge_trig	edge_trig[AD5706R_NUM_CH];
	/* Dither period setting. */
	enum ad5706r_dither_period 	dither_period[AD5706R_NUM_CH];
	/* Dither phase setting. */
	enum ad5706r_dither_phase 	dither_phase[AD5706R_NUM_CH];
	/* Multi-DAC channel mask. */
	uint16_t 			multi_dac_ch_mask;
};

struct ad5706r_transfer_data {
	/* Starting address for transfer */
	uint8_t		addr;
	/* Data to transfer */
	uint8_t		*data;
	/* Size of data to transfer */
	uint32_t	len;
	/* Read transaction if true, write transfer otherwise */
	bool		is_read;
	/* If NULL will be default or last configured will be used */
	struct ad5706r_device_spi_settings *spi_cfg;
};

struct ad5706r_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc		*gpio_ldac_tgp;
	struct no_os_gpio_desc		*gpio_reset;
	struct no_os_gpio_desc		*gpio_shdn;
	/** Reference voltage select */
	enum ad5706r_vref_select 	vref_enable;
	/** Output operating mode. */
	enum ad5706r_output_op_mode	op_mode[AD5706R_NUM_CH];
	/** Span option. */
	enum ad5706r_output_range	range[AD5706R_NUM_CH];
	/** CRC enable. */
	bool				crc_en;
	uint8_t crc_table[NO_OS_CRC8_TABLE_SIZE];
	/* Device SPI Settings */
	struct ad5706r_device_spi_settings	spi_cfg;
	/* LDAC Settings */
	struct ad5706r_ldac_settings	ldac_cfg;
	/* Device Address Settings */
	uint8_t				dev_addr;
	/* Monitor MUX Mode Setting */
	enum ad5706r_mux_out_state 	mux_out_mode;
	/* Monitor MUX parameter select */
	enum ad5706r_mux_out_select 	mux_out_sel;
};

struct ad5706r_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init_prm;
	/* GPIO */
	struct no_os_gpio_init_param	*gpio_ldac_tgp;
	struct no_os_gpio_init_param	*gpio_reset;
	struct no_os_gpio_init_param	*gpio_shdn;
	/** Reference voltage select */
	enum ad5706r_vref_select 	vref_enable;
	/** Output operating mode. */
	enum ad5706r_output_op_mode	op_mode[AD5706R_NUM_CH];
	/** Span option. */
	enum ad5706r_output_range	range[AD5706R_NUM_CH];
	/** CRC enable. */
	bool				crc_en;
	/* Device SPI Settings */
	struct ad5706r_device_spi_settings	spi_cfg;
	/* DAC Mode Initialization per channel */
	enum ad5706r_dac_mode		dac_mode[AD5706R_NUM_CH];
	/* LDAC Settings */
	struct ad5706r_ldac_settings	ldac_cfg;
	/* Device Address Settings */
	uint8_t				dev_addr;
	/* Monitor MUX Mode Setting */
	enum ad5706r_mux_out_state 	mux_out_mode;
	/* Monitor MUX parameter select */
	enum ad5706r_mux_out_select 	mux_out_sel;
};

/** Read from device. */
int ad5706r_spi_reg_read(struct ad5706r_dev *dev, uint32_t reg_addr,
			 uint16_t *reg_data);

/** Multiple read to device. */
int ad5706r_spi_reg_read_multiple(struct ad5706r_dev *dev,
				  uint32_t start_addr,
				  uint8_t count,
				  uint8_t *buff);

/** Write to device. */
int ad5706r_spi_reg_write(struct ad5706r_dev *dev, uint32_t reg_addr,
			  uint16_t reg_data);

/** Multiple write to device. */
int ad5706r_spi_reg_write_multiple(struct ad5706r_dev *dev,
				   uint32_t start_addr,
				   uint8_t count,
				   uint8_t *buff);

/** SPI write to device using a mask. */
int ad5706r_spi_write_mask(struct ad5706r_dev *dev,
			   uint32_t reg_addr,
			   uint32_t mask,
			   uint16_t data);

/** Set reference configuration. */
int ad5706r_set_reference(struct ad5706r_dev *dev,
			  enum ad5706r_vref_select reference_selector);

/** Set output operating mode of a channel. */
int ad5706r_set_operating_mode(struct ad5706r_dev *dev,
			       enum ad5706r_channels chan_sel,
			       enum ad5706r_output_op_mode op_mode);

/** Set out output range of a channel. */
int ad5706r_set_ch_output_range(struct ad5706r_dev *dev,
				enum ad5706r_channels chan_sel,
				enum ad5706r_output_range range_sel);

/** Set Function generator enable. */
int ad5706r_func_en(struct ad5706r_dev *dev, uint16_t en_mask);

/** Set Function generator mode. */
int ad5706r_func_mode_select(struct ad5706r_dev *dev,
			     enum ad5706r_channels chan_sel,
			     enum ad5706r_function_mode func_mode_sel);

/** Set period for dither function generator. */
int ad5706r_set_dither_period(struct ad5706r_dev *dev,
			      enum ad5706r_channels ch,
			      enum ad5706r_dither_period period);

/** Set phase for dither function generator. */
int ad5706r_set_dither_phase(struct ad5706r_dev *dev,
			     enum ad5706r_channels ch,
			     enum ad5706r_dither_phase phase);

/** Set HW/SW LDAC. */
int ad5706r_set_hw_sw_ldac(struct ad5706r_dev *dev, uint16_t mask_hw_sw_ldac);

/** Set sync/async ldac configuration. */
int ad5706r_set_sync_async_ldac(struct ad5706r_dev *dev,
				uint16_t mask_ldac_sync_async);

/** Set ldac config for single channel. */
int ad5706r_set_ldac_config(struct ad5706r_dev *dev,
			    enum ad5706r_channels chn,
			    enum ad5706r_dac_mode dac_mode,
			    struct ad5706r_ldac_settings *ldac_settings);

/** Trigger Multi DAC SW LDAC. */
int ad5706r_multi_dac_sw_ldac_trigger(struct ad5706r_dev *dev);

/** Set ldac config for multiple channels. */
int ad5706r_set_multi_ldac_config(struct ad5706r_dev *dev,
				  uint16_t ch_mask,
				  enum ad5706r_dac_mode dac_mode,
				  struct ad5706r_ldac_settings *ldac_settings);

/** Set dac value on DAC input register A. */
int ad5706r_set_dac_a_value(struct ad5706r_dev *dev,
			    enum ad5706r_channels chn,
			    uint16_t dac_value);

/** Set dac values on DAC input registers A and B. */
int ad5706r_set_dac_b_value(struct ad5706r_dev *dev,
			    enum ad5706r_channels chn,
			    uint16_t dac_value);

/** Set dac value on Multi DAC input register A. */
int ad5706r_set_multi_dac_a_value(struct ad5706r_dev *dev,
				  uint16_t dac_value);

/** Set the multiplexer output mode. */
int ad5706r_set_mux_out_mode(struct ad5706r_dev *dev,
			     enum ad5706r_mux_out_state mux_output_mode_sel);

/** Set the multiplexer output select register. */
int ad5706r_set_mux_out_select(struct ad5706r_dev *dev,
			       enum ad5706r_mux_out_select mux_output_sel);

/** HW LDAC trigger. */
int ad5706r_hw_ldac_trigger(struct ad5706r_dev *dev);

/** Set LDAC/TGP/DCLK edge trigger. */
int ad5706r_set_edge_trigger(struct ad5706r_dev *dev,
			     enum ad5706r_channels chn,
			     enum ad5706r_ldac_edge_trig trig_edge);

/** SW LDAC trigger. */
int ad5706r_sw_ldac_trigger(struct ad5706r_dev *dev);

/** Soft reset */
int ad5706r_sw_reset(struct ad5706r_dev *dev);

/** Hard reset */
int ad5706r_hw_reset(struct ad5706r_dev *dev);

/** Configures the device. */
int ad5706r_device_config(struct ad5706r_dev *dev,
			  struct ad5706r_init_param *init_param);

/** Set SPI device settings */
int ad5706r_set_device_spi(struct ad5706r_dev *dev,
			   struct ad5706r_device_spi_settings *spi_settings);

/** Enable CRC */
int ad5706r_crc_enable(struct ad5706r_dev *dev, bool crc_enable);

/** Set SHDN pin state. */
int ad5706r_shdn_trigger(struct ad5706r_dev *dev, bool shdn_state);

/** Initialize the device. */
int ad5706r_init(struct ad5706r_dev **device,
		 struct ad5706r_init_param *init_param);

/** Free the resources allocated by ad5706r_init(). */
int ad5706r_remove(struct ad5706r_dev *dev);

#endif /* AD5706R_H_ */
