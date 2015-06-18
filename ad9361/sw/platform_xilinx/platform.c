/***************************************************************************//**
 *   @file   Platform.c
 *   @brief  Implementation of Platform Driver.
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
#include <xparameters.h>
#ifdef _XPARAMETERS_PS_H_
#include <xgpiops.h>
#include <xspips.h>
#else
#include <xgpio.h>
#include <xgpio_l.h>
#include <xspi.h>
#endif
#include "util.h"
#include "adc_core.h"
#include "dac_core.h"
#include "platform.h"
#ifdef _XPARAMETERS_PS_H_
#include <sleep.h>
#else
static inline void usleep(unsigned long usleep)
{
	unsigned long delay = 0;

	for(delay = 0; delay < usleep * 10; delay++);
}
#endif

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
XSpiPs_Config	*spi_config;
XSpiPs			spi_instance;
XGpioPs_Config	*gpio_config;
XGpioPs			gpio_instance;
#else
XSpi_Config		*spi_config;
XSpi			spi_instance;
XGpio_Config	*gpio_config;
#endif

/***************************************************************************//**
 * @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t  clk_pha,
				 uint8_t  clk_pol)
{

	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;
#ifdef _XPARAMETERS_PS_H_
	uint8_t  byte		 = 0;

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
#else
	XSpi_Initialize(&spi_instance, device_id);
	XSpi_Stop(&spi_instance);
	spi_config = XSpi_LookupConfig(device_id);
	base_addr = spi_config->BaseAddress;
	XSpi_CfgInitialize(&spi_instance, spi_config, base_addr);
	control_val = XSP_MASTER_OPTION |
				  XSP_CLK_PHASE_1_OPTION |
				  XSP_MANUAL_SSELECT_OPTION;
	XSpi_SetOptions(&spi_instance, control_val);
	XSpi_Start(&spi_instance);
	XSpi_IntrGlobalDisable(&spi_instance);
	XSpi_SetSlaveSelect(&spi_instance, 1);
#endif
	return SUCCESS;
}

/***************************************************************************//**
 * @brief spi_read
*******************************************************************************/
int32_t spi_read(struct spi_device *spi,
				 uint8_t *data,
				 uint8_t bytes_number)
{
	uint32_t cnt		 = 0;
#ifdef _XPARAMETERS_PS_H_
	uint32_t base_addr	 = 0;
	uint32_t control_val = 0;
	uint32_t status	  	 = 0;

	base_addr = spi_config->BaseAddress;
	control_val = XSpiPs_ReadReg(base_addr, XSPIPS_CR_OFFSET);

	XSpiPs_WriteReg(base_addr, XSPIPS_CR_OFFSET,
					control_val & ~((spi->id_no == 0 ? 1 : 2) << XSPIPS_CR_SSCTRL_SHIFT));

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
#ifdef XPAR_AXI_SPI_0_DEVICE_ID
	uint8_t send_buffer[20];

	for(cnt = 0; cnt < bytes_number; cnt++)
	{
		send_buffer[cnt] = data[cnt];
	}

	XSpi_Transfer(&spi_instance, send_buffer, data, bytes_number);
#else
	Xil_Out32((spi_instance.BaseAddr + 0x60), 0x1e6);
	Xil_Out32((spi_instance.BaseAddr + 0x70), 0x000);
	while(cnt < bytes_number)
	{
		Xil_Out32((spi_instance.BaseAddr + 0x68), data[cnt]);
		Xil_Out32((spi_instance.BaseAddr + 0x60), 0x096);
		do {usleep(100);}
		while ((Xil_In32((spi_instance.BaseAddr + 0x64)) & 0x4) == 0x0);
		Xil_Out32((spi_instance.BaseAddr + 0x60), 0x186);
		data[cnt] = Xil_In32(spi_instance.BaseAddr + 0x6c);
		cnt++;
	}
	Xil_Out32((spi_instance.BaseAddr + 0x70), 0x001);
	Xil_Out32((spi_instance.BaseAddr + 0x60), 0x180);
#endif
#endif
	return SUCCESS;
}

/***************************************************************************//**
 * @brief spi_write_then_read
*******************************************************************************/
int spi_write_then_read(struct spi_device *spi,
		const unsigned char *txbuf, unsigned n_tx,
		unsigned char *rxbuf, unsigned n_rx)
{
	uint8_t buffer[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00};
	uint8_t byte;

	for(byte = 0; byte < n_tx; byte++)
	{
		buffer[byte] = (unsigned char)txbuf[byte];
	}
	spi_read(spi, buffer, n_tx + n_rx);
	for(byte = n_tx; byte < n_tx + n_rx; byte++)
	{
		rxbuf[byte - n_tx] = buffer[byte];
	}

	return SUCCESS;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
void gpio_init(uint32_t device_id)
{
#ifdef _XPARAMETERS_PS_H_
	gpio_config = XGpioPs_LookupConfig(device_id);
	XGpioPs_CfgInitialize(&gpio_instance, gpio_config, gpio_config->BaseAddr);
#else
	gpio_config = XGpio_LookupConfig(device_id);
#endif
}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
void gpio_direction(uint8_t pin, uint8_t direction)
{
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_SetDirectionPin(&gpio_instance, pin, direction);
	XGpioPs_SetOutputEnablePin(&gpio_instance, pin, 1);
#else
	uint32_t config = 0;

	config = Xil_In32((gpio_config->BaseAddress + XGPIO_TRI_OFFSET));
	if(direction)
	{
		config &= ~(1 << pin);
	}
	else
	{
		config |= (1 << pin);
	}
	Xil_Out32((gpio_config->BaseAddress + XGPIO_TRI_OFFSET), config);
#endif
}

/***************************************************************************//**
 * @brief gpio_is_valid
*******************************************************************************/
bool gpio_is_valid(int number)
{
	if(number >= 0)
		return 1;
	else
		return 0;
}

/***************************************************************************//**
 * @brief gpio_data
*******************************************************************************/
void gpio_data(uint8_t pin, uint8_t data)
{
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_WritePin(&gpio_instance, pin, data);
#else
	uint32_t config = 0;

	config = Xil_In32((gpio_config->BaseAddress + XGPIO_DATA_OFFSET));
	if(data)
	{
		config |= (1 << pin);
	}
	else
	{
		config &= ~(1 << pin);
	}
	Xil_Out32((gpio_config->BaseAddress + XGPIO_DATA_OFFSET), config);
#endif
}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
void gpio_set_value(unsigned gpio, int value)
{
	gpio_data(gpio, value);
}

/***************************************************************************//**
 * @brief udelay
*******************************************************************************/
void udelay(unsigned long usecs)
{
	usleep(usecs);
}

/***************************************************************************//**
 * @brief mdelay
*******************************************************************************/
void mdelay(unsigned long msecs)
{
	usleep(msecs * 1000);
}

/***************************************************************************//**
 * @brief msleep_interruptible
*******************************************************************************/
unsigned long msleep_interruptible(unsigned int msecs)
{
	mdelay(msecs);

	return 0;
}

/***************************************************************************//**
 * @brief axiadc_init
*******************************************************************************/
void axiadc_init(struct ad9361_rf_phy *phy)
{
	adc_init(phy);
	dac_init(phy, DATA_SEL_DDS, 0);
}

/***************************************************************************//**
 * @brief axiadc_post_setup
*******************************************************************************/
int axiadc_post_setup(struct ad9361_rf_phy *phy)
{
	return ad9361_post_setup(phy);
}

/***************************************************************************//**
 * @brief axiadc_read
*******************************************************************************/
unsigned int axiadc_read(struct axiadc_state *st, unsigned long reg)
{
	uint32_t val;

	adc_read(st->phy, reg, &val);

	return val;
}

/***************************************************************************//**
 * @brief axiadc_write
*******************************************************************************/
void axiadc_write(struct axiadc_state *st, unsigned reg, unsigned val)
{
	adc_write(st->phy, reg, val);
}

/***************************************************************************//**
 * @brief axiadc_set_pnsel
*******************************************************************************/
int axiadc_set_pnsel(struct axiadc_state *st, int channel, enum adc_pn_sel sel)
{
	unsigned reg;

	uint32_t version = axiadc_read(st, 0x4000);

	if (PCORE_VERSION_MAJOR(version) > 7) {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL_3(channel));
		reg &= ~ADI_ADC_PN_SEL(~0);
		reg |= ADI_ADC_PN_SEL(sel);
		axiadc_write(st, ADI_REG_CHAN_CNTRL_3(channel), reg);
	} else {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL(channel));

		if (sel == ADC_PN_CUSTOM) {
			reg |= ADI_PN_SEL;
		} else if (sel == ADC_PN9) {
			reg &= ~ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		} else {
			reg |= ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		}

		axiadc_write(st, ADI_REG_CHAN_CNTRL(channel), reg);
	}

	return 0;
}

/***************************************************************************//**
 * @brief axiadc_idelay_set
*******************************************************************************/
void axiadc_idelay_set(struct axiadc_state *st,
				unsigned lane, unsigned val)
{
	if (PCORE_VERSION_MAJOR(st->pcore_version) > 8) {
		axiadc_write(st, ADI_REG_DELAY(lane), val);
	} else {
		axiadc_write(st, ADI_REG_DELAY_CNTRL, 0);
		axiadc_write(st, ADI_REG_DELAY_CNTRL,
				ADI_DELAY_ADDRESS(lane)
				| ADI_DELAY_WDATA(val)
				| ADI_DELAY_SEL);
	}
}
