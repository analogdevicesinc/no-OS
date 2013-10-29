/**************************************************************************//**
*   @file   test.c
*   @brief  Test functions implementations.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include <xparameters.h>
#include <xil_io.h>
#include <xil_cache.h>
#include "timer.h"
#include "test.h"

/*****************************************************************************/
/***************************** External Functions ****************************/
/*****************************************************************************/
extern int32_t ad9122_write(uint8_t registerAddress, uint8_t registerValue);
extern int32_t ad9122_read(uint8_t registerAddress);
extern void xil_printf(const char *ctrl1, ...);

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
const uint16_t sine_lut[32] = {
	0x8000, 0x98F8, 0xB0FB, 0xC71C, 0xDA82, 0xEA6D, 0xF641, 0xFD89,
	0xFFFF, 0xFD89, 0xF641, 0xEA6D, 0xDA82, 0xC71C, 0xB0FB, 0x98F8,
	0x8000, 0x6707, 0x4F04, 0x38E3, 0x257D, 0x1592, 0x09BE, 0x0276,
	0x0000, 0x0276, 0x09BE, 0x1592, 0x257D, 0x38E3, 0x4F04, 0x6707
};

/**************************************************************************//**
* @brief Delays the program execution with the specified number of ms.
*
* @param ns_count Number of ms with which the program must be delayed.
* @return None.
******************************************************************************/
void delay_ms(uint32_t ms_count)
{
#ifdef _XPARAMETERS_PS_H_
	volatile uint32_t i;
	for(i = 0; i < ms_count*1000; i++);
#else
	TIMER0_WAIT(XPAR_AXI_TIMER_0_BASEADDR, ms_count*1000000);
#endif
}

/**************************************************************************//**
* @brief Computes the DDS increment and offset.
*
* @return DDS increment and offset.
******************************************************************************/
uint32_t dds_pf(uint32_t phase, uint32_t sin_clk, uint32_t dac_clk)
{
	uint32_t p_offset;
	uint32_t p_incr;

	p_offset = (phase*0xffff)/360;
	p_incr = ((sin_clk*0xffff)/dac_clk) | 0x1;

	return((p_offset<<16) | p_incr);
}

/**************************************************************************//**
* @brief Initializes the DDS core.
*
* @return None.
******************************************************************************/
void dds_setup(uint32_t sel, uint32_t f1, uint32_t f2)
{
	uint32_t baddr;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
	Xil_Out32((baddr + 0x4400), 0x1); // scale-1
	Xil_Out32((baddr + 0x4404), dds_pf( 0, f1, 500)); //dds-1
	Xil_Out32((baddr + 0x4408), 0x1); // scale-2
	Xil_Out32((baddr + 0x440c), dds_pf( 0, f1, 500)); //dds-1
	Xil_Out32((baddr + 0x4440), 0x1); // scale-1
	Xil_Out32((baddr + 0x4444), dds_pf(90, f2, 500)); //dds-1
	Xil_Out32((baddr + 0x4448), 0x1); // scale-2
	Xil_Out32((baddr + 0x444c), dds_pf(90, f2, 500)); //dds-2
	Xil_Out32((baddr + 0x4048), 0x10); // format, sel
	Xil_Out32((baddr + 0x4044), 0x0); // enable
	Xil_Out32((baddr + 0x4044), 0x1); // enable
	Xil_Out32((baddr + 0x4050), 0x0); // enable
	Xil_Out32((baddr + 0x4050), 0x1); // enable
	xil_printf("dac_dds: f1(%dMHz), f2(%dMHz).\n\r", f1, f2);
}

/**************************************************************************//**
* @brief Initializes the DAC DMA.
*
* @return None.
******************************************************************************/
void dac_dma_setup(uint32_t sel)
{
	uint32_t dac_baseaddr;
	uint32_t vdma_baseaddr;
	uint32_t index;
	uint32_t status;
	uint32_t tx_count;
	uint32_t index_i1;
	uint32_t index_q1;
	uint32_t index_i2;
	uint32_t index_q2;
	uint32_t i1_data;
	uint32_t q1_data;
	uint32_t i2_data;
	uint32_t q2_data;

	dac_baseaddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
	vdma_baseaddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? VDMA9122_1_BASEADDR : VDMA9122_0_BASEADDR;

	tx_count = sizeof(sine_lut) / sizeof(uint16_t);
	for(index = 0; index < (tx_count * 2); index += 2)
	{
		index_i1 = index;
		index_q1 = index_i1 + ((tx_count * 2) / 4);
		if(index_q1 >= (tx_count * 2))
			index_q1 -= (tx_count * 2);
		i1_data = ((sine_lut[index_i1 / 2]) & 0xFFFF);
		q1_data = ((sine_lut[index_q1 / 2]) & 0xFFFF);
		Xil_Out32((DDRDAC_BASEADDR + (2*index)), ((i1_data << 16) | q1_data));
		index_i2 = index_i1 + (tx_count / 2);
		index_q2 = index_q1 + (tx_count / 2);
		if(index_i2 >= (tx_count * 2))
			index_i2 -= (tx_count * 2);
		if(index_q2 >= (tx_count * 2))
			index_q2 -= (tx_count * 2);
		i2_data = ((sine_lut[index_i2 / 2]) & 0xFFFF);
		q2_data = ((sine_lut[index_q2 / 2]) & 0xFFFF);
		Xil_Out32((DDRDAC_BASEADDR + (2*(index + 1))), ((i2_data << 16) | q2_data));
	}

#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif
	xil_printf("dac_dma: buffer-count(%d).\n\r", index);
	Xil_Out32((dac_baseaddr + 0x4084), tx_count * 4); // vdma frame count
	Xil_Out32((dac_baseaddr + 0x4048), 0x2); // format, sel
	Xil_Out32((dac_baseaddr + 0x4044), 0x1); // enable
	Xil_Out32((vdma_baseaddr + 0x000), 0x4); // reset
	Xil_Out32((vdma_baseaddr + 0x000), 0x0); // reset
	Xil_Out32((vdma_baseaddr + 0x000), 0x3); // enable circular mode
	Xil_Out32((vdma_baseaddr + 0x018), 0x1);
	Xil_Out32((vdma_baseaddr + 0x05c), DDRDAC_BASEADDR); // start address
	Xil_Out32((vdma_baseaddr + 0x058), tx_count * 4);
	Xil_Out32((vdma_baseaddr + 0x054), tx_count * 4);
	Xil_Out32((vdma_baseaddr + 0x050), 1);
	delay_ms(10);
	Xil_Out32((dac_baseaddr + 0x4088), 0x3); // clear status
	xil_printf("dac_dma: f(60MHz).\n\r");
	delay_ms(10);
	status = Xil_In32((dac_baseaddr + 0x4088));
	if (status != 0x0)
	{
		xil_printf("dma_setup: status(%x)\n\r", status);
	}
}

/**************************************************************************//**
* @brief Puts the DAC in SED mode and verifies the correctness of the samples
*
* @return None.
******************************************************************************/
void dac_sed(uint32_t sel, uint32_t s0, uint32_t s1)
{
	uint32_t baddr;
	uint32_t rdata;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	ad9122_write(0x68, ((s0>> 0) & 0xff));
	ad9122_write(0x69, ((s0>> 8) & 0xff));
	ad9122_write(0x6a, ((s1>> 0) & 0xff));
	ad9122_write(0x6b, ((s1>> 8) & 0xff));
	ad9122_write(0x6c, ((s0>>16) & 0xff));
	ad9122_write(0x6d, ((s0>>24) & 0xff));
	ad9122_write(0x6e, ((s1>>16) & 0xff));
	ad9122_write(0x6f, ((s1>>24) & 0xff));
	ad9122_write(0x67, 0x00);
	ad9122_write(0x67, 0x80);
	Xil_Out32((baddr + 0x4410), s0); // pattern
	Xil_Out32((baddr + 0x4450), s1); // pattern
	Xil_Out32((baddr + 0x4048), 0x1); // format, sel
	Xil_Out32((baddr + 0x4044), 0x1); // enable
	delay_ms(10);
	ad9122_write(0x67, 0xa3);
	ad9122_write(0x07, 0x1c);
	delay_ms(100);

	xil_printf("dac_sed: s0(0x%08x), s1(0x%08x)\n\r", s0, s1);
	rdata = ad9122_read(0x67);
	if ((rdata & 0x20) == 0x20)
	{
		xil_printf("ERROR: Addr(0x67) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x07);
	if ((rdata & 0x04) == 0x04)
	{
		xil_printf("ERROR: Addr(0x07) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x70);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x70) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x71);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x71) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x72);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x72) Data(0x%02x)!\n\r", rdata);
	}
	rdata = ad9122_read(0x73);
	if (rdata != 0x00)
	{
		xil_printf("ERROR: Addr(0x73) Data(0x%02x)!\n\r", rdata);
	}

	return;
}

/**************************************************************************//**
* @brief Tests the communication with the DAC
*
* @return None.
******************************************************************************/
void dac_test(uint32_t sel)
{
	u32 rdata;
	u32 dac_clk_int;
	u32 dac_clk_frac;
	float dac_clk;
	uint32_t baddr;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;
	Xil_Out32((baddr + 0x4040), 0x3); // reset
	delay_ms(10);
	if (Xil_In32(baddr + 0x405c) == 0x0)
	{
		xil_printf("dac_setup: status NOT set!!\n\r");
	}
	rdata = Xil_In32(baddr + 0x4054);
	dac_clk = (float) rdata;
	rdata = Xil_In32(baddr + 0x4058);
	dac_clk = dac_clk * (float) rdata * (float) 100.0;
	dac_clk = dac_clk / (float) 65536.0;
	dac_clk_int = (u32) dac_clk;
	dac_clk_frac = (dac_clk - (float) dac_clk_int) * 1000;
	xil_printf("dac_setup: dac_clock(%d.%3dMHz)\n\r", dac_clk_int, dac_clk_frac);

	ad9122_write(0x16, 0x01);
	ad9122_read(0x16);
	dac_sed(sel, 0x0000aaaa, 0x00000000);
	dac_sed(sel, 0x00005555, 0x00000000);
	dac_sed(sel, 0xaaaa0000, 0x00000000);
	dac_sed(sel, 0x55550000, 0x00000000);
	dac_sed(sel, 0x00000000, 0x0000aaaa);
	dac_sed(sel, 0x00000000, 0x00005555);
	dac_sed(sel, 0x00000000, 0xaaaa0000);
	dac_sed(sel, 0x00000000, 0x55550000);
	dac_sed(sel, 0x00000000, 0x00000000);
	dac_sed(sel, 0xaaaaaaaa, 0x55555555);
	dac_sed(sel, 0x55555555, 0xaaaaaaaa);

	return;
}

/**************************************************************************//**
* @brief Captures data from the ADC
*
* @return None.
******************************************************************************/
void adc_capture(uint32_t sel, uint32_t qwcnt, uint32_t sa)
{
	uint32_t baddr;
	uint32_t ba;

	ba = sa + (qwcnt*8);
	Xil_Out32((ba + 0x000), (ba + 0x40)); // next descriptor
	Xil_Out32((ba + 0x004), 0x00); // reserved
	Xil_Out32((ba + 0x008), sa); // start address
	Xil_Out32((ba + 0x00c), 0x00); // reserved
	Xil_Out32((ba + 0x010), 0x00); // reserved
	Xil_Out32((ba + 0x014), 0x00); // reserved
	Xil_Out32((ba + 0x018), (qwcnt*8)); // no. of bytes
	Xil_Out32((ba + 0x01c), 0x00); // status
	Xil_Out32((ba + 0x040), (ba + 0x00)); // next descriptor
	Xil_Out32((ba + 0x044), 0x00); // reserved
	Xil_Out32((ba + 0x048), sa); // start address
	Xil_Out32((ba + 0x04c), 0x00); // reserved
	Xil_Out32((ba + 0x050), 0x00); // reserved
	Xil_Out32((ba + 0x054), 0x00); // reserved
	Xil_Out32((ba + 0x058), (qwcnt*8)); // no. of bytes
	Xil_Out32((ba + 0x05c), 0x00); // status
#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? DMA9643_1_BASEADDR : DMA9643_0_BASEADDR;
	Xil_Out32((baddr + 0x030), 4); // reset dma
	Xil_Out32((baddr + 0x030), 0); // clear dma operations
	Xil_Out32((baddr + 0x038), ba); // head descr.
	Xil_Out32((baddr + 0x030), 1); // enable dma operations
	Xil_Out32((baddr + 0x040), (ba+0x40)); // tail descr.

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9643_1_BASEADDR : CFAD9643_0_BASEADDR;

	Xil_Out32((baddr + 0x084), (qwcnt*8)); // start capture
	Xil_Out32((baddr + 0x080), 0x0); // capture disable
	Xil_Out32((baddr + 0x080), 0x1); // capture disable
	Xil_Out32((baddr + 0x088), 0xf); // clear status

	do
	{
		delay_ms(1);
	}
	while ((Xil_In32(baddr + 0x088) & 0x1) == 1);
	if (Xil_In32(baddr + 0x088) != 0x00)
	{
		xil_printf("adc_capture: overflow occured, data may be corrupted\n\r");
	}
#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif
}

/**************************************************************************//**
* @brief Tests the communication with the ADC
*
* @return None.
******************************************************************************/
void adc_test(uint32_t sel, uint32_t mode, uint32_t format)
{
	uint32_t n;
	uint32_t rdata;
	uint32_t adata;
	uint32_t bdata;
	uint32_t edata;
	uint32_t baddr;
	uint32_t adc_clk_int;
	uint32_t adc_clk_frac;
	float adc_clk;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9643_1_BASEADDR : CFAD9643_0_BASEADDR;

	Xil_Out32((baddr + 0x040), 0x3);
	delay_ms(10);
	if (Xil_In32(baddr + 0x05c) == 0x0)
	{
		xil_printf("adc_test: status NOT set!!\n\r");
	}
	if ((Xil_In32(baddr + 0x064) & 0x200) == 0x0)
	{
		xil_printf("adc_test: delay NOT locked!!\n\r");
	}
	rdata = Xil_In32(baddr + 0x054);
	adc_clk = (float) rdata;
	rdata = Xil_In32(baddr + 0x058);
	adc_clk = adc_clk * (float) rdata * (float) 100.0;
	adc_clk = adc_clk / (float) 65536.0;
	adc_clk_int = (u32) adc_clk;
	adc_clk_frac = (adc_clk - (float) adc_clk_int) * 1000;
	xil_printf("adc_test: adc_clock(%d.%3dMHz)\n\r", adc_clk_int, adc_clk_frac);

	Xil_Out32((baddr + 0x400), 0x1); // signextn, fmt & ch enable
	Xil_Out32((baddr + 0x440), 0x1); // signextn, fmt & ch enable

	adc_capture(sel, 16, DDR_BASEADDR);
	delay_ms(10);

	xil_printf("adc_test: mode(%2d), format(%2d)\n\r", mode, format);

	/* Alternating Checkerboard */
	if (mode == 0x4)
	{
		for (n = 0; n < 32; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*4));
			if (n == 0)
			{
				adata = ((rdata & 0xffff) == 0x2aaa) ? 0x2aaa : 0x1555;
				bdata = (((rdata>>16) & 0xffff) == 0x2aaa) ? 0x2aaa : 0x1555;
			}
			else
			{
				adata = (adata == 0x2aaa) ? 0x1555 : 0x2aaa;
				bdata = (bdata == 0x2aaa) ? 0x1555 : 0x2aaa;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* PnLongSeq or PnShortSeq */
	if ((mode == 0x5) || (mode == 0x6))
	{
		if (format == 0x1)
			goto end;
		Xil_Out32((baddr + 0x400), ((mode == 0x5) ? 0x2 : 0x0));
		Xil_Out32((baddr + 0x440), ((mode == 0x5) ? 0x2 : 0x0));
		delay_ms(10);
		Xil_Out32((baddr + 0x404), 0xff);
		Xil_Out32((baddr + 0x444), 0xff);
		delay_ms(100);
		if (Xil_In32(baddr + 0x404) != 0)
		{
			xil_printf("  ERROR: PN status(%02x).\n\r", Xil_In32(baddr + 0x404));
		}
		if (Xil_In32(baddr + 0x444) != 0)
		{
	    	xil_printf("  ERROR: PN status(%02x).\n\r", Xil_In32(baddr + 0x444));
		}
		goto end;
	}

	/* One Zero Toggle */
	if (mode == 0x7)
	{
		for (n = 0; n < 32; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*4));
			if (n == 0)
			{
				adata = ((rdata & 0xffff) == 0x3fff) ? 0x3fff : 0x0000;
				bdata = (((rdata>>16) & 0xffff) == 0x3fff) ? 0x3fff : 0x0000;
			}
			else
			{
				adata = (adata == 0x3fff) ? 0x0000 : 0x3fff;
				bdata = (bdata == 0x3fff) ? 0x0000 : 0x3fff;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* Ramp */
	if (mode == 0xf)
	{
		for (n = 0; n < 16; n++)
		{
			rdata = Xil_In32(DDR_BASEADDR+(n*8));
			if (n == 0)
			{
				adata = rdata & 0xffff;
				bdata = (rdata>>16) & 0xffff;
			}
			else
			{
				adata = (adata+1) & 0x3fff;
				bdata = (bdata+1) & 0x3fff;
			}
			edata = (bdata<<16) | adata;
			if (rdata != edata)
			{
				xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
			}
		}
		goto end;
	}

	/* MidscaleShort, PositiveFs, NegativeFs */
	edata = 0xffffffff;
	if ((mode == 0x1) && (format == 0))
		edata = 0x20002000;
	if ((mode == 0x2) && (format == 0))
		edata = 0x3fff3fff;
	if ((mode == 0x3) && (format == 0))
		edata = 0x00000000;
	if ((mode == 0x1) && (format == 1))
		edata = 0x00000000;
	if ((mode == 0x2) && (format == 1))
		edata = 0x1fff1fff;
	if ((mode == 0x3) && (format == 1))
		edata = 0x20002000;
	for (n = 0; n < 32; n++)
	{
		rdata = Xil_In32(DDR_BASEADDR+(n*4));
		if (rdata != edata)
		{
			xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata, edata);
		}
	}
end:
	Xil_Out32((baddr + 0x400), 0x31);
	Xil_Out32((baddr + 0x440), 0x31);
}
