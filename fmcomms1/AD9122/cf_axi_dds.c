/***************************************************************************//**
 *   @file   cf_axi_dds.c
 *   @brief  Implementation file of AD9122 Driver helper functions.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

#include "spi_interface.h"
#include "dac_core.h"
#include "cf_axi_dds.h"
#include "ad9122.h"

/******************************************************************************/
/***************************** Global State Variables *************************/
/******************************************************************************/
struct cf_axi_dds_state 	dds_state;
struct cf_axi_dds_converter dds_conv;

/******************************************************************************/
/***************************** Delay Functions ********************************/
/******************************************************************************/
extern void delay_us(uint32_t us_count);
#define mdelay(x) delay_us(x*1000)
#define msleep(x) delay_us(x*1000)

/******************************************************************************/
/***************************** Macros *****************************************/
/******************************************************************************/
#define test_bit(bit, val) \
	(((*val) & (1 << bit)) != 0)
#define set_bit(bit, val) \
	(*val |= (1 << bit))
#define sign_extend32(value, index) \
	((int32_t)(value << (31-index)) >> (31-index))
#define ARRAY_SIZE(x) \
	sizeof(x) / sizeof(x[0])

/******************************************************************************/
/***************************** DAC SED Patterns *******************************/
/******************************************************************************/
struct cf_axi_dds_sed
{
	uint16_t i0;
	uint16_t q0;
	uint16_t i1;
	uint16_t q1;
};

static struct cf_axi_dds_sed dac_sed_pattern[5] =
{
	{
		.i0 = 0x5555,
		.q0 = 0xAAAA,
		.i1 = 0xAAAA,
		.q1 = 0x5555,
	},
	{
		.i0 = 0,
		.q0 = 0,
		.i1 = 0xFFFF,
		.q1 = 0xFFFF,
	},
	{
		.i0 = 0,
		.q0 = 0,
		.i1 = 0,
		.q1 = 0,
	},
	{
		.i0 = 0xFFFF,
		.q0 = 0xFFFF,
		.i1 = 0xFFFF,
		.q1 = 0xFFFF,
	},
	{
		.i0 = 0x1248,
		.q0 = 0xEDC7,
		.i1 = 0xEDC7,
		.q1 = 0x1248,
	}
};

/***************************************************************************//**
 * @brief Computes the mod and integer division of two numbers.
 *
 * @return Returns the mod value, stores in the a parameter the integer division
*******************************************************************************/
uint64_t do_div(void* a, uint32_t b)
{
	uint64_t mod;
	mod = *(uint64_t *)a % b;
	*(uint64_t *)a = *(uint64_t *)a / b;

	return mod;
}

/***************************************************************************//**
 * @brief Writes data to the AXI DAC core.
 *
 * @return None.
*******************************************************************************/
void dds_write(struct cf_axi_dds_state* st, uint32_t reg, uint32_t val)
{
	DAC_Core_Write(reg, val);
}

/***************************************************************************//**
 * @brief Reads data from the AXI DAC core.
 *
 * @return Returns the read value.
*******************************************************************************/
uint32_t dds_read(struct cf_axi_dds_state* st, uint32_t reg)
{
	uint32_t data;
	DAC_Core_Read(reg, &data);

	return data;
}

/***************************************************************************//**
 * @brief Waits for the AXI DAC core to sync.
 *
 * @return None.
*******************************************************************************/
void cf_axi_dds_sync_frame()
{
	struct cf_axi_dds_state *st = &dds_state;
	struct cf_axi_dds_converter *conv = &dds_conv;
	uint32_t stat;
	static int32_t retry = 0;

	mdelay(10); /* Wait until clocks are stable */

	dds_write(st, CF_AXI_DDS_FRAME, 0);
	dds_write(st, CF_AXI_DDS_FRAME, CF_AXI_DDS_FRAME_SYNC);

	/* Check FIFO status */
	stat = conv->read(AD9122_REG_FIFO_STATUS_1);
	if (stat & (AD9122_FIFO_STATUS_1_FIFO_WARNING_1 |
		AD9122_FIFO_STATUS_1_FIFO_WARNING_2)) {
		if (retry++ > 3) {
			return;
		}

		cf_axi_dds_sync_frame();
	}

	retry = 0;
}

/***************************************************************************//**
 * @brief Stops the AXI DAC core.
 *
 * @return None.
*******************************************************************************/
void cf_axi_dds_stop(struct cf_axi_dds_state *st)
{
	dds_write(st, CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DDS_CLK_EN_V2);
}

/***************************************************************************//**
 * @brief Computes the frequency divider and phase offset.
 *
 * @return Returns the frequency divider and phase offset.
*******************************************************************************/
static uint32_t cf_axi_dds_calc(uint32_t phase, uint32_t freq, uint32_t dac_clk)
{

	uint64_t val64;
	uint32_t val;

	val64 = (uint64_t) freq * 0xFFFFULL;
	do_div(&val64, dac_clk);
	val = ((val64 & 0xFFFF) | 1);

	val64 = (uint64_t) phase * 0xFFFFULL;
	do_div(&val64, 360000);
	val |= val64 << 16;

	return val;
}

/***************************************************************************//**
 * @brief Finds the DCI.
 *
 * @return Returns the DCI value
*******************************************************************************/
static int32_t cf_axi_dds_find_dci(uint32_t *err_field, uint32_t entries)
{
	int32_t dci, cnt, start, max_start, max_cnt;
	int32_t ret;
#ifdef DEBUG_DCI
	int8_t str[33];
#endif

	for(dci = 0, cnt = 0, max_cnt = 0, start = -1, max_start = 0;
		dci < entries; dci++) {
		if (test_bit(dci, err_field) == 0) {
			if (start == -1)
				start = dci;
			cnt++;
#ifdef DEBUG_DCI
			str[dci] = 'o';
#endif
		} else {
			if (cnt > max_cnt) {
				max_cnt = cnt;
				max_start = start;
			}
			start = -1;
			cnt = 0;
#ifdef DEBUG_DCI
			str[dci] = '-';
#endif
		}
	}
#ifdef DEBUG_DCI
	str[dci] = 0;
#endif
	if (cnt > max_cnt) {
		max_cnt = cnt;
		max_start = start;
	}


	ret = max_start + ((max_cnt - 1) / 2);

#ifdef DEBUG_DCI
	str[ret] = '|';
	xil_printf("%s DCI %d\n",str, ret);
#endif

	return ret;
}

/***************************************************************************//**
 * @brief Calibrates the AD9122 DCI.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t cf_axi_dds_tune_dci(struct cf_axi_dds_state *st)
{
	struct cf_axi_dds_converter *conv = &dds_conv;
	uint32_t reg, err_mask, pwr;
	int32_t i = 0, dci;
	uint32_t err_bfield = 0;

	pwr = conv->read(AD9122_REG_POWER_CTRL);
	conv->write(AD9122_REG_POWER_CTRL, pwr |
			AD9122_POWER_CTRL_PD_I_DAC |
			AD9122_POWER_CTRL_PD_Q_DAC);

	for (dci = 0; dci < 4; dci++) {
		conv->write(AD9122_REG_DCI_DELAY, dci);
		for (i = 0; i < ARRAY_SIZE(dac_sed_pattern); i++) {

			conv->write(AD9122_REG_SED_CTRL, 0);

			dds_write(st, CF_AXI_DDS_PAT_DATA1,
				  (dac_sed_pattern[i].i1 << 16) |
				  dac_sed_pattern[i].i0);
			dds_write(st, CF_AXI_DDS_PAT_DATA2,
				  (dac_sed_pattern[i].q1 << 16) |
				  dac_sed_pattern[i].q0);

			dds_write(st, CF_AXI_DDS_CTRL, 0);
			dds_write(st, CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DDS_CLK_EN_V2 |
				 CF_AXI_DDS_CTRL_PATTERN_EN);
			dds_write(st, CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DDS_CLK_EN_V2 |
				 CF_AXI_DDS_CTRL_PATTERN_EN | CF_AXI_DDS_CTRL_DATA_EN);

			conv->write(AD9122_REG_COMPARE_I0_LSBS,
				dac_sed_pattern[i].i0 & 0xFF);
			conv->write(AD9122_REG_COMPARE_I0_MSBS,
				dac_sed_pattern[i].i0 >> 8);

			conv->write(AD9122_REG_COMPARE_Q0_LSBS,
				dac_sed_pattern[i].q0 & 0xFF);
			conv->write(AD9122_REG_COMPARE_Q0_MSBS,
				dac_sed_pattern[i].q0 >> 8);

			conv->write(AD9122_REG_COMPARE_I1_LSBS,
				dac_sed_pattern[i].i1 & 0xFF);
			conv->write(AD9122_REG_COMPARE_I1_MSBS,
				dac_sed_pattern[i].i1 >> 8);

			conv->write(AD9122_REG_COMPARE_Q1_LSBS,
				dac_sed_pattern[i].q1 & 0xFF);
			conv->write(AD9122_REG_COMPARE_Q1_MSBS,
				dac_sed_pattern[i].q1 >> 8);


			conv->write(AD9122_REG_SED_CTRL,
				    AD9122_SED_CTRL_SED_COMPARE_EN);

 			conv->write(AD9122_REG_EVENT_FLAG_2,
 				    AD9122_EVENT_FLAG_2_AED_COMPARE_PASS |
				    AD9122_EVENT_FLAG_2_AED_COMPARE_FAIL |
				    AD9122_EVENT_FLAG_2_SED_COMPARE_FAIL);

			conv->write(AD9122_REG_SED_CTRL,
				    AD9122_SED_CTRL_SED_COMPARE_EN);

			msleep(100);
			reg = conv->read(AD9122_REG_SED_CTRL);
			err_mask = conv->read(AD9122_REG_SED_I_LSBS);
			err_mask |= conv->read(AD9122_REG_SED_I_MSBS);
			err_mask |= conv->read(AD9122_REG_SED_Q_LSBS);
			err_mask |= conv->read(AD9122_REG_SED_Q_MSBS);

			if (err_mask || (reg & AD9122_SED_CTRL_SAMPLE_ERR_DETECTED))
				set_bit(dci, &err_bfield);
		}
	}

	conv->write(AD9122_REG_DCI_DELAY,
		    cf_axi_dds_find_dci(&err_bfield, 4));
	conv->write(AD9122_REG_SED_CTRL, 0);
	conv->write(AD9122_REG_POWER_CTRL, pwr);

	dds_write(st, CF_AXI_DDS_CTRL, 0);

	return 0;
}

/***************************************************************************//**
 * @brief Reads parameters from the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t cf_axi_dds_read_raw(uint32_t channel,
						uint32_t address,
						int32_t *val,
						int32_t *val2,
						int32_t m)
{
	struct cf_axi_dds_state *st = &dds_state;
	struct cf_axi_dds_converter *conv = &dds_conv;
	uint64_t val64;
	uint32_t reg;

	switch (m) {
	case 0:
		reg = dds_read(st, CF_AXI_DDS_CTRL);
		if (reg & CF_AXI_DDS_CTRL_DATA_EN)
			*val = 1;
		else
			*val = 0;
		return 0;
	case IIO_CHAN_INFO_SCALE:
		reg = dds_read(st, CF_AXI_DDS_SCALE);
		reg = (reg >> (channel * 4)) & 0xF;
		if (!reg) {
			*val = 1;
			*val2 = 0;
		} else {
			*val = 0;
			*val2 = 1000000 >> reg;
		}
		return 0;
	case IIO_CHAN_INFO_FREQUENCY:
		reg = dds_read(st, address);
		val64 = (uint64_t)(reg & 0xFFFF) * (uint64_t)st->dac_clk;
		do_div(&val64, 0xFFFF);
		*val = val64;
		return 0;
	case IIO_CHAN_INFO_PHASE:
		reg = dds_read(st, address);
		val64 = (uint64_t)(reg >> 16) * 360000ULL;
		do_div(&val64, 0xFFFF);
		*val = val64;
		return 0;
	case IIO_CHAN_INFO_SAMP_FREQ:
		if (!conv->get_data_clk)
			return -1;

		*val = st->dac_clk = conv->get_data_clk(conv);
		return 0;

	}
	return -1;
}

/***************************************************************************//**
 * @brief Writes parameters to the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t cf_axi_dds_write_raw(uint32_t channel,
						uint32_t address,
						int32_t val,
						int32_t val2,
						int32_t mask)
{
	struct cf_axi_dds_state *st = &dds_state;
	struct cf_axi_dds_converter *conv = &dds_conv;
	uint64_t val64;
	uint32_t reg, ctrl_reg;
	int32_t i, ret;

	ctrl_reg = dds_read(st, CF_AXI_DDS_CTRL);

	switch (mask) {
	case 0:

		if (val)
			ctrl_reg |= (CF_AXI_DDS_CTRL_DATA_EN |
					CF_AXI_DDS_CTRL_DDS_CLK_EN_V2);
		else
			ctrl_reg &= ~(CF_AXI_DDS_CTRL_DATA_EN);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		break;
	case IIO_CHAN_INFO_SCALE:
		if (val == 1) {
			i = 0;
		} else {
			for (i = 1; i < 16; i++)
				if (val2 == (1000000 >> i))
					break;
		}
		cf_axi_dds_stop(st);
		reg = dds_read(st, CF_AXI_DDS_SCALE);

		reg &= ~(0xF << (channel * 4));
		reg |= (i << (channel * 4));
		dds_write(st, CF_AXI_DDS_SCALE, reg);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		break;
	case IIO_CHAN_INFO_FREQUENCY:
		if (val > (st->dac_clk / 2))
			return -1;
		cf_axi_dds_stop(st);
		reg = dds_read(st, address);
		reg &= 0xFFFF0000;
		val64 = (uint64_t) val * 0xFFFFULL;
		do_div(&val64, st->dac_clk);
		reg |= (val64 & 0xFFFF) | 1;
		dds_write(st, address, reg);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		break;
	case IIO_CHAN_INFO_PHASE:
		if (val < 0 || val > 360000)
			return -1;
		cf_axi_dds_stop(st);
		reg = dds_read(st, address);
		reg &= 0x0000FFFF;
		val64 = (uint64_t) val * 0xFFFFULL;
		do_div(&val64, 360000);
		reg |= val64 << 16;
		dds_write(st, address, reg);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		break;
	case IIO_CHAN_INFO_SAMP_FREQ:
		if (!conv->set_data_clk)
			return -1;

		cf_axi_dds_stop(st);
		ret = conv->set_data_clk(conv, val);
		if (ret < 0) {
			dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
			return ret;
		}

		if (val != st->dac_clk) {
			cf_axi_dds_tune_dci(st);
		}

		st->dac_clk = conv->get_data_clk(conv);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		cf_axi_dds_sync_frame();

		break;
	default:
		return -1;
	}
//	cf_axi_dds_sync_frame(st);

	return 0;
}

/***************************************************************************//**
 * @brief Writes data to the AD9122.
 *
 * @return Returns negative error code or the written value in case of success.
*******************************************************************************/
uint32_t ad9122_dds_store(uint32_t address, int32_t readin)
{
	struct cf_axi_dds_converter *conv = &dds_conv;
	int32_t ret = 0;

	switch (address) {
	case AD9122_REG_I_DAC_OFFSET_MSB:
	case AD9122_REG_Q_DAC_OFFSET_MSB:
		if (readin < 0 || readin > 0xFFFF) {
			ret = -1;
			goto out;
		}
		break;
	case AD9122_REG_I_PHA_ADJ_MSB:
	case AD9122_REG_Q_PHA_ADJ_MSB:
		if (readin < -512 || readin > 511) {
			ret = -1;
			goto out;
		}
		break;
	default:
		if (readin < 0 || readin > 0x3FF) {
			ret = -1;
			goto out;
		}
		break;
	}

	ret = conv->write((uint32_t)address, readin >> 8);
	if (ret < 0)
		goto out;

	ret = conv->write((uint32_t)address - 1, readin & 0xFF);
	if (ret < 0)
		goto out;

out:

	return ret ? ret : readin;
}

/***************************************************************************//**
 * @brief Reads data from the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
uint32_t ad9122_dds_show(uint32_t address, int32_t* val)
{
	struct cf_axi_dds_converter *conv = &dds_conv;
	int32_t ret = 0;

	ret = conv->read((uint32_t)address);
	if (ret < 0)
		goto out;
	*val = ret << 8;

	ret = conv->read((uint32_t)address - 1);
	if (ret < 0)
		goto out;
	*val |= ret & 0xFF;

	switch ((uint32_t)address) {
	case AD9122_REG_I_PHA_ADJ_MSB:
	case AD9122_REG_Q_PHA_ADJ_MSB:
		*val = sign_extend32(*val, 9);
		break;
	}

out:
	return ret;
}

/***************************************************************************//**
 * @brief Writes interpolation data to the AD9122.
 *
 * @return Returns negative error code or the written data in case of success.
*******************************************************************************/
uint32_t ad9122_dds_interpolation_store(uint32_t address,
										int32_t readin)
{
	struct cf_axi_dds_state *st = &dds_state;
	struct cf_axi_dds_converter *conv = &dds_conv;
	uint32_t ctrl_reg;
	int32_t ret;


	if (!conv->set_interpol)
		return -1;

	ctrl_reg = dds_read(st, CF_AXI_DDS_CTRL);
	cf_axi_dds_stop(st);

	switch (address) {
	case 0:

		ret = conv->set_interpol(conv, readin,
					conv->fcenter_shift, 0);
		break;
	case 1:
		ret = conv->set_interpol(conv, conv->interp_factor,
					readin, 0);
		break;
	default:
		ret = -1;
	}

	dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
	cf_axi_dds_sync_frame();

	return ret ? ret : readin;
}

/***************************************************************************//**
 * @brief Reads interpolation data from the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
uint32_t ad9122_dds_interpolation_show(uint32_t address, int32_t* val)
{
	struct cf_axi_dds_converter *conv = &dds_conv;
	int32_t ret = 0;

	switch (address) {
	case 0:
		*val = conv->interp_factor;
		break;
	case 1:
		*val = conv->fcenter_shift;
		break;
	default:
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the DAC AXI core.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t  cf_axi_dds_of_probe()
{
	struct cf_axi_dds_state *st = &dds_state;
	struct cf_axi_dds_converter *conv = &dds_conv;

	st->dac_clk = conv->get_data_clk(conv);

	cf_axi_dds_tune_dci(st);

	dds_write(st, CF_AXI_DDS_INTERPOL_CTRL, 0x2aaa5555); /* Lin. Interp. */

	dds_write(st, CF_AXI_DDS_CTRL, 0x0);
	dds_write(st, CF_AXI_DDS_SCALE, 0x1111); /* divide by 4 */
	dds_write(st, CF_AXI_DDS_1A_OUTPUT_CTRL,
		  cf_axi_dds_calc(90000, 40000000, st->dac_clk));
	dds_write(st, CF_AXI_DDS_1B_OUTPUT_CTRL,
		  cf_axi_dds_calc(90000, 40000000, st->dac_clk));
	dds_write(st, CF_AXI_DDS_2A_OUTPUT_CTRL,
		  cf_axi_dds_calc(0, 40000000, st->dac_clk));
	dds_write(st, CF_AXI_DDS_2B_OUTPUT_CTRL,
		  cf_axi_dds_calc(0, 40000000, st->dac_clk));
	dds_write(st, CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DATA_EN |
		  CF_AXI_DDS_CTRL_DDS_CLK_EN_V2); /* clk, dds enable & ddsx select */

	cf_axi_dds_sync_frame();

	return 0;
}

