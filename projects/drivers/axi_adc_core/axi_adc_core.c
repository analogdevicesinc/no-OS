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
#include "platform_drivers.h"
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

/***************************************************************************//**
 * @brief axi_adc_read
 *******************************************************************************/
int32_t axi_adc_read(struct axi_adc *adc,
		     uint32_t reg_addr,
		     uint32_t *reg_data)
{
	*reg_data = Xil_In32((adc->base + reg_addr));

	return SUCCESS;
}

/***************************************************************************//**
 * @brief axi_adc_write
 *******************************************************************************/
int32_t axi_adc_write(struct axi_adc *adc,
		      uint32_t reg_addr,
		      uint32_t reg_data)
{
	Xil_Out32((adc->base + reg_addr), reg_data);

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

	printf("%s: Successfully initialized (%lu Hz)\n", adc->name, adc->clock_hz);

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
