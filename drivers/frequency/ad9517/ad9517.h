/***************************************************************************//**
 *   @file   AD9517.h
 *   @brief  Header file of AD9517 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __AD9517_H__
#define __AD9517_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/****************************** AD9517 ****************************************/
/******************************************************************************/
#define AD9517_READ				(1 << 15)
#define AD9517_WRITE				(0 << 15)
#define AD9517_CNT(x)				((x - 1) << 13)
#define AD9517_ADDR(x)				(x & 0x3FF)

#define AD9517_T1B				(1 << 16)
#define AD9517_T2B				(2 << 16)
#define AD9517_T3B				(3 << 16)
#define AD9517_TRANSF_LEN(x)			((uint32_t)x >> 16)

/* SPI Register Map */

/* Serial Port Configuration */
#define AD9517_REG_SERIAL_PORT_CONFIG		(AD9517_T1B | 0x000)
#define AD9517_REG_PART_ID			(AD9517_T1B | 0x003)
#define AD9517_REG_READBACK_CTRL		(AD9517_T1B | 0x004)

/* PLL */
#define AD9517_REG_PFD_CHARGE_PUMP		(AD9517_T1B | 0x010)
#define AD9517_REG_R_COUNTER			(AD9517_T2B | 0x012)
#define AD9517_REG_A_COUNTER			(AD9517_T1B | 0x013)
#define AD9517_REG_B_COUNTER			(AD9517_T2B | 0x015)
#define AD9517_REG_PLL_CTRL_1			(AD9517_T1B | 0x016)
#define AD9517_REG_PLL_CTRL_2			(AD9517_T1B | 0x017)
#define AD9517_REG_PLL_CTRL_3			(AD9517_T1B | 0x018)
#define AD9517_REG_PLL_CTRL_4			(AD9517_T1B | 0x019)
#define AD9517_REG_PLL_CTRL_5			(AD9517_T1B | 0x01A)
#define AD9517_REG_PLL_CTRL_6			(AD9517_T1B | 0x01B)
#define AD9517_REG_PLL_CTRL_7			(AD9517_T1B | 0x01C)
#define AD9517_REG_PLL_CTRL_8			(AD9517_T1B | 0x01D)
#define AD9517_REG_PLL_READBACK			(AD9517_T1B | 0x01F)

/* Fine Delay Adjust - OUT4 to OUT7 */
#define AD9517_REG_OUT4_DELAY_BYPASS		(AD9517_T1B | 0x0A0)
#define AD9517_REG_OUT4_DELAY_FULL_SCALE	(AD9517_T1B | 0x0A1)
#define AD9517_REG_OUT4_DELAY_FRACTION		(AD9517_T1B | 0x0A2)
#define AD9517_REG_OUT5_DELAY_BYPASS		(AD9517_T1B | 0x0A3)
#define AD9517_REG_OUT5_DELAY_FULL_SCALE	(AD9517_T1B | 0x0A4)
#define AD9517_REG_OUT5_DELAY_FRACTION		(AD9517_T1B | 0x0A5)
#define AD9517_REG_OUT6_DELAY_BYPASS		(AD9517_T1B | 0x0A6)
#define AD9517_REG_OUT6_DELAY_FULL_SCALE	(AD9517_T1B | 0x0A7)
#define AD9517_REG_OUT6_DELAY_FRACTION		(AD9517_T1B | 0x0A8)
#define AD9517_REG_OUT7_DELAY_BYPASS		(AD9517_T1B | 0x0A9)
#define AD9517_REG_OUT7_DELAY_FULL_SCALE	(AD9517_T1B | 0x0AA)
#define AD9517_REG_OUT7_DELAY_FRACTION		(AD9517_T1B | 0x0AB)

/* LVPECL Outputs */
#define AD9517_REG_LVPECL_OUT0			(AD9517_T1B | 0x0F0)
#define AD9517_REG_LVPECL_OUT1			(AD9517_T1B | 0x0F1)
#define AD9517_REG_LVPECL_OUT2			(AD9517_T1B | 0x0F4)
#define AD9517_REG_LVPECL_OUT3			(AD9517_T1B | 0x0F5)

/* LVDS/CMOS Outputs */
#define AD9517_REG_LVDS_CMOS_OUT4		(AD9517_T1B | 0x140)
#define AD9517_REG_LVDS_CMOS_OUT5		(AD9517_T1B | 0x141)
#define AD9517_REG_LVDS_CMOS_OUT6		(AD9517_T1B | 0x142)
#define AD9517_REG_LVDS_CMOS_OUT7		(AD9517_T1B | 0x143)

/* LVPECL Channel Dividers */
#define AD9517_REG_DIVIDER_0_0			(AD9517_T1B | 0x190)
#define AD9517_REG_DIVIDER_0_1			(AD9517_T1B | 0x191)
#define AD9517_REG_DIVIDER_0_2			(AD9517_T1B | 0x192)
#define AD9517_REG_DIVIDER_1_0			(AD9517_T1B | 0x196)
#define AD9517_REG_DIVIDER_1_1			(AD9517_T1B | 0x197)
#define AD9517_REG_DIVIDER_1_2			(AD9517_T1B | 0x198)

/* LVDS/CMOS Channel Dividers */
#define AD9517_REG_LVDS_CMOS_DIVIDER_2_0	(AD9517_T1B | 0x199)
#define AD9517_REG_LVDS_CMOS_DIVIDER_2_1	(AD9517_T1B | 0x19A)
#define AD9517_REG_LVDS_CMOS_DIVIDER_2_2	(AD9517_T1B | 0x19B)
#define AD9517_REG_LVDS_CMOS_DIVIDER_2_3	(AD9517_T1B | 0x19C)
#define AD9517_REG_LVDS_CMOS_DIVIDER_2_4	(AD9517_T1B | 0x19D)
#define AD9517_REG_LVDS_CMOS_DIVIDER_3_0	(AD9517_T1B | 0x19E)
#define AD9517_REG_LVDS_CMOS_DIVIDER_3_1	(AD9517_T1B | 0x19F)
#define AD9517_REG_LVDS_CMOS_DIVIDER_3_2	(AD9517_T1B | 0x1A0)
#define AD9517_REG_LVDS_CMOS_DIVIDER_3_3	(AD9517_T1B | 0x1A1)
#define AD9517_REG_LVDS_CMOS_DIVIDER_3_4	(AD9517_T1B | 0x1A2)

/* VCO Divider and CLK Input */
#define AD9517_REG_VCO_DIVIDER			(AD9517_T1B | 0x1E0)
#define AD9517_REG_INPUT_CLKS			(AD9517_T1B | 0x1E1)

/* System */
#define AD9517_REG_POWER_DOWN_SYNC		(AD9517_T1B | 0x230)

/* Update All Registers */
#define AD9517_REG_UPDATE_ALL_REGS		(AD9517_T1B | 0x232)

/* AD9517_REG_SERIAL_PORT_CONFIG Definition */
#define AD9517_SDO_ACTIVE			((1 << 7) | (1 << 0))
#define AD9517_LSB_FIRST			((1 << 6) | (1 << 1))
#define AD9517_SOFT_RESET			((1 << 5) | (1 << 2))
#define AD9517_LONG_INSTRUCTION			((1 << 4) | (1 << 3))

/* AD9517_REG_READBACK_CTRL Definition */
#define AD9517_REG_BANK_SELECTION		(1 << 0)

/* AD9517_REG_PFD_CHARGE_PUMP Definition */
#define AD9517_PFD_POLARITY			(1 << 7)
#define AD9517_CP_CURRENT(x)			((x & 0x7) << 4)
#define AD9517_CP_MODE(x)			((x & 0x3) << 2)
#define AD9517_PLL_POWER_DOWN(x)		((x & 0x3) << 0)

/* AD9517_REG_R_COUNTER Definition */
#define AD9517_R_COUNTER(x)			((x & 0x3FFF) << 0)

/* AD9517_REG_A_COUNTER Definition */
#define AD9517_A_COUNTER(x)			((x & 0x3F) << 0)

/* AD9517_REG_B_COUNTER Definition */
#define AD9517_B_COUNTER(x)			((x & 0x1FFF) << 0)

/* AD9517_REG_PLL_CTRL_1 Definition */
#define AD9517_CP_VCP_DIV2			(1 << 7)
#define AD9517_RESET_R_COUNTER			(1 << 6)
#define AD9517_RESET_A_B_COUNTERS		(1 << 5)
#define AD9517_RESET_ALL_COUNTERS		(1 << 4)
#define AD9517_B_COUNTER_BYPASS			(1 << 3)
#define AD9517_PRESCALER_P(x)			((x & 0x7) << 0)

/* AD9517_REG_PLL_CTRL_2 Definition */
#define AD9517_STATUS_PIN_CTRL(x)		((x & 0x3F) << 2)
#define AD9517_ANTIBACKLASH_PULSE_WIDTH(x)	((x & 0x3) << 0)

/* AD9517_REG_PLL_CTRL_3 Definition */
#define AD9517_LOCK_DETECT_COUNTER(x)		((x & 0x3) << 5)
#define AD9517_DIGITAL_LOCK_DETECT_WINDOW	(1 << 4)
#define AD9517_DIS_DIGITAL_LOCK_DETECT		(1 << 3)
#define AD9517_VCO_CAL_DIVIDER(x)		((x & 0x3) << 1)
#define AD9517_VCO_CAL_NOW			(1 << 0)

/* AD9517_REG_PLL_CTRL_4 Definition */
#define AD9517_SYNC_PIN_RESET(x)		((x & 0x3) << 6)
#define AD9517_R_PATH_DELAY(x)			((x & 0x7) << 3)
#define AD9517_N_PATH_DELAY(x)			((x & 0x7) << 0)

/* AD9517_REG_PLL_CTRL_5 Definition */
#define AD9517_REF_FREQ_MONITOR_THRESHOLD	(1 << 6)
#define AD9517_LD_PIN_CTRL(x)			((x & 0x3F) << 0)

/* AD9517_REG_PLL_CTRL_6 Definition */
#define AD9517_VCO_FREQ_MONITOR			(1 << 7)
#define AD9517_REF2_FREQ_MONITOR		(1 << 6)
#define AD9517_REF1_FREQ_MONITOR		(1 << 5)
#define AD9517_REFMON_PIN_CTRL(x)		((x & 0x1F) << 0)

/* AD9517_REG_PLL_CTRL_7 Definition */
#define AD9517_DIS_SWITCHOVER_DEGLITCH		(1 << 7)
#define AD9517_SELECT_REF2			(1 << 6)
#define AD9517_USE_REF_SEL_PIN			(1 << 5)
#define AD9517_REF2_POWER_ON			(1 << 2)
#define AD9517_REF1_POWER_ON			(1 << 1)
#define AD9517_DIFF_REF				(1 << 0)

/* AD9517_REG_PLL_CTRL_8 Definition */
#define AD9517_PLL_STATUS_REG_DIS		(1 << 4)
#define AD9517_LD_PIN_COMPARATOR_EN		(1 << 3)
#define AD9517_HOLDOVER_EN			((1 << 2) | (1 << 0))
#define AD9517_EXT_HOLDOVER_CTRL		(1 << 1)

/* AD9517_REG_PLL_READBACK Definition */
#define AD9517_VCO_CAL_FINISHED			(1 << 6)
#define AD9517_HOLDOVER_ACTIVE			(1 << 5)
#define AD9517_REF2_SELECTED			(1 << 4)
#define AD9517_VCO_FREQ_GREATER			(1 << 3)
#define AD9517_REF2_FREQ_GREATER		(1 << 2)
#define AD9517_REF1_FREQ_GREATER		(1 << 1)
#define AD9517_DIGITAL_LOCK_DETECT		(1 << 0)

/* AD9517_REG_OUTn_DELAY_BYPASS Definition */
#define AD9517_OUT_DELAY_BYPASS			(1 << 0)

/* AD9517_REG_OUTn_DELAY_FULL_SCALE Definition */
#define AD9517_OUT_RAMP_CAPACITORS(x)		((x & 0x7) << 3)
#define AD9517_OUT_RAMP_CURRENT(x)		((x & 0x7) << 0)

/* AD9517_REG_OUTn_DELAY_FRACTION Definition */
#define AD9517_OUT_DELAY_FRACTION(x)		((x & 0x3F) << 0)

/* AD9517_REG_LVPECL_OUTn Definition */
#define AD9517_OUT_LVPECL_INVERT		(1 << 4)
#define AD9517_OUT_LVPECL_DIFF_VOLTAGE(x)	((x & 0x3) << 2)
#define AD9517_OUT_LVPECL_POWER_DOWN(x)		((x & 0x3) << 0)

/* AD9517_REG_LVDS_CMOS_OUTn Definition */
#define AD9517_OUT_LVDS_CMOS_INVERT(x)		((x & 0x7) << 5)
#define AD9517_OUT_CMOS_B			(1 << 4)
#define AD9517_OUT_LVDS_CMOS			(1 << 3)
#define AD9517_OUT_LVDS_OUTPUT_CURRENT(x)	((x & 0x3) << 1)
#define AD9517_OUT_LVDS_CMOS_POWER_DOWN		(1 << 0)

/* AD9517_REG_DIVIDER_n_0 Definition */
#define AD9517_DIVIDER_LOW_CYCLES(x)		((x & 0xF) << 4)
#define AD9517_DIVIDER_HIGH_CYCLES(x)		((x & 0xF) << 0)

/* AD9517_REG_DIVIDER_n_1 Definition */
#define AD9517_DIVIDER_BYPASS			(1 << 7)
#define AD9517_LVPECL_DIVIDER_NOSYNC		(1 << 6)
#define AD9517_LVPECL_DIVIDER_FORCE_HIGH	(1 << 5)
#define AD9517_DIVIDER_START_HIGH		(1 << 4)
#define AD9517_DIVIDER_PHASE_OFFSET(x)		((x & 0xF) << 0)

/* AD9517_REG_DIVIDER_n_2 Definition */
#define AD9517_DIVIDER_DIRECT_TO_OUTPUT		(1 << 1)
#define AD9517_DIVIDER_DCCOFF			(1 << 0)

/* AD9517_REG_LVDS_CMOS_DIVIDER_n_0 Definition */
#define AD9517_LOW_CYCLES_DIVIDER_1(x)		((x & 0xF) << 4)
#define AD9517_HIGH_CYCLES_DIVIDER_1(x)		((x & 0xF) << 0)

/* AD9517_REG_LVDS_CMOS_DIVIDER_n_1 Definition */
#define AD9517_PHASE_OFFSET_DIVIDER_2(x)	((x & 0xF) << 4)
#define AD9517_PHASE_OFFSET_DIVIDER_1(x)	((x & 0xF) << 0)

/* AD9517_REG_LVDS_CMOS_DIVIDER_n_2 Definition */
#define AD9517_LOW_CYCLES_DIVIDER_2(x)		((x & 0xF) << 4)
#define AD9517_HIGH_CYCLES_DIVIDER_2(x)		((x & 0xF) << 0)

/* AD9517_REG_LVDS_CMOS_DIVIDER_n_3 Definition */
#define AD9517_BYPASS_DIVIDER_2			(1 << 5)
#define AD9517_BYPASS_DIVIDER_1			(1 << 4)
#define AD9517_LVDS_CMOS_DIVIDER_NOSYNC		(1 << 3)
#define AD9517_LVDS_CMOS_DIVIDER_FORCE_HIGH	(1 << 2)
#define AD9517_START_HIGH_DIVIDER_2		(1 << 1)
#define AD9517_START_HIGH_DIVIDER_1		(1 << 0)

/* AD9517_REG_LVDS_CMOS_DIVIDER_n_4 Definition */
#define AD9517_DIVIDER_DCCOFF			(1 << 0)

/* AD9517_REG_VCO_DIVIDER Definition */
#define AD9517_VCO_DIVIDER(x)			((x & 0x7) << 0)

/* AD9517_REG_INPUT_CLKS Definition */
#define AD9517_POWER_DOWN_CLK_INPUT_SECTION	(1 << 4)
#define AD9517_POWER_DOWN_VCO_CLK_INTERFACE	(1 << 3)
#define AD9517_POWER_DOWN_VCO_CLK		(1 << 2)
#define AD9517_SEL_VCO_CLK			(1 << 1)
#define AD9517_BYPASS_VCO_DIVIDER		(1 << 0)

/* AD9517_REG_POWER_DOWN_SYNC Definition */
#define AD9517_POWER_DOWN_SYNC			(1 << 2)
#define AD9517_POWER_DOWN_DIST_REF		(1 << 1)
#define AD9517_SOFT_SYNC			(1 << 0)

/* AD9517_REG_UPDATE_ALL_REGS Definition */
#define AD9517_UPDATE_ALL_REGS			(1 << 0)

#define AD9517_1_MIN_VCO_FREQ			2300000000
#define AD9517_1_MAX_VCO_FREQ			2650000000
#define AD9517_2_MIN_VCO_FREQ			2050000000
#define AD9517_2_MAX_VCO_FREQ			2330000000
#define AD9517_3_MIN_VCO_FREQ			1750000000
#define AD9517_3_MAX_VCO_FREQ			2250000000
#define AD9517_4_MIN_VCO_FREQ			1450000000
#define AD9517_4_MAX_VCO_FREQ			1800000000
#define AD9517_MAX_PFD_FREQ			100000000
#define AD9517_MAX_PRESCLAER_OUT_FREQ		300000000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Platform specific information */
struct ad9517_platform_data {
	/* PLL Reference */
	int32_t ref_1_freq;	// Frequency of the Reference 1.
	int32_t ref_2_freq;	// Frequency of the Reference 2.
	uint8_t diff_ref_en;	// Selects the differential PLL reference mode.
	uint8_t ref_1_power_on;	// Power on REF1.
	uint8_t ref_2_power_on;	// Power on REF2.
	uint8_t ref_sel_pin_en;	// Set method of PLL reference selection.
	uint8_t ref_sel_pin;	// State of the REF_SEL pin.
	uint8_t ref_2_en;	// Select Reference 2.

	/* External Clock  */
	int64_t ext_clk_freq;	// Frequency of the external clock.

	/* VCO  */
	int64_t int_vco_freq;	// Frequency of the internal VCO.

	/* External Clock or VCO selection */
	int32_t vco_clk_sel;

	uint8_t power_down_vco_clk;
	uint8_t name[16];
};

/* LVPECL output channel configuration. */
struct ad9517_lvpecl_channel_spec {
	uint8_t channel_num;	  // Output channel number.
	uint8_t out_invert_en;	  // Invert the polarity of the output clock.
	uint8_t out_diff_voltage; // LVPECL output differential voltage.
	uint8_t name[16];	  // Optional descriptive channel name.
};

enum ad9517_type {
	AD9517_1 = 0x51,
	AD9517_2 = 0x91,
	AD9517_3 = 0x53,
	AD9517_4 = 0xd3
};

enum out_diff_voltage_options {
	LVPECL_400mV,
	LVPECL_600mV,
	LVPECL_780mV,
	LVPECL_960mV,
};

/* LVDS/CMOS output channel configuration. */
struct ad9517_lvds_cmos_channel_spec {
	uint8_t channel_num;	  // Output channel number.
	uint8_t out_invert;	  // Invert the polarity of the output clock.
	uint8_t logic_level;	  // Select LVDS or CMOS logic levels.
	uint8_t cmos_b_en;	  // In CMOS mode, turn on/off the CMOS B output
	uint8_t out_lvds_current; // LVDS output current level.
	uint8_t name[16];	  // Optional descriptive channel name.
};

enum logic_level_options {
	LVDS,
	CMOS
};

enum out_lvds_current_options {
	LVDS_1_75mA,
	LVDS_3_5mA,
	LVDS_5_25mA,
	LVDS_7mA,
};

struct ad9517_state {
	struct ad9517_platform_data	     *pdata;
	struct ad9517_lvpecl_channel_spec    *lvpecl_channels;
	struct ad9517_lvds_cmos_channel_spec *lvds_cmos_channels;
	uint32_t			     r_counter;
	uint8_t 			     a_counter;
	uint16_t			     b_counter;
	uint8_t				     vco_divider;
	uint8_t				     prescaler_p;
	uint8_t				     antibacklash_pulse_width;
};

struct ad9517_dev {
	/* SPI */
	struct no_os_spi_desc	    *spi_desc;
	/* Device Settings */
	struct ad9517_state ad9517_st;
	enum ad9517_type	ad9517_type;
};

struct ad9517_init_param {
	/* SPI */
	struct no_os_spi_init_param    spi_init;
	/* Device Settings */
	struct ad9517_state ad9517_st;
	enum ad9517_type	ad9517_type;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Initializes the AD9517. */
int32_t ad9517_setup(struct ad9517_dev **device,
		     struct ad9517_init_param init_param);
/*! Free the resources allocated by ad9517_setup(). */
int32_t ad9517_remove(struct ad9517_dev *dev);
/*!  Writes data into a register. */
int32_t ad9517_write(struct ad9517_dev *dev,
		     uint32_t reg_addr,
		     uint16_t reg_val);
/*! Reads data from a register. */
int32_t ad9517_read(struct ad9517_dev *dev,
		    uint32_t reg_addr,
		    uint32_t *reg_value);
/*! Transfers the contents of the buffer registers into the active registers. */
int32_t ad9517_update(struct ad9517_dev *dev);
/*! Sets the VCO frequency. */
int64_t ad9517_vco_frequency(struct ad9517_dev *dev,
			     int64_t frequency);
/*! Sets the frequency on the specified channel. */
int64_t ad9517_frequency(struct ad9517_dev *dev,
			 int32_t channel,
			 int64_t frequency);
/*! Sets the phase on the specified channel. */
int32_t ad9517_phase(struct ad9517_dev *dev,
		     int32_t channel,
		     int32_t phase);
/*! Sets the power mode of the specified channel. */
int32_t ad9517_power_mode(struct ad9517_dev *dev,
			  int32_t channel,
			  int32_t mode);

#endif // __AD9517_H__
