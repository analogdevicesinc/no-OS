/***************************************************************************//**
 *   @file   dac_core.c
 *   @brief  Implementation of DAC Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
#include <stdint.h>
#include "socal/socal.h"
#include "dac_core.h"
#include "parameters.h"
#include "util.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct dds_state dds_st;

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
const uint16_t sine_lut[32] = {
		0x000, 0x031, 0x061, 0x08D, 0x0B4, 0x0D4, 0x0EC, 0x0FA,
		0x0FF, 0x0FA, 0x0EC, 0x0D4, 0x0B4, 0x08D, 0x061, 0x031,
		0x000, 0xFCE, 0xF9E, 0xF72, 0xF4B, 0xF2B, 0xF13, 0xF05,
		0xF00, 0xF05, 0xF13, 0xF2B, 0xF4B, 0xF72, 0xF9E, 0xFCE
};

/***************************************************************************//**
 * @brief dac_read
*******************************************************************************/
void dac_read(uint32_t regAddr, uint32_t *data)
{
	*data = alt_read_word(CF_AD9361_TX_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief dac_write
*******************************************************************************/
void dac_write(uint32_t regAddr, uint32_t data)
{
	alt_write_word(CF_AD9361_TX_BASEADDR + regAddr, data);
}

/***************************************************************************//**
 * @brief dac_dma_read
*******************************************************************************/
void dac_dma_read(uint32_t regAddr, uint32_t *data)
{
	//FIXME
}

/***************************************************************************//**
 * @brief dac_dma_write
*******************************************************************************/
void dac_dma_write(uint32_t regAddr, uint32_t data)
{
	//FIXME
}

/***************************************************************************//**
 * @brief dds_default_setup
*******************************************************************************/
static int dds_default_setup(uint32_t chan, uint32_t phase,
							 uint32_t freq, double scale)
{
	dds_set_phase(chan, phase);
	dds_set_frequency(chan, freq);
	dds_set_scale(chan, scale);
	dds_st.cached_freq[chan] = freq;
	dds_st.cached_phase[chan] = phase;
	dds_st.cached_scale[chan] = scale;

	return 0;
}

/***************************************************************************//**
 * @brief dac_stop
*******************************************************************************/
void dac_stop(void)
{
	if (PCORE_VERSION_MAJOR(dds_st.pcore_version) < 8)
	{
		dac_write(ADI_REG_CNTRL_1, 0);
	}
}

/***************************************************************************//**
 * @brief dac_start_sync
*******************************************************************************/
void dac_start_sync(bool force_on)
{
	if (PCORE_VERSION_MAJOR(dds_st.pcore_version) < 8)
	{
		dac_write(ADI_REG_CNTRL_1, (dds_st.enable || force_on) ? ADI_ENABLE : 0);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_1, ADI_SYNC);
	}
}

/***************************************************************************//**
 * @brief dac_init
*******************************************************************************/
void dac_init(struct ad9361_rf_phy *phy, uint8_t data_sel)
{
	uint32_t tx_count;
	uint32_t index;
	uint32_t index_i1;
	uint32_t index_q1;
	uint32_t index_i2;
	uint32_t index_q2;
	uint32_t data_i1;
	uint32_t data_q1;
	uint32_t data_i2;
	uint32_t data_q2;

	dac_write(ADI_REG_RSTN, 0x0);
	dac_write(ADI_REG_RSTN, ADI_RSTN | ADI_MMCM_RSTN);

	dac_write(ADI_REG_RATECNTRL, ADI_RATE(3));

	dds_st.dac_clk = &phy->clks[TX_SAMPL_CLK]->rate;
	dds_st.num_dds_channels = 8;	// FIXME

	dac_read(ADI_REG_VERSION, &dds_st.pcore_version);

	dac_stop();
	switch (data_sel) {
	case DATA_SEL_DDS:
		dds_default_setup(DDS_CHAN_TX1_I_F1, 90000, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX1_I_F2, 90000, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX1_Q_F1, 0, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX1_Q_F2, 0, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX2_I_F1, 90000, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX2_I_F2, 90000, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX2_Q_F1, 0, 1000000, 0.25);
		dds_default_setup(DDS_CHAN_TX2_Q_F2, 0, 1000000, 0.25);
		dac_write(ADI_REG_CNTRL_2, 0);
		dac_datasel(-1, DATA_SEL_DDS);
		break;
	case DATA_SEL_DMA:
		tx_count = sizeof(sine_lut) / sizeof(uint16_t);
		for(index = 0; index < (tx_count * 2); index += 2)
		{
			index_i1 = index;
			index_q1 = index + (tx_count / 2);
			if(index_q1 >= (tx_count * 2))
				index_q1 -= (tx_count * 2);
			data_i1 = (sine_lut[index_i1 / 2] << 20);
			data_q1 = (sine_lut[index_q1 / 2] << 4);
			// FIXME

			index_i2 = index_i1;
			index_q2 = index_q1;
			if(index_i2 >= (tx_count * 2))
				index_i2 -= (tx_count * 2);
			if(index_q2 >= (tx_count * 2))
				index_q2 -= (tx_count * 2);
			data_i2 = (sine_lut[index_i2 / 2] << 20);
			data_q2 = (sine_lut[index_q2 / 2] << 4);
			// FIXME
		}
		dac_dma_write(AXI_DMAC_REG_CTRL, 0);
		dac_dma_write(AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
		// FIXME dac_dma_write(AXI_DMAC_REG_SRC_ADDRESS, DAC_DDR_BASEADDR);
		dac_dma_write(AXI_DMAC_REG_SRC_STRIDE, 0x0);
		dac_dma_write(AXI_DMAC_REG_X_LENGTH, (tx_count * 8) - 1);
		dac_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);
		dac_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);
		dac_write(ADI_REG_CNTRL_2, 0);
		dac_datasel(-1, DATA_SEL_DMA);
		break;
	default:
		break;
	}
	dds_st.enable = true;
	dac_start_sync(0);
}

/***************************************************************************//**
 * @brief dds_set_frequency
*******************************************************************************/
void dds_set_frequency(uint32_t chan, uint32_t freq)
{
	uint64_t val64;
	uint32_t reg;

	dds_st.cached_freq[chan] = freq;
	dac_stop();
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INCR(~0);
	val64 = (uint64_t) freq * 0xFFFFULL;
	do_div(&val64, *dds_st.dac_clk);
	reg |= ADI_DDS_INCR(val64) | 1;
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_start_sync(0);
}

/***************************************************************************//**
 * @brief dds_set_phase
*******************************************************************************/
void dds_set_phase(uint32_t chan, uint32_t phase)
{
	uint64_t val64;
	uint32_t reg;

	dds_st.cached_phase[chan] = phase;
	dac_stop();
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INIT(~0);
	val64 = (uint64_t) phase * 0x10000ULL + (360000 / 2);
	do_div(&val64, 360000);
	reg |= ADI_DDS_INIT(val64);
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	dac_start_sync(0);
}

/***************************************************************************//**
 * @brief dds_set_phase
*******************************************************************************/
void dds_set_scale(uint32_t chan, double scale)
{
	uint32_t scale_reg;
	uint32_t sign_part;
	uint32_t int_part;
	uint32_t fract_part;

	if (PCORE_VERSION_MAJOR(dds_st.pcore_version) > 6)
	{
		if(scale >= 1.0)
		{
			sign_part = 0;
			int_part = 1;
			fract_part = 0;
			dds_st.cached_scale[chan] = 1.0;
			goto set_scale_reg;
		}
		if(scale <= -1.0)
		{
			sign_part = 1;
			int_part = 1;
			fract_part = 0;
			dds_st.cached_scale[chan] = -1.0;
			goto set_scale_reg;
		}
		if(scale < 0)
		{
			sign_part = 1;
			int_part = 0;
			dds_st.cached_scale[chan] = scale;
			scale *= -1;
			goto set_scale_reg;
		}
		sign_part = 0;
		int_part = 0;
		dds_st.cached_scale[chan] = scale;
		fract_part = (uint32_t)(scale * 0x4000);
	set_scale_reg:
		scale_reg = (sign_part << 15) | (int_part << 14) | fract_part;
	}
	else
	{
		if(scale >= 1.0)
		{
			scale_reg = 0;
			scale = 1.0;
		}
		if(scale <= 0.0)
		{
			scale_reg = 0;
			scale = 0.0;
		}
		dds_st.cached_scale[chan] = scale;
		fract_part = (uint32_t)(scale * 1000000);
		scale_reg = 500000 / fract_part;
	}
	dac_stop();
	dac_write(ADI_REG_CHAN_CNTRL_1_IIOCHAN(chan), ADI_DDS_SCALE(scale_reg));
	dac_start_sync(0);
}

/***************************************************************************//**
 * @brief dds_update
*******************************************************************************/
void dds_update(void)
{
	uint32_t chan;

	for(chan = DDS_CHAN_TX1_I_F1; chan <= DDS_CHAN_TX2_Q_F2; chan++)
	{
		dds_set_frequency(chan, dds_st.cached_freq[chan]);
		dds_set_phase(chan, dds_st.cached_phase[chan]);
		dds_set_scale(chan, dds_st.cached_scale[chan]);
	}
}

/***************************************************************************//**
 * @brief dac_datasel
*******************************************************************************/
int dac_datasel(int32_t chan, enum dds_data_select sel)
{
	if (PCORE_VERSION_MAJOR(dds_st.pcore_version) > 7) {
		if (chan < 0) { /* ALL */
			int i;
			for (i = 0; i < dds_st.num_dds_channels; i++) {
				dac_write(ADI_REG_CHAN_CNTRL_7(i), sel);
			}
		} else {
			dac_write(ADI_REG_CHAN_CNTRL_7(chan), sel);
		}
	} else {
		uint32_t reg;

		switch(sel) {
		case DATA_SEL_DDS:
		case DATA_SEL_SED:
		case DATA_SEL_DMA:
			dac_read(ADI_REG_CNTRL_2, &reg);
			reg &= ~ADI_DATA_SEL(~0);
			reg |= ADI_DATA_SEL(sel);
			dac_write(ADI_REG_CNTRL_2, reg);
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}
