/**************************************************************************//**
*   @file   spi.c
*   @brief  SPI functions implementations.
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
#include <xparameters.h>
#ifdef _XPARAMETERS_PS_H_
#include <xgpiops.h>
#include <xspips.h>
#include <sleep.h>
#else
#include <xspi.h>
static inline void usleep(unsigned long usleep)
{
	unsigned long delay = 0;

	for(delay = 0; delay < usleep * 10; delay++);
}
#endif
#include "spi.h"

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
#ifdef _XPARAMETERS_PS_H_
XSpiPs_Config	*spi_config;
XSpiPs			spi_instance;
#endif

/***************************************************************************//**
* @brief mdelay
*******************************************************************************/
void mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}

/***************************************************************************//**
* @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
		uint8_t clk_pha,
		uint8_t clk_pol)
{
#ifdef _XPARAMETERS_PS_H_
	uint8_t  byte		 = 0;
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;

	spi_config = XSpiPs_LookupConfig(device_id);
	base_addr = spi_config->BaseAddress;
	XSpiPs_CfgInitialize(&spi_instance, spi_config, base_addr);
	control_val = XSPIPS_CR_SSFORCE_MASK |
			XSPIPS_CR_SSCTRL_MASK |
			4 << XSPIPS_CR_PRESC_SHIFT |
			(clk_pha ? XSPIPS_CR_CPHA_MASK : 0) |
			(clk_pol ? XSPIPS_CR_CPOL_MASK : 0) |
			XSPIPS_CR_MSTREN_MASK;
	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET, control_val);
	for(byte = 0; byte < 128; byte++)
	{
		XSpiPs_ReadReg(base_addr, XSPIPS_RXD_OFFSET);
	}
#endif

	return 0;
}

/***************************************************************************//**
* @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(uint8_t ss, uint8_t *data,
				 uint8_t bytes_number)
{
	uint32_t cnt		 = 0;
#ifdef _XPARAMETERS_PS_H_
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;
	uint32_t status	  	 = 0;

	ss = (1 << ss);
	base_addr = spi_config->BaseAddress;
	control_val = XSpiPs_ReadReg(base_addr, XSPIPS_CR_OFFSET);

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET,
					control_val & ~(ss << XSPIPS_CR_SSCTRL_SHIFT));

	XSpiPs_WriteReg(base_addr, XSPIPS_TXWR_OFFSET, 0x01);

	XSpiPs_WriteReg(base_addr, XSPIPS_SR_OFFSET, XSPIPS_IXR_TXOW_MASK);
	XSpiPs_WriteReg(base_addr, XSPIPS_IER_OFFSET, XSPIPS_IXR_TXOW_MASK);

	while(cnt < bytes_number)
	{
		XSpiPs_WriteReg(base_addr, XSPIPS_TXD_OFFSET, data[cnt]);
		cnt++;
	}

	XSpiPs_WriteReg(base_addr, XSPIPS_ER_OFFSET, XSPIPS_ER_ENABLE_MASK);

	do
	{
		status = XSpiPs_ReadReg(base_addr, XSPIPS_SR_OFFSET);
	}
	while((status & XSPIPS_IXR_TXOW_MASK) == 0x0);

	XSpiPs_WriteReg(base_addr, XSPIPS_SR_OFFSET, XSPIPS_IXR_TXOW_MASK);

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET, control_val);

	cnt = 0;
	while(cnt < bytes_number)
	{
		data[cnt] = XSpiPs_ReadReg(base_addr, XSPIPS_RXD_OFFSET);
		cnt++;
	}

	XSpiPs_WriteReg(base_addr, XSPIPS_ER_OFFSET, 0x0);
#else
	uint8_t sel[2] = {0xE, 0xD};

	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x70), 0xfff);
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), 0x0e6);
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x70), sel[ss]);

	for(cnt = 0; cnt < bytes_number; cnt++)
	{
		Xil_Out32((XPAR_SPI_0_BASEADDR + 0x68), (data[cnt] & 0xff));
		Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), 0x086);
		do {mdelay(1);}
		while ((Xil_In32((XPAR_SPI_0_BASEADDR + 0x64)) & 0x4) == 0x0);
		Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), 0x086);
		data[cnt] = (Xil_In32(XPAR_SPI_0_BASEADDR + 0x6c) & 0xff);
	}

	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x70), 0xfff);
	Xil_Out32((XPAR_SPI_0_BASEADDR + 0x60), 0x080);
#endif

	return 0;
}
