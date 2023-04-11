/***************************************************************************//**
 *   @file   pico/pico_spi.c
 *   @brief  Implementation of pico spi driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "pico_spi.h"
#include "pico/stdlib.h"
#include "no_os_delay.h"
#include "hardware/resets.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define PICO_SPI_MAX_INSTANCES	2U

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static uint64_t last_slave_id[PICO_SPI_MAX_INSTANCES];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int pico_spi_config(struct no_os_spi_desc *desc)
{
	struct pico_spi_desc *pico_spi;
	uint8_t data_bits;
	spi_cpol_t cpol;
	spi_cpha_t cpha;

	pico_spi = desc->extra;

	switch (desc->device_id) {
	case 0:
		pico_spi->spi_instance = spi0;
		break;
	case 1:
		pico_spi->spi_instance = spi1;
		break;
	default:
		return -EINVAL;
	};

	/* SPI reset & unreset */
	reset_block(pico_spi->spi_instance == spi0 ? RESETS_RESET_SPI0_BITS :
		    RESETS_RESET_SPI1_BITS);
	unreset_block_wait(pico_spi->spi_instance == spi0 ? RESETS_RESET_SPI0_BITS :
			   RESETS_RESET_SPI1_BITS);

	desc->max_speed_hz = spi_set_baudrate(pico_spi->spi_instance,
					      desc->max_speed_hz);

	data_bits = 8;
	switch (desc->mode) {
	case NO_OS_SPI_MODE_0:
		cpol = SPI_CPOL_0;
		cpha = SPI_CPHA_0;
		break;
	case NO_OS_SPI_MODE_1:
		cpol = SPI_CPOL_0;
		cpha = SPI_CPHA_1;
		break;
	case NO_OS_SPI_MODE_2:
		cpol = SPI_CPOL_1;
		cpha = SPI_CPHA_0;
		break;
	case NO_OS_SPI_MODE_3:
		cpol = SPI_CPOL_1;
		cpha = SPI_CPHA_1;
		break;
	default:
		return -EINVAL;
	}

	spi_set_format(pico_spi->spi_instance, data_bits, cpol, cpha, SPI_MSB_FIRST);

	/* Set SPI pins */
	gpio_set_function(pico_spi->spi_rx_pin, GPIO_FUNC_SPI);
	gpio_set_function(pico_spi->spi_sck_pin, GPIO_FUNC_SPI);
	gpio_set_function(pico_spi->spi_tx_pin, GPIO_FUNC_SPI);
	gpio_init(pico_spi->spi_cs_pin);
	/* Set output direction for CS */
	gpio_set_dir(pico_spi->spi_cs_pin, true);
	/* Set CS pin high */
	gpio_put(pico_spi->spi_cs_pin, 1);
	/* Enable SPI */
	hw_set_bits(&spi_get_hw(pico_spi->spi_instance)->cr1, SPI_SSPCR1_SSE_BITS);

	return 0;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc  - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_spi_init(struct no_os_spi_desc **desc,
		      const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_desc *descriptor;
	struct pico_spi_desc *pico_spi;
	struct pico_spi_init_param *pico_spi_ip;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	descriptor = (struct no_os_spi_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	pico_spi = (struct pico_spi_desc *)no_os_calloc(1, sizeof(*pico_spi));
	if (!pico_spi) {
		ret = -ENOMEM;
		goto free_desc;
	}

	descriptor->device_id = param->device_id;
	descriptor->extra = pico_spi;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->mode = param->mode;

	/* Only MSB is supported */
	descriptor->bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST;

	pico_spi_ip = param->extra;
	pico_spi->spi_cs_pin = pico_spi_ip->spi_cs_pin;
	pico_spi->spi_sck_pin = pico_spi_ip->spi_sck_pin;
	pico_spi->spi_rx_pin = pico_spi_ip->spi_rx_pin;
	pico_spi->spi_tx_pin = pico_spi_ip->spi_tx_pin;

	descriptor->chip_select = (uint8_t)pico_spi_ip->spi_cs_pin;

	ret = pico_spi_config(descriptor);
	if (ret)
		goto error;

	*desc = descriptor;

	return 0;

error:
	no_os_free(pico_spi);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_spi_remove(struct no_os_spi_desc *desc)
{
	struct pico_spi_desc *pico_spi;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_spi = desc->extra;

	spi_deinit(pico_spi->spi_instance);

	no_os_free(desc->extra);
	no_os_free(desc);
	return 0;
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t pico_spi_transfer(struct no_os_spi_desc *desc,
			  struct no_os_spi_msg *msgs,
			  uint32_t len)
{
	struct pico_spi_desc *pico_spi;
	uint64_t slave_id;
	int ret;

	if (!desc || !desc->extra || !msgs)
		return -EINVAL;

	pico_spi = desc->extra;

	slave_id = desc->chip_select;
	if (slave_id != last_slave_id[desc->device_id]) {
		last_slave_id[desc->device_id] = slave_id;
		ret = pico_spi_config(desc);
		if (ret)
			return ret;
	}

	for (uint32_t i = 0; i < len; i++) {

		if (!msgs[i].tx_buff && !msgs[i].rx_buff)
			return -EINVAL;

		/* Assert CS */
		gpio_put(pico_spi->spi_cs_pin, 0);

		if (msgs[i].cs_delay_first)
			no_os_udelay(msgs[i].cs_delay_first);

		if (!msgs[i].tx_buff)
			spi_read_blocking(pico_spi->spi_instance, 0, msgs[i].rx_buff,
					  msgs[i].bytes_number);
		else if (!msgs[i].rx_buff)
			spi_write_blocking(pico_spi->spi_instance, msgs[i].tx_buff,
					   msgs[i].bytes_number);
		else
			spi_write_read_blocking(pico_spi->spi_instance, msgs[i].tx_buff,
						msgs[i].rx_buff, msgs[i].bytes_number);

		if(msgs[i].cs_delay_last)
			no_os_udelay(msgs[i].cs_delay_last);

		if (msgs[i].cs_change)
			/* De-assert CS */
			gpio_put(pico_spi->spi_cs_pin, 1);

		if(msgs[i].cs_change_delay)
			no_os_udelay(msgs[i].cs_change_delay);
	}

	return 0;
}


/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_spi_write_and_read(struct no_os_spi_desc *desc,
				uint8_t *data,
				uint16_t bytes_number)
{
	struct no_os_spi_msg msg = {
		.bytes_number = bytes_number,
		.cs_change = true,
		.rx_buff = data,
		.tx_buff = data,
	};

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	return pico_spi_transfer(desc, &msg, 1);
}

/**
 * @brief pico platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops pico_spi_ops = {
	.init = &pico_spi_init,
	.write_and_read = &pico_spi_write_and_read,
	.transfer = &pico_spi_transfer,
	.remove = &pico_spi_remove
};
