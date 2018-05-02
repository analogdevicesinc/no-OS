/***************************************************************************//**
 * @file adc_core.c
 * @brief Implementation of ADC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#include "adc_core.h"

/***************************************************************************//**
 * @brief adc_read
 *******************************************************************************/
int32_t adc_read(adc_core core,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_address + reg_addr));

	if (*reg_data == 0xDEADDEAD) {
		#ifdef DEBUG
			ad_printf("adc_read failed for register: %x", reg_addr);
		#endif
		return -1;
	} else {
		return 0;
	}
}

/***************************************************************************//**
 * @brief adc_write
 *******************************************************************************/
int32_t adc_write(adc_core core,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	ad_reg_write((core.base_address + reg_addr), reg_data);

		#ifdef DEBUG
			uint32_t reg_data_r;
			return adc_read(core, reg_addr, &reg_data_r);
		#endif

	return 0;
}

/***************************************************************************//**
 * @brief adc_setup
 *******************************************************************************/
int32_t adc_setup(adc_core core)
{
	uint8_t	 index;
	uint32_t reg_data;
	uint32_t adc_clock;

	adc_read(core, ADC_REG_ID, &reg_data);
	if (reg_data)
		core.master = 1;
	else
		core.master = 0;

	adc_write(core, ADC_REG_RSTN, 0);
	adc_write(core, ADC_REG_RSTN, ADC_MMCM_RSTN | ADC_RSTN);

	for(index = 0; index < core.no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_CNTRL(index), ADC_FORMAT_SIGNEXT |
							   ADC_FORMAT_ENABLE |
							   ADC_ENABLE);
	}

	mdelay(100);

	adc_read(core, ADC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		ad_printf("%s adc core Status errors.\n", __func__);
		return -1;
	}

	adc_read(core, ADC_REG_CLK_FREQ, &adc_clock);
	adc_read(core, ADC_REG_CLK_RATIO, &reg_data);
	adc_clock = (adc_clock * reg_data * 100) + 0x7fff;
	adc_clock = adc_clock >> 16;

	ad_printf("%s adc core initialized (%d MHz).\n", __func__, adc_clock);

	return 0;
}

/***************************************************************************//**
 * @brief ADC set delay.
 *
 * @param delay - Value of the delay.
 *	Note:
 *		The device must be in PRBS test mode, when calling this function
 *		and the output mode must be two's complement

 * @return 0.
*******************************************************************************/
uint32_t adc_set_delay(adc_core core, uint32_t no_of_lanes, uint32_t delay)
{
    uint32_t i;
    uint32_t rdata;
    uint32_t pcore_version;

    adc_read(core, 0x0, &pcore_version);
    pcore_version >>= 16;
    if (pcore_version < 9) {
			    ad_printf(" pcore_version is : %d\n\r", pcore_version);
			    ad_printf(" DRIVER DOES NOT SUPPORT PCORE VERSIONS OLDER THAN 10 !");
			    return -1;
    } else {
		for (i = 0; i < no_of_lanes; i++) {
			adc_write(core ,((0x200 + i)*4), delay);
			adc_read(core ,((0x200 + i)*4), &rdata);
			if (rdata != delay) {
				ad_printf("adc_delay_1: sel(%2d), rcv(%04x), exp(%04x)\n\r", i, rdata, delay);
			}
		}
    }

    return 0;
}

/***************************************************************************//**
 * @brief ADC delay.
 *
*******************************************************************************/
uint32_t adc_delay_calibrate(adc_core core,
			uint32_t no_of_lanes,
			enum adc_pn_sel sel)
{
	uint8_t err_field[32] = {0};
	uint16_t valid_range[5] = {0};
	uint16_t invalid_range[5] = {0};
	uint16_t delay = 0;
	uint16_t start_valid_delay = 32;
	uint16_t start_invalid_delay = 32;
	uint8_t interval = 0;
	uint8_t max_interval = 0;
	uint8_t max_valid_range = 0;
	uint8_t cnt_valid[5] = {0};
	uint8_t cnt_invalid = 0;
	uint8_t val = 0;
	uint8_t max_val = 32;

	for (delay = 0; delay < 32; delay++) {
		adc_set_delay(core, no_of_lanes, delay);
		mdelay(20);
		if (adc_pn_mon(core, sel) == 0) {
			err_field[delay] = 0;
			start_valid_delay = start_valid_delay == 32 ? delay : start_valid_delay;
		} else {
			err_field[delay] = 1;
		}
	}
	if (start_valid_delay > 31) {
		ad_printf("%s FAILED.\n", __func__);
		adc_set_delay(core, no_of_lanes, 0);
		return(1);
	}

	start_valid_delay = 32;
	start_invalid_delay = 32;
	for (val = 0; val < max_val; val++) {
		if (err_field[val] == 0) {
			if (start_valid_delay == 32) {
				start_valid_delay = val;
			}
			if (start_valid_delay != 32 && start_invalid_delay != 32 ) {
				start_valid_delay = 32;
				start_invalid_delay = 32;
			}
			cnt_valid[interval]++;
		}
		if((err_field[val] == 1) || (val == max_val - 1)) {
			if (start_invalid_delay == 32) {
				start_invalid_delay = val;
			}
			if (start_valid_delay != 32 && start_invalid_delay != 32 ) {
				valid_range[interval] = start_valid_delay;
				invalid_range[interval] = start_invalid_delay;
				start_valid_delay = 32;
				start_invalid_delay = 32;
				interval++;
			}
			cnt_invalid++;
		}
	}

	for (val = 0; val < 5; val++) {
		if (cnt_valid[val] > max_valid_range) {
			max_valid_range = cnt_valid[val];
			max_interval = val;
		}
	}

	delay = (valid_range[max_interval] + invalid_range[max_interval] - 1) / 2;

	ad_printf("adc_delay: setting zero error delay (%d)\n\r", delay);
	adc_set_delay(core, no_of_lanes, delay);

#ifdef DEBUG
	ad_printf("Error field (0=success, 1=fail):\n");
	for (delay = 0; delay < 32; delay++) {
		ad_printf("%d",err_field[delay]);
	}
#endif

    return(0);
}

/***************************************************************************//**
 * @brief adc_set_pnsel
 *	  Note: The device must be in PRBS test mode, when calling this function
 *******************************************************************************/
int32_t adc_set_pnsel(adc_core core,
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
int32_t adc_pn_mon(adc_core core,
		enum adc_pn_sel sel)
{
	uint8_t	index;
	uint32_t reg_data;
	int32_t pn_errors = 0;

	for (index = 0; index < core.no_of_channels; index++) {
 		adc_read(core, ADC_REG_CHAN_CNTRL(index), &reg_data);
 		reg_data |= ADC_ENABLE;
 		adc_write(core, ADC_REG_CHAN_CNTRL(index), reg_data);
		adc_set_pnsel(core, index, sel);
	}
	mdelay(1);

	for (index = 0; index < core.no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_STATUS(index), 0xff);
	}
	mdelay(100);

	for (index = 0; index < core.no_of_channels; index++) {
		adc_read(core, ADC_REG_CHAN_STATUS(index), &reg_data);
		if (reg_data != 0) {
			pn_errors = -1;
		}
	}

	return pn_errors;
}

/***************************************************************************//**
 * @brief adc_ramp_test
 *	  This functions supports channel number multiple of 2 (e.g 1/2/4/6/8...)
 *******************************************************************************/
int32_t adc_ramp_test(adc_core core,
		uint8_t no_of_cores,
		uint32_t no_of_samples,
		uint32_t start_address)
{
	uint8_t	 err_cnt = 0;
	uint16_t exp_data[32];
	uint16_t rcv_data[32];
	uint8_t index;
	uint32_t mask = ad_pow2(core.resolution);
	uint8_t no_of_channels = core.no_of_channels*no_of_cores;
	uint32_t current_address = start_address;
	uint32_t last_address = start_address + (no_of_channels*no_of_samples)*2;
	uint32_t sample_count = 0;

	while (current_address < last_address) {

		// read data back from memory, one samples from each channel, min a word
		for (index=0; index<no_of_channels; index+=2) {
			rcv_data[index] = ad_reg_read(current_address + index) & mask;
			rcv_data[index+1] = (ad_reg_read(current_address + index) >> 16) & mask;
		}

		// generate expected data
		for (index=0; index<no_of_channels; index+=2) {
			if (current_address == start_address) {
				exp_data[index] = rcv_data[index];
				exp_data[index+1] = rcv_data[index+1];
			} else {
				if(no_of_channels < 2) {
					exp_data[index] = ((exp_data[index]+2) > mask) ? ((exp_data[index]+2) & mask) : (exp_data[index] + 2);
					exp_data[index+1] = ((exp_data[index+1]+2) > mask) ? ((exp_data[index+1]+2) & mask) : (exp_data[index+1] + 2);
				} else {
					exp_data[index] = (exp_data[index] == mask) ? 0 : exp_data[index] + 1;
					exp_data[index+1] = (exp_data[index+1] == mask) ? 0 : exp_data[index+1] + 1;
				}
			}
		}

		// compare received and expected
		for (index=0; index<no_of_channels; index+=2) {
			if ((rcv_data[index] != exp_data[index]) || (rcv_data[index+1] != exp_data[index+1])) {
				ad_printf("%s Capture Error[%d]: rcv(%08x) exp(%08x).\n",
						__func__, sample_count+index, rcv_data[index], exp_data[index]);
				ad_printf("%s Capture Error[%d]: rcv(%08x) exp(%08x).\n",
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
