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
#include <xparameters.h>
#ifdef _XPARAMETERS_PS_H_
#include <xspips.h>
#include <xgpiops.h>
#include <sleep.h>
#else
#include <xspi.h>
#include <xgpio.h>
#include <xgpio_l.h>
#include <microblaze_sleep.h>
#endif
#include "platform_drivers.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
XSpiPs_Config	*spi_config;
XSpiPs			spi_instance;
XGpioPs_Config	*gpio_config;
XGpioPs			gpio_instance;
uint8_t			spi_decoded_cs = 0;
#else
XSpi_Config		*spi_config;
XSpi			spi_instance;
XGpio_Config	*gpio_config;
XGpio			gpio_instance;
#endif

/***************************************************************************//**
* @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t clk_pha,
				 uint8_t clk_pol)
{
	uint32_t base_addr	 = 0;
	uint32_t spi_options = 0;

#ifdef _XPARAMETERS_PS_H_
	spi_config = XSpiPs_LookupConfig(device_id);

	base_addr = spi_config->BaseAddress;
	XSpiPs_CfgInitialize(&spi_instance, spi_config, base_addr);

	spi_options = XSPIPS_MASTER_OPTION |
			(clk_pol ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
			(clk_pha ? XSPIPS_CLK_PHASE_1_OPTION : 0) |
			(spi_decoded_cs ? XSPIPS_DECODE_SSELECT_OPTION : 0) |
			XSPIPS_FORCE_SSELECT_OPTION;
	XSpiPs_SetOptions(&spi_instance, spi_options);

	XSpiPs_SetClkPrescaler(&spi_instance, XSPIPS_CLK_PRESCALE_32);

	/* FIXME: Temporary 15.2 Fix */
	XSpiPs_CfgInitialize(&spi_instance, spi_config, base_addr);

	XSpiPs_SetOptions(&spi_instance, spi_options);

	XSpiPs_SetClkPrescaler(&spi_instance, XSPIPS_CLK_PRESCALE_32);
#else
	XSpi_Initialize(&spi_instance, device_id);

	XSpi_Stop(&spi_instance);

	spi_config = XSpi_LookupConfig(device_id);

	base_addr = spi_config->BaseAddress;
	XSpi_CfgInitialize(&spi_instance, spi_config, base_addr);

	spi_options = XSP_MASTER_OPTION |
				  (clk_pol ? XSP_CLK_ACTIVE_LOW_OPTION : 0) |
				  (clk_pha ? XSP_CLK_PHASE_1_OPTION : 0) |
				  XSP_MANUAL_SSELECT_OPTION;
	XSpi_SetOptions(&spi_instance, spi_options);

	XSpi_Start(&spi_instance);

	XSpi_IntrGlobalDisable(&spi_instance);
#endif

	return 0;
}

/***************************************************************************//**
* @brief spi_write_and_read
*******************************************************************************/
int32_t spi_write_and_read(uint8_t ss,
						   uint8_t *data,
						   uint8_t bytes_number)
{
#ifdef _XPARAMETERS_PS_H_
	XSpiPs_SetSlaveSelect(&spi_instance, ss);

	XSpiPs_PolledTransfer(&spi_instance, data, data, bytes_number);
#else
	uint8_t	 send_buffer[20];
	uint32_t cnt = 0;

	ss = (1 << ss);
	XSpi_SetSlaveSelect(&spi_instance, ss);

	for(cnt = 0; cnt < bytes_number; cnt++)
	{
		send_buffer[cnt] = data[cnt];
	}
	XSpi_Transfer(&spi_instance, send_buffer, data, bytes_number);
#endif

	return 0;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
int32_t gpio_init(uint32_t device_id)
{
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
}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
int32_t gpio_direction(uint8_t pin, uint8_t direction)
{
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
}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
int32_t gpio_set_value(uint8_t pin, uint8_t data)
{
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
}

/***************************************************************************//**
 * @brief gpio_get_value
*******************************************************************************/
int32_t gpio_get_value(uint8_t pin, uint8_t *data)
{
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
