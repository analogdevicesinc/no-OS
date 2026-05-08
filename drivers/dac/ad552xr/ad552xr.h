/***************************************************************************//**
 *   @file   ad552xr.h
 *   @brief  Header file of AD552XR Driver.
 *   @author Naga Himanshu Indraganti (naga.indraganti@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#ifndef AD552XR_H_
#define AD552XR_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

#define AD552XR_R1B							(1ul << 16)		// 1-byte register address
#define AD552XR_R2B							(2ul << 16)  	// 2-byte register address
#define AD552XR_LEN(x)						((x) >> 16)		// Get register byte length
#define AD552XR_ADDR(x)						((x) & 0xFFFF)
#define AD552XR_REG_CH_OFFSET(x)			((x) * 2)

#define AD552XR_READ_BIT					NO_OS_BIT(7)
#define AD552XR_ADDR_MASK					(~AD552XR_READ_BIT)
#define AD552XR_WRITE_BIT_LONG_INSTR		0x00
#define AD552XR_SCRATCH_PAD_TEST_VAL		0xAB

#define AD552XR_MULTIBYTE_REG_START			0x14
#define AD552XR_MULTIBYTE_REG_END			0x71

/*
 * AD552XR Core Registers
 */
#define AD552XR_REG_INTERFACE_CONFIG_A		(AD552XR_R1B | 0x00)
#define AD552XR_REG_INTERFACE_CONFIG_B		(AD552XR_R1B | 0x01)
#define AD552XR_REG_DEVICE_CONFIG			(AD552XR_R1B | 0x02)
#define AD552XR_REG_CHIP_TYPE				(AD552XR_R1B | 0x03)
#define AD552XR_REG_PRODUCT_ID_L			(AD552XR_R1B | 0x04)
#define AD552XR_REG_PRODUCT_ID_H			(AD552XR_R1B | 0x05)
#define AD552XR_REG_CHIP_GRADE				(AD552XR_R1B | 0x06)
#define AD552XR_REG_SCRATCH_PAD				(AD552XR_R1B | 0x0A)
#define AD552XR_REG_SPI_REVISION			(AD552XR_R1B | 0x0B)
#define AD552XR_REG_VENDOR_L				(AD552XR_R1B | 0x0C)
#define AD552XR_REG_VENDOR_H				(AD552XR_R1B | 0x0D)
#define AD552XR_REG_STREAM_MODE				(AD552XR_R1B | 0x0E)
#define AD552XR_REG_TRANSFER_CONFIG			(AD552XR_R1B | 0x0F)
#define AD552XR_REG_INTERFACE_CONFIG_C		(AD552XR_R1B | 0x10)
#define AD552XR_REG_INTERFACE_STATUS_A		(AD552XR_R1B | 0x11)

/*
 * AD552XR DAC Configuration Registers
 */
#define AD552XR_REG_MULTI_INPUT_SEL			(AD552XR_R2B | 0x14)
#define AD552XR_REG_LDAC_SYNC_ASYNC			(AD552XR_R2B | 0x16)
#define AD552XR_REG_LDAC_HW_SW				(AD552XR_R2B | 0x18)
#define AD552XR_REG_LDAC_HW_SRC_CH(x)		(AD552XR_R2B | (0x1A + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_OUT_EN					(AD552XR_R2B | 0x3A)
#define AD552XR_REG_OUT_RANGE_CH(x)			(AD552XR_R2B | (0x3C + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_CAL_GAIN_CH(x)			(AD552XR_R2B | (0x5C + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_CAL_OFFSET_CH(x)		(AD552XR_R2B | (0x7C + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_EN					(AD552XR_R2B | 0x9C)
#define AD552XR_REG_FUNC_MODE_SEL_CH(x)		(AD552XR_R2B | (0x9E + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_DAC_INPUT_B_CH(x)	(AD552XR_R2B | (0xBE + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_DITHER_PERIOD_CH(x)(AD552XR_R2B | (0xDE + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_DITHER_PHASE_CH(x)	(AD552XR_R2B | (0xFE + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_RAMP_STEP_CH(x)	(AD552XR_R2B | (0x11E + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_INT_EN(x)			(AD552XR_R2B | 0x13E)
#define AD552XR_REG_MUX_OUT_SEL				(AD552XR_R2B | 0x140)
#define AD552XR_REG_MULTI_SW_LDAC			(AD552XR_R2B | 0x142)
#define AD552XR_REG_MULTI_INPUT				(AD552XR_R2B | 0x144)
#define AD552XR_REG_SW_LDAC					(AD552XR_R2B | 0x146)
#define AD552XR_REG_DAC_INPUT_A_CH(x)		(AD552XR_R2B | (0x148 + AD552XR_REG_CH_OFFSET(x)))
#define AD552XR_REG_FUNC_INT_STAT			(AD552XR_R2B | 0x168)
#define AD552XR_REG_DAC_DATA_READBACK_CH(x)	(AD552XR_R2B | (0x16A + AD552XR_REG_CH_OFFSET(x)))

/*
 * AD552XR DAC Product Specific Registers
 */
#define AD552XR_REG_TSENS_EN				(AD552XR_R2B | 0x18A)
#define AD552XR_REG_TSENS_ALERT_FLAG		(AD552XR_R2B | 0x18C)
#define AD552XR_REG_TSENS_SHTD_FLAG			(AD552XR_R2B | 0x18E)
#define AD552XR_REG_TSENS_ALERT_STAT		(AD552XR_R2B | 0x190)
#define AD552XR_REG_TSENS_SHTD_STAT			(AD552XR_R2B | 0x192)
#define AD552XR_REG_ALARMB_TSENS_EN			(AD552XR_R2B | 0x194)
#define AD552XR_REG_ALARMB_TSENS_SEL		(AD552XR_R2B | 0x196)
#define AD552XR_REG_TSENS_SHTD_EN_CH		(AD552XR_R2B | 0x198)
#define AD552XR_REG_DAC_DIS_DEGLITCH_CH		(AD552XR_R2B | 0x19A)
#define AD552XR_REG_DAC_INT_EN				(AD552XR_R2B | 0x19C)
#define AD552XR_REG_ALL_FUNC_INT_STAT		(AD552XR_R2B | 0x19E)
#define AD552XR_REG_FUNC_BUSY				(AD552XR_R2B | 0x1A0)
#define AD552XR_REG_REF_SEL					(AD552XR_R2B | 0x1A2)
#define AD552XR_REG_INIT_CRC_ERR_STAT		(AD552XR_R2B | 0x1A4)

/*
 * AD552XR_REG_INTERFACE_CONFIG_A
 */
#define AD552XR_INT_CONFIG_A_SW_RESET_MASK	(NO_OS_BIT(7) | NO_OS_BIT(0))
#define AD552XR_INT_CONFIG_A_ADDR_ASC_MASK	NO_OS_BIT(5)
#define AD552XR_INT_CONFIG_A_SDO_EN_MASK	NO_OS_BIT(4)

/*
 * AD552XR_REG_INTERFACE_CONFIG_B
 */
#define AD552XR_INT_CONFIG_B_SINGLE_INSTR_MASK	NO_OS_BIT(7)

/*
 * AD552XR_REG_DEVICE_CONFIG
 */
#define AD552XR_DEVICE_CONFIG_OPERATING_MODE	NO_OS_GENMASK(1, 0)

/*
 * AD552XR_REG_CHIP_TYPE
 */
#define AD552XR_CHIP_TYPE_MASK				NO_OS_GENMASK(3, 0)
#define AD552XR_CHIP_TYPE					0x4

/*
 * AD552XR_REG_PRODUCT_ID
 */
#define AD552XR_PRODUCT_ID_H				0x41

#define AD552XR_PRODUCT_ID(x)				((AD552XR_PRODUCT_ID_H << 8) | (x))
#define AD5529R_PID_16BIT_16CH_WLCSP		AD552XR_PRODUCT_ID(0x4A)

/*
 * AD552XR_REG_CHIP_GRADE
 */
#define AD552XR_CHIP_GRADE_MASK				NO_OS_GENMASK(7, 4)
#define AD552XR_CHIP_GRADE					0x0
#define AD552XR_DEVICE_REV_MASK				NO_OS_GENMASK(3, 0)
#define AD552XR_DEVICE_REV                 	0x0

/*
 * AD552XR_REG_SPI_REVISION
 */
#define AD552XR_SPI_TYPE_MASK				NO_OS_GENMASK(7, 6)
#define AD552XR_SPI_TYPE					0x2
#define AD552XR_SPI_VER_MASK				NO_OS_GENMASK(5, 0)
#define AD552XR_SPI_VER						0x5

/*
 * AD552XR_REG_VENDOR_ID
 */
#define AD552XR_VENDOR_ID_L					0x56
#define AD552XR_VENDOR_ID_H					0x04

/*
 * AD552XR_REG_TRANSFER_CONFIG
 */
#define AD552XR_TRANSFER_CONFIG_MASK        NO_OS_BIT(2)

/*
 * AD552XR_REG_INTERFACE_CONFIG_C
 */
#define AD552XR_INT_CONFIG_C_STRICT_REGISTER_A_ACCESS_MASK		NO_OS_BIT(5)

/*
 * AD552XR_REG_INTERFACE_STATUS_A
 */
#define AD552XR_NOT_READY_ERR_MASK			NO_OS_BIT(7)
#define AD552XR_CLK_COUNT_ERR_MASK			NO_OS_BIT(4)
#define AD552XR_CRC_ERR_MASK				NO_OS_BIT(3)
#define AD552XR_WRITE_TO_READ_ONLY_REG_ERR_MASK		NO_OS_BIT(2)
#define AD552XR_REG_PARTIAL_ACC_ERR_MASK	NO_OS_BIT(1)
#define AD552XR_ADDR_INV_ERR_MASK			NO_OS_BIT(0)

/*
 * AD552XR_REG_LDAC_HW_SRC_CHn
 */
#define AD552XR_LDAC_HW_SEL_CH_MASK			NO_OS_GENMASK(9, 8)
#define AD552XR_LDAC_HW_EDGE_SEL_CH_MASK	NO_OS_GENMASK(1, 0)

/*
 * AD552XR_OUT_RANGE_CHn
 */
#define AD552XR_OUT_RANGE_CHn_MASK			NO_OS_GENMASK(2, 0)

/*
 * AD552XR_CAL_GAIN_CHn
 */
#define AD552XR_CAL_GAIN_CHn_MASK			NO_OS_GENMASK(7, 0)

/*
 * AD552XR_FUNC_MODE_SEL_CHn
 */
#define AD552XR_FUNC_MODE_SEL_CHn			NO_OS_GENMASK(1, 0)

/*
 * AD552XR_REG_FUNC_DITHER_PERIOD_CHn
 */
#define AD552XR_FUNC_DITHER_PERIOD_CHn_MASK	NO_OS_GENMASK(2, 0)

/*
 * AD552XR_REG_FUNC_DITHER_PHASE_CHn
 */
#define AD552XR_FUNC_DITHER_PHASE_CHn_MASK	NO_OS_GENMASK(1, 0)

/*
 * AD552XR_REG_FUNC_RAMP_STEP_CHn
 */
#define AD552XR_FUNC_RAMP_STEP_CHn_MASK		NO_OS_GENMASK(7, 0)

/*
 * AD552XR_REG_MUX_OUT_SEL
 */
#define AD552XR_MUX_OUT_EN					NO_OS_BIT(7)
#define AD552XR_MUX_PARAM_SEL_MASK			NO_OS_GENMASK(5, 0)

/*
 * AD552XR_REG_MULTI_SW_LDAC
 */
#define AD552XR_MULTI_DAC_SW_LDAC_TRIG		NO_OS_BIT(0)

/*
 * AD552XR_REG_REF_SEL
 */
#define AD552XR_VREF_SEL_MASK				NO_OS_BIT(0)

/* Useful defines */
#define AD552XR_MAX_NUM_CH					16
#define AD552XR_CHANNEL_SEL(ch)				NO_OS_BIT(ch)
#define AD552XR_NUM_MD_ADDR_LINES			2
#define AD552XR_INTF_CFG_A_DEFAULT			0x10
#define AD552XR_NUM_REGS					51

/**
 * @enum ad552xr_type
 * @brief AD552XR list of supported device types
 */
enum ad552xr_type {
	ID_AD5529R,
	AD552XR_NUM_TYPES
};

/**
 * @enum ad552xr_dac_resolution
 * @brief AD552XR list of supported data resolution
 */
enum ad552xr_dac_resolution {
	AD552XR_RESOLUTION_16_BIT = 16
};

/**
 * @enum ad552xr_num_channels
 * @brief AD552XR list of supported number of channels
 */
enum ad552xr_num_channels {
	AD552XR_NUM_CHANNELS_16 = 16
};

/**
 * @enum ad552xr_vref_select
 * @brief Voltage reference options
 */
enum ad552xr_vref_select {
	/* External source with Vref I/O as input */
	AD552XR_EXTERNAL_VREF,
	/* Internal source with Vref I/O at 4.096V */
	AD552XR_INTERNAL_VREF,
};

/**
 * @enum ad552xr_ldac_edge_trig
 * @brief AD552XR hardware LDAC edge select options
 */
enum ad552xr_ldac_edge_trig {
	/** Trigger at Rising Edge. */
	RISING_EDGE_TRIG,
	/** Trigger at Falling Edge. */
	FALLING_EDGE_TRIG,
	/** Trigger at Any Edge. */
	ANY_EDGE_TRIG
};

/**
 * @enum ad552xr_operating_mode
 * @brief AD552XR hardware-software LDAC select options
 */
enum ad552xr_operating_mode {
	SHUTDOWN_SW,
	SHUTDOWN_SW_W_GND,
	NORMAL_SW,
	SHUTDOWN_HW,
	SHUTDOWN_HW_W_GND,
	NORMAL_HW
};

/**
 * @enum ad552xr_output_range
 * @brief AD552XR list of output range
 */
enum ad552xr_output_range {
	/** 0V to 5V output range */
	AD552XR_UNIPOLAR_5V,
	/** 0V to 10V output range */
	AD552XR_UNIPOLAR_10V,
	/** 0V to 20V output range */
	AD552XR_UNIPOLAR_20V,
	/** 0V to 40V output range */
	AD552XR_UNIPOLAR_40V,
	/** -5V to +5V output range */
	AD552XR_BIPOLAR_5V,
	/** -10V to +10V output range */
	AD552XR_BIPOLAR_10V,
	/** -15V to +15V output range */
	AD552XR_BIPOLAR_15V,
	/** -20V to +20V output range */
	AD552XR_BIPOLAR_20V,
};

/**
 * @enum ad552xr_mux_out_select
 * @brief Signals to monitor on MUX_OUT pin
 */
enum ad552xr_mux_out_select {
	AD552XR_MUX_OUT_AGND,
	AD552XR_MUX_OUT_VOUT0,
	AD552XR_MUX_OUT_VOUT1,
	AD552XR_MUX_OUT_VOUT2,
	AD552XR_MUX_OUT_VOUT3,
	AD552XR_MUX_OUT_VOUT4,
	AD552XR_MUX_OUT_VOUT5,
	AD552XR_MUX_OUT_VOUT6,
	AD552XR_MUX_OUT_VOUT7,
	AD552XR_MUX_OUT_VOUT8,
	AD552XR_MUX_OUT_VOUT9,
	AD552XR_MUX_OUT_VOUT10,
	AD552XR_MUX_OUT_VOUT11,
	AD552XR_MUX_OUT_VOUT12,
	AD552XR_MUX_OUT_VOUT13,
	AD552XR_MUX_OUT_VOUT14,
	AD552XR_MUX_OUT_VOUT15,
	AD552XR_MUX_OUT_IOUT0,
	AD552XR_MUX_OUT_IOUT1,
	AD552XR_MUX_OUT_IOUT2,
	AD552XR_MUX_OUT_IOUT3,
	AD552XR_MUX_OUT_IOUT4,
	AD552XR_MUX_OUT_IOUT5,
	AD552XR_MUX_OUT_IOUT6,
	AD552XR_MUX_OUT_IOUT7,
	AD552XR_MUX_OUT_IOUT8,
	AD552XR_MUX_OUT_IOUT9,
	AD552XR_MUX_OUT_IOUT10,
	AD552XR_MUX_OUT_IOUT11,
	AD552XR_MUX_OUT_IOUT12,
	AD552XR_MUX_OUT_IOUT13,
	AD552XR_MUX_OUT_IOUT14,
	AD552XR_MUX_OUT_IOUT15,
	AD552XR_MUX_OUT_PVDD3,
	AD552XR_MUX_OUT_TEMPSENS0,
	AD552XR_MUX_OUT_TEMPSENS1,
	AD552XR_MUX_OUT_TEMPSENS2,
	AD552XR_MUX_OUT_TEMPSENS3,
	AD552XR_MUX_OUT_VMON,
	AD552XR_MUX_OUT_IMON,
	AD552XR_MUX_OUT_HZ,
};

/**
 * @enum ad552xr_hw_sw_ldac
 * @brief AD552XR HW or SW LDAC mode
 */
enum ad552xr_hw_sw_ldac {
	/** HW LDAC */
	AD552XR_HW_LDAC,
	/** SW LDAC  */
	AD552XR_SW_LDAC
};

/**
 * @enum ad552xr_function_mode
 * @brief DAC Function modes
 */
enum ad552xr_function_mode {
	/** Toggle Function */
	AD552XR_FUNCTION_MODE_TOGGLE,
	/** Dither Function */
	AD552XR_FUNCTION_MODE_DITHER,
	/** Ramp Sawtooth Function */
	AD552XR_FUNCTION_MODE_SAWTOOTH,
	/** Ramp Triangular Function */
	AD552XR_FUNCTION_MODE_TRIANGULAR
};

/**
 * @enum ad552xr_ldac_toggle_sel
 * @brief LDAC Toggle pin select
 */
enum ad552xr_ldac_toggle_sel {
	AD552XR_LDAC_TGP_0,
	AD552XR_LDAC_TGP_1,
	AD552XR_LDAC_TGP_2,
	AD552XR_LDAC_TGP_3,
	AD552XR_NUM_LDAC_TGP_PINS
};

/**
 * @enum ad552xr_dither_period
 * @brief AD552XR list of dither function period settings
 */
enum ad552xr_dither_period {
	/** 128 sample period setting */
	SAMPLES_128,
	/** 64 sample period setting */
	SAMPLES_64,
	/** 32 sample period setting */
	SAMPLES_32,
	/** 16 sample period setting */
	SAMPLES_16,
	/** 8 sample period setting */
	SAMPLES_8,
	/** 4 sample period setting */
	SAMPLES_4,
	/** 2 sample period setting */
	SAMPLES_2,
};

/**
 * @enum ad552xr_dither_phase
 * @brief AD552XR list of dither function phase settings
 */
enum ad552xr_dither_phase {
	/** 0 Degrees setting */
	DEGREES_0,
	/** 90 Degrees setting */
	DEGREES_90,
	/** 180 Degrees setting */
	DEGREES_180,
	/** 270 Degrees setting */
	DEGREES_270
};

/**
 * @struct ad552xr_device_spi_settings
 * @brief AD552XR device SPI settings
 */
struct ad552xr_device_spi_settings {
	bool		addr_asc; //Address ascension enable
	bool		short_instr; // for 7-bit addressing
	bool		single_instr; // for multibyte read/write
	bool		stream_length_keep_value; // to keep stream length
	uint8_t		stream_mode_length;
};

/**
 * @struct ad552xr_ldac_settings
 * @brief AD552XR device LDAC settings
 */
struct ad552xr_ldac_settings {
	/* Enable function generator mask. */
	uint16_t			func_en_mask;
	/* Enable output mask. */
	uint16_t			output_en_mask;
	/* Function generator mode select. */
	enum ad552xr_function_mode	func_mode[AD552XR_MAX_NUM_CH];
	/* LDAC sync/async mask. */
	uint16_t			ldac_sync_async_mask;
	/* LDAC HW/SW mask. */
	uint16_t 			ldac_hw_sw_mask;
	/* LDAC/TGP/DCLK edge trig */
	enum ad552xr_ldac_edge_trig	edge_trig[AD552XR_MAX_NUM_CH];
	/* LDAC Toggle pin select */
	enum ad552xr_ldac_toggle_sel toggle_sel[AD552XR_MAX_NUM_CH];
	/* Dither period setting. */
	enum ad552xr_dither_period 	dither_period[AD552XR_MAX_NUM_CH];
	/* Dither phase setting. */
	enum ad552xr_dither_phase 	dither_phase[AD552XR_MAX_NUM_CH];
	/* Ramp step size. */
	uint8_t ramp_step[AD552XR_MAX_NUM_CH];
	/* Multi-DAC channel mask. */
	uint16_t 			multi_dac_ch_mask;
};

/**
 * @struct ad552xr_transfer_data
 * @brief AD552XR transfer data structure
 */
struct ad552xr_transfer_data {
	/* Starting address for transfer */
	uint8_t		addr;
	/* Data to transfer */
	uint8_t		*data;
	/* Size of data to transfer */
	uint32_t	len;
	/* Read transaction if true, write transfer otherwise */
	bool		is_read;
	/* If NULL will be default or last configured will be used */
	struct ad552xr_device_spi_settings *spi_cfg;
};


/**
 * @struct ad552xr_dev_info
 * @brief AD552XR active device info
 */
struct ad552xr_dev_info {
	/** Device type */
	enum ad552xr_type type;
	/** Device resolution */
	enum ad552xr_dac_resolution	resolution;
	/** Number of channels for the device */
	enum ad552xr_num_channels num_channels;
};

/**
 * @struct ad552xr_dev
 * @brief AD552XR device descriptor
 */
struct ad552xr_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* Reset_B GPIO */
	struct no_os_gpio_desc *gpio_resetb;
	/* Clear_B GPIO */
	struct no_os_gpio_desc *gpio_clearb;
	/* Alarm_B GPIO */
	struct no_os_gpio_desc *gpio_alarmb;
	/* Multi drop ADDR GPIO Lines */
	struct no_os_gpio_desc *gpio_md_addr[AD552XR_NUM_MD_ADDR_LINES];
	/* LDAC TGPx GPIO pins */
	struct no_os_gpio_desc *gpio_ldac_tgp[AD552XR_NUM_LDAC_TGP_PINS];
	/** Output operating mode. */
	enum ad552xr_operating_mode	op_mode[AD552XR_MAX_NUM_CH];
	/** Span option. */
	enum ad552xr_output_range	range[AD552XR_MAX_NUM_CH];
	/* Reference voltage selection */
	enum ad552xr_vref_select 	vref;
	/* Device SPI Settings */
	struct ad552xr_device_spi_settings spi_cfg;
	/* LDAC Settings */
	struct ad552xr_ldac_settings	ldac_cfg;
	/* Device Address */
	uint8_t	dev_addr;
	/** Device information. */
	struct ad552xr_dev_info dev_info;
	/* Monitor MUX parameter select */
	enum ad552xr_mux_out_select mux_out_sel;
};

/**
 * @struct ad552xr_init_param
 * @brief Structure holding the initialization parameters for AD552XR
 */
struct ad552xr_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init_prm;
	/* Reset_B GPIO */
	struct no_os_gpio_init_param *gpio_resetb;
	/* Clear_B GPIO */
	struct no_os_gpio_init_param *gpio_clearb;
	/* Alarm_B GPIO */
	struct no_os_gpio_init_param *gpio_alarmb;
	/* Multi drop ADDR GPIO Lines */
	struct no_os_gpio_init_param *gpio_md_addr[AD552XR_NUM_MD_ADDR_LINES];
	/* LDAC TGPx GPIO pins */
	struct no_os_gpio_init_param *gpio_ldac_tgp[AD552XR_NUM_LDAC_TGP_PINS];
	/** Output operating mode. */
	enum ad552xr_operating_mode	op_mode[AD552XR_MAX_NUM_CH];
	/** Span option. */
	enum ad552xr_output_range	range[AD552XR_MAX_NUM_CH];
	/* Reference voltage selection */
	enum ad552xr_vref_select 	vref;
	/* Device SPI Settings */
	struct ad552xr_device_spi_settings spi_cfg;
	/* LDAC Settings */
	struct ad552xr_ldac_settings	ldac_cfg;
	/* Device Address */
	uint8_t	dev_addr;
	/* Device type */
	enum ad552xr_type type;
	/* Monitor MUX parameter select */
	enum ad552xr_mux_out_select mux_out_sel;
};

/** Read from device. */
int ad552xr_spi_reg_read(struct ad552xr_dev *dev, uint32_t reg_addr,
			 uint16_t *reg_data);

/** Write to device. */
int ad552xr_spi_reg_write(struct ad552xr_dev *dev, uint32_t reg_addr,
			  uint16_t reg_data);

/** SPI write to device using a mask. */
int ad552xr_spi_write_mask(struct ad552xr_dev *dev, uint32_t reg_addr,
			   uint32_t mask, uint16_t data);

/** Set reference configuration. */
int ad552xr_set_reference(struct ad552xr_dev *dev,
			  enum ad552xr_vref_select ref_sel);

/** Set out output range of a channel. */
int ad552xr_set_ch_output_range(struct ad552xr_dev *dev,
				uint8_t ch,
				enum ad552xr_output_range range_sel);

/** Set channel output enable */
int ad552xr_channel_output_en(struct ad552xr_dev *dev, uint8_t ch, bool en);

/** Set Function generator enable. */
int ad552xr_func_en(struct ad552xr_dev *dev, uint8_t ch, bool en);

/** Set Function generator mode. */
int ad552xr_func_mode_select(struct ad552xr_dev *dev,
			     uint8_t ch,
			     enum ad552xr_function_mode func_mode_sel);

/** Set period for dither function generator. */
int ad552xr_set_dither_period(struct ad552xr_dev *dev, uint8_t ch,
			      enum ad552xr_dither_period period);

/** Set phase for dither function generator. */
int ad552xr_set_dither_phase(struct ad552xr_dev *dev, uint8_t ch,
			     enum ad552xr_dither_phase phase);

/** Set ramp step size. */
int ad552xr_set_ramp_step_size(struct ad552xr_dev *dev, uint8_t ch,
			       uint8_t step_size);

/** Set HW/SW LDAC. */
int ad552xr_set_hw_sw_ldac(struct ad552xr_dev *dev,
			   uint8_t ch,
			   enum ad552xr_hw_sw_ldac ldac_mode_sel);

/** Set sync/async ldac configuration. */
int ad552xr_set_sync_async_ldac(struct ad552xr_dev *dev,
				uint8_t ch,
				bool is_sync);

/** Set LDAC/TGP edge trigger. */
int ad552xr_set_hw_edge_trigger(struct ad552xr_dev *dev,
				uint8_t chn,
				enum ad552xr_ldac_edge_trig trig_edge);

/** Set LDAC/TGP source pin.  */
int ad552xr_set_hw_ldac_toggle_pin(struct ad552xr_dev *dev,
				   uint8_t ch,
				   enum ad552xr_ldac_toggle_sel ldac_hw_sel);

/** Set dac value on DAC input register A. */
int ad552xr_set_dac_a_value(struct ad552xr_dev *dev,
			    uint8_t ch,
			    uint16_t dac_value);

/** Set dac values on DAC input registers A and B. */
int ad552xr_set_dac_b_value(struct ad552xr_dev *dev,
			    uint8_t ch,
			    uint16_t dac_value);

/** Trigger the LDAC using hardware. */
int ad552xr_hw_ldac_trigger(struct ad552xr_dev *dev,
			    enum ad552xr_ldac_toggle_sel ldac_hw_sel,
			    uint32_t delay_us);

/** Trigger the LDAC using software. */
int ad552xr_sw_ldac_trigger(struct ad552xr_dev *dev);

/** Set device spi settings. */
int ad552xr_set_device_spi(struct ad552xr_dev *dev,
			   struct ad552xr_device_spi_settings *spi_settings);

/** Set the multiplexer output select register. */
int ad552xr_set_mux_out_select(struct ad552xr_dev *dev,
			       enum ad552xr_mux_out_select mux_output_sel);

/** Soft reset */
int ad552xr_sw_reset(struct ad552xr_dev *dev);

/** Hard reset */
int ad552xr_hw_reset(struct ad552xr_dev *dev);

/** Initialize the device. */
int ad552xr_init(struct ad552xr_dev **device,
		 struct ad552xr_init_param *init_param);

/** Free the resources allocated by ad552xr_init(). */
int ad552xr_remove(struct ad552xr_dev *dev);

#endif // AD552XR_H_
