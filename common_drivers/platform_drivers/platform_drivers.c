/***************************************************************************//**
 * @file platform_drivers.c
 * @brief Implementation of Platform Drivers.
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

#include "platform_drivers.h"

/***************************************************************************//**
 * @brief ad_spi_init
 *******************************************************************************/
// if not using the altera/xilinx spi cores, you may overwrite this function

int32_t ad_spi_init(spi_device *dev)
{

#ifdef ZYNQ_PS7
	dev->base_address = XPAR_PS7_SPI_0_BASEADDR;
	dev->device_id = XPAR_PS7_SPI_0_DEVICE_ID;
#endif

#ifdef ZYNQ_PSU
	dev->base_address = XPAR_PSU_SPI_0_BASEADDR;
	dev->device_id = XPAR_PSU_SPI_0_DEVICE_ID;
#endif

#ifdef MICROBLAZE
#endif

#ifdef NIOS_II
#endif

	dev->chip_select = 0xff;
	dev->cpha = 0;
	dev->cpol = 0;

	return(0);
}

/***************************************************************************//**
 * @brief ad_spi_xfer
 *******************************************************************************/

int32_t ad_spi_xfer(spi_device *dev, uint8_t *data, uint8_t no_of_bytes)
{

#if defined(ZYNQ_PS7) || defined(ZYNQ_PSU)

	uint32_t initss;

	XSpiPs  m_spi;
	XSpiPs_Config  *m_spi_config;

	m_spi_config = XSpiPs_LookupConfig(dev->device_id);

	if (m_spi_config == NULL) {
		return(-1);
	}

	if (XSpiPs_CfgInitialize(&m_spi, m_spi_config, m_spi_config->BaseAddress) != 0) {
		return(-1);
	}

	initss = XSpiPs_ReadReg(dev->base_address, XSPIPS_CR_OFFSET);
	initss = initss & (uint32_t)(~XSPIPS_CR_SSCTRL_MASK);
	initss = initss | (0x7 << XSPIPS_CR_SSCTRL_SHIFT);
	XSpiPs_WriteReg(dev->base_address, XSPIPS_CR_OFFSET, initss);
	XSpiPs_SetOptions(&m_spi, XSPIPS_MASTER_OPTION |
			XSPIPS_DECODE_SSELECT_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
			((dev->cpol == 1) ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			((dev->cpha == 1) ? XSPIPS_CLK_PHASE_1_OPTION : 0));
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0x7);
	XSpiPs_SetClkPrescaler(&m_spi, XSPIPS_CLK_PRESCALE_64);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) dev->chip_select);
	XSpiPs_PolledTransfer(&m_spi, data, data, no_of_bytes);
	XSpiPs_SetSlaveSelect(&m_spi,  (uint8_t) 0x7);

#endif

#ifdef NIOS_II

	uint32_t i;

	IOWR_32DIRECT(SYS_SPI_BASE, 0x0c, 0x400);
	IOWR_32DIRECT(SYS_SPI_BASE, 0x14, ~(dev->chip_select));
	for (i = 0; i < no_of_bytes; i++) {
		while ((IORD_32DIRECT(SYS_SPI_BASE, 0x08) & 0x40) == 0x00) {}
		IOWR_32DIRECT(SYS_SPI_BASE, 0x04, *(data + i));
		while ((IORD_32DIRECT(SYS_SPI_BASE, 0x08) & 0x80) == 0x00) {}
		*(data + i) = IORD_32DIRECT(SYS_SPI_BASE, 0x00);
	}
	IOWR_32DIRECT(SYS_SPI_BASE, 0x14, 0x000);
	IOWR_32DIRECT(SYS_SPI_BASE, 0x0c, 0x000);

#endif

#ifdef MICROBLAZE

	uint32_t i;

	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x70), dev->chip_select);
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), (0x086 | (dev->cpol<<3) | (dev->cpha<<4)));
	for (i = 0; i < no_of_bytes; i++) {
		Xil_Out32((XPAR_SPI_0_BASEADDR + 0x68), *(data + i));
		while ((Xil_In32(XPAR_SPI_0_BASEADDR + 0x64) & 0x1) == 0x1) {}
		*(data + i) = Xil_In32(XPAR_SPI_0_BASEADDR + 0x6c) & 0xff;
	}
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x70), 0xff);
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), (0x186 | (dev->cpol<<3) | (dev->cpha<<4)));

#endif

	return(0);
}

/***************************************************************************//**
 * @brief ad_gpio_set
 *******************************************************************************/

int32_t ad_gpio_set(uint8_t pin, uint8_t data)
{

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	if (pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = pin - 32;
	pmask = 0x1 << ppos;

#ifdef ZYNQ_PS7

	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata | pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c8);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c8), (pdata | pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x004c), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef ZYNQ_PSU

	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata | pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0308);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0308), (pdata | pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0050), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef NIOS_II

	pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
	IOWR_32DIRECT(SYS_GPIO_OUT_BASE, 0x0, ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

#ifdef MICROBLAZE

	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0xc);
	Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0xc), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
	Xil_Out32((XPAR_AXI_GPIO_BASEADDR + 0x8), ((pdata & ~pmask) | (data << ppos)));
	pstatus = 0;

#endif

	return(pstatus);
}

/***************************************************************************//**
 * @brief ad_gpio_get
 *******************************************************************************/

int32_t ad_gpio_get(uint8_t pin, uint8_t *data)
{

	int32_t pstatus;
	uint32_t ppos;
	uint32_t pdata;
#if defined(ZYNQ_PS7) || defined(ZYNQ_PSU)
	uint32_t pmask;
#endif

	if (pin < 32) {
		return(-1);
	}

	pstatus = -1;
	ppos = pin - 32;
#if defined(ZYNQ_PS7) || defined(ZYNQ_PSU)
	pmask = 0x1 << ppos;
#endif

#ifdef ZYNQ_PS7

	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x02c4);
	Xil_Out32((XPAR_PS7_GPIO_0_BASEADDR + 0x02c4), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_PS7_GPIO_0_BASEADDR + 0x004c);
	*data = (pdata >> ppos) & 0x1;
	pstatus = 0;

#endif

#ifdef ZYNQ_PSU

	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0304);
	Xil_Out32((XPAR_PSU_GPIO_0_BASEADDR + 0x0304), (pdata & ~pmask));
	pdata = Xil_In32(XPAR_PSU_GPIO_0_BASEADDR + 0x0050);
	*data = (pdata >> ppos) & 0x1;
	pstatus = 0;

#endif

#ifdef NIOS_II

	pdata = IORD_32DIRECT(SYS_GPIO_OUT_BASE, 0x0);
	*data = (pdata >> ppos) & 0x1;
	pstatus = 0;

#endif

#ifdef MICROBLAZE

	pdata = Xil_In32(XPAR_AXI_GPIO_BASEADDR + 0x8);
	*data = (pdata >> ppos) & 0x1;
	pstatus = 0;

#endif

	return(pstatus);
}

/***************************************************************************//**
 * @brief do_div
 *******************************************************************************/
uint64_t do_div(uint64_t* n, uint64_t base)
{
	uint64_t mod = 0;

	mod = *n % base;
	*n = *n / base;

	return mod;
}

/***************************************************************************//**
 * @brief ad_reg_write_16
 *******************************************************************************/
void ad_reg_write_16(uint32_t addr, uint32_t data)
{
	uint32_t m_data;

	m_data = ad_reg_read(addr & ~0x3);
	if ((addr & 0x3) == 0)
		m_data = (m_data & ~0xffff) | data;
	else
		m_data = (m_data & 0xffff) | (data<<16);
	ad_reg_write((addr & ~0x3), m_data);
}

#ifdef ALTERA
void ad_platform_init(void)
{
	return;
}

void ad_platform_close(void)
{
	alt_printf("%c", 0x04);
	return;
}
#endif

#ifdef MICROBLAZE
void usleep(uint32_t us_count)
{
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

/***************************************************************************//**
 * @brief ad_uart_read
 *******************************************************************************/

#ifdef ZYNQ_PS7
uint8_t ad_uart_read()
{
	u32 RecievedByte;
	int32_t timeout = 100000000;
	/* Wait until there is data */
	while (!XUartPs_IsReceiveData(STDIN_BASEADDRESS)&&timeout>0) {
		timeout--;
	}
	RecievedByte = XUartPs_ReadReg(STDIN_BASEADDRESS, XUARTPS_FIFO_OFFSET);
	/* Return the byte received */
	return (uint8_t)RecievedByte;
}
#endif
