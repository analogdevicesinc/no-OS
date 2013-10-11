
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"
#include "stdint.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xaxidma_hw.h"
#include "sleep.h"
#include "adc_core.h"

/***************************************************************************//**
 * @brief adc_read
*******************************************************************************/
void adc_read(uint32_t regAddr, uint32_t *data)
{
    *data = Xil_In32(XPAR_AXI_AD9361_0_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief adc_write
*******************************************************************************/
void adc_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(XPAR_AXI_AD9361_0_BASEADDR + regAddr, data);
}

/***************************************************************************//**
 * @brief adc_write
*******************************************************************************/
void dma_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(XPAR_AXI_DMA_1_BASEADDR + XAXIDMA_RX_OFFSET + regAddr, data);
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
