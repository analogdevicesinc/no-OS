/***************************************************************************//**
 *   @file   AD9523.h
 *   @brief  Header file of AD9523 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2012-2016(c) Analog Devices, Inc.
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
 *
 *******************************************************************************/
#ifndef _AD9523_H_
#define _AD9523_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/****************************** AD9523 ****************************************/
/******************************************************************************/
/* Registers */

#define AD9523_READ						(1 << 15)
#define AD9523_WRITE						(0 << 15)
#define AD9523_CNT(x)						(((x) - 1) << 13)
#define AD9523_ADDR(x)						((x) & 0xFFF)

#define AD9523_R1B						(1 << 16)
#define AD9523_R2B						(2 << 16)
#define AD9523_R3B						(3 << 16)
#define AD9523_TRANSF_LEN(x)					((x) >> 16)

#define AD9523_SERIAL_PORT_CONFIG				(AD9523_R1B | 0x0)
#define AD9523_VERSION_REGISTER					(AD9523_R1B | 0x2)
#define AD9523_PART_REGISTER					(AD9523_R1B | 0x3)
#define AD9523_READBACK_CTRL					(AD9523_R1B | 0x4)

#define AD9523_EEPROM_CUSTOMER_VERSION_ID			(AD9523_R2B | 0x6)

#define AD9523_PLL1_REF_A_DIVIDER				(AD9523_R2B | 0x11)
#define AD9523_PLL1_REF_B_DIVIDER				(AD9523_R2B | 0x13)
#define AD9523_PLL1_REF_TEST_DIVIDER				(AD9523_R1B | 0x14)
#define AD9523_PLL1_FEEDBACK_DIVIDER				(AD9523_R2B | 0x17)
#define AD9523_PLL1_CHARGE_PUMP_CTRL				(AD9523_R2B | 0x19)
#define AD9523_PLL1_INPUT_RECEIVERS_CTRL			(AD9523_R1B | 0x1A)
#define AD9523_PLL1_REF_CTRL					(AD9523_R1B | 0x1B)
#define AD9523_PLL1_MISC_CTRL					(AD9523_R1B | 0x1C)
#define AD9523_PLL1_LOOP_FILTER_CTRL				(AD9523_R1B | 0x1D)

#define AD9523_PLL2_CHARGE_PUMP					(AD9523_R1B | 0xF0)
#define AD9523_PLL2_FEEDBACK_DIVIDER_AB				(AD9523_R1B | 0xF1)
#define AD9523_PLL2_CTRL					(AD9523_R1B | 0xF2)
#define AD9523_PLL2_VCO_CTRL					(AD9523_R1B | 0xF3)
#define AD9523_PLL2_VCO_DIVIDER					(AD9523_R1B | 0xF4)
#define AD9523_PLL2_LOOP_FILTER_CTRL				(AD9523_R2B | 0xF6)
#define AD9523_PLL2_R2_DIVIDER					(AD9523_R1B | 0xF7)

#define AD9523_CHANNEL_CLOCK_DIST(ch)				(AD9523_R3B | (0x192 + 3 * ch))

#define AD9523_PLL1_OUTPUT_CTRL					(AD9523_R1B | 0x1BA)
#define AD9523_PLL1_OUTPUT_CHANNEL_CTRL				(AD9523_R1B | 0x1BB)

#define AD9523_READBACK_0					(AD9523_R1B | 0x22C)
#define AD9523_READBACK_1					(AD9523_R1B | 0x22D)

#define AD9523_STATUS_SIGNALS					(AD9523_R3B | 0x232)
#define AD9523_POWER_DOWN_CTRL					(AD9523_R1B | 0x233)
#define AD9523_IO_UPDATE					(AD9523_R1B | 0x234)

#define AD9523_EEPROM_DATA_XFER_STATUS				(AD9523_R1B | 0xB00)
#define AD9523_EEPROM_ERROR_READBACK				(AD9523_R1B | 0xB01)
#define AD9523_EEPROM_CTRL1					(AD9523_R1B | 0xB02)
#define AD9523_EEPROM_CTRL2					(AD9523_R1B | 0xB03)

/* AD9523_SERIAL_PORT_CONFIG */

#define AD9523_SER_CONF_SDO_ACTIVE				((1 << 7) | (1 << 0))
#define AD9523_SER_CONF_SOFT_RESET				((1 << 5) | (1 << 2))

/* AD9523_READBACK_CTRL */
#define AD9523_READBACK_CTRL_READ_BUFFERED			(1 << 0)

/* AD9523_PLL1_CHARGE_PUMP_CTRL */
#define AD9523_PLL1_CHARGE_PUMP_CURRENT_nA(x)			(((x) / 500) & 0x7F)
#define AD9523_PLL1_CHARGE_PUMP_TRISTATE			(1 << 7)
#define AD9523_PLL1_CHARGE_PUMP_MODE_NORMAL			(3 << 8)
#define AD9523_PLL1_CHARGE_PUMP_MODE_PUMP_DOWN			(2 << 8)
#define AD9523_PLL1_CHARGE_PUMP_MODE_PUMP_UP			(1 << 8)
#define AD9523_PLL1_CHARGE_PUMP_MODE_TRISTATE			(0 << 8)
#define AD9523_PLL1_BACKLASH_PW_MIN				(0 << 10)
#define AD9523_PLL1_BACKLASH_PW_LOW				(1 << 10)
#define AD9523_PLL1_BACKLASH_PW_HIGH				(2 << 10)
#define AD9523_PLL1_BACKLASH_PW_MAX				(3 << 10)

/* AD9523_PLL1_INPUT_RECEIVERS_CTRL */
#define AD9523_PLL1_REF_TEST_RCV_EN				(1 << 7)
#define AD9523_PLL1_REFB_DIFF_RCV_EN				(1 << 6)
#define AD9523_PLL1_REFA_DIFF_RCV_EN				(1 << 5)
#define AD9523_PLL1_REFB_RCV_EN					(1 << 4)
#define AD9523_PLL1_REFA_RCV_EN					(1 << 3)
#define AD9523_PLL1_REFA_REFB_PWR_CTRL_EN			(1 << 2)
#define AD9523_PLL1_OSC_IN_CMOS_NEG_INP_EN			(1 << 1)
#define AD9523_PLL1_OSC_IN_DIFF_EN				(1 << 0)

/* AD9523_PLL1_REF_CTRL */
#define AD9523_PLL1_BYPASS_REF_TEST_DIV_EN			(1 << 7)
#define AD9523_PLL1_BYPASS_FEEDBACK_DIV_EN			(1 << 6)
#define AD9523_PLL1_ZERO_DELAY_MODE_INT				(1 << 5)
#define AD9523_PLL1_ZERO_DELAY_MODE_EXT				(0 << 5)
#define AD9523_PLL1_OSC_IN_PLL_FEEDBACK_EN			(1 << 4)
#define AD9523_PLL1_ZD_IN_CMOS_NEG_INP_EN			(1 << 3)
#define AD9523_PLL1_ZD_IN_DIFF_EN				(1 << 2)
#define AD9523_PLL1_REFB_CMOS_NEG_INP_EN			(1 << 1)
#define AD9523_PLL1_REFA_CMOS_NEG_INP_EN			(1 << 0)

/* AD9523_PLL1_MISC_CTRL */
#define AD9523_PLL1_REFB_INDEP_DIV_CTRL_EN			(1 << 7)
#define AD9523_PLL1_OSC_CTRL_FAIL_VCC_BY2_EN			(1 << 6)
#define AD9523_PLL1_REF_MODE(x)					((x) << 2)
#define AD9523_PLL1_BYPASS_REFB_DIV				(1 << 1)
#define AD9523_PLL1_BYPASS_REFA_DIV				(1 << 0)

/* AD9523_PLL1_LOOP_FILTER_CTRL */
#define AD9523_PLL1_LOOP_FILTER_RZERO(x)			((x) & 0xF)

/* AD9523_PLL2_CHARGE_PUMP */
#define AD9523_PLL2_CHARGE_PUMP_CURRENT_nA(x)			((x) / 3500)

/* AD9523_PLL2_FEEDBACK_DIVIDER_AB */
#define AD9523_PLL2_FB_NDIV_A_CNT(x)				(((x) & 0x3) << 6)
#define AD9523_PLL2_FB_NDIV_B_CNT(x)				(((x) & 0x3F) << 0)
#define AD9523_PLL2_FB_NDIV(a, b)				(4 * (b) + (a))

/* AD9523_PLL2_CTRL */
#define AD9523_PLL2_CHARGE_PUMP_MODE_NORMAL			(3 << 0)
#define AD9523_PLL2_CHARGE_PUMP_MODE_PUMP_DOWN			(2 << 0)
#define AD9523_PLL2_CHARGE_PUMP_MODE_PUMP_UP			(1 << 0)
#define AD9523_PLL2_CHARGE_PUMP_MODE_TRISTATE			(0 << 0)
#define AD9523_PLL2_BACKLASH_PW_MIN				(0 << 2)
#define AD9523_PLL2_BACKLASH_PW_LOW				(1 << 2)
#define AD9523_PLL2_BACKLASH_PW_HIGH				(2 << 2)
#define AD9523_PLL2_BACKLASH_PW_MAX				(3 << 1)
#define AD9523_PLL2_BACKLASH_CTRL_EN				(1 << 4)
#define AD9523_PLL2_FREQ_DOUBLER_EN				(1 << 5)
#define AD9523_PLL2_LOCK_DETECT_PWR_DOWN_EN			(1 << 7)

/* AD9523_PLL2_VCO_CTRL */
#define AD9523_PLL2_VCO_CALIBRATE				(1 << 1)
#define AD9523_PLL2_FORCE_VCO_MIDSCALE				(1 << 2)
#define AD9523_PLL2_FORCE_REFERENCE_VALID			(1 << 3)
#define AD9523_PLL2_FORCE_RELEASE_SYNC				(1 << 4)

/* AD9523_PLL2_VCO_DIVIDER */
#define AD9523_PLL2_VCO_DIV_M1(x)				((((x) - 3) & 0x3) << 0)
#define AD9523_PLL2_VCO_DIV_M2(x)				((((x) - 3) & 0x3) << 4)
#define AD9523_PLL2_VCO_DIV_M1_PWR_DOWN_EN			(1 << 2)
#define AD9523_PLL2_VCO_DIV_M2_PWR_DOWN_EN			(1 << 6)

/* AD9523_PLL2_LOOP_FILTER_CTRL */
#define AD9523_PLL2_LOOP_FILTER_CPOLE1(x)			(((x) & 0x7) << 0)
#define AD9523_PLL2_LOOP_FILTER_RZERO(x)			(((x) & 0x7) << 3)
#define AD9523_PLL2_LOOP_FILTER_RPOLE2(x)			(((x) & 0x7) << 6)
#define AD9523_PLL2_LOOP_FILTER_RZERO_BYPASS_EN			(1 << 8)

/* AD9523_PLL2_R2_DIVIDER */
#define AD9523_PLL2_R2_DIVIDER_VAL(x)				(((x) & 0x1F) << 0)

/* AD9523_CHANNEL_CLOCK_DIST */
#define AD9523_CLK_DIST_DIV_PHASE(x)				(((x) & 0x3F) << 18)
#define AD9523_CLK_DIST_DIV_PHASE_REV(x)			((ret >> 18) & 0x3F)
#define AD9523_CLK_DIST_DIV(x)					((((x) - 1) & 0x3FF) << 8)
#define AD9523_CLK_DIST_DIV_REV(x)				(((ret >> 8) & 0x3FF) + 1)
#define AD9523_CLK_DIST_INV_DIV_OUTPUT_EN			(1 << 7)
#define AD9523_CLK_DIST_IGNORE_SYNC_EN				(1 << 6)
#define AD9523_CLK_DIST_PWR_DOWN_EN				(1 << 5)
#define AD9523_CLK_DIST_LOW_PWR_MODE_EN				(1 << 4)
#define AD9523_CLK_DIST_DRIVER_MODE(x)				(((x) & 0xF) << 0)

/* AD9523_PLL1_OUTPUT_CTRL */
#define AD9523_PLL1_OUTP_CTRL_VCO_DIV_SEL_CH6_M2		(1 << 7)
#define AD9523_PLL1_OUTP_CTRL_VCO_DIV_SEL_CH5_M2		(1 << 6)
#define AD9523_PLL1_OUTP_CTRL_VCO_DIV_SEL_CH4_M2		(1 << 5)
#define AD9523_PLL1_OUTP_CTRL_CMOS_DRV_WEAK			(1 << 4)
#define AD9523_PLL1_OUTP_CTRL_OUTPUT_DIV_1			(0 << 0)
#define AD9523_PLL1_OUTP_CTRL_OUTPUT_DIV_2			(1 << 0)
#define AD9523_PLL1_OUTP_CTRL_OUTPUT_DIV_4			(2 << 0)
#define AD9523_PLL1_OUTP_CTRL_OUTPUT_DIV_8			(4 << 0)
#define AD9523_PLL1_OUTP_CTRL_OUTPUT_DIV_16			(8 << 0)

/* AD9523_PLL1_OUTPUT_CHANNEL_CTRL */
#define AD9523_PLL1_OUTP_CH_CTRL_OUTPUT_PWR_DOWN_EN		(1 << 7)
#define AD9523_PLL1_OUTP_CH_CTRL_VCO_DIV_SEL_CH9_M2		(1 << 6)
#define AD9523_PLL1_OUTP_CH_CTRL_VCO_DIV_SEL_CH8_M2		(1 << 5)
#define AD9523_PLL1_OUTP_CH_CTRL_VCO_DIV_SEL_CH7_M2		(1 << 4)
#define AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH3		(1 << 3)
#define AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH2		(1 << 2)
#define AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH1		(1 << 1)
#define AD9523_PLL1_OUTP_CH_CTRL_VCXO_SRC_SEL_CH0		(1 << 0)

/* AD9523_READBACK_0 */
#define AD9523_READBACK_0_STAT_PLL2_REF_CLK			(1 << 7)
#define AD9523_READBACK_0_STAT_PLL2_FB_CLK			(1 << 6)
#define AD9523_READBACK_0_STAT_VCXO				(1 << 5)
#define AD9523_READBACK_0_STAT_REF_TEST				(1 << 4)
#define AD9523_READBACK_0_STAT_REFB				(1 << 3)
#define AD9523_READBACK_0_STAT_REFA				(1 << 2)
#define AD9523_READBACK_0_STAT_PLL2_LD				(1 << 1)
#define AD9523_READBACK_0_STAT_PLL1_LD				(1 << 0)

/* AD9523_READBACK_1 */
#define AD9523_READBACK_1_HOLDOVER_ACTIVE			(1 << 3)
#define AD9523_READBACK_1_AUTOMODE_SEL_REFB			(1 << 2)
#define AD9523_READBACK_1_VCO_CALIB_IN_PROGRESS			(1 << 0)

/* AD9523_STATUS_SIGNALS */
#define AD9523_STATUS_SIGNALS_SYNC_MAN_CTRL			(1 << 16)
#define AD9523_STATUS_MONITOR_01_PLL12_LOCKED			(0x302)
/* AD9523_POWER_DOWN_CTRL */
#define AD9523_POWER_DOWN_CTRL_PLL1_PWR_DOWN			(1 << 2)
#define AD9523_POWER_DOWN_CTRL_PLL2_PWR_DOWN			(1 << 1)
#define AD9523_POWER_DOWN_CTRL_DIST_PWR_DOWN			(1 << 0)

/* AD9523_IO_UPDATE */
#define AD9523_IO_UPDATE_EN					(1 << 0)

/* AD9523_EEPROM_DATA_XFER_STATUS */
#define AD9523_EEPROM_DATA_XFER_IN_PROGRESS			(1 << 0)

/* AD9523_EEPROM_ERROR_READBACK */
#define AD9523_EEPROM_ERROR_READBACK_FAIL			(1 << 0)

/* AD9523_EEPROM_CTRL1 */
#define AD9523_EEPROM_CTRL1_SOFT_EEPROM				(1 << 1)
#define AD9523_EEPROM_CTRL1_EEPROM_WRITE_PROT_DIS		(1 << 0)

/* AD9523_EEPROM_CTRL2 */
#define AD9523_EEPROM_CTRL2_REG2EEPROM				(1 << 0)

#define AD9523_NUM_CHAN						14
#define AD9523_NUM_CHAN_ALT_CLK_SRC				10

#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof(ar[0]))

/******************************************************************************/
/************************ Types Definitions ***********************************/
/******************************************************************************/
enum outp_drv_mode
{
	TRISTATE,
	LVPECL_8mA,
	LVDS_4mA,
	LVDS_7mA,
	HSTL0_16mA,
	HSTL1_8mA,
	CMOS_CONF1,
	CMOS_CONF2,
	CMOS_CONF3,
	CMOS_CONF4,
	CMOS_CONF5,
	CMOS_CONF6,
	CMOS_CONF7,
	CMOS_CONF8,
	CMOS_CONF9
};

enum ref_sel_mode
{
	NONEREVERTIVE_STAY_ON_REFB,
	REVERT_TO_REFA,
	SELECT_REFA,
	SELECT_REFB,
	EXT_REF_SEL
};

/**
 * struct ad9523_channel_spec - Output channel configuration
 *
 * @channel_num: Output channel number.
 * @divider_output_invert_en: Invert the polarity of the output clock.
 * @sync_ignore_en: Ignore chip-level SYNC signal.
 * @low_power_mode_en: Reduce power used in the differential output modes.
 * @use_alt_clock_src: Channel divider uses alternative clk source.
 * @output_dis: Disables, powers down the entire channel.
 * @driver_mode: Output driver mode (logic level family).
 * @divider_phase: Divider initial phase after a SYNC. Range 0..63
 LSB = 1/2 of a period of the divider input clock.
 * @channel_divider: 10-bit channel divider.
 * @extended_name: Optional descriptive channel name.
 */

typedef struct
{
	unsigned channel_num;
	unsigned char divider_output_invert_en;
	unsigned char sync_ignore_en;
	unsigned char low_power_mode_en;
	unsigned char use_alt_clock_src; /* CH0..CH3 VCXO, CH4..CH9 VCO2 */
	unsigned char output_dis;
	unsigned char driver_mode;
	unsigned char divider_phase;
	unsigned short channel_divider;
	char extended_name[16];
} ad9523_channel_spec;

enum pll1_rzero_resistor {
	RZERO_883_OHM,
	RZERO_677_OHM,
	RZERO_341_OHM,
	RZERO_135_OHM,
	RZERO_10_OHM,
	RZERO_USE_EXT_RES = 8,
};

enum rpole2_resistor {
	RPOLE2_900_OHM,
	RPOLE2_450_OHM,
	RPOLE2_300_OHM,
	RPOLE2_225_OHM,
};

enum rzero_resistor {
	RZERO_3250_OHM,
	RZERO_2750_OHM,
	RZERO_2250_OHM,
	RZERO_2100_OHM,
	RZERO_3000_OHM,
	RZERO_2500_OHM,
	RZERO_2000_OHM,
	RZERO_1850_OHM,
};

enum cpole1_capacitor {
	CPOLE1_0_PF,
	CPOLE1_8_PF,
	CPOLE1_16_PF,
	CPOLE1_24_PF,
	_CPOLE1_24_PF, /* place holder */
	CPOLE1_32_PF,
	CPOLE1_40_PF,
	CPOLE1_48_PF,
};

/**
 * struct ad9523_platform_data - platform specific information
 *
 * @vcxo_freq: External VCXO frequency in Hz
 * @refa_diff_rcv_en: REFA differential/single-ended input selection.
 * @refb_diff_rcv_en: REFB differential/single-ended input selection.
 * @zd_in_diff_en: Zero Delay differential/single-ended input selection.
 * @osc_in_diff_en: OSC differential/ single-ended input selection.
 * @refa_cmos_neg_inp_en: REFA single-ended neg./pos. input enable.
 * @refb_cmos_neg_inp_en: REFB single-ended neg./pos. input enable.
 * @zd_in_cmos_neg_inp_en: Zero Delay single-ended neg./pos. input enable.
 * @osc_in_cmos_neg_inp_en: OSC single-ended neg./pos. input enable.
 * @refa_r_div: PLL1 10-bit REFA R divider.
 * @refb_r_div: PLL1 10-bit REFB R divider.
 * @pll1_feedback_div: PLL1 10-bit Feedback N divider.
 * @pll1_charge_pump_current_nA: Magnitude of PLL1 charge pump current (nA).
 * @zero_delay_mode_internal_en: Internal, external Zero Delay mode selection.
 * @osc_in_feedback_en: PLL1 feedback path, local feedback from
 *			the OSC_IN receiver or zero delay mode
 * @pll1_loop_filter_rzero: PLL1 Loop Filter Zero Resistor selection.
 * @ref_mode: Reference selection mode.
 * @pll2_charge_pump_current_nA: Magnitude of PLL2 charge pump current (nA).
 * @pll2_ndiv_a_cnt: PLL2 Feedback N-divider, A Counter, range 0..4.
 * @pll2_ndiv_b_cnt: PLL2 Feedback N-divider, B Counter, range 0..63.
 * @pll2_freq_doubler_en: PLL2 frequency doubler enable.
 * @pll2_r2_div: PLL2 R2 divider, range 0..31.
 * @pll2_vco_diff_m1: VCO1 divider, range 3..5.
 * @pll2_vco_diff_m2: VCO2 divider, range 3..5.
 * @rpole2: PLL2 loop filter Rpole resistor value.
 * @rzero: PLL2 loop filter Rzero resistor value.
 * @cpole1: PLL2 loop filter Cpole capacitor value.
 * @rzero_bypass_en: PLL2 loop filter Rzero bypass enable.
 * @num_channels: Array size of struct ad9523_channel_spec.
 * @channels: Pointer to channel array.
 * @name: Optional alternative iio device name.
 */

typedef struct
{
	unsigned long vcxo_freq;
	unsigned char spi3wire;

	/* Differential/ Single-Ended Input Configuration */
	unsigned char refa_diff_rcv_en;
	unsigned char refb_diff_rcv_en;
	unsigned char zd_in_diff_en;
	unsigned char osc_in_diff_en;

	/*
	 * Valid if differential input disabled
	 * if not true defaults to pos input
	 */
	unsigned char refa_cmos_neg_inp_en;
	unsigned char refb_cmos_neg_inp_en;
	unsigned char zd_in_cmos_neg_inp_en;
	unsigned char osc_in_cmos_neg_inp_en;

	/* PLL1 Setting */
	unsigned short refa_r_div;
	unsigned short refb_r_div;
	unsigned short pll1_feedback_div;
	unsigned short pll1_charge_pump_current_nA;
	unsigned char zero_delay_mode_internal_en;
	unsigned char osc_in_feedback_en;
	unsigned char pll1_bypass_en;
	unsigned char pll1_loop_filter_rzero;

	/* Reference */
	unsigned char ref_mode;

	/* PLL2 Setting */
	unsigned int pll2_charge_pump_current_nA;
	unsigned char pll2_ndiv_a_cnt;
	unsigned char pll2_ndiv_b_cnt;
	unsigned char pll2_freq_doubler_en;
	unsigned char pll2_r2_div;
	unsigned char pll2_vco_diff_m1; /* 3..5 */
	unsigned char pll2_vco_diff_m2; /* 3..5 */

	/* Loop Filter PLL2 */
	unsigned char rpole2;
	unsigned char rzero;
	unsigned char cpole1;
	unsigned char rzero_bypass_en;

	/* Output Channel Configuration */
	int num_channels;
	ad9523_channel_spec *channels;

	char name[16];
} ad9523_platform_data;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Reads the value of the selected register. */
int32_t ad9523_spi_read(spi_device *dev,
		uint32_t reg_addr,
		uint32_t *reg_data);
/* Writes a value to the selected register. */
int32_t ad9523_spi_write(spi_device *dev,
		uint32_t reg_addr,
		uint32_t reg_data);
/* Updates the AD9523 configuration */
int32_t ad9523_io_update(spi_device *dev);
/* Sets the clock provider for selected channel. */
int32_t ad9523_vco_out_map(spi_device *dev,
		uint32_t ch,
		uint32_t out);
/* Updates the AD9523 configuration. */
int32_t ad9523_sync(spi_device *dev);
/* Initialize the AD9523 data structure*/
int32_t ad9523_init(ad9523_platform_data *pdata);
/* Configure the AD9523. */
int32_t ad9523_setup(spi_device *dev,
		ad9523_platform_data *pdata);
#endif // __AD9523_H__
