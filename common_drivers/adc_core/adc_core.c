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
#include <xil_io.h>
#include <xil_printf.h>
#ifdef ADC_DMAC_INTERRUPTS
#include <xscugic.h>
#endif
#include "adc_core.h"
#include "platform_drivers.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint32_t adc_baseaddr;
uint32_t adc_dmac_baseaddr;
#ifdef ADC_DMAC_INTERRUPTS
uint8_t  adc_sot_flag			= 0;
uint8_t  adc_eot_flag			= 0;
uint32_t adc_dmac_start_address	= 0;
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef ADC_DMAC_INTERRUPTS
#define ADC_DMAC_INT_ID			89
#define ADC_DMAC_TRANSFER_SIZE	32768
#endif

/***************************************************************************//**
* @brief adc_read
*******************************************************************************/
int32_t adc_read(uint32_t reg_addr, uint32_t *reg_data)
{
	*reg_data = Xil_In32((adc_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief adc_write
*******************************************************************************/
int32_t adc_write(uint32_t reg_addr, uint32_t reg_data)
{
	Xil_Out32((adc_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief adc_dmac_read
*******************************************************************************/
void adc_dmac_read(uint32_t reg_addr, uint32_t *reg_data)
{
	*reg_data = Xil_In32(adc_dmac_baseaddr + reg_addr);
}

/***************************************************************************//**
 * @brief adc_dmac_write
*******************************************************************************/
void adc_dmac_write(uint32_t reg_addr, uint32_t reg_data)
{
	Xil_Out32(adc_dmac_baseaddr + reg_addr, reg_data);
}

/***************************************************************************//**
* @brief adc_setup
*******************************************************************************/
int32_t adc_setup(uint32_t adc_addr, uint32_t dma_addr,  uint8_t ch_no)
{
	uint8_t index;
	uint32_t status;

	adc_baseaddr = adc_addr;
	adc_dmac_baseaddr = dma_addr;

	adc_write(ADC_REG_RSTN, 0x00);
	adc_write(ADC_REG_RSTN, 0x03);

	mdelay(100);

	for(index = 0; index < ch_no; index++)
	{
		adc_write(ADC_REG_CHAN_CNTRL(index), 0x51);
	}

	adc_read(ADC_REG_STATUS, &status);
	if(status == 0x0)
	{
		xil_printf("ADC Core Status errors.\n\r");

		return -1;
	}
	else
	{
		xil_printf("ADC Core successfully initialized.\n");

		return 0;
	}
}

#ifdef ADC_DMAC_INTERRUPTS
/***************************************************************************//**
 * @brief adc_dmac_isr
*******************************************************************************/
void adc_dmac_isr(void *instance)
{
	uint32_t reg_val;

	adc_dmac_read(ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	adc_dmac_write(ADC_DMAC_REG_IRQ_PENDING, reg_val);
	if(reg_val & ADC_DMAC_IRQ_SOT)
	{
		adc_sot_flag = 1;
		adc_dmac_start_address += ADC_DMAC_TRANSFER_SIZE;
		adc_dmac_write(ADC_DMAC_REG_DEST_ADDRESS, adc_dmac_start_address);
		/* The current transfer was started and a new one is queued. */
		adc_dmac_write(ADC_DMAC_REG_START_TRANSFER, 0x1);
	}
	if(reg_val & ADC_DMAC_IRQ_EOT)
	{
		adc_eot_flag = 1;
	}
}
#endif

/***************************************************************************//**
 * @brief adc_capture
*******************************************************************************/
int32_t adc_capture(uint32_t size, uint32_t start_address)
{

	uint32_t reg_val;
	uint32_t transfer_id;
	uint32_t length;

	length = (size * 4);

	adc_dmac_write(ADC_DMAC_REG_CTRL, 0x0);
	adc_dmac_write(ADC_DMAC_REG_CTRL, ADC_DMAC_CTRL_ENABLE);

	adc_dmac_write(ADC_DMAC_REG_IRQ_MASK, 0x0);

	adc_dmac_read(ADC_DMAC_REG_TRANSFER_ID, &transfer_id);
	adc_dmac_read(ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	adc_dmac_write(ADC_DMAC_REG_IRQ_PENDING, reg_val);

#ifndef ADC_DMAC_INTERRUPTS
	adc_dmac_write(ADC_DMAC_REG_DEST_ADDRESS, start_address);
	adc_dmac_write(ADC_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dmac_write(ADC_DMAC_REG_X_LENGTH, length - 1);
	adc_dmac_write(ADC_DMAC_REG_Y_LENGTH, 0x0);

	adc_dmac_write(ADC_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		adc_dmac_read(ADC_DMAC_REG_START_TRANSFER, &reg_val);
	}
	while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		adc_dmac_read(ADC_DMAC_REG_IRQ_PENDING, &reg_val);
	}
	while(reg_val != (ADC_DMAC_IRQ_SOT | ADC_DMAC_IRQ_EOT));
	adc_dmac_write(ADC_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		adc_dmac_read(ADC_DMAC_REG_TRANSFER_DONE, &reg_val);
	}
	while((reg_val & (1 << transfer_id)) != (1 << transfer_id));
#else
	XScuGic_Config	*gic_config;
	XScuGic			gic;
	int32_t			status;

	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if(gic_config == NULL)
		xil_printf("Error\n");

	status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);
	if(status)
		xil_printf("Error\n");

	XScuGic_SetPriorityTriggerType(&gic, ADC_DMAC_INT_ID, 0x0, 0x3);

	status = XScuGic_Connect(&gic, ADC_DMAC_INT_ID, (Xil_ExceptionHandler)adc_dmac_isr, NULL);
	if(status)
		xil_printf("Error\n");

	XScuGic_Enable(&gic, ADC_DMAC_INT_ID);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, (void *)&gic);
	Xil_ExceptionEnable();

	adc_dmac_start_address = start_address;

	adc_dmac_write(ADC_DMAC_REG_DEST_ADDRESS, adc_dmac_start_address);
	adc_dmac_write(ADC_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dmac_write(ADC_DMAC_REG_X_LENGTH, ADC_DMAC_TRANSFER_SIZE - 1);
	adc_dmac_write(ADC_DMAC_REG_Y_LENGTH, 0x0);

	adc_dmac_write(ADC_DMAC_REG_START_TRANSFER, 0x1);

	while(adc_dmac_start_address < (start_address + length + ADC_DMAC_TRANSFER_SIZE));
	adc_dmac_write(ADC_DMAC_REG_CTRL, 0x0);
#endif

	return 0;
}

/***************************************************************************//**
 * @brief adc_set_pnsel
*******************************************************************************/
int32_t adc_set_pnsel(uint8_t channel, enum adc_pn_sel sel)
{
	uint32_t reg;

	adc_read(ADC_REG_CHAN_CNTRL_3(channel), &reg);
	reg &= ~ADC_ADC_PN_SEL(~0);
	reg |= ADC_ADC_PN_SEL(sel);
	adc_write(ADC_REG_CHAN_CNTRL_3(channel), reg);

	return 0;
}
