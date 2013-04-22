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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "spi_interface.h"
#include "dac_core.h"
#include "cf_axi_dds.h"

/******************************************************************************/
/***************************** Global State Variables *************************/
/******************************************************************************/
extern struct cf_axi_converter dds_conv;
#ifdef CF_AXI_DDS
struct cf_axi_dds_state 	dds_state;
#endif

/******************************************************************************/
/***************************** Delay Functions ********************************/
/******************************************************************************/
extern void delay_us(uint32_t us_count);
#define mdelay(x) delay_us(x*1000)
#define msleep(x) delay_us(x*1000)

#ifdef CF_AXI_DDS
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

/*****************************************************************************/
/************************ Private Functions Prototypes ***********************/
/*****************************************************************************/
void cf_axi_dds_sync_frame();

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
	struct cf_axi_converter *conv = &dds_conv;
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
		*val = (int32_t)val64;
		return 0;
	case IIO_CHAN_INFO_PHASE:
		reg = dds_read(st, address);
		val64 = (uint64_t)(reg >> 16) * 360000ULL;
		do_div(&val64, 0xFFFF);
		*val = (int32_t)val64;
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
	struct cf_axi_converter *conv = &dds_conv;
	uint64_t val64;
	uint32_t reg, ctrl_reg;
	int32_t i;

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
		if (val > (int32_t)(st->dac_clk / 2))
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
		if (!conv->write_raw)
			return -1;

		cf_axi_dds_stop(st);
		conv->write_raw(channel, val, val2, mask);
		st->dac_clk = conv->get_data_clk(conv);
		dds_write(st, CF_AXI_DDS_CTRL, ctrl_reg);
		cf_axi_dds_sync_frame();

		break;
	default:
		return -1;
	}

	return 0;
}

#endif //CF_AXI_DDS

/***************************************************************************//**
 * @brief Waits for the AXI DAC core to sync.
 *
 * @return None.
*******************************************************************************/
void cf_axi_dds_sync_frame()
{
	struct cf_axi_converter *conv = &dds_conv;
	uint32_t stat;
	static int32_t retry = 0;

	mdelay(10); /* Wait until clocks are stable */

	DAC_Core_Write(CF_AXI_DDS_FRAME, 0);
	DAC_Core_Write(CF_AXI_DDS_FRAME, CF_AXI_DDS_FRAME_SYNC);

	/* Check FIFO status */
	stat = conv->get_fifo_status(conv);
	if (stat) {
		if (retry++ > 10) {
			return;
		}

		cf_axi_dds_sync_frame();
	}

	retry = 0;
}

/***************************************************************************//**
 * @brief Configures the DDS with the DAC SED pattern.
 *
 * @return None.
*******************************************************************************/
static void cf_axi_dds_set_sed_pattern(unsigned pat1, unsigned pat2)
{
	DAC_Core_Write(CF_AXI_DDS_PAT_DATA1, pat1);
	DAC_Core_Write(CF_AXI_DDS_PAT_DATA2, pat2);

	DAC_Core_Write(CF_AXI_DDS_CTRL, 0);
	DAC_Core_Write(CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DDS_CLK_EN_V2 |
		 CF_AXI_DDS_CTRL_PATTERN_EN);
	DAC_Core_Write(CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DDS_CLK_EN_V2 |
		 CF_AXI_DDS_CTRL_PATTERN_EN | CF_AXI_DDS_CTRL_DATA_EN);
}

/***************************************************************************//**
 * @brief Initializes the DAC AXI core.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t  cf_axi_dds_of_probe()
{
#ifdef CF_AXI_DDS
	struct cf_axi_dds_state *st = &dds_state;
#endif
	struct cf_axi_converter *conv = &dds_conv;

	conv->pcore_sync = cf_axi_dds_sync_frame;
	conv->pcore_set_sed_pattern = cf_axi_dds_set_sed_pattern;
	conv->setup(conv);

#ifdef CF_AXI_DDS
	st->dac_clk = conv->get_data_clk(conv);
	DAC_Core_Write(CF_AXI_DDS_INTERPOL_CTRL, 0x2aaa5555); /* Lin. Interp. */
#endif

	DAC_Core_Write(CF_AXI_DDS_CTRL, 0x0);
#ifdef CF_AXI_DDS
	DAC_Core_Write(CF_AXI_DDS_SCALE, 0x1111); /* divide by 4 */
	DAC_Core_Write(CF_AXI_DDS_1A_OUTPUT_CTRL,
		  cf_axi_dds_calc(90000, 40000000, st->dac_clk));
	DAC_Core_Write(CF_AXI_DDS_1B_OUTPUT_CTRL,
		  cf_axi_dds_calc(90000, 40000000, st->dac_clk));
	DAC_Core_Write(CF_AXI_DDS_2A_OUTPUT_CTRL,
		  cf_axi_dds_calc(0, 40000000, st->dac_clk));
	DAC_Core_Write(CF_AXI_DDS_2B_OUTPUT_CTRL,
		  cf_axi_dds_calc(0, 40000000, st->dac_clk));
#endif
	DAC_Core_Write(CF_AXI_DDS_CTRL, CF_AXI_DDS_CTRL_DATA_EN |
		  CF_AXI_DDS_CTRL_DDS_CLK_EN_V2); /* clk, dds enable & ddsx select */

	cf_axi_dds_sync_frame();

	return 0;
}
