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
#include "dac_buffer.h"

/******************************************************************************/
/******************************* Constant definitions *************************/
/******************************************************************************/

/* Sine Look Up Tables */
const uint16_t sine_lut_1[128] = {
	0x0000, 0x0640, 0x0C80, 0x12C0, 0x18F0, 0x1F10, 0x2520, 0x2B10,
	0x30F0, 0x36B0, 0x3C50, 0x41C0, 0x4710, 0x4C30, 0x5120, 0x55F0,
	0x5A70, 0x5ED0, 0x62E0, 0x66C0, 0x6A60, 0x6DC0, 0x70D0, 0x73A0,
	0x7630, 0x7870, 0x7A70, 0x7C20, 0x7D80, 0x7E90, 0x7F50, 0x7FD0,
	0x7FF0, 0x7FD0, 0x7F50, 0x7E90, 0x7D80, 0x7C20, 0x7A70, 0x7870,
	0x7630, 0x73A0, 0x70D0, 0x6DC0, 0x6A60, 0x66C0, 0x62E0, 0x5ED0,
	0x5A70, 0x55F0, 0x5120, 0x4C30, 0x4710, 0x41C0, 0x3C50, 0x36B0,
	0x30F0, 0x2B10, 0x2520, 0x1F10, 0x18F0, 0x12C0, 0x0C80, 0x0640,
	0x0000, 0xF9B0, 0xF370, 0xED30, 0xE700, 0xE0E0, 0xDAD0, 0xD4E0,
	0xCF00, 0xC940, 0xC3A0, 0xBE30, 0xB8E0, 0xB3C0, 0xAED0, 0xAA00,
	0xA580, 0xA120, 0x9D10, 0x9930, 0x9590, 0x9230, 0x8F20, 0x8C50,
	0x89C0, 0x8780, 0x8580, 0x83D0, 0x8270, 0x8160, 0x80A0, 0x8020,
	0x8000, 0x8020, 0x80A0, 0x8160, 0x8270, 0x83D0, 0x8580, 0x8780,
	0x89C0, 0x8C50, 0x8F20, 0x9230, 0x9590, 0x9930, 0x9D10, 0xA120,
	0xA580, 0xAA00, 0xAED0, 0xB3C0, 0xB8E0, 0xBE30, 0xC3A0, 0xC940,
	0xCF00, 0xD4E0, 0xDAD0, 0xE0E0, 0xE700, 0xED30, 0xF370, 0xF9B0
};

/* 90 degree offset from sine_lut_1 */
const uint16_t sine_lut_2[128] = {
	0x7FF0, 0x7FD0, 0x7F50, 0x7E90, 0x7D80, 0x7C20, 0x7A70, 0x7870,
	0x7630, 0x73A0, 0x70D0, 0x6DC0, 0x6A60, 0x66C0, 0x62E0, 0x5ED0,
	0x5A70, 0x55F0, 0x5120, 0x4C30, 0x4710, 0x41C0, 0x3C50, 0x36B0,
	0x30F0, 0x2B10, 0x2520, 0x1F10, 0x18F0, 0x12C0, 0x0C80, 0x0640,
	0x0000, 0xF9B0, 0xF370, 0xED30, 0xE700, 0xE0E0, 0xDAD0, 0xD4E0,
	0xCF00, 0xC940, 0xC3A0, 0xBE30, 0xB8E0, 0xB3C0, 0xAED0, 0xAA00,
	0xA580, 0xA120, 0x9D10, 0x9930, 0x9590, 0x9230, 0x8F20, 0x8C50,
	0x89C0, 0x8780, 0x8580, 0x83D0, 0x8270, 0x8160, 0x80A0, 0x8020,
	0x8000, 0x8020, 0x80A0, 0x8160, 0x8270, 0x83D0, 0x8580, 0x8780,
	0x89C0, 0x8C50, 0x8F20, 0x9230, 0x9590, 0x9930, 0x9D10, 0xA120,
	0xA580, 0xAA00, 0xAED0, 0xB3C0, 0xB8E0, 0xBE30, 0xC3A0, 0xC940,
	0xCF00, 0xD4E0, 0xDAD0, 0xE0E0, 0xE700, 0xED30, 0xF370, 0xF9B0,
	0x0000, 0x0640, 0x0C80, 0x12C0, 0x18F0, 0x1F10, 0x2520, 0x2B10,
	0x30F0, 0x36B0, 0x3C50, 0x41C0, 0x4710, 0x4C30, 0x5120, 0x55F0,
	0x5A70, 0x5ED0, 0x62E0, 0x66C0, 0x6A60, 0x6DC0, 0x70D0, 0x73A0,
	0x7630, 0x7870, 0x7A70, 0x7C20, 0x7D80, 0x7E90, 0x7F50, 0x7FD0
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

uint32_t dac_buffer_load(dac_core core, uint32_t start_address) {

	uint32_t no_of_samples;
	uint32_t index;
	uint32_t index_mem = 0;

	no_of_samples = sizeof(sine_lut_1) / sizeof(typeof(sine_lut_1[0]));

	for (index = 0; index < no_of_samples; index ++) {
		switch (core.no_of_channels) {
			case 1:
				ad_reg_write_16(start_address + index_mem * 2, sine_lut_1[index]);
				index_mem += 1;
				break;
			case 2:
				ad_reg_write_16(start_address + (index_mem + 0) * 2, sine_lut_1[index]);
				ad_reg_write_16(start_address + (index_mem + 1) * 2, sine_lut_2[index]);
				index_mem += 2;
				break;
			case 4:
				ad_reg_write_16(start_address + (index_mem + 0) * 2, sine_lut_1[index]);
				ad_reg_write_16(start_address + (index_mem + 1) * 2, sine_lut_2[index]);
				ad_reg_write_16(start_address + (index_mem + 2) * 2, sine_lut_1[index]);
				ad_reg_write_16(start_address + (index_mem + 3) * 2, sine_lut_2[index]);
				index_mem += 4;
				break;
			default:
				ad_printf("Unsupported mode.\n\r");
				return -1;
		}
	}

	ad_dcache_flush();

	return no_of_samples;
}

