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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>
#include "platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param init_param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(struct spi_desc **desc,
		 struct spi_init_param *param)
{
	uint8_t  clk_pha;
	uint8_t  clk_pol;
	uint32_t spi_options = 0;
	struct spi_desc        *d;

	d = (struct spi_desc *)malloc(sizeof(*d));
	if (!d)
		return -1;

	d->type = param->type;
	d->id = param->id;
	d->chip_select = param->chip_select;
	d->max_speed_hz = param->max_speed_hz;
	d->mode = param->mode;
	d->ps7_config = XSpiPs_LookupConfig(d->id);

	XSpiPs_CfgInitialize(&d->ps7_instance,
			     d->ps7_config,
			     d->ps7_config->BaseAddress);

	clk_pha = (d->mode & SPI_CPHA) >> 0;
	clk_pol = (d->mode & SPI_CPOL) >> 1;
	spi_options = XSPIPS_MASTER_OPTION |
		      (clk_pol ? XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
		      (clk_pha ? XSPIPS_CLK_PHASE_1_OPTION : 0);

	XSpiPs_SetOptions(&d->ps7_instance, spi_options);

	XSpiPs_SetClkPrescaler(&d->ps7_instance,
			       XSPIPS_CLK_PRESCALE_32);

	*desc = d;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(struct spi_desc *desc)
{
	free(desc);

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	XSpiPs_SetSlaveSelect(&desc->ps7_instance, desc->chip_select);
	XSpiPs_PolledTransfer(&desc->ps7_instance, data, data, bytes_number);

	return SUCCESS;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 struct gpio_init_param param)
{
	struct gpio_desc        *d;
	d = (struct gpio_desc *)malloc(sizeof(*d));
	if (!d)
		return -1;

	d->number = param.number;
	d->id = param.id;

	d->ps7_config = XGpioPs_LookupConfig(d->id);
	XGpioPs_CfgInitialize(&d->ps7_instance,
			      d->ps7_config,
			      d->ps7_config->BaseAddr);
	*desc = d;

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value)
{
	XGpioPs_SetDirectionPin(&desc->ps7_instance,
				desc->number, 1);
	XGpioPs_SetOutputEnablePin(&desc->ps7_instance,
				   desc->number, 1);
	XGpioPs_WritePin(&desc->ps7_instance,
			 desc->number, value);

	return 0;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	free(desc);

	return 0;
}
