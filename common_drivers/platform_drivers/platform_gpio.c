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

#include "platform_gpio.h"

int32_t ad_gpio_set(uint8_t pin, uint32_t data) {

	return(ad_gpio_set_range(pin, 1, data));
}

int32_t ad_gpio_get(uint8_t pin, uint32_t *data) {

	return(ad_gpio_get_range(pin, 1, data));
}

int32_t ad_gpio_set_range(uint8_t start_pin, uint8_t num_pins, uint32_t data) {

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (start_pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = start_pin - 32;
	pmask = ((1 << num_pins) - 1) << ppos;

#ifdef NIOS_II
	pdata = ad_reg_read(SYS_GPIO_OUT_BASE + 0x0);
	ad_reg_write((SYS_GPIO_OUT_BASE + 0x0), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;
#endif
#ifdef ZYNQ_PS7
	pdata = ad_reg_read(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	ad_reg_write((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata | pmask));
	pdata = ad_reg_read(XPAR_PS7_GPIO_0_BASEADDR + 0x02c8);
	ad_reg_write((XPAR_PS7_GPIO_0_BASEADDR + 0x02c8), (pdata | pmask));
	pdata = ad_reg_read(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	ad_reg_write((XPAR_PS7_GPIO_0_BASEADDR + 0x004c), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;
#endif
#ifdef ZYNQ_PSU
	pdata = ad_reg_read(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	ad_reg_write((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata | pmask));
	pdata = ad_reg_read(XPAR_PSU_GPIO_0_BASEADDR + 0x0308);
	ad_reg_write((XPAR_PSU_GPIO_0_BASEADDR + 0x0308), (pdata | pmask));
	pdata = ad_reg_read(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	ad_reg_write((XPAR_PSU_GPIO_0_BASEADDR + 0x0050), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;
#endif
#ifdef MICROBLAZE
	pdata = ad_reg_read(XPAR_AXI_GPIO_BASEADDR + 0xc);
	ad_reg_write((XPAR_AXI_GPIO_BASEADDR + 0xc), (pdata & ~pmask));
	pdata = ad_reg_read(XPAR_AXI_GPIO_BASEADDR + 0x8);
	ad_reg_write((XPAR_AXI_GPIO_BASEADDR + 0x8), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;
#endif

	return(pstatus);
}

int32_t ad_gpio_get_range(uint8_t start_pin, uint8_t num_pins, uint32_t *data) {

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (start_pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = start_pin - 32;
	pmask = ((1 << num_pins) - 1) << ppos;

#ifdef NIOS_II
	pdata = ad_reg_read(SYS_GPIO_OUT_BASE + 0x0);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;
#endif
#ifdef ZYNQ_PS7
	pdata = ad_reg_read(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	ad_reg_write((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata & ~pmask));
	pdata = ad_reg_read(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;
#endif
#ifdef ZYNQ_PSU
	pdata = ad_reg_read(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	ad_reg_write((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata & ~pmask));
	pdata = ad_reg_read(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;
#endif
#ifdef MICROBLAZE
	pdata = ad_reg_read(XPAR_AXI_GPIO_BASEADDR + 0x8);
	*data = (pdata & pmask) >> ppos;
	pstatus = 0;
#endif

	return(pstatus);
}

// ***************************************************************************
// ***************************************************************************
