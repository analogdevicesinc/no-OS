/***************************************************************************//**
 *   @file   axi_adc_core.c
 *   @brief  Driver for the Analog Devices AXI-ADC-CORE module.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include <inttypes.h>
#include "platform_drivers.h"
#ifdef ALTERA_PLATFORM
#include "io.h"
#else
#include "xil_io.h"
#endif
#include "util.h"
#include "axi_adc_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_ADC_REG_RSTN		0x0040
#define AXI_ADC_MMCM_RSTN		BIT(1)
#define AXI_ADC_RSTN			BIT(0)

#define AXI_ADC_REG_CLK_FREQ	0x0054
#define AXI_ADC_CLK_FREQ(x)		(((x) & 0xFFFFFFFF) << 0)
#define AXI_ADC_TO_CLK_FREQ(x)	(((x) >> 0) & 0xFFFFFFFF)

#define AXI_ADC_REG_CLK_RATIO	0x0058
#define AXI_ADC_CLK_RATIO(x)	(((x) & 0xFFFFFFFF) << 0)
#define AXI_ADC_TO_CLK_RATIO(x)	(((x) >> 0) & 0xFFFFFFFF)

#define AXI_ADC_REG_STATUS		0x005C
#define AXI_ADC_MUX_PN_ERR		BIT(3)
#define AXI_ADC_MUX_PN_OOS		BIT(2)
#define AXI_ADC_MUX_OVER_RANGE	BIT(1)
#define AXI_ADC_STATUS			BIT(0)

#define AXI_ADC_REG_CHAN_CNTRL(c)	(0x0400 + (c) * 0x40)
#define AXI_ADC_PN_SEL				BIT(10)
#define AXI_ADC_IQCOR_ENB			BIT(9)
#define AXI_ADC_DCFILT_ENB			BIT(8)
#define AXI_ADC_FORMAT_SIGNEXT		BIT(6)
#define AXI_ADC_FORMAT_TYPE			BIT(5)
#define AXI_ADC_FORMAT_ENABLE		BIT(4)
#define AXI_ADC_PN23_TYPE			BIT(1)
#define AXI_ADC_ENABLE				BIT(0)

#define AXI_ADC_REG_CHAN_CNTRL_1(c)	(0x0410 + (c) * 0x40)
#define AXI_ADC_DCFILT_OFFSET(x)	(((x) & 0xFFFF) << 16)
#define AXI_ADC_TO_DCFILT_OFFSET(x)	(((x) >> 16) & 0xFFFF)
#define AXI_ADC_DCFILT_COEFF(x)		(((x) & 0xFFFF) << 0)
#define AXI_ADC_TO_DCFILT_COEFF(x)	(((x) >> 0) & 0xFFFF)

#define AXI_ADC_REG_CHAN_CNTRL_2(c)	(0x0414 + (c) * 0x40)
#define AXI_ADC_IQCOR_COEFF_1(x)	(((x) & 0xFFFF) << 16)
#define AXI_ADC_TO_IQCOR_COEFF_1(x)	(((x) >> 16) & 0xFFFF)
#define AXI_ADC_IQCOR_COEFF_2(x)	(((x) & 0xFFFF) << 0)
#define AXI_ADC_TO_IQCOR_COEFF_2(x)	(((x) >> 0) & 0xFFFF)

#define AXI_ADC_REG_CHAN_CNTRL_3(c)	(0x0418 + (c) * 0x40)
#define AXI_ADC_ADC_PN_SEL(x)		(((x) & 0xF) << 16)
#define AXI_ADC_TO_ADC_PN_SEL(x)	(((x) >> 16) & 0xF)
#define AXI_ADC_ADC_DATA_SEL(x)		(((x) & 0xF) << 0)
#define AXI_ADC_TO_ADC_DATA_SEL(x)	(((x) >> 0) & 0xF)

#define AXI_ADC_REG_DELAY(l)		(0x0800 + (l) * 0x4)

/***************************************************************************//**
 * @brief axi_adc_read
 *******************************************************************************/
int32_t axi_adc_read(struct axi_adc *adc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
#ifdef ALTERA_PLATFORM
	*reg_data = IORD_32DIRECT(adc->base, reg_addr);
#else
	*reg_data = Xil_In32((adc->base + reg_addr));
#endif

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_write
 *******************************************************************************/
int32_t axi_adc_write(struct axi_adc *adc,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
#ifdef ALTERA_PLATFORM
	IOWR_32DIRECT(adc->base, reg_addr, reg_data);
#else
	Xil_Out32((adc->base + reg_addr), reg_data);
#endif

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_set_pnsel
*******************************************************************************/
int32_t axi_adc_set_pnsel(struct axi_adc *adc,
			  uint32_t chan,
			  enum axi_adc_pn_sel sel)
{
	uint32_t reg_data;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), &reg_data);
	reg_data &= ~AXI_ADC_ADC_PN_SEL(~0);
	reg_data |= AXI_ADC_ADC_PN_SEL(sel);
	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), reg_data);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_idelay_set
*******************************************************************************/
void axi_adc_idelay_set(struct axi_adc *adc,
			uint32_t lane,
			uint32_t val)
{
	axi_adc_write(adc, AXI_ADC_REG_DELAY(lane), val);

}

/***************************************************************************//**
 * @brief axi_adc_get_sampling_freq
*******************************************************************************/
int32_t axi_adc_get_sampling_freq(struct axi_adc *adc,
				uint32_t chan,
				uint64_t *sampling_freq)
{
	uint32_t freq;
	uint32_t ratio;

	axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
	axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);
	*sampling_freq = freq * ratio;
	*sampling_freq = ((*sampling_freq) * 390625) >> 8;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_set_calib_phase_scale
*******************************************************************************/
int32_t axi_adc_set_calib_phase_scale(struct axi_adc *adc,
				      uint32_t phase,
				      uint32_t chan,
				      int32_t val,
				      int32_t val2)
{
	uint32_t fract;
	uint64_t llval;
	uint32_t tmp;

	switch (val) {
	case 1:
		fract = 0x4000;
		break;
	case -1:
		fract = 0xC000;
		break;
	case 0:
		fract = 0;
		if (val2 < 0) {
			fract = 0x8000;
			val2 *= -1;
		}
		break;
	default:
		return FAILURE;
	}

	llval = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
	do_div(&llval, 1000000UL);
	fract |= llval;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	if (!((chan + phase) % 2)) {
		tmp &= ~AXI_ADC_IQCOR_COEFF_1(~0);
		tmp |= AXI_ADC_IQCOR_COEFF_1(fract);
	} else {
		tmp &= ~AXI_ADC_IQCOR_COEFF_2(~0);
		tmp |= AXI_ADC_IQCOR_COEFF_2(fract);
	}

	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), tmp);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_get_calib_scale_phase
*******************************************************************************/
int32_t axi_adc_get_calib_phase_scale(struct axi_adc *adc,
				      uint32_t phase,
				      uint32_t chan,
				      int32_t *val,
				      int32_t *val2)
{
	uint32_t tmp;
	int32_t sign;
	uint64_t llval;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	if (!((phase + chan) % 2)) {
		tmp = AXI_ADC_TO_IQCOR_COEFF_1(tmp);
	} else {
		tmp = AXI_ADC_TO_IQCOR_COEFF_2(tmp);
	}

	if (tmp & 0x8000)
		sign = -1;
	else
		sign = 1;

	if (tmp & 0x4000)
		*val = 1 * sign;
	else
		*val = 0;

	tmp &= ~0xC000;

	llval = tmp * 1000000ULL + (0x4000 / 2);
	do_div(&llval, 0x4000);
	if (*val == 0)
		*val2 = llval * sign;
	else
		*val2 = llval;

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_set_calib_phase
*******************************************************************************/
int32_t axi_adc_set_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2)
{
	return axi_adc_set_calib_phase_scale(adc, 1, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_get_calib_phase
*******************************************************************************/
int32_t axi_adc_get_calib_phase(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2)
{
	return axi_adc_get_calib_phase_scale(adc, 1, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_set_calib_scale
*******************************************************************************/
int32_t axi_adc_set_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t val,
				int32_t val2)
{
	return axi_adc_set_calib_phase_scale(adc, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_get_calib_scale
*******************************************************************************/
int32_t axi_adc_get_calib_scale(struct axi_adc *adc,
				uint32_t chan,
				int32_t *val,
				int32_t *val2)
{
	return axi_adc_get_calib_phase_scale(adc, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief axi_adc_set_calib_bias
*******************************************************************************/
int32_t axi_adc_set_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t val,
			       int32_t val2)
{
	uint32_t tmp;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);
	tmp &= ~AXI_ADC_DCFILT_OFFSET(~0);
	tmp |= AXI_ADC_DCFILT_OFFSET((uint16_t)val);

	axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), tmp);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief adc_get_calib_scale
*******************************************************************************/
int32_t axi_adc_get_calib_bias(struct axi_adc *adc,
			       uint32_t chan,
			       int32_t *val,
			       int32_t *val2)
{
	uint32_t tmp;

	axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);
	*val = (uint16_t)AXI_ADC_TO_DCFILT_OFFSET(tmp);

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_init
 *******************************************************************************/
int32_t axi_adc_init(struct axi_adc **adc_core,
		     const struct axi_adc_init *init)
{
	struct axi_adc *adc;
	uint32_t reg_data;
	uint32_t freq;
	uint32_t ratio;
	uint8_t ch;

	adc = (struct axi_adc *)malloc(sizeof(*adc));
	if (!adc)
		return FAILURE;

	adc->name = init->name;
	adc->base = init->base;
	adc->num_channels = init->num_channels;

	axi_adc_write(adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	for (ch = 0; ch < adc->num_channels; ch++)
		axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE);

	mdelay(100);

	axi_adc_read(adc, AXI_ADC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		printf("%s: Status errors\n", adc->name);
		goto error;
	}

	axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
	axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);
	adc->clock_hz = freq * ratio;
	adc->clock_hz = (adc->clock_hz * 390625) >> 8;

	printf("%s: Successfully initialized (%"PRIu64" Hz)\n",
	       adc->name, adc->clock_hz);

	*adc_core = adc;

	return SUCCESS;
error:
	free(adc);

	return FAILURE;
}

/***************************************************************************//**
 * @brief axi_adc_remove
 *******************************************************************************/
int32_t axi_adc_remove(struct axi_adc *adc)
{
	free(adc);

	return SUCCESS;
}
