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

XGpioPs_Config	*gpio_config;
XGpioPs			gpio_instance;

/***************************************************************************//**
 * @brief platform_init
*******************************************************************************/
int32_t platform_init(void)
{
	gpio_init(GPIO_DEVICE_ID);

	gpio_direction_output(AD9371_RESET_B, 1);
	gpio_direction_output(AD9528_RESET_B, 1);
	gpio_direction_output(AD9528_SYSREF_REQ, 0);

	return 0;
}

/***************************************************************************//**
 * @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
					   	   uint8_t *data,
						   uint8_t bytes_number)
{
	uint32_t initss;

	XSpiPs  spi_instance;
	XSpiPs_Config  *spi_config;

	spi_config = XSpiPs_LookupConfig(dev->device_id);

	if (spi_config == NULL)
		return -1;

	if (XSpiPs_CfgInitialize(&spi_instance, spi_config, spi_config->BaseAddress) != 0)
		return -1;

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

	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
 *******************************************************************************/
int32_t gpio_init(uint16_t device_id)
{
	gpio_config = XGpioPs_LookupConfig(device_id);
	XGpioPs_CfgInitialize(&gpio_instance, gpio_config, gpio_config->BaseAddr);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_direction_output
 *******************************************************************************/
int32_t gpio_direction_output(uint8_t gpio, uint8_t value)
{
	XGpioPs_SetDirectionPin(&gpio_instance, gpio, 1);
	XGpioPs_SetOutputEnablePin(&gpio_instance, gpio, 1);
	XGpioPs_WritePin(&gpio_instance, gpio, value);

	return 0;
}

/***************************************************************************//**
 * @brief gpio_set_value
 *******************************************************************************/
int32_t gpio_set_value(uint8_t gpio, uint8_t value)
{
	XGpioPs_WritePin(&gpio_instance, gpio, value);

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
