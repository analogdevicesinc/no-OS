/***************************************************************************//**
 * @file dac_buffer.c
 * @brief Implementation of DAC LUT driver.
 * @author Istvan Csomortani (istvan.csomortani@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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

#include "adc_buffer.h"

int32_t adc_ramp_test(struct dmac_xfer *xfer,
		uint32_t no_of_channels,
		uint32_t resolution)
{
	uint8_t	 err_cnt = 0;
	uint16_t exp_data[32];
	uint16_t rcv_data[32];
	uint8_t index;
	uint32_t mask = ad_pow2(resolution);
	uint32_t current_address = xfer->start_address;
	uint32_t last_address = xfer->start_address + xfer->no_of_samples*2;
	uint32_t sample_count = 0;

	while (current_address < last_address) {

		// read data back from memory, one samples from each channel, min a word
		for (index=0; index<no_of_channels; index+=2) {
			rcv_data[index] = ad_reg_read(current_address + index) & mask;
			rcv_data[index+1] = (ad_reg_read(current_address + index) >> 16) & mask;
		}

		// generate expected data
		for (index=0; index<no_of_channels; index+=2) {
			if (current_address == xfer->start_address) {
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
