/***************************************************************************//**
 *   @file   AD9528.h
 *   @brief  Header file of AD9528 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015-2016(c) Analog Devices, Inc.
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
#ifndef _AD9528_H_
#define _AD9528_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <altxil_base.h>

/******************************************************************************/
/****************************** AD9528 ****************************************/
/******************************************************************************/
/* Registers */

#define AD9528_READ	(1 << 15)
#define AD9528_WRITE	(0 << 15)
#define AD9528_CNT(x)	(((x) - 1) << 13)
#define AD9528_ADDR(x)	((x) & 0xFFF)

#define AD9528_1B(x)	((1 << 16) | ((x) + 0))
#define AD9528_2B(x)	((2 << 16) | ((x) + 1))
#define AD9528_3B(x)	((3 << 16) | ((x) + 2))
#define AD9528_4B(x)	((4 << 16) | ((x) + 3))
#define AD9528_TRANSF_LEN(x)			((x) >> 16)

#define AD9528_SERIAL_PORT_CONFIG		AD9528_1B(0x0)
#define AD9528_SERIAL_PORT_CONFIG_B		AD9528_1B(0x1)
#define AD9528_CHIP_ID				AD9528_4B(0x3)
#define AD9528_IO_UPDATE			AD9528_1B(0xF)

#define AD9528_PLL1_REF_A_DIVIDER		AD9528_2B(0x100)
#define AD9528_PLL1_REF_B_DIVIDER		AD9528_2B(0x102)
#define AD9528_PLL1_FEEDBACK_DIVIDER		AD9528_2B(0x104)
#define AD9528_PLL1_CHARGE_PUMP_CTRL		AD9528_2B(0x106)
#define AD9528_PLL1_CTRL			AD9528_3B(0x108)

#define AD9528_PLL2_CHARGE_PUMP			AD9528_1B(0x200)
#define AD9528_PLL2_FEEDBACK_DIVIDER_AB		AD9528_1B(0x201)
#define AD9528_PLL2_CTRL			AD9528_1B(0x202)
#define AD9528_PLL2_VCO_CTRL			AD9528_1B(0x203)
#define AD9528_PLL2_VCO_DIVIDER			AD9528_1B(0x204)
#define AD9528_PLL2_LOOP_FILTER_CTRL		AD9528_2B(0x205)
#define AD9528_PLL2_R1_DIVIDER			AD9528_1B(0x207)
#define AD9528_PLL2_N2_DIVIDER			AD9528_1B(0x208)

#define AD9528_CHANNEL_OUTPUT(ch)		AD9528_3B(0x300 + 3 * ch)
#define AD9528_CHANNEL_SYNC			AD9528_1B(0x32A)
#define AD9528_CHANNEL_SYNC_IGNORE		AD9528_2B(0x32B)

#define AD9528_SYSREF_K_DIVIDER			AD9528_2B(0x400)
#define AD9528_SYSREF_CTRL			AD9528_2B(0x402)

#define AD9528_PD_EN				AD9528_1B(0x500)
#define AD9528_CHANNEL_PD_EN			AD9528_2B(0x501)

#define AD9528_READBACK				AD9528_2B(0x508)

/* AD9528_SERIAL_PORT_CONFIG */
#define AD9528_SER_CONF_SOFT_RESET		((1 << 0) | (1 << 7))
#define AD9528_SER_CONF_LSB_FIRST		((1 << 1) | (1 << 6))
#define AD9528_SER_CONF_ADDR_INCREMENT		((1 << 2) | (1 << 5))
#define AD9528_SER_CONF_SDO_ACTIVE		((1 << 3) | (1 << 4))

/* AD9528_SERIAL_PORT_CONFIG_B */
#define AD9528_SER_CONF_READ_BUFFERED		(1 << 5)
#define AD9528_SER_CONF_RESET_SANS_REGMAP	(1 << 2)

/* AD9528_IO_UPDATE */
#define AD9528_IO_UPDATE_EN			(1 << 0)

/* AD9528_PLL1_CHARGE_PUMP_CTRL */
#define AD9528_PLL1_CHARGE_PUMP_AUTO_TRISTATE_DIS	(1 << 12)
#define AD9528_PLL1_CHARGE_PUMP_MODE_NORMAL	(3 << 8)
#define AD9528_PLL1_CHARGE_PUMP_MODE_PUMP_DOWN	(2 << 8)
#define AD9528_PLL1_CHARGE_PUMP_MODE_PUMP_UP	(1 << 8)
#define AD9528_PLL1_CHARGE_PUMP_MODE_TRISTATE	(0 << 8)
#define AD9528_PLL1_CHARGE_PUMP_TRISTATE	(1 << 7)
#define AD9528_PLL1_CHARGE_PUMP_CURRENT_nA(x)	(((x) / 500) & 0x7F)

/* AD9528_PLL1_CTRL */
#define AD9528_PLL1_OSC_CTRL_FAIL_VCC_BY2_EN	(1 << 18)
#define AD9528_PLL1_REF_MODE(x)			((x) << 16)
#define AD9528_PLL1_FEEDBACK_BYPASS_EN		(1 << 13)
#define AD9528_PLL1_REFB_BYPASS_EN		(1 << 12)
#define AD9528_PLL1_REFA_BYPASS_EN		(1 << 11)
#define AD9528_PLL1_SOURCE_VCXO			(1 << 10)
#define AD9528_PLL1_REFB_CMOS_NEG_INP_EN	(1 << 9)
#define AD9528_PLL1_REFA_CMOS_NEG_INP_EN	(1 << 8)
#define AD9528_PLL1_FREQ_DETECTOR_PD_EN		(1 << 7)
#define AD9528_PLL1_REFB_DIFF_RCV_EN		(1 << 6)
#define AD9528_PLL1_REFA_DIFF_RCV_EN		(1 << 5)
#define AD9528_PLL1_REFB_RCV_EN			(1 << 4)
#define AD9528_PLL1_REFA_RCV_EN			(1 << 3)
#define AD9528_PLL1_VCXO_RCV_PD_EN		(1 << 2)
#define AD9528_PLL1_OSC_IN_CMOS_NEG_INP_EN	(1 << 1)
#define AD9528_PLL1_OSC_IN_DIFF_EN		(1 << 0)

/* AD9528_PLL2_CHARGE_PUMP */
#define AD9528_PLL2_CHARGE_PUMP_CURRENT_nA(x)	((x) / 3500)

/* AD9528_PLL2_FEEDBACK_DIVIDER_AB */
#define AD9528_PLL2_FB_NDIV_A_CNT(x)		(((x) & 0x3) << 6)
#define AD9528_PLL2_FB_NDIV_B_CNT(x)		(((x) & 0x3F) << 0)
#define AD9528_PLL2_FB_NDIV(a, b)		(4 * (b) + (a))

/* AD9528_PLL2_CTRL */
#define AD9528_PLL2_LOCK_DETECT_PWR_DOWN_EN	(1 << 7)
#define AD9528_PLL2_FREQ_DOUBLER_EN		(1 << 5)
#define AD9528_PLL2_CHARGE_PUMP_MODE_NORMAL	(3 << 0)
#define AD9528_PLL2_CHARGE_PUMP_MODE_PUMP_DOWN	(2 << 0)
#define AD9528_PLL2_CHARGE_PUMP_MODE_PUMP_UP	(1 << 0)
#define AD9528_PLL2_CHARGE_PUMP_MODE_TRISTATE	(0 << 0)

/* AD9528_PLL2_VCO_CTRL */
#define	AD9528_PLL2_DOUBLER_R1_EN		(1 << 4)
#define AD9528_PLL2_FORCE_REFERENCE_VALID	(1 << 2)
#define AD9528_PLL2_FORCE_VCO_MIDSCALE		(1 << 1)
#define AD9528_PLL2_VCO_CALIBRATE		(1 << 0)

/* AD9528_PLL2_VCO_DIVIDER */
#define AD9528_PLL2_VCO_DIV_M1_PWR_DOWN_EN	(1 << 3)
#define AD9528_PLL2_VCO_DIV_M1(x)		(((x) & 0x7) << 0)

/* AD9528_PLL2_LOOP_FILTER_CTRL */
#define AD9528_PLL2_LOOP_FILTER_RZERO_BYPASS_EN	(1 << 8)
#define AD9528_PLL2_LOOP_FILTER_RPOLE2(x)	(((x) & 0x3) << 6)
#define AD9528_PLL2_LOOP_FILTER_RZERO(x)	(((x) & 0x7) << 3)
#define AD9528_PLL2_LOOP_FILTER_CPOLE1(x)	(((x) & 0x7) << 0)

/* AD9528_PLL2_R1_DIVIDER */
#define AD9528_PLL2_R1_DIV(x)			(((x) & 0x1F) << 0)

/* AD9528_PLL2_N2_DIVIDER */
#define AD9528_PLL2_N2_DIV(x)			((((x) - 1) & 0xFF) << 0)

/* AD9528_CHANNEL_OUTPUT */
#define AD9528_CLK_DIST_DIV(x)			((((x) - 1) & 0xFF) << 16)
#define AD9528_CLK_DIST_DIV_REV(x)		((((x) >> 16) & 0xFF) + 1)
#define AD9528_CLK_DIST_DRIVER_MODE(x)		(((x) & 0x3) << 13)
#define AD9528_CLK_DIST_DRIVER_MODE_REV(x)	(((x) >> 13) & 0x3)
#define AD9528_CLK_DIST_DIV_PHASE(x)		(((x) & 0x3F) << 8)
#define AD9528_CLK_DIST_DIV_PHASE_REV(x)	(((x) >> 8) & 0x3F)
#define AD9528_CLK_DIST_CTRL(x)			(((x) & 0x7) << 5)
#define AD9528_CLK_DIST_CTRL_REV(x)		(((x) >> 5) & 0x7)

#if 0
/* Leftovers */
#define AD9528_CLK_DIST_INV_DIV_OUTPUT_EN	(1 << 7)
#endif

/* AD9528_CHANNEL_SYNC */
#define AD9528_CHANNEL_SYNC_SET			(1 << 0)

/* AD9528_CHANNEL_SYNC_IGNORE */
#define AD9528_CHANNEL_IGNORE_MASK(x)		(((x) & 0x3FFF) << 0)
#define AD9528_CHANNEL_IGNORE_MASK_REV(x)	(((x) >> 0) & 0x3FFF)

/* AD9528_SYSREF_K_DIVIDER */
#define AD9528_SYSREF_K_DIV(x)			(((x) & 0xFFFF) << 0)

/* AD9528_SYSREF_CTRL */
#define AD9528_SYSREF_SOURCE(x)			(((x) & 0x3) << 14)
#define AD9528_SYSREF_PATTERN_MODE(x)		(((x) & 0x3) << 12)
#define AD9528_SYSREF_NSHOT_MODE(x)		(((x) & 0x7) << 9)
#define AD9528_SYSREF_PATTERN_REQ		(1 << 8)
#define AD9528_SYSREF_REQUEST_BY_PIN		(1 << 7)
#define AD9528_SYSREF_PATTERN_TRIGGER_CTRL(x)	(((x) & 0x3) << 5)
#define AD9528_SYSREF_RESAMPLER_CLK_SRC_PLL1	(1 << 4)
#define AD9528_SYSREF_PATTERN_CLK_SRC_PLL1	(1 << 3)
#define AD9528_SYSREF_TEST_MODE(x)		(((x) & 0x3) << 1)
#define AD9528_SYSREF_RESET			(1 << 0)

/* AD9528_PD_EN */
#define AD9528_PD_BIAS				(1 << 4)
#define AD9528_PD_PLL2				(1 << 3)
#define AD9528_PD_PLL1				(1 << 2)
#define AD9528_PD_OUT_CLOCKS			(1 << 1)
#define AD9528_PD_CHIP				(1 << 0)

/* AD9528_CHANNEL_PD_EN */
#define AD9528_CHANNEL_PD_MASK(x)		(((x) & 0x3FFF) << 0)
#define AD9528_CHANNEL_PD_MASK_REV(x)		(((x) >> 0) & 0x3FFF)


/* AD9528_READBACK */
#define AD9528_IS_CALIBRATING		(1 << 8)
#define	AD9528_PLL2_OK				(1 << 7)
#define	AD9528_PLL1_OK				(1 << 6)
#define AD9528_VCXO_OK				(1 << 5)
#define AD9528_REFA_REFB_NOK		(1 << 4)
#define AD9528_REFB_OK				(1 << 3)
#define AD9528_REFA_OK				(1 << 2)
#define AD9528_PLL2_LOCKED			(1 << 1)
#define AD9528_PLL1_LOCKED			(1 << 0)


#define AD9528_NUM_CHAN				14

#define AD9528_SPI_MAGIC			0x00FF05

/* Output Driver Mode */
#define DRIVER_MODE_LVDS		0
#define DRIVER_MODE_LVDS_BOOST	1
#define DRIVER_MODE_HSTL		2

/* Output Signal Source */
#define SOURCE_VCO				0
#define SOURCE_VCXO				1
#define SOURCE_SYSREF_VCO		2
#define SOURCE_SYSREF_VCXO		3
#define SOURCE_VCXO_INV			5
#define SOURCE_SYSREF_VCXO_INV	7

/* Reference Selection Mode */
#define REF_MODE_STAY_ON_REFB	0
#define REF_MODE_REVERT_TO_REFA	1
#define REF_MODE_SELECT_REFA	2
#define REF_MODE_SELECT_REFB	3
#define REF_MODE_EXT_REF		4

/* Sysref Source */
#define SYSREF_SRC_EXTERNAL				0
#define SYSREF_SRC_EXTERNAL_RESAMPLED	1
#define SYSREF_SRC_INTERNAL				2

/* Sysref Pattern Mode */
#define SYSREF_PATTERN_NSHOT			0
#define SYSREF_PATTERN_CONTINUOUS		1
#define SYSREF_PATTERN_PRBS				2
#define SYSREF_PATTERN_STOP				3

/* Rpole2 resistor */
#define RPOLE2_900_OHM	0
#define RPOLE2_450_OHM	1
#define RPOLE2_300_OHM	2
#define RPOLE2_225_OHM	3

/* Rzero resistor */
#define RZERO_3250_OHM	0
#define RZERO_2750_OHM	1
#define RZERO_2250_OHM	2
#define RZERO_2100_OHM	3
#define RZERO_3000_OHM	4
#define RZERO_2500_OHM	5
#define RZERO_2000_OHM	6
#define RZERO_1850_OHM	7

/* Cpole1 capacitor */
#define CPOLE1_0_PF		0
#define CPOLE1_8_PF		1
#define CPOLE1_16_PF	2
#define CPOLE1_24_PF	3
#define CPOLE1_32_PF	5
#define CPOLE1_40_PF	6
#define CPOLE1_48_PF	7

/******************************************************************************/
/************************ Types Definitions ***********************************/
/******************************************************************************/

/**
 * struct ad9528_channel_spec - Output channel configuration
 *
 * @channel_num: Output channel number.
 * @sync_ignore_en: Ignore chip-level SYNC signal.
 * @output_dis: Disables, powers down the entire channel.
 * @driver_mode: Output driver mode (logic level family).
 * @divider_phase: Divider initial phase after a SYNC. Range 0..63
		   LSB = 1/2 of a period of the divider input clock.
 * @channel_divider: 10-bit channel divider.
 * @extended_name: Optional descriptive channel name.
 */
struct ad9528_channel_spec {
	unsigned		channel_num;
	unsigned char			sync_ignore_en;
				 /* CH0..CH3 VCXO, CH4..CH9 VCO2 */
	unsigned char			output_dis;
	unsigned char		driver_mode;
	unsigned char		signal_source;
	unsigned char		divider_phase;
	unsigned short		channel_divider;
	char			extended_name[16];
};

/**
 * struct ad9528_platform_data - platform specific information
 *
 * @vcxo_freq: External VCXO frequency in Hz
 * @spi3wire: SPI 3-Wire mode enable;
 * @refa_en: REFA input enable.
 * @refb_en: REFB input enable.
 * @refa_diff_rcv_en: REFA differential/single-ended input selection.
 * @refb_diff_rcv_en: REFB differential/single-ended input selection.
 * @osc_in_diff_en: OSC differential/ single-ended input selection.
 * @refa_cmos_neg_inp_en: REFA single-ended neg./pos. input enable.
 * @refb_cmos_neg_inp_en: REFB single-ended neg./pos. input enable.
 * @osc_in_cmos_neg_inp_en: OSC single-ended neg./pos. input enable.
 * @refa_r_div: PLL1 10-bit REFA R divider.
 * @refb_r_div: PLL1 10-bit REFB R divider.
 * @pll1_feedback_div: PLL1 10-bit Feedback N divider.
 * @pll1_feedback_src_vcxo: PLL1 Feedback source, True = VCXO, False = VCO
 * @pll1_charge_pump_current_nA: Magnitude of PLL1 charge pump current (nA).
 * @pll1_bypass_en: Bypass PLL1 - Single loop mode
 * @ref_mode: Reference selection mode.
 * @sysref_src: SYSREF pattern generator clock source
 * @sysref_k_div: SYSREF pattern generator K divider
 * @pll2_charge_pump_current_nA: Magnitude of PLL2 charge pump current (nA).
 * @pll2_ndiv_a_cnt: PLL2 Feedback N-divider, A Counter, range 0..4.
 * @pll2_ndiv_b_cnt: PLL2 Feedback N-divider, B Counter, range 0..63.
 * @pll2_freq_doubler_en: PLL2 frequency doubler enable.
 * @pll2_r1_div: PLL2 R1 divider, range 1..31.
 * @pll2_n2_div: PLL2 N2 divider, range 1..256.
 * @pll2_vco_diff_m1: VCO1 divider, range 3..5.
 * @rpole2: PLL2 loop filter Rpole resistor value.
 * @rzero: PLL2 loop filter Rzero resistor value.
 * @cpole1: PLL2 loop filter Cpole capacitor value.
 * @rzero_bypass_en: PLL2 loop filter Rzero bypass enable.
 * @num_channels: Array size of struct ad9528_channel_spec.
 * @channels: Pointer to channel array.
 */
struct ad9528_platform_data
{
	uint32_t	vcxo_freq;
	uint8_t	spi3wire;

	/* REFA / REFB input configuration */
	uint8_t	refa_en;
	uint8_t	refb_en;

	/* Differential/ Single-Ended Input Configuration */
	uint8_t	refa_diff_rcv_en;
	uint8_t	refb_diff_rcv_en;
	uint8_t	osc_in_diff_en;

	/*
	 * Valid if differential input disabled
	 * if false defaults to pos input
	 */
	uint8_t	refa_cmos_neg_inp_en;
	uint8_t	refb_cmos_neg_inp_en;
	uint8_t	osc_in_cmos_neg_inp_en;

	/* PLL1 Setting */
	uint16_t	refa_r_div;
	uint16_t	refb_r_div;
	uint16_t	pll1_feedback_div;
	uint8_t	pll1_feedback_src_vcxo;
	uint16_t	pll1_charge_pump_current_nA;
	uint8_t	pll1_bypass_en;

	/* Reference */
	uint8_t	ref_mode;
	uint8_t	sysref_src;
	uint16_t	sysref_k_div;

	/* PLL2 Setting */
	unsigned int	pll2_charge_pump_current_nA;
	uint8_t	pll2_ndiv_a_cnt;
	uint8_t	pll2_ndiv_b_cnt;
	uint8_t	pll2_freq_doubler_en;
	uint8_t	pll2_r1_div;
	uint8_t	pll2_n2_div;
	uint8_t	pll2_vco_diff_m1; /* 3..5 */

	/* Loop Filter PLL2 */
	uint8_t	rpole2;
	uint8_t	rzero;
	uint8_t	cpole1;
	uint8_t	rzero_bypass_en;

	/* Output Channel Configuration */
	int							num_channels;
	struct ad9528_channel_spec	*channels;
};

#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof(ar[0]))

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9528_setup(spi_device *dev,
					 struct ad9528_platform_data ad9528_pdata);

#endif // __AD9528_H__
