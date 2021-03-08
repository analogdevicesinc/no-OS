/***************************************************************************//**
 *   @file   stm32/stm32_spi.c
 *   @brief  Implementation of stm32 spi driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include "util.h"
#include "gpio.h"
#include "stm32_gpio.h"
#include "spi.h"
#include "stm32_spi.h"

/**
 * @brief stm32 platform specific SPI platform ops structure
 */
const struct spi_platform_ops stm32_platform_ops = {
	.init = &stm32_spi_init,
	.write_and_read = &stm32_spi_write_and_read,
	.remove = &stm32_spi_remove
};

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_spi_init(struct spi_desc **desc,
		       const struct spi_init_param *param)
{
	int32_t ret;
	uint32_t input_clock;
	const uint32_t prescaler_default = SPI_BAUDRATEPRESCALER_64;
	const uint32_t prescaler_min = SPI_BAUDRATEPRESCALER_2;
	const uint32_t prescaler_max = SPI_BAUDRATEPRESCALER_256;
	uint32_t prescaler_reg = 0u;
	struct spi_desc	*spi_desc;

	if (!desc || !param)
		return -EINVAL;

	spi_desc = (struct spi_desc *)calloc(1, sizeof(*spi_desc));
	if (!spi_desc)
		return -ENOMEM;

	struct stm32_spi_desc *sdesc;
	struct stm32_spi_init_param *sinit;
	struct gpio_init_param csip;
	struct stm32_gpio_init_param csip_extra;

	sdesc = (stm32_spi_desc*)calloc(1,sizeof(stm32_spi_desc));
	if (!sdesc) {
		ret = -ENOMEM;
		goto error;
	}

	spi_desc->extra = sdesc;
	sinit = param->extra;

	csip_extra.port = sinit->chip_select_port;
	csip_extra.pull = GPIO_NOPULL;
	csip_extra.mode = GPIO_MODE_OUTPUT_PP;
	csip_extra.speed = GPIO_SPEED_FREQ_LOW;
	csip.number = param->chip_select;
	csip.extra = &csip_extra;
	csip.platform_ops = &stm32_gpio_platform_ops;
	ret = gpio_get(&sdesc->chip_select, &csip);
	if (ret < 0)
		goto error;

	ret = gpio_direction_output(sdesc->chip_select, GPIO_HIGH);
	if (ret < 0)
		goto error;

	/* automatically select prescaler based on max_speed_hz */
	if (param->max_speed_hz != 0u && sinit->get_input_clock) {
		input_clock = sinit->get_input_clock();
		uint32_t div = input_clock / param->max_speed_hz;
		uint32_t rem = input_clock % param->max_speed_hz;
		uint32_t po2 = !(div & (div - 1)) && !rem;

		// find the power of two just higher than div and
		// store the exponent in prescaler_reg
		while(div) {
			prescaler_reg += 1;
			div >>= 1u;
		}

		// this exponent - 1 is needed because of the way
		// stm32 stores it into registers:
		// reg = 0 eq. div = 2^1
		// reg = 1 eq. div = 2^2 etc.
		if (prescaler_reg)
			prescaler_reg -= 1;

		// this exponent - 1 is needed when initial div was
		// precisely a power of two
		if (prescaler_reg && po2)
			prescaler_reg -= 1;

		if (prescaler_reg < prescaler_min)
			prescaler_reg = prescaler_min;

		if (prescaler_reg > prescaler_max)
			prescaler_reg = prescaler_max;
	} else
		prescaler_reg = prescaler_default;

	sdesc->hspi.Instance = sinit->base;
	sdesc->hspi.Init.Mode = SPI_MODE_MASTER;
	sdesc->hspi.Init.Direction = SPI_DIRECTION_2LINES;
	sdesc->hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	sdesc->hspi.Init.CLKPolarity = param->mode & SPI_CPOL ? SPI_POLARITY_HIGH :
				       SPI_POLARITY_LOW;
	sdesc->hspi.Init.CLKPhase = param->mode & SPI_CPHA ? SPI_PHASE_2EDGE :
				    SPI_PHASE_1EDGE;
	sdesc->hspi.Init.NSS = SPI_NSS_SOFT;
	sdesc->hspi.Init.BaudRatePrescaler = prescaler_reg << SPI_CR1_BR_Pos;
	sdesc->hspi.Init.FirstBit = param->bit_order ? SPI_FIRSTBIT_LSB :
				    SPI_FIRSTBIT_MSB;
	sdesc->hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	sdesc->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	sdesc->hspi.Init.CRCPolynomial = 10;
	ret = HAL_SPI_Init(&sdesc->hspi);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	/* copy settings to device descriptor */
	spi_desc->max_speed_hz = param->max_speed_hz;
	spi_desc->mode = param->mode;
	spi_desc->bit_order = param->bit_order;
	spi_desc->chip_select = param->chip_select;
	*desc = spi_desc;

	return SUCCESS;
error:
	free(spi_desc);
	free(sdesc);
	return ret;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_spi_remove(struct spi_desc *desc)
{
	struct stm32_spi_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;
	HAL_SPI_DeInit(&sdesc->hspi);
	gpio_remove(sdesc->chip_select);
	free(desc->extra);
	free(desc);
	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t stm32_spi_write_and_read(struct spi_desc *desc,
				 uint8_t *data,
				 uint16_t bytes_number)
{
	uint8_t *tx = data;
	uint8_t *rx = data;
	struct stm32_spi_desc *sdesc;
	struct stm32_gpio_desc *gdesc;
	SPI_TypeDef * SPIx;
	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sdesc = desc->extra;
	gdesc = sdesc->chip_select->extra;
	SPIx = sdesc->hspi.Instance;

	gdesc->port->BSRR = BIT(sdesc->chip_select->number) << 16;
	__HAL_SPI_ENABLE(&sdesc->hspi);
	while(bytes_number--) {
		while(!(SPIx->SR & SPI_SR_TXE))
			;
		*(volatile uint8_t *)&SPIx->DR = *tx++;
		while(!(SPIx->SR & SPI_SR_RXNE))
			;
		*rx++ = *(volatile uint8_t *)&SPIx->DR;
	}
	__HAL_SPI_DISABLE(&sdesc->hspi);
	gdesc->port->BSRR = BIT(sdesc->chip_select->number);

	return SUCCESS;
}
