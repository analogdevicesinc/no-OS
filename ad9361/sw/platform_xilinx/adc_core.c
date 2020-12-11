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
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xil_io.h>
#include "adc_core.h"
#include "parameters.h"
#include "util.h"
#include "config.h"
#ifdef ADC_DMA_IRQ_EXAMPLE
#ifdef _XPARAMETERS_PS_H_
#include <xscugic.h>
#elif defined _MICROBLAZE_INTERFACE_H_
#include <xintc.h>
#endif
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef ADC_DMA_IRQ_EXAMPLE
#ifdef _XPARAMETERS_PS_H_
#ifdef XPS_BOARD_ZCU102
#define ADC_DMA_INT_ID			XPAR_FABRIC_AXI_AD9361_ADC_DMA_IRQ_INTR
#else
#define ADC_DMA_INT_ID			89
#endif
#elif defined _MICROBLAZE_INTERFACE_H_
#define ADC_DMA_INT_ID			12
#endif
#define ADC_DMA_TRANSFER_SIZE	32768
#endif

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct adc_state adc_st;
#ifdef ADC_DMA_IRQ_EXAMPLE
uint8_t  dma_transfer_queued_flag		= 0;
uint8_t  dma_transfer_completed_flag	= 0;
uint32_t dma_start_address				= 0;
#endif

/***************************************************************************//**
 * @brief adc_read
*******************************************************************************/
void adc_read(struct ad9361_rf_phy *phy, uint32_t regAddr, uint32_t *data)
{
	switch (phy->id_no)
	{
	case 0:
		*data = Xil_In32(AD9361_RX_0_BASEADDR + regAddr);
		break;
	case 1:
		*data = Xil_In32(AD9361_RX_1_BASEADDR + regAddr);
		break;
	default:
		break;
	}
}

/***************************************************************************//**
 * @brief adc_write
*******************************************************************************/
void adc_write(struct ad9361_rf_phy *phy, uint32_t regAddr, uint32_t data)
{
	switch (phy->id_no)
	{
	case 0:
		Xil_Out32(AD9361_RX_0_BASEADDR + regAddr, data);
		break;
	case 1:
		Xil_Out32(AD9361_RX_1_BASEADDR + regAddr, data);
		break;
	default:
		break;
	}
}

/***************************************************************************//**
 * @brief adc_dma_read
*******************************************************************************/
void adc_dma_read(uint32_t regAddr, uint32_t *data)
{
	*data = Xil_In32(CF_AD9361_RX_DMA_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief adc_dma_write
*******************************************************************************/
void adc_dma_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(CF_AD9361_RX_DMA_BASEADDR + regAddr, data);
}

/***************************************************************************//**
 * @brief adc_init
*******************************************************************************/
void adc_init(struct ad9361_rf_phy *phy)
{
	adc_write(phy, ADC_REG_RSTN, 0);
	adc_write(phy, ADC_REG_RSTN, ADC_RSTN);

	adc_write(phy, ADC_REG_CHAN_CNTRL(0),
		ADC_IQCOR_ENB | ADC_FORMAT_SIGNEXT | ADC_FORMAT_ENABLE | ADC_ENABLE);
	adc_write(phy, ADC_REG_CHAN_CNTRL(1),
		ADC_IQCOR_ENB | ADC_FORMAT_SIGNEXT | ADC_FORMAT_ENABLE | ADC_ENABLE);
	adc_st.rx2tx2 = phy->pdata->rx2tx2;
	if(adc_st.rx2tx2)
	{
		adc_write(phy, ADC_REG_CHAN_CNTRL(2),
			ADC_IQCOR_ENB | ADC_FORMAT_SIGNEXT | ADC_FORMAT_ENABLE | ADC_ENABLE);
		adc_write(phy, ADC_REG_CHAN_CNTRL(3),
			ADC_IQCOR_ENB | ADC_FORMAT_SIGNEXT | ADC_FORMAT_ENABLE | ADC_ENABLE);
	}
	else
	{
		adc_write(phy, ADC_REG_CHAN_CNTRL(2), 0);
		adc_write(phy, ADC_REG_CHAN_CNTRL(3), 0);
	}
}

#ifdef ADC_DMA_IRQ_EXAMPLE
/***************************************************************************//**
 * @brief adc_dma_isr
*******************************************************************************/
void adc_dma_isr(void *instance)
{
	uint32_t reg_val;

	adc_dma_read(AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	adc_dma_write(AXI_DMAC_REG_IRQ_PENDING, reg_val);
	if(reg_val & IRQ_TRANSFER_QUEUED)
	{
		dma_transfer_queued_flag = 1;
		dma_start_address += ADC_DMA_TRANSFER_SIZE;
		adc_dma_write(AXI_DMAC_REG_DEST_ADDRESS, dma_start_address);
		/* The current transfer was started and a new one is queued. */
		adc_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);
	}
	if(reg_val & IRQ_TRANSFER_COMPLETED)
	{
		dma_transfer_completed_flag = 1;
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

	if(adc_st.rx2tx2)
	{
		length = (size * 8);
	}
	else
	{
		length = (size * 4);
	}

#ifdef FMCOMMS5
	length = (size * 16);
#endif

	adc_dma_write(AXI_DMAC_REG_CTRL, 0x0);
	adc_dma_write(AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);

	adc_dma_write(AXI_DMAC_REG_IRQ_MASK, 0x0);

	adc_dma_read(AXI_DMAC_REG_TRANSFER_ID, &transfer_id);
	adc_dma_read(AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	adc_dma_write(AXI_DMAC_REG_IRQ_PENDING, reg_val);

#ifdef ADC_DMA_IRQ_EXAMPLE
#ifdef _XPARAMETERS_PS_H_
	XScuGic_Config	*gic_config;
	XScuGic			gic;
	int32_t			status;

	gic_config = XScuGic_LookupConfig(GIC_DEVICE_ID);
	if(gic_config == NULL)
		printf("XScuGic_LookupConfig Error\n");

	status = XScuGic_CfgInitialize(&gic,
			gic_config, gic_config->CpuBaseAddress);
	if(status)
		printf("XScuGic_CfgInitialize Error\n");

	XScuGic_SetPriorityTriggerType(&gic, ADC_DMA_INT_ID, 0x0, 0x3);

	status = XScuGic_Connect(&gic,
			ADC_DMA_INT_ID, (Xil_ExceptionHandler)adc_dma_isr, NULL);
	if(status)
		printf("XScuGic_Connect Error\n");

	XScuGic_Enable(&gic, ADC_DMA_INT_ID);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler, (void *)&gic);
	Xil_ExceptionEnable();
#elif defined _MICROBLAZE_INTERFACE_H_
	XIntc	intc;
	int32_t	status;

	status = XIntc_Initialize(&intc, XPAR_AXI_INTC_DEVICE_ID);
	if(status)
		printf("XIntc_Initialize Error\n");

	status = XIntc_Connect(&intc, ADC_DMA_INT_ID,
			(Xil_ExceptionHandler)adc_dma_isr, NULL);
	if(status)
		printf("XIntc_Connect Error\n");

	XIntc_Enable(&intc, ADC_DMA_INT_ID);

	status = XIntc_Start(&intc, XIN_REAL_MODE);
	if(status)
		printf("XIntc_Start Error\n");

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler, (void *)&intc);
	Xil_ExceptionEnable();
#endif

	dma_start_address = start_address;

	adc_dma_write(AXI_DMAC_REG_DEST_ADDRESS, dma_start_address);
	adc_dma_write(AXI_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dma_write(AXI_DMAC_REG_X_LENGTH, ADC_DMA_TRANSFER_SIZE - 1);
	adc_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);

	adc_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);

	while(dma_start_address < (start_address + length + ADC_DMA_TRANSFER_SIZE));
	adc_dma_write(ADC_REG_DMA_CNTRL, 0x0);
#else
	adc_dma_write(AXI_DMAC_REG_DEST_ADDRESS, start_address);
	adc_dma_write(AXI_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dma_write(AXI_DMAC_REG_X_LENGTH, length - 1);
	adc_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);

	adc_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		adc_dma_read(AXI_DMAC_REG_START_TRANSFER, &reg_val);
	}
	while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		adc_dma_read(AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	}
	while(reg_val != (IRQ_TRANSFER_QUEUED | IRQ_TRANSFER_COMPLETED));
	adc_dma_write(AXI_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		adc_dma_read(AXI_DMAC_REG_TRANSFER_DONE, &reg_val);
	}
	while((reg_val & (1 << transfer_id)) != (1 << transfer_id));
#endif
	return 0;
}

/***************************************************************************//**
 * @brief adc_set_calib_scale_phase
*******************************************************************************/
int32_t adc_set_calib_scale_phase(struct ad9361_rf_phy *phy,
								  uint32_t phase,
								  uint32_t chan,
								  int32_t val,
								  int32_t val2)
{
	uint32_t fract;
	uint64_t llval;
	uint32_t tmp;

	switch (val) {
	case 1:
		fract = 0x4000;
		break;
	case -1:
		fract = 0xC000;
		break;
	case 0:
		fract = 0;
		if (val2 < 0) {
			fract = 0x8000;
			val2 *= -1;
		}
		break;
	default:
		return -1;
	}

	llval = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
	do_div(&llval, 1000000UL);
	fract |= llval;

	adc_read(phy, ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	if (!((chan + phase) % 2)) {
		tmp &= ~ADC_IQCOR_COEFF_1(~0);
		tmp |= ADC_IQCOR_COEFF_1(fract);
	} else {
		tmp &= ~ADC_IQCOR_COEFF_2(~0);
		tmp |= ADC_IQCOR_COEFF_2(fract);
	}

	adc_write(phy, ADC_REG_CHAN_CNTRL_2(chan), tmp);

	return 0;
}

/***************************************************************************//**
 * @brief adc_get_calib_scale_phase
*******************************************************************************/
int32_t adc_get_calib_scale_phase(struct ad9361_rf_phy *phy,
								  uint32_t phase,
								  uint32_t chan,
								  int32_t *val,
								  int32_t *val2)
{
	uint32_t tmp;
	int32_t sign;
	uint64_t llval;

	adc_read(phy, ADC_REG_CHAN_CNTRL_2(chan), &tmp);

	/* format is 1.1.14 (sign, integer and fractional bits) */

	if (!((phase + chan) % 2)) {
		tmp = ADC_TO_IQCOR_COEFF_1(tmp);
	} else {
		tmp = ADC_TO_IQCOR_COEFF_2(tmp);
	}

	if (tmp & 0x8000)
		sign = -1;
	else
		sign = 1;

	if (tmp & 0x4000)
		*val = 1 * sign;
	else
		*val = 0;

	tmp &= ~0xC000;

	llval = tmp * 1000000ULL + (0x4000 / 2);
	do_div(&llval, 0x4000);
	if (*val == 0)
		*val2 = llval * sign;
	else
		*val2 = llval;

	return 0;
}

/***************************************************************************//**
 * @brief adc_set_calib_scale
*******************************************************************************/
int32_t adc_set_calib_scale(struct ad9361_rf_phy *phy,
							uint32_t chan,
							int32_t val,
							int32_t val2)
{
	return adc_set_calib_scale_phase(phy, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_get_calib_scale
*******************************************************************************/
int32_t adc_get_calib_scale(struct ad9361_rf_phy *phy,
							uint32_t chan,
							int32_t *val,
							int32_t *val2)
{
	return adc_get_calib_scale_phase(phy, 0, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_set_calib_phase
*******************************************************************************/
int32_t adc_set_calib_phase(struct ad9361_rf_phy *phy,
							uint32_t chan,
							int32_t val,
							int32_t val2)
{
	return adc_set_calib_scale_phase(phy, 1, chan, val, val2);
}

/***************************************************************************//**
 * @brief adc_get_calib_phase
*******************************************************************************/
int32_t adc_get_calib_phase(struct ad9361_rf_phy *phy,
							uint32_t chan,
							int32_t *val,
							int32_t *val2)
{
	return adc_get_calib_scale_phase(phy, 1, chan, val, val2);
}
