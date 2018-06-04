/***************************************************************************//**
 * @file platform_drivers.c
 * @brief Implementation of Platform Drivers.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include <stdio.h>
#include "platform_drivers.h"
#include "parameters.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
spi_device spi_dev = {
	SPI_BASEADDR, 	// base_address
	SPI_DEVICE_ID, 	// device_id
	0xff, 			// chip_select
	0, 				// cpha
	0, 				// cpol
	0				// id_no
};

#ifdef _XPARAMETERS_PS_H_
static XGpioPs_Config	*gpio_config;
static XGpioPs			gpio_instance;

static XSpiPs_Config 	*spi_config;
static XSpiPs			spi_instance;
#else
static XGpio_Config		*gpio_config;
#endif

/***************************************************************************//**
 * @brief platform_init
*******************************************************************************/
int32_t platform_init(void)
{
	if (gpio_init(GPIO_DEVICE_ID) != 0)
		return -1;
	if (spi_init(SPI_DEVICE_ID) != 0)
		return -1;

	gpio_direction_output(AD9371_RESET_B, 1);
	gpio_direction_output(AD9528_RESET_B, 1);
	gpio_direction_output(AD9528_SYSREF_REQ, 0);

	return 0;
}

/***************************************************************************//**
 * @brief spi_init
 *******************************************************************************/
int32_t spi_init(uint16_t device_id)
{
#ifdef _XPARAMETERS_PS_H_
	spi_config = XSpiPs_LookupConfig(device_id);
	if (spi_config == NULL)
		return -1;

	if (XSpiPs_CfgInitialize(&spi_instance, spi_config, spi_config->BaseAddress) != 0)
		return -1;
#endif
	return 0;
}

/***************************************************************************//**
 * @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
					   	   uint8_t *data,
						   uint8_t bytes_number)
{
#ifdef _XPARAMETERS_PS_H_
	uint32_t initss;

	initss = XSpiPs_ReadReg(dev->base_address, XSPIPS_CR_OFFSET);
	initss = initss & (uint32_t)(~XSPIPS_CR_SSCTRL_MASK);
	initss = initss | (0x7 << XSPIPS_CR_SSCTRL_SHIFT);
	XSpiPs_WriteReg(dev->base_address, XSPIPS_CR_OFFSET, initss);
	XSpiPs_SetOptions(&spi_instance, XSPIPS_MASTER_OPTION |
			XSPIPS_DECODE_SSELECT_OPTION | XSPIPS_FORCE_SSELECT_OPTION |
			((dev->cpol == 1) ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			((dev->cpha == 1) ? XSPIPS_CLK_PHASE_1_OPTION : 0));
	XSpiPs_SetSlaveSelect(&spi_instance, (uint8_t) 0x7);
	XSpiPs_SetClkPrescaler(&spi_instance, XSPIPS_CLK_PRESCALE_64);
	XSpiPs_SetSlaveSelect(&spi_instance,  (uint8_t) dev->chip_select);
	XSpiPs_PolledTransfer(&spi_instance, data, data, bytes_number);
	XSpiPs_SetSlaveSelect(&spi_instance,  (uint8_t) 0x7);
#else
	uint32_t i;

	Xil_Out32((SPI_BASEADDR + 0x70), ~(dev->chip_select));
	Xil_Out32((SPI_BASEADDR + 0x60), (0x086 | (dev->cpol<<3) | (dev->cpha<<4)));
	for (i = 0; i < bytes_number; i++) {
		Xil_Out32((SPI_BASEADDR + 0x68), *(data + i));
		while ((Xil_In32(SPI_BASEADDR + 0x64) & 0x1) == 0x1) {}
		*(data + i) = Xil_In32(SPI_BASEADDR + 0x6c) & 0xff;
	}
	Xil_Out32((SPI_BASEADDR + 0x70), 0xff);
	Xil_Out32((SPI_BASEADDR + 0x60), (0x186 | (dev->cpol<<3) | (dev->cpha<<4)));
#endif
	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
 *******************************************************************************/
int32_t gpio_init(uint16_t device_id)
{
#ifdef _XPARAMETERS_PS_H_
	gpio_config = XGpioPs_LookupConfig(device_id);
	if (gpio_config == NULL)
		return -1;

	if (XGpioPs_CfgInitialize(&gpio_instance, gpio_config, gpio_config->BaseAddr) != 0)
		return -1;
#else
	gpio_config = XGpio_LookupConfig(device_id);
#endif
	return 0;
}

/***************************************************************************//**
 * @brief gpio_direction_output
 *******************************************************************************/
int32_t gpio_direction_output(uint8_t gpio, uint8_t value)
{
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_SetDirectionPin(&gpio_instance, gpio, 1);
	XGpioPs_SetOutputEnablePin(&gpio_instance, gpio, 1);
	XGpioPs_WritePin(&gpio_instance, gpio, value);
#else
	uint32_t config = 0;
	uint32_t tri_reg_addr;

	if (gpio >= 32) {
		tri_reg_addr = XGPIO_TRI2_OFFSET;
		gpio -= 32;
	} else
		tri_reg_addr = XGPIO_TRI_OFFSET;

	config = Xil_In32((gpio_config->BaseAddress + tri_reg_addr));
	config &= ~(1 << gpio);
	Xil_Out32((gpio_config->BaseAddress + tri_reg_addr), config);
#endif
	return 0;
}

/***************************************************************************//**
 * @brief gpio_set_value
 *******************************************************************************/
int32_t gpio_set_value(uint8_t gpio, uint8_t value)
{
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_WritePin(&gpio_instance, gpio, value);
#else
	uint32_t config = 0;
	uint32_t data_reg_addr;

	if (gpio >= 32) {
		data_reg_addr = XGPIO_DATA2_OFFSET;
		gpio -= 32;
	} else
		data_reg_addr = XGPIO_DATA_OFFSET;

	config = Xil_In32((gpio_config->BaseAddress + data_reg_addr));
	if(value)
		config |= (1 << gpio);
	else
		config &= ~(1 << gpio);
	Xil_Out32((gpio_config->BaseAddress + data_reg_addr), config);
#endif
	return 0;
}

/***************************************************************************//**
 * @brief ad_pow2 Create a mask for a given number of bit
 *******************************************************************************/
uint32_t ad_pow2(uint32_t number) {

	uint32_t index;
	uint32_t mask = 1;

	for (index=1; index < number; index++) {
		mask = (mask << 1) ^ 1;
	}

	return mask;
}

/***************************************************************************//**
 * @brief find_first_bit
*******************************************************************************/
uint32_t find_first_bit(uint32_t word)
{
	int32_t num = 0;

	if ((word & 0xffff) == 0) {
			num += 16;
			word >>= 16;
	}
	if ((word & 0xff) == 0) {
			num += 8;
			word >>= 8;
	}
	if ((word & 0xf) == 0) {
			num += 4;
			word >>= 4;
	}
	if ((word & 0x3) == 0) {
			num += 2;
			word >>= 2;
	}
	if ((word & 0x1) == 0)
			num += 1;
	return num;
}
