/**************************************************************************//**
*   @file   test.c
*   @brief  Test functions implementations.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011-2015(c) Analog Devices, Inc.
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
#include "dac_core.h"

/*****************************************************************************/
/***************************** External Functions ****************************/
/*****************************************************************************/
extern int32_t ad9122_write(uint8_t registerAddress, uint8_t registerValue);
extern int32_t ad9122_read(uint8_t registerAddress);
extern void xil_printf(const char *ctrl1, ...);

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
const uint16_t sine_lut_q[32] = {
	32768, 34366, 35902, 37318, 38560, 39578, 40335, 40802,
	40959, 40802, 40335, 39578, 38560, 37318, 35902, 34366,
	32768, 31169, 29633, 28217, 26975, 25957, 25200, 24733,
	24576, 24733, 25200, 25957, 26975, 28217, 29633, 31169,
};

const uint16_t sine_lut_i[32] = {
	24576, 24733, 25200, 25957, 26975, 28217, 29633, 31169,
	32768, 34366, 35902, 37318, 38560, 39578, 40335, 40802,
	40959, 40802, 40335, 39578, 38560, 37318, 35902, 34366,
	32768, 31169, 29633, 28217, 26975, 25957, 25200, 24733,
};

static uint32_t dac_base_addr;

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
#ifdef XPAR_AXI_TIMER_0_BASEADDR
	TIMER0_WAIT(XPAR_AXI_TIMER_0_BASEADDR, ms_count*1000000);
#else
	TIMER0_WAIT(XPAR_AXI_TIMER_BASEADDR, ms_count*1000000);
#endif
#endif
}

/***************************************************************************//**
* @brief dac_read
*******************************************************************************/
void dac_read(uint32_t regAddr, uint32_t *data)
{
	*data = Xil_In32(dac_base_addr + 0x4000 + regAddr);
}

/***************************************************************************//**
* @brief dac_write
*******************************************************************************/
void dac_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(dac_base_addr + 0x4000 + regAddr, data);
}

/***************************************************************************//**
* @brief do_div
*******************************************************************************/
#define do_div(n,base) ({								\
		uint64_t __res;									\
		__res = ((uint64_t) (n)) % (uint64_t) (base);	\
		n = ((uint64_t) (n)) / (uint64_t) (base);		\
		__res;})

/***************************************************************************//**
* @brief dds_set_frequency
*******************************************************************************/
void dds_set_frequency(uint32_t chan, uint32_t freq)
{
	uint32_t pcore_version;
	uint64_t val64;
	uint32_t ctrl_reg, reg;
	uint32_t dac_clk;
	uint32_t val;

	dac_read(ADI_REG_VERSION, &pcore_version);
	dac_read(ADI_REG_CLK_FREQ, &val);
	dac_clk = val;
	dac_read(ADI_REG_CLK_RATIO, &val);
	dac_clk *= val * 100000000 / 65536;

	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INCR(~0);
	val64 = (u64) freq * 0xFFFFULL;
	do_div(val64, dac_clk);
	reg |= ADI_DDS_INCR(val64) | 1;
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	if (PCORE_VERSION_MAJOR(pcore_version) > 7)
	{
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_1, ctrl_reg);
	}
}

/***************************************************************************//**
* @brief dds_set_phase
*******************************************************************************/
void dds_set_phase(uint32_t chan, uint32_t phase)
{
	uint32_t pcore_version;
	uint64_t val64;
	uint32_t ctrl_reg, reg;

	dac_read(ADI_REG_VERSION, &pcore_version);
	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_read(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), &reg);
	reg &= ~ADI_DDS_INIT(~0);
	val64 = (u64) phase * 0x10000ULL + (360000 / 2);
	do_div(val64, 360000);
	reg |= ADI_DDS_INIT(val64);
	dac_write(ADI_REG_CHAN_CNTRL_2_IIOCHAN(chan), reg);
	if (PCORE_VERSION_MAJOR(pcore_version) > 7)
	{
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_1, ctrl_reg);
	}
}

/***************************************************************************//**
* @brief dds_set_scale
*******************************************************************************/
void dds_set_scale(uint32_t chan, double scale)
{
	uint32_t pcore_version;
	uint32_t ctrl_reg;
	uint32_t scale_reg;
	uint32_t sign_part;
	uint32_t int_part;
	uint32_t fract_part;

	dac_read(ADI_REG_VERSION, &pcore_version);
	if (PCORE_VERSION_MAJOR(pcore_version) > 6)
	{
		if(scale >= 1.0)
		{
			sign_part = 0;
			int_part = 1;
			fract_part = 0;
			goto set_scale_reg;
		}
		if(scale <= -1.0)
		{
			sign_part = 1;
			int_part = 1;
			fract_part = 0;
			goto set_scale_reg;
		}
		if(scale < 0)
		{
			sign_part = 1;
			int_part = 0;
			scale *= -1;
			goto set_scale_reg;
		}
		sign_part = 0;
		int_part = 0;
		fract_part = (uint32_t)(scale * 0x4000);
	set_scale_reg:
		scale_reg = (sign_part << 15) | (int_part << 14) | fract_part;
	}
	else
	{
		if(scale >= 1.0)
		{
			scale_reg = 0;
			scale = 1.0;
		}
		if(scale <= 0.0)
		{
			scale_reg = 0;
			scale = 0.0;
		}
		fract_part = (uint32_t)(scale * 1000000);
		scale_reg = 500000 / fract_part;
	}
	dac_read(ADI_REG_CNTRL_1, &ctrl_reg);
	dac_write(ADI_REG_CNTRL_1, 0);
	dac_write(ADI_REG_CHAN_CNTRL_1_IIOCHAN(chan), ADI_DDS_SCALE(scale_reg));
	if (PCORE_VERSION_MAJOR(pcore_version) > 7)
	{
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_1, ctrl_reg);
	}
}

/**************************************************************************//**
* @brief Initializes the DDS core.
*
* @return None.
******************************************************************************/
void dds_setup(uint32_t sel, uint32_t f1, uint32_t f2)
{
	uint32_t hdl_version;

	dac_base_addr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ?
					CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	dac_read(ADI_REG_VERSION, &hdl_version);

	if (PCORE_VERSION_MAJOR(hdl_version) > 7)
	{
		dac_write(ADI_REG_CHAN_CNTRL_7(0), 0);
		dac_write(ADI_REG_CHAN_CNTRL_7(1), 0);
		dac_write(ADI_REG_CNTRL_1, 0x0);
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_2, ADI_DATA_FORMAT | ADI_DATA_SEL(DATA_SEL_DDS));
		dac_write(ADI_REG_CNTRL_1, 0x0);
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}

	dds_set_frequency(0, f1);
	dds_set_phase(0, 90000);
	dds_set_scale(0, 0.125);
	dds_set_frequency(1, f1);
	dds_set_phase(1, 90000);
	dds_set_scale(1, 0.125);
	dds_set_frequency(2, f2);
	dds_set_phase(2, 0);
	dds_set_scale(2, 0.125);
	dds_set_frequency(3, f2);
	dds_set_phase(3, 0);
	dds_set_scale(3, 0.125);
}

/**************************************************************************//**
* @brief Initializes the DAC DMA.
*
* @return None.
******************************************************************************/
void dac_dma_setup(uint32_t sel)
{
	uint32_t baddr;
	uint32_t index;
	uint32_t tx_count;
	uint32_t data_i;
	uint32_t data_q;
	uint32_t dac_clk;
	uint32_t val;
	uint32_t sine_freq;
	uint32_t hdl_version;

	dac_base_addr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ?
					CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	dac_read(ADI_REG_VERSION, &hdl_version);

	tx_count = sizeof(sine_lut_i) / sizeof(uint16_t);
	for(index = 0; index < tx_count; index ++)
	{
		data_i = (sine_lut_i[index] << 16);
		data_q = (sine_lut_q[index] << 0);
		Xil_Out32(DDRDAC_BASEADDR + index * 4, data_i | data_q);
	}
#ifdef _XPARAMETERS_PS_H_
	Xil_DCacheFlush();
#else
	microblaze_flush_dcache();
	microblaze_invalidate_dcache();
#endif

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? DMA9122_1_BASEADDR : DMA9122_0_BASEADDR;

	Xil_Out32(baddr + AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
	Xil_Out32(baddr + AXI_DMAC_REG_SRC_ADDRESS, DDRDAC_BASEADDR);
	Xil_Out32(baddr + AXI_DMAC_REG_SRC_STRIDE, 0);
	Xil_Out32(baddr + AXI_DMAC_REG_X_LENGTH, (tx_count * 4) - 1);
	Xil_Out32(baddr + AXI_DMAC_REG_Y_LENGTH, 0x0);
	Xil_Out32(baddr + AXI_DMAC_REG_START_TRANSFER, 0x1);

	dac_read(ADI_REG_CLK_FREQ, &val);
	dac_clk = val;
	dac_read(ADI_REG_CLK_RATIO, &val);
	dac_clk *= val * 100000000 / 65536;
	sine_freq = dac_clk / (tx_count * 2);
	xil_printf("dac_dma: Sine frequency is %d Hz.\n\r", sine_freq);

	if (PCORE_VERSION_MAJOR(hdl_version) > 7)
	{
		dac_write(ADI_REG_CHAN_CNTRL_7(0), 2);
		dac_write(ADI_REG_CHAN_CNTRL_7(1), 2);
		dac_write(ADI_REG_CNTRL_1, 0x0);
		dac_write(ADI_REG_CNTRL_1, 0x1);
	}
	else
	{
		dac_write(ADI_REG_CNTRL_2, ADI_DATA_FORMAT | ADI_DATA_SEL(DATA_SEL_DMA));
		dac_write(ADI_REG_CNTRL_1, 0x0);
		dac_write(ADI_REG_CNTRL_1, 0x1);
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
	uint32_t hdl_version;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9122_1_BASEADDR : CFAD9122_0_BASEADDR;

	hdl_version = Xil_In32(baddr + 0x4000);

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
	if (PCORE_VERSION_MAJOR(hdl_version) > 7)
	{
		Xil_Out32((baddr + 0x4418), 0x1); // sed sel
		Xil_Out32((baddr + 0x4458), 0x1); // sed sel
		Xil_Out32((baddr + 0x4044), 0x1); // sync
	}
	else
	{
		Xil_Out32((baddr + 0x4048), 0x1); // format, sel
		Xil_Out32((baddr + 0x4044), 0x1); // enable
	}
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
void adc_capture(uint32_t sel, uint32_t size, uint32_t start_address)
{
	uint32_t baddr;
	uint32_t reg_val;
	uint32_t transfer_id;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? DMA9643_1_BASEADDR : DMA9643_0_BASEADDR;

	Xil_Out32(baddr + AXI_DMAC_REG_CTRL, 0x0);
	Xil_Out32(baddr + AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);

	Xil_Out32(baddr + AXI_DMAC_REG_IRQ_MASK, 0x0);

	transfer_id = Xil_In32(baddr + AXI_DMAC_REG_TRANSFER_ID);
	reg_val = Xil_In32(baddr + AXI_DMAC_REG_IRQ_PENDING);
	Xil_Out32(baddr + AXI_DMAC_REG_IRQ_PENDING, reg_val);

	Xil_Out32(baddr + AXI_DMAC_REG_DEST_ADDRESS, start_address);
	Xil_Out32(baddr + AXI_DMAC_REG_DEST_STRIDE, 0x0);
	Xil_Out32(baddr + AXI_DMAC_REG_X_LENGTH, (size * 8) - 1);
	Xil_Out32(baddr + AXI_DMAC_REG_Y_LENGTH, 0x0);

	Xil_Out32(baddr + AXI_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		reg_val = Xil_In32(baddr + AXI_DMAC_REG_START_TRANSFER);
	}
	while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		reg_val = Xil_In32(baddr + AXI_DMAC_REG_IRQ_PENDING);
	}
	while(reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT));
	Xil_Out32(baddr + AXI_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		reg_val = Xil_In32(baddr + AXI_DMAC_REG_TRANSFER_DONE);
	}
	while((reg_val & (1 << transfer_id)) != (1 << transfer_id));
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
	uint32_t pcore_version;

	baddr = ((sel == IICSEL_B1HPC_AXI)||(sel == IICSEL_B1HPC_PS7)) ? CFAD9643_1_BASEADDR : CFAD9643_0_BASEADDR;

	pcore_version = Xil_In32(baddr + 0x0000);

	Xil_Out32((baddr + 0x040), 0x3);
	delay_ms(10);
	if (Xil_In32(baddr + 0x05c) == 0x0)
	{
		xil_printf("adc_test: status NOT set!!\n\r");
	}

	if (PCORE_VERSION_MAJOR(pcore_version) > 8) {
		if ((Xil_In32(baddr + 0x800) & 0x200) == 0x1)
		{
			xil_printf("adc_test: delay NOT locked!!\n\r");
		}
	} else {
		if ((Xil_In32(baddr + 0x064) & 0x200) == 0x0)
		{
			xil_printf("adc_test: delay NOT locked!!\n\r");
		}
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
