/***************************************************************************//**
 * @file adc_core.c
 * @brief Implementation of ADC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include <stdio.h>
#include "adc_core.h"

/***************************************************************************//**
 * @brief adc_read
 *******************************************************************************/
int32_t adc_read(adc_core *core,
				 uint32_t reg_addr,
				 uint32_t *reg_data)
{
	*reg_data = Xil_In32((core->base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
 * @brief adc_write
 *******************************************************************************/
int32_t adc_write(adc_core *core,
				  uint32_t reg_addr,
				  uint32_t reg_data)
{
	Xil_Out32((core->base_address + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief adc_dma_read
*******************************************************************************/
void adc_dma_read(uint32_t regAddr,
				  uint32_t *data)
{
	*data = Xil_In32(XPAR_AXI_AD7405_DMA_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief adc_dma_write
*******************************************************************************/
void adc_dma_write(uint32_t regAddr,
				   uint32_t data)
{
	Xil_Out32(XPAR_AXI_AD7405_DMA_BASEADDR + regAddr, data);
}

/***************************************************************************//**
 * @brief adc_setup
 *******************************************************************************/
int32_t adc_setup(adc_core *core)
{
	uint8_t	 index;
	uint32_t reg_data;
	uint32_t adc_clock;

	adc_read(core, ADC_REG_ID, &reg_data);
	if (reg_data)
		core->master = 1;
	else
		core->master = 0;

	adc_write(core, ADC_REG_RSTN, 0);
	adc_write(core, ADC_REG_RSTN, ADC_MMCM_RSTN | ADC_RSTN);

	for(index = 0; index < core->no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_CNTRL(index), ADC_FORMAT_SIGNEXT |
							   ADC_FORMAT_ENABLE |
							   ADC_ENABLE);
	}

	mdelay(100);

	adc_read(core, ADC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		printf("%s adc core Status errors.\n", __func__);
		return -1;
	}

	adc_read(core, ADC_REG_CLK_FREQ, &adc_clock);
	adc_read(core, ADC_REG_CLK_RATIO, &reg_data);
	adc_clock = (adc_clock * reg_data * 100) + 0x7fff;
	adc_clock = adc_clock >> 16;

	printf("%s adc core initialized (%"PRIu32" MHz).\n", __func__, adc_clock);

	return 0;
}

/***************************************************************************//**
 * @brief adc_set_pnsel
 *******************************************************************************/
int32_t adc_set_pnsel(adc_core *core,
					  uint8_t channel,
					  enum adc_pn_sel sel)
{
	uint32_t reg;

	adc_read(core, ADC_REG_CHAN_CNTRL_3(channel), &reg);
	reg &= ~ADC_ADC_PN_SEL(~0);
	reg |= ADC_ADC_PN_SEL(sel);
	adc_write(core, ADC_REG_CHAN_CNTRL_3(channel), reg);

	return 0;
}

/***************************************************************************//**
 * @brief adc_pn_mon
 *	  Note: The device must be in PRBS test mode, when calling this function
 *******************************************************************************/
int32_t adc_pn_mon(adc_core *core,
				   enum adc_pn_sel sel)
{
	uint8_t	index;
	uint32_t reg_data;
	int32_t pn_errors = 0;

	for (index = 0; index < core->no_of_channels; index++) {
 		adc_read(core, ADC_REG_CHAN_CNTRL(index), &reg_data);
 		reg_data |= ADC_ENABLE;
 		adc_write(core, ADC_REG_CHAN_CNTRL(index), reg_data);
		adc_set_pnsel(core, index, sel);
	}
	mdelay(1);

	for (index = 0; index < core->no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_STATUS(index), 0xff);
	}
	mdelay(100);

	for (index = 0; index < core->no_of_channels; index++) {
		adc_read(core, ADC_REG_CHAN_STATUS(index), &reg_data);
		if (reg_data != 0) {
			pn_errors = -1;
			printf("%s ERROR: adc pn OUT OF SYNC: %d, %d, 0x%02"PRIx32"!\n", __func__, index, sel, reg_data);
		}
	}

	return pn_errors;
}

/***************************************************************************//**
 * @brief adc_ramp_test
 *	  This functions supports channel number multiple of 2 (e.g 1/2/4/6/8...)
 *******************************************************************************/
int32_t adc_ramp_test(adc_core *core,
					  uint8_t no_of_cores,
					  uint32_t no_of_samples,
					  uint32_t start_address)
{
	uint8_t	 err_cnt = 0;
	uint16_t exp_data[32];
	uint16_t rcv_data[32];
	uint8_t index;
	uint32_t mask = ad_pow2(core->resolution);
	uint8_t no_of_channels = core->no_of_channels*no_of_cores;
	uint32_t current_address = start_address;
	uint32_t last_address = start_address + (no_of_channels*no_of_samples)*2;
	uint32_t sample_count = 0;

	while (current_address < last_address) {

		// read data back from memory, one samples from each channel, min a word
		for (index=0; index<no_of_channels; index+=2) {
			rcv_data[index] = Xil_In32(current_address + index) & mask;
			rcv_data[index+1] = (Xil_In32(current_address + index) >> 16) & mask;
		}

		// generate expected data
		for (index=0; index<no_of_channels; index+=2) {
			if (current_address == start_address) {
				exp_data[index] = rcv_data[index];
				exp_data[index+1] = rcv_data[index+1];
			} else {
				if(no_of_channels < 2) {
					exp_data[index] = ((exp_data[index]+2) > mask) ? 0 : (exp_data[index] + 2);
					exp_data[index+1] = ((exp_data[index+1]+2) > mask) ? 1 : (exp_data[index+1] + 2);
				} else {
					exp_data[index] = (exp_data[index] == mask) ? 0 : exp_data[index] + 1;
					exp_data[index+1] = (exp_data[index+1] == mask) ? 0 : exp_data[index+1] + 1;
				}
			}
		}

		// compare received and expected
		for (index=0; index<no_of_channels; index+=2) {
			if ((rcv_data[index] != exp_data[index]) || (rcv_data[index+1] != exp_data[index+1])) {
				printf("%s Capture Error[%"PRIu32"]: rcv(%08x) exp(%08x).\n",
						__func__, sample_count+index, rcv_data[index], exp_data[index]);
				printf("%s Capture Error[%"PRIu32"]: rcv(%08x) exp(%08x).\n",
						__func__, sample_count+index+1, rcv_data[index+1], exp_data[index+1]);
				err_cnt++;
				if (err_cnt == 50) break;
			}
		}
		sample_count+=index;

		// increment address pointer
		current_address = (no_of_channels == 1) ? (current_address+4) :
							(current_address+(no_of_channels*2));
	}

	if (err_cnt)
		return -1;
	else
		return 0;
}

/***************************************************************************//**
 * @brief adc_capture
*******************************************************************************/
int32_t adc_capture(adc_core *core,
		uint32_t size)
{
	uint32_t length;

	length = (size * sizeof(uint32_t));

	adc_dma_write(AXI_DMAC_REG_CTRL, 0x0);
	adc_dma_write(AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
	adc_dma_write(AXI_DMAC_REG_DEST_ADDRESS, core->start_address);
	adc_dma_write(AXI_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dma_write(AXI_DMAC_REG_X_LENGTH, length - 1);
	adc_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);
	adc_dma_write(AXI_DMAC_REG_FLAGS, 0x01);
	adc_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);

	return 0;
}
