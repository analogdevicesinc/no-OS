/***************************************************************************//**
* @file platform_drivers.c
* @brief Implementation of Platform Drivers.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014-2016(c) Analog Devices, Inc.
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
#include <xparameters.h>
#ifdef _XPARAMETERS_PS_H_
#include <sleep.h>
#else
#include <microblaze_sleep.h>
#endif
#include "platform_drivers.h"
#include "spi_engine.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef XPAR_PS7_SPI_0_DEVICE_ID
uint8_t	spi_decoded_cs = 0;
#endif

/***************************************************************************//**
* @brief spi_init
*******************************************************************************/
int32_t spi_init(spi_device *dev)
{
	uint8_t	 clk_pha;
	uint8_t	 clk_pol;
	uint32_t spi_options = 0;
	spi_dev			*spi_dev;
	spi_init_param spi_default_init_param = {};

	clk_pha = (dev->mode & SPI_CPHA) >> 0;
	clk_pol = (dev->mode & SPI_CPOL) >> 1;

	switch (dev->type) {
	case AXI_SPI:
#ifdef XPAR_SPI_0_DEVICE_ID
		XSpi_Initialize(&dev->axi_instance,
						dev->device_id);

		XSpi_Stop(&dev->axi_instance);

		dev->axi_config = XSpi_LookupConfig(dev->device_id);

		XSpi_CfgInitialize(&dev->axi_instance,
						   dev->axi_config,
						   dev->axi_config->BaseAddress);

		spi_options = XSP_MASTER_OPTION |
					  (clk_pol ? XSP_CLK_ACTIVE_LOW_OPTION : 0) |
					  (clk_pha ? XSP_CLK_PHASE_1_OPTION : 0) |
					  XSP_MANUAL_SSELECT_OPTION;
		XSpi_SetOptions(&dev->axi_instance, spi_options);

		XSpi_Start(&dev->axi_instance);

		XSpi_IntrGlobalDisable(&dev->axi_instance);
#endif
		break;
	case PS7_SPI:
#ifdef XPAR_PS7_SPI_0_DEVICE_ID
		dev->ps7_config = XSpiPs_LookupConfig(dev->device_id);

		XSpiPs_CfgInitialize(&dev->ps7_instance,
							 dev->ps7_config,
							 dev->ps7_config->BaseAddress);

		spi_options = XSPIPS_MASTER_OPTION |
				(clk_pol ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
				(clk_pha ? XSPIPS_CLK_PHASE_1_OPTION : 0) |
				(spi_decoded_cs ? XSPIPS_DECODE_SSELECT_OPTION : 0) |
				XSPIPS_FORCE_SSELECT_OPTION;
		XSpiPs_SetOptions(&dev->ps7_instance,
						  spi_options);

		XSpiPs_SetClkPrescaler(&dev->ps7_instance,
							   XSPIPS_CLK_PRESCALE_32);
#endif
		break;
	case SPI_ENGINE:
		spi_eng_setup(&spi_dev, spi_default_init_param);
		break;
	default:
		return -1;
	}

	return 0;
}

/***************************************************************************//**
* @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(spi_device *dev,
						   uint8_t *data,
						   uint8_t bytes_number)
{
	switch (dev->type) {
	case AXI_SPI:
#ifdef XPAR_SPI_0_DEVICE_ID
		uint8_t	 ss = (1 << dev->chip_select);
		uint8_t	 send_buffer[20];
		uint32_t cnt = 0;

		XSpi_SetSlaveSelect(&dev->axi_instance, ss);

		for(cnt = 0; cnt < bytes_number; cnt++)
		{
			send_buffer[cnt] = data[cnt];
		}
		XSpi_Transfer(&dev->axi_instance,
					  send_buffer,
					  data,
					  bytes_number);
#endif
		break;
	case PS7_SPI:
#ifdef XPAR_PS7_SPI_0_DEVICE_ID
		XSpiPs_SetSlaveSelect(&dev->ps7_instance, dev->chip_select);
		XSpiPs_PolledTransfer(&dev->ps7_instance, data, data, bytes_number);
#endif
		break;
	case SPI_ENGINE:
		break;
	default:
		return -1;
	}

	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
int32_t gpio_init(gpio_device *dev)
{
	if (dev->type == AXI_GPIO) {
#ifdef XPAR_GPIO_0_DEVICE_ID
		dev->axi_config = XGpio_LookupConfig(dev->device_id);
		XGpio_CfgInitialize(&dev->axi_instance,
							dev->axi_config,
							dev->axi_config->BaseAddress);
#endif
	} else {
#ifdef XPAR_PS7_GPIO_0_DEVICE_ID
		dev->ps7_config = XGpioPs_LookupConfig(dev->device_id);
		XGpioPs_CfgInitialize(&dev->ps7_instance,
							  dev->ps7_config,
							  dev->ps7_config->BaseAddr);
#endif
	}

	return 0;
#if 0
	uint32_t base_addr = 0;

#ifdef _XPARAMETERS_PS_H_
	gpio_config = XGpioPs_LookupConfig(device_id);

	base_addr = gpio_config->BaseAddr;
	XGpioPs_CfgInitialize(&gpio_instance, gpio_config, base_addr);
#else
	gpio_config = XGpio_LookupConfig(device_id);

	base_addr = gpio_config->BaseAddress;
	XGpio_CfgInitialize(&gpio_instance, gpio_config, base_addr);
#endif

	return 0;
#endif
}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
int32_t gpio_set_direction(gpio_device *dev,
						   uint8_t pin,
						   uint8_t direction)
{
	if (dev->type == AXI_GPIO) {
#ifdef XPAR_GPIO_0_DEVICE_ID
		uint32_t channel = 1;
		uint32_t config	 = 0;

		/* We assume that pin 32 is the first pin from channel 2 */
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		}

		config = XGpio_GetDataDirection(&dev->axi_instance,
										channel);
		if (direction) {
			config &= ~(1 << pin);
		} else {
			config |= (1 << pin);
		}
		XGpio_SetDataDirection(&dev->axi_instance,
							   channel,
							   config);
#endif
	} else {
#ifdef XPAR_PS7_GPIO_0_DEVICE_ID
		XGpioPs_SetDirectionPin(&dev->ps7_instance,
								pin,
								direction);
		XGpioPs_SetOutputEnablePin(&dev->ps7_instance,
								   pin,
								   1);
#endif
	}

	return 0;
#if 0

#ifdef _XPARAMETERS_PS_H_
	XGpioPs_SetDirectionPin(&gpio_instance, pin, direction);
	XGpioPs_SetOutputEnablePin(&gpio_instance, pin, 1);
#else
	uint32_t channel = 1;
	uint32_t config	 = 0;

	/* We assume that pin 32 is the first pin from channel 2 */
	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	}

	config = XGpio_GetDataDirection(&gpio_instance, channel);
	if (direction) {
		config &= ~(1 << pin);
	} else {
		config |= (1 << pin);
	}
	XGpio_SetDataDirection(&gpio_instance, channel, config);
#endif

	return 0;
#endif
}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
int32_t gpio_set_value(gpio_device *dev,
					   uint8_t pin,
					   uint8_t data)
{
	if (dev->type == AXI_GPIO) {
#ifdef XPAR_GPIO_0_DEVICE_ID
		uint32_t channel = 1;
		uint32_t config	 = 0;

		/* We assume that pin 32 is the first pin from channel 2 */
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		}

		config = XGpio_DiscreteRead(&dev->axi_instance,
									channel);
		if(data) {
			config |= (1 << pin);
		} else {
			config &= ~(1 << pin);
		}
		XGpio_DiscreteWrite(&dev->axi_instance,
							channel,
							config);
#endif
	} else {
#ifdef XPAR_PS7_GPIO_0_DEVICE_ID
		XGpioPs_WritePin(&dev->ps7_instance,
						 pin,
						 data);
#endif
	}

	return 0;
#if 0
#ifdef _XPARAMETERS_PS_H_
	XGpioPs_WritePin(&gpio_instance, pin, data);
#else
	uint32_t channel = 1;
	uint32_t config	 = 0;

	/* We assume that pin 32 is the first pin from channel 2 */
	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	}

	config = XGpio_DiscreteRead(&gpio_instance, channel);
	if(data) {
		config |= (1 << pin);
	} else {
		config &= ~(1 << pin);
	}
	XGpio_DiscreteWrite(&gpio_instance, channel, config);
#endif

	return 0;
#endif
}

/***************************************************************************//**
 * @brief gpio_get_value
*******************************************************************************/
int32_t gpio_get_value(gpio_device *dev,
					   uint8_t pin,
					   uint8_t *data)
{
	if (dev->type == AXI_GPIO) {
#ifdef XPAR_GPIO_0_DEVICE_ID
		uint32_t channel = 1;
		uint32_t config	 = 0;

		/* We assume that pin 32 is the first pin from channel 2 */
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		}

		config = XGpio_DiscreteRead(&dev->axi_instance,
									channel);
		*data = (config & (1 << pin)) ? 1 : 0;
#endif
	} else {
#ifdef XPAR_PS7_GPIO_0_DEVICE_ID
		*data = XGpioPs_ReadPin(&dev->ps7_instance, pin);
#endif
	}

	return 0;
#if 0
#ifdef _XPARAMETERS_PS_H_
	*data = XGpioPs_ReadPin(&gpio_instance, pin);
#else
	uint32_t channel = 1;
	uint32_t config	 = 0;

	/* We assume that pin 32 is the first pin from channel 2 */
	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	}

	config = XGpio_DiscreteRead(&gpio_instance, channel);
	*data = (config & (1 << pin)) ? 1 : 0;
#endif

	return 0;
#endif
}

/***************************************************************************//**
* @brief mdelay
*******************************************************************************/
void mdelay(uint32_t msecs)
{
#ifdef _XPARAMETERS_PS_H_
	usleep(msecs * 1000);
#else
	MB_Sleep(msecs);
#endif
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
