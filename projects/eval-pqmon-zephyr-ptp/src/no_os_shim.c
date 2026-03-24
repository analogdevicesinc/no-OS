/********************************************************************************
 *   @file   no_os_shim.c
 *   @brief  Zephyr implementations of no-OS API shim
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "no_os_shim.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(no_os_shim, LOG_LEVEL_INF);

/* ADE9430 SPI device from devicetree */
#define ADE9430_SPI_NODE DT_NODELABEL(ade9430)

static const struct spi_dt_spec ade9430_spi_spec =
	SPI_DT_SPEC_GET(ADE9430_SPI_NODE,
			 SPI_OP_MODE_MASTER | SPI_WORD_SET(8) |
			 SPI_TRANSFER_MSB, 0);

/* ADE9430 reset GPIO from devicetree */
static const struct gpio_dt_spec ade9430_reset =
	GPIO_DT_SPEC_GET(ADE9430_SPI_NODE, reset_gpios);

/* Global init params (replaces common_data.c) */
struct no_os_spi_init_param spi_egy_ip = {
	.device_id = 1,
	.max_speed_hz = 6000000,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.chip_select = 0,
};

struct no_os_gpio_init_param reset_gpio_ip = {
	.port = 1,
	.number = 27,
	.pull = NO_OS_PULL_NONE,
};

/* --- SPI --- */

int no_os_spi_init(struct no_os_spi_desc **desc,
		   const struct no_os_spi_init_param *param)
{
	if (!device_is_ready(ade9430_spi_spec.bus)) {
		LOG_ERR("ADE9430 SPI bus not ready");
		return -ENODEV;
	}

	struct no_os_spi_desc *d = k_malloc(sizeof(*d));
	if (!d) {
		return -ENOMEM;
	}

	d->zephyr_dev = ade9430_spi_spec.bus;
	d->param = *param;
	*desc = d;

	LOG_DBG("ADE9430 SPI initialized");
	return 0;
}

int no_os_spi_write_and_read(struct no_os_spi_desc *desc,
			     uint8_t *data, uint16_t bytes_number)
{
	/* Same buffer for TX and RX — matches no-OS semantics exactly.
	 * TX data is sent on MOSI while MISO overwrites data[] in place. */
	struct spi_buf tx_buf = { .buf = data, .len = bytes_number };
	struct spi_buf rx_buf = { .buf = data, .len = bytes_number };
	struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
	struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

	return spi_transceive_dt(&ade9430_spi_spec, &tx_set, &rx_set);
}

int no_os_spi_remove(struct no_os_spi_desc *desc)
{
	if (desc) {
		k_free(desc);
	}
	return 0;
}

/* --- GPIO --- */

int no_os_gpio_get(struct no_os_gpio_desc **desc,
		   const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *d = k_malloc(sizeof(*d));
	if (!d) {
		return -ENOMEM;
	}
	d->port = param->port;
	d->number = param->number;

	/* For the ADE9430 reset pin, configure it via DT spec */
	if (param->port == 1 && param->number == 27) {
		if (!gpio_is_ready_dt(&ade9430_reset)) {
			k_free(d);
			return -ENODEV;
		}
	}

	*desc = d;
	return 0;
}

int no_os_gpio_direction_output(struct no_os_gpio_desc *desc, uint8_t value)
{
	if (desc->port == 1 && desc->number == 27) {
		/* Use _dt to inherit VDDIOH flag from DT, then set raw value. */
		int ret = gpio_pin_configure_dt(&ade9430_reset, GPIO_OUTPUT);
		if (ret == 0) {
			ret = gpio_pin_set_raw(ade9430_reset.port,
					       ade9430_reset.pin, value);
		}
		LOG_DBG("ADE9430 reset GPIO configured (value=%d, ret=%d)",
			value, ret);
		return ret;
	}
	return -ENOTSUP;
}

int no_os_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	if (desc->port == 1 && desc->number == 27) {
		/* Use gpio_pin_set_raw to bypass DT ACTIVE_LOW inversion. */
		return gpio_pin_set_raw(ade9430_reset.port,
					ade9430_reset.pin, value);
	}
	return -ENOTSUP;
}

int no_os_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc) {
		k_free(desc);
	}
	return 0;
}

/* --- Delay --- */

void no_os_mdelay(uint32_t msecs)
{
	k_msleep(msecs);
}

/* --- Utility --- */

void no_os_memswap64(void *buf, uint32_t bytes, uint32_t step)
{
	uint8_t *p = buf;
	uint32_t i, j;
	uint8_t temp[8];

	if (step < 2 || step > 8 || bytes < step || bytes % step != 0) {
		return;
	}
	for (i = 0; i < bytes; i += step) {
		memcpy(temp, p, step);
		for (j = step; j > 0; j--) {
			*p++ = temp[j - 1];
		}
	}
}
