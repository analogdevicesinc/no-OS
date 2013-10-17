/***************************************************************************//**
 *   @file   adc_core.c
 *   @brief  Implementation of ADC Core Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#include "stdint.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xaxidma_hw.h"
#include "sleep.h"
#include "parameters.h"
#include "adc_core.h"

/***************************************************************************//**
 * @brief adc_read
*******************************************************************************/
void adc_read(uint32_t regAddr, uint32_t *data)
{
    *data = Xil_In32(CF_AD9361_RX_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief adc_write
*******************************************************************************/
void adc_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(CF_AD9361_RX_BASEADDR + regAddr, data);
}

/***************************************************************************//**
 * @brief dma_write
*******************************************************************************/
void dma_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(CF_AD9361_DMA_BASEADDR + XAXIDMA_RX_OFFSET + regAddr, data);
}

/***************************************************************************//**
 * @brief adc_init
*******************************************************************************/
void adc_init(void)
{
	adc_write(ADI_REG_RSTN, 0);
	adc_write(ADI_REG_RSTN, ADI_RSTN);

	adc_write(ADI_REG_CHAN_CNTRL(0), ADI_FORMAT_TYPE | ADI_FORMAT_ENABLE | ADI_ENABLE);
	adc_write(ADI_REG_CHAN_CNTRL(1), ADI_FORMAT_TYPE | ADI_FORMAT_ENABLE | ADI_ENABLE);
	adc_write(ADI_REG_CHAN_CNTRL(2), ADI_FORMAT_TYPE | ADI_FORMAT_ENABLE | ADI_ENABLE);
	adc_write(ADI_REG_CHAN_CNTRL(3), ADI_FORMAT_TYPE | ADI_FORMAT_ENABLE | ADI_ENABLE);
}

/***************************************************************************//**
 * @brief adc_capture
*******************************************************************************/
void adc_capture(uint32_t size, uint32_t start_address)
{
	uint32_t status;
	uint32_t ba;

	ba = start_address + (size*16);
	Xil_Out32((ba + 0x000), (ba + 0x40)); // next descriptor
	Xil_Out32((ba + 0x004), 0x00); // reserved
	Xil_Out32((ba + 0x008), start_address); // start address
	Xil_Out32((ba + 0x00c), 0x00); // reserved
	Xil_Out32((ba + 0x010), 0x00); // reserved
	Xil_Out32((ba + 0x014), 0x00); // reserved
	Xil_Out32((ba + 0x018), (size*16)); // no. of bytes
	Xil_Out32((ba + 0x01c), 0x00); // status
	Xil_Out32((ba + 0x040), (ba + 0x00)); // next descriptor
	Xil_Out32((ba + 0x044), 0x00); // reserved
	Xil_Out32((ba + 0x048), start_address); // start address
	Xil_Out32((ba + 0x04c), 0x00); // reserved
	Xil_Out32((ba + 0x050), 0x00); // reserved
	Xil_Out32((ba + 0x054), 0x00); // reserved
	Xil_Out32((ba + 0x058), (size*16)); // no. of bytes
	Xil_Out32((ba + 0x05c), 0x00); // status
	Xil_DCacheFlush();
	dma_write(XAXIDMA_CR_OFFSET, XAXIDMA_CR_RESET_MASK); // Reset DMA engine
	dma_write(XAXIDMA_CR_OFFSET, 0);
	dma_write(XAXIDMA_CDESC_OFFSET, ba); // Current descriptor pointer
	dma_write(XAXIDMA_CR_OFFSET, XAXIDMA_CR_RUNSTOP_MASK); // Start DMA channel
	dma_write(XAXIDMA_TDESC_OFFSET, (ba+0x40)); // Tail descriptor pointer

	adc_write(ADI_REG_DMA_CNTRL, 0);
	adc_write(ADI_REG_STATUS, ~0);
	adc_write(ADI_REG_DMA_STATUS, ~0);
	adc_write(ADI_REG_DMA_COUNT, (size*16));
	adc_write(ADI_REG_DMA_CNTRL, ADI_DMA_START);
	do
	{
		adc_read(ADI_REG_DMA_STATUS, &status);
		usleep(1000);
	}
	while((status & ADI_DMA_UNF) == ADI_DMA_UNF);
	adc_read(ADI_REG_DMA_STATUS, &status);
	if((status & ADI_DMA_OVF) == ADI_DMA_OVF)
	{
		xil_printf("adc_capture: overflow occurred\n\r");
	}
	Xil_DCacheFlush();
}
