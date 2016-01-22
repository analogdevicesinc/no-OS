/***************************************************************************//**
 *   @file   AD9122.c
 *   @brief  Implementation of AD9122 Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "spi_interface.h"
#include "dac_core.h"
#include "cf_axi_dds.h"
#include "mmcm.h"
#include "AD9122.h"

struct cf_axi_converter dds_conv;

extern void delay_us(uint32_t us_count);

int64_t (*pfnSetDataClk)(int64_t Hz);
int64_t (*pfnSetDacClk)(int64_t Hz);
int64_t (*pfnSetRefClk)(int64_t Hz);
uint32_t (*pfnRoundRateDataClk)(int32_t rate);
uint32_t (*pfnRoundRateDacClk)(int32_t rate);
uint32_t (*pfnRoundRateRefClk)(int32_t rate);

/******************************************************************************/
/***************************** Local Types and Variables***********************/
/******************************************************************************/
static const uint32_t ad9122_reg_defaults[][2] =
{
	{AD9122_REG_COMM, 0x00},
	{AD9122_REG_COMM, AD9122_COMM_RESET},
	{AD9122_REG_COMM, 0x00},
	{AD9122_REG_POWER_CTRL, 0},
	{AD9122_REG_DATA_FORMAT, AD9122_DATA_FORMAT_BINARY},
	{AD9122_REG_INTERRUPT_EN_1, 0x00},
	{AD9122_REG_INTERRUPT_EN_2, 0x00},
	{AD9122_REG_CLK_REC_CTRL, AD9122_CLK_REC_CTRL_DACCLK_CROSS_CORRECTION |
				  AD9122_CLK_REC_CTRL_REFCLK_CROSS_CORRECTION |
				  0xF},
	{AD9122_REG_PLL_CTRL_1, AD9122_PLL_CTRL_1_PLL_MANUAL_EN},
	{AD9122_REG_PLL_CTRL_2, AD9122_PLL_CTRL_2_PLL_LOOP_BANDWIDTH(3) |
				AD9122_PLL_CTRL_2_PLL_CHARGE_PUMP_CURRENT(0x11)},
	{AD9122_REG_PLL_CTRL_3, AD9122_PLL_CTRL_3_N2(3) |
				AD9122_PLL_CTRL_3_PLL_CROSS_CTRL_EN |
				AD9122_PLL_CTRL_3_N0(1) |
				AD9122_PLL_CTRL_3_N1(2)},
	{AD9122_REG_SYNC_CTRL_1, AD9122_SYNC_CTRL_1_DATA_FIFO_RATE_TOGGLE |
				 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC},
	{AD9122_REG_SYNC_CTRL_2, 0x00},
	{AD9122_REG_DCI_DELAY, 0x00},
	{AD9122_REG_FIFO_CTRL, AD9122_FIFO_CTRL_FIFO_PHA_OFFSET(4)},
	{AD9122_REG_FIFO_STATUS_1, 0x00},
	{AD9122_REG_DATAPATH_CTRL, AD9122_DATAPATH_CTRL_BYPASS_PREMOD |
				   AD9122_DATAPATH_CTRL_BYPASS_INV_SINC |
				   AD9122_DATAPATH_CTRL_BYPASS_NCO},
	{AD9122_REG_HB1_CTRL, AD9122_HB1_CTRL_BYPASS_HB1},
	{AD9122_REG_HB2_CTRL, AD9122_HB2_CTRL_BYPASS_HB2},
	{AD9122_REG_HB3_CTRL, AD9122_HB3_CTRL_BYPASS_HB3},
	{AD9122_REG_FTW_7_0, 0x00},
	{AD9122_REG_FTW_15_8, 0x00},
	{AD9122_REG_FTW_23_16, 0x00},
	{AD9122_REG_FTW_31_24, 0x00},
	{AD9122_REG_NCO_PHA_OFFSET_LSB, 0x00},
	{AD9122_REG_NCO_PHA_OFFSET_MSB, 0x00},
	{AD9122_REG_NCO_FTW_UPDATE, 0x00},
	{AD9122_REG_I_PHA_ADJ_LSB, 0x00},
	{AD9122_REG_I_PHA_ADJ_MSB, 0x00},
	{AD9122_REG_Q_PHA_ADJ_LSB, 0x00},
	{AD9122_REG_Q_PHA_ADJ_MSB, 0x00},
	{AD9122_REG_I_DAC_OFFSET_LSB, 0x00},
	{AD9122_REG_I_DAC_OFFSET_MSB, 0x00},
	{AD9122_REG_Q_DAC_OFFSET_LSB, 0x00},
	{AD9122_REG_Q_DAC_OFFSET_MSB, 0x00},
	{AD9122_REG_I_DAC_FS_ADJ, 0xF9},
	{AD9122_REG_I_DAC_CTRL, 0x01},
	{AD9122_REG_I_AUX_DAC_DATA, 0x00},
	{AD9122_REG_I_AUX_DAC_CTRL, 0x00},
	{AD9122_REG_Q_DAC_FS_ADJ, 0xF9},
	{AD9122_REG_Q_DAC_CTRL, 0x01},
	{AD9122_REG_Q_AUX_DAC_DATA, 0x00},
	{AD9122_REG_Q_AUX_DAC_CTRL, 0x00},
	{AD9122_REG_DIE_TEMP_RANGE_CTRL, AD9122_DIE_TEMP_RANGE_CTRL_REF_CURRENT(1)},
	{AD9122_REG_FIFO_STATUS_1, AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_REQ},
};

struct ad9122_sed {
	unsigned short i0;
	unsigned short q0;
	unsigned short i1;
	unsigned short q1;
};

static struct ad9122_sed dac_sed_pattern[5] = {
	{
		0x5555,
		0xAAAA,
		0x5555,
		0xAAAA
	},
	{
		0,
		0xFFFF,
		0,
		0xFFFF
	},
	{
		0,
		0,
		0,
		0
	},
	{
		0xFFFF,
		0xFFFF,
		0xFFFF,
		0xFFFF
	},
	{
		0x1248,
		0xEDC7,
		0xEDC7,
		0x1248
	}
};

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

/***************************************************************************//**
 * @brief Writes a value to the selected register.
 *
 * @param registerAddress - The address of the register to write to.
 * @param registerValue - The value to write to the register.
 *
 * @return None.
*******************************************************************************/
int32_t ad9122_write(uint8_t registerAddress,
				     uint8_t registerValue)
{
	uint8_t regAddr = 0;
	
	regAddr = (0x7F & registerAddress);
	return SPI_Write(SPI_SEL_AD9122, regAddr, registerValue);
}

/***************************************************************************//**
 * @brief Reads the value of the selected register.
 *
 * @param regAddress - The address of the register to read.
 *
 * @return registerValue - The register's value or negative error code.
*******************************************************************************/
int32_t ad9122_read(uint8_t registerAddress)
{
	uint8_t regAddr			= 0;
	uint32_t registerValue 	= 0;
	int32_t ret;

	regAddr = 0x80 + (0x7F & registerAddress);
    ret = SPI_Read(SPI_SEL_AD9122, regAddr, &registerValue);

	return (ret < 0 ? ret : (int32_t)registerValue);
}

/***************************************************************************//**
 * @brief Finds the DCI.
 *
 * @return Returns the DCI value
*******************************************************************************/
static int32_t ad9122_find_dci(uint32_t *err_field, uint32_t entries)
{
	int32_t dci, cnt, start, max_start, max_cnt;
	int32_t ret;
	int32_t valid_dci = 0;
#ifdef DEBUG_DCI
	int8_t str[33];
#endif

	for(dci = 0, cnt = 0, max_cnt = 0, start = -1, max_start = 0;
		dci < (int32_t)entries; dci++) {
		if (test_bit(dci, err_field) == 0) {
			valid_dci = 1;
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
	if(valid_dci == 1)
	{
		return ret;
	}
	else
	{
		return -1;
	}
}

/***************************************************************************//**
 * @brief Calibrates the AD9122 DCI.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_tune_dci(struct cf_axi_converter *conv)
{
	uint32_t reg;
	int32_t i = 0, dci;
	uint32_t err_bfield = 0;

	for (dci = 0; dci < 4; dci++) {
		ad9122_write(AD9122_REG_DCI_DELAY, dci);
		for (i = 0; i < ARRAY_SIZE(dac_sed_pattern); i++) {

			ad9122_write(AD9122_REG_SED_CTRL, 0);

			if(conv->pcore_set_sed_pattern)
			{
				conv->pcore_set_sed_pattern(0,
					dac_sed_pattern[i].i0, dac_sed_pattern[i].i1);
				conv->pcore_set_sed_pattern(1,
					dac_sed_pattern[i].q0, dac_sed_pattern[i].q1);
			}

			ad9122_write(AD9122_REG_COMPARE_I0_LSBS,
				dac_sed_pattern[i].i0 & 0xFF);
			ad9122_write(AD9122_REG_COMPARE_I0_MSBS,
				dac_sed_pattern[i].i0 >> 8);

			ad9122_write(AD9122_REG_COMPARE_Q0_LSBS,
				dac_sed_pattern[i].q0 & 0xFF);
			ad9122_write(AD9122_REG_COMPARE_Q0_MSBS,
				dac_sed_pattern[i].q0 >> 8);

			ad9122_write(AD9122_REG_COMPARE_I1_LSBS,
				dac_sed_pattern[i].i1 & 0xFF);
			ad9122_write(AD9122_REG_COMPARE_I1_MSBS,
				dac_sed_pattern[i].i1 >> 8);

			ad9122_write(AD9122_REG_COMPARE_Q1_LSBS,
				dac_sed_pattern[i].q1 & 0xFF);
			ad9122_write(AD9122_REG_COMPARE_Q1_MSBS,
				dac_sed_pattern[i].q1 >> 8);

			ad9122_write(AD9122_REG_SED_CTRL,
				    AD9122_SED_CTRL_SED_COMPARE_EN);

			ad9122_write(AD9122_REG_EVENT_FLAG_2,
 				    AD9122_EVENT_FLAG_2_AED_COMPARE_PASS |
				    AD9122_EVENT_FLAG_2_AED_COMPARE_FAIL |
				    AD9122_EVENT_FLAG_2_SED_COMPARE_FAIL);

			ad9122_write(AD9122_REG_SED_CTRL,
				    AD9122_SED_CTRL_SED_COMPARE_EN |
				    AD9122_SED_CTRL_AUTOCLEAR_EN);

			msleep(100);
			reg = ad9122_read(AD9122_REG_SED_CTRL);
			if(!(reg & (AD9122_SED_CTRL_SAMPLE_ERR_DETECTED | AD9122_SED_CTRL_COMPARE_PASS)))
			{
				return -1;
			}
			if (reg & AD9122_SED_CTRL_SAMPLE_ERR_DETECTED)
				set_bit(dci, &err_bfield);
		}
	}
	dci = ad9122_find_dci(&err_bfield, 4);
	if(dci < 0)
	{
		return -1;
	}
	ad9122_write(AD9122_REG_DCI_DELAY, dci);
	ad9122_write(AD9122_REG_SED_CTRL, 0);

	return 0;
}

/***************************************************************************//**
 * @brief Gets the AD9122 FIFO status.
 *
 * @return Returns negative error code in case of FIFO warning
 * 		   or 0 in case of success.
*******************************************************************************/
int32_t ad9122_get_fifo_status(struct cf_axi_converter *conv)
{
	uint32_t stat;

	stat = ad9122_read(AD9122_REG_SYNC_STATUS_1);
	if (!(stat & AD9122_SYNC_STATUS_1_SYNC_LOCKED))
		return -1;

	stat = ad9122_read(AD9122_REG_FIFO_STATUS_1);
	if (stat & (AD9122_FIFO_STATUS_1_FIFO_WARNING_1 |
				AD9122_FIFO_STATUS_1_FIFO_WARNING_2))
		return -1;

	return 0;
}

/***************************************************************************//**
 * @brief Waits for the AD9122 to sync.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_sync()
{
	int32_t ret, timeout;

	timeout = 255;
	do
	{
		delay_us(1000);
		ret = ad9122_read(AD9122_REG_FIFO_STATUS_1);
		if (ret < 0)
			return ret;

	} while (timeout-- && !(ret & AD9122_FIFO_STATUS_1_FIFO_SOFT_ALIGN_ACK));

	ad9122_write(AD9122_REG_FIFO_STATUS_1, 0x0);
	ad9122_write(AD9122_REG_SYNC_CTRL_1,
		     	 AD9122_SYNC_CTRL_1_SYNC_EN |
		     	 AD9122_SYNC_CTRL_1_RISING_EDGE_SYNC);

	timeout = 255;
	do
	{
		delay_us(1000);
		ret = ad9122_read(AD9122_REG_SYNC_STATUS_1);
		if (ret < 0)
			return ret;

	} while (timeout-- && !(ret & AD9122_SYNC_STATUS_1_SYNC_LOCKED));

	return 0;
}

/***************************************************************************//**
 * @brief Returns the value of the data clock.
 *
 * @param conv - Pointer to a cf_axi_converter struct which contains the
 * 				 data clock value
 *
 * @return eturns the value of the data clock.
*******************************************************************************/
static uint32_t ad9122_get_data_clk(struct cf_axi_converter *conv)
{
	return conv->clk[CLK_DATA];
}

/***************************************************************************//**
 * @brief Updates the available interpolation frequencies list.
 *
 * @param conv - Pointer to a cf_axi_converter struct
 * @param dat_freq - Data clock frequency in Hz
 *
 * @return None.
*******************************************************************************/
static void ad9122_update_avail_intp_modes(struct cf_axi_converter *conv,
					 	 	 	 	 	   uint32_t dat_freq)
{
	uint32_t dac_freq;
	int32_t r_dac_freq;
	int32_t intp, i;

	for (i = 0, intp = 1; intp <= 8; intp *= 2) {
		dac_freq = dat_freq * intp;
		if (dac_freq > AD9122_MAX_DAC_RATE) {
			break;
		}
		r_dac_freq = pfnRoundRateDacClk(dac_freq);
		if (r_dac_freq != dac_freq)
			continue;
		else
			conv->intp_modes[i++] = dac_freq;
	}

	conv->intp_modes[i] = 0;
}

/***************************************************************************//**
 * @brief Updates the available center frequencies list.
 *
 * @param conv - Pointer to a cf_axi_converter struct
 * @param freq - Data clock value
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static void ad9122_update_avail_fcent_modes(struct cf_axi_converter *conv,
					  	  	  	  	  	    uint32_t dat_freq)
{
	uint32_t i;

	if (conv->interp_factor == 1) {
		conv->cs_modes[0] = 0;
		conv->cs_modes[1] = -1;
		return;
	}

	for (i = 0; i < (conv->interp_factor * 2); i++) {
		conv->cs_modes[i] = (dat_freq * i) / 2;
	}

	conv->cs_modes[i] = -1;
}

/***************************************************************************//**
 * @brief Sets the value of the data clock.
 *
 * @param conv - Pointer to a cf_axi_converter struct
 * @param freq - Data clock value
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_data_clk(struct cf_axi_converter *conv, uint32_t freq)
{
	uint32_t dac_freq;
	int32_t dat_freq, r_dac_freq, r_ref_freq;
	int64_t ret;

	dat_freq = pfnRoundRateDataClk(freq);
	if (dat_freq < 0 || dat_freq > AD9122_MAX_DAC_RATE) {
#ifdef CLK_DEBUG
		xil_printf("CLK_DATA: Error or requested rate exceeds maximum %ld (%lu)",
				   dat_freq, AD9122_MAX_DAC_RATE);
#endif
		return -1;
	}

	dac_freq = dat_freq * conv->interp_factor;
	if (dac_freq > AD9122_MAX_DAC_RATE) {
#ifdef CLK_DEBUG
		xil_printf("CLK_DAC: Requested Rate exceeds maximum %lu (%lu)",
					dac_freq, AD9122_MAX_DAC_RATE);
#endif
		return -1;
	}

	r_dac_freq = pfnRoundRateDacClk(dac_freq);
	if (r_dac_freq != dac_freq) {
#ifdef CLK_DEBUG
		xil_printf("CLK_DAC: Requested Rate mismatch %ld (%lu)",
				   r_dac_freq, dac_freq);
#endif
		return -1;
	}

	r_ref_freq = pfnRoundRateRefClk(dat_freq / 8);
	if (r_ref_freq != (dat_freq / 8)) {
#ifdef CLK_DEBUG
		xil_printf("CLK_REF: Requested Rate exceeds mismatch %ld (%lu)",
				r_ref_freq, (dat_freq / 8));
#endif
		return -1;
	}

	ret = pfnSetDataClk(dat_freq);
	if(ret < 0)
		return (int32_t)ret;
	conv->clk[CLK_DATA] = (uint32_t)ret;
	mmcm_set_rate(conv->clk[CLK_DATA], conv->clk[CLK_DATA]);

	ret = pfnSetRefClk(r_ref_freq);
	if(ret < 0)
		return (int32_t)ret;
	conv->clk[CLK_REF] = (uint32_t)ret;

	ret = pfnSetDacClk(dac_freq);
	if(ret < 0)
		return (int32_t)ret;
	conv->clk[CLK_DAC]  = (uint32_t)ret;

	ad9122_update_avail_fcent_modes(conv, dat_freq);
	ad9122_update_avail_intp_modes(conv, dat_freq);

	return 0;
}

/***************************************************************************//**
 * @brief Validates the interpolation factor
 *
 * @param fact - Interpolation factor
 *
 * @return Returns a valid interpolation factor.
*******************************************************************************/
static uint32_t ad9122_validate_interp_factor(unsigned fact)
{
	switch (fact)
	{
		case 1:
		case 2:
		case 4:
		case 8:
			return fact;
		default:
			return 1;
	}
}

/***************************************************************************//**
 * @brief Sets the interpolation factor and the center shift frequency.
 *
 * @param conv - Pointer to a cf_axi_converter struct.
 * @param interp - Interpolation factor
 * @param fcent_shift - Center frequency shift as a multiplier of fData / 2. 
*               		The shift values should be in the range [0, 15]
 * @param data_rate - Data rate in Hz
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_interpol(struct cf_axi_converter *conv,
								   uint32_t interp,
								   uint32_t fcent_shift,
								   uint32_t data_rate)
{
	uint32_t hb1, hb2, hb3, tmp;
	int32_t ret, cached;

	hb1 = AD9122_HB1_CTRL_BYPASS_HB1;
	hb2 = AD9122_HB2_CTRL_BYPASS_HB2;
	hb3 = AD9122_HB3_CTRL_BYPASS_HB3;

	switch (interp) {
		case 1:
			break;
		case 2:
			if (fcent_shift > 3)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift);
			break;
		case 4:
			if (fcent_shift > 7)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift % 4);
			hb2 = AD9122_HB23_INTERP(fcent_shift);
			break;
		case 8:
			if (fcent_shift > 15)
				return -1;
			hb1 = AD9122_HB1_INTERP(fcent_shift % 4);
			hb2 = AD9122_HB23_INTERP(fcent_shift % 8);
			hb3 = AD9122_HB23_INTERP(fcent_shift / 2);
			break;
		default:
			return -1;
	}

	cached = conv->interp_factor;
	conv->interp_factor = interp;
	ret = ad9122_set_data_clk(conv, data_rate ?
				 data_rate : ad9122_get_data_clk(conv));

	if (ret < 0) {
		conv->interp_factor = cached;

		return ret;
	}

	tmp = ad9122_read(AD9122_REG_DATAPATH_CTRL);
	switch (hb1) {
		case AD9122_HB1_INTERP(1):
		case AD9122_HB1_INTERP(3):
			tmp &= ~AD9122_DATAPATH_CTRL_BYPASS_PREMOD;
			break;
		default:
			tmp |= AD9122_DATAPATH_CTRL_BYPASS_PREMOD;
	}

	ad9122_write(AD9122_REG_DATAPATH_CTRL, tmp);
	ad9122_write(AD9122_REG_HB1_CTRL, hb1);
	ad9122_write(AD9122_REG_HB2_CTRL, hb2);
	ad9122_write(AD9122_REG_HB3_CTRL, hb3);
	conv->fcenter_shift = fcent_shift;

	return 0;
}
/***************************************************************************//**
 * @brief Sets the interpolation frequency.
 *
 * @param conv - Pointer to a cf_axi_converter struct.
 * @param freq - Interpolation frequency in Hz
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_interpol_freq(struct cf_axi_converter *conv,
										uint32_t freq)
{
	uint32_t dat_freq;
	int32_t ret;

	dat_freq = ad9122_get_data_clk(conv);
	ret = ad9122_set_interpol(conv, freq / dat_freq,
							  conv->fcenter_shift, 0);
	ad9122_update_avail_fcent_modes(conv, dat_freq);

	return ret;
}

/***************************************************************************//**
 * @brief Sets the interpolation center frequency shift.
 *
 * @param conv - Pointer to a cf_axi_converter struct.
 * @param freq - Center frequency shift in Hz
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
static int32_t ad9122_set_interpol_fcent_freq(struct cf_axi_converter *conv,
											  uint32_t freq)
{
	return ad9122_set_interpol(conv, conv->interp_factor,
							  (freq * 2) / ad9122_get_data_clk(conv), 0);
}

/***************************************************************************//**
 * @brief Returns the interpolation frequency.
 *
 * @param conv - Pointer to a cf_axi_converter struct.
 *
 * @return Returns negative error code or the interpolation frequency.
*******************************************************************************/
static uint32_t ad9122_get_interpol_freq(struct cf_axi_converter *conv)
{
	return ad9122_get_data_clk(conv) * conv->interp_factor;
}

/***************************************************************************//**
 * @brief Returns the center frequency shift.
 *
 * @param conv - Pointer to a cf_axi_converter struct.
 *
 * @return Returns negative error code or the center frequency shift.
*******************************************************************************/
static uint32_t ad9122_get_interpol_fcent_freq(struct cf_axi_converter *conv)
{
	return (ad9122_get_data_clk(conv) * conv->fcenter_shift) / 2;
}

/***************************************************************************//**
 * @brief Writes data to the AD9122.
 *
 * @return Returns negative error code or the written value in case of success.
*******************************************************************************/
uint32_t ad9122_store(uint32_t address, int32_t readin)
{
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

	ret = ad9122_write((uint32_t)address, readin >> 8);
	if (ret < 0)
		goto out;

	ret = ad9122_write((uint32_t)address - 1, readin & 0xFF);
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
uint32_t ad9122_show(uint32_t address, int32_t* val)
{
	int32_t ret = 0;

	ret = ad9122_read((uint32_t)address);
	if (ret < 0)
		goto out;
	*val = ret << 8;

	ret = ad9122_read((uint32_t)address - 1);
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
uint32_t ad9122_interpolation_store(uint32_t address, int32_t readin)
{
	struct cf_axi_converter *conv = &dds_conv;
	int32_t ret;

	switch (address) {
	case 0:
		ret = ad9122_set_interpol_freq(conv, readin);
		break;
	case 1:
		ret = ad9122_set_interpol_fcent_freq(conv, readin);
		break;
	default:
		ret = -1;
	}

	if (conv->pcore_sync)
		conv->pcore_sync();

	return ret ? ret : readin;
}

/***************************************************************************//**
 * @brief Reads interpolation data from the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
uint32_t ad9122_interpolation_show(uint32_t address, int32_t* val)
{
	struct cf_axi_converter *conv = &dds_conv;
	int32_t ret = 0;
	int32_t i;

	switch (address) {
	case 0:
		*val = ad9122_get_interpol_freq(conv);
		break;
	case 1:
		*val = ad9122_get_interpol_fcent_freq(conv);
		break;
	case 2:
		for (i = 0; conv->intp_modes[i] != 0; i++)
			val[i] = (int32_t)conv->intp_modes[i];
		val[i] = 0;
		break;
	case 3:
		for (i = 0; conv->cs_modes[i] != -1; i++)
			val[i] = (int32_t)conv->cs_modes[i];
		val[i] = -1;
	default:
		ret = -1;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_reset(void)
{
	int32_t ret;

    ret = ad9122_write(AD9122_REG_COMM, AD9122_COMM_RESET);
    if(ret < 0)
        return ret;

	ret = ad9122_write(AD9122_REG_COMM, 0x00);

    return ret;
}

/***************************************************************************//**
 * @brief Reads parameters from the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_read_raw(uint32_t channel,
		    	   	   	int32_t *val,
			   	   	   	int32_t *val2,
			   	   	   	int32_t m)
{
	struct cf_axi_converter *conv = &dds_conv;

	switch (m) {
	case IIO_CHAN_INFO_SAMP_FREQ:

		*val = ad9122_get_data_clk(conv);
		return 0;

	}
	return -1;
}

/***************************************************************************//**
 * @brief Writes parameters to the AD9122.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
 int32_t ad9122_write_raw(uint32_t channel,
						  int32_t val,
						  int32_t val2,
						  int32_t mask)
{
	struct cf_axi_converter *conv = &dds_conv;
	uint32_t rate;
	int32_t ret;
	uint32_t ctrl_reg_1;
	uint32_t ctrl_reg_2;

	DAC_Core_Read(ADI_REG_CNTRL_1, &ctrl_reg_1);
	DAC_Core_Read(ADI_REG_CNTRL_2, &ctrl_reg_2);
	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		rate = ad9122_get_data_clk(conv);
		ret = ad9122_set_data_clk(conv, val);
		if (ret < 0) {
			return ret;
		}

		if (val != rate) {
			ret = ad9122_tune_dci(conv);
		}
		break;
	default:
		return -1;
	}
	DAC_Core_Write(ADI_REG_CNTRL_1, ctrl_reg_1);
	DAC_Core_Write(ADI_REG_CNTRL_2, ctrl_reg_2);

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the AD9122.
 *
 * @param pfnSetDataClock - Pointer to a function which sets the data clock
 * @param pfnSetDacClock - Pointer to a function which sets the DAC clock
 * @param pfnRoundRateDataClock - Pointer to a function which computes the
 * 								  actual data clock for a desired clock value
 * @param pfnRoundRateDacClock - Pointer to a function which computes the
 * 								 actual DAC clock for a desired clock value
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_setup(void* pfnSetDataClock,
                     void* pfnSetDacClock,
                     void* pfnSetRefClock,
                     void* pfnRoundRateDataClock,
                     void* pfnRoundRateDacClock,
                     void* pfnRoundRateRefClock)
{
	int32_t ret;
	int32_t i;
	uint32_t datapath_ctrl, rate;
	struct cf_axi_converter *conv = &dds_conv;

	if(ad9122_reset() < 0)
		return -1;

	pfnSetDataClk = pfnSetDataClock;
	pfnSetDacClk  = pfnSetDacClock;
	pfnSetRefClk  = pfnSetRefClock;
	pfnRoundRateDataClk = pfnRoundRateDataClock;
	pfnRoundRateDacClk  = pfnRoundRateDacClock;
	pfnRoundRateRefClk  = pfnRoundRateRefClock;

	conv->write 		= ad9122_write;
	conv->read 			= ad9122_read;
	conv->setup = ad9122_tune_dci;
	conv->get_fifo_status = ad9122_get_fifo_status;
	conv->get_data_clk 	= ad9122_get_data_clk;
	conv->write_raw = ad9122_write_raw;
	conv->read_raw = ad9122_read_raw;

	for (i = 0; i < ARRAY_SIZE(ad9122_reg_defaults); i++)
	{
		ad9122_write(ad9122_reg_defaults[i][0],
					 ad9122_reg_defaults[i][1]);
	}

	if(ad9122_sync() < 0)
		return -1;

	conv->interp_factor = 1;
	conv->interp_factor = ad9122_validate_interp_factor(conv->interp_factor);

	conv->fcenter_shift = 0;

	datapath_ctrl = AD9122_DATAPATH_CTRL_BYPASS_PREMOD |
					AD9122_DATAPATH_CTRL_BYPASS_NCO |
					AD9122_DATAPATH_CTRL_BYPASS_INV_SINC;
	ad9122_write(AD9122_REG_DATAPATH_CTRL, datapath_ctrl);

	rate = 245760000;
	ret = ad9122_set_interpol(conv, conv->interp_factor,
			  	  	  	  	  conv->fcenter_shift, rate);

	cf_axi_dds_of_probe();

	return ret;
}

/***************************************************************************//**
 * @brief Calibrates the AD9122 DCI.
 *
 * @return Returns negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad9122_dci_calibrate()
{
	return ad9122_tune_dci(&dds_conv);
}

/***************************************************************************//**
 * @brief Sets the AD9122 data rate.
 *
 * @param rate - Desired data rate in Hz
 *
 * @return Returns the set data rate.
*******************************************************************************/
int32_t ad9122_set_data_rate(uint32_t rate)
{
	struct cf_axi_converter* conv = &dds_conv;
	int32_t ret = 0;

#ifdef CF_AXI_DDS
	ret = cf_axi_dds_write_raw(0, 0,
							  (int32_t)rate, 0,
							  IIO_CHAN_INFO_SAMP_FREQ);
#else
	ret = ad9122_write_raw(0,
						   (int32_t)rate, 0,
					   	   IIO_CHAN_INFO_SAMP_FREQ);
#endif

	if(ret < 0)
		return ret;

	return ad9122_get_data_clk(conv);
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the I DAC.
 *
 * @param val - Phase adjustment value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_phase(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_I_PHA_ADJ_MSB, val);
	}

	ret = ad9122_show(AD9122_REG_I_PHA_ADJ_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the Q DAC.
 *
 * @param val - Phase adjustment value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_phase(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_Q_PHA_ADJ_MSB, val);
	}

	ret = ad9122_show(AD9122_REG_Q_PHA_ADJ_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration bias of the I DAC.
 *
 * @param val - Calibration bias value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_calibbias(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_I_DAC_OFFSET_MSB, val);
	}

	ret = ad9122_show(AD9122_REG_I_DAC_OFFSET_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration bias of the Q DAC.
 *
 * @param val - Calibration bias value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_calibbias(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_Q_DAC_OFFSET_MSB, val);
	}

	ret = ad9122_show(AD9122_REG_Q_DAC_OFFSET_MSB, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration scale of the I DAC.
 *
 * @param val - Calibration scale value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage0_calibscale(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_I_DAC_CTRL, val);
	}

	ret = ad9122_show(AD9122_REG_I_DAC_CTRL, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the calibration scale of the Q DAC.
 *
 * @param val - Calibration scale value. If the value equals INT32_MAX then
 *              the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_voltage1_calibscale(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_store(AD9122_REG_Q_DAC_CTRL, val);
	}

	ret = ad9122_show(AD9122_REG_Q_DAC_CTRL, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Sets the interpolation frequency.
 *
 * @param val - Interpolation frequency in Hz. If the value equals INT32_MAX then
 *             the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_altvoltage_interpolation_frequency(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_interpolation_store(0, val);
	}

	ret = ad9122_interpolation_show(0, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Stores the available interpolation frequencies in the array
 * 		  supplied as parameter.
 *
 * @param val_array - Array to store the return values.
 * 					  The size of the array must be 5, the last valid value
 * 					  in the array is followed by a 0.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t out_altvoltage_interpolation_frequency_available(int32_t* val_array)
{
	return ad9122_interpolation_show(2, val_array);
}

/***************************************************************************//**
 * @brief Sets the center frequency shift value.
 *
 * @param val - Center frequency shift value in Hz. If the value equals INT32_MAX then
 *             the function returns the current set value.
 *
 * @return Returns the set value or negative error code.
*******************************************************************************/
int32_t ad9122_out_altvoltage_interpolation_center_shift_frequency(int32_t val)
{
	int32_t ret;

	if(val != INT32_MAX)
	{
		return ad9122_interpolation_store(1, val);
	}

	ret = ad9122_interpolation_show(1, &val);

	return ret > 0 ? val : ret;
}

/***************************************************************************//**
 * @brief Stores the available center shift frequencies in the array
 * 		  supplied as parameter.
 *
 * @param val_array - Array to store the return values.
 * 					  The size of the array must be 17, the last valid value
 * 					  in the array is followed by a -1.
 *
 * @return Returns 0 for success or negative error code.
*******************************************************************************/
int32_t out_altvoltage_interpolation_center_shift_frequency_available(int32_t* val_array)
{
	return ad9122_interpolation_show(3, val_array);
}

/***************************************************************************//**
 * @brief Sets the full-scale current for I DAC.
 *
 * @param fs_adj - Full scale current value. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set full-scale current.
*******************************************************************************/
int32_t ad9122_fs_adj_I_DAC(int32_t fs_adj)
{
	uint8_t sleepBit	= 0;
	uint8_t regData1	= 0;
    uint8_t regData2	= 0;

	if(fs_adj != INT32_MAX)
	{
		/* Read the current state of the sleep bit */
		sleepBit = (ad9122_read(AD9122_REG_I_DAC_CTRL) &
					AD9122_I_DAC_CTRL_I_DAC_SLEEP);

        /* Set the full-scale value and keep the state of the sleep bit. */
		regData1 = AD9122_I_DAC_CTRL_I_DAC_FS_ADJ_9_8((fs_adj & 0x0300) >> 8);
		ad9122_write(AD9122_REG_I_DAC_CTRL, (sleepBit | regData1));
		regData2 = AD9122_I_DAC_FS_ADJ_I_DAC_FS_ADJ_7_0((fs_adj & 0x00FF) >> 0);
		ad9122_write(AD9122_REG_I_DAC_FS_ADJ, regData2);

        /* Compute the set full scale current */
        fs_adj = ((regData1 & 0x3) << 8) + (regData2 << 0);
	}
    else
    {
	    fs_adj = ((ad9122_read(AD9122_REG_I_DAC_CTRL) & 0x3) << 8) +
			     (ad9122_read(AD9122_REG_I_DAC_FS_ADJ) << 0);
    }

	return fs_adj;
}

/***************************************************************************//**
 * @brief Sets the full-scale current for Q DAC.
 *
 * @param fs_adj - Full scale current value. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set full-scale current.
*******************************************************************************/
int32_t ad9122_fs_adj_Q_DAC(int32_t fs_adj)
{
	uint8_t sleepBit	= 0;
	uint8_t regData1	= 0;
    uint8_t regData2	= 0;

	if(fs_adj != INT32_MAX)
	{
		/* Read the current state of the sleep bit */
		sleepBit = (ad9122_read(AD9122_REG_Q_DAC_CTRL) &
					AD9122_Q_DAC_CTRL_Q_DAC_SLEEP);

		/* Set the full-scale value and keep the state of the sleep bit. */
		regData1 = AD9122_Q_DAC_CTRL_Q_DAC_FS_ADJ_9_8((fs_adj & 0x0300) >> 8);
		ad9122_write(AD9122_REG_Q_DAC_CTRL, (sleepBit | regData1));
		regData2 = AD9122_Q_DAC_FS_ADJ_Q_DAC_FS_ADJ_7_0((fs_adj & 0x00FF) >> 0);
		ad9122_write(AD9122_REG_Q_DAC_FS_ADJ, regData2);

        /* Compute the set full scale current */
        fs_adj = ((regData1 & 0x3) << 8) + (regData2 << 0);
	}
    else
    {
	    fs_adj = ((ad9122_read(AD9122_REG_Q_DAC_CTRL) & 0x3) << 8) +
			     (ad9122_read(AD9122_REG_Q_DAC_FS_ADJ) << 0);
    }

	return fs_adj;
}

/***************************************************************************//**
 * @brief Sets the offset of the I DAC.
 *
 * @param offset - The value that is added directly to the samples written
 *                 to the I DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set offset.
*******************************************************************************/
int32_t ad9122_offset_I_DAC(int32_t offset)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;

	if(offset != INT32_MAX)
	{
		regData1 = (offset & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_I_DAC_OFFSET_LSB, regData1);
		regData2 = (offset & 0xFF00) >> 8;
		ad9122_write(AD9122_REG_I_DAC_OFFSET_MSB, regData2);
	}
    else
    {
	    offset = (ad9122_read(AD9122_REG_I_DAC_OFFSET_MSB) << 8) +
			     (ad9122_read(AD9122_REG_I_DAC_OFFSET_LSB) << 0);
    }

	return offset;
}

/***************************************************************************//**
 * @brief Sets the offset of the Q DAC.
 *
 * @param offset - The value that is added directly to the samples written
 *                 to the Q DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set offset.
*******************************************************************************/
int32_t ad9122_offset_Q_DAC(int32_t offset)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;

	if(offset != INT32_MAX)
	{
		regData1 = (offset & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_Q_DAC_OFFSET_LSB, regData1);
		regData2 = (offset & 0xFF00) >> 8;
		ad9122_write(AD9122_REG_Q_DAC_OFFSET_MSB, regData2);
	}
    else
    {
	    offset = (ad9122_read(AD9122_REG_Q_DAC_OFFSET_MSB) << 8) +
			     (ad9122_read(AD9122_REG_Q_DAC_OFFSET_LSB) << 0);
    }

	return offset;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the I DAC.
 *
 * @param offset - The value that is added directly to the samples written
 *                 to the I DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set phase adjustment.
*******************************************************************************/
int32_t ad9122_phaseAdj_I_DAC(int32_t phaseAdj)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;

	if(phaseAdj != INT32_MAX)
	{
		regData1 = (phaseAdj & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_I_PHA_ADJ_LSB, regData1);
		regData2 = (phaseAdj & 0x0300) >> 8;
		ad9122_write(AD9122_REG_I_PHA_ADJ_MSB, regData2);
        phaseAdj &= 0x03FF;
	}
    else
    {
	    phaseAdj = ((ad9122_read(AD9122_REG_I_PHA_ADJ_MSB) & 0x03) << 8) +
			       (ad9122_read(AD9122_REG_I_PHA_ADJ_LSB) << 0);
    }

	return phaseAdj;
}

/***************************************************************************//**
 * @brief Sets the phase adjustment of the Q DAC.
 *
 * @param offset - The value that is added directly to the samples written
 *                 to the Q DAC. If the value equals INT32_MAX then
 *                 the function returns the current set value.
 *
 * @return Returns the set phase adjustment.
*******************************************************************************/
int32_t ad9122_phaseAdj_Q_DAC(int32_t phaseAdj)
{
	uint8_t regData1 = 0;
    uint8_t regData2 = 0;

	if(phaseAdj != INT32_MAX)
	{
		regData1 = (phaseAdj & 0x00FF) >> 0;
		ad9122_write(AD9122_REG_Q_PHA_ADJ_LSB, regData1);
		regData2 = (phaseAdj & 0x0300) >> 8;
		ad9122_write(AD9122_REG_Q_PHA_ADJ_MSB, regData2);
        phaseAdj &= 0x03FF;
	}
    else
    {
	    phaseAdj = ((ad9122_read(AD9122_REG_Q_PHA_ADJ_MSB) & 0x03) << 8) +
			       (ad9122_read(AD9122_REG_Q_PHA_ADJ_LSB) << 0);
    }

	return phaseAdj;
}

/***************************************************************************//**
 * @brief Gets the AD9122 FIFO status registers.
 *
 * @param status1 - AD9122_REG_FIFO_STATUS_1 value.
 * @param status2 - AD9122_REG_FIFO_STATUS_2 value.
 *
 * @return Returns negative error code in case of error
 * 		   or 0 in case of success.
*******************************************************************************/
int32_t ad9122_get_fifo_status_regs(uint8_t *status1,
									uint8_t *status2)
{
	int32_t ret = 0;

	ret = ad9122_read(AD9122_REG_FIFO_STATUS_1);
	if (ret < 0)
	{
		return ret;
	}
	else
	{
		*status1 = (uint8_t)ret;
	}
	ret = ad9122_read(AD9122_REG_FIFO_STATUS_2);
	if (ret < 0)
	{
		return ret;
	}
	else
	{
		*status2 = (uint8_t)ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Sets the input data format.
 *
 * @param format - The input data format.
 * 					Example: 0 - input data is in twos complement format.
 * 						     1 - input data is in binary format.
 *
 * @return Returns negative error code in case of error or the set
 *         input data format.
*******************************************************************************/
int32_t ad9122_set_data_format(uint8_t format)
{
	int32_t ret       = 0;
	uint8_t reg_value = 0;

	/* Read the current state of the AD9122_REG_DATA_FORMAT*/
	ret = ad9122_read(AD9122_REG_DATA_FORMAT);
	if(ret < 0)
	{
		return -1;
	}
	reg_value = (uint8_t)ret;

	if((format == 0) || (format == 1))
	{
		reg_value &= ~AD9122_DATA_FORMAT_BINARY;
		reg_value |= AD9122_DATA_FORMAT_BINARY * format;
		ret = ad9122_write(AD9122_REG_DATA_FORMAT, reg_value);
		if(ret < 0)
		{
			return -1;
		}
	}
	else
	{
		format = (reg_value & AD9122_DATA_FORMAT_BINARY) >> 7;
	}

	return format;
}

/***************************************************************************//**
 * @brief Gets the AD9122 die temperature code.
 *
 * @return Returns negative error code in case of error
 * 		   or the die temperature code.
*******************************************************************************/
int32_t ad9122_get_temperature_code(void)
{
	int32_t ret;
	int32_t tmp_code;

	ret = ad9122_read(AD9122_REG_DIE_TEMP_LSB);
	if (ret < 0)
		return ret;
	tmp_code = ret;
	ret = ad9122_read(AD9122_REG_DIE_TEMP_MSB);
	if (ret < 0)
		return ret;
	tmp_code |= (ret << 8);

	return tmp_code;
}
