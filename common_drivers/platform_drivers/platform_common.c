// ***************************************************************************
// ***************************************************************************
// Copyright 2014 - 2017 (c) Analog Devices, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in
// the documentation and/or other materials provided with the
// distribution.
// - Neither the name of Analog Devices, Inc. nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
// - The use of this software may or may not infringe the patent rights
// of one or more patent holders. This license does not release you
// from the requirement that you obtain separate licenses from these
// patent holders to use this software.
// - Use of the software either in source or binary form, must be run
// on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#include "platform_common.h"

// ***************************************************************************
// ***************************************************************************
/* altera */

#ifdef ALTERA

void ad_platform_init(void) {

	return;
}

void ad_platform_close(void) {

	alt_printf("%c", 0x04);
	return;
}

#endif

// ***************************************************************************
// ***************************************************************************
/* xilinx */

#ifdef XILINX
#ifdef MICROBLAZE

void usleep(uint32_t us_count) {

	uint32_t count;

	ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x4), 0x00);
	for (count = 0; count < us_count; count++) {
		ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x20);
		ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x80);
		while (ad_reg_read(XPAR_AXI_TIMER_BASEADDR + 0x8) < 100) {}
	}
	ad_reg_write((XPAR_AXI_TIMER_BASEADDR + 0x0), 0x20);
}

#endif
#endif

// ***************************************************************************
// ***************************************************************************
/* common */

void ad_reg_write_16(uint32_t addr, uint32_t data) {

	uint32_t m_data;

	m_data = ad_reg_read(addr & ~0x3);
	if ((addr & 0x3) == 0)
		m_data = (m_data & ~0xffff) | data;
	else
		m_data = (m_data & 0xffff) | (data<<16);
	ad_reg_write((addr & ~0x3), m_data);
}

uint64_t do_div(uint64_t* n, uint64_t base) {

	uint64_t mod = 0;

	mod = *n % base;
	*n = *n / base;

	return mod;
}

uint32_t ad_pow2(uint32_t number) {

	uint32_t index;
	uint32_t mask = 1;

	for (index=1; index < number; index++) {
		mask = (mask << 1) ^ 1;
	}

	return mask;
}

// ***************************************************************************
// ***************************************************************************
