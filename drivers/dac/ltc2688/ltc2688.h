/***************************************************************************//**
 *   @file   LTC2688.h
 *   @brief  Header file of LTC2688 Driver
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef __LTC2688_H__
#define __LTC2688_H__

#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "errno.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
#define LTC2688_DAC_CHANNELS	16

#define LTC2688_CMD_CH_CODE(x)			(0x00 + x)
#define LTC2688_CMD_CH_SETTING(x)		(0x10 + x)
#define LTC2688_CMD_CH_OFFSET(x)		(0X20 + x)
#define LTC2688_CMD_CH_GAIN(x)			(0x30 + x)
#define LTC2688_CMD_CH_CODE_UPDATE(x)		(0x40 + x)
#define LTC2688_CMD_CH_CODE_UPDATE_ALL(x)	(0x50 + x)
#define LTC2688_CMD_CH_UPDATE(x)		(0x60 + x)

#define LTC2688_CMD_CONFIG_REG			0x70
#define LTC2688_CMD_POWERDOWN_REG		0x71
#define LTC2688_CMD_A_B_SELECT_REG		0x72
#define LTC2688_CMD_SW_TOGGLE_REG		0x73
#define LTC2688_CMD_TOGGLE_DITHER_EN_REG	0x74
#define LTC2688_CMD_MUX_CTRL_REG		0x75
#define LTC2688_CMD_FAULT_REG			0x76
#define LTC2688_CMD_CODE_ALL			0x78
#define LTC2688_CMD_CODE_UPDATE_ALL		0x79
#define LTC2688_CMD_SETTING_ALL			0x7A
#define LTC2688_CMD_SETTING_UPDATE_ALL		0x7B
#define LTC2688_CMD_UPDATE_ALL			0x7C
#define LTC2688_CMD_NOOP			0xFF

#define LTC2688_READ_OPERATION			0x80

/* Channel Settings */
#define LTC2688_CH_SPAN_MSK			GENMASK(3, 0)
#define LTC2688_CH_SPAN(x)			field_prep(LTC2688_CH_SPAN_MSK, x)
#define LTC2688_CH_TD_SEL_MSK			GENMASK(5, 4)
#define LTC2688_CH_TD_SEL(x)			field_prep(LTC2688_CH_TD_SEL_MSK, x)
#define LTC2688_CH_DIT_PER_MSK			GENMASK(8, 6)
#define LTC2688_CH_DIT_PER(x)			field_prep(LTC2688_CH_DIT_PER_MSK, x)
#define LTC2688_CH_DIT_PH_MSK			GENMASK(10, 9)
#define LTC2688_CH_DIT_PH(x)			field_prep(LTC2688_CH_DIT_PH_MSK, x)
#define LTC2688_CH_MODE				BIT(11)

/* Configuration register */
#define LTC2688_CONFIG_RST			BIT(15)

#define LTC2688_PWDN(x)				(1 << ((x) & 0xF))
#define LTC2688_DITH_EN(x)			(1 << ((x) & 0xF))


/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ltc2688_voltage_range {
	LTC2688_VOLTAGE_RANGE_0V_5V,
	LTC2688_VOLTAGE_RANGE_0V_10V,
	LTC2688_VOLTAGE_RANGE_M5V_5V,
	LTC2688_VOLTAGE_RANGE_M10V_10V,
	LTC2688_VOLTAGE_RANGE_M15V_15V,
};

struct ltc2688_span_tbl {
	int min;
	int max;
};

enum ltc2688_dither_period {
	LTC2688_DITH_PERIOD_4,
	LTC2688_DITH_PERIOD_8,
	LTC2688_DITH_PERIOD_16,
	LTC2688_DITH_PERIOD_32,
	LTC2688_DITH_PERIOD_64
};

enum ltc2688_dither_phase {
	LTC2688_DITH_PHASE_0,
	LTC2688_DITH_PHASE_90,
	LTC2688_DITH_PHASE_180,
	LTC2688_DITH_PHASE_270
};

enum ltc2688_a_b_register {
	LTC2688_SELECT_A_REG,
	LTC2688_SELECT_B_REG
};

enum  ltc2688_clk_input {
	LTC2688_SOFT_TGL,
	LTC2688_TGP0,
	LTC2688_TGP1,
	LTC2688_TGP2
};

struct ltc2688_dev {
	spi_desc			*spi_desc;
	uint16_t			pwd_dac_setting;
	uint16_t			dither_toggle_en;
	bool				dither_mode[16];
	enum ltc2688_voltage_range 	crt_range[16];
	enum ltc2688_dither_phase	dither_phase[16];
	enum ltc2688_dither_period	dither_period[16];
	enum ltc2688_clk_input		clk_input[16];
	enum ltc2688_a_b_register	reg_select[16];
};

struct ltc2688_init_param {
	/* SPI */
	spi_init_param 			spi_init;
	uint16_t			pwd_dac_setting;
	uint16_t			dither_toggle_en;
	bool				dither_mode[16];
	enum ltc2688_voltage_range 	crt_range[16];
	enum ltc2688_dither_phase	dither_phase[16];
	enum ltc2688_dither_period	dither_period[16];
	enum ltc2688_clk_input		clk_input[16];
	enum ltc2688_a_b_register	reg_select[16];
};
/******************************************************************************/
/******************************** LTC2688 *************************************/
/******************************************************************************/
int32_t ltc2688_set_pwr_dac(struct ltc2688_dev *dev, uint16_t setting);
int32_t ltc2688_set_dither_toggle(struct ltc2688_dev *dev, uint16_t setting);
int32_t ltc2688_set_dither_mode(struct ltc2688_dev *dev, uint8_t channel,
				bool en);
int32_t ltc2688_set_span(struct ltc2688_dev *dev, uint8_t channel,
			 enum ltc2688_voltage_range range);
int32_t ltc2688_set_dither_phase(struct ltc2688_dev *dev, uint8_t channel,
				 enum  ltc2688_dither_phase phase);
int32_t ltc2688_set_dither_period(struct ltc2688_dev *dev, uint8_t channel,
				  enum  ltc2688_dither_period period);
int32_t ltc2688_select_tg_dith_clk(struct ltc2688_dev *dev, uint8_t channel,
				   enum  ltc2688_clk_input clk_input);
int32_t ltc2688_select_reg(struct ltc2688_dev *dev, uint8_t channel,
			   enum  ltc2688_a_b_register sel_reg);
int32_t ltc2688_software_reset(struct ltc2688_dev *dev);
int32_t ltc2688_set_voltage(struct ltc2688_dev *dev, uint8_t channel,
			    float voltage);
int32_t ltc2688_software_toggle(struct ltc2688_dev *dev, uint8_t channel);
int32_t ltc2688_init(struct ltc2688_dev **device,
		     struct ltc2688_init_param init_param);
int32_t ltc2688_remove(struct ltc2688_dev *dev);

#endif // __LTC2688_H__
