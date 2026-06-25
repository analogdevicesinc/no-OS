/***************************************************************************//**
 *   @file   adf4371.h
 *   @brief  Header file of ADF4371 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef ADF4371_H_
#define ADF4371_H_

#include <stdint.h>
#include <string.h>
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"

#define ADF4371_WRITE			(0 << 15)
#define ADF4371_READ			(1 << 15)
#define ADF4371_ADDR(x)			((x) & 0x7FFF)

/* Registers address macro */
#define ADF4371_REG(x)			(x)

/* ADF4371_REG0 */
#define ADF4371_ADDR_ASC_MSK		NO_OS_BIT(2)
#define ADF4371_ADDR_ASC(x)		no_os_field_prep(ADF4371_ADDR_ASC_MSK, x)
#define ADF4371_ADDR_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4371_ADDR_ASC_R(x)		no_os_field_prep(ADF4371_ADDR_ASC_R_MSK, x)
#define ADF4371_SDO_ACT_MSK		NO_OS_BIT(3)
#define ADF4371_SDO_ACT(x)		no_os_field_prep(ADF4371_SDO_ACT_MSK, x)
#define ADF4371_SDO_ACT_R_MSK		NO_OS_BIT(4)
#define ADF4371_SDO_ACT_R(x)		no_os_field_prep(ADF4371_SDO_ACT_R_MSK, x)
#define ADF4371_RESET_CMD		0x81

/* ADF4371_REG10 */
#define ADF4371_NINT_L_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_NINT_L(x)		no_os_field_prep(ADF4371_NINT_L_MSK, x)

/* ADF4371_REG11 */
#define ADF4371_NINT_M_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_NINT_M(x)		no_os_field_prep(ADF4371_NINT_M_MSK, x)

/* ADF4371_REG14*/
#define ADF4371_FRAC1WORD_L_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_FRAC1WORD_L(x)		no_os_field_prep(ADF4371_FRAC1WORD_L_MSK, x)

/* ADF4371_REG15 */
#define ADF4371_FRAC1WORD_M_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_FRAC1WORD_M(x)		no_os_field_prep(ADF4371_FRAC1WORD_M_MSK, x)

/* ADF4371_REG16 */
#define ADF4371_FRAC1WORD_MM_MSK	NO_OS_GENMASK(7, 0)
#define ADF4371_FRAC1WORD_MM(x)		no_os_field_prep(ADF4371_FRAC1WORD_MM_MSK, x)

/* ADF4371_REG17 */
#define ADF4371_FRAC2WORD_L_MSK		NO_OS_GENMASK(7, 1)
#define ADF4371_FRAC2WORD_L(x)		no_os_field_prep(ADF4371_FRAC2WORD_L_MSK, x)
#define ADF4371_FRAC1WORD_H_MSK		NO_OS_BIT(0)
#define ADF4371_FRAC1WORD_H(x)		no_os_field_prep(ADF4371_FRAC1WORD_H_MSK, x)

/* ADF4371_REG18 */
#define ADF4371_FRAC2WORD_H_MSK		NO_OS_GENMASK(6, 0)
#define ADF4371_FRAC2WORD_H(x)		no_os_field_prep(ADF4371_FRAC2WORD_H_MSK, x)

/* ADF4371_REG19 */
#define ADF4371_MOD2WORD_L_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_MOD2WORD_L(x)		no_os_field_prep(ADF4371_MOD2WORD_L_MSK, x)

/* ADF4371_REG1A */
#define ADF4371_MOD2WORD_H_MSK		NO_OS_GENMASK(5, 0)
#define ADF4371_MOD2WORD_H(x)		no_os_field_prep(ADF4371_MOD2WORD_H_MSK, x)

/* ADF4371_REG1A */
#define ADF4371_MOD2WORD_MSK		NO_OS_GENMASK(5, 0)
#define ADF4371_MOD2WORD(x)		no_os_field_prep(ADF4371_MOD2WORD_MSK, x)

/* ADF4371_REG1E */
#define ADF4371_CP_CURRENT_MSK		NO_OS_GENMASK(7, 4)
#define ADF4371_CP_CURRENT(x)		no_os_field_prep(ADF4371_CP_CURRENT_MSK, x)
#define ADF4371_PD_POL_MSK		NO_OS_BIT(3)
#define ADF4371_PD_POL(x)		no_os_field_prep(ADF4371_PD_POL_MSK, x)

/* ADF4371_REG1F */
#define ADF4371_R_WORD_MSK		NO_OS_GENMASK(4, 0)
#define ADF4371_R_WORD(x)		no_os_field_prep(ADF4371_R_WORD_MSK, x)

/* ADF4371_REG20 */
#define ADF4371_MUXOUT_MSK		NO_OS_GENMASK(7, 4)
#define ADF4371_MUXOUT(x)		no_os_field_prep(ADF4371_MUXOUT_MSK, x)
#define ADF4371_MUXOUT_LVL_MSK		NO_OS_BIT(2)
#define ADF4371_MUXOUT_LVL(x)		no_os_field_prep(ADF4371_MUXOUT_LVL_MSK, x)
#define ADF4371_MUXOUT_EN_MSK		NO_OS_BIT(3)
#define ADF4371_MUXOUT_EN(x)		no_os_field_prep(ADF4371_MUXOUT_EN_MSK, x)

/* ADF4371_REG22 */
#define ADF4371_REFIN_MODE_MASK		NO_OS_BIT(6)
#define ADF4371_REFIN_MODE(x)		no_os_field_prep(ADF4371_REFIN_MODE_MASK, x)
#define ADF4371_REF_DOUBLER_EN_MSK	NO_OS_BIT(5)
#define ADF4371_REF_DOUBLER_EN(x)	no_os_field_prep(ADF4371_REF_DOUBLER_EN_MSK, x)
#define ADF4371_RDIV2_EN_MSK		NO_OS_BIT(4)
#define ADF4371_RDIV2_EN(x)		no_os_field_prep(ADF4371_RDIV2_EN_MSK, x)
#define ADF4371_CNTR_DIV_WORD_MSK	NO_OS_GENMASK(3, 0)
#define ADF4371_CNTR_DIV_WORD(x)	no_os_field_prep(ADF4371_CNTR_DIV_WORD_MSK, x)


/* ADF4371_REG24 */
#define ADF4371_RF_DIV_SEL_MSK		NO_OS_GENMASK(6, 4)
#define ADF4371_RF_DIV_SEL(x)		no_os_field_prep(ADF4371_RF_DIV_SEL_MSK, x)

/* ADF4371_REG25 */
#define ADF4371_MUTE_LD_MSK		NO_OS_BIT(7)
#define ADF4371_MUTE_LD(x)		no_os_field_prep(ADF4371_MUTE_LD_MSK, x)
#define ADF4371_RF_DIVSEL_DB_MSK	NO_OS_BIT(5)
#define ADF4371_RF_DIVSEL_DB(x)		no_os_field_prep(ADF4371_RF_DIVSEL_DB_MSK, x)
#define ADF4371_X4_EN_MSK		NO_OS_BIT(4)
#define ADF4371_X4_EN(x)		no_os_field_prep(ADF4371_X4_EN_MSK, x)
#define ADF4371_X2_EN_MSK		NO_OS_BIT(3)
#define ADF4371_X2_EN(x)		no_os_field_prep(ADF4371_X2_EN_MSK, x)
#define ADF4371_RF_EN_MSK		NO_OS_BIT(2)
#define ADF4371_RF_EN(x)		no_os_field_prep(ADF4371_RF_EN_MSK, x)
#define ADF4371_RF_OUT_POWER_MSK	NO_OS_GENMASK(1, 0)
#define ADF4371_RF_OUT_POWER(x)		no_os_field_prep(ADF4371_RF_OUT_POWER_MSK, x)

/* ADF4371_REG26 */
#define ADF4371_BLEED_ICP_MSK		NO_OS_GENMASK(7, 0)
#define ADF4371_BLEED_ICP(x)		no_os_field_prep(ADF4371_BLEED_ICP_MSK, x)

/* ADF4371_REG27 */
#define ADF4371_LD_BIAS_MSK		NO_OS_GENMASK(7, 6)
#define ADF4371_LD_BIAS(x)		no_os_field_prep(ADF4371_LD_BIAS_MSK, x)
#define ADF4371_LDP_MSK			NO_OS_BIT(5)
#define ADF4371_LDP(x)			no_os_field_prep(ADF4371_LDP_MSK, x)
#define ADF4371_BLEED_GATE_MSK		NO_OS_BIT(4)
#define ADF4371_BLEED_GATE(x)		no_os_field_prep(ADF4371_BLEED_GATE_MSK, x)
#define ADF4371_BLEED_EN_MSK		NO_OS_BIT(3)
#define ADF4371_BLEED_EN(x)		no_os_field_prep(ADF4371_BLEED_EN_MSK, x)
#define ADF4371_VCOLDO_PD_MSK		NO_OS_BIT(2)
#define ADF4371_VCOLDO_PD(x)		no_os_field_prep(ADF4371_VCOLDO_PD_MSK, x)
#define ADF4371_RF_PBS_MSK		NO_OS_GENMASK(1, 0)
#define ADF4371_RF_PBS(x)		no_os_field_prep(ADF4371_RF_PBS_MSK, x)

/* ADF4371_REG2B */
#define ADF4371_LSB_P1_MSK		NO_OS_BIT(5)
#define ADF4371_LSB_P1(x)		no_os_field_prep(ADF4371_LSB_P1_MSK, x)
#define ADF4371_VAR_MOD_MSK		NO_OS_BIT(4)
#define ADF4371_VAR_MOD(x)		no_os_field_prep(ADF4371_VAR_MOD_MSK, x)
#define ADF4371_SD_LOAD_ENB_MSK		NO_OS_BIT(2)
#define ADF4371_SD_LOAD_ENB(x)		no_os_field_prep(ADF4371_SD_LOAD_ENB_MSK, x)
#define ADF4371_SD_EN_FRAC0_MSK		NO_OS_BIT(0)
#define ADF4371_SD_EN_FRAC0(x)		no_os_field_prep(ADF4371_SD_EN_FRAC0_MSK, x)

/* ADF4371_REG32 */
#define ADF4371_TIMEOUT_MSK		NO_OS_GENMASK(1, 0)
#define ADF4371_TIMEOUT(x)		no_os_field_prep(ADF4371_TIMEOUT_MSK, x)

/* ADF4371_REG34 */
#define ADF4371_VCO_ALC_TOUT_MSK	NO_OS_GENMASK(4, 0)
#define ADF4371_VCO_ALC_TOUT(x)		no_os_field_prep(ADF4371_VCO_ALC_TOUT_MSK, x)

/* Specifications */
#define ADF4371_MIN_VCO_FREQ		4000000000ULL /* 4000 MHz */
#define ADF4371_MAX_VCO_FREQ		8000000000ULL /* 8000 MHz */
#define ADF4371_MAX_OUT_RF8_FREQ	ADF4371_MAX_VCO_FREQ /* Hz */
#define ADF4371_MIN_OUT_RF8_FREQ	(ADF4371_MIN_VCO_FREQ / 64) /* Hz */
#define ADF4371_MAX_OUT_RF16_FREQ	(ADF4371_MAX_VCO_FREQ * 2) /* Hz */
#define ADF4371_MIN_OUT_RF16_FREQ	(ADF4371_MIN_VCO_FREQ * 2) /* Hz */
#define ADF4371_MAX_OUT_RF32_FREQ	(ADF4371_MAX_VCO_FREQ * 4) /* Hz */
#define ADF4371_MIN_OUT_RF32_FREQ	(ADF4371_MIN_VCO_FREQ * 4) /* Hz */
#define ADF4371_CPI_VAL_MAX		15
#define ADF4371_BLEED_ICP_MAX		255

#define ADF4371_MAX_FREQ_PFD		250000000UL /* Hz */
#define ADF4371_MIN_FREQ_REFIN		10000000UL /* Hz */
#define ADF4371_MAX_FREQ_REFIN		600000000UL /* Hz */

/* MOD1 is a 24-bit primary modulus with fixed value of 2^25 */
#define ADF4371_MODULUS1		33554432ULL
/* MOD2 is the programmable, 14-bit auxiliary fractional modulus */
#define ADF4371_MAX_MODULUS2		NO_OS_BIT(14)

#define ADF4371_CHANNEL_SPACING_MAX 	78125U
#define ADF4371_MAX_CHANNELS		3
#define ADF4371_RFOUT8_DIV_MAX		6

#define ADF4371_CHECK_RANGE(freq, range) \
	((freq > ADF4371_MAX_ ## range) || (freq < ADF4371_MIN_ ## range))


struct adf4371_cp_settings {
	uint32_t	icp;
	uint32_t	regval;
	uint32_t	bleed_icp;
};


struct adf4371_dev {
	struct no_os_spi_desc	*spi_desc;
	bool		spi_3wire_en;
	struct adf4371_cp_settings	cp_settings;
	bool		differential_ref_clk;
	uint32_t	pd_pol;
	bool		mute_till_lock_en;
	uint32_t	muxout_default_mode;
	bool		muxout_en;
	bool		muxout_1v8_en;
	uint32_t	ref_div_factor;
	uint32_t	clkin_freq;
	uint32_t	fpfd;
	uint32_t	integer;
	uint32_t	fract1;
	uint32_t	fract2;
	uint32_t	mod2;
	uint32_t	rf_div_sel;
	uint8_t		buf[10];
	bool		ref_doubler_en;
	bool		ref_div2_en;
	uint32_t	bleed_icp;
	uint64_t 	vco_freq;
	uint64_t 	rfout8_freq;
	uint64_t 	rfout16_freq;
	uint64_t 	rfout32_freq;
	/** GPIO Chip Enable */
	struct no_os_gpio_desc	*gpio_ce;
};

struct adf4371_init_param {
	struct no_os_spi_init_param	*spi_init;
	bool		spi_3wire_enable;
	/** GPIO Chip Enable */
	struct no_os_gpio_init_param	*gpio_ce_param;
	uint32_t	clkin_frequency;
	bool		differential_ref_clock;
	bool		loop_filter_inverting;
	bool		ref_div2_enable;
	uint32_t	rf_div_sel;
	bool		ref_doubler_enable;
	uint32_t	charge_pump_microamp;
	bool		mute_till_lock_enable;
	uint32_t	muxout_select;
	bool		muxout_level_1v8_enable;
	uint64_t 	rfout8_freq;
};

extern const struct no_os_clk_platform_ops adf4371_clk_ops;

enum {
	ADF4371_FREQ,
	ADF4371_POWER_DOWN,
	ADF4371_CHANNEL_NAME,
	ADF4371_MUXOUT_ENABLE
};

enum {
	ADF4371_CH_RF8,
	ADF4371_CH_RFAUX8,
	ADF4371_CH_RF16,
	ADF4371_CH_RF32
};

enum adf4371_variant {
	ADF4371,
	ADF4372
};

enum adf4371_muxout {
	ADF4371_TRISTATE = 0x00,
	ADF4371_DIG_LOCK = 0x01,
	ADF4371_CH_PUMP_UP = 0x02,
	ADF4371_CH_PUMP_DOWN = 0x03,
	ADF4371_RDIV2 = 0x04,
	ADF4371_N_DIV_OUT = 0x05,
	ADF4371_VCO_TEST = 0x06,
	ADF4371_HIGH = 0x08,
	ADF4371_VCO_CALIB_R_BAND = 0x09,
	ADF4371_VCO_CALIB_N_BAND = 0x0A
};

struct adf4371_pwrdown {
	uint32_t reg;
	uint32_t bit;
};

struct reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/* Update a bit in the register space over SPI. */
int adf4371_update(struct adf4371_dev *dev, uint16_t reg, uint8_t mask,
		   uint8_t val);

/* SPI register read from device. */
int adf4371_read(struct adf4371_dev *dev, uint16_t reg, uint8_t *val);

/* SPI register write to device. */
int adf4371_write(struct adf4371_dev *dev, uint16_t reg, uint8_t val);

/* SPI register write bulk to device. */
int adf4371_write_bulk(struct adf4371_dev *dev, uint16_t reg, uint8_t *val,
		       uint8_t size);

/* SPI register write a sequence of register-value pairs to device. */
int adf4371_write_regs(struct adf4371_dev *dev, const struct reg_sequence *regs,
		       uint32_t count);

/* ADF4371 Set Reference Clock Frequency */
int adf4371_set_ref_clk(struct adf4371_dev *dev, uint64_t val);

/* ADF4371 Get Reference Clock Frequency */
int adf4371_get_ref_clk(struct adf4371_dev *dev, uint64_t *val);

/* ADF4371 Set Charge Pump Current */
int adf4371_set_charge_pump_current(struct adf4371_dev *dev, uint32_t val);

/* ADF4371 Get Charge Pump Current */
int adf4371_get_charge_pump_current(struct adf4371_dev *dev, uint32_t *val);

/* ADF4371 Set Reference doubler enable */
int adf4371_set_ref_doubler(struct adf4371_dev *dev, bool enable);

/* ADF4371 Get Reference doubler enable */
int adf4371_get_ref_doubler(struct adf4371_dev *dev, bool *enable);

/* ADF4371 Set Reference divide by 2 enable */
int adf4371_set_rdiv2_enable(struct adf4371_dev *dev, bool enable);

/* ADF4371 Get Reference divide by 2 enable */
int adf4371_get_rdiv2_enable(struct adf4371_dev *dev, bool *enable);

/* ADF4371 Set R Word */
int adf4371_set_r_word(struct adf4371_dev *dev, uint32_t val);

/* ADF4371 Get R Word */
int adf4371_get_r_word(struct adf4371_dev *dev, uint32_t *val);

/* ADF4371 Set Channel Power Down */
int adf4371_set_channel_power_down(struct adf4371_dev *dev, uint32_t chan,
				   bool power_down);

/* ADF4371 Get Channel Power Down */
int adf4371_get_channel_power_down(struct adf4371_dev *dev, uint32_t chan,
				   bool *power_down);

/* ADF4371 Set Frequency */
int adf4371_set_frequency(struct adf4371_dev *dev);

/* ADF4371 Get RFOUT8 Frequency */
int adf4371_get_rfout(struct adf4371_dev *dev, uint64_t *freq);

/* ADF4371 Set RFOUT8 Frequency */
int adf4371_set_rf8_freq(struct adf4371_dev *dev, uint64_t freq);

/* ADF4371 Get RFOUT8AUX Frequency */
int adf4371_get_rfout8aux(struct adf4371_dev *dev, uint64_t *freq);

/* ADF4371 Get RFOUT16 Frequency */
int adf4371_get_rfout16(struct adf4371_dev *dev, uint64_t *freq);

/* ADF4371 Get RFOUT32 Frequency */
int adf4371_get_rfout32(struct adf4371_dev *dev, uint64_t *freq);

/* ADF4371 Set Bleed Current */
int adf4371_set_bleed_icp(struct adf4371_dev *dev, uint32_t val);

/* ADF4371 Get Bleed Current */
int adf4371_get_bleed_icp(struct adf4371_dev *dev, uint32_t *val);

/* ADF4371 Initialization */
int adf4371_init(struct adf4371_dev **device,
		 const struct adf4371_init_param *init_param);

/* ADF4371 Remove */
int adf4371_remove(struct adf4371_dev *device);

#endif
