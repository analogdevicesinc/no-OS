/***************************************************************************//**
 * @file si5391.h
 * @brief Header file of Si5391 Driver.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef SI5391_H_
#define SI5391_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "jesd204.h"

/******************************************************************************/
/********************** Macros and Constants Definitions ***********************/
/******************************************************************************/

#define SI5391_NUM_INPUTS		4
#define SI5391_MAX_NUM_OUTPUTS		12
#define SI5391_NUM_SYNTH		5

/* Range of the synthesizer fractional divider */
#define SI5391_SYNTH_N_MIN		10
#define SI5391_SYNTH_N_MAX		4095

/* PLL VCO frequency range (Hz) */
#define SI5391_PLL_VCO_MIN		13500000000ULL
#define SI5391_PLL_VCO_MAX		14256000000ULL

/* Register addresses */
#define SI5391_PAGE			0x0001
#define SI5391_PN_BASE			0x0002
#define SI5391_DEVICE_REV		0x0005
#define SI5391_STATUS			0x000C
#define SI5391_LOS			0x000D
#define SI5391_STATUS_STICKY		0x0011
#define SI5391_LOS_STICKY		0x0012
#define SI5391_SOFT_RST			0x001C
#define SI5391_IN_SEL			0x0021
#define SI5391_DEVICE_READY		0x00FE
#define SI5391_XAXB_CFG			0x090E
#define SI5391_IO_VDD_SEL		0x0943
#define SI5391_IN_EN			0x0949
#define SI5391_INX_TO_PFD_EN		0x094A

/* Status bits */
#define SI5391_STATUS_SYSINCAL		0x01
#define SI5391_STATUS_LOSXAXB		0x02
#define SI5391_STATUS_LOSREF		0x04
#define SI5391_STATUS_LOL		0x08

/* Input selection */
#define SI5391_IN_SEL_MASK		0x06
#define SI5391_IN_SEL_SHIFT		1
#define SI5391_IN_SEL_REGCTRL		0x01
#define SI5391_INX_TO_PFD_SHIFT		4

/* XTAL config bits */
#define SI5391_XAXB_CFG_EXTCLK_EN	0x01
#define SI5391_XAXB_CFG_PDNB		0x02

/* Input dividers (48-bit) */
#define SI5391_IN_PDIV(x)		(0x0208 + ((x) * 10))
#define SI5391_IN_PSET(x)		(0x020E + ((x) * 10))
#define SI5391_PX_UPD			0x0230

/* PLL configuration */
#define SI5391_PLL_M_NUM		0x0235
#define SI5391_PLL_M_DEN		0x023B

/* Output configuration */
#define SI5391_OUT_CONFIG(idx)		(si5391_reg_output_offset[(idx)])
#define SI5391_OUT_FORMAT(idx)		(SI5391_OUT_CONFIG(idx) + 1)
#define SI5391_OUT_CM(idx)		(SI5391_OUT_CONFIG(idx) + 2)
#define SI5391_OUT_MUX_SEL(idx)		(SI5391_OUT_CONFIG(idx) + 3)
#define SI5391_OUT_R_REG(idx)		(si5391_reg_rdiv_offset[(idx)])

#define SI5391_OUT_MUX_VDD_SEL_MASK	0x38

/* Synthesizer N divider */
#define SI5391_SYNTH_N_NUM(x)		(0x0302 + ((x) * 11))
#define SI5391_SYNTH_N_DEN(x)		(0x0308 + ((x) * 11))
#define SI5391_SYNTH_N_UPD(x)		(0x030C + ((x) * 11))

/* Synthesizer output enable, phase bypass, power mode */
#define SI5391_SYNTH_N_CLK_TO_OUTX_EN	0x0A03
#define SI5391_SYNTH_N_PIBYP		0x0A04
#define SI5391_SYNTH_N_PDNB		0x0A05
#define SI5391_SYNTH_N_CLK_DIS		0x0B4A

#define SI5391_REGISTER_MAX		0x0BFF

/* Output config bits */
#define SI5391_OUT_CFG_PDN		0x01
#define SI5391_OUT_CFG_OE		0x02
#define SI5391_OUT_CFG_RDIV_FORCE2	0x04

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Si5391 output format */
enum si5391_out_format {
	SI5391_OUT_FORMAT_DIFF_NORMAL = 1,
	SI5391_OUT_FORMAT_DIFF_LOW_POWER = 2,
	SI5391_OUT_FORMAT_LVCMOS = 4,
};

/** Si5391 output configuration */
struct si5391_output_config {
	uint8_t out_format_drv_bits;
	uint8_t out_cm_ampl_bits;
	uint8_t vdd_sel_bits;
	bool synth_master;
	bool always_on;
};

/** Si5391 register default pair */
struct si5391_reg_default {
	uint16_t address;
	uint8_t value;
};

/** Si5391 initialization parameters */
struct si5391_init_param {
	/* I2C */
	struct no_os_i2c_init_param i2c_init;
	/* Input clock frequency in Hz */
	uint64_t input_clk_freq;
	/* Selected input (0..3: IN0..IN2, XTAL) */
	uint8_t active_input;
	/* Use external clock on XAXB pins instead of crystal */
	bool xaxb_ext_clk;
	/* Set to true for 3.3V IO, false for 1.8V */
	bool iovdd_33;
	/* PLL M numerator (0 = use defaults from ClockBuilder Pro) */
	uint32_t pll_m_num;
	/* PLL M denominator */
	uint32_t pll_m_den;
	/* Force reprogramming even if already configured */
	bool reprogram;
	/* Per-output configuration (NULL entries = not configured) */
	struct si5391_output_config output_config[SI5391_MAX_NUM_OUTPUTS];
	/* JESD204 maximum SYSREF frequency (0 = no limit) */
	uint32_t jdev_max_sysref_freq;
	/* JESD204 desired SYSREF frequency (0 = auto) */
	uint32_t jdev_desired_sysref_freq;
};

/** Si5391 device descriptor */
struct si5391_dev {
	/* I2C */
	struct no_os_i2c_desc *i2c_desc;
	/* Cached VCO frequency */
	uint64_t freq_vco;
	/* Number of outputs for this chip variant */
	uint8_t num_outputs;
	/* Number of synthesizers */
	uint8_t num_synth;
	/* Chip model ID */
	uint16_t chip_id;
	/* Active input selection */
	uint8_t active_input;
	/* Input clock frequency */
	uint64_t input_clk_freq;
	/* Use external clock on XAXB */
	bool xaxb_ext_clk;
	/* IO voltage select */
	bool iovdd_33;
	/* True if hardware is not present (dummy mode) */
	bool dummy;
	/* Per-output configuration */
	struct si5391_output_config output_config[SI5391_MAX_NUM_OUTPUTS];
	/* JESD204 device */
	struct jesd204_dev *jdev;
	/* JESD204 LMFC/LEMC rate (Hz) */
	uint32_t jdev_lmfc_lemc_rate;
	/* JESD204 LMFC/LEMC GCD for SYSREF computation */
	uint32_t jdev_lmfc_lemc_gcd;
	/* JESD204 maximum SYSREF frequency */
	uint32_t jdev_max_sysref_freq;
	/* JESD204 desired SYSREF frequency */
	uint32_t jdev_desired_sysref_freq;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Register read/write */
int32_t si5391_reg_read(struct si5391_dev *dev, uint16_t reg_addr,
			uint8_t *reg_data);
int32_t si5391_reg_write(struct si5391_dev *dev, uint16_t reg_addr,
			 uint8_t reg_data);

/* Initialize the device */
int32_t si5391_setup(struct si5391_dev **device,
		     const struct si5391_init_param *init_param);

/* Free allocated resources */
int32_t si5391_remove(struct si5391_dev *dev);

/* Get device status */
int32_t si5391_status_get(struct si5391_dev *dev, uint8_t *status);

/* Check if PLL is locked */
int32_t si5391_pll_locked(struct si5391_dev *dev, bool *locked);

/* Check if input clock is present */
int32_t si5391_input_present(struct si5391_dev *dev, bool *present);

/* Clear sticky status bits */
int32_t si5391_clear_sticky(struct si5391_dev *dev);

/* Select the active input clock */
int32_t si5391_input_select(struct si5391_dev *dev, uint8_t input);

/* Synthesizer enable/disable */
int32_t si5391_synth_enable(struct si5391_dev *dev, uint8_t synth_index);
int32_t si5391_synth_disable(struct si5391_dev *dev, uint8_t synth_index);

/* Get synthesizer output frequency */
int32_t si5391_synth_get_freq(struct si5391_dev *dev, uint8_t synth_index,
			      uint64_t *freq);

/* Set synthesizer output frequency */
int32_t si5391_synth_set_freq(struct si5391_dev *dev, uint8_t synth_index,
			      uint64_t freq);

/* Output enable/disable */
int32_t si5391_output_enable(struct si5391_dev *dev, uint8_t output_index);
int32_t si5391_output_disable(struct si5391_dev *dev, uint8_t output_index);

/* Get output frequency */
int32_t si5391_output_get_freq(struct si5391_dev *dev, uint8_t output_index,
			       uint64_t *freq);

/* Set output frequency */
int32_t si5391_output_set_freq(struct si5391_dev *dev, uint8_t output_index,
			       uint64_t freq);

/* Set output mux to a given synthesizer */
int32_t si5391_output_set_mux(struct si5391_dev *dev, uint8_t output_index,
			      uint8_t synth_index);

/* Get which synthesizer an output is connected to */
int32_t si5391_output_get_mux(struct si5391_dev *dev, uint8_t output_index,
			      uint8_t *synth_index);

/* Recalculate VCO frequency (call after input clock changes) */
int32_t si5391_recalc_vco(struct si5391_dev *dev);

/* Synchronize all output clock dividers */
int32_t si5391_sync(struct si5391_dev *dev);

#endif /* SI5391_H_ */
