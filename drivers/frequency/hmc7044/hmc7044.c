/***************************************************************************//**
 *   @file   hmc7044.c
 *   @brief  Implementation of HMC7044, HMC7043 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018-2020(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "hmc7044.h"
#include "jesd204.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define HMC7044_WRITE		(0 << 15)
#define HMC7044_READ		(1 << 15)
#define HMC7044_CNT(x)		(((x) - 1) << 13)
#define HMC7044_ADDR(x)		((x) & 0xFFF)

/* Global Control */
#define HMC7044_REG_SOFT_RESET		0x0000
#define HMC7044_SOFT_RESET		NO_OS_BIT(0)

#define HMC7044_REG_REQ_MODE_0		0x0001
#define HMC7044_RESEED_REQ		NO_OS_BIT(7)
#define HMC7044_HIGH_PERF_DISTRIB_PATH	NO_OS_BIT(6)
#define HMC7044_HIGH_PERF_PLL_VCO	NO_OS_BIT(5)
#define HMC7044_FORCE_HOLDOVER		NO_OS_BIT(4)
#define HMC7044_MUTE_OUT_DIV		NO_OS_BIT(3)
#define HMC7044_PULSE_GEN_REQ		NO_OS_BIT(2)
#define HMC7044_RESTART_DIV_FSM		NO_OS_BIT(1)
#define HMC7044_SLEEP_MODE		NO_OS_BIT(0)

#define HMC7044_REG_REQ_MODE_1		0x0002
#define HMC7044_PLL2_AUTOTUNE_TRIG	NO_OS_BIT(2)
#define HMC7044_SLIP_REQ		NO_OS_BIT(1)

#define HMC7044_REG_EN_CTRL_0		0x0003
#define HMC7044_RF_RESEEDER_EN		NO_OS_BIT(5)
#define HMC7044_VCO_SEL(x)		(((x) & 0x3) << 3)
#define HMC7044_VCO_EXT			0
#define HMC7044_VCO_HIGH		1
#define HMC7044_VCO_LOW			2
#define HMC7044_SYSREF_TIMER_EN		NO_OS_BIT(2)
#define HMC7044_PLL2_EN			NO_OS_BIT(1)
#define HMC7044_PLL1_EN			NO_OS_BIT(0)

#define HMC7044_REG_EN_CTRL_1		0x0004
#define HMC7044_SEVEN_PAIRS(x)		((x) & 0x7f)

#define HMC7044_REG_GLOB_MODE		0x0005
#define HMC7044_REF_PATH_EN(x)		((x) & 0xf)
#define HMC7044_RFSYNC_EN		NO_OS_BIT(4)
#define HMC7044_VCOIN_MODE_EN		NO_OS_BIT(5)
#define HMC7044_SYNC_PIN_MODE(x)	(((x) & 0x3) << 6)

#define HMC7044_REG_SCRATCHPAD		0x0008

/* PLL1 */
#define HMC7044_REG_CLKIN0_BUF_CTRL	0x000A
#define HMC7044_REG_CLKIN1_BUF_CTRL	0x000B
#define HMC7044_REG_CLKIN2_BUF_CTRL	0x000C
#define HMC7044_REG_CLKIN3_BUF_CTRL	0x000D
#define HMC7044_REG_OSCIN_BUF_CTRL	0x000E

#define HMC7044_REG_PLL1_REF_PRIO_CTRL	0x0014

#define HMC7044_HIGH_Z_EN		NO_OS_BIT(4)
#define HMC7044_LVPECL_EN		NO_OS_BIT(3)
#define HMC7044_AC_COUPLING_EN		NO_OS_BIT(2)
#define HMC7044_100_OHM_EN		NO_OS_BIT(1)
#define HMC7044_BUF_EN			NO_OS_BIT(0)

#define HMC7044_REG_PLL1_CP_CTRL	0x001A
#define HMC7044_PLL1_CP_CURRENT(x)	((x) & 0xf)

#define HMC7044_REG_CLKIN_PRESCALER(x)	(0x001C + (x))
#define HMC7044_REG_OSCIN_PRESCALER	0x0020

#define HMC7044_REG_PLL1_R_LSB		0x0021
#define HMC7044_R1_LSB(x)		((x) & 0xff)

#define HMC7044_REG_PLL1_R_MSB		0x0022
#define HMC7044_R1_MSB(x)		(((x) & 0xff00) >> 8)

#define HMC7044_REG_PLL1_N_LSB		0x0026
#define HMC7044_N1_LSB(x)		((x) & 0xff)

#define HMC7044_REG_PLL1_N_MSB		0x0027
#define HMC7044_N1_MSB(x)		(((x) & 0xff00) >> 8)

#define HMC7044_REG_PLL1_LOCK_DETECT	0x0028
#define HMC7044_LOCK_DETECT_SLIP	NO_OS_BIT(5)
#define HMC7044_LOCK_DETECT_TIMER(x)	((x) & 0x1f)

#define HMC7044_REG_PLL1_REF_SWITCH	0x0029
#define HMC7044_BYPASS_DEBOUNCER	NO_OS_BIT(5)
#define HMC7044_MANUAL_MODE_SWITCH(x)	(((x) & 0x3) << 3)
#define HMC7044_HOLDOVER_DAC		NO_OS_BIT(2)
#define HMC7044_AUTO_REVERT_SWITCH	NO_OS_BIT(1)
#define HMC7044_AUTO_MODE_SWITCH	NO_OS_BIT(0)

/* PLL2 */
#define HMC7044_REG_PLL2_FREQ_DOUBLER	0x0032
#define HMC7044_PLL2_FREQ_DOUBLER_DIS	NO_OS_BIT(0)

#define HMC7044_REG_PLL2_R_LSB		0x0033
#define HMC7044_R2_LSB(x)		((x) & 0xff)

#define HMC7044_REG_PLL2_R_MSB		0x0034
#define HMC7044_R2_MSB(x)		(((x) & 0xf00) >> 8)

#define HMC7044_REG_PLL2_N_LSB		0x0035
#define HMC7044_N2_LSB(x)		((x) & 0xff)

#define HMC7044_REG_PLL2_N_MSB		0x0036
#define HMC7044_N2_MSB(x)		(((x) & 0xff00) >> 8)

#define HMC7044_REG_OSCOUT_PATH		0x0039
#define HMC7044_REG_OSCOUT_DRIVER_0	0x003A
#define HMC7044_REG_OSCOUT_DRIVER_1	0x003B

/* GPIO/SDATA Control */
#define HMC7044_REG_GPI_CTRL(x)		(0x0046 + (x))
#define HMC7044_REG_GPI_SEL(x)		((x) & 0xf)

#define HMC7044_REG_GPO_CTRL(x)		(0x0050 + (x))
#define HMC7044_GPO_SEL(x)		(((x) & 0x3f) << 2)
#define HMC7044_GPO_MODE		NO_OS_BIT(1)
#define HMC7044_GPO_EN			NO_OS_BIT(0)

/* SYSREF/SYNC Control */
#define HMC7044_REG_PULSE_GEN		0x005A
#define HMC7044_PULSE_GEN_MODE(x)	((x) & 0x7)

#define HMC7044_REG_SYNC		0x005B
#define HMC7044_SYNC_RETIME		NO_OS_BIT(2)
#define HMC7044_SYNC_THROUGH_PLL2	NO_OS_BIT(1)
#define HMC7044_SYNC_POLARITY		NO_OS_BIT(0)

#define HMC7044_REG_SYSREF_TIMER_LSB	0x005C
#define HMC7044_SYSREF_TIMER_LSB(x)	((x) & 0xff)

#define HMC7044_REG_SYSREF_TIMER_MSB	0x005D
#define HMC7044_SYSREF_TIMER_MSB(x)	(((x) & 0xf00) >> 8)

#define HMC7044_CLK_INPUT_CTRL		0x0064
#define HMC7044_LOW_FREQ_INPUT_MODE	NO_OS_BIT(0)
#define HMC7044_DIV_2_INPUT_MODE	NO_OS_BIT(1)

/* Status and Alarm readback */
#define HMC7044_REG_ALARM_READBACK	0x007D
#define HMC7044_REG_PLL1_STATUS		0x0082

#define HMC7044_PLL1_FSM_STATE(x)	((x) & 0x7)
#define HMC7044_PLL1_ACTIVE_CLKIN(x)	(((x) >> 3) & 0x3)

#define HMC7044_PLL2_LOCK_DETECT(x)	((x) & 0x1)
#define HMC7044_SYSREF_SYNC_STAT(x)	((x) & 0x2)
#define HMC7044_CLK_OUT_PH_STATUS(x)	((x) & 0x4)
#define HMC7044_PLL1_PLL2_LOCK_STAT(x)	((x) & 0x8)
#define HMC7044_SYNC_REQ_STATUS(x)	((x) & 0x10)

/* Other Controls */
#define HMC7044_REG_CLK_OUT_DRV_LOW_PW	0x009F
#define HMC7044_REG_CLK_OUT_DRV_HIGH_PW	0x00A0
#define HMC7044_REG_PLL1_DELAY		0x00A5
#define HMC7044_REG_PLL1_HOLDOVER	0x00A8
#define HMC7044_REG_VTUNE_PRESET	0x00B0

/* Clock Distribution */
#define HMC7044_REG_CH_OUT_CRTL_0(ch)	(0x00C8 + 0xA * (ch))
#define HMC7044_HI_PERF_MODE		NO_OS_BIT(7)
#define HMC7044_SYNC_EN			NO_OS_BIT(6)
#define HMC7044_CH_EN			NO_OS_BIT(0)
#define HMC7044_START_UP_MODE_DYN_EN	(NO_OS_BIT(3) | NO_OS_BIT(2))

#define HMC7044_REG_CH_OUT_CRTL_1(ch)	(0x00C9 + 0xA * (ch))
#define HMC7044_DIV_LSB(x)		((x) & 0xFF)

#define HMC7044_REG_CH_OUT_CRTL_2(ch)	(0x00CA + 0xA * (ch))
#define HMC7044_DIV_MSB(x)		(((x) >> 8) & 0xFF)

#define HMC7044_REG_CH_OUT_CRTL_3(ch)	(0x00CB + 0xA * (ch))
#define HMC7044_REG_CH_OUT_CRTL_4(ch)	(0x00CC + 0xA * (ch))
#define HMC7044_REG_CH_OUT_CRTL_5(ch)	(0x00CD + 0xA * (ch))
#define HMC7044_REG_CH_OUT_CRTL_6(ch)	(0x00CE + 0xA * (ch))
#define HMC7044_REG_CH_OUT_CRTL_7(ch)	(0x00CF + 0xA * (ch))

#define HMC7044_REG_CH_OUT_CRTL_8(ch)	(0x00D0 + 0xA * (ch))
#define HMC7044_DRIVER_MODE(x)		(((x) & 0x3) << 3)
#define HMC7044_DRIVER_Z_MODE(x)	(((x) & 0x3) << 0)
#define HMC7044_DYN_DRIVER_EN		NO_OS_BIT(5)
#define HMC7044_FORCE_MUTE_EN		NO_OS_BIT(7)

#define HMC7044_NUM_CHAN	14

#define HMC7044_LOW_VCO_MIN	2150000
#define HMC7044_LOW_VCO_MAX	2880000
#define HMC7044_HIGH_VCO_MIN	2650000
#define HMC7044_HIGH_VCO_MAX	3200000

#define HMC7044_RECOMM_LCM_MIN	30000
#define HMC7044_RECOMM_LCM_MAX	70000
#define HMC7044_RECOMM_PFD1	10000
#define HMC7044_MIN_PFD1	1
#define HMC7044_MAX_PFD1	50000

#define HMC7044_CP_CURRENT_STEP	120
#define HMC7044_CP_CURRENT_MIN	120
#define HMC7044_CP_CURRENT_MAX	1920
#define HMC7044_CP_CURRENT_DEF	1080

#define HMC7044_R1_MAX		65535
#define HMC7044_N1_MAX		65535

#define HMC7044_R2_MIN		1
#define HMC7044_R2_MAX		4095
#define HMC7044_N2_MIN		8
#define HMC7044_N2_MAX		65535

#define HMC7044_OUT_DIV_MIN	1
#define HMC7044_OUT_DIV_MAX	4094


static const char* const pll1_fsm_states[] = {
	"Reset",
	"Acquisition",
	"Locked",
	"Invalid",
	"Holdover",
	"DAC assisted holdover exit",
	"Invalid",
};

struct hmc7044_jesd204_priv {
	struct hmc7044_dev *hmc;
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * SPI register write to device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int hmc7044_write(struct hmc7044_dev *dev,
			 uint16_t reg,
			 uint8_t val)
{
	uint8_t buf[3];
	uint16_t cmd;

	cmd = HMC7044_WRITE | HMC7044_CNT(1) | HMC7044_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = val;

	return no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
}

/**
 * SPI register read from device.
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param val - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */

int32_t hmc7044_read(struct hmc7044_dev *dev, uint16_t reg, uint8_t *val)
{
	uint8_t buf[3];
	uint16_t cmd;
	int ret;

	cmd = HMC7044_READ | HMC7044_CNT(1) | HMC7044_ADDR(reg);
	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;
	buf[2] = 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*val = buf[2];

	return 0;
}

static void hmc7044_read_write_check(struct hmc7044_dev *dev)
{
	uint8_t val;

	hmc7044_write(dev, HMC7044_REG_SCRATCHPAD, 0xAD);
	hmc7044_read(dev, HMC7044_REG_SCRATCHPAD, &val);

	dev->read_write_confirmed = (val == 0xAD);

	if (!dev->read_write_confirmed)
		pr_warning("Read/Write check failed (0x%X)\n", val);
}

static int hmc7044_toggle_bit(struct hmc7044_dev *dev,
			      unsigned int reg,
			      unsigned int mask,
			      unsigned int us_delay)
{
	uint8_t val;
	int ret;

	if (dev->read_write_confirmed) {
		ret = hmc7044_read(dev, reg, &val);
		if (ret < 0)
			return ret;
	} else {
		val = 0;
	}

	ret = hmc7044_write(dev, reg, val | mask);
	if (ret < 0)
		return ret;

	val &= ~mask;

	ret = hmc7044_write(dev, reg, val);
	if (ret < 0)
		return ret;

	if (us_delay)
		no_os_udelay(us_delay);

	return 0;
}

/**
 * Calculate the output channel divider.
 * @param rate - The desired rate.
 * @param parent_rate - The parent rate.
 * @return The output divider.
 */
uint32_t hmc7044_calc_out_div(uint32_t rate,
			      uint32_t parent_rate)
{
	uint32_t div;

	div = NO_OS_DIV_ROUND_CLOSEST(parent_rate, rate);

	/* Supported odd divide ratios are 1, 3, and 5 */
	if ((div != 1) && (div != 3) && (div != 5) && (div % 2))
		div = NO_OS_DIV_ROUND_CLOSEST(parent_rate, rate * 2) * 2;

	div = no_os_clamp_t(unsigned int,
			    div,
			    HMC7044_OUT_DIV_MIN,
			    HMC7044_OUT_DIV_MAX);

	return div;
}

/**
 * Recalculate rate corresponding to a channel.
 * @param dev - The device structure.
 * @param chan_num - Channel number.
 * @param rate - Channel rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_clk_recalc_rate(struct hmc7044_dev *dev, uint32_t chan_num,
				uint64_t *rate)
{
	int i;
	struct hmc7044_chan_spec *chan = NULL;

	/* Find the reqested channel number */
	for (i = 0; i < dev->num_channels; i++) {
		if (dev->channels[i].num == chan_num) {
			chan = &dev->channels[i];
			break;
		}
	}
	if (chan == NULL )
		return -1;

	*rate = dev->pll2_freq / chan->divider;

	return 0;
}

/**
 * Calculate closest possible rate
 * @param dev - The device structure
 * @param rate - The desired rate.
 * @param rounded_rate - The closest possible rate of desired rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_clk_round_rate(struct hmc7044_dev *dev, uint32_t rate,
			       uint64_t *rounded_rate)
{
	uint32_t div = hmc7044_calc_out_div(rate, dev->pll2_freq);

	*rounded_rate = NO_OS_DIV_ROUND_CLOSEST(dev->pll2_freq, div);

	return 0;
}

/**
 * Set channel rate.
 * @param dev - The device structure.
 * @param chan_num - Channel number.
 * @param rate - Channel rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_clk_set_rate(struct hmc7044_dev *dev, uint32_t chan_num,
			     uint64_t rate)
{
	uint32_t div;
	int32_t ret;
	int i;
	struct hmc7044_chan_spec *chan = NULL;

	/* Find the reqested channel number */
	for (i = 0; i < dev->num_channels; i++) {
		if (dev->channels[i].num == chan_num) {
			chan = &dev->channels[i];
			break;
		}
	}
	if (chan == NULL )
		return -1;

	div = hmc7044_calc_out_div(rate, dev->pll2_freq);
	chan->divider = div;

	ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_1(chan->num),
			    HMC7044_DIV_LSB(div));
	if(ret < 0)
		return ret;

	return hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_2(chan->num),
			     HMC7044_DIV_MSB(div));
}

static int hmc7044_info(struct hmc7044_dev *dev)
{
	uint32_t clkin_freq, active;
	uint8_t alarm_stat = 0, pll1_stat = 0;
	int ret;

	if (!dev->read_write_confirmed) {
		pr_info("Probed, SPI read support failed\n");
		return 0;
	}

	if (!dev->is_hmc7043 && !dev->clkin1_vcoin_en) {
		ret = hmc7044_read(dev,
				   HMC7044_REG_PLL1_STATUS, &pll1_stat);
		if (ret < 0)
			return ret;

		if (HMC7044_PLL1_FSM_STATE(pll1_stat) != 2) { /* Lock */
			no_os_mdelay(NO_OS_DIV_ROUND_UP(5000, dev->pll1_loop_bw));
			ret = hmc7044_read(dev,
					   HMC7044_REG_PLL1_STATUS, &pll1_stat);
			if (ret < 0)
				return ret;
		}

		ret = hmc7044_read(dev,
				   HMC7044_REG_ALARM_READBACK, &alarm_stat);
		if (ret < 0)
			return ret;

		active = HMC7044_PLL1_ACTIVE_CLKIN(pll1_stat);
	} else {
		active = 1;
	}

	if (dev->is_hmc7043)
		active = 0;

	if (dev->clkin_freq_ccf[active])
		clkin_freq = dev->clkin_freq_ccf[active];
	else
		clkin_freq = dev->clkin_freq[active];

	if (!dev->is_hmc7043 && !dev->clkin1_vcoin_en)
		pr_info("PLL1: %s, CLKIN%u @ %u Hz, PFD: %u kHz - PLL2: %s @ %u.%06u MHz\n",
			pll1_fsm_states[HMC7044_PLL1_FSM_STATE(pll1_stat)],
			active, clkin_freq, dev->pll1_pfd,
			HMC7044_PLL2_LOCK_DETECT(alarm_stat) ?
			"Locked" : "Unlocked", dev->pll2_freq / 1000000,
			dev->pll2_freq % 1000000);
	else
		pr_info("CLKIN%u @ %u.%06u MHz\n", active,
			clkin_freq / 1000000, clkin_freq % 1000000);

	return 0;
}

/**
 * Setup the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t hmc7044_setup(struct hmc7044_dev *dev)
{
	struct hmc7044_chan_spec *chan;
	bool high_vco_en;
	bool pll2_freq_doubler_en;
	uint32_t vcxo_freq, pll2_freq;
	uint32_t clkin_freq[4];
	uint32_t lcm_freq;
	uint32_t in_prescaler[5];
	uint32_t pll1_lock_detect;
	uint32_t n1, r1;
	uint32_t n, r;
	uint32_t pfd1_freq;
	uint32_t vco_limit;
	uint32_t n2[2], r2[2];
	uint32_t i, c, ref_en = 0;
	int ret;

	vcxo_freq = dev->vcxo_freq / 1000;
	pll2_freq = dev->pll2_freq / 1000;

	lcm_freq = vcxo_freq;
	for (i = 0; i < NO_OS_ARRAY_SIZE(clkin_freq); i++) {
		if (dev->clkin_freq_ccf[i])
			clkin_freq[i] = dev->clkin_freq_ccf[i] / 1000;
		else
			clkin_freq[i] = dev->clkin_freq[i] / 1000;

		if (clkin_freq[i]) {
			lcm_freq = no_os_greatest_common_divisor(clkin_freq[i], lcm_freq);
			ref_en |= NO_OS_BIT(i);
		}
	}

	while (lcm_freq > HMC7044_RECOMM_LCM_MAX)
		lcm_freq /= 2;

	for (i = 0; i < NO_OS_ARRAY_SIZE(clkin_freq); i++) {
		if (clkin_freq[i])
			in_prescaler[i] = clkin_freq[i] / lcm_freq;
		else
			in_prescaler[i] = 1;
	}
	in_prescaler[4] = vcxo_freq / lcm_freq;

	pll1_lock_detect = no_os_log_base_2((lcm_freq * 4000) / dev->pll1_loop_bw);

	/* fVCXO / N1 = fLCM / R1 */
	no_os_rational_best_approximation(vcxo_freq, lcm_freq,
					  HMC7044_N1_MAX, HMC7044_R1_MAX,
					  &n1, &r1);

	pfd1_freq = vcxo_freq / n1;

	n = n1;
	r = r1;
	while (pfd1_freq > dev->pfd1_limit) {
		do {
			n++;
		} while (((vcxo_freq % n) || (lcm_freq * n % vcxo_freq)) &&
			 (n <= HMC7044_N1_MAX));
		r = lcm_freq * n / vcxo_freq;

		if ((n > HMC7044_N1_MAX) || (r > HMC7044_R1_MAX))
			break;

		n1 = n;
		r1 = r;
		pfd1_freq = vcxo_freq / n1;
	}

	dev->pll1_pfd = pfd1_freq;

	if (pll2_freq < HMC7044_LOW_VCO_MIN  ||
	    pll2_freq > HMC7044_HIGH_VCO_MAX)
		return -EINVAL;

	vco_limit = (HMC7044_LOW_VCO_MAX + HMC7044_HIGH_VCO_MIN) / 2;
	if (pll2_freq >= vco_limit)
		high_vco_en = true;
	else
		high_vco_en = false;

	/* fVCO / N2 = fVCXO * doubler / R2 */
	pll2_freq_doubler_en = true;
	no_os_rational_best_approximation(pll2_freq, vcxo_freq * 2,
					  HMC7044_N2_MAX, HMC7044_R2_MAX,
					  &n2[0], &r2[0]);

	if (pll2_freq != vcxo_freq * n2[0] / r2[0]) {
		no_os_rational_best_approximation(pll2_freq, vcxo_freq,
						  HMC7044_N2_MAX, HMC7044_R2_MAX,
						  &n2[1], &r2[1]);

		if (abs((int)pll2_freq - (int)(vcxo_freq * 2 * n2[0] / r2[0])) >
		    abs((int)pll2_freq - (int)(vcxo_freq * n2[1] / r2[1]))) {
			n2[0] = n2[1];
			r2[0] = r2[1];
			pll2_freq_doubler_en = false;
		}
	}

	while ((n2[0] < HMC7044_N2_MIN) && (r2[0] <= HMC7044_R2_MAX / 2)) {
		n2[0] *= 2;
		r2[0] *= 2;
	}
	if (n2[0] < HMC7044_N2_MIN)
		return -EINVAL;

	/* Resets all registers to default values */
	ret = hmc7044_toggle_bit(dev, HMC7044_REG_SOFT_RESET,
				 HMC7044_SOFT_RESET, 100);
	if (ret)
		return ret;

	hmc7044_read_write_check(dev);

	/* Disable all channels */
	for (i = 0; i < HMC7044_NUM_CHAN; i++) {
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_0(i), 0);
		if (ret)
			return ret;
	}

	/* Load the configuration updates (provided by Analog Devices) */
	ret = hmc7044_write(dev, HMC7044_REG_CLK_OUT_DRV_LOW_PW, 0x4d);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_CLK_OUT_DRV_HIGH_PW, 0xdf);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_DELAY, 0x06);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_HOLDOVER, 0x06);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_VTUNE_PRESET, 0x04);
	if (ret)
		return ret;


	ret = hmc7044_write(dev, HMC7044_REG_GLOB_MODE,
			    HMC7044_SYNC_PIN_MODE(dev->sync_pin_mode) |
			    (dev->clkin0_rfsync_en ? HMC7044_RFSYNC_EN : 0) |
			    (dev->clkin1_vcoin_en ? HMC7044_VCOIN_MODE_EN : 0) |
			    HMC7044_REF_PATH_EN(ref_en));
	if (ret)
		return ret;

	/* Program PLL2 */

	/* Select the VCO range */

	if (dev->clkin1_vcoin_en) {
		dev->pll2_freq = dev->clkin_freq_ccf[1] ?
				 dev->clkin_freq_ccf[1] : dev->clkin_freq[1];

		if (dev->pll2_freq < 1000000000U) {
			ret = hmc7044_write(dev, HMC7044_CLK_INPUT_CTRL,
					    HMC7044_LOW_FREQ_INPUT_MODE);
			if (ret)
				return ret;
		}

		ret = hmc7044_write(dev, HMC7044_REG_EN_CTRL_0,
				    (dev->rf_reseeder_en ? HMC7044_RF_RESEEDER_EN : 0) |
				    HMC7044_VCO_SEL(0) |
				    HMC7044_SYSREF_TIMER_EN);
		if (ret)
			return ret;

		ret = hmc7044_write(dev, HMC7044_REG_SYNC, HMC7044_SYNC_RETIME);
		if (ret)
			return ret;
	} else {
		ret = hmc7044_write(dev, HMC7044_REG_EN_CTRL_0,
				    (dev->rf_reseeder_en ? HMC7044_RF_RESEEDER_EN : 0) |
				    HMC7044_VCO_SEL(high_vco_en ?
						    HMC7044_VCO_HIGH :
						    HMC7044_VCO_LOW) |
				    HMC7044_SYSREF_TIMER_EN | HMC7044_PLL2_EN |
				    HMC7044_PLL1_EN);
		if (ret)
			return ret;
	}

	/* Program the dividers */
	ret = hmc7044_write(dev, HMC7044_REG_PLL2_R_LSB,
			    HMC7044_R2_LSB(r2[0]));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL2_R_MSB,
			    HMC7044_R2_MSB(r2[0]));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL2_N_LSB,
			    HMC7044_N2_LSB(n2[0]));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL2_N_MSB,
			    HMC7044_N2_MSB(n2[0]));
	if (ret)
		return ret;

	/* Program the reference doubler */
	ret = hmc7044_write(dev, HMC7044_REG_PLL2_FREQ_DOUBLER,
			    pll2_freq_doubler_en ? 0 : HMC7044_PLL2_FREQ_DOUBLER_DIS);
	if (ret)
		return ret;
	/* Program PLL1 */

	ret = hmc7044_write(dev, HMC7044_REG_PLL1_CP_CTRL,
			    HMC7044_PLL1_CP_CURRENT(dev->pll1_cp_current /
					    HMC7044_CP_CURRENT_STEP - 1));
	if (ret)
		return ret;
	/* Set the lock detect timer threshold */
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_LOCK_DETECT,
			    HMC7044_LOCK_DETECT_TIMER(pll1_lock_detect));

	/* Set the LCM */
	for (i = 0; i < NO_OS_ARRAY_SIZE(clkin_freq); i++) {
		ret = hmc7044_write(dev, HMC7044_REG_CLKIN_PRESCALER(i),
				    in_prescaler[i]);
		if (ret)
			return ret;
	}
	ret = hmc7044_write(dev, HMC7044_REG_OSCIN_PRESCALER,
			    in_prescaler[4]);
	if (ret)
		return ret;
	/* Program the dividers */
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_R_LSB,
			    HMC7044_R2_LSB(r1));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_R_MSB,
			    HMC7044_R2_MSB(r1));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_N_LSB,
			    HMC7044_N2_LSB(n1));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_N_MSB,
			    HMC7044_N2_MSB(n1));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_REF_PRIO_CTRL,
			    dev->pll1_ref_prio_ctrl);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_PLL1_REF_SWITCH,
			    HMC7044_HOLDOVER_DAC |
			    (dev->pll1_ref_autorevert_en ?
			     HMC7044_AUTO_REVERT_SWITCH : 0) |
			    HMC7044_AUTO_MODE_SWITCH);
	if (ret)
		return ret;
	/* Program the SYSREF timer */

	/* Set the divide ratio */
	ret = hmc7044_write(dev, HMC7044_REG_SYSREF_TIMER_LSB,
			    HMC7044_SYSREF_TIMER_LSB(dev->sysref_timer_div));
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_SYSREF_TIMER_MSB,
			    HMC7044_SYSREF_TIMER_MSB(dev->sysref_timer_div));
	if (ret)
		return ret;
	/* Set the pulse generator mode configuration */
	ret = hmc7044_write(dev, HMC7044_REG_PULSE_GEN,
			    HMC7044_PULSE_GEN_MODE(dev->pulse_gen_mode));
	if (ret)
		return ret;
	/* Enable the input buffers */
	ret = hmc7044_write(dev, HMC7044_REG_CLKIN0_BUF_CTRL,
			    dev->in_buf_mode[0]);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_CLKIN1_BUF_CTRL,
			    dev->in_buf_mode[1]);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_CLKIN2_BUF_CTRL,
			    dev->in_buf_mode[2]);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_CLKIN3_BUF_CTRL,
			    dev->in_buf_mode[3]);
	if (ret)
		return ret;
	ret = hmc7044_write(dev, HMC7044_REG_OSCIN_BUF_CTRL,
			    dev->in_buf_mode[4]);
	if (ret)
		return ret;
	/* Set GPIOs */
	for (i = 0; i < NO_OS_ARRAY_SIZE(dev->gpi_ctrl); i++) {
		ret = hmc7044_write(dev, HMC7044_REG_GPI_CTRL(i),
				    dev->gpi_ctrl[i]);
		if (ret)
			return ret;
	}

	for (i = 0; i < NO_OS_ARRAY_SIZE(dev->gpo_ctrl); i++) {
		ret = hmc7044_write(dev, HMC7044_REG_GPO_CTRL(i),
				    dev->gpo_ctrl[i]);
		if (ret)
			return ret;
	}

	no_os_mdelay(10);

	/* Program the output channels */
	for (i = 0; i < dev->num_channels; i++) {
		chan = &dev->channels[i];

		if (chan->num >= HMC7044_NUM_CHAN || chan->disable)
			continue;

		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_1(chan->num),
				    HMC7044_DIV_LSB(chan->divider));
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_2(chan->num),
				    HMC7044_DIV_MSB(chan->divider));
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_8(chan->num),
				    HMC7044_DRIVER_MODE(chan->driver_mode) |
				    HMC7044_DRIVER_Z_MODE(chan->driver_impedance) |
				    (chan->dynamic_driver_enable ?
				     HMC7044_DYN_DRIVER_EN : 0) |
				    (chan->force_mute_enable ?
				     HMC7044_FORCE_MUTE_EN : 0));
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_3(chan->num),
				    chan->fine_delay & 0x1F);
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_4(chan->num),
				    chan->coarse_delay & 0x1F);
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_7(chan->num),
				    chan->out_mux_mode & 0x3);
		if (ret)
			return ret;
		ret = hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_0(chan->num),
				    (chan->start_up_mode_dynamic_enable ?
				     HMC7044_START_UP_MODE_DYN_EN : 0) | NO_OS_BIT(4) |
				    (chan->high_performance_mode_dis ?
				     0 : HMC7044_HI_PERF_MODE) | HMC7044_SYNC_EN |
				    HMC7044_CH_EN);
		if (ret)
			return ret;
	}
	no_os_mdelay(10);

	/* Do a restart to reset the system and initiate calibration */
	ret = hmc7044_toggle_bit(dev, HMC7044_REG_REQ_MODE_0,
				 HMC7044_RESTART_DIV_FSM, 10000);
	if (ret)
		return ret;

	ret = hmc7044_toggle_bit(dev, HMC7044_REG_REQ_MODE_0,
				 HMC7044_RESEED_REQ, 1000);
	if (ret)
		return ret;

	ret = hmc7044_write(dev, HMC7044_REG_REQ_MODE_0,
			    (dev->high_performance_mode_clock_dist_en ?
			     HMC7044_HIGH_PERF_DISTRIB_PATH : 0));
	if (ret)
		return ret;

	if (!dev->clkin1_vcoin_en) {
		uint8_t pll1_stat;

		ret = hmc7044_read(dev, HMC7044_REG_PLL1_STATUS, &pll1_stat);
		if (ret < 0)
			return ret;

		c = HMC7044_PLL1_ACTIVE_CLKIN(pll1_stat);
	} else {
		c = 1; /* CLKIN1 */
	}

	for (i = 0; i < dev->num_channels; i++) {
		chan = &dev->channels[i];

		if (chan->num >= HMC7044_NUM_CHAN || chan->disable)
			continue;
	}

	ret = hmc7044_info(dev);
	if (ret)
		return ret;

	return 0;
}

/**
 * Setup the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t hmc7043_setup(struct hmc7044_dev *dev)
{
	struct hmc7044_chan_spec *chan;
	uint32_t i;

	if (dev->clkin_freq_ccf[0])
		dev->pll2_freq = dev->clkin_freq_ccf[0];
	else
		dev->pll2_freq  = dev->clkin_freq[0];

	if (!dev->pll2_freq) {
		printf("%s: Failed to get valid parent rate\n", __func__);
		return -1;
	}

	/* Resets all registers to default values */
	hmc7044_write(dev, HMC7044_REG_SOFT_RESET, HMC7044_SOFT_RESET);
	no_os_mdelay(10);
	hmc7044_write(dev, HMC7044_REG_SOFT_RESET, 0);
	no_os_mdelay(10);

	hmc7044_read_write_check(dev);

	/* Load the configuration updates (provided by Analog Devices) */
	hmc7044_write(dev, HMC7044_REG_CLK_OUT_DRV_LOW_PW, 0x4d);
	hmc7044_write(dev, HMC7044_REG_CLK_OUT_DRV_HIGH_PW, 0xdf);

	/* Disable all channels */
	for (i = 0; i < HMC7044_NUM_CHAN; i++)
		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_0(i), 0);

	if (dev->pll2_freq < 1000000000U)
		hmc7044_write(dev, HMC7044_CLK_INPUT_CTRL,
			      HMC7044_LOW_FREQ_INPUT_MODE);

	hmc7044_write(dev, HMC7044_REG_EN_CTRL_0,
		      (dev->rf_reseeder_en ? HMC7044_RF_RESEEDER_EN : 0) |
		      HMC7044_SYSREF_TIMER_EN);

	/* Program the SYSREF timer */

	/* Set the divide ratio */
	hmc7044_write(dev, HMC7044_REG_SYSREF_TIMER_LSB,
		      HMC7044_SYSREF_TIMER_LSB(dev->sysref_timer_div));
	hmc7044_write(dev, HMC7044_REG_SYSREF_TIMER_MSB,
		      HMC7044_SYSREF_TIMER_MSB(dev->sysref_timer_div));

	/* Set the pulse generator mode configuration */
	hmc7044_write(dev, HMC7044_REG_PULSE_GEN,
		      HMC7044_PULSE_GEN_MODE(dev->pulse_gen_mode));

	/* Enable the input buffers */
	hmc7044_write(dev, HMC7044_REG_CLKIN0_BUF_CTRL,
		      dev->in_buf_mode[0]);
	hmc7044_write(dev, HMC7044_REG_CLKIN1_BUF_CTRL,
		      dev->in_buf_mode[1]);

	/* Set GPIOs */
	hmc7044_write(dev, HMC7044_REG_GPI_CTRL(0),
		      dev->gpi_ctrl[0]);

	hmc7044_write(dev, HMC7044_REG_GPO_CTRL(0),
		      dev->gpo_ctrl[0]);

	/* Program the output channels */
	for (i = 0; i < dev->num_channels; i++) {
		chan = &dev->channels[i];

		if (chan->num >= HMC7044_NUM_CHAN || chan->disable)
			continue;

		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_1(chan->num),
			      HMC7044_DIV_LSB(chan->divider));
		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_2(chan->num),
			      HMC7044_DIV_MSB(chan->divider));
		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_8(chan->num),
			      HMC7044_DRIVER_MODE(chan->driver_mode) |
			      HMC7044_DRIVER_Z_MODE(chan->driver_impedance) |
			      (chan->dynamic_driver_enable ?
			       HMC7044_DYN_DRIVER_EN : 0) |
			      (chan->force_mute_enable ?
			       HMC7044_FORCE_MUTE_EN : 0));

		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_3(chan->num),
			      chan->fine_delay & 0x1F);
		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_4(chan->num),
			      chan->coarse_delay & 0x1F);
		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_7(chan->num),
			      chan->out_mux_mode & 0x3);

		hmc7044_write(dev, HMC7044_REG_CH_OUT_CRTL_0(chan->num),
			      (chan->start_up_mode_dynamic_enable ?
			       HMC7044_START_UP_MODE_DYN_EN : 0) |
			      (chan->output_control0_rb4_enable ? NO_OS_BIT(4) : 0) |
			      (chan->high_performance_mode_dis ?
			       0 : HMC7044_HI_PERF_MODE) | HMC7044_SYNC_EN |
			      HMC7044_CH_EN);
	}
	no_os_mdelay(10);


	/* Do a restart to reset the system and initiate calibration */
	hmc7044_write(dev, HMC7044_REG_REQ_MODE_0,
		      HMC7044_RESTART_DIV_FSM);
	no_os_mdelay(1);
	hmc7044_write(dev, HMC7044_REG_REQ_MODE_0,
		      (dev->high_performance_mode_clock_dist_en ?
		       HMC7044_HIGH_PERF_DISTRIB_PATH : 0));
	no_os_mdelay(1);

	return 0;
}

static int hmc7044_jesd204_sysref(struct jesd204_dev *jdev)
{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;
	int ret;

	pr_debug("%s:%d\n", __func__, __LINE__);

	ret = hmc7044_toggle_bit(hmc, HMC7044_REG_REQ_MODE_0,
				 HMC7044_PULSE_GEN_REQ, 0);

	return ret;
}

static int hmc7044_lmfc_lemc_validate(struct hmc7044_dev *hmc,
				      uint64_t dividend, uint32_t divisor)
{
	uint32_t rem, rem_l, rem_u, gcd_val, min;

	gcd_val = no_os_greatest_common_divisor(dividend, divisor);
	min = NO_OS_DIV_ROUND_CLOSEST(hmc->pll2_freq, HMC7044_OUT_DIV_MAX);

	if (gcd_val >= min) {
		pr_debug("%s: dividend=%llu divisor=%u GCD=%u (hmc->pll2_freq=%u, min=%u)",
			 __func__, dividend, divisor, gcd_val, hmc->pll2_freq, min);

		hmc->jdev_lmfc_lemc_gcd = gcd_val;
		return 0;
	}

	no_os_div_u64_rem(hmc->pll2_freq, divisor, &rem);

	pr_debug("%s: dividend=%llu divisor=%u GCD=%u rem=%u (hmc->pll2_freq=%u)",
		 __func__, dividend, divisor, gcd_val, rem, hmc->pll2_freq);

	no_os_div_u64_rem(dividend, divisor, &rem);
	no_os_div_u64_rem(dividend, divisor - 1, &rem_l);
	no_os_div_u64_rem(dividend, divisor + 1, &rem_u);

	if ((rem_l > rem) && (rem_u > rem)) {
		if (hmc->jdev_lmfc_lemc_gcd)
			hmc->jdev_lmfc_lemc_gcd = no_os_min(hmc->jdev_lmfc_lemc_gcd, divisor);
		else
			hmc->jdev_lmfc_lemc_gcd = divisor;
		return 0;
	}

	return -EINVAL;
}

static int hmc7044_jesd204_link_supported(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;
	int ret;
	unsigned long rate;

	if (reason != JESD204_STATE_OP_REASON_INIT) {
		hmc->jdev_lmfc_lemc_rate = 0;
		hmc->jdev_lmfc_lemc_gcd = 0;

		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	if (hmc->jdev_skip_sysref_freq_calc) {
		if (!hmc->jdev_desired_sysref_freq) {
			pr_err("%s:%d Error: requires adi,jesd204-desired-sysref-frequency-hz set\n",
			       __func__, __LINE__);
			return -EINVAL;
		}

		rate = hmc->jdev_desired_sysref_freq;
	} else {
		ret = jesd204_link_get_lmfc_lemc_rate(lnk, &rate);
		if (ret < 0)
			return ret;
	}

	if (hmc->jdev_lmfc_lemc_rate) {
		hmc->jdev_lmfc_lemc_rate = no_os_min(hmc->jdev_lmfc_lemc_rate, (uint32_t)rate);
		ret = hmc7044_lmfc_lemc_validate(hmc, hmc->jdev_lmfc_lemc_gcd, (uint32_t)rate);
	} else {
		hmc->jdev_lmfc_lemc_rate = rate;
		ret = hmc7044_lmfc_lemc_validate(hmc, hmc->pll2_freq, (uint32_t)rate);
	}

	pr_debug("%s:%d link_num %u LMFC/LEMC %u/%lu gcd %u\n",
		 __func__, __LINE__, lnk->link_id, hmc->jdev_lmfc_lemc_rate,
		 rate, hmc->jdev_lmfc_lemc_gcd);
	if (ret)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static int hmc7044_sync_pin_set(struct hmc7044_dev *hmc, unsigned mode)
{
	uint8_t val;
	int ret;

	if (hmc->read_write_confirmed) {
		ret = hmc7044_read(hmc, HMC7044_REG_GLOB_MODE, &val);
		if (ret < 0)
			return ret;
	} else {
		val = (hmc->clkin0_rfsync_en ? HMC7044_RFSYNC_EN : 0) |
		      (hmc->clkin1_vcoin_en ? HMC7044_VCOIN_MODE_EN : 0) |
		      HMC7044_REF_PATH_EN(0xF);
	}

	val &= ~HMC7044_SYNC_PIN_MODE(~0);
	val |= HMC7044_SYNC_PIN_MODE(mode);

	return hmc7044_write(hmc, HMC7044_REG_GLOB_MODE, val);
}

static int hmc7044_continuous_chan_sync_enable(struct hmc7044_dev *hmc,
		bool enable)
{
	struct hmc7044_chan_spec *chan;
	int ret, i;

	for (i = 0; i < hmc->num_channels; i++) {
		chan = &hmc->channels[i];

		if (chan->num >= HMC7044_NUM_CHAN || chan->disable)
			continue;

		ret = hmc7044_write(hmc, HMC7044_REG_CH_OUT_CRTL_0(chan->num),
				    (chan->start_up_mode_dynamic_enable ?
				     HMC7044_START_UP_MODE_DYN_EN : 0) | NO_OS_BIT(4) |
				    (chan->high_performance_mode_dis ?
				     0 : HMC7044_HI_PERF_MODE) |
				    ((enable || chan->start_up_mode_dynamic_enable) ?
				     HMC7044_SYNC_EN : 0) | HMC7044_CH_EN);
		if (ret < 0)
			return ret;
	}

	return 0;
}


static int hmc7044_jesd204_clks_sync1(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason)
{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;
	bool cont_mode = false;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	if (!hmc->hmc_two_level_tree_sync_en)
		goto reseed;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (hmc->is_sysref_provider) {
		if (!hmc->is_hmc7043) {
			ret = hmc7044_sync_pin_set(hmc, HMC7044_SYNC_PIN_DISABLED);
			if (ret)
				return ret;
		}
	} else {
		if (!hmc->is_hmc7043 && !hmc->clkin0_rfsync_en && !hmc->clkin1_vcoin_en) {
			ret = hmc7044_sync_pin_set(hmc, HMC7044_SYNC_PIN_SYNC);
			if (ret)
				return ret;
		} else {
			ret = hmc7044_continuous_chan_sync_enable(hmc, 1);
			if (ret)
				return ret;
		}
	}

	ret = hmc7044_toggle_bit(hmc, HMC7044_REG_REQ_MODE_0,
				 HMC7044_RESTART_DIV_FSM, (!hmc->is_hmc7043 &&
						 !hmc->clkin1_vcoin_en) ? 10000 : 1000);
	if (ret)
		return ret;

reseed:
	if (hmc->pulse_gen_mode == HMC7044_PULSE_GEN_CONT_PULSE) {
		cont_mode = true;

		hmc7044_write(hmc, HMC7044_REG_PULSE_GEN,
			      HMC7044_PULSE_GEN_MODE(HMC7044_PULSE_GEN_LEVEL_SENSITIVE));
		no_os_mdelay(10);
	}

	ret = hmc7044_toggle_bit(hmc, HMC7044_REG_REQ_MODE_0,
				 HMC7044_RESEED_REQ, 1000);
	if (ret)
		return ret;

	if (cont_mode)
		hmc7044_write(hmc, HMC7044_REG_PULSE_GEN,
			      HMC7044_PULSE_GEN_MODE(HMC7044_PULSE_GEN_CONT_PULSE));


	return JESD204_STATE_CHANGE_DONE;
}

static int hmc7044_jesd204_clks_sync2(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason)

{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;

	if (!hmc->hmc_two_level_tree_sync_en)
		return JESD204_STATE_CHANGE_DONE;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (hmc->is_sysref_provider) {
		int ret = hmc7044_jesd204_sysref(jdev);
		if (ret)
			return ret;
		no_os_mdelay(2);
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int hmc7044_jesd204_clks_sync3(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason)
{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;
	uint8_t val;
	int ret;

	if (!hmc->hmc_two_level_tree_sync_en)
		return JESD204_STATE_CHANGE_DONE;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d reason %s\n", __func__, __LINE__,
		 jesd204_state_op_reason_str(reason));

	if (hmc->is_sysref_provider)
		return JESD204_STATE_CHANGE_DONE;

	if (hmc->read_write_confirmed) {
		int ret;

		ret = hmc7044_read(hmc, HMC7044_REG_ALARM_READBACK, &val);
		if (ret < 0)
			return ret;

		if (!HMC7044_CLK_OUT_PH_STATUS(val))
			pr_warning(
				"%s: SYSREF of the HMC7044 is not valid; that is, its phase output is not stable (0x%X)\n",
				__func__, val & 0xFF);
	}

	if (!hmc->is_hmc7043 && !hmc->clkin0_rfsync_en && !hmc->clkin1_vcoin_en) {
		ret = hmc7044_sync_pin_set(hmc, HMC7044_SYNC_PIN_PULSE_GEN_REQ);
		if (ret)
			return ret;
	} else {
		ret = hmc7044_continuous_chan_sync_enable(hmc, 0);
		if (ret)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int hmc7044_jesd204_link_pre_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct hmc7044_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct hmc7044_dev *hmc = priv->hmc;
	int i, ret;
	uint32_t sysref_timer;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u\n", __func__, __LINE__, lnk->link_id);

	if (hmc->jdev_desired_sysref_freq && (hmc->jdev_lmfc_lemc_gcd %
					      hmc->jdev_desired_sysref_freq == 0)) {
		hmc->jdev_lmfc_lemc_gcd = hmc->jdev_desired_sysref_freq;
	} else {
		while ((hmc->jdev_lmfc_lemc_gcd > hmc->jdev_max_sysref_freq) &&
		       (hmc->jdev_lmfc_lemc_gcd %
			(hmc->jdev_lmfc_lemc_gcd >> 1) == 0))
			hmc->jdev_lmfc_lemc_gcd >>= 1;
	}
	/* Program the output channels */
	for (i = 0; i < hmc->num_channels; i++) {
		if (hmc->channels[i].start_up_mode_dynamic_enable
		    || hmc->channels[i].is_sysref) {
			uint64_t rate;

			pr_debug("%s:%d Found SYSREF channel%u setting f=%u Hz\n",
				 __func__, __LINE__, hmc->channels[i].num, hmc->jdev_lmfc_lemc_gcd);

			hmc7044_clk_round_rate(hmc, hmc->jdev_lmfc_lemc_gcd, &rate);

			if (rate == (long)hmc->jdev_lmfc_lemc_gcd)
				ret = hmc7044_clk_set_rate(hmc, hmc->channels[i].num, hmc->jdev_lmfc_lemc_gcd);
			else
				ret = -EINVAL;

			if (ret < 0)
				pr_err("%s: Link%u setting SYSREF rate %u failed (%d)\n",
				       __func__, lnk->link_id, hmc->jdev_lmfc_lemc_gcd, ret);

		}
	}

	/* Program the SYSREF timer
	 * Set the 12-bit timer to a submultiple of the lowest
	 * output SYSREF frequency, and program it to be no faster than 4 MHz.
	 */

	sysref_timer = hmc->jdev_lmfc_lemc_gcd / 2;

	while (sysref_timer >= 4000000U)
		sysref_timer >>= 1;

	sysref_timer = hmc->pll2_freq / sysref_timer;

	/* Set the divide ratio */
	ret = hmc7044_write(hmc, HMC7044_REG_SYSREF_TIMER_LSB,
			    HMC7044_SYSREF_TIMER_LSB(sysref_timer));
	if (ret)
		return ret;
	ret = hmc7044_write(hmc, HMC7044_REG_SYSREF_TIMER_MSB,
			    HMC7044_SYSREF_TIMER_MSB(sysref_timer));
	if (ret)
		return ret;

	if (lnk->sysref.mode == JESD204_SYSREF_CONTINUOUS) {
		/* Set the pulse generator mode configuration */
		if (hmc->pulse_gen_mode != HMC7044_PULSE_GEN_CONT_PULSE)
			pr_warning("%s: Link%u forcing continuous SYSREF mode\n",
				   __func__, lnk->link_id);

		hmc->pulse_gen_mode = HMC7044_PULSE_GEN_CONT_PULSE;

		ret = hmc7044_write(hmc, HMC7044_REG_PULSE_GEN,
				    HMC7044_PULSE_GEN_MODE(HMC7044_PULSE_GEN_CONT_PULSE));
		if (ret)
			return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_hmc7044_init = {
	.sysref_cb = hmc7044_jesd204_sysref,
	.state_ops = {
		[JESD204_OP_LINK_SUPPORTED] = {
			.per_link = hmc7044_jesd204_link_supported,
		},
		[JESD204_OP_CLK_SYNC_STAGE1] = {
			.per_device = hmc7044_jesd204_clks_sync1,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_CLK_SYNC_STAGE2] = {
			.per_device = hmc7044_jesd204_clks_sync2,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_CLK_SYNC_STAGE3] = {
			.per_device = hmc7044_jesd204_clks_sync3,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_link = hmc7044_jesd204_link_pre_setup,
		},
	},
};


/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_init(struct hmc7044_dev **device,
		     const struct hmc7044_init_param *init_param)
{
	struct hmc7044_jesd204_priv *priv;
	struct hmc7044_dev *dev;
	int32_t ret;
	unsigned int i;
	struct no_os_clk_desc **clocks = NULL;
	struct no_os_clk_init_param clk_init;
	const char *names[HMC7044_NUM_CHAN] = {
		"clock_0", "clock_1", "clock_2", "clock_3", "clock_4",
		"clock_5", "clock_6", "clock_7", "clock_8", "clock_9",
		"clock_10", "clock_11", "clock_12", "clock_13"
	};

	dev = (struct hmc7044_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		return ret;

	if (init_param->export_no_os_clk) {
		clocks = no_os_calloc(HMC7044_NUM_CHAN, sizeof(struct no_os_clk_desc *));
		if (!clocks)
			return -1;
		for (i = 0; i < HMC7044_NUM_CHAN; i++) {
			clocks[i] = no_os_calloc(1, sizeof(struct no_os_clk_desc));
			if (!clocks[i])
				return -1;
			/* Initialize clk component */
			clk_init.name = names[i];
			clk_init.hw_ch_num = i;
			clk_init.platform_ops = &hmc7044_clk_ops;
			clk_init.dev_desc = dev;

			ret = no_os_clk_init(&clocks[i], &clk_init);
			if (ret)
				return ret;
		}
	}

	dev->clk_desc = clocks;

	dev->is_hmc7043 = init_param->is_hmc7043;

	dev->clkin_freq[0] = init_param->clkin_freq[0];
	dev->clkin_freq[1] = init_param->clkin_freq[1];
	dev->clkin_freq[2] = init_param->clkin_freq[2];
	dev->clkin_freq[3] = init_param->clkin_freq[3];

	dev->clkin_freq_ccf[0] = init_param->clkin_freq_ccf[0];
	dev->clkin_freq_ccf[1] = init_param->clkin_freq_ccf[1];
	dev->clkin_freq_ccf[2] = init_param->clkin_freq_ccf[2];
	dev->clkin_freq_ccf[3] = init_param->clkin_freq_ccf[3];

	dev->vcxo_freq = init_param->vcxo_freq;
	dev->pll1_pfd = init_param->pll1_pfd;
	dev->pfd1_limit = init_param->pfd1_limit;
	if (dev->pfd1_limit) {
		dev->pfd1_limit /= 1000;
		dev->pfd1_limit = no_os_clamp_t(uint32_t, dev->pfd1_limit,
						HMC7044_MIN_PFD1, HMC7044_MAX_PFD1);
	} else {
		dev->pfd1_limit = HMC7044_RECOMM_PFD1;
	}
	dev->pll1_cp_current = init_param->pll1_cp_current;
	if (dev->pll1_cp_current)
		dev->pll1_cp_current = no_os_clamp_t(uint32_t, dev->pll1_cp_current,
						     HMC7044_CP_CURRENT_MIN, HMC7044_CP_CURRENT_MAX);
	else
		dev->pll1_cp_current = HMC7044_CP_CURRENT_DEF;
	dev->pll2_freq = init_param->pll2_freq;
	dev->pll1_loop_bw = init_param->pll1_loop_bw;

	dev->sysref_timer_div = init_param->sysref_timer_div;
	dev->pll1_ref_prio_ctrl = init_param->pll1_ref_prio_ctrl;
	dev->pll1_ref_autorevert_en = init_param->pll1_ref_autorevert_en;
	dev->clkin0_rfsync_en = init_param->clkin0_rfsync_en;
	dev->clkin1_vcoin_en = init_param->clkin1_vcoin_en;
	dev->high_performance_mode_clock_dist_en =
		init_param->high_performance_mode_clock_dist_en;
	dev->rf_reseeder_en = !init_param->rf_reseeder_disable;
	dev->sync_pin_mode = init_param->sync_pin_mode;
	dev->pulse_gen_mode = init_param->pulse_gen_mode;

	dev->is_sysref_provider = init_param->jesd204_sysref_provider;
	dev->hmc_two_level_tree_sync_en =
		init_param->hmc_two_level_tree_sync_en;
	dev->jdev_max_sysref_freq =
		init_param->jesd204_max_sysref_frequency_hz;
	if (!dev->jdev_max_sysref_freq)
		dev->jdev_max_sysref_freq = INT_MAX;
	dev->jdev_desired_sysref_freq =
		init_param->jesd204_desired_sysref_frequency_hz;
	dev->jdev_skip_sysref_freq_calc = init_param->jdev_skip_sysref_freq_calc;


	dev->in_buf_mode[0] = init_param->in_buf_mode[0];
	dev->in_buf_mode[1] = init_param->in_buf_mode[1];
	dev->in_buf_mode[2] = init_param->in_buf_mode[2];
	dev->in_buf_mode[3] = init_param->in_buf_mode[3];
	dev->in_buf_mode[4] = init_param->in_buf_mode[4];

	dev->gpi_ctrl[0] = init_param->gpi_ctrl[0];
	dev->gpi_ctrl[1] = init_param->gpi_ctrl[1];
	dev->gpi_ctrl[2] = init_param->gpi_ctrl[2];
	dev->gpi_ctrl[3] = init_param->gpi_ctrl[3];

	dev->gpo_ctrl[0] = init_param->gpo_ctrl[0];
	dev->gpo_ctrl[1] = init_param->gpo_ctrl[1];
	dev->gpo_ctrl[2] = init_param->gpo_ctrl[2];
	dev->gpo_ctrl[3] = init_param->gpo_ctrl[3];

	dev->num_channels = init_param->num_channels;
	dev->channels = (struct hmc7044_chan_spec *)
			no_os_malloc(sizeof(*dev->channels) * dev->num_channels);

	for (i = 0; i < dev->num_channels; i++) {
		dev->channels[i].num = init_param->channels[i].num;
		dev->channels[i].disable = init_param->channels[i].disable;
		dev->channels[i].divider = init_param->channels[i].divider;
		dev->channels[i].driver_mode =
			init_param->channels[i].driver_mode;
		dev->channels[i].high_performance_mode_dis =
			init_param->channels[i].high_performance_mode_dis;
		dev->channels[i].start_up_mode_dynamic_enable =
			init_param->channels[i].start_up_mode_dynamic_enable;
		dev->channels[i].dynamic_driver_enable =
			init_param->channels[i].dynamic_driver_enable;
		dev->channels[i].output_control0_rb4_enable =
			init_param->channels[i].output_control0_rb4_enable;
		dev->channels[i].force_mute_enable =
			init_param->channels[i].force_mute_enable;
		dev->channels[i].is_sysref =
			init_param->channels[i].is_sysref;
		dev->channels[i].driver_impedance =
			init_param->channels[i].driver_impedance;
		dev->channels[i].coarse_delay =
			init_param->channels[i].coarse_delay;
		dev->channels[i].fine_delay =
			init_param->channels[i].fine_delay;
		dev->channels[i].out_mux_mode =
			init_param->channels[i].out_mux_mode;
	}

	ret = jesd204_dev_register(&dev->jdev, &jesd204_hmc7044_init);
	if (ret < 0)
		return ret;
	priv = jesd204_dev_priv(dev->jdev);
	priv->hmc = dev;

	*device = dev;

	if (!dev->is_hmc7043)
		return hmc7044_setup(dev);
	else
		return hmc7043_setup(dev);
}

/**
 * Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_remove(struct hmc7044_dev *device)
{
	int32_t ret;

	ret = no_os_spi_remove(device->spi_desc);
	no_os_free(device->channels);
	no_os_free(device);

	return ret;
}

/**
 * @brief Recalculate the clock rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The desired rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int hmc7044_recalc_rate(struct no_os_clk_desc *desc, uint64_t *rate)
{
	return hmc7044_clk_recalc_rate(desc->dev_desc, desc->hw_ch_num,
				       rate);
}

/**
 * @brief Round the desired rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The desired rate.
 * @param rounded_rate - The rounded rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int hmc7044_round_rate(struct no_os_clk_desc *desc,
			      uint64_t rate, uint64_t *rounded_rate)
{
	return hmc7044_clk_round_rate(desc->dev_desc, rate,
				      rounded_rate);
}

/**
 * @brief Set the clock rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - The desired rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t hmc7044_set_rate(struct no_os_clk_desc *desc,
			 uint64_t rate)
{
	return hmc7044_clk_set_rate(desc->dev_desc, desc->hw_ch_num,
				    rate);
}

/**
 * @brief hmc7044 clock ops
 */
const struct no_os_clk_platform_ops hmc7044_clk_ops = {
	.clk_recalc_rate =&hmc7044_recalc_rate,
	.clk_round_rate = &hmc7044_round_rate,
	.clk_set_rate = &hmc7044_set_rate,
};
