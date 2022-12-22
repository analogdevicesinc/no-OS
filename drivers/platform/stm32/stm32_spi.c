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
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "stm32_gpio.h"
#include "no_os_spi.h"
#include "stm32_spi.h"

#warning SPI delays are not supported on the stm32 platform

static int stm32_spi_config(struct no_os_spi_desc *desc)
{
	int ret;

	const uint32_t prescaler_default = SPI_BAUDRATEPRESCALER_64;
	const uint32_t prescaler_min = SPI_BAUDRATEPRESCALER_2;
	const uint32_t prescaler_max = SPI_BAUDRATEPRESCALER_256;
	uint32_t prescaler_reg = 0u;
	SPI_TypeDef *base = NULL;
	struct stm32_spi_desc *sdesc = desc->extra;

	/* automatically select prescaler based on max_speed_hz */
	if (desc->max_speed_hz != 0u) {
		uint32_t div = sdesc->input_clock / desc->max_speed_hz;
		uint32_t rem = sdesc->input_clock % desc->max_speed_hz;
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

	switch (desc->device_id) {
#if defined(SPI1)
	case 1:
		base = SPI1;
		break;
#endif
#if defined(SPI2)
	case 2:
		base = SPI2;
		break;
#endif
#if defined(SPI3)
	case 3:
		base = SPI3;
		break;
#endif
		break;
#if defined(SPI4)
	case 4:
		base = SPI4;
		break;
#endif
#if defined(SPI5)
	case 5:
		base = SPI5;
		break;
#endif
#if defined(SPI6)
	case 6:
		base = SPI6;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error;
	};
	sdesc->hspi.Instance = base;
	sdesc->hspi.Init.Mode = SPI_MODE_MASTER;
	sdesc->hspi.Init.Direction = SPI_DIRECTION_2LINES;
	sdesc->hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	sdesc->hspi.Init.CLKPolarity = desc->mode & NO_OS_SPI_CPOL ?
				       SPI_POLARITY_HIGH :
				       SPI_POLARITY_LOW;
	sdesc->hspi.Init.CLKPhase = desc->mode & NO_OS_SPI_CPHA ? SPI_PHASE_2EDGE :
				    SPI_PHASE_1EDGE;
	sdesc->hspi.Init.NSS = SPI_NSS_SOFT;
	sdesc->hspi.Init.BaudRatePrescaler = prescaler_reg << SPI_CR1_BR_Pos;
	sdesc->hspi.Init.FirstBit = desc->bit_order ? SPI_FIRSTBIT_LSB :
				    SPI_FIRSTBIT_MSB;
	sdesc->hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	sdesc->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	sdesc->hspi.Init.CRCPolynomial = 10;
	ret = HAL_SPI_Init(&sdesc->hspi);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	return 0;
error:
	return ret;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_spi_init(struct no_os_spi_desc **desc,
		       const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_desc	*spi_desc;

	if (!desc || !param)
		return -EINVAL;

	spi_desc = (struct no_os_spi_desc *)calloc(1, sizeof(*spi_desc));
	if (!spi_desc)
		return -ENOMEM;

	struct stm32_spi_desc *sdesc;
	struct stm32_spi_init_param *sinit;
	struct no_os_gpio_init_param csip;
	struct stm32_gpio_init_param csip_extra;

	sdesc = (struct stm32_spi_desc*)calloc(1,sizeof(struct stm32_spi_desc));
	if (!sdesc) {
		ret = -ENOMEM;
		goto error;
	}

	spi_desc->extra = sdesc;
	sinit = param->extra;

	csip_extra.port = sinit->chip_select_port;
	csip_extra.mode = GPIO_MODE_OUTPUT_PP;
	csip_extra.speed = GPIO_SPEED_FREQ_LOW;
	csip.number = param->chip_select;
	csip.pull = NO_OS_PULL_NONE;
	csip.extra = &csip_extra;
	csip.platform_ops = &stm32_gpio_ops;
	ret = no_os_gpio_get(&sdesc->chip_select, &csip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(sdesc->chip_select, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	/* copy settings to device descriptor */
	spi_desc->device_id = param->device_id;
	spi_desc->max_speed_hz = param->max_speed_hz;
	spi_desc->mode = param->mode;
	spi_desc->bit_order = param->bit_order;
	spi_desc->chip_select = param->chip_select;
	if (sinit->get_input_clock)
		sdesc->input_clock = sinit->get_input_clock();

	ret = stm32_spi_config(spi_desc);
	if (ret)
		goto error;

	*desc = spi_desc;

	return 0;
error:
	free(spi_desc);
	free(sdesc);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_spi_remove(struct no_os_spi_desc *desc)
{
	struct stm32_spi_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;
	HAL_SPI_DeInit(&sdesc->hspi);
	no_os_gpio_remove(sdesc->chip_select);
	free(desc->extra);
	free(desc);
	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_spi_write_and_read(struct no_os_spi_desc *desc,
				 uint8_t *data,
				 uint16_t bytes_number)
{
	int ret;
	static uint64_t last_slave_id;
	uint64_t slave_id;
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

	// Compute a slave ID based on SPI instance and chip select.
	// If it did not change since last call to stm32_spi_write_and_read,
	// no need to reconfigure SPI. Otherwise, reconfigure it.
	slave_id = ((uint64_t)(uintptr_t)sdesc->hspi.Instance << 32) |
		   sdesc->chip_select->number;
	if (slave_id != last_slave_id) {
		last_slave_id = slave_id;
		ret = stm32_spi_config(desc);
		if (ret)
			return ret;
	}

	gdesc->port->BSRR = NO_OS_BIT(sdesc->chip_select->number) << 16;
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
	gdesc->port->BSRR = NO_OS_BIT(sdesc->chip_select->number);

	return 0;
}

/**
 * @brief stm32 platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops stm32_spi_ops = {
	.init = &stm32_spi_init,
	.write_and_read = &stm32_spi_write_and_read,
	.remove = &stm32_spi_remove
};
